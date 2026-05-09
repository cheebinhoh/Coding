# Copyright © 2026 Chee Bin HOH. All rights reserved.

cmake_minimum_required(VERSION 3.16)

project(Coding C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB IMPORTED_TARGET glib-2.0)
pkg_check_modules(GIO IMPORTED_TARGET gio-2.0)
pkg_check_modules(GTK4 IMPORTED_TARGET gtk4)

# C Coding Library
add_library(btree SHARED
    btree.c
    avlbstree.c
    llist.c
)

target_include_directories(btree PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(dgraph STATIC
    dgraph.c
)

target_include_directories(dgraph PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(llist STATIC
    llist.c
)

target_include_directories(llist PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(search-sort STATIC
    search-sort.c
)

target_include_directories(search-sort PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# C Coding Executables
add_executable(bigo-sample.out bigo-sample.c)

add_executable(btreebltraverse.out btreebltraverse.c)
target_link_libraries(btreebltraverse.out PRIVATE btree)

add_executable(btreebuild.out
    btreebuild.c
    btreetraverse.c
)
target_link_libraries(btreebuild.out PRIVATE btree)

add_executable(btreeidentical.out btreeidentical.c)
target_link_libraries(btreeidentical.out PRIVATE btree)

add_executable(btreeisbalanced.out btreeisbalanced.c)
target_link_libraries(btreeisbalanced.out PRIVATE btree)

add_executable(btreelca.out
    btreelca.c
    btreetraverse.c
)
target_link_libraries(btreelca.out PRIVATE btree)

add_executable(btreemaxminlevel.out btreemaxminlevel.c)
target_link_libraries(btreemaxminlevel.out PRIVATE btree)

add_executable(btreemaxnodeinlevel.out btreemaxnodeinlevel.c)
target_link_libraries(btreemaxnodeinlevel.out PRIVATE btree)

add_executable(btreemaxpathsum.out btreemaxpathsum.c)
target_link_libraries(btreemaxpathsum.out PRIVATE btree)

add_executable(btreemaxsumpathbetween2leaves.out btreemaxsumpathbetween2leaves.c)
target_link_libraries(btreemaxsumpathbetween2leaves.out PRIVATE btree)

add_executable(btreemaxsumpathtoleaf.out btreemaxsumpathtoleaf.c)
target_link_libraries(btreemaxsumpathtoleaf.out PRIVATE btree)

add_executable(btreemirrorswap.out btreemirrorswap.c)
target_link_libraries(btreemirrorswap.out PRIVATE btree)

add_executable(btreepathsum.out btreepathsum.c)
target_link_libraries(btreepathsum.out PRIVATE btree)

add_executable(btreerebalancing.out btreerebalancing.c)
target_link_libraries(btreerebalancing.out PRIVATE btree)

add_executable(btreesubtree.out btreesubtree.c)
target_link_libraries(btreesubtree.out PRIVATE btree)

add_executable(btreesymmetriccheck.out btreesymmetriccheck.c)
target_link_libraries(btreesymmetriccheck.out PRIVATE btree)

add_executable(btreetraverse.out
    btreetraverse.c
    btreetraverse-main.c
)
target_link_libraries(btreetraverse.out PRIVATE btree)

add_executable(btreeverticalsum.out btreeverticalsum.c)
target_link_libraries(btreeverticalsum.out PRIVATE btree)

add_executable(btree-test.out btree-test.c)
target_link_libraries(btree-test.out PRIVATE btree)

add_executable(calculator.out calculator.c)

add_executable(coding-test.out coding-test.c)
target_link_libraries(coding-test.out PRIVATE btree search-sort)

add_executable(coding-test-2.out coding-test-2.c)
target_link_libraries(coding-test-2.out PRIVATE btree llist search-sort)

add_executable(cntdown.out cntdown.c)

add_executable(dgraph-test.out dgraph-test.c)
target_link_libraries(dgraph-test.out PRIVATE dgraph llist)

add_executable(find2ndMaxNumber.out find2ndMaxNumber.c)

add_executable(fold-line.out fold-line.c)

add_executable(llist-test.out llist-test.c)
target_link_libraries(llist-test.out PRIVATE llist)

add_executable(remove-c-comment.out remove-c-comment.c)

add_executable(replace-tab-with-space.out replace-tab-with-space.c)

add_executable(search-sort.out search-sort-main.c)
target_link_libraries(search-sort.out PRIVATE search-sort)

add_executable(string-utility.out string-utility.c)

add_executable(shrink-space.out shrink-space.c)

add_executable(syntax-validate-parenthese.out syntax-validate-parenthese.c)

add_executable(trafficdemand.out trafficdemand.c)

add_executable(transformStr2Str.out transformStr2Str.c)

add_executable(tree2btreebuild.out
    tree2btreebuild.c
    btreetraverse.c
)
target_link_libraries(tree2btreebuild.out PRIVATE btree)

add_executable(trim-space.out trim-space.c)

# Other Coding Subdirectories
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/C++/CMakeLists.txt")
    add_subdirectory("C++")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Linux/CMakeLists.txt")
    add_subdirectory("Linux")
endif()

if (GLIB_FOUND)
    add_subdirectory("gnome/glib")
else()
    message(STATUS "glib-2.0 is not found; skipping subdirectory 'gnome/glib'")
endif()

if (GTK4_FOUND)
    add_subdirectory("gnome/gtk")
else()
    message(STATUS "gtk4 is not found; skipping subdirectory 'gnome/gtk'")
endif()

if (GIO_FOUND) 
    add_subdirectory("gnome/gio")
else()
    message(STATUS "Gio is not found; skipping subdirectory 'gnome/gio'")
endif()

# C Coding Test
add_test(
    NAME transformStr2Str_test
    COMMAND sh -c "./transformStr2Str.out -d | diff - ${CMAKE_CURRENT_SOURCE_DIR}/transformStr2Str_result.txt"
)

add_test(
    NAME coding_test
    COMMAND sh -c "./coding-test.out | diff - ${CMAKE_CURRENT_SOURCE_DIR}//coding-test_result.txt"
)

add_test(
    NAME coding_test_2
    COMMAND sh -c "./coding-test-2.out | diff - ${CMAKE_CURRENT_SOURCE_DIR}/coding-test-2_result.txt"
)

add_test(
    NAME btree_group_test
    COMMAND sh -c "for file in `echo *tree*.out | sort`; do echo \"run \${file}\"; echo; ./\${file}; done | diff - ${CMAKE_CURRENT_SOURCE_DIR}/btree_result.txt"
)
