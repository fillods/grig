/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2004  Alexandru Csete.

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
    along with this program; if not, write to the
          Free Software Foundation, Inc.,
	  59 Temple Place, Suite 330,
	  Boston, MA  02111-1307
	  USA
*/
/** \file rig-gui-smeter.c
 *  \ingroup smeter
 *  \brief Signal strength meter widget.
 *
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "rig-data.h"
#include "rig-gui-smeter-conv.h"
#include "rig-gui-smeter.h"




/* smeter */
static smeter_t smeter;

/* TX mode strings used for optionmenu */
static const gchar *TX_MODE_S[] = {
	N_("None"),
	N_("Power"),
	N_("SWR"),
	N_("ALC"),
	N_("Comp."),
	N_("IC")
};


/* TX scale strings used for optionmenu */
static const gchar *TX_SCALE_S[] = {
	N_("0..5"),
	N_("0..10"),
	N_("0..50"),
	N_("0..100"),
	N_("0..500")
};



/* private fuunction prototypes */
static void       rig_gui_smeter_create_canvas  (void);
static GtkWidget *rig_gui_mode_selector_create  (void);
static GtkWidget *rig_gui_scale_selector_create (void);

static gint rig_gui_smeter_timeout_exec  (gpointer);
static gint rig_gui_smeter_timeout_stop  (gpointer);

static void rig_gui_smeter_mode_cb     (GtkWidget *, gpointer);
static void rig_gui_smeter_scale_cb    (GtkWidget *, gpointer);


/** \brief Create signal strength meter widget.
 *  \return A mega widget containing the signal strength meter.
 *
 * This function creates creates and initializs the signal strength meter
 * and some other related widgets, like the option menu which is used to
 * select the functionality of the meter.
 */
GtkWidget *
rig_gui_smeter_create ()
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *frame;
	guint      timerid;

	/* initialize some data */
	smeter.value = convert_db_to_angle (-54, DB_TO_ANGLE_MODE_POLY);
	smeter.lastvalue = smeter.value;
	smeter.tval = RIG_GUI_SMETER_DEF_TVAL;
	smeter.falloff = RIG_GUI_SMETER_DEF_FALLOFF;
	smeter.txmode = SMETER_TX_MODE_NONE;
	smeter.scale = SMETER_SCALE_100;

	/* create horizontal box containing selectors */
	hbox = gtk_hbox_new (TRUE, 0);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), rig_gui_scale_selector_create ());
	gtk_box_pack_start_defaults (GTK_BOX (hbox), rig_gui_mode_selector_create ());

	/* disable for now (unsupported) */
	gtk_widget_set_sensitive (hbox, FALSE);

	/* create cnvas */
	rig_gui_smeter_create_canvas ();

	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
	gtk_container_add (GTK_CONTAINER (frame), smeter.canvas);

	/* create vertical box */
	vbox = gtk_vbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox), hbox,  FALSE, FALSE, 0);

	/* start readback timer but only if service is available */
	if (rig_data_has_get_strength ()) {
		timerid = gtk_timeout_add (RIG_GUI_SMETER_DEF_TVAL,
					   rig_gui_smeter_timeout_exec,
					   NULL);

		/* register timer_stop function at exit */
		gtk_quit_add (gtk_main_level (), rig_gui_smeter_timeout_stop,
			      GUINT_TO_POINTER (timerid));
	}

	return vbox;
}


/** \brief Create canvas widget.
 *
 * This function creates the GnomeCanvas widget, loads the background pixmap and
 * creates the needle GnomeCanvasItem. The canvas and needle widgets are stored in
 * the global smeter structure.
 */
static void
rig_gui_smeter_create_canvas (void)
{
	GdkPixbuf         *pixbuf;
	gchar             *fname;
	coordinate_t       coor;
	GnomeCanvasPoints *points;

	/* create canvas */
	smeter.canvas = gnome_canvas_new_aa ();
	gtk_widget_set_usize (smeter.canvas, 160, 80);
	gnome_canvas_set_scroll_region (GNOME_CANVAS (smeter.canvas),
					0.0, 0.0,
					160.0, 80.0);
	gnome_canvas_set_pixels_per_unit (GNOME_CANVAS (smeter.canvas), 1.0);
	
	/* create background pixmap and add it to canvas */
	fname = g_strconcat (PACKAGE_DATA_DIR, "/pixmaps/grig/smeter.png", NULL);
	pixbuf = gdk_pixbuf_new_from_file (fname, NULL);
	g_free (fname);

	smeter.pixmap = gnome_canvas_item_new (gnome_canvas_root (GNOME_CANVAS (smeter.canvas)),
					       gnome_canvas_pixbuf_get_type (),
					       "pixbuf", pixbuf,
					       NULL);
	/* decrease refcount */
	gdk_pixbuf_unref (pixbuf);

	/* create needle; first we ned to create the points... go figure... */

	/* get initial cordinates */
	convert_angle_to_rect (smeter.value, &coor);

	/* store coordinates as GnomeCanvasPoints */
	points = gnome_canvas_points_new (2);
	points->coords[0] = coor.x1;
	points->coords[1] = coor.y1;
	points->coords[2] = coor.x2;
	points->coords[3] = coor.y2;

	smeter.needle = gnome_canvas_item_new (gnome_canvas_root (GNOME_CANVAS (smeter.canvas)),
					       gnome_canvas_line_get_type (),
					       "fill-color-rgba", 0x3b3428A0,
					       "smooth", TRUE,
					       "width-pixels", 2,
					       "points", points,
					       "cap-style", GDK_CAP_ROUND,
					       NULL);

	gnome_canvas_points_unref (points);
					       
}


