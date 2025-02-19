cmake_minimum_required(VERSION 3.31)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hal)

include(CMakeMacro.cmake)

add_library(hal
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-proc.cpp
)

target_sources(hal
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-async.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-io.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-limit-buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-pipe.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-proc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-pub-sub.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-singleton.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-teepipe.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal-timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal.hpp
)

target_include_directories(hal
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(test)

add_executable(hal-dm
               hal-dm.cpp)

target_link_libraries(hal-dm
                      PRIVATE
                      hal)
