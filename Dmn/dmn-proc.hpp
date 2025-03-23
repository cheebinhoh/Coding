/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This module wraps the native pthread behind an object-oriented class with
 * delegation protocol where variance of thread functionality is achieved
 * by passing a closure (functor) that the thread runs than using inherittance
 * to varying the different functionalities and which always results in
 * proliferation of subclass and hard to be maintained.
 */

#ifndef DMN_PROC_HPP_HAVE_SEEN

#define DMN_PROC_HPP_HAVE_SEEN

#include <functional>
#include <string>
#include <string_view>

#include <pthread.h>

namespace Dmn {

/**
 * Dmn_Proc thread cancellation via (StopExec) is synchronous, so if the functor
 * runs infinitely without any pthread cancellation point, we should voluntarily
 * call Dmn_Proc::yield() at different point in time in the loop.
 *
 * It is RAII model where in destruction of Dmn_Proc object, it will try to
 * cancel the thread and join it to free resource, so the thread should respond
 * to pthread cancellation.
 */
class Dmn_Proc {
  using Task = std::function<void()>;

  enum State { Invalid, New, Ready, Running };

public:
  Dmn_Proc(std::string_view name, Dmn_Proc::Task fn = {});
  virtual ~Dmn_Proc() noexcept;

  Dmn_Proc(const Dmn_Proc &dmnProc) = delete;
  const Dmn_Proc &operator=(const Dmn_Proc &dmnProc) = delete;
  Dmn_Proc(Dmn_Proc &&dmnProc) = delete;
  Dmn_Proc &operator=(Dmn_Proc &&dmnProcc) = delete;

  bool exec(Dmn_Proc::Task fn = {});
  bool wait();

  static void yield();

protected:
  Dmn_Proc::State getState() const;
  Dmn_Proc::State setState(Dmn_Proc::State state);
  void setTask(Dmn_Proc::Task fn);

  bool stopExec();
  bool runExec();

private:
  static void *runFnInThreadHelper(void *context);

  /**
   * data members for constructor to instantiate the object.
   */
  const std::string m_name{};

  /**
   * data members for internal logic.
   */
  Dmn_Proc::Task m_fn{};
  Dmn_Proc::State m_state{};
  pthread_t m_th{};
}; /* End of class Dmn_Proc */

} /* End of namespace Dmn */

#endif /* End of macro DMN_PROC_HPP_HAVE_SEEN */
