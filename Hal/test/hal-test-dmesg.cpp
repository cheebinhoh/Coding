/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-dmesg.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  Hal::Hal_DMesg dmesg{"dmesg"};
  
  auto dmesgWriteHandler = dmesg.openHandler("writeHandler");
  EXPECT_TRUE(dmesgWriteHandler);

  auto dmesgReadHandler1 = dmesg.openHandler("readHandler1");
  auto dmesgReadHandler2 = dmesg.openHandler("readHandler2");

  Hal::DMesgPb dmesgPb{};
  dmesgPb.set_identifier("id1");
  dmesgPb.set_runningcounter(99);
  dmesgPb.set_source("unknown");
  dmesgPb.set_type(Hal::DMesgTypePb::message);

  Hal::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message("message string");

  Hal::Hal_Proc procRead1{"read1", [&dmesgReadHandler1, &dmesgPb]() {
    std::cout << "before read1\n";
    auto dmesgPbRead = dmesgReadHandler1->read();
    std::cout << "after read1, and proceed to validate read\n";
    EXPECT_TRUE(dmesgPbRead->identifier() == dmesgPb.identifier());
    EXPECT_TRUE(dmesgPbRead->source() == dmesgPb.source());
    EXPECT_TRUE(dmesgPbRead->runningcounter() == dmesgPb.runningcounter());
    EXPECT_TRUE(dmesgPbRead->type() == dmesgPb.type());
    EXPECT_TRUE(dmesgPbRead->body().message() == dmesgPb.body().message());
  }};

  Hal::Hal_Proc procRead2{"read2", [&dmesgReadHandler2, &dmesgPb]() {
    std::cout << "before read2\n";
    auto dmesgPbRead = dmesgReadHandler2->read();
    std::cout << "after read2, and proceed to validate read\n";
    EXPECT_TRUE(dmesgPbRead->identifier() == dmesgPb.identifier());
    EXPECT_TRUE(dmesgPbRead->source() == dmesgPb.source());
    EXPECT_TRUE(dmesgPbRead->runningcounter() == dmesgPb.runningcounter());
    EXPECT_TRUE(dmesgPbRead->type() == dmesgPb.type());
    EXPECT_TRUE(dmesgPbRead->body().message() == dmesgPb.body().message());
  }};

  procRead1.exec();
  procRead2.exec();
  Hal::Hal_Proc::yield();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "after sleep 3 seconds\n";

  dmesgWriteHandler->write(dmesgPb);
  dmesg.waitForEmpty();
  dmesgReadHandler1->waitForEmpty();
  dmesgReadHandler2->waitForEmpty();

  std::cout << "after sleep 5 seconds\n";
  std::this_thread::sleep_for(std::chrono::seconds(5));

  dmesg.closeHandler(dmesgWriteHandler);
  dmesg.closeHandler(dmesgReadHandler1);
  dmesg.closeHandler(dmesgReadHandler2);
 
/*
  Hal::Hal_Proc proc{"publish thread", [&dmesg, &dmesgPb]() {
    std::cout << "sleep 5 seconds before publishing\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    dmesg.publish(dmesgPb);
    std::cout << "done published\n";
  }};

  proc.exec();
  Hal::Hal_Proc::yield();

  auto dmesgPbRead = dmesgHandler->read();
  if (dmesgPbRead) {
    std::cout << "data read\n";
    EXPECT_TRUE(dmesgPbRead->identifier() == dmesgPb.identifier());
    EXPECT_TRUE(dmesgPbRead->runningcounter() == dmesgPb.runningcounter());
    EXPECT_TRUE(dmesgPbRead->source() == dmesgPb.source());
    EXPECT_TRUE(dmesgPbRead->type() == dmesgPb.type());
    EXPECT_TRUE(dmesgPbRead->body().message() == dmesgPb.body().message());
  }
*/


  return RUN_ALL_TESTS();
}
