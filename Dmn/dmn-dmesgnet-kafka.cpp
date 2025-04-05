/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesgnet-kafka.hpp"
#include "kafka/dmn-kafka.hpp"

#include <optional>
#include <string>

namespace Dmn {

Dmn_DMesgNet_Kafka::Dmn_DMesgNet_Kafka(std::string_view name,
                                       Dmn_Kafka::ConfigType kafkaConfigs)
    : m_name{name}, m_kafkaConfigs{kafkaConfigs} {
  m_kafkaConfigs["group.id"] = name;
}

Dmn_DMesgNet_Kafka::~Dmn_DMesgNet_Kafka() {}

} /* End of namespace Dmn */
