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
#include "grig-config.h"
#include "grig-druid.h"
#include "rig-gui.h"
#include "rig-data.h"


extern GnomeUIInfo grig_menubar[];  /* menubar defined in grig-menu.c */


/** \brief Shared GConf client. */
GConfClient *confclient;

/** \brief Main GUI application widget */
GtkWidget    *grigapp;




/* command line arguments */
static gint rignum      = -1;    /*!< Flag indicating which radio to use. */
static gint rotnum      = -1;    /*!< Flag indicating which rotator to use. */
static gint listrigs    =  0;    /*!< Flag indicating that configured radios should be listed. */ 
static gint listrots    =  0;    /*!< Flag indicating that configured rotators should be listed. */
// begin PATCH-996426
//static gint rundruid    =  0;    /*!< Flag indicating that execution of first time druid is requested. */
// end PATCH-996426


/* Static structure defining command line arguments for
   the application.
*/
static const struct poptOption grig_options[] =
{
	{ "list-rigs", 'i', POPT_ARG_NONE, &listrigs, 1,
	  N_("Show a list of configured radios"), NULL },
	{ "list-rots", 'o', POPT_ARG_NONE, &listrots, 1,
	  N_("Show a list of configured rotators"), NULL },
	{ "radio", 'r', POPT_ARG_INT, &rignum, 0,
	  N_("Use the radio with NUMBER"),
	  N_("NUMBER") },
	{ "antenna", 'a', POPT_ARG_INT, &rotnum, 0,
	  N_("Use the rotator with NUMBER"), 
	  N_("NUMBER") },
// begin PATCH-996426
//	{ "run-druid", 'd', POPT_ARG_NONE, &rundruid, 1,
//	  N_("Run first-time configuration druid"), NULL },
// end PATCH-996426
	{ NULL, '\0', 0, NULL, 0 }
};


/* private function prototypes */
// begin PATCH-996426
//static int         grig_check_config   (void);
// end PATCH-996426
static void        grig_list_rigs_rots (void);
static GtkWidget  *grig_app_create     (gint);
static gint        grig_app_delete     (GtkWidget *, GdkEvent *, gpointer);
static void        grig_app_destroy    (GtkWidget *, gpointer);

int
main (int argc, char *argv[])
{
	GnomeProgram *program;      /* GNOME program */


	/* Initialize NLS support */
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	/* initialize the GNOME application */
	program = gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE,
				      argc, argv,
				      /* optional parameters */
				      GNOME_PARAM_POPT_TABLE, grig_options, 
				      GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR,
				      NULL);

	/* initialize threads */
	if (!g_thread_supported ())
		g_thread_init (NULL);

	/* initialize GConf if needed */
	if (!gconf_is_initialized ())
		gconf_init (argc, argv, NULL);

	/* get default GConf client */
	confclient = gconf_client_get_default ();
	if (!confclient) {
		g_print(_("\nERROR: Could not get GConf client.\n"));
		return 1;
	}


	if (listrigs || listrots) {
		
		/* list rig and/or rotators */
		grig_list_rigs_rots ();

		/* exit */
		return 0;
	}


// begin PATCH-996426 
	/* Check whether first time druid needs to be executed.
	   This is the case if rundruid = 1 (cmd line option) or
	   the grig_check_config() function returns 1.
	*/
//	rundruid = rundruid && grig_check_config ();
//	if (rundruid) {

		/* run druid and check result */
//		if (grig_druid_run ()) {

			/* something bad happened */

//		}
//	}
// end PATCH-996426 

	/* check whether user has requested a specific radio,
	   if not, get the default.
	*/
	if (rignum == -1) {
		rignum = gconf_client_get_int (confclient, GRIG_CONFIG_RIG_DEF_KEY, NULL);
	}


	/* we set hamlib debug level to TRACE while we fire up the daemon; it will be
	   reset when we create the menubar
	*/
	rig_set_debug (RIG_DEBUG_TRACE);

	/* launch rig daemon */
	if (rig_daemon_start (rignum)) {
		return 1;
	}

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

//	gconf_client_notify_remove (confclient, cnx_id);
//	gconf_client_remove_dir    (confclient, GRIG_CONFIG_DIR, NULL);


	return 0;
}


/** \brief Check configuration.
 *  \return 0 if configuration is OK, 1 if configuration is not OK.
 *
 * This function checks whether the grig configuration is usable or not.
 * This is done by acquiring the index of the default radio and rotator,
 * and checking that those settings actually exist. If they do, a default
 * configuration exists and the druid needs not to be run.
 *
 * \note The user can explicitly request the execution of the first time
 *       druid using a command line argument.
 */
