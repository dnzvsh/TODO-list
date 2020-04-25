#include "ctest.h"
#include "database_func.h"
#include <string.h>

typedef struct {
    char date[26];
    char task[1000];
} test;

void show_database_with_par(sqlite3* db, char* par, test* buf, char* str)
{
    sqlite3_stmt* stmt;
    if (!strcmp("Task", par)) {
        sqlite3_prepare_v2(
                db,
                "select Date,Task from TODO WHERE Task = ?;",
                -1,
                &stmt,
                NULL);
    } else {
        sqlite3_prepare_v2(
                db,
                "select Date,Task from TODO WHERE Date = ?;",
                -1,
                &stmt,
                NULL);
    }
    sqlite3_bind_text(stmt, 1, str, -1, NULL);
    int err = sqlite3_step(stmt);
    if (err == 101) {
        buf->task[0] = '\0';
    } else {
        strcpy(buf->date, (char*)sqlite3_column_text(stmt, 0));
        strcpy(buf->task, (char*)sqlite3_column_text(stmt, 1));
    }
    sqlite3_finalize(stmt);
}

void initialize_db(Task_data* data)
{
    sqlite3_open("src/database_test.db", &data->db);
    sqlite3_exec(data->db, TODO, 0, 0, NULL);
    sqlite3_exec(data->db, CATEGORIES, 0, 0, NULL);
}

void clear_db(sqlite3* db)
{
    sqlite3_exec(db, "DELETE from TODO", 0, 0, NULL);
}

CTEST(test_show_task, correct_show_task)
{
    char test_task[20][1000];
    char test_date[20][26];
    char* real_task[3] = {"test_task_1", "test_task_2", "test_task_3"};
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    for (int i = 0; i < 3; i++) {
        strcpy(data.task, real_task[i]);
        add_task(&data);
    }
    int exp = 3;
    int real = show_task(data.db, test_task, test_date);
    ASSERT_EQUAL(exp, real);
    // ASSERT_STR(str[i].task, real_task[i]);
}

CTEST(test_add_task, correct_add_task)
{
    test str;
    char* real_task = "test_task";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.task, real_task);
    int exp = 0;
    int real = add_task(&data);
    show_database_with_par(data.db, "Task", &str, real_task);
    ASSERT_STR(str.task, real_task);
    ASSERT_EQUAL(exp, real);
}

CTEST(test_add_task, Null_add_task)
{
    test str;
    char* real_task = "";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.task, real_task);
    int exp = -3;
    int real = add_task(&data);
    show_database_with_par(data.db, "Task", &str, real_task);
    ASSERT_STR(str.task, real_task);
    ASSERT_EQUAL(exp, real);
}

CTEST(test_delete_task, correct_delete_task)
{
    test str;
    char* real_task = "";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.task, real_task);
    int exp = 0;
    add_task(&data);
    int real = delete_task(&data);
    show_database_with_par(data.db, "Date", &str, "test_delete");
    ASSERT_STR(str.task, real_task);
    ASSERT_EQUAL(exp, real);
}
