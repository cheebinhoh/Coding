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

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  std::shared_ptr<Dmn::Dmn_Io<std::string>> readSocket1 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1", 5001);
  std::shared_ptr<Dmn::Dmn_Io<std::string>> writeSocket1 = std::make_shared<Dmn::Dmn_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);

  Dmn::DMesgPb sysPb_3{};
  Dmn::Dmn_DMesgNet dmesgnet1{"dmesg-3", readSocket1, writeSocket1};

  auto listenHandler3 = dmesgnet1.openHandler("dmesg-3-listen", true,
                                              nullptr,
                                              [&sysPb_3](Dmn::DMesgPb data) mutable {
                                                if (data.type() == Dmn::DMesgTypePb::sys) {
                                                  sysPb_3 = data;
                                                }
                                              });

  Dmn::DMesgPb sysPb_4{};

  std::this_thread::sleep_for(std::chrono::seconds(3));
  Dmn::Dmn_Proc dmesg_4_Proc{"dmesg_4_Proc",
                             [&sysPb_4]() mutable {
                               std::shared_ptr<Dmn::Dmn_Io<std::string>> readSocket1 = 
                                 std::make_shared<Dmn::Dmn_Socket>("127.0.0.1", 5000);
                               std::shared_ptr<Dmn::Dmn_Io<std::string>> writeSocket1 =
                                 std::make_shared<Dmn::Dmn_Socket>("127.0.0.1",
                                                                   5001,
                                                                   true);

                               Dmn::Dmn_DMesgNet dmesgnet1{"dmesg-4", readSocket1, writeSocket1};
                               auto listenHandler4 = dmesgnet1.openHandler("dmesg-4-listen", true,
                                                                           nullptr,
                                                                           [&sysPb_4](Dmn::DMesgPb data) mutable {
                                                                             if (data.type() == Dmn::DMesgTypePb::sys) {
                                                                               sysPb_4 = data;
                                                                             }
                                                                           });

                               std::this_thread::sleep_for(std::chrono::seconds(7));
                               dmesgnet1.waitForEmpty();
                             }};

  dmesg_4_Proc.exec();

  std::this_thread::sleep_for(std::chrono::seconds(7));
  dmesgnet1.waitForEmpty();
  dmesg_4_Proc.wait();

  EXPECT_TRUE(sysPb_3.body().sys().self().identifier() == "dmesg-3");
  EXPECT_TRUE(sysPb_3.body().sys().self().masteridentifier() == "dmesg-3");
  EXPECT_TRUE(sysPb_4.body().sys().self().identifier() == "dmesg-4");
  EXPECT_TRUE(sysPb_4.body().sys().self().masteridentifier() == "dmesg-3");

  return RUN_ALL_TESTS();
}
