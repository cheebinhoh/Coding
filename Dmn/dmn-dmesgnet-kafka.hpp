/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Dmn_DMesgNet_KAFKA stands for Distributed Messaging Network over kafka,
 * it is an extension to the Dmn_DMesgNet with support of distributing the
 * DMesgPb message over kakfa network.
 */

#ifndef DMN_DMESGNET_KAFKA_HPP_HAVE_SEEN

#define DMN_DMESGNET_KAFKA_HPP_HAVE_SEEN

#include "dmn-debug.hpp"
#include "dmn-dmesgnet.hpp"

#include "kafka/dmn-kafka.hpp"

#include <memory>
#include <string>

namespace Dmn {

class Dmn_DMesgNet_Kafka {
public:
  Dmn_DMesgNet_Kafka(std::string_view name, Dmn_Kafka::ConfigType kafkaConfigs);
  ~Dmn_DMesgNet_Kafka();

  Dmn_DMesgNet_Kafka(const Dmn_DMesgNet_Kafka &dmnDMesgNetKafka) = delete;
  const Dmn_DMesgNet_Kafka &
  operator=(const Dmn_DMesgNet_Kafka &dmnDMesgNetKafka) = delete;
  Dmn_DMesgNet_Kafka(Dmn_DMesgNet_Kafka &&dmnDMesgNetKafka) = delete;
  Dmn_DMesgNet_Kafka &operator=(Dmn_DMesgNet_Kafka &&dmnDMesgNetKafka) = delete;

protected:
  /**
   * data members for constructor to instantiate the object.
   */
  std::string m_name{};
  Dmn_Kafka::ConfigType m_kafkaConfigs{};

  /**
   * data members for internal logic.
   */
  std::unique_ptr<Dmn_Kafka> m_consumer{};
  std::unique_ptr<Dmn_Kafka> m_producer{};
  std::unique_ptr<Dmn_DMesgNet> m_dmesgNet{};
}; /* End of class Dmn_DMesgNet_Kafka */

} /* End of namespace Dmn */

#endif /* DMN_DMESGNET_KAFKA_HPP_HAVE_SEEN */
