/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-kafka-util.hpp"

#include "rdkafka.h"

#include <cassert>
#include <expected>
#include <string>

namespace Dmn {

std::expected<rd_kafka_conf_res_t, std::string>
set_config(rd_kafka_conf_t *conf, const char *key, const char *value) {
  char errstr[512]{};
  rd_kafka_conf_res_t res{};

  assert(nullptr != conf || nullptr == "conf parameter must not be nullptr");

  res = rd_kafka_conf_set(conf, key, value, errstr, sizeof(errstr));
  if (RD_KAFKA_CONF_OK != res) {
    std::string unexpectedErrStr = std::string(errstr);

    return std::unexpected(unexpectedErrStr);
  }

  return res;
}

} // namespace Dmn
