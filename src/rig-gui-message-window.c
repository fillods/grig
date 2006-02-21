/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2005  Alexandru Csete.

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
#include <stdarg.h>
#include <time.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <hamlib/rig.h>
#include "rig-gui-message-window.h"


/* define message level type for convenience */
typedef enum rig_debug_level_e level_t;

/* columns in the message list */
typedef enum {
	MSG_LIST_COL_TIME = 0,
	MSG_LIST_COL_LEVEL,
	MSG_LIST_COL_MSG,
	MSG_LIST_COL_NUMBER
} msg_list_col_t;


/* data structure to hold one message */
typedef struct {
	time_t    time;     /* time stamp */
	level_t   level;    /* debug level */
	gchar    *message;  /* the message itself */
} message_t;


/* Easy access to column titles */
const gchar *MSG_LIST_COL_TITLE[MSG_LIST_COL_NUMBER] = {
	N_("Time"),
	N_("Level"),
	N_("Message")
};


const gfloat MSG_LIST_COL_TITLE_ALIGN[MSG_LIST_COL_NUMBER] = {
	0.5, 0.5, 0.0
};


const gchar *DEBUG_STR[6] = {
	N_("NONE"),
	N_("BUG"),
	N_("ERROR"),
	N_("WARNING"),
	N_("DEBUG"),
	N_("TRACE")
};


static gboolean visible     = FALSE;   /* Is message window visible? */
static gboolean initialised = FALSE;   /* Is module initialised? */

/* counters */
static guint32  bugs        = 0;       /* Number of bug messages */    
static guint32  errors      = 0;       /* Number of error messages */
static guint32  warnings    = 0;       /* Number of warning messages */
static guint32  verboses    = 0;       /* Number of verbose messages */
static guint32  traces      = 0;       /* Number of trace messages */


/* summary labels; they need to be accessible at runtime */
static GtkWidget *buglabel,*errlabel,*warnlabel,*verblabel,*tracelabel,*sumlabel;


/* The message window itself */
static GtkWidget *window;


/* the tree view model */
GtkTreeModel      *model;


static gint message_window_delete   (GtkWidget *, GdkEvent *, gpointer);
static void message_window_destroy  (GtkWidget *, gpointer);
static void message_window_response (GtkWidget *, gint, gpointer);

/* message list and tree widget functions */
static GtkWidget    *create_message_list    (void);
static GtkTreeModel *create_list_model      (void);
static GtkWidget    *create_message_summary (void);


/* Initialise message window.
 *
 * This function creates the message window and allocates all the internal
 * data structures. The function should be called when the main program
 * is initialised.
 */
void
rig_gui_message_window_init  ()
{
	GtkWidget *hbox;

	if (!initialised) {

		/* do some init stuff */


		hbox = gtk_hbox_new (FALSE, 10);
		gtk_box_pack_start_defaults (GTK_BOX (hbox),
					     create_message_list ());

		gtk_box_pack_start (GTK_BOX (hbox),
				    create_message_summary (),
				    FALSE, TRUE, 0);

		/* create dialog window; we use "fake" stock responses to catch user
		   button clicks (save_as and pause)
		*/
		window = gtk_dialog_new_with_buttons (_("Grig Message Window"),
						      NULL,
						      GTK_DIALOG_DESTROY_WITH_PARENT,
						      GTK_STOCK_MEDIA_PAUSE,
						      GTK_RESPONSE_YES,  /* cheating */
						      GTK_STOCK_SAVE_AS,
						      GTK_RESPONSE_NO,   /* cheating */
						      GTK_STOCK_CLOSE,
						      GTK_RESPONSE_CLOSE,
						      NULL);

		gtk_container_add (GTK_CONTAINER (GTK_DIALOG(window)->vbox), hbox);

		/* connect response signal */
		g_signal_connect (G_OBJECT (window), "response",
				  G_CALLBACK (message_window_response),
				  NULL);

		/* connect delete and destroy signals */
		g_signal_connect (G_OBJECT (window), "delete_event",
				  G_CALLBACK (gtk_widget_hide_on_delete), NULL);    
		g_signal_connect (G_OBJECT (window), "destroy",
				  G_CALLBACK (message_window_destroy), NULL);


		initialised = TRUE;
	}
};


/* Clean up message window.
 *
 * This function cleans up the message window by freeing the allocated
 * memory. It should be called when the main program exits.
 *
 * Note: It is not strictly neccessary to call this function, since it
 *       is also invoked by the 'destroy' callback of the window.
 *
 * FIXME: In the above case this function is not necessary at all.
 */
