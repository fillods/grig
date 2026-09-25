# Makefile for cross-compiling grig for win32 on Linux,
# using mingw compiler

include config.mk

# directories
topsrc = ..
grigdir = $(topsrc)/src

# tools
CC = $(MGW_PREFIX)gcc -Wall -O2 -mms-bitfields -DWIN32
AS = $(MGW_PREFIX)as
DLLWRAP = $(MGW_PREFIX)dllwrap
DLLWRAP_FLAGS = --as=$(AS) --export-all --driver-name $(CC) -s
RC = $(MGW_PREFIX)windres
RCFLAGS = --define __WIN32__ --define __WIN95__ --define MSRC \
	--define __GNUWIN32__


# libraries
#GTKLIBS := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs gtk+-win32-2.0)
GTKLIBS := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs gtk+-win32-3.0)
GLIBLIB := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs glib-2.0 gthread-2.0)
HAMLIBLIB := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs hamlib)
GUI_LIBS = $(GTKLIBS) $(GLIBLIB)
LIBS = -lm $(HAMLIBLIB)

# flags/defines
CFLAGS = -DVERSION=\"0.9.2\" -DHAVE_GETOPT_H -I. -I$(grigdir) -I$(CROSSDIR)/include
GTK_CFLAGS := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags gtk+-win32-3.0)
#GTK_CFLAGS := `pkg-config --cflags gtk+-win32-2.0`


# source paths
vpath %.c $(grigdir)
vpath %.rc $(topsrc)/win32

#### End of sys conf section

GRIGSRC = \
        compat.c \
        grig-about.c \
        grig-config.c \
        grig-debug.c \
        grig-gtk-workarounds.c \
        grig-menubar.c \
	key-press-handler.c \
        main.c \
        radio-conf.c \
        rig-anomaly.c \
        rig-daemon.c \
        rig-daemon-check.c \
        rig-data.c \
        rig-gui-buttons.c \
        rig-gui.c \
        rig-gui-ctrl2.c \
        rig-gui-func.c \
        rig-gui-info.c \
        rig-gui-keypad.c \
        rig-gui-lcd.c \
        rig-gui-levels.c \
        rig-gui-message-window.c \
        rig-gui-rx.c \
        rig-gui-smeter.c \
        rig-gui-smeter-conv.c \
        rig-gui-tx.c \
        rig-gui-vfo.c \
        rig-selector.c \
        rig-state.c \
        rig-utils.c

GRIGOBJ = $(GRIGSRC:.c=.o)


OBJS = $(GRIGOBJ)

RELEASE=$(shell /usr/bin/awk 'BEGIN{FS="["; RS="],"} /\[0\./ { print $2; exit }' ../configure.ac)
HL_FILENAME=grig-win-x64-$(RELEASE)
INST_DIR=./mingw64-inst
ZIP_DIR=./grig

%.o: %.c
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $<
	$(CC) -MM $(CFLAGS) $(GTK_CFLAGS) $< > $*.d 

# targets begin

all: grig.exe
# create bin and copy in executable and all relevant dlls
	mkdir -p ${INST_DIR}/bin
	cp ./grig.bat ${INST_DIR}
	cp ./grig.exe ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/gdk-pixbuf-query-loaders.exe ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/iconv.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libatk-1.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libbz2-1.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libcairo-2.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libcairo-gobject-2.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libepoxy-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libexpat-1.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libffi-8.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libfontconfig-1.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libfreetype-6.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libfribidi-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libgcc_s_seh-1.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libgdk-3-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libgdk_pixbuf-2.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libgio-2.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libglib-2.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libgmodule-2.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libgobject-2.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libgtk-3-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libhamlib-4.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libharfbuzz-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libintl-8.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libjpeg-62.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libpango-1.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libpangocairo-1.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libpangoft2-1.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libpangowin32-1.0-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libpcre2-8-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libpixman-1-0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libpng16-16.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libstdc++-6.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libtiff-6.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libusb-1.0.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/libwinpthread-1.dll ${INST_DIR}/bin
	cp $(CROSSDIR)/bin/zlib1.dll ${INST_DIR}/bin
# create lib and copy in all relevant pixbuf loaders
	mkdir -p ${INST_DIR}/lib/gdk-pixbuf-2.0/2.10.0/loaders
	cp $(CROSSDIR)/lib/gdk-pixbuf-2.0/2.10.0/loaders/*.dll ${INST_DIR}/lib/gdk-pixbuf-2.0/2.10.0/loaders
# create share and copy in all pixmaps
	mkdir -p $(INST_DIR)/share/pixmaps/grig
	cp ../pixmaps/*.png ${INST_DIR}/share/pixmaps/grig
	cp icons/*.ico ${INST_DIR}/share/pixmaps/grig
# copy in all supplemental distro info
	cp ../ABOUT-NLS ${INST_DIR}
	cp ../AUTHORS ${INST_DIR}
	cp ../ChangeLog ${INST_DIR}
	cp ../CONTRIBUTING.md ${INST_DIR}
	cp ../COPYING ${INST_DIR}
	cp ../NEWS ${INST_DIR}
	cp ../README.md ${INST_DIR}
	cp ../TODO ${INST_DIR}
#temporarily rename the installation directory and zip it up
	mv ${INST_DIR} ${ZIP_DIR}
	/usr/bin/zip -r ${HL_FILENAME}.zip $(basename ${ZIP_DIR})
	mv ${ZIP_DIR} ${INST_DIR}

#grig.exe: $(GRIGOBJ) grig_res.o $(CROSSDIR)/lib/libhamlib-4.lib
grig.exe: $(GRIGOBJ) grig_res.o
	$(CC) -mconsole -o $@ $^ $(GUI_LIBS) $(LIBS) -lmingwex -s

grig_res.o: grig.rc 
	$(RC) $(RCFLAGS) $< $@


clean:
	rm -f *.o *.d *.def *.exe *.zip
	rm -rf ${INST_DIR}
	
