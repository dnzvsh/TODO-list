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

#define TASK 10
#define CATEGORY 20
#define TASK_ADD 0
#define TASK_DELETE 1
#define TASK_UPDATE 2
#define CATEGORY_ADD 3
#define CATEGORY_DELETE 4
#define CATEGORY_UPDATE 5
#define CATEGORY_BIND 6

#include <gtk/gtk.h>
#include <sqlite3.h>

typedef struct {
    char task[1000];
    char date[26];
    char sql[150];
    char category_name[100];
    int argv[2];
    sqlite3* db;
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

int add_category(Task_data* data);

int bind_category_for_task(Task_data* data);

int delete_category(Task_data* data);

int update_category(Task_data* data);

#endif