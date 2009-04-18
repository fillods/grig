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
/** \file grig-gtk-workarounds.c
 *  \ingroup misc
 *  \brief Utility functions to work around Gtk+bug and limitations.
 *
 * This file contains various utility functions to work around some bugs
 * and limitations in Gtk+. These are currently:
 *
 *     Tooltips for GtkComboBox
 *
 * \bug These functionsshall eventually be removed as they appear or get
 *      fixed in Gtk+
 */
#include <gtk/gtk.h>
#include "compat.h"
#include "grig-gtk-workarounds.h"




/** \brief Create a horizontal pixmap button.
 *
 * The text will be placed to the right of the image.
 * file is only the icon name, not the full path.
 */
GtkWidget *
grig_hpixmap_button (const gchar *file, const gchar *text, const gchar *tooltip)
{
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *box;
    gchar     *path;

    path = pixmap_file_name (file);
    image = gtk_image_new_from_file (path);
    g_free (path);
    box = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), image, TRUE, TRUE, 0);
    if (text != NULL)
        gtk_box_pack_start (GTK_BOX (box), gtk_label_new (text), TRUE, TRUE, 0);

    button = gtk_button_new ();
    gtk_widget_set_tooltip_text (button, tooltip);
    gtk_container_add (GTK_CONTAINER (button), box);

    return button;
}




/** \brief Create a vertical pixmap button.
 *
 * The text will be placed under the image.
 * file is only the icon name, not the full path.
 */
GtkWidget *
grig_vpixmap_button (const gchar *file, const gchar *text, const gchar *tooltip)
{
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *box;
    gchar     *path;

    
    path = pixmap_file_name (file);
    image = gtk_image_new_from_file (path);
    g_free (path);
    box = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), image, TRUE, TRUE, 0);
    if (text != NULL)
        gtk_box_pack_start (GTK_BOX (box), gtk_label_new (text), TRUE, TRUE, 0);

    button = gtk_button_new ();
    gtk_widget_set_tooltip_text (button, tooltip);
    gtk_container_add (GTK_CONTAINER (button), box);

    return button;
}


/** \brief Create a horizontal pixmap button using stock pixmap.
 *
 * The text will be placed to the right of the image.
 * The icon size will be GTK_ICON_SIZE_BUTTON.
 */
GtkWidget *
grig_hstock_button (const gchar *stock_id, const gchar *text, const gchar *tooltip)
{
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *box;
    

    image = gtk_image_new_from_stock (stock_id, GTK_ICON_SIZE_BUTTON);
    box = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), image, TRUE, TRUE, 0);
    if (text != NULL)
        gtk_box_pack_start (GTK_BOX (box), gtk_label_new (text), TRUE, TRUE, 0);

    button = gtk_button_new ();
    gtk_widget_set_tooltip_text (button, tooltip);
    gtk_container_add (GTK_CONTAINER (button), box);

    return button;
}

