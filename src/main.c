#include "database_func.h"
#include "interface.h"
#include "sqlite3.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    GtkBuilder* builder;
    GtkButton* addTaskButton;
    GtkWidget* window;

    sqlite3* db = 0; // хэндл объекта соединение к БД
    char* error = 0;
    // открываем соединение
    if (sqlite3_open("src/database.db", &db)) {
        printf("Ошибка открытия/создания БД: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // выполняем SQL
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

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/mainWindow.glade", NULL);
    addTaskButton = GTK_BUTTON(gtk_builder_get_object(builder, "addButtonM"));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));

    GUI main_window;
    main_window.task.db = db;
    main_window.builder = builder;

    update_main_window(&main_window);

    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(
            G_OBJECT(addTaskButton),
            "clicked",
            G_CALLBACK(open_add_window),
            &main_window);
    GUI edit_button[20];

    for (int j = 0; j < 20; j++) {
        edit_button[j].task.db = db;
        edit_button[j].builder = builder;
        GtkLabel l_main;
        GtkLabel l_date;
        GtkButton* button_edit;
        char tm[14] = "editButton";
        char t[3];
        snprintf(t, 3, "%d", j + 1);
        strcat(tm, t);
        button_edit
                = GTK_BUTTON(gtk_builder_get_object(edit_button->builder, tm));
        GtkLabel* label_main = &l_main;
        GtkLabel* label_date = &l_date;
        edit_button[j].index = j + 1;
        read_labels(
                label_main, label_date, edit_button[j].index, &edit_button[j]);
        edit_button[j].rc = g_signal_connect(
                button_edit,
                "clicked",
                G_CALLBACK(open_view_window),
                &edit_button[j]);
    }
    gtk_widget_show(window);
    gtk_main();
    sqlite3_close(db);
    return 0;
}
