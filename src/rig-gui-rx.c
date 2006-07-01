/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2006  Alexandru Csete.

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
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "rig-gui-rx.h"
#include "grig-debug.h"

/* defined in main.c */
extern GtkWidget *grigapp;


static gint rx_window_delete  (GtkWidget *widget, GdkEvent *event, gpointer data);
static void rx_window_destroy (GtkWidget *widget, gpointer data);

static GtkWidget *dialog;

static gboolean visible = FALSE;


/** \brief Create level controls.
 *  \return A container widget containing the controls.
 *
 * This function creates and initializes the level controls for grig.
 * The controls are hidden by default using a GtkExpander widget.
 */
void
rig_gui_rx_create ()
{

	if (visible) {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: RX window already visible."),
				  __FUNCTION__);

		return;
	}
	
	/* create dialog window */
	dialog = gtk_dialog_new_with_buttons (_("RX"),
				 GTK_WINDOW (grigapp),
				 GTK_DIALOG_DESTROY_WITH_PARENT,
				 NULL);

	/* allow interaction with other windows */
	gtk_window_set_modal (GTK_WINDOW (dialog), FALSE);

	g_signal_connect (dialog, "delete_event",
			  G_CALLBACK (rx_window_delete), NULL);    
 	g_signal_connect (dialog, "destroy",
			  G_CALLBACK (rx_window_destroy), NULL);

	//gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), table);

	visible = TRUE;

	gtk_widget_show_all (dialog);
}


static gint
rx_window_delete      (GtkWidget *widget,
		       GdkEvent  *event,
		       gpointer   data)
{

	/* force menu item to unset */
	grig_menubar_force_rx_item (FALSE);

	/* return FALSE so that Gtk+ will emit the destroy signal */
	return FALSE;
}



static void
rx_window_destroy    (GtkWidget *widget,
		      gpointer   data)
{

	/* stop callback */

	/* clear rx-active flag in rig-data */

	visible = FALSE;

}


void
rig_gui_rx_close ()
{
	if (!visible) {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: RX window is not visible."),
				  __FUNCTION__);

		return;
	}

	gtk_widget_destroy (dialog);

}
