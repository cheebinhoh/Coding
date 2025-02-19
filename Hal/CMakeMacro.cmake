macro(ADD_TEST_EXECUTABLE ...)
  foreach (arg ${ARGN}) 
    message("adding hal test executable ${arg}")

    add_executable(${arg}
                   ${arg}.cpp)

    target_link_libraries(${arg}
                          PRIVATE
                          hal)
  endforeach()
endmacro()
