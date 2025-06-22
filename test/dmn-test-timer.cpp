/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "dmn-timer.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  bool timer_run{};
  dmn::Dmn_Timer timer{std::chrono::seconds(5), [&timer_run]() {
                         timer_run = true;
                         std::cout << "timer is run\n";
                       }};

  EXPECT_TRUE(!timer_run);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  EXPECT_TRUE(timer_run);

  return RUN_ALL_TESTS();
}
