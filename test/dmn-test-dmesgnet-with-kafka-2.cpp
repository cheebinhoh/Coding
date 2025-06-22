/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that the Dmn_DMesgNet object can integrate with
 * the external Dmn_Kafka objects that Dmn_DmesgPb message sent by Dmn_DMesgNet
 * object through outbound handler of Dmn_Kafka object (as producer) can be
 * consumed by external Dmn_kafka object serves as consumer of the same topic
 * published through outbound handler of Dmn_kafka object (within Dmn_DMesgNet
 * object).
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

  // reader
  dmn::Dmn_Kafka::ConfigType read_configs_other{};
  read_configs_other["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  read_configs_other["sasl.username"] = "ICCN4A57TNKONPQ3";
  read_configs_other["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  read_configs_other["security.protocol"] = "SASL_SSL";
  read_configs_other["sasl.mechanisms"] = "PLAIN";
  read_configs_other["group.id"] = "dmesg_other";
  read_configs_other["auto.offset.reset"] = "latest";
  read_configs_other[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  read_configs_other[dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  dmn::Dmn_Kafka consumer_other{dmn::Dmn_Kafka::Role::kConsumer,
                                read_configs_other};

  // writer for DMesgNet
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

  // reader for DMesgNet
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
  read_configs[dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  std::unique_ptr<dmn::Dmn_Kafka> consumer = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kConsumer, read_configs);

  dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", std::move(consumer),
                              std::move(producer)};
  consumer.reset();
  producer.reset();

  std::this_thread::sleep_for(std::chrono::seconds(3));

  // consume prior messages from topic.
  dmn::DMesgPb dmesgPbRead{};
  while (true) {
    auto dataRead = consumer_other.read();
    if (dataRead) {
      dmesgPbRead.ParseFromString(*dataRead);

      if (dmesgPbRead.body().sys().self().state() == dmn::DMesgStatePb::Ready) {
        break;
      }
    }
  }

  std::cout << "DebugPrint: " << dmesgPbRead.ShortDebugString() << "\n";

  return RUN_ALL_TESTS();
}
