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
#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include <glib/gi18n.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "grig-gtk-workarounds.h"
#include "rig-gui-buttons.h"



/** \brief Enumerated values representing the widgets.
 *
 * These values are used by the timeout function to identify
 * each particular widget within the main container. The values
 * are attached to the widgets.
 */
typedef enum rig_gui_buttons_e {
    RIG_GUI_POWER_BUTTON = 1,   /*!< The Power button */
    RIG_GUI_PTT_BUTTON,         /*!< The PTT button */
    RIG_GUI_LOCK_BUTTON,        /*!< Lock dial function */
    RIG_GUI_ATT_SELECTOR,       /*!< Attenuator selector. */
    RIG_GUI_PREAMP_SELECTOR     /*!< Preamp selector. */
} rig_gui_buttons_t;



/** \brief Key to use for attaching widget ID */
#define WIDGET_ID_KEY   "ID"

/** \brief Key to use for attaching signal handler ID */
#define HANDLER_ID_KEY  "SIG"


/* private function prototypes */
static GtkWidget *rig_gui_buttons_create_power_button    (void);
static GtkWidget *rig_gui_buttons_create_ptt_button      (void);
static GtkWidget *rig_gui_buttons_create_lock_button     (void);
static GtkWidget *rig_gui_buttons_create_att_selector    (void);
static GtkWidget *rig_gui_buttons_create_preamp_selector (void);

static void rig_gui_buttons_power_cb    (GtkWidget *, gpointer);
static void rig_gui_buttons_ptt_cb      (GtkWidget *, gpointer);
static void rig_gui_buttons_lock_cb     (GtkWidget *, gpointer);
static void rig_gui_buttons_att_cb      (GtkWidget *, gpointer);
static void rig_gui_buttons_preamp_cb   (GtkWidget *, gpointer);

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
    gtk_box_pack_start (GTK_BOX (vbox),
                rig_gui_buttons_create_lock_button (),
                FALSE, FALSE, 0);

    gtk_box_pack_end   (GTK_BOX (vbox),
                rig_gui_buttons_create_preamp_selector (),
                FALSE, FALSE, 0);

    gtk_box_pack_end   (GTK_BOX (vbox),
                rig_gui_buttons_create_att_selector (),
                FALSE, FALSE, 0);

    /* start readback timer */
    timerid = g_timeout_add (RIG_GUI_BUTTONS_DEF_TVAL,
                    rig_gui_buttons_timeout_exec,
                    vbox);

    /* register timer_stop function at exit */
    gtk_quit_add (gtk_main_level (), rig_gui_buttons_timeout_stop,
                GUINT_TO_POINTER (timerid));

    gtk_widget_show_all (vbox);

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
    powerstat_t  pstat;
    gint         sigid;

    /* create button widget */
    button = gtk_toggle_button_new_with_label (_("Power"));
    gtk_widget_set_tooltip_text (button, _("Power status"));

    /* set correct state */
    pstat = rig_data_get_pstat ();
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), 
                        pstat ? TRUE : FALSE);

    if (!rig_data_has_set_pstat ()) {
        gtk_widget_set_sensitive (button, FALSE);
    }

    /* connect "toggle" signal */
    sigid = g_signal_connect (G_OBJECT (button), "toggled",
                    G_CALLBACK (rig_gui_buttons_power_cb),
                    NULL);

    /* set handler ID */
    g_object_set_data (G_OBJECT (button),
                HANDLER_ID_KEY,
                GINT_TO_POINTER (sigid));

    /* set widget ID */
    g_object_set_data (G_OBJECT (button),
                WIDGET_ID_KEY,
                GUINT_TO_POINTER (RIG_GUI_POWER_BUTTON));

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
    ptt_t        ptt;
    gint         sigid;

    /* create button widget */
    button = gtk_toggle_button_new_with_label (_("PTT"));
    gtk_widget_set_tooltip_text (button, _("Push to talk"));

    /* set correct state */
    ptt = rig_data_get_ptt ();
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), 
                        ptt ? TRUE : FALSE);

    if (!rig_data_has_set_ptt ()) {
        gtk_widget_set_sensitive (button, FALSE);
    }

    /* connect "toggle" signal */
    sigid = g_signal_connect (G_OBJECT (button), "toggled",
                    G_CALLBACK (rig_gui_buttons_ptt_cb),
                    NULL);

    /* set handler ID */
    g_object_set_data (G_OBJECT (button),
                HANDLER_ID_KEY,
                GINT_TO_POINTER (sigid));

    /* set widget ID */
    g_object_set_data (G_OBJECT (button),
                    WIDGET_ID_KEY,
                    GUINT_TO_POINTER (RIG_GUI_PTT_BUTTON));

    return button;
}


