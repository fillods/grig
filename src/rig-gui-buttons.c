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
} rig_gui_buttons_t;





/** \brief Key to use for attaching widget ID */
#define WIDGET_ID_KEY   "ID"

/** \brief Key to use for attaching signal handler ID */
#define HANDLER_ID_KEY  "SIG"


/* private function prototypes */
static GtkWidget *rig_gui_buttons_create_power_button    (void);
static GtkWidget *rig_gui_buttons_create_ptt_button      (void);

static void rig_gui_buttons_power_cb    (GtkWidget *, gpointer);
static void rig_gui_buttons_ptt_cb      (GtkWidget *, gpointer);

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
	gtk_box_pack_start (GTK_BOX (vbox),
			    rig_gui_buttons_create_power_button (),
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox),
			    rig_gui_buttons_create_ptt_button (),
			    FALSE, FALSE, 0);

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
			      _("Turn the radio ON or OFF"));

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
			      _("Switch between receive and transmit mode"));

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

	default:
		break;

	}

}
