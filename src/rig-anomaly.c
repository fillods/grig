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

/** \file    rig-anomaly.c
 *  \ingroup rigan
 *  \brief   Rig anomaly manager.
 *
 * This object manages the anomalies and errors that occur during communication
 * with the radio. The rig-daemon process raises a specific anomaly every time
 * the execution of a command does not succeed. The anomaly manager will then record the
 * anomaly and, if the same anomaly has occured repeatedly within a certain time period,
 * disable the erroneous command. Therefore, this object needs access to the rig-data API.
 * Furthermore, in order to know about the various rig commands, this object needs
 * access to the rig-daemon data types as well.
 *
 * \bug File includes gtk.h but not really needed?
 */
#include <gtk/gtk.h>
#include <hamlib/rig.h>
#include "rig-data.h"
#include "rig-daemon.h"
#include "rig-anomaly.h"



/** \brief Table defining the command disable threshold.
 *
 * If an anomaly has been raised a specific number of times
 * within a certain period, the
 * corresponding command is disabled. This table lists
 * the allowed number of erroneous executions within the time
 * specified in the ANOMALY_COUNT_PERIOD array.
 */ 
static const anomaly_count_t ANOMALY_COUNT_MAX = {
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


/** \brief Anomaly count periods.
 *
 * This table defines the periods in seconds during which
 * anomalies are accumulated.
 */
static const anomaly_period_t ANOMALY_COUNT_PERIOD = {
	0.00,      /*  RIG_CMD_NONE          */
	2.00,      /*  RIG_CMD_GET_FREQ_1    */
	2.00,      /*  RIG_CMD_SET_FREQ_1    */
	2.00,      /*  RIG_CMD_GET_FREQ_2    */
	2.00,      /*  RIG_CMD_SET_FREQ_2    */
 	5.00,      /*  RIG_CMD_GET_RIT       */
	5.00,      /*  RIG_CMD_SET_RIT       */
	5.00,      /*  RIG_CMD_GET_XIT       */
	5.00,      /*  RIG_CMD_SET_XIT       */
	10.0,      /*  RIG_CMD_GET_VFO       */
	10.0,      /*  RIG_CMD_SET_VFO       */
	10.0,      /*  RIG_CMD_GET_PSTAT     */
	10.0,      /*  RIG_CMD_SET_PSTAT     */
	10.0,      /*  RIG_CMD_GET_PTT       */
	10.0,      /*  RIG_CMD_SET_PTT       */
	10.0,      /*  RIG_CMD_GET_MODE      */
	10.0,      /*  RIG_CMD_SET_MODE      */
	10.0,      /*  RIG_CMD_GET_STRENGTH  */
	10.0       /*  RIG_CMD_GET_PWR       */
};


/** \brief The anomaly occurence table.
 *
 * This table holds the accumulated number of anomalies
 * that have occured within a certain time period.
 */
static anomaly_count_t ANOMALY_COUNT = {
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



/** \brief First occurrence of an anomaly.
 *
 * This table holds the time of the first occurence of a given anomaly.
 */
static anomaly_time_t FIRST_ANOMALY = {
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




/** \brief Raise an anomaly.
 *  \param cmd The command which is the source of the anomaly.
 *
 * This function increments the anomaly counter of the specified
 * command. Furthermore, if the command disabling threshold is exceeded
 * within a specific time period, it disables the command.
 *
 * The anomaly counter is reset if a command is disabled or if the
 * time elapsed since the first anomaly has exceeded the given
 * count period.
 *
 */
void
rig_anomaly_raise (rig_cmd_t cmd)
{

	/* check whether it is the first occurence */
	if ((ANOMALY_COUNT[cmd] == 0) || (FIRST_ANOMALY[cmd] == 0)) {

		/* first occurrence */

		/* store the time */

	}
	else {

		/* check whether the time elapsed since the
		   first anomaly of this kind is longer than
		   the accumulating period.
		*/
		if (TRUE) {

			/* elapsed time is longer; reset the counter
			   to 1 and store the new time.
			*/

		}

	}

	/* test whether number of anomalies exceeds the threshold */
	if (++ANOMALY_COUNT[cmd] >= ANOMALY_COUNT_MAX[cmd]) {

		/* disable the command */

		/* reset the time and the counter */

	}

}
