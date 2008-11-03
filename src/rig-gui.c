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
/** \file rig-gui.c
 *  \ingroup gui
 *  \brief Master GUI object.
 *
 * This file encapsulates the various GUI parts into one big composite widget.
 */
#include <gtk/gtk.h>
#include "rig-gui.h"
#include "rig-gui-buttons.h"
#include "rig-gui-ctrl2.h"
#include "rig-gui-smeter.h"
#include "rig-gui-lcd.h"
#include "rig-gui-keypad.h"
#include "rig-gui-levels.h"
#include "rig-gui-vfo.h"
#include "grig-menubar.h"


/* we keep this global so that we can enable and disable it at runtime */
static GtkWidget *keypadbox = NULL;



static void
rig_gui_freq_changed_cb(GtkWidget *widget, gpointer data)
{
	grig_keypad_disable(data);
}

static void
rig_gui_keypad_enter_cb(GtkWidget * widget, gpointer data)
{
	rig_gui_lcd_begin_manual_entry();
}

static void
rig_gui_keypad_clear_cb(GtkWidget * widget, gpointer data)
{
	rig_gui_lcd_clear_manual_entry();
}

static void
rig_gui_keypad_num_cb(GtkWidget * widget, guint num)
{
	rig_gui_lcd_set_next_digit('0' + num);
}


void rig_gui_show_keypad (gboolean *show)
{
    if (keypadbox != NULL) {
        if (show) {
            gtk_widget_show_all (keypadbox);
        }
        else {
            gtk_widget_hide_all (keypadbox);
        }
    }
} 


/** \brief Create rig control widgets.
 *  \return A mega-widget containing the rig controls.
 *
 * This function creates the rig control mega-widget by calling the create
 * function of each sub-object and packing the into a main container.
 *
 */
GtkWidget *
rig_gui_create ()
{
	GtkWidget *hbox;     /* the main container */
	GtkWidget *vbox;
	GtkWidget *lcdbox;
	GtkWidget *lcd;
	GtkWidget *keypad;


	lcd = rig_gui_lcd_create();
	keypad = grig_keypad_new();

	/* horizontal box with keypad and vfo */

	keypadbox = gtk_hbox_new(FALSE, 0);

	gtk_box_pack_start (GTK_BOX (keypadbox), keypad,
			    TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (keypadbox), rig_gui_vfo_create (),
			    FALSE, FALSE, 0);
    gtk_widget_show (keypadbox);

	/* vertical box with lcd and keypad + vfo */

	lcdbox = gtk_vbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (lcdbox), lcd,
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (lcdbox), keypadbox,
			    FALSE, FALSE, 5);
    gtk_widget_show (lcdbox);

	/* create the main container */
	/* from left to right: buttons, smeter, (lcd + keypad), ctrl2 */

	hbox = gtk_hbox_new (FALSE, 5);

	gtk_box_pack_start (GTK_BOX (hbox), rig_gui_buttons_create (),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), rig_gui_smeter_create (),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), lcdbox,
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), rig_gui_ctrl2_create (),
			    FALSE, FALSE, 0);
    gtk_widget_show (hbox);

	/* ceate main vertical box */
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), grig_menubar_create (),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, FALSE, 0);

/* 	gtk_box_pack_start (GTK_BOX (vbox), rig_gui_levels_create (), */
/* 			    FALSE, FALSE, 5); */


	/* keypad callbacks */

	g_signal_connect(G_OBJECT(keypad), "grig-keypad-enter-pressed",
		G_CALLBACK(rig_gui_keypad_enter_cb), NULL);

	g_signal_connect(G_OBJECT(keypad), "grig-keypad-clear-pressed",
		G_CALLBACK(rig_gui_keypad_clear_cb), NULL);

	g_signal_connect(G_OBJECT(keypad), "grig-keypad-num-pressed",
		G_CALLBACK(rig_gui_keypad_num_cb), NULL);

	/* disable the keypad when the frequency has been set */
	g_signal_connect (G_OBJECT (lcd), "freq-changed",
			G_CALLBACK (rig_gui_freq_changed_cb), keypad);

    gtk_widget_show_all (vbox);
    
	return vbox;
}

