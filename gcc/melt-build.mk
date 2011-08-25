
# melt-build.mk is generated from melt-build.tpl by 'autogen melt-build.def'
# DON'T EDIT melt-build.mk but only edit: melt-build.tpl or melt-build.def
#
# Makefile fragment for MELT modules and MELT translator bootstrap.
#   Copyright (C) 2010,2011  Free Software Foundation
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

## the following Makefile variables are expected to be set
### melt_source_dir - directory containing *.melt (& corresponding *.c) files
### melt_module_dir - directory containing *.so MELT module files
### melt_make_module_makefile - our melt-module.mk Makefile  when making MELT
### melt_make_source_dir - directory containing the *.melt files when making MELT
### melt_make_module_dir - directory containing the *.so files when making MELT
### melt_default_modules_list - basename of the default module list
### melt_make_cc1 - cc1 program with MELT (or loading MELT plugin) (better use gcc -c rather
### than a direct call to cc1 when compiling as a plugin) or gcc -fplugin=melt.so
### melt_make_cc1_dependency - the make dependency for above i.e. cc1$(exeext) for MELT branch
### (usually, melt_make_cc1_dependency = melt_make_cc1)
### melt_make_gencdeps is an extra make dependency of generated C files [leave empty usually]
### melt_is_plugin - should be non empty in plugin mode
### melt_make_move - a move if change command for files
### melt_cflags - the CFLAGS for compiling MELT generated C code
### melt_xtra_cflags - the CFLAGS for compiling extra (applicative)
###                    MELT generated C code
### melt_default_variant can be quicklybuilt | optimized | debugnoline

melt_default_variant ?= optimized

## LN_S might not be defined, e.g. from MELT-Plugin-Makefile
ifndef LN_S
LN_S= ln -sv
endif

## GAWK is needed, the GNU awk
ifndef GAWK
GAWK=gawk
endif

## the various arguments to MELT - avoid spaces in them!
meltarg_mode=$(if $(melt_is_plugin),-fplugin-arg-melt-mode,-fmelt-mode)
meltarg_init=$(if $(melt_is_plugin),-fplugin-arg-melt-init,-fmelt-init)
meltarg_module_path=$(if $(melt_is_plugin),-fplugin-arg-melt-module-path,-fmelt-module-path)
meltarg_source_path=$(if $(melt_is_plugin),-fplugin-arg-melt-source-path,-fmelt-source-path)
meltarg_tempdir=$(if $(melt_is_plugin),-fplugin-arg-melt-tempdir,-fmelt-tempdir)

meltarg_workdir=$(if $(melt_is_plugin),-fplugin-arg-melt-workdir,-fmelt-workdir)

meltarg_arg=$(if $(melt_is_plugin),-fplugin-arg-melt-arg,-fmelt-arg)
meltarg_bootstrapping=$(if $(melt_is_plugin),-fplugin-arg-melt-bootstrapping,-fmelt-bootstrapping)
meltarg_makefile=$(if $(melt_is_plugin),-fplugin-arg-melt-module-makefile,-fmelt-module-makefile)
meltarg_makecmd=$(if $(melt_is_plugin),-fplugin-arg-melt-module-make-command,-fmelt-module-make-command)
meltarg_arglist=$(if $(melt_is_plugin),-fplugin-arg-melt-arglist,-fmelt-arglist)
meltarg_output=$(if $(melt_is_plugin),-fplugin-arg-melt-output,-fmelt-output)
meltarg_modulecflags=$(if $(melt_is_plugin),-fplugin-arg-melt-module-cflags,-fmelt-module-cflags)
meltarg_inhibitautobuild=$(if $(melt_is_plugin),-fplugin-arg-melt-inhibit-auto-build,-fmelt-inhibit-auto-build)
## MELT_DEBUG could be set to -fmelt-debug or -fplugin-arg-melt-debug
## the invocation to translate the very first initial MELT file
MELTCCINIT1ARGS= $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translateinit  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
              "$(meltarg_modulecflags)='$(melt_cflags)'" \
	      $(meltarg_tempdir)=. $(meltarg_bootstrapping) $(MELT_DEBUG)

## the invocation to translate the other files
MELTCCFILE1ARGS=  $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
              "$(meltarg_modulecflags)='$(melt_cflags)'" \
	      $(meltarg_tempdir)=. $(meltarg_bootstrapping)  $(MELT_DEBUG)

## the invocation to translate the application files -don't pass the -fmelt-bootstrap flag
MELTCCAPPLICATION1ARGS=  $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
              "$(meltarg_modulecflags)='$(melt_cflags)'" \
	      $(meltarg_tempdir)=. $(MELT_DEBUG)
MELTCCAPPLICATION1=$(melt_make_cc1) $(MELTCCAPPLICATION1ARGS)

vpath %.so $(melt_make_module_dir) . 
#vpath %.c $(melt_make_source_dir)/generated . $(melt_source_dir)
vpath %.h . $(melt_make_source_dir)/generated $(melt_source_dir)
vpath %.melt $(melt_make_source_dir) . $(melt_source_dir)

##
## the invoking command could set MELT_MAKE_MODULE_XTRAMAKEFLAGS=-j2
##always prefix $(MELT_MAKE_MODULE) with a + in this file.
MELT_MAKE_MODULE=$(MAKE) -f $(melt_make_module_makefile) $(MELT_MAKE_MODULE_XTRAMAKEFLAGS) VPATH=$(VPATH):.

## The base name of the MELT translator files
MELT_TRANSLATOR_BASE= \
    warmelt-first \
  warmelt-base \
  warmelt-debug \
  warmelt-macro \
  warmelt-normal \
  warmelt-normatch \
  warmelt-genobj \
  warmelt-outobj

## the MELT translator MELT source files
MELT_TRANSLATOR_SOURCE= $(patsubst %,$(melt_make_source_dir)/%.melt,$(MELT_TRANSLATOR_BASE))

## The base name of the MELT application files
MELT_APPLICATION_BASE= \
    xtramelt-parse-infix-syntax \
  xtramelt-ana-base \
  xtramelt-ana-simple \
  xtramelt-c-generator \
  xtramelt-opengpu

## The MELT application source files
MELT_APPLICATION_SOURCE= $(patsubst %,$(melt_make_source_dir)/%.melt,$(MELT_APPLICATION_BASE))

## The cold stage 0 of the translator

MELT_GENERATED_FIRST_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-first.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-first+*.c)
MELT_GENERATED_FIRST_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_FIRST_C_FILES)))

MELT_GENERATED_FIRST_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-first+meltdesc.c) 

MELT_GENERATED_BASE_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-base.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-base+*.c)
MELT_GENERATED_BASE_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_BASE_C_FILES)))

MELT_GENERATED_BASE_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-base+meltdesc.c) 

MELT_GENERATED_DEBUG_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-debug.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-debug+*.c)
MELT_GENERATED_DEBUG_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_DEBUG_C_FILES)))

MELT_GENERATED_DEBUG_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-debug+meltdesc.c) 

MELT_GENERATED_MACRO_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-macro.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-macro+*.c)
MELT_GENERATED_MACRO_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_MACRO_C_FILES)))

MELT_GENERATED_MACRO_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-macro+meltdesc.c) 

MELT_GENERATED_NORMAL_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-normal.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-normal+*.c)
MELT_GENERATED_NORMAL_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_NORMAL_C_FILES)))

MELT_GENERATED_NORMAL_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-normal+meltdesc.c) 

MELT_GENERATED_NORMATCH_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-normatch.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-normatch+*.c)
MELT_GENERATED_NORMATCH_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_NORMATCH_C_FILES)))

MELT_GENERATED_NORMATCH_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-normatch+meltdesc.c) 

MELT_GENERATED_GENOBJ_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-genobj.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-genobj+*.c)
MELT_GENERATED_GENOBJ_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_GENOBJ_C_FILES)))

MELT_GENERATED_GENOBJ_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-genobj+meltdesc.c) 

MELT_GENERATED_OUTOBJ_C_FILES= \
                  $(realpath $(melt_make_source_dir))/generated/warmelt-outobj.c \
                  $(wildcard $(realpath $(melt_make_source_dir))/generated/warmelt-outobj+*.c)
MELT_GENERATED_OUTOBJ_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_OUTOBJ_C_FILES)))