// begin PATCH-996426
//static int
//grig_check_config ()
//{
//	gboolean rigok = FALSE;    /* flag indicating result of rig check */
//	gboolean rotok = FALSE;    /* flag indicating result of rotator check */
//	gint     def   = -1;       /* default rig or rotator */
//	gint     number = 0;       /* number of rigs/rots */
//	gchar   *buff;             /* striing buffer */
//
//
	/* check rig config */
//	def = gconf_client_get_int (confclient, GRIG_CONFIG_RIG_DEF_KEY, NULL);

//	buff = g_strdup_printf ("%s/%i", GRIG_CONFIG_RIG_DIR, def);
//	rigok = gconf_client_dir_exists (confclient, buff, NULL);
//	g_free (buff);

	/* check that number of rigs > 0 */
//	number = gconf_client_get_int (confclient, GRIG_CONFIG_RIG_NUM_KEY, NULL);
	
//	rigok = (rigok && number);

//	def = -1;
//	number = 0;

	/* check rotator config */
//	def = gconf_client_get_int (confclient, GRIG_CONFIG_ROT_DEF_KEY, NULL);

//	buff = g_strdup_printf ("%s/%i", GRIG_CONFIG_ROT_DIR, def);
//	rotok = gconf_client_dir_exists (confclient, buff, NULL);
//	g_free (buff);

	/* check that number of rots > 0 */
//	number = gconf_client_get_int (confclient, GRIG_CONFIG_ROT_NUM_KEY, NULL);

//	rotok = (rotok && number);


//	return (rigok && rotok);
//}


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
	gint   i;             /* loop iterator */
	gchar *buff;          /* text buffer */
	gint   number;        /* number of rigs/rots */
	gint   def;           /* default rig/rot */
	gchar *brand;         /* brand of the rig/rot */
	gchar *model;         /* model of the rig/rot */
	gchar *port;          /* port of the rig/rot */
	gint   speed;         /* serial speed */
	gint   type;          /* rotator type */

	/* BUG */
	gchar *types[] = { "AZ/EL", "AZ", "EL" };


	if (listrigs) {
		/* get number of rigs */
		number = gconf_client_get_int (confclient, GRIG_CONFIG_RIG_NUM_KEY, NULL);

		/* if number is 0 no rigs are configured */
		if (number ==0) {
			g_print ("\n\n");
			g_print (_("   No radios have been configured.\n"));
			g_print (_("   Grig will use dummy backend.\n"));
		}
		else {

			/* get default rig */
			def = gconf_client_get_int (confclient, GRIG_CONFIG_RIG_DEF_KEY, NULL);
			
			g_print ("\n\n");
			g_print (_(" List of configured radios:\n\n"));
			g_print (_("  #   Brand         Model         Port            Speed      Def\n"));
		
			/* loop over aal rigs */
			for (i = 0; i < number; i++) {

				/* get Brand */
				buff = g_strdup_printf ("%s/%i/Brand", GRIG_CONFIG_RIG_DIR, i);
				brand = gconf_client_get_string (confclient, buff, NULL);
				g_free (buff);
				
				/* get model */
				buff = g_strdup_printf ("%s/%i/Model", GRIG_CONFIG_RIG_DIR, i);
				model = gconf_client_get_string (confclient, buff, NULL);
				g_free (buff);
				
				/* get port */
				buff = g_strdup_printf ("%s/%i/port", GRIG_CONFIG_RIG_DIR, i);
				port = gconf_client_get_string (confclient, buff, NULL);
				g_free (buff);
				
				/* get speed */
				buff = g_strdup_printf ("%s/%i/speed", GRIG_CONFIG_RIG_DIR, i);
				speed = gconf_client_get_int (confclient, buff, NULL);
				g_free (buff);
				
				/* get type */
				buff = g_strdup_printf ("%s/%i/type", GRIG_CONFIG_RIG_DIR, i);
				type = gconf_client_get_int (confclient, buff, NULL);
				g_free (buff);
				
				if (speed != 0) {
					printf ("%3d   %-13s %-13s %-15s %-10d %s\n",
						i, brand, model, port, speed, (i==def) ? " * " : "");
				}
				else {
					printf ("%3d   %-13s %-13s %-15s %-10s %s\n",
						i, brand, model, port, "DEF", (i==def) ? " * " : "");
				}

				g_free (brand);
				g_free (model);
				g_free (port);
			}
		}

	}

	if (listrots) {
		/* get number of rots */
		number = gconf_client_get_int (confclient, GRIG_CONFIG_ROT_NUM_KEY, NULL);

		/* if number is 0 no rotators are configured */
		if (number ==0) {
			g_print ("\n\n");
			g_print (_("   No rotators have been configured.\n"));
			g_print (_("   Grig will use dummy backend.\n"));
		}
		else {

			/* get default rot */
			def = gconf_client_get_int (confclient, GRIG_CONFIG_ROT_DEF_KEY, NULL);

			g_print ("\n\n");
			g_print (_(" List of configured rotators:\n\n"));
			g_print (_("  #   Brand         Model         Type    Port            Speed      Def\n"));
		
			/* loop over all rots */
			for (i = 0; i < number; i++) {

				/* get Brand */
				buff = g_strdup_printf ("%s/%i/Brand", GRIG_CONFIG_ROT_DIR, i);
				brand = gconf_client_get_string (confclient, buff, NULL);
				g_free (buff);

				/* get model */
				buff = g_strdup_printf ("%s/%i/Model", GRIG_CONFIG_ROT_DIR, i);
				model = gconf_client_get_string (confclient, buff, NULL);
				g_free (buff);

				/* get port */
				buff = g_strdup_printf ("%s/%i/port", GRIG_CONFIG_ROT_DIR, i);
				port = gconf_client_get_string (confclient, buff, NULL);
				g_free (buff);

				/* get speed */
				buff = g_strdup_printf ("%s/%i/speed", GRIG_CONFIG_ROT_DIR, i);
				speed = gconf_client_get_int (confclient, buff, NULL);
				g_free (buff);
				
				if (speed != 0) {
					printf ("%3d   %-13s %-13s %-7s %-15s %-10d %s\n",
						i, brand, model, types[type], port, speed, (i==def) ? " * " : "");
				}
				else {
					printf ("%3d   %-13s %-13s %-7s %-15s %-10s %s\n",
						i, brand, model, types[type], port, "DEF", (i==def) ? " * " : "");
				}

				g_free (brand);
				g_free (model);
				g_free (port);
			}
		}

	}
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

	/* just a last sanity check to make sure we have a rig */
	g_return_val_if_fail (rignum >= 0, NULL);


