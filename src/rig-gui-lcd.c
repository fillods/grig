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
/** \file rig-gui-lcd.c
 *  \ingroup lcd
 *  \brief LCD display.
 *
 * The main purpose of the LCD display widget is to show the current frequency
 * and to provide easy access to set the current working frequency. The display
 * has 6 large digits left of the decmal and three small digits right of the
 * decimal. It is therefore capable to display the frequency with 1 Hz of 
 * accuracy below 1 GHz and with 1kHz of accuracy above 1 GHz. 
 *
 * The master widget consists of a GtkDrawingArea holding on which the digits
 * are drawn (one for each display digit). The digits are pixmaps loaded from
 * two files, one containing the
 * normal sized digits and the other containing the small digits. The size of
 * the canvas is calculated from the size of the digits.
 *
 * In order to have predictable behaviour the pixmap files containing the
 * digits need to contain 12 equal-sized digits and a comma: '0123456789 -.'
 * Each of these digits will then be stored in a GdkPixbuf and used on the
 * canvas as necessary. Furthermore, the last column in the pixmaps must
 * contain alternting pixels with the foreground and the background color
 * which will be used on the canvas.
 *
 * The comma need not have the same size as the digits; it may be smaller.
 * When the  pixmap is read, the size of the digits is calclated as follows:
 \code
 DIGIT_WIDTH  = IMG_WIDTH div 12
 COMMA_WIDTH  = IMG_WIDTH mod 12 - 1  (first column contains color info)
 DIGIT_HEIGHT = IMG_HEIGHT

 \endcode
 * It is therefore quite important that IMG_WIDTH mod 11 > 0 and that the
 * width of the decimal separator is less than 10 pixels.
 *
 * \bug Currently it does not work with f >= 1 GHz
 *
 * \bug RIT/XIT can display up to 9.99
 *
 * \bug Needs to be cleaned up!
 *
 * \bug Mouse events are caught only if RIG has set_freq capabilities.
 *      set_rit / set_xit capability must be checked explicitly by the
 *      RIT/XIT handling code.
 */
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <math.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <hamlib/rig.h>
#include "compat.h"
#include "rig-data.h"
#include "grig-gtk-workarounds.h"
#include "rig-gui-lcd.h"



/** \brief Event objects.
 *
 * This enumeration is used to identify on which object
 * a particular event has occurred.
 */
typedef enum {
	EVENT_OBJECT_NONE = -1,      /*!< No object. */
	EVENT_OBJECT_FREQ_1G,       /*!< 1 GHz frequency digit. */
	EVENT_OBJECT_FREQ_100M,     /*!< 100 MHz frequency digit. */
	EVENT_OBJECT_FREQ_10M,      /*!< 10 MHz frequency digit. */
	EVENT_OBJECT_FREQ_1M,       /*!< 1 MHz frequency digit. */
	EVENT_OBJECT_FREQ_100k,     /*!< 100 kHz frequency digit. */
	EVENT_OBJECT_FREQ_10k,      /*!< 10 kHz frequency digit. */
	EVENT_OBJECT_FREQ_1k,       /*!< 1 kHz frequency digit. */
	EVENT_OBJECT_FREQ_100,      /*!< 100 Hz frequency digit. */
	EVENT_OBJECT_FREQ_10,       /*!< 10 Hz frequency digit. */
	EVENT_OBJECT_FREQ_1,        /*!< 1 Hz frequency digit. */
	EVENT_OBJECT_RIT_1k,        /*!< 1 kHz RIT digit. */
	EVENT_OBJECT_RIT_100,       /*!< 100 Hz RIT digit. */
	EVENT_OBJECT_RIT_10         /*!< 10 Hz RIT digit. */
} event_object_t;

/** \brief The space between the edge of the LCD and contents in pixels. */
#define LCD_MARGIN 50


/** \brief Pixmaps containing normal sized digits. */
static GdkPixbuf *digits_normal[13];

/** \brief Pixmaps containing small sized digits. */
static GdkPixbuf *digits_small[13];


lcd_t lcd;

/* private function prototypes */
static void           rig_gui_lcd_load_digits      (const gchar *fname);
static gboolean       rig_gui_lcd_expose_cb        (GtkWidget *, GdkEventExpose *, gpointer);
static gboolean       rig_gui_lcd_handle_event     (GtkWidget *, GdkEvent *, gpointer);
static event_object_t rig_gui_lcd_get_event_object (GdkEvent *event);
static void           rig_gui_lcd_calc_dim         (void);

static gint           rig_gui_lcd_timeout_exec     (gpointer);
static gint           rig_gui_lcd_timeout_stop     (GtkWidget *, GdkEvent *, gpointer);

static void           rig_gui_lcd_draw_text               (cairo_t *);
static void           rig_gui_lcd_draw_digit              (cairo_t *, gint, char);
static void           rig_gui_lcd_draw_freq_digits        (cairo_t *);
static void           rig_gui_lcd_draw_rit_digits         (cairo_t *);
static void           rig_gui_lcd_draw_freq_digits_manual (cairo_t *);
static void           rig_gui_lcd_update_vfo              (cairo_t *, PangoLayout *);

static void           ritval_to_bytearr            (gchar *, shortfreq_t);

