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
/** \file rig-gui-info.c
 *  \ingroup info
 *  \brief Radio info box.
 *
 * The purpose of this package is to show the contents of the rig_caps
 * structure in a nicely organized fashion.
 */
#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include <glib/gi18n.h>
#include "rig-data.h"
#include "rig-gui-info.h"
#include "rig-gui-info-data.h"

extern GtkWidget   *grigapp;    /* defined in main.c */
extern RIG         *myrig;      /* define in rig-demon.c */


/* subsystem containers */
static GtkWidget *rig_gui_info_create_header         (void);
static GtkWidget *rig_gui_info_create_offset_frame   (void);
static GtkWidget *rig_gui_info_create_level_frame    (void);
static GtkWidget *rig_gui_info_create_if_frame       (void);
static GtkWidget *rig_gui_info_create_tunstep_frame  (void);
static GtkWidget *rig_gui_info_create_frontend_frame (void);
static GtkWidget *rig_gui_info_create_func_frame     (void);
static GtkWidget *rig_gui_info_create_vfo_ops_frame  (void);


/** \brief Create info dialog.
 *
 * This function creates the dialog window which is used for showing
 * the radio capabilities.
 */
void
rig_gui_info_run ()
{
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *vbox1;
	GtkWidget *vbox2;
	GtkWidget *vbox3;
	GtkWidget *vbox4;

	vbox1 = gtk_vbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox1), 
			    rig_gui_info_create_if_frame (),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), 
			    rig_gui_info_create_offset_frame (),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), 
			    rig_gui_info_create_frontend_frame (),
			    FALSE, FALSE, 0);

	vbox2 = gtk_vbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox2),
			    rig_gui_info_create_level_frame (),
			    TRUE, TRUE, 0);

	vbox3 = gtk_vbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox3), 
			    rig_gui_info_create_func_frame (),
			    TRUE, TRUE, 0);

	vbox4 = gtk_vbox_new (FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox4), 
			    rig_gui_info_create_tunstep_frame (),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox4), 
			    rig_gui_info_create_vfo_ops_frame (),
			    TRUE, TRUE, 0);

	/* main horisontal box */
	hbox = gtk_hbox_new (TRUE, 15);

	gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox3, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox4, TRUE, TRUE, 0);


	/* create dialog and add hbox */
	dialog = gtk_dialog_new_with_buttons (_("Radio Info"), GTK_WINDOW (grigapp),
					      GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_CLOSE, GTK_RESPONSE_NONE, NULL);
  
	/* Ensure that the dialog box is destroyed when the user responds. */
	g_signal_connect_swapped (dialog,
				  "response", 
				  G_CALLBACK (gtk_widget_destroy),
				  dialog);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    rig_gui_info_create_header (),
			    FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    gtk_hseparator_new (),
			    FALSE, FALSE, 10);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    hbox, TRUE, TRUE, 10);

	gtk_widget_show_all (dialog);
	
}


/** \brief Create header.
 *  \return The header widget which can be packed into themain container.
 *
 * This funcion creates the header of the radio info dialog. The header
 * consists of the brand, model and driver info. The text is arranged in a
 * table with 3 rows and 2 columns. The brand and model is placed in the
 * upper row, the driver version is in the middle row and the driver status
 * is in the lower row.
 */
static GtkWidget *
rig_gui_info_create_header ()
{
	GtkWidget *table;
	GtkWidget *label;
	gchar     *text;

	/* create the main table */
	table = gtk_table_new (4, 2, FALSE);

	/* create label showing brand and model */
	label = gtk_label_new (NULL);
	text = g_strdup_printf ("<big><big><b>%s %s</b></big></big>",
				myrig->caps->mfg_name, myrig->caps->model_name);
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);

	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);

	/* driver version */
	label = gtk_label_new (NULL);
	text = g_strdup ("<b>Driver Version:</b>");
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);

	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
			  GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

	label = gtk_label_new (NULL);
	text = g_strdup_printf ("<b> %s</b>", myrig->caps->version);
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 1, 2,
			  GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

	/* driver status */
	label = gtk_label_new (NULL);
	text = g_strdup ("<b>Driver Status:</b>");
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);

	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3,
			  GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

	label = gtk_label_new (NULL);
	text = g_strdup_printf ("<b> %s</b>", rig_strstatus (myrig->caps->status));
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 2, 3,
			  GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

	/* copyright label */
	label = gtk_label_new (NULL);
	text = g_strdup ("<b>Driver License:</b>");
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);

	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4,
			  GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

	label = gtk_label_new (NULL);
	text = g_strdup_printf ("<b> %s</b>", myrig->caps->copyright);
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 3, 4,
			  GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	

	return table;
}



