/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Hal_DMesgNet stands for HAL Distributed Messaging over network.
 */

#ifndef HAL_DMESGNET_HPP_HAVE_SEEN

#define HAL_DMESGNET_HPP_HAVE_SEEN

#include "hal-dmesg.hpp"
#include "hal-io.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace Hal {

class Hal_DMesgNet : public Hal_DMesg {
public:
  Hal_DMesgNet(std::string_view name) : Hal_DMesg{name}, m_name{name} {
    m_subscriptHandler = Hal_DMesg::openHandler(
        m_name,
        [this](const Hal::DMesgPb &dmesgPb) {
          return dmesgPb.source() != this->m_name;
        },
        [this](Hal::DMesgPb dmesgPb) {
          // FIXME: send output to the network!
        });
  }

  virtual ~Hal_DMesgNet() noexcept try {
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Hal_DMesgNet(const Hal_DMesgNet &halDMesgNet) = delete;
  const Hal_DMesgNet &operator=(const Hal_DMesgNet &halDMesgNet) = delete;
  Hal_DMesgNet(Hal_DMesgNet &&halDMesgNet) = delete;
  Hal_DMesgNet &operator=(Hal_DMesgNet &&halDMesgNet) = delete;

private:
  std::string m_name{};
  std::shared_ptr<Hal_DMesgHandler> m_subscriptHandler{};
};

} /* namespace Hal */

#endif /* HAL_DMESGNET_HPP_HAVE_SEEN */
