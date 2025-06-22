/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that two publishers writes to the
 * same Dmn_DMesg object, first publisher will continue to write
 * to the Dmn_DMesg object 3 messages, where the 2nd publisher
 * will in respond to message written by first publisher and
 * write the same message back to th Dmn_DMesg and which results
 * in a conflict, the write back always fail besides the last
 * message written back.
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

  int cnt{1};

  std::shared_ptr<dmn::Dmn_DMesg::Dmn_DMesgHandler> dmesg_handle =
      dmesg.openHandler(
          "handler", false, [](const dmn::DMesgPb &msg) { return true; },
          [&dmesg_handle, &cnt](const dmn::DMesgPb &msg) mutable {
            std::cout << msg.ShortDebugString() << "\n";

            dmn::DMesgPb ret{msg};
            try {
              dmesg_handle->write(ret);
              cnt++;
            } catch (...) {
              std::cout << "except cnt: " << cnt << "\n";
              dmesg_handle->resolveConflict();
            }
          });
  EXPECT_TRUE(dmesg_handle);

  auto dmesg_write_handle = dmesg.openHandler("writeHandler");
  EXPECT_TRUE(dmesg_write_handle);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  for (int n = 0; n < 3; n++) {
    dmn::DMesgPb dmesgpb{};
    dmesgpb.set_topic("counter sync");
    dmesgpb.set_type(dmn::DMesgTypePb::message);

    std::string data{"Hello dmesg async"};
    dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
    dmesgpb_body->set_message(data);

    dmesg_write_handle->write(dmesgpb);
  }

  std::this_thread::sleep_for(std::chrono::seconds(8));

  dmesg.closeHandler(dmesg_write_handle);
  dmesg.closeHandler(dmesg_handle);
  EXPECT_TRUE(2 == cnt);

  return RUN_ALL_TESTS();
}
