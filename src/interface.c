#include "interface.h"
#include "database_func.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* define_name_widget(char* name, int id)
{
    char tmp[3];
    snprintf(tmp, 3, "%d", id);
    strcat(name, tmp);
    return name;
}

void window_disabled(GtkBuilder* builder, char* widget_name)
{
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
    gtk_widget_set_sensitive(window, FALSE);
}

void window_enabled(GtkBuilder* builder, char* widget_name)
{
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
    gtk_widget_set_sensitive(window, TRUE);
}

int read_buffer(GtkBuilder* builder, char* text_id, char* text, int type_label)
{
    GtkTextBuffer* buffer;
    GtkTextIter start;
    GtkTextIter end;
    buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW((gtk_builder_get_object(builder, text_id))));
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    if (type_label == 1) {
        if (strlen((char*)gtk_text_buffer_get_text(buffer, &start, &end, FALSE))
            >= MAX_CHAR_CATEGORY) {
            show_error(-19);
            text[0] = '\0';
            return -1;
        } else {
            strcpy(text,
                   (char*)gtk_text_buffer_get_text(
                           buffer, &start, &end, FALSE));
        }
    } else {
        if (strlen((char*)gtk_text_buffer_get_text(buffer, &start, &end, FALSE))
            >= MAX_CHAR_TASK) {
            show_error(-20);
            text[0] = '\0';
            return -1;
        } else {
            strcpy(text,
                   (char*)gtk_text_buffer_get_text(
                           buffer, &start, &end, FALSE));
        }
    }
    return 0;
}

void close_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GtkWidget* window = user_data;
    gtk_widget_hide(window);
}

void filling_label(GtkBuilder* builder, int id, char* task, char* label_type)
{
    GtkLabel* label = GTK_LABEL(gtk_builder_get_object(
            builder, define_name_widget(label_type, id + 1)));
    gtk_label_set_text(label, task);
}

void update_main_window(GUI* data)
{
    char label_main[MAX_COUNT][MAX_CHAR_TASK];
    char label_date[MAX_COUNT][MAX_CHAR_DATA];
    int j = show_task(data->task.db, label_main, label_date);
    int i = 0;
    while (i < j) {
        char l_date[MAX_CHAR_LABEL_TIME] = "labelTime";
        char l_main[MAX_CHAR_LABEL_MAIN] = "labelM";
        filling_label(data->builder, i, label_main[i], l_main);
        filling_label(data->builder, i, label_date[i], l_date);
        i++;
    }
    if (i < MAX_COUNT) {
        char l_date[MAX_CHAR_LABEL_TIME] = "labelTime";
        char l_main[MAX_CHAR_LABEL_MAIN] = "labelM";
        filling_label(data->builder, i, "\0", l_main);
        filling_label(data->builder, i, "\0", l_date);
    }
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
    char button_name[MAX_CHAR_BUTTON] = "editButton";
    GtkButton* button_edit = GTK_BUTTON(gtk_builder_get_object(
            data->builder, define_name_widget(button_name, len)));
    gtk_widget_set_sensitive((GtkWidget*)button_edit, TRUE);
}

void open_main_window(sqlite3* db)
{
    GtkBuilder* builder;
    GtkButton *addTaskButton, *CategoryButton;
    GtkWidget* window;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/GUI/mainWindow.glade", NULL);
    addTaskButton = GTK_BUTTON(gtk_builder_get_object(builder, "addButtonM"));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    CategoryButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "categoryButtonM"));
    GtkButton* DisplayCategoryBindButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "dateButtonM"));

    GUI* edit_button = malloc(sizeof(GUI) * MAX_COUNT);
    edit_button[0].task.db = db;
    edit_button[0].builder = builder;
    GUI* main_window = &edit_button[0];
    main_window->is_main = IS_MAIN;
    update_main_window(main_window);
    for (int j = 0; j < MAX_COUNT; j++) {
        edit_button[j].task.db = db;
        edit_button[j].builder = builder;
        GtkButton* button_edit;
        char button_name[MAX_CHAR_BUTTON] = "editButton";
        button_edit = GTK_BUTTON(gtk_builder_get_object(
                edit_button->builder, define_name_widget(button_name, j + 1)));
        GtkLabel label_main;
        GtkLabel label_date;
        edit_button[j].index = j + 1;
        read_labels(
                &label_main,
                &label_date,
                edit_button[j].index,
                &edit_button[j]);
        if (strlen(edit_button[j].task.task) == 0) {
            gtk_widget_set_sensitive((GtkWidget*)button_edit, FALSE);
        }
        edit_button[j].rc = g_signal_connect(
                button_edit,
                "clicked",
                G_CALLBACK(open_view_window),
                &edit_button[j]);
    }

    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(
            G_OBJECT(addTaskButton),
            "clicked",
            G_CALLBACK(open_add_window),
            main_window);
    g_signal_connect(
            G_OBJECT(CategoryButton),
            "clicked",
            G_CALLBACK(open_category_window),
            main_window);
    g_signal_connect(
            G_OBJECT(DisplayCategoryBindButton),
            "clicked",
            G_CALLBACK(open_task_sort_category_window),
            main_window);
    gtk_widget_show(window);
}

