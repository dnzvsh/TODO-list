#include "sqlite3.h"
#include "task.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    GtkBuilder* builder;
    GtkButton* addTaskButton;
    GtkWidget* window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/mainWindow.glade", NULL);

    addTaskButton = GTK_BUTTON(gtk_builder_get_object(builder, "addButtonM"));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    show_task(builder);

    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(
            G_OBJECT(addTaskButton),
            "clicked",
            G_CALLBACK(open_add_window),
            builder);
    gtk_widget_show(window);

    gtk_main();
    /*
    sqlite3* db = 0; // хэндл объекта соединение к БД
    char* error = 0;
    // открываем соединение
    if (sqlite3_open("src/database.db", &db)) {
        printf("Ошибка открытия/создания БД: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // выполняем SQL
    else if (sqlite3_exec(db, TODO, 0, 0, &error)) {
        printf("Ошибка SQL: %s\n", error);
        sqlite3_free(error);
        return 0;
    }
    if (sqlite3_exec(db, CATEGORIES, 0, 0, &error)) {
        printf("Ошибка SQL: %s\n", error);
        sqlite3_free(error);
        return 0;
    }
     if (argc == 2) {
         Task_data data;
         data.db = db;
         }  else if (!strcmp(argv[1], "addC")) {
              char category[1000];
              char* c = &category[0];
              int i = 0;
              char k;
              printf("Введите новую категорию: \n");
              while ((k = getchar()) != '\n') {
                  category[i] = k;
                  i++;
              }
              category[i] = '\0';
              printf("string = %s\n", c);
              int uncorrect = add_category(db, category);
              if (uncorrect) {
                  printf("Error\n");
                  sqlite3_close(db);
                  return 0;
              }

         if (!strcmp(argv[1], "show")) {
             show_task(db);
         } else if (
                 !strcmp(argv[1], "update") || !strcmp(argv[1], "delete")
                 || !strcmp(argv[1], "add")) {
             read_data(&data, argv[1]);
             int err;
             if (!strcmp(argv[1], "update")) {
                 err = update_task(&data);
             }
             if (!strcmp(argv[1], "add")) {
                 err = add_task(&data);
             }
             if (!strcmp(argv[1], "delete")) {
                 err = delete_task(&data);
             }
             if (err) {
                 parse_error(err);
                 sqlite3_close(db);
                 return 0;
             }
         } else {
             printf("Usage ./bin/todo {add/delete/update/show}\n");
         }
     } else {
         printf("Usage ./bin/todo {add/delete/update/show}\n");
     }
     // закрываем соединение
     sqlite3_close(db);
     */
    return 0;
}
