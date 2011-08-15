#########################################################################
#
# Copyright (C) 2009-2011 
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
###########################################################################
# Common environment for unix systems
#

# Default target is linux64 (release mode)
ifeq ($(findstring 64,$(shell uname -m)),)
  # If running on a 32-bit OS, default target is linux32
    ifeq ($(shell uname),Darwin)
        OUT ?= mac32
    else
        OUT ?= linux32
    endif
else
    ifeq ($(shell uname),Darwin)
        OUT ?= mac64
    else
        OUT ?= linux64
    endif
endif

# Strip the 'r' suffix off, if present.  Default is release mode.
ifeq ($(OUT),linux64r)
    override OUT = linux64
else ifeq ($(OUT),linux32r)
    override OUT = linux32
else ifeq ($(OUT),mac64r)
    override OUT = mac64
else ifeq ($(OUT),mac32r)
    override OUT = mac32
else ifeq ($(OUT),micr)
    override OUT = mic
else ifeq ($(OUT),miclinuxr)
    override OUT = miclinux
else ifeq ($(OUT),mic2linuxr)
    override OUT = mic2linux
endif

ifeq ($(OUT),linux64)
    # Linux 64 release mode
    SIZE = 64
else ifeq ($(OUT),linux32)
    # Linux 32 release mode
    SIZE = 32
else ifeq ($(OUT),mac64)
    # Mac OS 64 release mode
    SIZE = 64
else ifeq ($(OUT),mac32)
    # Mac OS 32 release mode
    SIZE = 32
else ifeq ($(OUT),mic)
    # MIC release mode
    SIZE = 64
else ifeq ($(OUT),miclinux)
    # MIC, Linux, release mode
    SIZE = 64
else ifeq ($(OUT),mic2linux)
    # MIC, Linux, release mode
    SIZE = 64
else ifeq ($(OUT),linux64d)
    # Linux 64 debug mode
    SIZE = 64
    OPT ?= -g --no-inline
else ifeq ($(OUT),linux32d)
    # Linux 32 debug mode
    SIZE = 32
    OPT ?= -g --no-inline
else ifeq ($(OUT),mac64d)
    # Mac OS 64 debug mode
    SIZE = 64
    OPT ?= -g --no-inline
else ifeq ($(OUT),mac32d)
    # Mac OS 32 debug mode
    SIZE = 32
    OPT ?= -g --no-inline
else ifeq ($(OUT),micd)
    # MIC debug mode
    SIZE = 64
    OPT ?= -g --no-inline
else ifeq ($(OUT),miclinuxd)
    # MIC LInux debug mode
    SIZE = 64
    OPT ?= -g --no-inline
else ifeq ($(OUT),mic2linuxd)
    # MIC2 Linux debug mode
    SIZE = 64
    OPT ?= -g --no-inline
else
    $(error OUT="$(OUT)" not one of linux32[rd], linux64[rd], mac64[rd], mac32[rd], mic[rd], miclinux[rd], mic2linux[rd])
endif

# Target type.
# Any OUT with a mic substring is a MIC target.
ifneq ($(findstring mic,$(OUT)),)
  mic_target=true
else ifeq ($(OUT:d=),mac64)
  macos_target=true
else ifeq ($(OUT:d=),mac32)
  macos_target=true
endif

ifdef macos_target
  OUTDIR = $(TOP)/mac-build
else
  OUTDIR = $(TOP)/unix-build
endif

THIRD_PARTY = $(TOP)/../../3rdparty
