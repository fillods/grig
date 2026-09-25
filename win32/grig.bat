@echo off
set XDG_DATA_DIRS=%cd%/lib/gdk-pixbuf-2.0/2.10.0/loaders
rem set GSETTINGS_SCHEMA_DIR=%cd%/share/glib-2.0/schemas

rem if not exist %cd%"\share\glib-2.0\schemas\gschemas.compiled" (
rem   echo compiling schemas
rem   bin\glib-compile-schemas.exe %cd%/share/glib-2.0/schemas
rem )

if not exist %cd%"\lib\gdk-pixbuf-2.0\2.10.0\loaders.cache" (
  echo updating loader cache
  bin\gdk-pixbuf-query-loaders.exe --update-cache
)

bin\grig.exe
