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
/** \brief TX level controls
 *
 * This module implements the TX level controls consisting of:
 *
 * - RIG_LEVEL_RFPOWER    W
 * - RIG_LEVEL_MICGAIN   RW
 * - RIG_LEVEL_KEYSPD    RW
 * - RIG_LEVEL_COMP       W
 * - RIG_LEVEL_ALC        W
 * - RIG_LEVEL_BKINDL    RW
 * - RIG_LEVEL_VOXGAIN   RW
 * - RIG_LEVEL_VOXDELAY  RW
 * - RIG_LEVEL_ANTIVOX   RW
 *
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
#include "rig-gui-tx.h"
#include "grig-debug.h"
#include "grig-menubar.h"

/* defined in main.c */
extern GtkWidget *grigapp;


static gint tx_window_delete  (GtkWidget *widget, GdkEvent *event, gpointer data);
static void tx_window_destroy (GtkWidget *widget, gpointer data);
static void create_controls   (GtkBox *box);
static void float_level_cb    (GtkRange *range, gpointer data);
static gchar *float_format_value_cb (GtkScale *scale, gdouble value);
static gchar *wpm_format_value_cb (GtkScale *scale, gdouble value);
static gchar *delay_format_value_cb (GtkScale *scale, gdouble value);
static gboolean tx_levels_update (gpointer data);


static GtkWidget *dialog;

static gboolean visible = FALSE;
static guint timerid = 0;


/* controls */
static GtkWidget *kss,*bks,*rfs,*als,*mgs,*cps,*vgs,*vds,*avs;

/* handler ids */
static gulong ksi,bki,mgi,vgi,vdi,avi;


/** \brief Create level controls.
 *  \return A container widget containing the controls.
 *
 * This function creates and initializes the level controls for grig.
 * The controls are hidden by default using a GtkExpander widget.
 */
void
rig_gui_tx_create ()
{
	GtkWidget *hbox;
	gchar     *title;


	if (visible) {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: TX window already visible."),
				  __FUNCTION__);

		return;
	}
	
	/* create hbox and add sliders */
	hbox = gtk_hbox_new (TRUE, 5);
	create_controls (GTK_BOX (hbox));
	
	/* create dialog window */
	title = g_strdup_printf (_("%s (TX Levels)"),
				 gtk_window_get_title (GTK_WINDOW (grigapp)));

	/* create dialog window */
	dialog = gtk_dialog_new_with_buttons (title,
				 GTK_WINDOW (grigapp),
				 GTK_DIALOG_DESTROY_WITH_PARENT,
				 NULL);
	g_free (title);
	gtk_window_set_default_size (GTK_WINDOW (dialog), -1, 150);

	/* allow interaction with other windows */
	gtk_window_set_modal (GTK_WINDOW (dialog), FALSE);

	g_signal_connect (dialog, "delete_event",
			  G_CALLBACK (tx_window_delete), NULL);    
 	g_signal_connect (dialog, "destroy",
			  G_CALLBACK (tx_window_destroy), NULL);


	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), hbox);

	visible = TRUE;

	gtk_widget_show_all (dialog);

	/* start callback */
	timerid = g_timeout_add (1073, tx_levels_update, NULL);
}


static gint
tx_window_delete      (GtkWidget *widget,
		       GdkEvent  *event,
		       gpointer   data)
{

	/* force menu item to unset */
	grig_menubar_force_tx_item (FALSE);

	/* return FALSE so that Gtk+ will emit the destroy signal */
	return FALSE;
}



static void
tx_window_destroy    (GtkWidget *widget,
		      gpointer   data)
{

	/* stop callback */
	g_source_remove (timerid);
	timerid = 0;

	/* clear tx-active flag in rig-data */

	visible = FALSE;

}


