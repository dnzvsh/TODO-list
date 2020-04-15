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
        sqlite3_finalize(stmt);
        return -2;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int update_task(sqlite3* db, char* task, char* date)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            db, "UPDATE TODO SET Task = ? WHERE Date = ? ", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, task, -1, NULL);
    sqlite3_bind_text(stmt, 2, date, -1, NULL);
    int err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -3;
    }
    sqlite3_finalize(stmt);
    return 0;
}

void show_task(sqlite3* db)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "select * from TODO;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int i;
        int num_cols = sqlite3_column_count(stmt);
        for (i = 0; i < num_cols; i++) {
            switch (sqlite3_column_type(stmt, i)) {
            case (SQLITE3_TEXT):
                printf("%s ", sqlite3_column_text(stmt, i));
                break;
            case (SQLITE_INTEGER):
                printf("%d ", sqlite3_column_int(stmt, i));
                break;
            default:
                break;
            }
        }
        printf("\n");
    }

    sqlite3_finalize(stmt);
}