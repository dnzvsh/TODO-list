#ifndef INTERFACE_H
#define INTERFACE_H

#include "database_func.h"
#include <gtk/gtk.h>
#include <sqlite3.h>

void read_labels(
        GtkLabel* label_main, GtkLabel* label_date, int num, Task_data* i);

void initialize_edit_button(GtkWidget* widget, gpointer user_data);

int open_view_window(GtkWidget* widget, gpointer data);

void update_main_window(Task_data* data);

// void read_data(Task_data* data, char* argv);

void add_task_click(GtkWidget* widget, gpointer user_data);

int open_add_window(GtkWidget* widget, gpointer data);

int open_view_window(GtkWidget* widget, gpointer data);

void delete_task_click(GtkWidget* widget, gpointer user_data);

void update_task_click(GtkWidget* widget, gpointer user_data);

#endif
