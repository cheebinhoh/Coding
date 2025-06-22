# Copyright © 2025 Chee Bin HOH. All rights reserved.

# set project and generic compilation options
cmake_minimum_required(VERSION 3.26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ABSL_PROPAGATE_CXX_STD ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

project(dmn)

enable_testing()


# set build options
option(BUILD_NDEBUG "build without debug" OFF)
option(BUILD_KAFKA_TEST "build with kafka test" OFF)


# include cmake macro and fetch dependent modules
include(CMakeMacro.cmake)
include(FetchContent)

message(STATUS "find or fetching googletest...")

FetchContent_Declare(googletest
                       GIT_REPOSITORY https://github.com/google/googletest.git
                       GIT_TAG        v1.14.0
)

FetchContent_MakeAvailable(googletest)

message(STATUS "find or fetching protobuf...")
find_package(Protobuf QUIET)

if (NOT Protobuf_FOUND)
FetchContent_Declare(protobuf
                       GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
                       GIT_TAG        v30.0-rc1
)

FetchContent_MakeAvailable(protobuf)
endif()

message(STATUS "fetching rdkafka...")
FetchContent_Declare(rdkafka
                       GIT_REPOSITORY https://github.com/confluentinc/librdkafka.git
                       GIT_TAG        v2.8.0
)

FetchContent_MakeAvailable(rdkafka)

string(REPLACE "-src" "-build" rdkafka_BUILD_DIR "${rdkafka_SOURCE_DIR}")
set(rdkafka_INCLUDE ${rdkafka_SOURCE_DIR}/src)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    set(rdkafka_LIB ${CMAKE_BINARY_DIR}/_deps/rdkafka-build/src/librdkafka.dylib)
else()
    message(STATUS "Running on none-macOS")
    set(rdkafka_LIB ${CMAKE_BINARY_DIR}/_deps/rdkafka-build/src/librdkafka.so)
endif()

message(STATUS "kakfa library reference is set to ${rdkafka_LIB}")

set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}/_deps/rdkafka-build/src")
set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/_deps/rdkafka-build/src")

if(BUILD_NDEBUG)
  add_compile_definitions(NDEBUG)
endif()


# build the dmn library
message(STATUS "build dmn...")
add_library(dmn
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-async.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-dmesg.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-dmesgnet.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-event.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-proc.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/dmn-socket.cpp

              # kafka
              ${CMAKE_CURRENT_SOURCE_DIR}/kafka/dmn-dmesgnet-kafka.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/kafka/dmn-kafka-util.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/kafka/dmn-kafka.cpp

              # proto c++ sources
              ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-body.pb.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-type.pb.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg.pb.cc
)

target_compile_options(dmn PRIVATE -Wall -Wextra -Wpedantic)
set_target_properties(dmn PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_dependencies(dmn rdkafka)
target_sources(dmn
                 PRIVATE
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn-debug.hpp
)

target_sources(dmn
                 INTERFACE
                 ${CMAKE_CURRENT_SOURCE_DIR}/dmn.hpp
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

                 # kafka
                 ${CMAKE_CURRENT_SOURCE_DIR}/kafka/dmn-dmesgnet-kafka.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/kafka/dmn-kafka-util.hpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/kafka/dmn-kafka.hpp

                 # proto c++ sources
                 ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-body.pb.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg-type.pb.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/proto/dmn-dmesg.pb.h
)

target_include_directories(dmn
                             INTERFACE
                             ${protobuf_SOURCE_DIR}/src
                             ${absl_SOURCE_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${rdkafka_INCLUDE}
)

target_include_directories(dmn
                             PRIVATE
                             ${rdkafka_INCLUDE}
)

GENERATE_PROTOBUF(dmn
                    dmn-dmesg-type.proto
                    dmn-dmesg-body.proto
                    dmn-dmesg.proto
)

target_link_libraries(dmn
                        PRIVATE
                        protobuf::libprotobuf
                        ${rdkafka_LIB}
)


# run unit test for DMN
message(STATUS "build and run unit test...")
add_subdirectory(test)


# build executable that uses dmn
message(STATUS "build dmn applications...")
add_executable(dmn-dmesg-daemon
                 dmn-dmesg-daemon.cpp
)

target_link_libraries(dmn-dmesg-daemon
                        PRIVATE
                        dmn
                        ${rdkafka_LIB}
)


# build executable that uses dmn kafka
add_executable(dmn-kafka-receiver
                 dmn-kafka-receiver.cpp
)

target_include_directories(dmn-kafka-receiver
                             PRIVATE
                             dmn
                             ${rdkafka_LIB}
)

target_link_libraries(dmn-kafka-receiver
                        PRIVATE
                        dmn
                        ${rdkafka_LIB}
)

add_executable(dmn-kafka-sender
                 dmn-kafka-sender.cpp
)

target_include_directories(dmn-kafka-sender
                             PRIVATE
                             dmn
                             ${rdkafka_LIB}
)

target_link_libraries(dmn-kafka-sender
                        PRIVATE
                        dmn
                        ${rdkafka_LIB}
)

