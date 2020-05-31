#ifndef DATABASE_FUNCTION_H
#define DATABASE_FUNCTION_H

#define DATABASE_PATH "src/database.db"

#define TODO                                                                  \
    "PRAGMA foreign_keys=on;CREATE TABLE IF NOT EXISTS TODO(task_id integer " \
    "primary key autoincrement, category_id integer, Task TEXT, Date "        \
    "TEXT,Foreign key (category_id) References CATEGORIES(category_id));"
#define CATEGORIES                                                           \
    "CREATE TABLE IF NOT EXISTS CATEGORIES(category_id integer primary key " \
    "autoincrement, category_name TEXT UNIQUE);"

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
    sqlite3* db;
    char task[1000];
    char date[26];
    char sql[150];
    char category_name[100];
    char new_category_name[100];
    int argv[2];
    int category_id;
    int task_id;
    int task_category_id;
} Task_data;

int database_open(sqlite3** db);

int show_task(sqlite3* db, char label_main[][1000], char label_date[][26]);

int update_task(Task_data* data);

int delete_task(Task_data* data);

int add_task(Task_data* data);

int add_category(Task_data* data);

int bind_category_for_task(Task_data* data);

int delete_category(Task_data* data);

int update_category(Task_data* data);

int category_score(sqlite3* db);

int task_score(sqlite3* db);

int show_category(sqlite3* db, char label_main[][100]);

int show_task_with_category(Task_data* data, char label_main[][1000]);

#endif