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

/** \file rig-daemon.c
 *  \ingroup rigd
 *  \brief Radio control interface to hamlib.
 *
 * This object is responsible for interfacing the Hamradio Control Libraries
 * (hamlib).
 *
 * After initialization of the radio it starts a cyclic thread which will
 * execute some pre-defined commands. Because some manufacturers discourage
 * agressive polling while in TX mode, the daemon will only acquire very
 * few things while in this mode.
 *
 * More about cycles and periods...
 *
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include <string.h>
#include <stdlib.h>
#include "grig-config.h"
#include "grig-debug.h"
#include "rig-anomaly.h"
#include "rig-data.h"
#include "rig-gui-smeter.h"
#include "rig-daemon-check.h"
#include "rig-daemon.h"



RIG *myrig;  /*!< The rig structure. We keep this public so GUI can access the info fields. */

//#define GRIG_DEBUG 1

#ifdef GRIG_DEBUG


static const rig_cmd_t DEF_RX_CYCLE[C_MAX_CMD_PER_CYCLE] = {
	RIG_CMD_VFO_TOGGLE,
	RIG_CMD_VFO_COPY,
	RIG_CMD_VFO_XCHG,
	RIG_CMD_SET_SPLIT,
	RIG_CMD_GET_SPLIT,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_NONE
};

#else

/** \brief Matrix defining the default RX cycle.
 *
 * More description of the idea.
 *
 */
static const rig_cmd_t DEF_RX_CYCLE[C_MAX_CMD_PER_CYCLE] = {
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_PSTAT,
	RIG_CMD_GET_PSTAT,
	RIG_CMD_SET_KEYSPD,
	RIG_CMD_GET_KEYSPD,
	RIG_CMD_SET_LOCK,
	RIG_CMD_SET_AF,
	RIG_CMD_GET_AF,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_BKINDEL,
	RIG_CMD_GET_BKINDEL,
	RIG_CMD_SET_ATT,
	RIG_CMD_GET_ATT,
	RIG_CMD_SET_RIT,
	RIG_CMD_GET_RIT,
	RIG_CMD_SET_RF,
	RIG_CMD_GET_RF,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_GET_LOCK,
	RIG_CMD_SET_VFO,
	RIG_CMD_GET_VFO,
	RIG_CMD_SET_VOXDEL,
	RIG_CMD_GET_VOXDEL,
	RIG_CMD_SET_PTT,
	RIG_CMD_GET_PTT,
	RIG_CMD_SET_IFS,
	RIG_CMD_GET_IFS,
	RIG_CMD_SET_AF,
	RIG_CMD_GET_AF,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_VOXGAIN,
	RIG_CMD_GET_VOXGAIN,
	RIG_CMD_SET_AGC,
	RIG_CMD_GET_AGC,
	RIG_CMD_SET_SQL,
	RIG_CMD_GET_SQL,
	RIG_CMD_VFO_TOGGLE,
	RIG_CMD_VFO_COPY,
	RIG_CMD_VFO_XCHG,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_ANTIVOX,
	RIG_CMD_GET_ANTIVOX,
	RIG_CMD_SET_PBT_IN,
	RIG_CMD_GET_PBT_IN,
	RIG_CMD_SET_PBT_OUT,
	RIG_CMD_GET_PBT_OUT,
	RIG_CMD_SET_AF,
	RIG_CMD_GET_AF,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_CW_PITCH,
	RIG_CMD_GET_CW_PITCH,
	RIG_CMD_SET_PREAMP,
	RIG_CMD_GET_PREAMP,
	RIG_CMD_SET_SPLIT,
	RIG_CMD_GET_SPLIT,
	RIG_CMD_SET_MICGAIN,
	RIG_CMD_GET_MICGAIN,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_MODE,
	RIG_CMD_GET_MODE,
	RIG_CMD_SET_BALANCE,
	RIG_CMD_GET_BALANCE,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_VFO,
	RIG_CMD_GET_VFO,
	RIG_CMD_SET_PTT,
	RIG_CMD_GET_PTT,
	RIG_CMD_SET_COMP,
	RIG_CMD_GET_COMP,
	RIG_CMD_VFO_TOGGLE,
	RIG_CMD_VFO_COPY,
	RIG_CMD_VFO_XCHG,
	RIG_CMD_SET_AF,
	RIG_CMD_GET_AF,
	RIG_CMD_GET_STRENGTH,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_SET_APF,
	RIG_CMD_GET_APF,
	RIG_CMD_SET_NR,
	RIG_CMD_GET_NR,
	RIG_CMD_SET_NOTCH,
	RIG_CMD_GET_NOTCH,
	RIG_CMD_SET_FUNC,
	RIG_CMD_GET_FUNC
};

#endif


/** \brief Matrix defining the default TX cycle.
 *
 * More description.
 *
 * \note Some radios do not like being polled while in TX mode so
 *       we make TX cycle easy...
 *
 */
static const rig_cmd_t DEF_TX_CYCLE[C_MAX_CMD_PER_CYCLE] = {
	RIG_CMD_SET_PTT,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_SET_POWER,
	RIG_CMD_GET_POWER,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_SWR,
	RIG_CMD_GET_ALC,
	RIG_CMD_NONE,
	RIG_CMD_SET_LOCK,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_PTT,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_POWER,
	RIG_CMD_GET_POWER,
	RIG_CMD_GET_SWR,
	RIG_CMD_GET_PTT,
	RIG_CMD_NONE,
	RIG_CMD_SET_ALC,
	RIG_CMD_GET_ALC,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_POWER,
	RIG_CMD_GET_PTT,
	RIG_CMD_NONE,
	RIG_CMD_GET_POWER,
	RIG_CMD_GET_SWR,
	RIG_CMD_GET_ALC,
	RIG_CMD_NONE,
	RIG_CMD_SET_PTT,
	RIG_CMD_NONE,
	RIG_CMD_GET_ALC,
	RIG_CMD_GET_POWER,
	RIG_CMD_GET_SWR,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_ALC,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_PTT,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_ALC,
	RIG_CMD_GET_POWER,
	RIG_CMD_GET_SWR,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_MICGAIN,
	RIG_CMD_GET_MICGAIN,
	RIG_CMD_GET_PTT,
	RIG_CMD_GET_POWER,
	RIG_CMD_GET_SWR,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_FREQ_1,
	RIG_CMD_GET_PTT,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_POWER,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_PTT,
	RIG_CMD_GET_SWR,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_ALC,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_PTT,
	RIG_CMD_SET_POWER,
	RIG_CMD_NONE,
	RIG_CMD_GET_FREQ_1,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_PTT,
	RIG_CMD_GET_POWER,
	RIG_CMD_GET_SWR,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_GET_ALC,
	RIG_CMD_GET_PTT,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_MICGAIN,
	RIG_CMD_GET_MICGAIN,
	RIG_CMD_GET_PTT,
	RIG_CMD_GET_POWER,
	RIG_CMD_GET_SWR,
	RIG_CMD_NONE,
	RIG_CMD_NONE,
	RIG_CMD_SET_COMP,
	RIG_CMD_GET_COMP
};


