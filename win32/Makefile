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
GTKLIBS := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs gtk+-win32-2.0)
GLIBLIB := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs glib-2.0 gthread-2.0)
GUI_LIBS = $(GTKLIBS) $(GLIBLIB)
LIBS = -lm

# flags/defines
CFLAGS = -DVERSION=\"0.8.0\" -DHAVE_GETOPT_H -I. -I$(grigdir) -I$(CROSSDIR)/include
GTK_CFLAGS := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags gtk+-win32-2.0)
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
        rig-anomaly.c \
        rig-daemon.c \
        rig-daemon-check.c \
        rig-data.c \
        rig-gui-buttons.c \
        rig-gui.c \
        rig-gui-ctrl2.c \
        rig-gui-info.c \
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

%.o: %.c
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $<
	$(CC) -MM $(CFLAGS) $(GTK_CFLAGS) $< > $*.d 

# targets begin

all: grig.exe

grig.exe: $(GRIGOBJ) grig_res.o $(CROSSDIR)/lib/libhamlib-2.lib
	$(CC) -mconsole -o $@ $^ $(GUI_LIBS) $(LIBS) -lmingwex -s

grig_res.o: grig.rc 
	$(RC) $(RCFLAGS) $< $@


clean:
	rm -f *.o *.d *.def *.exe
