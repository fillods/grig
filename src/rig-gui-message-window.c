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
#include <stdarg.h>
#include <time.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <hamlib/rig.h>
#include "grig-debug.h"
#include "rig-gui-message-window.h"


/* define message level type for convenience */
typedef enum rig_debug_level_e level_t;

/* columns in the message list */
typedef enum {
	MSG_LIST_COL_TIME = 0,
	MSG_LIST_COL_SOURCE,
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
	N_("Source"),
	N_("Level"),
	N_("Message")
};


const gfloat MSG_LIST_COL_TITLE_ALIGN[MSG_LIST_COL_NUMBER] = {
	0.5, 0.0, 0.5, 0.0
};


const gchar *DEBUG_STR[6] = {
	N_("NONE"),
	N_("BUG"),
	N_("ERROR"),
	N_("WARNING"),
	N_("DEBUG"),
	N_("TRACE")
};


extern GtkWidget    *grigapp;

static gboolean visible     = FALSE;   /* Is message window visible? */
static gboolean initialised = FALSE;   /* Is module initialised? */

/* counters */
static guint32  bugs        = 0;       /* Number of bug messages */    
static guint32  errors      = 0;       /* Number of error messages */
static guint32  warnings    = 0;       /* Number of warning messages */
static guint32  verboses    = 0;       /* Number of verbose messages */
static guint32  traces      = 0;       /* Number of trace messages */
static guint32  hamlibs     = 0;       /* Number of messages from hamlib */
static guint32  grigs       = 0;       /* Number of messages from grig */
static guint32  others      = 0;       /* Number of messages from other sources */

/* summary labels; they need to be accessible at runtime */
static GtkWidget *buglabel,*errlabel,*warnlabel,*verblabel,*tracelabel,*sumlabel;
static GtkWidget *hamliblabel, *griglabel, *otherlabel;

/* The message window itself */
static GtkWidget *window;


/* the tree view model */
GtkTreeModel      *model;


static void message_window_destroy  (GtkWidget *, gpointer);
static void message_window_response (GtkWidget *, gint, gpointer);

/* message list and tree widget functions */
static GtkWidget    *create_message_list    (void);
static GtkTreeModel *create_list_model      (void);
static GtkWidget    *create_message_summary (void);

/* load debug file related */
static void load_debug_file    (void);
static int  read_debug_file    (const gchar *filename);
static void clear_message_list (void);

static void add_debug_message (const gchar *datetime,
			       const gchar *source,
			       enum rig_debug_level_e debug_level,
			       const char *message);

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
		gtk_box_pack_start (GTK_BOX (hbox),
					     create_message_list (),
					     TRUE,
					     TRUE,
					     0);

		gtk_box_pack_start (GTK_BOX (hbox),
				    create_message_summary (),
				    FALSE, TRUE, 0);

		/* create dialog window; we use "fake" stock responses to catch user
		   button clicks (save_as and pause)
		*/
		window = gtk_dialog_new_with_buttons (_("Grig Message Window"),
						      NULL,
						      GTK_DIALOG_DESTROY_WITH_PARENT,
						      GTK_STOCK_OPEN,
						      GTK_RESPONSE_YES,  /* cheating */
						      GTK_STOCK_CLEAR,
						      GTK_RESPONSE_NO,   /* cheating */
						      GTK_STOCK_CLOSE,
						      GTK_RESPONSE_CLOSE,
						      NULL);
		
		gtk_window_set_default_size (GTK_WINDOW (window), 700, 300);

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
 * Note: It is not strictly necessary to call this function, since it
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



