/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "kafka/dmn-kafka.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

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
  writeConfigs[Dmn::Dmn_Kafka::Topic] = "timer_counter";
  writeConfigs[Dmn::Dmn_Kafka::Key] = "tick";

  Dmn::Dmn_Kafka producer{Dmn::Dmn_Kafka::Role::Producer, writeConfigs};
  producer.write("heartbeat : test 1");
 
 
  // reader
  Dmn::Dmn_Kafka::ConfigType readConfigs{};
  readConfigs["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  readConfigs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  readConfigs["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  readConfigs["security.protocol"] = "SASL_SSL";
  readConfigs["sasl.mechanisms"] = "PLAIN";
  readConfigs["group.id"] = "dmn-kafka-receiver";
  readConfigs[Dmn::Dmn_Kafka::Topic] = "timer_counter";
  readConfigs["auto.offset.reset"] =  "earliest";

  Dmn::Dmn_Kafka consumer{Dmn::Dmn_Kafka::Role::Consumer, readConfigs};

  auto data = consumer.read();
  EXPECT_TRUE(data);
  EXPECT_TRUE((*data) == "heartbeat : test 1");
 
  return RUN_ALL_TESTS();
}
