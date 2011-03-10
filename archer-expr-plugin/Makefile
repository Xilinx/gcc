# Makefile for GNU Compiler Collection C and C++ expression parser
# plug-ins.
# Copyright (C) 2011 Red Hat, Inc.
# Written by Keith Seitz <keiths@redhat.com>
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

C_PLUGIN_NAME=cexpr
CP_PLUGIN_NAME=cpexpr

GCC=gcc
PLUGIN_SOURCE_FILES= gccexpr.c
C_PLUGIN_OBJECT_FILES= cexpr.o
CP_PLUGIN_OBJECT_FILES= cpexpr.o
GCCPLUGINS_DIR:= $(shell $(GCC) -print-file-name=plugin)
CFLAGS+= -I$(GCCPLUGINS_DIR)/include -fPIC -g -c

all: c-common.h.stamp $(C_PLUGIN_NAME).so $(CP_PLUGIN_NAME).so

# This is a hack.  The source file gcc/c-family/c-common.h is not installed
# into $(GCCPLUGINS_DIR)/include/c-family.  It is instead installed into
# $(GCCPLUGINS_DIR)/include.  Normally not a problem, except that
# cp-tree.h #includes "c-family/c-common.h".  So the build will fail if this
# file is not in the right place.  To work around this temporarily, the
# build will check if this file is absent and copy it into the right place.
c-common.h.stamp: $(GCCPLUGINS_DIR)/include/c-common.h
	@test -f $(GCCPLUGINS_DIR)/include/c-family/c-common.h \
		|| (cp $^ $(GCCPLUGINS_DIR)/include/c-family/c-common.h \
		    && echo "WARNING: c-family/c-common.h missing: copying from $(GCCPLUGINS_DIR)/include")
	touch c-common.h.stamp

$(C_PLUGIN_NAME).so: $(C_PLUGIN_OBJECT_FILES)
	$(GCC) -shared $^ -o $@

$(CP_PLUGIN_NAME).so: $(CP_PLUGIN_OBJECT_FILES)
	$(GCC) -shared $^ -o $@

install: $(C_PLUGIN_NAME).so $(CP_PLUGIN_NAME).so
	cp $^ $(GCCPLUGINS_DIR)

$(C_PLUGIN_OBJECT_FILES): $(PLUGIN_SOURCE_FILES)
	$(GCC) $(CFLAGS) $^ -o $@

$(CP_PLUGIN_OBJECT_FILES): $(PLUGIN_SOURCE_FILES)
	$(GCC) $(CFLAGS) -DCPLUS $^ -o $@

clean:
	rm -f $(C_PLUGIN_OBJECT_FILES) $(CP_PLUGIN_OBJECT_FILES) \
		$(C_PLUGIN_NAME).so $(CP_PLUGIN_NAME).so c-common.h.stamp *~


