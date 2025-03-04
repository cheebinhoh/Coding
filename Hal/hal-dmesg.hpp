/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Hal_DMesg stands for HAL Distributed Messaging framework that
 * it inherits from Hal_Pub and Hal_Pub::Sub classes to implement
 * distributed message synchronization mechanism using Google protobuf
 * as a base message encoding/decoding.
 */

#ifndef HAL_DMESG_HPP_HAVE_SEEN

#define HAL_DMESG_HPP_HAVE_SEEN

#include "hal-debug.hpp"
#include "hal-pub-sub.hpp"
#include "proto/hal-dmesg.pb.h"

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <sys/time.h>

namespace Hal {
const std::string DMesgSysIdentifier = "sys.hal-dmesg";

class Hal_DMesg : public Hal_Pub<Hal::DMesgPb> {
  using FilterTask = std::function<bool(const Hal::DMesgPb &)>;
  using AsyncProcessTask = std::function<void(Hal::DMesgPb)>;

public:
  /**
   * @brief The Hal_DMesgHandler is intentionalled modolled to inherit from
   *        Hal_Io that provides read/write IO interface across a range of
   *        IO, like socket, file, etc, and the Hal_DMesgHandler is just
   *        another IO, because of that we do NOT able to inherit from Hal_Pub
   *        as Hal_Pub inherit Hal_Async which inherits from Hal_Pipe which
   *        inherit Hal_Io with template type specialized to functor, this
   *        is a diamond shape multiple inherittance where common parent
   *        has to have same instantiated template type. Object composition
   *        helps resolve the problem.
   */
  class Hal_DMesgHandler : public Hal_Io<Hal::DMesgPb> {
    using ConflictCallbackTask =
        std::function<void(Hal_DMesgHandler &handler, const Hal::DMesgPb &)>;

    class Hal_DMesgHandlerSub : public Hal::Hal_Pub<Hal::DMesgPb>::Hal_Sub {
    public:
      Hal_DMesgHandlerSub() = default;

      ~Hal_DMesgHandlerSub() noexcept try {
      } catch (...) {
        // explicit return to resolve exception as destructor must be noexcept
        return;
      }

      Hal_DMesgHandlerSub(const Hal_DMesgHandlerSub &halDMesgHandlerSub) =
          delete;
      const Hal_DMesgHandlerSub &
      operator=(const Hal_DMesgHandlerSub &halDMesgHandlerSub) = delete;
      Hal_DMesgHandlerSub(Hal_DMesgHandlerSub &&halDMesgHandlerSub) = delete;
      Hal_DMesgHandlerSub &
      operator=(Hal_DMesgHandlerSub &&halDMesgHandlerSub) = delete;

      /**
       * @brief The method will be called by publisher object about new DMesg
       *        data and insert the data into the buffer for subscriber to
       *        consume if the data has NOT been notified before (based on
       *        running counter).
       *
       * @param dmesgPb The DMesg protobuf data notified by publisher object
       */
      void notify(Hal::DMesgPb dmesgPb) override {
        if (dmesgPb.source() != m_owner->m_name ||
            dmesgPb.type() == Hal::DMesgTypePb::sys) {
          std::string id = dmesgPb.identifier();
          long long runningCounter = m_owner->m_identifierRunningCounter[id];

          if (dmesgPb.runningcounter() > runningCounter) {
            m_owner->m_identifierRunningCounter[id] = dmesgPb.runningcounter();

            if (dmesgPb.type() == Hal::DMesgTypePb::sys) {
              m_owner->m_lastDMesgSysPb = dmesgPb;

              if (dmesgPb.source() == m_owner->m_name) {
                m_owner->m_initialized = true;
              }

              HAL_DEBUG_PRINT(std::cout << "who: " << m_owner->m_name
                                        << ", initialized: " << std::boolalpha
                                        << m_owner->m_initialized
                                        << ", sys.source: " << dmesgPb.source()
                                        << ", sys.runningcounter: "
                                        << dmesgPb.runningcounter() << "\n");
            } else if (!m_owner->m_filterFn || m_owner->m_filterFn(dmesgPb)) {

              if (m_owner->m_asyncProcessFn) {
                m_owner->m_asyncProcessFn(std::move_if_noexcept(dmesgPb));
              } else {
                m_owner->m_buffers.push(dmesgPb);
              }
            }
          }
        }
      }

