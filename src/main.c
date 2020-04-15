#include "sqlite3.h"
#include "task.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

const char* TODO
        = "CREATE TABLE IF NOT EXISTS TODO(task_id integer primary key "
          "autoincrement, Task "
          "TEXT, Date TEXT);";
const char* CATEGORIES
        = "CREATE TABLE IF NOT EXISTS CATEGORIES(category_id integer primary "
          "key "
          "autoincrement, task_id integer,category_name TEXT,Foreign key "
          "(category_id) References TODO(task_id));";

/*

static void print_hello(GtkWidget* widget, gpointer data)
{
    g_print("Hello World\n");
}

static void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget* window;
    GtkWidget* button;
    GtkWidget* button_box;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(window), button_box);

    button = gtk_button_new_with_label("Hello World");
    g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
    g_signal_connect_swapped(
            button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_container_add(GTK_CONTAINER(button_box), button);

    gtk_widget_show_all(window);
}
*/
int main(int argc, char** argv)
{
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
        /*}  else if (!strcmp(argv[1], "addC")) {
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
         */
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

    /*
        GtkApplication* app;
        int status;

        app = gtk_application_new("org.gtk.example",
    G_APPLICATION_FLAGS_NONE); g_signal_connect(app, "activate",
    G_CALLBACK(activate), NULL); status =
    g_application_run(G_APPLICATION(app), argc, argv); g_object_unref(app);

    return status;

    */

    return 0;
}
