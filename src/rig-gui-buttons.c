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
#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "rig-gui-buttons.h"
#include "support.h"



/** \brief Enumerated values representing the widgets.
 *
 * These values are used by the timeout function to identify
 * each particular widget within the main container. The values
 * are attached to the widgets.
 */
typedef enum rig_gui_buttons_e {
	RIG_GUI_POWER_BUTTON = 1,   /*!< The Power button */
	RIG_GUI_PTT_BUTTON,         /*!< The PTT button */
	RIG_GUI_MODE_SELECTOR,      /*!< The mode selector */
	RIG_GUI_FILTER_SELECTOR,    /*!< The filter/passband width selector */
	RIG_GUI_AGC_SELECTOR        /*!< The AGC selector */
} rig_gui_buttons_t;





/** \brief Key to use for attaching widget ID */
#define WIDGET_ID_KEY   "ID"

/** \brief Key to use for attaching signal handler ID */
#define HANDLER_ID_KEY  "SIG"

/* private function prototypes */
static GtkWidget *rig_gui_buttons_create_power_button    (void);
static GtkWidget *rig_gui_buttons_create_ptt_button      (void);
static GtkWidget *rig_gui_buttons_create_agc_selector    (void);
static GtkWidget *rig_gui_buttons_create_mode_selector   (void);
static GtkWidget *rig_gui_buttons_create_filter_selector (void);

static void rig_gui_buttons_power_cb    (GtkWidget *, gpointer);
static void rig_gui_buttons_ptt_cb      (GtkWidget *, gpointer);
static void rig_gui_buttons_agc_cb      (GtkWidget *, gpointer);
static void rig_gui_buttons_mode_cb     (GtkWidget *, gpointer);
static void rig_gui_buttons_filter_cb   (GtkWidget *, gpointer);

static gint rig_gui_buttons_timeout_exec  (gpointer);
static gint rig_gui_buttons_timeout_stop  (gpointer);
static void rig_gui_buttons_update        (GtkWidget *, gpointer);


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
	guint timerid;

	/* create vertical box and add widgets */
	vbox = gtk_vbox_new (FALSE, 0);

	/* add controls */
	gtk_box_pack_start (GTK_BOX (vbox), rig_gui_buttons_create_power_button (), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), rig_gui_buttons_create_ptt_button (), FALSE, FALSE, 0);
	gtk_box_pack_end   (GTK_BOX (vbox), rig_gui_buttons_create_agc_selector (), FALSE, FALSE, 0);
	gtk_box_pack_end   (GTK_BOX (vbox), rig_gui_buttons_create_filter_selector (), FALSE, FALSE, 0);
	gtk_box_pack_end   (GTK_BOX (vbox), rig_gui_buttons_create_mode_selector (), FALSE, FALSE, 0);

	/* start readback timer */
	timerid = g_timeout_add (RIG_GUI_BUTTONS_DEF_TVAL,
				 rig_gui_buttons_timeout_exec,
				 vbox);

	/* register timer_stop function at exit */
	gtk_quit_add (gtk_main_level (), rig_gui_buttons_timeout_stop,
		      GUINT_TO_POINTER (timerid));


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
	gint         sigid;
	
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
	sigid = g_signal_connect (G_OBJECT (button), "toggled",
				  G_CALLBACK (rig_gui_buttons_power_cb),
				  NULL);

	/* set widget ID */
	g_object_set_data (G_OBJECT (button),
			   WIDGET_ID_KEY,
			   GUINT_TO_POINTER (RIG_GUI_POWER_BUTTON));

	/* set handler ID */
	g_object_set_data (G_OBJECT (button),
			   HANDLER_ID_KEY,
			   GINT_TO_POINTER (sigid));

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
	gint         sigid;
	
	/* create button widget */
	button = gtk_toggle_button_new_with_label (_("PTT"));
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, button,
			      _("Push To Talk"),
			      _("Use this button to switch between receive and transmit mode"));

	/* set correct state */
	ptt = rig_data_get_ptt ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), 
				      ptt ? TRUE : FALSE);

	/* connect "toggle" signal */
	sigid = g_signal_connect (G_OBJECT (button), "toggled",
				  G_CALLBACK (rig_gui_buttons_ptt_cb),
				  NULL);

	/* set widget ID */
	g_object_set_data (G_OBJECT (button),
			     WIDGET_ID_KEY,
			     GUINT_TO_POINTER (RIG_GUI_PTT_BUTTON));

	/* set handler ID */
	g_object_set_data (G_OBJECT (button),
			     HANDLER_ID_KEY,
			     GINT_TO_POINTER (sigid));

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
 *
 * \bug Grig does not implement the RIG_AGC_USER option!
 */
