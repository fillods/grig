## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = grig

grig_SOURCES = \
	main.c \
	grig-druid.c grig-druid.h \
	rig-data.c rig-data-h


grig_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

