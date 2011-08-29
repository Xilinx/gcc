## file melt-module.mk
## a -*- Makefile -*- for GNU make. 

# Copyright (C) 2009, 2011 Free Software Foundation, Inc.
# Contributed by Basile Starynkevitch  <basile@starynkevitch.net>
# This file is part of GCC.

# GCC is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.

# GCC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with GCC; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

### should be invoked to compile MELT generated C files srcdir/foo*.c
### with descriptive file srcdir/foo+meltdesc.c, perhaps from
### melt-runtime.c, with
#### make -f melt-module.mk \
####    GCCMELT_MODULE_SOURCEBASE=srcdir/foo \
####    GCCMELT_MODULE_BINARYBASE=moduledir/foo \
####    GCCMELT_MODULE_WORKSPACE=/tmp \
####    GCCMELT_MODULE_FLAVOR=quicklybuilt
#### to make moduledir/foo.so 

.PHONY: melt_module
### See also routine compile_gencsrc_to_binmodule of melt-runtime.c

ifndef GCCMELT_MODULE_SOURCEBASE
$(error GCCMELT_MODULE_SOURCEBASE not defined)
endif

ifndef GCCMELT_MODULE_BINARYBASE
$(error GCCMELT_MODULE_BINARYBASE not defined)
endif

# the workspace is the directory where files are built
ifndef GCCMELT_MODULE_WORKSPACE
GCCMELT_MODULE_WORKSPACE=.
endif

# the flavor is 'optimized' or 'quicklybuilt' or 'debugnoline'
ifndef GCCMELT_MODULE_FLAVOR
$(warning GCCMELT_MODULE_FLAVOR default to optimized)
GCCMELT_MODULE_FLAVOR=optimized
endif

# check that we do have a melt descriptive source file
GCCMELT_DESC:= $(wildcard $(GCCMELT_MODULE_SOURCEBASE)+meltdesc.c)
ifeq ($(GCCMELT_DESC),)
$(error MELT descriptive file $(GCCMELT_MODULE_SOURCEBASE)+meltdesc.c does not exist)
endif

## check the flavor
ifeq ($(findstring $(GCCMELT_MODULE_FLAVOR),optimized quicklybuilt debugnoline dynamic),)
$(error invalid GCCMELT_MODULE_FLAVOR $(GCCMELT_MODULE_FLAVOR))
endif

### GNU awk is required
ifndef GAWK
GAWK=gawk
endif

## The md5sum command is required
MD5SUM ?= md5sum

ifndef LN_S
LN_S=ln -s
endif

## The compiler used to compile MELT C code
GCCMELT_CC ?= $(firstword $(GCC) gcc)

GCCMELT_BASE=$(notdir $(basename $(GCCMELT_MODULE_SOURCEBASE)))
GCCMELT_SOURCEDIR=$(dir $(GCCMELT_MODULE_SOURCEBASE))

## these flags should only affect the quality of the generated
## binaries. No preprocessor flags please here!
GCCMELT_OPTIMIZED_FLAGS ?= -O1
GCCMELT_QUICKLYBUILT_FLAGS ?= -O0 
GCCMELT_DYNAMIC_FLAGS ?= -O0
GCCMELT_DEBUGNOLINE_FLAGS ?= -g

## the primary C file
GCCMELT_PRIMARY := $(wildcard $(GCCMELT_MODULE_SOURCEBASE).c)
ifeq ($(GCCMELT_PRIMARY),)
$(error Did not found MELT primary file  $(GCCMELT_MODULE_SOURCEBASE).c)
endif

## the md5 checksum of the primary file
GCCMELT_PRIMARYMD5:= $(shell $(MD5SUM) $(GCCMELT_PRIMARY) | $(GAWK) '{print $$1}')

## the secondary C files, in order
GCCMELT_SECONDARIES= $(sort $(wildcard $(GCCMELT_MODULE_SOURCEBASE)+[09]*.c))
## the corresponding checksums
GCCMELT_SECONDARIES_MD5:= $(if $(GCCMELT_SECONDARIES), \
		$(shell  $(MD5SUM) $(GCCMELT_SECONDARIES)  | $(GAWK) '{print $$1}'))

## the cumulated checksum
GCCMELT_CUMULATED_MD5 ?= $(shell cat $(GCCMELT_PRIMARY)  $(GCCMELT_SECONDARIES) | $(MD5SUM)  | $(GAWK) '{print $$1}')

## the primary object basename
GCCMELT_PRIMARY_OBJ:= $(basename $(notdir $(GCCMELT_PRIMARY))).$(GCCMELT_PRIMARYMD5)

