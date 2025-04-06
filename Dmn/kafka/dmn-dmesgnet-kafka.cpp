/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesgnet-kafka.hpp"

#include "dmn-dmesg.hpp"
#include "dmn-kafka.hpp"

#include <memory>
#include <string_view>

namespace Dmn {

Dmn_DMesgNet_Kafka::Dmn_DMesgNet_Kafka(std::string_view name,
                                       Dmn_Kafka::ConfigType configs) {}

Dmn_DMesgNet_Kafka::~Dmn_DMesgNet_Kafka() noexcept try {
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

template <class... U>
std::shared_ptr<Dmn_DMesg::Dmn_DMesgHandler>
Dmn_DMesgNet_Kafka::openHandler(U &&...arg) {}

} /* End of namespace Dmn */