void close_add_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_enabled(data->builder, "mainWindow");
    GtkWidget* window = GTK_WIDGET(
            gtk_builder_get_object(data->builder_window, "addWindow"));
    data->is_main = IS_MAIN;
    gtk_widget_hide(window);
}

void destroy_add_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    data->is_main = IS_MAIN;
    window_enabled(data->builder, "mainWindow");
}

void open_add_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkBuilder* builder;
    window_disabled(data->builder, "mainWindow");
    GtkButton* AddButton;
    data->is_main = IS_BIND;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/GUI/addWindow.glade", NULL);
    data->builder_window = builder;
    data->task.category_name[0] = '\0';
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
            G_OBJECT(AddButton), "clicked", G_CALLBACK(close_add_window), data);
    g_signal_connect(
            G_OBJECT(CategoryButton),
            "clicked",
            G_CALLBACK(open_category_window),
            data);
    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(destroy_add_window), data);
    gtk_widget_show(window);
}

void add_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    int err = read_buffer(
            data->builder_window, "textViewA", data->task.task, 0);
    if (!err) {
        err = add_task(&data->task);
        if (err) {
            show_error(err);
        }
        if (strlen(data->task.category_name) != 0) {
            int error = bind_category_for_task(&data->task);
            if (error) {
                show_error(error);
            }
        }
        data->task.category_name[0] = '\0';
    }
}

void delete_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_enabled(data->builder, "mainWindow");
    delete_task(&data->task);
}

void update_task_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    int err = read_buffer(
            data->builder_window, "textViewV", data->task.task, 0);
    if (!err) {
        err = update_task(&data->task);
        if (err) {
            show_error(err);
        }
        window_enabled(data->builder, "mainWindow");
    }
}

void read_labels(
        GtkLabel* label_main, GtkLabel* label_date, int num, GUI* interface)
{
    char label_name[MAX_CHAR_LABEL_MAIN] = "labelM";
    label_main = GTK_LABEL(gtk_builder_get_object(
            interface->builder, define_name_widget(label_name, num)));
    strcpy(interface->task.task, (char*)gtk_label_get_text(label_main));
    strcpy(label_name, "labelTime");
    label_date = GTK_LABEL(gtk_builder_get_object(
            interface->builder, define_name_widget(label_name, num)));
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
    char edit_button1[MAX_CHAR_BUTTON] = "editButton";
    char edit_button2[MAX_CHAR_BUTTON] = "editButton";
    button_edit = GTK_BUTTON(gtk_builder_get_object(
            interface->builder,
            define_name_widget(edit_button1, interface->index)));
    GtkLabel label_main;
    GtkLabel label_date;
    read_labels(&label_main, &label_date, interface->index, interface);
    GtkButton* be = GTK_BUTTON(gtk_builder_get_object(
            interface->builder,
            define_name_widget(edit_button2, task_score(interface->task.db))));
    if (interface->action == DELETE_TASK) {
        gtk_widget_set_sensitive((GtkWidget*)be, FALSE);
    } else {
        gtk_widget_set_sensitive((GtkWidget*)be, TRUE);
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

void window_enable_view(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_enabled(data->builder, "mainWindow");
}

int open_view_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_disabled(data->builder, "mainWindow");
    GtkBuilder* builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/GUI/viewWindow.glade", NULL);
    data->builder_window = builder;
    initialize_buffer_view(builder, data);
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "viewWindow"));
    g_signal_connect(
            G_OBJECT(window), "destroy", G_CALLBACK(window_enable_view), data);
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
    char deleteButton[MAX_CHAR_BUTTON] = "deleteButton";
    char editButton[MAX_CHAR_BUTTON] = "editButton";
    char categoryLabel[MAX_CHAR_BUTTON] = "categoryLabel";
    char selectButton[MAX_CHAR_BUTTON] = "selectButton";
    GtkLabel* label = GTK_LABEL(gtk_builder_get_object(
            data->builder_window_category,
            define_name_widget(categoryLabel, data->number_button_category)));
    strcpy(data->task.category_name, (char*)gtk_label_get_text(label));
    GtkButton* db = GTK_BUTTON(gtk_builder_get_object(
            data->builder_window_category,
            define_name_widget(deleteButton, category_score(data->task.db))));
    GtkButton* sb = GTK_BUTTON(gtk_builder_get_object(
            data->builder_window_category,
            define_name_widget(selectButton, category_score(data->task.db))));
    GtkButton* eb = GTK_BUTTON(gtk_builder_get_object(
            data->builder_window_category,
            define_name_widget(editButton, category_score(data->task.db))));
    int err = delete_category(&data->task);
    if (err) {
        show_error(err);
    } else {
        gtk_widget_set_sensitive((GtkWidget*)db, FALSE);
        gtk_widget_set_sensitive((GtkWidget*)eb, FALSE);
        gtk_widget_set_sensitive((GtkWidget*)sb, FALSE);
    }
    update_category_window(data);
}

