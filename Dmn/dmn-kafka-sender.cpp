// For study purpose, and copy from
// https://developer.confluent.io/get-started/c/

#include "kafka/dmn-kafka.hpp"

#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
  Dmn::Dmn_Kafka::ConfigType writeConfigs{};
  writeConfigs["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  writeConfigs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  writeConfigs["sasl.password"] =
      "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  writeConfigs["security.protocol"] = "SASL_SSL";
  writeConfigs["sasl.mechanisms"] = "PLAIN";
  writeConfigs["acks"] = "all";
  writeConfigs[Dmn::Dmn_Kafka::Topic] = "timer_counter";
  writeConfigs[Dmn::Dmn_Kafka::Key] = "tick";

  Dmn::Dmn_Kafka producer{Dmn::Dmn_Kafka::Role::Producer, writeConfigs};

  for (int i = 0; i < 10; i++) {
    std::stringstream os{};

    os << "heartbaet: " << i;

    producer.write(os.str());
  }

  return 0;
}
