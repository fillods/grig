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
#ifndef RIG_DAEMON_H
#define RIG_DAEMON_H 1




#define C_MAX_CMD_PER_CYCLE   6    /*!< Max number of commands executed in one cycle */
#define C_MAX_CYCLES          6    /*!< Number of cycles */

#define C_RX_CYCLE_DELAY   30   /*!< Delay between two RX cycles in milliseconds */
#define C_TX_CYCLE_DELAY   300  /*!< Delay between two TX cycles in milliseconds */


#define C_RIG_DAEMON_STOP_TIMEOUT 500000  /*!< Timeout to let the daemon process stop */


/** \brief List of available commands.
 *
 * This enumeration lists the available commands that can be executed by the RIG daemon.
 * These values should be used to define each daemon cycle.
 */
typedef enum {
	RIG_CMD_NONE = 0,     /*!< Not command. Can be used for delays between commands. */

	RIG_CMD_GET_FREQ_1,    /*!< Command to acquire primary frequency from rig. */
	RIG_CMD_SET_FREQ_1,    /*!< Command to set primary frequency. */
	RIG_CMD_GET_FREQ_2,    /*!< Command to acquire secondary frequency from rig. */
	RIG_CMD_SET_FREQ_2,    /*!< Command to set secondary frequency. */
	RIG_CMD_GET_RIT,       /*!< Command to get current RIT value. */
	RIG_CMD_SET_RIT,       /*!< Command to set new RIT value. */
	RIG_CMD_GET_XIT,       /*!< Command to get current XIT value. */
	RIG_CMD_SET_XIT,       /*!< Command to set new XIT value. */
	RIG_CMD_GET_VFO,       /*!< Command to get currently active VFO. */      
	RIG_CMD_SET_VFO,       /*!< Command to select new VFO. */
	RIG_CMD_GET_PSTAT,     /*!< Command to read power status (mains pwr, ON/OFF/STDBY). */
	RIG_CMD_SET_PSTAT,     /*!< Command to set new power status (ON/OFF/STDBY). */
	RIG_CMD_GET_PTT,       /*!< Command to get the current PTT status. */
	RIG_CMD_SET_PTT,       /*!< Command to set the current PTT status. */
	RIG_CMD_GET_MODE,      /*!< Command to get the current mode and passband width. */
	RIG_CMD_SET_MODE,      /*!< Command to set the new mode and/or passband width. */
	RIG_CMD_GET_AGC,       /*!< Command to get the automatic gain control level. */
	RIG_CMD_SET_AGC,       /*!< Command to set the automatic gain control level. */
	RIG_CMD_GET_ATT,       /*!< Command to get the attenuator level. */
	RIG_CMD_SET_ATT,       /*!< Command to set the attenuator level. */
	RIG_CMD_GET_PREAMP,    /*!< Command to get the pre-amplifier level. */
	RIG_CMD_SET_PREAMP,    /*!< Command to set the pre-amplifier level. */

	RIG_CMD_GET_STRENGTH,  /*!< Command to get signal strength. */
	RIG_CMD_SET_POWER,     /*!< Command to set TX power. */
	RIG_CMD_GET_POWER,     /*!< Command to get TX power. */
	RIG_CMD_GET_SWR,       /*!< Command to get SWR level. */
	RIG_CMD_GET_ALC,       /*!< Command to get ALC level. */

	RIG_CMD_NUMBER         /*!< Number of available commands. */
} rig_cmd_t;



int    rig_daemon_start (int, const gchar *, int, const gchar *, const gchar *);
void   rig_daemon_stop  (void);
gchar *rig_daemon_get_brand (void);
gchar *rig_daemon_get_model (void);

#endif
