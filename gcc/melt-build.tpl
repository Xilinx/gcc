[+ AutoGen5 template -*- Mode: Makefile -*-
mk
+][+COMMENT use 'autogen --trace=everything melt-build.def' 
in build directory to debug this+]

# melt-build.mk is generated from melt-build.tpl by 'autogen melt-build.def'
# DON'T EDIT melt-build.in but only edit: melt-build.tpl or melt-build.def
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

## the various arguments to MELT - avoid spaces in them!
meltarg_mode=$(if $(melt_is_plugin),-fplugin-arg-melt-mode,-fmelt-mode)
meltarg_init=$(if $(melt_is_plugin),-fplugin-arg-melt-init,-fmelt-init)
meltarg_module_path=$(if $(melt_is_plugin),-fplugin-arg-melt-module-path,-fmelt-module-path)
meltarg_source_path=$(if $(melt_is_plugin),-fplugin-arg-melt-source-path,-fmelt-source-path)
meltarg_tempdir=$(if $(melt_is_plugin),-fplugin-arg-melt-tempdir,-fmelt-tempdir)

meltarg_arg=$(if $(melt_is_plugin),-fplugin-arg-melt-arg,-fmelt-arg)
meltarg_makefile=$(if $(melt_is_plugin),-fplugin-arg-melt-module-makefile,-fmelt-module-makefile)
meltarg_makecmd=$(if $(melt_is_plugin),-fplugin-arg-melt-module-make-command,-fmelt-module-make-command)
meltarg_arglist=$(if $(melt_is_plugin),-fplugin-arg-melt-arglist,-fmelt-arglist)
meltarg_output=$(if $(melt_is_plugin),-fplugin-arg-melt-output,-fmelt-output)

## MELT_DEBUG could be set to -fmelt-debug or -fplugin-arg-melt-debug
## the invocation to translate the very first initial MELT file
MELTCCINIT1=$(melt_make_cc1) $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translateinit  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_tempdir)=.  $(MELT_DEBUG)

## the invocation to translate the other files
MELTCCFILE1=$(melt_make_cc1)  $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_module_path)=.:$(melt_make_module_dir) \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_source_path)=.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_tempdir)=.  $(MELT_DEBUG)


vpath %.so $(melt_make_module_dir) . 
vpath %.c $(melt_make_source_dir)/generated . $(melt_source_dir)
vpath %.h . $(melt_make_source_dir)/generated $(melt_source_dir)
vpath %.melt $(melt_make_source_dir) . $(melt_source_dir)

##
## the invoking command could set MELT_MAKE_MODULE_XTRAMAKEFLAGS=-j2
MELT_MAKE_MODULE=$(MAKE) -f $(melt_make_module_makefile) $(MELT_MAKE_MODULE_XTRAMAKEFLAGS) VPATH=$(VPATH):.

## The base name of the MELT translator files
MELT_TRANSLATOR_BASE= \
  [+FOR melt_translator_file +] [+base+] \
  [+ENDFOR melt_translator_file+]

## The base name of the MELT application files
MELT_APPLICATION_BASE= \
  [+FOR melt_application_file +] [+base+] \
  [+ENDFOR melt_application_file+]

## The cold stage 0 of the translator
[+FOR melt_translator_file +]
MELT_GENERATED_[+mkvarsuf+]_C_FILES= \
                  $(melt_make_source_dir)/generated/[+base+].0.c \
                  $(wildcard $(melt_make_source_dir)/generated/[+base+].0+*.c)
MELT_GENERATED_[+mkvarsuf+]_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_[+mkvarsuf+]_C_FILES)))

[+ENDFOR melt_translator_file+]

## the rules to build the static and dynamic version of stage0, that
## is with static or dynamic field object offsets
[+FOR melt_translator_file +]
## using static object fields offsets for [+base+]
melt-stage0-static/[+base+]-0.so: $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/[+base+].0.c \
              GCCMELT_MODULE_BINARY=$@

## using dynamic object fields offsets for [+base+]
melt-stage0-dynamic/[+base+]-0.d.so: $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_make_source_dir)/generated/[+base+].0.c \
              GCCMELT_MODULE_BINARY=$@

melt-stage0-dynamic/[+base+]-0.so: melt-stage0-dynamic/[+base+]-0.d.so
	$(LN_S) $(notdir $<) $@
[+ENDFOR melt_translator_file+]

