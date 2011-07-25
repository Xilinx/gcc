[+ AutoGen5 template -*- Mode: Makefile -*-
mk
+][+COMMENT use 'autogen --trace=everything melt-build.def' to debug this
+]
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

## LN_S might not be defined, e.g. from build-melt-plugin.sh
ifndef LN_S
LN_S= ln -s
endif

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
meltarg_modulecflags=$(if $(melt_is_plugin),-fplugin-arg-melt-module-cflags,-fmelt-module-cflags)
## MELT_DEBUG could be set to -fmelt-debug or -fplugin-arg-melt-debug
## the invocation to translate the very first initial MELT file
MELTCCINIT1ARGS= $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translateinit  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
              "$(meltarg_modulecflags)=$(melt_cflags)" \
	      $(meltarg_tempdir)=. $(meltarg_bootstrapping) $(MELT_DEBUG)

## the invocation to translate the other files
MELTCCFILE1ARGS=  $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
              "$(meltarg_modulecflags)=$(melt_cflags)" \
	      $(meltarg_tempdir)=. $(meltarg_bootstrapping)  $(MELT_DEBUG)

## the invocation to translate the application files -don't pass the -fmelt-bootstrap flag
MELTCCAPPLICATION1=$(melt_make_cc1)  $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
              "$(meltarg_modulecflags)=$(melt_cflags)" \
	      $(meltarg_tempdir)=. $(MELT_DEBUG)


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
  [+FOR melt_translator_file " \\\n"+]  [+base+][+ENDFOR melt_translator_file+]

## the MELT translator MELT source files
MELT_TRANSLATOR_SOURCE= $(patsubst %,$(melt_make_source_dir)/%.melt,$(MELT_TRANSLATOR_BASE))

## The base name of the MELT application files
MELT_APPLICATION_BASE= \
  [+FOR melt_application_file " \\\n"+]  [+base+][+ENDFOR melt_application_file+]

## The MELT application source files
MELT_APPLICATION_SOURCE= $(patsubst %,$(melt_make_source_dir)/%.melt,$(MELT_APPLICATION_BASE))

## The cold stage 0 of the translator
[+FOR melt_translator_file +]
MELT_GENERATED_[+mkvarsuf+]_C_FILES= \
                  $(melt_make_source_dir)/generated/[+base+].c \
                  $(wildcard $(melt_make_source_dir)/generated/[+base+]+*.c)
MELT_GENERATED_[+mkvarsuf+]_BASE= \
                  $(basename $(notdir $(MELT_GENERATED_[+mkvarsuf+]_C_FILES)))

[+ENDFOR melt_translator_file+]

## the rules to build the static and dynamic version of stage0, that
## is with static or dynamic field object offsets
[+FOR melt_translator_file +]
## using static object fields offsets for [+base+]
melt-stage0-static/[+base+].q.so: $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_quicklybuilt \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-static \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-stage0-static/[+base+]

melt-stage0-static/[+base+].so: melt-stage0-static//[+base+].q.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

## using dynamic object fields offsets for [+base+]
melt-stage0-dynamic/[+base+].d.so: $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) \
             melt-run.h melt-runtime.h melt-runtime.c \
             melt-predef.h $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_dynamic \
              GCCMELT_MODULE_WORKSPACE=melt-stage0-dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-stage0-dynamic/[+base+]

melt-stage0-dynamic/[+base+].so: melt-stage0-dynamic/[+base+].d.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)

melt-stage0-dynamic/[+base+].q.so: melt-stage0-dynamic/[+base+].d.so
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(notdir $<) $(notdir $@)


[+ENDFOR melt_translator_file+]


melt-stage0-static.stamp:  melt-stage0-static melt-stage0-static/warmelt.modlis melt-run.h
	date +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file "\n"+]	md5sum melt-run.h $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) >> $@-tmp[+ENDFOR melt_translator_file+]
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@

melt-stage0-dynamic.stamp:  melt-stage0-dynamic melt-stage0-dynamic/warmelt.modlis melt-run.h
	date +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file "\n"+]	md5sum melt-run.h $(MELT_GENERATED_[+mkvarsuf+]_C_FILES) >> $@-tmp[+ENDFOR melt_translator_file+]
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@




melt-stage0-static/warmelt.modlis: \
[+FOR melt_translator_file " \\\n" +]             melt-stage0-static/[+base+].q.so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+].q >> $@-tmp
[+ENDFOR melt_translator_file+]	$(melt_make_move) $@-tmp $@

