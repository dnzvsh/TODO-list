#ifndef TASK_H
#define TASK_H

#include <sqlite3.h>

typedef struct {
    char task[1000];
    char date[26];
    char sql[150];
    sqlite3* db;
    char argv[10];
} Task_data;

void read_data(Task_data* data, char* argv);

void parse_error(int err);

int add_task(Task_data* data);

int delete_task(Task_data* data);

int update_task(Task_data* data);

int add_category(sqlite3* db, char* category_name);

void show_task(sqlite3* db);

#endif