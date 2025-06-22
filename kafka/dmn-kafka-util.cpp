/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-kafka-util.hpp"

#include <cassert>
#include <expected>
#include <string>
#include <string_view>

#include "rdkafka.h"

namespace dmn {

std::expected<rd_kafka_conf_res_t, std::string>
set_config(rd_kafka_conf_t *config, std::string_view key,
           std::string_view value) {
  char err_str[kKafkaErrorStringLength]{};
  rd_kafka_conf_res_t res{};

  assert(nullptr != config ||
         nullptr == "config parameter must not be nullptr");

  res = rd_kafka_conf_set(config, key.data(), value.data(), err_str,
                          sizeof(err_str));
  if (RD_KAFKA_CONF_OK != res) {
    std::string unexpected_err_str = std::string(err_str);

    return std::unexpected(unexpected_err_str);
  }

  return res;
}

} // namespace dmn
