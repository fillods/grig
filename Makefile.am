## Process this file with automake to produce Makefile.in

SUBDIRS = m4  src

EXTRA_DIST = config.rpath mkinstalldirs  config.rpath mkinstalldirs  config.rpath  config.rpath mkinstalldirs \
	grig.1.in \
	grig.desktop \
	grig.spec.in \
	grig.spec


CLEANFILES = *~

install-data-local:
	@$(NORMAL_INSTALL)
 
	if test -d $(srcdir)/pixmaps; then \
		$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
		for pixmap in $(srcdir)/pixmaps/*; do \
			if test -f $$pixmap; then \
				$(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
			fi \
		done \
	fi
## manual page into mandir
	if test -f $(srcdir)/xlog.1; then \
		$(mkinstalldirs) $(DESTDIR)$(mandir)/man1; \
		$(INSTALL_DATA) xlog.1 $(DESTDIR)$(mandir)/man1; \
	fi

##	$(INSTALL_DATA) FAQ $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) README $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) NEWS $(DESTDIR)$(pkgdatadir)
##	$(INSTALL_DATA) TODO $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) AUTHORS $(DESTDIR)$(pkgdatadir)
##	$(INSTALL_DATA) THANKS $(DESTDIR)$(pkgdatadir)
##	$(INSTALL_DATA) MANUAL $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) ChangeLog $(DESTDIR)$(pkgdatadir)

dist-hook:
	if test -d pixmaps; then \
		mkdir $(distdir)/pixmaps; \
		for pixmap in pixmaps/*; do \
			if test -f $$pixmap; then \
				cp -p $$pixmap $(distdir)/pixmaps; \
			fi \
		done \
	fi

	if test -d debian; then \
		mkdir $(distdir)/debian; \
		for file in debian/*; do \
			if test -f $$file; then \
				cp -p $$file $(distdir)/debian; \
			fi \
		done \
	fi

rpm: Makefile
	make dist
	$(RPMBUILD) -ta $(PACKAGE)-$(VERSION).tar.gz
	rm $(PACKAGE)-$(VERSION).tar.gz

ACLOCAL_AMFLAGS = -I m4
