#ifndef HAL_PROC_HPP_HAVE_SEEN

#define HAL_PROC_HPP_HAVE_SEEN

#include <pthread.h>

#include <functional>
#include <string>
#include <string_view>

using Hal_ProcTask = std::function<void()>;

enum class Hal_ProcState
{
  Invalid,
  New,     
  Ready,  
  Running
};

class Hal_Proc
{
 public:
  Hal_Proc(std::string_view name, Hal_ProcTask fn = {});
  virtual ~Hal_Proc();

  Hal_Proc(const Hal_Proc &rCopy) = delete;
  const Hal_Proc &operator=(const Hal_Proc &rCopy) = delete;

  bool exec(Hal_ProcTask fn = {});
  bool wait();
  Hal_ProcState getState() const;
  static void yield();

 protected:
  bool stopExec();
  bool runExec();
  Hal_ProcState setState(Hal_ProcState state);
  void setTask(Hal_ProcTask fn);

 private:
  static void *runFnInThreadHelper(void *context);

  std::string m_name{};

  Hal_ProcTask m_fn{};

  pthread_t m_th{};

  Hal_ProcState m_state{};
};

#endif /* HAL_PROC_HPP_HAVE_SEEN */

