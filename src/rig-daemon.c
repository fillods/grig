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
 *  \brief Radio control interface to hamlib.
 *
 * This object is responsible for interfacing the Hamradio Control Libraries (hamlib).
 *
 * After initialization of the radio it starts a cyclic thread which will
 * execute some pre-defined commands. Because some manufacturers discourage
 * agressive polling while in TX mode, the daemon will only acquire very
 * few things while in this mode.
 *
 * More about cycles and periods...
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-daemon.h"



RIG *myrig = NULL;  /*!< The rig structure. We keep this public so GUI can access the info fields. */

extern GConfClient *confclient;  /*!< Shared GConfClient. */


/** \brief Matrix defining the default RX cycle.
 *
 * More description of the idea.
 *
 */
static const rig_cmd_t DEF_RX_CYCLE[C_MAX_CYCLES][C_MAX_CMD_PER_CYCLE] = {
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_1, RIG_CMD_GET_FREQ_1, RIG_CMD_GET_PSTAT, RIG_CMD_SET_PSTAT },
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_2, RIG_CMD_GET_FREQ_2, RIG_CMD_SET_RIT,   RIG_CMD_GET_RIT   },
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_1, RIG_CMD_GET_FREQ_1, RIG_CMD_SET_RIT,   RIG_CMD_GET_RIT   },
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_2, RIG_CMD_GET_FREQ_2, RIG_CMD_SET_XIT,   RIG_CMD_GET_XIT   },
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_1, RIG_CMD_GET_FREQ_1, RIG_CMD_SET_MODE,  RIG_CMD_GET_MODE  },
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_VFO,    RIG_CMD_GET_VFO,    RIG_CMD_SET_PTT,   RIG_CMD_GET_PTT   }
};



/** \brief MAtrix defining the default TX cycle.
 *
 * More description.
 *
 * \note Some radios do not like being polled while in TX mode so
 *       we make TX cycle easy...
 *
 */
static const rig_cmd_t DEF_TX_CYCLE[C_MAX_CYCLES][C_MAX_CMD_PER_CYCLE] = {
	{ RIG_CMD_SET_PTT, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE },
	{ RIG_CMD_GET_PTT, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE },
	{ RIG_CMD_NONE,    RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE },
	{ RIG_CMD_SET_PTT, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE },
	{ RIG_CMD_GET_PTT, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE },
	{ RIG_CMD_NONE,    RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE, RIG_CMD_NONE }
};


static gboolean stopdaemon = FALSE;   /*!< Used to signal the daemon thread that it should stop */


/* private function prototypes */
static void     rig_daemon_post_init (void);
static gpointer rig_daemon_cycle     (gpointer);
static void     rig_daemon_exec_cmd  (rig_cmd_t,
				      grig_settings_t  *,
				      grig_settings_t  *,
				      grig_cmd_avail_t *,
				      grig_cmd_avail_t *);



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
	grig_settings_t *get;

	/* check if rig is already initialized */
	if (myrig != NULL) {
		return 1;
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


	/* get capabilities and settings  */
	rig_daemon_post_init ();

	/* start daemon */
	g_thread_create (rig_daemon_cycle, NULL, FALSE, NULL);

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
	/* send stop signal to daemon process */
	stopdaemon = TRUE;

	/* wait */
	usleep (C_RIG_DAEMON_STOP_TIMEOUT);

	/* close radio device */
	rig_close (myrig);

	/* clean up hamlib */
	rig_cleanup (myrig);

	myrig = NULL;
}


/** \brief Execute post initialization tasks.
 *
 * This function executes some tasks after initialization of the radio
 * hardware. These include testing the radio capabilities and obtaining
 * the current settings. The test results are communicated to the user
 * via the rig_debug() Hamlib function.
 */
