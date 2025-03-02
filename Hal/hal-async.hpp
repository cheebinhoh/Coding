/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements an asynchronous execution extension model.
 *
 * A client class can inherit from Hal_Async or composes an Hal_Async object,
 * and implement the client class API to pass a functor to the Hal_Async object
 * for execution on behalf of the client API calls. This will help serialize
 * the API call execution, avoid any explicit mutex lock on client API calls,
 * and more important is that it can shorten the latency of calling the client
 * API and returns from the API call for functionalities that does not need to
 * be synchronized between caller and callee (see hal-pub-sub.hpp for an example
 * usage of this module).
 */

#ifndef HAL_ASYNC_HPP_HAVE_SEEN

#define HAL_ASYNC_HPP_HAVE_SEEN

#include "hal-pipe.hpp"

#include <chrono>
#include <functional>
#include <string_view>

#define HAL_ASYNC_CALL_WITH_COPY_CAPTURE(block)                                \
  do {                                                                         \
    this->write([=]() mutable { block; });                                     \
  } while (false)

#define HAL_ASYNC_CALL_WITH_REF_CAPTURE(block)                                 \
  do {                                                                         \
    this->write([&]() mutable { block; });                                     \
  } while (false)

#define HAL_ASYNC_CALL_WITH_CAPTURE(block, ...)                                \
  do {                                                                         \
    this->write([__VA_ARGS__]() mutable { block; });                           \
  } while (false)

namespace Hal {

class Hal_Async : public Hal_Pipe<std::function<void()>> {
public:
  Hal_Async(std::string_view name = "")
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

  /**
   * @brief The method will execute the asynchronous task after duration
   *        is elapsed, the task will NOT be executed before duration is
   *        elapsed, but might not be guaranteed that it is executed in
   *        moment that the duration is elapsed.
   *
   * @param duration time in duraton that must be elapsed before task
   *                 is executed
   * @param fn       asynchronous task to be executed
   */
  template <class Rep, class Period>
  void execAfter(const std::chrono::duration<Rep, Period> &duration,
                 std::function<void()> fn) {
    long long timeInFuture =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count() +
        std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

    this->execAfterInternal(timeInFuture, fn);
  }

private:
  /**
   * @brief The method will execute the asynchronous task after duration
   *        is elapsed, the task will NOT be executed before duration is
   *        elapsed, but might not be guaranteed that it is executed in
   *        moment that the duration is elapsed.
   *
   * @param timeInFuture nanoseconds that must be elapsed before the
   *                     task is executed
   * @param fn           asynchronous task to be executed
   */
  void execAfterInternal(long long timeInFuture, std::function<void()> fn) {
    this->write([this, timeInFuture, fn]() {
      long long now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();

      if (now >= timeInFuture) {
        if (fn) {
          fn();
        }
      } else {
        this->execAfterInternal(timeInFuture, fn);
      }
    });
  }
}; // class Hal_Async

} // namespace Hal

#endif /* HAL_ASYNC_HPP_HAVE_SEEN */