melt-stage0-static/warmelt-0.modlis: \
[+FOR melt_translator_file " \\\n" +]             melt-stage0-static/[+base+]-0.so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+]-0 >> $@-tmp
[+ENDFOR melt_translator_file+]	$(melt_make_move) $@-tmp $@

melt-stage0-dynamic/warmelt-0.modlis: \
[+FOR melt_translator_file " \\\n" +]              melt-stage0-dynamic/[+base+]-0.d.so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+]-0 >> $@-tmp
[+ENDFOR melt_translator_file+]
	$(melt_make_move) $@-tmp $@

## can be overridden manually to either melt-stage0-dynamic or
## melt-stage0-static
.PHONY: warmelt0
## the default stage0 melt-stage0-dynamic
MELT_STAGE_ZERO?= melt-stage0-dynamic
warmelt0: $(melt_make_cc1_dependency) $(MELT_STAGE_ZERO) $(MELT_STAGE_ZERO)/warmelt-0.modlis
$(MELT_STAGE_ZERO):
	-test -d $(MELT_STAGE_ZERO)/ || mkdir $(MELT_STAGE_ZERO)

[+FOR melt_stage+]
#### rules for [+melt_stage+][+ 
  (define stageindex (+ 1 (for-index)))
  (define previndex (for-index))
  (define prevstage (if (> stageindex 1) (sprintf "melt-stage%d" previndex) "$(MELT_STAGE_ZERO)"))
+], previous [+ (. prevstage)+]
[+FOR melt_translator_file+][+ 
  (define outbase (get "base")) (define outindex (for-index)) +]

################## [+ (. outbase)+] for [+melt_stage+]
[+melt_stage+]/[+(. outbase)+]-[+(. stageindex)+].so: [+melt_stage+]/[+ (. outbase)+]-[+(. stageindex)+].c \
              $(wildcard [+melt_stage+]/[+ (. outbase)+]-[+(. stageindex)+]+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=[+melt_stage+]/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=[+melt_stage+]/[+ (. outbase)+]-[+(. stageindex)+].c \
              GCCMELT_MODULE_BINARY=$@

[+melt_stage+]/[+ (. outbase)+]-[+(. stageindex)+].c: $(melt_make_source_dir)/[+ (. outbase)+].melt \
[+FOR includeload+]        [+includeload+] \
[+ENDFOR includeload
+][+FOR melt_translator_file+][+ (define inbase (get "base")) (define inindex (for-index)) 
  (define depstage (if (< inindex outindex) (get "melt_stage") prevstage))
  (define depindex (if (< inindex outindex) stageindex (- stageindex 1)))
+]                  [+ (. depstage)+]/[+ (. inbase)+]-[+(. depindex)+].so \
[+ENDFOR melt_translator_file
+]             empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)=\
[+FOR melt_translator_file ":\\\n"+][+ (define inbase (get "base")) (define inindex (for-index)) 
  (define depstage (if (< inindex outindex) (get "melt_stage") prevstage))
  (define depindex (if (< inindex outindex) stageindex (- stageindex 1)))
+][+ (. inbase)+]-[+(. depindex)+][+ENDFOR melt_translator_file
+] \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=[+melt_stage+]:[+ (. prevstage)+]:.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=[+melt_stage+]:[+ (. prevstage)+]:.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
	      $(meltarg_output)=$@  empty-file-for-melt.c
[+ENDFOR melt_translator_file+]

[+melt_stage+]/warmelt-[+(. stageindex)+].modlis:  \
[+FOR melt_translator_file " \\\n" +]             [+melt_stage+]/[+base+]-[+(. stageindex)+].so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+]-[+(. stageindex)+] >> $@-tmp
[+ENDFOR melt_translator_file+]	$(melt_make_move) $@-tmp $@
[+ (define laststage (get "melt_stage"))+]

.PHONY: warmelt[+(. stageindex)+]
warmelt[+(. stageindex)+]:  [+melt_stage+] [+melt_stage+]/warmelt-[+(. stageindex)+].modlis
[+melt_stage+]:
	if [ -d [+melt_stage+] ]; then true; else mkdir [+melt_stage+]; fi

### end of [+melt_stage+]

[+ENDFOR melt_stage+]

### MELT cleanup
.PHONY: melt-clean
melt-clean:
	rm -rf melt-stage0-static melt-stage0-dynamic \
[+FOR melt_stage " \\\n"+]           [+melt_stage+][+ENDFOR melt_stage+]
### last stage [+melt_stage+]

## end of generated file melt-build.mk