/** \brief Create LCD display widget.
 *  \return The LCD display widget.
 *
 * This function creates and initializes the LCD display widget which is
 * used to display the frequency.
 */
GtkWidget *
rig_gui_lcd_create ()
{
	guint      timerid;

	/* init data */
	lcd.exposed = FALSE;
	lcd.manual = FALSE;
	lcd.dirty = TRUE;

	/* load digit pixmaps from file */
	rig_gui_lcd_load_digits (NULL);

	/* calculate frequently used sizes and positions */
	rig_gui_lcd_calc_dim ();

	g_signal_new("freq-changed", GTK_TYPE_WIDGET,
		G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0, NULL,
		NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	/* create canvas */
	lcd.canvas = gtk_drawing_area_new ();
	gtk_widget_set_size_request (lcd.canvas, lcd.width, lcd.height);

	/* connect expose handler which will take care of adding
	   contents.
	*/
	g_signal_connect (G_OBJECT (lcd.canvas), "expose_event",  
                      G_CALLBACK (rig_gui_lcd_expose_cb), NULL);	

	/* connect mouse events but only if rig has set_freq;
	   XXX THIS IS A BUG SINCE WE DON'T DISTINGUISH BETWEEN SET_FREQ
	   AND SET_RIT/SET_XIT.
	*/
#ifndef DISABLE_HW
	if (rig_data_has_set_freq1 ()) {
#endif
		gtk_widget_add_events (lcd.canvas, GDK_BUTTON_PRESS_MASK);
		g_signal_connect (G_OBJECT (lcd.canvas), "event",
                          G_CALLBACK (rig_gui_lcd_handle_event), NULL);
#ifndef DISABLE_HW
	}
#endif

	/* start readback timer but only if service is available 
	   or we are in DISABLE_HW mode
	*/
#ifndef DISABLE_HW
	if (rig_data_has_get_freq1 ()) {
#endif
		timerid = g_timeout_add (RIG_GUI_LCD_DEF_TVAL,
                                 rig_gui_lcd_timeout_exec,
                                 NULL);

		/* register timer_stop function at exit */
        g_signal_connect(G_OBJECT(lcd.canvas), "destroy",
                         G_CALLBACK (rig_gui_lcd_timeout_stop),
                         GUINT_TO_POINTER(timerid));
#ifndef DISABLE_HW
	}
#endif

    gtk_widget_show_all (lcd.canvas);
    
	return lcd.canvas;
}



/** \brief Load digit pixmaps into memory.
 *  \param name The base file name.
 *
 * This function loads the pixmaps containing the digits, splits them and
 * stores each individual digit in memory for later use. The function also
 * obtains the drawing area bg/fg colors based on the loaded
 * digits. Because the first column in the pixmap contains the background and
 * foreground colors, the dimensions of each digits is calculated as follows:
 \code
 W = (pixmap_width - 1) / 12
 H = pixmap_height
 \endcode
 * The parameter name is used to specify the files from which the digits should
 * be loaded. If name is NULL, the default pixmaps will be read. Otherwise grig
 * will look for $PACKAGE_PIXMAPS_DIR/name_normal.png and
 * $PACKAGE_PIXMAPS_DIR/name_small.png
 *
 * \bug No fallback exists if standard pixmap is not where it is supposed to be!
 *
 */
static void
rig_gui_lcd_load_digits (const gchar *name)
{
	GdkPixbuf *digits;   /* pixmap in memory */
	gint i;
	guint dw,dh,cw;      /* width and height of a digit and width of comma */
	gint bps,rs;         /* bits pr.ample and rowstride */
	gchar *fname;
	gchar *tmp;
	guchar *pixels;

	
	/* normal digits */
	if (name == NULL) {
		//fname = g_strconcat (PACKAGE_PIXMAPS_DIR, G_DIR_SEPARATOR_S,
		//		     "digits_normal.png", NULL);
		fname = pixmap_file_name ("digits_normal.png");
	}
	else {
		//fname = g_strconcat (PACKAGE_PIXMAPS_DIR, G_DIR_SEPARATOR_S,
		//		     name, "_normal.png", NULL);
		tmp = g_strconcat (name, "_normal.png", NULL);
		fname = pixmap_file_name (tmp);
		g_free (tmp);
	}

	/* load pixmap */
	digits = gdk_pixbuf_new_from_file (fname, NULL);
	g_free (fname);

	/* calculate digit size */
	dw = gdk_pixbuf_get_width (digits) / 12;
	dh = gdk_pixbuf_get_height (digits);

	/* split pixmap into digits */
	for (i=0; i<12; i++) {
		digits_normal[i] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, dw, dh);
		gdk_pixbuf_copy_area (digits, 1 + dw*i, 0, dw, dh, digits_normal[i], 0, 0);
	}

	/* decimal point */
	cw = gdk_pixbuf_get_width (digits) % 12 - 1;
	digits_normal[12] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, cw, dh);
	gdk_pixbuf_copy_area (digits, 1 + dw*12, 0, cw, dh, digits_normal[12], 0, 0);

	/* get background and foreground colors */
	bps = gdk_pixbuf_get_bits_per_sample (digits);
	rs  = gdk_pixbuf_get_rowstride (digits);

	pixels = gdk_pixbuf_get_pixels (digits);

	/* get each 8-bit component and scale to 16-bits;
	   we use floating point aritmetics to allow more than
	   16 bits per sample.
	*/
	lcd.fg.red   = (guint16) (pixels[(bps/8)*0] * (65535.0 / (pow (2, bps) - 1)));
	lcd.fg.green = (guint16) (pixels[(bps/8)*1] * (65535.0 / (pow (2, bps) - 1)));
	lcd.fg.blue  = (guint16) (pixels[(bps/8)*2] * (65535.0 / (pow (2, bps) - 1)));

	lcd.bg.red   = (guint16) (pixels[(bps/8)*0 + rs] * (65535.0 / (pow (2, bps) - 1)));
	lcd.bg.green = (guint16) (pixels[(bps/8)*1 + rs] * (65535.0 / (pow (2, bps) - 1)));
	lcd.bg.blue  = (guint16) (pixels[(bps/8)*2 + rs] * (65535.0 / (pow (2, bps) - 1)));

	g_object_unref (digits);

	/* small digits */
	if (name == NULL) {
		//fname = g_strconcat (PACKAGE_PIXMAPS_DIR, G_DIR_SEPARATOR_S,
		//		     "digits_small.png", NULL);
		fname = pixmap_file_name ("digits_small.png");
	}
	else {
		//fname = g_strconcat (PACKAGE_PIXMAPS_DIR, G_DIR_SEPARATOR_S,
		//		     name, "_small.png", NULL);
		tmp = g_strconcat (name, "_small.png", NULL);
		fname = pixmap_file_name (tmp);
		g_free (tmp);
	}

	/* load pixmap */
	digits = gdk_pixbuf_new_from_file (fname, NULL);
	g_free (fname);

	/* calculate digit size */
	dw = gdk_pixbuf_get_width (digits) / 12;
	dh = gdk_pixbuf_get_height (digits);

	/* split pixmap into digits */
	for (i=0; i<12; i++) {
		digits_small[i] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, dw, dh);
		gdk_pixbuf_copy_area (digits, 1 + dw*i, 0, dw, dh, digits_small[i], 0, 0);
	}

	/* decimal point */
	cw = gdk_pixbuf_get_width (digits) % 12 - 1;
	digits_small[12] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, cw, dh);
	gdk_pixbuf_copy_area (digits, 1 + dw*12, 0, cw, dh, digits_small[12], 0, 0);

	/* get background and foreground colors */
	bps = gdk_pixbuf_get_bits_per_sample (digits);
	rs  = gdk_pixbuf_get_rowstride (digits);

	g_object_unref (digits);

}


