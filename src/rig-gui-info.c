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
#include "rig-gui-info.h"
#include "support.h"


extern GtkWidget   *grigapp;    /* defined in main.c */
extern RIG         *myrig;      /* define in rig-demon.c */


static GtkWidget *rig_gui_info_create_header (void);


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


