## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = grig

grig_SOURCES = \
	main.c \
	grig-about.c grig-about.h \
	grig-config.c grig-config.h \
	grig-druid.c grig-druid.h \
	rig-anomaly.c rig-anomaly.h \
	rig-daemon.c rig-daemon.h \
	rig-daemon-check.c rig-daemon-check.h \
	rig-data.c rig-data.h \
	rig-gui.c rig-gui.h \
	rig-gui-buttons.c rig-gui-buttons.h \
	rig-gui-smeter.c rig-gui-smeter.h \
	rig-utils.c rig-utils.h \
	grig-menu.c

grig_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

