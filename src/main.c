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
#include "grig-druid.h"
#include "rig-data.h"


GConfClient *confclient;


gchar *dummy = N_("Hello");


/* command line arguments */
static gint rignum      = -1;    /*!< Flag indicating which radio to use. */
static gint rotnum      = -1;    /*!< Flag indicating which rotator to use. */
static gint listrigs    =  0;    /*!< Flag indicating that configured radios should be listed. */ 
static gint listrots    =  0;    /*!< Flag indicating that configured rotators should be listed. */
static gint rundruid    =  0;    /*!< Flag indicating that execution of first time druid is requested. */


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
	{ "run-druid", 'd', POPT_ARG_NONE, &rundruid, 1,
	  N_("Run first-time configuration druid"), NULL },
	{ NULL, '\0', 0, NULL, 0 }
};


/* private function prototypes */
static int  grig_check_config   (void);
static void grig_list_rigs_rots (void);


int
main (int argc, char *argv[])
{
	GnomeProgram *program;

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


	/* Check whether first time druid needs to be executed.
	   This is the case if rundruid = 1 (cmd line option) or
	   the grig_check_config() function returns 1.
	*/
	rundruid = rundruid && grig_check_config ();
	if (rundruid) {
//		grig_druid_run ();
	}

	/* check whether user has requested a specific radio,
	   if not, get the default.
	*/
	if (rignum == -1) {
//		rignum = grig_get_default_rig ();
	}

	/* launch rig daemon */
	rig_daemon_start (rignum);

	/* launch rig UI */
//	rig_ctrl_create ();

	/* check whether user has requested a rotator,
	   if yes, start rotator controls
	*/
	if (rotnum != -1) {

		/* launch rotator daemon */
//		rot_daemon_start (rotnum);

		/* launch rotator UI */
//		rot_ctrl_create ();
	}

	/* register function which should be called when
	   the program is terminated
	*/
//	grig_exit_register ();

	gtk_main ();

	/* To clean up:
	   GUI
	   daemon
	   gconf
	*/

	rig_daemon_stop ();

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
static int
grig_check_config ()
{
	return 0;
}


/** \brief List rigs and/or rotators.
 *
 * This function lists the configured radios and rotators if it has
 * been requested by the user (cmd line option).
 */
static void
grig_list_rigs_rots ()
{

	if (listrigs) {

	}

	if (listrots) {

	}
}
