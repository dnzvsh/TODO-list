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
    char* err = 0;
    // открываем соединение
    if (sqlite3_open("src/database.db", &db)) {
        fprintf(stderr,
                "Ошибка открытия/создания БД: %s\n",
                sqlite3_errmsg(db));
        return 0;
    }

    // выполняем SQL
    else if (sqlite3_exec(db, TODO, 0, 0, &err)) {
        fprintf(stderr, "Ошибка SQL: %sn", err);
        sqlite3_free(err);
        return 0;
    }
    if (sqlite3_exec(db, CATEGORIES, 0, 0, &err)) {
        fprintf(stderr, "Ошибка SQL: %sn", err);
        sqlite3_free(err);
        return 0;
    }
    if (argc == 2) {
        if (!strcmp(argv[1], "add")) {
            char task[1000];
            char* t = &task[0];
            int i = 0;
            char k;
            printf("Введите заметку:\n");
            while ((k = getchar()) != '\n') {
                t[i] = k;
                i++;
            }
            t[i] = '\0';
            printf("string = %s\n", t);
            int uncorrect = add_task(db, t);
            if (uncorrect) {
                printf("Error\n");
                sqlite3_close(db);
                return 0;
            }
        } else if (!strcmp(argv[1], "delete")) {
            int id;
            printf("Введите Индекс для удаления\n");
            scanf("%d", &id);
            int uncorrect = delete_task(db, id);
            if (uncorrect) {
                printf("Error\n");
                sqlite3_close(db);
                return 0;
            }
        } else if (!strcmp(argv[1], "show")) {
            show_task(db);
        } else {
            printf("Usage ./bin/todo {add/delete/show}\n");
        }
    } else {
        printf("Usage ./bin/todo {add/delete/show}\n");
    }
    // закрываем соединение
    sqlite3_close(db);

    /*
        GtkApplication* app;
        int status;

        app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        status = g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);

    return status;

    */

    return 0;
}
