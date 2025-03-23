/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This is wrapper API to access DMesgPb protobuf message.
 */

#ifndef DMN_DMESG_PB_UTIL_HPP_HAVE_SEEN

#define DMN_DMESG_PB_UTIL_HPP_HAVE_SEEN

#include "proto/dmn-dmesg.pb.h"

#include <cassert>
#include <sys/time.h>

#define DMESG_PB_SET_TIMESTAMP_SECONDS(ts, val) (ts)->set_seconds(val)

#define DMESG_PB_SET_TIMESTAMP_NANOS(ts, val) (ts)->set_nanos(val)

#define DMESG_PB_SET_TIMESTAMP_FROM_TV(ts, tv)                                 \
  do {                                                                         \
    DMESG_PB_SET_TIMESTAMP_SECONDS(ts, (tv).tv_sec);                           \
    DMESG_PB_SET_TIMESTAMP_NANOS(ts, (tv).tv_usec * 1000);                     \
  } while (false)

/**
 * DMesgPb Message setters
 */
#define DMESG_PB_SET_MSG_TIMESTAMP_FROM_TV(pb, tv)                             \
  DMESG_PB_SET_TIMESTAMP_FROM_TV((pb).mutable_timestamp(), tv)

#define DMESG_PB_SET_MSG_TOPIC(pb, val) ((pb).set_topic((val)))

#define DMESG_PB_SET_MSG_RUNNINGCOUNTER(pb, val)                               \
  ((pb).set_runningcounter((val)))

#define DMESG_PB_SET_MSG_SOURCEIDENTIFIER(pb, val)                             \
  ((pb).set_sourceidentifier((val)))

#define DMESG_PB_SET_MSG_TYPE(pb, val) ((pb).set_type((val)))

/**
 * DMesgPb message body message setters
 */
#define DMESG_PB_MSG_SET_MESSAGE(pb, val)                                      \
  do {                                                                         \
    assert((pb).type() == Dmn::DMesgTypePb::message);                          \
    ((pb).mutable_body()->set_message((val)));                                 \
  } while (false)

/**
 * DMesgPb message body sys setters
 */
#define DMESG_PB_SYS_SET_TIMESTAMP_FROM_TV(pb, tv)                             \
  DMESG_PB_SET_TIMESTAMP_FROM_TV(                                              \
      (pb).mutable_body()->mutable_sys()->mutable_timestamp(), tv)

#define DMESG_PB_SYS_NODE_SET_IDENTIFIER(node, val) (node)->set_identifier(val)

#define DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(node, val)                      \
  (node)->set_masteridentifier(val)

#define DMESG_PB_SYS_NODE_SET_STATE(node, val) (node)->set_state(val)

#define DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(node, tv)               \
  DMESG_PB_SET_TIMESTAMP_FROM_TV((node)->mutable_updatedtimestamp(), tv)

#define DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP_FROM_TV(node, tv)           \
  DMESG_PB_SET_TIMESTAMP_FROM_TV((node)->mutable_initializedtimestamp(), tv)

#define DMESG_PB_SYS_SET_NODELIST_ELEM_IDENTIFIER(sys, index, val)             \
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(                                            \
      ((sys).mutable_body()->mutable_sys()->mutable_nodelist((index))), val)

#define DMESG_PB_SYS_SET_NODELIST_ELEM_MASTERIDENTIFIER(sys, index, val)       \
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(                                      \
      ((sys).mutable_body()->mutable_sys()->mutable_nodelist((index))), val)

#define DMESG_PB_SYS_SET_NODELIST_ELEM_STATE(sys, index, val)                  \
  DMESG_PB_SYS_NODE_SET_STATE(                                                 \
      ((sys).mutable_body()->mutable_sys()->mutable_nodelist((index))), val)

#define DMESG_PB_SYS_SET_NODELIST_ELEM_INITIALIZEDTIMESTAMP(sys, index, val)   \
  do {                                                                         \
    DMESG_PB_SET_TIMESTAMP_SECONDS(((sys)                                      \
                                        .mutable_body()                        \
                                        ->mutable_sys()                        \
                                        ->mutable_nodelist((index))            \
                                        ->mutable_initializedtimestamp()),     \
                                   (val).seconds());                           \
    DMESG_PB_SET_TIMESTAMP_NANOS(((sys)                                        \
                                      .mutable_body()                          \
                                      ->mutable_sys()                          \
                                      ->mutable_nodelist((index))              \
                                      ->mutable_initializedtimestamp()),       \
                                 (val).nanos());                               \
  } while (false)

#define DMESG_PB_SYS_SET_NODELIST_ELEM_UPDATEDTIMESTAMP(sys, index, val)       \
  do {                                                                         \
    DMESG_PB_SET_TIMESTAMP_SECONDS(((sys)                                      \
                                        .mutable_body()                        \
                                        ->mutable_sys()                        \
                                        ->mutable_nodelist((index))            \
                                        ->mutable_updatedtimestamp()),         \
                                   (val).seconds());                           \
    DMESG_PB_SET_TIMESTAMP_NANOS(((sys)                                        \
                                      .mutable_body()                          \
                                      ->mutable_sys()                          \
                                      ->mutable_nodelist((index))              \
                                      ->mutable_updatedtimestamp()),           \
                                 (val).nanos());                               \
  } while (false)

#endif /* DMN_DMESG_PB_UTIL_HPP_HAVE_SEEN */
