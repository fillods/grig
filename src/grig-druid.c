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
#include "grig-druid.h"


/* private function prototypes */
static GtkWidget *grig_druid_first_page_create (void);
static GtkWidget *grig_druid_last_page_create  (void);
static void       grig_druid_help_cb           (GtkWidget *, gpointer);
static void       grig_druid_cancel_cb         (GtkWidget *, gpointer);


/** \brief Run configuration druid.
 *  \return 0 if configuration process OK.
 *
 * This function creates and executes the configuration druid which
 * guides the user through the initial setup. 
 */
gint
grig_druid_run ()
{
	GtkWidget *druid;     /* configuration druid */
	GtkWidget *window;    /* top level window */
	gchar     *iconpath;  /* path to icon file */
	GdkPixbuf *icon;      /* window icon */

	/* create druid */
	druid = gnome_druid_new_with_window (_("Gnome RIG configuration druid"),
					     NULL,
					     TRUE,
					     &window);

	/* add druid pages */
	gnome_druid_append_page (GNOME_DRUID (druid),
				 GNOME_DRUID_PAGE (grig_druid_first_page_create ()));

/* 	gnome_druid_append_page (GNOME_DRUID (druid), */
/* 				 GNOME_DRUID_PAGE (grig_druid_location_page_create ())); */

/* 	gnome_druid_append_page (GNOME_DRUID (druid), */
/* 				 GNOME_DRUID_PAGE (grig_druid_rig_page_create ())); */
	
/* 	gnome_druid_append_page (GNOME_DRUID (druid), */
/* 				 GNOME_DRUID_PAGE (grig_druid_rot_page_create ())); */

	gnome_druid_append_page (GNOME_DRUID (druid),
				 GNOME_DRUID_PAGE (grig_druid_last_page_create ()));

	/* set up misc settings */
	gnome_druid_set_show_help (GNOME_DRUID (druid), TRUE);

	/* set window icon */
	iconpath = g_strconcat (PACKAGE_DATA_DIR, "/pixmaps/", PACKAGE, "/ic910.png", NULL);
	icon = gdk_pixbuf_new_from_file (iconpath, NULL);
	gtk_window_set_icon (GTK_WINDOW (window), icon);

	/* free memory */
	g_free (iconpath);
	g_object_unref (G_OBJECT (icon));

	/* show window */
	gtk_widget_show_all (window);

//	gtk_main ();

	/* check config result and return error code */
	return 0;
}



static GtkWidget *
grig_druid_first_page_create ()
{
	GtkWidget *page;
	gchar     *iconpath;  /* path to icon file */
	GdkPixbuf *icon;      /* logo icon */
	const gchar *text = N_("On the following pages you will be "\
			       "guided through the initial setup of "\
			       "Gnome RIG. You will be asked to specify "\
			       "your location, default radio and rotator.\n"\
			       "Please note that these steps are necessary "\
			       "in order to be able to run Gnome RIG."); 

	/* load icon */
	iconpath = g_strconcat (PACKAGE_DATA_DIR, "/pixmaps/", PACKAGE, "/ic910.png", NULL);
	icon = gdk_pixbuf_new_from_file (iconpath, NULL);

	/* create page */
	page = gnome_druid_page_edge_new_with_vals (GNOME_EDGE_START,
						    FALSE,
						    _("Welcome to Gnome RIG!"),
						    text,
						    icon,
						    NULL,
						    NULL);

	/* free some memory */
	g_free (iconpath);
	g_object_unref (G_OBJECT (icon));


	return page;
}



static GtkWidget *
grig_druid_last_page_create  ()
{
	GtkWidget *page;
	const gchar *text = N_("We have now gathered all the necessary information. "\
			       "You can find a summary of the settings below. If they "\
			       "look all right, press APPLY to continue, otherwise "\
			       "you can go back and modify your settings.");

	page = gnome_druid_page_edge_new_with_vals (GNOME_EDGE_FINISH,
						    FALSE,
						    _("Configuration complete!"),
						    text,
						    NULL,
						    NULL,
						    NULL);

	return page;
}



static void
grig_druid_help_cb           (GtkWidget *druid, gpointer data)
{
}




static void
grig_druid_cancel_cb         (GtkWidget *druid, gpointer data)
{
}
