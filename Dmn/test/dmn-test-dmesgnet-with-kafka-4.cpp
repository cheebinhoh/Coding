/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "kafka/dmn-dmesgnet-kafka.hpp"
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
  Dmn::Dmn_Kafka::ConfigType configs{};
  configs["bootstrap.servers"] = "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  configs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  configs["sasl.password"] = "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  configs["security.protocol"] = "SASL_SSL";
  configs["sasl.mechanisms"] = "PLAIN";

  // dmesgnet1
  Dmn::Dmn_DMesgNet_Kafka dmesgnet1{"dmesg1", configs};

  // dmesgnet2
  Dmn::Dmn_DMesgNet_Kafka dmesgnet2{"dmesg2", configs};

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
