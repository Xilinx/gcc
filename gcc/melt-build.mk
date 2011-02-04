
# melt-build.mk is generated from melt-build.tpl by 'autogen melt-build.def'
# DON'T EDIT melt-build.mk but only edit: melt-build.tpl or melt-build.def
#
# Makefile fragment for MELT modules and MELT translator bootstrap.
#   Copyright (C) 2010  Free Software Foundation
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
### melt_make_cc1 - cc1 program with MELT (or loading MELT plugin) or gcc -fplugin=melt.so
### melt_make_cc1_dependency - the make dependency for above i.e. cc1$(exeext) for MELT branch
### melt_make_gencdeps is an extra make dependency of generated C files [leave empty usually]
### melt_is_plugin - should be non empty in plugin mode
### melt_make_move - a copy or move command for files
### melt_cflags - the CFLAGS for compiling MELT generated C code
### melt_xtra_cflags - the CFLAGS for compiling extra (applicative)
###                    MELT generated C code

## the various arguments to MELT - avoid spaces in them!
meltarg_mode=$(if $(melt_is_plugin),-fplugin-arg-melt-mode,-fmelt-mode)
meltarg_init=$(if $(melt_is_plugin),-fplugin-arg-melt-init,-fmelt-init)
meltarg_module_path=$(if $(melt_is_plugin),-fplugin-arg-melt-module-path,-fmelt-module-path)
meltarg_source_path=$(if $(melt_is_plugin),-fplugin-arg-melt-source-path,-fmelt-source-path)
meltarg_tempdir=$(if $(melt_is_plugin),-fplugin-arg-melt-tempdir,-fmelt-tempdir)

meltarg_arg=$(if $(melt_is_plugin),-fplugin-arg-melt-arg,-fmelt-arg)
meltarg_bootstrapping=$(if $(melt_is_plugin),-fplugin-arg-melt-bootstrapping,-fmelt-bootstrapping)
meltarg_makefile=$(if $(melt_is_plugin),-fplugin-arg-melt-module-makefile,-fmelt-module-makefile)
meltarg_makecmd=$(if $(melt_is_plugin),-fplugin-arg-melt-module-make-command,-fmelt-module-make-command)
meltarg_arglist=$(if $(melt_is_plugin),-fplugin-arg-melt-arglist,-fmelt-arglist)
meltarg_output=$(if $(melt_is_plugin),-fplugin-arg-melt-output,-fmelt-output)

## MELT_DEBUG could be set to -fmelt-debug or -fplugin-arg-melt-debug
## the invocation to translate the very first initial MELT file
MELTCCINIT1=$(melt_make_cc1) $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translateinit  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_tempdir)=. $(meltarg_bootstrapping) $(MELT_DEBUG)

## the invocation to translate the other files
MELTCCFILE1=$(melt_make_cc1)  $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_tempdir)=. $(meltarg_bootstrapping)  $(MELT_DEBUG)

## the invocation to translate the application files -don't pass the -fmelt-bootstrap flag
MELTCCAPPLICATION1=$(melt_make_cc1)  $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_tempdir)=. $(MELT_DEBUG)


vpath %.so $(melt_make_module_dir) . 
vpath %.c $(melt_make_source_dir)/generated . $(melt_source_dir)
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

## The base name of the MELT application files
MELT_APPLICATION_BASE= \
    xtramelt-parse-infix-syntax \
  xtramelt-ana-base \
  xtramelt-ana-simple \
  xtramelt-c-generator \
  xtramelt-opengpu



## The cold stage 0 of the translator

MELT_GENERATED_FIRST_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-first-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-first-0+*.c)
MELT_GENERATED_FIRST_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_FIRST_C_FILES)))


MELT_GENERATED_BASE_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-base-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-base-0+*.c)
MELT_GENERATED_BASE_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_BASE_C_FILES)))


MELT_GENERATED_DEBUG_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-debug-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-debug-0+*.c)
MELT_GENERATED_DEBUG_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_DEBUG_C_FILES)))


MELT_GENERATED_MACRO_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-macro-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-macro-0+*.c)
MELT_GENERATED_MACRO_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_MACRO_C_FILES)))


MELT_GENERATED_NORMAL_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-normal-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-normal-0+*.c)
MELT_GENERATED_NORMAL_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_NORMAL_C_FILES)))


MELT_GENERATED_NORMATCH_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-normatch-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-normatch-0+*.c)
MELT_GENERATED_NORMATCH_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_NORMATCH_C_FILES)))


MELT_GENERATED_GENOBJ_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-genobj-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-genobj-0+*.c)
MELT_GENERATED_GENOBJ_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_GENOBJ_C_FILES)))


MELT_GENERATED_OUTOBJ_C_FILES= \
                  $(melt_make_source_dir)/generated/warmelt-outobj-0.c \
                  $(wildcard $(melt_make_source_dir)/generated/warmelt-outobj-0+*.c)
MELT_GENERATED_OUTOBJ_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_OUTOBJ_C_FILES)))



## the rules to build the static and dynamic version of stage0, that
## is with static or dynamic field object offsets

