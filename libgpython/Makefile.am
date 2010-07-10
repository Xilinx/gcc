## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libgpython.la

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_YFLAGS = -d --verbose

library_includedir=$(includedir)/gpython
library_include_HEADERS = include/gpython/gpython.h \
include/gpython/vectors.h \
include/gpython/garbage.h \
include/gpython/objects.h

libgpython_la_CFLAGS = -DPYHON -DSHARED
libgpython_la_LDFLAGS = -release 0.0.1
libgpython_la_SOURCES = runtime/py_runtime.c \
runtime/py_garbage.c \
runtime/vectors.c \
runtime/obj_integer.c \
runtime/backend.c
