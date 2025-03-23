/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements singleton instance to manage POSIX signal, timer
 * and programatic events in general.
 */

#ifndef DMN_EVENT_HPP_HAVE_SEEN

#define DMN_EVENT_HPP_HAVE_SEEN

#include "dmn-async.hpp"
#include "dmn-singleton.hpp"

#include <csignal>
#include <functional>
#include <map>
#include <mutex>

namespace Dmn {

class Dmn_Event_Manager : public Dmn_Singleton, private Dmn_Async {
  using SignalHandler = std::function<void(int signo)>;

public:
  Dmn_Event_Manager();
  virtual ~Dmn_Event_Manager() noexcept;

  Dmn_Event_Manager(const Dmn_Event_Manager &dmnEventMgr) = delete;
  const Dmn_Event_Manager &
  operator=(const Dmn_Event_Manager &dmnEventMgr) = delete;
  Dmn_Event_Manager(Dmn_Event_Manager &&dmnEventMgr) = delete;
  Dmn_Event_Manager &operator=(Dmn_Event_Manager &&dmnEventManager) = delete;

  void enterMainLoop();
  void exitMainLoop();
  void registerSignalHandler(int signo, SignalHandler handler);

  friend class Dmn_Singleton;

private:
  void exitMainLoopInternal();
  void registerSignalHandlerInternal(int signo, SignalHandler handler);
  void execSignalHandlerInternal(int signo);

  template <class... U>
  static std::shared_ptr<Dmn_Event_Manager> createInstanceInternal(U &&...u) {
    if (!Dmn_Event_Manager::s_instance) {
      std::call_once(
          s_initOnce,
          [](U &&...arg) {
            // We need to mask off signals before any thread is created, so that
            // all created threads will inherit the same signal mask, and block
            // the signals.
            //
            // We can NOT sigmask the signals in Dmn_Event_Manager constructor
            // as its parent class Dmn_Async thread has been created by the time
            // the Dmn_Event_Manager constructor is run.
            sigset_t oldmask{};
            int err{};

            sigemptyset(&Dmn_Event_Manager::s_mask);
            sigaddset(&Dmn_Event_Manager::s_mask, SIGINT);
            sigaddset(&Dmn_Event_Manager::s_mask, SIGTERM);
            sigaddset(&Dmn_Event_Manager::s_mask, SIGQUIT);
            sigaddset(&Dmn_Event_Manager::s_mask, SIGHUP);

            err = pthread_sigmask(SIG_BLOCK, &Dmn_Event_Manager::s_mask,
                                  &oldmask);
            if (0 != err) {
              throw std::runtime_error("Error in pthread_sigmask: " +
                                       std::string(strerror(errno)));
            }

            Dmn_Event_Manager::s_instance =
                std::make_shared<Dmn_Event_Manager>(std::forward<U>(arg)...);
          },
          std::forward<U>(u)...);
    }

    return Dmn_Event_Manager::s_instance;
  } /* End of static method createInstanceInternal() */

  /**
   * data members for internal logic.
   */
  Dmn_Proc m_signalWaitProc{"DmnEventManager_SignalWait"};
  sigset_t m_mask{};
  std::map<int, SignalHandler> m_signalHandlers{};
  std::map<int, std::vector<SignalHandler>> m_extSignalHandlers{};

  /**
   * static variables for the global singleton instance
   */
  static std::once_flag s_initOnce;
  static std::shared_ptr<Dmn_Event_Manager> s_instance;
  static sigset_t s_mask;
}; /* End of class Dmn_Event_Manager */

} /* End of namespace Dmn */

#endif /* End of macro DMN_EVENT_HPP_HAVE_SEEN */
