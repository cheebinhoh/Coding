/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesg.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  Dmn::Dmn_DMesg dmesg{"dmesg"};
  std::string data1{};
  std::string data2{};

  auto dmesgHandler1 = dmesg.openHandler("handler1", nullptr,
                                         [&data1](Dmn::DMesgPb dmesgPb) {
                                           data1 = dmesgPb.body().message();
                                         });
  EXPECT_TRUE(dmesgHandler1);

  auto dmesgHandler2 = dmesg.openHandler("handler2", nullptr,
                                         [&data2](Dmn::DMesgPb dmesgPb) {
                                           data2 = dmesgPb.body().message();
                                         });
 
  EXPECT_TRUE(dmesgHandler2);

  auto dmesgHandler3 = dmesg.openHandler("handler3", nullptr, nullptr);
  EXPECT_TRUE(dmesgHandler3);
 
  Dmn::DMesgPb dmesgPb{};
  dmesgPb.set_topic("counter sync");
  dmesgPb.set_type(Dmn::DMesgTypePb::message);

  std::string data{"Hello dmesg async"};
  Dmn::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
  dmsgbodyPb->set_message(data);

  dmesgHandler3->write(dmesgPb);

  std::this_thread::sleep_for(std::chrono::seconds(5));
 
  std::cout << "after wait for data to sync\n";

  dmesg.waitForEmpty();
  dmesgHandler1->waitForEmpty();
  dmesgHandler2->waitForEmpty();
  dmesgHandler3->waitForEmpty();
  dmesg.closeHandler(dmesgHandler1);
  dmesg.closeHandler(dmesgHandler2);
  dmesg.closeHandler(dmesgHandler3);

  EXPECT_TRUE(data == data1);
  EXPECT_TRUE(data == data2);
 
  return RUN_ALL_TESTS();
}
