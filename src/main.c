#include "database_func.h"
#include "interface.h"
#include "sqlite3.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    sqlite3* db = 0;
    char* error = 0;
    if (sqlite3_open("src/database.db", &db)) {
        printf("Ошибка открытия/создания БД: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    if (sqlite3_exec(db, TODO, 0, 0, &error)) {
        printf("Ошибка SQL: %s\n", error);
        sqlite3_free(error);
        return 0;
    }
    if (sqlite3_exec(db, CATEGORIES, 0, 0, &error)) {
        printf("Ошибка SQL: %s\n", error);
        sqlite3_free(error);
        return 0;
    }

    gtk_init(&argc, &argv);

    open_main_window(db);

    gtk_main();
    sqlite3_close(db);
    return 0;
}
