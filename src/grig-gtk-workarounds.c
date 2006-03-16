/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2006  Alexandru Csete.

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
    along with this program; if not, visit http://www.fsf.org/
 
 
 
 
*/
/** \file grig-gtk-workarounds.c
 *  \ingroup misc
 *  \brief Utility functions to work around Gtk+bug and limitations.
 *
 * This file contains various utility functions to work around some bugs
 * and limitations in Gtk+. These are currently:
 *
 *     Tooltips for GtkComboBox
 *
 * \bug These functionsshall eventually be removed as they appear or get
 *      fixed in Gtk+
 */
#include <gtk/gtk.h>
#include "grig-gtk-workarounds.h"


static void set_combo_tooltip (GtkWidget *combo, gpointer text);




/** \brief Create and set tooltips for GtkComboBox.
 *  \param combo The GtkComboBox widget.
 *  \param text  Pointer to the desired tooltip text.
 *
 * This function creates and sets the tooltips for the specified widget.
 * The interfaceis implemented such that this function can be connected
 * directly to the \a realized signal of the GtkComboBox.
 *
 * Actually, this function only loops over all the children of the GtkComboBox
 * and calls the set_combo_tooltip internal function.
 *
 * \note This works only if the funcion is actually used as callback fo the
 *       \a realized signal og the GtkComboBox.
 *
 * \note This great trick has been pointed out by Matthias Clasen, he has done the
 *       the same for the filter combo in the new GtkFileChooser
 *       ref: gtkfilechooserdefault.c:3151 in Gtk+ 2.5.5
 */
void
grig_set_combo_tooltips (GtkWidget *combo, gpointer text)
{

	/* for each child in the container call the internal
	   function which actually creates the tooltips.
	*/
	gtk_container_forall (GTK_CONTAINER (combo),
			      set_combo_tooltip,
			      text);

}


/** \brief Create and set tooltips for GtkComboBox.
 *  \param text  Pointer to the desired tooltip text.
 *
 * This function creates and sets the tooltips for the specified widget.
 * This function is called by the \a grig_set_combo_tooltips function which
 * is must be used as callback for the "realized" signal of the GtkComboBox.
 */
static void
set_combo_tooltip (GtkWidget *combo, gpointer text)
{

	/* if current child is a button we have BINGO! */
	if (GTK_IS_BUTTON (combo)) {

		GtkTooltips *tips;

		tips = gtk_tooltips_new ();

		gtk_tooltips_set_tip (tips, combo,
				      (gchar *) text,
				      NULL);
	}

}
