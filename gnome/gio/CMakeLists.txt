# Copyright © 2026 Chee Bin HOH. All rights reserved.

add_executable(g-app g-app.c)

target_link_libraries(g-app PRIVATE PkgConfig::GIO)

add_test(
    NAME g-mainloop-app
    COMMAND g-mainloop-app
)
