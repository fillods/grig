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
/** \file rig-gui-lcd.c
 *  \ingroup lcd
 *  \brief LCD display.
 *
 * The main pupose of the LCD display widget is to show the current frequency
 * and to provide easy access to set the current working frequency. The display
 * has 6 large digits left of the decmal and three small digits right of the
 * decimal. It is therefore capable to display the frequency with 1 Hz of accuracy
 * below 1 GHz and with 1kHz of accuracy above 1 GHz. 
 *
 * The master widget consists of a GtkDrawingArea holding on which the digits are
 * drawn (one for each
 * display digit). The digits are pixmaps loaded from two files, one containing the
 * normal sized digits and the other containing the small digits. The size of the
 * canvas is calculated from the size of the digits.
 *
 * In order to have predictable behaviour the pixmap files containing the digits
 * need to contain 11 equal-sized digits and a comma: '0123456789 .' Each of these
 * digits will then be stored in a GdkPixbuf and used on the canvas as necessary.
 * Furthermore, the last column in the pixmaps must contain alternting pixels with
 * the foreground and the background color which will be used on the canvas.
 *
 * The comma need not have the same size as the digits; it may be smaller. When the
 * pixmap is read, the size of the digits is calclated as follows:
\code
        DIGIT_WIDTH  = IMG_WIDTH div 11
	COMMA_WIDTH  = IMG_WIDTH mod 11 - 1  (first column contains color info)
	DIGIT_HEIGHT = IMG_HEIGHT

\endcode
 * It is therefore quite important that IMG_WIDTH mod 11 > 0 and that the width of the
 * decimal separator is less than 10 pixels. 
 */
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>
#include "rig-data.h"
#include "rig-gui-lcd.h"
#include "support.h"



/** \brief The space between the edge of the LCD and contents in pixels. */
#define LCD_MARGIN 20


/** \brief Pixmaps containing normal sized digits. */
static GdkPixbuf *digits_normal[12];

/** \brief Pixmaps containing small sized digits. */
static GdkPixbuf *digits_small[12];


lcd_t lcd;


static void     rig_gui_lcd_load_digits (const gchar *fname);
static gboolean rig_gui_lcd_expose_cb   (GtkWidget *, GdkEventExpose *, gpointer);
static void     rig_gui_lcd_calc_dim    (void);




/** \brief Create LCD display widget.
 *  \return The LCD display widget.
 *
 * This function creates and initializes the LCD display widget which is
 * used to diplay the frequency.
 */
GtkWidget *
rig_gui_lcd_create ()
{
	GtkWidget *vbox;

	/* init data */
	lcd.exposed = FALSE;

	/* load digit pixmaps from file */
	rig_gui_lcd_load_digits (NULL);

	/* calculate frequently used sizes and positions */
	rig_gui_lcd_calc_dim ();

	/* create canvas */
	lcd.canvas = gtk_drawing_area_new ();
	gtk_widget_set_size_request (lcd.canvas, lcd.width, lcd.height);

	/* connect expose handler which will take care of adding
	   contents.
	*/
	g_signal_connect (G_OBJECT (lcd.canvas), "expose_event",  
			  G_CALLBACK (rig_gui_lcd_expose_cb), NULL);	


	vbox = gtk_vbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (vbox), lcd.canvas, FALSE, FALSE, 5);
