/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offse: 4 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2007  Alexandru Csete.

    Authors: Alexandru Csete <csete@users.sourceforge.net>

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

#include "rig-selector.h"


/* private function declarations */
static gint rig_selector_delete     (GtkWidget *, GdkEvent *, gpointer);
static void rig_selector_destroy    (GtkWidget *, gpointer);
static void rig_selector_new_cb     (GtkWidget *, gpointer);
static void rig_selector_del_cb     (GtkWidget *, gpointer);
static void rig_selector_edit_cb    (GtkWidget *, gpointer);
static void rig_selector_cancel_cb  (GtkWidget *, gpointer);
static void rig_selector_connect_cb (GtkWidget *, gpointer);



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
 * The radio configurations are stored in $HOME/.grig/xyz.radio files
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
    
    /* add nutton */
    newbut = gtk_button_new_from_stock (GTK_STOCK_NEW);
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
    
    /* vertical box */
    vbox = gtk_vbox_new (FALSE, 10);
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
    gtk_container_add (GTK_CONTAINER (window), vbox);

    /* connect delete and destroy signals */
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (rig_selector_delete), NULL);
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (rig_selector_destroy), NULL);

    /* show window */
    gtk_widget_show_all (window);
    
    /* enter main loop that will only be quit
       when window is destroyed */
    gtk_main ();
    
    return NULL;
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

