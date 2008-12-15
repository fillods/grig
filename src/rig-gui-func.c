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
/** \brief FUNC controls
 *
 * This module implements all the FUNC controls except LOCK
 * which is handled in the main window.
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include <math.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "rig-gui-func.h"
#include "grig-debug.h"
#include "grig-menubar.h"

/* defined in main.c */
extern GtkWidget *grigapp;


static gint func_window_delete  (GtkWidget *widget, GdkEvent *event, gpointer data);
static void func_window_destroy (GtkWidget *widget, gpointer data);
static void create_controls   (GtkBox *box);
static void bool_state_cb    (GtkToggleButton *toggle_button, gpointer data);
static gboolean func_levels_update (gpointer data);


static GtkWidget *dialog;

static gboolean visible = FALSE;
static guint timerid = 0;


/* controls */
static GtkWidget *fctrls[RIG_SETTING_MAX];

/* handler ids */
static gulong hids[RIG_SETTING_MAX];


/** \brief Create level controls.
 *  \return A container widget containing the controls.
 *
 * This function creates and initializes the level controls for grig.
 * The controls are hidden by default using a GtkExpander widget.
 */
void
rig_gui_func_create ()
{
	GtkWidget *hbox;
	gchar     *title;


	if (visible) {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: FUNC window already visible."),
				  __FUNCTION__);

		return;
	}
	
	/* create hbox and add toggle buttons */
	hbox = gtk_hbox_new (TRUE, 5);
	create_controls (GTK_BOX (hbox));
	
	/* create dialog window */
	title = g_strdup_printf (_("%s (Special Functions)"),
				 gtk_window_get_title (GTK_WINDOW (grigapp)));

	/* create dialog window */
	dialog = gtk_dialog_new_with_buttons (title,
				 GTK_WINDOW (grigapp),
				 GTK_DIALOG_DESTROY_WITH_PARENT,
				 NULL);
	g_free (title);
	gtk_window_set_default_size (GTK_WINDOW (dialog), -1, 50);

	/* allow interaction with other windows */
	gtk_window_set_modal (GTK_WINDOW (dialog), FALSE);

	g_signal_connect (dialog, "delete_event",
			  G_CALLBACK (func_window_delete), NULL);    
 	g_signal_connect (dialog, "destroy",
			  G_CALLBACK (func_window_destroy), NULL);


	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), hbox);

	visible = TRUE;

	gtk_widget_show_all (dialog);

	/* start callback */
	timerid = g_timeout_add (1073, func_levels_update, NULL);
}


static gint
func_window_delete      (GtkWidget *widget,
		       GdkEvent  *event,
		       gpointer   data)
{

	/* force menu item to unset */
	grig_menubar_force_func_item (FALSE);

	/* return FALSE so that Gtk+ will emit the destroy signal */
	return FALSE;
}



static void
func_window_destroy    (GtkWidget *widget,
		      gpointer   data)
{

	/* stop callback */
	g_source_remove (timerid);
	timerid = 0;

	/* clear func-active flag in rig-data */

	visible = FALSE;

}


void
rig_gui_func_close ()
{
	if (!visible) {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: FUNC window is not visible."),
				  __FUNCTION__);

		return;
	}

	gtk_widget_destroy (dialog);

}


/** \brief Common callback used by bool state
 *
 */
static void
bool_state_cb (GtkToggleButton *toggle_button, gpointer data)
{
	int func = GPOINTER_TO_INT (data);
	int value = gtk_toggle_button_get_active (toggle_button);

	rig_data_set_func (func, value);

#if 0
	default:
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s:%d: Invalid level %d"),
				  __FILE__, __LINE__, level);
		break;
	}
#endif
}

static void
create_controls   (GtkBox *box)
{
	setting_t func;
	guint count = 0;
	int i;
	const gchar *funcstr;

	for (i=0; i<RIG_SETTING_MAX; i++) {

		func = rig_idx2setting(i);

		if (func == RIG_FUNC_LOCK)
			continue;

		funcstr = rig_strfunc(func);

		if (rig_data_has_set_func (func) && strlen(funcstr) != 0) {

			fctrls[i] = gtk_toggle_button_new_with_label (funcstr);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fctrls[i]), rig_data_get_func (func));
			hids[i] = g_signal_connect (fctrls[i], "toggled",
						G_CALLBACK (bool_state_cb),
						GINT_TO_POINTER (func));

			gtk_box_pack_start (GTK_BOX (box), fctrls[i], TRUE, TRUE, 0);
			count++;
		}
	}

	/* if no controls available, create a label
	   telling user why window is empty
	*/
	if (count == 0) {
		gtk_box_pack_start (box,
				    gtk_label_new (_("Rig has no support.")),
				    TRUE, TRUE, 0);
	}

}

static gboolean
func_levels_update (gpointer data)
{
	setting_t func;
	int i;

	for (i=0; i<RIG_SETTING_MAX; i++) {

		func = rig_idx2setting(i);

		if (func == RIG_FUNC_LOCK)
			continue;

		if (rig_data_has_get_func (func) && fctrls[i]) {
			g_signal_handler_block (fctrls[i], hids[i]);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fctrls[i]), rig_data_get_func (func));
			g_signal_handler_unblock (fctrls[i], hids[i]);
		}
	}

	return TRUE;
}

