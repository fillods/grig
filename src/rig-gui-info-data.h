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
/** \file rig-gui-info-data.h
 *  \ingroup info
 *  \brief Constant data structures for rig info.
 *
 */

#ifndef RIG_GUI_INFO_DATA_H
#define RIG_GUI_INFO_DATA_H 1


/** \brief string representations for RIG_LEVEL symbolic references. */
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
	N_("N/A"),
	N_("N/A"),
	N_("N/A"),
	N_("RAWSTR"),
	N_("SQLSTAT"),
	N_("SWR"),
	N_("ALC"),
	N_("STRENGTH")
};

/** \brief string representations for RIG_FUNC symbolic references. */
const gchar *RIG_FUNC_STR[] = {
	N_("FAST AGC"),
	N_("NB"),
	N_("COMPR"),
	N_("VOX"),
	N_("TONE"),
	N_("CTCSS"),
	N_("SEMI BK"),
	N_("FULL BK"),
	N_("ANF"),
	N_("NR"),
	N_("AIP"),
	N_("APF"),
	N_("MON"),
	N_("MAN NOTCH"),
	N_("RNF"),
	N_("AUTO RO"),
	N_("LOCK"),
	N_("MUTE"),
	N_("VOICE SCAN"),
	N_("REV TRX"),
	N_("SQL"),
	N_("ABM"),
	N_("BEAT CANC"),
	N_("MAN BC"),
	N_("N/A"),
	N_("AFC"),
	N_("SATMODE"),
	N_("SCOPE"),
	N_("RESUME"),
	N_("TBURST"),
	N_("TUNER")
};

/** \brief String representations for PTT_TYPE_T symbolic references. */
const gchar *PTT_TYPE_STR[] = {
	N_("None"),
	N_("Legacy"),
	N_("SER_DTR"),
	N_("SER_RTS"),
	N_("PARPORT")
};

/** \brief String representations for DCD_TYPE_T symbolic references. */
const gchar *DCD_TYPE_STR[] = {
	N_("None"),
	N_("Legacy"),
	N_("SER_DSR"),
	N_("SER_CTS"),
	N_("SER_CAR"),
	N_("PARPORT")
};


/** \brief String representations for RIG_PORT_T symbolic references. */
const gchar *RIG_PORT_STR[] = {
	N_("None"),
	N_("Serial"),
	N_("Network"),
	N_("Device"),
	N_("Packet"),
	N_("DTMF"),
	N_("IrDA"),
	N_("RPC"),
	N_("Parallel")
};


/** \brief String representations for SERIAL_PARITY_E symbolic references. */
const gchar *RIG_PARITY_STR[] = {
	N_("None"),
	N_("Odd"),
	N_("Even")
};

/** \brief String representations for SERIAL_HANDSHAKE_E symbolic references. */
const gchar *RIG_HANDSHAKE_STR[] = {
	N_("None"),
	N_("XONXOFF"),
	N_("Hardware")
};

/** \brief String representations for announce symbolic references. */
const gchar *ANN_STR[] = {
	N_("OFF"),
	N_("FREQ"),
	N_("RXMODE"),
	N_("CW"),
	N_("EMG"),
	N_("JAP")
};

/** \brief String representation for mode symbolic references. */
const gchar *MODE_STR[] = {
	N_("AM"),
	N_("CW"),
	N_("USB"),
	N_("LSB"),
	N_("RTTY"),
	N_("FM"),
	N_("WFM"),
	N_("CWR"),
	N_("RTTYR"),
	N_("AMS"),
	N_("PKTLSB"),
	N_("PKTUSB"),
	N_("PKTFM"),
	N_("ECSSUSB"),
	N_("ECSSLSB"),
	N_("FAX")
};


/** \brief String representation for VFO ops. */
const gchar *RIG_OP_STR[] = {
	N_("COPY A=B"),
	N_("XCHG A/B"),
	N_("VFO->MEM"),
	N_("MEM->VFO"),
	N_("MEMCLEAR"),
	N_("UP"),
	N_("DOWN"),
	N_("BAND UP"),
	N_("BAND DOWN"),
	N_("LEFT"),
	N_("RIGHT"),
	N_("TUNE"),
	N_("TOGGLE")
};

#endif
