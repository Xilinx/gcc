[+ AutoGen5 template -*- Mode: Makefile -*-
in
+][+COMMENT use 'autogen --trace=everything melt-build.def' 
in build directory to debug this+]

# melt-build.in is generated from melt-build.tpl by 'autogen melt-build.def'
# DON'T EDIT melt-build.in but only edit: melt-build.tpl or melt-build.def
#
# Makefile[.in] fragment for MELT modules and MELT translator bootstrap.
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
                  $(melt_source_dir)/generated/[+base+].0.c \
                  $(wildcard $(melt_source_dir)/generated/[+base+].0+*.c)
MELT_GENERATED_[+mkvarsuf+]_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_[+mkvarsuf+]_C_FILES)))

[+ENDFOR melt_translator_file+]

## the rules to build the static and dynamic version of stage0, that
## is with static or dynamic field object offsets
[+FOR melt_translator_file +]
## using static object fields offsets for [+base+]
melt-stage0-static/[+base+].so: $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_source_dir)/generated/[+base+].0.c \
              GCCMELT_MODULE_BINARY=$@

## using dynamic object fields offsets for [+base+]
melt-stage0-dynamic/[+base+].so: $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$(melt_source_dir)/generated/[+base+].0.c \
              GCCMELT_MODULE_BINARY=$@
[+ENDFOR melt_translator_file+]

melt-stage0-static/warmelt.modlis: \
[+FOR melt_translator_file " \\\n" +]             melt-stage0-static/[+base+].so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+] >> $@-tmp
[+ENDFOR melt_translator_file+]	$(melt_make_move) $@-tmp $@

melt-stage0-dynamic/warmelt.modlis: \
[+FOR melt_translator_file " \\\n" +]              melt-stage0-static/[+base+].so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+] >> $@-tmp
[+ENDFOR melt_translator_file+]
	$(melt_make_move) $@-tmp $@

## can be overridden manually to either melt-stage0-dynamic or
## melt-stage0-static
MELT_STAGE_ZERO= melt-stage0-dynamic

[+FOR meltstage IN melt-stage1 melt-stage2 melt-stage3+]
#### rules for [+meltstage+][+ 
  (define stageindex (+ 1 (for-index)))
  (define prevstage (if (> stageindex 1) (sprintf "melt-stage%d" (- stageindex 1)) "$(MELT_STAGE_ZERO)"))
+], previous [+ (. prevstage)+]
[+FOR melt_translator_file+][+ 
  (define outbase (get "base")) (define outindex (for-index)) +]

################## [+ (. outbase)+] for [+meltstage+]
[+meltstage+]/[+(. outbase)+].so: [+meltstage+]/[+ (. outbase)+].c \
              $(wildcard [+meltstage+]/[+ (. outbase)+]+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module \
              GCCMELT_MODULE_WORKSPACE=[+meltstage+]/ \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=[+meltstage+]/[+ (. outbase)+].c \
              GCCMELT_MODULE_BINARY=$@

[+meltstage+]/[+ (. outbase)+].c: $(melt_source_dir)/[+ (. outbase)+].melt \
[+FOR melt_translator_file+][+ (define inbase (get "base")) (define inindex (for-index)) 
  (define depstage (if (< inindex outindex) (get "meltstage") prevstage))
+]                  [+ (. depstage)+]/[+ (. inbase)+].so \
[+ENDFOR melt_translator_file
+]              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
[+ENDFOR melt_translator_file+]
[+ENDFOR meltstage+]
