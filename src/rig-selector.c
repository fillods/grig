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
#include <glib/gstdio.h>
#include <hamlib/rig.h>
#include "compat.h"
#include "grig-debug.h"
#include "radio-conf.h"
#include "rig-selector.h"


/* private function declarations */
static gint rig_selector_delete     (GtkWidget *, GdkEvent *, gpointer);
static void rig_selector_destroy    (GtkWidget *, gpointer);
static void add     (GtkWidget *, gpointer);
static void delete  (GtkWidget *, gpointer);
static void edit    (GtkWidget *, gpointer);
static void cancel  (GtkWidget *, gpointer);
static void connect (GtkWidget *, gpointer);
static void selection_changed (GtkTreeSelection *sel, gpointer data);

static void render_civ (GtkTreeViewColumn *col,
                        GtkCellRenderer   *renderer,
                        GtkTreeModel      *model,
                        GtkTreeIter       *iter,
                        gpointer           column);
static void render_dtr_rts (GtkTreeViewColumn *col,
                            GtkCellRenderer   *renderer,
                            GtkTreeModel      *model,
                            GtkTreeIter       *iter,
                            gpointer           column);
                                       
static void create_rig_list (void);
static GtkTreeModel *create_model (void);

static gchar *selected = NULL;
static GtkWidget *riglist;


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
    GtkWidget   *swin;
    GtkTreeSelection *sel;

    
    /* radio list */
    create_rig_list ();
    swin = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (swin), riglist);
    gtk_widget_show_all (swin);

    sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (riglist));
    gtk_tree_selection_set_mode (sel, GTK_SELECTION_SINGLE);
    
    /* connect button */
    conbut = gtk_button_new_from_stock (GTK_STOCK_CONNECT);
    gtk_widget_set_sensitive (conbut, FALSE);
    gtk_widget_set_tooltip_text (conbut,
                          _("Connect to the selected radio."
                            "Grig will attempt to establish connection to the "
                            "selected radio using the specified settings. If "
                            "the connection is successful, the main application "
                            "window will be loaded."));
    
    /* cancel button */
    cancbut = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
    gtk_widget_set_tooltip_text (cancbut,
                          _("Cancel radio selection. This will end grig."));
    
    /* add button */
    newbut = gtk_button_new_from_stock (GTK_STOCK_ADD);
    gtk_widget_set_tooltip_text (newbut,
                          _("Add a new radio to the list."
                            "A new configuration window will be shown allowing "
                            "you to select a radio and specify the connection "
                            "settings."));
    
    /* delete button */
    delbut = gtk_button_new_from_stock (GTK_STOCK_DELETE);
    gtk_widget_set_sensitive (delbut, FALSE);
    gtk_widget_set_tooltip_text (delbut,
                          _("Delete the currently selected radio."));
    
    /* edit button */
    editbut = gtk_button_new_from_stock (GTK_STOCK_EDIT);
    gtk_widget_set_sensitive (editbut, FALSE);
    gtk_widget_set_tooltip_text (editbut,
                          _("Edit the settings for the currently selected radio."));
    
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

    g_object_set_data (G_OBJECT(window), "conbut", conbut);
    g_object_set_data (G_OBJECT(window), "delbut", delbut);
    g_object_set_data (G_OBJECT(window), "editbut", editbut);
    g_object_set_data (G_OBJECT(window), "list", riglist);
    
    /* connect signals */
    g_signal_connect (G_OBJECT (cancbut), "clicked",
                      G_CALLBACK (cancel), window);
    g_signal_connect (G_OBJECT (conbut), "clicked",
                      G_CALLBACK (connect), window);
    g_signal_connect (G_OBJECT (delbut), "clicked",
                      G_CALLBACK (delete), NULL);
    g_signal_connect (G_OBJECT (newbut), "clicked",
                      G_CALLBACK (add), NULL);
    g_signal_connect (G_OBJECT (editbut), "clicked",
                      G_CALLBACK (edit), NULL);
    g_signal_connect (sel, "changed", G_CALLBACK(selection_changed), window);
    
    /* show window */
    gtk_widget_show_all (window);
    
    /* enter main loop that will only be quit
       when window is destroyed */
    gtk_main ();
    
    return selected;
}


static void create_rig_list (void)
{
    GtkTreeModel      *model;
    GtkCellRenderer   *renderer;
    GtkTreeViewColumn *column;

    
    riglist = gtk_tree_view_new ();
    
    model = create_model ();
    gtk_tree_view_set_model (GTK_TREE_VIEW (riglist), model);
    g_object_unref (model);
    //gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (riglist), TRUE);
    
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
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_civ, GUINT_TO_POINTER(6), NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* DTR:7 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("DTR"), renderer,
            "text", 7, NULL);
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_dtr_rts, GUINT_TO_POINTER(7), NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* RTS:8 */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("RTS"), renderer,
            "text", 8, NULL);
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_dtr_rts, GUINT_TO_POINTER(8), NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

}


static GtkTreeModel *create_model ()
{
    GtkListStore *store;
    GtkTreeIter iter;
    GDir         *dir = NULL;   /* directory handle */
    gchar        *dirname;      /* directory name */
    const gchar  *filename;     /* file name */
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
                                G_TYPE_INT,         /* CI-V */
                                G_TYPE_INT,      /* DTR */
                                G_TYPE_INT       /* RTS */
                               );
    
    /* Dummy rig is always number 1 */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        0, "dummy",
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
                                        6, conf.civ,
                                        7, conf.dtr,
                                        8, conf.rts,
                                        -1);
                    
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
    if (selected != NULL) {
        g_free (selected);
        selected = NULL;
    }
    gtk_widget_destroy (GTK_WIDGET (window));
}

