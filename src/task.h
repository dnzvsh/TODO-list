#ifndef TASK_H
#define TASK_H
#define DATABASE_PATH "src/database.db"

#define TODO                                                       \
    "CREATE TABLE IF NOT EXISTS TODO(task_id integer primary key " \
    "autoincrement, Task TEXT, Date TEXT);"
#define CATEGORIES                                                           \
    "CREATE TABLE IF NOT EXISTS CATEGORIES(category_id integer primary key " \
    "autoincrement, task_id integer,category_name TEXT,Foreign key "         \
    "(category_id) References TODO(task_id));"

#include <gtk/gtk.h>
#include <sqlite3.h>

typedef struct {
    char task[1000];
    char date[26];
    char sql[150];
    sqlite3* db;
    char argv[10];
} Task_data;

void read_data(Task_data* data, char* argv);

void show_task_on_add(GtkWidget* widget, gpointer user_data);

int add_db(sqlite3* db);

void parse_error(int err);

int add_task(GtkWidget* widget, gpointer user_data);

int open_add_window(GtkWidget* widget, gpointer data);

int delete_task(Task_data* data);

int update_task(Task_data* data);

int add_category(sqlite3* db, char* category_name);

int show_task(GtkBuilder* builder);

#endif