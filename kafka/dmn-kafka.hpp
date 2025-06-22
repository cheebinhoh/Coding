/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Dmn_Kafka is a wrapper module to Kafka c++ interface and implement Dmn_Io
 * interface with integration to Kafka broker through Kafka c++ interface.
 */

#ifndef DMN_KAFKA_HPP_

#define DMN_KAFKA_HPP_

#include <atomic>
#include <expected>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "rdkafka.h"

#include "dmn-io.hpp"
#include "dmn-kafka-util.hpp"

namespace dmn {

class Dmn_Kafka : public dmn::Dmn_Io<std::string> {
public:
  /**
   * @brief The configuration key specific to the Dmn_Kafka module (not directly
   *        to rdkafka).
   */
  const static std::string Topic;         // topic to read from or write to
  const static std::string Key;           // the key that the topic is writen to
  const static std::string PollTimeoutMs; // timeout in ms for consumer poll to
                                          // break out

  using ConfigType = std::unordered_map<std::string, std::string>;

  enum class Role {
    kConsumer,
    kProducer,
  };

  /**
   * @brief The constructor method that creates kafka consumer or producer with
   *        the set of configurations passed in.
   *
   * @param role    The object created acts as a kafka consumer or producer
   * @param configs The set of key value configurations to rdkafka or Dmn_Kafka
   */
  Dmn_Kafka(Role role, ConfigType configs = {});
  ~Dmn_Kafka() noexcept;

  Dmn_Kafka(const Dmn_Kafka &obj) = delete;
  const Dmn_Kafka &operator=(const Dmn_Kafka &obj) = delete;
  Dmn_Kafka(Dmn_Kafka &&obj) = delete;
  Dmn_Kafka &operator=(Dmn_Kafka &&obj) = delete;

  /**
   * @brief The method returns next topic' message that kafka consumer fetches
   *        from kafka broker, or nullptr if the PollTimeoutMs is elapsed
   *        without next message.
   *
   * @return nullptr if no message before PollTimeoutMs is elapsed, or next
   *         message.
   */
  std::optional<std::string> read() override;

  /**
   * @brief The method writes the message (of the topic) to Kafka broker.
   *
   * @param item The string to be written to the Kafka broker
   */
  void write(std::string &item) override;

  /**
   * @brief The method writes the message (of the topic) to Kafka broker, this
   *        might move the string if implementation desires and supports it.
   *
   * @param item The string to be written to the Kafka broker
   */
  void write(std::string &&item) override;

private:
  /**
   * @brief The callback for rdkafka message specific error, so it is callback
   *        for producer.
   *
   * @param kafka_handle The kafka handler from kafka c++ module
   * @param rkmessage    The kafka message sent delivered by kafka producer
   * @param opaque       The pointer to Dmn_Kafka object
   */
  static void producerCallback(rd_kafka_t *kafka_handle,
                               const rd_kafka_message_t *rkmessage,
                               void *opaque);

  /**
   * @brief The callback invoked by kafka c++ module for generic rdkafka error
   *        (not message specific error) for producer.
   *
   * @param kafka_handle The kafka handler from kafka c++ module
   * @param err          The kafka error code related to deliverying the message
   * @param reason       The kafka error code explanation in string
   * @param opaque       The pointer to Dmn_Kafka object
   */
  static void errorCallback(rd_kafka_t *kafka_handle, int err,
                            const char *reason, void *opaque);

  /**
   * @brief The method publishes the data item to kafka broker, the data item
   *        is moved (if supported) if move is true, or copy otherwise.
   *
   * @param item The data item to be published
   * @param move The data item is moved (if support) and then published if true
   */
  void write(std::string &item, bool move);

  /**
   * data members for constructor to instantiate the object.
   */
  Role m_role{};
  ConfigType m_configs{};

  /**
   * data members for internal logic.
   */
  std::string m_key{};
  std::string m_topic{};
  long long m_poll_timeout_ms{};

  rd_kafka_t *m_kafka{};
  rd_kafka_resp_err_t m_kafka_err{};

  std::atomic_flag m_write_atomic_flag{};
}; // class Dmn_Kafka

} // namespace dmn

#endif // DMN_KAFKA_HPP_
