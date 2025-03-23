/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Dmn_DMesg implements a specific publisher subscriber model (inherit
 * from dmn-pub-sub module) where the difference is that:
 * - the data item is a Protobuf message (Dmn::DMesgPb) defined in
 *   proto/dmn-dmesg.proto, so instead of subclass the Dmn_DMesg, clients of the
 *   Dmn_DMesg can extend the Dmn::DMesgPb protobuf message to varying the
 *   message without reinvent the wheel through subclass of the dmn-pub-sub
 *   module.
 * - instead of subclass Dmn_Pub::Dmn_Sub class to implement specific
 *   subscriber, the client of the API asks Dmn_DMesg to return a handler that
 *   subscribes to a specific set of topic (optional, or all topics), and can
 *   use to handler to publish and subscribe Dmn::DMesgPb message through the
 *   Dmn_Io like interface' read and write API methods, so instead of
 *   inherittence, clients can use object composition.
 * - it supports the concept that subscriber can subscribe to certain topic
 *   as defined in the Dmn::DMesgPb message.
 * - it allows various clients of the Dmn_Dmesg to publish data of certain
 *   topic at the same time, and implements a simple conflict detection and
 *   resolution for participated clients of the same Dmn_Dmesg object.
 */

#ifndef DMN_DMESG_HPP_HAVE_SEEN

#define DMN_DMESG_HPP_HAVE_SEEN

