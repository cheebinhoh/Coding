/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesgnet-kafka.hpp"

#include <memory>
#include <string_view>

#include "dmn-dmesg.hpp"
#include "dmn-kafka-util.hpp"
#include "dmn-kafka.hpp"

namespace dmn {

Dmn_DMesgNet_Kafka::Dmn_DMesgNet_Kafka(std::string_view name,
                                       Dmn_Kafka::ConfigType configs)
    : m_name{name} {
  // input handle for DMesgNet
  dmn::Dmn_Kafka::ConfigType input_configs{configs};
  input_configs["group.id"] = name;
  input_configs[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  input_configs["auto.offset.reset"] = "earliest";
  input_configs[dmn::Dmn_Kafka::PollTimeoutMs] = "500";

  std::unique_ptr<dmn::Dmn_Kafka> input = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kConsumer, input_configs);

  // output handle for DMesgNet
  dmn::Dmn_Kafka::ConfigType output_configs{configs};
  output_configs["acks"] = "all";
  output_configs[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  output_configs[dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::unique_ptr<dmn::Dmn_Kafka> output = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kProducer, output_configs);

  // DMesgNet
  m_dmesgnet = std::make_unique<dmn::Dmn_DMesgNet>(name, std::move(input),
                                                   std::move(output));
}

Dmn_DMesgNet_Kafka::~Dmn_DMesgNet_Kafka() noexcept try {
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

} // namespace dmn
