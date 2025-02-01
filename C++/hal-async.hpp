#ifndef HAL_ASYNC_HPP_HAVE_SEEN

#define HAL_ASYNC_HPP_HAVE_SEEN

#include "hal-pipe.hpp"

#include <functional>
#include <string_view>

using Hal_AsyncTask = std::function<void()>;
class Hal_Async : public Hal_Pipe<Hal_AsyncTask>
{
 public:
   Hal_Async(std::string_view name) : Hal_Pipe{name, [](Hal_AsyncTask task){
                                                       task();
                                                     }
                                              }
   {
   }
 
 private:
};


#endif /* HAL_ASYNC_HPP_HAVE_SEEN */
