/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that when two Dmn_DMesgNet objects
 * are existence at the same time, the first created object
 * is the master, and when the first object exits, the 2nd object
 * becomes master.
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
  std::unique_ptr<dmn::Dmn_DMesgNet> dmesgnet1 =
      std::make_unique<dmn::Dmn_DMesgNet>("dmesg-3", std::move(read_socket_1),
                                          std::move(write_socket_1));
  read_socket_1.reset();
  write_socket_1.reset();

  auto configs = dmn::Dmn_DMesg::kHandlerConfig_Default;
  configs[dmn::Dmn_DMesg::kHandlerConfig_IncludeSys] = "yes";

  auto listen_handle_3 = dmesgnet1->openHandler(
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

        std::this_thread::sleep_for(std::chrono::seconds(10));

        EXPECT_TRUE(dmesgpb_sys_4.body().sys().self().identifier() ==
                    "dmesg-4");
        EXPECT_TRUE(dmesgpb_sys_4.body().sys().self().masteridentifier() ==
                    "dmesg-3");

        std::this_thread::sleep_for(std::chrono::seconds(30));
        std::cout << "exit dmesg_4_proc\n";

        dmesgnet1.closeHandler(listen_handle_4);
        std::cout << "end dmesg4_Proc\n";
      }};

  dmesg_4_proc.exec();

  std::this_thread::sleep_for(std::chrono::seconds(15));

  dmesgnet1->closeHandler(listen_handle_3);
  std::cout << "close listen_handle_3\n";
  dmesgnet1 = {};

  std::cout << "before wait for dmesg_4_proc to end\n";
  dmesg_4_proc.wait();
  EXPECT_TRUE(dmesgpb_sys_4.body().sys().self().identifier() == "dmesg-4");
  EXPECT_TRUE(dmesgpb_sys_4.body().sys().self().masteridentifier() ==
              "dmesg-4");

  return RUN_ALL_TESTS();
}
