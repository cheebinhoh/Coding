/**
 * Copyright © 2026 Chee Bin HOH. All rights reserved.
 *
 * @file g-app.c
 * @brief g-application without GUI.
 */

#include <gio/gio.h>

static gboolean on_timeout(gpointer user_data) {
  static int count = 5;

  GApplication *app = G_APPLICATION(user_data);

  if (count < 1) {
    g_print("quitting...\n");
    g_application_quit(app);

    return G_SOURCE_REMOVE;
  } else {
    g_print("timer is fired, count down %d to zero...\n", count);
  }

  count--;

  return G_SOURCE_CONTINUE;
}

static void on_activate(GApplication *app, gpointer data) {
  g_print("Application started\n");

  g_application_hold(app);

  g_timeout_add(1000, on_timeout, app);
}

int main(int argc, char *argv[]) {
  GApplication *app = NULL;
  int status;

  app = g_application_new("com.cheebinhoh.g-app", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  status = g_application_run(app, argc, argv);
  g_object_unref(app);

  return status;
}
