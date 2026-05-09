# Copyright © 2026 Chee Bin HOH. All rights reserved.

add_executable(init-gtk init-gtk.c)

target_include_directories(init-gtk PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_directories(init-gtk PRIVATE ${GTK4_LIBRARY_DIRS})
target_link_libraries(init-gtk PRIVATE ${GTK4_LIBRARIES})
target_compile_options(init-gtk PRIVATE ${GTK4_CFLAGS_OTHER})

add_test(
    NAME init-gtk
    COMMAND init-gtk
)
