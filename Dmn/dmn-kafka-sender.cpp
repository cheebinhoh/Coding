// For study purpose, and copy from
// https://developer.confluent.io/get-started/c/

#include "dmn-kafka-util.hpp"
#include "rdkafka.h"

#include <cassert>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])))

/**
 * Optional per-message delivery callback (triggered by poll() or flush())
 * when a message has been successfully delivered or permanently
 * failed delivery (after retries).
 */
static void dr_msg_cb(rd_kafka_t *kafka_handle,
                      const rd_kafka_message_t *rkmessage, void *opaque) {
  if (rkmessage->err) {
    std::cerr << "Message delivery failed: " << rd_kafka_err2str(rkmessage->err)
              << "\n";
  }
}

int main(int argc, char **argv) {
  rd_kafka_t *producer;
  rd_kafka_conf_t *conf;
  char errstr[512];

  // Create client configuration
  conf = rd_kafka_conf_new();

  // User-specific properties that you must set
  auto res = Dmn::set_config(conf, "bootstrap.servers",
                             "pkc-619z3.us-east1.gcp.confluent.cloud:9092");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: bootstrap.servers: " << res.error()
              << "\n";
    exit(1);
  }

  assert(res.value() == RD_KAFKA_CONF_OK);

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

  res = Dmn::set_config(conf, "acks", "all");
  if (!res.has_value()) {
    std::cerr << "Error in set_config: acks: " << res.error() << "\n";
    exit(1);
  }

  // Install a delivery-error callback.
  rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

  // Create the Producer instance.
  producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
  if (!producer) {
    std::cerr << "Failed to create new producer: " << errstr << "\n";
    return 1;
  }

  // Configuration object is now owned, and freed, by the rd_kafka_t instance.
  conf = NULL;

  // Produce data by selecting random values from these lists.
  int message_count = 10;
  const char *topic = "timer_counter";
  const char *user_ids[6] = {"eabara",   "jsmith",  "sgarcia",
                             "jbernard", "htanaka", "awalther"};
  const char *products[5] = {"book", "alarm clock", "t-shirts", "gift card",
                             "batteries"};

  for (int i = 0; i < message_count; i++) {
    const char *key = user_ids[random() % ARR_SIZE(user_ids)];
    const char *value = products[random() % ARR_SIZE(products)];
    size_t key_len = strlen(key);
    size_t value_len = strlen(value);

    rd_kafka_resp_err_t err;

    err = rd_kafka_producev(producer, RD_KAFKA_V_TOPIC(topic),
                            RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                            RD_KAFKA_V_KEY((void *)key, key_len),
                            RD_KAFKA_V_VALUE((void *)value, value_len),
                            RD_KAFKA_V_OPAQUE(NULL), RD_KAFKA_V_END);

    if (err) {
      std::cerr << "Failed to produce to topic: " << topic
                << ", error: " << topic << rd_kafka_err2str(err) << "\n";
      return 1;
    } else {
      std::cout << "Produced event to topic: " << topic << ", key: " << key
                << ", value: " << value << "\n";
    }

    rd_kafka_poll(producer, 0);
  }

  // Block until the messages are all sent.
  std::cout << "Flushing final messages...\n";
  rd_kafka_flush(producer, 10 * 1000);

  if (rd_kafka_outq_len(producer) > 0) {
    std::cerr << "Number ofmessage(s) were not delivered: "
              << rd_kafka_outq_len(producer) << "\n";
  }

  std::cout << "Number of events (" << message_count
            << ") were produced to topic: " << topic << "\n";

  rd_kafka_destroy(producer);

  return 0;
}
