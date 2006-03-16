/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2006  Alexandru Csete.

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
    along with this program; if not, visit http://www.fsf.org/
 
 
 
 
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
#include <glib/gi18n.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "grig-debug.h"
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
 * write command is not executed since we don't know what to
 * set it to.
 *
 * \note Since bug #1082325 this is the only setting which is tested
 *       for real. All other setting availabilities are just obtained
 *       from the rig caps structure.
 *
 * \note See bug XXXXX there are rig, where one can only set the power
 *       status and not read. The current implementationshould be safe
 *       so we just check the claiimed availability.
 */
void
rig_daemon_check_pwrstat         (RIG              *myrig,
			          grig_settings_t  *get,
				  grig_cmd_avail_t *has_get,
				  grig_cmd_avail_t *has_set)
{

	has_get->pstat = (myrig->caps->get_powerstat != NULL) ? TRUE : FALSE;
	has_set->pstat = (myrig->caps->set_powerstat != NULL) ? TRUE : FALSE;
	get->pstat = RIG_POWER_ON;

}



/** \brief Check PTT availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of the PTT status. The check is done
 * by reading the caps->get_ptt and caps->set_ptt. These are pointersto the
 * actual backend functions and should only be non-null if the backend atually
 * supports these operations.
 */
void
rig_daemon_check_ptt     (RIG               *myrig,
			  grig_settings_t   *get,
			  grig_cmd_avail_t  *has_get,
			  grig_cmd_avail_t  *has_set)

{
	has_get->ptt = (myrig->caps->get_ptt != NULL) ? TRUE : FALSE;
	has_set->ptt = (myrig->caps->set_ptt != NULL) ? TRUE : FALSE;
	get->ptt = RIG_PTT_OFF;
}



/** \brief Check VFO availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of VFO selection. The check is done
 * by looking for in the rig_caps structure of myrig.
 */
void
rig_daemon_check_vfo     (RIG               *myrig,
			  grig_settings_t   *get,
			  grig_cmd_avail_t  *has_get,
			  grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	vfo_t             vfo = RIG_VFO_NONE;      /* current VFO */


	/* check whether we can get/set VFO */
	has_get->vfo = (myrig->caps->get_vfo != NULL) ? TRUE : FALSE;
	has_set->vfo = (myrig->caps->set_vfo != NULL) ? TRUE : FALSE;

	/* check for VFO operations */
	has_set->vfo_op_toggle = (myrig->caps->vfo_ops & RIG_OP_TOGGLE) ? TRUE : FALSE;
	has_set->vfo_op_copy   = (myrig->caps->vfo_ops & RIG_OP_CPY) ? TRUE : FALSE;
	has_set->vfo_op_xchg   = (myrig->caps->vfo_ops & RIG_OP_XCHG) ? TRUE : FALSE;

	/* Check for native split support; we blindly trust that all backends, which
	   have any possibility to set/get this feature will have a function defined
	   for it.
	   NOTE: The current implementation of rig_set_split in hamlib can actually
	         set split ON/OFF without native backend support.
	*/
	has_set->split = (myrig->caps->set_split_vfo != NULL) ? TRUE : FALSE;
	has_get->split = (myrig->caps->get_split_vfo != NULL) ? TRUE : FALSE;

	/* store available VFOs */
	if ((has_get->vfo || has_set->vfo) && (myrig->state.vfo_list != 0)) {
		rig_data_set_vfos (myrig->state.vfo_list);
	}
	else {
		grig_debug_local (RIG_DEBUG_BUG,
				  _("%s: Can not find VFO list for this "\
				    "backend! Bug in backend?"),
				  __FUNCTION__);
	}

	/* try to get current VFO */
	if (has_get->vfo) {
		retcode = rig_get_vfo (myrig, &vfo);
		if (retcode == RIG_OK) {
			get->vfo = vfo;
		}
		else {
			get->vfo = RIG_VFO_NONE;
		}
	}
}




/** \brief Check get/set frequency.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function tests whether the rig is capable to get/set the frequency. 
 * The test is done by checking the get_freq and set_freq pointers in the
 * rig_caps structure. Furthermore, if the rig is capable of getting the
 * frequency, the curent frequency is read.
 */
void
rig_daemon_check_freq     (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	freq_t            freq;                    /* current frequency */


	/* check get/set freq availabilities */
	has_get->freq1 = (myrig->caps->get_freq != NULL) ? TRUE : FALSE;
	has_set->freq1 = (myrig->caps->set_freq != NULL) ? TRUE : FALSE;
	

	if (has_get->freq1) {
		/* try to obtain current frequncy */
		retcode = rig_get_freq (myrig, RIG_VFO_CURR, &freq);
		if (retcode == RIG_OK) {
			get->freq1 = freq;
		}
		else {
			get->freq1 = 0.0;
		}
	}
	else {
		get->freq1 = 0.0;
	}
}