/** \brief Create frame containing offset info.
 *  \return Frame containing the widgets.
 *
 * This function creates the widgets to display various
 * offset information. The container is made of a 3x2 table
 * showing RIT, XIT and IF-Shift and is surrounded by a nice
 * frame.
 */
static GtkWidget *
rig_gui_info_create_offset_frame ()
{
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *label;
	gchar     *text;

	table = gtk_table_new (3, 2, TRUE);

	label = gtk_label_new (_("RIT:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* use UTF-8 code for plus/minus */
	text = g_strdup_printf ("\302\261%.2f kHz",
				((gfloat) myrig->caps->max_rit) / 1000.0);
	label = gtk_label_new (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);
	g_free (text);

	label = gtk_label_new (_("XIT:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 1, 2,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	text = g_strdup_printf ("\302\261%.2f kHz",
				((gfloat) myrig->caps->max_xit) / 1000.0);
	label = gtk_label_new (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 1, 2,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);
	g_free (text);

	label = gtk_label_new (_("IF-SHIFT:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 2, 3,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	text = g_strdup_printf ("\302\261%.2f kHz",
				((gfloat) myrig->caps->max_ifshift) / 1000.0);
	label = gtk_label_new (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 2, 3,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);
	g_free (text);

	frame = gtk_frame_new (_("Max. Offsets"));
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_container_add (GTK_CONTAINER (frame), table);

	return frame;
}



/** \brief Create frame containing levels info.
 *  \return Frame containing the widgets.
 *
 * This function creates the widget used to display the set and get
 * level availabilities. The various levels are listed in a vertical
 * table and for each of them a label indicates
 * whether the level is available or not (actualy one label for read and
 * one for write).
 *
 *             READ    WRITE
 *
 *  LEVEL 1     X        X
 *  LEVEL 2     X
 *
 */
static GtkWidget *
rig_gui_info_create_level_frame    ()
{
	GtkWidget *swin;
	GtkWidget *table;
	GtkWidget *label;
	setting_t  levels_rd;
	setting_t  levels_wr;
	guint      i;


	table = gtk_table_new (30, 3, FALSE);

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>LEVEL</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>READ</b>"));
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>WRITE</b>"));
	gtk_table_attach (GTK_TABLE (table), label,
			  2, 3, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	/* get levels */
	levels_rd = rig_has_get_level (myrig, 0xFFFFFFFF);
	levels_wr = rig_has_set_level (myrig, 0xFFFFFFFF);
	
	/* loop over all levels; unfortunately there is no nice way to avoid
	   the empty values but, since there are not so many of them, it is all
	   right...
	*/
	for (i = 0; i < 31; i++) {

		/* add RIG_LEVEL_STR[i] to the row i+1 */
		label = gtk_label_new (RIG_LEVEL_STR[i]);
		gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
		gtk_table_attach (GTK_TABLE (table), label,
				  0, 1, i+1, i+2,
				  GTK_EXPAND | GTK_FILL,
				  GTK_EXPAND | GTK_FILL,
				  5, 0);
		
		/* add READ label to row i+1 */
		label = gtk_label_new (_("-"));
		gtk_table_attach_defaults (GTK_TABLE (table), label,
					   1, 2, i+1, i+2);

		if (levels_rd & (1 << i)) {
			gtk_label_set_text (GTK_LABEL (label), _("X"));
		}

		/* add WRITE label to row i+1 */
		label = gtk_label_new (_("-"));
		gtk_table_attach_defaults (GTK_TABLE (table), label,
					   2, 3, i+1, i+2);

		if (levels_wr & (1 << i)) {
			gtk_label_set_text (GTK_LABEL (label), _("X"));
		}

	}

	/* scrolled window and frame */ 
	swin = gtk_scrolled_window_new (NULL,NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
					GTK_POLICY_NEVER,
					GTK_POLICY_ALWAYS);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), table);
		

	return swin;
}



/** \brief Create interface frame.
 *  \return Container widget containing the individual settings.
 *
 * This function creates the info widgets to show the rig port type, DCD type,
 * PTT type, parity and handshake.
 *
 */
static GtkWidget *
rig_gui_info_create_if_frame      ()
{
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *label;
	gchar     *text;

	table = gtk_table_new (7, 2, FALSE);

	/* connection type */
	label = gtk_label_new (_("Port Type:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	/* protection against index out of range */
	if ((myrig->caps->port_type >= RIG_PORT_NONE) &&
	    (myrig->caps->port_type <= RIG_PORT_PARALLEL)) {
		
		gtk_label_set_text (GTK_LABEL (label),
				    RIG_PORT_STR[myrig->caps->port_type]);
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("Unknown"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* DCD */
	label = gtk_label_new (_("DCD Type:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 1, 2,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	/* protection against index out of range */
	if ((myrig->caps->dcd_type >= RIG_DCD_NONE) &&
	    (myrig->caps->dcd_type <= RIG_DCD_PARALLEL)) {
		
		gtk_label_set_text (GTK_LABEL (label),
				    DCD_TYPE_STR[myrig->caps->dcd_type]);
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("Unknown"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 1, 2,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* PTT */
	label = gtk_label_new (_("PTT Type:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 2, 3,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	/* protection against index out of range */
	if ((myrig->caps->ptt_type >= RIG_PTT_NONE) &&
	    (myrig->caps->ptt_type <= RIG_PTT_PARALLEL)) {
		
		gtk_label_set_text (GTK_LABEL (label),
				    PTT_TYPE_STR[myrig->caps->dcd_type]);
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("Unknown"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 2, 3,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* serial speed */
	label = gtk_label_new (_("Serial Speed:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 3, 4,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	if (myrig->caps->port_type == RIG_PORT_SERIAL) {
		text = g_strdup_printf (_("%d..%d baud"),
					myrig->caps->serial_rate_min,
					myrig->caps->serial_rate_max);
		gtk_label_set_text (GTK_LABEL (label), text);
		g_free (text);
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("N/A"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 3, 4,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* data bits */
	label = gtk_label_new (_("Data bits:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 4, 5,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	if (myrig->caps->port_type == RIG_PORT_SERIAL) {
		text = g_strdup_printf ("%d", myrig->caps->serial_data_bits);
		gtk_label_set_text (GTK_LABEL (label), text);
		g_free (text);
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("N/A"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 4, 5,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* stop bits */
	label = gtk_label_new (_("Stop bits:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 5, 6,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	if (myrig->caps->port_type == RIG_PORT_SERIAL) {
		text = g_strdup_printf ("%d", myrig->caps->serial_stop_bits);
		gtk_label_set_text (GTK_LABEL (label), text);
		g_free (text);
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("N/A"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 5, 6,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* serial parity */
	label = gtk_label_new (_("Parity:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 6, 7,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	if (myrig->caps->port_type == RIG_PORT_SERIAL) {

		/* protection against index out of range */
		if ((myrig->caps->serial_parity >= RIG_PARITY_NONE) &&
		    (myrig->caps->serial_parity <= RIG_PARITY_EVEN)) {
		
			gtk_label_set_text (GTK_LABEL (label),
					    RIG_PARITY_STR[myrig->caps->serial_parity]);
		}
		else {
			gtk_label_set_text (GTK_LABEL (label), _("Unknown"));
		}
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("N/A"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 6, 7,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* serial handshake */
	label = gtk_label_new (_("Handshake:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 7, 8,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);

	if (myrig->caps->port_type == RIG_PORT_SERIAL) {

		/* protection against index out of range */
		if ((myrig->caps->serial_handshake >= RIG_HANDSHAKE_NONE) &&
		    (myrig->caps->serial_handshake <= RIG_HANDSHAKE_HARDWARE)) {
		
			gtk_label_set_text (GTK_LABEL (label),
					    RIG_HANDSHAKE_STR[myrig->caps->serial_handshake]);
		}
		else {
			gtk_label_set_text (GTK_LABEL (label), _("Unknown"));
		}
	}
	else {
		gtk_label_set_text (GTK_LABEL (label), _("N/A"));
	}

	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 7, 8,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	/* frame */
	frame = gtk_frame_new (_("Interface"));
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_container_add (GTK_CONTAINER (frame), table);



	return frame;
}



/** \brief Create tuning steps list.
 *  \return A container widget listing the tuning steps.
 *
 * This function creates the container widget which is used to list the
 * available tuning steps forthe rig. For each available tuning step it
 * lists the modes for which the tuning step can beused.
 *
 * The container structure is very similar to the one found in the level
 * container (table packed in a scrolled window).
 */
static GtkWidget *
rig_gui_info_create_tunstep_frame  ()
{
	GtkWidget *swin;
	GtkWidget *table;
	GtkWidget *label;
	guint      i,j;
	gchar     *text;
	gchar     *buffer;

	/* Create a table with enough rows to show the
	   max possible number of unique tuning steps.
	*/
	table = gtk_table_new (TSLSTSIZ, 2, FALSE);

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>STEP</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_SHRINK,
			  GTK_SHRINK,
			  5, 0);


	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>MODES</b>"));
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_SHRINK,
			  GTK_SHRINK,
			  5, 0);

	/* pseudo code:

	      for each available tuning step i  {
	          add tuning step to row i
		  for each mode j {
		      if mode is in bitfield for tuning step i {
		          append mode j to cell (i,1)
		      }
		  }
	      }
	*/
	/* for each available tuning ste */
	for (i = 0; i < TSLSTSIZ; i++) {

		gboolean firsthit = TRUE;   /* indicates whether found mode is the first one
					       for the current tuning step. */

		/* if tuning step is zero stop
		   (note that the RIG_IS_TS_END macro seem to be
		   buggy, at least when used on the dummy backend
		*/
		if (myrig->caps->tuning_steps[i].ts == 0) {

			i = TSLSTSIZ;
		}
		
		/* otherwise continue */
		else {

			/* create tuning step label */
			text = g_strdup_printf ("%ld Hz", myrig->caps->tuning_steps[i].ts);
			label = gtk_label_new (text);
			gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
			gtk_table_attach (GTK_TABLE (table), label,
					  0, 1, i+1, i+2,
					  GTK_EXPAND | GTK_FILL,
					  GTK_EXPAND | GTK_FILL,
					  5, 0);
			g_free (text);

			/* for each mode */
			for (j = 0; j < 16; j++) {

				/* if the mode is in the bitfield for this tuning step */
				if (myrig->caps->tuning_steps[i].modes & (1 << j)) {

					/* append mode string to text buffer;
					   note that the first mode requires
					   special attention, since text is empty.
					*/
					if (!firsthit) {
						buffer = g_strdup_printf ("%s %s", text, MODE_STR[j]);
						g_free (text);
					}
					else {
						buffer = g_strdup_printf ("%s", MODE_STR[j]);
						firsthit = FALSE;
					}
					text = g_strdup (buffer);
					g_free (buffer);
				}

			}

			/* create label containing the modes */
			label = gtk_label_new (text);
			gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
			gtk_table_attach (GTK_TABLE (table), label,
					  1, 2, i+1, i+2,
					  GTK_EXPAND | GTK_FILL,
					  GTK_EXPAND | GTK_FILL,
					  5, 0);
		}


	}

	/* scrolled window and frame */ 
	swin = gtk_scrolled_window_new (NULL,NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_ALWAYS);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), table);

	return swin;
}



/** \brief Create front end info frame.
 *  \return A container widget containing the info.
 *
 * This function creates the widgets used to display the front end
 * information (pre-amp, attenuator).
 */
static GtkWidget *
rig_gui_info_create_frontend_frame ()
{
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *label;
	gchar     *text;
	gchar     *buffer;
	guint      i;
	gint       data;

	table = gtk_table_new (2, 2, FALSE);

	label = gtk_label_new (_("PREAMP:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);
	
	text = g_strdup ("");

	/* loop over all available preamp values and concatenate them into a label */
	for (i = 0; i < MAXDBLSTSIZ; i++) {

		data = rig_data_get_preamp_data (i);

		/* check whether we have a real data or we have
		   reached the terminator
		*/
		if (data == 0) {
			i = MAXDBLSTSIZ;
		}
		else {
			if (i > 0) {

				buffer = g_strdup_printf ("%s %ddB", text, data);
				g_free (text);

				text = g_strdup (buffer);
				g_free (buffer);
			}
			else {
				g_free (text);
				text = g_strdup_printf ("%ddB", data);
			}

		}

	}
	label = gtk_label_new (text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	label = gtk_label_new (_("ATT:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 1, 2,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	text = g_strdup ("");
	/* loop over all available attenuator values and concatenate them into a label */
	for (i = 0; i < MAXDBLSTSIZ; i++) {

		data = rig_data_get_att_data (i);

		/* check whether we have a real data or we have
		   reached the terminator
		*/
		if (data == 0) {
			i = MAXDBLSTSIZ;
		}
		else {
			if (i > 0) {

				buffer = g_strdup_printf ("%s; -%ddB", text, data);
				g_free (text);

				text = g_strdup (buffer);
				g_free (buffer);
			}
			else {
				text = g_strdup_printf ("-%ddB", data);
			}

		}

	}
	label = gtk_label_new (text);
	g_free (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 1, 2,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	frame = gtk_frame_new (_("Front End"));
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_container_add (GTK_CONTAINER (frame), table);

	return frame;
}



/** \brief Create frame containing functions info.
 *  \return Frame containing the widgets.
 *
 * This function creates the widget used to display the set and get
 * special function availabilities. The various functions are listed in a vertical
 * table and for each of them a label indicates
 * whether the function is available or not (actualy one label for read and
 * one for write).
 *
 *             READ    WRITE
 *
 *   FUNC 1     X        X
 *   FUNC 2     X
 *
 */
static GtkWidget *
rig_gui_info_create_func_frame    ()
{
	GtkWidget *swin;
	GtkWidget *table;
	GtkWidget *label;
	setting_t  funcs_rd;
	setting_t  funcs_wr;
	guint      i;


	table = gtk_table_new (30, 3, FALSE);

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>FUNCTION</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>READ</b>"));
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>WRITE</b>"));
	gtk_table_attach (GTK_TABLE (table), label,
			  2, 3, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	/* get levels */
	funcs_rd = rig_has_get_func (myrig, 0xFFFFFFFF);
	funcs_wr = rig_has_set_func (myrig, 0xFFFFFFFF);
	
	/* loop over all levels; unfortunately there is no nice way to avoid
	   the empty values but, since there are not so many of them, it is all
	   right...
	*/
	for (i = 0; i < 31; i++) {

		/* add RIG_FUNC_STR[i] to the row i+1 */
		label = gtk_label_new (RIG_FUNC_STR[i]);
		gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
		gtk_table_attach (GTK_TABLE (table), label,
				  0, 1, i+1, i+2,
				  GTK_EXPAND | GTK_FILL,
				  GTK_EXPAND | GTK_FILL,
				  5, 0);
		
		/* add READ label to row i+1 */
		label = gtk_label_new (_("-"));
		gtk_table_attach_defaults (GTK_TABLE (table), label,
					   1, 2, i+1, i+2);

		if (funcs_rd & (1 << i)) {
			gtk_label_set_text (GTK_LABEL (label), _("X"));
		}

		/* add WRITE label to row i+1 */
		label = gtk_label_new (_("-"));
		gtk_table_attach_defaults (GTK_TABLE (table), label,
					   2, 3, i+1, i+2);

		if (funcs_wr & (1 << i)) {
			gtk_label_set_text (GTK_LABEL (label), _("X"));
		}

	}

	/* scrolled window and frame */ 
	swin = gtk_scrolled_window_new (NULL,NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
					GTK_POLICY_NEVER,
					GTK_POLICY_ALWAYS);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), table);
		

	return swin;
}


/** \brief Create frame containing VFO ops info.
 *  \return Frame containing the widgets.
 *
 */
static GtkWidget *
rig_gui_info_create_vfo_ops_frame    ()
{
	GtkWidget *swin;
	GtkWidget *table;
	GtkWidget *label;
	setting_t  vfo_ops;
	guint      i;


	table = gtk_table_new (14, 2, FALSE);

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>VFO OP</b>"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);


	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>SET</b>"));
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);



	/* get vfo ops */
	vfo_ops = myrig->caps->vfo_ops & 0xFFFFFFFF;

	
	/* loop over all levels; unfortunately there is no nice way to avoid
	   the empty values but, since there are not so many of them, it is all
	   right...
	*/
	for (i = 0; i < 13; i++) {

		/* add RIG_FUNC_STR[i] to the row i+1 */
		label = gtk_label_new (RIG_OP_STR[i]);
		gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
		gtk_table_attach (GTK_TABLE (table), label,
				  0, 1, i+1, i+2,
				  GTK_EXPAND | GTK_FILL,
				  GTK_EXPAND | GTK_FILL,
				  5, 0);
		
		/* add READ label to row i+1 */
		label = gtk_label_new (_("-"));
		gtk_table_attach_defaults (GTK_TABLE (table), label,
					   1, 2, i+1, i+2);

		if (vfo_ops & (1 << i)) {
			gtk_label_set_text (GTK_LABEL (label), _("X"));
		}


	}

	/* scrolled window and frame */ 
	swin = gtk_scrolled_window_new (NULL,NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
					GTK_POLICY_NEVER,
					GTK_POLICY_ALWAYS);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), table);
		

	return swin;
}