static void
rig_daemon_post_init ()
{
	grig_settings_t  *get;                     /* pointer to shared data 'get' */
	grig_cmd_avail_t *has_get;                 /* pointer to shared data 'has_get' */
	grig_cmd_avail_t *has_set;                 /* pointer to shared data 'has_set' */
	int               retcode;                 /* Hamlib status code */
	powerstat_t       pwr = RIG_POWER_OFF;     /* power status */
	ptt_t             ptt = RIG_PTT_OFF;       /* PTT status */
	vfo_t             vfo = RIG_VFO_NONE;      /* current VFO */
	freq_t            freq;                    /* current frequency */
	shortfreq_t       sfreq;                   /* current RIT/XIT setting */
	rmode_t           mode;                    /* current mode */
	pbwidth_t         pbw;                     /* current passband width */



	/* get pointers to shared data */
	get     = rig_data_get_get_addr ();
	has_get = rig_data_get_has_get_addr ();
	has_set = rig_data_get_has_set_addr ();


	/* get power status; we are very paranoid and accept only
	   RIG_OK as good status.
	*/
	retcode = rig_get_powerstat (myrig, &pwr);
	if (retcode == RIG_OK) {
		has_get->power = TRUE;
		get->power = pwr;
	}
	else {
		has_get->power = FALSE;
		get->power = RIG_POWER_OFF;
	}

	/* try to set power status */
	retcode = rig_set_powerstat (myrig, get->power);
	has_set->power = (retcode == RIG_OK) ? TRUE : FALSE;

	/* PTT status */
	retcode = rig_get_ptt (myrig, RIG_VFO_CURR, &ptt);
	if (retcode == RIG_OK) {
		has_get->ptt = TRUE;
		get->ptt = ptt;
	}
	else {
		has_get->ptt = FALSE;
		get->ptt = RIG_PTT_OFF;
	}

	/* try to set PTT status */
	retcode = rig_set_ptt (myrig, RIG_VFO_CURR, get->ptt);
	has_set->ptt = (retcode == RIG_OK) ? TRUE : FALSE;

	/* try to get current VFO */
	retcode = rig_get_vfo (myrig, &vfo);
	if (retcode == RIG_OK) {
		has_get->vfo = TRUE;
		get->vfo = vfo;
	}
	else {
		has_get->vfo = FALSE;
		get->vfo = RIG_VFO_NONE;
	}

	/* check set_vfo functionality */
	retcode = rig_set_vfo (myrig, get->vfo);
	has_set->vfo = (retcode == RIG_OK) ? TRUE : FALSE;

	/* check get_freq functionality for the primary/working
	   frequency.
	*/
	retcode = rig_get_freq (myrig, RIG_VFO_CURR, &freq);
	if (retcode == RIG_OK) {
		has_get->freq1 = TRUE;
		get->freq1 = freq;
	}
	else {
		has_get->freq1 = FALSE;
		get->freq1 = 0.0;
	}

	/* try to reset the current frequency but only if we
	   have get_freq; if not don't bother with set_freq ???
	*/
	if (has_get->freq1) {
		retcode = rig_set_freq (myrig, RIG_VFO_CURR, get->freq1);
		has_set->freq1 = (retcode == RIG_OK) ? TRUE : FALSE;
	}
	else {
		has_set->freq1 = FALSE;
	}


	/* now we try the secondary frequency, ie. not the one on RIG_VFO_CURR */
	if (has_get->freq1) {
		switch (get->vfo) {
			
			/* VFO A */
		case RIG_VFO_A:
			retcode = rig_get_freq (myrig, RIG_VFO_B, &freq);
			break;

			/* VFO B or C; grig is too stupid to know about 3 VFOs ...
			   or at least I am to lazy to bother about 3 VFOs ;)
			*/
		case RIG_VFO_B:
		case RIG_VFO_C:
			vfo = RIG_VFO_A;
			break;

			/* Main VFO */
		case RIG_VFO_MAIN:
			vfo = RIG_VFO_SUB;
			break;

			/* Sub VFO */
		case RIG_VFO_SUB:
			vfo = RIG_VFO_MAIN;
			break;

			/* trouble... */
		case RIG_VFO_CURR:
		case RIG_VFO_NONE:
		default:
			vfo = RIG_VFO_NONE;
			break;
		}

		if (vfo != RIG_VFO_NONE) {
			retcode = rig_get_freq (myrig, vfo, &freq);
			if (retcode == RIG_OK) {
				has_get->freq2 = TRUE;
				get->freq2 = freq;
			}
			else {
				has_get->freq2 = FALSE;
				get->freq2 = 0.0;
			}
		}
		else {
			has_get->freq2 = FALSE;
			get->freq2 = 0.0;
		}

		/* try to set secondary frequency; normally we should not do it
		   here (inside the if), but it makes no difference due to the
		   "No Get => No Set" policy
		*/
		if (has_get->freq2) {

			/* NB: has_get->freq2 TRUE => vfo is different from RIG_VFO_NONE */
			retcode = rig_set_freq (myrig, vfo, get->freq2);
			has_set->freq2 = (retcode == RIG_OK) ? TRUE : FALSE;
		}
		else {
			has_set->freq2 = FALSE;
		}

	}
	else {
		has_get->freq2 = FALSE;
		get->freq2 = 0.0;
	}


	/* try to get RIT setting */
	retcode = rig_get_rit (myrig, RIG_VFO_CURR, &sfreq);
	if (retcode == RIG_OK) {
		has_get->rit = TRUE;
		get->rit = sfreq;
	}
	else {
		has_get->rit = FALSE;
		get->rit = 0;
	}

	/* try to reset RIT */
	retcode = rig_set_rit (myrig, RIG_VFO_CURR, get->rit);
	has_set->rit = (retcode == RIG_OK) ? TRUE : FALSE;


	/* try to get XIT */
	retcode = rig_get_xit (myrig, RIG_VFO_CURR, &sfreq);
	if (retcode == RIG_OK) {
		has_get->xit = TRUE;
		get->xit = sfreq;
	}
	else {
		has_get->xit = FALSE;
		get->xit = 0;
	}

	/* try to reset XIT */
	retcode = rig_set_xit (myrig, RIG_VFO_CURR, get->xit);
	has_set->xit = (retcode == RIG_OK) ? TRUE : FALSE;


	/* try to get mode and passband width */
	retcode = rig_get_mode (myrig, RIG_VFO_CURR, &mode, &pbw);
	if (retcode == RIG_OK) {
		has_get->mode = TRUE;
		has_get->pbw  = TRUE;
		get->mode     = mode;
		get->pbw      = pbw;
	}
	else {
		has_get->mode = FALSE;
		has_get->pbw  = FALSE;
		get->mode     = RIG_MODE_NONE;
		get->pbw      = RIG_PASSBAND_NORMAL;
	}

	/* try to reset mode and passband width;
	   ok to set RIG_MODE_NONE?
	*/
	retcode = rig_set_mode (myrig, RIG_VFO_CURR, get->mode, get->pbw);
	if (retcode == RIG_OK) {
		has_set->mode = TRUE;
		has_set->pbw  = TRUE;
	}
	else {
		has_set->mode = FALSE;
		has_set->pbw  = FALSE;
	}
}



