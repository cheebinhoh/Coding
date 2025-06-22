/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that we can have two Dmn_DMesgNet objects with
 * external Dmn_Kafka objects as its input and output handlers and join in
 * a virtual distrbuted messaging network that spans cross a confluent.cloud
 * via Dmn_Kafka I/O and rdkafka, aka the primitive form of Dmn_DMesgNet_Kafka.
 *
 * Each Dmn_DMesgNet_Kafka object has a sys state' nodelist that includes
 * another object identifier as its neighbor.
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
  read_configs_other["auto.offset.reset"] = "earliest";
  read_configs_other[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  read_configs_other[dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  dmn::Dmn_Kafka consumer_other{dmn::Dmn_Kafka::Role::kConsumer,
                                read_configs_other};

  // dmesgnet1
  // writer for DMesgNet
  dmn::Dmn_Kafka::ConfigType write_configs_1{};
  write_configs_1["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  write_configs_1["sasl.username"] = "ICCN4A57TNKONPQ3";
  write_configs_1["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  write_configs_1["security.protocol"] = "SASL_SSL";
  write_configs_1["sasl.mechanisms"] = "PLAIN";
  write_configs_1["acks"] = "all";
  write_configs_1[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  write_configs_1[dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::unique_ptr<dmn::Dmn_Kafka> producer1 = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kProducer, write_configs_1);

  // reader for DMesgNet
  dmn::Dmn_Kafka::ConfigType read_configs_1{};
  read_configs_1["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  read_configs_1["sasl.username"] = "ICCN4A57TNKONPQ3";
  read_configs_1["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  read_configs_1["security.protocol"] = "SASL_SSL";
  read_configs_1["sasl.mechanisms"] = "PLAIN";
  read_configs_1["group.id"] = "dmesg1";
  read_configs_1[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  read_configs_1["auto.offset.reset"] = "earliest";
  read_configs_1[dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  std::unique_ptr<dmn::Dmn_Kafka> consumer1 = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kConsumer, read_configs_1);

  // dmesgnet2
  // writer for DMesgNet
  dmn::Dmn_Kafka::ConfigType write_configs_2{};
  write_configs_2["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  write_configs_2["sasl.username"] = "ICCN4A57TNKONPQ3";
  write_configs_2["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  write_configs_2["security.protocol"] = "SASL_SSL";
  write_configs_2["sasl.mechanisms"] = "PLAIN";
  write_configs_2["acks"] = "all";
  write_configs_2[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  write_configs_2[dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::unique_ptr<dmn::Dmn_Kafka> producer2 = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kProducer, write_configs_2);

  // reader for DMesgNet
  dmn::Dmn_Kafka::ConfigType read_configs_2{};
  read_configs_2["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  read_configs_2["sasl.username"] = "ICCN4A57TNKONPQ3";
  read_configs_2["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  read_configs_2["security.protocol"] = "SASL_SSL";
  read_configs_2["sasl.mechanisms"] = "PLAIN";
  read_configs_2["group.id"] = "dmesg2";
  read_configs_2[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  read_configs_2["auto.offset.reset"] = "earliest";
  read_configs_2[dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  std::unique_ptr<dmn::Dmn_Kafka> consumer2 = std::make_unique<dmn::Dmn_Kafka>(
      dmn::Dmn_Kafka::Role::kConsumer, read_configs_2);

  // dmesgnet1
  dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", std::move(consumer1),
                              std::move(producer1)};
  producer1.reset();
  consumer1.reset();

  // dmesgnet2
  dmn::Dmn_DMesgNet dmesgnet2{"dmesg2", std::move(consumer2),
                              std::move(producer2)};
  producer2.reset();
  consumer2.reset();

  std::this_thread::sleep_for(std::chrono::seconds(5));

  // consume prior messages from topic.
  dmn::DMesgPb dmesgpb_read{};
  std::map<std::string, std::string> node_list{};
  std::map<std::string, std::string> master_list{};
  int n{};
  while (n < 10000) {
    auto data_read = consumer_other.read();
    if (data_read) {
      dmesgpb_read.ParseFromString(*data_read);

      int i = 0;
      while (i < dmesgpb_read.body().sys().nodelist().size()) {
        auto id = dmesgpb_read.body().sys().nodelist().Get(i).identifier();
        node_list[dmesgpb_read.body().sys().self().identifier()] = id;
        i++;
      }

      EXPECT_TRUE(i <= 1);

      master_list[dmesgpb_read.body().sys().self().identifier()] =
          dmesgpb_read.body().sys().self().masteridentifier();

      if (node_list.size() == 2) {
        std::string master{};
        bool ok{true};

        for (auto &mp : master_list) {
          if (master != "") {
            if (master != mp.second) {
              ok = false;
              break;
            }
          }

          master = mp.second;
        }

        if (ok) {
          std::cout << "all checked\n";
          break;
        }
      }
    }

    n++;
  }

  EXPECT_TRUE(n < 10000);

  return RUN_ALL_TESTS();
}
