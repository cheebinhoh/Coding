#include "hal-proc.hpp"

#include <cassert>
#include <cstring>
#include <exception>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <stdexcept>
#include <string>
#include <string_view>

Hal_Proc::Hal_Proc(std::string_view name, Hal_ProcTask fn) : m_name{name} {
  setState(Hal_ProcState::New);

  if (fn) {
    setTask(fn);
  }
}

Hal_Proc::~Hal_Proc() {
  if (getState() == Hal_ProcState::Running) {
    stopExec();
  }

  setState(Hal_ProcState::Invalid);
}

bool Hal_Proc::exec(Hal_ProcTask fn) {
  if (fn) {
    setTask(fn);
  }

  return runExec();
}

Hal_ProcState Hal_Proc::getState() const { return m_state; }

Hal_ProcState Hal_Proc::setState(Hal_ProcState state) {
  Hal_ProcState oldstate = this->m_state;

  this->m_state = state;

  return oldstate;
}

void Hal_Proc::setTask(Hal_ProcTask fn) {
  assert(getState() == Hal_ProcState::New ||
         getState() == Hal_ProcState::Ready);

  this->m_fn = fn;
  setState(Hal_ProcState::Ready);
}

bool Hal_Proc::wait() {
  int err{};
  void *pRet{};

  if (getState() != Hal_ProcState::Running) {
    throw std::runtime_error("No task is exec");
  }

  err = pthread_join(m_th, &pRet);
  if (err) {
    std::cerr << strerror(err) << "\n";
  }

  setState(Hal_ProcState::Ready);

  return 0 == err;
}

void Hal_Proc::yield() { sched_yield(); }

void *Hal_Proc::runFnInThreadHelper(void *context) {
  Hal_Proc *proc{};
  int oldstate{};
  int err;

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

bool Hal_Proc::stopExec() {
  int err{};

  if (getState() != Hal_ProcState::Running) {
    throw std::runtime_error("No task is exec");
  }

  err = pthread_cancel(m_th);
  if (err) {
    std::cerr << strerror(err) << "\n";
  }

  return wait();
}

bool Hal_Proc::runExec() {
  int err{};
  Hal_ProcState oldstate;

  if (getState() != Hal_ProcState::Ready) {
    throw std::runtime_error("No task is assigned to the Hal_Proc (" + m_name +
                             ")");
  }

  oldstate = setState(Hal_ProcState::Running);
  err = pthread_create(&m_th, NULL, &(Hal_Proc::runFnInThreadHelper), this);
  if (err) {
    setState(oldstate);
    return false;
  }

  return true;
}
