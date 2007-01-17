SUBDIRS = src doc pixmaps po

install-data-local:
	@$(NORMAL_INSTALL)
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) ChangeLog $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) README $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) COPYING $(DESTDIR)$(pkgdatadir)

EXTRA_DIST = \
	autogen.sh            \
	intltool-extract.in   \
	intltool-merge.in     \
	intltool-update.in    \
	win32/config.mk       \
	win32/grig.rc         \
	win32/Makefile        \
	win32/icons/grig-icon.ico
