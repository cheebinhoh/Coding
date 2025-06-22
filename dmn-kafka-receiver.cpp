// For study purpose, and copy from
// https://developer.confluent.io/get-started/c/

#include <signal.h>
#include <stdlib.h>

#include <atomic>
#include <cassert>
#include <iostream>

#include "kafka/dmn-kafka.hpp"

static volatile sig_atomic_t run = 1;

/**
 * @brief Signal termination of program
 */
static void stop(int sig) { run = 0; }

int main(int argc, char **argv) {
  dmn::Dmn_Kafka::ConfigType read_configs{};
  read_configs["bootstrap.servers"] =
      "pkc-619z3.us-east1.gcp.confluent.cloud:9092";
  read_configs["sasl.username"] = "C3T2TGVAQYYF7H6T";
  read_configs["sasl.password"] =
      "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr";
  read_configs["security.protocol"] = "SASL_SSL";
  read_configs["sasl.mechanisms"] = "PLAIN";
  read_configs["group.id"] = "dmn-kafka-receiver";
  read_configs[dmn::Dmn_Kafka::Topic] = "timer_counter";
  read_configs["auto.offset.reset"] = "earliest";

  dmn::Dmn_Kafka consumer{dmn::Dmn_Kafka::Role::kConsumer, read_configs};

  // Install a signal handler for clean shutdown.
  signal(SIGINT, stop);

  // Start polling for messages.
  while (run) {
    auto data = consumer.read();

    if (data) {
      std::cout << "Consumed message: " << (*data) << "\n";
    }
  }

  return 0;
}
