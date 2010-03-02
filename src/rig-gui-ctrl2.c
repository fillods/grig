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

/** \bug should disable widgets if not available! */

#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include <glib/gi18n.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "grig-gtk-workarounds.h"
#include "rig-gui-ctrl2.h"



/** \brief Enumerated values representing the widgets.
 *
 * These values are used by the timeout function to identify
 * each particular widget within the main container. The values
 * are attached to the widgets.
 */
typedef enum rig_gui_ctrl2_e {
    RIG_GUI_MODE_SELECTOR = 1,  /*!< The mode selector */
    RIG_GUI_FILTER_SELECTOR,    /*!< The filter/passband width selector */
    RIG_GUI_AGC_SELECTOR,       /*!< The AGC selector */
    RIG_GUI_ANTENNA_SELECTOR    /*!< The antenna selector */
} rig_gui_ctrl2_t;




/** \brief Key to use for attaching widget ID */
#define WIDGET_ID_KEY   "ID"

/** \brief Key to use for attaching signal handler ID */
#define HANDLER_ID_KEY  "SIG"


/** \brief Table to convert mode index to combo box index
 *
 * The hamlib modes can be converted to a linear index using the
 * rig_utils_mode_to_index function. That index can be used in this
 * table to find the actual index in the mode selection combo box.
 * The reason that these two indices are not the same is, that grig
 * only shows the modes, which are supported by the rig.
 *
 * \note -1 means that the mode is not supported by the rig and thus
 *       not present in the combo box.
 *
 * \note RIG_MODE_NONE is not present, thus the first entry is RIG_MODE_AM.
 */
gint midx2cidx[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};


/** \brief Table to convert mode index to string */
const gchar *midx2str[16] = {
    N_("AM"),
    N_("CW"),
    N_("USB"),
    N_("LSB"),
    N_("RTTY"),
    N_("FM Narrow"),
    N_("FM Wide"),
    N_("CW Rev"),
    N_("RTTY Rev"),
    N_("AM Synch"),
    N_("Pkt (LSB)"),
    N_("Pkt (USB)"),
    N_("Pkt (FM)"),
    N_("ECUSB"),
    N_("ECLSB"),
    N_("FAX")
};


/** \brief Table to convert combo box index to hamlib mode. */
gint cidx2mode[16] = {
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE,
    RIG_MODE_NONE
};


/* private function prototypes */
static GtkWidget *rig_gui_ctrl2_create_agc_selector    (void);
static GtkWidget *rig_gui_ctrl2_create_mode_selector   (void);
static GtkWidget *rig_gui_ctrl2_create_filter_selector (void);
static GtkWidget *rig_gui_ctrl2_create_antenna_selector (void);

static void rig_gui_ctrl2_agc_cb      (GtkWidget *, gpointer);
static void rig_gui_ctrl2_mode_cb     (GtkWidget *, gpointer);
static void rig_gui_ctrl2_filter_cb   (GtkWidget *, gpointer);
static void rig_gui_ctrl2_antenna_cb  (GtkWidget *, gpointer);

static gint rig_gui_ctrl2_timeout_exec  (gpointer);
static gint rig_gui_ctrl2_timeout_stop  (gpointer);
static void rig_gui_ctrl2_update        (GtkWidget *, gpointer);



/** \brief Create mode, filter, agc, and antenna buttons.
 *  \return a composite widget containing the controls.
 *
 * This function creates the widgets which are used to select the
 * mode, bandwidth, AGC, and antenna.
 */
GtkWidget *
rig_gui_ctrl2_create ()
{
    GtkWidget *vbox;    /* container */
    guint timerid;

    /* create vertical box and add widgets */
    vbox = gtk_vbox_new (FALSE, 0);

    /* add controls */
    gtk_box_pack_start   (GTK_BOX (vbox),
                    rig_gui_ctrl2_create_mode_selector (),
                    FALSE, FALSE, 0);
    gtk_box_pack_start   (GTK_BOX (vbox),
                    rig_gui_ctrl2_create_filter_selector (),
                    FALSE, FALSE, 0);
    gtk_box_pack_start   (GTK_BOX (vbox),
                    rig_gui_ctrl2_create_agc_selector (),
                    FALSE, FALSE, 0);
    gtk_box_pack_start   (GTK_BOX (vbox),
                    rig_gui_ctrl2_create_antenna_selector (),
                    FALSE, FALSE, 0);

    /* start readback timer */
    timerid = g_timeout_add (RIG_GUI_CTRL2_DEF_TVAL,
                    rig_gui_ctrl2_timeout_exec,
                    vbox);

    /* register timer_stop function at exit */
    gtk_quit_add (gtk_main_level (), rig_gui_ctrl2_timeout_stop,
                GUINT_TO_POINTER (timerid));

    gtk_widget_show_all (vbox);

    return vbox;
}



