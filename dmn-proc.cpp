/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-proc.cpp
 * @brief The source implementation file for dmn-proc.
 */

#include "dmn-proc.hpp"

#include <pthread.h>
#include <sched.h>

#include <cassert>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace dmn {

void cleanupFuncToUnlockPthreadMutex(void *arg) {
  pthread_mutex_t *mutex = (pthread_mutex_t *)arg;

  pthread_mutex_unlock(mutex);
}

Dmn_Proc::Dmn_Proc(std::string_view name, Dmn_Proc::Task fn) : m_name{name} {
  setState(State::kNew);

  if (fn) {
    setTask(fn);
  }
}

Dmn_Proc::~Dmn_Proc() noexcept try {
  if (getState() == State::kRunning) {
    stopExec();
  }

  setState(State::kInvalid);
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

bool Dmn_Proc::exec(Dmn_Proc::Task fn) {
  if (fn) {
    setTask(fn);
  }

  return runExec();
}

Dmn_Proc::State Dmn_Proc::getState() const { return m_state; }

Dmn_Proc::State Dmn_Proc::setState(State state) {
  State old_state = this->m_state;

  this->m_state = state;

  return old_state;
}

void Dmn_Proc::setTask(Dmn_Proc::Task fn) {
  assert(getState() == State::kNew || getState() == State::kReady);

  this->m_fn = fn;
  setState(State::kReady);
}

bool Dmn_Proc::wait() {
  int err{};
  void *ret{};

  if (getState() != State::kRunning) {
    throw std::runtime_error("No task is exec");
  }

  err = pthread_join(m_th, &ret);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  setState(State::kReady);

  return 0 == err;
}

void Dmn_Proc::yield() {
  pthread_testcancel();
  sched_yield();
}

bool Dmn_Proc::stopExec() {
  int err{};

  if (getState() != State::kRunning) {
    throw std::runtime_error("No task is exec");
  }

  err = pthread_cancel(m_th);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  return wait();
}

bool Dmn_Proc::runExec() {
  int err{};
  State old_state{};

  if (getState() != State::kReady) {
    throw std::runtime_error("No task is assigned to the Dmn_Proc (" + m_name +
                             ")");
  }

  old_state = setState(State::kRunning);
  err = pthread_create(&m_th, NULL, &(Dmn_Proc::runFnInThreadHelper), this);
  if (err) {
    setState(old_state);
    return false;
  }

  return true;
}

void *Dmn_Proc::runFnInThreadHelper(void *context) {
  Dmn_Proc *proc{};
  int old_state{};
  int err{};

  err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  err = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old_state);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  proc = (Dmn_Proc *)context;
  proc->m_fn();

  return NULL;
}

} // namespace dmn
