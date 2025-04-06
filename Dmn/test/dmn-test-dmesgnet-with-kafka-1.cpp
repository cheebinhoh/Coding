/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-dmesgnet.hpp"
#include "kafka/dmn-kafka.hpp"

#include "proto/dmn-dmesg.pb.h"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <sys/time.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  // writer
  Dmn::Dmn_Kafka::ConfigType writeConfigs{};
  writeConfigs["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  writeConfigs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  writeConfigs["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  writeConfigs["security.protocol"] = "SASL_SSL";
  writeConfigs["sasl.mechanisms"] = "PLAIN";
  writeConfigs["acks"] = "all";
  writeConfigs[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  writeConfigs[Dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::shared_ptr<Dmn::Dmn_Kafka> producer = std::make_shared<Dmn::Dmn_Kafka>(Dmn::Dmn_Kafka::Role::Producer, writeConfigs);

  // reader
  Dmn::Dmn_Kafka::ConfigType readConfigs{};
  readConfigs["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  readConfigs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  readConfigs["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  readConfigs["security.protocol"] = "SASL_SSL";
  readConfigs["sasl.mechanisms"] = "PLAIN";
  readConfigs["group.id"] = "dmesg1";
  readConfigs[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  readConfigs["auto.offset.reset"] =  "latest";
  readConfigs[Dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  std::shared_ptr<Dmn::Dmn_Kafka> consumer = std::make_shared<Dmn::Dmn_Kafka>(Dmn::Dmn_Kafka::Role::Consumer, readConfigs);

  Dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", consumer, producer};
  std::this_thread::sleep_for(std::chrono::seconds(5));
  producer = {};
  consumer = {};

  return RUN_ALL_TESTS();
}