/** \brief Create AGC selector.
 *  \return The AGC selector widget.
 *
 * This function creates the widget used to select the AGC setting.
 * The used widget is a GtkComboBox
 *
 * \bug Grig does not implement the RIG_AGC_USER option!
 */
static GtkWidget *
rig_gui_ctrl2_create_agc_selector    ()
{
    GtkWidget         *combo;
    gint               sigid;


    /* create and initialize widget */
    combo = gtk_combo_box_new_text ();

    /* FIXME: Hamlib does also have 'user' */
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("AGC OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Super Fast"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Fast"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Medium"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Slow"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Auto"));

    gtk_widget_set_tooltip_text (combo, _("Automatic Gain Control Level"));

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

    case RIG_AGC_MEDIUM:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 3);
        break;

    case RIG_AGC_SLOW:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 4);
        break;

    case RIG_AGC_AUTO:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 5);
        break;

    default:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), -1);
        break;
    }

    /* connect 'changed' signal */
    sigid = g_signal_connect (G_OBJECT (combo), "changed",
                    G_CALLBACK (rig_gui_ctrl2_agc_cb),
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
 * rig-utils package.
 *
 * \bug Grig does not implement the RIG_MODE_NONE mode.
 */
static GtkWidget *
rig_gui_ctrl2_create_mode_selector   ()
{
    GtkWidget   *combo;
    gint         sigid;
    gint         index = 0;
    gint         i,mode;

    /* create and initialize widget */
    combo = gtk_combo_box_new_text ();

    /* loop over all modes */
    for (i = 0; i < 16; i++) {
        
        mode = 1 << i;

        /* if this mode is supported, add entry to combo box,
            store indices and increment combo box index
        */
        if (rig_data_get_all_modes () & mode) {

            gtk_combo_box_append_text (GTK_COMBO_BOX (combo),
                            _(midx2str[i]));

            midx2cidx[i] = index;
            cidx2mode[index] = mode;
            index++;
        }
    }


    /* set current mode */
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo),
                    midx2cidx[rig_utils_mode_to_index (rig_data_get_mode ())]);

    gtk_widget_set_tooltip_text (combo, _("Communication mode"));

    /* connect 'changed' signal */
    sigid = g_signal_connect (G_OBJECT (combo), "changed",
                    G_CALLBACK (rig_gui_ctrl2_mode_cb),
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
rig_gui_ctrl2_create_filter_selector ()
{
    GtkWidget   *combo;
    gint         sigid;


    /* create and initialize widget */
    combo = gtk_combo_box_new_text ();

    /* Add items */
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Wide"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Normal"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Narrow"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("[User]"));

    /* set current passband width */
    switch (rig_data_get_pbwidth ()) {

    case RIG_DATA_PB_WIDE:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
        break;

    case RIG_DATA_PB_NORMAL:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);
        break;

    case RIG_DATA_PB_NARROW:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
        break;

    default:
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 3);
        break;
    }

    gtk_widget_set_tooltip_text (combo, _("Passband Width"));

    /* connect 'changed' signal */
    sigid = g_signal_connect (G_OBJECT (combo), "changed",
                    G_CALLBACK (rig_gui_ctrl2_filter_cb),
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

/** \brief Create antenna selector.
 *  \return The antenna selector widget.
 *
 * This function creates the widget used to select the current antenna.
 * The numerical values for the different antenna is not linear
 * (1, 2, 3, 4, ...). They are defined as individual bits in a
 * 16 bit integer (1, 2, 4, 8, ...). Thus, to convert between
 * hamlib mode and combo box index we use the following relations:
 *
 *        index = rint[log(mode)/log(2)]
 *        mode  = 1 << index
 *
 * These conversions are done using dedicated functions in the
 * rig-utils package.
 *
 * \bug Grig does not implement the RIG_ANT_NONE antenna.
 */
static GtkWidget *
rig_gui_ctrl2_create_antenna_selector   ()
{
	GtkWidget   *combo;
	gint         sigid;
	gint         index = 0;
	gint         i,antenna;
	gchar        antstr[16];

	/* create and initialize widget */
	combo = gtk_combo_box_new_text ();

	/* loop over all antennas */
	for (i = 0; i < 8; i++) {
		
		antenna = 1 << i;

		/* if this antenna is supported, add entry to combo box,
		   store indices and increment combo box index
		*/
		if (rig_data_get_all_antennas () & antenna) {

			snprintf(antstr, sizeof(antstr)-1, _("ANT %d"), i+1);

			gtk_combo_box_append_text (GTK_COMBO_BOX (combo),
						   antstr);

			midx2cidx[i] = index;
			cidx2mode[index] = antenna;
			index++;
		}
	}
	

	/* set current antenna */
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo),
				  midx2cidx[rig_utils_mode_to_index (rig_data_get_antenna ())]);

	/* add tooltips when widget is realized */
    gtk_widget_set_tooltip_text (combo, _("Antenna Port"));

	/* connect 'changed' signal */
	sigid = g_signal_connect (G_OBJECT (combo), "changed",
				  G_CALLBACK (rig_gui_ctrl2_antenna_cb),
				  NULL);

	/* set widget ID */
	g_object_set_data (G_OBJECT (combo),
			   WIDGET_ID_KEY,
			   GUINT_TO_POINTER (RIG_GUI_ANTENNA_SELECTOR));

	/* set handler ID */
	g_object_set_data (G_OBJECT (combo),
			   HANDLER_ID_KEY,
			   GINT_TO_POINTER (sigid));

	return combo;
}




