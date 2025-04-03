/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef DMN_KAFKA_HPP_HAVE_SEEN

#define DMN_KAFKA_HPP_HAVE_SEEN

#include "dmn-io.hpp"
#include "dmn-kafka-util.hpp"

#include "rdkafka.h"

#include <atomic>
#include <expected>
#include <functional>
#include <map>
#include <optional>
#include <string>

namespace Dmn {

class Dmn_Kafka : public Dmn::Dmn_Io<std::string> {
public:
  /**
   * @brief The configuration key specific to the Dmn_Kafka module (not directly
   *        to rdkafka).
   */
  const static std::string Topic; // topic to read from or write to
  const static std::string Key;   // the key that the topic is writen to
  const static std::string
      PollTimeoutMs; // timeout in ms before consumer poll is break out

  using ConfigType = std::map<std::string, std::string>;

  enum class Role {
    Consumer,
    Producer,
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

  Dmn_Kafka(const Dmn_Kafka &dmnDMesgHandlerSub) = delete;
  const Dmn_Kafka &operator=(const Dmn_Kafka &dmnDMesgHandlerSub) = delete;
  Dmn_Kafka(Dmn_Kafka &&dmnDMesgHandlerSub) = delete;
  Dmn_Kafka &operator=(Dmn_Kafka &&dmnDMesgHandlerSub) = delete;

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
   *        might move the string if implementation desires it.
   *
   * @param item The string to be written to the Kafka broker
   */
  void write(std::string &&item) override;

private:
  /**
   * @brief The callback for rdkafka general error (not message specific error).
   */
  static void producerCallback(rd_kafka_t *kafka_handle,
                               const rd_kafka_message_t *rkmessage,
                               void *opaque);

  /**
   * @brief The callback for rdkafka message specific error, so it is callback
   *        for producer.
   */
  static void errorCallback(rd_kafka_t *kafka_handle, int err,
                            const char *reason, void *opaque);

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
  long long m_pollTimeoutMs{};

  rd_kafka_t *m_kafka{};
  rd_kafka_resp_err_t m_kafkaErr{};

  std::atomic_flag m_atomicFlag{};
};

} /* End of namespace Dmn */

#endif /* End of macro DMN_KAFKA_HPP_HAVE_SEEN */
