#ifndef INTERFACE_H
#define INTERFACE_H

#include "database_func.h"
#include <gtk/gtk.h>
#include <sqlite3.h>

#define EDIT_TASK 101
#define ADD_TASK 100
#define DELETE_TASK 200
#define IS_MAIN 10
#define IS_BIND 0

typedef struct {
    Task_data task;
    GtkBuilder* builder;
    int rc;
    int index;
    int action;
    GtkBuilder* builder_window;
    GtkBuilder* builder_window_category;
    GtkBuilder* builder_add_window_category;
    int is_main;
} GUI;

void read_labels(
        GtkLabel* label_main, GtkLabel* label_date, int num, GUI* interface);

void initialize_edit_button(GUI* interface);

int open_view_window(GtkWidget* widget, gpointer data);

void update_main_window(GUI* data);

void add_task_click(GtkWidget* widget, gpointer user_data);

void open_add_window(GtkWidget* widget, gpointer data);

int open_view_window(GtkWidget* widget, gpointer data);

void delete_task_click(GtkWidget* widget, gpointer user_data);

void update_task_click(GtkWidget* widget, gpointer user_data);

void show_error(int err);

int task_score(sqlite3* db);

void open_category_window(GtkWidget* widget, gpointer user_data);

void add_category_window(GtkWidget* widget, gpointer user_data);

void update_category_window(GUI* data);

void show_category_on_add(GtkWidget* widget, gpointer user_data);

void delete_category_click(GtkWidget* widget, gpointer user_data);

#endif
