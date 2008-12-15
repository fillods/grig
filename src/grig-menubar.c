/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2007  Alexandru Csete.

    Authors: Alexandru Csete <oz9aec@gmail.com>

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
#include "rig-gui-rx.h"
#include "rig-gui-tx.h"
#include "rig-gui-func.h"
#include "rig-state.h"
#include "grig-debug.h"



extern GtkWidget   *grigapp;    /* defined in main.c */



/* private function prototypes */
static void  grig_menu_app_exit        (GtkWidget *, gpointer);
static void  grig_menu_set_debug_level (GtkRadioAction *, gpointer);
static void  rx_window_cb (GtkToggleAction *toggleaction, gpointer data);
static void  tx_window_cb (GtkToggleAction *toggleaction, gpointer data);
static void  func_window_cb (GtkToggleAction *toggleaction, gpointer data);


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
	{ "Save", GTK_STOCK_SAVE, N_("_Save State"), "<control>S", N_("Save the state of the rig to a file"), G_CALLBACK (rig_state_save_cb) },
	{ "Load", GTK_STOCK_OPEN, N_("_Load State"), "<control>O", N_("Load the state of the rig from a file"), G_CALLBACK (rig_state_load_cb) },
	{ "Exit", GTK_STOCK_QUIT, N_("E_xit"), "<control>Q", N_("Exit the program"), G_CALLBACK (grig_menu_app_exit) },

	/* SettingsMenu */
	{ "Debug", GTK_STOCK_HARDDISK, N_("_Debug Level"), NULL, N_("Set Hamlib debug level"), NULL },

	/* ViewMenu */
	{ "MsgWin", GTK_STOCK_JUSTIFY_LEFT, N_("Message _Window"), NULL, N_("Show window with debug messages"), G_CALLBACK (rig_gui_message_window_show) },

	/* ToolsMenu */
	{ "Mem", NULL, N_("_SW Memory"), NULL, N_("Software Memory Mamager"), NULL },
	{ "BandMap", GTK_STOCK_INDEX, N_("_Band Map"), NULL, N_("Show the band map"), NULL },
	{ "Spectrum", GTK_STOCK_JUMP_TO, N_("S_pectrum Scope"), NULL, N_("Show the spectrum scope"), NULL },

	/* HelpMenu */
	{ "About", GTK_STOCK_ABOUT, N_("_About Grig"), NULL, N_("Show about dialog"), G_CALLBACK (grig_about_run) },
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