void edit_button_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    int err = read_buffer(
            data->builder_add_window_category,
            "textViewA",
            data->task.new_category_name,
            1);
    if (!err) {
        err = update_category(&data->task);
        if (err) {
            show_error(err);
        }
        update_category_window(data);
    }
}

void close_window_edit_category(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(
            data->builder_add_window_category, "addWindow"));
    window_enabled(data->builder_window_category, "categoryWindow");
    gtk_widget_hide(window);
}

void edit_category_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_disabled(data->builder_window_category, "categoryWindow");
    char categoryLabel[MAX_CHAR_BUTTON] = "categoryLabel";
    GtkLabel* label = GTK_LABEL(gtk_builder_get_object(
            data->builder_window_category,
            define_name_widget(categoryLabel, data->number_button_category)));
    strcpy(data->task.category_name, (char*)gtk_label_get_text(label));

    GtkBuilder* builder = gtk_builder_new();
    gtk_builder_add_from_file(
            builder, "src/GUI/categoryeditWindow.glade", NULL);
    data->builder_add_window_category = builder;
    GtkTextBuffer* buffer;
    buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textViewA")));
    gtk_text_buffer_set_text(buffer, data->task.category_name, -1);

    GtkWidget* editWindow
            = GTK_WIDGET(gtk_builder_get_object(builder, "addWindow"));
    GtkButton* closeButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "addButton"));
    g_signal_connect(
            G_OBJECT(closeButton),
            "clicked",
            G_CALLBACK(close_window_edit_category),
            data);

    GtkButton* editButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "categoryButton"));
    g_signal_connect(
            G_OBJECT(editButton),
            "clicked",
            G_CALLBACK(edit_button_click),
            data);
    g_signal_connect(
            G_OBJECT(editButton),
            "clicked",
            G_CALLBACK(close_window_edit_category),
            data);
    gtk_widget_show(editWindow);
}

void close_window_category(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(
            data[0].builder_window_category, "categoryWindow"));
    if (data[0].is_main) {
        window_enabled(data[0].builder, "mainWindow");
    } else {
        window_enabled(data[0].builder_window, "addWindow");
    }

    free(data);

    gtk_widget_hide(window);
}

void bind_category_task_click(GUI* data)
{
    char categoryLabel[MAX_CHAR_BUTTON] = "categoryLabel";
    data->task.category_name[0] = '\0';
    GtkLabel* label = GTK_LABEL(gtk_builder_get_object(
            data->builder_window_category,
            define_name_widget(categoryLabel, data->number_button_category)));
    strcpy(data->task.category_name, (char*)gtk_label_get_text(label));
    window_enabled(data->builder_window, "addWindow");
}

