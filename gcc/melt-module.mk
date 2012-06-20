## file melt-module.mk
## a -*- Makefile -*- for GNU make. 

# Copyright (C) 2009, 2011, 2012 Free Software Foundation, Inc.
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
### with descriptive file srcdir/foo+meltdesc.c & timestamp file
### srcdir/foo+melttime.h, perhaps from melt-runtime.c, with
#### make -f melt-module.mk \
####    GCCMELT_MODULE_SOURCEBASE=srcdir/foo \
####    GCCMELT_MODULE_BINARYBASE=moduledir/foo \
####    GCCMELT_MODULE_WORKSPACE=/tmp \
####    GCCMELT_MODULE_FLAVOR=quicklybuilt
#### to make moduledir/foo.so 

.PHONY: melt_module melt_workspace
### See also routine compile_gencsrc_to_binmodule of melt-runtime.c


## we export all make variables 
export

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

# the extra libraries or link flags for generated MELT modules;
# Advanced MELT users might change that...
GCCMELT_MODULE_EXTRALIBES ?= 

# check that we do have a melt descriptive source file
GCCMELT_DESC:= $(wildcard $(GCCMELT_MODULE_SOURCEBASE)+meltdesc.c)
ifeq ($(GCCMELT_DESC),)
$(error MELT descriptive file $(GCCMELT_MODULE_SOURCEBASE)+meltdesc.c does not exist)
endif

# warn if the melt time stamp file is missing
GCCMELT_TIMEF:= $(wildcard $(GCCMELT_MODULE_SOURCEBASE)+melttime.h)
ifeq ($(GCCMELT_TIMEF),)
$(warning MELT time stamp file $(GCCMELT_MODULE_SOURCEBASE)+melttime.h does not exist)
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

## the compiler with which MELT, and the MELT modules, are used
ifndef MELTGCC
MELTGCC = $(or $(CC),gcc)
endif

### the auto-host.h for the MELTGCC compiler
ifndef MELT_AUTOHOST_H
MELT_AUTOHOST_H = $(or $(wildcard auto-host.h),$(shell $(MELTGCC) -print-file-name=plugin/include/auto-host.h))
endif

## gives yes if MELTGCC has been built with C++ or else the empty
## string.  Notice that the auto-host.h file can either be from
## plugin, or from the current directory when building the MELT
## branch.
ifndef MELTGCC_BUILD_WITH_CXX
MELTGCC_BUILD_WITH_CXX = $(shell grep -q -s 'define[[:space:]]\+ENABLE_BUILD_WITH_CXX[[:space:]]\+1' $(MELT_AUTOHOST_H)\
   && echo yes)
endif

## The compiler and flags used to compile MELT generated code.  For a
## melt plugin to GCC 4.7 or later, that could be a C++ compiler! eg
## make MELTGCC=gcc-4.7 GCCMELT_CC=g++-4.7 hence we add a test if
## $(MELTGCC) was built with C++ or with C
ifndef GCCMELT_CC
ifeq ($(strip $(MELTGCC_BUILD_WITH_CXX)),)
GCCMELT_CC = $(or $(CC),gcc) -Wc++-compat
else
GCCMELT_CC = $(or $(CXX),g++)
endif
endif

GCCMELT_BASE=$(notdir $(basename $(GCCMELT_MODULE_SOURCEBASE)))
GCCMELT_SOURCEDIR=$(dir $(GCCMELT_MODULE_SOURCEBASE))

## FIXME: this probably should be autoconf-ed...
ifeq ($(shell uname),Darwin)
SHARED_LIBRARY_FLAGS=-bundle -undefined dynamic_lookup
else
SHARED_LIBRARY_FLAGS=-shared
endif

## these flags should only affect the quality of the generated
## binaries. No preprocessor flags please here!
GCCMELT_OPTIMIZED_FLAGS ?= -O1
#could add -Wc++-compat in GCCMELT_QUICKLYBUILT_FLAGS below
GCCMELT_QUICKLYBUILT_FLAGS ?= -O0 -Wall
GCCMELT_DYNAMIC_FLAGS ?= -O0
GCCMELT_DEBUGNOLINE_FLAGS ?= -g
GCCMELT_DESCRIPTOR_FLAGS ?= -O

## the primary C file
GCCMELT_PRIMARY := $(wildcard $(GCCMELT_MODULE_SOURCEBASE).c)
ifeq ($(GCCMELT_PRIMARY),)
$(error Did not found MELT primary file  $(GCCMELT_MODULE_SOURCEBASE).c)
endif

