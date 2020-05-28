#include "interface.h"
#include "database_func.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void close_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GtkWidget* window = user_data;
    gtk_widget_hide(window);
}

void filling_label(GtkBuilder* builder, int id, char* task, char* label_type)
{
    char tmp[3];
    snprintf(tmp, 3, "%d", id + 1);
    strcat(label_type, tmp);
    GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, label_type));
    gtk_label_set_text(label, task);
}

void update_main_window(GUI* data)
{
    char label_main[20][1000];
    char label_date[20][26];
    int j = show_task(data->task.db, label_main, label_date);
    int i = 0;
    while (i < j) {
        char l_date[12] = "labelTime";
        char l_main[15] = "labelM";
        filling_label(data->builder, i, label_main[i], l_main);
        filling_label(data->builder, i, label_date[i], l_date);
        i++;
    }
    char l_date[12] = "labelTime";
    char l_main[15] = "labelM";
    filling_label(data->builder, i, "\0", l_main);
    filling_label(data->builder, i, "\0", l_date);
}

void show_task_on_add(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = user_data;
    update_main_window(data);
}

void update_edit_button_status(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    int len = task_score(data->task.db);
    char tm[14] = "editButton";
    char t[3];
    snprintf(t, 3, "%d", len);
    strcat(tm, t);
    GtkButton* button_edit
            = GTK_BUTTON(gtk_builder_get_object(data->builder, tm));
    gtk_widget_set_sensitive((GtkWidget*)button_edit, TRUE);
}

void open_add_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkBuilder* builder;

    GtkButton* AddButton;
    data->is_main = IS_BIND;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/GUI/addWindow.glade", NULL);
    data->builder_window = builder;
    AddButton = GTK_BUTTON(gtk_builder_get_object(builder, "addButtonA"));
    GtkButton* CategoryButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "categoryButtonA"));
    g_signal_connect(
            G_OBJECT(AddButton), "clicked", G_CALLBACK(add_task_click), data);
    g_signal_connect(
            G_OBJECT(AddButton), "clicked", G_CALLBACK(show_task_on_add), data);
    g_signal_connect(
            G_OBJECT(AddButton),
            "clicked",
            G_CALLBACK(update_edit_button_status),
            data);
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "addWindow"));
    g_signal_connect(
            G_OBJECT(AddButton), "clicked", G_CALLBACK(close_window), window);
    g_signal_connect(
            G_OBJECT(CategoryButton),
            "clicked",
            G_CALLBACK(open_category_window),
            data);
    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(gtk_widget_hide), NULL);
    gtk_widget_show(window);
}

void add_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    read_buffer(data->builder_window, "textViewA", data->task.task);
    int err = add_task(&data->task);
    if (err) {
        show_error(err);
    }
}

void delete_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    delete_task(&data->task);
}

void update_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    read_buffer(data->builder_window, "textViewV", data->task.task);
    int err = update_task(&data->task);
    if (err) {
        show_error(err);
    }
}

void read_labels(
        GtkLabel* label_main, GtkLabel* label_date, int num, GUI* interface)
{
    char label_name[15] = "labelM";
    char tmp[3];
    snprintf(tmp, 3, "%d", num);
    strcat(label_name, tmp);
    label_main
            = GTK_LABEL(gtk_builder_get_object(interface->builder, label_name));
    strcpy(interface->task.task, (char*)gtk_label_get_text(label_main));
    strcpy(label_name, "labelTime");
    strcat(label_name, tmp);
    label_date
            = GTK_LABEL(gtk_builder_get_object(interface->builder, label_name));
    strcpy(interface->task.date, (char*)gtk_label_get_text(label_date));
}

void initialize_buffer_view(GtkBuilder* builder, GUI* data)
{
    GtkTextBuffer* buffer;
    buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textViewV")));
    GtkLabel label_main;
    GtkLabel label_date;
    read_labels(&label_main, &label_date, data->index, data);
    gtk_text_buffer_set_text(buffer, data->task.task, -1);
}

