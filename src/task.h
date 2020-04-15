#ifndef TASK_H
#define TASK_H

#include <sqlite3.h>

typedef struct{
    char task[1000];
    char date[25];
    char sql[150];
    sqlite3* db;
}Task_data;

int add_task(sqlite3* db, char* task);

int delete_task(sqlite3* db, char* date);

int update_task(sqlite3* db, char* task, char* date);

int add_category(sqlite3* db, char* category_name);

void show_task(sqlite3* db);

#endif