/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-async.hpp
 * @brief The header file for dmn-async which implements base class for other
 *        class to adapt and implementing asynchronous execution API.
 *
 *        A client class can inherit from Dmn_Async or composes an Dmn_Async
 *        object, and implement the class API to pass a functor to the Dmn_Async
 *        object for execution on behalf of the client API call' execution. This
 *        will help serialize multiple the API call executions, avoid any
 *        explicit mutex lock on client API calls, and more important is that it
 *        can shorten the latency of calling the client API and returns from the
 *        API call for functionalities that does not need to be synchronized
 *        between caller and callee' API execution (see dmn-pub-sub.hpp for an
 *        example usage of this class).
 */

#ifndef DMN_ASYNC_HPP_

#define DMN_ASYNC_HPP_

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string_view>

#include "dmn-pipe.hpp"

#define DMN_ASYNC_CALL_WITH_COPY_CAPTURE(block)                                \
  do {                                                                         \
    this->write([=]() mutable { block; });                                     \
  } while (false)

#define DMN_ASYNC_CALL_WITH_REF_CAPTURE(block)                                 \
  do {                                                                         \
    this->write([&]() mutable { block; });                                     \
  } while (false)

#define DMN_ASYNC_CALL_WITH_CAPTURE(block, ...)                                \
  do {                                                                         \
    this->write([__VA_ARGS__]() mutable { block; });                           \
  } while (false)

namespace dmn {

class Dmn_Async : public Dmn_Pipe<std::function<void()>> {
public:
  class Dmn_Async_Wait {
    friend class Dmn_Async;

  public:
    void wait();

  private:
    std::mutex m_mutex{};
    std::condition_variable m_cond_var{};

    bool m_done{};
  };

  /**
   * @brief The contructor for Dmn_Async object
   *
   * @param name The object identification string name
   */
  Dmn_Async(std::string_view name = "");
  virtual ~Dmn_Async() noexcept;

  Dmn_Async(const Dmn_Async &obj) = delete;
  const Dmn_Async &operator=(const Dmn_Async &obj) = delete;
  Dmn_Async(Dmn_Async &&obj) = delete;
  Dmn_Async &operator=(Dmn_Async &&obj) = delete;

  /**
   * @brief The method add an asynchronous task to be executed and then returns
   *        an Dmn_Async_Wait object for rendezvous point where the wait method
   *        guarantees that the asynchronous task has been executed.
   *
   * @param fn asynchronous task to be executed
   *
   * @return Dmn_Async_Wait object fo rendezvous point
   */
  std::shared_ptr<Dmn_Async_Wait> addExecTaskWithWait(std::function<void()> fn);

  /**
   * @brief The method will execute the asynchronous task after duration
   *        is elapsed, the task will NOT be executed before duration is
   *        elapsed, but might not be guaranteed that it is executed in
   *        exact moment that the duration is elapsed.
   *
   * @param duration time in duraton that must be elapsed before task
   *                 is executed
   * @param fn       asynchronous task to be executed
   */
  template <class Rep, class Period>
  void addExecTaskAfter(const std::chrono::duration<Rep, Period> &duration,
                        std::function<void()> fn);

  /**
   * @brief The method will execute the asynchronous task after duration
   *        is elapsed, the task will NOT be executed before duration is
   *        elapsed, but might not be guaranteed that it is executed in
   *        exact moment that the duration is elapsed.
   *
   *        The method will return an Dmn_Async_Wait object for rendezvous point
   *        where the wait method guarantees that the asynchronous task has been
   *        executed.
   *
   * @param duration time in duraton that must be elapsed before task
   *                 is executed
   * @param fn       asynchronous task to be executed
   *
   * @return Dmn_Async_Wait object fo rendezvous point
   */
  template <class Rep, class Period>
  std::shared_ptr<Dmn_Async_Wait>
  addExecTaskAfterWithWait(const std::chrono::duration<Rep, Period> &duration,
                           std::function<void()> fn);

private:
  /**
   * @brief The method will execute the asynchronous task after duration
   *        is elapsed, the task will NOT be executed before duration is
   *        elapsed, but might not be guaranteed that it is executed in
   *        exact moment that the duration is elapsed.
   *
   * @param time_in_future nanoseconds that must be elapsed before the
   *                       task is executed
   * @param fn             asynchronous task to be executed
   */
  void addExecTaskAfterInternal(long long time_in_future,
                                std::function<void()> fn);
}; // class Dmn_Async

template <class Rep, class Period>
void Dmn_Async::addExecTaskAfter(
    const std::chrono::duration<Rep, Period> &duration,
    std::function<void()> fn) {
  long long time_in_future =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count() +
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

  this->addExecTaskAfterInternal(time_in_future, fn);
}

template <class Rep, class Period>
std::shared_ptr<Dmn_Async::Dmn_Async_Wait> Dmn_Async::addExecTaskAfterWithWait(
    const std::chrono::duration<Rep, Period> &duration,
    std::function<void()> fn) {
  auto wait_shared_ptr = std::make_shared<Dmn_Async::Dmn_Async_Wait>();

  this->addExecTaskAfter(duration, [wait_shared_ptr, fn]() {
    fn();

    std::unique_lock<std::mutex> lock(wait_shared_ptr->m_mutex);
    wait_shared_ptr->m_done = true;
    wait_shared_ptr->m_cond_var.notify_all();
  });

  return wait_shared_ptr;
}

} // namespace dmn

#endif // DMN_ASYNC_HPP_
