/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesgnet.hpp"
#include "dmn-socket.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  Dmn::DMesgPb dmesgPbLast{};
  std::shared_ptr<Dmn::Dmn_Io<std::string>> readSocket2 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1", 5000);
  Dmn::Dmn_Proc readProc{"readSocket2", [&readSocket2, &dmesgPbLast]() mutable {
    while (true) {
      auto data = readSocket2->read();
      if (data) {
        Dmn::DMesgPb dmesgPbRead{};
      
        dmesgPbRead.ParseFromString(*data);
        dmesgPbLast = dmesgPbRead;
        std::cout << "DMesgPb: " << dmesgPbRead.ShortDebugString() << "\n";
      } else {
        break;
      }
    }
  }};

  readProc.exec();
  Dmn::Dmn_Proc::yield();

  std::shared_ptr<Dmn::Dmn_Io<std::string>> writeSocket1 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);

  std::shared_ptr<Dmn::Dmn_Io<std::string>> readSocket1 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1", 5001);
  Dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", readSocket1, writeSocket1};


  std::this_thread::sleep_for(std::chrono::seconds(10));
  dmesgnet1.waitForEmpty();
  EXPECT_TRUE(dmesgPbLast.body().sys().self().masteridentifier() != "");
  EXPECT_TRUE(dmesgPbLast.body().sys().self().state() == Dmn::DMesgStatePb::Ready);

  return RUN_ALL_TESTS();
}
