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

  std::shared_ptr<Hal::Hal_Io<std::string>> readSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5000);
  std::shared_ptr<Hal::Hal_Io<std::string>> writeSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);

  Hal::Hal_DMesgNet dmesgnet1{"dmesg1", writeSocket1};

  Hal::DMesgPb dmesgPbRead{};
  Hal::Hal_Proc proc2{"dmesg2", [readSocket1, &dmesgPbRead](){
    auto data = readSocket1->read();
    if (data) {
      dmesgPbRead.ParseFromString(*data);
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

  std::string source = dmesgPbRead.source();
  EXPECT_TRUE(dmesgPbRead.type() == dmesgPb.type());
  EXPECT_TRUE(dmesgPbRead.source() == "dmesg1");
  EXPECT_TRUE(dmesgPbRead.body().message() == dmesgPb.body().message());
 
  return RUN_ALL_TESTS();
}
