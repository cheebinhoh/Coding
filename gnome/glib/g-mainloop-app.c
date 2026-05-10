/**
 * Copyright © 2026 Chee Bin HOH. All rights reserved.
 *
 * @file g-mainloop-app.c
 * @brief glib mainloop app with timer count down from 5 to 1.
 */

#include <glib.h>

static gboolean on_timeout(gpointer user_data) {
  GMainLoop *mainloop = (GMainLoop *)user_data;

  static int count = 5;

  if (count < 1) {
    g_print("quitting...\n");
    g_main_loop_quit(mainloop);

    return G_SOURCE_REMOVE;
  } else {
    g_print("timer is fired, count down %d to zero...\n", count);
  }

  count--;

  return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]) {
  GMainLoop *mainloop = NULL;

  mainloop = g_main_loop_new(NULL, FALSE);

  g_timeout_add(1000, on_timeout, mainloop);

  g_main_loop_run(mainloop);

  g_main_loop_unref(mainloop);

  return 0;
}
