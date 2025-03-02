/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-socket.hpp"
#include "hal-proc.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  Hal::Hal_Socket readSocket{"127.0.0.1", 5000};
  Hal::Hal_Socket writeSocket{"127.0.0.1", 5000, true};

  std::string readData{};

  Hal::Hal_Proc readProc{"readProc", [&readSocket, &readData]() {
    auto data = readSocket.read();
    if (data) {
      readData = std::move_if_noexcept(*data);
    }
  }};

  std::string writeData{"hello socket"};
  Hal::Hal_Proc writeProc{"readProc", [&writeSocket, &writeData]() {
    writeSocket.write(writeData);
  }};

  readProc.exec();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  writeProc.exec();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  writeProc.wait();
  readProc.wait();

  EXPECT_TRUE(writeData == readData);

  return RUN_ALL_TESTS();
}
