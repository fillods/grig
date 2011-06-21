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
/**  \file    rig-data.c
 *   \ingroup shdata
 *   \brief   Shared data between rig daemon and GUI.
 *
 * This object implements the data structures that are shared between
 * the rig daemon and the GUI. These data include commanded and acquired
 * rig settings.
 *
 * Although the data is global and publicly available, the idea is that
 * while the rig daemon should access them directly, the GUI should
 * only use the API functions.
 *
 * \note The rig-daemon object is responsible for the correct initialization
 *       of the shared data structures and their contents before they can
 *       be accessed by the GUI.
 *
 * \note 'set' functions will also modify the 'get' variable to avoid temporary
 *       flipping to he current value (in case the daemon does not update the
 *       'get' variable before the GUI reads it again).
 * 
 * \bug Must add rig_data_has_get_xxx and rig_data_has_set_xxx functions.
 *
 * \bug File includes gtk.h but not really needed?
 */

#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include <glib/gi18n.h>
#include "rig-data.h"


grig_settings_t  set;      /*!< These values are sent to the radio. */
grig_settings_t  get;      /*!< These values are read from the radio. */
grig_cmd_avail_t new;      /*!< Flags to indicate whether new value is available. */
grig_cmd_avail_t has_set;  /*!< Flags to indicate writing capabilities. */
grig_cmd_avail_t has_get;  /*!< Flags to indicate reading capabilities. */


/** \brief List of attenuator values (absolute values). */
static int att[MAXDBLSTSIZ];

/** \brief List of preamp values. */
static int preamp[MAXDBLSTSIZ];

/** \brief Bit field of available VFO's */
static int vfo_list;

/** \brief Maximum power in W */
static float maxpwr = 0.0;


/** \brief Getavailable VFOs.
 *  \return Bit field of available VFOs.
 *
 * This function returns the available VFOs in a bit field
 * Although the details of the bitfield can be deduced from the hamlib api
 * documentation, grig is only interested in the symbolic references like
 * RIG_VFO_A, RIG_VFO_B and such.
 */
int
rig_data_get_vfos         ()
{
	return vfo_list;
}


/** \brief Set available VFOs.
 *  \param vfos Bit field of available VFOs.
 * 
 * This function sets the bit field of available VFOs.
 * It should be used by the daemon after the VFOs are checked.
 */ 
void
rig_data_set_vfos         (int vfos)
{
	vfo_list = vfos;
}



/** \brief Set attenuator data.
 *  \param index The index of the element to set
 *  \param data The value of the element.
 *
 * This function sets the element at index in the att array to data.
 * This function is usedby the rig-daemon-check function to initialise
 * the attenuator array.
 */
void
rig_data_set_att_data (int index, int data)
{
	if ((index >= 0) && (index < MAXDBLSTSIZ))
		att[index] = data;
}


/** \brief Get attenuator value.
 *  \param index The index of the element.
 *  \return The value at index.
 *
 * This function returns the attenuator vlue stored at index.
 * the function can be used by the GUI to initialise the attenuator
 * widget.
 */
int
rig_data_get_att_data (int index)
{
	if ((index >= 0) && (index < MAXDBLSTSIZ)) {
		return att[index];
	}
	else {
		return 0;
	}
}


/** \brief Get array index of a specific att value.
 *  \param data The att value to check.
 *  \return The array index of data. -1 if data not in array.
 *
 * This function scans the att array for data and returns it's array
 * index if data can be found in the array.
 */
int
rig_data_get_att_index    (int data)
{
	int i = 0;

	/* invali att value */
	if (data <= 0)
		return -1;

	/* scan through the array */
	while ((i < MAXDBLSTSIZ) && (att[i] != 0)) {
		if (att[i] == data) {
			return i;
		}
		i++;
	}

	/* data not in array */
	return -1;
}



