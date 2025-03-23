/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-event.hpp"
#include "dmn-async.hpp"
#include "dmn-proc.hpp"

#include <csignal>
#include <iostream>
#include <memory>

namespace Dmn {

std::once_flag Dmn_Event_Manager::s_initOnce{};
std::shared_ptr<Dmn_Event_Manager> Dmn_Event_Manager::s_instance{};
sigset_t Dmn_Event_Manager::s_mask{};

Dmn_Event_Manager::Dmn_Event_Manager()
    : Dmn_Singleton{}, Dmn_Async{"Dmn_Event_Manager"},
      m_mask{Dmn_Event_Manager::s_mask} {
  // default and to be overridden if needed
  m_signalHandlers[SIGTERM] = [this](int signo) {
    this->exitMainLoopInternal();
  };

  m_signalHandlers[SIGINT] = [this](int signo) {
    this->exitMainLoopInternal();
  };

  m_signalWaitProc.exec([this]() {
    while (true) {
      int signo{};
      int err{};

      err = sigwait(&m_mask, &signo);
      if (err) {
        throw std::runtime_error("Error in sigwait: " +
                                 std::string(strerror(errno)));
      }

      DMN_ASYNC_CALL_WITH_CAPTURE({ this->execSignalHandlerInternal(signo); },
                                  this, signo);
    }
  });
}

Dmn_Event_Manager::~Dmn_Event_Manager() noexcept try {
  this->stopExec();
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

/**
 * @brief The method will exit the Dmn_Event_Manager mainloop, returns control
 *        (usually the mainthread) to the main() function to be continued.
 */
void Dmn_Event_Manager::exitMainLoop() {
  DMN_ASYNC_CALL_WITH_REF_CAPTURE({ this->exitMainLoopInternal(); });
}

/**
 * @brief The method will exit the Dmn_Event_Manager mainloop, returns control
 *        (usually the mainthread) to the main() function to be continued.
 *        This is private method to be called in the Dmn_Event_Manager instance
 *        asynchronous thread context.
 */
void Dmn_Event_Manager::exitMainLoopInternal() { this->stopExec(); }

/**
 * @brief The method will enter the Dmn_Event_Manager mainloop, and wait
 *        for event loop to be exited. this is usually called by the main()
 *        method.
 */
void Dmn_Event_Manager::enterMainLoop() {
  Dmn_Proc::yield();
  this->wait();
}

/**
 * @brief The method executes the signal handlers in asynchronous thread context
 *
 * @param signo signal number that is raised
 */
void Dmn_Event_Manager::execSignalHandlerInternal(int signo) {
  auto extHandlers = m_extSignalHandlers.find(signo);
  if (m_extSignalHandlers.end() != extHandlers) {
    for (auto &handler : extHandlers->second) {
      handler(signo);
    }
  }

  auto handler = m_signalHandlers.find(signo);
  if (m_signalHandlers.end() != handler) {
    handler->second(signo);
  }
}

/**
 * @brief The method registers signal handler for the signal number. Note that
 *        SIGKILL and SIGSTOP can NOT be handled.
 *
 * @param signo   The POSIX signal number
 * @param handler The signal handler to be called when the signal is raised.
 */
void Dmn_Event_Manager::registerSignalHandler(int signo,
                                              SignalHandler handler) {
  DMN_ASYNC_CALL_WITH_CAPTURE(
      { this->registerSignalHandlerInternal(signo, handler); }, this, signo,
      handler);
}

/**
 * @brief The method registers external signal handler for the signal number.
 *        The external signal handlers are executed before default handler from
 *        Dmn_Event_Manager. Note that SIGKILL and SIGSTOP can NOT be handled.
 *        This is private method to be called in the Dmn_Event_Manager instance
 *        asynchronous thread context.
 *
 * @param signo   The POSIX signal number
 * @param handler The signal handler to be called when the signal is raised.
 */
void Dmn_Event_Manager::registerSignalHandlerInternal(int signo,
                                                      SignalHandler handler) {
  auto &extHandlers = m_extSignalHandlers[signo];
  extHandlers.push_back(handler);
}

} // namespace Dmn
