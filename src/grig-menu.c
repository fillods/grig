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
#include <hamlib/rig.h>
#include "grig-config.h"



extern GtkWidget   *grigapp;    /* defined in main.c */
extern GConfClient *confclient;



/* private function prototypes */
static void  grig_menu_rot_open        (GtkWidget *, gpointer);
static void  grig_menu_app_exit        (GtkWidget *, gpointer);
static void  grig_menu_set_debug_level (GtkWidget *, gpointer);
static void  grig_menu_config          (GtkWidget *, gpointer);
static void  grig_menu_about           (GtkWidget *, gpointer);



/** \brief File menu declaration. */
GnomeUIInfo grig_file_menu[] = {
	GNOMEUIINFO_ITEM_DATA (N_("Rotator"), N_("Ope rotator control window"),
			       grig_menu_rot_open, NULL, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM (grig_menu_app_exit, NULL),
	GNOMEUIINFO_END
};



/** \brief rig_set_debug items in a radiolist */
GnomeUIInfo grig_settings_debug_list[] = {
	GNOMEUIINFO_RADIOITEM_DATA (N_("_None"), N_("No debug info"),
				    grig_menu_set_debug_level,
				    GUINT_TO_POINTER (RIG_DEBUG_NONE), NULL),
	GNOMEUIINFO_RADIOITEM_DATA (N_("_Bug"), N_("Serious bugs"),
				    grig_menu_set_debug_level,
				    GUINT_TO_POINTER (RIG_DEBUG_BUG), NULL),
	GNOMEUIINFO_RADIOITEM_DATA (N_("_Error"), N_("Error case"),
				    grig_menu_set_debug_level,
				    GUINT_TO_POINTER (RIG_DEBUG_ERR), NULL),
	GNOMEUIINFO_RADIOITEM_DATA (N_("_Warning"), N_("Also show warnings"),
				    grig_menu_set_debug_level,
				    GUINT_TO_POINTER (RIG_DEBUG_WARN), NULL),
	GNOMEUIINFO_RADIOITEM_DATA (N_("_Verbose"), N_("Verbose output"),
				    grig_menu_set_debug_level,
				    GUINT_TO_POINTER (RIG_DEBUG_VERBOSE), NULL),
	GNOMEUIINFO_RADIOITEM_DATA (N_("_Trace"), N_("Print everything"),
				    grig_menu_set_debug_level,
				    GUINT_TO_POINTER (RIG_DEBUG_TRACE), NULL),
	GNOMEUIINFO_END
};


/** \brief The submenu containing the debug level radiolist */
GnomeUIInfo grig_settings_debug_submenu[] = {
	GNOMEUIINFO_RADIOLIST (grig_settings_debug_list),
	GNOMEUIINFO_END
};


/** \brief Settings menu declaration. */
GnomeUIInfo grig_settings_menu[] = {
	GNOMEUIINFO_MENU_PREFERENCES_ITEM (grig_menu_config, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_SUBTREE (N_("_Debug Level"), grig_settings_debug_submenu),
	GNOMEUIINFO_END
};


/** \brief The help menu declaration. */
GnomeUIInfo grig_help_menu[] = {
	GNOMEUIINFO_HELP ("grig"),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_ABOUT_ITEM (grig_menu_about, NULL),
	GNOMEUIINFO_END
};


/** \brief The menubar declaration. */
GnomeUIInfo grig_menubar[] = {
	GNOMEUIINFO_MENU_FILE_TREE (grig_file_menu),
	GNOMEUIINFO_SUBTREE (N_("_Settings"), grig_settings_menu),
	GNOMEUIINFO_MENU_HELP_TREE (grig_help_menu),
	GNOMEUIINFO_END
};


/** \brief Open rotator control window.
 *  \param widget The widget which received the signal.
 *  \param data   User data (NULL).
 *
 * This function calls the function which initializes and starts the
 * rotator related services (daemon, GUI, etc.). It is designed as a
 * callback function to be directly callable from the menubar, but it
 * does not use the parameters to anything.
 */
static void
grig_menu_rot_open        (GtkWidget *widget, gpointer data)
{

}


/** \brief Exit application.
 *  \param widget The widget which received the signal.
 *  \param data   User data (NULL).
 *
 * This function exits the main grig application. It is designed as a
 * callback function to be directly callable from the menubar, but it
 * does not use the parameters to anything.
 */
static void
grig_menu_app_exit       (GtkWidget *widget, gpointer data)
{

	gtk_widget_destroy (grigapp);
}



/** \brief Set debug level.
 *  \param widget The menu item that received the signal.
 *  \param level  Pointer to an integer representing the debug level.
 *
 * This function is called when the user selects a new debug level.
 * The new debug level is stored in the GConf domain.
 *
 */
static void
grig_menu_set_debug_level (GtkWidget *widget, gpointer level)
{
	rig_set_debug (GPOINTER_TO_UINT (level));
	gconf_client_set_int (confclient, GRIG_CONFIG_DEBUG_KEY,
			      GPOINTER_TO_UINT (level),
			      NULL);
	gconf_client_suggest_sync (confclient, NULL);
}


/** \brief Run configuration component.
 *  \param widget The menu item that received the signal.
 *  \param data   User data (NULL).
 *
 * This function is called when the user selects the configuration
 * menu item in the menu bar. It executes the grig configuration
 * component.
 */
static void
grig_menu_config (GtkWidget *widget, gpointer data)
{
//	grig_config_run ();
}


/** \brief Show about box.
 *  \param widget The menu item that received the signal.
 *  \param data   User data (NULL).
 *
 * This function is called when the user selects the about menu item
 * in the menu bar. It executes the about box component.
 */
static void
grig_menu_about (GtkWidget *widget, gpointer data)
{
	grig_about_run ();
}

