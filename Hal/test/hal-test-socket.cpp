/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-socket.hpp"
#include "hal-proc.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  Hal::Hal_Socket readSocket{"127.0.0.1", 5000};
  Hal::Hal_Socket writeSocket{"127.0.0.1", 5000, true};
  Hal::Hal_Io<std::string> * input {&readSocket};
  Hal::Hal_Io<std::string> * output {&writeSocket};

  std::string readData{};

  Hal::Hal_Proc readProc{"readProc", [input, &readData]() {
    auto data = input->read();
    if (data) {
      readData = std::move_if_noexcept(*data);
    }
  }};

  std::string writeData{"hello socket"};
  Hal::Hal_Proc writeProc{"readProc", [output, &writeData]() {
    output->write(writeData);
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
