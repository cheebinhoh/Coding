/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that the Dmn_DMesgNet object can be constructed
 * with the external Dmn_Kafka objects as its input and output handler and
 * perform object teardown without problem.
 */

#include <sys/time.h>

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "dmn-dmesgnet.hpp"

#include "kafka/dmn-kafka.hpp"

#include "proto/dmn-dmesg.pb.h"

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
  write_configs[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  write_configs[dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::unique_ptr<dmn::Dmn_Kafka> producer = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kProducer, write_configs);

  // reader
  dmn::Dmn_Kafka::ConfigType read_configs{};
  read_configs["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  read_configs["sasl.username"] = "ICCN4A57TNKONPQ3";
  read_configs["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  read_configs["security.protocol"] = "SASL_SSL";
  read_configs["sasl.mechanisms"] = "PLAIN";
  read_configs["group.id"] = "dmesg1";
  read_configs[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  read_configs["auto.offset.reset"] = "latest";
  read_configs[dmn::Dmn_Kafka::PollTimeoutMs] = "1000";

  std::unique_ptr<dmn::Dmn_Kafka> consumer = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kConsumer, read_configs);

  dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", std::move(consumer),
                              std::move(producer)};
  consumer.reset();
  producer.reset();

  std::this_thread::sleep_for(std::chrono::seconds(5));

  return RUN_ALL_TESTS();
}