/** \brief Check RIT availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of the RIT value. The test is done
 * by checking the get_rit and set-rit pointers in the rig_caps structure.
 * Furthermore, if get_rit is available the current value is read and stored.
 *
 * \bug The code sets the ritstep to 10Hz.
 */
void
rig_daemon_check_rit      (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{

	int               retcode;                 /* Hamlib status code */
	shortfreq_t       sfreq;                   /* current RIT setting */

	/* checkfor RIT availability */
	has_get->rit = (myrig->caps->get_rit != NULL) ? TRUE : FALSE;
	has_set->rit = (myrig->caps->set_rit != NULL) ? TRUE : FALSE;

	if (has_get->rit) {
		/* try to get RIT setting */
		retcode = rig_get_rit (myrig, RIG_VFO_CURR, &sfreq);
		if (retcode == RIG_OK) {
			get->rit = sfreq;
		}
		else {
			get->rit = s_kHz(0.00);
		}
	}
	else {
		get->rit = s_kHz(0.00);
	}

	if (has_get->rit || has_set->rit) {
		/* get RIT range and tuning step */
		get->ritmax = myrig->caps->max_rit;
		get->ritstep = s_Hz(10);

	}
	else {
		get->ritmax = s_kHz(0.00);
		get->ritstep = s_Hz(0);
	}
		
}



/** \brief Check XIT availability.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function check the availability of the XIT value. The test is done
 * by checking the get_xit and set-xit pointers in the rig_caps structure.
 * Furthermore, if get_xit is available the current value is read and stored.
 *
 * \bug The code sets the xitstep to 10Hz.
 */
void
rig_daemon_check_xit      (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{

	int               retcode;                 /* Hamlib status code */
	shortfreq_t       sfreq;                   /* current XIT setting */

	/* checkfor XIT availability */
	has_get->xit = (myrig->caps->get_xit != NULL) ? TRUE : FALSE;
	has_set->xit = (myrig->caps->set_xit != NULL) ? TRUE : FALSE;

	if (has_get->xit) {
		/* try to get RIT setting */
		retcode = rig_get_xit (myrig, RIG_VFO_CURR, &sfreq);
		if (retcode == RIG_OK) {
			get->xit = sfreq;
		}
		else {
			get->xit = s_kHz(0.00);
		}
	}
	else {
		get->xit = s_kHz(0.00);
	}

	if (has_get->xit || has_set->xit) {
		/* get XIT range and tuning step */
		get->xitmax = myrig->caps->max_xit;
		get->xitstep = s_Hz(10);
	}
	else {
		get->xitmax = s_kHz(0.00);
		get->xitstep = s_Hz(0);
	}
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
 *
 * This function also checks for the frequency range and resolution for
 * the current mode and also builds the global list of available modes.
 *
 * \bug much of the freq. range code is similar to ode in the RIG_CMD_GET_MODE code
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


	has_get->mode = (myrig->caps->get_mode != NULL) ? TRUE : FALSE;
	has_set->mode = (myrig->caps->set_mode != NULL) ? TRUE : FALSE;

	has_get->pbw = has_get->mode;
	has_set->pbw = has_set->mode;

	/* try to get mode and passband width */
	retcode = rig_get_mode (myrig, RIG_VFO_CURR, &mode, &pbw);
	if (retcode == RIG_OK) {
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
		
		/* initialize the frequency range and tuning step
		   note: we loop through *all* ranges so that we can create
		   a list of supported modes as well
		*/
		while (!RIG_IS_FRNG_END(myrig->state.rx_range_list[i])) {
					       
			/* store modes */
			get->allmodes |= myrig->state.rx_range_list[i].modes;

			/* if we have not yet found a mode      AND
			   this list is good for current mode   AND
			   the current frequency is within this range
			*/
			if (!found_mode &&
			    ((mode & myrig->state.rx_range_list[i].modes) == mode) &&
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
					  _("%s: Can not find frequency range for this "\
					    "mode (%d)! Bug in backend?"),
					  __FUNCTION__, mode);
		}

		/* get the smallest tuning step */
		get->fstep = rig_get_resolution (myrig, mode);

	}

	else {
		get->mode     = RIG_MODE_NONE;
		get->pbw      = RIG_PASSBAND_NORMAL;

		/* initialize frequency range and tuning step to what? */
		get->fmin  = kHz(30);
		get->fmax  = GHz(1);
		get->fstep = Hz(10);
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
 * by grig are tested.
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
	has_get->power    = ((haslevel & RIG_LEVEL_RFPOWER) ? 1 : 0);
	has_get->strength = ((haslevel & RIG_LEVEL_STRENGTH) ? 1 : 0);
	has_get->swr      = ((haslevel & RIG_LEVEL_SWR) ? 1 : 0);
	has_get->alc      = ((haslevel & RIG_LEVEL_ALC) ? 1 : 0);
	has_get->agc      = ((haslevel & RIG_LEVEL_AGC) ? 1 : 0);
	has_get->att      = ((haslevel & RIG_LEVEL_ATT) ? 1 : 0);
	has_get->preamp   = ((haslevel & RIG_LEVEL_PREAMP) ? 1 : 0);

	/* read values */
	if (has_get->power) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &val);
		if (retcode == RIG_OK) {
			get->power = val.f;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get RF power"),
					  __FUNCTION__);
		}
	}

	if (has_get->strength) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_STRENGTH, &val);
		if (retcode == RIG_OK) {
			get->strength = val.i;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get signal strength"),
					  __FUNCTION__);

			get->strength = -54;
		}
	}

	if (has_get->swr) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_SWR, &val);
		if (retcode == RIG_OK) {
			get->swr = val.f;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get SWR"),
					  __FUNCTION__);
		}
	}

	if (has_get->alc) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ALC, &val);
		if (retcode == RIG_OK) {
			get->alc = val.f;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get ALC"),
					  __FUNCTION__);
		}
	}

	if (has_get->agc) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_AGC, &val);
		if (retcode == RIG_OK) {
			get->agc = val.i;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get AGC"),
					  __FUNCTION__);
		}
	}

	if (has_get->att) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_ATT, &val);
		if (retcode == RIG_OK) {
			get->att = val.i;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get ATT"),
					  __FUNCTION__);
		}
	}

	if (has_get->preamp) {
		retcode = rig_get_level (myrig, RIG_VFO_CURR, RIG_LEVEL_PREAMP, &val);
		if (retcode == RIG_OK) {
			get->preamp = val.i;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get PREAMP"),
					  __FUNCTION__);
		}
	}


	/* get available write levels */
	haslevel = rig_has_set_level (myrig, GRIG_LEVEL_WR);

	/* we don't perform explicit testing of set levels
	   (like we did with the get levels) since we might
	   not have any good values to send
	*/
	has_set->power  = ((haslevel & RIG_LEVEL_RFPOWER) ? 1 : 0);
	has_set->agc    = ((haslevel & RIG_LEVEL_AGC) ? 1 : 0);
	has_set->att    = ((haslevel & RIG_LEVEL_ATT) ? 1 : 0);
	has_set->preamp = ((haslevel & RIG_LEVEL_PREAMP) ? 1 : 0);

	/* initialise preamp and att arrays in rig-data */
	if (has_get->att || has_set->att) {
		int i = 0;

		while ((i < MAXDBLSTSIZ) && (myrig->caps->attenuator[i] != 0)) {
			rig_data_set_att_data (i, myrig->caps->attenuator[i]);
			i++;
		}
		
	}
		

	if (has_get->preamp || has_set->preamp) {
		int i = 0;

		while ((i < MAXDBLSTSIZ) && (myrig->caps->preamp[i] != 0)) {
			rig_data_set_preamp_data (i, myrig->caps->preamp[i]);
			i++;
		}
	}

	/* FIXME: AGC ARRAY? */
}



