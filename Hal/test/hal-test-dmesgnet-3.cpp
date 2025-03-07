/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-dmesgnet.hpp"
#include "hal-socket.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  std::shared_ptr<Hal::Hal_Io<std::string>> readSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5001);
  std::shared_ptr<Hal::Hal_Io<std::string>> writeSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);
  Hal::Hal_DMesgNet dmesgnet1{"dmesg-3", writeSocket1, readSocket1};

  std::this_thread::sleep_for(std::chrono::seconds(10));
  dmesgnet1.waitForEmpty();

  return RUN_ALL_TESTS();
}