void
rig_gui_tx_close ()
{
	if (!visible) {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: TX window is not visible."),
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

		/* FIXME */
	case RIG_LEVEL_RFPOWER:
		rig_data_set_power (value);
		break;

	case RIG_LEVEL_MICGAIN:
		rig_data_set_micg (value);
		break;

	case RIG_LEVEL_KEYSPD:
		rig_data_set_keyspd ((int) value);
		break;

	case RIG_LEVEL_COMP:
		rig_data_set_comp ( value);
		break;

	case RIG_LEVEL_ALC:
		rig_data_set_alc (value);
		break;

	case RIG_LEVEL_BKINDL:
		rig_data_set_bkindel ((int) value);
		break;

	case RIG_LEVEL_VOXGAIN:
		rig_data_set_voxg (value);
		break;

	case RIG_LEVEL_VOXDELAY:
		rig_data_set_voxdel ((int) value);
		break;

	case RIG_LEVEL_ANTIVOX:
		rig_data_set_antivox (value);
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
wpm_format_value_cb (GtkScale *scale, gdouble value)
{
	return g_strdup_printf ("%0.0f WPM", -1.0 * value);
}

static gchar *
delay_format_value_cb (GtkScale *scale, gdouble value)
{
	if (fabs (value) < 1000) {
		return g_strdup_printf ("%0.0f ms", -1.0 * value);
	}
	else {
		value = value / 1000.0;
		return g_strdup_printf ("%0.2f s", -1.0 * value);
	}
}


static void
create_controls   (GtkBox *box)
{
	GtkWidget *label;
	GtkWidget *vbox;
	guint count = 0;

	/* kss */
	if (rig_data_has_set_keyspd ()) {
		kss = gtk_vscale_new_with_range (-50.0, -1.0, 1.0);
		gtk_range_set_value (GTK_RANGE (kss), -1.0*rig_data_get_keyspd ());
		ksi = g_signal_connect (kss, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_KEYSPD));
		g_signal_connect (kss, "format-value",
				  G_CALLBACK (wpm_format_value_cb),
				  NULL);
		label = gtk_label_new (_("CW SPD"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), kss, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* bks */
	if (rig_data_has_set_bkindel ()) {
		bks = gtk_vscale_new_with_range (-10000.0, 0.0, 10.0);
		gtk_range_set_value (GTK_RANGE (bks), -1.0*rig_data_get_bkindel ());
		bki = g_signal_connect (bks, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_BKINDL));
		g_signal_connect (bks, "format-value",
				  G_CALLBACK (delay_format_value_cb),
				  NULL);
		label = gtk_label_new (_("BKIN DEL"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), bks, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* rfs */
	if (rig_data_has_set_power ()) {
		rfs = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (rfs), -1.0*rig_data_get_power ());
		g_signal_connect (rfs, "value-changed",
				  G_CALLBACK (float_level_cb),
				  GINT_TO_POINTER (RIG_LEVEL_RFPOWER));
		g_signal_connect (rfs, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("POWER"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), rfs, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}


	/* als */
	if (rig_data_has_set_alc ()) {
		als = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (als), -1.0*rig_data_get_alc ());
		g_signal_connect (als, "value-changed",
				  G_CALLBACK (float_level_cb),
				  GINT_TO_POINTER (RIG_LEVEL_ALC));
		g_signal_connect (als, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("ALC"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), als, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}


	/* mgs */
	if (rig_data_has_set_micg ()) {
		mgs = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (mgs), -1.0*rig_data_get_micg ());
		mgi = g_signal_connect (mgs, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_MICGAIN));
		g_signal_connect (mgs, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("MIC GAIN"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), mgs, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* cps */
	if (rig_data_has_set_comp ()) {
		cps = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (cps), -1.0*rig_data_get_comp ());
		g_signal_connect (cps, "value-changed",
				  G_CALLBACK (float_level_cb),
				  GINT_TO_POINTER (RIG_LEVEL_COMP));
		g_signal_connect (cps, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("COMPR"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), cps, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* vgs */
	if (rig_data_has_set_voxg ()) {
		vgs = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (cps), -1.0*rig_data_get_voxg ());
		vgi = g_signal_connect (vgs, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_VOXGAIN));
		g_signal_connect (vgs, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("VOX GAIN"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), vgs, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* vds */
	if (rig_data_has_set_voxdel ()) {
		vds = gtk_vscale_new_with_range (-10000.0, 0.0, 10.0);
		gtk_range_set_value (GTK_RANGE (bks), -1.0*rig_data_get_voxdel ());
		vdi = g_signal_connect (vds, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_VOXDELAY));
		g_signal_connect (vds, "format-value",
				  G_CALLBACK (delay_format_value_cb),
				  NULL);
		label = gtk_label_new (_("VOX DEL"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), vds, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (box, vbox, TRUE, TRUE, 0);
		count++;
	}

	/* avs */
	if (rig_data_has_set_antivox ()) {
		avs = gtk_vscale_new_with_range (-1.0, 0.0, 0.01);
		gtk_range_set_value (GTK_RANGE (avs), -1.0*rig_data_get_antivox ());
		avi = g_signal_connect (avs, "value-changed",
					G_CALLBACK (float_level_cb),
					GINT_TO_POINTER (RIG_LEVEL_ANTIVOX));
		g_signal_connect (avs, "format-value",
				  G_CALLBACK (float_format_value_cb),
				  NULL);
		label = gtk_label_new (_("ANTI VOX"));
		gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), avs, TRUE, TRUE, 0);
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
tx_levels_update (gpointer data)
{
	/* kss */
	if (rig_data_has_get_keyspd ()) {
		g_signal_handler_block (kss, ksi);
		gtk_range_set_value (GTK_RANGE (kss), -1.0*rig_data_get_keyspd ());
		g_signal_handler_unblock (kss, ksi);
	}

	/* bks */
	if (rig_data_has_get_bkindel ()) {
		g_signal_handler_block (bks, bki);
		gtk_range_set_value (GTK_RANGE (bks), -1.0*rig_data_get_bkindel ());
		g_signal_handler_unblock (bks, bki);
	}

	/* mgs */
	if (rig_data_has_get_micg ()) {
		g_signal_handler_block (mgs, mgi);
		gtk_range_set_value (GTK_RANGE (mgs), -1.0*rig_data_get_micg ());
		g_signal_handler_unblock (mgs, mgi);
	}

	/* vgs */
	if (rig_data_has_get_voxg ()) {
		g_signal_handler_block (vgs, vgi);
		gtk_range_set_value (GTK_RANGE (vgs), -1.0*rig_data_get_voxg ());
		g_signal_handler_unblock (vgs, vgi);
	}

	/* vds */
	if (rig_data_has_get_voxdel ()) {
		g_signal_handler_block (vds, vdi);
		gtk_range_set_value (GTK_RANGE (vds), -1.0*rig_data_get_voxdel ());
		g_signal_handler_unblock (vds, vdi);
	}

	/* avs */
	if (rig_data_has_get_antivox ()) {
		g_signal_handler_block (avs, avi);
		gtk_range_set_value (GTK_RANGE (avs), -1.0*rig_data_get_antivox ());
		g_signal_handler_unblock (avs, avi);
	}

	return TRUE;
}