melt-stage0-dynamic/warmelt.modlis: \
[+FOR melt_translator_file " \\\n" +]              melt-stage0-dynamic/[+base+].q.so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+].q >> $@-tmp
[+ENDFOR melt_translator_file+]
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
warmelt0: $(melt_make_cc1_dependency) $(MELT_STAGE_ZERO) $(MELT_STAGE_ZERO).stamp 
$(MELT_STAGE_ZERO):
	-test -d $(MELT_STAGE_ZERO)/ || mkdir $(MELT_STAGE_ZERO)



#### making our melt stages

[+FOR melt_stage+]
#### rules for [+melt_stage+][+ 
  (define stageindex (+ 1 (for-index)))
  (define previndex (for-index))
  (define prevstage (if (> stageindex 1) (sprintf "melt-stage%d" previndex) "$(MELT_STAGE_ZERO)"))
+], previous [+ (. prevstage)+]
[+FOR melt_translator_file+][+ 
  (define outbase (get "base")) (define outindex (for-index)) +]


### the C source of [+melt_stage+] for [+ (. outbase)+]
[+melt_stage+]/[+ (. outbase)+].c:  $(melt_make_source_dir)/[+ (. outbase)+].melt \
 $(MELT_TRANSLATOR_SOURCE) \
 [+ (. prevstage)+].stamp [+ (. prevstage)+]/warmelt.modlis \
[+FOR includeload+]        [+includeload+] \
[+ENDFOR includeload
+][+FOR melt_translator_file+][+ (define inbase (get "base")) (define inindex (for-index)) 
  (define depstage (if (< inindex outindex) (get "melt_stage") prevstage))
  (define depindex (if (< inindex outindex) stageindex (- stageindex 1)))
+]      [+IF (< inindex outindex)+][+ (. depstage)+]/[+ (. inbase)+].q.so \
[+ENDIF+][+ENDFOR melt_translator_file
+]  empty-file-for-melt.c melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
## 
	@echo generating $< for [+melt_stage+]
	@echo [+IF (= outindex 0)+] $(MELTCCINIT1ARGS) $(meltarg_init)=\[+ELSE+] $(MELTCCFILE1ARGS) $(meltarg_init)=\[+ENDIF+]
[+FOR melt_translator_file ":\\\n"+][+ (define inbase (get "base")) (define inindex (for-index)) 
  (define depstage (if (< inindex outindex) (get "melt_stage") prevstage))
  (define depindex (if (< inindex outindex) stageindex (- stageindex 1)))
+][+ (. depstage)+]/[+ (. inbase)+].q[+ENDFOR melt_translator_file
+] \
           $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_module_path)=$(realpath .):$(realpath [+melt_stage+]):$(realpath [+ (. prevstage)+]):.:$(realpath  $(melt_make_module_dir)) \
	      $(meltarg_sourcue_path)=$(realpath .):$(realpath [+melt_stage+]):$(realpath [+ (. prevstage)+]):.:$(realpath $(melt_make_source_dir)):$(realpath $(melt_make_source_dir)/generated):$(realpath $(melt_source_dir)) \
	      $(meltarg_output)=$@ empty-file-for-melt.c > $(basename $@).args-tmp
	@mv $(basename $@).args-tmp $(basename $@).args
	@echo -n $(basename $@).args: ; cat $(basename $@).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(basename $@).args
	@rm $(basename $@).args; echo

################## quickmodule [+ (. outbase)+] for [+melt_stage+]
[+melt_stage+]/[+(. outbase)+].q.so: [+melt_stage+]/[+ (. outbase)+].c \
              $(wildcard [+melt_stage+]/[+ (. outbase)+]+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_quicklybuilt \
              GCCMELT_MODULE_WORKSPACE=$(realpath [+melt_stage+]) \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=$(realpath [+melt_stage+])/[+(. outbase)+]

[+melt_stage+]/[+(. outbase)+].n.so: [+melt_stage+]/[+ (. outbase)+].c \
              $(wildcard [+melt_stage+]/[+ (. outbase)+]+*.c) \
              melt-run.h melt-runtime.h melt-predef.h \
              $(melt_make_cc1_dependency)
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
              GCCMELT_MODULE_WORKSPACE=$(realpath [+melt_stage+]) \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=$(realpath [+melt_stage+])/[+(. outbase)+]

[+ENDFOR melt_translator_file+]


## the module list in [+melt_stage+]
[+melt_stage+]/warmelt.modlis:  \
[+FOR melt_translator_file " \\\n" +]             [+melt_stage+]/[+base+].q.so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+].q >> $@-tmp
[+ENDFOR melt_translator_file+]	$(melt_make_move) $@-tmp $@
[+ (define laststage (get "melt_stage"))
   (define lastindex stageindex)
+]

