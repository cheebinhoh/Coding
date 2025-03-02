/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-dmesgnet.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  Hal::Hal_DMesgNet dmesgnet1{"dmesg1"};
  Hal::Hal_DMesgNet dmesgnet2{"dmesg2"};
 
  auto dmesgHandler = dmesgnet1.openHandler("writeHandler", nullptr, nullptr);
  EXPECT_TRUE(dmesgHandler);
 
  Hal::DMesgPb dmesgPb{};
  dmesgPb.set_identifier("counter sync");
  dmesgPb.set_type(Hal::DMesgTypePb::message);

  std::string data{"Hello dmesg async"};
  Hal::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message(data);

  dmesgHandler->write(dmesgPb);

  std::this_thread::sleep_for(std::chrono::seconds(5));
 
  return RUN_ALL_TESTS();
}
