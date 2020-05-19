#include "ctest.h"
#include "database_func.h"
#include <string.h>

typedef struct {
    char date[26];
    char category_name[100];
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
    }
    if (!strcmp("Date", par)) {
        sqlite3_prepare_v2(
                db,
                "select Date,Task from TODO WHERE Date = ?;",
                -1,
                &stmt,
                NULL);
    }
    if (!strcmp("Category", par)) {
        sqlite3_prepare_v2(
                db,
                "select category_name from CATEGORIES WHERE category_name = ?;",
                -1,
                &stmt,
                NULL);
    }
    sqlite3_bind_text(stmt, 1, str, -1, NULL);
    int err = sqlite3_step(stmt);
    if (err == 101) {
        buf->task[0] = '\0';
    } else {
        if (!strcmp("Category", par)) {
            strcpy(buf->category_name, (char*)sqlite3_column_text(stmt, 0));
        } else {
            strcpy(buf->date, (char*)sqlite3_column_text(stmt, 0));
            strcpy(buf->task, (char*)sqlite3_column_text(stmt, 1));
        }
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
    sqlite3_exec(db, "DELETE from CATEGORIES", 0, 0, NULL);
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
    show_database_with_par(data.db, "Task", &str, "test_delete");
    ASSERT_STR(str.task, real_task);
    ASSERT_EQUAL(exp, real);
}

CTEST(test_edit_task, correct_edit_task)
{
    test str;
    char* task = "First_task";
    char* real_task = "Second_task";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.task, task);
    int exp = 0;
    add_task(&data);
    strcpy(data.task, real_task);
    int real = update_task(&data);
    show_database_with_par(data.db, "Task", &str, "Second_task");
    ASSERT_STR(str.task, real_task);
    ASSERT_EQUAL(exp, real);
}

CTEST(test_edit_task, uncorrect_edit_task)
{
    test str;
    char* real_task = "";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.task, real_task);
    int exp = -5;
    add_task(&data);
    int real = update_task(&data);
    show_database_with_par(data.db, "Date", &str, "");
    ASSERT_STR(str.task, real_task);
    ASSERT_EQUAL(exp, real);
}

CTEST(test_category, correct_add_category)
{
    test str;
    char* real_category = "test_category";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.category_name, real_category);
    int exp = 0;
    int real = add_category(&data);
    show_database_with_par(data.db, "Category", &str, real_category);
    ASSERT_STR(str.category_name, real_category);
    ASSERT_EQUAL(exp, real);
}

CTEST(test_category, Null_add_category)
{
    Task_data data;
    test str;
    char* real_category = "";
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.category_name, real_category);
    int exp = -8;
    int real = add_category(&data);
    show_database_with_par(data.db, "Category", &str, real_category);
    ASSERT_EQUAL(exp, real);
    ASSERT_STR(str.category_name, real_category);
}

CTEST(test_category, correct_delete_category)
{
    test str;
    char* real_category = "";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.category_name, real_category);
    int exp = 0;
    add_category(&data);
    int real = delete_category(&data);
    show_database_with_par(data.db, "Category", &str, "test_delete");
    ASSERT_STR(str.category_name, real_category);
    ASSERT_EQUAL(exp, real);
}

CTEST(test_category, correct_edit_category)
{
    test str;
    char* category = "First_category";
    char* real_category = "Second_category";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.category_name, category);
    int exp = 0;
    add_category(&data);
    strcpy(data.new_category_name, real_category);
    int real = update_category(&data);
    show_database_with_par(data.db, "Category", &str, "Second_category");
    ASSERT_EQUAL(exp, real);
    ASSERT_STR(str.category_name, real_category);
}

CTEST(test_category, uncorrect_edit_category)
{
    test str;
    char* real_category = "";
    Task_data data;
    initialize_db(&data);
    clear_db(data.db);
    strcpy(data.category_name, real_category);
    int exp = -12;
    add_category(&data);
    int real = update_category(&data);
    show_database_with_par(data.db, "Category", &str, "");
    ASSERT_STR(str.category_name, real_category);
    ASSERT_EQUAL(exp, real);
}