void initialize_edit_button(GUI* interface)
{
    GtkButton* button_edit;
    char tm[14] = "editButton";
    char t[3];
    snprintf(t, 3, "%d", interface->index);
    strcat(tm, t);
    button_edit = GTK_BUTTON(gtk_builder_get_object(interface->builder, tm));
    GtkLabel label_main;
    GtkLabel label_date;
    read_labels(&label_main, &label_date, interface->index, interface);
    if (interface->action == DELETE_TASK) {
        gtk_widget_set_sensitive((GtkWidget*)button_edit, FALSE);
    } else {
        gtk_widget_set_sensitive((GtkWidget*)button_edit, TRUE);
    }
    if (interface->rc != 0) {
        g_signal_handler_disconnect(button_edit, interface->rc);
    }
    interface->rc = g_signal_connect(
            button_edit, "clicked", G_CALLBACK(open_view_window), interface);
}

void open_error_window(char* error)
{
    GtkBuilder* builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/GUI/errorWindow.glade", NULL);
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "errorWindow"));
    gtk_widget_show(window);
    GtkLabel* label = GTK_LABEL(gtk_builder_get_object(builder, "errorLabel"));
    gtk_label_set_text(label, error);
    GtkButton* button
            = GTK_BUTTON(gtk_builder_get_object(builder, "errorButton"));
    g_signal_connect(
            G_OBJECT(button), "clicked", G_CALLBACK(close_window), window);
    g_signal_connect(
            G_OBJECT(button), "destroy", G_CALLBACK(close_window), window);
}

void update_main_window_on_delete(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    data->action = DELETE_TASK;
    initialize_edit_button(data);
}

void update_main_window_on_edit(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    data->action = EDIT_TASK;
    initialize_edit_button(data);
}

int open_view_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkBuilder* builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/GUI/viewWindow.glade", NULL);
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
            G_CALLBACK(update_main_window_on_delete),
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
            G_CALLBACK(update_main_window_on_edit),
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

void delete_category_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    char t[3];
    char tm[18] = "deleteButton";
    char tm2[18] = "editButton";
    snprintf(t, 3, "%d", data->number_button_category);
    char tmp[18] = "categoryLabel";
    strcat(tmp, t);

    GtkLabel* label = GTK_LABEL(
            gtk_builder_get_object(data->builder_window_category, tmp));
    strcpy(data->task.category_name, (char*)gtk_label_get_text(label));
    snprintf(t, 3, "%d", category_score(data->task.db));
    strcat(tm, t);
    strcat(tm2, t);
    GtkButton* db = GTK_BUTTON(
            gtk_builder_get_object(data->builder_window_category, tm));
    GtkButton* eb = GTK_BUTTON(
            gtk_builder_get_object(data->builder_window_category, tm2));
    gtk_widget_set_sensitive((GtkWidget*)db, FALSE);
    gtk_widget_set_sensitive((GtkWidget*)eb, FALSE);
    int err = delete_category(&data->task);
    if (err) {
        show_error(err);
    }
    update_category_window(data);
}


void open_category_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkBuilder* builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/GUI/categoryWindow.glade", NULL);
    data->builder_window_category = builder;
    if (data->is_main) {
        int i = 1;
        while (i <= 20) {
            char tmp[15] = "selectButton";
            char t[3];
            snprintf(t, 3, "%d", i);
            strcat(tmp, t);
            GtkButton* button_bind
                    = GTK_BUTTON(gtk_builder_get_object(builder, tmp));
            gtk_widget_set_sensitive((GtkWidget*)button_bind, FALSE);
            i++;
        }
    }
    int i = 0;

    update_category_window(data);
    GUI* delete_button = malloc(sizeof(GUI) * 20);
    int j = category_score(data->task.db);
    while (i < 20) {
        char tm[15] = "deleteButton";
        char tmp[15] = "editButton";
        char t[3];
        snprintf(t, 3, "%d", i + 1);
        strcat(tm, t);
        strcat(tmp, t);
        GtkButton* delbut = GTK_BUTTON(gtk_builder_get_object(builder, tm));
        GtkButton* editbut = GTK_BUTTON(gtk_builder_get_object(builder, tmp));
        if (i >= j) {
            gtk_widget_set_sensitive((GtkWidget*)delbut, FALSE);
            gtk_widget_set_sensitive((GtkWidget*)editbut, FALSE);
        }
        delete_button[i].task.db = data->task.db;
        delete_button[i].number_button_category = i + 1;
        delete_button[i].builder_window_category = builder;
        g_signal_connect(
                G_OBJECT(delbut),
                "clicked",
                G_CALLBACK(delete_category_click),
                &delete_button[i]);
        g_signal_connect(
                G_OBJECT(editbut),
                "clicked",
                G_CALLBACK(edit_category_window),
                &delete_button[i]);
        i++;
    }
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "categoryWindow"));
    GtkButton* closeButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "cancelButton"));

    GtkButton* addButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "addCategory"));

    g_signal_connect(
            G_OBJECT(closeButton), "clicked", G_CALLBACK(close_window), window);
    g_signal_connect(
            G_OBJECT(addButton),
            "clicked",
            G_CALLBACK(add_category_window),
            data);
    gtk_widget_show(window);
}

