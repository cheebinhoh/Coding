/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Hal_DMesgNet stands for HAL Distributed Messaging over network,
 * it acts as short of agent to integrating the Hal_DMesg within a
 * host over Hal_Io with another Hal_DMesg across network or IPC.
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
  Hal_DMesgNet(std::string_view name,
               std::shared_ptr<Hal_Io<std::string>> output = nullptr)
      : Hal_DMesg{name}, m_name{name}, m_outputHandler{output} {
    m_subscriptHandler = Hal_DMesg::openHandler(
        m_name,
        [this](const Hal::DMesgPb &dmesgPb) {
          return dmesgPb.source() != this->m_name;
        },
        [this](Hal::DMesgPb dmesgPb) mutable {
          if (m_outputHandler) {
            std::string serialized_string{};

            dmesgPb.set_source(this->m_name); // so that we can skip it
                                              // if we read it back too.
            dmesgPb.SerializeToString(&serialized_string);

            m_outputHandler->write(serialized_string);
          }
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
  std::shared_ptr<Hal_Io<std::string>> m_outputHandler{};

  std::shared_ptr<Hal_DMesgHandler> m_subscriptHandler{};
};

} /* namespace Hal */

#endif /* HAL_DMESGNET_HPP_HAVE_SEEN */
