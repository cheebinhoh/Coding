/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesgnet-kafka.hpp"
#include "kafka/dmn-kafka.hpp"

#include <optional>
#include <string>

namespace Dmn {

Dmn_DMesgNet_Kafka::Dmn_DMesgNet_Kafka(std::string_view name) : m_name{name} {}

Dmn_DMesgNet_Kafka::~Dmn_DMesgNet_Kafka() {}

} /* End of namespace Dmn */
