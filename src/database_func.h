#ifndef DATABASE_FUNCTION_H
#define DATABASE_FUNCTION_H

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
    GtkBuilder* builder;
    int rc;
    int index;
    GtkBuilder* builder_window;
} Task_data;

int show_task(sqlite3* db, char label_main[][1000], char label_date[][26]);

int update_task(Task_data* data);

void parse_error(int err);

int delete_task(Task_data* data);

int add_task(Task_data* data);

#endif