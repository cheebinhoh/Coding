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
  auto inst = Hal::Hal_Singleton::createInstance<Hal::Hal_Event_Manager>();

  // for TESTING
  Hal::Hal_Proc proc{
      "exitMainLoop", [&inst]() {
        std::cout << "sleep 10 seconds before setting handler for SIGTERM\n";
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << "set signal handler to respond to " << SIGTERM << "\n";

        inst->registerSignalHandler(
            SIGTERM, [&inst](int signo) { std::cout << "handling SIGTERM\n"; });
      }};

  proc.exec();

  inst->enterMainLoop();

  return 0;
}