#include "dmn-debug.hpp"
#include "dmn-dmesg-pb-util.hpp"
#include "dmn-pub-sub.hpp"
#include "dmn-util.hpp"
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
       * @brief The method will be called by publisher object about new DMesgPb
       *        message and insert the the new message into the buffer for
       *        subscriber to be consumed, or call the Handler' asynchrounous
       *        process function.
       *
       * @param dmesgPb The DMesgPb message notified by publisher object
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

      // WARNING: it is marked as public so that a closure function
      // to Dmn_DMesg can access and manipulate it, as there is no
      // direct way to declare an inline closure as friend (we can
      // define a function that accept object of Dmn_DMesgHandlerSub
      // class and then pass the object as capture to a closure
      // created within the function and returns the closure to
      // DMesg, but all the trouble to access this.
      //
      // No external client will access the nested Dmn_DMesgHandlerSub
      // class anyway and it is just a composite object within the
      // Dmn_DMesgHandler class to integrate with dmn-pub-sub through
      // Dmn_Pub::Dmn_sub, so marking m_owner as public does not violate
      // data encapsulation.
      Dmn_DMesgHandler *m_owner{};
    }; /* End of class Dmn_DMesgHandlerSub */

  public:
    /**
     * @brief The wrapper constructor for Dmn_DMesgHandler.
     *
     * @param name         the name or unique identification to the handler
     * @param filterFn     the functor callback that returns false to filter out
     *                     DMesgPB message, if no functor is provided, no filter
     *                     is performed
     * @param asyncProcess the functor callback to process each notified DMesgPb
     *                     message
     */
    Dmn_DMesgHandler(std::string_view name, FilterTask filterFn = nullptr,
                     AsyncProcessTask asyncProcessFn = nullptr)
        : Dmn_DMesgHandler{name, false, filterFn, asyncProcessFn} {}

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler.
     *
     * @param name            the name or unique identification to the handler
     * @param includeDMesgSys True if the handler will be notified of DMesgPb
     *                        sys message, default is false
     * @param filterFn        the functor callback that returns false to filter
     *                        out DMesgPB message, if no functor is provided,
     *                        no filter is performed
     * @param asyncProcess    the functor callback to process each notified
     * DMesgPb message
     */
    Dmn_DMesgHandler(std::string_view name, bool includeDMesgSys,
                     FilterTask filterFn, AsyncProcessTask asyncProcessFn)
        : m_name{name}, m_includeDMesgSys{includeDMesgSys},
          m_filterFn{filterFn}, m_asyncProcessFn{asyncProcessFn} {
      // set the chained of owner for composite Dmn_DMesgHandlerSub object
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

    /**
     * @brief The method reads a DMesgPb message out of the handler
     *        opened with DMesg. This is a blocking call until a DMesgPb
     *        message is returned or exception is thrown, then nullopt
     *        is returned.
     *
     * @return DMesgPb The next DMesgPb message or nullopt if exception
     *         is thrown.
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
     *        asynchronous action on publisher singleton asynchronous thread
     *        context which is one responsible for putting the handler in
     *        conflict state, hence same responsible to reset it and get it
     *        out of the conflict state.
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
     * @brief The method writes and publishes the DMesgPb message through DMesg
     *        publisher queue to all subscribers. This method will move the
     *        DMesg data.
     *
     * @param dMesgPb The DMesgPb message to be published
     */
    void write(Dmn::DMesgPb &&dmesgPb) override {
      Dmn::DMesgPb movedDMesgPb = std::move_if_noexcept(dmesgPb);

      writeDMesgInternal(movedDMesgPb, true);
    }

    /**
     * @brief The method writes and publishes the DMesgPb message through DMesg
     *        publisher queue to all subscribers. This method will copy the
     *        DMesg data.
     *
     * @param dMesgPb The DMesgPb message to be published
     */
    void write(Dmn::DMesgPb &dmesgPb) override {
      writeDMesgInternal(dmesgPb, false);
    }

    void waitForEmpty() { m_sub.waitForEmpty(); }

    friend class Dmn_DMesg;
    friend class Dmn_DMesgHandlerSub;

  protected:
    /**
     * @brief The method writes and publishes the DMesgPb message through
     *        DMesg publisher queue to all subscribers. This method will
     *        move the DMesgPb message if move argument is true, otherwise
     *        copy the message.
     *
     * @param dmesgPb The DMesgPb messsgeto be published
     * @param move    True to move than copy the data
     */
    void writeDMesgInternal(Dmn::DMesgPb &dmesgPb, bool move) {
      if (m_inConflict) {
        throw std::runtime_error("last write results in conflicted");
      }

      struct timeval tv;
      gettimeofday(&tv, NULL);

      std::string topic = dmesgPb.topic();
      long long nextRunningCounter =
          incrementByOne(m_topicRunningCounter[topic]);

      DMESG_PB_SET_MSG_TIMESTAMP_FROM_TV(dmesgPb, tv);
      DMESG_PB_SET_MSG_SOURCEIDENTIFIER(dmesgPb, m_name);
      DMESG_PB_SET_MSG_RUNNINGCOUNTER(dmesgPb, nextRunningCounter);

      if (move) {
        m_owner->publish(std::move_if_noexcept(dmesgPb));
      } else {
        m_owner->publish(dmesgPb);
      }

      m_topicRunningCounter[topic] = nextRunningCounter;
    }

  private:
    /**
     * @brief The method returns true if the handler is in conflict state.
     *
     * @return True if the handler is in conflict state, false otherwise
     */
    bool isInConflict() const { return m_inConflict; }

    /**
     * @brief The method marks the handler as conflict resolved, and to be
     *        executed by the publisher's singleton asynchronous thread
     *        context (to avoid the need of additional mutex).
     */
    void resolveConflictInternal() { m_inConflict = false; }

    /**
     * @brief The method marks the writer as in conflict state and executes the
     *        conflict callback function in the handler singleton asynchronous
     *        thread context.
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

    /**
     * data member for constructor to instantiate the object.
     */
    std::string m_name{};
    bool m_includeDMesgSys{};
    FilterTask m_filterFn{};
    AsyncProcessTask m_asyncProcessFn{};

    /**
     * data members for internal logic
     */
    Dmn_DMesg *m_owner{};
    Dmn_DMesgHandlerSub m_sub{};

    std::vector<std::string> m_subscribedTopics{};

    Dmn_Buffer<Dmn::DMesgPb> m_buffers{};
    std::map<std::string, long long> m_topicRunningCounter{};
    Dmn::DMesgPb m_lastDMesgSysPb{};

    ConflictCallbackTask m_conflictCallbackFn{};
    bool m_inConflict{};
  }; /* End of class Dmn_DMesgHandler */

  /**
   * @brief The constructor for Dmn_DMesg.
   *
   * @param name The identification name for the instantiated object
   */
  Dmn_DMesg(std::string_view name)
      : Dmn_Pub{name, 10,
                [this](const Dmn_Sub *const sub, const Dmn::DMesgPb &msg) {
                  const Dmn_DMesgHandler::Dmn_DMesgHandlerSub
                      *const handlerSub = dynamic_cast<
                          const Dmn_DMesgHandler::Dmn_DMesgHandlerSub *const>(
                          sub);
                  assert(handlerSub != nullptr);

                  Dmn_DMesgHandler *handler = handlerSub->m_owner;

                  return handler->m_subscribedTopics.size() == 0 ||
                         std::find(handler->m_subscribedTopics.begin(),
                                   handler->m_subscribedTopics.end(),
                                   msg.topic()) !=
                             handler->m_subscribedTopics.end();
                }},
        m_name{name} {}

  virtual ~Dmn_DMesg() noexcept try {
    for (auto &handler : m_handlers) {
      this->unregisterSubscriber(&(handler->m_sub));
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
   *        published message and returns the handler to the caller. It takes
   *        forward arguments as in Dmn_DMesgHandler::openHandler(...).
   *
   * @param name         the name or unique identification to the handler
   * @param filterFn     the functor callback that returns false to filter out
   *                     DMesgPB message, if no functor is provided, no filter
   *                     is performed
   * @param asyncProcess the functor callback to process each notified DMesgPb
   *                     message
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
   * @brief The method creates a new handler, registers the handler to receive
   *        certain published message (by topic) and returns the handler to the
   *        caller.
   *
   * @param topics       the list of topics to be subscribed for the opened
   *                     handler
   * @param name         the name or unique identification to the handler
   * @param filterFn     the functor callback that returns false to filter out
   *                     DMesgPB message, if no functor is provided, no filter
   *                     is performed
   * @param asyncProcess the functor callback to process each notified DMesgPb
   *                     message
   *
   * @return newly created handler
   */
  template <class... U>
  std::shared_ptr<Dmn_DMesgHandler> openHandler(std::vector<std::string> topics,
                                                U &&...arg) {
    auto handlerRet = this->openHandler(std::forward<U>(arg)...);

    /* The topic filter is executed within the DMesg singleton asynchronous
     * thread context, but the filter value is maintained per Dmn_DMesgHandler,
     * and this allow the DMesg to be mutex free while thread safe.
     */
    handlerRet->m_subscribedTopics = topics;

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
   * @param dmesgSysPb The system DMesgPb message
   */
  void publishSysInternal(Dmn::DMesgPb dmesgSysPb) {
    assert(dmesgSysPb.topic() == DMesgSysIdentifier);
    assert(dmesgSysPb.type() == Dmn::DMesgTypePb::sys);

    std::string id = dmesgSysPb.topic();
    long long nextRunningCounter = incrementByOne(m_topicRunningCounter[id]);

    DMESG_PB_SET_MSG_RUNNINGCOUNTER(dmesgSysPb, nextRunningCounter);
    Dmn_Pub::publishInternal(dmesgSysPb);
    m_topicRunningCounter[id] = nextRunningCounter;
  }

  /**
   * @brief The method publishes dmesgPb to registered subscribers. If the to be
   *        published dmesgPb' topic has smaller runningcounter than what is in
   *        the m_topicRunningCounter, it means that the writer is out of sync
   *        and in race condition that its published Dmn::DMesgPb' topic has a
   *        runningcounter that is early in value than the same topic's running
   *        counter published by the DMesg. In this case, we put the writer
   *        handler to be in conflict state, and throws exception for future
   *        write (of the writer) until the client of the handler manually marks
   *        it as conflict resolved. We do not put all handlers in conflict, but
   *        the particular writer' handler.
   *
   * @param dmesgPb The dmesgPb to be published
   */
  void publishInternal(Dmn::DMesgPb dmesgPb) override {
    std::string id = dmesgPb.topic();

    long long nextRunningCounter = incrementByOne(m_topicRunningCounter[id]);

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

    DMESG_PB_SET_MSG_RUNNINGCOUNTER(dmesgPb, nextRunningCounter);
    Dmn_Pub::publishInternal(dmesgPb);
    m_topicRunningCounter[id] = nextRunningCounter;
  }

  /**
   * @brief The method posts an asynchronous action in the publisher's singleton
   *        asynchronous thread context to reset the handler's conflict state.
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
   *        within the publisher' singleton asynchronous thread context to be
   *        thread safe and that is the same thread that puts the handler in
   *        conflict state.
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

  /**
   * data members for constructor to instantiate the object.
   */
  std::string m_name{};

  /**
   * data members for internal logic.
   */
  std::vector<std::shared_ptr<Dmn_DMesgHandler>> m_handlers{};
  std::map<std::string, long long> m_topicRunningCounter{};
}; /* End of class Dmn_DMesg */

} /* End of namespace Dmn */

#endif /* DMN_DMESG_HPP_HAVE_SEEN */