static GtkToggleActionEntry toggle_entries[] = 
{
	{ "LevelsRX", NULL, N_("_RX Level Controls"), NULL, N_("Show receiver level controls"), G_CALLBACK (rx_window_cb) },
	{ "LevelsTX", NULL, N_("_TX Level Controls"), NULL, N_("Show transmitter level controls"), G_CALLBACK (tx_window_cb) },
	{ "Tones", NULL, N_("_DCS/CTCSS"), NULL, N_("Show DCS and CTCSS controls"), NULL },
	{ "Func", GTK_STOCK_DIALOG_INFO, N_("_Special Functions"), NULL, N_("Radio specific functions"), G_CALLBACK (func_window_cb) },
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
"       <menuitem action='Save'/>"
"       <menuitem action='Load'/>"
"       <separator/>"
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
"       <menuitem action='LevelsRX'/>"
"       <menuitem action='LevelsTX'/>"
"       <separator/>"
/* "       <menuitem action='Tones'/>" */
"       <menuitem action='Func'/>"
"       <separator/>"
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


static GtkUIManager   *uimgr;


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
	GtkAccelGroup  *accgrp;
	GError         *error;
	gint           i;

	/* create action group */
	actgrp = gtk_action_group_new ("MenuActions");
	/* i18n */
	for (i=0; i<G_N_ELEMENTS (entries); i++) {
		if (entries[i].label)
			entries[i].label = _(entries[i].label);
		if (entries[i].tooltip)
			entries[i].tooltip = _(entries[i].tooltip);
	}
	for (i=0; i<G_N_ELEMENTS (radio_entries); i++) {
		if (radio_entries[i].label)
			radio_entries[i].label = _(radio_entries[i].label);
		if (radio_entries[i].tooltip)
			radio_entries[i].tooltip = _(radio_entries[i].tooltip);
	}
	for (i=0; i<G_N_ELEMENTS (toggle_entries); i++) {
		if (toggle_entries[i].label)
			toggle_entries[i].label = _(toggle_entries[i].label);
		if (toggle_entries[i].tooltip)
			toggle_entries[i].tooltip = _(toggle_entries[i].tooltip);
	}

	gtk_action_group_add_actions (actgrp, entries, G_N_ELEMENTS (entries), NULL);

	/* debug level radio group */
	gtk_action_group_add_radio_actions (actgrp, radio_entries, G_N_ELEMENTS (radio_entries),
					    grig_debug_get_level (),
					    G_CALLBACK (grig_menu_set_debug_level), NULL);

	/* View toggle items */
	gtk_action_group_add_toggle_actions (actgrp, toggle_entries, G_N_ELEMENTS (toggle_entries), NULL);

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
 *
 */
static void
grig_menu_set_debug_level (GtkRadioAction *action, gpointer data)
{
	grig_debug_set_level (gtk_radio_action_get_current_value (action));
}




/** \brief Show/hide RX controls
 *
 * This function is called when the user selects the "RX controls" menu item.
 * Depending on the state of the item (on/off) we have to either open or close
 * the CW controls window
 */
static void
rx_window_cb (GtkToggleAction *toggleaction, gpointer user_data)
{

	if (gtk_toggle_action_get_active (toggleaction)) {
		rig_gui_rx_create ();
	}
	else {
		rig_gui_rx_close ();
	}
}


/** \brief Show/hide TX controls
 *
 * This function is called when the user selects the "TX controls" menu item.
 * Depending on the state of the item (on/off) we have to either open or close
 * the CW controls window
 */
static void
tx_window_cb (GtkToggleAction *toggleaction, gpointer user_data)
{

	if (gtk_toggle_action_get_active (toggleaction)) {
		rig_gui_tx_create ();
	}
	else {
		rig_gui_tx_close ();
	}
}


/** \brief Show/hide FUNC controls
 *
 * This function is called when the user selects the "Func controls" menu item.
 * Depending on the state of the item (on/off) we have to either open or close
 * the CW controls window
 */
static void
func_window_cb (GtkToggleAction *toggleaction, gpointer user_data)
{

	if (gtk_toggle_action_get_active (toggleaction)) {
		rig_gui_func_create ();
	}
	else {
		rig_gui_func_close ();
	}
}


/** \bried Force TX menu item.
 *
 * This function can be used to force the TX controls menu item to
 * TRUE or FALSE. This is useful when the TX controls window is closed
 * without any menu action
 */
void
grig_menubar_force_tx_item (gboolean val)
{
	GtkWidget *item = NULL;

	item = gtk_ui_manager_get_widget (uimgr, "/GrigMenu/ViewMenu/LevelsTX");

	if (item != NULL)
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), val);
}

/** \bried Force RX menu item.
 *
 * This function can be used to force the RX controls menu item to
 * TRUE or FALSE. This is useful when the RX controls window is closed
 * without any menu action
 */
void
grig_menubar_force_rx_item (gboolean val)
{
	GtkWidget *item = NULL;

	item = gtk_ui_manager_get_widget (uimgr, "/GrigMenu/ViewMenu/LevelsRX");

	if (item != NULL)
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), val);
}

/** \bried Force FUNC menu item.
 *
 * This function can be used to force the FUNC controls menu item to
 * TRUE or FALSE. This is useful when the FUNC controls window is closed
 * without any menu action
 */
void
grig_menubar_force_func_item (gboolean val)
{
	GtkWidget *item = NULL;

	item = gtk_ui_manager_get_widget (uimgr, "/GrigMenu/ViewMenu/Func");

	if (item != NULL)
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), val);
}

