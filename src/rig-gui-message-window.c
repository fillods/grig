/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2005  Alexandru Csete.

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
#include "rig-gui-message-window.h"


static gboolean visible     = FALSE;   /* Is message window visible? */
static gboolean initialised = FALSE;   /* Is module initialised? */
static guint32  bugs        = 0;       /* Number of bug messages */    
static guint32  errors      = 0;       /* Number of error messages */
static guint32  warnings    = 0;       /* Number of warning messages */
static guint32  verboses    = 0;       /* Number of verbose messages */
static guint32  traces      = 0;       /* Number of trace messages */


/* Initialise message window.
 *
 * This function creates the message window and allocates all the internal
 * data structures. The function should be called when the main program
 * is initialised.
 */
void
rig_gui_message_window_init  ()
{
	if (!initialised) {

		/* do some init stuff */

		initialised = TRUE;
	}
}


/* Clean up message window.
 *
 * This function cleans up the message window by freeing the allocated
 * memory. It should be called when the main program exits.
 *
 * Note: It is not strictly neccessary to call this function, since it
 *       is also invoked by the 'destroy' callback of the window.
 *
 * FIXME: In the above case this function is not necessary at all.
 */
void
rig_gui_message_window_clean ()
{
	if (initialised) {

		if (visible) {
			rig_gui_message_window_hide ();
		}

		/* do some cleaning stuff */

		initialised = FALSE;
	}
}



void
rig_gui_message_window_show ()
{
	if (!initialised)
		rig_gui_message_window_init ();

	if (!visible) {
		g_print ("Show window\n");

		visible = TRUE;
	}

}


void
rig_gui_message_window_hide  ()
{
	if (visible) {
		g_print ("Hide window\n");
		visible = FALSE;
	}
}



/** \brief Debug handler callback
 *  \param debug_level The debug level.
 *  \param user_data Unused.
 *  \param fmt Format string (see printf).
 *  \returns Always RIG_OK.
 *
 *  This function should be passed to hamlib as the debug handler using
 *  the rig_set_debug_callback API function.
 */
int
rig_gui_message_window_add_cb   (enum rig_debug_level_e debug_level,
				 rig_ptr_t user_data,
				 const char *fmt,
				 ...)
{

	return 0;
}
