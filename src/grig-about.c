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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include "grig-about.h"




/** \brief Show about dialog.
 *
 * This function creates and executes the about dialog.
 */
void
grig_about_run ()
{
	GtkWidget *dialog;

	const gchar *authors[] = {
		"Alexandru Csete, OZ9AEC, <csete@users.sourceforge.net>",
		NULL
	};

	const gchar *documenters[] = {
		"Jon Doe",
		NULL
	};

	/* This should be "translated" in the .po files */
	const gchar *translator = N_("Alexandru Csete, OZ9AEC");


	/* create the about dialog box */
	dialog = gnome_about_new (_("Gnome RIG"),
				  VERSION,
				  _("Copyright (C)  2001-2004  Alexandru Csete"),
				  _("Gnome RIG is a graphical user interface to the hamradio control libraries."),
				  authors,
				  documenters,
				  translator,
				  NULL);

	/* show dialog */
	gtk_widget_show (dialog);
}

