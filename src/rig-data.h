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

/**  \file    rig-data.h
 *   \ingroup shdata
 *   \brief   Data type definitions for shared rig data.
 *
 * This file containes the data type definitions for the shared rig data
 * object.
 *
 */

#ifndef RIG_DATA_H
#define RIG_DATA_H 1


#include <hamlib/rig.h>

/** \brief Grig representation of passband widths.
 *
 * Grig has to keep it's own passband values due to the fact
 * that the passband width is varying with the mode and the
 * GUI is not supposed to poll hamlib in order to have the
 * correct numerical value of the passband width. Therefore we
 * use this representation and the daemon will convert it while
 * talking to hamlib.
 */
typedef enum rig_data_pbw_e {
	RIG_DATA_PB_WIDE = 0,   /**!< Passband corresponding to wide */
	RIG_DATA_PB_NORMAL,     /**!< Passband corresponding to normal */
	RIG_DATA_PB_NARROW      /**!< Passband corresponding to narrow */
} rig_data_pbw_t;


/** \brief Structure representing rig settings
 *
 * This structure is used to hold rig settings (frequency, mode, vfo, etc).
 * One structure holds the values commanded by the user while the other
 * holds the actual settings obtained from the rig.
 */ 
typedef struct {
	powerstat_t      pstat;   /*!< Power status (ON/OFF). */
	ptt_t            ptt;     /*!< PTT status (ON/OFF). */
	vfo_t            vfo;     /*!< VFO. */
	rmode_t          mode;    /*!< Mode. */
	rig_data_pbw_t   pbw;     /*!< Passband width. */
	freq_t           freq1;   /*!< Primary (working) frequency. */
	freq_t           freq2;   /*!< Secondary frequency. */
	shortfreq_t      rit;     /*!< RIT. */
	shortfreq_t      xit;     /*!< XIT. */
	float            power;   /*!< TX power. */
	int              agc;     /*!< AGC level. */

	/* read only fields */
	int         strength; /*!< Signal strength. */
	float       swr;      /*!< SWR. */
	float       alc;      /*!< ALC. */
} grig_settings_t;


typedef struct {
	int         pstat;
	int         ptt;
	int         vfo;
	int         mode;
	int         pbw;
	int         freq1;
	int         freq2;
	int         rit;
	int         xit;
	int         power;
	int         agc;

	/* read only fields */
	int         strength;
	int         swr;
	int         alc;
} grig_cmd_avail_t;


#define GRIG_LEVEL_RD (RIG_LEVEL_RFPOWER | RIG_LEVEL_AGC | RIG_LEVEL_SWR | RIG_LEVEL_ALC | RIG_LEVEL_STRENGTH)
#define GRIG_LEVEL_WR (RIG_LEVEL_RFPOWER | RIG_LEVEL_AGC)

/* init functions */
void rig_data_init        (void);
void rig_data_free        (void);
int  rig_data_initialized (void);

/* set functions */
void rig_data_set_pstat   (powerstat_t);
void rig_data_set_ptt     (ptt_t);
void rig_data_set_vfo     (vfo_t);
void rig_data_set_mode    (rmode_t);
void rig_data_set_pbwidth (rig_data_pbw_t);
void rig_data_set_freq    (int, freq_t);
void rig_data_set_rit     (shortfreq_t);
void rig_data_set_xit     (shortfreq_t);
void rig_data_set_agc     (int);

/* get functions */
powerstat_t      rig_data_get_pstat   (void);
ptt_t            rig_data_get_ptt     (void);
vfo_t            rig_data_get_vfo     (void);
rmode_t          rig_data_get_mode    (void);
rig_data_pbw_t   rig_data_get_pbwidth (void);
freq_t           rig_data_get_freq    (int);
shortfreq_t      rig_data_get_rit     (void);
shortfreq_t      rig_data_get_xit     (void);
int              rig_data_get_agc     (void);

/* address acquisition functions */
grig_settings_t  *rig_data_get_get_addr     (void);
grig_settings_t  *rig_data_get_set_addr     (void);
grig_cmd_avail_t *rig_data_get_new_addr     (void);
grig_cmd_avail_t *rig_data_get_has_set_addr (void);
grig_cmd_avail_t *rig_data_get_has_get_addr (void);

#endif
