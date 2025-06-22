/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This test program asserts that we can have two Dmn_DMesgNet_Kafka object(s)
 * that joins in a virtual distrbuted messaging network that spans cross a
 * confluent.cloud via Dmn_Kafka I/O and rdkafka.
 *
 * Each Dmn_DMesgNet_Kafka object has a sys state' nodelist that includes
 * another object identifier as its neighbor.
 *
 * Part of the last test is to have a handler from one Dmn_DMesgNet_Kafka
 * object to subscribe a topic and then another handler of another
 * Dmn_DMesgNet_Kafka object to write a series of topic message over kafka
 * network.
 */

#include <sys/time.h>

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "kafka/dmn-dmesgnet-kafka.hpp"

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
  dmn::Dmn_Kafka::ConfigType configs{};
  configs["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  configs["sasl.username"] = "ICCN4A57TNKONPQ3";
  configs["sasl.password"] =
      "Fz6AqWg1WCBqkBV2FX2FD/9iBNbs1qHM5Po12iaVn6OMVKZm8WhH4W20IaZTTEcV";
  configs["security.protocol"] = "SASL_SSL";
  configs["sasl.mechanisms"] = "PLAIN";

  // dmesgnet1
  dmn::Dmn_DMesgNet_Kafka dmesgnet1{"dmesg1", configs};

  // dmesgnet2
  dmn::Dmn_DMesgNet_Kafka dmesgnet2{"dmesg2", configs};

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

  std::vector<std::string> topics{"counter sync 1", "counter sync 2"};
  std::string subscribed_topic{"counter sync 1"};

  int cnt{0};
  std::shared_ptr<dmn::Dmn_DMesg::Dmn_DMesgHandler> dmesg_handle =
      dmesgnet1.openHandler("handler1", subscribed_topic, nullptr,
                            [&cnt](const dmn::DMesgPb &msg) mutable {
                              std::cout << "read\n";
                              EXPECT_TRUE("counter sync 1" == msg.topic());
                              cnt++;
                            });
  EXPECT_TRUE(dmesg_handle);

  auto dmesg_write_handle1 = dmesgnet2.openHandler("writeHandler1", topics[0]);
  EXPECT_TRUE(dmesg_write_handle1);

  auto dmesg_write_handle2 = dmesgnet2.openHandler("writeHandler2", topics[1]);
  EXPECT_TRUE(dmesg_write_handle2);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  for (int n = 0; n < 6; n++) {
    dmn::DMesgPb dmesgpb{};
    dmesgpb.set_type(dmn::DMesgTypePb::message);

    std::string data{"Hello dmesg async"};
    dmn::DMesgBodyPb *dmesgpb_body = dmesgpb.mutable_body();
    dmesgpb_body->set_message(data);

    if ((n % 2) == 0) {
      dmesg_write_handle1->write(dmesgpb);
    } else {
      dmesg_write_handle2->write(dmesgpb);
    }
  }

  std::this_thread::sleep_for(std::chrono::seconds(8));

  dmesgnet2.closeHandler(dmesg_write_handle2);
  dmesgnet2.closeHandler(dmesg_write_handle1);
  dmesgnet1.closeHandler(dmesg_handle);

  EXPECT_TRUE(3 == cnt);

  return RUN_ALL_TESTS();
}
