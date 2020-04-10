#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int add_task(sqlite3* db, char* task)
{
    char* err = 0;
    char add[1100] = "INSERT INTO TODO (task,date) VALUES ('";
    char* sql = &add[0];
    const time_t sec = time(NULL);
    char* t = ctime(&sec);
    sql = strncat(sql, task, 1000);
    sql = strncat(sql, "','", 4);
    sql = strncat(sql, t, 30);
    sql = strncat(sql, "');", 4);
    printf("%s\n", sql);
    int rc = sqlite3_exec(db, sql, 0, 0, &err);
    if (rc != SQLITE_OK) {
        sqlite3_free(err);
        return -1;
    }
    return 0;
}
