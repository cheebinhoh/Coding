#include <cstdlib>
#include <ctime>
#include <cstring>
#include <chrono>
#include <iostream>
#include <map>
#include <mutex>

#include <thread>
#include <pthread.h>
#include <unistd.h>

#include "hal-pipe.hpp"
#include "hal-proc.hpp"

std::mutex log_mutex {};

#define safethread_log(stmt) do { \
  std::lock_guard<std::mutex> lck(log_mutex); \
  stmt; \
} while (0)

int main(int argc, char *argv[])
{
  using std::chrono::high_resolution_clock;
  using std::chrono::system_clock;

  std::map<std::string, long long> input_cnt {};
  Hal_Proc gpr_input {"gpr input"};
  Hal_Proc ext_input {"ext input"};

  Hal_Pipe<std::string> out_pipe {"out_pipe", [&input_cnt](std::string item) {
    std::size_t found = item.find(": ");
    if (found != std::string::npos) {
        std::string source = item.substr(0, found);

        input_cnt[source]++;
    }
  } };

  Hal_Pipe<std::string> staging_pipe {"staging_input", [&out_pipe](std::string item) {
    out_pipe.push(item);
  } };

  gpr_input.exec([&out_pipe]() {
    int i {0};

    system_clock::time_point start = system_clock::now();

    while (true) {
       std::string item = "gpr_input: " + std::to_string(i) + ": data";
       out_pipe.push(item);

       i++;

       std::this_thread::sleep_for(std::chrono::milliseconds(1));
       system_clock::time_point wakeup = system_clock::now();
       if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start).count() >= 5) {
          break;
       }
    }

    safethread_log(std::cout << "gpr input: end: " << i << "\n");
  } );

  ext_input.exec([&out_pipe]() {
    int i {0};

    system_clock::time_point start = system_clock::now();

    while (true) {
       std::string item = "ext_input: " + std::to_string(i) + ": data";
       out_pipe.push(item);

       i++;

       std::this_thread::sleep_for(std::chrono::milliseconds(1));
       system_clock::time_point wakeup = system_clock::now();
       if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start).count() >= 5) {
          break;
       }
    }

    safethread_log(std::cout << "ext_input: end: " << i << "\n");
  } );

  safethread_log(std::cout << "Before mainloop\n");
  while (true) {
       std::this_thread::sleep_for(std::chrono::seconds(10));
       safethread_log(std::cout << "mainloop wakeup\n");

       for (auto & pair : input_cnt) {
          safethread_log(std::cout << "source: " << pair.first << ", cnt: " << pair.second << "\n");
       }
  }

  return 0;
}
