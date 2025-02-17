/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-event.hpp"

#include "hal-async.hpp"

#include "memory"

std::once_flag Hal_Event_Manager::s_InitOnce{};
std::shared_ptr<Hal_Event_Manager> Hal_Event_Manager::s_instance{};

Hal_Event_Manager::Hal_Event_Manager()
    : Hal_Singleton{}, Hal_Async{"Hal_Event_Manager"} {}

Hal_Event_Manager::~Hal_Event_Manager() {}

void Hal_Event_Manager::exitMainLoop() {
  HAL_ASYNC_CALL_WITH_REF_CAPTURE({ this->exitMainLoopPrivate(); });
}

void Hal_Event_Manager::enterMainLoop() {
  Hal_Proc::yield();
  this->wait();
}

void Hal_Event_Manager::exitMainLoopPrivate() { this->stopExec(); }
