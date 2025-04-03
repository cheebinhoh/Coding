/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef DMN_KAFKA_UTIL_HPP_HAVE_SEEN

#define DMN_KAFKA_UTIL_HPP_HAVE_SEEN

#include "rdkafka.h"

#include <expected>
#include <string>

namespace Dmn {

/**
 * @brief The method sets kafka configuration to key value.
 *
 * @param conf The rd_kafka_conf_t object to set the configuration' key value
 * @param key The configuration key
 * @param value The configuration value
 *
 * @return It returns the RD_KAFKA_CONF_OK if it has expected result, else
 * string describes the error
 */
std::expected<rd_kafka_conf_res_t, std::string>
set_config(rd_kafka_conf_t *conf, const char *key, const char *value);

} /* End of namespace Dmn */

#endif /* End of macro DMN_KAFKA_UTIL_HPP_HAVE_SEEN */