[+melt_stage+]/warmelt.n.modlis:  \
[+FOR melt_translator_file " \\\n" +]             [+melt_stage+]/[+base+].n.so[+
ENDFOR melt_translator_file+]
	date  +"#$@ generated %F" > $@-tmp
[+FOR melt_translator_file+]	echo [+base+].n >> $@-tmp
[+ENDFOR melt_translator_file+]	$(melt_make_move) $@-tmp $@
[+ (define laststage (get "melt_stage"))
   (define lastindex stageindex)
+]

## the stamp for [+melt_stage+]
[+melt_stage+].stamp: [+melt_stage+]/warmelt.modlis melt-run.h
	date +"#$@ generated %F" > $@-tmp
	md5sum melt-run.h >> $@-tmp
[+FOR melt_translator_file "\n"+]	md5sum $(wildcard [+melt_stage+]/[+base+]*.c) < /dev/null >> $@-tmp[+ENDFOR melt_translator_file+]
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@


### phony targets for  [+melt_stage+]
.PHONY: warmelt[+(. stageindex)+] warmelt[+(. stageindex)+]n
warmelt[+(. stageindex)+]:  [+melt_stage+] [+melt_stage+]/warmelt.modlis
	@echo MELT build made $@
warmelt[+(. stageindex)+]n:  [+melt_stage+] [+melt_stage+]/warmelt.n.modlis
	@echo MELT build made $@
[+melt_stage+]:
	if [ -d [+melt_stage+] ]; then true; else mkdir [+melt_stage+]; fi
[+ (define laststage (get "melt_stage"))+]
### end of [+melt_stage+]

[+ENDFOR melt_stage+]

######## last stage [+ (. laststage)+]
MELT_LAST_STAGE=[+ (. laststage)+]
WARMELT_LAST= warmelt[+ (. lastindex)+]
WARMELT_LAST_MODLIS= [+ (. laststage)+]/warmelt.modlis

.PHONY: warmelt
warmelt: $(WARMELT_LAST)

####### final targets
.PHONY: all-melt melt-all-modules melt-all-sources
all-melt: melt-modules melt-sources melt-all-modules melt-all-sources

### the final module directory
melt-modules: 
	test -d melt-modules/ || mkdir  melt-modules/
	mkdir -p melt-modules/quicklybuilt
	mkdir -p melt-modules/optimized
	mkdir -p melt-modules/debugnoline

### the final source directory
melt-sources: 
	test -d melt-sources/ || mkdir  melt-sources/

### the final temporary build directory
melt-tempbuild: 
	test -d melt-tempbuild/ || mkdir  melt-tempbuild/

melt-all-sources: $(WARMELT_LAST_MODLIS) empty-file-for-melt.c \
              melt-run.h melt-runtime.h melt-predef.h melt-sources \
              $(melt_make_cc1_dependency) \
[+FOR melt_translator_file+]      melt-sources/[+base+].melt \
              melt-sources/[+base+].c \
[+FOR includeload+]       melt-sources/[+includeload+] \
[+ENDFOR includeload+][+ENDFOR melt_translator_file+][+FOR melt_application_file" \\\n"
+]            melt-sources/[+base+].melt \
              melt-sources/[+base+].c [+ENDFOR melt_application_file+]


#### melt-sources translator files
[+FOR melt_translator_file+]
[+ (define transindex (for-index)) +]

## melt translator [+base+] # [+ (. transindex) +]
melt-sources/[+base+].melt: $(melt_make_source_dir)/[+base+].melt melt-sources
	cd melt-sources; rm -f $(notdir $@); $(LN_S) $(realpath $^)

### melt included files with (load ...) macro
[+FOR includeload+]
# included [+includeload+]
melt-sources/[+includeload+]: [+includeload+]
	rm -f melt-sources/[+includeload+]*
	cp $^ melt-sources/[+includeload+]-tmp
	mv  melt-sources/[+includeload+]-tmp  melt-sources/[+includeload+]
[+ENDFOR includeload+]

