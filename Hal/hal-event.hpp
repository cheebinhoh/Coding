/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements singleton instance to manage POSIX signal, timer
 * and programatic events in general.
 */

#ifndef HAL_EVENT_HPP_HAVE_SEEN

#define HAL_EVENT_HPP_HAVE_SEEN

#include "hal-async.hpp"
#include "hal-singleton.hpp"

#include <csignal>
#include <functional>
#include <map>
#include <mutex>

class Hal_Event_Manager : public Hal_Singleton, private Hal_Async {
  using SignalHandler = std::function<void(int signo)>;

public:
  Hal_Event_Manager();
  virtual ~Hal_Event_Manager() noexcept;

  Hal_Event_Manager(const Hal_Event_Manager &halEventMgr) = delete;
  const Hal_Event_Manager &
  operator=(const Hal_Event_Manager &halEventMgr) = delete;
  Hal_Event_Manager(Hal_Event_Manager &&halEventMgr) = delete;
  Hal_Event_Manager &operator=(Hal_Event_Manager &&halEventManager) = delete;

  void enterMainLoop();
  void exitMainLoop();
  void registerSignalHandler(int signo, SignalHandler handler);

  friend class Hal_Singleton;

private:
  void exitMainLoopInternal();
  void registerSignalHandlerInternal(int signo, SignalHandler handler);
  void execSignalHandlerInternal(int signo);

  template <class... U>
  static std::shared_ptr<Hal_Event_Manager> createInstanceInternal(U &&...u) {
    if (!Hal_Event_Manager::s_instance) {
      std::call_once(
          s_initOnce,
          [](U &&...arg) {
            // We need to mask off signals before any thread is created, so that
            // all created threads will inherit the same signal mask, and block
            // the signals.
            //
            // We can NOT sigmask the signals in Hal_Event_Manager constructor
            // as its parent class Hal_Async thread has been created by the time
            // the Hal_Event_Manager constructor is run.
            sigset_t oldmask{};
            int err{};

            sigemptyset(&Hal_Event_Manager::s_mask);
            sigaddset(&Hal_Event_Manager::s_mask, SIGINT);
            sigaddset(&Hal_Event_Manager::s_mask, SIGTERM);
            sigaddset(&Hal_Event_Manager::s_mask, SIGQUIT);
            sigaddset(&Hal_Event_Manager::s_mask, SIGHUP);

            err = pthread_sigmask(SIG_BLOCK, &Hal_Event_Manager::s_mask,
                                  &oldmask);
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
  std::map<int, SignalHandler> m_signalHandlers{};
  std::map<int, std::vector<SignalHandler>> m_extSignalHandlers{};

  // static variables for the global singleton instance
  static std::once_flag s_initOnce;
  static std::shared_ptr<Hal_Event_Manager> s_instance;
  static sigset_t s_mask;
};

#endif /* HAL_EVENT_HPP_HAVE_SEEN */