/** \brief Handle expose events for the drawing area.
 *  \param widget The drawing area widget.
 *  \param event  The event.
 *  \param data   User data; always NULL.
 *
 * This function is responsible for redrawing the LCD widget. State is stored in
 * the lcd object and used to draw the various elements.
 *
 * \bug canvas height is hadcoded according to smeter height.
 *
 */
static gboolean
rig_gui_lcd_expose_cb   (GtkWidget      *widget,
                         GdkEventExpose *event,
                         gpointer        data)
{

    /* create cairo context */
    GdkWindow *window = gtk_widget_get_window (widget);
    cairo_t *cr = gdk_cairo_create (window);

    /* set background */
    gdk_cairo_set_source_color (cr, &lcd.bg);
    cairo_paint (cr);

    /* draw border around the meter */
    gdk_cairo_set_source_color (cr, &lcd.fg);
    cairo_rectangle (cr, 0, 0, lcd.width, lcd.height);
    cairo_stroke (cr);

    /* large dot */
    cairo_save (cr);
    cairo_translate (cr, lcd.dots[0].x, lcd.dots[0].y);
    gdk_cairo_set_source_pixbuf (cr, digits_normal[12], 0, 0);
    cairo_paint (cr);
    cairo_restore (cr);

    /* small dot */
    cairo_save (cr);
    cairo_translate (cr, lcd.dots[1].x, lcd.dots[1].y);
    gdk_cairo_set_source_pixbuf (cr, digits_small[12], 0, 0);
    cairo_paint (cr);
    cairo_restore (cr);

    /* draw text */
    rig_gui_lcd_draw_text (cr);

    /* draw digits */
    if (lcd.manual) {

        rig_gui_lcd_draw_freq_digits_manual (cr);
    }
    else {

        rig_gui_lcd_draw_freq_digits (cr);
    }

    /* draw rit digits */
    rig_gui_lcd_draw_rit_digits (cr);

    /* indicate that widget is ready to be used */
    lcd.exposed = TRUE;

    cairo_destroy(cr);

    return TRUE;
}



/** \brief Handle drawing area events.
 *  \param widget The drawing area widget receiving the evend.
 *  \param event  The received event
 *  \param data   User data; always NULL.
 *  \return TRUE if the event can be handled, FALSE otherwise.
 *
 * This function is called every time an event occurs on the drawing area.
 * The function currently handles three type of events:
 * \li GDK_EXPOSE: The drawing area has been exposed and needs to be redrawn
 * \li GDK_BUTTON_PRES: One of the three mouse button has been pressed
 * \li GDK_SCROLL: The scroll wheel of the mouse has been used
 * All other events are ignored in which case FALSE is returned to notify the
 * parent that the event has not been handled.
 *
 * The detailed management of each of the three events is done by other internal
 * functions.
 *
 * \bug Cyclomatic omplexity to high?
 *
 * \sa rig_gui_lcd_expose_cb, rig_gui_lcd_get_event_object
 */