/** \brief Set preamp data.
 *  \param index The index of the element to set
 *  \param data The value of the element.
 *
 * This function sets the element at index in the preamp array to data.
 * This function is usedby the rig-daemon-check function to initialise
 * the preamp array.
 */
void
rig_data_set_preamp_data (int index, int data)
{
	if ((index >= 0) && (index < MAXDBLSTSIZ))
		preamp[index] = data;
}


/** \brief Get preamp value.
 *  \param index The index of the element.
 *  \return The value at index.
 *
 * This function returns the preamp value stored at index.
 * the function can be used by the GUI to initialise the preamp
 * widget.
 */
int
rig_data_get_preamp_data (int index)
{
	if ((index >= 0) && (index < MAXDBLSTSIZ)) {
		return preamp[index];
	}
	else {
		return 0;
	}
}



/** \brief Get array index of a specific preamp value.
 *  \param data The preamp value to check.
 *  \return The array index of data. -1 if data not in array.
 *
 * This function scans the preamp array for data and returns it's array
 * index if data can be found in the array.
 */
int
rig_data_get_preamp_index    (int data)
{
	int i = 0;

	/* invalid preamp value */
	if (data <= 0)
		return -1;

	/* scan through the array */
	while ((i < MAXDBLSTSIZ) && (preamp[i] != 0)) {
		if (preamp[i] == data) {
			return i;
		}
		i++;
	}

	/* data not in array */
	return -1;
}


/** \brief Set power status.
 *  \param pwr The new power status.
 *
 * This function sets the targeted power status to pwr.
 */
void 
rig_data_set_pstat   (powerstat_t pwr)
{
	set.pstat = pwr;
	get.pstat = pwr;
	new.pstat = 1;
}



/** \brief Set PTT status.
 *  \param ptt The new PTT status.
 *
 * This function sets the targeted PTT status to ptt.
 */
void
rig_data_set_ptt     (ptt_t ptt)
{
	set.ptt = ptt;
	get.ptt = ptt;
	new.ptt = 1;
}







/** \brief Set TX power.
 *  \param power The new TX power.
 *
 * This function sets the desired TX power.
 */
void
rig_data_set_power   (float power)
{
	set.power = power;
	get.power = power;
	new.power = TRUE;
}





/** \brief Set mode.
 *  \param mode The new mode.
 *
 * This function sets the targeted mode to mode.
 *
 * \note The current Hamlib API requires that the mode and passband width
 *       are set within the same function call.
 */
void
rig_data_set_mode    (rmode_t mode)
{
	set.mode = mode;
	get.mode = mode;
	new.mode = 1;
}



/** \brief Set passband width.
 *  \param pbw The new passband width.
 *
 * This function set the targeted passband width to pbw.
 *
 * \note The current Hamlib API requires that the mode and passband width
 *       are set within the same function call.
 */
void
rig_data_set_pbwidth (rig_data_pbw_t pbw)
{
	set.pbw = pbw;
	get.pbw = pbw;
	new.pbw = 1;
}


/** \brief Set frequency.
 *  \param int Number indicating which frequency to set. 1 corresponds to
 *             the main/primary/working frequency and 2 to the secondary.
 *  \param freq The new frequency.
 *
 * This function sets the targeted frequency to frew.
 */ 
void
rig_data_set_freq    (int num, freq_t freq)
{
	switch (num) {

		/* primary frequency */
	case 1: set.freq1 = freq;
		get.freq1 = freq;
		new.freq1 = 1;
		break;

		/* secondary frequency */
	case 2: set.freq2 = freq;
		get.freq2 = freq;
		new.freq2 = 1;
		break;

		/* this is a bug */
	default:
		g_warning (_("%s: Invalid target: %d\n"), __FUNCTION__, num);
		break;
	}
}


/** \brief Set RIT offset.
 *  \param rit The new RIT offset.
 *
 * This function sets the targeted RIT offset to rit.
 */
