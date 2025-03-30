/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "kafka/dmn-kafka.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
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
  writeConfigs[Dmn::Dmn_Kafka::Topic] = "timer_counter";
  writeConfigs[Dmn::Dmn_Kafka::Key] = "tick";

  Dmn::Dmn_Kafka producer{Dmn::Dmn_Kafka::Role::Producer, writeConfigs};

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

  std::vector<std::string> data{"heartbeat : test 1", "heartbeat : test 2"};
  for (auto &d : data) {
    producer.write(d);
  }

  // sleep and wait for data to sync to reader
  std::this_thread::sleep_for(std::chrono::seconds(7));
  int index = 0;

  while (true) {
    auto dataRead = consumer.read();
    if (!dataRead) {
      break; // no data
    }

    EXPECT_TRUE((*dataRead) == data[index]);

    index++;
  }

  EXPECT_TRUE(data.size() == index);

  readConfigs[Dmn::Dmn_Kafka::PollTimeoutMs] = "7000";
  Dmn::Dmn_Kafka consumer2{Dmn::Dmn_Kafka::Role::Consumer, readConfigs};

  std::cout << "read without data\n";
  struct timeval tv;
  gettimeofday(&tv, NULL);

  auto dataRead = consumer2.read();
  struct timeval tvAfter;
  gettimeofday(&tvAfter, NULL);

  EXPECT_TRUE((tvAfter.tv_sec - tv.tv_sec) >= 5);

  return RUN_ALL_TESTS();
}