## using static object fields offsets for warmelt-first
melt-stage0-static/warmelt-first-0.so: $(MELT_GENERATED_FIRST_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-first-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-first-0

## using dynamic object fields offsets for warmelt-first
melt-stage0-dynamic/warmelt-first-0.d.so: $(MELT_GENERATED_FIRST_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-first-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-first-0

melt-stage0-dynamic/warmelt-first-0.so: melt-stage0-dynamic/warmelt-first-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-first-0.so; $(LN_S) warmelt-first-0.d.so warmelt-first-0.so


## using static object fields offsets for warmelt-base
melt-stage0-static/warmelt-base-0.so: $(MELT_GENERATED_BASE_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-base-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-base-0

## using dynamic object fields offsets for warmelt-base
melt-stage0-dynamic/warmelt-base-0.d.so: $(MELT_GENERATED_BASE_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-base-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-base-0

melt-stage0-dynamic/warmelt-base-0.so: melt-stage0-dynamic/warmelt-base-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-base-0.so; $(LN_S) warmelt-base-0.d.so warmelt-base-0.so


## using static object fields offsets for warmelt-debug
melt-stage0-static/warmelt-debug-0.so: $(MELT_GENERATED_DEBUG_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-debug-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-debug-0

## using dynamic object fields offsets for warmelt-debug
melt-stage0-dynamic/warmelt-debug-0.d.so: $(MELT_GENERATED_DEBUG_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-debug-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-debug-0

melt-stage0-dynamic/warmelt-debug-0.so: melt-stage0-dynamic/warmelt-debug-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-debug-0.so; $(LN_S) warmelt-debug-0.d.so warmelt-debug-0.so


## using static object fields offsets for warmelt-macro
melt-stage0-static/warmelt-macro-0.so: $(MELT_GENERATED_MACRO_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-macro-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-macro-0

## using dynamic object fields offsets for warmelt-macro
melt-stage0-dynamic/warmelt-macro-0.d.so: $(MELT_GENERATED_MACRO_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-macro-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-macro-0

melt-stage0-dynamic/warmelt-macro-0.so: melt-stage0-dynamic/warmelt-macro-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-macro-0.so; $(LN_S) warmelt-macro-0.d.so warmelt-macro-0.so


## using static object fields offsets for warmelt-normal
melt-stage0-static/warmelt-normal-0.so: $(MELT_GENERATED_NORMAL_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-normal-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-normal-0

## using dynamic object fields offsets for warmelt-normal
melt-stage0-dynamic/warmelt-normal-0.d.so: $(MELT_GENERATED_NORMAL_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-normal-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-normal-0

melt-stage0-dynamic/warmelt-normal-0.so: melt-stage0-dynamic/warmelt-normal-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-normal-0.so; $(LN_S) warmelt-normal-0.d.so warmelt-normal-0.so


## using static object fields offsets for warmelt-normatch
melt-stage0-static/warmelt-normatch-0.so: $(MELT_GENERATED_NORMATCH_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-normatch-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-normatch-0

## using dynamic object fields offsets for warmelt-normatch
melt-stage0-dynamic/warmelt-normatch-0.d.so: $(MELT_GENERATED_NORMATCH_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-normatch-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-normatch-0

melt-stage0-dynamic/warmelt-normatch-0.so: melt-stage0-dynamic/warmelt-normatch-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-normatch-0.so; $(LN_S) warmelt-normatch-0.d.so warmelt-normatch-0.so


## using static object fields offsets for warmelt-genobj
melt-stage0-static/warmelt-genobj-0.so: $(MELT_GENERATED_GENOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-genobj-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-genobj-0

## using dynamic object fields offsets for warmelt-genobj
melt-stage0-dynamic/warmelt-genobj-0.d.so: $(MELT_GENERATED_GENOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-genobj-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-genobj-0

melt-stage0-dynamic/warmelt-genobj-0.so: melt-stage0-dynamic/warmelt-genobj-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-genobj-0.so; $(LN_S) warmelt-genobj-0.d.so warmelt-genobj-0.so


## using static object fields offsets for warmelt-outobj
melt-stage0-static/warmelt-outobj-0.so: $(MELT_GENERATED_OUTOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-outobj-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-static/warmelt-outobj-0

## using dynamic object fields offsets for warmelt-outobj
melt-stage0-dynamic/warmelt-outobj-0.d.so: $(MELT_GENERATED_OUTOBJ_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/warmelt-outobj-0.c \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/warmelt-outobj-0

melt-stage0-dynamic/warmelt-outobj-0.so: melt-stage0-dynamic/warmelt-outobj-0.d.so
	cd melt-stage0-dynamic/ ; rm -f warmelt-outobj-0.so; $(LN_S) warmelt-outobj-0.d.so warmelt-outobj-0.so




melt-stage0-static.timestamp:  melt-stage0-static melt-stage0-static/warmelt-0.modlis
	date +"$@ %c" > $@

melt-stage0-dynamic.timestamp:  melt-stage0-dynamic melt-stage0-dynamic/warmelt-0.modlis
	date +"$@ %c" > $@




melt-stage0-static/warmelt-0.modlis: \
             melt-stage0-static/warmelt-first-0.so \
             melt-stage0-static/warmelt-base-0.so \
             melt-stage0-static/warmelt-debug-0.so \
             melt-stage0-static/warmelt-macro-0.so \
             melt-stage0-static/warmelt-normal-0.so \
             melt-stage0-static/warmelt-normatch-0.so \
             melt-stage0-static/warmelt-genobj-0.so \
             melt-stage0-static/warmelt-outobj-0.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-0 >> $@-tmp
	echo warmelt-base-0 >> $@-tmp
	echo warmelt-debug-0 >> $@-tmp
	echo warmelt-macro-0 >> $@-tmp
	echo warmelt-normal-0 >> $@-tmp
	echo warmelt-normatch-0 >> $@-tmp
	echo warmelt-genobj-0 >> $@-tmp
	echo warmelt-outobj-0 >> $@-tmp
	$(melt_make_move) $@-tmp $@

melt-stage0-dynamic/warmelt-0.modlis: \
              melt-stage0-dynamic/warmelt-first-0.d.so \
              melt-stage0-dynamic/warmelt-base-0.d.so \
              melt-stage0-dynamic/warmelt-debug-0.d.so \
              melt-stage0-dynamic/warmelt-macro-0.d.so \
              melt-stage0-dynamic/warmelt-normal-0.d.so \
              melt-stage0-dynamic/warmelt-normatch-0.d.so \
              melt-stage0-dynamic/warmelt-genobj-0.d.so \
              melt-stage0-dynamic/warmelt-outobj-0.d.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-0 >> $@-tmp
	echo warmelt-base-0 >> $@-tmp
	echo warmelt-debug-0 >> $@-tmp
	echo warmelt-macro-0 >> $@-tmp
	echo warmelt-normal-0 >> $@-tmp
	echo warmelt-normatch-0 >> $@-tmp
	echo warmelt-genobj-0 >> $@-tmp
	echo warmelt-outobj-0 >> $@-tmp

	$(melt_make_move) $@-tmp $@

## An empty file is needed for every MELT translation!
empty-file-for-melt.c:
	date +"/* empty-file-for-melt.c %c */" > $@-tmp
	mv $@-tmp $@

## can be overridden manually to either melt-stage0-dynamic or
## melt-stage0-static
.PHONY: warmelt0
## the default stage0 melt-stage0-dynamic
MELT_STAGE_ZERO?= melt-stage0-dynamic
warmelt0: $(melt_make_cc1_dependency) $(MELT_STAGE_ZERO) $(MELT_STAGE_ZERO).timestamp 
$(MELT_STAGE_ZERO):
	-test -d $(MELT_STAGE_ZERO)/ || mkdir $(MELT_STAGE_ZERO)


#### rules for melt-stage1, previous $(MELT_STAGE_ZERO)


################## warmelt-first for melt-stage1
melt-stage1/warmelt-first-1.so: melt-stage1/warmelt-first-1.c \
              $(wildcard melt-stage1/warmelt-first-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-first-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-first-1

melt-stage1/warmelt-first-1.n.so: melt-stage1/warmelt-first-1.c \
              $(wildcard melt-stage1/warmelt-first-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-first-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-first-1

melt-stage1/warmelt-first-1.c: $(melt_make_source_dir)/warmelt-first.melt \
                  $(MELT_STAGE_ZERO)/warmelt-first-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-base-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-debug-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-macro-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normal-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normatch-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-genobj-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)=\
warmelt-first-0:\
warmelt-base-0:\
warmelt-debug-0:\
warmelt-macro-0:\
warmelt-normal-0:\
warmelt-normatch-0:\
warmelt-genobj-0:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-base for melt-stage1
melt-stage1/warmelt-base-1.so: melt-stage1/warmelt-base-1.c \
              $(wildcard melt-stage1/warmelt-base-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-base-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-base-1

melt-stage1/warmelt-base-1.n.so: melt-stage1/warmelt-base-1.c \
              $(wildcard melt-stage1/warmelt-base-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-base-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-base-1

melt-stage1/warmelt-base-1.c: $(melt_make_source_dir)/warmelt-base.melt \
                  melt-stage1/warmelt-first-1.so \
                  $(MELT_STAGE_ZERO)/warmelt-base-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-debug-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-macro-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normal-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normatch-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-genobj-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-0:\
warmelt-debug-0:\
warmelt-macro-0:\
warmelt-normal-0:\
warmelt-normatch-0:\
warmelt-genobj-0:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-debug for melt-stage1
melt-stage1/warmelt-debug-1.so: melt-stage1/warmelt-debug-1.c \
              $(wildcard melt-stage1/warmelt-debug-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-debug-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-debug-1

melt-stage1/warmelt-debug-1.n.so: melt-stage1/warmelt-debug-1.c \
              $(wildcard melt-stage1/warmelt-debug-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-debug-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-debug-1

melt-stage1/warmelt-debug-1.c: $(melt_make_source_dir)/warmelt-debug.melt \
                  melt-stage1/warmelt-first-1.so \
                  melt-stage1/warmelt-base-1.so \
                  $(MELT_STAGE_ZERO)/warmelt-debug-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-macro-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normal-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normatch-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-genobj-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-1:\
warmelt-debug-0:\
warmelt-macro-0:\
warmelt-normal-0:\
warmelt-normatch-0:\
warmelt-genobj-0:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-macro for melt-stage1
melt-stage1/warmelt-macro-1.so: melt-stage1/warmelt-macro-1.c \
              $(wildcard melt-stage1/warmelt-macro-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-macro-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-macro-1

melt-stage1/warmelt-macro-1.n.so: melt-stage1/warmelt-macro-1.c \
              $(wildcard melt-stage1/warmelt-macro-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-macro-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-macro-1

melt-stage1/warmelt-macro-1.c: $(melt_make_source_dir)/warmelt-macro.melt \
                  melt-stage1/warmelt-first-1.so \
                  melt-stage1/warmelt-base-1.so \
                  melt-stage1/warmelt-debug-1.so \
                  $(MELT_STAGE_ZERO)/warmelt-macro-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normal-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normatch-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-genobj-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-1:\
warmelt-debug-1:\
warmelt-macro-0:\
warmelt-normal-0:\
warmelt-normatch-0:\
warmelt-genobj-0:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-normal for melt-stage1
melt-stage1/warmelt-normal-1.so: melt-stage1/warmelt-normal-1.c \
              $(wildcard melt-stage1/warmelt-normal-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-normal-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-normal-1

melt-stage1/warmelt-normal-1.n.so: melt-stage1/warmelt-normal-1.c \
              $(wildcard melt-stage1/warmelt-normal-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-normal-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-normal-1

melt-stage1/warmelt-normal-1.c: $(melt_make_source_dir)/warmelt-normal.melt \
        warmelt-predef.melt \
                  melt-stage1/warmelt-first-1.so \
                  melt-stage1/warmelt-base-1.so \
                  melt-stage1/warmelt-debug-1.so \
                  melt-stage1/warmelt-macro-1.so \
                  $(MELT_STAGE_ZERO)/warmelt-normal-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-normatch-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-genobj-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-1:\
warmelt-debug-1:\
warmelt-macro-1:\
warmelt-normal-0:\
warmelt-normatch-0:\
warmelt-genobj-0:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-normatch for melt-stage1
melt-stage1/warmelt-normatch-1.so: melt-stage1/warmelt-normatch-1.c \
              $(wildcard melt-stage1/warmelt-normatch-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-normatch-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-normatch-1

melt-stage1/warmelt-normatch-1.n.so: melt-stage1/warmelt-normatch-1.c \
              $(wildcard melt-stage1/warmelt-normatch-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-normatch-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-normatch-1

melt-stage1/warmelt-normatch-1.c: $(melt_make_source_dir)/warmelt-normatch.melt \
                  melt-stage1/warmelt-first-1.so \
                  melt-stage1/warmelt-base-1.so \
                  melt-stage1/warmelt-debug-1.so \
                  melt-stage1/warmelt-macro-1.so \
                  melt-stage1/warmelt-normal-1.so \
                  $(MELT_STAGE_ZERO)/warmelt-normatch-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-genobj-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-1:\
warmelt-debug-1:\
warmelt-macro-1:\
warmelt-normal-1:\
warmelt-normatch-0:\
warmelt-genobj-0:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-genobj for melt-stage1
melt-stage1/warmelt-genobj-1.so: melt-stage1/warmelt-genobj-1.c \
              $(wildcard melt-stage1/warmelt-genobj-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-genobj-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-genobj-1

melt-stage1/warmelt-genobj-1.n.so: melt-stage1/warmelt-genobj-1.c \
              $(wildcard melt-stage1/warmelt-genobj-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-genobj-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-genobj-1

melt-stage1/warmelt-genobj-1.c: $(melt_make_source_dir)/warmelt-genobj.melt \
                  melt-stage1/warmelt-first-1.so \
                  melt-stage1/warmelt-base-1.so \
                  melt-stage1/warmelt-debug-1.so \
                  melt-stage1/warmelt-macro-1.so \
                  melt-stage1/warmelt-normal-1.so \
                  melt-stage1/warmelt-normatch-1.so \
                  $(MELT_STAGE_ZERO)/warmelt-genobj-0.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-1:\
warmelt-debug-1:\
warmelt-macro-1:\
warmelt-normal-1:\
warmelt-normatch-1:\
warmelt-genobj-0:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-outobj for melt-stage1
melt-stage1/warmelt-outobj-1.so: melt-stage1/warmelt-outobj-1.c \
              $(wildcard melt-stage1/warmelt-outobj-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-outobj-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-outobj-1

melt-stage1/warmelt-outobj-1.n.so: melt-stage1/warmelt-outobj-1.c \
              $(wildcard melt-stage1/warmelt-outobj-1+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage1/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage1/warmelt-outobj-1.c \
              GCCMELT_MODULE_BINARY=melt-stage1/warmelt-outobj-1

melt-stage1/warmelt-outobj-1.c: $(melt_make_source_dir)/warmelt-outobj.melt \
                  melt-stage1/warmelt-first-1.so \
                  melt-stage1/warmelt-base-1.so \
                  melt-stage1/warmelt-debug-1.so \
                  melt-stage1/warmelt-macro-1.so \
                  melt-stage1/warmelt-normal-1.so \
                  melt-stage1/warmelt-normatch-1.so \
                  melt-stage1/warmelt-genobj-1.so \
                  $(MELT_STAGE_ZERO)/warmelt-outobj-0.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-1:\
warmelt-debug-1:\
warmelt-macro-1:\
warmelt-normal-1:\
warmelt-normatch-1:\
warmelt-genobj-1:\
warmelt-outobj-0 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage1:$(MELT_STAGE_ZERO):.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


melt-stage1/warmelt-1.modlis:  \
             melt-stage1/warmelt-first-1.so \
             melt-stage1/warmelt-base-1.so \
             melt-stage1/warmelt-debug-1.so \
             melt-stage1/warmelt-macro-1.so \
             melt-stage1/warmelt-normal-1.so \
             melt-stage1/warmelt-normatch-1.so \
             melt-stage1/warmelt-genobj-1.so \
             melt-stage1/warmelt-outobj-1.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-1 >> $@-tmp
	echo warmelt-base-1 >> $@-tmp
	echo warmelt-debug-1 >> $@-tmp
	echo warmelt-macro-1 >> $@-tmp
	echo warmelt-normal-1 >> $@-tmp
	echo warmelt-normatch-1 >> $@-tmp
	echo warmelt-genobj-1 >> $@-tmp
	echo warmelt-outobj-1 >> $@-tmp
	$(melt_make_move) $@-tmp $@


melt-stage1/warmelt-1.n.modlis:  \
             melt-stage1/warmelt-first-1.so \
             melt-stage1/warmelt-base-1.so \
             melt-stage1/warmelt-debug-1.so \
             melt-stage1/warmelt-macro-1.so \
             melt-stage1/warmelt-normal-1.so \
             melt-stage1/warmelt-normatch-1.so \
             melt-stage1/warmelt-genobj-1.so \
             melt-stage1/warmelt-outobj-1.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-1.n >> $@-tmp
	echo warmelt-base-1.n >> $@-tmp
	echo warmelt-debug-1.n >> $@-tmp
	echo warmelt-macro-1.n >> $@-tmp
	echo warmelt-normal-1.n >> $@-tmp
	echo warmelt-normatch-1.n >> $@-tmp
	echo warmelt-genobj-1.n >> $@-tmp
	echo warmelt-outobj-1.n >> $@-tmp
	$(melt_make_move) $@-tmp $@


.PHONY: warmelt1 warmelt1n
warmelt1:  melt-stage1 melt-stage1/warmelt-1.modlis
warmelt1n:  melt-stage1 melt-stage1/warmelt-1.n.modlis
melt-stage1:
	if [ -d melt-stage1 ]; then true; else mkdir melt-stage1; fi

### end of melt-stage1


#### rules for melt-stage2, previous melt-stage1


################## warmelt-first for melt-stage2
melt-stage2/warmelt-first-2.so: melt-stage2/warmelt-first-2.c \
              $(wildcard melt-stage2/warmelt-first-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-first-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-first-2

melt-stage2/warmelt-first-2.n.so: melt-stage2/warmelt-first-2.c \
              $(wildcard melt-stage2/warmelt-first-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-first-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-first-2

melt-stage2/warmelt-first-2.c: $(melt_make_source_dir)/warmelt-first.melt \
                  melt-stage1/warmelt-first-1.so \
                  melt-stage1/warmelt-base-1.so \
                  melt-stage1/warmelt-debug-1.so \
                  melt-stage1/warmelt-macro-1.so \
                  melt-stage1/warmelt-normal-1.so \
                  melt-stage1/warmelt-normatch-1.so \
                  melt-stage1/warmelt-genobj-1.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)=\
warmelt-first-1:\
warmelt-base-1:\
warmelt-debug-1:\
warmelt-macro-1:\
warmelt-normal-1:\
warmelt-normatch-1:\
warmelt-genobj-1:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-base for melt-stage2
melt-stage2/warmelt-base-2.so: melt-stage2/warmelt-base-2.c \
              $(wildcard melt-stage2/warmelt-base-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-base-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-base-2

melt-stage2/warmelt-base-2.n.so: melt-stage2/warmelt-base-2.c \
              $(wildcard melt-stage2/warmelt-base-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-base-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-base-2

melt-stage2/warmelt-base-2.c: $(melt_make_source_dir)/warmelt-base.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage1/warmelt-base-1.so \
                  melt-stage1/warmelt-debug-1.so \
                  melt-stage1/warmelt-macro-1.so \
                  melt-stage1/warmelt-normal-1.so \
                  melt-stage1/warmelt-normatch-1.so \
                  melt-stage1/warmelt-genobj-1.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-1:\
warmelt-debug-1:\
warmelt-macro-1:\
warmelt-normal-1:\
warmelt-normatch-1:\
warmelt-genobj-1:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-debug for melt-stage2
melt-stage2/warmelt-debug-2.so: melt-stage2/warmelt-debug-2.c \
              $(wildcard melt-stage2/warmelt-debug-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-debug-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-debug-2

melt-stage2/warmelt-debug-2.n.so: melt-stage2/warmelt-debug-2.c \
              $(wildcard melt-stage2/warmelt-debug-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-debug-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-debug-2

melt-stage2/warmelt-debug-2.c: $(melt_make_source_dir)/warmelt-debug.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage1/warmelt-debug-1.so \
                  melt-stage1/warmelt-macro-1.so \
                  melt-stage1/warmelt-normal-1.so \
                  melt-stage1/warmelt-normatch-1.so \
                  melt-stage1/warmelt-genobj-1.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-2:\
warmelt-debug-1:\
warmelt-macro-1:\
warmelt-normal-1:\
warmelt-normatch-1:\
warmelt-genobj-1:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-macro for melt-stage2
melt-stage2/warmelt-macro-2.so: melt-stage2/warmelt-macro-2.c \
              $(wildcard melt-stage2/warmelt-macro-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-macro-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-macro-2

melt-stage2/warmelt-macro-2.n.so: melt-stage2/warmelt-macro-2.c \
              $(wildcard melt-stage2/warmelt-macro-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-macro-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-macro-2

melt-stage2/warmelt-macro-2.c: $(melt_make_source_dir)/warmelt-macro.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage1/warmelt-macro-1.so \
                  melt-stage1/warmelt-normal-1.so \
                  melt-stage1/warmelt-normatch-1.so \
                  melt-stage1/warmelt-genobj-1.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-2:\
warmelt-debug-2:\
warmelt-macro-1:\
warmelt-normal-1:\
warmelt-normatch-1:\
warmelt-genobj-1:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-normal for melt-stage2
melt-stage2/warmelt-normal-2.so: melt-stage2/warmelt-normal-2.c \
              $(wildcard melt-stage2/warmelt-normal-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-normal-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-normal-2

melt-stage2/warmelt-normal-2.n.so: melt-stage2/warmelt-normal-2.c \
              $(wildcard melt-stage2/warmelt-normal-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-normal-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-normal-2

melt-stage2/warmelt-normal-2.c: $(melt_make_source_dir)/warmelt-normal.melt \
        warmelt-predef.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage1/warmelt-normal-1.so \
                  melt-stage1/warmelt-normatch-1.so \
                  melt-stage1/warmelt-genobj-1.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-2:\
warmelt-debug-2:\
warmelt-macro-2:\
warmelt-normal-1:\
warmelt-normatch-1:\
warmelt-genobj-1:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-normatch for melt-stage2
melt-stage2/warmelt-normatch-2.so: melt-stage2/warmelt-normatch-2.c \
              $(wildcard melt-stage2/warmelt-normatch-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-normatch-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-normatch-2

melt-stage2/warmelt-normatch-2.n.so: melt-stage2/warmelt-normatch-2.c \
              $(wildcard melt-stage2/warmelt-normatch-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-normatch-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-normatch-2

melt-stage2/warmelt-normatch-2.c: $(melt_make_source_dir)/warmelt-normatch.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage1/warmelt-normatch-1.so \
                  melt-stage1/warmelt-genobj-1.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-2:\
warmelt-debug-2:\
warmelt-macro-2:\
warmelt-normal-2:\
warmelt-normatch-1:\
warmelt-genobj-1:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-genobj for melt-stage2
melt-stage2/warmelt-genobj-2.so: melt-stage2/warmelt-genobj-2.c \
              $(wildcard melt-stage2/warmelt-genobj-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-genobj-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-genobj-2

melt-stage2/warmelt-genobj-2.n.so: melt-stage2/warmelt-genobj-2.c \
              $(wildcard melt-stage2/warmelt-genobj-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-genobj-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-genobj-2

melt-stage2/warmelt-genobj-2.c: $(melt_make_source_dir)/warmelt-genobj.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage1/warmelt-genobj-1.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-2:\
warmelt-debug-2:\
warmelt-macro-2:\
warmelt-normal-2:\
warmelt-normatch-2:\
warmelt-genobj-1:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-outobj for melt-stage2
melt-stage2/warmelt-outobj-2.so: melt-stage2/warmelt-outobj-2.c \
              $(wildcard melt-stage2/warmelt-outobj-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-outobj-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-outobj-2

melt-stage2/warmelt-outobj-2.n.so: melt-stage2/warmelt-outobj-2.c \
              $(wildcard melt-stage2/warmelt-outobj-2+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage2/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage2/warmelt-outobj-2.c \
              GCCMELT_MODULE_BINARY=melt-stage2/warmelt-outobj-2

melt-stage2/warmelt-outobj-2.c: $(melt_make_source_dir)/warmelt-outobj.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage1/warmelt-outobj-1.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-2:\
warmelt-debug-2:\
warmelt-macro-2:\
warmelt-normal-2:\
warmelt-normatch-2:\
warmelt-genobj-2:\
warmelt-outobj-1 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage2:melt-stage1:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage2:melt-stage1:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


melt-stage2/warmelt-2.modlis:  \
             melt-stage2/warmelt-first-2.so \
             melt-stage2/warmelt-base-2.so \
             melt-stage2/warmelt-debug-2.so \
             melt-stage2/warmelt-macro-2.so \
             melt-stage2/warmelt-normal-2.so \
             melt-stage2/warmelt-normatch-2.so \
             melt-stage2/warmelt-genobj-2.so \
             melt-stage2/warmelt-outobj-2.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-2 >> $@-tmp
	echo warmelt-base-2 >> $@-tmp
	echo warmelt-debug-2 >> $@-tmp
	echo warmelt-macro-2 >> $@-tmp
	echo warmelt-normal-2 >> $@-tmp
	echo warmelt-normatch-2 >> $@-tmp
	echo warmelt-genobj-2 >> $@-tmp
	echo warmelt-outobj-2 >> $@-tmp
	$(melt_make_move) $@-tmp $@


melt-stage2/warmelt-2.n.modlis:  \
             melt-stage2/warmelt-first-2.so \
             melt-stage2/warmelt-base-2.so \
             melt-stage2/warmelt-debug-2.so \
             melt-stage2/warmelt-macro-2.so \
             melt-stage2/warmelt-normal-2.so \
             melt-stage2/warmelt-normatch-2.so \
             melt-stage2/warmelt-genobj-2.so \
             melt-stage2/warmelt-outobj-2.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-2.n >> $@-tmp
	echo warmelt-base-2.n >> $@-tmp
	echo warmelt-debug-2.n >> $@-tmp
	echo warmelt-macro-2.n >> $@-tmp
	echo warmelt-normal-2.n >> $@-tmp
	echo warmelt-normatch-2.n >> $@-tmp
	echo warmelt-genobj-2.n >> $@-tmp
	echo warmelt-outobj-2.n >> $@-tmp
	$(melt_make_move) $@-tmp $@


.PHONY: warmelt2 warmelt2n
warmelt2:  melt-stage2 melt-stage2/warmelt-2.modlis
warmelt2n:  melt-stage2 melt-stage2/warmelt-2.n.modlis
melt-stage2:
	if [ -d melt-stage2 ]; then true; else mkdir melt-stage2; fi

### end of melt-stage2


#### rules for melt-stage3, previous melt-stage2


################## warmelt-first for melt-stage3
melt-stage3/warmelt-first-3.so: melt-stage3/warmelt-first-3.c \
              $(wildcard melt-stage3/warmelt-first-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-first-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-first-3

melt-stage3/warmelt-first-3.n.so: melt-stage3/warmelt-first-3.c \
              $(wildcard melt-stage3/warmelt-first-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-first-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-first-3

melt-stage3/warmelt-first-3.c: $(melt_make_source_dir)/warmelt-first.melt \
                  melt-stage2/warmelt-first-2.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)=\
warmelt-first-2:\
warmelt-base-2:\
warmelt-debug-2:\
warmelt-macro-2:\
warmelt-normal-2:\
warmelt-normatch-2:\
warmelt-genobj-2:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-base for melt-stage3
melt-stage3/warmelt-base-3.so: melt-stage3/warmelt-base-3.c \
              $(wildcard melt-stage3/warmelt-base-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-base-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-base-3

melt-stage3/warmelt-base-3.n.so: melt-stage3/warmelt-base-3.c \
              $(wildcard melt-stage3/warmelt-base-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-base-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-base-3

melt-stage3/warmelt-base-3.c: $(melt_make_source_dir)/warmelt-base.melt \
                  melt-stage3/warmelt-first-3.so \
                  melt-stage2/warmelt-base-2.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-3:\
warmelt-base-2:\
warmelt-debug-2:\
warmelt-macro-2:\
warmelt-normal-2:\
warmelt-normatch-2:\
warmelt-genobj-2:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-debug for melt-stage3
melt-stage3/warmelt-debug-3.so: melt-stage3/warmelt-debug-3.c \
              $(wildcard melt-stage3/warmelt-debug-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-debug-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-debug-3

melt-stage3/warmelt-debug-3.n.so: melt-stage3/warmelt-debug-3.c \
              $(wildcard melt-stage3/warmelt-debug-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-debug-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-debug-3

melt-stage3/warmelt-debug-3.c: $(melt_make_source_dir)/warmelt-debug.melt \
                  melt-stage3/warmelt-first-3.so \
                  melt-stage3/warmelt-base-3.so \
                  melt-stage2/warmelt-debug-2.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-3:\
warmelt-base-3:\
warmelt-debug-2:\
warmelt-macro-2:\
warmelt-normal-2:\
warmelt-normatch-2:\
warmelt-genobj-2:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-macro for melt-stage3
melt-stage3/warmelt-macro-3.so: melt-stage3/warmelt-macro-3.c \
              $(wildcard melt-stage3/warmelt-macro-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-macro-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-macro-3

melt-stage3/warmelt-macro-3.n.so: melt-stage3/warmelt-macro-3.c \
              $(wildcard melt-stage3/warmelt-macro-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-macro-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-macro-3

melt-stage3/warmelt-macro-3.c: $(melt_make_source_dir)/warmelt-macro.melt \
                  melt-stage3/warmelt-first-3.so \
                  melt-stage3/warmelt-base-3.so \
                  melt-stage3/warmelt-debug-3.so \
                  melt-stage2/warmelt-macro-2.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-3:\
warmelt-base-3:\
warmelt-debug-3:\
warmelt-macro-2:\
warmelt-normal-2:\
warmelt-normatch-2:\
warmelt-genobj-2:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-normal for melt-stage3
melt-stage3/warmelt-normal-3.so: melt-stage3/warmelt-normal-3.c \
              $(wildcard melt-stage3/warmelt-normal-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-normal-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-normal-3

melt-stage3/warmelt-normal-3.n.so: melt-stage3/warmelt-normal-3.c \
              $(wildcard melt-stage3/warmelt-normal-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-normal-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-normal-3

melt-stage3/warmelt-normal-3.c: $(melt_make_source_dir)/warmelt-normal.melt \
        warmelt-predef.melt \
                  melt-stage3/warmelt-first-3.so \
                  melt-stage3/warmelt-base-3.so \
                  melt-stage3/warmelt-debug-3.so \
                  melt-stage3/warmelt-macro-3.so \
                  melt-stage2/warmelt-normal-2.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-3:\
warmelt-base-3:\
warmelt-debug-3:\
warmelt-macro-3:\
warmelt-normal-2:\
warmelt-normatch-2:\
warmelt-genobj-2:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-normatch for melt-stage3
melt-stage3/warmelt-normatch-3.so: melt-stage3/warmelt-normatch-3.c \
              $(wildcard melt-stage3/warmelt-normatch-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-normatch-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-normatch-3

melt-stage3/warmelt-normatch-3.n.so: melt-stage3/warmelt-normatch-3.c \
              $(wildcard melt-stage3/warmelt-normatch-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-normatch-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-normatch-3

melt-stage3/warmelt-normatch-3.c: $(melt_make_source_dir)/warmelt-normatch.melt \
                  melt-stage3/warmelt-first-3.so \
                  melt-stage3/warmelt-base-3.so \
                  melt-stage3/warmelt-debug-3.so \
                  melt-stage3/warmelt-macro-3.so \
                  melt-stage3/warmelt-normal-3.so \
                  melt-stage2/warmelt-normatch-2.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-3:\
warmelt-base-3:\
warmelt-debug-3:\
warmelt-macro-3:\
warmelt-normal-3:\
warmelt-normatch-2:\
warmelt-genobj-2:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-genobj for melt-stage3
melt-stage3/warmelt-genobj-3.so: melt-stage3/warmelt-genobj-3.c \
              $(wildcard melt-stage3/warmelt-genobj-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-genobj-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-genobj-3

melt-stage3/warmelt-genobj-3.n.so: melt-stage3/warmelt-genobj-3.c \
              $(wildcard melt-stage3/warmelt-genobj-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-genobj-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-genobj-3

melt-stage3/warmelt-genobj-3.c: $(melt_make_source_dir)/warmelt-genobj.melt \
                  melt-stage3/warmelt-first-3.so \
                  melt-stage3/warmelt-base-3.so \
                  melt-stage3/warmelt-debug-3.so \
                  melt-stage3/warmelt-macro-3.so \
                  melt-stage3/warmelt-normal-3.so \
                  melt-stage3/warmelt-normatch-3.so \
                  melt-stage2/warmelt-genobj-2.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-3:\
warmelt-base-3:\
warmelt-debug-3:\
warmelt-macro-3:\
warmelt-normal-3:\
warmelt-normatch-3:\
warmelt-genobj-2:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


################## warmelt-outobj for melt-stage3
melt-stage3/warmelt-outobj-3.so: melt-stage3/warmelt-outobj-3.c \
              $(wildcard melt-stage3/warmelt-outobj-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-outobj-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-outobj-3

melt-stage3/warmelt-outobj-3.n.so: melt-stage3/warmelt-outobj-3.c \
              $(wildcard melt-stage3/warmelt-outobj-3+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=melt-stage3/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=melt-stage3/warmelt-outobj-3.c \
              GCCMELT_MODULE_BINARY=melt-stage3/warmelt-outobj-3

melt-stage3/warmelt-outobj-3.c: $(melt_make_source_dir)/warmelt-outobj.melt \
                  melt-stage3/warmelt-first-3.so \
                  melt-stage3/warmelt-base-3.so \
                  melt-stage3/warmelt-debug-3.so \
                  melt-stage3/warmelt-macro-3.so \
                  melt-stage3/warmelt-normal-3.so \
                  melt-stage3/warmelt-normatch-3.so \
                  melt-stage3/warmelt-genobj-3.so \
                  melt-stage2/warmelt-outobj-2.so \
             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
warmelt-first-3:\
warmelt-base-3:\
warmelt-debug-3:\
warmelt-macro-3:\
warmelt-normal-3:\
warmelt-normatch-3:\
warmelt-genobj-3:\
warmelt-outobj-2 \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=melt-stage3:melt-stage2:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=melt-stage3:melt-stage2:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


melt-stage3/warmelt-3.modlis:  \
             melt-stage3/warmelt-first-3.so \
             melt-stage3/warmelt-base-3.so \
             melt-stage3/warmelt-debug-3.so \
             melt-stage3/warmelt-macro-3.so \
             melt-stage3/warmelt-normal-3.so \
             melt-stage3/warmelt-normatch-3.so \
             melt-stage3/warmelt-genobj-3.so \
             melt-stage3/warmelt-outobj-3.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-3 >> $@-tmp
	echo warmelt-base-3 >> $@-tmp
	echo warmelt-debug-3 >> $@-tmp
	echo warmelt-macro-3 >> $@-tmp
	echo warmelt-normal-3 >> $@-tmp
	echo warmelt-normatch-3 >> $@-tmp
	echo warmelt-genobj-3 >> $@-tmp
	echo warmelt-outobj-3 >> $@-tmp
	$(melt_make_move) $@-tmp $@


melt-stage3/warmelt-3.n.modlis:  \
             melt-stage3/warmelt-first-3.so \
             melt-stage3/warmelt-base-3.so \
             melt-stage3/warmelt-debug-3.so \
             melt-stage3/warmelt-macro-3.so \
             melt-stage3/warmelt-normal-3.so \
             melt-stage3/warmelt-normatch-3.so \
             melt-stage3/warmelt-genobj-3.so \
             melt-stage3/warmelt-outobj-3.so
	date  +"#$@ generated %F" > $@-tmp
	echo warmelt-first-3.n >> $@-tmp
	echo warmelt-base-3.n >> $@-tmp
	echo warmelt-debug-3.n >> $@-tmp
	echo warmelt-macro-3.n >> $@-tmp
	echo warmelt-normal-3.n >> $@-tmp
	echo warmelt-normatch-3.n >> $@-tmp
	echo warmelt-genobj-3.n >> $@-tmp
	echo warmelt-outobj-3.n >> $@-tmp
	$(melt_make_move) $@-tmp $@


.PHONY: warmelt3 warmelt3n
warmelt3:  melt-stage3 melt-stage3/warmelt-3.modlis
warmelt3n:  melt-stage3 melt-stage3/warmelt-3.n.modlis
melt-stage3:
	if [ -d melt-stage3 ]; then true; else mkdir melt-stage3; fi

### end of melt-stage3



######## last stage melt-stage3
MELT_LAST_STAGE=melt-stage3
WARMELT_LAST= warmelt3
WARMELT_LAST_MODLIS= melt-stage3/warmelt-3.modlis

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

### the final temporary build directory
melt-tempbuild: 
	test -d melt-tempbuild/ || mkdir  melt-tempbuild/

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
	cd melt-sources; rm -f warmelt-first.melt; $(LN_S) $(realpath $^)



melt-sources/warmelt-first.c: melt-sources/warmelt-first.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCINIT1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-first.so: melt-sources/warmelt-first.c \
        $(wildcard  melt-sources/warmelt-first+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-first

melt-modules/warmelt-first.n.so: melt-sources/warmelt-first.c \
        $(wildcard  melt-sources/warmelt-first+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-first
# end translator warmelt-first




## melt translator warmelt-base # 1
melt-sources/warmelt-base.melt: $(melt_make_source_dir)/warmelt-base.melt melt-sources
	cd melt-sources; rm -f warmelt-base.melt; $(LN_S) $(realpath $^)



melt-sources/warmelt-base.c: melt-sources/warmelt-base.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCFILE1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-base.so: melt-sources/warmelt-base.c \
        $(wildcard  melt-sources/warmelt-base+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-base

melt-modules/warmelt-base.n.so: melt-sources/warmelt-base.c \
        $(wildcard  melt-sources/warmelt-base+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-base
# end translator warmelt-base




## melt translator warmelt-debug # 2
melt-sources/warmelt-debug.melt: $(melt_make_source_dir)/warmelt-debug.melt melt-sources
	cd melt-sources; rm -f warmelt-debug.melt; $(LN_S) $(realpath $^)



melt-sources/warmelt-debug.c: melt-sources/warmelt-debug.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCFILE1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-debug.so: melt-sources/warmelt-debug.c \
        $(wildcard  melt-sources/warmelt-debug+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-debug

melt-modules/warmelt-debug.n.so: melt-sources/warmelt-debug.c \
        $(wildcard  melt-sources/warmelt-debug+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-debug
# end translator warmelt-debug




## melt translator warmelt-macro # 3
melt-sources/warmelt-macro.melt: $(melt_make_source_dir)/warmelt-macro.melt melt-sources
	cd melt-sources; rm -f warmelt-macro.melt; $(LN_S) $(realpath $^)



melt-sources/warmelt-macro.c: melt-sources/warmelt-macro.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCFILE1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-macro.so: melt-sources/warmelt-macro.c \
        $(wildcard  melt-sources/warmelt-macro+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-macro

melt-modules/warmelt-macro.n.so: melt-sources/warmelt-macro.c \
        $(wildcard  melt-sources/warmelt-macro+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-macro
# end translator warmelt-macro




## melt translator warmelt-normal # 4
melt-sources/warmelt-normal.melt: $(melt_make_source_dir)/warmelt-normal.melt melt-sources
	cd melt-sources; rm -f warmelt-normal.melt; $(LN_S) $(realpath $^)


# included warmelt-predef.melt
melt-sources/warmelt-predef.melt: warmelt-predef.melt
	rm -f melt-sources/warmelt-predef.melt*
	cp $^ melt-sources/warmelt-predef.melt-tmp
	mv  melt-sources/warmelt-predef.melt-tmp  melt-sources/warmelt-predef.melt


melt-sources/warmelt-normal.c: melt-sources/warmelt-normal.melt melt-sources/warmelt-predef.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCFILE1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-normal.so: melt-sources/warmelt-normal.c \
        $(wildcard  melt-sources/warmelt-normal+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-normal

melt-modules/warmelt-normal.n.so: melt-sources/warmelt-normal.c \
        $(wildcard  melt-sources/warmelt-normal+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-normal
# end translator warmelt-normal




## melt translator warmelt-normatch # 5
melt-sources/warmelt-normatch.melt: $(melt_make_source_dir)/warmelt-normatch.melt melt-sources
	cd melt-sources; rm -f warmelt-normatch.melt; $(LN_S) $(realpath $^)



melt-sources/warmelt-normatch.c: melt-sources/warmelt-normatch.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCFILE1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-normatch.so: melt-sources/warmelt-normatch.c \
        $(wildcard  melt-sources/warmelt-normatch+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-normatch

melt-modules/warmelt-normatch.n.so: melt-sources/warmelt-normatch.c \
        $(wildcard  melt-sources/warmelt-normatch+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-normatch
# end translator warmelt-normatch




## melt translator warmelt-genobj # 6
melt-sources/warmelt-genobj.melt: $(melt_make_source_dir)/warmelt-genobj.melt melt-sources
	cd melt-sources; rm -f warmelt-genobj.melt; $(LN_S) $(realpath $^)



melt-sources/warmelt-genobj.c: melt-sources/warmelt-genobj.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCFILE1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-genobj.so: melt-sources/warmelt-genobj.c \
        $(wildcard  melt-sources/warmelt-genobj+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-genobj

melt-modules/warmelt-genobj.n.so: melt-sources/warmelt-genobj.c \
        $(wildcard  melt-sources/warmelt-genobj+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-genobj
# end translator warmelt-genobj




## melt translator warmelt-outobj # 7
melt-sources/warmelt-outobj.melt: $(melt_make_source_dir)/warmelt-outobj.melt melt-sources
	cd melt-sources; rm -f warmelt-outobj.melt; $(LN_S) $(realpath $^)



melt-sources/warmelt-outobj.c: melt-sources/warmelt-outobj.melt  \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild 

	$(MELTCCFILE1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=$(MELT_LAST_STAGE):melt-sources \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ 

melt-modules/warmelt-outobj.so: melt-sources/warmelt-outobj.c \
        $(wildcard  melt-sources/warmelt-outobj+*.c) \
        melt-tempbuild melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-outobj

melt-modules/warmelt-outobj.n.so: melt-sources/warmelt-outobj.c \
        $(wildcard  melt-sources/warmelt-outobj+*.c) \
        melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/warmelt-outobj
# end translator warmelt-outobj



#### melt-sources application files




## melt application xtramelt-parse-infix-syntax
melt-sources/xtramelt-parse-infix-syntax.melt: $(melt_make_source_dir)/xtramelt-parse-infix-syntax.melt
	cd melt-sources; rm -f xtramelt-parse-infix-syntax.melt; $(LN_S) $(realpath $^)


melt-sources/xtramelt-parse-infix-syntax.c: melt-sources/xtramelt-parse-infix-syntax.melt  \
  melt-modules/warmelt-first.so melt-modules/warmelt-base.so melt-modules/warmelt-debug.so melt-modules/warmelt-macro.so melt-modules/warmelt-normal.so melt-modules/warmelt-normatch.so melt-modules/warmelt-genobj.so melt-modules/warmelt-outobj.so \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	$(MELTCCAPPLICATION1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=melt-modules:$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=melt-sources:$(MELT_LAST_STAGE) \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))): \
	     $(meltarg_output)=$@ 

melt-modules/xtramelt-parse-infix-syntax.so: melt-sources/xtramelt-parse-infix-syntax.c \
        $(wildcard  melt-sources/xtramelt-parse-infix-syntax+*.c) \
        melt-run.h melt-runtime.h melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-parse-infix-syntax

melt-modules/xtramelt-parse-infix-syntax.n.so: melt-sources/xtramelt-parse-infix-syntax.c \
        $(wildcard  melt-sources/xtramelt-parse-infix-syntax+*.c) \
        melt-run.h melt-runtime.h  melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-parse-infix-syntax


# end application xtramelt-parse-infix-syntax




## melt application xtramelt-ana-base
melt-sources/xtramelt-ana-base.melt: $(melt_make_source_dir)/xtramelt-ana-base.melt
	cd melt-sources; rm -f xtramelt-ana-base.melt; $(LN_S) $(realpath $^)


melt-sources/xtramelt-ana-base.c: melt-sources/xtramelt-ana-base.melt  \
  melt-modules/warmelt-first.so melt-modules/warmelt-base.so melt-modules/warmelt-debug.so melt-modules/warmelt-macro.so melt-modules/warmelt-normal.so melt-modules/warmelt-normatch.so melt-modules/warmelt-genobj.so melt-modules/warmelt-outobj.so \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	$(MELTCCAPPLICATION1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=melt-modules:$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=melt-sources:$(MELT_LAST_STAGE) \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))):xtramelt-parse-infix-syntax \
	     $(meltarg_output)=$@ 

melt-modules/xtramelt-ana-base.so: melt-sources/xtramelt-ana-base.c \
        $(wildcard  melt-sources/xtramelt-ana-base+*.c) \
        melt-run.h melt-runtime.h melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-ana-base

melt-modules/xtramelt-ana-base.n.so: melt-sources/xtramelt-ana-base.c \
        $(wildcard  melt-sources/xtramelt-ana-base+*.c) \
        melt-run.h melt-runtime.h  melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-ana-base


# end application xtramelt-ana-base




## melt application xtramelt-ana-simple
melt-sources/xtramelt-ana-simple.melt: $(melt_make_source_dir)/xtramelt-ana-simple.melt
	cd melt-sources; rm -f xtramelt-ana-simple.melt; $(LN_S) $(realpath $^)


melt-sources/xtramelt-ana-simple.c: melt-sources/xtramelt-ana-simple.melt  \
  melt-modules/warmelt-first.so melt-modules/warmelt-base.so melt-modules/warmelt-debug.so melt-modules/warmelt-macro.so melt-modules/warmelt-normal.so melt-modules/warmelt-normatch.so melt-modules/warmelt-genobj.so melt-modules/warmelt-outobj.so \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	$(MELTCCAPPLICATION1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=melt-modules:$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=melt-sources:$(MELT_LAST_STAGE) \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))):xtramelt-parse-infix-syntax:xtramelt-ana-base \
	     $(meltarg_output)=$@ 

melt-modules/xtramelt-ana-simple.so: melt-sources/xtramelt-ana-simple.c \
        $(wildcard  melt-sources/xtramelt-ana-simple+*.c) \
        melt-run.h melt-runtime.h melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-ana-simple

melt-modules/xtramelt-ana-simple.n.so: melt-sources/xtramelt-ana-simple.c \
        $(wildcard  melt-sources/xtramelt-ana-simple+*.c) \
        melt-run.h melt-runtime.h  melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-ana-simple


# end application xtramelt-ana-simple




## melt application xtramelt-c-generator
melt-sources/xtramelt-c-generator.melt: $(melt_make_source_dir)/xtramelt-c-generator.melt
	cd melt-sources; rm -f xtramelt-c-generator.melt; $(LN_S) $(realpath $^)


melt-sources/xtramelt-c-generator.c: melt-sources/xtramelt-c-generator.melt  \
  melt-modules/warmelt-first.so melt-modules/warmelt-base.so melt-modules/warmelt-debug.so melt-modules/warmelt-macro.so melt-modules/warmelt-normal.so melt-modules/warmelt-normatch.so melt-modules/warmelt-genobj.so melt-modules/warmelt-outobj.so \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	$(MELTCCAPPLICATION1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=melt-modules:$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=melt-sources:$(MELT_LAST_STAGE) \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))):xtramelt-parse-infix-syntax:xtramelt-ana-base:xtramelt-ana-simple \
	     $(meltarg_output)=$@ 

melt-modules/xtramelt-c-generator.so: melt-sources/xtramelt-c-generator.c \
        $(wildcard  melt-sources/xtramelt-c-generator+*.c) \
        melt-run.h melt-runtime.h melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-c-generator

melt-modules/xtramelt-c-generator.n.so: melt-sources/xtramelt-c-generator.c \
        $(wildcard  melt-sources/xtramelt-c-generator+*.c) \
        melt-run.h melt-runtime.h  melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-c-generator


# end application xtramelt-c-generator




## melt application xtramelt-opengpu
melt-sources/xtramelt-opengpu.melt: $(melt_make_source_dir)/xtramelt-opengpu.melt
	cd melt-sources; rm -f xtramelt-opengpu.melt; $(LN_S) $(realpath $^)


melt-sources/xtramelt-opengpu.c: melt-sources/xtramelt-opengpu.melt  \
  melt-modules/warmelt-first.so melt-modules/warmelt-base.so melt-modules/warmelt-debug.so melt-modules/warmelt-macro.so melt-modules/warmelt-normal.so melt-modules/warmelt-normatch.so melt-modules/warmelt-genobj.so melt-modules/warmelt-outobj.so \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	$(MELTCCAPPLICATION1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=melt-modules:$(MELT_LAST_STAGE) \
	     $(meltarg_source_path)=melt-sources:$(MELT_LAST_STAGE) \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))):xtramelt-parse-infix-syntax:xtramelt-ana-base:xtramelt-ana-simple:xtramelt-c-generator \
	     $(meltarg_output)=$@ 

melt-modules/xtramelt-opengpu.so: melt-sources/xtramelt-opengpu.c \
        $(wildcard  melt-sources/xtramelt-opengpu+*.c) \
        melt-run.h melt-runtime.h melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-opengpu

melt-modules/xtramelt-opengpu.n.so: melt-sources/xtramelt-opengpu.c \
        $(wildcard  melt-sources/xtramelt-opengpu+*.c) \
        melt-run.h melt-runtime.h  melt-tempbuild
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/xtramelt-opengpu


# end application xtramelt-opengpu



melt-all-modules: \
    melt-modules/warmelt-first.so \
    melt-modules/warmelt-base.so \
    melt-modules/warmelt-debug.so \
    melt-modules/warmelt-macro.so \
    melt-modules/warmelt-normal.so \
    melt-modules/warmelt-normatch.so \
    melt-modules/warmelt-genobj.so \
    melt-modules/warmelt-outobj.so \
     melt-modules/xtramelt-parse-infix-syntax.so \
     melt-modules/xtramelt-ana-base.so \
     melt-modules/xtramelt-ana-simple.so \
     melt-modules/xtramelt-c-generator.so \
     melt-modules/xtramelt-opengpu.so

$(melt_default_modules_list).modlis: melt-all-modules
	date  +"#$@ generated %F" > $@-tmp
	echo "# translator files" >> $@-tmp
	echo warmelt-first >> $@-tmp
	echo warmelt-base >> $@-tmp
	echo warmelt-debug >> $@-tmp
	echo warmelt-macro >> $@-tmp
	echo warmelt-normal >> $@-tmp
	echo warmelt-normatch >> $@-tmp
	echo warmelt-genobj >> $@-tmp
	echo warmelt-outobj >> $@-tmp

	echo "# application files" >> $@-tmp
	echo xtramelt-parse-infix-syntax >> $@-tmp
	echo xtramelt-ana-base >> $@-tmp
	echo xtramelt-ana-simple >> $@-tmp
	echo xtramelt-c-generator >> $@-tmp
	echo xtramelt-opengpu >> $@-tmp

	echo "#end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


### MELT upgrade
.PHONY: warmelt-upgrade-translator

warmelt-upgrade-translator: \
   melt-sources/warmelt-first.c \
         $(wildcard  melt-sources/warmelt-first+*.c) \
   melt-sources/warmelt-base.c \
         $(wildcard  melt-sources/warmelt-base+*.c) \
   melt-sources/warmelt-debug.c \
         $(wildcard  melt-sources/warmelt-debug+*.c) \
   melt-sources/warmelt-macro.c \
         $(wildcard  melt-sources/warmelt-macro+*.c) \
   melt-sources/warmelt-normal.c \
         $(wildcard  melt-sources/warmelt-normal+*.c) \
   melt-sources/warmelt-normatch.c \
         $(wildcard  melt-sources/warmelt-normatch+*.c) \
   melt-sources/warmelt-genobj.c \
         $(wildcard  melt-sources/warmelt-genobj+*.c) \
   melt-sources/warmelt-outobj.c \
         $(wildcard  melt-sources/warmelt-outobj+*.c)
	@echo upgrading the MELT translator

	@echo upgrading MELT translator warmelt-first	
	for f in melt-sources/warmelt-first*.c ; do \
	  bf=`basename $$f | sed s/warmelt-first/warmelt-first-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done

	@echo upgrading MELT translator warmelt-base	
	for f in melt-sources/warmelt-base*.c ; do \
	  bf=`basename $$f | sed s/warmelt-base/warmelt-base-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done

	@echo upgrading MELT translator warmelt-debug	
	for f in melt-sources/warmelt-debug*.c ; do \
	  bf=`basename $$f | sed s/warmelt-debug/warmelt-debug-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done

	@echo upgrading MELT translator warmelt-macro	
	for f in melt-sources/warmelt-macro*.c ; do \
	  bf=`basename $$f | sed s/warmelt-macro/warmelt-macro-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done

	@echo upgrading MELT translator warmelt-normal	
	for f in melt-sources/warmelt-normal*.c ; do \
	  bf=`basename $$f | sed s/warmelt-normal/warmelt-normal-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done

	@echo upgrading MELT translator warmelt-normatch	
	for f in melt-sources/warmelt-normatch*.c ; do \
	  bf=`basename $$f | sed s/warmelt-normatch/warmelt-normatch-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done

	@echo upgrading MELT translator warmelt-genobj	
	for f in melt-sources/warmelt-genobj*.c ; do \
	  bf=`basename $$f | sed s/warmelt-genobj/warmelt-genobj-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done

	@echo upgrading MELT translator warmelt-outobj	
	for f in melt-sources/warmelt-outobj*.c ; do \
	  bf=`basename $$f | sed s/warmelt-outobj/warmelt-outobj-0/`; \
	  rm -f $(srcdir)/melt/generated/$$bf-tmp; \
          grep -v '^#line' < $$f \
            | unifdef -UMELTGCC_NOLINENUMBERING \
                 > $(srcdir)/melt/generated/$$bf-tmp; \
	  ls -l $(srcdir)/melt/generated/$$bf-tmp; \
	  if [ -f $(srcdir)/melt/generated/$$bf ]; then \
             mv -f $(srcdir)/melt/generated/$$bf $(srcdir)/melt/generated/$$bf~ ; \
	  fi ; \
	  mv -f $(srcdir)/melt/generated/$$bf-tmp \
                     $(srcdir)/melt/generated/$$bf ; \
        done



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
	$(melt_make_cc1)  $(melt_make_cc1flags) $(meltarg_mode)=makedoc  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_tempdir)=.  $(meltarg_bootstrapping)  $(MELT_DEBUG) \
	      $(meltarg_init)=@$(melt_default_modules_list) \
	      $(meltarg_module_path)=melt-modules:. \
	      $(meltarg_source_path)=melt-sources:. \
	      $(meltarg_output)=$@  \
              $(meltarg_arglist)=warmelt-first.melt,warmelt-base.melt,warmelt-debug.melt,warmelt-macro.melt,warmelt-normal.melt,warmelt-normatch.melt,warmelt-genobj.melt,warmelt-outobj.melt,\
xtramelt-parse-infix-syntax.melt,xtramelt-ana-base.melt,xtramelt-ana-simple.melt,xtramelt-c-generator.melt,xtramelt-opengpu.melt \
              empty-file-for-melt.c


vpath %.so $(melt_make_module_dir) . 
vpath %.c $(melt_make_source_dir)/generated . $(melt_source_dir) 
vpath %.h $(melt_make_source_dir)/generated . $(melt_source_dir)




.PHONY: meltrun-generate
meltrun-generate: $(melt_default_modules_list).modlis  empty-file-for-melt.c \
                  melt-all-sources melt-all-modules  $(melt_make_cc1_dependency)
	rm -f $(wildcard meltrunsup*)
	$(melt_make_cc1)  $(melt_make_cc1flags) \
	      $(meltarg_mode)=runtypesupport  \
	      $(meltarg_tempdir)=.  $(meltarg_bootstrapping)  $(MELT_DEBUG) \
	      $(meltarg_init)=@$(melt_default_modules_list) \
	      $(meltarg_module_path)=melt-modules:. \
	      $(meltarg_source_path)=melt-sources:. \
	      $(meltarg_output)=meltrunsup  \
	      empty-file-for-melt.c
	if [ -n "$(GCCMELTRUNGEN_DEST)" ]; then \
	   for f in $(GCCMELTRUNGEN_DEST)/meltrunsup*.[ch]; \
	     do mv $$f $$f.bak; \
	   done; \
	   cp -v meltrunsup*.[ch] $(GCCMELTRUNGEN_DEST) ; \
        fi

### MELT cleanup
.PHONY: melt-clean
melt-clean:
	rm -rf melt-stage0-static melt-stage0-dynamic \
	       melt-stage0-static.timestamp melt-stage0-dynamic.timestamp \
           melt-stage1  melt-stage1.timestamp \
           melt-stage2  melt-stage2.timestamp \
           melt-stage3  melt-stage3.timestamp \
               melt-sources melt-modules

## eof melt-build.mk generated from melt-build.tpl & melt-melt-build.def
