/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offsett: 4 -*- */
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
/** \brief Grig configuration utilities.
 *  \ingroup util
 *
 * These functions are used to read and save grig configuration data
 * to and from the grig.cfg file. For a description of the configuration
 * parameters see the Grig Technical Manual
 *
 */
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "grig-debug.h"
#include "grig-config.h"



static gint check_grig_dir (void);
static gint check_cfg_file (void);
static gint check_rig_files (void);
static gint check_rot_files (void);
static gint check_mem_files (void);



/** \brief Check configuration for compatibility.
 *  \return TRUE if configuration is OK, FALSE if there was a non-fixable problem.
 *
 * This function checks the grig configuration for consistency and version
 * comaptibility.
 */
gint grig_config_check ()
{
	gboolean error = FALSE;


	grig_debug_local (RIG_DEBUG_VERBOSE,
					  _("Checking GRIG configuration."));

	/* check for .grig folder in user home directory */
	error |= check_grig_dir ();
	error |= check_cfg_file ();
	error |= check_rig_files ();
	error |= check_rot_files ();
	error |= check_mem_files ();


	return !error;
}



/** \brief Check configuration directory.
 *  \return 0 if successful, -1 if an error ocurred.
 *
 * This function checks for the existence of the .grig directory in the
 * user's home folder and creates it if the directory doesn't already exist.
 */
static gint
check_grig_dir ()
{
	gchar *dir;
	gint status = 0;

	dir = g_strconcat (g_get_home_dir (), G_DIR_SEPARATOR_S, ".grig", NULL);

	if (!g_file_test (dir, G_FILE_TEST_IS_DIR)) {
		
		/* try to create directory */
		status = g_mkdir (dir, 0750);
	}

	g_free (dir);

	grig_debug_local (RIG_DEBUG_VERBOSE,
					  _("..Configuration directory: %s"),
					  status ? _("ERROR") : _("OK"));

	return status;
}



static gint
check_cfg_file ()
{
	gint status = 0;


	return status;
}


/** \brief Check version of .rig files and update if necessary
 *  \return 0 if all checks/updates were successful, -1 if an error ocurred
 *
 * This function checks all .rig files in the configuration directory. If the
 * config version is lower than GRIG_RIG_CFG_VER, it tries to update to the
 * new .rig format. If the config version is higher (i.e. grig has been downgraded)
 * the function raises an error flag.
 *
 * The function does nothing if the config version equals GRIG_RIG_CFG_VER.
 *
 */
static gint
check_rig_files ()
{
	gint status = 0;
	GDir  *dir = NULL;
	gchar *dirname;
	const gchar *fname;
	gchar *fpath;
	GError *err = NULL;
	

	grig_debug_local (RIG_DEBUG_VERBOSE,
					  _("..Radio config files:"));

	/* scan .grig directory for .rig files */
	dirname = g_strconcat (g_get_home_dir (), G_DIR_SEPARATOR_S, ".grig", NULL);
	dir = g_dir_open (dirname, 0, &err);

	if (err != NULL) {
		grig_debug_local (RIG_DEBUG_ERR,
						  _("%s: %s"),
						  __FUNCTION__, err->message);
		g_clear_error (&err);

		return -1;
	}

	while ((fname = g_dir_read_name (dir)) != NULL) {

		/* we are only interested in .rig files */
		fpath = g_strconcat (dirname, G_DIR_SEPARATOR_S, fname, NULL);
		if ((!g_file_test (fpath, G_FILE_TEST_IS_DIR)) &&
			(g_strrstr (fname, ".grc"))) {

            /* FIXME: check config version */
            
			grig_debug_local (RIG_DEBUG_VERBOSE,
							  _("....%s OK"),
							  fname);

		}
		g_free (fpath);

	}

	g_dir_close (dir);
	g_free (dirname);

	return status;
}


/** \brief Check version of .rot files and update if necessary
 *  \return 0 if all checks/updates were successful, -1 if an error ocurred
 *
 * This function checks all .rot files in the configuration directory. If the
 * config version is lower than GRIG_ROT_CFG_VER, it tries to update to the
 * new .rot format. If the config version is higher (i.e. grig has been downgraded)
 * the function raises an error flag.
 *
 * The function does nothing if the config version equals GRIG_ROT_CFG_VER.
 *
 */
static gint
check_rot_files ()
{
	gint status = 0;


	return status;
}



/** \brief Check version of .mem files and update if necessary
 *  \return 0 if all checks/updates were successful, -1 if an error ocurred
 *
 * This function checks all .mem files in the configuration directory. If the
 * config version is lower than GRIG_MEM_CFG_VER, it tries to update to the
 * new .mem format. If the config version is higher (i.e. grig has been downgraded)
 * the function raises an error flag.
 *
 * The function does nothing if the config version equals GRIG_MEM_CFG_VER.
 *
 */
static gint
check_mem_files ()
{
	gint status = 0;

	return status;
}
