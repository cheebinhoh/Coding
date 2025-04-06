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

  // reader
  Dmn::Dmn_Kafka::ConfigType readConfigs_other{};
  readConfigs_other["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  readConfigs_other["sasl.username"] = "C3T2TGVAQYYF7H6T";
  readConfigs_other["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  readConfigs_other["security.protocol"] = "SASL_SSL";
  readConfigs_other["sasl.mechanisms"] = "PLAIN";
  readConfigs_other["group.id"] = "dmesg_other";
  readConfigs_other["auto.offset.reset"] = "earliest";
  readConfigs_other[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  readConfigs_other[Dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  Dmn::Dmn_Kafka consumer_other{Dmn::Dmn_Kafka::Role::Consumer, readConfigs_other};

  // dmesgnet1
  // writer for DMesgNet
  Dmn::Dmn_Kafka::ConfigType writeConfigs1{};
  writeConfigs1["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  writeConfigs1["sasl.username"] = "C3T2TGVAQYYF7H6T";
  writeConfigs1["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  writeConfigs1["security.protocol"] = "SASL_SSL";
  writeConfigs1["sasl.mechanisms"] = "PLAIN";
  writeConfigs1["acks"] = "all";
  writeConfigs1[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  writeConfigs1[Dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::shared_ptr<Dmn::Dmn_Kafka> producer1 = std::make_shared<Dmn::Dmn_Kafka>(Dmn::Dmn_Kafka::Role::Producer, writeConfigs1);

  // reader for DMesgNet
  Dmn::Dmn_Kafka::ConfigType readConfigs1{};
  readConfigs1["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  readConfigs1["sasl.username"] = "C3T2TGVAQYYF7H6T";
  readConfigs1["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  readConfigs1["security.protocol"] = "SASL_SSL";
  readConfigs1["sasl.mechanisms"] = "PLAIN";
  readConfigs1["group.id"] = "dmesg1";
  readConfigs1[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  readConfigs1["auto.offset.reset"] = "earliest";
  readConfigs1[Dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  std::shared_ptr<Dmn::Dmn_Kafka> consumer1 = std::make_shared<Dmn::Dmn_Kafka>(Dmn::Dmn_Kafka::Role::Consumer, readConfigs1);


  // dmesgnet2
  // writer for DMesgNet
  Dmn::Dmn_Kafka::ConfigType writeConfigs2{};
  writeConfigs2["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  writeConfigs2["sasl.username"] = "C3T2TGVAQYYF7H6T";
  writeConfigs2["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  writeConfigs2["security.protocol"] = "SASL_SSL";
  writeConfigs2["sasl.mechanisms"] = "PLAIN";
  writeConfigs2["acks"] = "all";
  writeConfigs2[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  writeConfigs2[Dmn::Dmn_Kafka::Key] = "Dmn_dmesgnet";

  std::shared_ptr<Dmn::Dmn_Kafka> producer2 = std::make_shared<Dmn::Dmn_Kafka>(Dmn::Dmn_Kafka::Role::Producer, writeConfigs2);

  // reader for DMesgNet
  Dmn::Dmn_Kafka::ConfigType readConfigs2{};
  readConfigs2["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  readConfigs2["sasl.username"] = "C3T2TGVAQYYF7H6T";
  readConfigs2["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  readConfigs2["security.protocol"] = "SASL_SSL";
  readConfigs2["sasl.mechanisms"] = "PLAIN";
  readConfigs2["group.id"] = "dmesg2";
  readConfigs2[Dmn::Dmn_Kafka::Topic] = "Dmn_dmesgnet";
  readConfigs2["auto.offset.reset"] =  "earliest";
  readConfigs2[Dmn::Dmn_Kafka::PollTimeoutMs] = "7000";

  std::shared_ptr<Dmn::Dmn_Kafka> consumer2 = std::make_shared<Dmn::Dmn_Kafka>(Dmn::Dmn_Kafka::Role::Consumer, readConfigs2);


  // dmesgnet1
  Dmn::Dmn_DMesgNet dmesgnet1{"dmesg1", consumer1, producer1};
  producer1 = {};
  consumer1 = {};

  // dmesgnet2
  Dmn::Dmn_DMesgNet dmesgnet2{"dmesg2", consumer2, producer2};
  producer2 = {};
  consumer2 = {};


  std::this_thread::sleep_for(std::chrono::seconds(5));

  // consume prior messages from topic.
  Dmn::DMesgPb dmesgPbRead{};
  std::map<std::string, std::string> nodeList{};
  std::map<std::string, std::string> masterList{};
  int n{};
  while (n < 10000) {
    auto dataRead = consumer_other.read();
    if (dataRead) {
      dmesgPbRead.ParseFromString(*dataRead);

      int i = 0;
      while (i < dmesgPbRead.body().sys().nodelist().size()) {
        auto id = dmesgPbRead.body().sys().nodelist().Get(i).identifier();
        nodeList[dmesgPbRead.body().sys().self().identifier()] = id;
        i++;
      }

      EXPECT_TRUE(i <= 1);

      masterList[dmesgPbRead.body().sys().self().identifier()] = dmesgPbRead.body().sys().self().masteridentifier();

      if (nodeList.size() == 2) {
        std::string master{};
        bool ok{true};

        for (auto & mp : masterList) {
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
