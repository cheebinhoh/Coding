/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef DMN_DMESGNET_KAFKA_HPP_HAVE_SEEN

#define DMN_DMESGNET_KAFKA_HPP_HAVE_SEEN

#include "dmn-dmesg.hpp"
#include "dmn-dmesgnet.hpp"
#include "dmn-kafka.hpp"

#include <memory>
#include <string_view>

namespace Dmn {

class Dmn_DMesgNet_Kafka {
public:
  Dmn_DMesgNet_Kafka(std::string_view name, Dmn_Kafka::ConfigType configs);
  ~Dmn_DMesgNet_Kafka() noexcept;

  Dmn_DMesgNet_Kafka(const Dmn_DMesgNet_Kafka &dmnDMesgNet_Kafka) = delete;
  const Dmn_DMesgNet_Kafka &
  operator=(const Dmn_DMesgNet_Kafka &dmnDMesgNet_Kafka) = delete;
  Dmn_DMesgNet_Kafka(Dmn_DMesgNet_Kafka &&dmnDMesgNet_Kafka) = delete;
  Dmn_DMesgNet_Kafka &
  operator=(Dmn_DMesgNet_Kafka &&dmnDMesgNet_Kafka) = delete;

  template <class... U>
  std::shared_ptr<Dmn_DMesg::Dmn_DMesgHandler> openHandler(U &&...arg);

private:
  std::string m_name{};
  std::shared_ptr<Dmn_DMesgNet> m_dmesgNet{};
};

} /* End of namespace Dmn */

#endif /* End of macro DMN_DMESGNET_KAFKA_HPP_HAVE_SEEN */
