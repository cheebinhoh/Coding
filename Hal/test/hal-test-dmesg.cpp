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
  
  auto dmesgHandler = dmesg.openHandler("handler1");
  EXPECT_TRUE(dmesgHandler);

  Hal::DMesgPb dmesgPb{};
  dmesgPb.set_identifier("id1");
  dmesgPb.set_runningcounter(99);
  dmesgPb.set_source("cheebin th");
  dmesgPb.set_type(Hal::DMesgTypePb::message);

  Hal::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message("message string");

  Hal::Hal_Proc proc{"publish thread", [&dmesg, &dmesgPb]() {
    std::cout << "sleep 5 seconds before publishing\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    dmesg.publish(dmesgPb);
    std::cout << "done published\n";
  }};

  proc.exec();
  Hal::Hal_Proc::yield();

  auto dmesgPbRead = dmesgHandler->readDMesg();
  if (dmesgPbRead) {
    std::cout << "data read\n";
    EXPECT_TRUE(dmesgPbRead->identifier() == dmesgPb.identifier());
    EXPECT_TRUE(dmesgPbRead->runningcounter() == dmesgPb.runningcounter());
    EXPECT_TRUE(dmesgPbRead->source() == dmesgPb.source());
    EXPECT_TRUE(dmesgPbRead->type() == dmesgPb.type());
    EXPECT_TRUE(dmesgPbRead->body().message() == dmesgPb.body().message());
  }

  dmesg.closeHandler(dmesgHandler);
 
  return RUN_ALL_TESTS();
}
