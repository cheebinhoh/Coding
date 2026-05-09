# Copyright © 2026 Chee Bin HOH. All rights reserved.

add_executable(hello-glib hello-glib.c)
target_link_libraries(hello-glib PRIVATE PkgConfig::GLIB)

add_test(
    NAME hello-glib
    COMMAND hello-glib
)
