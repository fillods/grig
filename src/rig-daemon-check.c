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
 *  \brief Checks performed after rig init
 *
 * This file contains various supporting functions which are executed
 * after the radio has been initialized.
 *
 * \bug File includes gtk.h but not really needed?
 */
#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-daemon-check.h"




/** \brief Check availability of power status.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This functions checks the availability of the power status
 * function by trying to read it and then trying to set it to
 * the read value. If the read command is unsuccessful the
 * write comman is executed with RIG_POWER_OFF parameter.
 */
void
rig_daemon_check_pwrstat         (RIG              *myrig,
			          grig_settings_t  *get,
				  grig_cmd_avail_t *has_get,
				  grig_cmd_avail_t *has_set)
{

	int               retcode;                 /* hamlib execution code */
	powerstat_t       pwr = RIG_POWER_OFF;     /* power status */

	/* get power status; we are very paranoid and accept only
	   RIG_OK as good status.
	*/
	retcode = rig_get_powerstat (myrig, &pwr);
	if (retcode == RIG_OK) {
		has_get->pstat = TRUE;
		get->pstat = pwr;
	}
	else {
		has_get->pstat = FALSE;
		get->pstat = RIG_POWER_OFF;
	}

	/* try to set power status */
	retcode = rig_set_powerstat (myrig, get->pstat);
	has_set->pstat = (retcode == RIG_OK) ? TRUE : FALSE;

}



/** \brief Check PTT availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of the PTT status. The check is done
 * by trying to read the PTT status and then trying to reset it to the read
 * value. If the read was unsuccessful the write check is performed with
 * RIG_PTT_OFF.
 */
void
rig_daemon_check_ptt     (RIG               *myrig,
			  grig_settings_t   *get,
			  grig_cmd_avail_t  *has_get,
			  grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	ptt_t             ptt = RIG_PTT_OFF;       /* PTT status */


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

}



/** \brief Check VFO availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of VFO selection. The check is done
 * by trying to read the selected VFO and then trying to reset it to the read
 * value. If the read was unsuccessful the write check is not performed because
 * it doesn't make much sense to set any VFO when we don't even know what kind
 * of VFO's we have (VFOA/B or MAIN/SUB).
 */
void
rig_daemon_check_vfo     (RIG               *myrig,
			  grig_settings_t   *get,
			  grig_cmd_avail_t  *has_get,
			  grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	vfo_t             vfo = RIG_VFO_NONE;      /* current VFO */


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

	if (has_get->vfo) {
		/* check set_vfo functionality */
		retcode = rig_set_vfo (myrig, get->vfo);
		has_set->vfo = (retcode == RIG_OK) ? TRUE : FALSE;
	}
	else {
		has_set->vfo = FALSE;
	}
}




/** \brief Check get/set frequency.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function tests whether the rig is capable to get/set the frequency. First
 * the primary frequency is checked (the one on RIG_VFO_CURR). Then, if the primary
 * frequency operations are ok and we know about the available VFOs, the secondary
 * frequency is tested.
 */
void
rig_daemon_check_freq     (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	vfo_t             vfo = RIG_VFO_NONE;      /* current VFO */
	freq_t            freq;                    /* current frequency */



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
				   "*** GRIG: %s: I can't figure out available VFOs (got %d)",
				   __FUNCTION__, get->vfo);

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


}


/** \brief Check RIT availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of the RIT value. The check is done
 * by trying to read the RIT value and then trying to reset it to the read
 * value. If the read was unsuccessful the write check is performed with 0Hz.
 */