## the md5 checksum of the primary file
GCCMELT_PRIMARYMD5:= $(shell $(MD5SUM) $(GCCMELT_PRIMARY) | $(GAWK) '{print $$1}')

## the secondary C files, in order
GCCMELT_SECONDARY_CFILES= $(sort $(wildcard $(GCCMELT_MODULE_SOURCEBASE)+[09]*.c))
## the corresponding checksums
GCCMELT_SECONDARY_MD5SUMS:= $(if $(GCCMELT_SECONDARY_CFILES), \
		$(shell  $(MD5SUM) $(GCCMELT_SECONDARY_CFILES)  | $(GAWK) '{print $$1}'))

## the cumulated checksum
GCCMELT_CUMULATED_MD5 ?= $(shell cat $(GCCMELT_PRIMARY)  $(GCCMELT_SECONDARY_CFILES) | $(MD5SUM)  | $(GAWK) '{print $$1}')

## the primary object basename
GCCMELT_PRIMARY_MDSUMED_BASE:= $(basename $(notdir $(GCCMELT_PRIMARY))).$(GCCMELT_PRIMARYMD5)

## the secondary objects basenames
GCCMELT_SECONDARY_MDSUMED_BASES := $(join $(basename $(notdir $(GCCMELT_SECONDARY_CFILES))),$(addprefix .,$(GCCMELT_SECONDARY_MD5SUMS)))


################################################################
## if available, include the melt generated make fragment
GCCMELTGEN_BUILD=$(GCCMELT_MODULE_WORKSPACE)/
-include $(GCCMELT_MODULE_SOURCEBASE)+meltbuild.mk

################
## rules for meltpic.o  object files
## quicklybuilt flavor
$(GCCMELTGEN_BUILD)%.quicklybuilt.meltpic.o:
	@echo @+@melt-module quicklybuilt.meltpic at= $@ inf= $< question= $? caret= $^ realpathinf= $(realpath $<)
	if [ -z "$(filter %.mdsumed.c, $(realpath $<))" ]; then \
	   $(GCCMELT_CC) -DMELTGCC_MODULE_QUICKLYBUILT -DMELT_HAVE_DEBUG=1 \
              $(GCCMELT_QUICKLYBUILT_FLAGS) $(GCCMELT_CFLAGS) \
	        -fPIC -c -o $@ $< ; \
	else \
          GCCMELTGENMDSUMEDPIC=$(GCCMELTGEN_BUILD)$(notdir $(patsubst %.mdsumed.c,%.quicklybuilt.meltmdsumedpic.o,$(realpath $<))); \
	  echo  @+@melt-module quicklybuilt.meltpicmd GCCMELTGENMDSUMEDPIC=  $$GCCMELTGENMDSUMEDPIC ; \
          $(MAKE) -e -f $(filter %melt-module.mk, $(MAKEFILE_LIST)) $$GCCMELTGENMDSUMEDPIC ;  \
          $(LN_S) -v -f `realpath $$GCCMELTGENMDSUMEDPIC` $@ ; fi

$(GCCMELTGEN_BUILD)%.quicklybuilt.meltmdsumedpic.o: $(GCCMELTGEN_BUILD)%.mdsumed.c
	@echo @+@melt-module quicklybuilt.meltmdsumedpic at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -DMELTGCC_MODULE_QUICKLYBUILT  -DMELT_HAVE_DEBUG=1 \
           $(GCCMELT_QUICKLYBUILT_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@ $<

## optimized flavor
$(GCCMELTGEN_BUILD)%.optimized.meltpic.o:
	@echo @+@melt-module optimized.meltpic at= $@ inf= $< question= $? caret= $^ realpathinf= $(realpath $<)
	if [ -z "$(filter %.mdsumed.c, $(realpath $<))" ]; then \
	   $(GCCMELT_CC) -DMELTGCC_MODULE_OPTIMIZED -DMELT_HAVE_DEBUG=0 \
              $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) \
	        -fPIC -c -o $@ $< ; \
	else \
          GCCMELTGENMDSUMEDPIC=$(GCCMELTGEN_BUILD)$(notdir $(patsubst %.mdsumed.c,%.optimized.meltmdsumedpic.o,$(realpath $<))); \
	  echo  @+@melt-module optimized.meltpicmd GCCMELTGENMDSUMEDPIC=  $$GCCMELTGENMDSUMEDPIC ; \
          $(MAKE) -e -f $(filter %melt-module.mk, $(MAKEFILE_LIST)) $$GCCMELTGENMDSUMEDPIC ;  \
          $(LN_S) -v -f `realpath $$GCCMELTGENMDSUMEDPIC` $@ ; fi

