/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesgnet.hpp"
#include "dmn-socket.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <sys/time.h>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  std::shared_ptr<Dmn::Dmn_Io<std::string>> readSocket1 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1", 5001);
  std::shared_ptr<Dmn::Dmn_Io<std::string>> writeSocket1 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);

  Dmn::DMesgPb sysPb_3{};
  std::unique_ptr<Dmn::Dmn_DMesgNet> dmesgnet1 = std::make_unique<Dmn::Dmn_DMesgNet>("dmesg-3",
                                                                                     readSocket1,
                                                                                     writeSocket1);

  auto listenHandler3 = dmesgnet1->openHandler("dmesg-3-listen", true,
                                               nullptr,
                                               [&sysPb_3](Dmn::DMesgPb data) mutable {
                                                 if (data.type() == Dmn::DMesgTypePb::sys) {
                                                   sysPb_3 = data;
                                                 }
                                               });

  Dmn::DMesgPb sysPb_4{};

  std::this_thread::sleep_for(std::chrono::seconds(1));
  Dmn::Dmn_Proc dmesg_4_Proc{"dmesg_4_Proc",
                             [&sysPb_4]() mutable {
                               std::shared_ptr<Dmn::Dmn_Io<std::string>> writeSocket1 =
                                 std::make_shared<Dmn::Dmn_Socket>("127.0.0.1",
                                                                   5001,
                                                                   true);
                               Dmn::DMesgPb sys{};
                               struct timeval tv;

                               gettimeofday(&tv, NULL);

                               DMESG_PB_SET_MSG_TOPIC(sys, "sys.dmn-dmesg");                          
                               DMESG_PB_SET_MSG_TYPE(sys, Dmn::DMesgTypePb::sys);
                               DMESG_PB_SYS_SET_TIMESTAMP_FROM_TV(sys, tv);
                               DMESG_PB_SET_MSG_SOURCEIDENTIFIER(sys, "dmesg-4");

                               auto *self = sys.mutable_body()->mutable_sys()->mutable_self();          
                               DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP_FROM_TV(self, tv);
                               DMESG_PB_SYS_NODE_SET_IDENTIFIER(self, "dmesg-4");
                               DMESG_PB_SYS_NODE_SET_STATE(self, Dmn::DMesgStatePb::Ready);
                               DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "dmesg-4");

                               for (long long n = 0; n < 30; n++) {
                                 DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);
                                 std::string serialized_string{};
            
                                 sys.SerializeToString(&serialized_string);
        
                                 writeSocket1->write(serialized_string);
                                 std::this_thread::sleep_for(std::chrono::milliseconds(100));

                                 gettimeofday(&tv, NULL);
                               }
                             }};

  dmesg_4_Proc.exec();
  dmesg_4_Proc.wait();

  std::this_thread::sleep_for(std::chrono::seconds(10));
  EXPECT_TRUE(sysPb_3.body().sys().self().masteridentifier() == "dmesg-3");

  return RUN_ALL_TESTS();
}
