/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2007  Alexandru Csete.

    Authors: Alexandru Csete <oz9aec@gmail.com>

    Comments, questions and bugreports should be submitted via
    http://sourceforge.net/projects/groundstation/
    More details can be found at the project home page:

            http://groundstation.sourceforge.net/
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, visit http://www.fsf.org/
 
*/
/** \brief Rig selection window used at startup.
 * 
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include "compat.h"
#include "radio-conf.h"
#include "rig-selector.h"


/* private function declarations */
static gint rig_selector_delete     (GtkWidget *, GdkEvent *, gpointer);
static void rig_selector_destroy    (GtkWidget *, gpointer);
static void rig_selector_new_cb     (GtkWidget *, gpointer);
static void rig_selector_del_cb     (GtkWidget *, gpointer);
static void rig_selector_edit_cb    (GtkWidget *, gpointer);
static void rig_selector_cancel_cb  (GtkWidget *, gpointer);
static void rig_selector_connect_cb (GtkWidget *, gpointer);

static void cancel (GtkWidget*, gpointer);

static GtkWidget    *create_rig_list (void);
static GtkTreeModel *create_model (void);

static gchar *selected = NULL;


/** \brief Execute radio selector.
 *  \return The config file name of the selected radio or NULL if
 *          selection has been aborted.
 * 
 * This function creates a window containing a list with the currently
 * configured radios, allowing the user to select which radio to
 * connect to.
 *
 * Additionally, the window contains buttons to delete, edit, and add
 * new radios to the list.
 *
 * The dummy rig is always listed on the top of the list.
 *
 * The radio configurations are stored in $HOME/.grig/xyz.grc files and the
 * functions in radio-conf.c can be used for reading and saving them.
 */
gchar *
rig_selector_execute ()
{
    GtkWidget   *window;   /* the main rig-selector window */
    gchar       *icon;     /* window icon file name */
    GtkWidget   *vbox;     /* the main vertical box in the window */
    GtkWidget   *butbox1;  /* The button box with New, edit, and delete butons */
    GtkWidget   *butbox2;  /* the button box in the bottom of the window */
    GtkWidget   *conbut;   /* Connect button */
    GtkWidget   *cancbut;  /* Cancel button */
    GtkWidget   *newbut;   /* New button */
    GtkWidget   *editbut;  /* Edit button */
    GtkWidget   *delbut;   /* delete button */
    GtkWidget   *riglist;
    GtkWidget   *swin;
    GtkTooltips *tips;


    tips = gtk_tooltips_new ();

    /* connect button */
    conbut = gtk_button_new_from_stock (GTK_STOCK_CONNECT);
    gtk_widget_set_sensitive (conbut, FALSE);
    gtk_tooltips_set_tip (tips, conbut,
                          _("Connect to the selected radio."),
                          _("Grig will attempt to establish connection to the "
                            "selected radio using the specified settings. If "
                            "the connection is successful, the main application "
                            "window will be loaded."));
    
    /* cancel button */
    cancbut = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
    gtk_tooltips_set_tip (tips, cancbut,
                          _("Cancel radio selection."),
                          _("Cancel radio selection. This will end grig."));
    
    /* add button */
    newbut = gtk_button_new_from_stock (GTK_STOCK_ADD);
    gtk_tooltips_set_tip (tips, newbut,
                          _("Add a new radio to the list."),
                          _("A new configuration window will be shown allowing "
                            "you to select a radio and specify the connection "
                            "settings."));
    
    /* delete button */
    delbut = gtk_button_new_from_stock (GTK_STOCK_DELETE);
    gtk_widget_set_sensitive (delbut, FALSE);
    gtk_tooltips_set_tip (tips, delbut,
                          _("Delete the currently selected radio."), NULL);
    
    /* edit button */
    editbut = gtk_button_new_from_stock (GTK_STOCK_EDIT);
    gtk_widget_set_sensitive (editbut, FALSE);
    gtk_tooltips_set_tip (tips, editbut,
                          _("Edit the settings for the currently selected radio."),
                            NULL);
    
    /* button box*/
    butbox1 = gtk_hbutton_box_new ();
    gtk_button_box_set_layout (GTK_BUTTON_BOX (butbox1), GTK_BUTTONBOX_START);
    butbox2 = gtk_hbutton_box_new ();
    gtk_button_box_set_layout (GTK_BUTTON_BOX (butbox2), GTK_BUTTONBOX_END);
    gtk_box_set_spacing (GTK_BOX (butbox2), 10);
    gtk_container_add (GTK_CONTAINER (butbox1), newbut);
    gtk_container_add (GTK_CONTAINER (butbox1), editbut);
    gtk_container_add (GTK_CONTAINER (butbox1), delbut);
    gtk_container_add (GTK_CONTAINER (butbox2), cancbut);
    gtk_container_add (GTK_CONTAINER (butbox2), conbut);
    
    /* radio list */
    riglist = create_rig_list ();
    swin = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (swin), riglist);

    /* vertical box */
    vbox = gtk_vbox_new (FALSE, 10);
    gtk_box_pack_start (GTK_BOX (vbox), swin, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), butbox1, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (vbox), butbox2, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, FALSE, 0);

    /* create window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), _("Select a Radio"));
    icon = pixmap_file_name ("ic910.png");
    gtk_window_set_icon_from_file (GTK_WINDOW (window), icon, NULL);
    g_free (icon);
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 300);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    /* connect delete and destroy signals */
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (rig_selector_delete), NULL);
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (rig_selector_destroy), window);

    
    g_signal_connect (G_OBJECT (cancbut), "clicked",
                      G_CALLBACK (cancel), window);
    
    /* show window */
    gtk_widget_show_all (window);
    
    /* enter main loop that will only be quit
       when window is destroyed */
    gtk_main ();
    
    return selected;
}