$(GCCMELTGEN_BUILD)%.optimized.meltmdsumedpic.o: $(GCCMELTGEN_BUILD)%.mdsumed.c
	@echo @+@melt-module optimized.meltmdsumedpic at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -DMELTGCC_MODULE_OPTIMIZED  -DMELT_HAVE_DEBUG=0 \
           $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@ $<

## debugnoline flavor
$(GCCMELTGEN_BUILD)%.debugnoline.meltpic.o:
	@echo @+@melt-module debugnoline.meltpic at= $@ inf= $< question= $? caret= $^ realpathinf= $(realpath $<)
	if [ -z "$(filter %.mdsumed.c, $(realpath $<))" ]; then \
	   $(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE  -DMELTGCC_NOLINENUMBERING -DMELT_HAVE_DEBUG=1 \
              $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) \
	        -fPIC -c -o $@ $< ; \
	else \
          GCCMELTGENMDSUMEDPIC=$(GCCMELTGEN_BUILD)$(notdir $(patsubst %.mdsumed.c,%.debugnoline.meltmdsumedpic.o,$(realpath $<))); \
	  echo  @+@melt-module debugnoline.meltpicmd GCCMELTGENMDSUMEDPIC=  $$GCCMELTGENMDSUMEDPIC ; \
          $(MAKE) -e -f $(filter %melt-module.mk, $(MAKEFILE_LIST)) $$GCCMELTGENMDSUMEDPIC ;  \
          $(LN_S) -v -f `realpath $$GCCMELTGENMDSUMEDPIC` $@ ; fi

$(GCCMELTGEN_BUILD)%.debugnoline.meltmdsumedpic.o: $(GCCMELTGEN_BUILD)%.mdsumed.c
	@echo @+@melt-module debugnoline.meltmdsumedpic at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE   -DMELTGCC_NOLINENUMBERING -DMELT_HAVE_DEBUG=1 \
           $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@ $<


## dynamic flavor
$(GCCMELTGEN_BUILD)%.dynamic.meltpic.o:
	@echo @+@melt-module dynamic.meltpic at= $@ inf= $< question= $? caret= $^ realpathinf= $(realpath $<)
	if [ -z "$(filter %.mdsumed.c, $(realpath $<))" ]; then \
	   $(GCCMELT_CC) -DMELTGCC_MODULE_DYNAMIC  -DMELTGCC_NOLINENUMBERING -DMELT_HAVE_DEBUG=1  -DMELTGCC_DYNAMIC_OBJSTRUCT \
              $(GCCMELT_DYNAMIC_FLAGS) $(GCCMELT_CFLAGS) \
	        -fPIC -c -o $@ $< ; \
	else \
          GCCMELTGENMDSUMEDPIC=$(GCCMELTGEN_BUILD)$(notdir $(patsubst %.mdsumed.c,%.dynamic.meltmdsumedpic.o,$(realpath $<))); \
	  echo  @+@melt-module dynamic.meltpicmd GCCMELTGENMDSUMEDPIC=  $$GCCMELTGENMDSUMEDPIC ; \
          $(MAKE) -e -f $(filter %melt-module.mk, $(MAKEFILE_LIST)) $$GCCMELTGENMDSUMEDPIC ;  \
          $(LN_S) -v -f `realpath $$GCCMELTGENMDSUMEDPIC` $@ ; fi

$(GCCMELTGEN_BUILD)%.dynamic.meltmdsumedpic.o: $(GCCMELTGEN_BUILD)%.mdsumed.c
	@echo @+@melt-module dynamic.meltmdsumedpic at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -DMELTGCC_MODULE_DYNAMIC   -DMELTGCC_NOLINENUMBERING -DMELT_HAVE_DEBUG=1  -DMELTGCC_DYNAMIC_OBJSTRUCT \
           $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@ $<




## descriptor quasi-flavor, never symlinked!
$(GCCMELTGEN_BUILD)%.descr.meltpic.o:
	@echo @+@melt-module descriptor.meltpic at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -DMELTGCC_MODULE_DESCRIPTOR  \
          $(GCCMELT_DESCRIPTOR_FLAGS) $(GCCMELT_CFLAGS)  \
	   -fPIC -c -o $@ $<


################
## rules for meltmod.so shared objects
$(GCCMELTGEN_BUILD)%.quicklybuilt.meltmod.so:
	@echo @+@melt-module quicklybuilt.meltmod at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -o $@ \
          $(GCCMELT_QUICKLYBUILT_FLAGS) $(GCCMELT_CFLAGS) \
          -shared $^

