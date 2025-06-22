/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-timer.hpp
 * @brief The header for dmn-timer which implements watchdog or timer, executing
 *        a callback function repeatedly after a specified duration has elapsed.
 *        Note that while it is guaranteed that the callback function will not
 *        be executed before the specified duration has elapsed, it is not
 *        guaranteed to be executed immediately upon duration's completion.
 */

#ifndef DMN_TIMER_HPP_

#define DMN_TIMER_HPP_

#include <chrono>
#include <functional>
#include <thread>

#include "dmn-debug.hpp"
#include "dmn-proc.hpp"

namespace dmn {

template <typename T> class Dmn_Timer : public Dmn_Proc {
public:
  Dmn_Timer(const T &reltime, std::function<void()> fn);
  virtual ~Dmn_Timer() noexcept;

  Dmn_Timer(const Dmn_Timer &obj) = delete;
  const Dmn_Timer &operator=(const Dmn_Timer &obj) = delete;
  Dmn_Timer(Dmn_Timer &&obj) = delete;
  Dmn_Timer &operator=(Dmn_Timer &&obj) = delete;

  /**
   * @brief The method starts the timer that executes fn repeatedly after
   *        every reltime interval. Any existing timer is stopped before
   *        the new timer is started.
   *
   * @param reltime The std::chrono::duration timer
   * @param fn      The functor to be run by timer
   */
  void start(const T &reltime, std::function<void()> fn = {});

  /**
   * @brief The method stops the timer.
   */
  void stop();

private:
  /**
   * data members for constructor to instantiate the object.
   */
  std::function<void()> m_fn{};
  T m_reltime{};
}; // class Dmn_Timer

template <typename T>
Dmn_Timer<T>::Dmn_Timer(const T &reltime, std::function<void()> fn)
    : Dmn_Proc{"timer"}, m_fn{fn}, m_reltime{reltime} {
  this->start(this->m_reltime, this->m_fn);
}

template <typename T> Dmn_Timer<T>::~Dmn_Timer() noexcept try {
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

template <typename T>
void Dmn_Timer<T>::start(const T &reltime, std::function<void()> fn) {
  m_reltime = reltime;

  if (fn) {
    m_fn = fn;
  }

  this->stop();

  this->exec([this]() {
    while (true) {
      std::this_thread::sleep_for(this->m_reltime);
      Dmn_Proc::yield();

      try {
        if (m_fn) {
          this->m_fn();
        }
      } catch (const std::exception &e) {
        DMN_DEBUG_PRINT(std::cerr << e.what() << "\n");
      }
    }
  });
}

template <typename T> void Dmn_Timer<T>::stop() {
  try {
    this->stopExec();
  } catch (...) {
  }
}

} // namespace dmn

#endif // DMN_TIMER_HPP_
