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

/** \file rig-daemon.c
 *  \ingroup rigd
 *  \brief Radio control daemon.
 *
 * This object manages the connection to the hamradio control libraries.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <hamlib/rig.h>
#include "rig-daemon.h"




RIG *myrig = NULL;  /*!< The rig structure. */


extern GConfClient *confclient;  /*!< Shared GConfClient. */



/** \brief Start radio control daemon.
 *  \param rignum The index of the radio.
 *  \return 0 if the daemon has been initialized correctly.
 *
 * This function initializes the radio and starts the control daemon. The rignum
 * parameter is the rig number in the GConf configuration.
 */
int
rig_daemon_start (int rignum)
{
	gchar *buff;
	guint  rigid;
	gint   speed;
	gchar *rigport;
	gint   retcode;


	/* check if rig is already initialized */
	if (myrig != NULL) {
		return;
	}


	/* get configuration */
	buff = g_strdup_printf ("%s/%i/ID", GRIG_CONFIG_RIG_DIR, rignum);
	rigid = gconf_client_get_int (confclient, buff, NULL);
	g_free (buff);

	buff = g_strdup_printf ("%s/%i/port", GRIG_CONFIG_RIG_DIR, rignum);
	rigport = gconf_client_get_string (confclient, buff, NULL);
	g_free (buff);
	
	buff = g_strdup_printf ("%s/%i/speed", GRIG_CONFIG_RIG_DIR, rignum);
	speed = gconf_client_get_int (confclient, buff, NULL);
	g_free (buff);
	

	/* initilize rig */
	myrig = rig_init (rigid);
	if (myrig == NULL) {
		return 1;
	}

	/* configure and open device */
	strncpy (myrig->state.rigport.pathname, rigport, FILPATHLEN);
	g_free (rigport);
	if (speed) {
		myrig->state.rigport.parm.serial.rate = speed;
	}
	retcode = rig_open (myrig);
	if (retcode != RIG_OK) {
		rig_cleanup (myrig);
		return 1;
	}


	/* everything is all rigth; initialized shared data */


	/* get current settings  */

	/* start daemon */

	return 0;
}



/** \brief Stop the radio control daemon.
 *
 * This function stops the radio control daemon and frees the resources used
 * by the control process and backends.
 */
void
rig_daemon_stop  ()
{
	/* close radio device */
	rig_close (myrig);

	/* clean up hamlib */
	rig_cleanup (myrig);

	myrig = NULL;
}


