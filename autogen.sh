#!/bin/sh

# generate all the initial makefiles, etc.
AUTOCONF=autoconf
AUTOMAKE=automake-1.7
ACLOCAL=aclocal-1.7

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

cd $srcdir
PROJECT=grig
FILE=pixmaps/grig-logo.png

DIE=0

($AUTOCONF --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have autoconf installed to compile $PROJECT."
        echo "Download the appropriate package for your distribution,"
        echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/autoconf."
        DIE=1
}

($AUTOMAKE --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have automake installed to compile $PROJECT."
        echo "Download the appropriate package for your distribution,"
        echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/automake."
        DIE=1
}

(gettext --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have gettext installed to compile $PROJECT."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/gettext."
    DIE=1
}


if test "$DIE" -eq 1; then
        exit 1
fi

test -f $FILE || {
        echo "You must run this script in the top-level $PROJECT directory"
        exit 1
}

echo "Running gettextize...  Ignore non-fatal messages."
gettextize --force --copy --no-changelog
$ACLOCAL
$AUTOMAKE --add-missing --copy
$AUTOCONF

echo "Running configure --enable-maintainer-mode --enable-compile-warnings."
$srcdir/configure --enable-maintainer-mode --enable-compile-warnings "$@"
