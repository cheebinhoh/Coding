/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that two Dmn_DMesgNet objects that
 * participates in the same network through its inbound and outbound
 * Dmn_Io objects are in sync in the Dmn network, and message sent through
 * one Dmn_DMesgNet object is only received by other Dmn_DMesgNet objects
 * which participates in the same Dmn network but not itself.
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

  std::unique_ptr<dmn::Dmn_Io<std::string>> readSocket1 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5001);
  std::unique_ptr<dmn::Dmn_Io<std::string>> writeSocket1 =
      std::make_unique<dmn::Dmn_Socket>("127.0.0.1", 5000, true);

  bool readData{};
  dmn::DMesgPb msgPb{};
  dmn::Dmn_DMesgNet dmesgnet1{"dmesg-1", std::move(readSocket1),
                              std::move(writeSocket1)};
  readSocket1.reset();
  writeSocket1.reset();

  auto readHandler =
      dmesgnet1.openHandler("dmesg-1-handler", nullptr,
                            [&msgPb, &readData](dmn::DMesgPb data) mutable {
                              readData = true;
                              msgPb = data;
                            });

  dmn::DMesgPb dmesgpb{};
  dmesgpb.set_topic("counter sync");
  dmesgpb.set_type(dmn::DMesgTypePb::message);
  dmesgpb.set_sourceidentifier("handler-1");
  std::string data{"Hello dmesg async"};
  dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
  dmesgpb_body->set_message(data);

  readHandler->write(dmesgpb);

  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(!readData);

  dmesgnet1.closeHandler(readHandler);

  return RUN_ALL_TESTS();
}
