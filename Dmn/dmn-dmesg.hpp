/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Dmn_DMesg stands for Distributed Messaging framework that
 * it inherits from Dmn_Pub and Dmn_Pub::Sub classes to implement
 * distributed message synchronization mechanism using Google protobuf
 * as a base message encoding/decoding.
 */

#ifndef DMN_DMESG_HPP_HAVE_SEEN

#define DMN_DMESG_HPP_HAVE_SEEN

#include "dmn-debug.hpp"
#include "dmn-dmesg-pb-util.hpp"
#include "dmn-pub-sub.hpp"
#include "proto/dmn-dmesg.pb.h"

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <sys/time.h>

namespace Dmn {
const std::string DMesgSysIdentifier = "sys.dmn-dmesg";

class Dmn_DMesg : public Dmn_Pub<Dmn::DMesgPb> {
  using FilterTask = std::function<bool(const Dmn::DMesgPb &)>;
  using AsyncProcessTask = std::function<void(Dmn::DMesgPb)>;

public:
  /**
   * @brief The Dmn_DMesgHandler is intentionalled modolled to inherit from
   *        Dmn_Io that provides read/write IO interface across a range of
   *        IO, like socket, file, etc, and the Dmn_DMesgHandler is just
   *        another IO, because of that we do NOT able to inherit from Dmn_Pub
   *        as Dmn_Pub inherit Dmn_Async which inherits from Dmn_Pipe which
   *        inherit Dmn_Io with template type specialized to functor, this
   *        is a diamond shape multiple inherittance where common parent
   *        has to have same instantiated template type. Object composition
   *        helps resolve the problem.
   */
  class Dmn_DMesgHandler : public Dmn_Io<Dmn::DMesgPb> {
    using ConflictCallbackTask =
        std::function<void(Dmn_DMesgHandler &handler, const Dmn::DMesgPb &)>;

    class Dmn_DMesgHandlerSub : public Dmn::Dmn_Pub<Dmn::DMesgPb>::Dmn_Sub {
    public:
      Dmn_DMesgHandlerSub() = default;

      ~Dmn_DMesgHandlerSub() noexcept try {
      } catch (...) {
        // explicit return to resolve exception as destructor must be noexcept
        return;
      }

      Dmn_DMesgHandlerSub(const Dmn_DMesgHandlerSub &dmnDMesgHandlerSub) =
          delete;
      const Dmn_DMesgHandlerSub &
      operator=(const Dmn_DMesgHandlerSub &dmnDMesgHandlerSub) = delete;
      Dmn_DMesgHandlerSub(Dmn_DMesgHandlerSub &&dmnDMesgHandlerSub) = delete;
      Dmn_DMesgHandlerSub &
      operator=(Dmn_DMesgHandlerSub &&dmnDMesgHandlerSub) = delete;

      /**
       * @brief The method will be called by publisher object about new DMesg
       *        data and insert the data into the buffer for subscriber to
       *        consume if the data has NOT been notified before (based on
       *        running counter).
       *
       * @param dmesgPb The DMesg protobuf data notified by publisher object
       */
      void notify(Dmn::DMesgPb dmesgPb) override {
        if (dmesgPb.sourceidentifier() != m_owner->m_name ||
            dmesgPb.type() == Dmn::DMesgTypePb::sys) {
          std::string id = dmesgPb.topic();
          long long runningCounter = m_owner->m_topicRunningCounter[id];

          if (dmesgPb.runningcounter() > runningCounter) {
            m_owner->m_topicRunningCounter[id] = dmesgPb.runningcounter();

            if (dmesgPb.type() == Dmn::DMesgTypePb::sys) {
              m_owner->m_lastDMesgSysPb = dmesgPb;
            }

            if ((Dmn::DMesgTypePb::sys != dmesgPb.type() ||
                 m_owner->m_includeDMesgSys) &&
                (!m_owner->m_filterFn || m_owner->m_filterFn(dmesgPb))) {
              if (m_owner->m_asyncProcessFn) {
                m_owner->m_asyncProcessFn(std::move_if_noexcept(dmesgPb));
              } else {
                m_owner->m_buffers.push(dmesgPb);
              }
            }
          }
        }
      }

      friend class Dmn_DMesgHandler;

    private:
      Dmn_DMesgHandler *m_owner{};
    };

  public:
    Dmn_DMesgHandler(std::string_view name, FilterTask filterFn = nullptr,
                     AsyncProcessTask asyncProcessFn = nullptr)
        : Dmn_DMesgHandler{name, false, filterFn, asyncProcessFn} {}

    Dmn_DMesgHandler(std::string_view name, bool includeDMesgSys,
                     FilterTask filterFn, AsyncProcessTask asyncProcessFn)
        : m_name{name}, m_includeDMesgSys{includeDMesgSys},
          m_filterFn{filterFn}, m_asyncProcessFn{asyncProcessFn} {
      m_sub.m_owner = this;
    }

