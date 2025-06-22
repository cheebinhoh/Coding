/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-kafka.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <expected>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <string>

#include "rdkafka.h"

#include "dmn-kafka-util.hpp"
#include "dmn-proc.hpp"

namespace dmn {

const std::string Dmn_Kafka::Topic = "Dmn_Kafka.Topic";
const std::string Dmn_Kafka::Key = "Dmn_Kafka.Key";
const std::string Dmn_Kafka::PollTimeoutMs = "Dmn_Kafka.PollTimeoutMs";

void Dmn_Kafka::producerCallback([[maybe_unused]] rd_kafka_t *kafka_handle,
                                 const rd_kafka_message_t *rkmessage,
                                 void *opaque) {
  Dmn_Kafka *obj = static_cast<Dmn_Kafka *>(opaque);

  obj->m_kafka_err = rkmessage->err;
  obj->m_write_atomic_flag.clear();
  obj->m_write_atomic_flag.notify_all();
}

void Dmn_Kafka::errorCallback([[maybe_unused]] rd_kafka_t *kafka_handle,
                              int err, [[maybe_unused]] const char *reason,
                              void *opaque) {
  Dmn_Kafka *obj = static_cast<Dmn_Kafka *>(opaque);

  obj->m_kafka_err = static_cast<rd_kafka_resp_err_t>(err);
  obj->m_write_atomic_flag.clear();
  obj->m_write_atomic_flag.notify_all();
}

Dmn_Kafka::Dmn_Kafka(Dmn_Kafka::Role role, Dmn_Kafka::ConfigType configs)
    : m_role{role}, m_configs{configs} {
  rd_kafka_conf_t *kafka_config{};

  kafka_config = rd_kafka_conf_new();

  for (auto &c : m_configs) {
    if (Dmn_Kafka::Topic == c.first) {
      m_topic = c.second;
    } else if (Dmn_Kafka::Key == c.first) {
      m_topic = c.second;
    } else if (Dmn_Kafka::PollTimeoutMs == c.first) {
      m_poll_timeout_ms = std::strtoll(c.second.c_str(), nullptr, 0);
    } else {
      auto res =
          dmn::set_config(kafka_config, c.first.c_str(), c.second.c_str());
      if (!res.has_value()) {
        rd_kafka_conf_destroy(kafka_config);

        throw std::runtime_error(
            "Error in setting kafka configuration: " + c.first +
            " to value: " + c.second + ", error: " + res.error());
      }

      assert(res.value() == RD_KAFKA_CONF_OK);
    }
  }

  char err_str[kKafkaErrorStringLength]{};

  rd_kafka_conf_set_opaque(kafka_config, (void *)this);
  rd_kafka_conf_set_error_cb(kafka_config, Dmn_Kafka::errorCallback);

  if (Role::kProducer == m_role) {
    rd_kafka_conf_set_dr_msg_cb(kafka_config, Dmn_Kafka::producerCallback);

    m_kafka =
        rd_kafka_new(RD_KAFKA_PRODUCER, kafka_config, err_str, sizeof(err_str));
    if (!m_kafka) {
      rd_kafka_conf_destroy(kafka_config);

      throw std::runtime_error("Failed to create new producer: " +
                               std::string(err_str));
    }

    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    kafka_config = NULL;
  } else {
    assert(Role::kConsumer == m_role);

    m_kafka =
        rd_kafka_new(RD_KAFKA_CONSUMER, kafka_config, err_str, sizeof(err_str));
    if (!m_kafka) {
      rd_kafka_conf_destroy(kafka_config);

      throw std::runtime_error("Failed to create new consumer: " +
                               std::string(err_str));
    }

    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    kafka_config = NULL;

    rd_kafka_poll_set_consumer(m_kafka);

    // Convert the list of topics to a format suitable for librdkafka.
    const char *topic = m_topic.c_str();
    rd_kafka_topic_partition_list_t *subscription =
        rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(subscription, topic,
                                      RD_KAFKA_PARTITION_UA);

    // Subscribe to the list of topics.
    rd_kafka_resp_err_t err = rd_kafka_subscribe(m_kafka, subscription);
    if (err) {
      rd_kafka_topic_partition_list_destroy(subscription);
      rd_kafka_destroy(m_kafka);

      throw std::runtime_error(
          "Failed to subscribe to " + std::to_string(subscription->cnt) +
          +" topics, error: " + std::string(rd_kafka_err2str(err)));
    }

    rd_kafka_topic_partition_list_destroy(subscription);
  }
}

Dmn_Kafka::~Dmn_Kafka() noexcept try {
  assert(m_kafka);

  if (Role::kConsumer == m_role) {
    rd_kafka_consumer_close(m_kafka);
  } else {
    rd_kafka_poll(m_kafka, 100);
    rd_kafka_flush(m_kafka, 1000); // this is not configurable
  }

  rd_kafka_destroy(m_kafka);
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

std::optional<std::string> Dmn_Kafka::read() {
  rd_kafka_message_t *consumer_message{};

  consumer_message = rd_kafka_consumer_poll(m_kafka, m_poll_timeout_ms);
  Dmn_Proc::yield();

  if (!consumer_message) {
    return {};
  }

  std::optional<std::string> ret{};

  // after this point, we need to free consumer_message
  if (consumer_message->err) {
    if (consumer_message->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
      /* We can ignore this error - it just means we've read
       * everything and are waiting for more data.
       */
    } else {
      std::cerr << "Consumer error: "
                << rd_kafka_message_errstr(consumer_message) << "\n";
      m_kafka_err = consumer_message->err;
    }
  } else {
    ret = std::string((char *)consumer_message->payload,
                      (int)consumer_message->len);
  }

  rd_kafka_message_destroy(consumer_message);

  return ret;
}

void Dmn_Kafka::write(std::string &item) { write(item, false); }

void Dmn_Kafka::write(std::string &&item) { write(item, true); }

void Dmn_Kafka::write(std::string &item, [[maybe_unused]] bool move) {
  assert(Role::kProducer == m_role);

  // this make sure only one thread is accessing the Dmn_Kafka write,
  // it is used to wait for message to be delivered to kafka broker and
  // the producerCallback() to be called, so we can return from this
  // method and assert that the message is delivered successfully or
  // fail affirmatively.
  while (m_write_atomic_flag.test_and_set(std::memory_order_acquire)) {
    m_write_atomic_flag.wait(true, std::memory_order_relaxed);
  }

  m_kafka_err = static_cast<rd_kafka_resp_err_t>(0);

  const char *topic = m_topic.c_str();
  const char *key = m_key.c_str();
  const size_t key_len = m_key.size();
  const char *value = item.c_str();
  const size_t value_len = item.size();

  rd_kafka_resp_err_t err = rd_kafka_producev(
      m_kafka, RD_KAFKA_V_TOPIC(topic),
      RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY), // maybe RD_KAFKA_MSG_F_FREE?
      RD_KAFKA_V_KEY((void *)key, key_len),
      RD_KAFKA_V_VALUE((void *)value, value_len), RD_KAFKA_V_OPAQUE(NULL),
      RD_KAFKA_V_END);

  Dmn_Proc::yield();

  if (err) {
    m_write_atomic_flag.clear();

    throw std::runtime_error("Failed to produce to topic: " + m_topic +
                             ", error: " + std::string(rd_kafka_err2str(err)));
  }

  // we loop until message is delivered or pronounced fail, and the poll is
  // needed to poll generic error like network not reachable beyond message
  // specific error.
  while (m_write_atomic_flag.test()) {
    rd_kafka_poll(m_kafka, 100);
    rd_kafka_flush(m_kafka, 1000); // this is not configurable, and 1000ms shall
                                   // be good enough for a single message to be
                                   // flushed to the kafka broker.
  }

  // the producerCallback is returned, if an error from delivery the message,
  // we want to return it to the caller of the write api.
  if (m_kafka_err) {
    throw std::runtime_error(
        "Failed to delivered to topic: " + m_topic +
        ", error: " + std::string(rd_kafka_err2str(m_kafka_err)));
  }
}

} // namespace dmn