void
rig_gui_message_window_clean ()
{
	if (initialised) {

		if (visible) {
			rig_gui_message_window_hide ();
		}

		/* do some cleaning stuff */

		initialised = FALSE;
	}
}



void
rig_gui_message_window_show ()
{
	if (!initialised)
		rig_gui_message_window_init ();

//	if (!visible) {
//		g_print ("Show window\n");

		gtk_widget_show_all (window);

		visible = TRUE;
//	}

}


void
rig_gui_message_window_hide  ()
{
	if (visible) {
		gtk_widget_hide_all (window);
		visible = FALSE;
	}
}



/** \brief Debug handler callback
 *  \param debug_level The debug level.
 *  \param user_data Unused.
 *  \param fmt Format string (see printf).
 *  \returns Always RIG_OK.
 *
 *  This function should be passed to hamlib as the debug handler using
 *  the rig_set_debug_callback API function.
 */
int
rig_gui_message_window_add_cb   (enum rig_debug_level_e debug_level,
				 rig_ptr_t user_data,
				 const char *fmt,
				 va_list  ap)
{
	guint        total;     /* totalt number of messages */
	gchar       *str;       /* string to show message count */
	gchar       *msg;       /* formatted debug message */
	gchar      **msgv;      /* debug message line by line */
	guint        numlines;  /* the number of lines in the message */
	guint        i;
	GtkTreeIter  item;      /* new item added to the list store */


	/* create character string and split it in case
	   it is a multi-line message */
	msg = g_strdup_vprintf (fmt, ap);

	/* remove trailing \n */
	g_strchomp (msg);

	/* split the message in case it is a multiline message */
	msgv = g_strsplit_set (msg, "\n", 0);
	numlines = g_strv_length (msgv);

	g_printf ("%d: %s\n", debug_level, msg);
	g_free (msg);

	/* get the time */

	/* for each line in msgv, add the line to the list
	   and update the counters
	*/
	for (i = 0; i < numlines; i++) {

		gtk_list_store_append (GTK_LIST_STORE (model), &item);
		gtk_list_store_set (GTK_LIST_STORE (model), &item,
				    MSG_LIST_COL_TIME, "00:00:00",
				    MSG_LIST_COL_LEVEL, DEBUG_STR[debug_level],
				    MSG_LIST_COL_MSG, msgv[i],
				    -1);


		switch (debug_level) {

			/* internal bugs */
		case RIG_DEBUG_BUG:
			bugs++;
			str = g_strdup_printf ("%d", bugs);
			gtk_label_set_text (GTK_LABEL (buglabel), str);
			g_free (str);
			break;

			/* runtime error */
		case RIG_DEBUG_ERR:
			errors++;
			str = g_strdup_printf ("%d", errors);
			gtk_label_set_text (GTK_LABEL (errlabel), str);
			g_free (str);
			break;

			/* warning */
		case RIG_DEBUG_WARN:
			warnings++;
			str = g_strdup_printf ("%d", warnings);
			gtk_label_set_text (GTK_LABEL (warnlabel), str);
			g_free (str);
			break;

			/* verbose info */
		case RIG_DEBUG_VERBOSE:
			verboses++;
			str = g_strdup_printf ("%d", verboses);
			gtk_label_set_text (GTK_LABEL (verblabel), str);
			g_free (str);
			break;

			/* trace */
		case RIG_DEBUG_TRACE:
			traces++;
			str = g_strdup_printf ("%d", traces);
			gtk_label_set_text (GTK_LABEL (tracelabel), str);
			g_free (str);
			break;

		default:
			break;
		}

	}

	/* the sum does not have to be updated for each line */
	total = bugs+errors+warnings+verboses+traces;
	str = g_strdup_printf ("<b>%d</b>", total);
	gtk_label_set_markup (GTK_LABEL (sumlabel), str);
	g_free (str);

	g_strfreev (msgv);
	
	return RIG_OK;
}




/*** FIXME: does not seem to be necessary */
static gint
message_window_delete      (GtkWidget *widget,
			    GdkEvent  *event,
			    gpointer   data)
{

	gtk_widget_hide_all (widget);
	visible = FALSE;

	/* return TRUE to indicate that message window
	   should not be destroyed */
	return TRUE;
}


/* callback function called when the dialog window is destroyed */
static void
message_window_destroy    (GtkWidget *widget,
			   gpointer   data)
{
	/* clean up memory */
	/* GSList, ... */

	visible = FALSE;
	initialised = FALSE;
}


