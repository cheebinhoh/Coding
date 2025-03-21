# Copyright © 2025 Chee Bin HOH. All rights reserved.

cmake_minimum_required(VERSION 3.31)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dmn)

# set options
option(BUILD_NDEBUG "build without debug" OFF)

set(ABSL_PROPAGATE_CXX_STD ON)

include(CMakeMacro.cmake)
include(FetchContent)

enable_testing()

FetchContent_Declare(googletest
                       GIT_REPOSITORY https://github.com/google/googletest.git
                       GIT_TAG        v1.14.0
)

FetchContent_MakeAvailable(googletest)

FetchContent_Declare(protobuf
                       GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
                       GIT_TAG        v30.0-rc1
)

FetchContent_MakeAvailable(protobuf)

FetchContent_Declare(rdkafka
                       GIT_REPOSITORY https://github.com/confluentinc/librdkafka.git
                       GIT_TAG        v2.8.0
)

FetchContent_MakeAvailable(rdkafka)

string(REPLACE "-src" "-build" rdkafka_BUILD_DIR "${rdkafka_SOURCE_DIR}")

if(BUILD_NDEBUG)
  add_compile_definitions(NDEBUG)
endif()

# build hardware abstraction layer library
add_library(dmn
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-event.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-proc.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-socket.cpp

              # proto c++ sources
              ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-body.pb.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-type.pb.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg.pb.cc
)

target_sources(dmn
                 PRIVATE
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-debug.hpp
)

target_sources(dmn
                 INTERFACE
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-async.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-buffer.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-event.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-io.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-limit-buffer.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-dmesg.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-dmesgnet.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-dmesg-pb-util.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-pipe.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-proc.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-pub-sub.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-singleton.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-socket.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-teepipe.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-timer.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn.hpp

                 # proto c++ sources
                 ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-body.pb.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-type.pb.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg.pb.h
)

target_include_directories(dmn
                             INTERFACE
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${protobuf_SOURCE_DIR}/src
                             ${absl_SOURCE_DIR}
)

GENERATE_PROTOBUF(dmn
                    dmn-dmesg-type.proto
                    dmn-dmesg-body.proto
                    dmn-dmesg.proto
)

target_link_libraries(dmn
                        PRIVATE
                        protobuf::libprotobuf
)


# run unit test for DMN
add_subdirectory(test)


# build executable that uses dmn
add_executable(dmn-dmesg-daemon
                 dmn-dmesg-daemon.cpp
)

target_link_libraries(dmn-dmesg-daemon
                        PRIVATE
                        dmn
)

# Kafka
add_executable(dmn-kafka-receiver
                 dmn-kafka-receiver.cpp
                 kafka/dmn-kafka-util.cpp
)

target_include_directories(dmn-kafka-receiver
                             PRIVATE
                             ${CMAKE_CURRENT_SOURCE_DIR}/kafka
                             ${rdkafka_SOURCE_DIR}/src
)

target_link_libraries(dmn-kafka-receiver
                        PRIVATE
                        ${rdkafka_BUILD_DIR}/src/librdkafka.dylib
)

add_executable(dmn-kafka-sender
                 dmn-kafka-sender.cpp
                 kafka/dmn-kafka-util.cpp
)

target_include_directories(dmn-kafka-sender
                             PRIVATE
                             ${CMAKE_CURRENT_SOURCE_DIR}/kafka
                             ${rdkafka_SOURCE_DIR}/src
)

target_link_libraries(dmn-kafka-sender
                        PRIVATE
                        ${rdkafka_BUILD_DIR}/src/librdkafka.dylib
)

