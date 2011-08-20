/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
    Grig:  Gtk+ user interface for the Hamradio Control Libraries.

    Copyright (C)  2001-2007  Alexandru Csete, OZ9AEC.

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
/** \brief Utilities to ensure compatibility across multiple platforms.
 */

#include <glib.h>
#include "compat.h"
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif



/** \brief Get data directory.
 *
 * On linux it corresponds to the PACKAGE_DATA_DIR macro defined in config.h
 * The function returns a newly allocated gchar * which must be free when
 * it is no longer needed.
 */
gchar *
get_data_dir   ()
{
        gchar *dir = NULL;


#ifdef G_OS_UNIX
        dir = g_strconcat (PACKAGE_DATA_DIR, G_DIR_SEPARATOR_S, "data", NULL);
#else
#  ifdef G_OS_WIN32
        gchar *buff = g_win32_get_package_installation_directory_of_module (NULL);
        dir = g_strconcat (buff, G_DIR_SEPARATOR_S,
                           "share", G_DIR_SEPARATOR_S,
                           "grig", G_DIR_SEPARATOR_S,
                           "data", NULL);
        g_free (buff);
#  endif
#endif

        return dir;
}


/** \brief Get absolute file name of a data file.
 *
 * This function returns the absolute file name of a data file. It is intended to
 * be a one-line filename constructor.
 * The returned gchar * should be freed when no longer needed.
 */
gchar *
data_file_name (const gchar *data)
{
        gchar *filename = NULL;
        gchar *buff;

        buff = get_data_dir ();
        filename = g_strconcat (buff, G_DIR_SEPARATOR_S, data, NULL);
        g_free (buff);

        return filename;
}



/** \brief Get pixmaps directory.
 *
 * On linux it corresponds to the PACKAGE_DATA_DIR/pixmaps/maps
 * The function returns a newly allocated gchar * which must be free when
 * it is no longer needed.
 */
gchar *
get_pixmaps_dir   ()
{
        gchar *dir = NULL;


#ifdef G_OS_UNIX
        dir = g_strdup (PACKAGE_PIXMAPS_DIR);
#else
#  ifdef G_OS_WIN32
        gchar *buff = g_win32_get_package_installation_directory_of_module (NULL);
        dir = g_strconcat (buff, G_DIR_SEPARATOR_S,
                           "share", G_DIR_SEPARATOR_S,
                           "pixmaps", G_DIR_SEPARATOR_S,
                           "grig", NULL);
        g_free (buff);
#  endif
#endif

        return dir;
}


/** \brief Get absolute file name of a pixmap file.
 *
 * This function returns the absolute file name of a pixmap file. It is intended to
 * be a one-line filename constructor.
 * The returned gchar * should be freed when no longer needed.
 */
gchar *
pixmap_file_name (const gchar *map)
{
        gchar *filename = NULL;
        gchar *buff;

        buff = get_pixmaps_dir ();
        filename = g_strconcat (buff, G_DIR_SEPARATOR_S, map, NULL);
        g_free (buff);

        return filename;
}


/** \brief Get config directory.
 * \param subdir Optional subdirectory to append
 * \return A newly allocated string containing the conf dir.
 *
 * This funxction returns the absolute path of the user config directory
 * typically $HOME/.grig/
 */
gchar *
get_conf_dir (const gchar *subdir)
{
    gchar *dir;
    
    if (subdir != NULL) {
        dir = g_strconcat (g_get_home_dir(), G_DIR_SEPARATOR_S, 
                           ".grig", G_DIR_SEPARATOR_S,
                           subdir, NULL);
    }
    else {
        dir = g_strconcat (g_get_home_dir(), G_DIR_SEPARATOR_S,
                          ".grig", NULL);
    }
    
    return dir;
    
}

