macro(ADD_TEST_EXECUTABLE ...)
  foreach (arg ${ARGN}) 
    message("adding dmn test executable ${arg}")

    add_executable(${arg}
                   ${arg}.cpp)

    target_link_libraries(${arg}
                          PRIVATE
                          gtest_main
                          gtest
                          dmn 
                          protobuf::libprotobuf
                         )

    add_test(NAME ${arg} COMMAND ${arg})
    set_tests_properties(${arg} PROPERTIES LABELS "dmn")
  endforeach()
endmacro()

macro(GENERATE_PROTOBUF ...)
  foreach(arg ${ARGN})
    target_include_directories(${ARGV0} PRIVATE
                               ${CMAKE_CURRENT_SOURCE_DIR})

    protobuf_generate(TARGET ${ARGV0}
                      PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/proto/${arg}
                      LANGUAGE cpp
                      OUT_VAR PROTOBUF_GENERATED_FILES
                      PROTOC_OPTIONS "-I${protobuf_SOURCE_DIR}/src"
                      PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
  endforeach()
endmacro()
