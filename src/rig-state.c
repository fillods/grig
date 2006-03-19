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
/** \file rig-state.c
 *  \brief Load and save rig state
 *
 * The rig state is a snapshot of the current settings in the rig-data
 * object. The rig state is saved to a file using the glib key/value
 * infrastructure.
 *
 * To begin with we just implement it in a simple straight forward way,
 * but later the could should be checnged to something more generic,
 * if possible.
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "rig-data.h"
#include "rig-state.h"


/** \brief Load rig state from file
 *  \param file The file to read the rig state from
 *
 * The file parameter may not be NULL. If you need to open
 * the file selector use the callback functions instead.
 */
gint
rig_state_load (const gchar *file)
{
}


/** \brief Save rig state to file
 *  \param file The file to save the rig state to
 *
 * The file parameter may not be NULL. If you need to open
 * the file selector use the callback functions instead.
 * existing file will be replaced without any warning!
 */
gint
rig_state_save (const gchar *file)
{
}


/** \brief Get connection info about radio
 *
 * This fiunction will return connection details found in the
 * specified rig state file. This can be used to re-establish
 * the link to a radio when the rig state file is loaded via
 * the command line. When the rig state is read via the menu
 * bar, grig is already running a connection and this can not
 * be changed. In those cases this function has no use.
 */
gint
rig_state_get_link_info (const gchar *file,
			 rig_model_t *model,
			 gchar *model,
			 gchar *mfg,
			 gchar *port,
			 int   *ser_rate)
{

}



/** \brief Load the rig state
 *
 * This function first asks the user for a file name via the
 * GtkFileChooser dialog, whereafter it calls rig_state_load
 * with the specified file name.
 * An error message is generated if the file does not exist or
 * the contents could not be read.
 */
void
rig_state_load_cb (GtkWidget *widget, gpointer data)
{
}


/** \brief Save the rig state
 *
 * This function first asks the user for a file name via the
 * GtkFileChooser dialog, whereafter it calls rig_state_save
 * with the specified file name.
 * If the file already exists it will ask the user whether to
 * replace the contents or not.
 */
void
rig_state_save_cb (GtkWidget *widget, gpointer data)
{
}
y
