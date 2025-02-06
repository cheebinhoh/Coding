/**
 * This module wraps the native thread behind an object-oriented class with
 * delegation protocol where variance of thread functionality is achieved
 * by passing a closure (functor) that the thread runs than using inherittance
 * which always results in proliferation of subclass and hard to be maintained. 
 */

#ifndef HAL_PROC_HPP_HAVE_SEEN

#define HAL_PROC_HPP_HAVE_SEEN

#include <pthread.h>

#include <functional>
#include <string>
#include <string_view>

using Hal_ProcTask = std::function<void()>;

class Hal_Proc
{
  enum State
  {
    Invalid,
    New,     
    Ready,  
    Running
  };

 public:
  Hal_Proc(std::string_view name, Hal_ProcTask fn = {});
  virtual ~Hal_Proc();

  Hal_Proc(const Hal_Proc &rCopy) = delete;
  const Hal_Proc &operator=(const Hal_Proc &rCopy) = delete;

  bool exec(Hal_ProcTask fn = {});
  bool wait();
  State getState() const;
  static void yield();

 protected:
  bool stopExec();
  bool runExec();
  State setState(State state);
  void setTask(Hal_ProcTask fn);

 private:
  static void *runFnInThreadHelper(void *context);

  std::string m_name{};

  Hal_ProcTask m_fn{};

  pthread_t m_th{};

  State m_state{};
};

#endif /* HAL_PROC_HPP_HAVE_SEEN */

