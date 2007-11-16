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
/** \file rig-gui-smeter.h
 *  \ingroup smeter
 *  \brief Signal strength meter widget (interface).
 *
 * \bug The defaults and ranges defined in this files shall be moved
 *      to a centralized object.
 */
#ifndef RIG_GUI_SMETER_H
#define RIG_GUI_SMETER_H 1



/* These constants are kept public to allow their usage in e.g. adjustment ranges */

/** \brief Minimum delay in msec between s-meter updates (50 fps) */
#define RIG_GUI_SMETER_MIN_TVAL 20

/** \brief Default delay in msec between s-meter updates (25 fps). */
#define RIG_GUI_SMETER_DEF_TVAL 40

/** \brief Maximum delay in msec between s-meter updates (5 fps). */
#define RIG_GUI_SMETER_MAX_TVAL 500


/** \brief Minimum falloff speed in deg/sec */
#define RIG_GUI_SMETER_MIN_FALLOFF 10.0

/** \brief Default falloff speed in deg/sec */
#define RIG_GUI_SMETER_DEF_FALLOFF 200.0

/** \brief Maximum falloff speed in deg/sec */
#define RIG_GUI_SMETER_MAX_FALLOFF 500.0


/** \brief Scale setting for s-meter.
 *
 * The s-meter has 3 scales: The upper scale which is used to show the
 * received signal strength and two lower scales which can be used to
 * show transmitter parameters (power, alc, etc.). One of the lower
 * scales has range 0..5 while the other has range 0..10. This enumeration
 * is used to select one of these two ranges.
 */
typedef enum {
	SMETER_SCALE_5 = 0,     /*!< Use scale 0..5 */
	SMETER_SCALE_10,        /*!< Use scale 0..10 */
	SMETER_SCALE_50,        /*!< Use scale 0..50 */
	SMETER_SCALE_100,       /*!< Use scale 0..100 */
	SMETER_SCALE_500,       /*!< Use scale 0..500 */
	SMETER_SCALE_LAST       /*!< Dummy...         */
} smeter_scale_t;



/** \brief TX mode setting.
 *
 * These valuesare used to select the meter isplay mode when the rig is
 * in TX mode.
 */
typedef enum {
	SMETER_TX_MODE_NONE = 0,       /*!< No display in TX mode. */
	SMETER_TX_MODE_POWER,          /*!< Show TX power.         */
	SMETER_TX_MODE_SWR,            /*!< Show SWR.              */
	SMETER_TX_MODE_ALC,            /*!< Show ALC level.        */
	SMETER_TX_MODE_LAST            /*!< Dummy...               */
} smeter_tx_mode_t;


/** \brief Data type for signal strength meter.
 *
 * This structure is used to store the data for the signal strength
 * meter. The signal strength meter is a GnomeCanvas having a background
 * pixmap and a needle. The data structure also hols some numerical values
 * needed to calculate the dynamic behaviour of the needle.
 */
typedef struct {
	GtkWidget              *canvas;      /*!< The drawing area widget. */
	GdkPixbuf              *pixbuf;      /*!< The background pixmap.   */
	GdkGC                  *gc;          /*!< Graphics context for drawing. */
	gboolean                exposed;     /*!< Flag to indicate whether canvas is ready. */
	gfloat                  value;       /*!< Current value (angle).   */
	gfloat                  lastvalue;   /*!< Previous value (angle).  */
	guint                   tval;        /*!< Current update delay.    */
	gfloat                  falloff;     /*!< Current falloff delay.   */
	smeter_scale_t          scale;       /*!< Current scale.           */
	smeter_tx_mode_t        txmode;      /*!< Display mode in TX.      */
} smeter_t;


GtkWidget        *rig_gui_smeter_create (void);
smeter_tx_mode_t  rig_gui_smeter_get_tx_mode (void);


#endif


