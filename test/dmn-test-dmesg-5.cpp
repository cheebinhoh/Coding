/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that the subscriber can subscribe to
 * certain topic of the Dmn_DMesg object.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "dmn-dmesg.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  dmn::Dmn_DMesg dmesg{"dmesg"};

  std::vector<std::string> topics{"counter sync 1", "counter sync 2"};
  std::string subscribed_topic{"counter sync 1"};

  int cnt{0};
  std::shared_ptr<dmn::Dmn_DMesg::Dmn_DMesgHandler> dmesg_handler =
      dmesg.openHandler("handler", subscribed_topic, nullptr,
                        [&cnt](const dmn::DMesgPb &msg) mutable {
                          EXPECT_TRUE("counter sync 1" == msg.topic());
                          cnt++;
                        });
  EXPECT_TRUE(dmesg_handler);

  auto dmesg_write_handler1 = dmesg.openHandler("writeHandler", topics[0]);
  EXPECT_TRUE(dmesg_write_handler1);

  auto dmesg_write_handler2 = dmesg.openHandler("writeHandler", topics[1]);
  EXPECT_TRUE(dmesg_write_handler2);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  for (int n = 0; n < 6; n++) {
    dmn::DMesgPb dmesgpb{};
    dmesgpb.set_type(dmn::DMesgTypePb::message);

    std::string data{"Hello dmesg async"};
    dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
    dmesgpb_body->set_message(data);

    if ((n % 2) == 0) {
      dmesg_write_handler1->write(dmesgpb);
    } else {
      dmesg_write_handler2->write(dmesgpb);
    }
  }

  std::this_thread::sleep_for(std::chrono::seconds(8));

  dmesg.closeHandler(dmesg_write_handler2);
  dmesg.closeHandler(dmesg_write_handler1);
  dmesg.closeHandler(dmesg_handler);
  EXPECT_TRUE(3 == cnt);

  return RUN_ALL_TESTS();
}
