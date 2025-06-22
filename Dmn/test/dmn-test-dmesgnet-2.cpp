/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test programs asserts that the Dmn_DMesgNet object does
 * send out sys message and self-proclaim as a master node by
 * set up another Dmn_Socket that receives data at the particular
 * ip and port that the Dmn_DMesgNet object send out message through
 * its outbound Dmn_Io object.
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

  dmn::DMesgPb dmesgpb_last{};
  std::unique_ptr<dmn::Dmn_Io<std::string>> read_socket_1 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000);
  dmn::Dmn_Proc read_proc{
      "read_socket_1", [&read_socket_1, &dmesgpb_last]() mutable {
        while (true) {
          auto data = read_socket_1->read();
          if (data) {
            dmn::DMesgPb dmesgpb_read{};

            dmesgpb_read.ParseFromString(*data);
            dmesgpb_last = dmesgpb_read;
            std::cout << "DMesgPb: " << dmesgpb_read.ShortDebugString() << "\n";
          } else {
            break;
          }
        }
      }};

  read_proc.exec();
  dmn::Dmn_Proc::yield();

  std::unique_ptr<dmn::Dmn_Io<std::string>> write_socket_1 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000, true);

  std::unique_ptr<dmn::Dmn_Io<std::string>> read_socket_2 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5001);
  dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", std::move(read_socket_2),
                              std::move(write_socket_1)};
  read_socket_2.reset();
  write_socket_1.reset();

  std::this_thread::sleep_for(std::chrono::seconds(10));
  dmesgnet1.waitForEmpty();
  EXPECT_TRUE(dmesgpb_last.body().sys().self().masteridentifier() != "");
  EXPECT_TRUE(dmesgpb_last.body().sys().self().state() ==
              dmn::DMesgStatePb::Ready);

  return RUN_ALL_TESTS();
}