/** \brief Create LOCK button.
 *  \return The lock button widget.
 *
 * This function creates the widget which is used to control the
 * dial LOCK ON/OFF.
 */
static GtkWidget *
rig_gui_buttons_create_lock_button    ()
{
    GtkWidget   *button;
    int          status;
    gint         sigid;

    /* create button widget */
    button = gtk_toggle_button_new_with_label (_("Lock"));
    gtk_widget_set_tooltip_text (button, _("Lock tuning dial"));

    /* set correct state */
    status = rig_data_get_lock ();
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), 
                        status ? TRUE : FALSE);

    if (!rig_data_has_set_lock ()) {
        gtk_widget_set_sensitive (button, FALSE);
    }

    /* connect "toggle" signal */
    sigid = g_signal_connect (G_OBJECT (button), "toggled",
                    G_CALLBACK (rig_gui_buttons_lock_cb),
                    NULL);

    /* set handler ID */
    g_object_set_data (G_OBJECT (button),
                HANDLER_ID_KEY,
                GINT_TO_POINTER (sigid));

    /* set widget ID */
    g_object_set_data (G_OBJECT (button),
                    WIDGET_ID_KEY,
                    GUINT_TO_POINTER (RIG_GUI_LOCK_BUTTON));

    return button;
}



/** \brief Create ATT selector.
 *  \return
 */
static GtkWidget *
rig_gui_buttons_create_att_selector    ()
{
    GtkWidget *att;
    gint       i = 0;
    gchar     *text;
    gint       sigid;

    att = gtk_combo_box_new_text ();
                
    /* add ATT OFF ie. 0 dB */
    gtk_combo_box_append_text (GTK_COMBO_BOX (att), _("ATT OFF"));

    /* note: MAXDBLSTSIZ is defined in hamlib; it is the max size of the
        ATT and preamp arrays.
    */
    while ((i < MAXDBLSTSIZ) && rig_data_get_att_data (i)) {

        text = g_strdup_printf ("-%d dB", rig_data_get_att_data (i));
        gtk_combo_box_append_text (GTK_COMBO_BOX (att), text);
        g_free (text);
        i++;
    }

    /* get current ATT value; remember that -1 => ATT OFF
        which is the 0th element in the combo box list.
    */
    i = rig_data_get_att_index (rig_data_get_att ()) + 1;
    gtk_combo_box_set_active (GTK_COMBO_BOX (att), i);

    if (!rig_data_has_set_att ()) {
        gtk_widget_set_sensitive (att, FALSE);
    }

    gtk_widget_set_tooltip_text (att, _("Attenuator level"));

    /* connect 'changed' signal */
    sigid = g_signal_connect (G_OBJECT (att), "changed",
                    G_CALLBACK (rig_gui_buttons_att_cb),
                    NULL);

    /* set handler ID */
    g_object_set_data (G_OBJECT (att),
                HANDLER_ID_KEY,
                GINT_TO_POINTER (sigid));

    /* set widget ID */
    g_object_set_data (G_OBJECT (att),
                WIDGET_ID_KEY,
                GUINT_TO_POINTER (RIG_GUI_ATT_SELECTOR));


    return att;
}



/** \brief Create preamp selector.
 *  \return
 */
