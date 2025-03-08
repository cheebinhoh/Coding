/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-dmesg.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  Hal::Hal_DMesg dmesg{"dmesg"};
  
  auto dmesgHandler1 = dmesg.openHandler("handler1");
  EXPECT_TRUE(dmesgHandler1);

  auto dmesgHandler2 = dmesg.openHandler("handler2");
  EXPECT_TRUE(dmesgHandler2);

  Hal::Hal_Proc proc1{"proc1", [&dmesgHandler1]() {
    int valCheck{1};

    while (true) {
      auto dmesgPb = dmesgHandler1->read();
      if (!dmesgPb) {
        break;
      }

      assert(dmesgPb->type() == Hal::DMesgTypePb::message);
      if (dmesgPb->body().message() == "") {
        break;
      }

      std::cout << "proc1: message: " << dmesgPb->body().message() << "\n";

      std::stringstream is{dmesgPb->body().message()};
      int val{};
      
      is >> val;
      EXPECT_TRUE(!is.fail());
      EXPECT_TRUE(val == valCheck);
   
      val++;

      std::stringstream os;
      os << val;

      Hal::DMesgPb dmesgPbRet{};
      dmesgPbRet.set_topic("counter sync");
      dmesgPbRet.set_type(Hal::DMesgTypePb::message);

      Hal::DMesgBodyPb *dmsgbodyPbRet = dmesgPbRet.mutable_body();
      dmsgbodyPbRet->set_message(os.str());

      dmesgHandler1->write(dmesgPbRet);
 
      valCheck += 2;
    }
  }};

  Hal::Hal_Proc proc2{"proc2", [&dmesgHandler2]() {
    int val{1};

    while (true) {
      std::stringstream os{};

      if (val <= 10) {
        os << val;
      } else {
        os << "";
      }

      Hal::DMesgPb dmesgPb{};
      dmesgPb.set_topic("counter sync");
      dmesgPb.set_type(Hal::DMesgTypePb::message);

      Hal::DMesgBodyPb *dmsgbodyPb = dmesgPb.mutable_body();
      dmsgbodyPb->set_message(os.str());

      dmesgHandler2->write(dmesgPb);

      if (os.str() == "") {
        break;
      }

      auto dmesgPbRet = dmesgHandler2->read();
      if (!dmesgPbRet) {
        break;
      }

      assert(dmesgPbRet->type() == Hal::DMesgTypePb::message);
      assert(dmesgPbRet->body().message() != "");

      std::cout << "proc2: message: " << dmesgPbRet->body().message() << "\n";

      int valRet{};

      std::stringstream is{dmesgPbRet->body().message()};
      is >> valRet;
 
      EXPECT_TRUE(valRet == (val + 1));
      val += 2;
    }
  }};

  proc2.exec();
  proc1.exec();

  proc2.wait();
  proc1.wait();

  dmesg.closeHandler(dmesgHandler1);
  dmesg.closeHandler(dmesgHandler2);
 
  return RUN_ALL_TESTS();
}