/** \brief Execute timeout function.
 *  \param data User data; currently NULL.
 *  \return Always TRUE to keep the timer running.
 *
 * This function reads the relevant rig settings from the rid-data object and
 * updates the control widgets within vbox. The function is called peridically
 * by the Gtk+ scheduler.
 *
 * \note Optimize?
 */
static gint 
rig_gui_smeter_timeout_exec  (gpointer data)
{
	gfloat             rdang;  /* angle obtainedfrom rig-data */
	gint               db;
	gfloat             maxdelta;
	gfloat             delta;
	coordinate_t       coor;
	GnomeCanvasPoints *points;


	/* are we in RX or TX mode? */
	if (rig_data_get_ptt () == RIG_PTT_OFF) {

		/* get current value from rig-data */
		db = rig_data_get_strength ();
		rdang = convert_db_to_angle (db, DB_TO_ANGLE_MODE_POLY);

		delta = fabs (rdang - smeter.value);
	}
	else {
		/* TX mode; use -54dB */
		db = -54;
		rdang = convert_db_to_angle (db, DB_TO_ANGLE_MODE_POLY);

		delta = fabs (rdang - smeter.value);
	}

	/* is there a significant change? */
	if (delta > 0.01) {

		/* calculate max delta = deg/sec * sec  */
		maxdelta = smeter.falloff * (smeter.tval * 0.001);
		
		smeter.lastvalue = smeter.value;
			
		/* check whether the delta is less than what the falloff allows */
		if (delta < maxdelta) {
			smeter.value = rdang;
		}

		/* otherwise use maxdelta */
		else {
			if (rdang > smeter.value) {
				smeter.value += maxdelta;
			}
			else {
				smeter.value -= maxdelta;
			}
		}

		/* update widget */
		convert_angle_to_rect (smeter.value, &coor);

		/* get GnomeCanvasPoints */
		g_object_get (smeter.needle, "points", &points, NULL);

		points->coords[0] = coor.x1;
		points->coords[1] = coor.y1;
		points->coords[2] = coor.x2;
		points->coords[3] = coor.y2;
		
		gnome_canvas_item_set (smeter.needle, "points", points, NULL);
 
	}

	return TRUE;
}



/** \brief Stop timeout function.
 *  \param timer The ID of the timer to stop.
 *  \return Always TRUE.
 *
 * This function is used to stop the readback timer just before the
 * program is quit. It should be called automatically by Gtk+ when
 * the gtk_main_loop is exited.
 */
static gint 
rig_gui_smeter_timeout_stop  (gpointer timer)
{

	gtk_timeout_remove (GPOINTER_TO_UINT (timer));

	return TRUE;
}


/** \brief Create TX display mode selector widget.
 *  \return The mode selctor widget.
 *
 * This function creates the combo box which is used to select the unction of the s-meter
 * in TX mode (Power, SWR, ALC, ...).
 */
static GtkWidget *
rig_gui_mode_selector_create  ()
{
	GtkWidget *combo;
	guint i;

	combo = gtk_combo_box_new_text ();

	/* Add entries to combo box */
	for (i = SMETER_TX_MODE_NONE; i < SMETER_TX_MODE_LAST; i++) {
		gtk_combo_box_append_text (GTK_COMBO_BOX (combo), TX_MODE_S[i]);
	}

	/* temporary disable */
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo), SMETER_TX_MODE_NONE);

	/* connect changed signal */
	g_signal_connect (G_OBJECT (combo), "changed",
			  G_CALLBACK (rig_gui_smeter_mode_cb),
			  NULL);

	return combo;
}


/** \brief Create scale selector widget.
 *  \return The scale selector widget.
 *
 * This function is used to create the combo box whih can be used to select the
 * scale/range of the s-meter in TX mode.
 */
static GtkWidget *
rig_gui_scale_selector_create ()
{
	GtkWidget *combo;
	guint i;

	combo = gtk_combo_box_new_text ();

	/* Add entries to combo box */
	for (i = SMETER_SCALE_5; i < SMETER_SCALE_LAST; i++) {
		gtk_combo_box_append_text (GTK_COMBO_BOX (combo), TX_SCALE_S[i]);
	}

	/* temporary disable */
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo), SMETER_SCALE_100);

	/* connect changed signal */
	g_signal_connect (G_OBJECT (combo), "changed",
			  G_CALLBACK (rig_gui_smeter_scale_cb),
			  NULL);

	return combo;
}


/** \brief Select s-meter mode.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new mode for the s-meter.
 * It acquires the selected menu item, and stores the corresponding display
 * mode in the s-meter structure.
 */
static void
rig_gui_smeter_mode_cb   (GtkWidget *widget, gpointer data)
{
	gint index;

	/* get selected item */
	index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

	/* store the mode if value is self-consistent */
	if ((index > -1) && (index < SMETER_TX_MODE_LAST)) {
		smeter.txmode = index;
	}

}


/** \brief Select s-meter range.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new range for the s-meter.
 * It acquires the selected menu item, and stores the corresponding display
 * mode in the s-meter structure.
 */
static void
rig_gui_smeter_scale_cb   (GtkWidget *widget, gpointer data)
{
	gint index;

	/* get selected item */
	index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

	/* store the mode if value is self-consistent */
	if ((index > -1) && (index < SMETER_SCALE_LAST)) {
		smeter.scale = index;
	}

}
