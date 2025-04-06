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

  std::shared_ptr<Dmn::Dmn_Io<std::string>> writeSocket1 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);
  std::shared_ptr<Dmn::Dmn_Io<std::string>> readSocket2 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1", 5000);

  Dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", nullptr, writeSocket1};
  Dmn::Dmn_DMesgNet dmesgnet2{"dmesg2", readSocket2};

  auto readHandler2 = dmesgnet2.openHandler("dmesg2.readHandler");

  Dmn::DMesgPb dmesgPbRead{};
  Dmn::Dmn_Proc proc2{"dmesg2", [readHandler2, &dmesgPbRead](){
    auto data = readHandler2->read();
    if (data) {
      dmesgPbRead = *data;
    }
  }};

  proc2.exec();
  Dmn::Dmn_Proc::yield();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto dmesgHandler = dmesgnet1.openHandler("writeHandler", nullptr, nullptr);
  EXPECT_TRUE(dmesgHandler);
 
  Dmn::DMesgPb dmesgPb{};
  dmesgPb.set_topic("counter sync");
  dmesgPb.set_type(Dmn::DMesgTypePb::message);

  std::string data{"Hello dmesg async"};
  Dmn::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message(data);

  dmesgHandler->write(dmesgPb);

  std::this_thread::sleep_for(std::chrono::seconds(5));

  std::string source = dmesgPbRead.sourceidentifier();
  EXPECT_TRUE(dmesgPbRead.type() == dmesgPb.type());
  EXPECT_TRUE(dmesgPbRead.sourceidentifier() == "dmesg2"); // the source is the local DmesgNet agent that read
  EXPECT_TRUE(dmesgPbRead.body().message() == dmesgPb.body().message());
 
  return RUN_ALL_TESTS();
}
