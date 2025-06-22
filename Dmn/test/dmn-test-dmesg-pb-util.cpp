/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include <sys/time.h>

#include <gtest/gtest.h>

#include <iostream>

#include "dmn-dmesg-pb-util.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  struct timeval tv;
  gettimeofday(&tv, NULL);

  dmn::DMesgPb dmesgpb{};

  DMESG_PB_SET_MSG_TIMESTAMP_FROM_TV(dmesgpb, tv);
  DMESG_PB_SET_MSG_TOPIC(dmesgpb, "id1");
  DMESG_PB_SET_MSG_RUNNINGCOUNTER(dmesgpb, 99);
  DMESG_PB_SET_MSG_SOURCEIDENTIFIER(dmesgpb, "node1");
  DMESG_PB_SET_MSG_TYPE(dmesgpb, dmn::DMesgTypePb::message);
  DMESG_PB_MSG_SET_MESSAGE(dmesgpb, "hello dmesg");

  EXPECT_TRUE(dmesgpb.timestamp().seconds() != 0);
  EXPECT_TRUE(dmesgpb.timestamp().nanos() != 0);
  EXPECT_TRUE(dmesgpb.topic() == "id1");
  EXPECT_TRUE(dmesgpb.runningcounter() == 99);
  EXPECT_TRUE(dmesgpb.sourceidentifier() == "node1");
  EXPECT_TRUE(dmesgpb.type() == dmn::DMesgTypePb::message);
  EXPECT_TRUE(dmesgpb.body().message() == "hello dmesg");

  dmn::DMesgPb dmesgpb2{};
  DMESG_PB_SET_MSG_TIMESTAMP_FROM_TV(dmesgpb2, tv);
  DMESG_PB_SET_MSG_TOPIC(dmesgpb2, "id2");
  DMESG_PB_SET_MSG_RUNNINGCOUNTER(dmesgpb2, 100);
  DMESG_PB_SET_MSG_SOURCEIDENTIFIER(dmesgpb2, "node2");
  DMESG_PB_SET_MSG_TYPE(dmesgpb2, dmn::DMesgTypePb::sys);

  EXPECT_TRUE(dmesgpb2.timestamp().seconds() != 0);
  EXPECT_TRUE(dmesgpb2.timestamp().nanos() != 0);
  EXPECT_TRUE(dmesgpb2.topic() == "id2");
  EXPECT_TRUE(dmesgpb2.runningcounter() == 100);
  EXPECT_TRUE(dmesgpb2.sourceidentifier() == "node2");
  EXPECT_TRUE(dmesgpb2.type() == dmn::DMesgTypePb::sys);

  auto *self = dmesgpb2.mutable_body()->mutable_sys()->mutable_self();
  DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP_FROM_TV(self, tv);
  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(self, "id4");
  DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::Ready);
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "id5");

  EXPECT_TRUE(self->mutable_initializedtimestamp()->seconds() != 0);
  EXPECT_TRUE(self->mutable_initializedtimestamp()->nanos() != 0);
  EXPECT_TRUE(self->mutable_updatedtimestamp()->seconds() != 0);
  EXPECT_TRUE(self->mutable_updatedtimestamp()->nanos() != 0);
  EXPECT_TRUE(self->state() == dmn::DMesgStatePb::Ready);
  EXPECT_TRUE(self->identifier() == "id4");
  EXPECT_TRUE(self->masteridentifier() == "id5");

  auto *item1 = dmesgpb2.mutable_body()->mutable_sys()->add_nodelist();
  DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP_FROM_TV(item1, tv);
  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(item1, tv);
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(item1, "id10");
  DMESG_PB_SYS_NODE_SET_STATE(item1, dmn::DMesgStatePb::MasterPending);
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(item1, "id15");

  auto *item2 = dmesgpb2.mutable_body()->mutable_sys()->add_nodelist();
  DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP_FROM_TV(item2, tv);
  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(item2, tv);
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(item2, "id20");
  DMESG_PB_SYS_NODE_SET_STATE(item2, dmn::DMesgStatePb::Ready);
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(item2, "id25");

  EXPECT_TRUE(dmesgpb2.body().sys().nodelist().size() == 2);

  EXPECT_TRUE(dmesgpb2.body()
                  .sys()
                  .nodelist()
                  .Get(0)
                  .initializedtimestamp()
                  .seconds() != 0);
  EXPECT_TRUE(
      dmesgpb2.body().sys().nodelist().Get(0).initializedtimestamp().nanos() !=
      0);
  EXPECT_TRUE(dmesgpb2.body().sys().nodelist().Get(0).identifier() == "id10");
  EXPECT_TRUE(dmesgpb2.body().sys().nodelist().Get(0).masteridentifier() ==
              "id15");
  EXPECT_TRUE(dmesgpb2.body().sys().nodelist().Get(0).state() ==
              dmn::DMesgStatePb::MasterPending);

  EXPECT_TRUE(dmesgpb2.body()
                  .sys()
                  .nodelist()
                  .Get(1)
                  .initializedtimestamp()
                  .seconds() != 0);
  EXPECT_TRUE(
      dmesgpb2.body().sys().nodelist().Get(1).initializedtimestamp().nanos() !=
      0);
  EXPECT_TRUE(dmesgpb2.body().sys().nodelist().Get(1).identifier() == "id20");
  EXPECT_TRUE(dmesgpb2.body().sys().nodelist().Get(1).masteridentifier() ==
              "id25");
  EXPECT_TRUE(dmesgpb2.body().sys().nodelist().Get(1).state() ==
              dmn::DMesgStatePb::Ready);

  return RUN_ALL_TESTS();
}
