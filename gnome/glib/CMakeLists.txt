# Copyright © 2025 Chee Bin HOH. All rights reserved.

add_executable(hello-glib hello-glib.c)

target_include_directories(hello-glib PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_directories(hello-glib PRIVATE ${GLIB_LIBRARY_DIRS})
target_link_libraries(hello-glib PRIVATE ${GLIB_LIBRARIES})
target_compile_options(hello-glib PRIVATE ${GLIB_CFLAGS_OTHER})

add_test(
    NAME hello-glib
    COMMAND hello-glib
)