/** \brief Conversion table to convert rig error to string */
static const gchar *ERR_TO_STR[] = {
	N_("No error"),
	N_("Invalid parameter"),
	N_("Invalid configuration"),
	N_("Memory shortage"),
	N_("Function not implemented"),
	N_("Communication timed out"),
	N_("I/O error"),
	N_("Internal Hamlib error :-("),
	N_("Protocol error"),
	N_("Command rejected"),
	N_("Command performed, but arg truncated"),
	N_("Function not available"),
	N_("VFO not targetable"),
	N_("BUS error"),
	N_("Collision on the bus"),
	N_("NULL RIG handle or invalid pointer param"),
	N_("Invalid VFO"),
	N_("Argument out of domain")
};


static gboolean stopdaemon   = FALSE;   /*!< Used to signal the daemon thread that it should stop */
static gboolean daemonclear  = FALSE;   /*!< Used to signal back when daemon is finished */
static gint     cmd_delay    = 0;       /*!< Delay between two RX commands TX = 3*RX */
static gint     timeoutid    = -1;      /*!< The ID of the timeout callback when we don't use threads. */
static gboolean timeout_busy = FALSE;   /*!< Flag used to avoid to callbacks at the same time. */
static gboolean suspended    = FALSE;   /*!< Flag indicating whether the daemon is susended or not. */

/* private function prototypes */
static void     rig_daemon_post_init (gboolean, gboolean);
static gpointer rig_daemon_cycle     (gpointer);
static gint     rig_daemon_cycle_cb  (gpointer);
static gint     rig_daemon_exec_cmd  (rig_cmd_t,
				      grig_settings_t  *,
				      grig_settings_t  *,
				      grig_cmd_avail_t *,
				      grig_cmd_avail_t *,
				      grig_cmd_avail_t *);



/** \brief Start radio control daemon.
 *  \param rignum   The Hamlib ID of the radio (0 to use default).
 *  \param port     The port device (NULL to use default).
 *  \param speed    The serial speed (0 to use default).
 *  \param civaddr  CIV address for ICOM rigs (NULL means no need to set conf).
 *  \param rigconf  Additional config options necessary for some rigs.
 *  \param cmddel   Delay between two RX commands.
 *  \param nothread Whether to use threads (FALSE) or just a timeout callback.
 *  \return 0 if the daemon has been initialized correctly.
 *
 * This function initializes the radio and starts the control daemon. The rignum
 * parameter is the rig ID in hamlib.
 *
 * The \a rigconf parameter contains one or more configuration options that are
 * necessary for some rigs. The syntax is param=value and if more than one config
 * options are specified, they are separated by comma.
 *
 * \note The default port is /dev/ttyS0 for regular backends and localhost for RPC rig.
 *
 */
int
rig_daemon_start       (int          rigid,
			const gchar *port,
			int          speed,
			const gchar *civaddr,
			const gchar *rigconf,
			gint         cmddel,
			gboolean     nothread,
			gboolean     ptt,
			gboolean     pstat)
{

	gchar  *rigport;
	gint    retcode;
	gchar **confvec;   
	gchar **confent;
	GError *err = NULL;  /* used when starting daemon thread */
#if GLIB_CHECK_VERSION(2,32,0)
  GThread* thread = NULL;
#endif


	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s entered"),
			  __FUNCTION__);

	/* in order to be sure that we have a sensible command delay
	   we set it already here
	*/
	if (cmddel > 0) {
		cmd_delay = cmddel;
	}
	else {
		cmd_delay = C_DEF_RX_CMD_DELAY;
	}


	/* check if rig is already initialized */
	if (myrig != NULL) {
		return 1;
	}

	/* use dummy backend if no ID pecified */
	if (!rigid) {
		rigid = 1;
	}

	/* use default port, if none specified; localhost for RPC rig
	   first serial port otherwise.
	*/
	if (port == NULL) {

		if (rigid == 1901) {
			rigport = g_strdup ("localhost");
		}
		else {
			rigport = g_strdup ("/dev/ttyS0");
		}
	}
	else {
		rigport = g_strdup (port);
	}


	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s: Initializing rig (id=%d)"),
			  __FUNCTION__, rigid);

	/* initilize rig */
	myrig = rig_init (rigid);

	if (myrig == NULL) {

		grig_debug_local (RIG_DEBUG_ERR,
				  _("%s: Init failed; Hamlib returned NULL!"),
				  __FUNCTION__);

		g_free (rigport);
		return 1;
	}

	/* configure and open rig device */
	strncpy (myrig->state.rigport.pathname, rigport, FILPATHLEN);
	g_free (rigport);

	/* set speed if any special whishes */
	if (speed) {
		myrig->state.rigport.parm.serial.rate = speed;
	}

	if (civaddr) {
		retcode = rig_set_conf (myrig, rig_token_lookup (myrig, "civaddr"), civaddr);
	}

	/* split conf parameter string; */
	if (rigconf) {
		guint i = 0;

		confvec = g_strsplit (rigconf, ",", 0);

		/* split each conf entity into param and val
		   and set conf
		*/
		while (confvec[i]) {

			confent = g_strsplit (confvec[i], "=", 2);

			grig_debug_local (RIG_DEBUG_VERBOSE,
					  _("%s: Setting conf param (%s,%s)..."),
					  __FUNCTION__, confent[0], confent[1]);

			retcode = rig_set_conf (myrig,
						rig_token_lookup (myrig, confent[0]),
						confent[1]);

			if (retcode == RIG_OK) {
				grig_debug_local (RIG_DEBUG_VERBOSE,
						  _("%s: Set conf OK"),
						  __FUNCTION__);
			}
			else {
				grig_debug_local (RIG_DEBUG_VERBOSE,
						  _("%s: Set conf failed (%d)"),
						  __FUNCTION__, retcode);
			}
					   
			i++;

			g_strfreev (confent);
		}

		g_strfreev (confvec);
	}


#ifndef DISABLE_HW
	/* open rig */
	retcode = rig_open (myrig);
	if (retcode != RIG_OK) {

		/* send error report */
		grig_debug_local (RIG_DEBUG_ERR,
				  _("%s: Failed to open rig port %s: %s (permissions?)"),
				  __FUNCTION__,
				  myrig->state.rigport.pathname,
				  rigerror(retcode));

		rig_cleanup (myrig);
		return 1;
	}
