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

void filling_label(GUI* data, int id, char* task, char* label_type)
{
    char tmp[3];
    snprintf(tmp, 3, "%d", id + 1);
    strcat(label_type, tmp);
    GtkLabel* label
            = GTK_LABEL(gtk_builder_get_object(data->builder, label_type));
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
        filling_label(data, i, label_main[i], l_main);
        filling_label(data, i, label_date[i], l_date);
        i++;
    }
    char l_date[12] = "labelTime";
    char l_main[15] = "labelM";
    filling_label(data, i, " ", l_main);
    filling_label(data, i, " ", l_date);
}

void show_task_on_add(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = user_data;
    update_main_window(data);
}

int open_add_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
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

void add_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    read_buffer(data->builder_window, "textViewA", data->task.task);
    add_task(&data->task);
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
    update_task(&data->task);
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

void initialize_edit_button(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* interface = (GUI*)user_data;
    GtkButton* button_edit;
    char tm[14] = "editButton";
    char t[3];
    snprintf(t, 3, "%d", interface->index);
    strcat(tm, t);
    button_edit = GTK_BUTTON(gtk_builder_get_object(interface->builder, tm));
    GtkLabel label_main;
    GtkLabel label_date;
    read_labels(&label_main, &label_date, interface->index, interface);
    if (interface->rc != 0) {
        g_signal_handler_disconnect(button_edit, interface->rc);
    }
    interface->rc = g_signal_connect(
            button_edit, "clicked", G_CALLBACK(open_view_window), interface);
}

int open_view_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
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