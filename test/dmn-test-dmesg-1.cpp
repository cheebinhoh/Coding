/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-test-dmesg-1.cpp
 * @brief The unit test that asserts that the dmn-dmesg with one publisher and
 *        two subscribers work.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "dmn-dmesg.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  dmn::Dmn_DMesg dmesg{"dmesg"};

  auto dmesg_write_handle = dmesg.openHandler("writeHandler");
  EXPECT_TRUE(dmesg_write_handle);

  auto dmesg_read_handle1 = dmesg.openHandler("readHandler1");

  auto dmesg_read_handle2 = dmesg.openHandler(
      "readHandler2", [](const dmn::DMesgPb &dmesgpb) -> bool {
        return dmesgpb.body().message() != "message string 1";
      });

  dmn::DMesgPb dmesgpb1{};
  dmesgpb1.set_topic("id1");
  dmesgpb1.set_runningcounter(99);
  dmesgpb1.set_sourceidentifier("unknown");
  dmesgpb1.set_type(dmn::DMesgTypePb::message);

  dmn::DMesgBodyPb *dmesgpb_body1 = dmesgpb1.mutable_body();
  dmesgpb_body1->set_message("message string 1");

  dmn::DMesgPb dmesgpb2{};
  dmesgpb2.set_topic("id1");
  dmesgpb2.set_runningcounter(99);
  dmesgpb2.set_sourceidentifier("unknown");
  dmesgpb2.set_type(dmn::DMesgTypePb::message);

  dmn::DMesgBodyPb *dmesgpb_body2 = dmesgpb2.mutable_body();
  dmesgpb_body2->set_message("message string 2");

  dmn::Dmn_Proc proc_read1{
      "read1", [&dmesg_read_handle1, &dmesgpb1]() {
        std::cout << "before read1\n";
        auto dmesgpb_read = dmesg_read_handle1->read();
        std::cout << "after read1, and proceed to validate read\n";
        EXPECT_TRUE(dmesgpb_read->topic() == dmesgpb1.topic());
        EXPECT_TRUE(dmesgpb_read->sourceidentifier() ==
                    dmesgpb1.sourceidentifier());
        EXPECT_TRUE(dmesgpb_read->runningcounter() ==
                    dmesgpb1.runningcounter());
        EXPECT_TRUE(dmesgpb_read->type() == dmesgpb1.type());
        EXPECT_TRUE(dmesgpb_read->body().message() ==
                    dmesgpb1.body().message());
      }};

  dmn::Dmn_Proc proc_read2{
      "read2", [&dmesg_read_handle2, &dmesgpb2]() {
        std::cout << "before read2\n";
        auto dmesgpb_read = dmesg_read_handle2->read();
        std::cout << "after read2, and proceed to validate read\n";
        EXPECT_TRUE(dmesgpb_read->topic() == dmesgpb2.topic());
        EXPECT_TRUE(dmesgpb_read->sourceidentifier() ==
                    dmesgpb2.sourceidentifier());
        EXPECT_TRUE(dmesgpb_read->runningcounter() ==
                    dmesgpb2.runningcounter());
        EXPECT_TRUE(dmesgpb_read->type() == dmesgpb2.type());
        EXPECT_TRUE(dmesgpb_read->body().message() ==
                    dmesgpb2.body().message());
      }};

  proc_read1.exec();
  proc_read2.exec();
  dmn::Dmn_Proc::yield();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "after sleep 3 seconds\n";

  dmesg_write_handle->write(dmesgpb1);
  dmesg_write_handle->write(dmesgpb2);

  dmesg.waitForEmpty();
  dmesg_read_handle1->waitForEmpty();
  dmesg_read_handle2->waitForEmpty();

  std::cout << "after sleep 5 seconds\n";
  std::this_thread::sleep_for(std::chrono::seconds(5));

  dmesg.closeHandler(dmesg_write_handle);
  dmesg.closeHandler(dmesg_read_handle1);
  dmesg.closeHandler(dmesg_read_handle2);

  return RUN_ALL_TESTS();
}
