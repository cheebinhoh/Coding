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

  std::shared_ptr<Hal::Hal_Io<std::string>> readSocket2 = std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5000);
  Hal::Hal_Proc readProc{"readSocket2", [&readSocket2]() {
    while (true) {
      auto data = readSocket2->read();
      if (data) {
        Hal::DMesgPb dmesgPbRead{};
      
        dmesgPbRead.ParseFromString(*data);
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
  Hal::Hal_DMesgNet dmesgnet1{"dmesg1", writeSocket1, readSocket1};


  std::this_thread::sleep_for(std::chrono::seconds(10));
  dmesgnet1.waitForEmpty();

/*
  std::shared_ptr<Hal::Hal_Io<std::string>> writeSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);
  std::shared_ptr<Hal::Hal_Io<std::string>> readSocket2 = std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5000);

  Hal::Hal_DMesgNet dmesgnet1{"dmesg1", writeSocket1};
  Hal::Hal_DMesgNet dmesgnet2{"dmesg2", nullptr, readSocket2};

  auto readHandler2 = dmesgnet2.openHandler("dmesg2.readHandler");

  Hal::DMesgPb dmesgPbRead{};
  Hal::Hal_Proc proc2{"dmesg2", [readHandler2, &dmesgPbRead](){
    auto data = readHandler2->read();
    if (data) {
      dmesgPbRead = *data;
    }
  }};

  proc2.exec();
  Hal::Hal_Proc::yield();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto dmesgHandler = dmesgnet1.openHandler("writeHandler", nullptr, nullptr);
  EXPECT_TRUE(dmesgHandler);
 
  Hal::DMesgPb dmesgPb{};
  dmesgPb.set_identifier("counter sync");
  dmesgPb.set_type(Hal::DMesgTypePb::message);

  std::string data{"Hello dmesg async"};
  Hal::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message(data);

  dmesgHandler->write(dmesgPb);

  std::this_thread::sleep_for(std::chrono::seconds(5));

  std::string source = dmesgPbRead.sourceidentifier();
  EXPECT_TRUE(dmesgPbRead.type() == dmesgPb.type());
  EXPECT_TRUE(dmesgPbRead.sourceidentifier() == "dmesg2"); // the source is the local DmesgNet agent that read
  EXPECT_TRUE(dmesgPbRead.body().message() == dmesgPb.body().message());
 */

  return RUN_ALL_TESTS();
}