/** \brief Check FUNC setting and reading availabilities.
 *  \param rig The radio handle.
 *  \param get Pointer to shared data 'get'.
 *  \param has_get Pointer to shared data 'has_get'.
 *  \param has_set Pointer to shared data 'has_set'.
 *
 * This function tests the availability of various special functions,
 * which are available through the rig_setfunc and rig_get_func API calls.
 * Only the functions supported by grig are tested.
 */
void
rig_daemon_check_func     (RIG               *myrig,
			   grig_settings_t   *get,
			   grig_cmd_avail_t  *has_get,
			   grig_cmd_avail_t  *has_set)

{
	int               retcode;                 /* Hamlib status code */
	setting_t         hasfunc;                 /* available func settings */
	int               val;                     /* generic value */


	/* get available read funcs
	 */
	hasfunc = rig_has_get_func (myrig, GRIG_FUNC_RD);

	/* unmask bits */
	has_get->lock    = ((hasfunc & RIG_FUNC_LOCK) ? 1 : 0);

	/* read values */
	if (has_get->lock) {
		retcode = rig_get_func (myrig, RIG_VFO_CURR, RIG_FUNC_LOCK, &val);
		if (retcode == RIG_OK) {
			get->lock = val;
		}
		else {
			/* send an error report */
			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s: Could not get LOCK status"),
					  __FUNCTION__);
		}
	}


	/* get available write funcs */
	hasfunc = rig_has_set_func (myrig, GRIG_FUNC_WR);

	/* we don't perform explicit testing of set functions
	   (like we did with the get levels) since we might
	   not have any good values to send
	*/
	has_set->lock  = ((hasfunc & RIG_FUNC_LOCK) ? 1 : 0);

}
