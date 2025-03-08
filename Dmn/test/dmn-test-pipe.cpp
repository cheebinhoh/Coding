/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-pipe.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  long cnt{};

  Dmn::Dmn_Pipe<long> pipe{"pipe", [&cnt](long val) {
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        cnt++;
                        std::cout << "read\n";
                      }};

  for (int i = 0; i < 5; i++) {
    pipe.write(i);
  }

  std::cout << "Before wait for empty\n";
  pipe.waitForEmpty();
  EXPECT_TRUE(5 == cnt);

  return RUN_ALL_TESTS();
}
