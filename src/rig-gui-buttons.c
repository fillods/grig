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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <gconf/gconf.h>
#include "rig-data.h"
#include "rig-gui-buttons.h"


/* private function prototypes */
static GtkWidget *rig_gui_buttons_create_power_button    (void);
static GtkWidget *rig_gui_buttons_create_agc_selector    (void);
static GtkWidget *rig_gui_buttons_create_mode_selector   (void);
static GtkWidget *rig_gui_buttons_create_filter_selector (void);



/** \brief Create power, mode, filter and agc buttons.
 *  \return a composite widget containing the controls.
 *
 * This function creates the widgets which are used to set the power,
 * mode, bandwidth and AGC.
 */
GtkWidget *
rig_gui_buttons_create ()
{
	GtkWidget *vbox;    /* container */


	/* create vertical box and add widgets */
	vbox = gtk_vbox_new (TRUE, 0);

	gtk_box_pack_start_defaults (GTK_BOX (vbox), rig_gui_buttons_create_power_button ());
	gtk_box_pack_end_defaults   (GTK_BOX (vbox), rig_gui_buttons_create_agc_selector ());
	gtk_box_pack_end_defaults   (GTK_BOX (vbox), rig_gui_buttons_create_filter_selector ());
	gtk_box_pack_end_defaults   (GTK_BOX (vbox), rig_gui_buttons_create_mode_selector ());

	return vbox;
}


/** \brief Create power button.
 *  \return The power button widget.
 *
 * This function creates the widget which is used to control the
 * power state of the rig.
 */
static GtkWidget *
rig_gui_buttons_create_power_button    ()
{
	GtkWidget *button;

	

	return button;
}


/** \brief Create AGC selector.
 *  \return The AGC selector widget.
 *
 * This function creates the widget used to select the AGC setting.
 */
static GtkWidget *
rig_gui_buttons_create_agc_selector    ()
{
	return NULL;
}


/** \brief Create mode selector.
 *  \return The mode selector widget.
 *
 * This function creates the widget used to select the current mode.
 */
static GtkWidget *
rig_gui_buttons_create_mode_selector   ()
{
	return NULL;
}


/** \brief Create filter selector.
 *  \return The filter selctor widget.
 *
 * This function creates the filter/bandwidth selector widget.
 */
static GtkWidget *
rig_gui_buttons_create_filter_selector ()
{
	return NULL;
}

