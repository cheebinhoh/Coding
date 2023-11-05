#include <exception>
#include <iostream>
#include <string>
#include <thread>

#include "hal-proc.hpp"

Hal_Proc::Hal_Proc(std::string name, std::function<void()> fn) : m_fn {fn}, m_name {name} {
}

bool Hal_Proc::exec(std::function<void()> fn)
{
  if (fn) {
    run_exec(fn);
  }
  else if (m_fn) {
    run_exec(m_fn);
  }
  else {
    throw std::runtime_error("no fn for Hal_proc");
  }

  return true;
}

void Hal_Proc::run_exec(std::function<void()> fn)
{
  m_th = std::thread(fn);
}
