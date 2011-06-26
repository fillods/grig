SUBDIRS = src doc pixmaps po

install-data-local:
	@$(NORMAL_INSTALL)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) ChangeLog $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) README $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) COPYING $(DESTDIR)$(pkgdatadir)

EXTRA_DIST = \
	win32/config.mk       \
	win32/grig.rc         \
	win32/Makefile        \
	win32/icons/grig-icon.ico