void
rig_data_set_rit     (shortfreq_t rit)
{
	set.rit = rit;
	get.rit = rit;
	new.rit = 1;
}


/** \brief Set XIT offset.
 *  \param rit The new XIT offset.
 *
 * This function sets the targeted XIT offset to xit.
 */
void
rig_data_set_xit     (shortfreq_t xit)
{
	set.xit = xit;
	get.xit = xit;
	new.xit = 1;
}


/** \brief Set AGC level.
 *  \param rit The new AGC level.
 *
 * This function sets the targeted AGC level to agc.
 */
void
rig_data_set_agc     (int agc)
{
	set.agc = agc;
	get.agc = agc;
	new.agc = 1;
}


/** \brief Set attenuator level.
 *  \param rit The new attenuator level.
 *
 * This function sets the targeted attenuator level to att.
 */
void
rig_data_set_att     (int att)
{
	set.att = att;
	get.att = att;
	new.att = 1;
}


/** \brief Set pre-amplifier level.
 *  \param rit The new pre-amplifier level.
 *
 * This function sets the targeted pre-amplifier level to preamp.
 */
void
rig_data_set_preamp     (int preamp)
{
	set.preamp = preamp;
	get.preamp = preamp;
	new.preamp = 1;
}


/** \brief Set antenna.
 *  \param antenna The new antenna.
 *
 * This function sets the targeted antenna to antenna.
 */
void
rig_data_set_antenna    (ant_t antenna)
{
	set.antenna = antenna;
	get.antenna = antenna;
	new.antenna = 1;
}


/** \brief Get power status.
 *  \return The current power status.
 *
 * This function returns the current power status.
 */
powerstat_t
rig_data_get_pstat   ()
{
	return get.pstat;
}


/** \brief Get PTT status.
 *  \return The current PTT status.
 *
 * This function returns the current PTT status.
 */
ptt_t
rig_data_get_ptt     ()
{
	return get.ptt;
}


/* SET and GET VFO */
vfo_t
rig_data_get_vfo     ()
{
	return get.vfo;
}

void
rig_data_set_vfo     (vfo_t vfo)
{
	set.vfo = vfo;
	get.vfo = vfo;
	new.vfo = 1;
}

int
rig_data_has_get_vfo  ()
{
	return has_get.vfo;
}


int
rig_data_has_set_vfo  ()
{
	return has_set.vfo;
}



/** \brief Get current mode.
 *  \return The current mode.
 *
 * This function returns the current mode.
 */
rmode_t
rig_data_get_mode    ()
{
	return get.mode;
}



/** \brief Get current passband width.
 *  \returns The current passband width.
 *
 * This function returns the current passband width.
 */
rig_data_pbw_t
rig_data_get_pbwidth ()
{
	return get.pbw;
}



/** \brief Get current frequency.
 *  \param num Integer indicating which frequency to obtain.
 *  \return The current frequency.
 *
 * This function returns the current frequnecy of the specified target.
 * If num is 1 the value of the primary frequency is returned, while if
 * num is equal to 2 the value of the secondary frequency is returned.
 */
freq_t
rig_data_get_freq    (int num)
{
	switch (num) {

		/* primary frequency */
	case 1: return get.freq1;
		break;

		/* secondary frequenct */
	case 2: return get.freq2;
		break;

		/* bug */
	default: g_warning (_("%s: Invalid target: %d\n"), __FUNCTION__, num);
		return get.freq1;
		break;
	}
}


/** \brief Get lower freqency limit.
 *  \return The current lower frequency limit.
 *
 * This function returns the lower frequency limit which applies to
 * the current mode.
 */
freq_t
rig_data_get_fmin     ()
{
	return get.fmin;
}



/** \brief Get upper freqency limit.
 *  \return The current upper frequency limit.
 *
 * This function returns the upper frequency limit which applies to
 * the current mode.
 */
freq_t
rig_data_get_fmax     ()
{
	return get.fmax;
}



