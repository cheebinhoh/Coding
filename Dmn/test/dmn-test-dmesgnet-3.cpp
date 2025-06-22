/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that two Dmn_DMesgNet objects that
 * participates in the same network through its inbound and outbound
 * Dmn_Io objects are sync in their sys message and agree on the
 * master node through the Dmn_DMesgNet algorithm.
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

  dmn::DMesgPb dmesgpb_sys_3{};
  dmn::Dmn_DMesgNet dmesgnet1{"dmesg-3", std::move(read_socket_1),
                              std::move(write_socket_1)};
  read_socket_1.reset();
  write_socket_1.reset();

  auto configs = dmn::Dmn_DMesg::kHandlerConfig_Default;
  configs[dmn::Dmn_DMesg::kHandlerConfig_IncludeSys] = "yes";

  auto listen_handle_3 = dmesgnet1.openHandler(
      "dmesg-3-listen", nullptr,
      [&dmesgpb_sys_3](dmn::DMesgPb data) mutable {
        if (data.type() == dmn::DMesgTypePb::sys) {
          dmesgpb_sys_3 = data;
        }
      },
      configs);

  dmn::DMesgPb dmesgpb_sys_4{};

  std::this_thread::sleep_for(std::chrono::seconds(3));
  dmn::Dmn_Proc dmesg_4_proc{
      "dmesg_4_proc", [&dmesgpb_sys_4]() mutable {
        std::unique_ptr<dmn::Dmn_Io<std::string>> read_socket_1 =
            std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000);
        std::unique_ptr<dmn::Dmn_Io<std::string>> write_socket_1 =
            std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5001, true);

        dmn::Dmn_DMesgNet dmesgnet1{"dmesg-4", std::move(read_socket_1),
                                    std::move(write_socket_1)};
        read_socket_1.reset();
        write_socket_1.reset();

        auto configs = dmn::Dmn_DMesg::kHandlerConfig_Default;
        configs[dmn::Dmn_DMesg::kHandlerConfig_IncludeSys] = "yes";

        auto listen_handle_4 = dmesgnet1.openHandler(
            "dmesg-4-listen", nullptr,
            [&dmesgpb_sys_4](dmn::DMesgPb data) mutable {
              if (data.type() == dmn::DMesgTypePb::sys) {
                dmesgpb_sys_4 = data;
              }
            },
            configs);

        std::this_thread::sleep_for(std::chrono::seconds(7));
        dmesgnet1.waitForEmpty();
      }};

  dmesg_4_proc.exec();

  dmesg_4_proc.wait();
  std::this_thread::sleep_for(std::chrono::seconds(7));

  EXPECT_TRUE(dmesgpb_sys_3.body().sys().self().identifier() == "dmesg-3");
  EXPECT_TRUE(dmesgpb_sys_3.body().sys().self().masteridentifier() ==
              "dmesg-3");
  EXPECT_TRUE(dmesgpb_sys_4.body().sys().self().identifier() == "dmesg-4");
  EXPECT_TRUE(dmesgpb_sys_4.body().sys().self().masteridentifier() ==
              "dmesg-3");

  return RUN_ALL_TESTS();
}
