/**
 * This module wraps the native thread behind an object-oriented class with
 * delegation protocol where variance of thread functionality is achieved
 * by passing a closure (functor) that the thread runs than using inherittance
 * to varying the different functionalities and which always results in
 * proliferation of subclass and hard to be maintained. 
 */

#ifndef HAL_PROC_HPP_HAVE_SEEN

#define HAL_PROC_HPP_HAVE_SEEN

#include <functional>
#include <string>
#include <string_view>

#include <pthread.h>

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

  Hal_Proc(const Hal_Proc &halProc) = delete;
  const Hal_Proc &operator=(const Hal_Proc &halProc) = delete;
  Hal_Proc(Hal_Proc &&halProc) = delete;
  Hal_Proc &operator=(Hal_Proc &&halProcc) = delete;

  bool exec(Hal_Proc::Task fn = {});
  bool wait();

  static void yield();

 protected:
  Hal_Proc::State getState() const;
  Hal_Proc::State setState(Hal_Proc::State state);
  void setTask(Hal_Proc::Task fn);

  bool stopExec();
  bool runExec();

 private:
  static void *runFnInThreadHelper(void *context);

  const std::string m_name{};
  Hal_Proc::Task    m_fn{};
  Hal_Proc::State   m_state{};
  pthread_t         m_th{};
};

#endif /* HAL_PROC_HPP_HAVE_SEEN */

