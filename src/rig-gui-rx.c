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
/** \brief RX Level Controls
 *
 * This module implements the RX level controls consisting of:
 *
 *  - RIG_LEVEL_AF
 *  - RIG_LEVEL_RF
 *  - RIG_LEVEL_IF
 *  - RIG_LEVEL_CWPITCH
 *  - RIG_LEVEL_PBT_IN
 *  - RIG_LEVEL_PBT_OUT
 *  - RIG_LEVEL_APF
 *  - RIG_LEVEL_NR
 *  - RIG_LEVEL_NOTCH
 *  - RIG_LEVEL_SQL
 *  - RIG_LEVEL_BALANCE
 *
 * Note: In Gtk+ vertical sliders have their minimum on top (nice...)
 * so all values have to be inverted.
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include <math.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "rig-gui-rx.h"
#include "grig-debug.h"
#include "grig-menubar.h"

/* defined in main.c */
extern GtkWidget *grigapp;


static gint rx_window_delete  (GtkWidget *widget, GdkEvent *event, gpointer data);
static void rx_window_destroy (GtkWidget *widget, gpointer data);
static void create_controls   (GtkBox *box);
static void float_level_cb    (GtkRange *range, gpointer data);
static gchar *float_format_value_cb (GtkScale *scale, gdouble value);
static gchar *sfreq_format_value_cb (GtkScale *scale, gdouble value);
static gboolean rx_levels_update (gpointer data);



static GtkWidget *dialog;
static gboolean visible = FALSE;
static guint timerid = 0;

/* controls */
static GtkWidget *afs,*rfs,*ifs,*cwp,*pbti,*pbto,*apf,*nrs,*not,*sql,*bal;

/* handler ids */
static gulong afi,rfi,ifi,cwi,pbii,pboi,api,nri,noi,sqi,bai;


/** \brief Create level controls.
 *  \return A container widget containing the controls.
 *
 * This function creates and initializes the level controls for grig.
 * The controls are hidden by default using a GtkExpander widget.
 */
void
rig_gui_rx_create ()
{
	GtkWidget *hbox;
	gchar     *title;


	if (visible) {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: RX window already visible."),
				  __FUNCTION__);

		return;
	}

	/* create hbox and add sliders */
	hbox = gtk_hbox_new (TRUE, 5);
	create_controls (GTK_BOX (hbox));
	
	/* create dialog window */
	title = g_strdup_printf (_("%s (RX Levels)"),
				 gtk_window_get_title (GTK_WINDOW (grigapp)));
	dialog = gtk_dialog_new_with_buttons (title,
					      GTK_WINDOW (grigapp),
					      GTK_DIALOG_DESTROY_WITH_PARENT,
					      NULL);
	g_free (title);
	gtk_window_set_default_size (GTK_WINDOW (dialog), -1, 150);

	/* allow interaction with other windows */
	gtk_window_set_modal (GTK_WINDOW (dialog), FALSE);

	g_signal_connect (dialog, "delete_event",
			  G_CALLBACK (rx_window_delete), NULL);    
 	g_signal_connect (dialog, "destroy",
			  G_CALLBACK (rx_window_destroy), NULL);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), hbox);

	visible = TRUE;

	gtk_widget_show_all (dialog);

	/* start callback */
	timerid = g_timeout_add (1007, rx_levels_update, NULL);
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
	g_source_remove (timerid);
	timerid = 0;

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



/** \brief Common callback used by float levels
 *
 * - actually, int level are managed here, too
 */
static void
float_level_cb (GtkRange *range, gpointer data)
{
	int level = GPOINTER_TO_INT (data);
	float value = -1.0 * gtk_range_get_value (range);

	switch (level) {

	case RIG_LEVEL_AF:
		rig_data_set_afg (value);
		break;

	case RIG_LEVEL_RF:
		rig_data_set_rfg (value);
		break;

	case RIG_LEVEL_IF:
		rig_data_set_ifs ((int) value);
		break;

	case RIG_LEVEL_CWPITCH:
		rig_data_set_cwpitch ((int) value);
		break;

	case RIG_LEVEL_PBT_IN:
		rig_data_set_pbtin (value);
		break;

	case RIG_LEVEL_PBT_OUT:
		rig_data_set_pbtout (value);
		break;

	case RIG_LEVEL_APF:
		rig_data_set_apf (value);
		break;

	case RIG_LEVEL_NR:
		rig_data_set_nr (value);
		break;

	case RIG_LEVEL_NOTCHF:
		rig_data_set_notch ((int) value);
		break;

	case RIG_LEVEL_SQL:
		rig_data_set_sql (value);
		break;

	case RIG_LEVEL_BALANCE:
		rig_data_set_balance (value);
		break;

	default:
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s:%d: Invalid level %d"),
				  __FILE__, __LINE__, level);
		break;
	}
}

