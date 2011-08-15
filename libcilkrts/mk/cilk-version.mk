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
# cilk-version.mk
#
# The one place we look up information from the code management system
#
# Note that the build number is *only* valid on the build machines

ifeq ($(wildcard $(TOP)/../.hg),)
  # If this is the open source release, there is no Mercurial repository,
  # so set some reasonable defaults.
  CILK_VERSION_MAJOR := 2
  CILK_VERSION_MINOR := 0
  CILK_VERSION_BUILD := 1
  CILK_VERSION_REV   := 0

  CILK_VERSION_HASH  := 000000000000
  CILK_VERSION_BRANCH := oss
else
  CILK_VERSION_MAJOR := 2
  CILK_VERSION_MINOR := 0
  CILK_VERSION_BUILD := $(firstword $(subst +, ,$(shell hg id --num)))
  CILK_VERSION_REV   := 0

  CILK_VERSION_HASH := $(firstword $(subst +, ,$(shell hg id --id)))
  CILK_VERSION_BRANCH := $(shell hg id --branch)
endif

