/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-pipe.hpp"
#include "dmn-proc.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include <pthread.h>
#include <unistd.h>

std::mutex log_mutex{};

#define safethread_log(stmt)                                                   \
  do {                                                                         \
    std::lock_guard<std::mutex> lck(log_mutex);                                \
    stmt;                                                                      \
  } while (0)

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  using std::chrono::high_resolution_clock;
  using std::chrono::system_clock;

  std::map<std::string, long long> input_cnt{};
  Dmn::Dmn_Proc sensor_input{"sensor input"};
  Dmn::Dmn_Proc gps_input{"gps input"};
  Dmn::Dmn_Proc imu_input{"imu input"};
  Dmn::Dmn_Proc ext_input{"ext input"};

  // parameters to tune
  bool input_to_sleep_use_ns = true; /* nano or milliseconds */
  int input_to_sleep_milliseconds = 1;
  int input_to_sleep_nanoseconds = 500000; /* 0.5 milliseconds */
  int input_to_run_seconds = 5;

  Dmn::Dmn_Pipe<std::string> out_pipe{"out_pipe", [&input_cnt](std::string item) {
                                   std::size_t found = item.find(": ");
                                   if (found != std::string::npos) {
                                     std::string source = item.substr(0, found);

                                     input_cnt[source]++;
                                   }
                                 }};

  Dmn::Dmn_Pipe<std::string> cal_pipe{
      "cal_input", [&out_pipe](std::string item) { out_pipe.write(item); }};

  Dmn::Dmn_Pipe<std::string> filter_pipe{
      "filter_input", [&cal_pipe](std::string item) { cal_pipe.write(item); }};

  Dmn::Dmn_Pipe<std::string> staging_pipe{
      "staging_input",
      [&filter_pipe](std::string item) { filter_pipe.write(item); }};

  int sensor_input_i{0};
  sensor_input.exec([&staging_pipe, input_to_sleep_use_ns,
                     input_to_sleep_nanoseconds, input_to_sleep_milliseconds,
                     input_to_run_seconds, &sensor_input_i]() {
    system_clock::time_point start = system_clock::now();

    while (true) {
      std::string item =
          "sensor_input: " + std::to_string(sensor_input_i) + ": " + std::string(2000, 'x');
      staging_pipe.write(item);

      sensor_input_i++;

      if (input_to_sleep_use_ns) {
        std::this_thread::sleep_for(
            std::chrono::nanoseconds(input_to_sleep_nanoseconds));
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(input_to_sleep_milliseconds));
      }
      system_clock::time_point wakeup = system_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start)
              .count() >= input_to_run_seconds) {
        break;
      }
    }

    safethread_log(std::cout << "sensor input: end: " << sensor_input_i << "\n");
  });

  int gps_input_i{0};
  gps_input.exec([&staging_pipe, input_to_sleep_use_ns,
                  input_to_sleep_nanoseconds, input_to_sleep_milliseconds,
                  input_to_run_seconds, &gps_input_i]() {
    system_clock::time_point start = system_clock::now();

    while (true) {
      std::string item = "gps_input: " + std::to_string(gps_input_i) + ": data";
      staging_pipe.write(item);

      gps_input_i++;

      if (input_to_sleep_use_ns) {
        std::this_thread::sleep_for(
            std::chrono::nanoseconds(input_to_sleep_nanoseconds));
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(input_to_sleep_milliseconds));
      }

      system_clock::time_point wakeup = system_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start)
              .count() >= input_to_run_seconds) {
        break;
      }
    }

    safethread_log(std::cout << "gps_input: end: " << gps_input_i << "\n");
  });

  int imu_input_i{0};
  imu_input.exec([&staging_pipe, input_to_sleep_use_ns,
                  input_to_sleep_nanoseconds, input_to_sleep_milliseconds,
                  input_to_run_seconds, &imu_input_i]() {
    system_clock::time_point start = system_clock::now();

    while (true) {
      std::string item = "imu_input: " + std::to_string(imu_input_i) + ": data";
      staging_pipe.write(item);

      imu_input_i++;

      if (input_to_sleep_use_ns) {
        std::this_thread::sleep_for(
            std::chrono::nanoseconds(input_to_sleep_nanoseconds));
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(input_to_sleep_milliseconds));
      }

      system_clock::time_point wakeup = system_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start)
              .count() >= input_to_run_seconds) {
        break;
      }
    }

    safethread_log(std::cout << "imu_input: end: " << imu_input_i << "\n");
  });

  int ext_input_i{0};
  ext_input.exec([&staging_pipe, input_to_sleep_use_ns,
                  input_to_sleep_nanoseconds, input_to_sleep_milliseconds,
                  input_to_run_seconds, &ext_input_i]() {

    system_clock::time_point start = system_clock::now();

    while (true) {
      std::string item = "ext_input: " + std::to_string(ext_input_i) + ": data";
      staging_pipe.write(item);

      ext_input_i++;

      if (input_to_sleep_use_ns) {
        std::this_thread::sleep_for(
            std::chrono::nanoseconds(input_to_sleep_nanoseconds));
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(input_to_sleep_milliseconds));
      }

      system_clock::time_point wakeup = system_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(wakeup - start)
              .count() >= input_to_run_seconds) {
        break;
      }
    }

    safethread_log(std::cout << "ext_input: end: " << ext_input_i << "\n");
  });

  safethread_log(std::cout << "Before mainloop\n");
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(30));
    safethread_log(std::cout << "\nmainloop wakeup\n");

    for (auto &pair : input_cnt) {
      safethread_log(std::cout << "source: " << pair.first
                               << ", cnt: " << pair.second << "\n");

      if (pair.first == "gps_input") {
        EXPECT_TRUE(gps_input_i == pair.second);
      }
      else if (pair.first == "ext_input") {
        EXPECT_TRUE(ext_input_i == pair.second);
      }
      else if (pair.first == "imu_input") {
        EXPECT_TRUE(imu_input_i == pair.second);
      }
      else if (pair.first == "sensor_input") {
        EXPECT_TRUE(sensor_input_i == pair.second);
      }
    }

    break;
  }

  // Dmn_Proc and Dmn_Pipe will be destroyed and display statistics
  return RUN_ALL_TESTS();
}
