/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-async.hpp"
#include "hal-proc.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class Counter : Hal::Hal_Async {
public:
  Counter() : Hal::Hal_Async{"counter"} {}

  void increment() {
    HAL_ASYNC_CALL_WITH_REF_CAPTURE({ this->count++; });
  }

  operator long long() { return count; }

private:
  long long count{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  Counter cnt{};
  Hal::Hal_Proc proc1{"proc1", [&cnt]() {
                   for (int n = 0; n < 100; n++) {
                     cnt.increment();
                     Hal::Hal_Proc::yield();
                   }
                 }};

  Hal::Hal_Proc proc2{"proc2", [&cnt]() {
                   for (int n = 0; n < 100; n++) {
                     cnt.increment();
                     Hal::Hal_Proc::yield();
                   }
                 }};

  proc1.exec();
  proc2.exec();
  std::this_thread::sleep_for(std::chrono::seconds(5));

  EXPECT_TRUE(static_cast<long long>(cnt) == 200);

  Hal::Hal_Async async{"timer"};
  int val = 1;
  async.execAfter(std::chrono::seconds(5), [&val]() {
                    val = 2;
                  });
  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(1 == val);

  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(2 == val);

  return RUN_ALL_TESTS();
}
