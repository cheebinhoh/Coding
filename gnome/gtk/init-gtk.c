/**
 * Copyright © 2026 Chee Bin HOH. All rights reserved.
 *
 * @file init-gtk.c
 * @brief Program to init gtk and check gtk library is setup.
 */

#include <gtk/gtk.h>

int main(int argc,char *argv[]) {
  gtk_init();

  return 0;
}