static GtkWidget *
rig_gui_buttons_create_agc_selector    ()
{
	GtkWidget         *combo;
	GtkTooltips       *tips;
	rig_gui_buttons_t  id;
	gint               sigid;
	

	/* create and initialize widget */
	combo = gtk_combo_box_new_text ();
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, combo,
			      _("Automatic Gain Control"),
			      _("Use this menu to select the delay of the AGC"));

	/* FIXME: Hamlib does also have 'user' */
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("AGC OFF"));
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
	sigid = g_signal_connect (G_OBJECT (combo), "changed",
				  G_CALLBACK (rig_gui_buttons_agc_cb),
				  NULL);

	/* set widget ID */
	g_object_set_data (G_OBJECT (combo),
			     WIDGET_ID_KEY,
			     GUINT_TO_POINTER (RIG_GUI_AGC_SELECTOR));

	/* set handler ID */
	g_object_set_data (G_OBJECT (combo),
			     HANDLER_ID_KEY,
			     GINT_TO_POINTER (sigid));

	return combo;
}



/** \brief Create mode selector.
 *  \return The mode selector widget.
 *
 * This function creates the widget used to select the current mode.
 * The numerical values for the different modes is not linear
 * (1, 2, 3, 4, ...). They are defined as individual bits in a
 * 16 bit integer (1, 2, 4, 8, ...). Thus, to convert between
 * hamlib mode and combo box index we use the following relations:
 *
 *        index = rint[log(mode)/log(2)]
 *        mode  = 1 << index
 *
 * These conversions are done using dedicated functions in the
 * rig-utils component.
 *
 * \bug Grig does not implement the RIG_MODE_NONE mode.
 */
