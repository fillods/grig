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
#include "grig-debug.h"
#include "rig-gui-vfo.h"





/* private function prototypes */

/* VFO frame */
static GtkWidget *rig_gui_vfo_create_toggle       (void);
static GtkWidget *rig_gui_vfo_create_eq_button    (void);
static GtkWidget *rig_gui_vfo_create_xchg_button  (void);
static GtkWidget *rig_gui_vfo_create_split_button (void);

static void rig_gui_vfo_toggle_cb  (GtkWidget *, gpointer);
static void rig_gui_vfo_eq_cb      (GtkWidget *, gpointer);
static void rig_gui_vfo_xchg_cb    (GtkWidget *, gpointer);
static void rig_gui_vfo_split_cb   (GtkWidget *, gpointer);

/* MEM Frame */


/* Band frame */


/** \brief Create VFO operation controls.
 *  \return a composite widget containing the controls.
 *
 */
GtkWidget *
rig_gui_vfo_create ()
{
	GtkWidget *hbox;
	GtkWidget *vfobox;
	GtkWidget *bandbox;

	/* VFO Frame */
	vfobox = gtk_vbutton_box_new ();
	gtk_button_box_set_layout (GTK_BUTTON_BOX (vfobox), GTK_BUTTONBOX_START);
	gtk_container_add (GTK_CONTAINER (vfobox), rig_gui_vfo_create_toggle ());
	gtk_container_add (GTK_CONTAINER (vfobox), rig_gui_vfo_create_split_button ());
	gtk_container_add (GTK_CONTAINER (vfobox), rig_gui_vfo_create_eq_button ());
	gtk_container_add (GTK_CONTAINER (vfobox), rig_gui_vfo_create_xchg_button ());

	/* BAND UP/DOWN */
	/* XXX not yet implemented */
/*
	bandbox = gtk_hbutton_box_new ();
	gtk_button_box_set_layout (GTK_BUTTON_BOX (bandbox), GTK_BUTTONBOX_END);
*/
			    
	return vfobox;
}


/**** A/B button ****/


static GtkWidget *
rig_gui_vfo_create_toggle ()
{
	GtkWidget   *button;
	GtkTooltips *tips;
	gint         sigid;
	gint         vfos;
	
	/* Create button widget.
           The label will be "A/B" if the rig has VFO_A and VFO_B
           or Main/Sub if the rig has those two.
        */
	vfos = rig_data_get_vfos ();
	if (vfos & RIG_VFO_MAIN) {
		button = gtk_button_new_with_label (_("Main / Sub"));
		tips = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tips, button,
				      _("Toggle active VFO"),
				      _("Toggle between the two available VFOs"));
	}
	else {
		button = gtk_button_new_with_label (_("A / B"));
		tips = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tips, button,
				      _("Toggle VFO"),
				      _("Toggle between the two available VFOs"));
	}


        /* Disable control if the rig has no capability of
           either setting a specific VFO or to toggle
        */
	if (!(rig_data_has_vfo_op_toggle () ||
              (rig_data_has_set_vfo () && rig_data_has_get_vfo ())) ){
		gtk_widget_set_sensitive (button, FALSE);
	}


	/* connect "toggle" signal */
	sigid = g_signal_connect (G_OBJECT (button), "pressed",
				  G_CALLBACK (rig_gui_vfo_toggle_cb),
				  NULL);


	return button;
}




static void
rig_gui_vfo_toggle_cb (GtkWidget *widget, gpointer data)
{

	if (rig_data_has_vfo_op_toggle ()) {
		rig_data_vfo_op_toggle ();
	}
	else if (rig_data_has_set_vfo () &&
                 rig_data_has_get_vfo ()) {

                /* do we have VFO A and B? */
                if (rig_data_get_vfos() & (RIG_VFO_A | RIG_VFO_B)) {
                        
                        if (rig_data_get_vfo () == RIG_VFO_A) {
                                rig_data_set_vfo (RIG_VFO_B);
                        }
                        else {
                                rig_data_set_vfo (RIG_VFO_A);
                        }

                }
                /* else try MAIN/SUB */
                else if (rig_data_get_vfos () & (RIG_VFO_MAIN | RIG_VFO_SUB)) {

                        if (rig_data_get_vfo () == RIG_VFO_MAIN) {
                                rig_data_set_vfo (RIG_VFO_SUB);
                        }
                        else {
                                rig_data_set_vfo (RIG_VFO_MAIN);
                        }
                }

                /* it's a bug, because button should be disabled */
                else {
                        grig_debug_local (RIG_DEBUG_BUG,
					  "%s: VFO_TOGGLE button should have been disabled "\
					  "(neither A/B nor MAIN/SUB)\n", __FUNCTION__);
                }
	}

        /* it's a bug, because button should be disabled */
        else {
		grig_debug_local (RIG_DEBUG_BUG,
				  "%s: VFO_TOGGLE button should have been disabled "\
				  "(no way to toggle)\n", __FUNCTION__);
        }

}


/**** A=B button ****/

