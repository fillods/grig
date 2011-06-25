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
/** \file rig-gui-lcd.h
 *  \ingroup lcd
 *  \brief LCD display (interface).
 *
 * \bug The defaults defined in this file shall be moved to a centralized object.
 */
#ifndef RIG_GUI_LCD_H
#define RIG_GUI_LCD_H 1

#include <hamlib/rig.h>


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


#define RIG_GUI_LCD_DEF_TVAL 400

/** \brief Coordinate structure for digits. */
typedef struct {
	guint x;     /*!< X coordinate. */
	guint y;     /*!< Y coordinate. */
} lcd_coor_t;



/** \brief LCD data structure.
 *
 * Other elements:
 *
 *   0    first dot
 *   1    second dot
 *   2    third (small) dot.
 *   3    RIT sign
 *   4    RIT text
 */
typedef struct {
	GtkWidget        *canvas;          /*!< The main canvas. */
	GdkGC            *gc1;             /*!< Graphics context (normal). */
	GdkGC            *gc2;             /*!< Graphics context (inverted). */
	guint             width;           /*!< Canvas width. */
	guint             height;          /*!< Canvas height. */
	lcd_coor_t        digits[13];      /*!< Starting points for all digits. */
	lcd_coor_t        dots[2];       /*!< Starting points for dots. */
	guint             dlw;             /*!< Width of large digits. */
	guint             dlh;             /*!< Height of large digits. */
	guint             clw;             /*!< Width of large separator. */
	guint             dsw;             /*!< Width of small digits. */
	guint             dsh;             /*!< Height of small digits. */
	guint             csw;             /*!< Width of small separator. */
	GdkColor          bg;              /*!< Background color. */
	GdkColor          fg;              /*!< Foreground color. */
	gboolean          exposed;         /*!< Exposed flag. */
	gboolean	  manual;	   /*!< Manual freq entry flag. */
	gint		  digit;	   /*!< Current digit when in manual entry. */

	vfo_t             vfo;             /*!< Current VFO. */
	gdouble           freq1;           /*!< Main frequency value. */
	gdouble           freq2;           /*!< Secondary frequency value. */
	gdouble           freqm;           /*!< Manually entered frequency value. */
	gchar             freqs1[10];      /*!< Frequency array. */
	gint              rit;             /*!< Current RIT value. */
	gchar             rits[4];         /*!< -9999 Hz but last digit not shown */
	gint              xit;             /*!< Current XIT value. */
	gchar             xits[4];         /*!< -9999 Hz but last digit not shown */
} lcd_t;


GtkWidget *rig_gui_lcd_create (void);
void       rig_gui_lcd_set_freq_digits  (freq_t freq);
void       rig_gui_lcd_set_rit_digits   (shortfreq_t freq);
void	   rig_gui_lcd_begin_manual_entry  (void);
void	   rig_gui_lcd_clear_manual_entry  (void);
void	   rig_gui_lcd_set_next_digit  (char digit);

#endif