/** \brief Get tuning step.
 *  \return The current tuning step.
 *
 * This function returns the tuning step corresponding to the
 * current mode.
 */
shortfreq_t
rig_data_get_fstep    ()
{
	return get.fstep;
}



/** \brief Get RIT offset.
 *  \return The current value of the RIT offset.
 *
 * This function returns the current value of the RIT offset.
 */
shortfreq_t
rig_data_get_rit     ()
{
	return get.rit;
}


/** \brief Get XIT offset.
 *  \return The current value of the XIT offset.
 *
 * This function returns the current value of the XIT offset.
 */
shortfreq_t
rig_data_get_xit     ()
{
	return get.xit;
}


/** \brief Get AGC level.
 *  \return The current value of the AGC.
 *
 * This function returns the current value of the AGC.
 */
int
rig_data_get_agc     ()
{
	return get.agc;
}


/** \brief Get attenuator level.
 *  \return The current value of the attenuator.
 *
 * This function returns the current value of the attenuator.
 */
int
rig_data_get_att     ()
{
	return get.att;
}


/** \brief Get preamp level.
 *  \return The current value of the preamp.
 *
 * This function returns the current value of the preamp.
 */
int
rig_data_get_preamp     ()
{
	return get.preamp;
}


/** \brief Get signal strength.
 *  \return The current value of the signal strength.
 *
 * This function returns the current value of the signal strength.
 */
int
rig_data_get_strength ()
{
	return get.strength;
}


/** \brief Get TX power.
 *  \return The current value of the TX power.
 *
 * This function returns the current value of the TX power.
 */
float
rig_data_get_power    ()
{
	return get.power;
}


/** \brief Get SWR.
 *  \return The current value of the SWR.
 *
 * This function returns the current value of the SWR.
 */
float
rig_data_get_swr      ()
{
	return get.swr;
}


/** \brief Get ALC.
 *  \return The current value of the ALC.
 *
 * This function returns the current value of the ALC.
 */
float
rig_data_get_alc      ()
{
	return get.alc;
}


void
rig_data_set_alc      (float alc)
{
	set.alc = alc;
	new.alc = TRUE;
}

/** \brief Get current antenna.
 *  \return The current antenna.
 *
 * This function returns the current antenna.
 */
ant_t
rig_data_get_antenna    ()
{
	return get.antenna;
}


/** \brief Get availablility of signal strength readback.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.strength variable.
 */
int
rig_data_has_get_strength ()
{
	return has_get.strength;
}




/** \brief Get availablility of power status.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.pstat variable.
 */
int
rig_data_has_get_pstat ()
{
	return has_get.pstat;
}



/** \brief Get availablility of PTT.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.ptt variable.
 */
int
rig_data_has_get_ptt ()
{
	return has_get.ptt;
}



/** \brief Get availablility of RIT.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.rit variable.
 */
int
rig_data_has_get_rit ()
{
	return has_get.rit;
}



/** \brief Get availablility of XIT.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.xit variable.
 */
int
rig_data_has_get_xit ()
{
	return has_get.xit;
}


/** \brief Get availablility of RIT.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_set.rit variable.
 */
int
rig_data_has_set_rit ()
{
	return has_set.rit;
}



/** \brief Get availablility of XIT.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_set.xit variable.
 */
int
rig_data_has_set_xit ()
{
	return has_set.xit;
}


/** \brief Get availablility of AGC.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.agc variable.
 */
int
rig_data_has_get_agc ()
{
	return has_get.agc;
}



/** \brief Get availablility of attenuator.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.att variable.
 */
int
rig_data_has_get_att ()
{
	return has_get.att;
}



/** \brief Get availablility of pre-amp.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.preamp variable.
 */
int
rig_data_has_get_preamp ()
{
	return has_get.preamp;
}




/** \brief Get availability of reading primary frequency.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.freq1 variable.
 *
 */
int
rig_data_has_get_freq1     ()
{
	return has_get.freq1;
}