$(GCCMELTGEN_BUILD)%.optimized.meltmod.so:
	@echo @+@melt-module optimized.meltmod at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -o $@ \
          $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) \
          -shared $^

$(GCCMELTGEN_BUILD)%.debugnoline.meltmod.so:
	@echo @+@melt-module debugnoline.meltmod at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -o $@ \
          $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) \
          -shared $^

$(GCCMELTGEN_BUILD)%.dynamic.meltmod.so:
	@echo @+@melt-module dynamic.meltmod at= $@ inf= $< question= $? caret= $^
	$(GCCMELT_CC) -o $@ \
          $(GCCMELT_DYNAMIC_FLAGS) $(GCCMELT_CFLAGS) \
          -shared $^

################################################################
vpath %.optimized.pic.o $(GCCMELT_MODULE_WORKSPACE)
$(GCCMELT_MODULE_WORKSPACE)/%.optimized.pic.o: 
	$(GCCMELT_CC) -DMELTGCC_MODULE_OPTIMIZED  -DMELT_HAVE_DEBUG=0  $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@  $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))
##bad$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).optimized.pic.o:  $(GCCMELT_MODULE_SOURCEBASE).c
##bad	@echo melt-module optimized pic at= $@ inf= $<
##bad	$(GCCMELT_CC) -DMELTGCC_MODULE_OPTIMIZED  -DMELT_HAVE_DEBUG=0  $(GCCMELT_OPTIMIZED_FLAGS) \
##bad	   $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<

vpath %.quicklybuilt.pic.o $(GCCMELT_MODULE_WORKSPACE)
$(GCCMELT_MODULE_WORKSPACE)/%.quicklybuilt.pic.o: 
	$(GCCMELT_CC) -DMELTGCC_MODULE_QUICKLYBUILT -DMELT_HAVE_DEBUG=1 $(GCCMELT_QUICKLYBUILT_FLAGS) $(GCCMELT_CFLAGS) \
	  -fPIC -c -o $@ $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))
##bad  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).quicklybuilt.pic.o: $(GCCMELT_MODULE_SOURCEBASE).c
##bad  	@echo melt-module quicklybuilt pic at= $@ inf= $<
##bad  	$(GCCMELT_CC) -DMELTGCC_MODULE_QUICKLYBUILT -DMELT_HAVE_DEBUG=1 $(GCCMELT_QUICKLYBUILT_FLAGS) \
##bad  	    $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<

vpath %.debugnoline.pic.o $(GCCMELT_MODULE_WORKSPACE)
$(GCCMELT_MODULE_WORKSPACE)/%.debugnoline.pic.o: 
	$(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE -DMELTGCC_NOLINENUMBERING -DMELT_HAVE_DEBUG=1  $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@  $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))
##bad  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).debugnoline.pic.o: $(GCCMELT_MODULE_SOURCEBASE).c
##bad  	@echo melt-module primary debugnoline pic at= $@ inf= $<
##bad  	$(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE -DMELTGCC_NOLINENUMBERING -DMELT_HAVE_DEBUG=1  $(GCCMELT_DEBUGNOLINE_FLAGS) \
##bad  	    $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<

vpath %.dynamic.pic.o $(GCCMELT_MODULE_WORKSPACE)
$(GCCMELT_MODULE_WORKSPACE)/%.dynamic.pic.o: 
	$(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE  -DMELT_HAVE_DEBUG=1  -DMELTGCC_DYNAMIC_OBJSTRUCT $(GCCMELT_DYNAMIC_FLAGS) $(GCCMELT_CFLAGS) \
	   -fPIC -c -o $@ $(patsubst %, $(GCCMELT_SOURCEDIR)%.c, $(basename $(basename $(basename $(basename $(notdir $@))))))
##bad  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).dynamic.pic.o: $(GCCMELT_MODULE_SOURCEBASE).c
##bad  	@echo melt-module primary dynamic pic at= $@ inf= $<
##bad  	$(GCCMELT_CC) -DMELTGCC_MODULE_DEBUGNOLINE  -DMELT_HAVE_DEBUG=1  -DMELTGCC_DYNAMIC_OBJSTRUCT $(GCCMELT_DYNAMIC_FLAGS) \
##bad  	    $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<



################
$(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o: $(GCCMELT_DESC) $(GCCMELT_TIMEF)
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	$(GCCMELT_CC) -DMELTGCC_MODULE_DESCRFILE $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<


$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).optimized.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).optimized.pic.o \
  $(addsuffix .optimized.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE)  $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARY_MDSUMED_BASES))) \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)
	$(MAKE)  -f  $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) -fPIC $(SHARED_LIBRARY_FLAGS) -o $@  $^ $(GCCMELT_MODULE_EXTRALIBES)


