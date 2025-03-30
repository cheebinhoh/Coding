// For study purpose, and copy from
// https://developer.confluent.io/get-started/c/

#include "kafka/dmn-kafka.hpp"

#include <cassert>
#include <iostream>

#include <signal.h>
#include <stdlib.h>

static volatile sig_atomic_t run = 1;

/**
 * @brief Signal termination of program
 */
static void stop(int sig) { run = 0; }

int main(int argc, char **argv) {
  Dmn::Dmn_Kafka::ConfigType readConfigs{};
  readConfigs["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  readConfigs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  readConfigs["sasl.password"] =
      "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  readConfigs["security.protocol"] = "SASL_SSL";
  readConfigs["sasl.mechanisms"] = "PLAIN";
  readConfigs["group.id"] = "dmn-kafka-receiver";
  readConfigs[Dmn::Dmn_Kafka::Topic] = "timer_counter";
  readConfigs["auto.offset.reset"] = "earliest";

  Dmn::Dmn_Kafka consumer{Dmn::Dmn_Kafka::Role::Consumer, readConfigs};

  // Install a signal handler for clean shutdown.
  signal(SIGINT, stop);

  // Start polling for messages.
  while (run) {
    auto data = consumer.read();

    if (data) {
      printf("Consumed message: %s\n", data->c_str());
    }
  }

  return 0;
}