void add_category_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    read_buffer(
            data->builder_add_window_category,
            "textViewA",
            data->task.category_name);
    int err = add_category(&data->task);
    if (err) {
        show_error(err);
    }
    char t[3];
    char tm[18] = "deleteButton";
    char tmp[18] = "editButton";
    snprintf(t, 3, "%d", category_score(data->task.db));
    strcat(tm, t);
    strcat(tmp, t);
    GtkButton* db = GTK_BUTTON(
            gtk_builder_get_object(data->builder_window_category, tm));
    GtkButton* eb = GTK_BUTTON(
            gtk_builder_get_object(data->builder_window_category, tmp));
    gtk_widget_set_sensitive((GtkWidget*)db, TRUE);
    gtk_widget_set_sensitive((GtkWidget*)eb, TRUE);
}

void add_category_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkBuilder* builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(
            builder, "src/GUI/categoryeditWindow.glade", NULL);
    data->builder_add_window_category = builder;
    GtkWidget* addWindow
            = GTK_WIDGET(gtk_builder_get_object(builder, "addWindow"));
    GtkButton* closeButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "addButton"));
    g_signal_connect(
            G_OBJECT(closeButton),
            "clicked",
            G_CALLBACK(close_window),
            addWindow);
    GtkButton* addButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "categoryButton"));
    g_signal_connect(
            G_OBJECT(addButton),
            "clicked",
            G_CALLBACK(add_category_click),
            data);
    g_signal_connect(
            G_OBJECT(addButton),
            "clicked",
            G_CALLBACK(close_window),
            addWindow);
    g_signal_connect(
            G_OBJECT(addButton),
            "clicked",
            G_CALLBACK(show_category_on_add),
            data);
    gtk_widget_show(addWindow);
}

void update_category_window(GUI* data)
{
    char label[20][100];
    int j = show_category(data->task.db, label);
    int i = 0;
    while (i < j) {
        char l_category[18] = "categoryLabel";
        filling_label(data->builder_window_category, i, label[i], l_category);
        i++;
    }
    char l_category[18] = "categoryLabel";
    filling_label(data->builder_window_category, i, "\0", l_category);
}

void show_category_on_add(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = user_data;
    update_category_window(data);
}

void show_error(int err)
{
    switch (err) {
    case -1:
        open_error_window("Ошибка в запросе\n");
        break;
    case -2:
        open_error_window("Ошибка при добавлении задания\n");
        break;
    case -3:
        open_error_window("Добавление пустого задания!\n");
        break;
    case -4:
        open_error_window("Ошибка при удалении задания\n");
        break;
    case -5:
        open_error_window("Обновление на пустое задание!\n");
        break;
    case -6:
        open_error_window("Ошибка при обновлении задания!\n");
        break;
    case -7:
        open_error_window("Ошибка открытия бд\n");
        break;
    case -8:
        open_error_window("Добавление пустой категории!\n");
        break;
    case -9:
        open_error_window("Ошибка при добавлении категории\n");
        break;
    case -10:
        open_error_window("Ошибка при добавлении категории к задаче");
        break;
    case -11:
        open_error_window("Ошибка при удалении категории\n");
        break;
    case -12:
        open_error_window("Ошибка при обновлении пустой категории\n");
        break;
    case -13:
        open_error_window("Ошибка при обновлении категории\n");
        break;
    }
}
