/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test programs asserts that two Dmn_DMesgNet objects can
 * one send message through a Dmn_Socket at a particular ip and port
 * and another one receive sent message through another Dmn_Socket
 * at the same ip and port.
 */

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include "dmn-dmesgnet.hpp"
#include "dmn-socket.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  std::unique_ptr<dmn::Dmn_Io<std::string>> write_socket_1 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000, true);
  std::unique_ptr<dmn::Dmn_Io<std::string>> read_socket_2 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000);

  dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", nullptr, std::move(write_socket_1)};
  write_socket_1.reset();

  dmn::Dmn_DMesgNet dmesgnet2{"dmesg2", std::move(read_socket_2)};
  read_socket_2.reset();

  auto readHandler2 = dmesgnet2.openHandler("dmesg2.readHandler");

  dmn::DMesgPb dmesgpb_read{};
  dmn::Dmn_Proc proc2{"dmesg2", [readHandler2, &dmesgpb_read]() {
                        auto data = readHandler2->read();
                        if (data) {
                          dmesgpb_read = *data;
                        }
                      }};

  proc2.exec();
  dmn::Dmn_Proc::yield();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto dmesg_handle = dmesgnet1.openHandler("writeHandler");
  EXPECT_TRUE(dmesg_handle);

  dmn::DMesgPb dmesgpb{};
  dmesgpb.set_topic("counter sync");
  dmesgpb.set_type(dmn::DMesgTypePb::message);
  dmesgpb.set_sourceidentifier("writehandler");

  std::string data{"Hello dmesg async"};
  dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
  dmesgpb_body->set_message(data);

  dmesg_handle->write(dmesgpb);

  std::this_thread::sleep_for(std::chrono::seconds(5));

  std::string source = dmesgpb_read.sourceidentifier();
  EXPECT_TRUE(dmesgpb_read.type() == dmesgpb.type());
  EXPECT_TRUE(dmesgpb_read.sourceidentifier() ==
              dmesgpb.sourceidentifier()); // the source is the local DmesgNet
                                           // agent that read
  EXPECT_TRUE(dmesgpb_read.body().message() == dmesgpb.body().message());

  std::this_thread::sleep_for(std::chrono::seconds(5));

  return RUN_ALL_TESTS();
}
