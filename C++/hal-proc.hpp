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

class Hal_Proc
{
  using Task = std::function<void()>;

  enum State
  {
    Invalid,
    New,     
    Ready,  
    Running
  };

 public:
  Hal_Proc(std::string_view name, Hal_Proc::Task fn = {});
  virtual ~Hal_Proc();

  Hal_Proc(const Hal_Proc &rCopy) = delete;
  const Hal_Proc &operator=(const Hal_Proc &rCopy) = delete;

  bool exec(Hal_Proc::Task fn = {});
  bool wait();
  Hal_Proc::State getState() const;
  static void yield();

 protected:
  bool stopExec();
  bool runExec();
  Hal_Proc::State setState(Hal_Proc::State state);
  void setTask(Hal_Proc::Task fn);

 private:
  static void *runFnInThreadHelper(void *context);

  std::string m_name{};

  Hal_Proc::Task m_fn{};

  pthread_t m_th{};

  Hal_Proc::State m_state{};
};

#endif /* HAL_PROC_HPP_HAVE_SEEN */

