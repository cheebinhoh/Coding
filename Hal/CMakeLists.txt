cmake_minimum_required(VERSION 3.31)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hal)

include(CMakeMacro.cmake)
include(FetchContent)

enable_testing()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  protobuf
  GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
  GIT_TAG v30.0-rc1 # Replace with desired version
)

FetchContent_MakeAvailable(protobuf)

# options
option(BUILD_NDEBUG "build without debug" OFF)

if(BUILD_NDEBUG)
add_compile_definitions(NDEBUG)
endif()

# build hardware abstraction layer library
add_library(hal
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-proc.cpp

    # proto c++ sources
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/hal-dmesg-body.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/hal-dmesg-type.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/hal-dmesg.pb.cc
)

target_sources(hal
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-debug.hpp
)

target_sources(hal
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-async.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-io.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-limit-buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-dmesg.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-pipe.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-proc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-pub-sub.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-singleton.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-teepipe.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal.hpp

    # proto c++ sources
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/hal-dmesg-body.pb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/hal-dmesg-type.pb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/hal-dmesg.pb.h
)

target_include_directories(hal
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

GENERATE_PROTOBUF(hal 
                    hal-dmesg-type.proto
                    hal-dmesg-body.proto
                    hal-dmesg.proto)

target_link_libraries(hal
  PRIVATE
  protobuf::libprotobuf
)

# run unit test
add_subdirectory(test)

# build executable that uses hal
add_executable(hal-dmesg-daemon
               hal-dmesg-daemon.cpp)

target_link_libraries(hal-dmesg-daemon
                      PRIVATE
                      hal)
