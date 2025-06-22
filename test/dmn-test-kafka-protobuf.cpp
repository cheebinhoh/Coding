/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include <sys/time.h>

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

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
  read_configs["group.id"] = "dmesg1";
  read_configs["auto.offset.reset"] = "latest";
  read_configs[dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  read_configs[dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  dmn::Dmn_Kafka consumer{dmn::Dmn_Kafka::Role::kConsumer, read_configs};

  // consume prior messages from topic.
  while (true) {
    auto data_read = consumer.read();
    if (!data_read) {
      break; // no data
    }

    std::cout << "skip data\n";
  }

  std::vector<std::string> data{"heartbeat : test 1", "heartbeat : test 2"};
  for (auto &d : data) {
    dmn::DMesgPb dmesgpb{};

    dmesgpb.set_topic("id1");
    dmesgpb.set_runningcounter(99);
    dmesgpb.set_sourceidentifier("dmesg1");
    dmesgpb.set_type(dmn::DMesgTypePb::message);

    dmn::DMesgBodyPb *dmsgbodyPb = dmesgpb.mutable_body();
    dmsgbodyPb->set_message(d);

    std::string protobufString{};

    dmesgpb.SerializeToString(&protobufString);

    producer.write(protobufString);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  // sleep and wait for data to sync to reader
  std::this_thread::sleep_for(std::chrono::seconds(7));
  size_t index = 0;

  while (true) {
    auto data_read = consumer.read();
    if (!data_read) {
      break; // no data
    }

    dmn::DMesgPb dmesgpb_read{};
    dmesgpb_read.ParseFromString(*data_read);

    EXPECT_TRUE("id1" == dmesgpb_read.topic());
    EXPECT_TRUE("dmesg1" == dmesgpb_read.sourceidentifier());
    EXPECT_TRUE(99 == dmesgpb_read.runningcounter());
    EXPECT_TRUE(dmn::DMesgTypePb::message == dmesgpb_read.type());

    std::cout << "message read: \"" << dmesgpb_read.body().message() << "\"\n";
    EXPECT_TRUE(data[index] == dmesgpb_read.body().message());

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