/* callback function called when a dialog button is clicked */
static void
message_window_response (GtkWidget *widget,
			 gint       response,
			 gpointer   data)
{
	switch (response) {

		/* close button */
	case GTK_RESPONSE_CLOSE:
		gtk_widget_hide_all (widget);
		visible = FALSE;
		break;

		/* PAUSE button */
	case GTK_RESPONSE_YES:
		g_print ("PAUSE\n");
		break;

		/* SAVE AS button */
	case GTK_RESPONSE_NO:
		g_print ("SAVE AS\n");
		break;

	default:
		break;
	}
}


/* Create list view */
static GtkWidget *
create_message_list    ()
{
	/* high level treev iew widget */
	GtkWidget *treeview;

	/* scrolled window containing the tree view */
	GtkWidget *swin;

	/* cell renderer used to create a column */
	GtkCellRenderer   *renderer;

	/* place holder for a tree view column */
	GtkTreeViewColumn *column;


	guint i;



	treeview = gtk_tree_view_new ();

	for (i = 0; i < MSG_LIST_COL_NUMBER; i++) {

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (MSG_LIST_COL_TITLE[i],
								   renderer,
								   "text", i,
								   NULL);
		gtk_tree_view_insert_column (GTK_TREE_VIEW (treeview),
					     column,
					     -1);

		/* only aligns the headers? */
		gtk_tree_view_column_set_alignment (column, MSG_LIST_COL_TITLE_ALIGN[i]);

	}

	/* create tree view model and finalise tree view */
	model = create_list_model ();
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), model);
	g_object_unref (model);


	/* treeview is packed into a scroleld window */
	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (swin), treeview);

	return swin;
}


/* create tree view model; we actually create a GtkListStore because we are
   only interested in a flat list. A GtkListStore can be cast to a GtkTreeModel
   without any problems.
*/
static GtkTreeModel *
create_list_model ()
{
	GtkListStore *liststore;

	liststore = gtk_list_store_new (MSG_LIST_COL_NUMBER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING);

	/*** Fill existing data into the list here ***/

	return GTK_TREE_MODEL (liststore);
}



/* create summary */
static GtkWidget *
create_message_summary ()
{
	GtkWidget *table;   /* table containing everything */
	GtkWidget *frame;   /* surrounding frame */
	GtkWidget *label;   /* dummy label */

	/* create labels */
	buglabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (buglabel), 1.0, 0.5);

	errlabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (errlabel), 1.0, 0.5);

	warnlabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (warnlabel), 1.0, 0.5);

	verblabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (verblabel), 1.0, 0.5);

	tracelabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (tracelabel), 1.0, 0.5);

	sumlabel = gtk_label_new (NULL);
	gtk_label_set_use_markup (GTK_LABEL (sumlabel), TRUE);
	gtk_label_set_markup (GTK_LABEL (sumlabel), "<b>0</b>");
	gtk_misc_set_alignment (GTK_MISC (sumlabel), 1.0, 0.5);

	/* create table and add widgets */
	table = gtk_table_new (7, 2, TRUE);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_container_set_border_width (GTK_CONTAINER (table), 10);

	label = gtk_label_new (_("Bugs"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 0, 1);

	label = gtk_label_new (_("Errors"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 1, 2);

	label = gtk_label_new (_("Warning"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 2, 3);

	label = gtk_label_new (_("Verbose"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 3, 4);

	label = gtk_label_new (_("Trace"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 4, 5);

	gtk_table_attach_defaults (GTK_TABLE (table),
				   gtk_hseparator_new (),
				   0, 2, 5, 6);

	label = gtk_label_new (NULL);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>Total</b>"));
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 6, 7);

	gtk_table_attach_defaults (GTK_TABLE (table), buglabel,
				   1, 2, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), errlabel,
				   1, 2, 1, 2);
	gtk_table_attach_defaults (GTK_TABLE (table), warnlabel,
				   1, 2, 2, 3);
	gtk_table_attach_defaults (GTK_TABLE (table), verblabel,
				   1, 2, 3, 4);
	gtk_table_attach_defaults (GTK_TABLE (table), tracelabel,
				   1, 2, 4, 5);
	gtk_table_attach_defaults (GTK_TABLE (table), sumlabel,
				   1, 2, 6, 7);

	/* frame around the table */
	frame = gtk_frame_new (_(" Summary "));
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_container_add (GTK_CONTAINER (frame), table);

	return frame;
}

