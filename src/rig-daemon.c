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
 *  \bug This object MUST be decomposed!
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
#include "grig-config.h"
#include "rig-data.h"
#include "rig-daemon-check.h"
#include "rig-daemon.h"



RIG *myrig;  /*!< The rig structure. We keep this public so GUI can access the info fields. */

extern GConfClient *confclient;  /*!< Shared GConfClient. */


/** \brief Matrix defining the default RX cycle.
 *
 * More description of the idea.
 *
 */
static const rig_cmd_t DEF_RX_CYCLE[C_MAX_CYCLES][C_MAX_CMD_PER_CYCLE] = {
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_1, RIG_CMD_GET_FREQ_1, RIG_CMD_GET_PSTAT, RIG_CMD_SET_PSTAT },
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_2, RIG_CMD_GET_FREQ_2, RIG_CMD_SET_RIT,   RIG_CMD_GET_RIT   },
	{ RIG_CMD_GET_STRENGTH, RIG_CMD_SET_FREQ_1, RIG_CMD_GET_FREQ_1, RIG_CMD_SET_AGC,   RIG_CMD_GET_AGC   },
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

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE, "*** GRIG: %s entered\n", __FUNCTION__);

	/* check if rig is already initialized */
	if (myrig != NULL) {
		return 1;
	}

// begin PATCH-996426
	/* check if rignum exists; otherwise use dummy backend */
	buff = g_strdup_printf ("%s/%i", GRIG_CONFIG_RIG_DIR);
	if (!gconf_client_dir_exists (confclient, buff, NULL)) {
		/* free buffer */
		g_free (buff);

		/* send a debug message */
		rig_debug (RIG_DEBUG_TRACE,
			   "*** GRIG: %s: Config not found; using dummy backend\n",
			   __FUNCTION__);

		/* initialize values */
		rigid = 1;
		rigport = g_strdup ("/dev/ttyS0");
		speed = 0;
	}
	else {
		/* free buffer */
		g_free (buff);
//end PATCH-996426
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
// begin PATCH-996426
	}
//end PATCH-996426

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE,
		   "*** GRIG: %s: Initializing rig (id=%d)\n",
		   __FUNCTION__, rigid);

	/* initilize rig */
	myrig = rig_init (rigid);
	if (myrig == NULL) {

		/* send error report */
		rig_debug (RIG_DEBUG_ERR,
			   "*** GRIG: %s: Init failed; Hamlib returned NULL!\n", __FUNCTION__);

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

		/* send error report */
		rig_debug (RIG_DEBUG_ERR,
			   "*** GRIG: %s: Failed to open rig port %s (permissions?)\n",
			   __FUNCTION__,
			   myrig->state.rigport.pathname);

		rig_cleanup (myrig);
		return 1;
	}

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE,
		   "*** GRIG: %s: Init successfull; executing post-init\n",
		   __FUNCTION__);

	/* get capabilities and settings  */
	rig_daemon_post_init ();

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE,
		   "*** GRIG: %s: Starting rig daemon\n",
		   __FUNCTION__);

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

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE,
		   "*** GRIG: %s: Sending stop signal to rig daemon\n",
		   __FUNCTION__);


	/* send stop signal to daemon process */
	stopdaemon = TRUE;

	/* give the daemon som time to exit */
	usleep (C_RIG_DAEMON_STOP_TIMEOUT);

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE,
		   "*** GRIG: %s: Cleaning up rig\n",
		   __FUNCTION__);

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
 *
 */