#endif

	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s: Init successfull, executing post-init"),
			  __FUNCTION__);

	/* get capabilities and settings  */
	rig_daemon_post_init (ptt, pstat);

	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s: Starting rig daemon"),
			  __FUNCTION__);

#ifndef DISABLE_HW

	/* if nothread flag is TRUE start a usual timeout, otherwise
	   fork a separate thread.
	*/
	if (nothread == TRUE) {

		/* we start a regular g_timeout;
		   we use C_MAX_CYCLES * C_MAX_CMD_PER_CYCLE * cmd_delay
		   for delay.
		*/
		timeoutid = g_timeout_add (2 * C_MAX_CYCLES * C_MAX_CMD_PER_CYCLE * cmd_delay,
					   rig_daemon_cycle_cb,
					   NULL);

		grig_debug_local (RIG_DEBUG_VERBOSE,
				  _("%s: Daemon timeout started, ID: %d"),
				  __FUNCTION__, timeoutid);

	}
	else {
#if !GLIB_CHECK_VERSION(2,32,0)
		g_thread_create (rig_daemon_cycle, NULL, FALSE, &err);
#else
    thread = g_thread_try_new ("daemon thread", rig_daemon_cycle, NULL, &err);
    if (thread != NULL) {
      g_thread_unref(thread);
    }
#endif

		/* check whether any error occurred when starting the daemon
		   thread; if yes, close rig and return with error code
		   (assuming that err->code will be non-zero)
		*/
		if (err != NULL) {

			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Failed to start daemon thread"),
					  __FUNCTION__);
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Error %d: %s"),
					    __FUNCTION__, err->code, err->message);

			rig_close (myrig);
			rig_cleanup (myrig);

			return err->code;
		}
		else {
			grig_debug_local (RIG_DEBUG_VERBOSE,
					  _("%s: Daemon thread started"),
					  __FUNCTION__);

		}
	}

#endif

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
	guint i = 0;


	/* send a debug message */
	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s: Sending stop signal to rig daemon"),
			  __FUNCTION__);


	/* if we are running in time-out mode
	   we can remove the callback directly here;
	   otherwise, send stop signal to daemon and
	   wait until 'daemonclear' flag is TRUE or
	   we time out (in case of time out we also send
	   and error message
	*/
	if (timeoutid != -1) {
		g_source_remove (timeoutid);
	}
	else {
		stopdaemon = TRUE;

		/* wait until flag is clear or we time out */
		while ((daemonclear == FALSE) &&
		       (i*C_RIG_DAEMON_STOP_SLEEP_TIME < C_RIG_DAEMON_STOP_TIMEOUT)) {

			i++;
			g_usleep (C_RIG_DAEMON_STOP_SLEEP_TIME * 1000);
		}

		/* print an error message if the flag has not been cleared */
		if (daemonclear == FALSE) {
			g_print ("\n\nCRITICAL: Daemon process has not been shut down properly. "\
				 "You may have a zombie hanging around :-(\n\n");
		}
	}

	/* send a debug message */
	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s: Cleaning up rig"),
			  __FUNCTION__);

#ifndef DISABLE_HW
	/* close radio device */
	rig_close (myrig);
#endif

	/* clean up hamlib */
	rig_cleanup (myrig);

	myrig = NULL;
}


/** \brief Execute post initialization tasks.
 *  \param ptt Flag indicating whether to enable PTT.
 *  \param pstat Flag indicting whether to enable POWER.
 *
 * This function executes some tasks after initialization of the radio
 * hardware. These include testing the radio capabilities and obtaining
 * the current settings. The test results are communicated to the user
 * via the rig_debug() Hamlib function.
 *
 */
static void
rig_daemon_post_init (gboolean ptt, gboolean pstat)
{
	grig_settings_t  *get;        /* pointer to shared data 'get' */
	grig_settings_t  *set;        /* pointer to shared data 'set' */
	grig_cmd_avail_t *has_get;    /* pointer to shared data 'has_get' */
	grig_cmd_avail_t *has_set;    /* pointer to shared data 'has_set' */


	/* get pointers to shared data */
	get     = rig_data_get_get_addr ();
	set     = rig_data_get_set_addr ();
	has_get = rig_data_get_has_get_addr ();
	has_set = rig_data_get_has_set_addr ();


	/* check command availabilities */
	if (pstat == TRUE) {
		rig_daemon_check_pwrstat (myrig, get, has_get, has_set);
	}
	else {
		has_get->pstat = FALSE;
		has_set->pstat = FALSE;
		get->pstat = RIG_POWER_ON;
		set->pstat = RIG_POWER_ON;
	}

	if (ptt == TRUE) {
		rig_daemon_check_ptt     (myrig, get, has_get, has_set);
	}
	else {
		has_get->ptt = FALSE;
		has_set->ptt = FALSE;
		get->ptt = RIG_PTT_OFF;
		set->ptt = RIG_PTT_OFF;
	}

	rig_daemon_check_vfo     (myrig, get, has_get, has_set);
	rig_daemon_check_freq    (myrig, get, has_get, has_set);
	rig_daemon_check_rit     (myrig, get, has_get, has_set);
	rig_daemon_check_xit     (myrig, get, has_get, has_set);
	rig_daemon_check_mode    (myrig, get, has_get, has_set);
	rig_daemon_check_level   (myrig, get, has_get, has_set);
	rig_daemon_check_func    (myrig, get, has_get, has_set);

	/* debug info about detected has-get caps */
	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s: GET bits: %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"),
			  __FUNCTION__,
			  has_get->pstat,
			  has_get->ptt,
			  has_get->att,
			  has_get->preamp,
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

	/*** FIXME: FUNC **/

	/* debug info about detected has-set caps */
	grig_debug_local (RIG_DEBUG_TRACE,
			  _("%s: SET bits: %d%d%d%d%d%d%d%d%d%d%d%d%dXXX"),
			  __FUNCTION__,
			  has_set->pstat,
			  has_set->ptt,
			  has_set->att,
			  has_set->preamp,
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
		   has_get->alc
*/
		);

}



