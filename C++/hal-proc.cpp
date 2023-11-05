#include <cstring>
#include <exception>
#include <iostream>
#include <string>

#include <pthread.h>

#include "hal-proc.hpp"

Hal_Proc::Hal_Proc(std::string name, std::function<void()> fn) : m_fn {fn}, m_name {name} {
}

Hal_Proc::~Hal_Proc() {
   int err {};
   void *retptr {};

   std::cout << "~Hal_Proc(): " << m_name << " cancel thread and destroy\n";
   err = pthread_cancel(m_th);
   if (err) {
     std::cerr << strerror(err);
   }

   err = pthread_join(m_th, &retptr);
   if (err) {
     std::cerr << strerror(err);
   }
}

bool Hal_Proc::exec(std::function<void()> fn)
{
  if (fn) {
    m_fn = fn;
  }

  if (m_fn) {
    run_exec(m_fn);
  }
  else {
    throw std::runtime_error("no fn for Hal_proc");
  }

  return true;
}

void * Hal_Proc::thread_fn_helper(void *context) {
  Hal_Proc *proc {};
  int oldstate {};
  int err;

  err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldstate);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  proc = (Hal_Proc *)context;
  proc->m_fn();

  return NULL;
}

void Hal_Proc::run_exec(std::function<void()> fn)
{
  int err {};

  err = pthread_create(&m_th, NULL, &(Hal_Proc::thread_fn_helper), this);
  if (err) {
    throw std::runtime_error(strerror(err));
  }
}
