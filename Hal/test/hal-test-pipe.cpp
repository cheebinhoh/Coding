#include "hal-pipe.hpp"

#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  long cnt{};

  Hal_Pipe<long> pipe{"pipe", [&cnt](long val) {
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        cnt++;
                        std::cout << "read\n";
                      }};

  for (int i = 0; i < 5; i++) {
    pipe.write(i);
  }

  std::cout << "Before wait for empty\n";
  pipe.waitForEmpty();
  std::cout << "Total count: " << cnt << "\n";

  return 0;
}
