## Process this file with automake to produce Makefile.in

## May be used by toolexeclibdir.
gcc_version = $(shell cat $(top_srcdir)/../gcc/BASE-VER)

lib_LTLIBRARIES = libgpython.la

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_YFLAGS = -d --verbose

libgpython_la_CFLAGS = -DPYHON -DSHARED
libgpython_la_LDFLAGS = -release 0.0.1
libgpython_la_SOURCES = runtime/py_runtime.c
