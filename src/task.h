#ifndef TASK_H
#define TASK_H

#include <sqlite3.h>

int add_task(sqlite3* db, char* task);

int delete_task(sqlite3* db, int id);

int update_task(sqlite3* db, char* task, char* date);

void show_task(sqlite3* db);

#endif