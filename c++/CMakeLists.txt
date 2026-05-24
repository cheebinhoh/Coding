cmake_minimum_required(VERSION 3.16)

project(CodingCppExamples LANGUAGES CXX)

# Optional: keep outputs in the build dir root (so you see *.out there)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

function(add_cpp_prog target standard)
  add_executable(${target} ${ARGN})
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD ${standard}
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
  )
endfunction()

add_cpp_prog(exception-vs-std-optional.out 17 exception-vs-std-optional.cpp)
add_cpp_prog(lambda-functor.out 14 lambda-functor.cpp)
add_cpp_prog(lOrRValue.out 11 lOrRValue.cpp)
add_cpp_prog(pointer-to-member.out 20 pointer-to-member.cpp)
add_cpp_prog(ring-buffer-for-multiple-writers-n-one-reader.out 14 ring-buffer-for-multiple-writers-n-one-reader.cpp)
add_cpp_prog(singleton-design-pattern.out 14 singleton-design-pattern.cpp)
add_cpp_prog(smart-pointer-with-object-graph.out 17 smart-pointer-with-object-graph.cpp)

add_cpp_prog(std-array.out 14 std-array.cpp)
add_cpp_prog(std-chrono.out 14 std-chrono.cpp)
add_cpp_prog(std-containers-and-algorithms.out 17 std-containers-and-algorithms.cpp)
add_cpp_prog(std-function-object.out 17 std-function-object.cpp)

add_cpp_prog(std-future-async.out 17 std-future-async.cpp)
add_cpp_prog(std-future-packaged-task.out 17 std-future-packaged-task.cpp)
add_cpp_prog(std-future-promise.out 17 std-future-promise.cpp)

add_cpp_prog(std-map.out 14 std-map.cpp)
add_cpp_prog(std-pair.out 14 std-pair.cpp)
add_cpp_prog(std-priority-queue.out 14 std-priority-queue.cpp)
add_cpp_prog(std-reference-in-std-container.out 17 std-reference-in-std-container.cpp)

add_cpp_prog(std-thread-with-multiple-readers-n-one-writer.out 14 std-thread-with-multiple-readers-n-one-writer.cpp)
add_cpp_prog(std-thread-with-multiple-worker-threads.out 14 std-thread-with-multiple-worker-threads.cpp)

add_cpp_prog(std-tuple.out 14 std-tuple.cpp)

add_cpp_prog(test-template-class.out 14 test-template-class.cpp template-class.cpp)
