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
/** \file grig-about.c
 *  \ingroup about
 *  \brief About dialog window.
 *
 * This file contains the function to create and manage the about dialog box.
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "grig-about.h"


extern GtkWidget   *grigapp;    /* defined in main.c */


static GtkWidget *grig_about_about_create   (void);
static GtkWidget *grig_about_credits_create (void);
static GtkWidget *grig_about_sysinfo_create (void);
static GtkWidget *grig_about_logo_create    (void);



/** \brief Show about dialog.
 *
 * This function creates and executes the about dialog.
 */
void
grig_about_run ()
{
	GtkWidget *dialog;
	GtkWidget *notebook;

	dialog = gtk_dialog_new_with_buttons (_("About GRIG"), GTK_WINDOW (grigapp),
					      GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_CLOSE, GTK_RESPONSE_NONE, NULL);

	/* Ensure that the dialog box is destroyed when the user responds. */
	g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);

	/* create notebook and add pages */
	notebook = gtk_notebook_new ();
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), grig_about_about_create (), 
				  gtk_label_new (_("About")));
/* 	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), grig_about_credits_create (),  */
/* 				  gtk_label_new (_("Credits"))); */
/* 	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), grig_about_sysinfo_create (),  */
/* 				  gtk_label_new (_("Sysinfo"))); */


	/* Add the logo and notebook; show everything we've added to the dialog. */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
			    grig_about_logo_create (), FALSE, FALSE, 10);
	gtk_box_pack_end   (GTK_BOX (GTK_DIALOG (dialog)->vbox), notebook, FALSE, FALSE, 5);
	gtk_widget_show_all (dialog);
}



/** \brief Create 'about' page.
 *  \return A GtkWidget containing the page.
 *
 * This function creates the first notebook page containing some
 * very basic information about GRIG.
 */
static GtkWidget *
grig_about_about_create ()
{
	GtkWidget *vbox;
	GtkWidget *label;
	gchar     *text;


	vbox = gtk_vbox_new (FALSE, 0);

	/* program label and version */
	label = gtk_label_new (NULL);
	text = g_strdup_printf ("<b><big><big>%s %s</big></big></b>", _("Grig"), VERSION);
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 10);

	/* copyright info */
	label = gtk_label_new (_("Copyright 2001-2004 Alexandru Csete, OZ9AEC"));
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 5);

	/* info */
	label = gtk_label_new (_("Grig is a graphical user interface for the Hamradio "\
				 "Control Libraries.\n"\
				 "Grig is free software licensed under the terms and "\
				 "conditions of the\n"\
				 "GNU General Public License version 2 or later.\n\n"\
				 "Comments, questions and bug reports can be made on "
				 "the project\npage at:"));
	gtk_misc_set_padding (GTK_MISC (label), 5, 0);
	gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);


	label = gtk_label_new (_("http://sourceforge.net/projects/groundstation"));
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);



	return vbox;
}


/** \brief Create 'credits' page.
 *  \return A GtkWidget containing the page.
 *
 * This function creates the second notebook page containing the
 * credits information.
 */
static GtkWidget *
grig_about_credits_create ()
{
	GtkWidget *vbox;
	GtkWidget *label;

	vbox = gtk_vbox_new (FALSE, 0);

	return vbox;
}

/** \brief Create 'sysinfo' page.
 *  \return A GtkWidget containing the page.
 *
 * This function creates the third notebook page containing the
 * sysinfo information.
 */
static GtkWidget *
grig_about_sysinfo_create ()
{
	GtkWidget *vbox;
	GtkWidget *table;


	table = gtk_table_new (10, 3, FALSE);


	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), table);
				    
	return vbox;
}



/** \brief Create grig log page.
 *  \return A GtkWidget containing the logo.
 *
 * This function creates the grig logo and returns it as a GtkWidget.
 *
 * \bug PIXMAP_DIR should be defined as constant somewhere!
 */
static GtkWidget *
grig_about_logo_create ()
{
	GtkWidget *logo;
	gchar     *name;

	name = g_strconcat (PACKAGE_PIXMAPS_DIR, G_DIR_SEPARATOR_S,
			    "grig-logo.png", NULL);

	logo = gtk_image_new_from_file (name);

	g_free (name);

	return logo;
}
