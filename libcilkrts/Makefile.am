# Copyright (C) 2011 
# Intel Corporation
# 
# This file is part of the Intel Cilk Plus Library.  This library is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# Under Section 7 of GPL version 3, you are granted additional
# permissions described in the GCC Runtime Library Exception, version
# 3.1, as published by the Free Software Foundation.
# 
# You should have received a copy of the GNU General Public License and
# a copy of the GCC Runtime Library Exception along with this program;
# see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
# <http://www.gnu.org/licenses/>.

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I ..

# Compiler and linker flags.
GENERAL_FLAGS = -I$(top_srcdir)/include
GENERAL_FLAGS += -fpic -O3
GENERAL_FLAGS += -DBUILD_USER=\"$(USER)\"
GENERAL_FLAGS += -DBUILD_HOST=\"`hostname`\"
AM_CFLAGS = $(GENERAL_FLAGS) -std=c99
AM_CPPFLAGS = $(GENERAL_FLAGS)
AM_LDFLAGS = -lpthread -ldl

# Target list.
lib_LTLIBRARIES = libcilkrts.la

libcilkrts_la_SOURCES = runtime/cilk-abi.c runtime/cilk_api.c           \
  runtime/cilk_malloc.c runtime/c_reducers.c                            \
  runtime/frame_malloc.c runtime/full_frame.c runtime/jmpbuf.c          \
  runtime/local_state.c runtime/metacall_impl.c runtime/os_mutex-unix.c \
  runtime/os-unix.c runtime/scheduler.c runtime/signal_node.c           \
  runtime/stacks.c runtime/stats.c runtime/symbol_test.c                \
  runtime/sysdep-unix.c runtime/worker_mutex.c runtime/bug.cpp          \
  runtime/cilk-abi-cilk-for.cpp runtime/except-gcc.cpp                  \
  runtime/global_state.cpp runtime/reducer_impl.cpp

# Load the $(REVISION) value.
include include/internal/rev.mk

libcilkrts_la_LDFLAGS  = -rpath '$(libdir)'
libcilkrts_la_LDFLAGS += -version-info 5:$(CILK_REVISION):0
libcilkrts_la_LDFLAGS += -lpthread -ldl

# C/C++ header files for Cilk.
cilkincludedir = $(includedir)/cilk
cilkinclude_HEADERS =              \
  include/cilk/cilk_api.h          \
  include/cilk/cilk_api_linux.h    \
  include/cilk/cilk.h              \
  include/cilk/cilk_stub.h         \
  include/cilk/cilk_undocumented.h \
  include/cilk/common.h            \
  include/cilk/holder.h            \
  include/cilk/hyperobject_base.h  \
  include/cilk/reducer_file.h      \
  include/cilk/reducer.h           \
  include/cilk/reducer_list.h      \
  include/cilk/reducer_max.h       \
  include/cilk/reducer_min.h       \
  include/cilk/reducer_opadd.h     \
  include/cilk/reducer_opand.h     \
  include/cilk/reducer_opor.h      \
  include/cilk/reducer_opxor.h     \
  include/cilk/reducer_ostream.h   \
  include/cilk/reducer_string.h
