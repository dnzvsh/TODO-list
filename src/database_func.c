#include "database_func.h"
#include <stdio.h>
#include <time.h>

int sql_request(Task_data* data)
{
    sqlite3_stmt* stmt;
    int i = 1;
    sqlite3_prepare_v2(data->db, data->sql, -1, &stmt, NULL);
    if (data->argv[0] == TASK) {
        if (data->argv[1] != TASK_DELETE) {
            sqlite3_bind_text(stmt, i, data->task, -1, NULL);
            i++;
        }
        if (data->date) {
            sqlite3_bind_text(stmt, i, data->date, -1, NULL);
            i++;
        }
    }
    if (data->argv[0] == CATEGORY) {
        if (data->argv[1] != CATEGORY_BIND) {
            if (data->argv[1] == CATEGORY_UPDATE) {
                sqlite3_bind_text(stmt, i, data->new_category_name, -1, NULL);
                i++;
            }
            sqlite3_bind_text(stmt, i, data->category_name, -1, NULL);
            i++;
        } else {
            sqlite3_bind_int(stmt, i, data->category_id);
            i++;
            sqlite3_bind_int(stmt, i, data->task_id);
        }
    }
    int err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        if (err == SQLITE_CONSTRAINT) {
            sqlite3_finalize(stmt);
            return -15;
        }
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}

int add_task(Task_data* data)
{
    data->argv[0] = TASK;
    strcpy(data->sql, "INSERT INTO TODO (Task,Date) VALUES (?,?);");
    const time_t sec = time(NULL);
    char* t = ctime(&sec);
    strcpy(data->date, t);
    if (strlen(data->task) == 0) {
        return -3;
    }
    int err = sql_request(data);
    if (err) {
        return -2;
    }
    return 0;
}

int delete_task(Task_data* data)
{
    data->argv[0] = TASK;
    data->argv[1] = TASK_DELETE;
    strcpy(data->sql, "DELETE FROM TODO WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        return -4;
    }
    return 0;
}

int update_task(Task_data* data)
{
    data->argv[0] = TASK;
    data->argv[1] = TASK_UPDATE;
    if (strlen(data->task) == 0) {
        return -5;
    }
    strcpy(data->sql, "UPDATE TODO SET Task = ? WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        return -6;
    }
    return 0;
}

int add_category(Task_data* data)
{
    data->argv[0] = CATEGORY;
    data->argv[1] = CATEGORY_ADD;
    strcpy(data->sql, "INSERT INTO CATEGORIES (category_name) VALUES (?);");
    if (strlen(data->category_name) == 0) {
        return -8;
    }
    int err = sql_request(data);
    if (err) {
        return -9;
    }
    return 0;
}

int delete_category(Task_data* data)
{
    data->argv[0] = CATEGORY;
    data->argv[1] = CATEGORY_DELETE;
    strcpy(data->sql, "DELETE FROM CATEGORIES WHERE category_name = ?;");
    int err = sql_request(data);
    if (err) {
        if (err == -15) {
            return -15;
        }
        return -11;
    }
    return 0;
}

int update_category(Task_data* data)
{
    data->argv[0] = CATEGORY;
    data->argv[1] = CATEGORY_UPDATE;
    if (strlen(data->new_category_name) == 0) {
        return -12;
    }
    strcpy(data->sql,
           "UPDATE CATEGORIES SET category_name = ? WHERE category_name = ?;");
    int err = sql_request(data);
    if (err) {
        return -13;
    }
    return 0;
}

void get_category_id(Task_data* data)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            data->db,
            "SELECT category_id FROM CATEGORIES WHERE category_name = ?;",
            -1,
            &stmt,
            NULL);
    sqlite3_bind_text(stmt, 1, data->category_name, -1, NULL);
    sqlite3_step(stmt);
    data->category_id = (int)sqlite3_column_int(stmt, 0);
}

void get_task_id(Task_data* data)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            data->db,
            "select task_id from TODO WHERE Date = ?;",
            -1,
            &stmt,
            NULL);
    sqlite3_bind_text(stmt, 1, data->date, -1, NULL);
    sqlite3_step(stmt);
    data->task_id = (int)sqlite3_column_int(stmt, 0);
}

int bind_category_for_task(Task_data* data)
{
    data->argv[0] = CATEGORY;
    data->argv[1] = CATEGORY_BIND;
    get_category_id(data);
    get_task_id(data);
    // printf("%d %d\n", data->category_id, data->task_id);
    if (data->category_id == 0 || data->task_id == 0) {
        return -14;
    }
    strcpy(data->sql, "UPDATE TODO SET category_id = ? WHERE task_id = ?");
    int err = sql_request(data);
    if (err) {
        return -10;
    }
    return 0;
}

int task_score(sqlite3* db)
{
    sqlite3_stmt* stmt;
    int j = 0;
    sqlite3_prepare_v2(db, "select Task from TODO;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        j++;
    }
    return j;
}

int category_score(sqlite3* db)
{
    sqlite3_stmt* stmt;
    int j = 0;
    sqlite3_prepare_v2(
            db, "select category_name from CATEGORIES;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        j++;
    }
    return j;
}

int show_task(sqlite3* db, char label_main[][1000], char label_date[][26])
{
    sqlite3_stmt* stmt;
    int j = 0;
    sqlite3_prepare_v2(db, "select Task,Date from TODO;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        strcpy(label_main[j], (char*)sqlite3_column_text(stmt, 0));
        strcpy(label_date[j], (char*)sqlite3_column_text(stmt, 1));
        j++;
    }
    sqlite3_finalize(stmt);
    return j;
}

int show_category(sqlite3* db, char label_main[][100])
{
    sqlite3_stmt* stmt;
    int j = 0;
    sqlite3_prepare_v2(
            db, "SELECT category_name FROM CATEGORIES;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        strcpy(label_main[j], (char*)sqlite3_column_text(stmt, 0));
        j++;
    }
    sqlite3_finalize(stmt);
    return j;
}