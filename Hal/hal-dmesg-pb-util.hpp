/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 * This is wrapper for DMesg protobuf message.
 */

#ifndef HAL_DMESG_PB_UTIL_HPP_HAVE_SEEN

#define HAL_DMESG_PB_UTIL_HPP_HAVE_SEEN

#include "proto/hal-dmesg.pb.h"

#include <cassert>
#include <sys/time.h>

#define DMESG_PB_INIT(pb)                                                      \
  Hal::DMesgPb(pb){};                                                          \
  struct timeval pb##_tv;                                                      \
  gettimeofday(&(pb##_tv), NULL);                                              \
  (pb).mutable_timestamp()->set_seconds(pb##_tv.tv_sec);                       \
  (pb).mutable_timestamp()->set_nanos(pb##_tv.tv_usec * 1000)

#define DMESG_PB_SET_IDENTIFIER(pb, val) ((pb).set_identifier((val)))

#define DMESG_PB_SET_RUNNINGCOUNTER(pb, val) ((pb).set_runningcounter((val)))

#define DMESG_PB_SET_SOURCEIDENTIFIER(pb, val)                                 \
  ((pb).set_sourceidentifier((val)))

#define DMESG_PB_SET_TYPE(pb, val) ((pb).set_type((val)))

#define DMESG_PB_MSG_SET_MESSAGE(pb, val)                                      \
  assert((pb).type() == Hal::DMesgTypePb::message);                            \
  ((pb).mutable_body()->set_message((val)))

#define DMESG_PB_SYS_INIT_TIMESTAMP(pb)                                                     \
  assert((pb).type() == Hal::DMesgTypePb::sys);                                             \
  struct timeval pb##sys_tv;                                                                \
  gettimeofday(&(pb##sys_tv), NULL);                                                        \
  ((pb).mutable_body()->mutable_sys()->mutable_timestamp()->set_seconds(pb##sys_tv.tv_sec);       \
  ((pb).mutable_body()->mutable_sys()->mutable_timestamp()->set_nanos(pb##sys_tv.tv_usec * 1000)

#define DMESG_PB_SYS_NODE_SET_IDENTIFIER(node, val) (node)->set_identifier(val)

#define DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(node, val)                      \
  (node)->set_masteridentifier(val)

#define DMESG_PB_SYS_NODE_SET_STATE(node, val) (node)->set_state(val)

#define DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(node, tv)                       \
  (node)->mutable_updatedtimestamp()->set_seconds((tv).tv_sec);                \
  (node)->mutable_updatedtimestamp()->set_nanos((tv).tv_usec * 1000)

#define DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP(node, tv)                   \
  (node)->mutable_initializedtimestamp()->set_seconds((tv).tv_sec);            \
  (node)->mutable_initializedtimestamp()->set_nanos((tv).tv_usec * 1000)

#endif /* HAL_DMESG_PB_UTIL_HPP_HAVE_SEEN */