## the secondary objects basenames
GCCMELT_SECONDARIES_OBJ := $(join $(basename $(notdir $(GCCMELT_SECONDARIES))),$(addprefix .,$(GCCMELT_SECONDARIES_MD5)))

vpath %.optimized.pic.o $(GCCMELT_MODULE_WORKSPACE)
$(GCCMELT_MODULE_WORKSPACE)/%.optimized.pic.o: 
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	echo optimized pic at $@  \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
#	echo optimized base1name at  $(basename $@)
#	echo optimized base2name at  $(basename $(basename $@))
#	echo optimized base3name at  $(basename $(basename $(basename $@)))
#	echo optimized base4name at  $(basename $(basename $(basename $(basename $@))))
	$(GCCMELT_CC) -DMELTGCC_MODULE_OPTIMIZED  -DMELT_HAVE_DEBUG=0  $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@  $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))

vpath %.quicklybuilt.pic.o $(GCCMELT_MODULE_WORKSPACE)
$(GCCMELT_MODULE_WORKSPACE)/%.quicklybuilt.pic.o: 
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	echo quicklybuilt pic at $@ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
#	echo quicklybuilt base1name at  $(basename $@)
#	echo quicklybuilt base2name at  $(basename $(basename $@))
#	echo quicklybuilt base3name at  $(basename $(basename $(basename $@)))
#	echo quicklybuilt base4name at  $(basename $(basename $(basename $(basename $@))))
	$(GCCMELT_CC) -DMELTGCC_MODULE_QUICKLYBUILT -DMELT_HAVE_DEBUG=1 $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) \
	  -fPIC -c -o $@ $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))

vpath %.debugnoline.pic.o $(GCCMELT_MODULE_WORKSPACE)
$(GCCMELT_MODULE_WORKSPACE)/%.debugnoline.pic.o: 
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	echo debugnoline pic at $@  \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE  -DMELT_HAVE_DEBUG=1  $(GCCMELT_QUICKLYBUILT_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@  $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))

vpath %.dynamic.pic.o $(GCCMELT_MODULE_WORKSPACE)
 $(GCCMELT_MODULE_WORKSPACE)/%.dynamic.pic.o: 
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	echo dynamic pic at $@  \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
#	echo dynamic base1name at  $(basename $@)
#	echo dynamic base2name at  $(basename $(basename $@))
#	echo dynamic base3name at  $(basename $(basename $(basename $@)))
#	echo dynamic base4name at  $(basename $(basename $(basename $(basename $@))))
	$(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE  -DMELT_HAVE_DEBUG=1  -DMELTGCC_DYNAMIC_OBJSTRUCT $(GCCMELT_DYNAMIC_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@ $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))

$(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o: $(GCCMELT_DESC)
	echo descfile at $@ caret $^ GCCMELT_DESC=$(GCCMELT_DESC)
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	$(GCCMELT_CC) -DMELTGCC_MODULE_DESCRFILE $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<

$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).optimized.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ).optimized.pic.o \
  $(addsuffix .optimized.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ)  $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARIES_OBJ)))
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	echo optimized so at $@ caret $^ makefilelist $(MAKEFILE_LIST) \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(MAKE)  -f  $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) -fPIC -shared -o $@ $^

$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).dynamic.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ).dynamic.pic.o \
  $(addsuffix .dynamic.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ) $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARIES_OBJ)))
	echo dynamic so at $@ caret $^  makefilelist $(MAKEFILE_LIST) \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(MAKE) -f $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_DYNAMIC_FLAGS) $(GCCMELT_CFLAGS) -fPIC -shared -o $@ $^

$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).quicklybuilt.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ).quicklybuilt.pic.o \
  $(addsuffix .quicklybuilt.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ) $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARIES_OBJ)))
	echo quicklybuilt so at $@ caret $^ makefilelist $(MAKEFILE_LIST)  \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(MAKE) -f  $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_QUICKLYBUILT_FLAGS) $(GCCMELT_CFLAGS) -fPIC -shared -o $@ $^


$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).debugnoline.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ).debugnoline.pic.o \
  $(addsuffix .debugnoline.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_OBJ) $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARIES_OBJ)))
	echo debugnoline so at $@ caret $^ makefilelist $(MAKEFILE_LIST) \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(MAKE) -f $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) -fPIC -shared -o $@ $^

melt_module: $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).$(GCCMELT_MODULE_FLAVOR).so
	$(RM) $(GCCMELT_MODULE_BINARYBASE).$(GCCMELT_MODULE_FLAVOR).so
	$(LN_S) $(realpath $<)  $(GCCMELT_MODULE_BINARYBASE).$(GCCMELT_MODULE_FLAVOR).so



## eof melt-module.mk