/** \brief Get availability of reading secondary frequency.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.freq2 variable.
 *
 */
int
rig_data_has_get_freq2     ()
{
	return has_get.freq2;
}



/** \brief Get availability of reading TX power.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.power variable.
 *
 */
int
rig_data_has_get_power    ()
{
	return has_get.power;
}

int
rig_data_has_set_power    ()
{
	return has_set.power;
}



/** \brief Get availability of reading SWR.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.swr variable.
 *
 */
int
rig_data_has_get_swr      ()
{
	return has_get.swr;
}


/** \brief Get availability of reading ALC.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_get.alc variable.
 *
 */
int
rig_data_has_get_alc      ()
{
	return has_get.alc;
}


int
rig_data_has_set_alc      ()
{
	return has_set.alc;
}

/** \brief Get availablility of power status.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_set.pstat variable.
 */
int
rig_data_has_set_pstat ()
{
	return has_set.pstat;
}



/** \brief Get availablility of PTT.
 *  \return 1 if available, otherwise 0.
 *
 * This function returns the value of the has_set.ptt variable.
 */
int
rig_data_has_set_ptt ()
{
	return has_set.ptt;
}



/** \brief Some text.
 *  \return description
 *
 * Detailed description.
 *
 * \bug writeme
 *
 * \bug should have 1 or 2 as param like get_freq
 */
int
rig_data_has_set_freq1     ()
{
	return has_set.freq1;
}



/** \brief Some text.
 *  \return description
 *
 * Detailed description.
 *
 * \bug writeme
 */
int
rig_data_has_set_freq2     ()
{
	return has_set.freq2;
}



/** \brief Some text.
 *  \return description
 *
 * Detailed description.
 *
 * \bug writeme
 */
int
rig_data_has_set_att    ()
{
	return has_set.att;
}




/** \brief Some text.
 *  \return description
 *
 * Detailed description.
 *
 * \bug writeme
 */
int
rig_data_has_set_preamp     ()
{
	return has_set.preamp;
}



/** \brief Get lower RIT limit.
 *  \return The current lower RIT limit.
 *
 * This function returns the lower RIT limit which applies to
 * the current mode.
 */
shortfreq_t
rig_data_get_ritmin     ()
{
	return -get.ritmax;
}



/** \brief Get upper RIT limit.
 *  \return The current upper RIT limit.
 *
 * This function returns the upper RIT limit which applies to
 * the current mode.
 */
shortfreq_t
rig_data_get_ritmax     ()
{
	return get.ritmax;
}



/** \brief Get RIT tuning step.
 *  \return The current RIT tuning step.
 *
 * This function returns the RIT tuning step corresponding to the
 * current mode.
 */
shortfreq_t
rig_data_get_ritstep    ()
{
	return get.ritstep;
}



/** \brief Get lower XIT limit.
 *  \return The current lower XIT limit.
 *
 * This function returns the lower XIT limit which applies to
 * the current mode.
 */
shortfreq_t
rig_data_get_xitmin     ()
{
	return -get.xitmax;
}



/** \brief Get upper XIT limit.
 *  \return The current upper XIT limit.
 *
 * This function returns the upper XIT limit which applies to
 * the current mode.
 */
shortfreq_t
rig_data_get_xitmax     ()
{
	return get.xitmax;
}



/** \brief Get XIT tuning step.
 *  \return The current XIT tuning step.
 *
 * This function returns the XIT tuning step corresponding to the
 * current mode.
 */
shortfreq_t
rig_data_get_xitstep    ()
{
	return get.xitstep;
}


/***   FUNC  ***/
int
rig_data_has_set_func (setting_t func)
{
	return has_set.funcs[rig_setting2idx(func)];
}


int
rig_data_has_get_func (setting_t func)
{
	return has_get.funcs[rig_setting2idx(func)];
}


