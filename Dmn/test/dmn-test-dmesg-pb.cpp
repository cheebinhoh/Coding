/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "proto/dmn-dmesg.pb.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sys/time.h>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
 
  Dmn::DMesgPb dmesgPb{};
  dmesgPb.set_topic("id1");
  dmesgPb.set_runningcounter(99);
  dmesgPb.set_sourceidentifier("cheebin th");
  dmesgPb.set_type(Dmn::DMesgTypePb::message);

  Dmn::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message("message string");

  EXPECT_TRUE(dmesgPb.topic() == "id1");
  EXPECT_TRUE(dmesgPb.runningcounter() == 99);
  EXPECT_TRUE(dmesgPb.sourceidentifier() == "cheebin th");
  EXPECT_TRUE(dmesgPb.type() == Dmn::DMesgTypePb::message);
  EXPECT_TRUE(dmesgPb.body().has_message());

  Dmn::DMesgPb dmesgDefaultPb{};
  EXPECT_TRUE(!dmesgDefaultPb.body().has_message());

  Dmn::DMesgPb dmesgSysPb{};
  dmesgSysPb.set_type(Dmn::DMesgTypePb::sys);
  Dmn::DMesgBodyPb *dmesgSysBodyPb = dmesgSysPb.mutable_body();
  Dmn::DMesgSysPb *dmesgSysBodySysPb = dmesgSysBodyPb->mutable_sys();

  struct timeval tv;
  gettimeofday(&tv, NULL);
  dmesgSysBodySysPb->mutable_timestamp()->set_seconds(tv.tv_sec);
  dmesgSysBodySysPb->mutable_timestamp()->set_nanos(tv.tv_usec * 1000);

  Dmn::DMesgSysNodePb *self = dmesgSysBodySysPb->mutable_self();
  self->set_state(Dmn::DMesgStatePb::MasterPending);
  self->set_identifier("id2");
  self->mutable_initializedtimestamp()->set_seconds(tv.tv_sec);
  self->mutable_initializedtimestamp()->set_nanos(tv.tv_usec * 1000);

  auto neightborNode1 = dmesgSysBodySysPb->add_nodelist();
  neightborNode1->set_state(Dmn::DMesgStatePb::Ready);
  neightborNode1->set_identifier("id3");
  neightborNode1->set_masteridentifier("id3");
  neightborNode1->mutable_initializedtimestamp()->set_seconds(tv.tv_sec);
  neightborNode1->mutable_initializedtimestamp()->set_nanos(tv.tv_usec * 1000);

  auto neightborNode2 = dmesgSysBodySysPb->add_nodelist();
  neightborNode2->set_state(Dmn::DMesgStatePb::Ready);
  neightborNode2->set_identifier("id4");
  neightborNode2->set_masteridentifier("id3");
  neightborNode2->mutable_initializedtimestamp()->set_seconds(tv.tv_sec);
  neightborNode1->mutable_initializedtimestamp()->set_nanos(tv.tv_usec * 1000);

  EXPECT_TRUE(dmesgSysPb.type() == Dmn::DMesgTypePb::sys);
  EXPECT_TRUE(dmesgSysPb.body().has_sys());
  EXPECT_TRUE(!dmesgSysPb.body().has_message());
  EXPECT_TRUE(dmesgSysPb.body().sys().nodelist().size() == 2);
  EXPECT_TRUE(dmesgSysPb.body().sys().nodelist().Get(0).identifier() == "id3");
  EXPECT_TRUE(dmesgSysPb.body().sys().nodelist().Get(0).masteridentifier() == "id3");
  EXPECT_TRUE(dmesgSysPb.body().sys().nodelist().Get(0).state() == Dmn::DMesgStatePb::Ready);

  return RUN_ALL_TESTS();
}
