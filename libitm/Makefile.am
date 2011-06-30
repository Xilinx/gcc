## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I .. -I ../config
SUBDIRS = testsuite

## May be used by toolexeclibdir.
gcc_version := $(shell cat $(top_srcdir)/../gcc/BASE-VER)

abi_version = -fabi-version=4

config_path = @config_path@
search_path = $(addprefix $(top_srcdir)/config/, $(config_path)) $(top_srcdir)

fincludedir = $(libdir)/gcc/$(target_alias)/$(gcc_version)/finclude
libsubincludedir = $(libdir)/gcc/$(target_alias)/$(gcc_version)/include

vpath % $(strip $(search_path))

AM_CPPFLAGS = $(addprefix -I, $(search_path))
AM_CFLAGS = $(XCFLAGS)
AM_CXXFLAGS = -std=gnu++0x -funwind-tables -fno-exceptions -fno-rtti \
	$(XCFLAGS) $(abi_version)
AM_CCASFLAGS = $(XCFLAGS)
AM_LDFLAGS = $(XLDFLAGS) $(SECTION_LDFLAGS) $(OPT_LDFLAGS)

toolexeclib_LTLIBRARIES = libitm.la
nodist_toolexeclib_HEADERS = libitm.spec

if LIBITM_BUILD_VERSIONED_SHLIB
libitm_version_script = -Wl,--version-script,$(top_srcdir)/libitm.map
else
libitm_version_script =
endif
libitm_version_info = -version-info $(libtool_VERSION)

# Force link with C, not C++.  For now, while we're using C++ we don't
# want or need libstdc++.
libitm_la_LINK = $(LINK)
libitm_la_LDFLAGS = $(libitm_version_info) $(libitm_version_script) \
        -no-undefined

libitm_la_SOURCES = \
	aatree.cc alloc.cc alloc_c.cc alloc_cpp.cc barrier.cc beginend.cc \
	clone.cc cacheline.cc cachepage.cc eh_cpp.cc local.cc \
	query.cc retry.cc rwlock.cc useraction.cc util.cc \
	sjlj.S tls.cc method-serial.cc

if ARCH_X86
libitm_la_SOURCES += x86_sse.cc x86_avx.cc
x86_sse.lo : XCFLAGS += -msse
x86_avx.lo : XCFLAGS += -mavx
endif
