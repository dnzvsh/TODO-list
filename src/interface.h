#ifndef INTERFACE_H
#define INTERFACE_H

#include "database_func.h"
#include <gtk/gtk.h>
#include <sqlite3.h>

typedef struct {
    Task_data task;
    GtkBuilder* builder;
    int rc;
    int index;
    GtkBuilder* builder_window;
} GUI;

void read_labels(
        GtkLabel* label_main, GtkLabel* label_date, int num, GUI* interface);

void initialize_edit_button(GtkWidget* widget, gpointer user_data);

int open_view_window(GtkWidget* widget, gpointer data);

void update_main_window(GUI* data);

void add_task_click(GtkWidget* widget, gpointer user_data);

int open_add_window(GtkWidget* widget, gpointer data);

int open_view_window(GtkWidget* widget, gpointer data);

void delete_task_click(GtkWidget* widget, gpointer user_data);

void update_task_click(GtkWidget* widget, gpointer user_data);

void show_error(int err);

#endif
