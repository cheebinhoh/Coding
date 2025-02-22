/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-event.hpp"
#include "hal-async.hpp"
#include "hal-proc.hpp"

#include <csignal>
#include <iostream>
#include <memory>

namespace Hal {

std::once_flag Hal_Event_Manager::s_initOnce{};
std::shared_ptr<Hal_Event_Manager> Hal_Event_Manager::s_instance{};
sigset_t Hal_Event_Manager::s_mask{};

Hal_Event_Manager::Hal_Event_Manager()
    : Hal_Singleton{}, Hal_Async{"Hal_Event_Manager"},
      m_mask{Hal_Event_Manager::s_mask} {
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

      HAL_ASYNC_CALL_WITH_CAPTURE({ this->execSignalHandlerInternal(signo); },
                                  this, signo);
    }
  });
}

Hal_Event_Manager::~Hal_Event_Manager() noexcept try {
  this->stopExec();
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

/**
 * @brief The method will exit the Hal_Event_Manager mainloop, returns control
 *        (usually the mainthread) to the main() function to be continued.
 */
void Hal_Event_Manager::exitMainLoop() {
  HAL_ASYNC_CALL_WITH_REF_CAPTURE({ this->exitMainLoopInternal(); });
}

/**
 * @brief The method will exit the Hal_Event_Manager mainloop, returns control
 *        (usually the mainthread) to the main() function to be continued.
 *        This is private method to be called in the Hal_Event_Manager instance
 *        asynchronous thread context.
 */
void Hal_Event_Manager::exitMainLoopInternal() { this->stopExec(); }

/**
 * @brief The method will enter the Hal_Event_Manager mainloop, and wait
 *        for event loop to be exited. this is usually called by the main()
 *        method.
 */
void Hal_Event_Manager::enterMainLoop() {
  Hal_Proc::yield();
  this->wait();
}

/**
 * @brief The method executes the signal handlers in asynchronous thread context
 *
 * @param signo signal number that is raised
 */
void Hal_Event_Manager::execSignalHandlerInternal(int signo) {
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
void Hal_Event_Manager::registerSignalHandler(int signo,
                                              SignalHandler handler) {
  HAL_ASYNC_CALL_WITH_CAPTURE(
      { this->registerSignalHandlerInternal(signo, handler); }, this, signo,
      handler);
}

/**
 * @brief The method registers external signal handler for the signal number.
 * The external signal handlers are executed before default handler from
 *        Hal_Event_Manager. Note that SIGKILL and SIGSTOP can NOT be handled.
 *        This is private method to be called in the Hal_Event_Manager instance
 *        asynchronous thread context.
 *
 * @param signo   The POSIX signal number
 * @param handler The signal handler to be called when the signal is raised.
 */
void Hal_Event_Manager::registerSignalHandlerInternal(int signo,
                                                      SignalHandler handler) {
  auto &extHandlers = m_extSignalHandlers[signo];
  extHandlers.push_back(handler);
}

} // namespace Hal
