#ifndef HAL_PROC_H_HPP_HAVE_SEEN

#define HAL_PROC_H_HPP_HAVE_SEEN

#include <functional>
#include <string>
#include <thread>

class Hal_Proc
{
  public:
    Hal_Proc(std::string name, std::function<void()> fn = {});
    bool exec(std::function<void()> fn = {});

  protected:
    void run_exec(std::function<void()> fn);

  private:
    std::function<void()> m_fn {};
    std::string           m_name {};
    std::thread           m_th {};
};

#endif /* HAL_PROC_H_HPP_HAVE_SEEN */
