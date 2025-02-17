/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef HAL_EVENT_HPP_HAVE_SEEN
#define HAL_EVENT_HPP_HAVE_SEEN

#include "hal-async.hpp"
#include "hal-singleton.hpp"

#include <csignal>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>

class Hal_Event_Manager : public Hal_Singleton, Hal_Async {
public:
  Hal_Event_Manager();
  virtual ~Hal_Event_Manager();

  void exitMainLoop();

  void enterMainLoop();

  friend class Hal_Singleton;

private:
  void exitMainLoopPrivate();

  template <class... U>
  static std::shared_ptr<Hal_Event_Manager> createInstanceInternal(U &&...u) {
    if (!Hal_Event_Manager::s_instance) {
      std::call_once(
          s_InitOnce,
          [](U &&...arg) {
            // FIXME: duplicated code
            sigset_t oldmask{};
            sigset_t mask{};
            int err{};

            sigemptyset(&mask);
            sigaddset(&mask, SIGINT);
            sigaddset(&mask, SIGTERM);
            sigaddset(&mask, SIGQUIT);
            sigaddset(&mask, SIGHUP);

            err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
            if (0 != err) {
              throw std::runtime_error("Error in pthread_sigmask: " +
                                       std::string(strerror(errno)));
            }

            Hal_Event_Manager::s_instance =
                std::make_shared<Hal_Event_Manager>(std::forward<U>(arg)...);
          },
          std::forward<U>(u)...);
    }

    return Hal_Event_Manager::s_instance;
  }

  Hal_Proc m_signalWaitProc{"HalEventManager_SignalWait"};
  sigset_t m_mask{};
  std::map<int, std::function<void()>> m_signalHandlers{};

  static std::shared_ptr<Hal_Event_Manager> s_instance;
  static std::once_flag s_InitOnce;
};

#endif /* HAL_EVENT_HPP_HAVE_SEEN */
