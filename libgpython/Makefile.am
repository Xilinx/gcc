## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libgpython.la

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_YFLAGS = -d --verbose

library_includedir=$(includedir)/gpython
library_include_HEADERS = \
include/gpython/gpython.h \
include/gpython/vectors.h \
include/gpython/garbage.h \
include/gpython/objects.h

libgpython_la_CFLAGS = -DPYHON -DSHARED
libgpython_la_LDFLAGS = -release 0.0.1
libgpython_la_SOURCES = \
runtime/obj_boolean.c \
runtime/obj-string.c \
runtime/obj-float.c \
runtime/obj-list.c \
runtime/obj-integer.c \
runtime/py-runtime.c \
runtime/py_garbage.c \
runtime/py-utils.c \
runtime/backend.c \
runtime/vectors.c
