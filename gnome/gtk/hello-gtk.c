/**
 * Copyright © 2026 Chee Bin HOH. All rights reserved.
 *
 * @file hello-world-gtk.c
 * @brief First hello world with GTK toolkit
 */

#include <gtk/gtk.h>

void print_hello(GtkWidget *widget, gpointer data) {
  g_print("Hello World from Chee Bin!\n");
}
             
static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window = NULL;
  GtkWidget *button = NULL;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Hello World");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

  button = gtk_button_new_with_label("Hello World");
  gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
  g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
  gtk_window_set_child(GTK_WINDOW(window), button);

  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
  GtkApplication *app = NULL;
  int status = 0;
  
  app = gtk_application_new("com.cheebinhoh.helloHelloGtk", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
