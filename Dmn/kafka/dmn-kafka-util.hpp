/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef DMN_KAFKA_UTIL_HPP_HAVE_SEEN

#define DMN_KAFKA_UTIL_HPP_HAVE_SEEN

#include "rdkafka.h"

#include <expected>
#include <string>

namespace Dmn {

std::expected<rd_kafka_conf_res_t, std::string>
set_config(rd_kafka_conf_t *conf, const char *key, const char *value);

} /* namespace Dmn */

#endif /* DMN_KAFKA_UTIL_HPP_HAVE_SEEN */