/** \brief Radio control daemon main cycle.
 *  \param data Unused.
 *  \return Always NULL.
 *
 * This function implements the main cycle of the radio control daemon. The executed
 * commands are defined in the DEF_RX_CYCLE and DEF_TX_CYCLE constant arrays.
 */
static gpointer
rig_daemon_cycle     (gpointer data)
{
	grig_settings_t  *get;             /* pointer to shared data 'get' */
	grig_settings_t  *set;             /* pointer to shared data 'set' */
	grig_cmd_avail_t *new;             /* pointer to shared data 'new' */
	grig_cmd_avail_t *has_get;         /* pointer to shared data 'has_get' */
	grig_cmd_avail_t *has_set;         /* pointer to shared data 'has_set' */

	guint major, minor;                /* major and minor cycle */


	/* get pointers to shared data */
	get     = rig_data_get_get_addr ();
	set     = rig_data_get_set_addr ();
	new     = rig_data_get_new_addr ();
	has_get = rig_data_get_has_get_addr ();
	has_set = rig_data_get_has_set_addr ();

	/* initialize major cycle */
	major = 0;

	/* loop forever until reception of STOP signal */
	while (stopdaemon == FALSE) {

		/* check whether we are in RX or TX mode;
		   note that the major cycle is not influenced
		   by any change in RX/TX state
		*/
		if (get->ptt == RIG_PTT_OFF) {
			/* Execute receiver cycle */

			/* loop through the current cycle in the command table */
			for (minor = 0; minor < C_MAX_CMD_PER_CYCLE; minor++) {

				rig_daemon_exec_cmd (DEF_RX_CYCLE[major][minor],
						     get, set,
						     has_get, has_set);
						     
			}

			usleep (1000 * C_RX_CYCLE_DELAY);

		}
		else {
			/* Execute transmitter cycle */

			/* loop through the current cycle in the commad table. */
			for (minor = 0; minor < C_MAX_CMD_PER_CYCLE; minor++) {

				rig_daemon_exec_cmd (DEF_TX_CYCLE[major][minor],
						     get, set,
						     has_get, has_set);

			}

			usleep (1000 * C_TX_CYCLE_DELAY);

		}

		/* increment major cycle counter;
		   reset to zero if it reaches the maximum count
		*/
		if (++major == C_MAX_CYCLES)
			major = 0;

	}

	return NULL;
}



