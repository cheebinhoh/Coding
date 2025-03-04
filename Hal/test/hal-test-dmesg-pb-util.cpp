/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-dmesg-pb-util.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sys/time.h>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
 
  DMESG_PB_INIT(dmesgPb);
  DMESG_PB_SET_IDENTIFIER(dmesgPb, "id1");
  DMESG_PB_SET_RUNNINGCOUNTER(dmesgPb, 99);
  DMESG_PB_SET_SOURCEIDENTIFIER(dmesgPb, "node1");
  DMESG_PB_SET_TYPE(dmesgPb, Hal::DMesgTypePb::message);
  DMESG_PB_MSG_SET_MESSAGE(dmesgPb, "hello dmesg");

  EXPECT_TRUE(dmesgPb.timestamp().seconds() != 0);
  EXPECT_TRUE(dmesgPb.timestamp().nanos() != 0);
  EXPECT_TRUE(dmesgPb.identifier() == "id1");
  EXPECT_TRUE(dmesgPb.runningcounter() == 99);
  EXPECT_TRUE(dmesgPb.sourceidentifier() == "node1");
  EXPECT_TRUE(dmesgPb.type() == Hal::DMesgTypePb::message);
  EXPECT_TRUE(dmesgPb.body().message() == "hello dmesg");

  DMESG_PB_INIT(dmesgPb2);
  DMESG_PB_SET_IDENTIFIER(dmesgPb2, "id2");
  DMESG_PB_SET_RUNNINGCOUNTER(dmesgPb2, 100);
  DMESG_PB_SET_SOURCEIDENTIFIER(dmesgPb2, "node2");
  DMESG_PB_SET_TYPE(dmesgPb2, Hal::DMesgTypePb::sys);

  EXPECT_TRUE(dmesgPb2.timestamp().seconds() != 0);
  EXPECT_TRUE(dmesgPb2.timestamp().nanos() != 0);
  EXPECT_TRUE(dmesgPb2.identifier() == "id2");
  EXPECT_TRUE(dmesgPb2.runningcounter() == 100);
  EXPECT_TRUE(dmesgPb2.sourceidentifier() == "node2");
  EXPECT_TRUE(dmesgPb2.type() == Hal::DMesgTypePb::sys);

  return RUN_ALL_TESTS();
}

