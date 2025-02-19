#include "hal-timer.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
  Hal_Timer timer{std::chrono::seconds(1),
                  []() { std::cout << "timer is run\n"; }};

  std::this_thread::sleep_for(std::chrono::seconds(5));

  timer.start(std::chrono::seconds(2));

  std::this_thread::sleep_for(std::chrono::seconds(10));

  return 0;
}
