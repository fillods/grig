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
/** \file rig-gui-smeter-conv.h
 *  \ingroup smeter
 *  \brief Coordinate conversion utilities (interface).
 */
#ifndef RIG_GUI_SMETER_CONV_H
#define RIG_GUI_SMETER_CONV_H 1

/** \brief Structure used to obtained coordinates in one pass. */
typedef struct {
	gfloat x1;   /*!< X1 coordinate; upper left  */
	gfloat y1;   /*!< Y1 coordinate; upper left  */
	gfloat x2;   /*!< X2 coordinate; lower right */
	gfloat y2;   /*!< Y2 coordinate; lower right */
} coordinate_t;


/** \brief Enumeration for specifying the conversion mode (linear or polynomial
 *         fit) when converting from dB to angle.
 */
typedef enum {
	DB_TO_ANGLE_MODE_LINEAR = 0,   /*!< Use data from linear fit */
	DB_TO_ANGLE_MODE_POLY   = 1    /*!< Use data from 3. degree polynomial fit */
} db_to_angle_mode_t;



/* conversions for signal strength */
gfloat  convert_db_to_angle    (gint db, db_to_angle_mode_t mode);

/* conversion of 0.0 ... 1.0 float to angle */
gfloat  convert_valf_to_angle  (gfloat valf);

/* conversion of angle to rectangular coordinate set */
void    convert_angle_to_rect  (gfloat angle, coordinate_t *coor);




#endif