static GtkWidget *
rig_gui_vfo_create_eq_button ()
{
	GtkWidget   *button;
	GtkTooltips *tips;
	gint         sigid;
	gint         vfos;
	
	/* Create button widget.
           The label will be "A=B" if the rig has VFO_A and VFO_B
           or Main=Sub if the rig has those two.
        */
	vfos = rig_data_get_vfos ();
	if (vfos & RIG_VFO_MAIN) {
		button = gtk_button_new_with_label (_("Main = Sub"));
		tips = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tips, button,
				      _("Set Main VFO = Sub VFO"),
				      NULL);
	}
	else {
		button = gtk_button_new_with_label (_("A = B"));
		tips = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tips, button,
				      _("Set VFO A = VFO B"),
				      NULL);
	}


        /* Disable control if the rig has no capability of
           either setting a specific VFO or to toggle
        */
	if (!(rig_data_has_vfo_op_toggle () ||
              (rig_data_has_set_vfo () && rig_data_has_get_vfo ())) ){
		gtk_widget_set_sensitive (button, FALSE);
	}


	/* connect "toggle" signal */
	sigid = g_signal_connect (G_OBJECT (button), "pressed",
				  G_CALLBACK (rig_gui_vfo_eq_cb),
				  NULL);


	return button;
}




static void
rig_gui_vfo_eq_cb (GtkWidget *widget, gpointer data)
{

	if (rig_data_has_vfo_op_copy ()) {
		rig_data_vfo_op_copy ();
	}
	else if (rig_data_has_set_vfo () &&
                 rig_data_has_get_vfo ()) {

		grig_debug_local (RIG_DEBUG_BUG,
				  "%s: VFO COPY without RIG_OP_COPY not imlemented\n",
				  __FUNCTION__);

	}

        /* it's a bug, because button should be disabled */
        else {
		grig_debug_local (RIG_DEBUG_BUG,
				  "%s: VFO_EQ button should have been disabled "\
				  "(no way to equalise)\n", __FUNCTION__);
        }

}



/**** A<->B button ****/

static GtkWidget *
rig_gui_vfo_create_xchg_button ()
{
	GtkWidget   *button;
	GtkTooltips *tips;
	gint         sigid;
	gint         vfos;
	
	/* Create button widget.
           The label will be "A<->B" if the rig has VFO_A and VFO_B
           or Main<->Sub if the rig has those two.
        */
	vfos = rig_data_get_vfos ();
	if (vfos & RIG_VFO_MAIN) {
		button = gtk_button_new_with_label (_("Main\302\253\302\273Sub"));
		tips = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tips, button,
				      _("Exchange Main and sub VFOs"),
				      NULL);
	}
	else {
		button = gtk_button_new_with_label (_("A\302\253\302\273B"));
		tips = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tips, button,
				      _("Exchange VFO A and B"),
				      NULL);
	}


        /* Disable control if the rig has no capability of
           either setting a specific VFO or to toggle
        */
	if (!(rig_data_has_vfo_op_xchg () ||
              (rig_data_has_set_vfo () && rig_data_has_get_vfo ())) ){
		gtk_widget_set_sensitive (button, FALSE);
	}


	/* connect "toggle" signal */
	sigid = g_signal_connect (G_OBJECT (button), "pressed",
				  G_CALLBACK (rig_gui_vfo_xchg_cb),
				  NULL);


	return button;
}




static void
rig_gui_vfo_xchg_cb (GtkWidget *widget, gpointer data)
{

	if (rig_data_has_vfo_op_xchg ()) {
		rig_data_vfo_op_xchg ();
	}
	else if (rig_data_has_set_vfo () &&
                 rig_data_has_get_vfo ()) {

		grig_debug_local (RIG_DEBUG_BUG,
				  "%s: VFO XCHG without RIG_OP_XCHG not imlemented\n",
				  __FUNCTION__);

	}

        /* it's a bug, because button should be disabled */
        else {
		grig_debug_local (RIG_DEBUG_BUG,
				  "%s: VFO_XCHG button should have been disabled "\
				  "(no way to exchange)\n", __FUNCTION__);
        }

}




/**** SPLIT button ****/
/** FIXME: need readback from RIG **/
static GtkWidget *
rig_gui_vfo_create_split_button ()
{
	GtkWidget   *button;
	GtkTooltips *tips;
	gint         sigid;

	
	/* Create button widget.
           The label will be "A<->B" if the rig has VFO_A and VFO_B
           or Main<->Sub if the rig has those two.
        */
	button = gtk_toggle_button_new_with_label (_("Split"));
	tips = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tips, button,
			      _("Toggle split mode operation"),
			      NULL);

	/* set button status before we do anything else */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button),
				      rig_data_get_split ());

        /* Disable control if the rig has no capability of
           either setting a specific VFO or to toggle
        */
	if (!(rig_data_has_set_split ()))  {
		gtk_widget_set_sensitive (button, FALSE);
	}


	/* connect "toggle" signal */
	sigid = g_signal_connect (G_OBJECT (button), "toggled",
				  G_CALLBACK (rig_gui_vfo_split_cb),
				  NULL);


	return button;
}




static void
rig_gui_vfo_split_cb (GtkWidget *widget, gpointer data)
{

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
		rig_data_set_split (TRUE);
	}
	else {
		rig_data_set_split (FALSE);
	}

}