void
rig_daemon_check_rit      (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{

	int               retcode;                 /* Hamlib status code */
	shortfreq_t       sfreq;                   /* current RIT setting */


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

	/* get RIT range and tuning step */
	get->ritmax = 9990; //myrig->caps->max_rit;
	get->ritstep = s_Hz(10);

}



/** \brief Check XIT availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of the XIT value. The check is done
 * by trying to read the XIT value and then trying to reset it to the read
 * value. If the read was unsuccessful the write check is performed with 0Hz.
 */
void
rig_daemon_check_xit      (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{

	int               retcode;                 /* Hamlib status code */
	shortfreq_t       sfreq;                   /* current XIT setting */


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

	/* get XIT range and tuning step */
	get->xitmax = myrig->caps->max_xit;
	get->xitstep = s_Hz(10);

}




/** \brief Check mode and passband width availabilities.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function tests the availability of the mode and passband width
 * settings. These are checked together because this is the way hamlib
 * manages them.
 */
void
rig_daemon_check_mode     (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	rmode_t           mode;                    /* current mode */
	pbwidth_t         pbw;                     /* current passband width */
	int               i = 0;                   /* iterator */
	int               found_mode = 0;          /* flag to indicate found mode */


	/* try to get mode and passband width */
	retcode = rig_get_mode (myrig, RIG_VFO_CURR, &mode, &pbw);
	if (retcode == RIG_OK) {
		has_get->mode = TRUE;
		has_get->pbw  = TRUE;
		get->mode     = mode;

		/* convert and store the new passband width */
		if (pbw == rig_passband_wide (myrig, mode)) {
			get->pbw = RIG_DATA_PB_WIDE;
		}
		else if (pbw == rig_passband_narrow (myrig, mode)) {
			get->pbw  = RIG_DATA_PB_NARROW;
		}
		else {
			get->pbw  = RIG_DATA_PB_NORMAL;
		}
		
		/* initialize the frequency range and tuning step */
		while (!RIG_IS_FRNG_END(myrig->state.rx_range_list[i]) && !found_mode) {
						
			/* is this list good for current mode? */
			if ((mode & myrig->state.rx_range_list[i].modes) == mode) {
							
				found_mode = 1;
				get->fmin = myrig->state.rx_range_list[i].start;
				get->fmax = myrig->state.rx_range_list[i].end;
			}
			else {
				i++;
			}
			
		}

		/* if we did not find any suitable range there could be a bug
		   in the backend!
		*/
		if (!found_mode) {
			rig_debug (RIG_DEBUG_BUG,
				   "*** GRIG: %s: Can not find frequency range for this mode (%d)! "\
				   "Bug in backed?\n", __FUNCTION__, mode);
		}
	}

	else {
		has_get->mode = FALSE;
		has_get->pbw  = FALSE;
		get->mode     = RIG_MODE_NONE;
		get->pbw      = RIG_PASSBAND_NORMAL;

		/* initialize frequency range and tuning step to what? */
		get->fmin  = kHz(30);
		get->fmax  = GHz(1);
		get->fstep = Hz(10);
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


/** \brief Check level setting and reading availabilities.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function tests the availability of various level settings.
 * Please note, that while some levels are both readable and writeable, others
 * are only readable (eg. signal strength, SWR). Only the levels supported
 * byt grig are tested.
 */
void
rig_daemon_check_level     (RIG               *myrig,
			    grig_settings_t   *get,
			    grig_cmd_avail_t  *has_get,
			    grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	setting_t         haslevel;                /* available level settings */
	value_t           val;                     /* generic value */


	/* get available read levels
	 */
	haslevel = rig_has_get_level (myrig, GRIG_LEVEL_RD);

	/* unmask bits */
	has_get->power    = (haslevel && RIG_LEVEL_RFPOWER);
	has_get->strength = (haslevel && RIG_LEVEL_STRENGTH);
	has_get->swr      = (haslevel && RIG_LEVEL_SWR);
	has_get->alc      = (haslevel && RIG_LEVEL_ALC);
	has_get->agc      = (haslevel && RIG_LEVEL_AGC);

	/* read values */
	if (has_get->power) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &val);
		if (retcode == RIG_OK) {
			get->power = val.f;
		}
		else {
			/* send an error report */
			rig_debug (RIG_DEBUG_ERR, "*** GRIG: %s: Could not get RF power",
				   __FUNCTION__);
			
			/* disable command */
			has_get->power = FALSE;
		}
	}

	if (has_get->strength) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_STRENGTH, &val);
		if (retcode == RIG_OK) {
			get->strength = val.i;
		}
		else {
			/* send an error report */
			rig_debug (RIG_DEBUG_ERR, "*** GRIG: %s: Could not get signal strength",
				   __FUNCTION__);
			
			/* disable command */
			has_get->strength = FALSE;
		}
	}

	if (has_get->swr) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_SWR, &val);
		if (retcode == RIG_OK) {
			get->swr = val.f;
		}
		else {
			/* send an error report */
			rig_debug (RIG_DEBUG_ERR, "*** GRIG: %s: Could not get SWR",
				   __FUNCTION__);
			
			/* disable command */
			has_get->swr = FALSE;
		}
	}

	if (has_get->alc) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ALC, &val);
		if (retcode == RIG_OK) {
			get->alc = val.f;
		}
		else {
			/* send an error report */
			rig_debug (RIG_DEBUG_ERR, "*** GRIG: %s: Could not get ALC",
				   __FUNCTION__);
			
			/* disable command */
			has_get->alc = FALSE;
			
		}
	}

	if (has_get->agc) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_AGC, &val);
		if (retcode == RIG_OK) {
			get->agc = val.i;
		}
		else {
			/* send an error report */
			rig_debug (RIG_DEBUG_ERR, "*** GRIG: %s: Could not get AGC",
				   __FUNCTION__);
			
			/* disable command */
			has_get->agc = FALSE;
			
		}
	}


	/* get available write levels
	 */
	haslevel = rig_has_set_level (myrig, GRIG_LEVEL_WR);

	has_set->power = (haslevel && RIG_LEVEL_RFPOWER);
	has_set->agc   = (haslevel && RIG_LEVEL_AGC);
}