static GtkWidget *
rig_gui_buttons_create_mode_selector   ()
{
	GtkWidget   *combo;
	GtkTooltips *tips;
	gint         sigid;
	

	/* create and initialize widget */
	combo = gtk_combo_box_new_text ();
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, combo,
			      _("Communication Mode"),
			      _("Use this menu to select the communication mode"));

	/* FIXME: Hamlib does also have RIG_MODE_NONE */
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("AM"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("CW"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("USB"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("LSB"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("RTTY"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("FM Narrow"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("FM Wide"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("CW Rev"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("RTTY Rev"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("AM Synch"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Packet (LSB)"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Packet (USB)"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Packet (FM)"));

	/* set current mode */
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo),
				  rig_utils_mode_to_index (rig_data_get_mode ()));

	/* connect 'changed' signal */
	sigid = g_signal_connect (G_OBJECT (combo), "changed",
				  G_CALLBACK (rig_gui_buttons_mode_cb),
				  NULL);

	/* set widget ID */
	g_object_set_data (G_OBJECT (combo),
			     WIDGET_ID_KEY,
			     GUINT_TO_POINTER (RIG_GUI_MODE_SELECTOR));

	/* set handler ID */
	g_object_set_data (G_OBJECT (combo),
			     HANDLER_ID_KEY,
			     GINT_TO_POINTER (sigid));

	return combo;
}


/** \brief Create filter selector.
 *  \return The filter selctor widget.
 *
 * This function creates the filter/bandwidth selector widget. The current
 * implementation only supports the traditional RIG_PASSBAND_NARROW, RIG_PASSBAND_NORMAL
 * and RIG_PASSBAND_WIDE.
 */
static GtkWidget *
rig_gui_buttons_create_filter_selector ()
{
	GtkWidget   *combo;
	GtkTooltips *tips;
	gint         sigid;
	

	/* create and initialize widget */
	combo = gtk_combo_box_new_text ();
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, combo,
			      _("Passband width"),
			      _("Use this menu to select the passband width"));

	/* Add items */
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Wide"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Normal"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Narrow"));

	/* set current passband width */
	switch (rig_data_get_pbwidth ()) {

	case RIG_DATA_PB_WIDE:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
		break;

	case RIG_DATA_PB_NARROW:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
		break;

	default:
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);
		break;
	}

	/* connect 'changed' signal */
	sigid = g_signal_connect (G_OBJECT (combo), "changed",
				  G_CALLBACK (rig_gui_buttons_filter_cb),
				  NULL);

	/* set widget ID */
	g_object_set_data (G_OBJECT (combo),
			     WIDGET_ID_KEY,
			     GUINT_TO_POINTER (RIG_GUI_FILTER_SELECTOR));

	/* set handler ID */
	g_object_set_data (G_OBJECT (combo),
			     HANDLER_ID_KEY,
			     GINT_TO_POINTER (sigid));

	return combo;
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



/** \brief Select mode.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new mode.
 * It acquires the selected menu item, converts it to hamlib mode type
 * and sends the new mode to the rig-data component.
 */
static void
rig_gui_buttons_mode_cb   (GtkWidget *widget, gpointer data)
{
	gint index;

	/* get selected item */
	index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

	/* convert it and send to rig-data */
	rig_data_set_mode (rig_utils_index_to_mode (index));


}





/** \brief Select passband width.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new passband width.
 */
static void
rig_gui_buttons_filter_cb   (GtkWidget *widget, gpointer data)
{
	gint index;

	/* get selected item */
	index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

	/* send it to rig-data */
	rig_data_set_pbwidth (index);


}


/** \brief Execute timeout function.
 *  \param vbox The composite widget containing the controls.
 *  \return Always TRUE to keep the timer running.
 *
 * This function reads the relevant rig settings from the rid-data object and
 * updates the control widgets within vbox. The function is called peridically
 * by the Gtk+ scheduler.
 *
 * \note Because this is an internal service, no checks are made on the sanity
 *       of the parameter (ie. whether it really is the vbox we think it is).
 */
static gint 
rig_gui_buttons_timeout_exec  (gpointer vbox)
{

	/* update each child widget of the container */
	gtk_container_foreach (GTK_CONTAINER (vbox),
			       rig_gui_buttons_update,
			       NULL);

	return TRUE;
}



/** \brief Stop timeout function.
 *  \param timer The ID of the timer to stop.
 *  \return Always TRUE.
 *
 * This function is used to stop the readback timer just before the
 * program is quit. It should be called automatically by Gtk+ when
 * the gtk_main_loop is exited.
 */
static gint 
rig_gui_buttons_timeout_stop  (gpointer timer)
{

	g_source_remove (GPOINTER_TO_UINT (timer));

	return TRUE;
}



/** \brief Update control widget.
 *  \param widget The widget to update.
 *  \param data User data; always NULL.
 *
 * This function is called by the periodic timeout function in
 * order to update the control widgets. It is called with one
 * widget at a time. The function then checks the internal ID
 * of the widget and updates it accordingly.
 *
 * \note No checks are done to compare the current rig setting
 *       with the widget settings, instead the callback signal
 *       is blocked and the widget state is set to the rig state.
 *       Hereafter the signal handler is unblocked again.
 */
static void
rig_gui_buttons_update        (GtkWidget *widget, gpointer data)
{
	guint id;
	gint  handler;
	powerstat_t pstat;
	ptt_t       ptt;


	/* get widget id */
	id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (widget), WIDGET_ID_KEY));

	switch (id) {

		/* power button */
	case RIG_GUI_POWER_BUTTON:
		
		/* get power status */
		pstat = rig_data_get_pstat ();

		/* get signal handler ID */
		handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), HANDLER_ID_KEY));

		/* block the signal handler */
		g_signal_handler_block (G_OBJECT (widget), handler);

		/* set widget state */
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), 
					      pstat ? TRUE : FALSE);
		
		/* unblock signal handler */
		g_signal_handler_unblock (G_OBJECT (widget), handler);

		break;

		/* ptt button */
	case RIG_GUI_PTT_BUTTON:
		
		/* get PTT status */
		ptt = rig_data_get_ptt ();

		/* get signal handler ID */
		handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), HANDLER_ID_KEY));

		/* block the signal handler */
		g_signal_handler_block (G_OBJECT (widget), handler);

		/* set widget state */
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), 
					      ptt ? TRUE : FALSE);
		
		/* unblock signal handler */
		g_signal_handler_unblock (G_OBJECT (widget), handler);

		break;

		/* agc selector */
	case RIG_GUI_AGC_SELECTOR:

		/* get signal handler ID */
		handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), HANDLER_ID_KEY));

		/* block the signal handler */
		g_signal_handler_block (G_OBJECT (widget), handler);

		/* select current level; because we don't support all available AGC
		   settings (like RIG_AGC_USER), we need to handle each supported
		   case individually.
		*/
		switch (rig_data_get_agc ()) {

		case RIG_AGC_OFF:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 0);
			break;

		case RIG_AGC_SUPERFAST:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 1);
			break;

		case RIG_AGC_FAST:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 2);
			break;

		case RIG_AGC_SLOW:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 3);
			break;

		default:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), -1);
			break;
		}
		
		/* unblock signal handler */
		g_signal_handler_unblock (G_OBJECT (widget), handler);

		break;

		/* mode selector */
	case RIG_GUI_MODE_SELECTOR:

		/* get signal handler ID */
		handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), HANDLER_ID_KEY));

		/* block the signal handler */
		g_signal_handler_block (G_OBJECT (widget), handler);

		/* set current mode */
		gtk_combo_box_set_active (GTK_COMBO_BOX (widget),
					  rig_utils_mode_to_index (rig_data_get_mode ()));
		
		/* unblock signal handler */
		g_signal_handler_unblock (G_OBJECT (widget), handler);

		break;

		/* filter selector */
	case RIG_GUI_FILTER_SELECTOR:

		/* get signal handler ID */
		handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), HANDLER_ID_KEY));

		/* block the signal handler */
		g_signal_handler_block (G_OBJECT (widget), handler);

		/* set current passband width */
		switch (rig_data_get_pbwidth ()) {

		case RIG_DATA_PB_WIDE:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 0);
			break;

		case RIG_DATA_PB_NARROW:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 2);
			break;

		default:
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 1);
			break;
		}
		
		/* unblock signal handler */
		g_signal_handler_unblock (G_OBJECT (widget), handler);

		break;

	default:
		break;

	}

}
