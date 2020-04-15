#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int sql_request(Task_data* data)
{
    sqlite3_stmt* stmt;
    int i = 1;
    sqlite3_prepare_v2(data->db, data->sql, -1, &stmt, NULL);
    if (strcmp(data->task, " ")) {
        sqlite3_bind_text(stmt, i, data->task, -1, NULL);
        i++;
    }
    if (data->date) {
        sqlite3_bind_text(stmt, i, data->date, -1, NULL);
        i++;
    }
    int err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int add_task(sqlite3* db, char* task)
{
    if (!task) {
        return -3;
    }
    Task_data data;
    data.db = db;
    strcpy(data.sql, "INSERT INTO TODO (Task,Date) VALUES (?,?);");
    strcpy(data.task, task);
    const time_t sec = time(NULL);
    char* t = ctime(&sec);
    strcpy(data.date, t);
    int err = sql_request(&data);
    if (err) {
        return -2;
    }
    return 0;
}

int delete_task(sqlite3* db, char* date)
{
    Task_data data;
    data.db = db;
    strcpy(data.sql, "DELETE FROM TODO WHERE Date = ?;");
    strcpy(data.task, " ");
    strcpy(data.date, date);
    int err = sql_request(&data);
    if (err) {
        return -4;
    }
    return 0;
}

int update_task(sqlite3* db, char* task, char* date)
{
    if (!task) {
        return -5;
    }
    Task_data data;
    data.db = db;
    strcpy(data.sql, "UPDATE TODO SET Task = ? WHERE Date = ?;");
    strcpy(data.date, date);
    strcpy(data.task, task);
    int err = sql_request(&data);
    if (err) {
        return -6;
    }
    return 0;
}
/*
int add_category(sqlite3* db, char* category_name)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            db,
            "INSERT INTO CATEGORIES (category_name) VALUES (?);",
            -1,
            &stmt,
            NULL);
    sqlite3_bind_text(stmt, 1, category_name, -1, NULL);
    int err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}
*/
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

void parse_error(int err)
{
    switch (err) {
    case -1:
        printf("Ошибка в запросе\n");
    case -2:
        printf("Ошибка при добавлении задания\n");
    case -3:
        printf("Добавление пустого задания!\n");
    case -4:
        printf("Ошибка при удалении задания\n");
    case -5:
        printf("Обновление на пустое задание!\n");
    case -6:
        printf("Ошибка при обновлении задания!\n");
    }
}