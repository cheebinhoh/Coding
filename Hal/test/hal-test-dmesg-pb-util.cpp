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

  struct timeval tv;
  gettimeofday(&tv, NULL);

  EXPECT_TRUE(dmesgPb2.timestamp().seconds() != 0);
  EXPECT_TRUE(dmesgPb2.timestamp().nanos() != 0);
  EXPECT_TRUE(dmesgPb2.identifier() == "id2");
  EXPECT_TRUE(dmesgPb2.runningcounter() == 100);
  EXPECT_TRUE(dmesgPb2.sourceidentifier() == "node2");
  EXPECT_TRUE(dmesgPb2.type() == Hal::DMesgTypePb::sys);

  auto *self = dmesgPb2.mutable_body()->mutable_sys()->mutable_self();
  DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP(self, tv);
  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(self, "id4");
  DMESG_PB_SYS_NODE_SET_STATE(self,Hal::DMesgStatePb::Ready);
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "id5");

  EXPECT_TRUE(self->mutable_initializedtimestamp()->seconds() != 0);
  EXPECT_TRUE(self->mutable_initializedtimestamp()->nanos() != 0);
  EXPECT_TRUE(self->mutable_updatedtimestamp()->seconds() != 0);
  EXPECT_TRUE(self->mutable_updatedtimestamp()->nanos() != 0);
  EXPECT_TRUE(self->state() == Hal::DMesgStatePb::Ready);
  EXPECT_TRUE(self->identifier() == "id4");
  EXPECT_TRUE(self->masteridentifier() == "id5");

  auto *item1 = dmesgPb2.mutable_body()->mutable_sys()->add_nodelist();
  DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP(item1, tv);
  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(item1, tv);
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(item1, "id10");
  DMESG_PB_SYS_NODE_SET_STATE(item1, Hal::DMesgStatePb::MasterPending);
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(item1, "id15");

  auto *item2 = dmesgPb2.mutable_body()->mutable_sys()->add_nodelist();
  DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP(item2, tv);
  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(item2, tv);
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(item2, "id20");
  DMESG_PB_SYS_NODE_SET_STATE(item2, Hal::DMesgStatePb::Ready);
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(item2, "id25");

  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().size() == 2);

  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(0).initializedtimestamp().seconds() != 0);
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(0).initializedtimestamp().nanos() != 0);
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(0).identifier() == "id10");
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(0).masteridentifier() == "id15");
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(0).state() == Hal::DMesgStatePb::MasterPending);

  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(1).initializedtimestamp().seconds() != 0);
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(1).initializedtimestamp().nanos() != 0);
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(1).identifier() == "id20");
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(1).masteridentifier() == "id25");
  EXPECT_TRUE(dmesgPb2.body().sys().nodelist().Get(1).state() == Hal::DMesgStatePb::Ready);

  return RUN_ALL_TESTS();
}

