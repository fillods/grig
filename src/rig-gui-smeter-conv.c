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
/** \file rig-gui-smeter-conv.c
 *  \ingroup smeter
 *  \brief Coordinate conversion utilities.
 *
 * This file contains functions to convert the signal strength to canvas
 * coordinates. Basically, we need to do two conversions: From dB to
 * needle angle and from needle angle to (x,y) coordinates. Although both
 * of these conversions can be done using one function, it has been
 * decided to implement them separately in order to allow the clling
 * function to adjust the needle angle for various corrections (faalback
 * delay, etc.)
 *
 * \bug  The conversion functions depend on the physical size of the smeter
 *       pixmap. The corresponding constant must therefore be updated if
 *       the pixmap size changes.
 */
#include <gtk/gtk.h>
#include <math.h>
#include "rig-gui-smeter-conv.h"



/** \brief Coefficient for converting dB to deg (linear). */
#define RIG_DB_TO_DEG_A  1.0966

/** \brief Offset for converting dB to deg (linear). */
#define RIG_DB_TO_DEG_B  100.934


/** \brief 3. order coefficient for converting dB to deg (polynomial). */
#define RIG_DB_TO_DEG_POLY_A   -0.000103582

/** \brief 2. order coefficient for converting dB to deg (polynomial). */
#define RIG_DB_TO_DEG_POLY_B   -0.00289134

/** \brief 1. order coefficient for converting dB to deg (polynomial). */
#define RIG_DB_TO_DEG_POLY_C    1.2021

/** \brief Offset for converting dB to deg (polynomial). */
#define RIG_DB_TO_DEG_POLY_D  102.033


/** \brief Coefficient for converting [0.0;1.0] to deg (linear). */
#define RIG_VALF_TO_DEG_A  0.0

/** \brief Offset for converting [0.0;1.0] to deg (linear). */
#define RIG_VALF_TO_DEG_B  0.0


/** \brief 3. order coefficient for converting [0.0;1.0] to deg (polynomial). */
#define RIG_VALF_TO_DEG_POLY_A   0.0

/** \brief 2. order coefficient for converting [0.0;1.0] to deg (polynomial). */
#define RIG_VALF_TO_DEG_POLY_B   0.0

/** \brief 1. order coefficient for converting [0.0;1.0] to deg (polynomial). */
#define RIG_VALF_TO_DEG_POLY_C   0.0

/** \brief Offset for converting [0.0;1.0] to deg (polynomial). */
#define RIG_VALF_TO_DEG_POLY_D   0.0



/** \brief Pixmap width divide by 2. */
#define PIXMAP_HALF_WIDTH  80

/** \brief Pixmap height. */
#define PIXMAP_HEIGHT 80

/** \brief Needle length. */
#define NEEDLE_LENGTH 105

/** \brief Distance from pixmap bottom to fix point of the needle. */
#define NEEDLE_VERTEX 33

#ifndef PI
#define PI 3.141592653
#endif

/** \brief Convert signal strength in dB to needle angle.
 *  \param db   The signalstrength as received from hamlib.
 *  \param mode The mode specifying whether data from linear
 *               or polynomial fit should be used.
 *  \return The needle angle in dgrees.
 *
 * This function convertsthe signal strength in dB, as received from hamlib,
 * to the needle angle. The valid range in -54..30, with -54dB corresponding to
 * S0 and 30dB coresponding to S9+30. Values outside range will be truncated to
 * the corresponding limit.
 * \verbatim
         S    dB   deg
         S0  -54   45.00
         S1  -48   48.85
         S2  -42   54.64
         S3  -36   60.21
         S4  -30   65.96
         S5  -24   72.03
         S6  -18   80.36
         S7  -12   86.36
         S8   -6   95.00
         S9    0  103.95
        +10   10  113.71
        +20   20  122.31
        +30   30  133.48
     \endverbatim
 * The linear fit to this data results in:
 * \verbatim
       a   = 1.0966           +/- 0.02087      (1.903%)
       b   = 100.934          +/- 0.625        (0.6192%)

correlation matrix of the fit parameters:

                a      b      
       a      1.000 
       b      0.539  1.000 
   \endverbatim
 * while 3. order polynomial fit gives:
 * \verbatim
     f(x) = a*x**3 + b*x**2 + c*x + d

       a   = -0.000103582     +/- 2.175e-05    (21%)
       b   = -0.00289134      +/- 0.0009361    (32.38%)
       c   = 1.2021           +/- 0.02398      (1.995%)
       d   = 102.033          +/- 0.5287       (0.5181%)

   correlation matrix of the fit parameters:

                      a      b      c      d      
       a               1.000 
       b               0.861  1.000 
       c              -0.710 -0.351  1.000 
       d              -0.617 -0.752  0.388  1.000 

   \endverbatim
 */