void bind_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    Bind* data = (Bind*)user_data;
    data->bind_data->number_button_category
            = data->button->number_button_category;
    bind_category_task_click(data->bind_data);
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
        window_disabled(data->builder, "mainWindow");
        int i = 1;
        while (i <= MAX_COUNT) {
            char selectButton[MAX_CHAR_LABEL_MAIN] = "selectButton";
            GtkButton* button_bind = GTK_BUTTON(gtk_builder_get_object(
                    builder, define_name_widget(selectButton, i)));
            gtk_widget_set_sensitive((GtkWidget*)button_bind, FALSE);
            i++;
        }
    } else {
        window_disabled(data->builder_window, "addWindow");
    }
    int i = 0;
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "categoryWindow"));
    update_category_window(data);
    GUI* delete_button = malloc(sizeof(GUI) * MAX_COUNT);
    int j = category_score(data->task.db);
    while (i < MAX_COUNT) {
        char deleteButton[MAX_CHAR_BUTTON] = "deleteButton";
        char selectButton[MAX_CHAR_BUTTON] = "selectButton";
        char editButton[MAX_CHAR_BUTTON] = "editButton";
        GtkButton* delbut = GTK_BUTTON(gtk_builder_get_object(
                builder, define_name_widget(deleteButton, i + 1)));
        GtkButton* selbut = GTK_BUTTON(gtk_builder_get_object(
                builder, define_name_widget(selectButton, i + 1)));
        GtkButton* editbut = GTK_BUTTON(gtk_builder_get_object(
                builder, define_name_widget(editButton, i + 1)));
        if (i >= j) {
            gtk_widget_set_sensitive((GtkWidget*)delbut, FALSE);
            gtk_widget_set_sensitive((GtkWidget*)selbut, FALSE);
            gtk_widget_set_sensitive((GtkWidget*)editbut, FALSE);
        }
        delete_button[i].builder = data->builder;
        delete_button[i].builder_window = data->builder_window;
        delete_button[i].is_main = data->is_main;
        delete_button[i].task.db = data->task.db;
        delete_button[i].number_button_category = i + 1;
        delete_button[i].builder_window_category = builder;
        Bind* d = malloc(sizeof(Bind));
        d->bind_data = data;
        d->button = &delete_button[i];
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
        g_signal_connect(
                G_OBJECT(selbut), "clicked", G_CALLBACK(bind_click), d);
        g_signal_connect(
                G_OBJECT(selbut), "clicked", G_CALLBACK(close_window), window);
        i++;
    }
    GtkButton* closeButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "cancelButton"));

    GtkButton* addButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "addCategory"));

    g_signal_connect(
            G_OBJECT(closeButton),
            "clicked",
            G_CALLBACK(close_window_category),
            delete_button);
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
    window_enabled(data->builder_window_category, "categoryWindow");
    int err = read_buffer(
            data->builder_add_window_category,
            "textViewA",
            data->task.category_name,
            1);
    if (!err) {
        err = add_category(&data->task);
        if (err) {
            show_error(err);
        }
        char deleteButton[MAX_CHAR_BUTTON] = "deleteButton";
        char selectButton[MAX_CHAR_BUTTON] = "selectButton";
        char editButton[MAX_CHAR_BUTTON] = "editButton";
        GtkButton* db = GTK_BUTTON(gtk_builder_get_object(
                data->builder_window_category,
                define_name_widget(
                        deleteButton, category_score(data->task.db))));
        GtkButton* eb = GTK_BUTTON(gtk_builder_get_object(
                data->builder_window_category,
                define_name_widget(editButton, category_score(data->task.db))));
        GtkButton* sb = GTK_BUTTON(gtk_builder_get_object(
                data->builder_window_category,
                define_name_widget(
                        selectButton, category_score(data->task.db))));
        gtk_widget_set_sensitive((GtkWidget*)db, TRUE);
        gtk_widget_set_sensitive((GtkWidget*)eb, TRUE);
        if (!data->is_main)
            gtk_widget_set_sensitive((GtkWidget*)sb, TRUE);
    }
}

void close_window_add_category(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_enabled(data->builder_window_category, "categoryWindow");
    GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(
            data->builder_add_window_category, "addWindow"));
    gtk_widget_hide(window);
}

void add_category_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_disabled(data->builder_window_category, "categoryWindow");
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
            G_CALLBACK(close_window_add_category),
            data);
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
    char label[MAX_COUNT][MAX_CHAR_CATEGORY];
    int j = show_category(data->task.db, label);
    int i = 0;
    while (i < j) {
        char l_category[MAX_CHAR_BUTTON] = "categoryLabel";
        filling_label(data->builder_window_category, i, label[i], l_category);
        i++;
    }
    if (i < MAX_COUNT) {
        char l_category[MAX_CHAR_BUTTON] = "categoryLabel";
        filling_label(data->builder_window_category, i, "\0", l_category);
    }
}

void show_category_on_add(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = user_data;
    update_category_window(data);
}

void close_window_show_task_category(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_enabled(data->builder_window, "categoryForTask");
    GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(
            data->builder_window_category_task, "taskForCategory"));
    gtk_widget_hide(window);
}