static gchar *
float_format_value_cb (GtkScale *scale, gdouble value)
{
	return g_strdup_printf ("%0.2f", -1.0 * value);
}

static gchar *
sfreq_format_value_cb (GtkScale *scale, gdouble value)
{
	if (fabs (value) <= 999) {
		return g_strdup_printf ("%0.0fHz", -1.0 * value);
	}
	else {
		value = value / 1000.0;
		return g_strdup_printf ("%0.2fkHz", -1.0 * value);
	}
}


static void
create_controls   (GtkBox *box)
{
	GtkWidget *label;
	GtkWidget *vbox;
	guint count = 0;

	/* afs */
	if (rig_data_has_set_afg ()) {
		afs = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (afs), -1.0*rig_data_get_afg ());
		afi = g_signal_connect (afs, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_AF));
		g_signal_connect (afs, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("AF Gain"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), afs, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* rfs */
	if (rig_data_has_set_rfg ()) {
		rfs = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (rfs), -1.0*rig_data_get_rfg ());
		rfi = g_signal_connect (rfs, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_RF));
		g_signal_connect (rfs, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("RF Gain"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), rfs, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* ifs */
	if (rig_data_has_set_ifs ()) {
		if (rig_data_get_ifsmax () > 0) {
			ifs = gtk_vscale_new_with_range (-rig_data_get_ifsmax (),
							 rig_data_get_ifsmax (),
							 10.0);
		}
		else {
			ifs = gtk_vscale_new_with_range (-10000, 10000, 10);
		}
		gtk_range_set_value (GTK_RANGE (ifs), -1.0*rig_data_get_ifs ());
		ifi = g_signal_connect (ifs, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_IF));
		g_signal_connect (ifs, "format-value",
				  G_CALLBACK (sfreq_format_value_cb),
				  NULL);
		label = gtk_label_new (_("IF Shift"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), ifs, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* cwp */
	if (rig_data_has_set_cwpitch ()) {
		cwp = gtk_vscale_new_with_range (-1000, -500, 10.0);
		gtk_range_set_value (GTK_RANGE (cwp), -1.0*rig_data_get_cwpitch ());
		cwi = g_signal_connect (cwp, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_CWPITCH));
		g_signal_connect (cwp, "format-value",
				  G_CALLBACK (sfreq_format_value_cb),
				  NULL);
		label = gtk_label_new (_("CW Pitch"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), cwp, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* pbti */
	if (rig_data_has_set_pbtin ()) {
		pbti = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (pbti), -1.0*rig_data_get_pbtin ());
		pbii = g_signal_connect (pbti, "value-changed",
					 G_CALLBACK (float_level_cb),
					 GINT_TO_POINTER (RIG_LEVEL_PBT_IN));
		g_signal_connect (pbti, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("PBT In"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), pbti, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* pbto */
	if (rig_data_has_set_pbtout ()) {
		pbto = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (pbto), -1.0*rig_data_get_pbtout ());
		pboi = g_signal_connect (pbto, "value-changed",
					 G_CALLBACK (float_level_cb),
					 GINT_TO_POINTER (RIG_LEVEL_PBT_OUT));
		g_signal_connect (pbto, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("PBT Out"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), pbto, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}


	/* apf */
	if (rig_data_has_set_apf ()) {
		apf = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (apf), -1.0*rig_data_get_apf ());
		api = g_signal_connect (apf, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_APF));
		g_signal_connect (apf, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("APF"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), apf, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* nrs */
	if (rig_data_has_set_nr ()) {
		nrs = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (nrs), -1.0*rig_data_get_nr ());
		nri = g_signal_connect (nrs, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_NR));
		g_signal_connect (nrs, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("N.R."));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), nrs, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* not */
	if (rig_data_has_set_notch ()) {
		not = gtk_vscale_new_with_range (-3000, -500, 10.0);
		gtk_range_set_value (GTK_RANGE (not), -1.0*rig_data_get_notch ());
		noi = g_signal_connect (not, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_NOTCHF));
		g_signal_connect (not, "format-value",
				  G_CALLBACK (sfreq_format_value_cb),
				  NULL);
		label = gtk_label_new (_("NOTCH"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), not, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* sql */
	if (rig_data_has_set_sql ()) {
		sql = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (sql), -1.0*rig_data_get_sql ());
		sqi = g_signal_connect (sql, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_SQL));
		g_signal_connect (sql, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("Squelch"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), sql, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* bal */
	if (rig_data_has_set_balance ()) {
		bal = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (bal), -1.0*rig_data_get_balance ());
		bai = g_signal_connect (bal, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_BALANCE));
		g_signal_connect (bal, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("Balance"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), bal, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
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
rx_levels_update (gpointer data)
{
	/* afs */
	if (rig_data_has_get_afg ()) {
		g_signal_handler_block (afs, afi);
		gtk_range_set_value (GTK_RANGE (afs), -1.0*rig_data_get_afg ());
		g_signal_handler_unblock (afs, afi);
	}


	/* rfs */
	if (rig_data_has_get_rfg ()) {
		g_signal_handler_block (rfs, rfi);
		gtk_range_set_value (GTK_RANGE (rfs), -1.0*rig_data_get_rfg ());
		g_signal_handler_unblock (rfs, rfi);
	}


	/* ifs */
	if (rig_data_has_get_ifs ()) {
		g_signal_handler_block (ifs, ifi);
		gtk_range_set_value (GTK_RANGE (ifs), -1.0*rig_data_get_ifs ());
		g_signal_handler_unblock (ifs, ifi);
	}


	/* cwp */
	if (rig_data_has_get_cwpitch ()) {
		g_signal_handler_block (cwp, cwi);
		gtk_range_set_value (GTK_RANGE (cwp), -1.0*rig_data_get_cwpitch ());
		g_signal_handler_unblock (cwp, cwi);
	}


	/* pbti */
	if (rig_data_has_get_pbtin ()) {
		g_signal_handler_block (pbti, pbii);
		gtk_range_set_value (GTK_RANGE (pbti), -1.0*rig_data_get_pbtin ());
		g_signal_handler_unblock (pbti, pbii);
	}


	/* pbto */
	if (rig_data_has_get_pbtout ()) {
		g_signal_handler_block (pbto, pboi);
		gtk_range_set_value (GTK_RANGE (pbto), -1.0*rig_data_get_pbtout ());
		g_signal_handler_unblock (pbto, pboi);
	}


	/* apf */
	if (rig_data_has_get_apf ()) {
		g_signal_handler_block (apf, api);
		gtk_range_set_value (GTK_RANGE (apf), -1.0*rig_data_get_apf ());
		g_signal_handler_unblock (apf, api);
	}


	/* nrs */
	if (rig_data_has_get_nr ()) {
		g_signal_handler_block (nrs, nri);
		gtk_range_set_value (GTK_RANGE (nrs), -1.0*rig_data_get_nr ());
		g_signal_handler_unblock (nrs, nri);
	}


	/* not */
	if (rig_data_has_get_notch ()) {
		g_signal_handler_block (not, noi);
		gtk_range_set_value (GTK_RANGE (not), -1.0*rig_data_get_notch ());
		g_signal_handler_unblock (not, noi);
	}


	/* sql */
	if (rig_data_has_get_sql ()) {
		g_signal_handler_block (sql, sqi);
		gtk_range_set_value (GTK_RANGE (sql), -1.0*rig_data_get_sql ());
		g_signal_handler_unblock (sql, sqi);
	}


	/* bal */
	if (rig_data_has_get_balance ()) {
		g_signal_handler_block (bal, bai);
		gtk_range_set_value (GTK_RANGE (bal), -1.0*rig_data_get_balance ());
		g_signal_handler_unblock (bal, bai);
	}


	return TRUE;
}

