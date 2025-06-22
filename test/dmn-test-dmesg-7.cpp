/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-test-dmesg-7.cpp
 * @brief The unit test that asserts that the dmn-dmesg with two subscribers of
 *        the same Dmn_DMesg object will receive different message filtered by
 *        topic.
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
  auto topic1_write_handler =
      dmesg.openHandler("topic1_write_handler", "topic1");
  auto topic2_write_handler =
      dmesg.openHandler("topic2_write_handler", "topic2");

  dmn::DMesgPb dmesgpb_topic1_read{};
  auto topic1_read_handler =
      dmesg.openHandler("topic1_read_handler", "topic1", nullptr,
                        [&dmesgpb_topic1_read](dmn::DMesgPb dmesgpb) {
                          std::cout << "reat topic1\n";
                          dmesgpb_topic1_read = dmesgpb;
                        });

  dmn::DMesgPb dmesgpb_topic2_read{};
  auto topic2_read_handler =
      dmesg.openHandler("topic2_read_handler", "topic2", nullptr,
                        [&dmesgpb_topic2_read](dmn::DMesgPb dmesgpb) {
                          std::cout << "reat topic2\n";
                          dmesgpb_topic2_read = dmesgpb;
                        });
  dmn::DMesgPb dmesgpb{};
  dmesgpb.set_type(dmn::DMesgTypePb::message);

  std::string data1{"Hello topic1"};
  dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
  dmesgpb_body->set_message(data1);

  topic1_write_handler->write(dmesgpb);

  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE("topic1" == dmesgpb_topic1_read.topic());
  EXPECT_TRUE("Hello topic1" == dmesgpb_topic1_read.body().message());
  EXPECT_TRUE("" == dmesgpb_topic2_read.body().message());

  dmn::DMesgPb dmesgpb2{};
  dmesgpb2.set_type(dmn::DMesgTypePb::message);

  std::string data2{"Hello topic2"};
  dmn::DMesgBodyPb *dmesgpb_body2 = dmesgpb2.mutable_body();
  dmesgpb_body2->set_message(data2);

  topic2_write_handler->write(dmesgpb2);

  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE("topic1" == dmesgpb_topic1_read.topic());
  EXPECT_TRUE("Hello topic1" == dmesgpb_topic1_read.body().message());

  EXPECT_TRUE("topic2" == dmesgpb_topic2_read.topic());
  EXPECT_TRUE("Hello topic2" == dmesgpb_topic2_read.body().message());

  return RUN_ALL_TESTS();
}
