/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn.hpp
 * @brief Main header file for the Distributed Messaging Network (DMN) project,
 *        and the file aggregates all necessary header files for the DMN
 *        library.
 */

#ifndef DMN_HPP_

#define DMN_HPP_

#include "dmn-async.hpp"
#include "dmn-buffer.hpp"
#include "dmn-debug.hpp"
#include "dmn-dmesg-pb-util.hpp"
#include "dmn-dmesg.hpp"
#include "dmn-event.hpp"
#include "dmn-io.hpp"
#include "dmn-limit-buffer.hpp"
#include "dmn-pipe.hpp"
#include "dmn-proc.hpp"
#include "dmn-pub-sub.hpp"
#include "dmn-singleton.hpp"
#include "dmn-socket.hpp"
#include "dmn-teepipe.hpp"
#include "dmn-timer.hpp"
#include "dmn-util.hpp"

#include "kafka/dmn-dmesgnet-kafka.hpp"
#include "kafka/dmn-kafka-util.hpp"
#include "kafka/dmn-kafka.hpp"

#endif // DMN_HPP_
