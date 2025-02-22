/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "proto/hal-dmesg.pb.h"

#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
 
  Hal::DMesg dmesg{};
  dmesg.set_identifier("id1");
  dmesg.set_runningcounter(99);
  dmesg.set_source("cheebin th");
  dmesg.set_type(Hal::DMesgType::message);

  Hal::DMesgBody *dmsgbody = dmesg.mutable_body();
  dmsgbody->set_message("message string");

  EXPECT_TRUE(dmesg.identifier() == "id1");
  EXPECT_TRUE(dmesg.runningcounter() == 99);
  EXPECT_TRUE(dmesg.source() == "cheebin th");
  EXPECT_TRUE(dmesg.type() == Hal::DMesgType::message);
  EXPECT_TRUE(dmesg.body().has_message());

  Hal::DMesg dmesgDefault{};
  EXPECT_TRUE(!dmesgDefault.body().has_message());

  return RUN_ALL_TESTS();
}
