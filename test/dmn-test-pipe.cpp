/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "dmn-pipe.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  int cnt{};
  dmn::Dmn_Pipe<int> pipe{"pipe", [&cnt](int val) {
                            std::this_thread::sleep_for(
                                std::chrono::seconds(1));

                            EXPECT_TRUE(val == cnt);

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
