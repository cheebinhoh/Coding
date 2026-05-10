# Copyright © 2026 Chee Bin HOH. All rights reserved.

add_executable(hello-glib hello-glib.c)
target_link_libraries(hello-glib PRIVATE PkgConfig::GLIB)

add_test(
    NAME hello-glib
    COMMAND hello-glib
)

add_executable(g-mainloop-app g-mainloop-app.c)
target_link_libraries(g-mainloop-app PRIVATE PkgConfig::GLIB)

add_test(
    NAME g-mainloop-app
    COMMAND g-mainloop-app
)
