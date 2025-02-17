/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This is the hal daemon that runs as after a system bootup,
 * and receive and read, write and process message from network
 * of distributed hal nodes.
 */

#include "hal.hpp"

#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  auto inst = Hal_Singleton::createInstance<Hal_Event_Manager>();

  // Hal_Proc proc{"exitMainLoop", [inst]() {
  //                 std::this_thread::sleep_for(std::chrono::seconds(3));
  //                 inst->exitMainLoop();
  //               }};

  // proc.exec();

  inst->enterMainLoop();

  return 0;
}
