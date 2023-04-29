/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates some features of std::chrono
 */

#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  using std::chrono::system_clock;

  std::chrono::minutes span(5);
  system_clock::time_point now = system_clock::now();
  system_clock::time_point after_5_min = now + span;

  std::time_t tt;
  tt = system_clock::to_time_t(now);
  std::cout << "Now is " << ctime(&tt);

  std::cout << "Add the duration: " << span.count() << "\n\n";

  tt = system_clock::to_time_t(after_5_min);
  std::cout << "After 5 min is " << ctime(&tt) << "\n";

  long sum = 0;
  now = system_clock::now();
  for (long i = 0; i < 10000; i++) {
    for (long j = 0; j < 100000; j++) {
      sum = sum + i + j;
    }
  }

  system_clock::time_point done_1 = system_clock::now();
  std::cout
      << "Duration (in seconds): "
      << std::chrono::duration_cast<std::chrono::seconds>(done_1 - now).count()
      << "\n";
  std::cout << "Duration (in milliseconds): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(done_1 -
                                                                     now)
                   .count()
            << "\n";
  std::cout << "Duration (in nanoseconds): "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(done_1 -
                                                                    now)
                   .count()
            << "\n";

  std::cout << "\n";
  std::cout << "Time since epoch (in nanoseconds) for start: "
            << std::chrono::time_point_cast<std::chrono::nanoseconds>(now)
                   .time_since_epoch()
                   .count()
            << "\n";
  std::cout << "Time since epoch (in nanoseconds) for done: "
            << std::chrono::time_point_cast<std::chrono::nanoseconds>(done_1)
                   .time_since_epoch()
                   .count()
            << "\n";
  std::cout << "Duration (in nanoseconds) between start to done: "
            << std::chrono::time_point_cast<std::chrono::nanoseconds>(done_1)
                       .time_since_epoch()
                       .count() -
                   std::chrono::time_point_cast<std::chrono::nanoseconds>(now)
                       .time_since_epoch()
                       .count()
            << "\n";

  return 0;
}
