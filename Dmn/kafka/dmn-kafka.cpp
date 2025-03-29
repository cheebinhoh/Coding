/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-kafka.hpp"
#include "dmn-kafka-util.hpp"

#include "rdkafka.h"

#include <cassert>
#include <exception>
#include <expected>
#include <iostream>
#include <functional>
#include <string>

namespace Dmn {
  const std::string Dmn_Kafka::Topic = "Dmn_Kafka.Topic";
  const std::string Dmn_Kafka::Key = "Dmn_Kafka.Key";

  void Dmn_Kafka::kafkaCallback(rd_kafka_t *kafka_handle,
                                const rd_kafka_message_t *rkmessage, void *opaque) {
    Dmn_Kafka *obj = (Dmn_Kafka *)opaque;

    if (rkmessage->err) {
      obj->m_kafkaErr = rkmessage->err;
    }

    obj->m_atomicFlag.clear();
    obj->m_atomicFlag.notify_all();
  }

  Dmn_Kafka::Dmn_Kafka(Dmn_Kafka::Role role, Dmn_Kafka::ConfigType configs) : m_role{role}, m_configs{configs}{
    // create kafka configuration
    m_kafkaConf = rd_kafka_conf_new();

    for (auto & c : m_configs) {
       if (Dmn_Kafka::Topic == c.first) {
         m_topic = c.second;

         continue;
       }

       if (Dmn_Kafka::Key == c.first) {
         m_topic = c.second;

         continue;
       }

       auto res = Dmn::set_config(m_kafkaConf, c.first.c_str(), c.second.c_str());
       if (!res.has_value()) {
         throw std::runtime_error("Error in setting kafka configuration: "
                                  + c.first + " to value: " + c.second);
       }

       assert(res.value() == RD_KAFKA_CONF_OK);
    }

    rd_kafka_conf_set_dr_msg_cb(m_kafkaConf, Dmn_Kafka::kafkaCallback);
    rd_kafka_conf_set_opaque(m_kafkaConf, (void *)this);

    // Create the Producer instance.
    char errstr[512];
    m_kafka = rd_kafka_new(RD_KAFKA_PRODUCER, m_kafkaConf, errstr, sizeof(errstr));
    if (!m_kafka) {
       throw std::runtime_error("Failed to create new producer: " + std::string(errstr));
    }

    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    m_kafkaConf = NULL;
  }

  Dmn_Kafka::~Dmn_Kafka() noexcept try {

    if (m_kafkaConf) {
      rd_kafka_conf_destroy(m_kafkaConf);
    }

    if (m_kafka) {
      rd_kafka_flush(m_kafka, 10 * 1000);

      rd_kafka_destroy(m_kafka);
    }
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  std::optional<std::string> Dmn_Kafka::read() {
    return {};
  }

  void Dmn_Kafka::write(std::string &item) {
    write(item, false);
  }

  void Dmn_Kafka::write(std::string &&item) {
    write(item, true);
  }

  void Dmn_Kafka::write(std::string &item, bool move) {
    while (m_atomicFlag.test_and_set(std::memory_order_acquire))
      m_atomicFlag.wait(true, std::memory_order_relaxed);

    const char *topic = m_topic.c_str();
    const char *key = m_key.c_str();
    const size_t keyLen = m_key.size();
    const char *value = item.c_str();
    const size_t valueLen = item.size();

    rd_kafka_resp_err_t err = rd_kafka_producev(m_kafka, RD_KAFKA_V_TOPIC(topic),
                                                RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                                                RD_KAFKA_V_KEY((void *)key, keyLen),
                                                RD_KAFKA_V_VALUE((void *)value, valueLen),
                                                RD_KAFKA_V_OPAQUE(NULL),
                                                RD_KAFKA_V_END);

    if (err) {
      throw std::runtime_error("Failed to produce to topic: " + m_topic + ", error: " + std::string(rd_kafka_err2str(err)));
    }

    rd_kafka_poll(m_kafka, 0);
    rd_kafka_flush(m_kafka, 10 * 1000);

    m_atomicFlag.wait(true);
    if (m_kafkaErr) {
      throw std::runtime_error("Failed to delivered to topic: " + m_topic + ", error: " + std::string(rd_kafka_err2str(m_kafkaErr)));
    }
  }
}