static gboolean
rig_gui_lcd_handle_event     (GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data)
{
	event_object_t object;     /* event object */
	freq_t         deltaf;     /* frequency change */
	freq_t         newfreq;    /* new frequency. */
	shortfreq_t    deltar;     /* RIT/XIT change */
	shortfreq_t    newrit;     /* new RIT/XIT value */
	guint          power;      /* usd for 10**power */
	gchar         *str;

	/* in case of expose-event call the expose event handler */
	switch (event->type) {

		/* drawing area has been exposed */
	case GDK_EXPOSE:
		return rig_gui_lcd_expose_cb (widget, (GdkEventExpose *) event, data);
		break;

		/* button press */
	case GDK_BUTTON_PRESS:

		object = rig_gui_lcd_get_event_object (event);

		/* if no object, just return  */
		if (object == EVENT_OBJECT_NONE) {
			return FALSE;
		}

		else if (object > EVENT_OBJECT_FREQ_1) {

			/* RIT/XIT event;
			   object is in the range 10..12
			*/
			power = 13 - object;
			deltar = pow (10, power);

			if (deltar < rig_data_get_ritstep ()) {
                //				return TRUE;
				deltar = rig_data_get_ritstep ();
			}

			/* check which mouse button */
			switch (event->button.button) {

				/* LEFT button: inrease frequency */
			case 1:
				/* ensure correct sign flip */
				if ((lcd.rit < 0) && (deltar > abs (lcd.rit)))
					newrit = -lcd.rit;
				else
					newrit = lcd.rit + deltar;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newrit <= rig_data_get_ritmax ()) {

					rig_data_set_rit (newrit);
					rig_gui_lcd_set_rit_digits (newrit);
				}

				break;

				/* MIDDLE button: clear digit */
			case 2:
				/* convert current frequency to string and
				   clear corresponding digit; then convert
				   back to freq_t

				   WARNING: don't use lcd.freqs1 because it is
				   not 0-terminated; it's just a byte array!
				*/
				str = g_strdup_printf ("%5d", lcd.rit);
				str[object-9] = '0';

				newrit = (freq_t) g_strtod (str, NULL);

				/* try new frequency */
				if (newrit >= rig_data_get_ritmin ()) {

					rig_data_set_rit (newrit);
					rig_gui_lcd_set_rit_digits (newrit);
				}

				g_free (str);

				break;

				/* RIGHT button: decrease frequency */
			case 3:
				/* ensure correct sign flip */
				if ((lcd.rit > 0) && (deltar > lcd.rit))
					newrit = -lcd.rit;
				else
					newrit = lcd.rit - deltar;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newrit >= rig_data_get_ritmin ()) {

					rig_data_set_rit (newrit);
					rig_gui_lcd_set_rit_digits (newrit);
				}

				break;

			default:
				break;

			} /* case */

		}  /* else if object > EVENT_OBJECT_FREQ_1 */

		else {  /* frequency event;
                   object is in the range 0..9 with
                   0 corresponding to 1 GHz
                */
			power = 9 - object;
			deltaf = pow (10, power);

			if (deltaf < rig_data_get_fstep ()) {
                //				return TRUE;
				deltaf = rig_data_get_fstep ();
			}

			/* check which mouse button */
			switch (event->button.button) {

				/* LEFT button: inrease frequency */
			case 1:
				newfreq = lcd.freq1 + deltaf;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newfreq <= rig_data_get_fmax ()) {

					rig_data_set_freq (1, newfreq);
					rig_gui_lcd_set_freq_digits (newfreq);
				}

				break;

				/* MIDDLE button: clear digit */
			case 2:
				/* convert current frequency to string and
				   clear corresponding digit; then convert
				   back to freq_t

				   WARNING: don't use lcd.freqs1 because it is
				   not 0-terminated; it's just a byte array!
				*/
				str = g_strdup_printf ("%10.0f", lcd.freq1);
				str[object] = '0';

				newfreq = (freq_t) g_strtod (str, NULL);

				/* try new frequency */
				if (newfreq >= rig_data_get_fmin ()) {

					rig_data_set_freq (1, newfreq);
					rig_gui_lcd_set_freq_digits (newfreq);
				}

				g_free (str);

				break;

				/* RIGHT button: decrease frequency */
			case 3:
				newfreq = lcd.freq1 - deltaf;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newfreq >= rig_data_get_fmin ()) {

					rig_data_set_freq (1, newfreq);
					rig_gui_lcd_set_freq_digits (newfreq);
				}

				break;

			default:
				break;

			} /* case event->button.button */
		} /* else */


		return TRUE;
		break;

		/* mouse wheel */
	case GDK_SCROLL:

		object = rig_gui_lcd_get_event_object (event);

		/* if no object, just return */
		if (object == EVENT_OBJECT_NONE) {
			return FALSE;
		}

		else if (object > EVENT_OBJECT_FREQ_1) {

			/* RIT/XIT event;
			   object is in the range 10..12
			*/
			power = 13 - object;
			deltar = pow (10, power);

			if (deltar < rig_data_get_ritstep ()) {
                //				return TRUE;
				deltar = rig_data_get_ritstep ();
			}

			/* check which mouse button */
			switch (event->scroll.direction) {

				/* WHEEL UP: inrease frequency */
			case GDK_SCROLL_UP:
				/* ensure correct sign flip */
				if ((lcd.rit < 0) && (deltar > abs (lcd.rit)))
					newrit = -lcd.rit;
				else
					newrit = lcd.rit + deltar;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newrit <= rig_data_get_ritmax ()) {

					rig_data_set_rit (newrit);
					rig_gui_lcd_set_rit_digits (newrit);
				}

				break;

				/* SCROLL DOWN: decrease frequency */
			case GDK_SCROLL_DOWN:
				/* ensure correct sign flip */
				if ((lcd.rit > 0) && (deltar > lcd.rit))
					newrit = -lcd.rit;
				else
					newrit = lcd.rit - deltar;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newrit >= rig_data_get_ritmin ()) {

					rig_data_set_rit (newrit);
					rig_gui_lcd_set_rit_digits (newrit);
				}

				break;

			default:
				break;

			} /* case */

		}

		else {	/* frequency event
                   object is in the range 0..9 with
                   0 corresponding to 1 GHz
                */
			power = 9 - object;
			deltaf = pow (10, power);

			if (deltaf < rig_data_get_fstep ()) {
                //				return TRUE;
				deltaf = rig_data_get_fstep ();
			}

			/* check which mouse button */
			switch (event->scroll.direction) {

				/* WHEEL UP: inrease frequency */
			case GDK_SCROLL_UP:
				newfreq = lcd.freq1 + deltaf;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newfreq <= rig_data_get_fmax ()) {

					rig_data_set_freq (1, newfreq);
					rig_gui_lcd_set_freq_digits (newfreq);
				}

				break;

				/* WHEEL DOWN: decrease frequency */
			case GDK_SCROLL_DOWN:
				newfreq = lcd.freq1 - deltaf;

				/* check whether we are within current
				   frequency range; if so, apply new frequency
				*/
				if (newfreq >= rig_data_get_fmin ()) {

					rig_data_set_freq (1, newfreq);
					rig_gui_lcd_set_freq_digits (newfreq);
				}

				break;

			default:
				break;

			}
		}

		return TRUE;
		break;

		
	default:
		return FALSE;
		break;
	}

	return FALSE;
}



