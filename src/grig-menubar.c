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
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include "grig-about.h"
#include "grig-config.h"
#include "grig-menubar.h"
#include "rig-gui-info.h"
#include "rig-gui-message-window.h"




extern GtkWidget   *grigapp;    /* defined in main.c */
extern gint         debug;      /* defined in main.c */


/* private function prototypes */
static void  grig_menu_app_exit        (GtkWidget *, gpointer);
static void  grig_menu_set_debug_level (GtkRadioAction *, gpointer);


/** \brief Regular menu items. */
static GtkActionEntry entries[] = {
	{ "FileMenu", NULL, N_("_Radio") },
	{ "SettingsMenu", NULL, N_("_Settings") },
	{ "ViewMenu", NULL, N_("_View") },
	{ "ToolsMenu", NULL, N_("_Tools") },
	{ "HelpMenu", NULL, N_("_Help") },

	/* FileMenu */
	{ "Info", GTK_STOCK_DND, N_("_Info"), "<control>I", N_("Show info about radio"), G_CALLBACK (rig_gui_info_run) },
	{ "Stop", GTK_STOCK_STOP, N_("St_op daemon"), NULL, N_("Stop the Grig daemon"), NULL },
	{ "Start", GTK_STOCK_EXECUTE, N_("St_art daemon"), NULL, N_("Start the Grig daemon"), NULL },
	{ "Save", GTK_STOCK_SAVE, N_("_Save State"), "<control>S", N_("Save the state of the rig to a file"), NULL },
	{ "Load", GTK_STOCK_OPEN, N_("_Load State"), "<control>O", N_("Load the state of the rig from a file"), NULL },
	{ "Exit", GTK_STOCK_QUIT, N_("E_xit"), "<control>Q", N_("Exit the program"), G_CALLBACK (grig_menu_app_exit) },

	/* SettingsMenu */
	{ "Debug", GTK_STOCK_HARDDISK, N_("_Debug Level"), NULL, N_("Set Hamlib debug level"), NULL },

	/* ViewMenu */
	{ "LevelsRX", NULL, N_("_RX Level Controls"), NULL, N_("Show receiver level controls"), NULL },
	{ "LevelsTX", NULL, N_("_TX Level Controls"), NULL, N_("Show transmitter level controls"), NULL },
	{ "CW", NULL, N_("_CW Controls"), NULL, N_("Show CW related controls"), NULL },
	{ "Tones", NULL, N_("_DCS/CTCSS"), NULL, N_("Show DCS and CTCSS controls"), NULL },
	{ "Func", GTK_STOCK_DIALOG_INFO, N_("_Special Functions"), NULL, N_("Radio specific functions"), NULL },
	{ "MsgWin", GTK_STOCK_JUSTIFY_LEFT, N_("Message _Window"), NULL, N_("Show window with debug messages"), G_CALLBACK (rig_gui_message_window_show) },

	/* ToolsMenu */
	{ "Mem", NULL, N_("_SW Memory"), NULL, N_("Software Memory Mamager"), NULL },
	{ "BandMap", GTK_STOCK_INDEX, N_("_Band Map"), NULL, N_("Show the band map"), NULL },
	{ "Spectrum", GTK_STOCK_JUMP_TO, N_("S_pectrum Scope"), NULL, N_("Show the spectrum scope"), NULL },

	/* HelpMenu */
	{ "About", GTK_STOCK_ABOUT, N_("_About Grig"), NULL, N_("Shouw about dialog"), G_CALLBACK (grig_about_run) },
};


/** \brief Radio items for selectinghamlib debug level. */
static GtkRadioActionEntry radio_entries[] = {
  { "None",    NULL, N_("_No Debug"), NULL, N_("Don't show any debug mesages"),                0 },
  { "Bug",     NULL, N_("_Bug"),      NULL, N_("Show error messages caused by possible bugs"), 1 },
  { "Error",   NULL, N_("_Error"),    NULL, N_("Show run-time error messages"),                2 },
  { "Warn",    NULL, N_("_Warning"),  NULL, N_("Show warnings"),                               3 },
  { "Verbose", NULL, N_("_Verbose"),  NULL, N_("Verbose reporting"),                           4 },
  { "Trace",   NULL, N_("_Trace"),    NULL, N_("Trace everything"),                            5 }
};


