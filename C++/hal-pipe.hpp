#ifndef HAL_PIPE_HPP_HAVE_SEEN

#define HAL_PIPE_HPP_HAVE_SEEN

#include <functional>
#include "hal-buffer.hpp"
#include "hal-proc.hpp"

template <typename T>
class Hal_Pipe : public Hal_Buffer<T>, public Hal_Proc
{
  public:
    Hal_Pipe(std::string name, std::function<void(T)> fn = {}) : Hal_Proc { name } {

      if (fn) {
        exec([this, &fn]() { 
          while (true) {
            std::string item = this->pop();

            fn(std::move(item));
          }
        });
      } 
    }

  private:
};

#endif /* HAL_PIPE_HPP_HAVE_SEEN */
