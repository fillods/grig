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
#ifndef GRIG_DEBUG_H
#define GRIG_DEBUG_H 1

#include <glib.h>
#include <hamlib/rig.h>


#define GRIG_DEBUG_SEPARATOR ";;"

/** \brief Debug message sources. */
typedef enum {
	MSG_SRC_NONE   = 0,     /*!< No source, unknown source. */
	MSG_SRC_HAMLIB = 1,     /*!< Debug message comes from hamlib */
	MSG_SRC_GRIG   = 2      /*!< Debug message comes from grig itself */
} debug_msg_src_t;



void grig_debug_init           (gchar *filename);
void grig_debug_close          (void);
int  grig_debug_hamlib_cb      (enum rig_debug_level_e debug_level,
                                rig_ptr_t user_data,
                                const char *fmt,
                                va_list ap);

int  grig_debug_local      (enum rig_debug_level_e debug_level,
                            const char *fmt,
                            ...);

gchar *grig_debug_get_log_file (void);


void grig_debug_set_level (enum rig_debug_level_e level);
int  grig_debug_get_level (void);

#endif
