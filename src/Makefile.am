## Process this file with automake to produce Makefile.in

INCLUDES = \
	@PACKAGE_CFLAGS@ -I.. \
	-DG_DISABLE_DEPRECATED \
	-DGDK_DISABLE_DEPRECATED \
	-DGDK_PIXBUF_DISABLE_DEPRECATED \
	-DGTK_DISABLE_DEPRECATED
##	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
##	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
##	@PACKAGE_CFLAGS@

bin_PROGRAMS = grig

grig_SOURCES = \
	main.c \
	compat.c compat.h \
	grig-about.c grig-about.h \
	grig-config.c grig-config.h \
	grig-debug.c grig-debug.h \
	grig-gtk-workarounds.c grig-gtk-workarounds.h \
	grig-menubar.c grig-menubar.h \
	rig-anomaly.c rig-anomaly.h \
	rig-daemon.c rig-daemon.h \
	rig-daemon-check.c rig-daemon-check.h \
	rig-data.c rig-data.h \
	rig-gui.c rig-gui.h \
	rig-gui-buttons.c rig-gui-buttons.h \
	rig-gui-ctrl2.c rig-gui-ctrl2.h \
	rig-gui-info.c rig-gui-info.h rig-gui-info-data.h \
	rig-gui-lcd.c rig-gui-lcd.h \
	rig-gui-levels.c rig-gui-leves.h \
	rig-gui-message-window.c rig-gui-message-window.h \
	rig-gui-smeter.c rig-gui-smeter.h \
	rig-gui-smeter-conv.c rig-gui-smeter-conv.h \
	rig-gui-vfo.c rig-gui-vfo.h \
	rig-utils.c rig-utils.h

grig_LDADD = @PACKAGE_LIBS@

## $(INTLLIBS)

