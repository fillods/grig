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
/** \file    key-press-handler.c
 *  \brief   Manage key press events.
 *
 * This module is snoops key press events. To be written...
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>
#include <hamlib/rig.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "rig-data.h"
#include "grig-debug.h"
#include "rig-gui-lcd.h"
#include "key-press-handler.h"



static gint snooper (GtkWidget *grab_widget, GdkEventKey *event, gpointer func_data);

static gint handler_id = -1;


void
key_press_handler_init  ()
{
    grig_debug_local (RIG_DEBUG_VERBOSE, _("Initialising key press handler"));

    handler_id = gtk_key_snooper_install (snooper, NULL);
}



void
key_press_handler_close ()
{
    grig_debug_local (RIG_DEBUG_VERBOSE, _("Closing key press handler"));

    gtk_key_snooper_remove (handler_id);
}


static gint
snooper (GtkWidget *grab_widget, GdkEventKey *event, gpointer func_data)
{
    gint stop_processing = FALSE;
    freq_t freq;


    switch (event->keyval) {

        /* Arrow Right: Increase frequency with lowest step */
    case GDK_Right:

        if (event->type == GDK_KEY_PRESS) {
            freq = rig_data_get_freq (1) + rig_data_get_fstep ();
            rig_data_set_freq (1, freq);
            rig_gui_lcd_set_freq_digits(freq);
        }
        
        /* inhibit further processing of event */
        stop_processing = TRUE;
        break;

        /* Arrow Left: Descrease frequency with lowest step */
    case GDK_Left:

        if (event->type == GDK_KEY_PRESS) {
            freq = rig_data_get_freq (1) - rig_data_get_fstep ();
            rig_data_set_freq (1, freq);
            rig_gui_lcd_set_freq_digits(freq);
        }

        /* inhibit further processing of event */
        stop_processing = TRUE;
        break; 

    default:
        /* key is not handled */
        stop_processing = FALSE;
        break;
    }
    
    return stop_processing;
}

