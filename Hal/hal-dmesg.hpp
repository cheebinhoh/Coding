/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 * The Hal_DMesg stands for HAL Distributed Messaging framework that
 * it inherits from Hal_Pub and Hal_Pub::Sub classes to implement
 * distributed message synchronization mechanism using Google protobuf
 * as a base message encoding/decoding.
 *
 * It is intentionally modelled to behave like IO but not inherit
 * the Hal_Io due to conflict of template type for Hal_Io in parent
 * classes that also inherits Hal_Io but uses different template type,
 * classic diamond-shape problem of C++ multiple inherittance. NOTE that
 * one solution is to have Hal_DMesgHandler inheritted from Hal_Io but
 * encomposes Hal::Hal_Pub<...> as object than inherit from it.
 */

#ifndef HAL_DMESG_HPP_HAVE_SEEN

#define HAL_DMESG_HPP_HAVE_SEEN

#include "hal-pub-sub.hpp"
#include "proto/hal-dmesg.pb.h"

#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace Hal {

class Hal_DMesg : public Hal_Pub<Hal::DMesgPb> {

  class Hal_DMesgHandler : public Hal::Hal_Pub<Hal::DMesgPb>::Hal_Sub {
  public:
    Hal_DMesgHandler(std::string_view name) : m_name{name} {}

    ~Hal_DMesgHandler() noexcept try {
    } catch (...) {
      // explicit return to resolve exception as destructor must be noexcept
      return;
    }

    Hal_DMesgHandler(const Hal_DMesgHandler &halDMesgHandler) = delete;
    const Hal_DMesgHandler &
    operator=(const Hal_DMesgHandler &halDMesgHandler) = delete;
    Hal_DMesgHandler(Hal_DMesgHandler &&halDMesgHandler) = delete;
    Hal_DMesgHandler &operator=(Hal_DMesgHandler &&halDMesgHandler) = delete;

    /**
     * @brief The method read a DMesg protobuf message out of the handler
     *        opened with DMesg. This is a blocking call until a DMesg
     *        protobuf message is returned or exception is thrown, then
     *        nullopt is returned.
     *
     * @return DMesg protobuf or nullopt if exception is thrown.
     */
    std::optional<Hal::DMesgPb> readDMesg() {
      try {
        Hal::DMesgPb mesgPb = m_buffers.pop();

        return mesgPb;
      } catch (...) {
      }

      return {};
    }

    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     * publisher queue to all subscribers. This method will move the DMesg data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     */
    void writeDMesg(Hal::DMesgPb &&dmesgPb) {
      Hal::DMesgPb movedDMesgPb = std::move_if_noexcept(dmesgPb);

      writeDMesgInternal(movedDMesgPb, true);
    }

    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     * publisher queue to all subscribers. This method will copy the DMesg data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     */
    void writeDMesg(Hal::DMesgPb &dmesgPb) {
      writeDMesgInternal(dmesgPb, false);
    }

    friend class Hal_DMesg;

  protected:
    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     * publisher queue to all subscribers. This method will move the DMesg data
     * if move argument is true, otherwise copy the data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     * @param move True to move than copy the data
     */
    void writeDMesgInternal(Hal::DMesgPb &dmesgPb, bool move) {
      std::string id = dmesgPb.identifier();
      long long nextRunningCounter = m_identifierRunningCounter[id] + 1;

      dmesgPb.set_source(m_name);
      dmesgPb.set_runningcounter(nextRunningCounter);

      if (move) {
        m_owner->publish(std::move_if_noexcept(dmesgPb));
      } else {
        m_owner->publish(dmesgPb);
      }

      m_identifierRunningCounter[id] = nextRunningCounter;
    }

    /**
     * @brief The method will be called by publisher object about new DMesg data
     *        and insert the data into the buffer for subscriber to consume if
     *        the data has NOT been notified before (based on running counter).
     *
     * @param dmesgPb The DMesg protobuf data notified by publisher object
     */
    void notify(Hal::DMesgPb dmesgPb) override {
      if (dmesgPb.source() != m_name) {
        std::string id = dmesgPb.identifier();
        long long runningCounter = m_identifierRunningCounter[id];

        if (dmesgPb.runningcounter() > runningCounter) {
          m_identifierRunningCounter[id] = dmesgPb.runningcounter();
          m_buffers.push(dmesgPb);
        }
      }
    }

  private:
    std::string m_name{};
    Hal_Buffer<Hal::DMesgPb> m_buffers{};
    Hal_DMesg *m_owner{};
    std::map<std::string, long long> m_identifierRunningCounter{};
  }; /* Hal_DMesgHandler */

public:
  Hal_DMesg(std::string_view name) : Hal_Pub{name, 10} {}

  virtual ~Hal_DMesg() noexcept try {
    for (auto &handler : m_handlers) {
      this->unregisterSubscriber(handler.get());
      handler->m_owner = nullptr;
    }
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Hal_DMesg(const Hal_DMesg &halDMesg) = delete;
  const Hal_DMesg &operator=(const Hal_DMesg &halDMesg) = delete;
  Hal_DMesg(Hal_DMesg &&halDMesg) = delete;
  Hal_DMesg &operator=(Hal_DMesg &&halDMesg) = delete;

  /**
   * @brief The method creates a new handler, registers the handler to receive
   *        published message and returns he handler to the caller.
   *
   * @param name the name or unique identification to the handler
   *
   * @return newly created handler
   */
  std::shared_ptr<Hal_DMesgHandler> openHandler(std::string_view name) {
    std::shared_ptr<Hal_DMesgHandler> handler =
        std::make_shared<Hal_DMesgHandler>(name);
    auto handlerRet = handler;

    m_handlers.push_back(handler);
    handler->m_owner = this;
    this->registerSubscriber(handler.get());

    return handlerRet;
  }

  /**
   * @brief The method unregisters and removes the handler from the DMesg and
   *        then free the handler (if handlerToClose argument is the only
   *        shared pointer than one owned by DMesg).
   *
   * @param handlerToClose the handler to be closed
   */
  void closeHandler(std::shared_ptr<Hal_DMesgHandler> &handlerToClose) {
    this->unregisterSubscriber(handlerToClose.get());

    std::vector<std::shared_ptr<Hal_DMesgHandler>>::iterator it = std::find_if(
        m_handlers.begin(), m_handlers.end(), [handlerToClose](auto handler) {
          return handler.get() == handlerToClose.get();
        });

    if (it != m_handlers.end()) {
      m_handlers.erase(it);
    }

    handlerToClose->m_owner = nullptr;
    handlerToClose = {};
  }

protected:
  using Hal_Pub::publish;

  /**
   * @brief The method publishes dmesgPb to registered subscribers.
   *
   * @param dmesgPb The dmesgPb to be published
   */
  void publishInternal(Hal::DMesgPb dmesgPb) override {
    std::string id = dmesgPb.identifier();
    long long nextRunningCounter = m_identifierRunningCounter[id] + 1;

    try {
      dmesgPb.set_runningcounter(nextRunningCounter);
      Hal_Pub::publishInternal(dmesgPb);
      m_identifierRunningCounter[id] = nextRunningCounter;
    } catch (...) {
      throw;
    }
  }

private:
  std::vector<std::shared_ptr<Hal_DMesgHandler>> m_handlers{};
  std::map<std::string, long long> m_identifierRunningCounter{};
};

} /* namespace Hal */

#endif /* HAL_DMESG_HPP_HAVE_SEEN */
