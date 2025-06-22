/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-async.cpp
 * @brief The source implementation file for dmn-async.
 */

#include "dmn-async.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string_view>

#include "dmn-pipe.hpp"

namespace dmn {

void Dmn_Async::Dmn_Async_Wait::wait() {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_cond_var.wait(lock, [this] { return m_done; });
}

Dmn_Async::Dmn_Async(std::string_view name)
    : Dmn_Pipe{name, [](std::function<void()> &&task) {
                 task();
                 Dmn_Proc::yield();
               }} {}

Dmn_Async::~Dmn_Async() noexcept try { this->waitForEmpty(); } catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

std::shared_ptr<Dmn_Async::Dmn_Async_Wait>
Dmn_Async::addExecTaskWithWait(std::function<void()> fn) {
  auto wait_shared_ptr = std::make_shared<Dmn_Async_Wait>();

  this->write([wait_shared_ptr, fn]() {
    fn();

    std::unique_lock<std::mutex> lock(wait_shared_ptr->m_mutex);
    wait_shared_ptr->m_done = true;
    wait_shared_ptr->m_cond_var.notify_all();
  });

  return wait_shared_ptr;
}

void Dmn_Async::addExecTaskAfterInternal(long long time_in_future,
                                         std::function<void()> fn) {
  this->write([this, time_in_future, fn]() {
    long long now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();

    if (now >= time_in_future) {
      if (fn) {
        fn();
      }
    } else {
      this->addExecTaskAfterInternal(time_in_future, fn);
    }
  });
}

} // namespace dmn
