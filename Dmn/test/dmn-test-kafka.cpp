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

  Dmn::Dmn_Kafka::ConfigType configs{};
  configs["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  configs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  configs["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  configs["security.protocol"] = "SASL_SSL";
  configs["sasl.mechanisms"] = "PLAIN";
  configs["acks"] = "all";
  configs[Dmn::Dmn_Kafka::Topic] = "timer_counter";
  configs[Dmn::Dmn_Kafka::Key] = "tick";

  Dmn::Dmn_Kafka producer{Dmn::Dmn_Kafka::Role::Producer, configs};
 
  producer.write("heartbeat : test 1");
 
  return RUN_ALL_TESTS();
}
