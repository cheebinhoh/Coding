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

  Hal::DMesgPb dmesgPbLast{};
  std::shared_ptr<Hal::Hal_Io<std::string>> readSocket2 = std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5000);
  Hal::Hal_Proc readProc{"readSocket2", [&readSocket2, &dmesgPbLast]() mutable {
    while (true) {
      auto data = readSocket2->read();
      if (data) {
        Hal::DMesgPb dmesgPbRead{};
      
        dmesgPbRead.ParseFromString(*data);
        dmesgPbLast = dmesgPbRead;
        std::cout << "DMesgPb: " << dmesgPbRead.ShortDebugString() << "\n";
      } else {
        break;
      }
    }
  }};

  readProc.exec();
  Hal::Hal_Proc::yield();

  std::shared_ptr<Hal::Hal_Io<std::string>> writeSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);

  std::shared_ptr<Hal::Hal_Io<std::string>> readSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5001);
  std::unique_ptr<Hal::Hal_DMesgNet> dmesgnet1 = std::make_unique<Hal::Hal_DMesgNet>("dmesg1", writeSocket1, readSocket1);

  std::this_thread::sleep_for(std::chrono::seconds(10));
  dmesgnet1->waitForEmpty();
  EXPECT_TRUE(dmesgPbLast.body().sys().self().masteridentifier() != "");
  EXPECT_TRUE(dmesgPbLast.body().sys().self().state() == Hal::DMesgStatePb::Ready);

  dmesgnet1 = {};
  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(dmesgPbLast.body().sys().self().masteridentifier() == "");
  EXPECT_TRUE(dmesgPbLast.body().sys().self().state() == Hal::DMesgStatePb::Destroyed);

  return RUN_ALL_TESTS();
}