    ~Dmn_DMesgHandler() noexcept try {
    } catch (...) {
      // explicit return to resolve exception as destructor must be noexcept
      return;
    }

    Dmn_DMesgHandler(const Dmn_DMesgHandler &dmnDMesgHandler) = delete;
    const Dmn_DMesgHandler &
    operator=(const Dmn_DMesgHandler &dmnDMesgHandler) = delete;
    Dmn_DMesgHandler(Dmn_DMesgHandler &&dmnDMesgHandler) = delete;
    Dmn_DMesgHandler &operator=(Dmn_DMesgHandler &&dmnDMesgHandler) = delete;

    bool isInConflict() const { return m_inConflict; }

    /**
     * @brief The method reads a DMesg protobuf message out of the handler
     *        opened with DMesg. This is a blocking call until a DMesg
     *        protobuf message is returned or exception is thrown, then
     *        nullopt is returned.
     *
     * @return DMesg protobuf or nullopt if exception is thrown.
     */
    std::optional<Dmn::DMesgPb> read() override {
      try {
        Dmn::DMesgPb mesgPb = m_buffers.pop();

        return mesgPb;
      } catch (...) {
      }

      return {};
    }

    /**
     * @brief The method marks the handler as conflict resolved by posting an
     *        asynchronous action on publisher async thread which is one
     *        responsible for putting the handler in conflict state, hence one
     *        responsible to reset it and get it out of the conflict state.
     */
    void resolveConflict() { m_owner->resetHandlerConflictState(this->m_name); }

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
    void write(Dmn::DMesgPb &&dmesgPb) override {
      Dmn::DMesgPb movedDMesgPb = std::move_if_noexcept(dmesgPb);

      writeDMesgInternal(movedDMesgPb, true);
    }

    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     *        publisher queue to all subscribers. This method will copy the
     *        DMesg data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     */
    void write(Dmn::DMesgPb &dmesgPb) override {
      writeDMesgInternal(dmesgPb, false);
    }

    void waitForEmpty() { m_sub.waitForEmpty(); }

    friend class Dmn_DMesg;
    friend class Dmn_DMesgHandlerSub;

  protected:
    /**
     * @brief The method writes and publishes the DMesg protobuf through DMesg
     *        publisher queue to all subscribers. This method will move the
     *        DMesg data if move argument is true, otherwise copy the data.
     *
     * @param dMesgPb The DMesg protobuf to be published
     * @param move True to move than copy the data
     */
    void writeDMesgInternal(Dmn::DMesgPb &dmesgPb, bool move) {
      if (m_inConflict) {
        throw std::runtime_error("last write results in conflicted");
      }

      struct timeval tv;
      gettimeofday(&tv, NULL);

      std::string id = dmesgPb.topic();
      long long nextRunningCounter = m_topicRunningCounter[id] + 1;

      DMESG_PB_SET_TIMESTAMP(dmesgPb, tv);
      DMESG_PB_SET_SOURCEIDENTIFIER(dmesgPb, m_name);
      DMESG_PB_SET_RUNNINGCOUNTER(dmesgPb, nextRunningCounter);

      if (move) {
        m_owner->publish(std::move_if_noexcept(dmesgPb));
      } else {
        m_owner->publish(dmesgPb);
      }

      m_topicRunningCounter[id] = nextRunningCounter;
    }

  private:
    /**
     * @brief The method marks the handler as conflict resolved, and to be
     * executed by the publisher's asynchronous thread context.
     */
    void resolveConflictInternal() { m_inConflict = false; }

    /**
     * @brief The method marks the writer as in conflict state and executes the
     *        conflict callback function in the caller async context.
     *
     * @param mesgPb The dmesgPb data that results in conflict state
     */
    void throwConflict(const Dmn::DMesgPb dmesgPb) {
      m_inConflict = true;

      if (m_conflictCallbackFn) {
        m_sub.write(
            [this, dmesgPb]() { this->m_conflictCallbackFn(*this, dmesgPb); });
      }
    }

    std::string m_name{};
    bool m_includeDMesgSys{};
    FilterTask m_filterFn{};
    AsyncProcessTask m_asyncProcessFn{};

    Dmn_Buffer<Dmn::DMesgPb> m_buffers{};
    Dmn_DMesg *m_owner{};
    std::map<std::string, long long> m_topicRunningCounter{};
    Dmn_DMesgHandlerSub m_sub{};
    ConflictCallbackTask m_conflictCallbackFn{};
    bool m_inConflict{};
    Dmn::DMesgPb m_lastDMesgSysPb{};
  }; /* Dmn_DMesgHandler */

  Dmn_DMesg(std::string_view name) : Dmn_Pub{name, 10}, m_name{name} {}

