## Process this file with automake to produce Makefile.in
# NB: don't define GDK_DISABLE_DEPRECATED/GTK_DISABLE_DEPRECATED
#     until the code base is converted to more recent gtk.

AM_CPPFLAGS = \
	@PACKAGE_CFLAGS@ -I.. \
	-DG_DISABLE_DEPRECATED \
	-DGDK_PIXBUF_DISABLE_DEPRECATED \
    -DDATADIR=\""$(datadir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)/grig"\" \
	-DPACKAGE_PIXMAPS_DIR=\""$(datadir)/pixmaps/grig"\" \
	-DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"


bin_PROGRAMS = grig

grig_SOURCES = \
	main.c \
	compat.c compat.h \
	grig-about.c grig-about.h \
	grig-config.c grig-config.h \
	grig-debug.c grig-debug.h \
	grig-gtk-workarounds.c grig-gtk-workarounds.h \
	grig-menubar.c grig-menubar.h \
	key-press-handler.c key-press-handler.h \
	radio-conf.c radio-conf.h \
	rig-anomaly.c rig-anomaly.h \
	rig-daemon.c rig-daemon.h \
	rig-daemon-check.c rig-daemon-check.h \
	rig-data.c rig-data.h \
	rig-gui.c rig-gui.h \
	rig-gui-buttons.c rig-gui-buttons.h \
	rig-gui-ctrl2.c rig-gui-ctrl2.h \
	rig-gui-info.c rig-gui-info.h rig-gui-info-data.h \
	rig-gui-lcd.c rig-gui-lcd.h \
	rig-gui-keypad.c rig-gui-keypad.h \
	rig-gui-levels.c rig-gui-levels.h \
	rig-gui-message-window.c rig-gui-message-window.h \
	rig-gui-rx.c rig-gui-rx.h \
	rig-gui-smeter.c rig-gui-smeter.h \
	rig-gui-smeter-conv.c rig-gui-smeter-conv.h \
	rig-gui-tx.c rig-gui-tx.h \
	rig-gui-func.c rig-gui-func.h \
	rig-gui-vfo.c rig-gui-vfo.h \
	rig-selector.c rig-selector.h \
	rig-state.c rig-state.h \
	rig-utils.c rig-utils.h

grig_LDADD = @PACKAGE_LIBS@

## $(INTLLIBS)

