AC_INIT(src/main.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(grig, 0.3.99.3)

AM_MAINTAINER_MODE
AC_PROG_INTLTOOL([0.21])


dnl check for programs
AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

AC_CHECK_HEADERS([sys/time.h unistd.h getopt.h])

if test "${ac_cv_c_compiler_gnu}" = "yes"; then
  CFLAGS="${CFLAGS} -Wall"
fi

pkg_modules="gtk+-2.0 >= 2.4.0 gthread-2.0 >= 2.4.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl Add the languages which your application supports here.
ALL_LINGUAS=
GETTEXT_PACKAGE=grig
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,
                   "$GETTEXT_PACKAGE",
                   "definition of GETTEXT_PACKAGE")

dnl Set PACKAGE_LOCALE_DIR
dnl if test "x${prefix}" = "xNONE"; then
dnl  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/share/locale")
dnl else
dnl   AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/share/locale")
dnl fi


dnl Set PACKAGE_DATA_DIR and PACKAGE_PIXMAPS_DIR
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR,
                       "${ac_default_prefix}/share/${PACKAGE}",
		       "Data directory")
    AC_DEFINE_UNQUOTED(PACKAGE_PIXMAPS_DIR,
                       "${ac_default_prefix}/share/${PACKAGE}/pixmaps",
                       "Pixmaps directory")
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR,
                       "${prefix}/share/${PACKAGE}",
		       "Data directory")
    AC_DEFINE_UNQUOTED(PACKAGE_PIXMAPS_DIR,
                       "${prefix}/share/${PACKAGE}/pixmaps",
                       "Pixmaps directory")
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR,
                     "${datadir}/${PACKAGE}",
		     "Data directory")
  AC_DEFINE_UNQUOTED(PACKAGE_PIXMAPS_DIR,
                     "${datadir}/${PACKAGE}/pixmaps",
                     "Pixmaps directory")
fi


dnl Set PACKAGE_SOURCE_DIR
dnl packagesrcdir=`cd $srcdir && pwd`
dnl AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}")

dnl Check hamlib
hamlib_modules="hamlib >= 1.2.0"
PKG_CHECK_MODULES(HAMLIB, [$hamlib_modules], [
  CFLAGS="$CFLAGS $HAMLIB_CFLAGS";
  LIBS="$LIBS $HAMLIB_LIBS";
dnl  AC_DEFINE(WANT_HAMLIB, 1)
], [
  AC_MSG_ERROR([Hamradio control libraries not found...])
])


dnl various developer/devloper options
dnl diable HW interaction; usefull to access RIG caps without
dnl having rig; default=no
AC_ARG_ENABLE(hardware, [  --disable-hardware      disable hardware IO],disable_hadware=yes,disable_hadware=no)
if test "$disable_hadware" = yes ; then
	AC_DEFINE(DISABLE_HW, 1, [Define if hardware is disabled.])
fi

dnl compiler flags to enable generating coverage report
dnl using gcov
AC_ARG_ENABLE(coverage, [  --enable-coverage       enable coverge reports],enable_coerage=yes,enable_coverage=no)
if test "$enable_coverage" = yes ; then
        CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage";
	AC_DEFINE(DISABLE_HW, 1, [Define if hardware is disabled.])
fi


dnl store library versions
HAMLIB_V=`pkg-config --modversion hamlib`
GLIB_V=`pkg-config --modversion glib-2.0`
GTHR_V=`pkg-config --modversion gthread-2.0`
GDK_V=`pkg-config --modversion gdk-2.0`
GTK_V=`pkg-config --modversion gtk+-2.0`

AC_DEFINE_UNQUOTED([HAMLIB_VERSION],[`pkg-config --modversion hamlib`],["Hamlib version"])


AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)

AC_OUTPUT([
Makefile
doc/Makefile
doc/man/grig.1
doc/man/Makefile
grig.spec
src/Makefile
pixmaps/Makefile
po/Makefile.in
])

echo
echo SUMMARY:
echo
echo grig version....... : $VERSION
echo Hamlib version..... : $HAMLIB_V
echo Glib version....... : $GLIB_V
echo Gthread version.... : $GTHR_V
echo Gdk version........ : $GDK_V
echo Gtk+ version....... : $GTK_V
echo Disable hardware... : $disable_hadware
echo Enable coverage.... : $enable_coverage
echo
