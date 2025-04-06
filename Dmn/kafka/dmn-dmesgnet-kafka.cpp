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
                                       Dmn_Kafka::ConfigType configs) {
  // reader for DMesgNet
  Dmn::Dmn_Kafka::ConfigType readConfigs{configs};
  readConfigs["group.id"] = name;
  readConfigs[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  readConfigs["auto.offset.reset"] = "earliest";

  // FIXME: for som reason if polltimeout is not less than 3000, we have a
  // segmentation upon tear down of consumer, I suspect that it is because a
  // thread is spending more time waiting on a data in rdkafka software stack at
  // a particular cancellation upon a tear down happens.
  readConfigs[Dmn::Dmn_Kafka::PollTimeoutMs] = "3000";

  std::shared_ptr<Dmn::Dmn_Kafka> consumer = std::make_shared<Dmn::Dmn_Kafka>(
      Dmn::Dmn_Kafka::Role::Consumer, readConfigs);

  // writer for DMesgNet
  Dmn::Dmn_Kafka::ConfigType writeConfigs{configs};
  writeConfigs["acks"] = "all";
  writeConfigs[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  writeConfigs[Dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::shared_ptr<Dmn::Dmn_Kafka> producer = std::make_shared<Dmn::Dmn_Kafka>(
      Dmn::Dmn_Kafka::Role::Producer, writeConfigs);

  m_dmesgNet = std::make_shared<Dmn::Dmn_DMesgNet>(name, producer, consumer);
}

Dmn_DMesgNet_Kafka::~Dmn_DMesgNet_Kafka() noexcept try {
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

template <class... U>
std::shared_ptr<Dmn_DMesg::Dmn_DMesgHandler>
Dmn_DMesgNet_Kafka::openHandler(U &&...arg) {
  return m_dmesgNet->openHandler(std::forward<U>(arg)...);
}

} /* End of namespace Dmn */
