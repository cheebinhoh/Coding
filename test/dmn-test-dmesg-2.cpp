/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-test-dmesg-2.cpp
 * @brief The unit test that asserts that the dmn-dmesg with one publisher and
 *        one subscriber are able to continuing read and write multiple messages
 *        between them.
 */

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <thread>

#include "dmn-dmesg.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  dmn::Dmn_DMesg dmesg{"dmesg"};

  auto dmesg_handle1 = dmesg.openHandler("handler1");
  EXPECT_TRUE(dmesg_handle1);

  auto dmesg_handle2 = dmesg.openHandler("handler2");
  EXPECT_TRUE(dmesg_handle2);

  dmn::Dmn_Proc proc1{
      "proc1", [&dmesg_handle1]() {
        while (true) {
          auto dmesgpb = dmesg_handle1->read();
          if (!dmesgpb) {
            break;
          }

          assert(dmesgpb->type() == dmn::DMesgTypePb::message);
          if (dmesgpb->body().message() == "") {
            break;
          }

          std::cout << "proc1: message: " << dmesgpb->body().message() << "\n";

          std::stringstream is{dmesgpb->body().message()};
          int val{};

          is >> val;
          EXPECT_TRUE(!is.fail());
          EXPECT_TRUE(val == val);

          val++;

          std::stringstream os;
          os << val;

          dmn::DMesgPb dmesgpb_ret{};
          dmesgpb_ret.set_topic("counter sync");
          dmesgpb_ret.set_type(dmn::DMesgTypePb::message);

          dmn::DMesgBodyPb *dmesgpb_body_ret = dmesgpb_ret.mutable_body();
          dmesgpb_body_ret->set_message(os.str());

          dmesg_handle1->write(dmesgpb_ret);

          val += 2;
        }
      }};

  dmn::Dmn_Proc proc2{
      "proc2", [&dmesg_handle2]() {
        int val{1};

        while (true) {
          std::stringstream os{};

          if (val <= 10) {
            os << val;
          } else {
            os << "";
          }

          dmn::DMesgPb dmesgpb{};
          dmesgpb.set_topic("counter sync");
          dmesgpb.set_type(dmn::DMesgTypePb::message);

          dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
          dmesgpb_body->set_message(os.str());
          dmesg_handle2->write(dmesgpb);

          if (os.str() == "") {
            break;
          }

          auto dmesgpb_ret = dmesg_handle2->read();
          if (!dmesgpb_ret) {
            break;
          }

          assert(dmesgpb_ret->type() == dmn::DMesgTypePb::message);
          assert(dmesgpb_ret->body().message() != "");

          std::cout << "proc2: message: " << dmesgpb_ret->body().message()
                    << "\n";

          int val_ret{};

          std::stringstream is{dmesgpb_ret->body().message()};
          is >> val_ret;

          EXPECT_TRUE(val_ret == (val + 1));
          val += 2;
        }
      }};

  proc2.exec();
  proc1.exec();

  proc2.wait();
  proc1.wait();

  dmesg.closeHandler(dmesg_handle1);
  dmesg.closeHandler(dmesg_handle2);

  return RUN_ALL_TESTS();
}
