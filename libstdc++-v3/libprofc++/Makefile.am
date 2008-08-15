## Makefile for the GNU C++ Support library.
##
## Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
## Free Software Foundation, Inc.
##
## Process this file with automake to produce Makefile.in.
##
## This file is part of GCC.
##
## GCC is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## GCC is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with GCC; see the file COPYING.  If not, write to
## the Free Software Foundation, 51 Franklin Street, Fifth Floor,
## Boston, MA 02110-1301, USA.

include $(top_srcdir)/fragment.am

toolexeclib_LTLIBRARIES = libprofc++.la
noinst_LTLIBRARIES = libprofc++convenience.la

headers = \
    profiler.h \

sources = \
	profiler_node.cc \
    profiler_trace.cc \
    profiler_state.cc \
    profiler_container_size.cc \
    profiler_hashtable_size.cc \
    profiler_vector_size.cc

libprofc___la_SOURCES = $(sources) $(c_sources)
libprofc__convenience_la_SOURCES = $(sources) $(c_sources)

glibcxxinstalldir = $(gxx_include_dir)
glibcxxinstall_HEADERS = $(headers)
