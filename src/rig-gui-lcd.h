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
/** \file rig-gui-lcd.h
 *  \ingroup lcd
 *  \brief LCD display (interface).
 *
 * \bug The defaults defined in this file shall be moved to a centralized object.
 */
#ifndef RIG_GUI_LCD_H
#define RIG_GUI_LCD_H 1


/* default LCD color components.  We kep them public so that the config
   module can access them.
*/

/** \brief Red component of the default background (104 in 8bit). */
#define LCD_BG_DEFAULT_RED    26749

/** \brief Green component of the default background (189 in 8 bit). */
#define LCD_BG_DEFAULT_GREEN  48544

/** \brief Blue componentof te default background (226 in 8 bit). */
#define LCD_BG_DEFAULT_BLUE   57996


/** \brief Red component of thedefult foreground(40 in 8 bit). */
#define LCD_FG_DEFAULT_RED    10280

/** \brief Green component of the default foreground (85 in 8 bit). */
#define LCD_FG_DEFAULT_GREEN  21845

/** \brief Blue component of the default foreground (129 in 8 bit). */
#define LCD_FG_DEFAULT_BLUE   33153


/** \brief Coordinate structure for digits. */
typedef struct {
	guint x;     /*!< X coordinate. */
	guint y;     /*!< Y coordinate. */
} lcd_coor_t;


/** \brief LCD data structure. */
typedef struct {
	GtkWidget        *canvas;          /*!< The main canvas. */
	GdkGC            *gc1;             /*!< Graphics context (normal). */
	GdkGC            *gc2;             /*!< Graphics context (inverted). */
	guint             width;           /*!< Canvas width. */
	guint             height;          /*!< Canvas height. */
	lcd_coor_t        digits[15];      /*!< Starting points for all digits and commas. */
	guint             dlw;             /*!< Width of large digits. */
	guint             dlh;             /*!< Height of large digits. */
	guint             clw;             /*!< Width of large separator. */
	guint             dsw;             /*!< Width of small digits. */
	guint             dsh;             /*!< Height of small digits. */
	guint             csw;             /*!< Width of small separator. */
	GdkColor          bg;              /*!< Background color. */
	GdkColor          fg;              /*!< Foreground color. */
	gboolean          exposed;         /*!< Exposed flag. */

	gdouble           main_freq;       /*!< Main frequency value. */
	gdouble           sub_freq;        /*!< Secondary frequency value. */
} lcd_t;


GtkWidget *rig_gui_lcd_create (void);


#endif
