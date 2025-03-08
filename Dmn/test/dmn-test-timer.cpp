/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-timer.hpp"

#include <gtest/gtest.h>

#include <iostream>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  bool timerRun{};
  Dmn::Dmn_Timer timer{std::chrono::seconds(5),
                  [&timerRun]() {
                    timerRun = true; 
                    std::cout << "timer is run\n"; 
                  }};

  EXPECT_TRUE(!timerRun);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  EXPECT_TRUE(timerRun);

  return RUN_ALL_TESTS();
}
