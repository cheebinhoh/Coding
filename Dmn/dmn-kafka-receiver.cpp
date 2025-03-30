// For study purpose, and copy from
// https://developer.confluent.io/get-started/c/

#include "dmn-kafka-util.hpp"
#include "rdkafka.h"

#include <cassert>
#include <iostream>

#include "rdkafka.h"
#include <signal.h>
#include <stdlib.h>

static volatile sig_atomic_t run = 1;

/**
 * @brief Signal termination of program
 */
static void stop(int sig) { run = 0; }

int main(int argc, char **argv) {
  rd_kafka_t *consumer;
  rd_kafka_conf_t *conf;
  rd_kafka_resp_err_t err;
  char errstr[512];

  // Create client configuration
  conf = rd_kafka_conf_new();

  // User-specific properties that you must set
  // User-specific properties that you must set
  auto res = Dmn::set_config(conf, "bootstrap.servers",
                             "pkc-619z3.us-east1.gcp.confluent.cloud:9092");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: bootstrap.servers: " << res.error()
              << "\n";
    exit(1);
  }

  res = Dmn::set_config(conf, "sasl.username", "C3T2TGVAQYYF7H6T");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: sasl.username: " << res.error() << "\n";
    exit(1);
  }

  res = Dmn::set_config(
      conf, "sasl.password",
      "4JNbCdwNsK6HSgj65AHdZT5d9VsyWPX+lQysSPca70ehKN7uHsCuIyPlHw32gmNr");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: sasl.password: " << res.error() << "\n";
    exit(1);
  }

  // Fixed properties
  res = Dmn::set_config(conf, "security.protocol", "SASL_SSL");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: security.protocol: " << res.error()
              << "\n";
    exit(1);
  }

  res = Dmn::set_config(conf, "sasl.mechanisms", "PLAIN");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: sasl.mechanisms: " << res.error()
              << "\n";
    exit(1);
  }

  res = Dmn::set_config(conf, "group.id", "dmn-kafka-receiver");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: group.id: " << res.error() << "\n";
    exit(1);
  }

  res = Dmn::set_config(conf, "auto.offset.reset", "earliest");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: auto.offset.reset: " << res.error()
              << "\n";
    exit(1);
  }

  // Create the Consumer instance.
  consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
  if (!consumer) {
    std::cerr << "Failed to create new consumer: " << errstr << "\n";
    exit(1);
  }

  rd_kafka_poll_set_consumer(consumer);

  // Configuration object is now owned, and freed, by the rd_kafka_t instance.
  conf = NULL;

  // Convert the list of topics to a format suitable for librdkafka.
  const char *topic = "timer_counter";
  rd_kafka_topic_partition_list_t *subscription =
      rd_kafka_topic_partition_list_new(1);
  rd_kafka_topic_partition_list_add(subscription, topic, RD_KAFKA_PARTITION_UA);

  // Subscribe to the list of topics.
  err = rd_kafka_subscribe(consumer, subscription);
  if (err) {
    std::cerr << "Failed to subscribe to " << subscription->cnt
              << " topics, error: " << rd_kafka_err2str(err) << "\n";
    rd_kafka_topic_partition_list_destroy(subscription);
    rd_kafka_destroy(consumer);
    return 1;
  }

  rd_kafka_topic_partition_list_destroy(subscription);

  // Install a signal handler for clean shutdown.
  signal(SIGINT, stop);

  // Start polling for messages.
  while (run) {
    rd_kafka_message_t *consumer_message;

    consumer_message = rd_kafka_consumer_poll(consumer, 500);
    if (!consumer_message) {
      std::cout << "Waiting... \n";
      continue;
    }

    if (consumer_message->err) {
      if (consumer_message->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
        /* We can ignore this error - it just means we've read
         * everything and are waiting for more data.
         */
      } else {
        std::cerr << "Consumer error: "
                  << rd_kafka_message_errstr(consumer_message) << "\n";
        return {};
      }
    } else {
      printf("Consumed event from topic %s: key = %.*s value = %.*s\n",
             rd_kafka_topic_name(consumer_message->rkt),
             (int)consumer_message->key_len, (char *)consumer_message->key,
             (int)consumer_message->len, (char *)consumer_message->payload);
    }

    // Free the message when we're done.
    rd_kafka_message_destroy(consumer_message);
  }

  // Close the consumer: commit final offsets and leave the group.
  std::cout << "Closing consumer\n";
  rd_kafka_consumer_close(consumer);

  // Destroy the consumer.
  rd_kafka_destroy(consumer);

  return 0;
}
