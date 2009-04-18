/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C) 2008 Alessandro Zummo

    Authors: Alessandro Zummo <a.zummo@towertech.it>

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
#include <gdk/gdkkeysyms.h>
#include <hamlib/rig.h>
#include <glib/gi18n.h>
#include "rig-data.h"
#include "rig-utils.h"
#include "grig-gtk-workarounds.h"
#include "grig-debug.h"
#include "rig-gui-keypad.h"
#include "rig-gui-lcd.h"


static char *labels[] = {
    "0", "1", "2",
    "3", "4", "5",
    "6", "7", "8", "9",
};

/* private function prototypes */

static GtkWidget *rig_gui_keypad_create_button(gint num, GrigKeypad *self);
static GtkWidget *rig_gui_keypad_create_enter(void);
static GtkWidget *rig_gui_keypad_create_clear(void);


/* callbacks */

static void
rig_gui_keypad_enter_cb(GtkWidget * widget, gpointer data)
{
    GrigKeypad *self = data;

    if (self->enabled)
        grig_keypad_disable(self);
    else
        grig_keypad_enable(self);

    g_signal_emit_by_name(self, "grig-keypad-enter-pressed");
}

static void
rig_gui_keypad_clear_cb(GtkWidget * widget, gpointer data)
{
    GrigKeypad *self = data;

    grig_keypad_disable(self);
    g_signal_emit_by_name(self, "grig-keypad-clear-pressed");
}

static void
rig_gui_keypad_num_cb(GtkWidget * widget, gpointer data)
{
    GrigKeypad *self = data;

    gint num = GPOINTER_TO_INT(g_object_get_data
                (G_OBJECT(widget), "number"));

    g_signal_emit_by_name(self, "grig-keypad-num-pressed", num);
}

static gint
rig_gui_keypad_key_press_cb(GtkWidget * widget, GdkEventKey *e, gpointer data)
{
    GrigKeypad *self = data;

    if (e->type != GDK_KEY_PRESS)
        return FALSE;

    if (e->keyval == GDK_Insert) {
        rig_gui_keypad_enter_cb(widget, data);
        return TRUE;
    }

    if (!self->enabled)
        return FALSE;

    if (e->keyval >= '0' && e->keyval <= '9') {
        g_signal_emit_by_name(self, "grig-keypad-num-pressed",
            e->keyval - '0');

        return TRUE;
    }

    /* numeric keypad */
    if (e->keyval >= GDK_KP_0 && e->keyval <= GDK_KP_9) {
        g_signal_emit_by_name(self, "grig-keypad-num-pressed",
                                e->keyval - GDK_KP_0);

        return TRUE;
    }

    if (e->keyval == GDK_Delete) {
        rig_gui_keypad_clear_cb(widget, data);
        return TRUE;
    }

    return FALSE;
}

/* class */

GtkWidget *
grig_keypad_new(void)
{
    return GTK_WIDGET(g_object_new(GRIG_KEYPAD_TYPE, NULL));
}

static void
grig_keypad_init(GrigKeypad * self)
{
    gint i, j;


    self->enabled = FALSE;

    gtk_table_resize(GTK_TABLE(self), 3, 4);
    gtk_table_set_homogeneous(GTK_TABLE(self), TRUE);

    /* create buttons 1 - 9 */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {

            gint index = (3 * i) + j + 1;

            self->buttons[index] =
                rig_gui_keypad_create_button(index, self);

            gtk_table_attach_defaults(GTK_TABLE(self),
                            self->buttons[index], j,
                            j + 1, i, i + 1);

            gtk_widget_set_sensitive(self->buttons[index], FALSE);
        }
    }

    /* create CLR, 0, ENT */
    self->buttons[0] = rig_gui_keypad_create_button(0, self);
    self->clear = rig_gui_keypad_create_clear();
    self->enter = rig_gui_keypad_create_enter();

    gtk_widget_set_sensitive(self->buttons[0], FALSE);
    gtk_widget_set_sensitive(self->clear, FALSE);


    /* attach CLR, 0, ENT */
    gtk_table_attach_defaults(GTK_TABLE(self), self->clear, 0, 1, 3, 4);

    gtk_table_attach_defaults(GTK_TABLE(self),
                    self->buttons[0], 1, 2, 3, 4);

    gtk_table_attach_defaults(GTK_TABLE(self), self->enter, 2, 3, 3, 4);


    /* create new signals */
    /* XXX maybe this should go in class_init? */
    g_signal_new("grig-keypad-enter-pressed", GTK_TYPE_WIDGET,
                G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0, NULL,
                NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    g_signal_new("grig-keypad-clear-pressed", GTK_TYPE_WIDGET,
                G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0, NULL,
                NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    g_signal_new("grig-keypad-num-pressed", GTK_TYPE_WIDGET,
                G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0, NULL,
                NULL, g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, 1,
                G_TYPE_UINT);


    /* connect signal handlers */
    g_signal_connect(G_OBJECT(self->enter), "pressed",
                G_CALLBACK(rig_gui_keypad_enter_cb), self);

    g_signal_connect(G_OBJECT(self->clear), "pressed",
                G_CALLBACK(rig_gui_keypad_clear_cb), self);


    gtk_key_snooper_install(rig_gui_keypad_key_press_cb, self);
}

GType
grig_keypad_get_type(void)
{
    static GType grig_keypad_type = 0;

    if (!grig_keypad_type) {

        static const GTypeInfo grig_keypad_info = {
            sizeof(GrigKeypadClass),
            NULL, NULL, NULL, NULL, NULL,
            sizeof(GrigKeypad),
            0,
            (GInstanceInitFunc) grig_keypad_init,
        };

        grig_keypad_type = g_type_register_static(GTK_TYPE_TABLE,
                                "GrigKeypad",
                                &grig_keypad_info,
                                0);
    }

    return grig_keypad_type;
}

/* exported functions */

/* enables everything but CLR */
void
grig_keypad_enable(GrigKeypad * self)
{
    gint i;

    for (i = 0; i < 10; i++) {
        gtk_widget_set_sensitive(self->buttons[i], TRUE);
    }

    gtk_widget_set_sensitive(self->clear, TRUE);

    self->enabled = TRUE;
}

/* disables everything but ENT */
void
grig_keypad_disable(GrigKeypad * self)
{
    gint i;

    for (i = 0; i < 10; i++) {
        gtk_widget_set_sensitive(self->buttons[i], FALSE);
    }

    gtk_widget_set_sensitive(self->clear, FALSE);
    gtk_widget_set_sensitive(self->enter, TRUE);

    self->enabled = FALSE;
}

/* internal functions */

static GtkWidget *
rig_gui_keypad_create_button(gint num, GrigKeypad *self)
{
    GtkWidget *button;

    button = gtk_button_new_with_label(labels[num]);

    g_object_set_data(G_OBJECT(button), "number", GINT_TO_POINTER(num));

    g_signal_connect(G_OBJECT(button), "pressed",
                G_CALLBACK(rig_gui_keypad_num_cb), self);

    return button;
}

static GtkWidget *
rig_gui_keypad_create_enter(void)
{
    GtkWidget *button;

    button = gtk_button_new_with_label(_("ENT"));
    gtk_widget_set_tooltip_text (button,
                                  _("Begin manual frequency entry mode"));

    return button;
}

static GtkWidget *
rig_gui_keypad_create_clear(void)
{
    GtkWidget *button;

    button = gtk_button_new_with_label(_("CLR"));
    gtk_widget_set_tooltip_text (button,
                                _("Clear manual frequency entry mode"));

    return button;
}
