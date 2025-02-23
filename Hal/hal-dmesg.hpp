/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 */

#ifndef HAL_DMESG_HPP_HAVE_SEEN

#define HAL_DMESG_HPP_HAVE_SEEN

#include "hal-pub-sub.hpp"
#include "proto/hal-dmesg.pb.h"

#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace Hal {
class Hal_DMesg : public Hal_Pub<Hal::DMesgPb> {
  class Hal_DMesgHandler : public Hal::Hal_Pub<Hal::DMesgPb>::Hal_Sub {
  public:
    Hal_DMesgHandler(std::string_view name) : m_name{name} {}

    Hal_DMesgHandler(const Hal_DMesgHandler &halDMesgHandler) = delete;
    const Hal_DMesgHandler &
    operator=(const Hal_DMesgHandler &halDMesgHandler) = delete;
    Hal_DMesgHandler(Hal_DMesgHandler &&halDMesgHandler) = delete;
    Hal_DMesgHandler &operator=(Hal_DMesgHandler &&halDMesgHandler) = delete;

    void notify(Hal::DMesgPb dmesgPb) override { m_buffers.push(dmesgPb); }

    std::optional<Hal::DMesgPb> readDMesg() {
      Hal::DMesgPb mesgPb = m_buffers.pop();

      return mesgPb;
    }

  private:
    std::string m_name{};
    Hal_Buffer<Hal::DMesgPb> m_buffers{};
  }; /* Hal_DMesgHandler */

public:
  Hal_DMesg(std::string_view name) : Hal_Pub{name, 10} {}

  virtual ~Hal_DMesg() noexcept try {
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Hal_DMesg(const Hal_DMesg &halDMesg) = delete;
  const Hal_DMesg &operator=(const Hal_DMesg &halDMesg) = delete;
  Hal_DMesg(Hal_DMesg &&halDMesg) = delete;
  Hal_DMesg &operator=(Hal_DMesg &&halDMesg) = delete;

  std::shared_ptr<Hal_DMesgHandler> openHandler(std::string_view name) {
    std::shared_ptr<Hal_DMesgHandler> handler =
        std::make_shared<Hal_DMesgHandler>(name);
    auto handlerRet = handler;

    m_handlers.push_back(handler);
    this->registerSubscriber(handler.get());

    return handlerRet;
  }

  // open shared ptr of Hal_DMesgHandler which is subclass of
  // Hal_Pub<Hal::DMesgPb>::Sub Hal_DMesgHandler maintains a list of
  // Hal::DMesgPb read from Hal_Pub but yet to be read by client of the
  // Hal_DMesgHandler So the Hal_DMesgHandler also inherit Hal::Hal_Io Each read
  // of Hal_DMesgHandler will either blocking and waiting for data from Hal_Pub
  // or it returns the next Hal::DMesgPb, so it is blocking like Tcp socket.

protected:
private:
  std::vector<std::shared_ptr<Hal_DMesgHandler>> m_handlers{};
};
} /* namespace Hal */

#endif /* HAL_DMESG_HPP_HAVE_SEEN */
