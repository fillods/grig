/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2007  Alexandru Csete.

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
/** \brief Rig selection window used at startup.
 * 
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include "rig-selector.h"


/** \brief Execute radio selector.
 *  \return The config file name of the selected radio
 * 
 * This function creates a window containing a list with the currently
 * configured radios, allowing the user to select which radio to
 * connect to.
 *
 * Additionally, the window contains buttons to delete, edit, and add
 * new radios to the list.
 *
 * The dummy rig is always listed on the top of the list.
 *
 * The radio configurations are stored in $HOME/.grig/xyz.radio files
 */
gchar *
rig_selector_execute ()
{

    return NULL;
}