      friend class Hal_DMesgHandler;

    private:
      Hal_DMesgHandler *m_owner{};
    };

  public:
    Hal_DMesgHandler(std::string_view name, FilterTask filterFn = nullptr,
                     AsyncProcessTask asyncProcessFn = nullptr)
        : m_name{name}, m_filterFn{filterFn}, m_asyncProcessFn{asyncProcessFn} {
      m_sub.m_owner = this;
    }

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

    bool isInConflict() const { return m_inConflict; }

    /**
     * @brief The method reads a DMesg protobuf message out of the handler
     *        opened with DMesg. This is a blocking call until a DMesg
     *        protobuf message is returned or exception is thrown, then
     *        nullopt is returned.
     *
     * @return DMesg protobuf or nullopt if exception is thrown.
     */
    std::optional<Hal::DMesgPb> read() override {
      try {
        Hal::DMesgPb mesgPb = m_buffers.pop();

        return mesgPb;
      } catch (...) {
      }

      return {};
    }

    /**
     * @brief The method marks the handler as conflict resolved.
     */
    void resolveConflict() { m_inConflict = false; }

    /**
     * @brief The method set the callback function for conflict.
     *
     * @param cb The conflict callback function
     */
    void setConflictCallbackTask(ConflictCallbackTask cb) {
      m_conflictCallbackFn = cb;
    }

    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     *        publisher queue to all subscribers. This method will move the
     *        DMesg data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     */
    void write(Hal::DMesgPb &&dmesgPb) override {
      Hal::DMesgPb movedDMesgPb = std::move_if_noexcept(dmesgPb);

      writeDMesgInternal(movedDMesgPb, true);
    }

    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     *        publisher queue to all subscribers. This method will copy the
     *        DMesg data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     */
    void write(Hal::DMesgPb &dmesgPb) override {
      writeDMesgInternal(dmesgPb, false);
    }

    void waitForEmpty() { m_sub.waitForEmpty(); }

    friend class Hal_DMesg;
    friend class Hal_DMesgHandlerSub;

  protected:
    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     *        publisher queue to all subscribers. This method will move the
     *        DMesg data if move argument is true, otherwise copy the data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     * @param move True to move than copy the data
     */
    void writeDMesgInternal(Hal::DMesgPb &dmesgPb, bool move) {
      if (m_inConflict) {
        throw std::runtime_error("last write results in conflicted");
      }

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

  private:
    /**
     * @brief The method marks the writer as in conflict state and executes the
     *        conflict callback function in the caller async context.
     *
     * @param mesgPb The dmesgPb data that results in conflict state
     */
    void throwConflict(const Hal::DMesgPb dmesgPb) {
      m_inConflict = true;

      if (m_conflictCallbackFn) {
        m_sub.write(
            [this, dmesgPb]() { this->m_conflictCallbackFn(*this, dmesgPb); });
      }
    }

    std::string m_name{};
    FilterTask m_filterFn{};
    AsyncProcessTask m_asyncProcessFn{};

    Hal_Buffer<Hal::DMesgPb> m_buffers{};
    Hal_DMesg *m_owner{};
    std::map<std::string, long long> m_identifierRunningCounter{};
    Hal_DMesgHandlerSub m_sub{};
    ConflictCallbackTask m_conflictCallbackFn{};
    bool m_inConflict{};
    Hal::DMesgPb m_lastDMesgSysPb{};
    bool m_initialized{};
  }; /* Hal_DMesgHandler */

  Hal_DMesg(std::string_view name) : Hal_Pub{name, 10}, m_name{name} {}

