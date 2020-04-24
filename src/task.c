#include "task.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void read_buffer(GtkBuilder* builder, char* text_id, char* text)
{
    GtkTextBuffer* buffer;
    GtkTextIter start;
    GtkTextIter end;
    buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW((gtk_builder_get_object(builder, text_id))));
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    strcpy(text, (char*)gtk_text_buffer_get_text(buffer, &start, &end, FALSE));
}

int sql_request(Task_data* data)
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
    (void)widget;
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
    (void)widget;
    Task_data* data = user_data;
    show_task(data);
}

int open_add_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    Task_data* data = (Task_data*)user_data;
    GtkBuilder* builder;

    GtkButton* button;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/addWindow.glade", NULL);
    data->builder_window = builder;
    button = GTK_BUTTON(gtk_builder_get_object(builder, "addButtonA"));

    g_signal_connect(
            G_OBJECT(button), "clicked", G_CALLBACK(add_task_click), data);
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

int add_task(Task_data* data)
{
    read_buffer(data->builder_window, "textViewA", data->task);
    strcpy(data->sql, "INSERT INTO TODO (Task,Date) VALUES (?,?);");
    const time_t sec = time(NULL);
    char* t = ctime(&sec);
    strcpy(data->date, t);
    if (data->task[0] == 0) {
        return -3;
    }
    int err = sql_request(data);
    if (err) {
        return -2;
    }
    // gtk_widget_hide();
    return 0;
}

void add_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    Task_data* data = (Task_data*)user_data;
    add_task(data);
}

int delete_task(Task_data* data)
{
    strcpy(data->argv, "delete");
    strcpy(data->sql, "DELETE FROM TODO WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        return -4;
    }
    return 0;
}

void delete_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    Task_data* data = (Task_data*)user_data;
    delete_task(data);
}

int update_task(Task_data* data)
{
    read_buffer(data->builder_window, "textViewV", data->task);
    strcpy(data->argv, "update");
    if (!data->task) {
        return -5;
    }
    strcpy(data->sql, "UPDATE TODO SET Task = ? WHERE Date = ?;");
    int err = sql_request(data);
    if (err) {
        g_print("error\n");
        return -6;
    }
    return 0;
}

void update_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    Task_data* data = (Task_data*)user_data;
    update_task(data);
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
    snprintf(tmp, 3, "%d", id + 1);
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

void read_labels(
        GtkLabel* label_main, GtkLabel* label_date, int num, Task_data* i)
{
    char label_name[15] = "labelM";
    char tmp[3];
    snprintf(tmp, 3, "%d", num);
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
    (void)widget;
    Task_data* i = (Task_data*)user_data;
    GtkButton* button_edit;
    char tm[14] = "editButton";
    char t[3];
    snprintf(t, 3, "%d", i->index);
    strcat(tm, t);
    button_edit = GTK_BUTTON(gtk_builder_get_object(i->builder, tm));
    GtkLabel label_main;
    GtkLabel label_date;
    read_labels(&label_main, &label_date, i->index, i);
    if (i->rc != 0) {
        g_signal_handler_disconnect(button_edit, i->rc);
    }
    i->rc = g_signal_connect(
            button_edit, "clicked", G_CALLBACK(open_view_window), i);
}

void initialize_buffer_view(GtkBuilder* builder, Task_data* data)
{
    GtkTextBuffer* buffer;
    buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textViewV")));
    GtkLabel label_main;
    GtkLabel label_date;
    read_labels(&label_main, &label_date, data->index, data);
    gtk_text_buffer_set_text(buffer, data->task, -1);
}

int open_view_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    Task_data* data = (Task_data*)user_data;
    GtkBuilder* builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/viewWindow.glade", NULL);
    data->builder_window = builder;
    initialize_buffer_view(builder, data);
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
            G_OBJECT(delete_button),
            "clicked",
            G_CALLBACK(delete_task_click),
            data);
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
    GtkButton* edit_button
            = GTK_BUTTON(gtk_builder_get_object(builder, "editButtonV"));
    g_signal_connect(
            G_OBJECT(edit_button),
            "clicked",
            G_CALLBACK(initialize_edit_button),
            data);
    g_signal_connect(
            G_OBJECT(edit_button),
            "clicked",
            G_CALLBACK(update_task_click),
            data);
    g_signal_connect(
            G_OBJECT(edit_button),
            "clicked",
            G_CALLBACK(show_task_on_add),
            data);
    g_signal_connect(
            G_OBJECT(edit_button), "clicked", G_CALLBACK(close_window), window);
    gtk_widget_show(window);
    return 0;
}

void parse_error(int err)
{
    switch (err) {
    case -1:
        printf("Ошибка в запросе\n");
        break;
    case -2:
        printf("Ошибка при добавлении задания\n");
        break;
    case -3:
        printf("Добавление пустого задания!\n");
        break;
    case -4:
        printf("Ошибка при удалении задания\n");
        break;
    case -5:
        printf("Обновление на пустое задание!\n");
        break;
    case -6:
        printf("Ошибка при обновлении задания!\n");
        break;
    case -7:
        printf("Ошибка открытия бд\n");
        break;
    }
}
