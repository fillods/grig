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
#include "rig-gui.h"
#include "rig-gui-buttons.h"



/** \brief Create rig control widgets.
 *  \return A mega-widget containing the rig controls.
 *
 * This function creates the rig control mega-widget by calling the create
 * function of each sub-object and packing the into a main container.
 *
 * \note The main container is a horizontal box.
 */
GtkWidget *
rig_gui_create ()
{
	GtkWidget *hbox;     /* the main container */


	/* create the main container */
	hbox = gtk_hbox_new (FALSE, 5);

	gtk_box_pack_start_defaults (GTK_BOX (hbox), rig_gui_buttons_create ());

	return hbox;
}