melt-sources/[+base+].c: melt-sources/[+base+].melt [+FOR includeload
+]melt-sources/[+includeload+] [+ENDFOR includeload+] \
                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency) melt-tempbuild melt-sources 
	@echo [+IF (= transindex 0)+] $(MELTCCINIT1ARGS) \[+ELSE+] $(MELTCCFILE1) \[+ENDIF+]
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath $(MELT_LAST_STAGE)) \
	     $(meltarg_source_path)=$(realpath $(MELT_LAST_STAGE)):$(realpath melt-sources) \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))) \
	     $(meltarg_output)=$@ empty-file-for-melt.c > $(basename $@).args-tmp
	@mv $(basename $@).args-tmp $(basename $@).args
	@echo -n $(basename $@).args: ; cat $(basename $@).args ; echo "***** doing " $@
	$(melt_make_cc1) @$(basename $@).args
	@rm $(basename $@).args; echo


melt-modules/optimized/[+base+].so: melt-sources/[+base+].c \
        $(wildcard  melt-sources/[+base+]+*.c) \
        melt-tempbuild melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_BINARY=melt-modules/optimized/[+base+]

melt-modules/debugnoline/[+base+].n.so: melt-sources/[+base+].c \
        $(wildcard  melt-sources/[+base+]+*.c) \
        melt-tempbuild melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/debugnoline/[+base+]

melt-modules/quicklybuilt/[+base+].q.so: melt-sources/[+base+].c \
        $(wildcard  melt-sources/[+base+]+*.c) \
        melt-tempbuild melt-modules melt-sources melt-run.h melt-runtime.h 
	+$(MELT_MAKE_MODULE) melt_module_quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/quicklybuilt/[+base+]
# end translator [+base+]

[+ENDFOR melt_translator_file+]

#### melt-sources application files
[+ (define prevapplbase (list)) +]
[+FOR melt_application_file+]

## melt application [+base+]
melt-sources/[+base+].melt: $(melt_make_source_dir)/[+base+].melt
	cd melt-sources; rm -f [+base+].melt; $(LN_S) $(realpath $^)


melt-sources/[+base+].c: melt-sources/[+base+].melt [+FOR includeload
+]melt-sources/[+includeload+] [+ENDFOR includeload+] \
 melt-sources melt-modules $(MELT_TRANSLATOR_SOURCE) \
 [+FOR melt_translator_file+] melt-modules/optimized/[+base+].so[+ENDFOR melt_translator_file+] \
	                    $(WARMELT_LAST) $(WARMELT_LAST_MODLIS) \
                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	$(MELTCCAPPLICATION1) \
	     $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	     $(meltarg_module_path)=$(realpath melt-modules):$(realpath melt-modules/optimized):$(realpath $(MELT_LAST_STAGE)) \
	     $(meltarg_source_path)=$(realpath melt-sources):$(realpath $(MELT_LAST_STAGE)) \
	     $(meltarg_init)=@$(notdir $(basename $(WARMELT_LAST_MODLIS))):[+ (. (join ":" (reverse prevapplbase)))+] \
	     $(meltarg_output)=$@ empty-file-for-melt.c 

melt-modules/optimized/[+base+].so: melt-sources/[+base+].c \
        $(wildcard  melt-sources/[+base+]+*.c) \
        melt-run.h melt-runtime.h melt-tempbuild melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/optimized/[+base+]

melt-modules/debugnoline/[+base+].n.so: melt-sources/[+base+].c \
        $(wildcard  melt-sources/[+base+]+*.c) \
        melt-run.h melt-runtime.h  melt-tempbuild melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module_withoutline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/debugnoline/[+base+]

melt-modules/quicklybuilt/[+base+].q.so: melt-sources/[+base+].c \
        $(wildcard  melt-sources/[+base+]+*.c) \
        melt-run.h melt-runtime.h  melt-tempbuild melt-sources melt-modules
	+$(MELT_MAKE_MODULE) melt_module_quicklybuilt \
	      GCCMELT_CFLAGS="$(melt_cflags) $(melt_extra_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< \
              GCCMELT_MODULE_WORKSPACE=melt-tempbuild \
              GCCMELT_MODULE_BINARY=melt-modules/quicklybuilt/[+base+]

[+ (define prevapplbase (cons (get "base") prevapplbase)) +]
# end application [+base+]

[+ENDFOR melt_application_file+]

######