void show_task_category_click(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_disabled(data->builder_window, "categoryForTask");
    char categoryLabel[MAX_CHAR_BUTTON] = "categoryLabel";
    GtkLabel* label = GTK_LABEL(gtk_builder_get_object(
            data->builder_window,
            define_name_widget(categoryLabel, data->index)));
    strcpy(data->task.category_name, (char*)gtk_label_get_text(label));
    GtkBuilder* builder = gtk_builder_new();
    data->builder_window_category_task = builder;
    gtk_builder_add_from_file(
            builder, "src/GUI/taskForCategoryWindow.glade", NULL);
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "taskForCategory"));
    GtkButton* closeButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "cancelButtonTask"));
    g_signal_connect(
            G_OBJECT(closeButton),
            "clicked",
            G_CALLBACK(close_window_show_task_category),
            data);
    g_signal_connect(
            G_OBJECT(window),
            "destroy",
            G_CALLBACK(close_window_show_task_category),
            data);
    char tasks[MAX_COUNT][MAX_CHAR_TASK];
    for (int i = 0; i < MAX_COUNT; i++) {
        tasks[i][0] = '\0';
    }
    show_task_with_category(&data->task, tasks);
    int i = 0;
    while (strlen(tasks[i])) {
        char labelTask[MAX_CHAR_BUTTON] = "labelTask";
        GtkLabel* label = GTK_LABEL(gtk_builder_get_object(
                builder, define_name_widget(labelTask, i + 1)));
        gtk_label_set_text(label, tasks[i]);
        i++;
    }
    gtk_widget_show(window);
}

void close_window_task_category(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    GtkWidget* window = GTK_WIDGET(
            gtk_builder_get_object(data[0].builder_window, "categoryForTask"));
    window_enabled(data[0].builder, "mainWindow");
    free(data);
    gtk_widget_hide(window);
}

void open_task_sort_category_window(GtkWidget* widget, gpointer user_data)
{
    (void)widget;
    GUI* data = (GUI*)user_data;
    window_disabled(data->builder, "mainWindow");
    GtkBuilder* builder = gtk_builder_new();
    gtk_builder_add_from_file(
            builder, "src/GUI/categoryForTaskWindow.glade", NULL);
    data->builder_window = builder;
    GtkWidget* window
            = GTK_WIDGET(gtk_builder_get_object(builder, "categoryForTask"));
    GtkButton* backButton
            = GTK_BUTTON(gtk_builder_get_object(builder, "cancelButton"));

    GUI* showButton = malloc(sizeof(GUI) * MAX_COUNT);
    char labels_category[MAX_COUNT][MAX_CHAR_CATEGORY];
    for (int i = 0; i < MAX_COUNT; i++) {
        labels_category[i][0] = '\0';
    }
    show_category(data->task.db, labels_category);
    for (int i = 0; i < MAX_COUNT; i++) {
        showButton[i].task.db = data->task.db;
        showButton[i].builder_window = builder;
        showButton[i].index = i + 1;
        showButton[i].builder = data->builder;
        char selectButton[MAX_CHAR_BUTTON] = "selectButton";
        char categoryLabel[MAX_CHAR_BUTTON] = "categoryLabel";
        GtkButton* ShowB = GTK_BUTTON(gtk_builder_get_object(
                builder, define_name_widget(selectButton, i + 1)));
        GtkLabel* label = GTK_LABEL(gtk_builder_get_object(
                builder, define_name_widget(categoryLabel, i + 1)));
        if (strlen(labels_category[i]) != 0) {
            gtk_label_set_text(label, labels_category[i]);
        }

        if (i >= category_score(data->task.db)) {
            gtk_widget_set_sensitive((GtkWidget*)ShowB, FALSE);
        }
        g_signal_connect(
                G_OBJECT(ShowB),
                "clicked",
                G_CALLBACK(show_task_category_click),
                &showButton[i]);
    }

    g_signal_connect(
            G_OBJECT(backButton),
            "clicked",
            G_CALLBACK(close_window_task_category),
            showButton);
    g_signal_connect(
            G_OBJECT(window),
            "destroy",
            G_CALLBACK(close_window_task_category),
            showButton);

    gtk_widget_show(window);
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
    case -14:
        open_error_window("Ошибка при связывании задания\n");
        break;
    case -15:
        open_error_window("Нельзя удалить категорию, связанную с задачами!\n");
        break;
    case -16:
        open_error_window("Категория с таким названием уже есть!\n");
        break;
    case -17:
        open_error_window("Уже добавлено максимальное кол-во категорий!\n");
        break;
    case -18:
        open_error_window("Уже добавлено максимальное кол-во заданий!\n");
        break;
    case -19:
        open_error_window(
                "Превышено максимальное количество символов для категории!\n");
        break;
    case -20:
        open_error_window(
                "Превышено максимальное количество символов для задания!\n");
        break;
    }
}