static void
rig_daemon_post_init ()
{
	grig_settings_t  *get;                     /* pointer to shared data 'get' */
	grig_cmd_avail_t *has_get;                 /* pointer to shared data 'has_get' */
	grig_cmd_avail_t *has_set;                 /* pointer to shared data 'has_set' */


	/* get pointers to shared data */
	get     = rig_data_get_get_addr ();
	has_get = rig_data_get_has_get_addr ();
	has_set = rig_data_get_has_set_addr ();


	/* check command availabilities */
	rig_daemon_check_pwrstat (myrig, get, has_get, has_set);
	rig_daemon_check_ptt     (myrig, get, has_get, has_set);
	rig_daemon_check_vfo     (myrig, get, has_get, has_set);
	rig_daemon_check_freq    (myrig, get, has_get, has_set);
	rig_daemon_check_rit     (myrig, get, has_get, has_set);
	rig_daemon_check_xit     (myrig, get, has_get, has_set);
	rig_daemon_check_mode    (myrig, get, has_get, has_set);
	rig_daemon_check_level   (myrig, get, has_get, has_set);

	/* debug info about detected has-get caps */
	rig_debug (RIG_DEBUG_TRACE,
		   "*** GRIG: %s: GET bits: %d%d%d%d%d%d%d%d%d%d%d%d%d%d\n",
		   __FUNCTION__,
		   has_get->pstat,
		   has_get->ptt,
		   has_get->vfo,
		   has_get->mode,
		   has_get->pbw,
		   has_get->freq1,
		   has_get->freq2,
		   has_get->rit,
		   has_get->xit,
		   has_get->agc,
		   has_get->power,
		   has_get->strength,
		   has_get->swr,
		   has_get->alc);

	/* debug info about detected has-set caps */
	rig_debug (RIG_DEBUG_TRACE,
		   "*** GRIG: %s: SET bits: %d%d%d%d%d%d%d%d%d%d%dXXX\n",
		   __FUNCTION__,
		   has_set->pstat,
		   has_set->ptt,
		   has_set->vfo,
		   has_set->mode,
		   has_set->pbw,
		   has_set->freq1,
		   has_set->freq2,
		   has_set->rit,
		   has_set->xit,
		   has_set->agc,
		   has_set->power
/* not settable
		   has_get->strength,
		   has_get->swr,
		   has_get->alc*/
		);

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

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE, "*** GRIG: %s started.\n", __FUNCTION__);

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
						     get, set, new,
						     has_get, has_set);
						     
			}

			usleep (1000 * C_RX_CYCLE_DELAY);

		}
		else {
			/* Execute transmitter cycle */

			/* loop through the current cycle in the commad table. */
			for (minor = 0; minor < C_MAX_CMD_PER_CYCLE; minor++) {

				rig_daemon_exec_cmd (DEF_TX_CYCLE[major][minor],
						     get, set, new,
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

	/* send a debug message */
	rig_debug (RIG_DEBUG_TRACE, "*** GRIG: %s stopped\n", __FUNCTION__);

	return NULL;
}



/** \brief Execute a specific command.
 *  \param cmd The command to be executed.
 *  \param get Pointer to the 'get' command buffer.
 *  \param set Pointer to the 'set' command buffer.
 *  \param new Pointer to the 'new' command buffer.
 *  \param has_get Pointer to get capabilities record.
 *  \param has_set Pointer to set capabilities record.
 *
 * This function is responsible for the execution of the specified rig command.
 * First, it checks whether the command is supported by the current radio, if yes,
 * it executes the corresponding hamlib API call. If the command execution is not
 * successfull, an anomaly report is sent to the rig error manager which will take
 * care of any further actions like disabling repeatedly failing commands.
 *
 * \note The 'get' commands use local buffers for the acquired value and do not
 *       write directly to the shared memory. This way the contents of the shared memory
 *       do not get corrupted if the command execution was erroneous.
 */
static void
rig_daemon_exec_cmd         (rig_cmd_t cmd,
			     grig_settings_t  *get,
			     grig_settings_t  *set,
			     grig_cmd_avail_t *new,
			     grig_cmd_avail_t *has_get,
			     grig_cmd_avail_t *has_set)

{
	int retcode;


	switch (cmd) {

		/* No command. Do nothing */
	case RIG_CMD_NONE:
		break;

		/* get primary frequency */
	case RIG_CMD_GET_FREQ_1:

		/* check whether command is available */
		if (has_get->freq1) {
			freq_t freq;

			/* try to execute command */
			retcode = rig_get_freq (myrig, RIG_VFO_CURR, &freq);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_FREQ_1\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_FREQ_1);
			}
			else {
				get->freq1 = freq;
			}
		}

		break;

		/* set primary frequency */
	case RIG_CMD_SET_FREQ_1:

		/* check whether command is available */
		if (has_set->freq1 && new->freq1) {

			/* try to execute command */
			retcode = rig_set_freq (myrig, RIG_VFO_CURR, set->freq1);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_FREQ_1\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_FREQ_1);
			}
			else {
				/* reset flag */
				new->freq1 = FALSE;
			}
		}
		
		/* if rig doesn't have get_freq, we set the get->freq
		   manually to make widgets happy
		*/
		if (!has_get->freq1) {
			get->freq1 = set->freq1;
		}

		break;

		/* get secondary frequency */
	case RIG_CMD_GET_FREQ_2:

		/* check whether command is available */
		if (has_get->freq2) {
			freq_t freq;
			vfo_t  vfo;

			/* find out which is the secondary VFO */
			switch (get->vfo) {
			
				/* VFO A */
			case RIG_VFO_A:
				vfo = RIG_VFO_B;
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

				/* send an error report */
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: I can't figure out available VFOs (got %d)\n",
					   __FUNCTION__, get->vfo);

				vfo = RIG_VFO_NONE;
				break;
			}

			/* try to execute command */
			retcode = rig_get_freq (myrig, vfo, &freq);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_FREQ_2\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_FREQ_2);
			}
			else {
				get->freq2 = freq;
			}
		}

		break;

		/* set secondary frequency */
	case RIG_CMD_SET_FREQ_2:

		/* check whether command is available */
		if (has_set->freq2 && new->freq2) {
			vfo_t  vfo;

			/* find out which is the secondary VFO */
			switch (get->vfo) {
			
				/* VFO A */
			case RIG_VFO_A:
				vfo = RIG_VFO_B;
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

				/* send an error report */
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: I can't figure out available VFOs (got %d)\n",
					   __FUNCTION__, get->vfo);

				vfo = RIG_VFO_NONE;
				break;
			}

			/* try to execute command */
			retcode = rig_set_freq (myrig, vfo, set->freq2);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_FREQ_2\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_FREQ_2);
			}
			else {
				/* reset flag */
				new->freq2 = FALSE;
			}

		}
		
		/* if rig doesn't have get_freq, we set the get->freq
		   manually to make widgets happy
		*/
		if (!has_get->freq2) {
			get->freq2 = set->freq2;
		}

		break;
	
		/* get RIT offset */
	case RIG_CMD_GET_RIT:

		/* check whether command is available */
		if (has_get->rit) {
			shortfreq_t rit;

			/* try to execute command */
			retcode = rig_get_rit (myrig, RIG_VFO_CURR, &rit);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_RIT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_RIT);
			}
			else {
				get->rit = rit;
			}

		}

		break;

		/* set RIT offset */
	case RIG_CMD_SET_RIT:

		/* check whether command is available */
		if (has_set->rit && new->rit) {

			/* try to execute command */
			retcode = rig_set_rit (myrig, RIG_VFO_CURR, set->rit);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_RIT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_RIT);
			}
			else {

				/* reset flag */
				new->rit = FALSE;
			}	

		}

		/* if rig doesn't have get_rit we set it manually to
		   make widgets happy
		*/
		if (!has_get->rit) {
			get->rit = set->rit;
		}

		break;

		/* get XIT offset */
	case RIG_CMD_GET_XIT:

		/* check whether command is available */
		if (has_get->xit) {
			shortfreq_t xit;

			/* try to execute command */
			retcode = rig_get_xit (myrig, RIG_VFO_CURR, &xit);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_XIT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_XIT);
			}
			else {
				get->xit = xit;
			}

		}

		break;

		/* set XIT offset */
	case RIG_CMD_SET_XIT:

		/* check whether command is available */
		if (has_set->xit && new->xit) {

			/* try to execute command */
			retcode = rig_set_xit (myrig, RIG_VFO_CURR, set->xit);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_XIT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_XIT);
			}
			else {

				/* reset flag */
				new->xit = FALSE;
			}	

		}

		/* if rig doesn't have get_xit we set it manually to
		   make widgets happy
		*/
		if (!has_get->xit) {
			get->xit = set->xit;
		}

		break;

		/* get current VFO */
	case RIG_CMD_GET_VFO:

		/* check whether command is available */
		if (has_get->vfo) {
			vfo_t vfo;

			/* try to execute command */
			retcode = rig_get_vfo (myrig, &vfo);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_VFO\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_VFO);
			}
			else {
				get->vfo = vfo;
			}
		}

		break;

		/* set current VFO */
	case RIG_CMD_SET_VFO:

		/* check whether command is available */
		if (has_set->vfo && new->vfo) {

			/* try to execute command */
			retcode = rig_set_vfo (myrig, set->vfo);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_VFO\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_VFO);
			}
			else {

				/* reset flag */
				new->vfo = FALSE;
			}	
		}

		/* if rig doesn't have get_vfo we set it manually to
		   make widgets happy.
		*/
		if (!has_get->vfo) {
			get->vfo = set->vfo;
		}

		break;

		/* get power status */
	case RIG_CMD_GET_PSTAT:

		/* check whether command is available */
		if (has_get->pstat) {
			powerstat_t pstat;

			/* try to execute command */
			retcode = rig_get_powerstat (myrig, &pstat);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_PSTAT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_PSTAT);
			}
			else {
				get->pstat = pstat;
			}
		}

		break;

		/* set power status */
	case RIG_CMD_SET_PSTAT:

		/* check whether command is available */
		if (has_set->pstat && new->pstat) {

			/* try to execute command */
			retcode = rig_set_powerstat (myrig, set->pstat);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_PSTAT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_PSTAT);
			}
			else {

				/* reset flag */
				new->pstat = FALSE;
			}	
		}

		/* if rig doesn't have get_pstat we set it manually to
		   make widgets happy.
		*/
		if (!has_get->pstat) {
			get->pstat = set->pstat;
		}

		break;

		/* get PTT status */
	case RIG_CMD_GET_PTT:

		/* check whether command is available */
		if (has_get->ptt) {
			ptt_t ptt;

			/* try to execute command */
			retcode = rig_get_ptt (myrig, RIG_VFO_CURR, &ptt);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_PTT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_PTT);
			}
			else {
				get->ptt = ptt;
			}
		}

		break;

		/* set PTT status */
	case RIG_CMD_SET_PTT:

		/* check whether command is available */
		if (has_set->ptt && new->ptt) {

			/* try to execute command */
			retcode = rig_set_ptt (myrig, RIG_VFO_CURR, set->ptt);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_PTT\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_PTT);
			}
			else {

				/* reset flag */
				new->ptt = FALSE;
			}	
		}

		/* if rig doesn't have get_ptt we set it manually to
		   make widgets happy.
		*/
		if (!has_get->ptt) {
			get->ptt = set->ptt;
		}

		break;

		/* get current mode and passband width */
	case RIG_CMD_GET_MODE:

		/* check whether command is available */
		if (has_get->mode || has_get->pbw) {
			rmode_t   mode;
			pbwidth_t pbw;

			/* try to execute command */
			retcode = rig_get_mode (myrig, RIG_VFO_CURR, &mode, &pbw);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_MODE\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_MODE);
			}
			else {
				get->mode = mode;
				get->pbw  = pbw;
			}
		}

		break;

		/* set current mode and passband width */
	case RIG_CMD_SET_MODE:

		/* check whether command is available */
		if ((has_set->mode && new->mode) || (has_set->pbw  && new->pbw)) {

			/* try to execute command */
			retcode = rig_set_mode (myrig, RIG_VFO_CURR, set->mode, set->pbw);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_MODE\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_MODE);
			}
			else {
				new->mode = FALSE;
				new->pbw  = FALSE;
			}
		}

		/* if rig doesn't have get_mode we set it manually to
		   make widgets happy.
		*/
		if (!has_get->mode) {
			get->mode = set->mode;
			get->pbw  = set->pbw;
		}

		break;

		/* get AGC level */
	case RIG_CMD_GET_AGC:

		/* check whether command is available */
		if (has_get->agc) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_AGC, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_AGC\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_AGC);
			}
			else {
				get->agc = val.i;
			}
		}

		break;

		/* set AGC level */
	case RIG_CMD_SET_AGC:

		/* check whether command is available */
		if (has_set->agc && new->agc) {
			value_t val;

			val.i = set->agc;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_AGC, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_SET_AGC\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_SET_AGC);
			}
			else {

				/* reset flag */
				new->agc = FALSE;
			}	
		}

		/* if rig doesn't have get_ptt we set it manually to
		   make widgets happy.
		*/
		if (!has_get->agc) {
			get->agc = set->agc;
		}
		break;


		/* get signal strength, S-meter */
	case RIG_CMD_GET_STRENGTH:

		/* check whether command is available */
		if (has_get->strength) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_STRENGTH, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_STRENGTH\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_STRENGTH);
			}
			else {
				get->strength = val.i;
			}
		}

		break;

		/* get transmitter power */
	case RIG_CMD_GET_PWR:

		/* check whether command is available */
		if (has_get->power) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				rig_debug (RIG_DEBUG_ERR,
					   "*** GRIG: %s: Failed to execute RIG_CMD_GET_PWR\n",
					   __FUNCTION__);

				rig_anomaly_raise (RIG_CMD_GET_PWR);
			}
			else {
				get->power = val.f;
			}
		}

		break;

		/* bug in grig! */
	default:
		rig_debug (RIG_DEBUG_BUG,
			   "*** GRIG: %s: Unknown command %d (grig bug)\n",
			   __FUNCTION__, cmd);
		break;

	}


}