/** \brief Radio control daemon main cycle (threaded version).
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

	guint step;    /* step counter */


	/* get pointers to shared data */
	get     = rig_data_get_get_addr ();
	set     = rig_data_get_set_addr ();
	new     = rig_data_get_new_addr ();
	has_get = rig_data_get_has_get_addr ();
	has_set = rig_data_get_has_set_addr ();

	
	/* send a debug message */
	grig_debug_local (RIG_DEBUG_TRACE, _("%s started."), __FUNCTION__);

	/* loop forever until reception of STOP signal */
	while (stopdaemon == FALSE) {

		/* first we check whether rig is powered ON since some rigs
		   will not talk to us in power-off tate.
		   NOTE: code should be safe even if rig does not support
		         get_powerstat since get->pstat is set to ON if rig
			 does not have functionality.
		*/
		if (get->pstat == RIG_POWER_ON) {

			/* execute one cylce; note that the switch between the
			   RX and TX tables can happen within a cycle :-)
			*/
			for (step = 0; step < C_MAX_CMD_PER_CYCLE; step++) {
				
				/* only execute commands if the daemon is not
				   suspended.
				*/
				if (!suspended) {

					/* check whether we are in RX or TX mode; */
					if (get->ptt == RIG_PTT_OFF) {

						/* Execute a receiver command */
						rig_daemon_exec_cmd (DEF_RX_CYCLE[step],
								     get,
								     set,
								     new,
								     has_get,
								     has_set);
/* slow motion in debug mode */
#ifdef GRIG_DEBUG
						g_usleep (5000 * cmd_delay);
#else
						g_usleep (1000 * cmd_delay);
#endif
					}
					else {

						/* Execute transmitter command */
						rig_daemon_exec_cmd (DEF_TX_CYCLE[step],
								     get,
								     set,
								     new,
								     has_get,
								     has_set);
/* slow motion in debug mode */
#ifdef GRIG_DEBUG
						g_usleep (15000 * cmd_delay);
#else
						g_usleep (3000 * cmd_delay);
#endif
					}
				}

			}

		}

		/* otherwise check the power status, but only if daemon
		   is not suspended */
		else {
			if (!suspended) {
				rig_daemon_exec_cmd (RIG_CMD_SET_PSTAT,
						     get, set, new,
						     has_get, has_set);
/* slow motion in debug mode */

/* If the rig has been powered OFF, we only execute set_pstat in order
   to be compatible with as many rigs as possible. Even like this, there
   is no guarantee that the rig will react to a wake-up command.
*/

/* #ifdef GRIG_DEBUG */
/* 				g_usleep (15000 * cmd_delay); */
/* #else */
/* 				g_usleep (3000 * cmd_delay); */
/* #endif */
/* 				rig_daemon_exec_cmd (RIG_CMD_GET_PSTAT, */
/* 						     get, set, new, */
/* 						     has_get, has_set); */
			}

/* slow motion in debug mode */
#ifdef GRIG_DEBUG
			g_usleep (15000 * cmd_delay);
#else
			g_usleep (3000 * cmd_delay);
#endif

		}

	}

	/* send a debug message */
	grig_debug_local (RIG_DEBUG_TRACE, _("%s stopped"), __FUNCTION__);

	/* set clear flag to indicate that daemon terminated */
	daemonclear = TRUE;

	return NULL;
}


/** \brief Radio control daemon main cycle (callback version).
 *  \param data Unused.
 *  \return Always TRUE.
 *
 * This function implements the main cycle of the radio control daemon. The executed
 * commands are defined in the DEF_RX_CYCLE and DEF_TX_CYCLE constant arrays.
 */
static gint
rig_daemon_cycle_cb  (gpointer data)
{

	grig_settings_t  *get;             /* pointer to shared data 'get' */
	grig_settings_t  *set;             /* pointer to shared data 'set' */
	grig_cmd_avail_t *new;             /* pointer to shared data 'new' */
	grig_cmd_avail_t *has_get;         /* pointer to shared data 'has_get' */
	grig_cmd_avail_t *has_set;         /* pointer to shared data 'has_set' */

	guint step;        /* step counter */

	/* check whether the previous callback has terminated.
	   if not, skip this cycle.
	*/
	if (timeout_busy == TRUE) {
		return TRUE;
	}

	timeout_busy = TRUE;

	/* get pointers to shared data */
	get     = rig_data_get_get_addr ();
	set     = rig_data_get_set_addr ();
	new     = rig_data_get_new_addr ();
	has_get = rig_data_get_has_get_addr ();
	has_set = rig_data_get_has_set_addr ();


	/* send a debug message */
	grig_debug_local (RIG_DEBUG_TRACE, _("%s called."), __FUNCTION__);


	/* first we check whether rig is powered ON since some rigs
	   will not talk to us in power-off state.
	   NOTE: code should be safe even if rig does not support
	   get_powerstat since get->pstat is set to ON if rig
	   does not have functionality.
	*/
	if (get->pstat == RIG_POWER_ON) {

		for (step = 0; step < C_MAX_CMD_PER_CYCLE; step++) {

			/* check whether we are in RX or TX mode; */
			if (get->ptt == RIG_PTT_OFF) {

				/* Execute receiver command;
				   sleep for cmd_delay ms if command has been executed
				*/
				if (rig_daemon_exec_cmd (DEF_RX_CYCLE[step],
							 get,
							 set,
							 new,
							 has_get,
							 has_set)) {
/* slow motion in debug mode */
#ifdef GRIG_DEBUG
					g_usleep (5000 * cmd_delay);
#else
					g_usleep (1000 * cmd_delay);
#endif
				}

			}
			else {
				
				/* Execute transmitter command;
				   sleep for cmd_delay ms if command has been executed
				*/
				if (rig_daemon_exec_cmd (DEF_TX_CYCLE[step],
							 get,
							 set,
							 new,
							 has_get,
							 has_set)) {
/* slow motion in debug mode */
#ifdef GRIG_DEBUG
					g_usleep (10000 * cmd_delay);
#else
					g_usleep (2000 * cmd_delay);
#endif
				}
			}
		}

	}

	/* otherwise check the power status only */
	else {
		if (rig_daemon_exec_cmd (RIG_CMD_SET_PSTAT, get, set, new, has_get, has_set)) {

/* slow motion in debug mode */
#ifdef GRIG_DEBUG
			g_usleep (15000 * cmd_delay);
#else
			g_usleep (3000 * cmd_delay);
#endif
		}

		rig_daemon_exec_cmd (RIG_CMD_GET_PSTAT, get, set, new, has_get, has_set);
	}


	timeout_busy = FALSE;


	return TRUE;
}




