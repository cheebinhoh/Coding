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
#include <string>

namespace Dmn {

class Dmn_Kafka : public Dmn::Dmn_Io<std::string> {
public:
  const static std::string Topic;
  const static std::string Key;

  using ConfigType = std::map<std::string, std::string>;

  enum class Role {
    Consumer = 0,
    Producer = 0
  };

  Dmn_Kafka(Role role, ConfigType configs = {});

  ~Dmn_Kafka() noexcept;

  Dmn_Kafka(const Dmn_Kafka &dmnDMesgHandlerSub) = delete;
  const Dmn_Kafka & operator=(const Dmn_Kafka &dmnDMesgHandlerSub) = delete;
  Dmn_Kafka(Dmn_Kafka &&dmnDMesgHandlerSub) = delete;
  Dmn_Kafka & operator=(Dmn_Kafka &&dmnDMesgHandlerSub) = delete;

  std::optional<std::string> read() override;
  void write(std::string &item) override;
  void write(std::string &&item) override;

  static void kafkaCallback(rd_kafka_t *kafka_handle,
                            const rd_kafka_message_t *rkmessage,
                            void *opaque);

private:
  void write(std::string &item, bool move);

  /**
   * data members for constructor to instantiate the object.
   */
  Role       m_role{};
  ConfigType m_configs{};

  /**
   * data members for internal logic.
   */
  std::string m_key{};
  std::string m_topic{};

  rd_kafka_t *m_kafka{};
  rd_kafka_conf_t *m_kafkaConf{};
  rd_kafka_resp_err_t m_kafkaErr{};

  std::atomic_flag m_atomicFlag{};
};

} /* namespace Dmn */

#endif /* DMN_KAFKA_HPP_HAVE_SEEN */



