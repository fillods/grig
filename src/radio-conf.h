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
#ifndef RADIO_CONF_H
#define RADIO_CONF_H 1

#include <glib.h>



/** \brief Possibilities for using DTR and CTS lines. */
typedef enum {
    LINE_OFF = 0,   /*!< Turn line permanently OFF */
    LINE_ON,        /*!< Turn line permanently ON */
    LINE_PTT,       /*!< Use line for set PTT */
    LINE_CW         /*!< Use line for sending CW */
} ctrl_stat_t;


/** \brief Radio configuration structure. */
typedef struct {
    gchar      *name;      /*!< Configuration name */
    gchar      *company;   /*!< Rig manufacturer */
    gchar      *model;     /*!< Rig model */
    guint       id;        /*!< Hamlib ID of rig */
    gchar      *port;      /*!< Port rig is attached to */
    guint       speed;     /*!< Serial speed */
    guint       civ;       /*!< CI-V address for icoms */
    ctrl_stat_t dtr;       /*!< DTR line usage */
    ctrl_stat_t rts;       /*!< RTS line usage */
    gboolean    ptt;       /*!< Set/get PTT via CAT */
    gboolean    pow;       /*!< Set/get power on/off via CAT */
    guint       version;   /*!< Configuration version, see grig-config.h */
} radio_conf_t;


gboolean radio_conf_read (radio_conf_t *conf);
void radio_conf_save (radio_conf_t *conf);

#endif