/** \brief Execute a specific command.
 *  \param cmd The command to be executed.
 *  \param get Pointer to the 'get' command buffer.
 *  \param set Pointer to the 'set' command buffer.
 *  \param new Pointer to the 'new' command buffer.
 *  \param has_get Pointer to get capabilities record.
 *  \param has_set Pointer to set capabilities record.
 *  \return 1 if the command has been executed, 0 otherwise.
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
static gint
rig_daemon_exec_cmd         (rig_cmd_t cmd,
			     grig_settings_t  *get,
			     grig_settings_t  *set,
			     grig_cmd_avail_t *new,
			     grig_cmd_avail_t *has_get,
			     grig_cmd_avail_t *has_set)

{
	int  retcode;
	gint status = 0;
	setting_t func;
	int i;


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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_FREQ_1:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_FREQ_1);
			}
			else {
				get->freq1 = freq;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_FREQ_1:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_FREQ_1);
			}

                        /* reset flag */
			new->freq1 = FALSE;
			get->freq1 = set->freq1;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: I can't figure out available VFOs (got %d)"),
						  __FUNCTION__, get->vfo);

				vfo = RIG_VFO_NONE;
				break;
			}

			/* try to execute command */
			retcode = rig_get_freq (myrig, vfo, &freq);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_FREQ_2:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_FREQ_2);
			}
			else {
				get->freq2 = freq;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: I can't figure out available VFOs (got %d)"),
						    __FUNCTION__, get->vfo);

				vfo = RIG_VFO_NONE;
				break;
			}

			/* try to execute command */
			retcode = rig_set_freq (myrig, vfo, set->freq2);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_FREQ_2:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_FREQ_2);
			}

			/* reset flag */
			new->freq2 = FALSE;
			get->freq2 = set->freq2;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_RIT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_RIT);
			}
			else {
				get->rit = rit;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_RIT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_RIT);
			}

			/* reset flag */
			new->rit = FALSE;
			get->rit = set->rit;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_XIT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_XIT);
			}
			else {
				get->xit = xit;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_XIT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_XIT);
			}

			/* reset flag */
			new->xit = FALSE;
			get->xit = set->xit;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_VFO:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_VFO);
			}
			else {
				get->vfo = vfo;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_VFO:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_VFO);
			}

			/* reset flag */
			new->vfo = FALSE;
			get->vfo = set->vfo;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_PSTAT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_PSTAT);
			}
			else {
				get->pstat = pstat;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_PSTAT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_PSTAT);
			}

			/* reset flag */
			new->pstat = FALSE;
			get->pstat = set->pstat;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_PTT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_PTT);
			}
			else {
				get->ptt = ptt;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_PTT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_PTT);
			}

			/* reset flag */
			new->ptt = FALSE;
			get->ptt = set->ptt;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_MODE:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_MODE);
			}
			else {
				int i = 0;           /* iterator */
				int found_mode = 0;  /* flag to indicate found mode */

				/* convert and store the new passband width;
				   note: RIG_PASSBAND_NORMAL = 0, which is also
				   the value returned by rig_passband_wide and
				   rig_passband_narrow if these passbands are not
				   defined in the backend.
				*/
				if ((pbw == rig_passband_wide (myrig, mode)) &&
				    (pbw > 0)) {
					get->pbw = RIG_DATA_PB_WIDE;
				}
				else if ((pbw == rig_passband_narrow (myrig, mode)) &&
					 (pbw > 0)) {
					get->pbw  = RIG_DATA_PB_NARROW;
				}
				else {
					get->pbw  = RIG_DATA_PB_NORMAL;
				}

				/* if mode has changed we need to update frequency limits */
				if (get->mode != mode) {

					get->mode = mode;

					/* FIXME: VY SIMILAR CODE IS PRESENT IN RIG_DAEMON_CHECK_MODE */

					/* get frequency limits for this mode; we use the rx_range_list
					   stored in the rig_state structure
					*/
					while (!RIG_IS_FRNG_END(myrig->state.rx_range_list[i]) && !found_mode) {
						
						/* is this list good for current mode?
						   is the current frequency within this range?
						*/
						if (((mode & myrig->state.rx_range_list[i].modes) == mode) &&
						    (get->freq1 >= myrig->state.rx_range_list[i].start)    &&
						    (get->freq1 <= myrig->state.rx_range_list[i].end)) {
					
							found_mode = 1;
							get->fmin = myrig->state.rx_range_list[i].start;
							get->fmax = myrig->state.rx_range_list[i].end;
				
							grig_debug_local (RIG_DEBUG_VERBOSE,
									  _("%s: Found frequency range for mode %d"),
									  __FUNCTION__, mode);
							grig_debug_local (RIG_DEBUG_VERBOSE,
									  _("%s: %.0f...(%.0f)...%.0f kHz"),
									  __FUNCTION__,
									  get->fmin / 1.0e3,
									  get->freq1 / 1.0e3,
									  get->fmax / 1.0e3);

						}
						else {
							i++;
						}
						

					}

					/* if we did not find any suitable range there could be a bug
					   in the backend!
					*/
					if (!found_mode) {
						grig_debug_local (RIG_DEBUG_BUG,
								  _("%s: Can not find frequency range for this mode (%d)!"\
								    "Bug in backend?"),
								  __FUNCTION__, mode);
					}

					/* get the smallest tuning step */
					get->fstep = rig_get_resolution (myrig, mode);
				}
			}

			status = 1;
		}

		break;


		/* set current mode and passband width */
	case RIG_CMD_SET_MODE:

		/* check whether command is available */
		if ((has_set->mode && new->mode) || (has_set->pbw  && new->pbw)) {
			pbwidth_t pbw;
			rmode_t   mode;

			if (new->mode) {
				mode = set->mode;
			}
			else {
				mode = get->mode;
			}

			/* don't modify pbw unless asked by user */
			if (new->pbw) {
				switch (set->pbw) {
				case RIG_DATA_PB_WIDE:
					pbw = rig_passband_wide (myrig, mode);
					break;
				case RIG_DATA_PB_NORMAL:
					pbw = rig_passband_normal (myrig, mode);
					break;
				case RIG_DATA_PB_NARROW:
					pbw = rig_passband_narrow (myrig, mode);
					break;
				default:
					/* we have no idea what to set! */
					pbw = rig_passband_normal (myrig, mode);
					break;
				}
			}
			else {
				switch (get->pbw) {
				case RIG_DATA_PB_WIDE:
					pbw = rig_passband_wide (myrig, mode);
					break;
				case RIG_DATA_PB_NORMAL:
					pbw = rig_passband_normal (myrig, mode);
					break;
				case RIG_DATA_PB_NARROW:
					pbw = rig_passband_narrow (myrig, mode);
					break;
				default:
					/* we have no idea what to set! */
					pbw = rig_passband_normal (myrig, mode);
					break;
				}
			}

			/* try to execute command */
			retcode = rig_set_mode (myrig, RIG_VFO_CURR, set->mode, pbw);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_MODE:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_MODE);
			}

			if (new->mode) {
				get->mode = set->mode;
				new->mode = FALSE;
			}
			if (new->pbw) {
				get->pbw  = set->pbw;
				new->pbw  = FALSE;
			}
			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_AGC:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_AGC);
			}
			else {
				get->agc = val.i;
			}

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_AGC:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_AGC);
			}
			/* reset flag */
			new->agc = FALSE;
			get->agc = set->agc;

			status = 1;
		}

		break;


		/* get attenuator level */
	case RIG_CMD_GET_ATT:

		/* check whether command is available */
		if (has_get->att) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ATT, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_ATT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_ATT);
			}
			else {
				get->att = val.i;
			}
		}
		
		break;

			
		/* set attenuator level */
	case RIG_CMD_SET_ATT:

		/* check whether command is available */
		if (has_set->att && new->att) {
			value_t val;

			val.i = set->att;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ATT, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_ATT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_ATT);
			}
			/* reset flag */
			new->att = FALSE;
			get->att = set->att;

			status = 1;
		}

		break;


		/* get pre-amp level */
	case RIG_CMD_GET_PREAMP:

		/* check whether command is available */
		if (has_get->preamp) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_PREAMP, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_PREAMP:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_PREAMP);
			}
			else {
				get->preamp = val.i;
			}

			status = 1;
		}
		
		break;

			
		/* set pre-amp level */
	case RIG_CMD_SET_PREAMP:

		/* check whether command is available */
		if (has_set->preamp && new->preamp) {
			value_t val;

			val.i = set->preamp;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_PREAMP, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_PREAMP:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_PREAMP);
			}
			/* reset flag */
			new->preamp = FALSE;
			get->preamp = set->preamp;

			status = 1;
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
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_STRENGTH:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_STRENGTH);
			}
			else {
				get->strength = val.i;
			}

			status = 1;
		}

		break;

		/* set transmitter power */
	case RIG_CMD_SET_POWER:

		/* check whether command is available */
		if (has_set->power && new->power) {
			value_t val;

			val.f = set->power;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_POWER:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_POWER);
			}

			/* reset flag */
			new->power = FALSE;
			get->power = set->power;

			status = 1;
		}

		break;

		/* get transmitter power */
	case RIG_CMD_GET_POWER:

		/* check whether command is available */
		if (has_get->power && (rig_gui_smeter_get_tx_mode() == SMETER_TX_MODE_POWER)) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_POWER:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_POWER);
			}
			else {
				get->power = val.f;
			}

			status = 1;
		}

		break;

		/* get SWR */
	case RIG_CMD_GET_SWR:

		/* check whether command is available */
		if (has_get->swr && (rig_gui_smeter_get_tx_mode() == SMETER_TX_MODE_SWR)) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_SWR, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_SWR:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_SWR);
			}
			else {
				get->swr = val.f;
			}

			status = 1;
		}

		break;

		/* get ALC */
	case RIG_CMD_GET_ALC:

		/* check whether command is available */
		if (has_get->alc && (rig_gui_smeter_get_tx_mode() == SMETER_TX_MODE_ALC)) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ALC, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_ALC:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_ALC);
			}
			else {
				get->alc = val.f;
			}

			status = 1;
		}

		break;

		/* set ALC */
	case RIG_CMD_SET_ALC:

		/* check whether command is available */
		if (has_set->alc && new->alc) {
			value_t val;

			val.f = set->alc;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ALC, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_ALC:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_ALC);
			}

			/* reset flag */
			new->alc = FALSE;

			status = 1;
		}

		break;

		/* set LOCK status */
	case RIG_CMD_SET_LOCK:

		if (has_set->lock && new->lock) {
			retcode = rig_set_func (myrig,
						RIG_VFO_CURR,
						RIG_FUNC_LOCK,
						set->lock);

			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_LOCK:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_LOCK);
			}
			
			get->lock = set->lock;
			new->lock = 0;

			status = 1;
		}

		break;

		/* get LOCK status */
	case RIG_CMD_GET_LOCK:

		/* check whether command is available */
		if (has_get->lock) {
			int lock;

			/* try to execute command */
			retcode = rig_get_func (myrig, RIG_VFO_CURR, RIG_FUNC_LOCK, &lock);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_LOCK:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_LOCK);
			}
			else {
				get->lock = lock;
			}

			status = 1;
		}

		break;

		/* execute RIG_OP_TOGGLE */
	case RIG_CMD_VFO_TOGGLE:

		if (has_set->vfo_op_toggle && new->vfo_op_toggle) {

			retcode = rig_vfo_op (myrig, RIG_VFO_CURR, RIG_OP_TOGGLE);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_VFO_TOGGLE:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_VFO_TOGGLE);
			}

			new->vfo_op_toggle = 0;

			status = 1;
		}

		break;

		/* execute RIG_OP_COPY */
	case RIG_CMD_VFO_COPY:

		if (has_set->vfo_op_copy && new->vfo_op_copy) {

			retcode = rig_vfo_op (myrig, RIG_VFO_CURR, RIG_OP_CPY);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_VFO_COPY:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_VFO_COPY);
			}

			new->vfo_op_copy = 0;

			status = 1;
		}

		break;

		/* execute RIG_OP_XCHG */
	case RIG_CMD_VFO_XCHG:

		if (has_set->vfo_op_xchg && new->vfo_op_xchg) {

			retcode = rig_vfo_op (myrig, RIG_VFO_CURR, RIG_OP_XCHG);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_VFO_XCHG:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_VFO_XCHG);
			}

			new->vfo_op_xchg = 0;

			status = 1;
		}

		break;

		/* set split on or off */
	case RIG_CMD_SET_SPLIT:
		if (has_set->split && new->split) {

			retcode = rig_set_split_vfo (myrig, RIG_VFO_RX, set->split, RIG_VFO_TX);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_SPLIT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_SPLIT);
			}

			new->split = 0;

			status = 1;
		}

		break;

	case RIG_CMD_GET_SPLIT:
		if (has_get->split) {

			retcode = rig_get_split (myrig, RIG_VFO_RX, &get->split);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_SPLIT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_SPLIT);
			}

			status = 1;
		}

		break;


	case RIG_CMD_SET_AF:
		/* check whether command is available */
		if (has_set->afg && new->afg) {
			value_t val;

			val.f = set->afg;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_AF, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_AF:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_AF);
			}

			/* reset flag */
			new->afg = FALSE;
			get->afg = set->afg;

			status = 1;
		}
		break;

	case RIG_CMD_GET_AF:
		/* check whether command is available */
		if (has_get->afg) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_AF, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_AF:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_AF);
			}
			else {
				get->afg = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_RF:
		/* check whether command is available */
		if (has_set->rfg && new->rfg) {
			value_t val;

			val.f = set->rfg;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_RF, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_RF:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_RF);
			}

			/* reset flag */
			new->rfg = FALSE;
			get->rfg = set->rfg;

			status = 1;
		}
		break;

	case RIG_CMD_GET_RF:
		/* check whether command is available */
		if (has_get->rfg) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_RF, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_RF:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_RF);
			}
			else {
				get->rfg = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_SQL:
		/* check whether command is available */
		if (has_set->sql && new->sql) {
			value_t val;

			val.f = set->sql;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_SQL, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_SQL:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_SQL);
			}

			/* reset flag */
			new->sql = FALSE;
			get->sql = set->sql;

			status = 1;
		}
		break;

	case RIG_CMD_GET_SQL:
		/* check whether command is available */
		if (has_get->sql) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_SQL, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_SQL:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_SQL);
			}
			else {
				get->sql = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_IFS:
		/* check whether command is available */
		if (has_set->ifs && new->ifs) {
			value_t val;

			val.i = set->ifs;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_IF, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_IFS:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_IFS);
			}

			/* reset flag */
			new->ifs = FALSE;
			get->ifs = set->ifs;

			status = 1;
		}
		break;

	case RIG_CMD_GET_IFS:
		/* check whether command is available */
		if (has_get->ifs) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_IF, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_IFS:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_IFS);
			}
			else {
				get->ifs = val.i;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_APF:
		/* check whether command is available */
		if (has_set->apf && new->apf) {
			value_t val;

			val.f = set->apf;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_APF, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_APF:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_APF);
			}

			/* reset flag */
			new->apf = FALSE;
			get->apf = set->apf;

			status = 1;
		}
		break;

	case RIG_CMD_GET_APF:
		/* check whether command is available */
		if (has_get->apf) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_APF, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_APF:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_APF);
			}
			else {
				get->apf = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_NR:
		/* check whether command is available */
		if (has_set->nr && new->nr) {
			value_t val;

			val.f = set->nr;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_NR, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_NR:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_NR);
			}

			/* reset flag */
			new->nr = FALSE;
			get->nr = set->nr;

			status = 1;
		}
		break;

	case RIG_CMD_GET_NR:
		/* check whether command is available */
		if (has_get->nr) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_NR, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_NR:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_NR);
			}
			else {
				get->nr = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_NOTCH:
		/* check whether command is available */
		if (has_set->notch && new->notch) {
			value_t val;

			val.i = set->notch;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_NOTCHF, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_NOTCH:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_NOTCH);
			}

			/* reset flag */
			new->notch = FALSE;
			get->notch = set->notch;

			status = 1;
		}
		break;

	case RIG_CMD_GET_NOTCH:
		/* check whether command is available */
		if (has_get->notch) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_NOTCHF, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_NOTCH:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_NOTCH);
			}
			else {
				get->notch = val.i;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_PBT_IN:
		/* check whether command is available */
		if (has_set->pbtin && new->pbtin) {
			value_t val;

			val.f = set->pbtin;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_PBT_IN, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_PBT_IN:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_PBT_IN);
			}

			/* reset flag */
			new->pbtin = FALSE;
			get->pbtin = set->pbtin;

			status = 1;
		}
		break;

	case RIG_CMD_GET_PBT_IN:
		/* check whether command is available */
		if (has_get->pbtin) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_PBT_IN, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_PBT_IN:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_PBT_IN);
			}
			else {
				get->pbtin = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_PBT_OUT:
		/* check whether command is available */
		if (has_set->pbtout && new->pbtout) {
			value_t val;

			val.f = set->pbtout;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_PBT_OUT, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_PBT_OUT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_PBT_OUT);
			}

			/* reset flag */
			new->pbtout = FALSE;
			get->pbtout = set->pbtout;

			status = 1;
		}
		break;

	case RIG_CMD_GET_PBT_OUT:
		/* check whether command is available */
		if (has_get->pbtout) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_PBT_OUT, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_PBT_OUT:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_PBT_OUT);
			}
			else {
				get->pbtout = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_CW_PITCH:
		/* check whether command is available */
		if (has_set->cwpitch && new->cwpitch) {
			value_t val;

			val.i = set->cwpitch;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_CWPITCH, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_CW_PITCH:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_CW_PITCH);
			}

			/* reset flag */
			new->cwpitch = FALSE;
			get->cwpitch = set->cwpitch;

			status = 1;
		}
		break;

	case RIG_CMD_GET_CW_PITCH:
		/* check whether command is available */
		if (has_get->cwpitch) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_CWPITCH, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_CW_PITCH:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_CW_PITCH);
			}
			else {
				get->cwpitch = val.i;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_KEYSPD:
		/* check whether command is available */
		if (has_set->keyspd && new->keyspd) {
			value_t val;

			val.i = set->keyspd;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_KEYSPD, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_KEYSPD:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_KEYSPD);
			}

			/* reset flag */
			new->keyspd = FALSE;
			get->keyspd = set->keyspd;

			status = 1;
		}
		break;

	case RIG_CMD_GET_KEYSPD:
		/* check whether command is available */
		if (has_get->keyspd) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_KEYSPD, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_KEYSPD:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_KEYSPD);
			}
			else {
				get->keyspd = val.i;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_BKINDEL:
		/* check whether command is available */
		if (has_set->bkindel && new->bkindel) {
			value_t val;

			val.i = set->bkindel;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_BKINDL, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_BKINDEL:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_BKINDEL);
			}

			/* reset flag */
			new->bkindel = FALSE;
			get->bkindel = set->bkindel;

			status = 1;
		}
		break;

	case RIG_CMD_GET_BKINDEL:
		/* check whether command is available */
		if (has_get->bkindel) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_BKINDL, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_BKINDEL:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_BKINDEL);
			}
			else {
				get->bkindel = val.i;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_BALANCE:
		/* check whether command is available */
		if (has_set->balance && new->balance) {
			value_t val;

			val.f = set->balance;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_BALANCE, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_BALANCE:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_BALANCE);
			}

			/* reset flag */
			new->balance = FALSE;
			get->balance = set->balance;

			status = 1;
		}
		break;

	case RIG_CMD_GET_BALANCE:
		/* check whether command is available */
		if (has_get->balance) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_BALANCE, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_BALANCE:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_BALANCE);
			}
			else {
				get->balance = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_VOXDEL:
		/* check whether command is available */
		if (has_set->voxdel && new->voxdel) {
			value_t val;

			val.i = set->voxdel;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_VOX, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_VOXDEL:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_VOXDEL);
			}

			/* reset flag */
			new->voxdel = FALSE;
			get->voxdel = set->voxdel;

			status = 1;
		}
		break;

	case RIG_CMD_GET_VOXDEL:
		/* check whether command is available */
		if (has_get->voxdel) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_VOX, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_VOXDEL:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_VOXDEL);
			}
			else {
				get->voxdel = val.i;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_VOXGAIN:
		/* check whether command is available */
		if (has_set->voxg && new->voxg) {
			value_t val;

			val.f = set->voxg;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_VOXGAIN, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_VOXGAIN:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_VOXGAIN);
			}

			/* reset flag */
			new->voxg = FALSE;
			get->voxg = set->voxg;

			status = 1;
		}
		break;

	case RIG_CMD_GET_VOXGAIN:
		/* check whether command is available */
		if (has_get->voxg) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_VOXGAIN, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_VOXGAIN:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_VOXGAIN);
			}
			else {
				get->voxg = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_ANTIVOX:
		/* check whether command is available */
		if (has_set->antivox && new->antivox) {
			value_t val;

			val.f = set->antivox;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ANTIVOX, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_ANTIVOX:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_ANTIVOX);
			}

			/* reset flag */
			new->antivox = FALSE;
			get->antivox = set->antivox;

			status = 1;
		}
		break;

	case RIG_CMD_GET_ANTIVOX:
		/* check whether command is available */
		if (has_get->antivox) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ANTIVOX, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_ANTIVOX:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_ANTIVOX);
			}
			else {
				get->antivox = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_MICGAIN:
		/* check whether command is available */
		if (has_set->micg && new->micg) {
			value_t val;

			val.f = set->micg;

			/* try to execute command */
			retcode = rig_set_level (myrig, RIG_VFO_CURR, RIG_LEVEL_MICGAIN, val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_SET_MICGAIN:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_SET_MICGAIN);
			}

			/* reset flag */
			new->micg = FALSE;
			get->micg = set->micg;

			status = 1;
		}
		break;

	case RIG_CMD_GET_MICGAIN:
		/* check whether command is available */
		if (has_get->micg) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_MICGAIN, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_MICGAIN:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_MICGAIN);
			}
			else {
				get->micg = val.f;
			}

			status = 1;
		}
		break;

	case RIG_CMD_SET_COMP:
		break;

	case RIG_CMD_GET_COMP:
		/* check whether command is available */
		if (has_get->comp) {
			value_t val;

			/* try to execute command */
			retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_COMP, &val);

			/* raise anomaly if execution did not succeed */
			if (retcode != RIG_OK) {
				grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: Failed to execute RIG_CMD_GET_COMP:\n%s"),
						  __FUNCTION__, ERR_TO_STR[abs(retcode)]);

				rig_anomaly_raise (RIG_CMD_GET_COMP);
			}
			else {
				get->comp = val.f;
			}

			status = 1;
		}
		break;

		/* set FUNC's status */
	case RIG_CMD_SET_FUNC:

		for (i = 0; i < RIG_SETTING_MAX; i++) {
			func = rig_idx2setting(i);
			if (has_set->funcs[i] && new->funcs[i]) {
				retcode = rig_set_func (myrig,
							RIG_VFO_CURR,
							func,
							set->funcs[i]);

				if (retcode != RIG_OK) {
					grig_debug_local (RIG_DEBUG_ERR,
							  _("%s: Failed to execute RIG_CMD_SET_FUNC(%s):\n%s"),
							  __FUNCTION__, rig_strfunc(func), ERR_TO_STR[abs(retcode)]);

					rig_anomaly_raise (RIG_CMD_SET_FUNC);
				}
				
				get->funcs[i] = set->funcs[i];
				new->funcs[i] = 0;

				status = 1;
			}
		}

		break;

		/* get FUNC's status */
	case RIG_CMD_GET_FUNC:

		for (i = 0; i < RIG_SETTING_MAX; i++) {
			func = rig_idx2setting(i);
			/* check whether command is available */
			if (has_get->funcs[i]) {
				int func_status;

				/* try to execute command */
				retcode = rig_get_func (myrig, RIG_VFO_CURR, func, &func_status);

				/* raise anomaly if execution did not succeed */
				if (retcode != RIG_OK) {
					grig_debug_local (RIG_DEBUG_ERR,
							  _("%s: Failed to execute RIG_CMD_GET_FUNC(%s):\n%s"),
							  __FUNCTION__, rig_strfunc(func), ERR_TO_STR[abs(retcode)]);

					rig_anomaly_raise (RIG_CMD_GET_FUNC);
				}
				else {
					get->funcs[i] = func_status;
				}

				status = 1;
			}
		}

		break;


		/* bug in grig! */
	default:
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: Unknown command %d (grig bug)"),
				  __FUNCTION__, cmd);
		break;

	}

	return status;

}