static GtkWidget *
rig_gui_buttons_create_preamp_selector    ()
{
    GtkWidget *preamp;
    gint       i = 0;
    gchar     *text;
    gint       sigid;

    preamp = gtk_combo_box_new_text ();
                
    /* add ATT OFF ie. 0 dB */
    gtk_combo_box_append_text (GTK_COMBO_BOX (preamp), _("PREAMP OFF"));

    /* note: MAXDBLSTSIZ is defined in hamlib; it is the max size of the
        ATT and preamp arrays.
    */
    while ((i < MAXDBLSTSIZ) && rig_data_get_preamp_data (i)) {

        text = g_strdup_printf ("%d dB", rig_data_get_preamp_data (i));
        gtk_combo_box_append_text (GTK_COMBO_BOX (preamp), text);
        g_free (text);
        i++;
    }

    /* get current preamp value; remember that -1 => PREAMP OFF
        which is the 0th element in the combo box list.
    */
    i = rig_data_get_preamp_index (rig_data_get_preamp ()) + 1;
    gtk_combo_box_set_active (GTK_COMBO_BOX (preamp), i);

    if (!rig_data_has_set_preamp ()) {
        gtk_widget_set_sensitive (preamp, FALSE);
    }

    gtk_widget_set_tooltip_text (preamp, _("Preamp level"));

    /* connect 'changed' signal */
    sigid = g_signal_connect (G_OBJECT (preamp), "changed",
                    G_CALLBACK (rig_gui_buttons_preamp_cb),
                    NULL);

    /* set handler ID */
    g_object_set_data (G_OBJECT (preamp),
                HANDLER_ID_KEY,
                GINT_TO_POINTER (sigid));

    /* set widget ID */
    g_object_set_data (G_OBJECT (preamp),
                WIDGET_ID_KEY,
                GUINT_TO_POINTER (RIG_GUI_PREAMP_SELECTOR));


    return preamp;
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


/** \brief Set LOCK status.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user clicks on the 'LOCK' button.
 * It check the status of the button and sets the LOCK status accordingly.
 */
static void
rig_gui_buttons_lock_cb   (GtkWidget *widget, gpointer data)
{

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        rig_data_set_lock (1);
    }
    else {
        rig_data_set_lock (0);
    }

}


/** \brief Select attenuator level.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new attenuator level.
 * It acquires the selected menu item, converts it to hamlib ATT value
 * and sends the new value to the rig-data component.
 */
static void
rig_gui_buttons_att_cb   (GtkWidget *widget, gpointer data)
{
    gint index;

    /* get selected item */
    index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

    /* convert it and send to rig-data */
    rig_data_set_att (rig_data_get_att_data (index-1));
}


/** \brief Select preamp level.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new preamp level.
 * It acquires the selected menu item, converts it to hamlib preamp value
 * and sends the new value to the rig-data component.
 */
static void
rig_gui_buttons_preamp_cb   (GtkWidget *widget, gpointer data)
{
    gint index;

    /* get selected item */
    index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

    /* convert it and send to rig-data */
    rig_data_set_preamp (rig_data_get_preamp_data (index-1));
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
 *
 * \note We don't check the availability of the get functionssince
 *       it is done by the daemon an we wish to display some values
 *       anyway.
 */
static void
rig_gui_buttons_update        (GtkWidget *widget, gpointer data)
{
    guint id;
    gint  handler;
    powerstat_t pstat;
    ptt_t       ptt;
    int         attidx;

    /* get widget id */
    id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (widget), WIDGET_ID_KEY));

    switch (id) {

        /* power button */
    case RIG_GUI_POWER_BUTTON:
        
        /* get power status */
        pstat = rig_data_get_pstat ();

        /* get signal handler ID */
        handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), 
                                    HANDLER_ID_KEY));

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
        handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), 
                                    HANDLER_ID_KEY));

        /* block the signal handler */
        g_signal_handler_block (G_OBJECT (widget), handler);

        /* set widget state */
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), 
                            ptt ? TRUE : FALSE);
        
        /* unblock signal handler */
        g_signal_handler_unblock (G_OBJECT (widget), handler);

        break;

        /* ATT selector */
    case RIG_GUI_ATT_SELECTOR:

        /* get signal handler ID */
        handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), 
                                    HANDLER_ID_KEY));
            
        /* block the signal handler */
        g_signal_handler_block (G_OBJECT (widget), handler);

        /* get current ATT value; remember that -1 => ATT OFF
            which is the 0th element in the combo box list.
        */
        attidx = rig_data_get_att_index (rig_data_get_att ()) + 1;
        gtk_combo_box_set_active (GTK_COMBO_BOX (widget), attidx);
            
        /* unblock signal handler */
        g_signal_handler_unblock (G_OBJECT (widget), handler);

        break;

        /* PREAMP selector */
    case RIG_GUI_PREAMP_SELECTOR:

        /* get signal handler ID */
        handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), 
                                    HANDLER_ID_KEY));
            
        /* block the signal handler */
        g_signal_handler_block (G_OBJECT (widget), handler);

        /* get current preamp value; remember that -1 => ATT OFF
            which is the 0th element in the combo box list.
        */
        attidx = rig_data_get_preamp_index (rig_data_get_preamp ()) + 1;
        gtk_combo_box_set_active (GTK_COMBO_BOX (widget), attidx);
            
        /* unblock signal handler */
        g_signal_handler_unblock (G_OBJECT (widget), handler);

        break;


    default:
        break;

    }

}