static GtkWidget    *create_rig_list (void)
{
    GtkWidget   *riglist;
    GtkTreeModel      *model;
    GtkCellRenderer   *renderer;
    GtkTreeViewColumn *column;

    
    riglist = gtk_tree_view_new ();

    model = create_model ();
    gtk_tree_view_set_model (GTK_TREE_VIEW (riglist), model);
    g_object_unref (model);
    
    /* Company:1 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Company"), renderer,
            "text", 1, NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
    /* model:2 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Model"), renderer,
            "text", 2, NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* port:4 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Port"), renderer,
            "text", 4, NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* speed:5 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Speed"), renderer,
            "text", 5, NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* CI-V:6 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("CI-V"), renderer,
            "text", 6, NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* DTR:7 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("DTR"), renderer,
            "text", 7, NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* RTS:8 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("RTS"), renderer,
            "text", 8, NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    return riglist;
}


static GtkTreeModel *create_model ()
{
    GtkListStore *store;
    GtkTreePath *path;
    GtkTreeIter iter;
    GDir         *dir = NULL;   /* directory handle */
    gchar        *dirname;      /* directory name */
    const gchar  *filename;     /* file name */
    gchar        *buff;
    gchar       **vbuf;
    radio_conf_t  conf;

    /* Note that we create a column for each field but will hide
    some field in the treeview */
    store = gtk_list_store_new (9,
                                G_TYPE_STRING,      /* Name */
                                G_TYPE_STRING,      /* Company */
                                G_TYPE_STRING,      /* Model */
                                G_TYPE_INT,         /* ID */
                                G_TYPE_STRING,      /* Port */
                                G_TYPE_INT,         /* Speed */
                                G_TYPE_STRING,      /* CI-V */
                                G_TYPE_STRING,      /* DTR */
                                G_TYPE_STRING       /* RTS */
                               );
    
    /* Dummy rig is always number 1 */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        1, "Hamlib",
                        2, "DEMO",
                        3, 1,
                        4, "/dev/null",
                        -1);
    
    /* now add the configured radios */
    dirname = g_strconcat (g_get_home_dir (), G_DIR_SEPARATOR_S,
                           ".grig", NULL);
    dir = g_dir_open (dirname, 0, NULL);

    if (dir) {

        while ((filename = g_dir_read_name (dir))) {

            if (g_strrstr (filename, ".grc")) {

                /*buff = g_strconcat (dirname, G_DIR_SEPARATOR_S,
                                    filename, NULL);
*/
                vbuf = g_strsplit (filename, ".grc", 2);
                conf.name = g_strdup (vbuf[0]);
                g_strfreev(vbuf);
                
                if (radio_conf_read (&conf)) {
                    gtk_list_store_append (store, &iter);
                    gtk_list_store_set (store, &iter,
                                        0, conf.name,
                                        1, conf.company,
                                        2, conf.model,
                                        3, conf.id,
                                        4, conf.port,
                                        5, conf.speed,
                                        -1);
                    
                    if (conf.civ) {
                        buff = g_strdup_printf ("0x%X", conf.civ);
                        gtk_list_store_set (store, &iter, 6, buff, -1);
                        g_free (buff);
                    }
                    
                    switch (conf.dtr) {
                        
                        case LINE_ON:
                            gtk_list_store_set (store, &iter, 7, "ON", -1);
                            break;
                            
                        case LINE_PTT:
                            gtk_list_store_set (store, &iter, 7, "PTT", -1);
                            break;
                            
                        case LINE_CW:
                            gtk_list_store_set (store, &iter, 7, "CW", -1);
                            break;
                            
                        default:
                            gtk_list_store_set (store, &iter, 7, "OFF", -1);
                            break;
                    }

                    switch (conf.rts) {
                        
                        case LINE_ON:
                            gtk_list_store_set (store, &iter, 8, "ON", -1);
                            break;
                            
                        case LINE_PTT:
                            gtk_list_store_set (store, &iter, 8, "PTT", -1);
                            break;
                            
                        case LINE_CW:
                            gtk_list_store_set (store, &iter, 8, "CW", -1);
                            break;
                            
                        default:
                            gtk_list_store_set (store, &iter, 8, "OFF", -1);
                            break;
                    }
                    
                }
                
                /* clean up memmory */
                //g_free (buff);
                
                if (conf.name)
                    g_free (conf.name);
                
                if (conf.company)
                    g_free (conf.company);
                if (conf.model)
                    g_free (conf.model);
                
                if (conf.port)
                    g_free (conf.port);
                
            }
        }
    }

    g_free (dirname);
    g_dir_close (dir);

    
    return GTK_TREE_MODEL (store);
}


