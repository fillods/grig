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
/** \file rig-gui-info-data.h
 *  \ingroup info
 *  \brief Constant data structures for rig info.
 *
 */

#ifndef RIG_GUI_INFO_DATA_H
#define RIG_GUI_INFO_DATA_H 1


/** \brief string representations for level symbolic references. */
const gchar *RIG_LEVEL_STR[] = {
	N_("PREAMP"),
	N_("ATT"),
	N_("VOX"),
	N_("AF"),
	N_("RF"),
	N_("SQL"),
	N_("IF"),
	N_("APF"),
	N_("NR"),
	N_("PBT_IN"),
	N_("PBT_OUT"),
	N_("CWPITCH"),
	N_("RFPOWER"),
	N_("MICGAIN"),
	N_("KEYSPD"),
	N_("NOTCHF"),
	N_("COMP"),
	N_("AGC"),
	N_("BKINDL"),
	N_("BALANCE"),
	N_("METER"),
	N_("VOXGAIN"),
	N_("ANTIVOX"),
	"",
	"",
	"",
	N_("RAWSTR"),
	N_("SQLSTAT"),
	N_("SWR"),
	N_("ALC"),
	N_("STRENGTH")
};


#endif
