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


/** \brief Set VFO.
 *  \param vfo The new VFO.
 *
 * This function sets the targeted VFO, ie. active VFO to vfo.
 */
void
rig_data_set_vfo     (vfo_t vfo)
{
	set.vfo = vfo;
	get.vfo = vfo;
	new.vfo = 1;
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


/** \brief Get current VFO.
 *  \return The currently selected VFO.
 *
 * This function returns the currently selected VFO.
 */
vfo_t
rig_data_get_vfo     ()
{
	return get.vfo;
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