/** \brief Find event object.
 *  \param event The occurred GdkEvent.
 *  \return The ID of the object on which the event occurred.
 *
 * This function scans through the coordinates of the existing
 * digits(objects) and return the corresponding event object ID.
 * If no object matches the event coordinates EVENT_OBJECT_NONE
 * is returned.
 *
 * \bug \b CRITICAL: the commas are caughht as valid objects and are
 * reported back with negative object ID. Not anymore?
 */
static event_object_t
rig_gui_lcd_get_event_object (GdkEvent *event)
{
	guint x,y;    /* coordinates */
    gint i;

	x = (guint) ((GdkEventButton*)event)->x;
	y = (guint) ((GdkEventButton*)event)->y;

	/* check vertical range */
	if ((y < lcd.digits[0].y) || (y > lcd.digits[0].y+lcd.dlh)) {

		return EVENT_OBJECT_NONE;
	}

	/* check x */
	if ((x < lcd.digits[0].x)                                     || 
	    (x > lcd.digits[12].x+lcd.dsw)                            ||
	    ((x > lcd.digits[9].x+lcd.dsw) && (x < lcd.digits[10].x))  ||
	    ((x > lcd.digits[10].x+lcd.dsw) && (x < lcd.digits[11].x)))   {
		
		return EVENT_OBJECT_NONE;
	}
	
	else {
		/* loop over all digits until we find one;
		   we loop from the end, because small frequencies
		   are changed more often.
		*/
		/* small digits */
		for (i=12; i>6; i--) {
			
			if ((x > lcd.digits[i].x) && (x < lcd.digits[i].x+lcd.dsw)) {
				return i;
			}
			
		}
		
		/* then try large digits */
		for (i=6; i>=0; i--) {
			
			if ((x > lcd.digits[i].x) && (x < lcd.digits[i].x+lcd.dlw)) {
				return i;
			}
		}
		
		return EVENT_OBJECT_NONE;
		
	}
	
	return EVENT_OBJECT_NONE;

}




/** \brief Calculate and store frequently used sizes and positions.
 *
 * This function calculates and stores frequently used dimensions and
 * positions to avoid too much CPU-load during update of therawing area.
 * The involved parameters are canvas size, digit sizes and position for
 * each digit, which will be part of the frequency display. The calculated
 * quantities are stored in predefined fields of the lcd structure.
 *
 * \bug A loop could have been used, but it would be messy anyway with several
 *      if's.
 */
