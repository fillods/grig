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
#include "grig-config.h"
#include "rig-gui.h"
#include "rig-daemon.h"
#include "rig-data.h"
#include "support.h"





/** \brief Main GUI application widget */
GtkWidget    *grigapp;


/* command line arguments */
static gint rignum      = -1;    /*!< Flag indicating which radio to use. */
static gint rotnum      = -1;    /*!< Flag indicating which rotator to use. */
static gint listrigs    =  0;    /*!< Flag indicating that configured radios should be listed. */ 
static gint listrots    =  0;    /*!< Flag indicating that configured rotators should be listed. */



/* Static structure defining command line arguments for
   the application.
*/
/* static const struct poptOption grig_options[] = */
/* { */
/* 	{ "list-rigs", 'i', POPT_ARG_NONE, &listrigs, 1, */
/* 	  N_("Show a list of configured radios"), NULL }, */
/* 	{ "list-rots", 'o', POPT_ARG_NONE, &listrots, 1, */
/* 	  N_("Show a list of configured rotators"), NULL }, */
/* 	{ "radio", 'r', POPT_ARG_INT, &rignum, 0, */
/* 	  N_("Use the radio with NUMBER (see --list-rigs)"), */
/* 	  N_("NUMBER") }, */
/* 	{ "antenna", 'a', POPT_ARG_INT, &rotnum, 0, */
/* 	  N_("Use the rotator with NUMBER (see --list-rots)"),  */
/* 	  N_("NUMBER") }, */
/* 	{ NULL, '\0', 0, NULL, 0 } */
/* }; */


static void        grig_list_rigs_rots (void);
static GtkWidget  *grig_app_create     (gint);
static gint        grig_app_delete     (GtkWidget *, GdkEvent *, gpointer);
static void        grig_app_destroy    (GtkWidget *, gpointer);

int
main (int argc, char *argv[])
{


	/* Initialize NLS support */
#ifdef ENABLE_NLS
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	textdomain (PACKAGE);
#endif

	gtk_set_locale ();
	gtk_init (&argc, &argv);
/* 	setlocale (LCNUMERIC, "C"); */


	/* initialize threads */
	if (!g_thread_supported ())
		g_thread_init (NULL);



	if (listrigs || listrots) {
		
		/* list rig and/or rotators */
		grig_list_rigs_rots ();

		/* exit */
		return 0;
	}



	/* check whether user has requested a specific radio,
	   if not, get the default.
	*/
	if (rignum == -1) {
		rignum = 1;
	}


	/* we set hamlib debug level to TRACE while we fire up the daemon; it will be
	   reset when we create the menubar
	*/
	rig_set_debug (RIG_DEBUG_TRACE);

	/* launch rig daemon */
	if (rig_daemon_start (rignum)) {
		return 1;
	}

	rig_set_debug (RIG_DEBUG_WARN);

	/* create application */
	grigapp = grig_app_create (rignum);
	gtk_widget_show_all (grigapp);



	/* check whether user has requested a rotator,
	   if yes, start rotator controls
	*/
	if (rotnum != -1) {

		/* launch rotator daemon */
//		rot_daemon_start (rotnum);

		/* launch rotator UI */
//		rot_ctrl_create ();
	}

	gtk_main ();



	return 0;
}




/** \brief List rigs and/or rotators.
 *
 * This function lists the configured radios and rotators if it has
 * been requested by the user (cmd line option).
 *
 * \bug The types local should be moved to the rot ctrl object.
 */
static void
grig_list_rigs_rots ()
{
}



/** \brief Create and initialize main application window.
 *  \param rignum The index of the radio wich is controled by the app
 *  \return The new GnomeApp widget or NULL if the specified rig number is invalid
 *
 * This function creates and initializes a new GnomeApp widget, adds
 * menubar and statusbar, etc...
 *
 */
