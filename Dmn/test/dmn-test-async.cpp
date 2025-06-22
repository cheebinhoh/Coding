/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-test-async.cpp
 * @brief The unit test for dmn-async module.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "dmn-async.hpp"
#include "dmn-proc.hpp"

class Counter : dmn::Dmn_Async {
public:
  Counter() : dmn::Dmn_Async{"counter"} {}

  void increment() {
    DMN_ASYNC_CALL_WITH_REF_CAPTURE({ this->m_count++; });
  }

  operator long long() { return m_count; }

private:
  long long m_count{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  Counter cnt{};
  dmn::Dmn_Proc proc1{"proc1", [&cnt]() {
                        for (int n = 0; n < 100; n++) {
                          cnt.increment();
                          dmn::Dmn_Proc::yield();
                        }
                      }};

  dmn::Dmn_Proc proc2{"proc2", [&cnt]() {
                        for (int n = 0; n < 100; n++) {
                          cnt.increment();
                          dmn::Dmn_Proc::yield();
                        }
                      }};

  proc1.exec();
  proc2.exec();
  std::this_thread::sleep_for(std::chrono::seconds(5));

  EXPECT_TRUE(static_cast<long long>(cnt) == 200);

  dmn::Dmn_Async async{"timer"};
  int val = 1;
  async.addExecTaskAfter(std::chrono::seconds(5), [&val]() { val = 2; });
  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(1 == val);

  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(2 == val);

  bool done{};
  dmn::Dmn_Async asyncWithWait{"async"};

  auto waitHandler = asyncWithWait.addExecTaskWithWait([&done] {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    done = true;
  });

  waitHandler->wait();
  EXPECT_TRUE(done);

  return RUN_ALL_TESTS();
}