$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).dynamic.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).dynamic.pic.o \
  $(addsuffix .dynamic.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE) $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARY_MDSUMED_BASES))) \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o
	$(MAKE) -f $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_DYNAMIC_FLAGS) $(GCCMELT_CFLAGS) -fPIC $(SHARED_LIBRARY_FLAGS) -o $@  $^  $(GCCMELT_MODULE_EXTRALIBES)


$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).quicklybuilt.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).quicklybuilt.pic.o \
  $(addsuffix .quicklybuilt.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE) $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARY_MDSUMED_BASES))) \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o
	$(MAKE) -f  $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_QUICKLYBUILT_FLAGS) $(GCCMELT_CFLAGS) -fPIC $(SHARED_LIBRARY_FLAGS) -o $@ $^  $(GCCMELT_MODULE_EXTRALIBES)


$(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).debugnoline.so: \
  $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE).debugnoline.pic.o \
  $(addsuffix .debugnoline.pic.o, $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_PRIMARY_MDSUMED_BASE) $(patsubst %,$(GCCMELT_MODULE_WORKSPACE)/%,$(GCCMELT_SECONDARY_MDSUMED_BASES))) \
  $(GCCMELT_MODULE_WORKSPACE)/$(basename $(notdir $(GCCMELT_DESC))).$(GCCMELT_CUMULATED_MD5).pic.o
	$(MAKE) -f $(filter %melt-module.mk, $(MAKEFILE_LIST))  $^ \
	   GCCMELT_MODULE_SOURCEBASE=$(GCCMELT_MODULE_SOURCEBASE) \
	   GCCMELT_MODULE_BINARYBASE=$(GCCMELT_MODULE_BINARYBASE) \
	   GCCMELT_MODULE_WORKSPACE=$(GCCMELT_MODULE_WORKSPACE) \
	   GCCMELT_MODULE_FLAVOR=$(GCCMELT_MODULE_FLAVOR) \
	   GCCMELT_CUMULATED_MD5=$(GCCMELT_CUMULATED_MD5)
	$(GCCMELT_CC) $(GCCMELT_DEBUGNOLINE_FLAGS) $(GCCMELT_CFLAGS) -fPIC $(SHARED_LIBRARY_FLAGS) -o $@  $^  $(GCCMELT_MODULE_EXTRALIBES)



################################################################
ifdef MELTGEN_MODULENAME
$(GCCMELT_MODULE_BINARYBASE).$(GCCMELT_MODULE_FLAVOR).so: \
  $(GCCMELTGEN_BUILD)$(notdir $(MELTGEN_MODULENAME)).$(GCCMELT_CUMULATED_MD5).$(GCCMELT_MODULE_FLAVOR).meltmod.so
	@echo @-@melt-module with MELTGEN_MODULENAME= $(MELTGEN_MODULENAME) inf= $< at= $@
## the melt-runtime.c requires the link with the cumulated md5
	$(LN_S) -v -f $(realpath $<) $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).$(GCCMELT_MODULE_FLAVOR).so
	$(LN_S) -v -f $(realpath $<) $@

else
$(GCCMELT_MODULE_BINARYBASE).$(GCCMELT_MODULE_FLAVOR).so: $(GCCMELT_MODULE_WORKSPACE)/$(GCCMELT_BASE).$(GCCMELT_CUMULATED_MD5).$(GCCMELT_MODULE_FLAVOR).so
	echo @+@melt-module inf=  without MELTGEN_MODULENAME $< at= $@
	$(LN_S) -v -f $(realpath $<) $@

endif

melt_module: melt_workspace $(GCCMELT_MODULE_BINARYBASE).$(GCCMELT_MODULE_FLAVOR).so

melt_workspace: 
	@echo @+@melt-module workspace $(GCCMELT_MODULE_WORKSPACE)  GCCMELT_PRIMARY_MDSUMED_BASE= $(GCCMELT_PRIMARY_MDSUMED_BASE) GCCMELT_SECONDARY_MDSUMED_BASES= $(GCCMELT_SECONDARY_MDSUMED_BASES) GCCMELT_MODULE_SOURCEBASE= $(GCCMELT_MODULE_SOURCEBASE)
	[ -d $(GCCMELT_MODULE_WORKSPACE) ] || mkdir -p $(GCCMELT_MODULE_WORKSPACE)

## eof melt-module.mk

