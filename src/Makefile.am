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
	rig-ctrl.c rig-ctrl.h \
	rig-daemon.c rig-daemon.h \
	rig-daemon-check.c rig-daemon-check.h \
	rig-data.c rig-data-h \
	grig-menu.c


grig_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

