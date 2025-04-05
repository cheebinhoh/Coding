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
#include "dmn-dmesg-pb-util.hpp"
#include "dmn-dmesgnet.hpp"
#include "dmn-io.hpp"
#include "dmn-util.hpp"

#include "kafka/dmn-kafka.hpp"

#include "proto/dmn-dmesg.pb.h"

#include <optional>
#include <string>

namespace Dmn {

class Dmn_DMesgNet_Kafka : public Dmn_Io<Dmn::DMesgPb> {
  Dmn_DMesgNet_Kafka(std::string_view name);
  ~Dmn_DMesgNet_Kafka();

  std::optional<Dmn::DMesgPb> read() override;
  void write(Dmn::DMesgPb &item) override;
  void write(Dmn::DMesgPb &&item) override;

public:
  Dmn_DMesgNet_Kafka(const Dmn_DMesgNet_Kafka &dmnDMesgNetKafka) = delete;
  const Dmn_DMesgNet_Kafka &
  operator=(const Dmn_DMesgNet_Kafka &dmnDMesgNetKafka) = delete;
  Dmn_DMesgNet_Kafka(Dmn_DMesgNet_Kafka &&dmnDMesgNetKafka) = delete;
  Dmn_DMesgNet_Kafka &operator=(Dmn_DMesgNet_Kafka &&dmnDMesgNetKafka) = delete;

protected:
  std::string m_name{};
}; /* End of class Dmn_DMesgNet_Kafka */

} /* End of namespace Dmn */

#endif /* DMN_DMESGNET_KAFKA_HPP_HAVE_SEEN */