void
rig_data_set_func     (setting_t func, int status)
{
	set.funcs[rig_setting2idx(func)] = status;
	new.funcs[rig_setting2idx(func)] = 1;
}


int
rig_data_get_func     (setting_t func)
{
	return get.funcs[rig_setting2idx(func)];
}

/***   LOCK  ***/
int
rig_data_has_set_lock ()
{
	return has_set.lock;
}


int
rig_data_has_get_lock ()
{
	return has_get.lock;
}


void
rig_data_set_lock     (int lock)
{
	set.lock = lock;
	new.lock = 1;
}


int
rig_data_get_lock     ()
{
	return get.lock;
}


/* VFO TOGGLE */
int
rig_data_has_vfo_op_toggle ()
{
	return has_set.vfo_op_toggle;
}


void
rig_data_vfo_op_toggle     ()
{
	set.vfo_op_toggle = 1;
	new.vfo_op_toggle = 1;
}


/* VFO COPY */
int
rig_data_has_vfo_op_copy ()
{
	return has_set.vfo_op_copy;
}


void
rig_data_vfo_op_copy     ()
{
	set.vfo_op_copy = 1;
	new.vfo_op_copy = 1;
}


/* VFO EXCHANGE */
int
rig_data_has_vfo_op_xchg ()
{
	return has_set.vfo_op_xchg;
}


void
rig_data_vfo_op_xchg     ()
{
	set.vfo_op_xchg = 1;
	new.vfo_op_xchg = 1;
}


/* set SPLIT ON/OFF */
int
rig_data_has_set_split ()
{
	return has_set.split;
}

int
rig_data_has_get_split ()
{
	return has_get.split;
}

void
rig_data_set_split (int split)
{
	if (split)
		set.split = RIG_SPLIT_ON;
	else
		set.split = RIG_SPLIT_OFF;

	new.split = TRUE;
}

int
rig_data_get_split ()
{
	return (get.split == RIG_SPLIT_ON ? 1 : 0);
}



/** \brief Get address of 'get' variable.
 *  \return A pointer to the shared data.
 *
 * This function is used to obtain the address of the 'get' global data.
 * This is primarly used by the radio daemon for fast access to the data
 * structure.
 */
grig_settings_t  *
rig_data_get_get_addr ()
{
	return &get;
}



/** \brief Get address of 'set' variable.
 *  \return A pointer to the shared data.
 *
 * This function is used to obtain the address of the 'set' global data.
 * This is primarly used by the radio daemon for fast access to the data
 * structure.
 */
grig_settings_t  *
rig_data_get_set_addr ()
{
	return &set;
}



/** \brief Get address of 'new' variable.
 *  \return A pointer to the shared data.
 *
 * This function is used to obtain the address of the 'new' global data.
 * This is primarly used by the radio daemon for fast access to the data
 * structure.
 */
grig_cmd_avail_t *
rig_data_get_new_addr ()
{
	return &new;
}



/** \brief Get address of 'has_set' variable.
 *  \return A pointer to the shared data.
 *
 * This function is used to obtain the address of the 'has_set' global data.
 * This is primarly used by the radio daemon for fast access to the data
 * structure.
 */
grig_cmd_avail_t *
rig_data_get_has_set_addr ()
{
	return &has_set;
}



/** \brief Get address of 'has_get' variable.
 *  \return A pointer to the shared data.
 *
 * This function is used to obtain the address of the 'has_get' global data.
 * This is primarly used by the radio daemon for fast access to the data
 * structure.
 */
grig_cmd_avail_t *
rig_data_get_has_get_addr ()
{
	return &has_get;
}



/** \brief Get the modes bitfield */
int
rig_data_get_all_modes    ()
{
	return get.allmodes;
}


/** \brief Get the antenna bitfield */
int
rig_data_get_all_antennas    ()
{
	return get.allantennas;
}


/** \brief Store tha maximum RF power level */
void
rig_data_set_max_rfpwr (float maxpow)
{
	maxpwr = maxpow;
}


