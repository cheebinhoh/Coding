/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-event.hpp"

#include "hal-async.hpp"

#include "hal-proc.hpp"

#include <csignal>
#include <memory>

std::once_flag Hal_Event_Manager::s_InitOnce{};
std::shared_ptr<Hal_Event_Manager> Hal_Event_Manager::s_instance{};

Hal_Event_Manager::Hal_Event_Manager()
    : Hal_Singleton{}, Hal_Async{"Hal_Event_Manager"} {
  sigset_t oldmask{};
  int err{};

  sigemptyset(&m_mask);
  sigaddset(&m_mask, SIGINT);
  sigaddset(&m_mask, SIGTERM);
  sigaddset(&m_mask, SIGQUIT);
  sigaddset(&m_mask, SIGHUP);

  m_signalHandlers[SIGTERM] = [this]() {
    std::cout << "exit main loop\n";
    this->exitMainLoop();
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

      auto handler = m_signalHandlers.find(signo);
      if (m_signalHandlers.end() == handler) {
        std::cerr << "Signal number: " << signo << " has no handler\n";
      } else {
        handler->second();
      }
    }
  });
}

Hal_Event_Manager::~Hal_Event_Manager() {}

void Hal_Event_Manager::exitMainLoop() {
  HAL_ASYNC_CALL_WITH_REF_CAPTURE({ this->exitMainLoopPrivate(); });
}

void Hal_Event_Manager::enterMainLoop() {
  Hal_Proc::yield();
  this->wait();
}

void Hal_Event_Manager::exitMainLoopPrivate() { this->stopExec(); }