/** \brief Handle delete events.
 *  \param widget The widget which received the delete event signal.
 *  \param event  Data structure describing the event.
 *  \param data   User data (NULL).
 *  \param return Always FALSE to indicate that the app should be destroyed.
 *
 * This function handles the delete event received by the rig selector
 * window (eg. when the window is closed by the WM). This function simply
 * returns FALSE indicating that the main application window should be
 * destroyed by emiting the destroy signal.
 *
 */
static gint
rig_selector_delete      (GtkWidget *widget,
                          GdkEvent  *event,
                          gpointer   data)
{

    /* return FALSE so that Gtk+ will emit the destroy signal */
    return FALSE;
}



/** \brief Handle destroy signals.
 *  \param widget The widget which received the signal.
 *  \param data   User data (NULL).
 *
 * This function is called when the rig selector window receives the
 * destroy signal, ie. it is destroyed. This function signals all daemons
 * and other threads to stop and exits the Gtk+ main loop.
 *
 */
static void
rig_selector_destroy    (GtkWidget *widget,
                         gpointer   data)
{

    /* exit Gtk+ */
    gtk_main_quit ();
}


/** \brief Handle Cancel button signals.
 * \param button The Cancel button
 * \param window Pointer to the rig selector window.
 * 
 * This function is called when the user clicks on the Cancel button.
 * It simply destroys the rig selector window and returns control 
 * to the main() function.
 */
static void cancel (GtkWidget *button, gpointer window)
{
    gtk_widget_destroy (GTK_WIDGET (window));
}