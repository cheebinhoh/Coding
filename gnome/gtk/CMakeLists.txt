# Copyright © 2026 Chee Bin HOH. All rights reserved.

add_executable(init-gtk init-gtk.c)
target_link_libraries(init-gtk PRIVATE PkgConfig::GTK4)

add_test(
    NAME init-gtk
    COMMAND init-gtk
)

add_executable(hello-gtk hello-gtk.c)

target_include_directories(hello-gtk PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_directories(hello-gtk PRIVATE ${GTK4_LIBRARY_DIRS})
target_link_libraries(hello-gtk PRIVATE ${GTK4_LIBRARIES})
target_compile_options(hello-gtk PRIVATE ${GTK4_CFLAGS_OTHER})
