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
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-gui-buttons.h"



/** \brief Enumerated values representing the widgets.
 *
 * These values are used by the timeout function to identify
 * each particular widget within the main container. The values
 * are attached to the widgets as gpointers.
 */
typedef enum rig_gui_buttons_e {
	RIG_GUI_POWER_BUTTON = 0,   /*!< The Power button */
	RIG_GUI_PTT_BUTTON,         /*!< The PTT button */
	RIG_GUI_MODE_SELECTOR,      /*!< The mode selector */
	RIG_GUI_FILTER_SELECTOR,    /*!< The filter/passband width selector */
	RIG_GUI_AGC_SELECTOR        /*!< The AGC selector */
} rig_gui_buttons_t;


/* private function prototypes */
static GtkWidget *rig_gui_buttons_create_power_button    (void);
static GtkWidget *rig_gui_buttons_create_ptt_button      (void);
static GtkWidget *rig_gui_buttons_create_agc_selector    (void);
static GtkWidget *rig_gui_buttons_create_mode_selector   (void);
static GtkWidget *rig_gui_buttons_create_filter_selector (void);

static void rig_gui_buttons_power_cb (GtkWidget *, gpointer);
static void rig_gui_buttons_ptt_cb   (GtkWidget *, gpointer);
static void rig_gui_buttons_agc_cb   (GtkWidget *, gpointer);


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
	gtk_box_pack_start_defaults (GTK_BOX (vbox), rig_gui_buttons_create_ptt_button ());
	gtk_box_pack_end_defaults   (GTK_BOX (vbox), rig_gui_buttons_create_agc_selector ());
//	gtk_box_pack_end_defaults   (GTK_BOX (vbox), rig_gui_buttons_create_filter_selector ());
//	gtk_box_pack_end_defaults   (GTK_BOX (vbox), rig_gui_buttons_create_mode_selector ());

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
	GtkWidget   *button;
	GtkTooltips *tips;
	powerstat_t  pstat;
	
	/* create button widget */
	button = gtk_toggle_button_new_with_label (_("Power"));
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, button,
			      _("Power status"),
			      _("Use this button to turn the radio ON or OFF"));

	/* set correct state */
	pstat = rig_data_get_pstat ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), 
				      pstat ? TRUE : FALSE);

	/* connect "toggle" signal */
	g_signal_connect (G_OBJECT (button), "toggled",
			  G_CALLBACK (rig_gui_buttons_power_cb),
			  NULL);

	return button;
}


/** \brief Create PTT button.
 *  \return The power button widget.
 *
 * This function creates the widget which is used to control the
 * PTT state of the rig.
 */
static GtkWidget *
rig_gui_buttons_create_ptt_button    ()
{
	GtkWidget   *button;
	GtkTooltips *tips;
	ptt_t        ptt;
	
	/* create button widget */
	button = gtk_toggle_button_new_with_label (_("PTT"));
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, button,
			      _("Push To Talk"),
			      _("Use this button to switch between receve and transmit mode"));

	/* set correct state */
	ptt = rig_data_get_ptt ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), 
				      ptt ? TRUE : FALSE);

	/* connect "toggle" signal */
	g_signal_connect (G_OBJECT (button), "toggled",
			  G_CALLBACK (rig_gui_buttons_ptt_cb),
			  NULL);

	return button;
}



/** \brief Create AGC selector.
 *  \return The AGC selector widget.
 *
 * This function creates the widget used to select the AGC setting.
 * The used widget is a GtkComboBox
 *
 * \note Hamlib implement 'superfast', 'fast' and 'slow' but we choose to call them
 * 'fast', 'medium' and 'slow' in the same numerical order.
 */
static GtkWidget *
rig_gui_buttons_create_agc_selector    ()
{
	GtkWidget   *combo;
	GtkTooltips *tips;
	

	/* create and initialize widget */
	combo = gtk_combo_box_new_text ();
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, combo,
			      _("Automatic Gain Control"),
			      _("Use this menu to select the delay of the AGC"));

	/* FIXME: Hamlib does also have 'user' */
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("OFF"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Fast"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Medium"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Slow"));

	/* select current level */
	switch (rig_data_get_agc ()) {

	case RIG_AGC_OFF:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
		break;

	case RIG_AGC_SUPERFAST:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);
		break;

	case RIG_AGC_FAST:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
		break;

	case RIG_AGC_SLOW:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 3);
		break;

	default:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), -1);
		break;
	}

	/* connect 'changed' signal */
	g_signal_connect (G_OBJECT (combo), "changed",
			  G_CALLBACK (rig_gui_buttons_agc_cb),
			  NULL);

	return combo;
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



/** \brief Set power status.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user clicks on the 'Power' button.
 * It check the status of the button and sets the power status accordingly.
 */
static void
rig_gui_buttons_power_cb (GtkWidget *widget, gpointer data)
{

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
		rig_data_set_pstat (RIG_POWER_ON);
	}
	else {
		rig_data_set_pstat (RIG_POWER_OFF);
	}

	
}


/** \brief Set PTT status.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user clicks on the 'PTT' button.
 * It check the status of the button and sets the PTT status accordingly.
 */
static void
rig_gui_buttons_ptt_cb   (GtkWidget *widget, gpointer data)
{

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
		rig_data_set_ptt (RIG_PTT_ON);
	}
	else {
		rig_data_set_ptt (RIG_PTT_OFF);
	}

}



/** \brief Select AGC delay.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new AGC value.
 * It acquires the selected menu item and set the agc level accordingly.
 */
static void
rig_gui_buttons_agc_cb   (GtkWidget *widget, gpointer data)
{

	switch (gtk_combo_box_get_active (GTK_COMBO_BOX (widget))) {

	case 0:
		rig_data_set_agc (RIG_AGC_OFF);
		break;

	case 1:
		rig_data_set_agc (RIG_AGC_SUPERFAST);
		break;

	case 2:
		rig_data_set_agc (RIG_AGC_FAST);
		break;

	case 3:
		rig_data_set_agc (RIG_AGC_SLOW);
		break;

	default:
		break;
	}

}