// begin PATCH-996426
	/* check whether we have a configuration at rignum; otherwise we will
	   just use the dummy backend.
	*/
	buff = g_strdup_printf ("%s/%i", GRIG_CONFIG_RIG_DIR, rignum);

	if (!gconf_client_dir_exists (confclient, buff, NULL)) {
		g_free (buff);
		
		brand = g_strdup ("Hamlib");
		model = g_strdup ("Dummy");
	}
	else {
		g_free (buff);
// end PATCH-996426
		/* get rig brand */
		buff = g_strdup_printf ("%s/%i/Brand", GRIG_CONFIG_RIG_DIR, rignum);
		brand = gconf_client_get_string (confclient, buff, NULL);
		g_free (buff);

		/* get rig model */
		buff = g_strdup_printf ("%s/%i/Model", GRIG_CONFIG_RIG_DIR, rignum);
		model = gconf_client_get_string (confclient, buff, NULL);
		g_free (buff);

// begin PATCH-996426
	}
// end PATCH-996426

	/* construct title */
	title = g_strdup_printf (_("Gnome RIG %s: %s %s"), VERSION, brand, model);

	/* create application */
	app = gnome_app_new (PACKAGE, title);

	g_free (title);
	g_free (brand);
	g_free (model);

	/* connect delete and destroy signals */
	g_signal_connect (G_OBJECT (app), "delete_event",
			  G_CALLBACK (grig_app_delete), NULL);    
	g_signal_connect (G_OBJECT (app), "destroy",
			  G_CALLBACK (grig_app_destroy), NULL);

	/* add menubar */
	gnome_app_create_menus (GNOME_APP (app), grig_menubar);	 

	/* Set the correct debug level in the menubar */
	debug = gconf_client_get_int (confclient, GRIG_CONFIG_DEBUG_KEY, NULL);
	switch (debug) {
	case RIG_DEBUG_BUG:
		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar,
						     "Settings/Debug Level/Bug",
						     &pos);
		break;
	case RIG_DEBUG_ERR:
		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar,
						     "Settings/Debug Level/Error",
						     &pos);
		break;
	case RIG_DEBUG_WARN:
		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar,
						     "Settings/Debug Level/Warning",
						     &pos);
		break;
	case RIG_DEBUG_VERBOSE:
		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar,
						     "Settings/Debug Level/Verbose",
						     &pos);
		break;
	case RIG_DEBUG_TRACE:
		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar,
						     "Settings/Debug Level/Trace",
						     &pos);
		break;
	default:
		menushell = gnome_app_find_menu_pos (GNOME_APP(app)->menubar,
						     "Settings/Debug Level/None",
						     &pos);
	}
	menuitem = GTK_WIDGET (g_list_nth_data (GTK_MENU_SHELL (menushell)->children, pos-1));
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem), TRUE);


	/* add contents */
	gnome_app_set_contents (GNOME_APP (app), rig_gui_create ());


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

	/* stop timeouts */

	/* exit Gtk+ */
	gtk_main_quit ();
}


