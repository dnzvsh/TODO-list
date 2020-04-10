#ifndef TASK_H
#define TASK_H

#include <sqlite3.h>

int add_task(sqlite3* db, char* task);

#endif