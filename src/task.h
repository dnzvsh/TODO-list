#ifndef TASK_H
#define TASK_H

#include <sqlite3.h>

int add_task(sqlite3* db, char* task);

int delete_task(sqlite3* db, char* date);

int update_task(sqlite3* db, char* task, char* date);

int add_category(sqlite3* db, char* category_name);

void show_task(sqlite3* db);

#endif