/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 */

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

using std::chrono::system_clock;

void reader_func(int seconds) {
  int mins = seconds / 60;
  seconds = seconds % 60;
  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);

  ptm->tm_min += mins;
  ptm->tm_sec += seconds;

  std::time_t next_tt = mktime(ptm);
  tt = system_clock::to_time_t(system_clock::now());
  while (tt < next_tt) {
    std::this_thread::yield();

    tt = system_clock::to_time_t(system_clock::now());
  }
}

int main(int argc, char *argv[]) {
  reader_func(5);

  std::cout << "End\n";

  return 0;
}