MELT_GENERATED_OUTOBJ_CUMULMD5 := $(shell $(GAWK) -F\" '/melt_cumulated_hexmd5/{print $$2}' $(melt_make_source_dir)/generated/warmelt-outobj+meltdesc.c) 


melt-workdir:
	mkdir -p melt-workdir
## the rules to build the static and dynamic version of stage0, that
## is with static or dynamic field object offsets


## for STAGE0 in warmelt-first use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-first
melt-stage0-quicklybuilt/warmelt-first.$(MELT_GENERATED_FIRST_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_FIRST_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-first MELT_GENERATED_FIRST_CUMULMD5= $(MELT_GENERATED_FIRST_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-first \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_FIRST_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-first

melt-stage0-quicklybuilt/warmelt-first.so:  melt-stage0-quicklybuilt/warmelt-first.$(MELT_GENERATED_FIRST_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-first
melt-stage0-dynamic/warmelt-first.$(MELT_GENERATED_FIRST_CUMULMD5).dynamic.so: $(MELT_GENERATED_FIRST_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-first+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-first MELT_GENERATED_FIRST_CUMULMD5= $(MELT_GENERATED_FIRST_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-first \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_FIRST_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-first

melt-stage0-dynamic/warmelt-first.so: melt-stage0-dynamic/warmelt-first.$(MELT_GENERATED_FIRST_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-first.quicklybuilt.so: melt-stage0-dynamic/warmelt-first.$(MELT_GENERATED_FIRST_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-first




## for STAGE0 in warmelt-base use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-base
melt-stage0-quicklybuilt/warmelt-base.$(MELT_GENERATED_BASE_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_BASE_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-base MELT_GENERATED_BASE_CUMULMD5= $(MELT_GENERATED_BASE_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-base \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_BASE_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-base

melt-stage0-quicklybuilt/warmelt-base.so:  melt-stage0-quicklybuilt/warmelt-base.$(MELT_GENERATED_BASE_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-base
melt-stage0-dynamic/warmelt-base.$(MELT_GENERATED_BASE_CUMULMD5).dynamic.so: $(MELT_GENERATED_BASE_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-base+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-base MELT_GENERATED_BASE_CUMULMD5= $(MELT_GENERATED_BASE_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-base \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_BASE_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-base

melt-stage0-dynamic/warmelt-base.so: melt-stage0-dynamic/warmelt-base.$(MELT_GENERATED_BASE_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-base.quicklybuilt.so: melt-stage0-dynamic/warmelt-base.$(MELT_GENERATED_BASE_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-base




## for STAGE0 in warmelt-debug use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-debug
melt-stage0-quicklybuilt/warmelt-debug.$(MELT_GENERATED_DEBUG_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_DEBUG_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-debug MELT_GENERATED_DEBUG_CUMULMD5= $(MELT_GENERATED_DEBUG_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-debug \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_DEBUG_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-debug

melt-stage0-quicklybuilt/warmelt-debug.so:  melt-stage0-quicklybuilt/warmelt-debug.$(MELT_GENERATED_DEBUG_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-debug
melt-stage0-dynamic/warmelt-debug.$(MELT_GENERATED_DEBUG_CUMULMD5).dynamic.so: $(MELT_GENERATED_DEBUG_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-debug+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-debug MELT_GENERATED_DEBUG_CUMULMD5= $(MELT_GENERATED_DEBUG_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-debug \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_DEBUG_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-debug

melt-stage0-dynamic/warmelt-debug.so: melt-stage0-dynamic/warmelt-debug.$(MELT_GENERATED_DEBUG_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-debug.quicklybuilt.so: melt-stage0-dynamic/warmelt-debug.$(MELT_GENERATED_DEBUG_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-debug




## for STAGE0 in warmelt-macro use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-macro
melt-stage0-quicklybuilt/warmelt-macro.$(MELT_GENERATED_MACRO_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_MACRO_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-macro MELT_GENERATED_MACRO_CUMULMD5= $(MELT_GENERATED_MACRO_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-macro \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_MACRO_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-macro

melt-stage0-quicklybuilt/warmelt-macro.so:  melt-stage0-quicklybuilt/warmelt-macro.$(MELT_GENERATED_MACRO_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-macro
melt-stage0-dynamic/warmelt-macro.$(MELT_GENERATED_MACRO_CUMULMD5).dynamic.so: $(MELT_GENERATED_MACRO_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-macro+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-macro MELT_GENERATED_MACRO_CUMULMD5= $(MELT_GENERATED_MACRO_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-macro \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_MACRO_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-macro

melt-stage0-dynamic/warmelt-macro.so: melt-stage0-dynamic/warmelt-macro.$(MELT_GENERATED_MACRO_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-macro.quicklybuilt.so: melt-stage0-dynamic/warmelt-macro.$(MELT_GENERATED_MACRO_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-macro




## for STAGE0 in warmelt-normal use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-normal
melt-stage0-quicklybuilt/warmelt-normal.$(MELT_GENERATED_NORMAL_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_NORMAL_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-normal MELT_GENERATED_NORMAL_CUMULMD5= $(MELT_GENERATED_NORMAL_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-normal \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_NORMAL_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-normal

melt-stage0-quicklybuilt/warmelt-normal.so:  melt-stage0-quicklybuilt/warmelt-normal.$(MELT_GENERATED_NORMAL_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-normal
melt-stage0-dynamic/warmelt-normal.$(MELT_GENERATED_NORMAL_CUMULMD5).dynamic.so: $(MELT_GENERATED_NORMAL_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-normal+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-normal MELT_GENERATED_NORMAL_CUMULMD5= $(MELT_GENERATED_NORMAL_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-normal \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_NORMAL_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-normal

melt-stage0-dynamic/warmelt-normal.so: melt-stage0-dynamic/warmelt-normal.$(MELT_GENERATED_NORMAL_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-normal.quicklybuilt.so: melt-stage0-dynamic/warmelt-normal.$(MELT_GENERATED_NORMAL_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-normal




## for STAGE0 in warmelt-normatch use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-normatch
melt-stage0-quicklybuilt/warmelt-normatch.$(MELT_GENERATED_NORMATCH_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_NORMATCH_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-normatch MELT_GENERATED_NORMATCH_CUMULMD5= $(MELT_GENERATED_NORMATCH_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-normatch \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_NORMATCH_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-normatch

melt-stage0-quicklybuilt/warmelt-normatch.so:  melt-stage0-quicklybuilt/warmelt-normatch.$(MELT_GENERATED_NORMATCH_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-normatch
melt-stage0-dynamic/warmelt-normatch.$(MELT_GENERATED_NORMATCH_CUMULMD5).dynamic.so: $(MELT_GENERATED_NORMATCH_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-normatch+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-normatch MELT_GENERATED_NORMATCH_CUMULMD5= $(MELT_GENERATED_NORMATCH_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-normatch \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_NORMATCH_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-normatch

melt-stage0-dynamic/warmelt-normatch.so: melt-stage0-dynamic/warmelt-normatch.$(MELT_GENERATED_NORMATCH_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-normatch.quicklybuilt.so: melt-stage0-dynamic/warmelt-normatch.$(MELT_GENERATED_NORMATCH_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-normatch




## for STAGE0 in warmelt-genobj use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-genobj
melt-stage0-quicklybuilt/warmelt-genobj.$(MELT_GENERATED_GENOBJ_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_GENOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-genobj MELT_GENERATED_GENOBJ_CUMULMD5= $(MELT_GENERATED_GENOBJ_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-genobj \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_GENOBJ_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-genobj

melt-stage0-quicklybuilt/warmelt-genobj.so:  melt-stage0-quicklybuilt/warmelt-genobj.$(MELT_GENERATED_GENOBJ_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-genobj
melt-stage0-dynamic/warmelt-genobj.$(MELT_GENERATED_GENOBJ_CUMULMD5).dynamic.so: $(MELT_GENERATED_GENOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-genobj+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-genobj MELT_GENERATED_GENOBJ_CUMULMD5= $(MELT_GENERATED_GENOBJ_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-genobj \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_GENOBJ_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-genobj

melt-stage0-dynamic/warmelt-genobj.so: melt-stage0-dynamic/warmelt-genobj.$(MELT_GENERATED_GENOBJ_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-genobj.quicklybuilt.so: melt-stage0-dynamic/warmelt-genobj.$(MELT_GENERATED_GENOBJ_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-genobj




## for STAGE0 in warmelt-outobj use GAWK [GNU awk] to retrieve the cumulated_hexmd5
## from the MELT descriptor C file 

## using static object fields offsets for warmelt-outobj
melt-stage0-quicklybuilt/warmelt-outobj.$(MELT_GENERATED_OUTOBJ_CUMULMD5).quicklybuilt.so:  $(MELT_GENERATED_OUTOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h  $(melt_make_cc1_dependency)
	@echo stage0static warmelt-outobj MELT_GENERATED_OUTOBJ_CUMULMD5= $(MELT_GENERATED_OUTOBJ_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-outobj \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_OUTOBJ_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-quicklybuilt/warmelt-outobj

melt-stage0-quicklybuilt/warmelt-outobj.so:  melt-stage0-quicklybuilt/warmelt-outobj.$(MELT_GENERATED_OUTOBJ_CUMULMD5).quicklybuilt.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for warmelt-outobj
melt-stage0-dynamic/warmelt-outobj.$(MELT_GENERATED_OUTOBJ_CUMULMD5).dynamic.so: $(MELT_GENERATED_OUTOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
	     $(melt_make_source_dir)/generated/warmelt-outobj+meltdesc.c \
             melt-predef.h $(melt_make_cc1_dependency)
	@echo stage0dynamic warmelt-outobj MELT_GENERATED_OUTOBJ_CUMULMD5= $(MELT_GENERATED_OUTOBJ_CUMULMD5)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=$(melt_make_source_dir)/generated/warmelt-outobj \
	      GCCMELT_CUMULATED_MD5=$(MELT_GENERATED_OUTOBJ_CUMULMD5) \
              GCCMELT_MODULE_BINARYBASE=melt-stage0-dynamic/warmelt-outobj

melt-stage0-dynamic/warmelt-outobj.so: melt-stage0-dynamic/warmelt-outobj.$(MELT_GENERATED_OUTOBJ_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/warmelt-outobj.quicklybuilt.so: melt-stage0-dynamic/warmelt-outobj.$(MELT_GENERATED_OUTOBJ_CUMULMD5).dynamic.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

#### end STAGE0 of warmelt-outobj





melt-stage0-quicklybuilt.stamp:  melt-stage0-quicklybuilt melt-run.h  $(wildcard $(patsubst %,$(melt_make_source_dir)/generated/%*.c,$(MELT_TRANSLATOR_BASE))) | melt-stage0-quicklybuilt/warmelt.modlis
	date +"#$@ generated %F" > $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_FIRST_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_BASE_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_DEBUG_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_MACRO_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_NORMAL_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_NORMATCH_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_GENOBJ_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_OUTOBJ_C_FILES) >> $@-tmp
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@
	rm -f $(patsubst %,melt-stage0-quicklybuilt/%*.c,$(MELT_TRANSLATOR_BASE))
	$(LN_S)  $(realpath $(sort $(wildcard $(patsubst %,$(realpath $(melt_make_source_dir))/generated/%*.c,$(MELT_TRANSLATOR_BASE))))) melt-stage0-quicklybuilt/
	@echo STAMPstage0static after $@ ; ls -l  melt-stage0-quicklybuilt/*

melt-stage0-dynamic.stamp:  melt-stage0-dynamic melt-run.h  $(wildcard $(patsubst %,$(melt_make_source_dir)/generated/%*.c,$(MELT_TRANSLATOR_BASE))) | melt-stage0-dynamic/warmelt.modlis
	date +"#$@ generated %F" > $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_FIRST_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_BASE_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_DEBUG_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_MACRO_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_NORMAL_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_NORMATCH_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_GENOBJ_C_FILES) >> $@-tmp
	md5sum melt-run.h $(MELT_GENERATED_OUTOBJ_C_FILES) >> $@-tmp
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@
	rm -f $(patsubst %,melt-stage0-dynamic/%*.c,$(MELT_TRANSLATOR_BASE))
	$(LN_S)  $(realpath $(sort $(wildcard $(patsubst %,$(realpath $(melt_make_source_dir))/generated/%*.c,$(MELT_TRANSLATOR_BASE))))) melt-stage0-dynamic/
	@echo STAMPstage0dynamic after $@ ; ls -l  melt-stage0-dynamic/*




melt-stage0-quicklybuilt/warmelt.modlis: | \
             melt-stage0-quicklybuilt/warmelt-first.$(MELT_GENERATED_FIRST_CUMULMD5).quicklybuilt.so \
             melt-stage0-quicklybuilt/warmelt-base.$(MELT_GENERATED_BASE_CUMULMD5).quicklybuilt.so \
             melt-stage0-quicklybuilt/warmelt-debug.$(MELT_GENERATED_DEBUG_CUMULMD5).quicklybuilt.so \
             melt-stage0-quicklybuilt/warmelt-macro.$(MELT_GENERATED_MACRO_CUMULMD5).quicklybuilt.so \
             melt-stage0-quicklybuilt/warmelt-normal.$(MELT_GENERATED_NORMAL_CUMULMD5).quicklybuilt.so \
             melt-stage0-quicklybuilt/warmelt-normatch.$(MELT_GENERATED_NORMATCH_CUMULMD5).quicklybuilt.so \
             melt-stage0-quicklybuilt/warmelt-genobj.$(MELT_GENERATED_GENOBJ_CUMULMD5).quicklybuilt.so \
             melt-stage0-quicklybuilt/warmelt-outobj.$(MELT_GENERATED_OUTOBJ_CUMULMD5).quicklybuilt.so
	date  +"#$@ generated %F" > $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-first.quicklybuilt >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-base.quicklybuilt >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-debug.quicklybuilt >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-macro.quicklybuilt >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-normal.quicklybuilt >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-normatch.quicklybuilt >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-genobj.quicklybuilt >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-outobj.quicklybuilt >> $@-tmp
	$(melt_make_move) $@-tmp $@

melt-stage0-dynamic/warmelt.modlis: | \
              melt-stage0-dynamic/warmelt-first.$(MELT_GENERATED_FIRST_CUMULMD5).dynamic.so \
              melt-stage0-dynamic/warmelt-base.$(MELT_GENERATED_BASE_CUMULMD5).dynamic.so \
              melt-stage0-dynamic/warmelt-debug.$(MELT_GENERATED_DEBUG_CUMULMD5).dynamic.so \
              melt-stage0-dynamic/warmelt-macro.$(MELT_GENERATED_MACRO_CUMULMD5).dynamic.so \
              melt-stage0-dynamic/warmelt-normal.$(MELT_GENERATED_NORMAL_CUMULMD5).dynamic.so \
              melt-stage0-dynamic/warmelt-normatch.$(MELT_GENERATED_NORMATCH_CUMULMD5).dynamic.so \
              melt-stage0-dynamic/warmelt-genobj.$(MELT_GENERATED_GENOBJ_CUMULMD5).dynamic.so \
              melt-stage0-dynamic/warmelt-outobj.$(MELT_GENERATED_OUTOBJ_CUMULMD5).dynamic.so
	date  +"#$@ generated %F" > $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-first.dynamic >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-base.dynamic >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-debug.dynamic >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-macro.dynamic >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-normal.dynamic >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-normatch.dynamic >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-genobj.dynamic >> $@-tmp
	echo $(melt_make_source_dir)/generated/warmelt-outobj.dynamic >> $@-tmp

	$(melt_make_move) $@-tmp $@

## An empty file is needed for every MELT translation!
empty-file-for-melt.c:
	date +"/* empty-file-for-melt.c %c */" > $@-tmp
	mv $@-tmp $@

## can be overridden manually to either melt-stage0-dynamic or
## melt-stage0-quicklybuilt
.PHONY: warmelt0
## the default stage0 melt-stage0-dynamic
MELT_STAGE_ZERO?= melt-stage0-dynamic
MELT_ZERO_FLAVOR=$(patsubst melt-stage0-%,%,$(MELT_STAGE_ZERO))
warmelt0: $(melt_make_cc1_dependency) $(MELT_STAGE_ZERO) $(MELT_STAGE_ZERO).stamp 
$(MELT_STAGE_ZERO):
	-test -d $(MELT_STAGE_ZERO)/ || mkdir $(MELT_STAGE_ZERO)



#### making our melt stages


#### rules for melt-stage1, previous $(MELT_STAGE_ZERO)



### the C source of melt-stage1 for warmelt-first
melt-stage1/warmelt-first.c:  $(melt_make_source_dir)/warmelt-first.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
                                                  empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCINIT1ARGS) $(meltarg_init)=\
$(MELT_STAGE_ZERO)/warmelt-first.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-base.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-debug.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-macro.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normal.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normatch.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-genobj.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-first for melt-stage1
melt-stage1/warmelt-first.quicklybuilt.so: melt-stage1/warmelt-first.c \
              $(wildcard melt-stage1/warmelt-first+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-first

################## debugnoline module warmelt-first for melt-stage1
melt-stage1/warmelt-first.debugnoline.so:   melt-stage1/warmelt-first.c \
              $(wildcard melt-stage1/warmelt-first+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-first




### the C source of melt-stage1 for warmelt-base
melt-stage1/warmelt-base.c:  $(melt_make_source_dir)/warmelt-base.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
      $(realpath melt-stage1/warmelt-first.quicklybuilt.so) \
                                            empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
$(MELT_STAGE_ZERO)/warmelt-base.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-debug.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-macro.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normal.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normatch.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-genobj.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-base for melt-stage1
melt-stage1/warmelt-base.quicklybuilt.so: melt-stage1/warmelt-base.c \
              $(wildcard melt-stage1/warmelt-base+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-base

################## debugnoline module warmelt-base for melt-stage1
melt-stage1/warmelt-base.debugnoline.so:   melt-stage1/warmelt-base.c \
              $(wildcard melt-stage1/warmelt-base+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-base




### the C source of melt-stage1 for warmelt-debug
melt-stage1/warmelt-debug.c:  $(melt_make_source_dir)/warmelt-debug.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
      $(realpath melt-stage1/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-base.quicklybuilt.so) \
                                      empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
$(MELT_STAGE_ZERO)/warmelt-debug.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-macro.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normal.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normatch.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-genobj.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-debug for melt-stage1
melt-stage1/warmelt-debug.quicklybuilt.so: melt-stage1/warmelt-debug.c \
              $(wildcard melt-stage1/warmelt-debug+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-debug

################## debugnoline module warmelt-debug for melt-stage1
melt-stage1/warmelt-debug.debugnoline.so:   melt-stage1/warmelt-debug.c \
              $(wildcard melt-stage1/warmelt-debug+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-debug




### the C source of melt-stage1 for warmelt-macro
melt-stage1/warmelt-macro.c:  $(melt_make_source_dir)/warmelt-macro.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
      $(realpath melt-stage1/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-debug.quicklybuilt.so) \
                                empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
$(MELT_STAGE_ZERO)/warmelt-macro.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normal.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normatch.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-genobj.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-macro for melt-stage1
melt-stage1/warmelt-macro.quicklybuilt.so: melt-stage1/warmelt-macro.c \
              $(wildcard melt-stage1/warmelt-macro+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-macro

################## debugnoline module warmelt-macro for melt-stage1
melt-stage1/warmelt-macro.debugnoline.so:   melt-stage1/warmelt-macro.c \
              $(wildcard melt-stage1/warmelt-macro+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-macro




### the C source of melt-stage1 for warmelt-normal
melt-stage1/warmelt-normal.c:  $(melt_make_source_dir)/warmelt-normal.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
        warmelt-predef.melt \
      $(realpath melt-stage1/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-macro.quicklybuilt.so) \
                          empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
$(MELT_STAGE_ZERO)/warmelt-normal.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-normatch.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-genobj.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-normal for melt-stage1
melt-stage1/warmelt-normal.quicklybuilt.so: melt-stage1/warmelt-normal.c \
              $(wildcard melt-stage1/warmelt-normal+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-normal

################## debugnoline module warmelt-normal for melt-stage1
melt-stage1/warmelt-normal.debugnoline.so:   melt-stage1/warmelt-normal.c \
              $(wildcard melt-stage1/warmelt-normal+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-normal




### the C source of melt-stage1 for warmelt-normatch
melt-stage1/warmelt-normatch.c:  $(melt_make_source_dir)/warmelt-normatch.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
      $(realpath melt-stage1/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-normal.quicklybuilt.so) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
$(MELT_STAGE_ZERO)/warmelt-normatch.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-genobj.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-normatch for melt-stage1
melt-stage1/warmelt-normatch.quicklybuilt.so: melt-stage1/warmelt-normatch.c \
              $(wildcard melt-stage1/warmelt-normatch+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-normatch

################## debugnoline module warmelt-normatch for melt-stage1
melt-stage1/warmelt-normatch.debugnoline.so:   melt-stage1/warmelt-normatch.c \
              $(wildcard melt-stage1/warmelt-normatch+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-normatch




### the C source of melt-stage1 for warmelt-genobj
melt-stage1/warmelt-genobj.c:  $(melt_make_source_dir)/warmelt-genobj.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
      $(realpath melt-stage1/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-normal.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-normatch.quicklybuilt.so) \
              empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
$(MELT_STAGE_ZERO)/warmelt-genobj.$(MELT_ZERO_FLAVOR):\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-genobj for melt-stage1
melt-stage1/warmelt-genobj.quicklybuilt.so: melt-stage1/warmelt-genobj.c \
              $(wildcard melt-stage1/warmelt-genobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-genobj

################## debugnoline module warmelt-genobj for melt-stage1
melt-stage1/warmelt-genobj.debugnoline.so:   melt-stage1/warmelt-genobj.c \
              $(wildcard melt-stage1/warmelt-genobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-genobj




### the C source of melt-stage1 for warmelt-outobj
melt-stage1/warmelt-outobj.c:  $(melt_make_source_dir)/warmelt-outobj.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | $(MELT_STAGE_ZERO).stamp $(MELT_STAGE_ZERO)/warmelt.modlis \
      $(realpath melt-stage1/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-normal.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-normatch.quicklybuilt.so) \
      $(realpath melt-stage1/warmelt-genobj.quicklybuilt.so) \
        empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage1
	@rm -f $(notdir $(basename $@)melt-stage1.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
$(MELT_STAGE_ZERO)/warmelt-outobj.$(MELT_ZERO_FLAVOR) > $(notdir $(basename $@)melt-stage1.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage1):$(realpath $(MELT_STAGE_ZERO)):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage1.args-tmp)
	@mv $(notdir $(basename $@))melt-stage1.args-tmp $(notdir $(basename $@))melt-stage1.args
	@echo -n $(notdir $(basename $@)melt-stage1.args): ; cat $(notdir $(basename $@))melt-stage1.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage1.args)

################## quicklybuilt module warmelt-outobj for melt-stage1
melt-stage1/warmelt-outobj.quicklybuilt.so: melt-stage1/warmelt-outobj.c \
              $(wildcard melt-stage1/warmelt-outobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-outobj

################## debugnoline module warmelt-outobj for melt-stage1
melt-stage1/warmelt-outobj.debugnoline.so:   melt-stage1/warmelt-outobj.c \
              $(wildcard melt-stage1/warmelt-outobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage1/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage1)/warmelt-outobj




## the module list in melt-stage1
melt-stage1/warmelt.modlis:  \
             melt-stage1/warmelt-first.quicklybuilt.so \
             melt-stage1/warmelt-base.quicklybuilt.so \
             melt-stage1/warmelt-debug.quicklybuilt.so \
             melt-stage1/warmelt-macro.quicklybuilt.so \
             melt-stage1/warmelt-normal.quicklybuilt.so \
             melt-stage1/warmelt-normatch.quicklybuilt.so \
             melt-stage1/warmelt-genobj.quicklybuilt.so \
             melt-stage1/warmelt-outobj.quicklybuilt.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first.quicklybuilt >> $@-tmp
	echo warmelt-base.quicklybuilt >> $@-tmp
	echo warmelt-debug.quicklybuilt >> $@-tmp
	echo warmelt-macro.quicklybuilt >> $@-tmp
	echo warmelt-normal.quicklybuilt >> $@-tmp
	echo warmelt-normatch.quicklybuilt >> $@-tmp
	echo warmelt-genobj.quicklybuilt >> $@-tmp
	echo warmelt-outobj.quicklybuilt >> $@-tmp
	$(melt_make_move) $@-tmp $@


melt-stage1/warmelt.debugnoline.modlis:  \
             melt-stage1/warmelt-first.debugnoline.so \
             melt-stage1/warmelt-base.debugnoline.so \
             melt-stage1/warmelt-debug.debugnoline.so \
             melt-stage1/warmelt-macro.debugnoline.so \
             melt-stage1/warmelt-normal.debugnoline.so \
             melt-stage1/warmelt-normatch.debugnoline.so \
             melt-stage1/warmelt-genobj.debugnoline.so \
             melt-stage1/warmelt-outobj.debugnoline.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first.debugnoline >> $@-tmp
	echo warmelt-base.debugnoline >> $@-tmp
	echo warmelt-debug.debugnoline >> $@-tmp
	echo warmelt-macro.debugnoline >> $@-tmp
	echo warmelt-normal.debugnoline >> $@-tmp
	echo warmelt-normatch.debugnoline >> $@-tmp
	echo warmelt-genobj.debugnoline >> $@-tmp
	echo warmelt-outobj.debugnoline >> $@-tmp
	$(melt_make_move) $@-tmp $@


## the stamp for melt-stage1, using an order only prerequisite
melt-stage1.stamp:  melt-run.h | melt-stage1/warmelt.modlis
	date +"#$@ generated %F" > $@-tmp
	md5sum melt-run.h >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-first*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-base*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-debug*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-macro*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-normal*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-normatch*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-genobj*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage1/warmelt-outobj*.c) < /dev/null >> $@-tmp
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


### phony targets for  melt-stage1
.PHONY: warmelt1 warmelt1n
warmelt1:  melt-stage1 melt-stage1/warmelt.modlis
	@echo MELT build made $@
warmelt1n:  melt-stage1 melt-stage1/warmelt.debugnoline.modlis
	@echo MELT build made $@
melt-stage1:
	if [ -d melt-stage1 ]; then true; else mkdir melt-stage1; fi

### end of melt-stage1


#### rules for melt-stage2, previous melt-stage1



### the C source of melt-stage2 for warmelt-first
melt-stage2/warmelt-first.c:  $(melt_make_source_dir)/warmelt-first.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
                                                  empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCINIT1ARGS) $(meltarg_init)=\
melt-stage1/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-first for melt-stage2
melt-stage2/warmelt-first.quicklybuilt.so: melt-stage2/warmelt-first.c \
              $(wildcard melt-stage2/warmelt-first+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-first

################## debugnoline module warmelt-first for melt-stage2
melt-stage2/warmelt-first.debugnoline.so:   melt-stage2/warmelt-first.c \
              $(wildcard melt-stage2/warmelt-first+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-first




### the C source of melt-stage2 for warmelt-base
melt-stage2/warmelt-base.c:  $(melt_make_source_dir)/warmelt-base.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
      $(realpath melt-stage2/warmelt-first.quicklybuilt.so) \
                                            empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage1/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-base for melt-stage2
melt-stage2/warmelt-base.quicklybuilt.so: melt-stage2/warmelt-base.c \
              $(wildcard melt-stage2/warmelt-base+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-base

################## debugnoline module warmelt-base for melt-stage2
melt-stage2/warmelt-base.debugnoline.so:   melt-stage2/warmelt-base.c \
              $(wildcard melt-stage2/warmelt-base+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-base




### the C source of melt-stage2 for warmelt-debug
melt-stage2/warmelt-debug.c:  $(melt_make_source_dir)/warmelt-debug.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
      $(realpath melt-stage2/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-base.quicklybuilt.so) \
                                      empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage1/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-debug for melt-stage2
melt-stage2/warmelt-debug.quicklybuilt.so: melt-stage2/warmelt-debug.c \
              $(wildcard melt-stage2/warmelt-debug+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-debug

################## debugnoline module warmelt-debug for melt-stage2
melt-stage2/warmelt-debug.debugnoline.so:   melt-stage2/warmelt-debug.c \
              $(wildcard melt-stage2/warmelt-debug+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-debug




### the C source of melt-stage2 for warmelt-macro
melt-stage2/warmelt-macro.c:  $(melt_make_source_dir)/warmelt-macro.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
      $(realpath melt-stage2/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-debug.quicklybuilt.so) \
                                empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage1/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-macro for melt-stage2
melt-stage2/warmelt-macro.quicklybuilt.so: melt-stage2/warmelt-macro.c \
              $(wildcard melt-stage2/warmelt-macro+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-macro

################## debugnoline module warmelt-macro for melt-stage2
melt-stage2/warmelt-macro.debugnoline.so:   melt-stage2/warmelt-macro.c \
              $(wildcard melt-stage2/warmelt-macro+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-macro




### the C source of melt-stage2 for warmelt-normal
melt-stage2/warmelt-normal.c:  $(melt_make_source_dir)/warmelt-normal.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
        warmelt-predef.melt \
      $(realpath melt-stage2/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-macro.quicklybuilt.so) \
                          empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage1/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-normal for melt-stage2
melt-stage2/warmelt-normal.quicklybuilt.so: melt-stage2/warmelt-normal.c \
              $(wildcard melt-stage2/warmelt-normal+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-normal

################## debugnoline module warmelt-normal for melt-stage2
melt-stage2/warmelt-normal.debugnoline.so:   melt-stage2/warmelt-normal.c \
              $(wildcard melt-stage2/warmelt-normal+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-normal




### the C source of melt-stage2 for warmelt-normatch
melt-stage2/warmelt-normatch.c:  $(melt_make_source_dir)/warmelt-normatch.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
      $(realpath melt-stage2/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-normal.quicklybuilt.so) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage1/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-normatch for melt-stage2
melt-stage2/warmelt-normatch.quicklybuilt.so: melt-stage2/warmelt-normatch.c \
              $(wildcard melt-stage2/warmelt-normatch+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-normatch

################## debugnoline module warmelt-normatch for melt-stage2
melt-stage2/warmelt-normatch.debugnoline.so:   melt-stage2/warmelt-normatch.c \
              $(wildcard melt-stage2/warmelt-normatch+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-normatch




### the C source of melt-stage2 for warmelt-genobj
melt-stage2/warmelt-genobj.c:  $(melt_make_source_dir)/warmelt-genobj.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
      $(realpath melt-stage2/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-normal.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-normatch.quicklybuilt.so) \
              empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage1/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-genobj for melt-stage2
melt-stage2/warmelt-genobj.quicklybuilt.so: melt-stage2/warmelt-genobj.c \
              $(wildcard melt-stage2/warmelt-genobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-genobj

################## debugnoline module warmelt-genobj for melt-stage2
melt-stage2/warmelt-genobj.debugnoline.so:   melt-stage2/warmelt-genobj.c \
              $(wildcard melt-stage2/warmelt-genobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-genobj




### the C source of melt-stage2 for warmelt-outobj
melt-stage2/warmelt-outobj.c:  $(melt_make_source_dir)/warmelt-outobj.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage1.stamp melt-stage1/warmelt.modlis \
      $(realpath melt-stage2/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-normal.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-normatch.quicklybuilt.so) \
      $(realpath melt-stage2/warmelt-genobj.quicklybuilt.so) \
        empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage2
	@rm -f $(notdir $(basename $@)melt-stage2.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage1/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage2.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage2):$(realpath melt-stage1):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage2.args-tmp)
	@mv $(notdir $(basename $@))melt-stage2.args-tmp $(notdir $(basename $@))melt-stage2.args
	@echo -n $(notdir $(basename $@)melt-stage2.args): ; cat $(notdir $(basename $@))melt-stage2.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage2.args)

################## quicklybuilt module warmelt-outobj for melt-stage2
melt-stage2/warmelt-outobj.quicklybuilt.so: melt-stage2/warmelt-outobj.c \
              $(wildcard melt-stage2/warmelt-outobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-outobj

################## debugnoline module warmelt-outobj for melt-stage2
melt-stage2/warmelt-outobj.debugnoline.so:   melt-stage2/warmelt-outobj.c \
              $(wildcard melt-stage2/warmelt-outobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage2/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage2)/warmelt-outobj




## the module list in melt-stage2
melt-stage2/warmelt.modlis:  \
             melt-stage2/warmelt-first.quicklybuilt.so \
             melt-stage2/warmelt-base.quicklybuilt.so \
             melt-stage2/warmelt-debug.quicklybuilt.so \
             melt-stage2/warmelt-macro.quicklybuilt.so \
             melt-stage2/warmelt-normal.quicklybuilt.so \
             melt-stage2/warmelt-normatch.quicklybuilt.so \
             melt-stage2/warmelt-genobj.quicklybuilt.so \
             melt-stage2/warmelt-outobj.quicklybuilt.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first.quicklybuilt >> $@-tmp
	echo warmelt-base.quicklybuilt >> $@-tmp
	echo warmelt-debug.quicklybuilt >> $@-tmp
	echo warmelt-macro.quicklybuilt >> $@-tmp
	echo warmelt-normal.quicklybuilt >> $@-tmp
	echo warmelt-normatch.quicklybuilt >> $@-tmp
	echo warmelt-genobj.quicklybuilt >> $@-tmp
	echo warmelt-outobj.quicklybuilt >> $@-tmp
	$(melt_make_move) $@-tmp $@


melt-stage2/warmelt.debugnoline.modlis:  \
             melt-stage2/warmelt-first.debugnoline.so \
             melt-stage2/warmelt-base.debugnoline.so \
             melt-stage2/warmelt-debug.debugnoline.so \
             melt-stage2/warmelt-macro.debugnoline.so \
             melt-stage2/warmelt-normal.debugnoline.so \
             melt-stage2/warmelt-normatch.debugnoline.so \
             melt-stage2/warmelt-genobj.debugnoline.so \
             melt-stage2/warmelt-outobj.debugnoline.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first.debugnoline >> $@-tmp
	echo warmelt-base.debugnoline >> $@-tmp
	echo warmelt-debug.debugnoline >> $@-tmp
	echo warmelt-macro.debugnoline >> $@-tmp
	echo warmelt-normal.debugnoline >> $@-tmp
	echo warmelt-normatch.debugnoline >> $@-tmp
	echo warmelt-genobj.debugnoline >> $@-tmp
	echo warmelt-outobj.debugnoline >> $@-tmp
	$(melt_make_move) $@-tmp $@


## the stamp for melt-stage2, using an order only prerequisite
melt-stage2.stamp:  melt-run.h | melt-stage2/warmelt.modlis
	date +"#$@ generated %F" > $@-tmp
	md5sum melt-run.h >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-first*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-base*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-debug*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-macro*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-normal*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-normatch*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-genobj*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage2/warmelt-outobj*.c) < /dev/null >> $@-tmp
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


### phony targets for  melt-stage2
.PHONY: warmelt2 warmelt2n
warmelt2:  melt-stage2 melt-stage2/warmelt.modlis
	@echo MELT build made $@
warmelt2n:  melt-stage2 melt-stage2/warmelt.debugnoline.modlis
	@echo MELT build made $@
melt-stage2:
	if [ -d melt-stage2 ]; then true; else mkdir melt-stage2; fi

### end of melt-stage2


#### rules for melt-stage3, previous melt-stage2



### the C source of melt-stage3 for warmelt-first
melt-stage3/warmelt-first.c:  $(melt_make_source_dir)/warmelt-first.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
                                                  empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCINIT1ARGS) $(meltarg_init)=\
melt-stage2/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-first for melt-stage3
melt-stage3/warmelt-first.quicklybuilt.so: melt-stage3/warmelt-first.c \
              $(wildcard melt-stage3/warmelt-first+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-first

################## debugnoline module warmelt-first for melt-stage3
melt-stage3/warmelt-first.debugnoline.so:   melt-stage3/warmelt-first.c \
              $(wildcard melt-stage3/warmelt-first+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-first




### the C source of melt-stage3 for warmelt-base
melt-stage3/warmelt-base.c:  $(melt_make_source_dir)/warmelt-base.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
      $(realpath melt-stage3/warmelt-first.quicklybuilt.so) \
                                            empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage3/warmelt-first.quicklybuilt:\
melt-stage2/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-base for melt-stage3
melt-stage3/warmelt-base.quicklybuilt.so: melt-stage3/warmelt-base.c \
              $(wildcard melt-stage3/warmelt-base+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-base

################## debugnoline module warmelt-base for melt-stage3
melt-stage3/warmelt-base.debugnoline.so:   melt-stage3/warmelt-base.c \
              $(wildcard melt-stage3/warmelt-base+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-base




### the C source of melt-stage3 for warmelt-debug
melt-stage3/warmelt-debug.c:  $(melt_make_source_dir)/warmelt-debug.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
      $(realpath melt-stage3/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-base.quicklybuilt.so) \
                                      empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage3/warmelt-first.quicklybuilt:\
melt-stage3/warmelt-base.quicklybuilt:\
melt-stage2/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-debug for melt-stage3
melt-stage3/warmelt-debug.quicklybuilt.so: melt-stage3/warmelt-debug.c \
              $(wildcard melt-stage3/warmelt-debug+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-debug

################## debugnoline module warmelt-debug for melt-stage3
melt-stage3/warmelt-debug.debugnoline.so:   melt-stage3/warmelt-debug.c \
              $(wildcard melt-stage3/warmelt-debug+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-debug




### the C source of melt-stage3 for warmelt-macro
melt-stage3/warmelt-macro.c:  $(melt_make_source_dir)/warmelt-macro.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
      $(realpath melt-stage3/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-debug.quicklybuilt.so) \
                                empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage3/warmelt-first.quicklybuilt:\
melt-stage3/warmelt-base.quicklybuilt:\
melt-stage3/warmelt-debug.quicklybuilt:\
melt-stage2/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-macro for melt-stage3
melt-stage3/warmelt-macro.quicklybuilt.so: melt-stage3/warmelt-macro.c \
              $(wildcard melt-stage3/warmelt-macro+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-macro

################## debugnoline module warmelt-macro for melt-stage3
melt-stage3/warmelt-macro.debugnoline.so:   melt-stage3/warmelt-macro.c \
              $(wildcard melt-stage3/warmelt-macro+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-macro




### the C source of melt-stage3 for warmelt-normal
melt-stage3/warmelt-normal.c:  $(melt_make_source_dir)/warmelt-normal.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
        warmelt-predef.melt \
      $(realpath melt-stage3/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-macro.quicklybuilt.so) \
                          empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage3/warmelt-first.quicklybuilt:\
melt-stage3/warmelt-base.quicklybuilt:\
melt-stage3/warmelt-debug.quicklybuilt:\
melt-stage3/warmelt-macro.quicklybuilt:\
melt-stage2/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-normal for melt-stage3
melt-stage3/warmelt-normal.quicklybuilt.so: melt-stage3/warmelt-normal.c \
              $(wildcard melt-stage3/warmelt-normal+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-normal

################## debugnoline module warmelt-normal for melt-stage3
melt-stage3/warmelt-normal.debugnoline.so:   melt-stage3/warmelt-normal.c \
              $(wildcard melt-stage3/warmelt-normal+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-normal




### the C source of melt-stage3 for warmelt-normatch
melt-stage3/warmelt-normatch.c:  $(melt_make_source_dir)/warmelt-normatch.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
      $(realpath melt-stage3/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-normal.quicklybuilt.so) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage3/warmelt-first.quicklybuilt:\
melt-stage3/warmelt-base.quicklybuilt:\
melt-stage3/warmelt-debug.quicklybuilt:\
melt-stage3/warmelt-macro.quicklybuilt:\
melt-stage3/warmelt-normal.quicklybuilt:\
melt-stage2/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-normatch for melt-stage3
melt-stage3/warmelt-normatch.quicklybuilt.so: melt-stage3/warmelt-normatch.c \
              $(wildcard melt-stage3/warmelt-normatch+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-normatch

################## debugnoline module warmelt-normatch for melt-stage3
melt-stage3/warmelt-normatch.debugnoline.so:   melt-stage3/warmelt-normatch.c \
              $(wildcard melt-stage3/warmelt-normatch+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-normatch




### the C source of melt-stage3 for warmelt-genobj
melt-stage3/warmelt-genobj.c:  $(melt_make_source_dir)/warmelt-genobj.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
      $(realpath melt-stage3/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-normal.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-normatch.quicklybuilt.so) \
              empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage3/warmelt-first.quicklybuilt:\
melt-stage3/warmelt-base.quicklybuilt:\
melt-stage3/warmelt-debug.quicklybuilt:\
melt-stage3/warmelt-macro.quicklybuilt:\
melt-stage3/warmelt-normal.quicklybuilt:\
melt-stage3/warmelt-normatch.quicklybuilt:\
melt-stage2/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-genobj for melt-stage3
melt-stage3/warmelt-genobj.quicklybuilt.so: melt-stage3/warmelt-genobj.c \
              $(wildcard melt-stage3/warmelt-genobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-genobj

################## debugnoline module warmelt-genobj for melt-stage3
melt-stage3/warmelt-genobj.debugnoline.so:   melt-stage3/warmelt-genobj.c \
              $(wildcard melt-stage3/warmelt-genobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-genobj




### the C source of melt-stage3 for warmelt-outobj
melt-stage3/warmelt-outobj.c:  $(melt_make_source_dir)/warmelt-outobj.melt \
 $(MELT_TRANSLATOR_SOURCE) \
 | melt-stage2.stamp melt-stage2/warmelt.modlis \
      $(realpath melt-stage3/warmelt-first.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-base.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-debug.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-macro.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-normal.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-normatch.quicklybuilt.so) \
      $(realpath melt-stage3/warmelt-genobj.quicklybuilt.so) \
        empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for melt-stage3
	@rm -f $(notdir $(basename $@)melt-stage3.args)
	@echo  $(MELTCCFILE1ARGS) $(meltarg_init)=\
melt-stage3/warmelt-first.quicklybuilt:\
melt-stage3/warmelt-base.quicklybuilt:\
melt-stage3/warmelt-debug.quicklybuilt:\
melt-stage3/warmelt-macro.quicklybuilt:\
melt-stage3/warmelt-normal.quicklybuilt:\
melt-stage3/warmelt-normatch.quicklybuilt:\
melt-stage3/warmelt-genobj.quicklybuilt:\
melt-stage2/warmelt-outobj.quicklybuilt > $(notdir $(basename $@)melt-stage3.args-tmp)
	@echo $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_source_path)=$(realpath .):$(realpath melt-stage3):$(realpath melt-stage2):$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$(basename $@) $(meltarg_workdir)=melt-workdir \
	      empty-file-for-melt.c >> $(notdir $(basename $@)melt-stage3.args-tmp)
	@mv $(notdir $(basename $@))melt-stage3.args-tmp $(notdir $(basename $@))melt-stage3.args
	@echo -n $(notdir $(basename $@)melt-stage3.args): ; cat $(notdir $(basename $@))melt-stage3.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)melt-stage3.args)

################## quicklybuilt module warmelt-outobj for melt-stage3
melt-stage3/warmelt-outobj.quicklybuilt.so: melt-stage3/warmelt-outobj.c \
              $(wildcard melt-stage3/warmelt-outobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-outobj

################## debugnoline module warmelt-outobj for melt-stage3
melt-stage3/warmelt-outobj.debugnoline.so:   melt-stage3/warmelt-outobj.c \
              $(wildcard melt-stage3/warmelt-outobj+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
              GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-stage3/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=$(realpath melt-stage3)/warmelt-outobj




## the module list in melt-stage3
melt-stage3/warmelt.modlis:  \
             melt-stage3/warmelt-first.quicklybuilt.so \
             melt-stage3/warmelt-base.quicklybuilt.so \
             melt-stage3/warmelt-debug.quicklybuilt.so \
             melt-stage3/warmelt-macro.quicklybuilt.so \
             melt-stage3/warmelt-normal.quicklybuilt.so \
             melt-stage3/warmelt-normatch.quicklybuilt.so \
             melt-stage3/warmelt-genobj.quicklybuilt.so \
             melt-stage3/warmelt-outobj.quicklybuilt.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first.quicklybuilt >> $@-tmp
	echo warmelt-base.quicklybuilt >> $@-tmp
	echo warmelt-debug.quicklybuilt >> $@-tmp
	echo warmelt-macro.quicklybuilt >> $@-tmp
	echo warmelt-normal.quicklybuilt >> $@-tmp
	echo warmelt-normatch.quicklybuilt >> $@-tmp
	echo warmelt-genobj.quicklybuilt >> $@-tmp
	echo warmelt-outobj.quicklybuilt >> $@-tmp
	$(melt_make_move) $@-tmp $@


melt-stage3/warmelt.debugnoline.modlis:  \
             melt-stage3/warmelt-first.debugnoline.so \
             melt-stage3/warmelt-base.debugnoline.so \
             melt-stage3/warmelt-debug.debugnoline.so \
             melt-stage3/warmelt-macro.debugnoline.so \
             melt-stage3/warmelt-normal.debugnoline.so \
             melt-stage3/warmelt-normatch.debugnoline.so \
             melt-stage3/warmelt-genobj.debugnoline.so \
             melt-stage3/warmelt-outobj.debugnoline.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first.debugnoline >> $@-tmp
	echo warmelt-base.debugnoline >> $@-tmp
	echo warmelt-debug.debugnoline >> $@-tmp
	echo warmelt-macro.debugnoline >> $@-tmp
	echo warmelt-normal.debugnoline >> $@-tmp
	echo warmelt-normatch.debugnoline >> $@-tmp
	echo warmelt-genobj.debugnoline >> $@-tmp
	echo warmelt-outobj.debugnoline >> $@-tmp
	$(melt_make_move) $@-tmp $@


## the stamp for melt-stage3, using an order only prerequisite
melt-stage3.stamp:  melt-run.h | melt-stage3/warmelt.modlis
	date +"#$@ generated %F" > $@-tmp
	md5sum melt-run.h >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-first*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-base*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-debug*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-macro*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-normal*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-normatch*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-genobj*.c) < /dev/null >> $@-tmp
	md5sum $(wildcard melt-stage3/warmelt-outobj*.c) < /dev/null >> $@-tmp
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


### phony targets for  melt-stage3
.PHONY: warmelt3 warmelt3n
warmelt3:  melt-stage3 melt-stage3/warmelt.modlis
	@echo MELT build made $@
warmelt3n:  melt-stage3 melt-stage3/warmelt.debugnoline.modlis
	@echo MELT build made $@
melt-stage3:
	if [ -d melt-stage3 ]; then true; else mkdir melt-stage3; fi

### end of melt-stage3



######## last stage melt-stage3
MELT_LAST_STAGE=melt-stage3
WARMELT_LAST= warmelt3
WARMELT_LAST_MODLIS= melt-stage3/warmelt.modlis

.PHONY: warmelt
warmelt: $(WARMELT_LAST)

####### final targets
.PHONY: all-melt melt-all-modules melt-all-sources
all-melt: melt-modules melt-sources melt-all-modules melt-all-sources

### the final module directory
melt-modules: 
	test -d melt-modules/ || mkdir  melt-modules/

### the final source directory
melt-sources: 
	test -d melt-sources/ || mkdir  melt-sources/


melt-all-sources: $(WARMELT_LAST_MODLIS) empty-file-for-melt.c \
              melt-run.h melt-runtime.h melt-predef.h melt-sources \
              $(melt_make_cc1_dependency) \
      melt-sources/warmelt-first.melt \
              melt-sources/warmelt-first.c \
      melt-sources/warmelt-base.melt \
              melt-sources/warmelt-base.c \
      melt-sources/warmelt-debug.melt \
              melt-sources/warmelt-debug.c \
      melt-sources/warmelt-macro.melt \
              melt-sources/warmelt-macro.c \
      melt-sources/warmelt-normal.melt \
              melt-sources/warmelt-normal.c \
       melt-sources/warmelt-predef.melt \
      melt-sources/warmelt-normatch.melt \
              melt-sources/warmelt-normatch.c \
      melt-sources/warmelt-genobj.melt \
              melt-sources/warmelt-genobj.c \
      melt-sources/warmelt-outobj.melt \
              melt-sources/warmelt-outobj.c \
            melt-sources/xtramelt-parse-infix-syntax.melt \
              melt-sources/xtramelt-parse-infix-syntax.c  \
            melt-sources/xtramelt-ana-base.melt \
              melt-sources/xtramelt-ana-base.c  \
            melt-sources/xtramelt-ana-simple.melt \
              melt-sources/xtramelt-ana-simple.c  \
            melt-sources/xtramelt-c-generator.melt \
              melt-sources/xtramelt-c-generator.c  \
            melt-sources/xtramelt-opengpu.melt \
              melt-sources/xtramelt-opengpu.c 


#### melt-sources translator files



## melt translator warmelt-first # 0
melt-sources/warmelt-first.melt: $(melt_make_source_dir)/warmelt-first.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro


# MELT translator warmelt-first in melt-sources/
melt-sources/warmelt-first.c: melt-sources/warmelt-first.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCINIT1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-first.optimized.so: melt-sources/warmelt-first.c \
        $(wildcard  melt-sources/warmelt-first+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-first 

melt-modules/warmelt-first.debugnoline.so: melt-sources/warmelt-first.c \
        $(wildcard  melt-sources/warmelt-first+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-first 

melt-modules/warmelt-first.quicklybuilt.so: melt-sources/warmelt-first.c \
        $(wildcard  melt-sources/warmelt-first+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-first \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-first \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-first




## melt translator warmelt-base # 1
melt-sources/warmelt-base.melt: $(melt_make_source_dir)/warmelt-base.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro


# MELT translator warmelt-base in melt-sources/
melt-sources/warmelt-base.c: melt-sources/warmelt-base.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCFILE1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-base.optimized.so: melt-sources/warmelt-base.c \
        $(wildcard  melt-sources/warmelt-base+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-base 

melt-modules/warmelt-base.debugnoline.so: melt-sources/warmelt-base.c \
        $(wildcard  melt-sources/warmelt-base+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-base 

melt-modules/warmelt-base.quicklybuilt.so: melt-sources/warmelt-base.c \
        $(wildcard  melt-sources/warmelt-base+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-base \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-base \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-base




## melt translator warmelt-debug # 2
melt-sources/warmelt-debug.melt: $(melt_make_source_dir)/warmelt-debug.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro


# MELT translator warmelt-debug in melt-sources/
melt-sources/warmelt-debug.c: melt-sources/warmelt-debug.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCFILE1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-debug.optimized.so: melt-sources/warmelt-debug.c \
        $(wildcard  melt-sources/warmelt-debug+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-debug 

melt-modules/warmelt-debug.debugnoline.so: melt-sources/warmelt-debug.c \
        $(wildcard  melt-sources/warmelt-debug+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-debug 

melt-modules/warmelt-debug.quicklybuilt.so: melt-sources/warmelt-debug.c \
        $(wildcard  melt-sources/warmelt-debug+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-debug \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-debug \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-debug




## melt translator warmelt-macro # 3
melt-sources/warmelt-macro.melt: $(melt_make_source_dir)/warmelt-macro.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro


# MELT translator warmelt-macro in melt-sources/
melt-sources/warmelt-macro.c: melt-sources/warmelt-macro.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCFILE1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-macro.optimized.so: melt-sources/warmelt-macro.c \
        $(wildcard  melt-sources/warmelt-macro+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-macro 

melt-modules/warmelt-macro.debugnoline.so: melt-sources/warmelt-macro.c \
        $(wildcard  melt-sources/warmelt-macro+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-macro 

melt-modules/warmelt-macro.quicklybuilt.so: melt-sources/warmelt-macro.c \
        $(wildcard  melt-sources/warmelt-macro+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-macro \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-macro \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-macro




## melt translator warmelt-normal # 4
melt-sources/warmelt-normal.melt: $(melt_make_source_dir)/warmelt-normal.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro

# included warmelt-predef.melt
melt-sources/warmelt-predef.melt: warmelt-predef.melt
	rm -f melt-sources/warmelt-predef.melt*
	cp $^ melt-sources/warmelt-predef.melt-tmp
	mv  melt-sources/warmelt-predef.melt-tmp  melt-sources/warmelt-predef.melt


# MELT translator warmelt-normal in melt-sources/
melt-sources/warmelt-normal.c: melt-sources/warmelt-normal.melt melt-sources/warmelt-predef.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCFILE1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-normal.optimized.so: melt-sources/warmelt-normal.c \
        $(wildcard  melt-sources/warmelt-normal+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-normal 

melt-modules/warmelt-normal.debugnoline.so: melt-sources/warmelt-normal.c \
        $(wildcard  melt-sources/warmelt-normal+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-normal 

melt-modules/warmelt-normal.quicklybuilt.so: melt-sources/warmelt-normal.c \
        $(wildcard  melt-sources/warmelt-normal+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-normal \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-normal \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-normal




## melt translator warmelt-normatch # 5
melt-sources/warmelt-normatch.melt: $(melt_make_source_dir)/warmelt-normatch.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro


# MELT translator warmelt-normatch in melt-sources/
melt-sources/warmelt-normatch.c: melt-sources/warmelt-normatch.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCFILE1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-normatch.optimized.so: melt-sources/warmelt-normatch.c \
        $(wildcard  melt-sources/warmelt-normatch+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-normatch 

melt-modules/warmelt-normatch.debugnoline.so: melt-sources/warmelt-normatch.c \
        $(wildcard  melt-sources/warmelt-normatch+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-normatch 

melt-modules/warmelt-normatch.quicklybuilt.so: melt-sources/warmelt-normatch.c \
        $(wildcard  melt-sources/warmelt-normatch+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-normatch \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-normatch \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-normatch




## melt translator warmelt-genobj # 6
melt-sources/warmelt-genobj.melt: $(melt_make_source_dir)/warmelt-genobj.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro


# MELT translator warmelt-genobj in melt-sources/
melt-sources/warmelt-genobj.c: melt-sources/warmelt-genobj.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCFILE1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-genobj.optimized.so: melt-sources/warmelt-genobj.c \
        $(wildcard  melt-sources/warmelt-genobj+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-genobj 

melt-modules/warmelt-genobj.debugnoline.so: melt-sources/warmelt-genobj.c \
        $(wildcard  melt-sources/warmelt-genobj+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-genobj 

melt-modules/warmelt-genobj.quicklybuilt.so: melt-sources/warmelt-genobj.c \
        $(wildcard  melt-sources/warmelt-genobj+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-genobj \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-genobj \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-genobj




## melt translator warmelt-outobj # 7
melt-sources/warmelt-outobj.melt: $(melt_make_source_dir)/warmelt-outobj.melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro


# MELT translator warmelt-outobj in melt-sources/
melt-sources/warmelt-outobj.c: melt-sources/warmelt-outobj.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources 
	@rm -f $(notdir $(basename $@)sources.args)
	@echo  $(MELTCCFILE1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-modules): \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources):$(realpath $(melt_source_dir)) \
	     $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	     $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@))sources.args-tmp
	@mv $(notdir $(basename $@))sources.args-tmp $(notdir $(basename $@))sources.args
	@echo -n $(notdir $(basename $@))sources.args: ; cat $(notdir $(basename $@))sources.args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@))sources.args



melt-modules/warmelt-outobj.optimized.so: melt-sources/warmelt-outobj.c \
        $(wildcard  melt-sources/warmelt-outobj+*.c) \
        melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=optimized \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-outobj 

melt-modules/warmelt-outobj.debugnoline.so: melt-sources/warmelt-outobj.c \
        $(wildcard  melt-sources/warmelt-outobj+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-outobj 

melt-modules/warmelt-outobj.quicklybuilt.so: melt-sources/warmelt-outobj.c \
        $(wildcard  melt-sources/warmelt-outobj+*.c) \
         melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/warmelt-outobj \
              GCCMELT_MODULE_BINARYBASE=melt-modules/warmelt-outobj \
              GCCMELT_MODULE_WORKSPACE=melt-workdir 
# end translator warmelt-outobj





#### melt-sources warmelt-quicklybuilt is the sequence of translator files:
melt-sources/warmelt-quicklybuilt.modlis: melt-modules/warmelt-first.optimized.so  \
melt-modules/warmelt-base.optimized.so  \
melt-modules/warmelt-debug.optimized.so  \
melt-modules/warmelt-macro.optimized.so  \
melt-modules/warmelt-normal.optimized.so  \
melt-modules/warmelt-normatch.optimized.so  \
melt-modules/warmelt-genobj.optimized.so  \
melt-modules/warmelt-outobj.optimized.so 
	@echo building quicklybuilt module list $@
	date  +"# MELT warmelt-quicklybuilt list $@ generated %F" > $@-tmp
	echo "#  quicklybuilt translator files" >> $@-tmp
	echo warmelt-first.quicklybuilt >> $@-tmp
	echo warmelt-base.quicklybuilt >> $@-tmp
	echo warmelt-debug.quicklybuilt >> $@-tmp
	echo warmelt-macro.quicklybuilt >> $@-tmp
	echo warmelt-normal.quicklybuilt >> $@-tmp
	echo warmelt-normatch.quicklybuilt >> $@-tmp
	echo warmelt-genobj.quicklybuilt >> $@-tmp
	echo warmelt-outobj.quicklybuilt >> $@-tmp

	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


#### melt-sources warmelt-optimized is the sequence of translator files:
melt-sources/warmelt-optimized.modlis: melt-modules/warmelt-first.optimized.so  \
melt-modules/warmelt-base.optimized.so  \
melt-modules/warmelt-debug.optimized.so  \
melt-modules/warmelt-macro.optimized.so  \
melt-modules/warmelt-normal.optimized.so  \
melt-modules/warmelt-normatch.optimized.so  \
melt-modules/warmelt-genobj.optimized.so  \
melt-modules/warmelt-outobj.optimized.so 
	@echo building optimized module list $@
	date  +"# MELT warmelt-optimized list $@ generated %F" > $@-tmp
	echo "#  optimized translator files" >> $@-tmp
	echo warmelt-first.optimized >> $@-tmp
	echo warmelt-base.optimized >> $@-tmp
	echo warmelt-debug.optimized >> $@-tmp
	echo warmelt-macro.optimized >> $@-tmp
	echo warmelt-normal.optimized >> $@-tmp
	echo warmelt-normatch.optimized >> $@-tmp
	echo warmelt-genobj.optimized >> $@-tmp
	echo warmelt-outobj.optimized >> $@-tmp

	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


#### melt-sources warmelt-debugnoline is the sequence of translator files:
melt-sources/warmelt-debugnoline.modlis: melt-modules/warmelt-first.optimized.so  \
melt-modules/warmelt-base.optimized.so  \
melt-modules/warmelt-debug.optimized.so  \
melt-modules/warmelt-macro.optimized.so  \
melt-modules/warmelt-normal.optimized.so  \
melt-modules/warmelt-normatch.optimized.so  \
melt-modules/warmelt-genobj.optimized.so  \
melt-modules/warmelt-outobj.optimized.so 
	@echo building debugnoline module list $@
	date  +"# MELT warmelt-debugnoline list $@ generated %F" > $@-tmp
	echo "#  debugnoline translator files" >> $@-tmp
	echo warmelt-first.debugnoline >> $@-tmp
	echo warmelt-base.debugnoline >> $@-tmp
	echo warmelt-debug.debugnoline >> $@-tmp
	echo warmelt-macro.debugnoline >> $@-tmp
	echo warmelt-normal.debugnoline >> $@-tmp
	echo warmelt-normatch.debugnoline >> $@-tmp
	echo warmelt-genobj.debugnoline >> $@-tmp
	echo warmelt-outobj.debugnoline >> $@-tmp

	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@



#### melt-sources application files



## melt application xtramelt-parse-infix-syntax
melt-sources/xtramelt-parse-infix-syntax.melt: $(melt_make_source_dir)/xtramelt-parse-infix-syntax.melt
	cd melt-sources; rm -f xtramelt-parse-infix-syntax.melt; $(LN_S) $(realpath $^)


## melt application xtramelt-parse-infix-syntax generated files
melt-sources/xtramelt-parse-infix-syntax.c: melt-sources/xtramelt-parse-infix-syntax.melt melt-sources/warmelt-optimized.modlis  \
 melt-modules $(MELT_TRANSLATOR_SOURCE) \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources
	@rm -f $(notdir $(basename $@)).args
	@echo 	$(MELTCCAPPLICATION1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath melt-modules) \
	     $(meltarg_source_path)=$(realpath melt-sources) \
             $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_init)=@warmelt-optimized: \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@)).args-tmp
	@mv $(notdir $(basename $@)).args-tmp $(notdir $(basename $@)).args
	@echo -n $(notdir $(basename $@)).args: ; cat $(notdir $(basename $@)).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)).args

## melt application xtramelt-parse-infix-syntax various flavors of modules
melt-modules/xtramelt-parse-infix-syntax.optimized.so: melt-sources/xtramelt-parse-infix-syntax.c \
        $(wildcard  melt-sources/xtramelt-parse-infix-syntax+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=optimized \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-parse-infix-syntax \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-parse-infix-syntax 

melt-modules/xtramelt-parse-infix-syntax.debugnoline.so: melt-sources/xtramelt-parse-infix-syntax.c \
        $(wildcard  melt-sources/xtramelt-parse-infix-syntax+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-parse-infix-syntax \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-parse-infix-syntax 

melt-modules/xtramelt-parse-infix-syntax.quicklybuilt.so: melt-sources/xtramelt-parse-infix-syntax.c \
        $(wildcard  melt-sources/xtramelt-parse-infix-syntax+*.c) \
        melt-run.h melt-runtime.h  | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-parse-infix-syntax \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-parse-infix-syntax 


# end application xtramelt-parse-infix-syntax



## melt application xtramelt-ana-base
melt-sources/xtramelt-ana-base.melt: $(melt_make_source_dir)/xtramelt-ana-base.melt
	cd melt-sources; rm -f xtramelt-ana-base.melt; $(LN_S) $(realpath $^)


## melt application xtramelt-ana-base generated files
melt-sources/xtramelt-ana-base.c: melt-sources/xtramelt-ana-base.melt melt-sources/warmelt-optimized.modlis  \
 melt-modules $(MELT_TRANSLATOR_SOURCE) \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources
	@rm -f $(notdir $(basename $@)).args
	@echo 	$(MELTCCAPPLICATION1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath melt-modules) \
	     $(meltarg_source_path)=$(realpath melt-sources) \
             $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_init)=@warmelt-optimized:xtramelt-parse-infix-syntax \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@)).args-tmp
	@mv $(notdir $(basename $@)).args-tmp $(notdir $(basename $@)).args
	@echo -n $(notdir $(basename $@)).args: ; cat $(notdir $(basename $@)).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)).args

## melt application xtramelt-ana-base various flavors of modules
melt-modules/xtramelt-ana-base.optimized.so: melt-sources/xtramelt-ana-base.c \
        $(wildcard  melt-sources/xtramelt-ana-base+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=optimized \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-ana-base \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-ana-base 

melt-modules/xtramelt-ana-base.debugnoline.so: melt-sources/xtramelt-ana-base.c \
        $(wildcard  melt-sources/xtramelt-ana-base+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-ana-base \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-ana-base 

melt-modules/xtramelt-ana-base.quicklybuilt.so: melt-sources/xtramelt-ana-base.c \
        $(wildcard  melt-sources/xtramelt-ana-base+*.c) \
        melt-run.h melt-runtime.h  | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-ana-base \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-ana-base 


# end application xtramelt-ana-base



## melt application xtramelt-ana-simple
melt-sources/xtramelt-ana-simple.melt: $(melt_make_source_dir)/xtramelt-ana-simple.melt
	cd melt-sources; rm -f xtramelt-ana-simple.melt; $(LN_S) $(realpath $^)


## melt application xtramelt-ana-simple generated files
melt-sources/xtramelt-ana-simple.c: melt-sources/xtramelt-ana-simple.melt melt-sources/warmelt-optimized.modlis  \
 melt-modules $(MELT_TRANSLATOR_SOURCE) \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources
	@rm -f $(notdir $(basename $@)).args
	@echo 	$(MELTCCAPPLICATION1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath melt-modules) \
	     $(meltarg_source_path)=$(realpath melt-sources) \
             $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_init)=@warmelt-optimized:xtramelt-parse-infix-syntax:xtramelt-ana-base \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@)).args-tmp
	@mv $(notdir $(basename $@)).args-tmp $(notdir $(basename $@)).args
	@echo -n $(notdir $(basename $@)).args: ; cat $(notdir $(basename $@)).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)).args

## melt application xtramelt-ana-simple various flavors of modules
melt-modules/xtramelt-ana-simple.optimized.so: melt-sources/xtramelt-ana-simple.c \
        $(wildcard  melt-sources/xtramelt-ana-simple+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=optimized \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-ana-simple \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-ana-simple 

melt-modules/xtramelt-ana-simple.debugnoline.so: melt-sources/xtramelt-ana-simple.c \
        $(wildcard  melt-sources/xtramelt-ana-simple+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-ana-simple \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-ana-simple 

melt-modules/xtramelt-ana-simple.quicklybuilt.so: melt-sources/xtramelt-ana-simple.c \
        $(wildcard  melt-sources/xtramelt-ana-simple+*.c) \
        melt-run.h melt-runtime.h  | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-ana-simple \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-ana-simple 


# end application xtramelt-ana-simple



## melt application xtramelt-c-generator
melt-sources/xtramelt-c-generator.melt: $(melt_make_source_dir)/xtramelt-c-generator.melt
	cd melt-sources; rm -f xtramelt-c-generator.melt; $(LN_S) $(realpath $^)


## melt application xtramelt-c-generator generated files
melt-sources/xtramelt-c-generator.c: melt-sources/xtramelt-c-generator.melt melt-sources/warmelt-optimized.modlis  \
 melt-modules $(MELT_TRANSLATOR_SOURCE) \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources
	@rm -f $(notdir $(basename $@)).args
	@echo 	$(MELTCCAPPLICATION1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath melt-modules) \
	     $(meltarg_source_path)=$(realpath melt-sources) \
             $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_init)=@warmelt-optimized:xtramelt-parse-infix-syntax:xtramelt-ana-base:xtramelt-ana-simple \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@)).args-tmp
	@mv $(notdir $(basename $@)).args-tmp $(notdir $(basename $@)).args
	@echo -n $(notdir $(basename $@)).args: ; cat $(notdir $(basename $@)).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)).args

## melt application xtramelt-c-generator various flavors of modules
melt-modules/xtramelt-c-generator.optimized.so: melt-sources/xtramelt-c-generator.c \
        $(wildcard  melt-sources/xtramelt-c-generator+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=optimized \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-c-generator \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-c-generator 

melt-modules/xtramelt-c-generator.debugnoline.so: melt-sources/xtramelt-c-generator.c \
        $(wildcard  melt-sources/xtramelt-c-generator+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-c-generator \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-c-generator 

melt-modules/xtramelt-c-generator.quicklybuilt.so: melt-sources/xtramelt-c-generator.c \
        $(wildcard  melt-sources/xtramelt-c-generator+*.c) \
        melt-run.h melt-runtime.h  | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-c-generator \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-c-generator 


# end application xtramelt-c-generator



## melt application xtramelt-opengpu
melt-sources/xtramelt-opengpu.melt: $(melt_make_source_dir)/xtramelt-opengpu.melt
	cd melt-sources; rm -f xtramelt-opengpu.melt; $(LN_S) $(realpath $^)


## melt application xtramelt-opengpu generated files
melt-sources/xtramelt-opengpu.c: melt-sources/xtramelt-opengpu.melt melt-sources/warmelt-optimized.modlis  \
 melt-modules $(MELT_TRANSLATOR_SOURCE) \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) | melt-sources
	@rm -f $(notdir $(basename $@)).args
	@echo 	$(MELTCCAPPLICATION1ARGS) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath melt-modules) \
	     $(meltarg_source_path)=$(realpath melt-sources) \
             $(meltarg_workdir)=melt-workdir $(meltarg_inhibitautobuild) \
	     $(meltarg_init)=@warmelt-optimized:xtramelt-parse-infix-syntax:xtramelt-ana-base:xtramelt-ana-simple:xtramelt-c-generator \
	     $(meltarg_output)=$(basename $@) empty-file-for-melt.c > $(notdir $(basename $@)).args-tmp
	@mv $(notdir $(basename $@)).args-tmp $(notdir $(basename $@)).args
	@echo -n $(notdir $(basename $@)).args: ; cat $(notdir $(basename $@)).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)).args

## melt application xtramelt-opengpu various flavors of modules
melt-modules/xtramelt-opengpu.optimized.so: melt-sources/xtramelt-opengpu.c \
        $(wildcard  melt-sources/xtramelt-opengpu+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=optimized \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-opengpu \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-opengpu 

melt-modules/xtramelt-opengpu.debugnoline.so: melt-sources/xtramelt-opengpu.c \
        $(wildcard  melt-sources/xtramelt-opengpu+*.c) \
        melt-run.h melt-runtime.h | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=debugnoline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-opengpu \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-opengpu 

melt-modules/xtramelt-opengpu.quicklybuilt.so: melt-sources/xtramelt-opengpu.c \
        $(wildcard  melt-sources/xtramelt-opengpu+*.c) \
        melt-run.h melt-runtime.h  | melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_MODULE_FLAVOR=quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-workdir \
	      GCCMELT_MODULE_SOURCEBASE=melt-sources/xtramelt-opengpu \
              GCCMELT_MODULE_BINARYBASE=melt-modules/xtramelt-opengpu 


# end application xtramelt-opengpu



######

melt-all-modules:  melt-workdir \
    melt-modules/warmelt-first.optimized.so \
    melt-modules/warmelt-base.optimized.so \
    melt-modules/warmelt-debug.optimized.so \
    melt-modules/warmelt-macro.optimized.so \
    melt-modules/warmelt-normal.optimized.so \
    melt-modules/warmelt-normatch.optimized.so \
    melt-modules/warmelt-genobj.optimized.so \
    melt-modules/warmelt-outobj.optimized.so \
 \
    melt-modules/warmelt-first.quicklybuilt.so \
    melt-modules/warmelt-base.quicklybuilt.so \
    melt-modules/warmelt-debug.quicklybuilt.so \
    melt-modules/warmelt-macro.quicklybuilt.so \
    melt-modules/warmelt-normal.quicklybuilt.so \
    melt-modules/warmelt-normatch.quicklybuilt.so \
    melt-modules/warmelt-genobj.quicklybuilt.so \
    melt-modules/warmelt-outobj.quicklybuilt.so \
 \
    melt-modules/warmelt-first.debugnoline.so \
    melt-modules/warmelt-base.debugnoline.so \
    melt-modules/warmelt-debug.debugnoline.so \
    melt-modules/warmelt-macro.debugnoline.so \
    melt-modules/warmelt-normal.debugnoline.so \
    melt-modules/warmelt-normatch.debugnoline.so \
    melt-modules/warmelt-genobj.debugnoline.so \
    melt-modules/warmelt-outobj.debugnoline.so \
 \
    \
    melt-modules/xtramelt-parse-infix-syntax.optimized.so \
    melt-modules/xtramelt-ana-base.optimized.so \
    melt-modules/xtramelt-ana-simple.optimized.so \
    melt-modules/xtramelt-c-generator.optimized.so \
    melt-modules/xtramelt-opengpu.optimized.so \
 \
    melt-modules/xtramelt-parse-infix-syntax.quicklybuilt.so \
    melt-modules/xtramelt-ana-base.quicklybuilt.so \
    melt-modules/xtramelt-ana-simple.quicklybuilt.so \
    melt-modules/xtramelt-c-generator.quicklybuilt.so \
    melt-modules/xtramelt-opengpu.quicklybuilt.so \
 \
    melt-modules/xtramelt-parse-infix-syntax.debugnoline.so \
    melt-modules/xtramelt-ana-base.debugnoline.so \
    melt-modules/xtramelt-ana-simple.debugnoline.so \
    melt-modules/xtramelt-c-generator.debugnoline.so \
    melt-modules/xtramelt-opengpu.debugnoline.so \


$(melt_default_modules_list).modlis: melt-all-modules \
       $(melt_default_modules_list)-quicklybuilt.modlis \
       $(melt_default_modules_list)-optimized.modlis \
       $(melt_default_modules_list)-debugnoline.modlis
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(melt_default_modules_list)-$(melt_default_variant).modlis  $(notdir $@)

## MELT various variants of module lists


### quicklybuilt default module list
$(melt_default_modules_list)-quicklybuilt.modlis:  melt-all-modules  melt-modules/ $(wildcard melt-modules/*.quicklybuilt.so)
	@echo building quicklybuilt module list $@
	date  +"# MELT module  quicklybuilt list $@ generated %F" > $@-tmp
	echo "#  quicklybuilt translator files" >> $@-tmp
	echo warmelt-first.quicklybuilt >> $@-tmp
	echo warmelt-base.quicklybuilt >> $@-tmp
	echo warmelt-debug.quicklybuilt >> $@-tmp
	echo warmelt-macro.quicklybuilt >> $@-tmp
	echo warmelt-normal.quicklybuilt >> $@-tmp
	echo warmelt-normatch.quicklybuilt >> $@-tmp
	echo warmelt-genobj.quicklybuilt >> $@-tmp
	echo warmelt-outobj.quicklybuilt >> $@-tmp

	echo "#  quicklybuilt application files" >> $@-tmp
	echo xtramelt-parse-infix-syntax.quicklybuilt >> $@-tmp
	echo xtramelt-ana-base.quicklybuilt >> $@-tmp
	echo xtramelt-ana-simple.quicklybuilt >> $@-tmp
	echo xtramelt-c-generator.quicklybuilt >> $@-tmp
	echo xtramelt-opengpu.quicklybuilt >> $@-tmp

	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


### optimized default module list
$(melt_default_modules_list)-optimized.modlis:  melt-all-modules  melt-modules/ $(wildcard melt-modules/*.optimized.so)
	@echo building optimized module list $@
	date  +"# MELT module  optimized list $@ generated %F" > $@-tmp
	echo "#  optimized translator files" >> $@-tmp
	echo warmelt-first.optimized >> $@-tmp
	echo warmelt-base.optimized >> $@-tmp
	echo warmelt-debug.optimized >> $@-tmp
	echo warmelt-macro.optimized >> $@-tmp
	echo warmelt-normal.optimized >> $@-tmp
	echo warmelt-normatch.optimized >> $@-tmp
	echo warmelt-genobj.optimized >> $@-tmp
	echo warmelt-outobj.optimized >> $@-tmp

	echo "#  optimized application files" >> $@-tmp
	echo xtramelt-parse-infix-syntax.optimized >> $@-tmp
	echo xtramelt-ana-base.optimized >> $@-tmp
	echo xtramelt-ana-simple.optimized >> $@-tmp
	echo xtramelt-c-generator.optimized >> $@-tmp
	echo xtramelt-opengpu.optimized >> $@-tmp

	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


### debugnoline default module list
$(melt_default_modules_list)-debugnoline.modlis:  melt-all-modules  melt-modules/ $(wildcard melt-modules/*.debugnoline.so)
	@echo building debugnoline module list $@
	date  +"# MELT module  debugnoline list $@ generated %F" > $@-tmp
	echo "#  debugnoline translator files" >> $@-tmp
	echo warmelt-first.debugnoline >> $@-tmp
	echo warmelt-base.debugnoline >> $@-tmp
	echo warmelt-debug.debugnoline >> $@-tmp
	echo warmelt-macro.debugnoline >> $@-tmp
	echo warmelt-normal.debugnoline >> $@-tmp
	echo warmelt-normatch.debugnoline >> $@-tmp
	echo warmelt-genobj.debugnoline >> $@-tmp
	echo warmelt-outobj.debugnoline >> $@-tmp

	echo "#  debugnoline application files" >> $@-tmp
	echo xtramelt-parse-infix-syntax.debugnoline >> $@-tmp
	echo xtramelt-ana-base.debugnoline >> $@-tmp
	echo xtramelt-ana-simple.debugnoline >> $@-tmp
	echo xtramelt-c-generator.debugnoline >> $@-tmp
	echo xtramelt-opengpu.debugnoline >> $@-tmp

	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@



### MELT upgrade
.PHONY: warmelt-upgrade-translator

warmelt-upgrade-translator: $(WARMELT_LAST) \
   $(MELT_LAST_STAGE)/warmelt-first.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-first+*.c) \
   $(MELT_LAST_STAGE)/warmelt-base.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-base+*.c) \
   $(MELT_LAST_STAGE)/warmelt-debug.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-debug+*.c) \
   $(MELT_LAST_STAGE)/warmelt-macro.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-macro+*.c) \
   $(MELT_LAST_STAGE)/warmelt-normal.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-normal+*.c) \
   $(MELT_LAST_STAGE)/warmelt-normatch.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-normatch+*.c) \
   $(MELT_LAST_STAGE)/warmelt-genobj.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-genobj+*.c) \
   $(MELT_LAST_STAGE)/warmelt-outobj.c \
         $(wildcard  $(MELT_LAST_STAGE)/warmelt-outobj+*.c)
	@echo upgrading the MELT translator
	@which unifdef || (echo missing unifdef for warmelt-upgrade-translator; exit 1)
	@which indent || (echo missing indent for warmelt-upgrade-translator; exit 1)

	@echo upgrading MELT translator warmelt-first
## dont indent the warmelt-first+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-first+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-first+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-first+meltdesc.c > $(srcdir)/melt/generated/warmelt-first+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-first.c $(MELT_LAST_STAGE)/warmelt-first+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-first*.o

	@echo upgrading MELT translator warmelt-base
## dont indent the warmelt-base+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-base+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-base+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-base+meltdesc.c > $(srcdir)/melt/generated/warmelt-base+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-base.c $(MELT_LAST_STAGE)/warmelt-base+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-base*.o

	@echo upgrading MELT translator warmelt-debug
## dont indent the warmelt-debug+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-debug+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-debug+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-debug+meltdesc.c > $(srcdir)/melt/generated/warmelt-debug+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-debug.c $(MELT_LAST_STAGE)/warmelt-debug+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-debug*.o

	@echo upgrading MELT translator warmelt-macro
## dont indent the warmelt-macro+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-macro+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-macro+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-macro+meltdesc.c > $(srcdir)/melt/generated/warmelt-macro+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-macro.c $(MELT_LAST_STAGE)/warmelt-macro+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-macro*.o

	@echo upgrading MELT translator warmelt-normal
## dont indent the warmelt-normal+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-normal+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-normal+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-normal+meltdesc.c > $(srcdir)/melt/generated/warmelt-normal+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-normal.c $(MELT_LAST_STAGE)/warmelt-normal+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-normal*.o

	@echo upgrading MELT translator warmelt-normatch
## dont indent the warmelt-normatch+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-normatch+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-normatch+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-normatch+meltdesc.c > $(srcdir)/melt/generated/warmelt-normatch+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-normatch.c $(MELT_LAST_STAGE)/warmelt-normatch+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-normatch*.o

	@echo upgrading MELT translator warmelt-genobj
## dont indent the warmelt-genobj+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-genobj+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-genobj+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-genobj+meltdesc.c > $(srcdir)/melt/generated/warmelt-genobj+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-genobj.c $(MELT_LAST_STAGE)/warmelt-genobj+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-genobj*.o

	@echo upgrading MELT translator warmelt-outobj
## dont indent the warmelt-outobj+meltdesc.c 
	$(melt_make_move) $(MELT_LAST_STAGE)/warmelt-outobj+meltdesc.c  $(MELT_LAST_STAGE)/warmelt-outobj+meltdesc.c~; \
	sed s/$(MELT_LAST_STAGE)/MELT-STAGE-ZERO/g $(MELT_LAST_STAGE)/warmelt-outobj+meltdesc.c > $(srcdir)/melt/generated/warmelt-outobj+meltdesc.c 
	for f in $(MELT_LAST_STAGE)/warmelt-outobj.c $(MELT_LAST_STAGE)/warmelt-outobj+[0-9]*.c ; do \
	  bf=`basename $$f`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING | indent \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             $(melt_make_move) $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  $(melt_make_move) $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done
	rm $(MELT_STAGE_ZERO)/warmelt-outobj*.o



### Generated MELT documentation
meltgendoc.texi: $(melt_default_modules_list).modlis \
                    melt-sources/warmelt-first.melt \
                    melt-sources/warmelt-base.melt \
                    melt-sources/warmelt-debug.melt \
                    melt-sources/warmelt-macro.melt \
                    melt-sources/warmelt-normal.melt \
                    melt-sources/warmelt-normatch.melt \
                    melt-sources/warmelt-genobj.melt \
                    melt-sources/warmelt-outobj.melt \
                    melt-sources/xtramelt-parse-infix-syntax.melt \
                    melt-sources/xtramelt-ana-base.melt \
                    melt-sources/xtramelt-ana-simple.melt \
                    melt-sources/xtramelt-c-generator.melt \
                    melt-sources/xtramelt-opengpu.melt \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	echo $(melt_make_cc1flags) $(meltarg_mode)=makedoc  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_workdir)=melt-workdir  $(meltarg_bootstrapping)  $(MELT_DEBUG) \
	      $(meltarg_init)=@$(melt_default_modules_list) \
	      $(meltarg_module_path)=$(realpath melt-modules):. \
	      $(meltarg_source_path)=$(realpath melt-sources):. \
	      $(meltarg_output)=$(basename $@)  \
              $(meltarg_arglist)=warmelt-first.melt,warmelt-base.melt,warmelt-debug.melt,warmelt-macro.melt,warmelt-normal.melt,warmelt-normatch.melt,warmelt-genobj.melt,warmelt-outobj.melt,\
xtramelt-parse-infix-syntax.melt,xtramelt-ana-base.melt,xtramelt-ana-simple.melt,xtramelt-c-generator.melt,xtramelt-opengpu.melt \
              empty-file-for-melt.c > $(notdir $(basename $@)).args-tmp
	mv  $(notdir $(basename $@)).args-tmp  $(notdir $(basename $@)).args
	@echo -n $(notdir $(basename $@)).args: ; cat $(notdir $(basename $@)).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(notdir $(basename $@)).args


vpath %.so $(melt_make_module_dir) . 
vpath %.c $(melt_make_source_dir)/generated . $(melt_source_dir) 
vpath %.h $(melt_make_source_dir)/generated . $(melt_source_dir)




.PHONY: meltrun-generate
meltrun-generate: $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) empty-file-for-melt.c \
                   $(melt_make_cc1_dependency)
	rm -f $(wildcard meltrunsup*)
	@echo  \
	      $(meltarg_mode)=runtypesupport  \
	      $(meltarg_tempdir)=.  $(meltarg_bootstrapping)  $(MELT_DEBUG) \
	      $(meltarg_init)=@$(basename $(WARMELT_LAST_MODLIS)) \
	      $(meltarg_module_path)=$(MELT_LAST_STAGE):. \
	      $(meltarg_source_path)=$(MELT_LAST_STAGE):$(melt_source_dir):. \
	      $(meltarg_output)=meltrunsup  \
	      empty-file-for-melt.c > $(basename $@).args-tmp
	@mv $(basename $@).args-tmp $(basename $@).args
	@echo -n $(basename $@).args: ; cat $(basename $@).args ; echo "***** doing " $@
	 $(melt_make_cc1flags) @$(basename $@).args
	if [ -n "$(GCCMELTRUNGEN_DEST)" ]; then \
	   for f in $(GCCMELTRUNGEN_DEST)/meltrunsup*.[ch]; \
	     do mv $$f $$f.bak; \
	   done; \
	   cp -v meltrunsup*.[ch] "$(GCCMELTRUNGEN_DEST)" ; \
        fi

### MELT cleanup
.PHONY: melt-clean
melt-clean:
	rm -rf *melt*.args melt-stage0-quicklybuilt melt-stage0-dynamic \
	       melt-stage0-quicklybuilt.stamp melt-stage0-dynamic.stamp \
           melt-stage1  melt-stage1.stamp \
           melt-stage2  melt-stage2.stamp \
           melt-stage3  melt-stage3.stamp \
               melt-sources melt-modules

## eof melt-build.mk generated from melt-build.tpl & melt-melt-build.def
