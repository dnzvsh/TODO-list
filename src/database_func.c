#include "database_func.h"
#include <stdio.h>
#include <time.h>

int sql_request(Task_data* data)
{
    sqlite3_stmt* stmt;
    int i = 1;
    sqlite3_prepare_v2(data->db, data->sql, -1, &stmt, NULL);
    if (strcmp(data->argv, "delete")) {
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

int add_task(Task_data* data)
{
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
    // gtk_widget_hide();
    return 0;
}

int delete_task(Task_data* data)
{
    strcpy(data->argv, "delete");
    strcpy(data->sql, "DELETE FROM TODO WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        return -4;
    }
    return 0;
}

int update_task(Task_data* data)
{
    strcpy(data->argv, "update");
    if (strlen(data->task) == 0) {
        return -5;
    }
    strcpy(data->sql, "UPDATE TODO SET Task = ? WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        g_print("error\n");
        return -6;
    }
    return 0;
}

int add_category(Task_data* data) 
{
    sqlite3_stmt* stmt;
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

int add_category_for_task(Task_data* data)
{
    sqlite3_stmt* stmt;
    strcpy(data->argv, "updateC");
    strcpy(data->sql, "SELECT * FROM TODO INNER JOIN CATEGORIES ON (task_id) = (category_id);");
    int err = sql_request(data);
    if (err) {
        return -10;
    }
    return 0;
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

void parse_error(int err)
{
    switch (err) {
    case -1:
        printf("Ошибка в запросе\n");
        break;
    case -2:
        printf("Ошибка при добавлении задания\n");
        break;
    case -3:
        printf("Добавление пустого задания!\n");
        break;
    case -4:
        printf("Ошибка при удалении задания\n");
        break;
    case -5:
        printf("Обновление на пустое задание!\n");
        break;
    case -6:
        printf("Ошибка при обновлении задания!\n");
        break;
    case -7:
        printf("Ошибка открытия бд\n");
        break;
    case -8:
        printf("Добавление пустой категории!\n");
        break;
    case -9:
        printf("Ошибка при добавлении категории\n");
        break;
    case -10:
        printf("Ошибка при добавлении категории к задаче");
        break;
    }
}
