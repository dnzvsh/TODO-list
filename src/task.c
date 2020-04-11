#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int add_task(sqlite3* db, char* task)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            db, "INSERT INTO TODO (task,date) VALUES (?,?);", -1, &stmt, NULL);
    const time_t sec = time(NULL);
    char* t = ctime(&sec);
    sqlite3_bind_text(stmt, 1, task, -1, NULL);
    sqlite3_bind_text(stmt, 2, t, -1, NULL);
    int err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}
