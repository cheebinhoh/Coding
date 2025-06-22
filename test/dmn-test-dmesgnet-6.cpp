/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This program asserts that a Dmn_DMesgNet object will self-proclaim
 * as a master if it is started without other Dmn_DMesgNet objects
 * and a emulated system message with other node identifier as master
 * will not derail the Dmn_DMesgNet object notion of who is master
 * as long as the Dmn_DMesgNet object timestamp is earlier than other.
 */

#include <sys/time.h>

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

  std::this_thread::sleep_for(std::chrono::seconds(1));
  dmn::Dmn_Proc dmesg_4_proc{
      "dmesg_4_proc", [&dmesgpb_sys_4]() mutable {
        std::unique_ptr<dmn::Dmn_Io<std::string>> write_socket_1 =
            std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5001, true);
        dmn::DMesgPb sys{};
        struct timeval tv;

        gettimeofday(&tv, NULL);

        DMESG_PB_SET_MSG_TOPIC(sys, "sys.dmn-dmesg");
        DMESG_PB_SET_MSG_TYPE(sys, dmn::DMesgTypePb::sys);
        DMESG_PB_SYS_SET_TIMESTAMP_FROM_TV(sys, tv);
        DMESG_PB_SET_MSG_SOURCEIDENTIFIER(sys, "dmesg-4");

        auto *self = sys.mutable_body()->mutable_sys()->mutable_self();
        DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP_FROM_TV(self, tv);
        DMESG_PB_SYS_NODE_SET_IDENTIFIER(self, "dmesg-4");
        DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::Ready);
        DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "dmesg-4");

        for (long long n = 0; n < 30; n++) {
          DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);
          std::string serialized_string{};

          sys.SerializeToString(&serialized_string);

          write_socket_1->write(serialized_string);
          std::this_thread::sleep_for(std::chrono::milliseconds(100));

          gettimeofday(&tv, NULL);
        }
      }};

  dmesg_4_proc.exec();
  dmesg_4_proc.wait();

  std::this_thread::sleep_for(std::chrono::seconds(10));
  EXPECT_TRUE(dmesgpb_sys_3.body().sys().self().masteridentifier() ==
              "dmesg-3");

  return RUN_ALL_TESTS();
}
