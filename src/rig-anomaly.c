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

/** \file    rig-anomaly.c
 *  \ingroup rigan
 *  \brief   Rig anomaly manager.
 *
 * This object manages the anomalies and errors that occur during communication
 * with the radio. The rig-daemon process raises a specific anomaly every time
 * the execution of a command isn't successful. This object will then record the
 * anomaly and - in case the same anomaly has occured repeatedly - disable the
 * problematic command. Therefor, this object needs access to the rig-data object.
 * Furthermore, in order to know about the various rig commands, this object needs
 * access to the rig-daemon data types as well.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-daemon.h"
#include "rig-anomaly.h"



/** \brief Table defining the command disable threshold.
 *
 * If an anomaly has been raised a specific number of times, the
 * corresponding command has to be disabled. This table lists
 * the allowed number of erroneous executions within 10 seconds
 * for each command
 */ 
static const anomaly_table_t CMD_DISABLE_THLD = {
	0,      /*  RIG_CMD_NONE          */
	0,      /*  RIG_CMD_GET_FREQ_1    */
	0,      /*  RIG_CMD_SET_FREQ_1    */
	0,      /*  RIG_CMD_GET_FREQ_2    */
	0,      /*  RIG_CMD_SET_FREQ_2    */
 	0,      /*  RIG_CMD_GET_RIT       */
	0,      /*  RIG_CMD_SET_RIT       */
	0,      /*  RIG_CMD_GET_XIT       */
	0,      /*  RIG_CMD_SET_XIT       */
	0,      /*  RIG_CMD_GET_VFO       */
	0,      /*  RIG_CMD_SET_VFO       */
	0,      /*  RIG_CMD_GET_PSTAT     */
	0,      /*  RIG_CMD_SET_PSTAT     */
	0,      /*  RIG_CMD_GET_PTT       */
	0,      /*  RIG_CMD_SET_PTT       */
	0,      /*  RIG_CMD_GET_MODE      */
	0,      /*  RIG_CMD_SET_MODE      */
	0,      /*  RIG_CMD_GET_STRENGTH  */
	0       /*  RIG_CMD_GET_PWR       */
};


/** \brief The anomaly occurence table.
 */
static anomaly_table_t ANOMALY_TABLE;



/** \brief Raise an anomaly.
 *  \param cmd The command which is the source of the anomaly.
 *
 * This function increments the anomaly counter of the specified
 * command. Firthermore, if the command disabling threshold is exceeded,
 * it disables the command.
 *
 */
void
rig_anomaly_raise (rig_cmd_t cmd)
{

	/* test whether number of anomalies exceed the threshold */
	if (++ANOMALY_TABLE[cmd] >= CMD_DISABLE_THLD[cmd]) {

	}

}