/** \brief Get maximum RF power level */
float
rig_data_get_max_rfpwr ()
{
	return maxpwr;
}


/* AF gain */
int
rig_data_has_get_afg (void)
{
	return has_get.afg;
}

int
rig_data_has_set_afg (void)
{
	return has_set.afg;
}

float
rig_data_get_afg     (void)
{
	return get.afg;
}

void
rig_data_set_afg     (float afg)
{
	set.afg = afg;
	get.afg = afg;
	new.afg = TRUE;
}


/* RF gain */
int
rig_data_has_get_rfg (void)
{
	return has_get.rfg;
}

int
rig_data_has_set_rfg (void)
{
	return has_set.rfg;
}

float
rig_data_get_rfg     (void)
{
	return get.rfg;
}

void
rig_data_set_rfg     (float rfg)
{
	set.rfg = rfg;
	get.rfg = rfg;
	new.rfg = TRUE;
}


/* SQL */
int
rig_data_has_get_sql (void)
{
	return has_get.sql;
}

int
rig_data_has_set_sql (void)
{
	return has_set.sql;
}

float
rig_data_get_sql     (void)
{
	return get.sql;
}

void
rig_data_set_sql     (float sql)
{
	set.sql = sql;
	get.sql = sql;
	new.sql = TRUE;
}


/* IF shift */
int
rig_data_has_get_ifs (void)
{
	return has_get.ifs;
}

int
rig_data_has_set_ifs (void)
{
	return has_set.ifs;
}

int
rig_data_get_ifs     (void)
{
	return get.ifs;
}

void
rig_data_set_ifs     (int ifs)
{
	set.ifs = ifs;
	get.ifs = ifs;
	new.ifs = TRUE;
}

shortfreq_t
rig_data_get_ifsmax     ()
{
	return get.ifsmax;
}

shortfreq_t
rig_data_get_ifsstep    ()
{
	return get.ifsstep;
}


/* APF */
int
rig_data_has_get_apf (void)
{
	return has_get.apf;
}

int
rig_data_has_set_apf (void)
{
	return has_set.apf;
}

float
rig_data_get_apf     (void)
{
	return get.apf;
}

void
rig_data_set_apf     (float apf)
{
	set.apf = apf;
	get.apf = apf;
	new.apf = TRUE;
}


/* NR */
int
rig_data_has_get_nr (void)
{
	return has_get.nr;
}

int
rig_data_has_set_nr (void)
{
	return has_set.nr;
}

float rig_data_get_nr     (void)
{
	return get.nr;
}

void  rig_data_set_nr     (float nr)
{
	set.nr = nr;
	get.nr = nr;
	new.nr = TRUE;
}
	

/* Notch */
int
rig_data_has_get_notch (void)
{
	return has_get.notch;
}

int
rig_data_has_set_notch (void)
{
	return has_set.notch;
}

int
rig_data_get_notch     (void)
{
	return get.notch;
}

void
rig_data_set_notch     (int notch)
{
	set.notch = notch;
	get.notch = notch;
	new.notch = TRUE;
}


/* PBT in */
int
rig_data_has_get_pbtin (void)
{
	return has_get.pbtin;
}

int
rig_data_has_set_pbtin (void)
{
	return has_set.pbtin;
}

float
rig_data_get_pbtin     (void)
{
	return get.pbtin;
}

void
rig_data_set_pbtin     (float pbt)
{
	set.pbtin = pbt;
	get.pbtin = pbt;
	new.pbtin = TRUE;
}


/* PBT out */
int
rig_data_has_get_pbtout (void)
{
	return has_get.pbtout;
}

int
rig_data_has_set_pbtout (void)
{
	return has_set.pbtout;
}

float
rig_data_get_pbtout     (void)
{
	return get.pbtout;
}

