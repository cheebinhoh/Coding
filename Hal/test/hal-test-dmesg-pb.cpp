/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "proto/hal-dmesg.pb.h"

#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
 
  Hal::DMesgPb dmesgPb{};
  dmesgPb.set_identifier("id1");
  dmesgPb.set_runningcounter(99);
  dmesgPb.set_source("cheebin th");
  dmesgPb.set_type(Hal::DMesgTypePb::message);

  Hal::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message("message string");

  EXPECT_TRUE(dmesgPb.identifier() == "id1");
  EXPECT_TRUE(dmesgPb.runningcounter() == 99);
  EXPECT_TRUE(dmesgPb.source() == "cheebin th");
  EXPECT_TRUE(dmesgPb.type() == Hal::DMesgTypePb::message);
  EXPECT_TRUE(dmesgPb.body().has_message());

  Hal::DMesgPb dmesgDefaultPb{};
  EXPECT_TRUE(!dmesgDefaultPb.body().has_message());

  return RUN_ALL_TESTS();
}