static void
rig_gui_lcd_calc_dim    ()
{
	guint i;   /* iterator */


	/* store digit sizes to avoid frequent call to
	   gdk_pixbuf_get_width and gdk_pixbuf_get_height
	*/
	lcd.dlw = gdk_pixbuf_get_width  (digits_normal[0]);
	lcd.dlh = gdk_pixbuf_get_height (digits_normal[0]);
	lcd.clw = gdk_pixbuf_get_width  (digits_normal[12]);
	lcd.dsw = gdk_pixbuf_get_width  (digits_small[0]);
	lcd.dsh = gdk_pixbuf_get_height (digits_small[0]);
	lcd.csw = gdk_pixbuf_get_width  (digits_small[12]);

	/* calculate drawing area dimensions */
	lcd.width = 7*lcd.dlw + 3*lcd.clw + 8*lcd.dsw + lcd.csw + 2*LCD_MARGIN;
	lcd.height = 80;

	/* calculate screen position for each digit; this will ease the
	   update of the LCD 
	*/
	/* VFO digits */
	lcd.digits[0].x = LCD_MARGIN / 2;
	lcd.digits[1].x = lcd.digits[0].x + lcd.clw + lcd.dlw;
	lcd.digits[2].x = lcd.digits[1].x + lcd.dlw;
	lcd.digits[3].x = lcd.digits[2].x + lcd.dlw;
	lcd.digits[4].x = lcd.digits[3].x + lcd.clw + lcd.dlw;   /**/
	lcd.digits[5].x = lcd.digits[4].x + lcd.dlw;
	lcd.digits[6].x = lcd.digits[5].x + lcd.dlw;
	lcd.digits[7].x = lcd.digits[6].x + lcd.clw + lcd.dlw;   /**/
	lcd.digits[8].x = lcd.digits[7].x + lcd.dsw;
	lcd.digits[9].x = lcd.digits[8].x + lcd.dsw;
	lcd.digits[10].x = lcd.digits[9].x + 4*lcd.dsw; /**/
	lcd.digits[11].x = lcd.digits[10].x + lcd.csw + lcd.dsw;   /**/
	lcd.digits[12].x = lcd.digits[11].x + lcd.dsw;
	lcd.digits[13].x = lcd.digits[10].x - lcd.dsw; /* rit sign */

	for (i=0; i<7; i++)
		lcd.digits[i].y = (lcd.height - lcd.dlh)/2;

	for (i=7; i<14; i++)
		lcd.digits[i].y = lcd.digits[1].y + (lcd.dlh-lcd.dsh)-1;

	lcd.dots[0].x = lcd.digits[6].x + lcd.dlw;
	lcd.dots[1].x = lcd.digits[10].x + lcd.dsw;

	lcd.dots[0].y = (lcd.height - lcd.dlh)/2;
	lcd.dots[1].y = lcd.digits[1].y + (lcd.dlh-lcd.dsh)-1;


}


/** \brief Draw the LCD frequency digits
 *  \param cr The cairo graphics context
 *
 */
static void
rig_gui_lcd_draw_freq_digits (cairo_t *cr)
{

    gchar *freqs = g_strdup_printf ("%10.0f", lcd.freq1);

    for (gint pos = 0; pos < 10; pos++) {
        rig_gui_lcd_draw_digit (cr, pos, freqs[pos]);
    }
}


/** \brief Set LCD display frequency.
 *  \param freq The frequency.
 *
 * This function updates the internal frequency display state.
 *
 * \bug 'default' case should send a critical error message.
 *
 * \sa rig_gui_lcd_set_rit_digits
 */
void
rig_gui_lcd_set_freq_digits (freq_t freq)
{

    /* validate frequency */
    if (freq < rig_data_get_fmin ())
        return;

    /* don't attempt to update freq while in manual mode */
    if (lcd.manual)
        return;

    /* saturate frequency */
    if (freq >= GHz(10))
        freq = MHz(9999.999999);

    if (lcd.freq1 != freq) {
        /* frequency has changed */
        lcd.freq1 = freq;
        lcd.dirty = TRUE;

        /* emit changed signal */
        g_signal_emit_by_name(lcd.canvas, "freq-changed");

    }
}

/** \brief Draw a single LCD digit
 *  \param cr The cairo graphics context
 */
static void
rig_gui_lcd_draw_digit(cairo_t *cr, gint position, char digit)
{
    gint       ipixmap; /* index in pixmap */
    GdkPixbuf *digitbuf; /* local pointer to digits */

    if (digit >= '0' && digit <= '9') {
        ipixmap = digit - 48;
    }
    else if (digit == ' ') {
        ipixmap = 10;
    }
    else if (digit == '-') {
        ipixmap = 11;
    } else {
        /* critical error */
        return;
    }

    digitbuf = position < 7
        ? digits_normal[ipixmap]
        : digits_small[ipixmap];

    cairo_save (cr);
    cairo_translate (cr, lcd.digits[position].x,
                         lcd.digits[position].y);
    gdk_cairo_set_source_pixbuf (cr, digitbuf, 0, 0);
    cairo_paint (cr);
    cairo_restore (cr);

}


/** \brief Draw the LCD frequency digits, when in manual entry mode.
 *  \param cr The cairo graphics context
 *
 * This function is similar to rig_gui_lcd_draw_freq_digits, however we handle
 * leading characters differently.
 */