/** \brief Handle Connect button signals.
 * \param button The Connect button
 * \param window Pointer to the rig selector window.
 * 
 * This function is called when the user clicks on the Connect button.
 * It storest the name of the currently selected radio configuration and
 * simply destroys the rig selector window and whereby control is returned
 * to the main() function.
 */
static void connect (GtkWidget *button, gpointer window)
{
    
    
    
    gtk_widget_destroy (GTK_WIDGET (window));
}




/** \brief Handle delete button signals */
static void delete (GtkWidget *button, gpointer data)
{
    GtkTreeSelection *sel;
    GtkTreeModel *model;
    GtkTreeIter   iter;
    gboolean      havesel = FALSE;
    gchar        *name,*fname;

    
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (riglist));
    sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (riglist));
    havesel = gtk_tree_selection_get_selected (sel, NULL, &iter);

    if (havesel) {
        gtk_tree_model_get (model, &iter, 0, &name, -1);
        fname = g_strconcat (g_get_home_dir(), G_DIR_SEPARATOR_S,
                             ".grig", G_DIR_SEPARATOR_S,
                             name, ".grc", NULL);
        g_free (name);
        
        // gtk_list_store_remove crashes no matter what...
        // the same code works in gpredict 
        //gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
        gtk_list_store_clear (GTK_LIST_STORE (model));

        
        /* delete .grc file and remove entry from riglist */
        if (g_remove (fname)) {
            grig_debug_local (RIG_DEBUG_ERR,
                              _("%s:%s: Failed to delete %s"),
                                __FILE__, __FUNCTION__, fname);
        }
        else {
            grig_debug_local (RIG_DEBUG_VERBOSE,
                              _("%s:%s: Removed %s"),
                                __FILE__, __FUNCTION__, fname);
                    

        }
        g_free (fname);
    }
}


static void add     (GtkWidget *button, gpointer data)
{
    g_print ("TO BE IMPLEMENTED\n");
}


static void edit    (GtkWidget *button, gpointer data)
{
    g_print ("TO BE IMPLEMENTED\n");
}




static void selection_changed (GtkTreeSelection *sel, gpointer data)
{
    GtkWidget    *window = GTK_WIDGET(data);
    GtkWidget    *conbut,*editbut,*delbut;
    GtkTreeModel *model;
    GtkTreeIter   iter;
    guint         id;
    gboolean      havesel = FALSE;
    
    /* get tree view & co */
    gtk_tree_selection_get_tree_view (sel);
    havesel = gtk_tree_selection_get_selected (sel, &model, &iter);
    gtk_tree_model_get (model, &iter, 3, &id, -1);
    
    /* set selection */
    if (selected) {
        g_free (selected);
        selected = NULL;
    }
    if (havesel) {
        gtk_tree_model_get (model, &iter, 0, &selected, -1);
        g_print ("SEELCTED: %s\n", selected);
    }
    
    /* get buttons */
    delbut = GTK_WIDGET (g_object_get_data (G_OBJECT(window), "delbut"));
    editbut = GTK_WIDGET (g_object_get_data (G_OBJECT(window), "editbut"));
    conbut = GTK_WIDGET (g_object_get_data (G_OBJECT(window), "conbut"));
    
    /* Dummy can't be deleted or edited */
    if (id == 1) {
        /* disable delete and edit buttons */
        gtk_widget_set_sensitive (delbut, FALSE);
        gtk_widget_set_sensitive (editbut, FALSE);
        
        /* enable connect button */
        gtk_widget_set_sensitive (conbut, TRUE);
    }
    else if (havesel) {
        
        /* enable all three buttons */
        gtk_widget_set_sensitive (conbut, TRUE);
        gtk_widget_set_sensitive (delbut, TRUE);
        gtk_widget_set_sensitive (editbut, TRUE);
    }
    else {
        /* disable all three buttons */
        gtk_widget_set_sensitive (conbut, FALSE);
        gtk_widget_set_sensitive (delbut, FALSE);
        gtk_widget_set_sensitive (editbut, FALSE);
    }
}


/** \brief Render CIV address. */
static void render_civ (GtkTreeViewColumn *col,
                        GtkCellRenderer   *renderer,
                        GtkTreeModel      *model,
                        GtkTreeIter       *iter,
                        gpointer           column)
{
    guint    number;
    gchar  *buff;
    guint   coli = GPOINTER_TO_UINT (column);
    
    gtk_tree_model_get (model, iter, coli, &number, -1);

    if (number > 0)
        buff = g_strdup_printf ("0x%X", number);
    else
        buff = g_strdup_printf (" ");
        
    g_object_set (renderer, "text", buff, NULL);
    g_free (buff);
}

/** \brief Render DTR or RTS columns address. */
static void render_dtr_rts (GtkTreeViewColumn *col,
                            GtkCellRenderer   *renderer,
                            GtkTreeModel      *model,
                            GtkTreeIter       *iter,
                            gpointer           column)
{
    guint    number;
    guint   coli = GPOINTER_TO_UINT (column);
    
    gtk_tree_model_get (model, iter, coli, &number, -1);

    switch (number) {
                        
        case LINE_ON:
            g_object_set (renderer, "text", "ON", NULL);
            break;
                            
        case LINE_PTT:
            g_object_set (renderer, "text", "PTT", NULL);
            break;
                            
        case LINE_CW:
            g_object_set (renderer, "text", "CW", NULL);
            break;
                            
        default:
            g_object_set (renderer, "text", "OFF", NULL);
            break;
        
    }
    
}