  virtual ~Hal_DMesg() noexcept try {
    for (auto &handler : m_handlers) {
      this->unregisterSubscriber(&(handler->m_sub));
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
   * @param filterFn the functor callback that returns false to filter out Dmesg
   *                 message, if no functor is provided, no filter is performed.
   * @param asyncProcess the functor callback to process each notified Dmesg
   *                 message
   *
   * @return newly created handler
   */
  std::shared_ptr<Hal_DMesgHandler>
  openHandler(std::string_view name, FilterTask filterFn = nullptr,
              AsyncProcessTask asyncProcessFn = nullptr) {
    std::shared_ptr<Hal_DMesgHandler> handler =
        std::make_shared<Hal_DMesgHandler>(name, filterFn, asyncProcessFn);
    auto handlerRet = handler;

    m_handlers.push_back(handler);
    handler->m_owner = this;
    this->registerSubscriber(&(handler->m_sub));

    // generate a sys message on behalf of the opening handler as a sort
    // of heartbeat.
    Hal::DMesgPb hbSysPb{};
    hbSysPb.set_identifier(DMesgSysIdentifier);
    hbSysPb.set_source(name);
    hbSysPb.set_type(Hal::DMesgTypePb::sys);

    Hal::DMesgBodyPb *dmesgSysBodyPb = hbSysPb.mutable_body();
    Hal::DMesgSysPb *dmesgSysBodySysPb = dmesgSysBodyPb->mutable_sys();
    dmesgSysBodySysPb->set_identifier(m_name);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    dmesgSysBodySysPb->mutable_timestamp()->set_seconds(tv.tv_sec);
    dmesgSysBodySysPb->mutable_timestamp()->set_nanos(tv.tv_usec * 1000);

    HAL_ASYNC_CALL_WITH_CAPTURE({ this->publishSysInternal(hbSysPb); }, this,
                                hbSysPb);

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
    this->unregisterSubscriber(&(handlerToClose->m_sub));

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
   * @brief The method publishes system message through async context queue.
   *
   * @param dmesgSysPb The system dmesg protobuf message
   */
  void publishSysInternal(Hal::DMesgPb dmesgSysPb) {
    assert(dmesgSysPb.identifier() == DMesgSysIdentifier);
    assert(dmesgSysPb.type() == Hal::DMesgTypePb::sys);

    std::string id = dmesgSysPb.identifier();
    long long nextRunningCounter = m_identifierRunningCounter[id] + 1;

    try {
      dmesgSysPb.set_runningcounter(nextRunningCounter);
      Hal_Pub::publishInternal(dmesgSysPb);
      m_identifierRunningCounter[id] = nextRunningCounter;
    } catch (...) {
      assert("unexpected expection" == nullptr);
    }
  }

  /**
   * @brief The method publishes dmesgPb to registered subscribers. If the to be
   *        published dmesgPb has smaller runningcounter than what is in core,
   *        it means that the writer is out of sync and in race condition that
   *        it has not yet receive the last published data from core, but write
   *        to publisher with backdate running counter, in this case, we set the
   *        writer handler to be in conflict state, and throws exception for
   *        future write until it manually mark it as conflict resolved. We do
   *        not put the whole core as in conflict, but the particular writer.
   *
   * @param dmesgPb The dmesgPb to be published
   */
  void publishInternal(Hal::DMesgPb dmesgPb) override {
    std::string id = dmesgPb.identifier();
    long long nextRunningCounter = m_identifierRunningCounter[id] + 1;

    if (dmesgPb.runningcounter() < nextRunningCounter) {
      std::vector<std::shared_ptr<Hal_DMesgHandler>>::iterator it =
          std::find_if(m_handlers.begin(), m_handlers.end(),
                       [&dmesgPb](auto handler) {
                         return handler->m_name == dmesgPb.source();
                       });

      if (it != m_handlers.end()) {
        (*it)->throwConflict(dmesgPb);

        return;
      }
    }

    try {
      dmesgPb.set_runningcounter(nextRunningCounter);
      Hal_Pub::publishInternal(dmesgPb);
      m_identifierRunningCounter[id] = nextRunningCounter;
    } catch (...) {
      throw;
    }
  }

private:
  std::string m_name{};

  std::vector<std::shared_ptr<Hal_DMesgHandler>> m_handlers{};
  std::map<std::string, long long> m_identifierRunningCounter{};
};

} /* namespace Hal */

#endif /* HAL_DMESG_HPP_HAVE_SEEN */