/** \brief Select AGC delay.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new AGC value.
 * It acquires the selected menu item and set the agc level accordingly.
 *
 * \note Numerically MEDIUM > SLOW > FAST (mediumwas added to hamlib too late)
 */
static void
rig_gui_ctrl2_agc_cb   (GtkWidget *widget, gpointer data)
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
        rig_data_set_agc (RIG_AGC_MEDIUM);
        break;

    case 4:
        rig_data_set_agc (RIG_AGC_SLOW);
        break;
    case 5:
        rig_data_set_agc (RIG_AGC_AUTO);
        break;


    default:
        /* internal error; bug */
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
rig_gui_ctrl2_mode_cb   (GtkWidget *widget, gpointer data)
{
    gint index;

    /* get selected item */
    index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

    /* convert it and send to rig-data */
    rig_data_set_mode (cidx2mode[index]);
}



/** \brief Select passband width.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new passband width.
 *
 */
static void
rig_gui_ctrl2_filter_cb   (GtkWidget *widget, gpointer data)
{
    gint index;

    /* get selected item */
    index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

    if (index > 2)
        index = 1;

    /* send it to rig-data */
    rig_data_set_pbwidth (index);

}

/** \brief Select antenna.
 *  \param widget The widget which received the signal.
 *  \param data   User data, always NULL.
 *
 * This function is called when the user selects a new antenna.
 * It acquires the selected menu item, converts it to hamlib antenna type
 * and sends the new antenna to the rig-data component.
 */
static void
rig_gui_ctrl2_antenna_cb   (GtkWidget *widget, gpointer data)
{
	gint index;

	/* get selected item */
	index = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

	/* convert it and send to rig-data */
	rig_data_set_antenna (cidx2mode[index]);


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
rig_gui_ctrl2_timeout_exec  (gpointer vbox)
{

    /* update each child widget of the container */
    gtk_container_foreach (GTK_CONTAINER (vbox),
                    rig_gui_ctrl2_update,
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
rig_gui_ctrl2_timeout_stop  (gpointer timer)
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
rig_gui_ctrl2_update        (GtkWidget *widget, gpointer data)
{
    guint id;
    gint  handler;


    /* get widget id */
    id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (widget), WIDGET_ID_KEY));

    switch (id) {


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

        case RIG_AGC_MEDIUM:
            gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 3);
            break;

        case RIG_AGC_SLOW:
            gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 4);
            break;

        case RIG_AGC_AUTO:
            gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 5);
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
                        midx2cidx[rig_utils_mode_to_index (rig_data_get_mode ())]);
        
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

		/* antenna selector */
	case RIG_GUI_ANTENNA_SELECTOR:

		/* get signal handler ID */
		handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (widget), HANDLER_ID_KEY));

		/* block the signal handler */
		g_signal_handler_block (G_OBJECT (widget), handler);

		/* set current antenna */
		gtk_combo_box_set_active (GTK_COMBO_BOX (widget),
					  midx2cidx[rig_utils_mode_to_index (rig_data_get_antenna ())]);
		
		/* unblock signal handler */
		g_signal_handler_unblock (G_OBJECT (widget), handler);

		break;

    default:
        break;

    }

}
