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
#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "rig-gui-levels.h"
#include "support.h"



/** \brief Create level controls.
 *  \return A container widget containing the controls.
 *
 * This function creates and initializes the level controls for grig.
 * The controls are hidden by default using a GtkExpander widget.
 */
GtkWidget *
rig_gui_levels_create ()
{
	GtkWidget *expander;
	GtkTooltips *tips;
	
	expander = gtk_expander_new (_("Level Controls"));

	/* add tooltips */
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, expander,
			      _("Show/hide level controls"),
			      _("Show/hide level controls"));

	return expander;
}
