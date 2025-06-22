/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that two subscribers of the same
 * Dmn_DMesg object will receive the same DMesgPb message
 * published by publisher of the Dmn_DMesg object.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "dmn-dmesg.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  dmn::Dmn_DMesg dmesg{"dmesg"};
  std::string data1{};
  std::string data2{};

  auto dmesg_handle1 =
      dmesg.openHandler("handler1", nullptr, [&data1](dmn::DMesgPb dmesgpb) {
        data1 = dmesgpb.body().message();
      });
  EXPECT_TRUE(dmesg_handle1);

  auto dmesg_handle2 =
      dmesg.openHandler("handler2", nullptr, [&data2](dmn::DMesgPb dmesgpb) {
        data2 = dmesgpb.body().message();
      });

  EXPECT_TRUE(dmesg_handle2);

  auto dmesg_handle3 = dmesg.openHandler("handler3");
  EXPECT_TRUE(dmesg_handle3);

  dmn::DMesgPb dmesgpb{};
  dmesgpb.set_topic("counter sync");
  dmesgpb.set_type(dmn::DMesgTypePb::message);

  std::string data{"Hello dmesg async"};
  dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
  dmesgpb_body->set_message(data);

  dmesg_handle3->write(dmesgpb);

  std::this_thread::sleep_for(std::chrono::seconds(5));

  std::cout << "after wait for data to sync\n";

  dmesg.waitForEmpty();
  dmesg_handle1->waitForEmpty();
  dmesg_handle2->waitForEmpty();
  dmesg_handle3->waitForEmpty();

  EXPECT_TRUE(data == data1);
  EXPECT_TRUE(data == data2);

  std::string data3{};

  auto dmesg_handle4 =
      dmesg.openHandler("handler4", nullptr, [&data3](dmn::DMesgPb dmesgpb) {
        data3 = dmesgpb.body().message();
      });

  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(data == data3);

  dmesg.closeHandler(dmesg_handle1);
  dmesg.closeHandler(dmesg_handle2);
  dmesg.closeHandler(dmesg_handle3);
  dmesg.closeHandler(dmesg_handle4);

  return RUN_ALL_TESTS();
}
