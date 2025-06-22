/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-test-dmesg-7.cpp
 * @brief The unit test that asserts that the dmn-dmesg with messages left in
 *        the handler will be closed properly.
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

  dmn::Dmn_Proc::yield();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "after sleep 3 seconds\n";

  dmesg_write_handle->write(dmesgpb1);
  dmesg_write_handle->write(dmesgpb2);

  dmesg.waitForEmpty();

  std::cout << "after sleep 5 seconds\n";
  std::this_thread::sleep_for(std::chrono::seconds(5));

  dmesg.closeHandler(dmesg_write_handle);

  return RUN_ALL_TESTS();
}
