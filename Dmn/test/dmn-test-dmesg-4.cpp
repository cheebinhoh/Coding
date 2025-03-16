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

  int cnt{1};

  std::shared_ptr<Dmn::Dmn_DMesg::Dmn_DMesgHandler> dmesgHandler = dmesg.openHandler("handler", false,
                                                                                     [](const Dmn::DMesgPb &msg) {
                                                                                       return true;
                                                                                     },
                                                                                     [&dmesgHandler, &cnt](const Dmn::DMesgPb &msg) mutable {
                                                                                       std::cout << msg.ShortDebugString() << "\n";

                                                                                       Dmn::DMesgPb ret{msg};
                                                                                       try {
                                                                                         dmesgHandler->write(ret);
                                                                                         cnt++;
                                                                                       } catch (...) {
                                                                                         std::cout << "except cnt: " << cnt << "\n";
                                                                                         dmesgHandler->resolveConflict();
                                                                                       }
                                                                                     });
  EXPECT_TRUE(dmesgHandler);

  auto dmesgWriteHandler = dmesg.openHandler("writeHandler");
  EXPECT_TRUE(dmesgWriteHandler);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  for (int n = 0; n < 3; n++) {
    Dmn::DMesgPb dmesgPb{};
    dmesgPb.set_topic("counter sync");
    dmesgPb.set_type(Dmn::DMesgTypePb::message);

    std::string data{"Hello dmesg async"};
    Dmn::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
    dmsgbodyPb->set_message(data);

    dmesgWriteHandler->write(dmesgPb);
  }

  std::this_thread::sleep_for(std::chrono::seconds(8));

  dmesg.closeHandler(dmesgWriteHandler);
  dmesg.closeHandler(dmesgHandler);
  EXPECT_TRUE(2 == cnt);

  return RUN_ALL_TESTS();
}
