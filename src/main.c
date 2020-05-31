#include "database_func.h"
#include "interface.h"
#include "sqlite3.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    sqlite3* db = 0;

    database_open(&db);

    gtk_init(&argc, &argv);

    open_main_window(db);

    gtk_main();
    sqlite3_close(db);
    return 0;
}
