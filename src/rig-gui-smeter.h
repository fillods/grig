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
#ifndef RIG_GUI_SMETER_H
#define RIG_GUI_SMETER_H 1


/** \brief Minimum delay in msec between s-meter updates (50 fps) */
#define RIG_GUI_SMETER_MIN_TVAL 20

/** \brief Default delay in msec between s-meter updates (25 fps). */
#define RIG_GUI_SMETER_DEF_TVAL 40

/** \brief Maximum delay in msec between s-meter updates (5 fps). */
#define RIG_GUI_SMETER_MAX_TVAL 500

/* FIXME: Add RIG_GUI_SMETER_FALLOFF_XXX values */


GtkWidget *rig_gui_smeter_create (void);


#endif


