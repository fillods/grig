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
#ifndef GRIG_CONFIG_H
#define GRIG_CONFIG_H 1


/** \brief Main configuration directory. */
#define GRIG_CONFIG_DIR "/apps/grig"


/** \brief Radio config directory. */
#define GRIG_CONFIG_RIG_DIR GRIG_CONFIG_DIR "/radios"


/** \brief Rotator config directory. */
#define GRIG_CONFIG_ROT_DIR GRIG_CONFIG_DIR "/rotators"


/** \brief Hamlib configuration directory. */
#define GRIG_CONFIG_HAMLIB_DIR GRIG_CONFIG_DIR "/hamlib"


/** \brief Locations configuration directory. */
#define GRIG_CONFIG_LOC_DIR GRIG_CONFIG_DIR "/locations"


/** \brief GConf key for default radio. */
#define GRIG_CONFIG_RIG_DEF_KEY GRIG_CONFIG_RIG_DIR "/default"


/** \brief GConf key for number of radios. */
#define GRIG_CONFIG_RIG_NUM_KEY GRIG_CONFIG_RIG_DIR "/number"


/** \brief GConf key for default rotator. */
#define GRIG_CONFIG_ROT_DEF_KEY GRIG_CONFIG_ROT_DIR "/default"


/** \brief GConf key for number of rotators. */
#define GRIG_CONFIG_ROT_NUM_KEY GRIG_CONFIG_ROT_DIR "/number"


/** \brief GConf dir for hamlib settings. */
#define GRIG_CONFIG_HAMLIB_DIR GRIG_CONFIG_DIR "/hamlib"

/** \brief Gconf key for hamlib debug level. */
#define GRIG_CONFIG_DEBUG_KEY GRIG_CONFIG_HAMLIB_DIR "/debug" 

#endif
