# Configuration variables governing the build of grig for win32

# directory containing the cross tools
CROSSDIR = /grga/mingw32/buildenv

# prefix for mingw tools (e.g. i586-mingw32msvc-gcc)
MGW_PREFIX = i586-mingw32msvc-

# mingw include dir
MGW_INC = $(CROSSDIR)/include

# libxml2 includes: adjust to match your system
XML2_INC = $(MGW_INC)/libxml2

# msgfmt command for producing win32 messages file


# pkgconfig path
PKG_CONFIG_PATH = $(CROSSDIR)/lib/pkgconfig
