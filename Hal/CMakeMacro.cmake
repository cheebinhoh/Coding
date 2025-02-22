macro(ADD_TEST_EXECUTABLE ...)
  foreach (arg ${ARGN}) 
    message("adding hal test executable ${arg}")

    add_executable(${arg}
                   ${arg}.cpp)

    target_link_libraries(${arg}
                          PRIVATE
                          gtest_main
                          gtest
                          hal)

    add_test(NAME ${arg} COMMAND ${arg})
  endforeach()
endmacro()

macro(GENERATE_PROTOBUF ...)
  foreach(arg ${ARGN})
    target_include_directories(${ARGV0} PRIVATE
                               ${CMAKE_CURRENT_BINARY_DIR})

    protobuf_generate(TARGET ${ARGV0}
                      PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/proto/${arg}
                      LANGUAGE cpp
                      OUT_VAR PROTOBUF_GENERATED_FILES
                      PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
  endforeach()
endmacro()