/** \brief Execute a specific command.
 *  \param cmd The command to be executed.
 *  \param get Pointer to the 'get' command buffer.
 *  \param set Pointer to the 'set' command buffer.
 *  \param has_get Pointer to get capabilities record.
 *  \param has_set Pointer to set capabilities record.
 *
 * This function is responsible for the execution of the specified rig command.
 * First, it checks whether the command is supported by the current radio, if yes,
 * it executes the corresponding hamlib API call. If the command execution is not
 * successfull, an anomaly report is sent to the rig error manager.
 */
static void
rig_daemon_exec_cmd         (rig_cmd_t cmd,
			     grig_settings_t  *get,
			     grig_settings_t  *set,
			     grig_cmd_avail_t *has_get,
			     grig_cmd_avail_t *has_set)

{

	switch (cmd) {

		/* get primary frequency */
	case RIG_CMD_GET_FREQ_1:
		break;

		/* set primary frequency */
	case RIG_CMD_SET_FREQ_1:
		break;

		/* get secondary frequency */
	case RIG_CMD_GET_FREQ_2:
		break;

		/* set secondary frequency */
	case RIG_CMD_SET_FREQ_2:
		break;
	
		/* get RIT offset */
	case RIG_CMD_GET_RIT:
		break;

		/* set RIT offset */
	case RIG_CMD_SET_RIT:
		break;

		/* get XIT offset */
	case RIG_CMD_GET_XIT:
		break;

		/* set XIT offset */
	case RIG_CMD_SET_XIT:
		break;

		/* get current VFO */
	case RIG_CMD_GET_VFO:
		break;

		/* set current VFO */
	case RIG_CMD_SET_VFO:
		break;

		/* get power status */
	case RIG_CMD_GET_PSTAT:
		break;

		/* set power status */
	case RIG_CMD_SET_PSTAT:
		break;

		/* get PTT status */
	case RIG_CMD_GET_PTT:
		break;

		/* set PTT status */
	case RIG_CMD_SET_PTT:
		break;

		/* get current mode and passband width */
	case RIG_CMD_GET_MODE:
		break;

		/* set current mode and passband width */
	case RIG_CMD_SET_MODE:
		break;

		/* get signal strength, S-meter */
	case RIG_CMD_GET_STRENGTH:
		break;

		/* get transmitter power */
	case RIG_CMD_GET_PWR:
		break;

	default:
		break;

	}


}
