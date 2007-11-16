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
#ifndef RIG_DAEMON_CHECK_H
#define RIG_DAEMON_CHECK_H 1

#include "rig-data.h"

void rig_daemon_check_pwrstat (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_ptt     (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_vfo     (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_freq    (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_rit     (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_xit     (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_mode    (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_level   (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);
void rig_daemon_check_func    (RIG *, grig_settings_t *, grig_cmd_avail_t *, grig_cmd_avail_t *);

#endif
