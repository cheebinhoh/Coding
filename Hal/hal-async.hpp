/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements an asynchronous execution extension for event-driven
 * model.
 *
 * A client class can inherit from Hal_Async or composes an Hal_Async object,
 * and implement the client class API to pass a functor to the Hal_Async object
 * for execution. This will help serialize the execution and avoid any explicit
 * mutex on the client class state and simplify the code.
 */

#ifndef HAL_ASYNC_HPP_HAVE_SEEN

#define HAL_ASYNC_HPP_HAVE_SEEN

#include "hal-pipe.hpp"

#include <functional>
#include <string_view>

#define HAL_ASYNC_CALL(block)                                                  \
  do {                                                                         \
    std::function<void()> functor{[&]() mutable { (block); }};                 \
    this->write(std::ref(functor));                                            \
  } while (false)

class Hal_Async : public Hal_Pipe<std::function<void()>> {
public:
  Hal_Async(std::string_view name)
      : Hal_Pipe{name, [](std::function<void()> &&task) {
                   task();
                   Hal_Proc::yield();
                 }} {}

  ~Hal_Async() noexcept try {
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Hal_Async(const Hal_Async &halAsync) = delete;
  const Hal_Async &operator=(const Hal_Async &halAsync) = delete;
  Hal_Async(Hal_Async &&halAsync) = delete;
  Hal_Async &operator=(Hal_Async &&halAsync) = delete;
};

#endif /* HAL_ASYNC_HPP_HAVE_SEEN */
