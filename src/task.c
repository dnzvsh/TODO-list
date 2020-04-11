#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int add_task(sqlite3* db, char* task)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            db, "INSERT INTO TODO (Task,Date) VALUES (?,?);", -1, &stmt, NULL);
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

int delete_task(sqlite3* db, int id)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            db, "DELETE FROM TODO WHERE task_id = ?;", -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
    int err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        printf("%d\n", err);
        sqlite3_finalize(stmt);
        return -3;
    }
    sqlite3_finalize(stmt);
    /*char* error;
    sqlite3_exec(
            db,
            "SELECT row_number() over(order by task_id) as task_id "
            "from TODO;",
            NULL,
            0,
            &error);
    if (error != NULL) {
        printf("%s\n", error);
        sqlite3_free(error);
        return -2;
    }*/
    return 0;
}
