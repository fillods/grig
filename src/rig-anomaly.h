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

#ifndef RIG_ANOMALY_H
#define RIG_ANOMALY_H 1


#include <time.h>
#include "rig-daemon.h"


/** \brief Type used to hold system times vs. rig command. */
typedef time_t anomaly_time_t[RIG_CMD_NUMBER];

/** \brief Type used to hold periods in seconds vs. rig command. */
typedef gfloat anomaly_period_t[RIG_CMD_NUMBER];

/** \brief Type used to hold anomaly occurrences vs. rig command. */
typedef guint8 anomaly_count_t[RIG_CMD_NUMBER];

void rig_anomaly_raise (rig_cmd_t);


#endif