  virtual ~Dmn_DMesg() noexcept try {
    for (auto &handler : m_handlers) {
      this->unregisterSubscriber(&(handler->m_sub));
      handler->m_owner = nullptr;
    }
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Dmn_DMesg(const Dmn_DMesg &dmnDMesg) = delete;
  const Dmn_DMesg &operator=(const Dmn_DMesg &dmnDMesg) = delete;
  Dmn_DMesg(Dmn_DMesg &&dmnDMesg) = delete;
  Dmn_DMesg &operator=(Dmn_DMesg &&dmnDMesg) = delete;

  /**
   * @brief The method creates a new handler, registers the handler to receive
   *        published message and returns he handler to the caller. It takes
   *        forward arguments as in Dmn_DMesgHandler::openHandler(...).
   *
   * @param name the name or unique identification to the handler
   * @param filterFn the functor callback that returns false to filter out Dmesg
   *                 message, if no functor is provided, no filter is performed.
   * @param asyncProcess the functor callback to process each notified Dmesg
   *                 message
   *
   * @return newly created handler
   */
  template <class... U>
  std::shared_ptr<Dmn_DMesgHandler> openHandler(U &&...arg) {
    std::shared_ptr<Dmn_DMesgHandler> handler =
        std::make_shared<Dmn_DMesgHandler>(std::forward<U>(arg)...);
    auto handlerRet = handler;

    m_handlers.push_back(handler);
    handler->m_owner = this;
    this->registerSubscriber(&(handler->m_sub));

    return handlerRet;
  }

  /**
   * @brief The method unregisters and removes the handler from the DMesg and
   *        then free the handler (if handlerToClose argument is the only
   *        shared pointer than one owned by DMesg).
   *
   * @param handlerToClose the handler to be closed
   */
  void closeHandler(std::shared_ptr<Dmn_DMesgHandler> &handlerToClose) {
    this->unregisterSubscriber(&(handlerToClose->m_sub));

    std::vector<std::shared_ptr<Dmn_DMesgHandler>>::iterator it = std::find_if(
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
  using Dmn_Pub::publish;

  /**
   * @brief The method publishes system message through async context queue.
   *
   * @param dmesgSysPb The system dmesg protobuf message
   */
  void publishSysInternal(Dmn::DMesgPb dmesgSysPb) {
    assert(dmesgSysPb.topic() == DMesgSysIdentifier);
    assert(dmesgSysPb.type() == Dmn::DMesgTypePb::sys);

    std::string id = dmesgSysPb.topic();
    long long nextRunningCounter = m_topicRunningCounter[id] + 1;

    try {
      DMESG_PB_SET_RUNNINGCOUNTER(dmesgSysPb, nextRunningCounter);
      Dmn_Pub::publishInternal(dmesgSysPb);
      m_topicRunningCounter[id] = nextRunningCounter;
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
  void publishInternal(Dmn::DMesgPb dmesgPb) override {
    std::string id = dmesgPb.topic();
    long long nextRunningCounter = m_topicRunningCounter[id] + 1;
    std::vector<std::shared_ptr<Dmn_DMesgHandler>>::iterator it = std::find_if(
        m_handlers.begin(), m_handlers.end(), [&dmesgPb](auto handler) {
          return handler->m_name == dmesgPb.sourceidentifier();
        });

    if (it != m_handlers.end() && (*it)->isInConflict()) {
      // avoid throw conflict multiple times
      return;
    } else if (dmesgPb.runningcounter() < nextRunningCounter) {
      if (it != m_handlers.end()) {
        (*it)->throwConflict(dmesgPb);

        return;
      }
    }

    try {
      DMESG_PB_SET_RUNNINGCOUNTER(dmesgPb, nextRunningCounter);
      Dmn_Pub::publishInternal(dmesgPb);
      m_topicRunningCounter[id] = nextRunningCounter;
    } catch (...) {
      throw;
    }
  }

  /**
   * @brief The method posts an asynchronous action in the async thread context
   *        to reset the handler's conflict state.
   *
   * @param handlerName the identification string for the open handler
   */
  void resetHandlerConflictState(std::string_view handlerName) {
    DMN_ASYNC_CALL_WITH_CAPTURE(
        { this->resetHandlerConflictStateInternal(handlerName); }, this,
        handlerName);
  }

private:
  /**
   * @brief The method resets the handler conflict state, it must be called
   * within the publisher asynchronous thread as it is the thread which puts the
   *        handler in conflict state at first place.
   *
   * @param handlerName the identification string for the open handler
   */
  void resetHandlerConflictStateInternal(std::string_view handlerName) {
    std::vector<std::shared_ptr<Dmn_DMesgHandler>>::iterator it = std::find_if(
        m_handlers.begin(), m_handlers.end(),
        [handlerName](auto handler) { return handler->m_name == handlerName; });

    if (it != m_handlers.end()) {
      (*it)->resolveConflictInternal();
    }
  }

  std::string m_name{};

  std::vector<std::shared_ptr<Dmn_DMesgHandler>> m_handlers{};
  std::map<std::string, long long> m_topicRunningCounter{};
};

} /* namespace Dmn */

#endif /* DMN_DMESG_HPP_HAVE_SEEN */
