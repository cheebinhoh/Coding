/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-proc.hpp"

#include <cassert>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <pthread.h>
#include <sched.h>

namespace Hal {

Hal_Proc::Hal_Proc(std::string_view name, Hal_Proc::Task fn) : m_name{name} {
  setState(State::New);

  if (fn) {
    setTask(fn);
  }
}

Hal_Proc::~Hal_Proc() noexcept try {
  if (getState() == State::Running) {
    stopExec();
  }

  setState(State::Invalid);
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

bool Hal_Proc::exec(Hal_Proc::Task fn) {
  if (fn) {
    setTask(fn);
  }

  return runExec();
}

Hal_Proc::State Hal_Proc::getState() const { return m_state; }

Hal_Proc::State Hal_Proc::setState(State state) {
  State oldstate = this->m_state;

  this->m_state = state;

  return oldstate;
}

void Hal_Proc::setTask(Hal_Proc::Task fn) {
  assert(getState() == State::New || getState() == State::Ready);

  this->m_fn = fn;
  setState(State::Ready);
}

bool Hal_Proc::wait() {
  int err{};
  void *pRet{};

  if (getState() != State::Running) {
    throw std::runtime_error("No task is exec");
  }

  err = pthread_join(m_th, &pRet);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  setState(State::Ready);

  return 0 == err;
}

void Hal_Proc::yield() {
  pthread_testcancel();
  sched_yield();
}

bool Hal_Proc::stopExec() {
  int err{};

  if (getState() != State::Running) {
    throw std::runtime_error("No task is exec");
  }

  err = pthread_cancel(m_th);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  return wait();
}

bool Hal_Proc::runExec() {
  int err{};
  State oldstate{};

  if (getState() != State::Ready) {
    throw std::runtime_error("No task is assigned to the Hal_Proc (" + m_name +
                             ")");
  }

  oldstate = setState(State::Running);
  err = pthread_create(&m_th, NULL, &(Hal_Proc::runFnInThreadHelper), this);
  if (err) {
    setState(oldstate);
    return false;
  }

  return true;
}

void *Hal_Proc::runFnInThreadHelper(void *context) {
  Hal_Proc *proc{};
  int oldstate{};
  int err{};

  err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  err = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldstate);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  proc = (Hal_Proc *)context;
  proc->m_fn();

  return NULL;
}

} // namespace Hal
