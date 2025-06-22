/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include <gtest/gtest.h>

#include <sys/time.h>

#include <iostream>

#include "proto/dmn-dmesg.pb.h"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  dmn::DMesgPb dmesgpb{};
  dmesgpb.set_topic("id1");
  dmesgpb.set_runningcounter(99);
  dmesgpb.set_sourceidentifier("cheebin th");
  dmesgpb.set_type(dmn::DMesgTypePb::message);

  dmn::DMesgBodyPb *dmsgbodyPb = dmesgpb.mutable_body();
  dmsgbodyPb->set_message("message string");

  EXPECT_TRUE(dmesgpb.topic() == "id1");
  EXPECT_TRUE(dmesgpb.runningcounter() == 99);
  EXPECT_TRUE(dmesgpb.sourceidentifier() == "cheebin th");
  EXPECT_TRUE(dmesgpb.type() == dmn::DMesgTypePb::message);
  EXPECT_TRUE(dmesgpb.body().has_message());

  dmn::DMesgPb dmesgpb_default{};
  EXPECT_TRUE(!dmesgpb_default.body().has_message());

  dmn::DMesgPb dmesgpb_sys{};
  dmesgpb_sys.set_type(dmn::DMesgTypePb::sys);
  dmn::DMesgBodyPb *dmesgpb_sys_body = dmesgpb_sys.mutable_body();
  dmn::DMesgSysPb *dmesgpb_sys_body_sys = dmesgpb_sys_body->mutable_sys();

  struct timeval tv;
  gettimeofday(&tv, NULL);
  dmesgpb_sys_body_sys->mutable_timestamp()->set_seconds(tv.tv_sec);
  dmesgpb_sys_body_sys->mutable_timestamp()->set_nanos(tv.tv_usec * 1000);

  dmn::DMesgSysNodePb *self = dmesgpb_sys_body_sys->mutable_self();
  self->set_state(dmn::DMesgStatePb::MasterPending);
  self->set_identifier("id2");
  self->mutable_initializedtimestamp()->set_seconds(tv.tv_sec);
  self->mutable_initializedtimestamp()->set_nanos(tv.tv_usec * 1000);

  auto neightbor_node1 = dmesgpb_sys_body_sys->add_nodelist();
  neightbor_node1->set_state(dmn::DMesgStatePb::Ready);
  neightbor_node1->set_identifier("id3");
  neightbor_node1->set_masteridentifier("id3");
  neightbor_node1->mutable_initializedtimestamp()->set_seconds(tv.tv_sec);
  neightbor_node1->mutable_initializedtimestamp()->set_nanos(tv.tv_usec * 1000);

  auto neightbor_node2 = dmesgpb_sys_body_sys->add_nodelist();
  neightbor_node2->set_state(dmn::DMesgStatePb::Ready);
  neightbor_node2->set_identifier("id4");
  neightbor_node2->set_masteridentifier("id3");
  neightbor_node2->mutable_initializedtimestamp()->set_seconds(tv.tv_sec);
  neightbor_node1->mutable_initializedtimestamp()->set_nanos(tv.tv_usec * 1000);

  EXPECT_TRUE(dmesgpb_sys.type() == dmn::DMesgTypePb::sys);
  EXPECT_TRUE(dmesgpb_sys.body().has_sys());
  EXPECT_TRUE(!dmesgpb_sys.body().has_message());
  EXPECT_TRUE(dmesgpb_sys.body().sys().nodelist().size() == 2);
  EXPECT_TRUE(dmesgpb_sys.body().sys().nodelist().Get(0).identifier() == "id3");
  EXPECT_TRUE(dmesgpb_sys.body().sys().nodelist().Get(0).masteridentifier() ==
              "id3");
  EXPECT_TRUE(dmesgpb_sys.body().sys().nodelist().Get(0).state() ==
              dmn::DMesgStatePb::Ready);

  return RUN_ALL_TESTS();
}