static void
rig_gui_lcd_draw_freq_digits_manual (cairo_t *cr)
{

    gint pos = 0;
    gchar *freqs = g_strdup_printf ("%10.0f", lcd.freqm);

    /* draw the digits already entered */
    for (pos = 0; pos < lcd.digit; pos++) {
        rig_gui_lcd_draw_digit (cr, pos, freqs[pos]);
    }

    /* draw unentered digits as '-' */
    for (pos = lcd.digit; pos < 10; pos++) {
        rig_gui_lcd_draw_digit (cr, pos, '-');
    }
}


/** \brief Add a user-entered digit to the manual frequency entry
 *  \param n The receieved character
 */
void
rig_gui_lcd_set_next_digit(char n)
{
	/* is drawing area ready? */
	if (!lcd.exposed)
		return;

	if (!lcd.manual)
		return;

	lcd.freqm += pow(10, 9 - lcd.digit) * (n - '0');

	/* increment for next digit */
	lcd.digit++;

	/* check if this is the last digit and set the freq */
	if (lcd.digit == 10) {

		rig_data_set_freq (1, lcd.freqm);
		lcd.manual = FALSE;

		rig_gui_lcd_set_freq_digits (lcd.freqm);
	}
}

void
rig_gui_lcd_begin_manual_entry  (void)
{
	guint i;

	/* is drawing area ready? */
	if (!lcd.exposed)
		return;

	/* if we already were in manual keypad mode,
	 * pad the frequency with zeros
	 */
	if (lcd.manual) {

		if (lcd.digit == 0) {
			/* revert to previous freq if no digit has been entered */
			rig_gui_lcd_clear_manual_entry();
			return;
		}

		for (i = lcd.digit; i < 10; i++) {
			rig_gui_lcd_set_next_digit('0');
		}

		return;
	}

	lcd.manual = TRUE;
	lcd.digit = 0;
	lcd.freqm = 0;
	lcd.dirty = TRUE;

}

void
rig_gui_lcd_clear_manual_entry  (void)
{

	/* is drawing area ready? */
	if (!lcd.exposed)
		return;

	if (!lcd.manual)
		return;

	lcd.manual = FALSE;

	rig_gui_lcd_set_freq_digits(lcd.freq1);
}


/** \brief Set LCD display frequency (RIT/XIT).
 *  \param freq The frequency.
 *
 * This function updates the internal RIT/XIT state.
 *
 * \sa rig_gui_lcd_set_freq_digits
 */
void
rig_gui_lcd_set_rit_digits (shortfreq_t freq)
{

	if (freq > s_kHz(9.99)) {
		freq = kHz(9.99);
	}
	else if (freq < s_kHz(-9.99)) {
		freq = s_kHz(-9.99);
	}

	if (freq != lcd.rit) {

		/* store RIT/XIT frequency for later use */
		lcd.rit = freq;
		lcd.dirty = TRUE;
	}

}


/** \brief Draw the LCD display frequency (RIT/XIT).
 *  \param cr The cairo graphics context.
 *
 * This function draws the RIT/XIT digits on the LCD display. The frequency is
 * stored internally in hamlib format (signed long), and converted to a string
 * with 3 digits with a 0.01kHz resolution.
 */
static void
rig_gui_lcd_draw_rit_digits (cairo_t *cr)
{
    gchar *rits;

    rits = g_strdup("-0000");
    ritval_to_bytearr (rits, lcd.rit);

    if (rits[0] == ' ' || rits[0] == '-') {

        rig_gui_lcd_draw_digit (cr, 13, rits[0]);
    }

    for (gint i = 1; i < 4; i++) {

        rig_gui_lcd_draw_digit (cr, i + 9, rits[i]);
    }

    g_free (rits);
}


/** \brief Execute timeout function.
 *  \param data User data; currently NULL.
 *  \return Always TRUE to keep the timer running.
 *
 * This function is in charge for updating the LCD. It acquires various
 * measurements from the rig-data object, and updates the internal LCD state,
 * requesting a redraw of the widget if neccessary.
 *
 * The function is called peridically by the Gtk+ scheduler.
 *
 * \bug Add XIT support
 */
static gint
rig_gui_lcd_timeout_exec  (gpointer data)
{
	static guint vfoupd;

    /* update frequency if applicable */
    if (rig_data_has_get_freq1 ()) {

        freq_t freq = rig_data_get_freq (1);

        if (lcd.freq1 != freq) {

            lcd.freq1 = freq;
            rig_gui_lcd_set_freq_digits (lcd.freq1);
        }
    }

    /* update RIT/XIT if applicable */
    if (rig_data_has_get_rit () || rig_data_has_set_rit ()) {

        shortfreq_t rit = rig_data_get_rit ();

        if (lcd.rit != rit) {

            lcd.rit = rit;
            rig_gui_lcd_set_rit_digits (lcd.rit);
        }
    }

    /* VFO updated every second cycle */
    if (vfoupd) {

        /* update vfo state */
        vfo_t vfo = rig_data_get_vfo ();

        if (vfo != lcd.vfo) {

            lcd.vfo = vfo;
            lcd.dirty = TRUE;
        }
        vfoupd = 0;
    }
    else {
        vfoupd += 1;
    }

    if (lcd.dirty) {

        /* request redraw */
        gdk_window_invalidate_rect (gtk_widget_get_window (lcd.canvas),
                                    NULL, FALSE);
    }

    return TRUE;
}