/** \brief UI description string. */
static const char *menu_desc = 
"<ui>"
"  <menubar name='GrigMenu'>"
"    <menu action='FileMenu'>"
"       <menuitem action='Info'/>"
"       <separator/>"
/*"       <menuitem action='Start'/>"
"       <menuitem action='Stop'/>"
"       <separator/>"*/
/* "       <menuitem action='Save'/>" */
/* "       <menuitem action='Load'/>" */
/* "       <separator/>" */
"       <menuitem action='Exit'/>"
"    </menu>"
"    <menu action='SettingsMenu'>"
"       <menu action='Debug'>"
"          <menuitem action='None'/>"
"          <separator/>"
"          <menuitem action='Bug'/>"
"          <menuitem action='Error'/>"
"          <menuitem action='Warn'/>"
"          <menuitem action='Verbose'/>"
"          <menuitem action='Trace'/>"
"       </menu>"
"    </menu>"
"    <menu action='ViewMenu'>"
/* "       <menuitem action='LevelsRX'/>" */
/* "       <menuitem action='LevelsTX'/>" */
/* "       <menuitem action='CW'/>" */
/* "       <separator/>" */
/* "       <menuitem action='Tones'/>" */
/* "       <menuitem action='Func'/>" */
/* "       <separator/>" */
"       <menuitem action='MsgWin'/>"
"    </menu>"
/* "    <menu action='ToolsMenu'>" */
/* "       <menuitem action='Mem'/>" */
/* "       <menuitem action='BandMap'/>" */
/* "       <menuitem action='Spectrum'/>" */
/* "    </menu>" */
"    <menu action='HelpMenu'>"
"       <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";



/** \brief Create Grig menubar.
 *  \return The menubar widget.
 *
 * This function creates and initializes the main menubar for grig.
 * It should be called from the main gui_create function.
 */
GtkWidget *
grig_menubar_create ()
{
	GtkWidget      *menubar;
	GtkActionGroup *actgrp;
	GtkUIManager   *uimgr;
	GtkAccelGroup  *accgrp;
	GError         *error;

	/* create action group */
	actgrp = gtk_action_group_new ("MenuActions");
	gtk_action_group_add_actions (actgrp, entries, G_N_ELEMENTS (entries), NULL);
	gtk_action_group_add_radio_actions (actgrp, radio_entries, G_N_ELEMENTS (radio_entries), debug,
					    G_CALLBACK (grig_menu_set_debug_level), NULL);


	/* create UI manager */
	uimgr = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (uimgr, actgrp, 0);

	/* accelerator group */
	accgrp = gtk_ui_manager_get_accel_group (uimgr);
	gtk_window_add_accel_group (GTK_WINDOW (grigapp), accgrp);

	/* try to create UI from XML*/
	error = NULL;
	if (!gtk_ui_manager_add_ui_from_string (uimgr, menu_desc, -1, &error)) {
		g_print (_("Failed to build menubar: %s"), error->message);
		g_error_free (error);

		return NULL;
	}

	/* now, finally, get the menubar */
	menubar = gtk_ui_manager_get_widget (uimgr, "/GrigMenu");

	return menubar;
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
 *  \param action The GtkRadioAction item.
 *  \param data  Pointer to user data (not used).
 *
 * This function is called when the user selects a new debug level.
 * The new debug level is stored in the GConf domain.
 *
 */
static void
grig_menu_set_debug_level (GtkRadioAction *action, gpointer data)
{
	rig_set_debug (gtk_radio_action_get_current_value (action));
}