melt-all-modules: \
[+FOR melt_translator_file+]    melt-modules/optimized/[+base+].so \
[+ENDFOR melt_translator_file+][+FOR melt_application_file " \\\n"
+]     melt-modules/optimized/[+base+].so[+ENDFOR melt_application_file+] \
[+FOR melt_translator_file+]    melt-modules/quicklybuilt/[+base+].q.so \
[+ENDFOR melt_translator_file+][+FOR melt_application_file " \\\n"
+]     melt-modules/quicklybuilt/[+base+].q.so[+ENDFOR melt_application_file+] \
[+FOR melt_translator_file+]    melt-modules/debugnoline/[+base+].n.so \
[+ENDFOR melt_translator_file+][+FOR melt_application_file " \\\n"
+]     melt-modules/debugnoline/[+base+].n.so[+ENDFOR melt_application_file+] \

$(melt_default_modules_list).modlis: melt-all-modules \
       $(melt_default_modules_list)-quicklybuilt.modlis \
       $(melt_default_modules_list)-optimized.modlis \
       $(melt_default_modules_list)-debugnoline.modlis
	cd $(dir $@) ; rm -f $(notdir $@); $(LN_S) $(melt_default_modules_list)-$(melt_default_variant).modlis  $(notdir $@)

## MELT various variants of module lists

[+FOR variant IN quicklybuilt optimized debugnoline+]
### [+variant+] default module list
$(melt_default_modules_list)-[+variant+].modlis:  melt-all-modules  melt-modules/ $(wildcard melt-modules/[+variant+]/*.so)
	@echo building [+variant+] module list $@
	date  +"# MELT module list $@ generated %F" > $@-tmp
	echo "#  [+variant+] translator files" >> $@-tmp
[+FOR melt_translator_file+]	echo [+variant+]/[+base+] >> $@-tmp
[+ENDFOR melt_translator_file+]
	echo "#  [+variant+] application files" >> $@-tmp
[+FOR melt_application_file+]	echo [+variant+]/[+base+] >> $@-tmp
[+ENDFOR melt_application_file+]
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@

[+ENDFOR variant+]

### MELT upgrade
.PHONY: warmelt-upgrade-translator

warmelt-upgrade-translator: $(WARMELT_LAST) \
[+FOR melt_translator_file " \\\n"
+]   $(MELT_LAST_STAGE)/[+base+].c \
         $(wildcard  $(MELT_LAST_STAGE)/[+base+]+*.c)[+
ENDFOR melt_translator_file+]
	@echo upgrading the MELT translator
	@which unifdef || (echo missing unifdef for warmelt-upgrade-translator; exit 1)
	@which indent || (echo missing indent for warmelt-upgrade-translator; exit 1)
[+FOR melt_translator_file+]
	@echo upgrading MELT translator [+base+]	
	for f in $(MELT_LAST_STAGE)/[+base+]*.c ; do \
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
	rm $(MELT_STAGE_ZERO)/[+base+]*.o
[+ENDFOR melt_translator_file+]


### Generated MELT documentation
meltgendoc.texi: $(melt_default_modules_list).modlis \
[+FOR melt_translator_file+]                    melt-sources/[+base+].melt \
[+ENDFOR melt_translator_file+][+FOR melt_application_file+]                    melt-sources/[+base+].melt \
[+ENDFOR melt_application_file+]                    empty-file-for-melt.c melt-run.h melt-runtime.h \
                    $(melt_make_cc1_dependency)
	echo $(melt_make_cc1flags) $(meltarg_mode)=makedoc  \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_tempdir)=.  $(meltarg_bootstrapping)  $(MELT_DEBUG) \
	      $(meltarg_init)=@$(melt_default_modules_list) \
	      $(meltarg_module_path)=$(realpath melt-modules):. \
	      $(meltarg_source_path)=$(realpath melt-sources):. \
	      $(meltarg_output)=$@  \
              $(meltarg_arglist)=[+FOR melt_translator_file+][+base+].melt,[+ENDFOR melt_translator_file+]\
[+FOR melt_application_file "," +][+base+].melt[+ENDFOR melt_application_file+] \
              empty-file-for-melt.c > $(basename $@).args-tmp
	mv  $(basename $@).args-tmp  $(basename $@).args
	@echo -n $(basename $@).args: ; cat $(basename $@).args ; echo "***** doing " $@
	$(melt_make_cc1flags) @$(basename $@).args


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
	rm -rf *melt*.args melt-stage0-static melt-stage0-dynamic \
	       melt-stage0-static.stamp melt-stage0-dynamic.stamp \
[+FOR melt_stage+]           [+melt_stage+]  [+melt_stage+].stamp \
[+ENDFOR melt_stage+]               melt-sources melt-modules

## eof melt-build.mk generated from melt-build.tpl & melt-melt-build.def
