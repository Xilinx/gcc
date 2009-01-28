## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I .. -I ../config
SUBDIRS = testsuite

## May be used by toolexeclibdir.
gcc_version := $(shell cat $(top_srcdir)/../gcc/BASE-VER)

config_path = @config_path@
search_path = $(addprefix $(top_srcdir)/config/, $(config_path)) $(top_srcdir)

fincludedir = $(libdir)/gcc/$(target_alias)/$(gcc_version)/finclude
libsubincludedir = $(libdir)/gcc/$(target_alias)/$(gcc_version)/include

vpath % $(strip $(search_path))

AM_CPPFLAGS = $(addprefix -I, $(search_path))
AM_CFLAGS = $(XCFLAGS)
AM_LDFLAGS = $(XLDFLAGS) $(SECTION_LDFLAGS) $(OPT_LDFLAGS)

CCAS = $(CC)
CCASFLAGS = $(CFLAGS)
LTCCASCOMPILE = $(LIBTOOL) --mode=compile --tag=CC $(CCAS) \
	$(AM_CCASFLAGS) $(CCASFLAGS)

toolexeclib_LTLIBRARIES = libitm.la
nodist_toolexeclib_HEADERS = libitm.spec

if LIBITM_BUILD_VERSIONED_SHLIB
libitm_version_script = -Wl,--version-script,$(top_srcdir)/libitm.map
else
libitm_version_script =
endif
libitm_version_info = -version-info $(libtool_VERSION)
libitm_la_LDFLAGS = $(libitm_version_info) $(libitm_version_script) \
        -no-undefined

libitm_la_SOURCES = beginend.c dispatch.c local.c query.c retry.c \
	rwlock.c serial.c sjlj.S useraction.c futex.c \
	method-wbetl.c
