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
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>


GtkWidget *
rig_gui_smeter_create ()
{
	GtkWidget *meter;
	GtkWidget *vbox;
	GtkWidget *combo;
	gchar     *fname;
	GtkWidget *frame;

	fname = g_strconcat (PACKAGE_DATA_DIR, "/pixmaps/grig/smeter.png", NULL);
	meter = gtk_image_new_from_file (fname);
	g_free (fname);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
	gtk_container_add (GTK_CONTAINER (frame), meter);

	combo = gtk_combo_box_new_text ();

	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Signal / SWR"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Signal / PWR"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Signal / ALC"));

	vbox = gtk_vbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox), combo, FALSE, FALSE, 0);

	return vbox;
}
