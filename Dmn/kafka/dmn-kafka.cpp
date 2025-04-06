/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-kafka.hpp"
#include "dmn-kafka-util.hpp"
#include "dmn-proc.hpp"

#include "rdkafka.h"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <expected>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <string>

namespace Dmn {

const std::string Dmn_Kafka::Topic = "Dmn_Kafka.Topic";
const std::string Dmn_Kafka::Key = "Dmn_Kafka.Key";
const std::string Dmn_Kafka::PollTimeoutMs = "Dmn_Kafka.PollTimeoutMs";

void Dmn_Kafka::producerCallback(rd_kafka_t *kafka_handle,
                                 const rd_kafka_message_t *rkmessage,
                                 void *opaque) {
  Dmn_Kafka *obj = (Dmn_Kafka *)opaque;

  obj->m_kafkaErr = rkmessage->err;
  obj->m_atomicFlag.clear();
  obj->m_atomicFlag.notify_all();
}

void Dmn_Kafka::errorCallback(rd_kafka_t *kafka_handle, int err,
                              const char *reason, void *opaque) {
  Dmn_Kafka *obj = (Dmn_Kafka *)opaque;

  obj->m_kafkaErr = static_cast<rd_kafka_resp_err_t>(err);
  obj->m_atomicFlag.clear();
  obj->m_atomicFlag.notify_all();
}

Dmn_Kafka::Dmn_Kafka(Dmn_Kafka::Role role, Dmn_Kafka::ConfigType configs)
    : m_role{role}, m_configs{configs} {
  rd_kafka_conf_t *kafkaConf{};

  kafkaConf = rd_kafka_conf_new();

  for (auto &c : m_configs) {
    if (Dmn_Kafka::Topic == c.first) {
      m_topic = c.second;
    } else if (Dmn_Kafka::Key == c.first) {
      m_topic = c.second;
    } else if (Dmn_Kafka::PollTimeoutMs == c.first) {
      m_pollTimeoutMs = std::strtoll(c.second.c_str(), nullptr, 0);
    } else {
      auto res = Dmn::set_config(kafkaConf, c.first.c_str(), c.second.c_str());
      if (!res.has_value()) {
        rd_kafka_conf_destroy(kafkaConf);

        throw std::runtime_error("Error in setting kafka configuration: " +
                                 c.first + " to value: " + c.second);
      }

      assert(res.value() == RD_KAFKA_CONF_OK);
    }
  }

  char errstr[512]{};

  rd_kafka_conf_set_opaque(kafkaConf, (void *)this);
  rd_kafka_conf_set_error_cb(kafkaConf, Dmn_Kafka::errorCallback);

  if (Role::Producer == m_role) {
    rd_kafka_conf_set_dr_msg_cb(kafkaConf, Dmn_Kafka::producerCallback);

    m_kafka =
        rd_kafka_new(RD_KAFKA_PRODUCER, kafkaConf, errstr, sizeof(errstr));
    if (!m_kafka) {
      rd_kafka_conf_destroy(kafkaConf);

      throw std::runtime_error("Failed to create new producer: " +
                               std::string(errstr));
    }

    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    kafkaConf = NULL;
  } else {
    assert(Role::Consumer == m_role);

    m_kafka =
        rd_kafka_new(RD_KAFKA_CONSUMER, kafkaConf, errstr, sizeof(errstr));
    if (!m_kafka) {
      rd_kafka_conf_destroy(kafkaConf);

      throw std::runtime_error("Failed to create new consumer: " +
                               std::string(errstr));
    }

    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    kafkaConf = NULL;

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

  if (Role::Consumer == m_role) {
    rd_kafka_consumer_close(m_kafka);
  } else {
    rd_kafka_poll(m_kafka, 100);
    rd_kafka_flush(m_kafka, 1000); // this is not configurable, and 1000ms shall
  }

  rd_kafka_destroy(m_kafka);
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

std::optional<std::string> Dmn_Kafka::read() {
  rd_kafka_message_t *consumer_message{};

  consumer_message = rd_kafka_consumer_poll(m_kafka, m_pollTimeoutMs);
  if (!consumer_message) {
    return {};
  }

  Dmn_Proc::yield();

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
      m_kafkaErr = consumer_message->err;
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

void Dmn_Kafka::write(std::string &item, bool move) {
  // this make sure only one thread is accessing the Dmn_Kafka write,
  // it is used to wait for message to be delivered to kafka broker and
  // the producerCallback() to be called, so we can return from this
  // method and assert that the message is delivered successfully or
  // fail affirmatively.
  while (m_atomicFlag.test_and_set(std::memory_order_acquire)) {
    m_atomicFlag.wait(true, std::memory_order_relaxed);
  }

  m_kafkaErr = static_cast<rd_kafka_resp_err_t>(0);

  const char *topic = m_topic.c_str();
  const char *key = m_key.c_str();
  const size_t keyLen = m_key.size();
  const char *value = item.c_str();
  const size_t valueLen = item.size();

  rd_kafka_resp_err_t err = rd_kafka_producev(
      m_kafka, RD_KAFKA_V_TOPIC(topic),
      RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY), // maybe RD_KAFKA_MSG_F_FREE?
      RD_KAFKA_V_KEY((void *)key, keyLen),
      RD_KAFKA_V_VALUE((void *)value, valueLen), RD_KAFKA_V_OPAQUE(NULL),
      RD_KAFKA_V_END);

  if (err) {
    m_atomicFlag.clear();

    throw std::runtime_error("Failed to produce to topic: " + m_topic +
                             ", error: " + std::string(rd_kafka_err2str(err)));
  }

  // we loop until message is delivered or declared fail, and the poll is needed
  // to poll generic error like network not reachable.
  while (m_atomicFlag.test()) {
    rd_kafka_poll(m_kafka, 100);
    rd_kafka_flush(m_kafka, 1000); // this is not configurable, and 1000ms shall
                                   // be good enough for a single message to be
                                   // flushed to the kafka broker.
  }

  // the producerCallback is returned, if an error from delivery the message,
  // we want to return it to the caller of the write api.
  if (m_kafkaErr) {
    throw std::runtime_error(
        "Failed to delivered to topic: " + m_topic +
        ", error: " + std::string(rd_kafka_err2str(m_kafkaErr)));
  }
}

} // namespace Dmn
