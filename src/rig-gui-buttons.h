
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
#ifndef RIG_GUI_BUTTONS_H
#define RIG_GUI_BUTTONS_H 1


/** \brief Minimum delay in msec between widget updates. */
#define RIG_GUI_BUTTONS_MIN_TVAL 1100

/** \brief Default delay in msec between widget updates. */
#define RIG_GUI_BUTTONS_DEF_TVAL 1367

/** \brief Maximum delay in msec between widget updates. */
#define RIG_GUI_BUTTONS_MAX_TVAL 10000


GtkWidget *rig_gui_buttons_create (void);

#endif