//	gtk_box_pack_start (GTK_BOX (vbox), hbox,  FALSE, FALSE, 0);


	return vbox;
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
 * will look for $PACKAGE_DATA_DIR/pixmaps/name_normal.png and
 * $PACKAGE_DATA_DIR/pixmaps/name_small.png
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
	guint bpsscale;      /* scale factor between image bps and 16 bit */
	gint bps,rs;         /* bits pr.ample and rowstride */
	gchar *fname;
	guchar *pixels;

	
	/* normal digits */
	if (name == NULL) {
		fname = g_strconcat (PACKAGE_DATA_DIR, G_DIR_SEPARATOR_S, "pixmaps",
				     G_DIR_SEPARATOR_S, "digits_normal.png", NULL);
	}
	else {
		fname = g_strconcat (PACKAGE_DATA_DIR, G_DIR_SEPARATOR_S, "pixmaps",
				     G_DIR_SEPARATOR_S, name, "_normal.png", NULL);
	}

	/* load pixmap */
	digits = gdk_pixbuf_new_from_file (fname, NULL);
	g_free (fname);

	/* calculate digit size */
	dw = gdk_pixbuf_get_width (digits) / 11;
	dh = gdk_pixbuf_get_height (digits);

	/* split pixmap into digits */
	for (i=0; i<11; i++) {
		digits_normal[i] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, dw, dh);
		gdk_pixbuf_copy_area (digits, 1 + dw*i, 0, dw, dh, digits_normal[i], 0, 0);
	}

	/* decimal point */
	cw = gdk_pixbuf_get_width (digits) % 11 - 1;
	digits_normal[11] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, cw, dh);
	gdk_pixbuf_copy_area (digits, 1 + dw*11, 0, cw, dh, digits_normal[11], 0, 0);

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
//	gdk_colormap_alloc_color (gdk_rgb_get_colormap (), &(lcd.fg), FALSE, TRUE);

	lcd.bg.red   = (guint16) (pixels[(bps/8)*0 + rs] * (65535.0 / (pow (2, bps) - 1)));
	lcd.bg.green = (guint16) (pixels[(bps/8)*1 + rs] * (65535.0 / (pow (2, bps) - 1)));
	lcd.bg.blue  = (guint16) (pixels[(bps/8)*2 + rs] * (65535.0 / (pow (2, bps) - 1)));
//	gdk_colormap_alloc_color (gdk_rgb_get_colormap (), &(lcd.bg), FALSE, TRUE);

	g_object_unref (digits);

	/* small digits */
	if (name == NULL) {
		fname = g_strconcat (PACKAGE_DATA_DIR, G_DIR_SEPARATOR_S, "pixmaps",
				     G_DIR_SEPARATOR_S, "digits_small.png", NULL);
	}
	else {
		fname = g_strconcat (PACKAGE_DATA_DIR, G_DIR_SEPARATOR_S, "pixmaps",
				     G_DIR_SEPARATOR_S, name, "_small.png", NULL);
	}

	/* load pixmap */
	digits = gdk_pixbuf_new_from_file (fname, NULL);
	g_free (fname);

	/* calculate digit size */
	dw = gdk_pixbuf_get_width (digits) / 11;
	dh = gdk_pixbuf_get_height (digits);

	/* split pixmap into digits */
	for (i=0; i<11; i++) {
		digits_small[i] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, dw, dh);
		gdk_pixbuf_copy_area (digits, 1 + dw*i, 0, dw, dh, digits_small[i], 0, 0);
	}

	/* decimal point */
	cw = gdk_pixbuf_get_width (digits) % 11 - 1;
	digits_small[11] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, cw, dh);
	gdk_pixbuf_copy_area (digits, 1 + dw*11, 0, cw, dh, digits_small[11], 0, 0);

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
 * This function is called when the rawing area widget is finalized
 * and exposed. Itis used to finish the initialization of those
 * parameters, which need attributes rom visible widgets.
 *
 * \bug canvas height is hadcoded according to smeter height.
 *
 */ 
