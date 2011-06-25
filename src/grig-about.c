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
#include "compat.h"
#include "grig-about.h"



const gchar *authors[] = {
    "Alexandru Csete, OZ9AEC.",
    "",
    "Contributors:",
    "Alessandro Zummo (keypad)",
    "Stephane Fillod (extra function)",
    NULL
};


const gchar license[] = N_("Copyright (C) 2001-2009 Alexandru Csete OZ9AEC>\n\n"\
                    "Grig is free software; you can redistribute it and/or modify\n"\
                    "it under the terms of the GNU General Public License as published\n"\
                    "by the Free Software Foundation; either version 2 of the License,\n"\
                    "or (at your option) any later version.\n\n"\
                    "This program is distributed in the hope that it will be useful,\n"\
                    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"\
                    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"\
                    "GNU Library General Public License for more details.\n\n"\
                    "You should have received a copy of the GNU General Public License\n"\
                    "along with this program; if not, you can find a copy on the FSF\n"\
                    "website http://www.fsf.org/ or you can write to the\n\n"
                    "Free Software Foundation, Inc.\n"\
                    "51 Franklin Street - Fifth Floor\n"
                    "Boston\n"\
                    "MA 02110-1301\n"
                    "USA.\n");



/** \brief Show about dialog.
 *
 * This function creates and executes the about dialog.
 */
void
grig_about_run ()
{
    GtkWidget *dialog;
    GdkPixbuf *icon;
    gchar     *iconfile;


    dialog = gtk_about_dialog_new ();
    gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), _("Grig"));
    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), VERSION);
    gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog),
                    _("Copyright (C) 2001-2007 Alexandru Csete OZ9AEC"));
    gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog),
                        "http://groundstation.sourceforge.net/grig");
    gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (dialog),
                        _("Grig Website"));
    gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), license);
    iconfile = pixmap_file_name ("grig-logo.png");
    icon = gdk_pixbuf_new_from_file (iconfile, NULL);
    gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), icon);
    g_free (iconfile);
    g_object_unref (icon);

    gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
    gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog),
                        _("translator-credits"));

    gtk_dialog_run (GTK_DIALOG (dialog));

    gtk_widget_destroy (dialog);

}



