/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that two Dmn_DMesgNet objects that
 * participates in the same network through its inbound and outbound
 * Dmn_Io objects are in sync in the Dmn network, and message sent priorly
 * in one Dmn_DMesgNet will be resent by the master when a new
 * Dmn_DMesgNet object joins the same Dmn network.
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

  std::unique_ptr<dmn::Dmn_Io<std::string>> read_socket_1 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5001);
  std::unique_ptr<dmn::Dmn_Io<std::string>> write_socket_1 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000, true);

  bool read_data{};
  dmn::DMesgPb dmesgpb_read{};
  dmn::Dmn_DMesgNet dmesgnet1{"dmesg-1", std::move(read_socket_1),
                              std::move(write_socket_1)};
  read_socket_1.reset();
  write_socket_1.reset();

  auto write_handle_1 = dmesgnet1.openHandler(
      "dmesg-1-handler", "counter sync", nullptr,
      [&dmesgpb_read, &read_data](dmn::DMesgPb data) mutable {
        read_data = true;
        dmesgpb_read = data;
      });

  dmn::DMesgPb dmesgpb{};
  dmesgpb.set_type(dmn::DMesgTypePb::message);
  dmesgpb.set_sourceidentifier("handler-1");
  std::string data{"Hello dmesg async"};
  dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
  dmesgpb_body->set_message(data);
  write_handle_1->write(dmesgpb);

  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(!read_data);

  std::unique_ptr<dmn::Dmn_Io<std::string>> read_socket_2 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000);
  std::unique_ptr<dmn::Dmn_Io<std::string>> write_socket_2 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5001, true);

  dmn::Dmn_DMesgNet dmesgnet2{"dmesg-2", std::move(read_socket_2),
                              std::move(write_socket_2)};
  read_socket_2.reset();
  write_socket_2.reset();

  dmn::DMesgPb dmesgpb_read_2{};
  auto read_handle_2 = dmesgnet2.openHandler(
      "dmesg-2-handler", "counter sync", nullptr,
      [&dmesgpb_read_2, &read_data](dmn::DMesgPb data) mutable {
        read_data = true;
        dmesgpb_read_2 = data;
      });

  std::this_thread::sleep_for(std::chrono::seconds(10));
  EXPECT_TRUE(read_data);
  EXPECT_TRUE(dmesgpb_read_2.body().message() == dmesgpb.body().message());

  dmesgnet2.closeHandler(read_handle_2);
  dmesgnet1.closeHandler(write_handle_1);

  return RUN_ALL_TESTS();
}