void
rig_data_set_pbtout     (float pbt)
{
	set.pbtout = pbt;
	get.pbtout = pbt;
	new.pbtout = TRUE;
}

/* CW pitch */
int
rig_data_has_get_cwpitch (void)
{
	return has_get.cwpitch;
}

int
rig_data_has_set_cwpitch (void)
{
	return has_set.cwpitch;
}

int
rig_data_get_cwpitch     (void)
{
	return get.cwpitch;
}

void
rig_data_set_cwpitch     (int cwp)
{
	set.cwpitch = cwp;
	get.cwpitch = cwp;
	new.cwpitch = TRUE;
}


/* keyer speed */
int
rig_data_has_get_keyspd (void)
{
	return has_get.keyspd;
}

int
rig_data_has_set_keyspd (void)
{
	return has_set.keyspd;
}

int
rig_data_get_keyspd     (void)
{
	return get.keyspd;
}

void
rig_data_set_keyspd     (int keyspd)
{
	set.keyspd = keyspd;
	get.keyspd = keyspd;
	new.keyspd = TRUE;
}

/* break-in delay */
int
rig_data_has_get_bkindel (void)
{
	return has_get.bkindel;
}

int
rig_data_has_set_bkindel (void)
{
	return has_set.bkindel;
}

int
rig_data_get_bkindel     (void)
{
	return get.bkindel;
}

void
rig_data_set_bkindel     (int bkindel)
{
	set.bkindel = bkindel;
	get.bkindel = bkindel;
	new.bkindel = TRUE;
}


/* balance */
int
rig_data_has_get_balance (void)
{
	return has_get.balance;
}

int
rig_data_has_set_balance (void)
{
	return has_set.balance;
}

float
rig_data_get_balance     (void)
{
	return get.balance;
}

void
rig_data_set_balance     (float bal)
{
	set.balance = bal;
	get.balance = bal;
	new.balance = TRUE;
}

/* VOX delay */
int
rig_data_has_get_voxdel (void)
{
	return has_get.voxdel;
}

int
rig_data_has_set_voxdel (void)
{
	return has_set.voxdel;
}

int
rig_data_get_voxdel     (void)
{
	return get.voxdel;
}

void
rig_data_set_voxdel     (int voxdel)
{
	set.voxdel = voxdel;
	get.voxdel = voxdel;
	new.voxdel = TRUE;
}

/* VOX gain */
int
rig_data_has_get_voxg (void)
{
	return has_get.voxg;
}

int
rig_data_has_set_voxg (void)
{
	return has_set.voxg;
}

float
rig_data_get_voxg     (void)
{
	return get.voxg;
}

void
rig_data_set_voxg     (float voxg)
{
	set.voxg = voxg;
	get.voxg = voxg;
	new.voxg = TRUE;
}

/* anti VOX */
int
rig_data_has_get_antivox (void)
{
	return has_get.antivox;
}

int
rig_data_has_set_antivox (void)
{
	return has_set.antivox;
}

float
rig_data_get_antivox     (void)
{
	return get.antivox;
}

void
rig_data_set_antivox     (float antivox)
{
	set.antivox = antivox;
	get.antivox = antivox;
	new.antivox = TRUE;
}

/* MIC gain */
int
rig_data_has_get_micg (void)
{
	return has_get.micg;
}

int
rig_data_has_set_micg (void)
{
	return has_set.micg;
}

float
rig_data_get_micg     (void)
{
	return get.micg;
}

void
rig_data_set_micg     (float micg)
{
	set.micg = micg;
	get.micg = micg;
	new.micg = TRUE;
}

/* compression */
int
rig_data_has_get_comp (void)
{
	return has_get.comp;
}

int
rig_data_has_set_comp (void)
{
	return has_set.comp;
}

float
rig_data_get_comp     (void)
{
	return get.comp;
}

void
rig_data_set_comp     (float comp)
{
	set.comp = comp;
	get.comp = comp;
	new.comp = TRUE;
}


