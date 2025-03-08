/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-async.hpp"
#include "dmn-proc.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

class Counter : Dmn::Dmn_Async {
public:
  Counter() : Dmn::Dmn_Async{"counter"} {}

  void increment() {
    DMN_ASYNC_CALL_WITH_REF_CAPTURE({ this->count++; });
  }

  operator long long() { return count; }

private:
  long long count{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  Counter cnt{};
  Dmn::Dmn_Proc proc1{"proc1", [&cnt]() {
                   for (int n = 0; n < 100; n++) {
                     cnt.increment();
                     Dmn::Dmn_Proc::yield();
                   }
                 }};

  Dmn::Dmn_Proc proc2{"proc2", [&cnt]() {
                   for (int n = 0; n < 100; n++) {
                     cnt.increment();
                     Dmn::Dmn_Proc::yield();
                   }
                 }};

  proc1.exec();
  proc2.exec();
  std::this_thread::sleep_for(std::chrono::seconds(5));

  EXPECT_TRUE(static_cast<long long>(cnt) == 200);

  Dmn::Dmn_Async async{"timer"};
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
