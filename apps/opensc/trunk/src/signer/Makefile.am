# Process this file with automake to create Makefile.in

plugindir = $(PLUGINDIR)

MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = . npinclude

EXTRA_DIST = testprog.c

INCLUDES = -I$(srcdir)/npinclude $(CPPFLAGS) $(OPENSSL_CFLAGS) $(LIBASSUAN_CFLAGS)

if ENABLE_NSPLUGIN
lib_LTLIBRARIES = opensc-signer.la
endif

opensc_signer_la_LDFLAGS = -module -avoid-version
opensc_signer_la_LIBADD = @LIBOPENSC@ $(OPENSSL_LIBS) $(LIBASSUAN_LIBS)
opensc_signer_la_CFLAGS = $(AM_CFLAGS) -DXP_UNIX
opensc_signer_la_SOURCES = \
	opensc-crypto.c opensc-support.c signer.c stubs.c dialog.c
noinst_HEADERS = \
	opensc-crypto.h opensc-support.h signer.h 

install-exec-local: install-libLTLIBRARIES
if ENABLE_NSPLUGIN
	-$(mkdir_p) $(DESTDIR)$(plugindir)/
	-$(LN_S) -f $(libdir)/opensc-signer.so $(DESTDIR)$(plugindir)/
endif