/** \brief Add a message to message list */
static void
add_debug_message (const gchar *datetime,
		   const gchar *source,
		   enum rig_debug_level_e debug_level,
		   const char *message)
{
	guint        total;     /* totalt number of messages */
	gchar       *str;       /* string to show message count */
	GtkTreeIter  item;      /* new item added to the list store */


	gtk_list_store_append (GTK_LIST_STORE (model), &item);
	gtk_list_store_set (GTK_LIST_STORE (model), &item,
			    MSG_LIST_COL_TIME, datetime,
			    MSG_LIST_COL_SOURCE, source,
			    MSG_LIST_COL_LEVEL, _(DEBUG_STR[debug_level]),
			    MSG_LIST_COL_MSG, message,
			    -1);

	/* increment source counter */
	if (!g_ascii_strcasecmp (source, "HAMLIB")) {
		hamlibs++;
		str = g_strdup_printf ("%d", hamlibs);
		gtk_label_set_text (GTK_LABEL (hamliblabel), str);
		g_free (str);
	}
	else if (!g_ascii_strcasecmp (source, "GRIG")) {
		grigs++;
		str = g_strdup_printf ("%d", grigs);
		gtk_label_set_text (GTK_LABEL (griglabel), str);
		g_free (str);
	}
	else {
		others++;
		str = g_strdup_printf ("%d", others);
		gtk_label_set_text (GTK_LABEL (otherlabel), str);
		g_free (str);
	}


	/* increment severity counter */
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

	/* the sum does not have to be updated for each line */
	total = bugs+errors+warnings+verboses+traces;
	str = g_strdup_printf ("<b>%d</b>", total);
	gtk_label_set_markup (GTK_LABEL (sumlabel), str);
	g_free (str);

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

		/* OPEN button */
	case GTK_RESPONSE_YES:
		load_debug_file ();
		break;

		/* CLEAR button */
	case GTK_RESPONSE_NO:
		clear_message_list ();
		break;

	default:
		break;
	}
}


/** \brief Load debug file.
 *
 * This function creates the file chooser dialog, which can be used to select
 * a file containing debug messages. When the dialog returns, the selected
 * file is checked and, if the file exists, is read line by line.
 */
