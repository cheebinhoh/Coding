/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This module a watchdog or timer that executes a callback function
 * repeatedly after certain duration.
 */

#ifndef HAL_TIMER_HPP_HAVE_SEEN

#define HAL_TIMER_HPP_HAVE_SEEN

#include "hal-proc.hpp"

#include <chrono>
#include <functional>
#include <thread>

template <typename T> class Hal_Timer : public Hal_Proc {
public:
  Hal_Timer(const T &reltime, std::function<void()> fn)
      : Hal_Proc{"timer"}, m_fn{fn}, m_reltime{reltime} {
    this->start(this->m_reltime, this->m_fn);
  }

  ~Hal_Timer() noexcept try {
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  /**
   * @brief The method starts the timer that executes fn repeatedly after
   *        every reltime interval. Any existing timer is stopped before
   *        the new timer is started.
   *
   * @param reltime The std::chrono::duration timer
   * @param fn The functor to be run by timer
   */
  void start(const T &reltime, std::function<void()> fn = {}) {
    m_reltime = reltime;

    if (fn) {
      m_fn = fn;
    }

    this->stop();

    this->exec([this]() {
      while (true) {
        std::this_thread::sleep_for(this->m_reltime);
        Hal_Proc::yield();

        this->m_fn();
      }
    });
  }

  /**
   * @brief The method stops the timer.
   */
  void stop() {
    try {
      this->stopExec();
    } catch (...) {
    }
  }

private:
  std::function<void()> m_fn{};
  T m_reltime{};
};

#endif /* HAL_TIMER_HPP_HAVE_SEEN */
