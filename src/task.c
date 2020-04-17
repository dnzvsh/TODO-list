#include "task.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int itoa_rec(int a, char str[])
{
    int i = 0; //индекс для записывания
    if (a < 0) //ставим 'минус' в начале массива, если число отрицательное
    {
        str[0] = '-';
        i++;
    }
    if ((a / 10)) //условие выхода из рекурсии:если число одного элемента
    {
        i = itoa_rec(a / 10, str);
    }
    str[i++] = abs(a % 10) + '0'; //записываем в массив символов
    str[i] = '\0';
    return i;
}

const int sql_request(Task_data* data)
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

void close_window(GtkWidget* widget, gpointer user_data)
{
    GtkWidget* window = user_data;
    gtk_widget_hide(window);
}

void read_data(Task_data* data, char* argv)
{
    int i = 0;
    char k;
    if (strcmp("add", argv)) {
        printf("Введите дату: ");
        while ((k = getchar()) != '\n') {
            data->date[i] = k;
            i++;
        }
        data->date[24] = '\n';
        data->date[25] = '\0';
        i = 0;
    }
    if (strcmp("delete", argv)) {
        printf("Введите заметку: ");
        while ((k = getchar()) != '\n') {
            data->task[i] = k;
            i++;
        }
        data->task[i] = '\0';
    }
    strcpy(data->argv, argv);
}

int open_add_window(GtkWidget* widget, gpointer data)
{
    GtkBuilder* builder;

    GtkButton* button;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/addWindow.glade", NULL);
    button = GTK_BUTTON(gtk_builder_get_object(builder, "addButtonA"));

    g_signal_connect(
            G_OBJECT(button), "clicked", G_CALLBACK(add_task), builder);
    g_signal_connect(
            G_OBJECT(button), "clicked", G_CALLBACK(show_task_on_add), data);
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "addWindow"));
    g_signal_connect(
            G_OBJECT(button), "clicked", G_CALLBACK(close_window), window);
    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(gtk_widget_hide), NULL);
    gtk_widget_show(window);
    return 0;
}

void show_task_on_add(GtkWidget* widget, gpointer data)
{
    GtkBuilder* builder = data;
    show_task(builder);
}

int add_task(GtkWidget* widget, gpointer user_data)
{
    Task_data data;
    sqlite3_open(DATABASE_PATH, &data.db);
    GtkBuilder* builder = user_data;
    GtkTextBuffer* buffer;

    GtkTextIter start;
    GtkTextIter end;
    char* text;
    buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW((gtk_builder_get_object(builder, "textViewA"))));
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    text = (char*)gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    g_print("%s\n", text);
    strcpy(data.task, text);
    strcpy(data.sql, "INSERT INTO TODO (Task,Date) VALUES (?,?);");
    const time_t sec = time(NULL);
    char* t = ctime(&sec);
    strcpy(data.date, t);
    if (!data.task) {
        return -3;
    }
    int err = sql_request(&data);
    if (err) {
        g_print("%zd\n", strlen(text));
        return -2;
    }
    // gtk_widget_hide();
    return 0;
}

int delete_task(Task_data* data)
{
    printf("%s\n", data->date);
    strcpy(data->sql, "DELETE FROM TODO WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        return -4;
    }
    return 0;
}

int update_task(Task_data* data)
{
    if (!data->task) {
        return -5;
    }
    strcpy(data->sql, "UPDATE TODO SET Task = ? WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        return -6;
    }
    return 0;
}
/*
int add_category(sqlite3* db, char* category_name)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
            db,
            "INSERT INTO CATEGORIES (category_name) VALUES (?);",
            -1,
            &stmt,
            NULL);
    sqlite3_bind_text(stmt, 1, category_name, -1, NULL);
    int err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    return 0;
}
*/
int show_task(GtkBuilder* builder)
{
    sqlite3* db;
    int r = sqlite3_open(DATABASE_PATH, &db);
    if (r) {
        return -7;
    }
    sqlite3_stmt* stmt;
    int j = 1;
    sqlite3_prepare_v2(db, "select (Task) from TODO;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int i;
        int num_cols = sqlite3_column_count(stmt);
        for (i = 0; i < num_cols; i++) {
            char tmp[9] = "labelM";
            char t[3];
            itoa_rec(j, t);
            strcat(tmp, t);
            GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, tmp));
            gtk_label_set_text(label, (char*)sqlite3_column_text(stmt, i));
        }
        j++;
    }
    sqlite3_finalize(stmt);
    return 0;
}

void parse_error(int err)
{
    switch (err) {
    case -1:
        printf("Ошибка в запросе\n");
    case -2:
        printf("Ошибка при добавлении задания\n");
    case -3:
        printf("Добавление пустого задания!\n");
    case -4:
        printf("Ошибка при удалении задания\n");
    case -5:
        printf("Обновление на пустое задание!\n");
    case -6:
        printf("Ошибка при обновлении задания!\n");
    case -7:
        printf("Ошибка открытия бд\n");
    }
}