/** \brief Get hamlib id of radio.
 *  \return The id of the rig
 */
gint
rig_daemon_get_rig_id ()
{
	if (myrig == NULL) {
		return -1;
	}

	return myrig->caps->rig_model;
}

/** \brief Get radio brand.
 *  \return A character string containing the radio brand.
 *
 * This function returns the brand of the currently active radio.
 * If no radio has yet been initialised the function returns NULL.
 * The returned string should be freed when no longer needed.
 */
gchar *
rig_daemon_get_brand ()
{
	gchar *text;

	if (myrig == NULL) {
		return NULL;
	}
	text = g_strdup (myrig->caps->mfg_name);

	return text;
}


/** \brief Get radio model.
 *  \return A character string containing the radio model.
 *
 * This function returns the model of the currently active radio.
 * If no radio has yet been initialised the function returns NULL.
 * The returned string should be freed when no longer needed.
 */
gchar *
rig_daemon_get_model ()
{
	gchar *text;

	if (myrig == NULL) {
		return NULL;
	}
	text = g_strdup (myrig->caps->model_name);

	return text;
}


/** \brief Get command delay.
 *  \return The current command delay in msec.
 *
 * This function returns the current command delay
 * in milliseconds. This allows the GUI to have a rough idea
 * about what delay to use in the readback timeout functions.
 */
gint
rig_daemon_get_delay ()
{
	return cmd_delay;
}


/** \brief Suspend daemon.
 *  \param spnd Flag indicating whether to suspend or re-enable the daemon.
 *
 * This function can be used to suspend the daemon without shutting it down.
 * TRUE means suspend the daemon while FALSE means re-enable execution.
 * When in suspended mode, the daemon will keep on executing it's cycles,
 * but no commands will be sent to hamlib, as long as the daemon is suspended. 
 */
void
rig_daemon_set_suspend (gboolean spnd)
{
	suspended = spnd;

	grig_debug_local (RIG_DEBUG_VERBOSE, _("%s: %d"), __FUNCTION__, spnd);
}


/** \brief Check suspended status of daemon.
 *  \return TRUE if the daemon is suspended, FALSE otherwise.
 *
 * This function can be used to check whether the daemon is currently suspended
 * or not.
 */
gboolean
rig_daemon_get_suspend (void)
{
	return suspended;
}
