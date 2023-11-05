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
  Hal_Proc gps_input {"gps input"};
  Hal_Proc imu_input {"imu input"};
  Hal_Proc ext_input {"ext input"};

  // parameters to tune
  int input_to_sleep_milliseconds = 1;
  int input_to_run_seconds = 5;

  Hal_Pipe<std::string> out_pipe {"out_pipe", [&input_cnt](std::string item) {
    std::size_t found = item.find(": ");
    if (found != std::string::npos) {
        std::string source = item.substr(0, found);

        input_cnt[source]++;
    }
  } };

  Hal_Pipe<std::string> cal_pipe {"cal_input", [&out_pipe](std::string item) {
    out_pipe.push(item);
  } };

  Hal_Pipe<std::string> filter_pipe {"filter_input", [&cal_pipe](std::string item) {
    cal_pipe.push(item);
  } };

  Hal_Pipe<std::string> staging_pipe {"staging_input", [&filter_pipe](std::string item) {
    filter_pipe.push(item);
  } };

  gpr_input.exec([&staging_pipe, input_to_sleep_milliseconds, input_to_run_seconds]() {
    int i {0};

    system_clock::time_point start = system_clock::now();

    while (true) {
       std::string item = "gpr_input: " + std::to_string(i) + ": " + std::string(2000, 'x');
       staging_pipe.push(item);

       i++;

       std::this_thread::sleep_for(std::chrono::milliseconds(input_to_sleep_milliseconds));
       system_clock::time_point wakeup = system_clock::now();
       if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start).count() >= input_to_run_seconds) {
          break;
       }
    }

    safethread_log(std::cout << "gpr input: end: " << i << "\n");
  } );

  gps_input.exec([&staging_pipe, input_to_sleep_milliseconds, input_to_run_seconds]() {
    int i {0};

    system_clock::time_point start = system_clock::now();

    while (true) {
       std::string item = "gps_input: " + std::to_string(i) + ": data";
       staging_pipe.push(item);

       i++;

       std::this_thread::sleep_for(std::chrono::milliseconds(input_to_sleep_milliseconds));
       system_clock::time_point wakeup = system_clock::now();
       if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start).count() >= input_to_run_seconds) {
          break;
       }
    }

    safethread_log(std::cout << "gps_input: end: " << i << "\n");
  } );

  imu_input.exec([&staging_pipe, input_to_sleep_milliseconds, input_to_run_seconds]() {
    int i {0};

    system_clock::time_point start = system_clock::now();

    while (true) {
       std::string item = "imu_input: " + std::to_string(i) + ": data";
       staging_pipe.push(item);

       i++;

       std::this_thread::sleep_for(std::chrono::milliseconds(input_to_sleep_milliseconds));
       system_clock::time_point wakeup = system_clock::now();
       if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start).count() >= input_to_run_seconds) {
          break;
       }
    }

    safethread_log(std::cout << "imu_input: end: " << i << "\n");
  } );

  ext_input.exec([&staging_pipe, input_to_sleep_milliseconds, input_to_run_seconds]() {
    int i {0};

    system_clock::time_point start = system_clock::now();

    while (true) {
       std::string item = "ext_input: " + std::to_string(i) + ": data";
       staging_pipe.push(item);

       i++;

       std::this_thread::sleep_for(std::chrono::milliseconds(input_to_sleep_milliseconds));
       system_clock::time_point wakeup = system_clock::now();
       if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start).count() >= input_to_run_seconds) {
          break;
       }
    }

    safethread_log(std::cout << "ext_input: end: " << i << "\n");
  } );

  safethread_log(std::cout << "Before mainloop\n");
  while (true) {
       std::this_thread::sleep_for(std::chrono::seconds(10));
       safethread_log(std::cout << "\nmainloop wakeup\n");

       for (auto & pair : input_cnt) {
          safethread_log(std::cout << "source: " << pair.first << ", cnt: " << pair.second << "\n");
       }

       break;
  }

  // Hal_Proc and Hal_Pipe will be destroyed and display statistics

  return 0;
}
