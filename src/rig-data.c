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
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <hamlib/rig.h>
#include "rig-data.h"


grig_settings_t  *set = NULL;
grig_settings_t  *get = NULL;
grig_cmd_avail_t *new = NULL;


/** \brief Initialized shared data.
 *
 * This function allocates memory to the shared data structures
 * and initializes them to zeros. 
 */
void
rig_data_init ()
{
	/* initialize target settings */
	if (set != NULL) {
		g_free (set);
	}
	set = g_new0 (grig_settings_t, 1);

	/* initialize acquired settings */
	if (get != NULL) {
		g_free (get);
	}
	get = g_new0 (grig_settings_t, 1);

	/* initialize target availability record */
	if (new != NULL) {
		g_free (new);
	}
	new = g_new0 (grig_cmd_avail_t, 1);
}


/** \brief Clean up shared data.
 *
 * This function deallocates the memory used by the shared rig data.
 */
void
rig_data_free ()
{
	g_free (set);
	set = NULL;

	g_free (get);
	get = NULL;

	g_free (new);
	new = NULL;
}


/** \brief Check whether shared data has been initialized.
 *  \return 1 if all the data is initialized 0 otherwise.
 *
 * This function checks whether the shared data structures have been initialized
 * or not.
 */
int
rig_data_initialized ()
{
	return ((set != NULL) && (get != NULL) && (new != NULL));
}



/** \brief Set power status.
 *  \param pwr The new power status.
 *
 * This function sets the targeted power status to pwr.
 */
void 
rig_data_set_power   (powerstat_t pwr)
{
	set->power = pwr;
	new->power = 1;
}



/** \brief Set PTT status.
 *  \param ptt The new PTT status.
 *
 * This function sets the targeted PTT status to ptt.
 */
void
rig_data_set_ptt     (ptt_t ptt)
{
	set->ptt = ptt;
	new->ptt = 1;
}


/** \brief Set VFO.
 *  \param vfo The new VFO.
 *
 * This function sets the targeted VFO, ie. active VFO to vfo.
 */
void
rig_data_set_vfo     (vfo_t vfo)
{
	set->vfo = vfo;
	new->vfo = 1;
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
	set->mode = mode;
	new->mode = 1;
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
rig_data_set_pbwidth (pbwidth_t pbw)
{
	set->pbw = pbw;
	new->pbw = 1;
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
	case 1: set->freq1 = freq;
		new->freq1 = 1;
		break;

		/* secondary frequency */
	case 2: set->freq2 = freq;
		new->freq2 = 1;
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
	set->rit = rit;
	new->rit = 1;
}


/** \brief Set XIT offset.
 *  \param rit The new XIT offset.
 *
 * This function sets the targeted XIT offset to xit.
 */
void
rig_data_set_xit     (shortfreq_t xit)
{
	set->xit = xit;
	new->xit = 1;
}


/** \brief Get power status.
 *  \return The current power status.
 *
 * This function returns the current power status.
 */
powerstat_t
rig_data_get_power   ()
{
	return get->power;
}


/** \brief Get PTT status.
 *  \return The current PTT status.
 *
 * This function returns the current PTT status.
 */
ptt_t
rig_data_get_ptt     ()
{
	return get->ptt;
}


/** \brief Get current VFO.
 *  \return The currently selected VFO.
 *
 * This function returns the currently selected VFO.
 */
vfo_t
rig_data_get_vfo     ()
{
	return get->vfo;
}


/** \brief Get current mode.
 *  \return The current mode.
 *
 * This function returns the current mode.
 */
rmode_t
rig_data_get_mode    ()
{
	return get->mode;
}



/** \brief Get current passband width.
 *  \returns The current passband width.
 *
 * This function returns the current passband width.
 */
pbwidth_t
rig_data_get_pbwidth ()
{
	return get->pbw;
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
	case 1: return get->freq1;
		break;

		/* secondary frequenct */
	case 2: return get->freq2;
		break;

		/* bug */
	default: g_warning (_("%s: Invalid target: %d\n"), __FUNCTION__, num);
		return get->freq1;
		break;
	}
}


/** \brief Get RIT offset.
 *  \return The current value of the RIT offset.
 *
 * This function returns the current value of the RIT offset.
 */
shortfreq_t
rig_data_get_rit     ()
{
	return get->rit;
}


/** \brief Get XIT offset.
 *  \return The current value of the XIT offset.
 *
 * This function returns the current value of the XIT offset.
 */
shortfreq_t
rig_data_get_xit     ()
{
	return get->xit;
}