static GtkWidget *
grig_app_create       (gint rignum)
{
	GtkWidget *app;        /* The main application */
	GtkWidget *menushell;  /* Menu shell; used to set debug level */
	GtkWidget *menuitem;   /* Menu item; used to set debug level */
	gint       pos;        /* position of the menu item */
	gint       debug;      /* hamlib debug level */
	gchar     *title;      /* the window title  */
	gchar     *brand;
	gchar     *model;
	gchar     *buff;       /* text buffer */
	gchar     *icon;

	/* just a last sanity check to make sure we have a rig */
	g_return_val_if_fail (rignum >= 0, NULL);


	brand = g_strdup ("Hamlib");
	model = g_strdup ("Dummy");

	/* construct title */
	title = g_strdup_printf (_("GRIG: %s %s"), brand, model);

	/* window icon file */
	icon = g_strconcat (PACKAGE_DATA_DIR, G_DIR_SEPARATOR_S, "pixmaps",
			    G_DIR_SEPARATOR_S, "ic910.png", NULL);

	/* create application */
	app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (app), title);
	gtk_window_set_icon_from_file (GTK_WINDOW (app), icon, NULL);

	g_free (title);
	g_free (brand);
	g_free (model);
	g_free (icon);

	/* connect delete and destroy signals */
	g_signal_connect (G_OBJECT (app), "delete_event",
			  G_CALLBACK (grig_app_delete), NULL);    
	g_signal_connect (G_OBJECT (app), "destroy",
			  G_CALLBACK (grig_app_destroy), NULL);

	/** create menus **/

/* 	/\* Set the correct debug level in the menubar *\/ */
/* 	debug = gconf_client_get_int (confclient, GRIG_CONFIG_DEBUG_KEY, NULL); */
/* 	switch (debug) { */
/* 	case RIG_DEBUG_BUG: */
/* 		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar, */
/* 						     "Settings/Debug Level/Bug", */
/* 						     &pos); */
/* 		break; */
/* 	case RIG_DEBUG_ERR: */
/* 		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar, */
/* 						     "Settings/Debug Level/Error", */
/* 						     &pos); */
/* 		break; */
/* 	case RIG_DEBUG_WARN: */
/* 		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar, */
/* 						     "Settings/Debug Level/Warning", */
/* 						     &pos); */
/* 		break; */
/* 	case RIG_DEBUG_VERBOSE: */
/* 		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar, */
/* 						     "Settings/Debug Level/Verbose", */
/* 						     &pos); */
/* 		break; */
/* 	case RIG_DEBUG_TRACE: */
/* 		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar, */
/* 						     "Settings/Debug Level/Trace", */
/* 						     &pos); */
/* 		break; */
/* 	default: */
/* 		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar, */
/* 						     "Settings/Debug Level/None", */
/* 						     &pos); */
/* 	} */
/* 	menuitem = GTK_WIDGET (g_list_nth_data (GTK_MENU_SHELL (menushell)->children, pos-1)); */
/* 	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem), TRUE); */


	/* add contents */
	gtk_container_add (GTK_CONTAINER (app), rig_gui_create ());


	return app;
}



/** \brief Handle delete events.
 *  \param widget The widget which received the delete event signal.
 *  \param event  Data structure describing the event.
 *  \param data   User data (NULL).
 *  \param return Always FALSE to indicate that the app should be destroyed.
 *
 * This function handles the delete event received by the main application window
 * (eg. when the window is closed by the WM). This function simply returns FALSE
 * indicating that the main application window should be destroyed by emiting the
 * destroy signal.
 *
 */
static gint
grig_app_delete      (GtkWidget *widget,
		      GdkEvent  *event,
		      gpointer   data)
{

	/* return FALSE so that Gtk+ will emit the destroy signal */
	return FALSE;
}



/** \brief Handle destroy signals.
 *  \param widget The widget which received the signal.
 *  \param data   User data (NULL).
 *
 * This function is called when the main application window receives the
 * destroy signal, ie. it is destroyed. This function signals all daemons
 * and other threads to stop and exits the Gtk+ main loop.
 *
 */
static void
grig_app_destroy    (GtkWidget *widget,
		     gpointer   data)
{

	/* set debug level to TRACE */
	rig_set_debug (RIG_DEBUG_TRACE);

	/* stop daemons */
	rig_daemon_stop ();
//	rot_daemon_stop ();

	/* GUI timers are stopped automatically */

	/* stop timeouts */

	/* exit Gtk+ */
	gtk_main_quit ();
}