static void
load_debug_file ()
{

	gchar *filename;


	GtkWidget *dialog;

	/* create file chooser dialog */
	dialog = gtk_file_chooser_dialog_new (_("Open Debug File"),
					      GTK_WINDOW (grigapp),
					      GTK_FILE_CHOOSER_ACTION_OPEN,
					      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {

		clear_message_list ();

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

		/* sanity check of filename will be performed 
		   in read_debug_file */
		read_debug_file (filename);

		g_free (filename);
	}

	gtk_widget_destroy (dialog);

}


/** \brief Read contents of debug file. */
static int
read_debug_file (const gchar *filename)
{
	GIOChannel  *logfile = NULL;   /* the log file */
	GError      *error = NULL;     /* error structure */
	gint         errorcode = 0;    /* error code returned by function */
	gchar       *line;             /* line read from file */
	gsize        length;           /* length of line read from file */
	gchar      **buff;


	/* check file and read contents */
	if (g_file_test (filename, G_FILE_TEST_EXISTS)) {

		/* open file */
		logfile = g_io_channel_new_file (filename, "r", &error);

		if (logfile) {
			/* read the file line by line */
			while (g_io_channel_read_line (logfile,
						       &line,
						       &length,
						       NULL,
						       NULL) != G_IO_STATUS_EOF) {

				/* trim line and split it */
				line = g_strdelimit (line, "\n", '\0');

				buff = g_strsplit (line,
						   GRIG_DEBUG_SEPARATOR,
						   MSG_LIST_COL_NUMBER);

				/* buff[0] = date and time
				   buff[1] = source
				   buff[2] = level
				   buff[3] = message
				   unless it comes from Gtk+/Glib, in which case
				   buff[0] contains the message and it is the
				   only element
				*/
				switch (g_strv_length (buff)) {

				case 1:
					add_debug_message ("", _("SYS"), RIG_DEBUG_ERR, buff[0]);
					break;

				case 4:
					add_debug_message (buff[0], buff[1],
							   (guint) g_ascii_strtod (buff[2], NULL),
							   buff[3]);
					break;
				default:
					add_debug_message ("", _("GRIG"), RIG_DEBUG_ERR,
							   _("Log file seems corrupt"));
					break;
				}

				/* clean up */
				g_free (line);
				g_strfreev (buff);
			}

			errorcode = 0;

			/* Close IO channel; don't care about status.
			   Shutdown will flush the stream and close the channel
			   as soon as the reference count is dropped. Order matters!
			*/
			g_io_channel_shutdown (logfile, TRUE, NULL);
			g_io_channel_unref (logfile);

		}
		else {
			/* an error occurred */

			grig_debug_local (RIG_DEBUG_ERR,
					  _("%s:%d: Error open debug log (%s)"),
					  __FILE__, __LINE__, error->message);

			g_clear_error (&error);

			errorcode = 1;
		}
	}
	else {
		errorcode = 1;
	}

	return errorcode;
}


/** \brief Clear the message list
 *
 * Besides clearing the message list, the function also resets
 * the counters and set the text of the corresponding widgets
 * to zero.
 */
static void
clear_message_list ()
{
	/* clear the meaase list */
	gtk_list_store_clear (GTK_LIST_STORE (model));

	/* reset the counters and text widgets */
	bugs = 0;
	errors = 0;
	warnings = 0;
	verboses = 0;
	traces = 0;
	grigs = 0;
	hamlibs = 0;
	others = 0;

	gtk_label_set_text (GTK_LABEL (buglabel), "0");
	gtk_label_set_text (GTK_LABEL (errlabel), "0");
	gtk_label_set_text (GTK_LABEL (warnlabel), "0");
	gtk_label_set_text (GTK_LABEL (verblabel), "0");
	gtk_label_set_text (GTK_LABEL (tracelabel), "0");
	gtk_label_set_text (GTK_LABEL (hamliblabel), "0");
	gtk_label_set_text (GTK_LABEL (griglabel), "0");
	gtk_label_set_text (GTK_LABEL (otherlabel), "0");
	gtk_label_set_markup (GTK_LABEL (sumlabel), "<b>0</b>");
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
		column = gtk_tree_view_column_new_with_attributes (_(MSG_LIST_COL_TITLE[i]),
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
	hamliblabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (hamliblabel), 1.0, 0.5);

	griglabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (griglabel), 1.0, 0.5);

	otherlabel = gtk_label_new ("0");
	gtk_misc_set_alignment (GTK_MISC (otherlabel), 1.0, 0.5);


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
	table = gtk_table_new (10, 2, TRUE);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_container_set_border_width (GTK_CONTAINER (table), 10);

	label = gtk_label_new (_("Hamlib"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 0, 1);

	label = gtk_label_new (_("Grig"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 1, 2);

	label = gtk_label_new (_("Other"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 2, 3);

	gtk_table_attach_defaults (GTK_TABLE (table),
				   gtk_hseparator_new (),
				   0, 2, 3, 4);

	label = gtk_label_new (_("Bugs"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 4, 5);

	label = gtk_label_new (_("Errors"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 5, 6);

	label = gtk_label_new (_("Warning"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 6, 7);

	label = gtk_label_new (_("Verbose"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 7, 8);

	label = gtk_label_new (_("Trace"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 8, 9);

	gtk_table_attach_defaults (GTK_TABLE (table),
				   gtk_hseparator_new (),
				   0, 2, 9, 10);

	label = gtk_label_new (NULL);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
	gtk_label_set_markup (GTK_LABEL (label), _("<b>Total</b>"));
	gtk_table_attach_defaults (GTK_TABLE (table),
				   label,
				   0, 1, 10, 11);

	gtk_table_attach_defaults (GTK_TABLE (table), hamliblabel,
				   1, 2, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), griglabel,
				   1, 2, 1, 2);
	gtk_table_attach_defaults (GTK_TABLE (table), otherlabel,
				   1, 2, 2, 3);
	gtk_table_attach_defaults (GTK_TABLE (table), buglabel,
				   1, 2, 4, 5);
	gtk_table_attach_defaults (GTK_TABLE (table), errlabel,
				   1, 2, 5, 6);
	gtk_table_attach_defaults (GTK_TABLE (table), warnlabel,
				   1, 2, 6, 7);
	gtk_table_attach_defaults (GTK_TABLE (table), verblabel,
				   1, 2, 7, 8);
	gtk_table_attach_defaults (GTK_TABLE (table), tracelabel,
				   1, 2, 8, 9);
	gtk_table_attach_defaults (GTK_TABLE (table), sumlabel,
				   1, 2, 10, 11);

	/* frame around the table */
	frame = gtk_frame_new (_(" Summary "));
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_container_add (GTK_CONTAINER (frame), table);

	return frame;
}