gfloat
convert_db_to_angle    (gint db, db_to_angle_mode_t mode)
{
	gfloat fdb;

	/* ensure that input is within range */
	if (db < -54) {
		db = -54;
	}
	else if (db > 30) {
		db = 30;
	}

	fdb = (gfloat) db;

	/* calculate angle according to selected mode */
	if (mode == DB_TO_ANGLE_MODE_LINEAR) {
		return (gfloat) (RIG_DB_TO_DEG_A*fdb + RIG_DB_TO_DEG_B);
	}

	else if (mode == DB_TO_ANGLE_MODE_POLY) {
		return (gfloat) (RIG_DB_TO_DEG_POLY_A*fdb*fdb*fdb +
				 RIG_DB_TO_DEG_POLY_B*fdb*fdb   +
				 RIG_DB_TO_DEG_POLY_C*fdb     +
				 RIG_DB_TO_DEG_POLY_D);
	}

	else {
		return 0.0;
	}
}




/** \brief Convert val.f type [0.0;1.0] to needle angle.
 *  \param valf  The floating point value as received from hamlib.
 *  \param mode The mode specifying whether data from linear
 *               or polynomial fit should be used.
 *  \return The needle angle in dgrees.
 *
 * This function converts a floating point number within the range [0.0;1.0],
 * as received from hamlib,
 * to the needle angle. Values outside the valid range will be truncated to
 * the corresponding limit.
 * \verbatim
         S    fp   deg
         S0        45.00
         S1        48.85
         S2        54.64
         S3        60.21
         S4        65.96
         S5        72.03
         S6        80.36
         S7        86.36
         S8        95.00
         S9       103.95
        +10       113.71
        +20       122.31
        +30       133.48
     \endverbatim
 * The linear fit to this data results in:
 * \verbatim


   \endverbatim
 * while 3. order polynomial fit gives:
 * \verbatim
     f(x) = a*x**3 + b*x**2 + c*x + d


   \endverbatim
 */
gfloat
convert_valf_to_angle    (gfloat valf, db_to_angle_mode_t mode)
{

	/* ensure that input is within range */
	if (valf < 0.0) {
		valf = 0.0;
	}
	else if (valf > 1.0) {
		valf = 1.0;
	}


	/* calculate angle according to selected mode */
	if (mode == DB_TO_ANGLE_MODE_LINEAR) {
		return (gfloat) (RIG_VALF_TO_DEG_A*valf + RIG_VALF_TO_DEG_B);
	}

	else if (mode == DB_TO_ANGLE_MODE_POLY) {
		return (gfloat) (RIG_VALF_TO_DEG_POLY_A*valf*valf*valf +
				 RIG_VALF_TO_DEG_POLY_B*valf*valf      +
				 RIG_VALF_TO_DEG_POLY_C*valf           +
				 RIG_VALF_TO_DEG_POLY_D);
	}

	else {
		return 0.0;
	}
}





/** \brief Convert needle angle to canvas coordinates.
 *  \param angle The needle angle.
 *  \param coor  Coordinate structurewhere the result is stored.
 *
 *  This function converts the needle angle and calculates the two (x,y)
 *  cordinates necessary to draw the needle on the canvas. In order to do
 *  this the size of the canvas and information about the pixmap is needed.
 *  These are given byconstants in this file and must be adjustedin case
 *  of a new pixmap.
 */
void
convert_angle_to_rect  (gfloat angle, coordinate_t *coor)
{
	gfloat rad;
	gfloat s,c;

	/* numerical protection: 0 < angle < 180 */
	if (!(0.0 < angle) || !(angle < 180.0)) {
		angle = 90.0;
	}

	if (angle <= 90.0) {

		/* convert angle to radians */
		rad = PI * angle / 180.0;

		/* calculate sin and cos */
		s = sin (rad);
		c = cos (rad);

		coor->x2 = PIXMAP_HALF_WIDTH - NEEDLE_VERTEX * sqrt (1/(s*s) - 1);
		coor->y2 = PIXMAP_HEIGHT;

		coor->x1 = coor->x2 - (NEEDLE_LENGTH - NEEDLE_VERTEX/s) * c;
		coor->y1 = PIXMAP_HEIGHT + NEEDLE_VERTEX - NEEDLE_LENGTH * s;
	}
	
	else {

		angle = angle - 90.0;

		/* convert angle to radians */
		rad = PI * angle / 180.0;

		/* calculate sin and cos */
		s = sin (rad);
		c = cos (rad);

		coor->x2 = PIXMAP_HALF_WIDTH + NEEDLE_VERTEX * sqrt (1/(c*c) - 1);
		coor->y2 = PIXMAP_HEIGHT;

		coor->x1 = coor->x2 + (NEEDLE_LENGTH - NEEDLE_VERTEX/c) * s;
		coor->y1 = PIXMAP_HEIGHT + NEEDLE_VERTEX - NEEDLE_LENGTH * c;
	}

}
