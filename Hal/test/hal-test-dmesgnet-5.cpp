/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-dmesgnet.hpp"
#include "hal-socket.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  std::shared_ptr<Hal::Hal_Io<std::string>> readSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5001);
  std::shared_ptr<Hal::Hal_Io<std::string>> writeSocket1 = std::make_shared<Hal::Hal_Socket>("127.0.0.1",
                                                                                            5000,
                                                                                            true);

  Hal::DMesgPb sysPb_3{};
  std::unique_ptr<Hal::Hal_DMesgNet> dmesgnet1 = std::make_unique<Hal::Hal_DMesgNet>("dmesg-3",
                                                                                     writeSocket1,
                                                                                     readSocket1);

  auto listenHandler3 = dmesgnet1->openHandler("dmesg-3-listen", true,
                                               nullptr,
                                               [&sysPb_3](Hal::DMesgPb data) mutable {
                                                 if (data.type() == Hal::DMesgTypePb::sys) {
                                                   sysPb_3 = data;
                                                 }
                                               });

  Hal::DMesgPb sysPb_4{};

  std::this_thread::sleep_for(std::chrono::seconds(3));
  Hal::Hal_Proc dmesg_4_Proc{"dmesg_4_Proc",
                             [&sysPb_4]() mutable {
                               std::shared_ptr<Hal::Hal_Io<std::string>> readSocket1 = 
                                 std::make_shared<Hal::Hal_Socket>("127.0.0.1", 5000);
                               std::shared_ptr<Hal::Hal_Io<std::string>> writeSocket1 =
                                 std::make_shared<Hal::Hal_Socket>("127.0.0.1",
                                                                   5001,
                                                                   true);

                               Hal::Hal_DMesgNet dmesgnet1{"dmesg-4", writeSocket1, readSocket1};
                               auto listenHandler4 = dmesgnet1.openHandler("dmesg-4-listen", true,
                                                                           nullptr,
                                                                           [&sysPb_4](Hal::DMesgPb data) mutable {
                                                                             if (data.type() == Hal::DMesgTypePb::sys) {
                                                                               sysPb_4 = data;
                                                                             }
                                                                           });

                               std::this_thread::sleep_for(std::chrono::seconds(10));

                               EXPECT_TRUE(sysPb_4.body().sys().self().identifier() == "dmesg-4");
                               EXPECT_TRUE(sysPb_4.body().sys().self().masteridentifier() == "dmesg-3");

                               std::this_thread::sleep_for(std::chrono::seconds(30));
                               std::cout << "exit dmesg_4_proc\n";
                             }};

  dmesg_4_Proc.exec();

  std::this_thread::sleep_for(std::chrono::seconds(15));

  listenHandler3 = {};
  dmesgnet1 = {};
  
  dmesg_4_Proc.wait();
  EXPECT_TRUE(sysPb_4.body().sys().self().identifier() == "dmesg-4");
  EXPECT_TRUE(sysPb_4.body().sys().self().masteridentifier() == "dmesg-4");


  return RUN_ALL_TESTS();
}
