/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesg.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  Dmn::Dmn_DMesg dmesg{"dmesg"};
  
  auto dmesgWriteHandler = dmesg.openHandler("writeHandler");
  EXPECT_TRUE(dmesgWriteHandler);

  auto dmesgReadHandler1 = dmesg.openHandler("readHandler1");
  auto dmesgReadHandler2 = dmesg.openHandler("readHandler2", [](const Dmn::DMesgPb &dmesgPb) -> bool {
    return dmesgPb.body().message() != "message string 1";
  });

  Dmn::DMesgPb dmesgPb1{};
  dmesgPb1.set_topic("id1");
  dmesgPb1.set_runningcounter(99);
  dmesgPb1.set_sourceidentifier("unknown");
  dmesgPb1.set_type(Dmn::DMesgTypePb::message);

  Dmn::DMesgBodyPb *dmsgbodyPb1 = dmesgPb1.mutable_body();
  dmsgbodyPb1->set_message("message string 1");

  Dmn::DMesgPb dmesgPb2{};
  dmesgPb2.set_topic("id1");
  dmesgPb2.set_runningcounter(99);
  dmesgPb2.set_sourceidentifier("unknown");
  dmesgPb2.set_type(Dmn::DMesgTypePb::message);

  Dmn::DMesgBodyPb *dmsgbodyPb2 = dmesgPb2.mutable_body();
  dmsgbodyPb2->set_message("message string 2");

  Dmn::Dmn_Proc procRead1{"read1", [&dmesgReadHandler1, &dmesgPb1]() {
    std::cout << "before read1\n";
    auto dmesgPbRead = dmesgReadHandler1->read();
    std::cout << "after read1, and proceed to validate read\n";
    EXPECT_TRUE(dmesgPbRead->topic() == dmesgPb1.topic());
    EXPECT_TRUE(dmesgPbRead->sourceidentifier() == dmesgPb1.sourceidentifier());
    EXPECT_TRUE(dmesgPbRead->runningcounter() == dmesgPb1.runningcounter());
    EXPECT_TRUE(dmesgPbRead->type() == dmesgPb1.type());
    EXPECT_TRUE(dmesgPbRead->body().message() == dmesgPb1.body().message());
  }};

  Dmn::Dmn_Proc procRead2{"read2", [&dmesgReadHandler2, &dmesgPb2]() {
    std::cout << "before read2\n";
    auto dmesgPbRead = dmesgReadHandler2->read();
    std::cout << "after read2, and proceed to validate read\n";
    EXPECT_TRUE(dmesgPbRead->topic() == dmesgPb2.topic());
    EXPECT_TRUE(dmesgPbRead->sourceidentifier() == dmesgPb2.sourceidentifier());
    EXPECT_TRUE(dmesgPbRead->runningcounter() == dmesgPb2.runningcounter());
    EXPECT_TRUE(dmesgPbRead->type() == dmesgPb2.type());
    EXPECT_TRUE(dmesgPbRead->body().message() == dmesgPb2.body().message());
  }};

  procRead1.exec();
  procRead2.exec();
  Dmn::Dmn_Proc::yield();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "after sleep 3 seconds\n";

  dmesgWriteHandler->write(dmesgPb1);
  dmesgWriteHandler->write(dmesgPb2);

  dmesg.waitForEmpty();
  dmesgReadHandler1->waitForEmpty();
  dmesgReadHandler2->waitForEmpty();

  std::cout << "after sleep 5 seconds\n";
  std::this_thread::sleep_for(std::chrono::seconds(5));

  dmesg.closeHandler(dmesgWriteHandler);
  dmesg.closeHandler(dmesgReadHandler1);
  dmesg.closeHandler(dmesgReadHandler2);
 
  return RUN_ALL_TESTS();
}
