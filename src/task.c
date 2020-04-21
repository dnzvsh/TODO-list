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
/*
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
*/
void show_task_on_add(GtkWidget* widget, gpointer user_data)
{
    Task_data* data = user_data;
    show_task(data);
}

int open_add_window(GtkWidget* widget, gpointer user_data)
{
    Task_data* data = (Task_data*)user_data;
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
        return -2;
    }
    // gtk_widget_hide();
    return 0;
}

int delete_task(GtkWidget* widget, gpointer user_data)
{
    Task_data* data = (Task_data*)user_data;
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
void filling_label(Task_data* data, int id, char* task, char* label_type)
{
    char tmp[3];
    itoa_rec(id + 1, tmp);
    strcat(label_type, tmp);
    GtkLabel* label
            = GTK_LABEL(gtk_builder_get_object(data->builder, label_type));
    gtk_label_set_text(label, task);
}

int show_task(Task_data* data)
{
    sqlite3_stmt* stmt;
    int j = 0;
    sqlite3_prepare_v2(
            data->db, "select Task,Date from TODO;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        char label_main[15] = "labelM";
        char label_date[12] = "labelTime";
        filling_label(data, j, (char*)sqlite3_column_text(stmt, 0), label_main);
        filling_label(data, j, (char*)sqlite3_column_text(stmt, 1), label_date);
        j++;
    }
    char label_date[12] = "labelTime";
    char label_main[15] = "labelM";
    filling_label(data, j, " ", label_main);
    filling_label(data, j, " ", label_date);
    sqlite3_finalize(stmt);
    return 0;
}

void read_rows(
        GtkLabel* label_main, GtkLabel* label_date, int num, Task_data* i)
{
    char label_name[15] = "labelM";
    char tmp[3];
    itoa_rec(num, tmp);
    strcat(label_name, tmp);
    label_main = GTK_LABEL(gtk_builder_get_object(i->builder, label_name));
    strcpy(i->task, (char*)gtk_label_get_text(label_main));

    strcpy(label_name, "labelTime");
    strcat(label_name, tmp);
    label_date = GTK_LABEL(gtk_builder_get_object(i->builder, label_name));
    strcpy(i->date, (char*)gtk_label_get_text(label_date));
}

void initialize_edit_button(GtkWidget* widget, gpointer user_data)
{
    Task_data* i = (Task_data*)user_data;
    GtkButton* button_edit;
    char tm[14] = "editButton";
    char t[3];
    itoa_rec(i->index, t);
    strcat(tm, t);
    button_edit = GTK_BUTTON(gtk_builder_get_object(i->builder, tm));
    GtkLabel label_main;
    GtkLabel label_date;
    read_rows(&label_main, &label_date, i->index, i);
    if (i->rc != 0) {
        g_signal_handler_disconnect(button_edit, i->rc);
    }
    i->rc = g_signal_connect(
            button_edit, "clicked", G_CALLBACK(open_view_window), i);
}

int open_view_window(GtkWidget* widget, gpointer user_data)
{
    Task_data* data = (Task_data*)user_data;
    GtkBuilder* builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/viewWindow.glade", NULL);
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "viewWindow"));
    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(gtk_widget_hide), window);

    GtkButton* delete_button
            = GTK_BUTTON(gtk_builder_get_object(builder, "addButtonV"));
    g_signal_connect(
            G_OBJECT(delete_button),
            "clicked",
            G_CALLBACK(initialize_edit_button),
            data);
    g_signal_connect(
            G_OBJECT(delete_button), "clicked", G_CALLBACK(delete_task), data);
    g_signal_connect(
            G_OBJECT(delete_button),
            "clicked",
            G_CALLBACK(show_task_on_add),
            data);

    g_signal_connect(
            G_OBJECT(delete_button),
            "clicked",
            G_CALLBACK(close_window),
            window);

    // GtkButton* edit_button
    //        = GTK_BUTTON(gtk_builder_get_object(builder, "editButtonV"));

    gtk_widget_show(window);

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