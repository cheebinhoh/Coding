/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include <sys/time.h>

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "kafka/dmn-kafka.hpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  // writer
  dmn::Dmn_Kafka::ConfigType write_configs{};
  write_configs["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  write_configs["sasl.username"] = "ICCN4A57TNKONPQ3";
  write_configs["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  write_configs["security.protocol"] = "SASL_SSL";
  write_configs["sasl.mechanisms"] = "PLAIN";
  write_configs["acks"] = "all";
  write_configs[dmn::Dmn_Kafka::Topic] = "timer_counter";
  write_configs[dmn::Dmn_Kafka::Key] = "tick";

  dmn::Dmn_Kafka producer{dmn::Dmn_Kafka::Role::kProducer, write_configs};

  // reader
  dmn::Dmn_Kafka::ConfigType read_configs{};
  read_configs["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  read_configs["sasl.username"] = "ICCN4A57TNKONPQ3";
  read_configs["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  read_configs["security.protocol"] = "SASL_SSL";
  read_configs["sasl.mechanisms"] = "PLAIN";
  read_configs["group.id"] = "dmn-kafka-receiver";
  read_configs[dmn::Dmn_Kafka::Topic] = "timer_counter";
  read_configs["auto.offset.reset"] = "earliest";

  dmn::Dmn_Kafka consumer{dmn::Dmn_Kafka::Role::kConsumer, read_configs};

  std::vector<std::string> data{"heartbeat : test 1", "heartbeat : test 2"};
  for (auto &d : data) {
    producer.write(d);
  }

  // sleep and wait for data to sync to reader
  std::this_thread::sleep_for(std::chrono::seconds(7));
  size_t index = 0;

  while (true) {
    auto data_read = consumer.read();
    if (!data_read) {
      break; // no data
    }

    EXPECT_TRUE((*data_read) == data[index]);

    index++;
  }

  EXPECT_TRUE(data.size() == index);

  read_configs[dmn::Dmn_Kafka::PollTimeoutMs] = "7000";
  dmn::Dmn_Kafka consumer2{dmn::Dmn_Kafka::Role::kConsumer, read_configs};

  std::cout << "read without data\n";
  struct timeval tv;
  gettimeofday(&tv, NULL);

  auto data_read = consumer2.read();
  struct timeval tv_after;
  gettimeofday(&tv_after, NULL);

  EXPECT_TRUE((tv_after.tv_sec - tv.tv_sec) >= 5);

  return RUN_ALL_TESTS();
}
