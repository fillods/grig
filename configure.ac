AC_INIT(src/main.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(grig, 0.7.0)

AM_MAINTAINER_MODE
AC_PROG_INTLTOOL([0.21])


dnl check for programs
AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

AC_CHECK_HEADERS([sys/time.h getopt.h])

if test "${ac_cv_c_compiler_gnu}" = "yes"; then
  CFLAGS="${CFLAGS} -Wall"
fi

pkg_modules="gtk+-2.0 >= 2.6.0 gthread-2.0 >= 2.6.0"
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


dnl Check hamlib
hamlib_modules="hamlib >= 1.2.5"
PKG_CHECK_MODULES(HAMLIB, [$hamlib_modules], [
  CFLAGS="$CFLAGS $HAMLIB_CFLAGS";
  LIBS="$LIBS $HAMLIB_LIBS";
dnl  AC_DEFINE(WANT_HAMLIB, 1)
], [
  AC_MSG_ERROR([Hamradio control libraries 1.2.5 or later not found...])
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

