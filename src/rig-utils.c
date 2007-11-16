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
#include <gtk/gtk.h>
#include "rig-utils.h"



/** \brief Convert hamlib mode to index.
 *  \param mode The hamlib mode (1,2,4,8,..)
 *  \return The converted index value.
 *  
 * This function converts the hamlib mode (1,2,4,8,...) to an array
 * index (0,1,2,3,...). It is done using the formula
 *
 *    index = rint[log(mode)/log(2)]
 *
 * \note Hamlib uses 0 for RIG_MODE_NONE which clearly has to be treated as
 *       illegal input. In case of zero input, however, this function will
 *       simply return 0 as if the input was 1. The same protection is done
 *       for negative input values.
 *
 * \bug It would probably be much better if we used rmode_t for input param type.
 */
guint
rig_utils_mode_to_index (gint mode)
{
	guint index;

	/* if input is numerically sane, compute the index */
	if (mode > 0) {
//		index = (log (mode) / LOG2);
		
		index = 0;
		
//		g_print ("%d", mode);

		while (mode > 1) {
			mode = mode >> 1;
			index++;
		}

//		g_print (" => %d\n", index);
	}

	/* otherwise just return 0 */
	else {
		index = 0;
	}

	return index;
}


/** \brief Convert array index to hamlib mode.
 *  \param index An integer to convert.
 *  \return The hamlib mode.
 *
 * This function converts an array index (0,1,2,3,...) to hamlib mode type
 * (1,2,4,8,...). The conversion is done using the formula:
 *
 *     mode  = 1 << index
 *
 * and index has to be in the range [0..12]
 *
 * \bug 12 is the last mode (RIG_MODE_PKTFM) in hamlib 1.2; this may have to be
 *      updated if the hamlib API changes!
 *
 * \bug It would probably be better if we returned rmode_t type instead of guint.
 */
guint
rig_utils_index_to_mode (gint index)
{
	guint mode;

	/* check numerical sanity of input */
	if ((index >= 0) && (index <= 15)) {
		mode = 1 << index;
	}
	else {
		mode = 1;
	}

	return mode;
}


/** \brief Check filename for extension.
 *  \param filename The file name to check
 *  \param ext The extension to check for
 *
 * This function check filename to see, whether it has extension ext.
 * If not, it appends ext to filename.
 */
void
rig_utils_chk_ext (gchar **filename, const gchar *ext)
{
	gchar *buff;

	if (!g_str_has_suffix (*filename, ext)) {

		buff = g_strconcat (*filename, ext, NULL);

		g_free (*filename);

		*filename = g_strdup (buff);

		g_free (buff);
	}

}