static gboolean
rig_gui_lcd_expose_cb   (GtkWidget      *widget,
			 GdkEventExpose *event,
			 gpointer        data)
{

	guint i;

	/* finalize the graphics context */
	lcd.gc1 = gdk_gc_new (GDK_DRAWABLE (widget->window));
	gdk_gc_set_rgb_fg_color (lcd.gc1, &lcd.fg);
	gdk_gc_set_rgb_bg_color (lcd.gc1, &lcd.bg);
	gdk_gc_set_line_attributes (lcd.gc1, 1,
				    GDK_LINE_SOLID,
				    GDK_CAP_ROUND,
				    GDK_JOIN_ROUND);

	lcd.gc2 = gdk_gc_new (GDK_DRAWABLE (widget->window));
	gdk_gc_set_rgb_fg_color (lcd.gc2, &lcd.bg);
	gdk_gc_set_rgb_bg_color (lcd.gc2, &lcd.fg);
	gdk_gc_set_line_attributes (lcd.gc2, 1,
				    GDK_LINE_SOLID,
				    GDK_CAP_ROUND,
				    GDK_JOIN_ROUND);

				    
	/* draw border around the meter */
	gdk_draw_rectangle (GDK_DRAWABLE (widget->window), lcd.gc2,
			    TRUE, 0, 0, lcd.width, lcd.height);

	gdk_draw_rectangle (GDK_DRAWABLE (widget->window), lcd.gc1,
			    FALSE, 0, 0, lcd.width-1, lcd.height-1);


	for (i=0; i<8; i++) {
		if ((i+1) % 4 == 0) {
			gdk_draw_pixbuf (GDK_DRAWABLE (widget->window),
					 NULL,
					 digits_normal[11],
					 0,
					 0,
					 lcd.digits[i].x,
					 lcd.digits[i].y,
					 -1,
					 -1,
					 GDK_RGB_DITHER_NONE,
					 0,
					 0);
		}
		else {
			gdk_draw_pixbuf (GDK_DRAWABLE (widget->window),
					 NULL,
					 digits_normal[8],
					 0,
					 0,
					 lcd.digits[i].x,
					 lcd.digits[i].y,
					 -1,
					 -1,
					 GDK_RGB_DITHER_NONE,
					 0,
					 0);
		}
	}

	for (i=8; i<15; i++) {
		if (i==12) {
			gdk_draw_pixbuf (GDK_DRAWABLE (widget->window),
					 NULL,
					 digits_small[11],
					 0,
					 0,
					 lcd.digits[i].x,
					 lcd.digits[i].y,
					 -1,
					 -1,
					 GDK_RGB_DITHER_NONE,
					 0,
					 0);
		}
		else {
			gdk_draw_pixbuf (GDK_DRAWABLE (widget->window),
					 NULL,
					 digits_small[8],
					 0,
					 0,
					 lcd.digits[i].x,
					 lcd.digits[i].y,
					 -1,
					 -1,
					 GDK_RGB_DITHER_NONE,
					 0,
					 0);
		}
	}


	/* initialize offscreen buffer */
//	buffer = gdk_pixmap_new (GDK_DRAWABLE (lcd.canvas->window),
//				 lcd.width, lcd.height, -1);

	/* indicate that widget is ready to 
	   be used
	*/
	lcd.exposed = TRUE;


	return TRUE;
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
	lcd.clw = gdk_pixbuf_get_width  (digits_normal[11]);
	lcd.dsw = gdk_pixbuf_get_width  (digits_small[0]);
	lcd.dsh = gdk_pixbuf_get_height (digits_small[0]);
	lcd.csw = gdk_pixbuf_get_width  (digits_small[11]);

	/* calculate drawing area dimensions */
	lcd.width = 6*lcd.dlw + 2*lcd.clw + 8*lcd.dsw + lcd.csw + 2*LCD_MARGIN;
	lcd.height = 80;

	/* calculate screen position for each digit; this will ease the
	   update of the LCD 
	*/
	/* VFO digits */
	lcd.digits[0].x = LCD_MARGIN;
	lcd.digits[1].x = lcd.digits[0].x + lcd.dlw;
	lcd.digits[2].x = lcd.digits[1].x + lcd.dlw;
	lcd.digits[3].x = lcd.digits[2].x + lcd.dlw;
	lcd.digits[4].x = lcd.digits[3].x + lcd.clw;   /**/
	lcd.digits[5].x = lcd.digits[4].x + lcd.dlw;
	lcd.digits[6].x = lcd.digits[5].x + lcd.dlw;
	lcd.digits[7].x = lcd.digits[6].x + lcd.dlw;
	lcd.digits[8].x = lcd.digits[7].x + lcd.clw;   /**/
	lcd.digits[9].x = lcd.digits[8].x + lcd.dsw;
	lcd.digits[10].x = lcd.digits[9].x + lcd.dsw;
	lcd.digits[11].x = lcd.digits[10].x + 3*lcd.dsw; /**/
	lcd.digits[12].x = lcd.digits[11].x + lcd.dsw;
	lcd.digits[13].x = lcd.digits[12].x + lcd.csw;   /**/
	lcd.digits[14].x = lcd.digits[13].x + lcd.dsw;
	
	for (i=0; i<8; i++)
		lcd.digits[i].y = (lcd.height - lcd.dlh)/2;

	for (i=8; i<15; i++)
		lcd.digits[i].y = lcd.digits[1].y + (lcd.dlh-lcd.dsh)-1;

//	lcd.y1l = (lcd.height - gdk_pixbuf_get_height (digits_normal[0])) / 2;
//	lcd.y1s = lcd.y1l + gdk_pixbuf_get_height (digits_normal[0]) - gdk_pixbuf_get_height (digits_small[0]);

}
