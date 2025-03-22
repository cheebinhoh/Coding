/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesg.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  Dmn::Dmn_DMesg dmesg{"dmesg"};

  std::vector<std::string> topics{"counter sync 1", "counter sync 2"};
  std::vector<std::string> subscribedTopics{"counter sync 1"};

  int cnt{0};
  std::shared_ptr<Dmn::Dmn_DMesg::Dmn_DMesgHandler> dmesgHandler = dmesg.openHandler(subscribedTopics, "handler", false, nullptr,
                                                                                     [&cnt](const Dmn::DMesgPb &msg) mutable {
                                                                                        EXPECT_TRUE("counter sync 1" == msg.topic());
                                                                                        cnt++;
                                                                                     });
  EXPECT_TRUE(dmesgHandler);

  auto dmesgWriteHandler = dmesg.openHandler("writeHandler");
  EXPECT_TRUE(dmesgWriteHandler);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  for (int n = 0; n < 6; n++) {
    Dmn::DMesgPb dmesgPb{};
    dmesgPb.set_topic(topics[n % 2]);
    dmesgPb.set_type(Dmn::DMesgTypePb::message);

    std::string data{"Hello dmesg async"};
    Dmn::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
    dmsgbodyPb->set_message(data);

    dmesgWriteHandler->write(dmesgPb);
  }

  std::this_thread::sleep_for(std::chrono::seconds(8));

  dmesg.closeHandler(dmesgWriteHandler);
  dmesg.closeHandler(dmesgHandler);
  EXPECT_TRUE(3 == cnt);

  return RUN_ALL_TESTS();
}
