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
/** \file rig-gui-info.c
 *  \ingroup info
 *  \brief Radio info box.
 *
 * The purpose of this package is to show the contents of the rig_caps
 * structure in a nicely organized fashion.
 */
#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include "support.h"
#include "rig-gui-info.h"
#include "rig-gui-info-data.h"

extern GtkWidget   *grigapp;    /* defined in main.c */
extern RIG         *myrig;      /* define in rig-demon.c */


/* subsystem containers */
static GtkWidget *rig_gui_info_create_header         (void);
static GtkWidget *rig_gui_info_create_offset_frame   (void);
static GtkWidget *rig_gui_info_create_level_frame    (void);


/** \brief Create info dialog.
 *
 * This function creates the dialog window which is used for showing
 * the radio capabilities.
 */
void
rig_gui_info_run ()
{
	GtkWidget *dialog;

	dialog = gtk_dialog_new_with_buttons (_("Radio Info"), GTK_WINDOW (grigapp),
					      GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_CLOSE, GTK_RESPONSE_NONE, NULL);
   
	/* Ensure that the dialog box is destroyed when the user responds. */
	g_signal_connect_swapped (dialog,
				  "response", 
				  G_CALLBACK (gtk_widget_destroy),
				  dialog);

	/* Add the label, and show everything we've added to the dialog. */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    rig_gui_info_create_header (),
			    FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    gtk_hseparator_new (),
			    FALSE, FALSE, 10);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    rig_gui_info_create_offset_frame (),
			    FALSE, FALSE, 10);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    rig_gui_info_create_level_frame (),
			    FALSE, FALSE, 10);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    gtk_label_new ("rig caps to come ..."),
			    FALSE, FALSE, 10);

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
	text = g_strdup_printf (_("\302\261%.2f kHz"),
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

	text = g_strdup_printf (_("\302\261%.2f kHz"),
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

	text = g_strdup_printf (_("\302\261%.2f kHz"),
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
 * This function creates the widgets to display various
 * level settings information. The container is made of a 2x2 table
 * showing SET and GET levels and it is surrounded by a nice
 * frame.
 *
 *  REWRITE: vertical scrollbar!
 *
 *             READ    WRITE
 *
 *  LEVEL 1     X        X
 *  LEVEL 2     X
 */
static GtkWidget *
rig_gui_info_create_level_frame    ()
{
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *label;
	gchar     *text;
	gchar     *buff;
	setting_t  levels;
	guint      i;
	guint      count = 0;

	table = gtk_table_new (2, 2, FALSE);

	label = gtk_label_new (_("READ:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* get read levels */
	text = g_strdup (" ");
	levels = rig_has_get_level (myrig, 0xFFFFFF);
	
	/* loop over all values */
	for (i = 0; i < 31; i++) {

		/* if we have level append it to string */
		if (levels & (1 << i)) {

			buff = g_strdup_printf ("%s %s", text, RIG_LEVEL_STR[i]);
			count++;

			g_free (text);
			text = g_strdup (buff);
			g_free (buff);
		}

		if (count >= 5) {
			buff = g_strdup_printf ("%s\n", text);
			count = 0;

			g_free (text);
			text = g_strdup (buff);
			g_free (buff);
		}


	}

	label = gtk_label_new (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);
	g_free (text);

	label = gtk_label_new (_("WRITE:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label,
			  0, 1, 1, 2,
			  GTK_EXPAND | GTK_FILL,
			  GTK_EXPAND | GTK_FILL,
			  5, 0);

	/* get write levels */
	levels = rig_has_set_level (myrig, 0xFFFFFF);


	frame = gtk_frame_new (_("Levels"));
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_container_add (GTK_CONTAINER (frame), table);

	return frame;
}