/** \brief Stop timeout function.
 *  \param timer The ID of the timer to stop.
 *  \return Always TRUE.
 *
 * This function is used to stop the readback timer just before the
 * program is quit. It should be called automatically by Gtk+ when
 * the gtk_main_loop is exited.
 *
 * \bug DDD is wrong; copied from smeter.
 */
static gint
rig_gui_lcd_timeout_stop (GtkWidget *widget,
                          GdkEvent  *event,
                          gpointer   data)
{

	GSource *source = g_main_context_find_source_by_id (NULL, GPOINTER_TO_UINT (data));
    if (source)
        g_source_destroy (source);

	return TRUE;
}



/** \brief Draw miscellaneous text.
 *  \param cr The cairo graphics context
 *
 * This function is in charge of drawing miscellaneous text on the display,
 * like RIT, kHz and such.
 */
static void
rig_gui_lcd_draw_text (cairo_t *cr)
{

    PangoLayout          *layout;
    PangoFontDescription *desc;
    gint w, h;

    GdkColor color = {
        .red   = LCD_FG_DEFAULT_RED,
        .green = LCD_FG_DEFAULT_GREEN,
        .blue  = LCD_FG_DEFAULT_BLUE
    };

    /* set up cairo context */
    cairo_save (cr);
    gdk_cairo_set_source_color (cr, &color);

    /* create a new PangoLayout */
    layout  = pango_cairo_create_layout (cr);

    /* set text properties */
    desc = pango_font_description_from_string (RIG_GUI_LCD_FONT);
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);

    /* set text: kHz */
    pango_layout_set_text (layout, _("kHz"), -1);

    /* calculate coordinates */
    pango_layout_get_size (layout, &w, &h);
    w /= PANGO_SCALE;
    h /= PANGO_SCALE;

	/* draw text; frequency */
    cairo_save (cr);
    cairo_translate (cr, lcd.digits[9].x + lcd.dsw + 5,
                         lcd.digits[9].y + lcd.dsh - h);
    pango_cairo_show_layout (cr, layout);
    cairo_restore (cr);

    /* draw text; rit */
    cairo_save (cr);
    cairo_translate (cr, lcd.digits[12].x + lcd.dsw + 5,
                         lcd.digits[12].y + lcd.dsh - h);
    pango_cairo_show_layout (cr, layout);
    cairo_restore (cr);

    /* update vfo text */
    rig_gui_lcd_update_vfo (cr, layout);

    /* set text: RIT */
    pango_layout_set_text (layout, _("RIT"), -1);

    /* calculate coordinates */
    pango_layout_get_size (layout, &w, &h);
    w /= PANGO_SCALE;
    h /= PANGO_SCALE;

    /* draw text; RIT */
    cairo_save (cr);
    cairo_translate (cr, lcd.digits[11].x,
                         lcd.digits[0].y - h);
    pango_cairo_show_layout (cr, layout);
    cairo_restore (cr);

    /* jump back to original cairo context */
    cairo_restore (cr);

    /* free PangoLayout */
    g_object_unref (G_OBJECT (layout));
}


static void
rig_gui_lcd_update_vfo (cairo_t *cr, PangoLayout *layout)
{
	gint  w, h;

	/* is drawing area ready? */
	if (!lcd.exposed)
		return;

	/* set text: VFO */
	switch (lcd.vfo) {

	case RIG_VFO_A:
		pango_layout_set_text (layout, _("VFO A"), -1);
		break;

	case RIG_VFO_B:
		pango_layout_set_text (layout, _("VFO B"), -1);
		break;

	case RIG_VFO_C:
		pango_layout_set_text (layout, _("VFO C"), -1);
		break;

	case RIG_VFO_MAIN:
		pango_layout_set_text (layout, _("MAIN VFO"), -1);
		break;

	case RIG_VFO_SUB:
		pango_layout_set_text (layout, _("SUB VFO"), -1);
		break;

	case RIG_VFO_MEM:
		pango_layout_set_text (layout, _("MEM"), -1);
		break;

	default:
		pango_layout_set_text (layout, _("VFO ?"), -1);
		break;
	}

	/* calculate coordinates */
	pango_layout_get_size (layout, &w, &h);
	w /= PANGO_SCALE;
	h /= PANGO_SCALE;

	cairo_save (cr);
	cairo_translate (cr, lcd.digits[5].x,
						 lcd.digits[5].x);
	pango_cairo_show_layout (cr, layout);
	cairo_restore (cr);

}

/** \brief Convert RIT value to byte array.
 *  \param array The array to store the result in.
 *  \param freq  The frequency to convert.
 *
 * This function converts an integer value in the range [-9999;+9999] to a byte array.
 * The byte array has to be allocated by the caller and have a length of 5 bytes not
 * including the trailing \0.
 *
 * \bug This is a hack! Consider implementing it in a cleaner way.
 */
static void
ritval_to_bytearr (gchar *array, shortfreq_t freq)
{
	gint i;
	shortfreq_t delta;

	if (freq < 0)
		array[0] = '-';
	else
		array[0] = ' ';

	freq = abs (freq);

	for (i = 3; i >= 0; i--) {

		delta = pow (10, i);
		
		if (freq >= delta) {
			array[4-i] = freq / delta + '0';
			freq %= delta;
		}
		else {
			array[4-i] = '0';
		}
	}
}


