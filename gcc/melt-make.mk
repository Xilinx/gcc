## file melt-make.mk
## a -*- Makefile -*- fragment for GNU make. 

# Copyright (C) 2009,2010 Free Software Foundation, Inc.
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

## this file is *not* preprocessed by any autoconf trickery.
## it is just included by GNU make

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
	      $(meltarg_module_path)=.:$(melt_make_module_dir) \
	      $(meltarg_makefile)=$(melt_make_module_makefile) \
	      $(meltarg_makecmd)=$(MAKE) \
	      $(meltarg_source_path)=.:$(melt_make_source_dir):$(melt_make_source_dir)/generated:$(melt_source_dir) \
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

warmelt-%.0.so: warmelt-%.0.c $(melt_make_module_makefile) melt-predef.h melt-run.h \
                melt-runtime.h melt-runtime.c melt-runtime.o $(melt_make_cc1_dependency)
	echo in melt-make.mk melt_cflags= $(melt_cflags) 
	$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@
warmelt-%.0.d.so: warmelt-%.0.c $(melt_make_module_makefile) melt-predef.h  melt-run.h \
                melt-runtime.h melt-runtime.c melt-runtime.o $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module_dynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$(shell basename $@ .d.so).so
warmelt-%-h.d.so: warmelt-%-h.c $(melt_make_module_makefile)
	$(MELT_MAKE_MODULE) melt_module_rawdynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@
warmelt-%-h2.d.so: warmelt-%-h2.c $(melt_make_module_makefile)
	$(MELT_MAKE_MODULE) melt_module_rawdynamic \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@

warm%.n.so: warm%.c $(melt_make_module_makefile)
	$(MELT_MAKE_MODULE) melt_module_rawwithoutline \
	      GCCMELT_CFLAGS="$(melt_cflags)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@
## warmeltbig*.c is so big that it can only be compiled with -O0
warmeltbig-%.so: warmeltbig-%.c $(melt_make_module_makefile) melt-predef.h  $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) -O0"   \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@
warmelt-%.so: warmelt-%.c  $(melt_make_module_makefile) melt-predef.h $(melt_make_cc1_dependency)
	$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) $(MELT_FINAL_CFLAGS)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@

###

xtra%.n.so: xtra%.c $(melt_make_module_makefile)
	$(MELT_MAKE_MODULE) melt_module_rawwithoutline \
	      GCCMELT_CFLAGS="$(melt_cflags) $(MELT_FINAL_CFLAGS)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@

xtra%.d.so: xtra%.c $(melt_make_module_makefile)
	$(MELT_MAKE_MODULE) melt_module_rawdynamic \
	      GCCMELT_CFLAGS="$(melt_cflags) $(MELT_FINAL_CFLAGS)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@

xtra%.so: xtra%.c $(melt_make_module_makefile)
	$(MELT_MAKE_MODULE) melt_module \
	      GCCMELT_CFLAGS="$(melt_cflags) $(MELT_FINAL_CFLAGS)" \
	      GCCMELT_MODULE_SOURCE=$< GCCMELT_MODULE_BINARY=$@

## the warmelt files - order is important!
WARMELT_RESTFILES= \
	 warmelt-base.melt \
	 warmelt-debug.melt \
	 warmelt-macro.melt  \
	 warmelt-normal.melt \
	 warmelt-normatch.melt \
	 warmelt-genobj.melt \
	 warmelt-outobj.melt 
WARMELT_FILES= 	 warmelt-first.melt $(WARMELT_RESTFILES)
WARMELT_SRCFILES= $(patsubst %.melt, $(melt_make_source_dir)/%.melt, $(WARMELT_FILES))
WARMELT_SRCRESTFILES= $(patsubst %.melt, $(melt_make_source_dir)/%.melt, $(WARMELT_RESTFILES))
WARMELT_SRCARGLIST:=$(shell echo $(realpath $(WARMELT_SRCFILES))|sed 's: :,:g')
WARMELT_BASE= $(basename $(WARMELT_FILES))
WARMELT_REST= $(basename $(WARMELT_RESTFILES))
WARMELT_BASELIST:=$(shell echo $(WARMELT_BASE)|sed 's: :,:g')

WARMELT_BASE0= $(patsubst %, %.0, $(WARMELT_BASE))
WARMELT_BASE0C= $(patsubst %, %.0.c, $(WARMELT_BASE))
WARMELT_BASE0SO= $(patsubst %, %.0.so, $(WARMELT_BASE))
## the file warmelt-first should never be C compiled in "dynamic"
## mode, otherwise it cannot have private [non-exported] classes like
## class_delayed_queue
WARMELT_BASE0DSO= warmelt-first.0.so $(patsubst %, %.0.d.so, $(WARMELT_REST))
WARMELT_BASE0ROW:=$(shell echo $(WARMELT_BASE0)|sed 's/ /:/g')
WARMELT_BASE0DROW:=$(shell echo $(patsubst %, %.0.d, $(WARMELT_BASE))|sed 's/ /:/g')
##
WARMELT_BASEH= $(patsubst %, %-h, $(WARMELT_BASE))
WARMELT_BASEHSO= $(patsubst %, %-h.so, $(WARMELT_BASE))
WARMELT_BASEHDSO=  warmelt-first-h.so $(patsubst %, %-h.d.so, $(WARMELT_REST))
WARMELT_BASEHC= $(patsubst %, %-h.c, $(WARMELT_BASE))
WARMELT_BASEHROW:=$(shell echo $(WARMELT_BASEH)|sed 's/ /:/g')
WARMELT_BASEHDROW:=$(shell echo $(patsubst %, %-h.d, $(WARMELT_BASE))|sed 's/ /:/g')
##
##
WARMELT_BASEH2= $(patsubst %, %-h2, $(WARMELT_BASE))
WARMELT_BASEH2SO= $(patsubst %, %-h2.so, $(WARMELT_REST))
WARMELT_BASEH2DSO= warmelt-first-h2.so $(patsubst %, %-h2.d.so, $(WARMELT_BASE))
WARMELT_BASEH2C= $(patsubst %, %-h2.c, $(WARMELT_BASE))
WARMELT_BASEH2ROW:=$(shell echo $(WARMELT_BASEH2)|sed 's/ /:/g')
##
WARMELT_BASE1= $(patsubst %, %.1, $(WARMELT_BASE))
WARMELT_BASE1SO= $(patsubst %, %.1.so, $(WARMELT_BASE))
WARMELT_BASE1NSO= $(patsubst %, %.1.n.so, $(WARMELT_BASE))
WARMELT_BASE1C= $(patsubst %, %.1.c, $(WARMELT_BASE))
WARMELT_BASE1ROW:=$(shell echo $(WARMELT_BASE1)|sed 's/ /:/g')
##
WARMELT_BASE2= $(patsubst %, %.2, $(WARMELT_BASE))
WARMELT_BASE2N= $(patsubst %, %.2.n, $(WARMELT_BASE))
WARMELT_BASE2SO= $(patsubst %, %.2.so, $(WARMELT_BASE))
WARMELT_BASE2NSO= $(patsubst %, %.2.n.so, $(WARMELT_BASE))
WARMELT_BASE2C= $(patsubst %, %.2.c, $(WARMELT_BASE))
WARMELT_BASE2ROW:=$(shell echo $(WARMELT_BASE2)|sed 's/ /:/g')
##
WARMELT_BASESO= $(patsubst %, %.so, $(WARMELT_BASE))
WARMELT_BASENSO= $(patsubst %, %.n.so, $(WARMELT_BASE))
WARMELT_BASEC= $(patsubst %, %.c, $(WARMELT_BASE))
WARMELT_BASEROW:=$(shell echo $(WARMELT_BASE)|sed 's/ /:/g')

## force a dependency
$(WARMELT_BASE0SO): empty-file-for-melt.c melt-run.h melt-runtime.h

.PHONY: warmelt0 warmelt1 warmelt2 warmelt warmelt1n warmelt2n

## warmelt0 is useful to contrib/build-melt-plugin.sh!
warmelt0: warmelt0.modlis $(WARMELT_BASE0SO)
warmelt1: warmelt1.modlis $(WARMELT_BASE1SO)
warmelt2: warmelt2.modlis $(WARMELT_BASE2SO)
warmelt: warmelt.modlis $(WARMELT_BASESO)

warmelt1n: warmelt1n.modlis $(WARMELT_BASE1NSO)
warmelt2n: warmelt2n.modlis $(WARMELT_BASE2NSO)
################
## the extra MELT files. They are not needed to bootstrap the MELT
## translation, but they are useful when using MELT on a GCC
## compiled source 

## order is important
XTRAMELT_FILES= xtramelt-ana-base.melt xtramelt-ana-simple.melt \
	xtramelt-parse-infix-syntax.melt xtramelt-opengpu.melt

XTRAMELT_SRCFILES= $(patsubst %.melt, $(melt_make_source_dir)/%.melt, $(XTRAMELT_FILES))
XTRAMELT_SRCARGLIST:=$(shell echo $(realpath $(XTRAMELT_SRCFILES))|sed 's: :,:g')
XTRAMELT_BASE= $(basename $(XTRAMELT_FILES))
XTRAMELT_BASELIST:=$(shell echo $(XTRAMELT_BASE)|sed 's: :,:g')

XTRAMELT_BASESO= $(patsubst %, %.so, $(XTRAMELT_BASE))
XTRAMELT_BASEC= $(patsubst %, %.c, $(XTRAMELT_BASE))
XTRAMELT_BASEROW:=$(shell echo $(XTRAMELT_BASE)|sed 's/ /:/g')

## keep the generated warm*.c files!
.SECONDARY:$(WARMELT_BASE1C) $(WARMELT_BASE2C) $(WARMELT_BASEC) $(XTRAMELT_BASEC) $(WARMELT_BASEHC) $(WARMELT_BASEH2C)

warmelt0.modlis: $(WARMELT_BASE0SO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE0); do echo $$f >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

##

warmelt1.modlis: $(WARMELT_BASE1SO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE1); do echo $$f >> $@-tmp; done
	$(melt_make_move) $@-tmp $@


warmelt1n.modlis: $(WARMELT_BASE1NSO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE1); do echo $$f.n >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

### we need ad hoc rules, since warmelt-first.1 is build using the
### warmelt*0.c files from SVN repository but warmelt-macro.1 is build
### using wamelt-first.1.so
empty-file-for-melt.c:
	date +"/* empty-file-for-melt.c %c */" > $@-tmp
	$(melt_make_move) $@-tmp $@

warmelt-first.1.c: $(melt_make_source_dir)/warmelt-first.melt warmelt0.modlis $(melt_make_gencdeps)  $(WARMELT_BASE0SO) empty-file-for-melt.c melt-predef.h melt-run.h melt-runtime.h melt-runtime.c $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASE0ROW) \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c



warmelt-first-h.c: $(melt_make_source_dir)/warmelt-first.melt warmelt0.modlis $(melt_make_gencdeps)  $(WARMELT_BASE0DSO) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASE0DROW) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-%-h.c: $(melt_make_source_dir)/warmelt-%.melt $(melt_make_gencdeps) \
	 $(WARMELT_BASE0DSO) warmelt-first-h.d.so  empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=$(subst warmelt-first.0.d,warmelt-first-h.d,$(WARMELT_BASE0DROW)) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelth.modlis: $(WARMELT_BASE0DSO) $(WARMELT_BASEHDSO)
	date +"#$@ generated %c" > $@-tmp
	for f in  $(WARMELT_BASEH); do echo $$f.d >> $@-tmp; done
	$(melt_make_move) $@-tmp $@



warmelt-first-h2.c: $(melt_make_source_dir)/warmelt-first.melt warmelth.modlis $(melt_make_gencdeps)  $(WARMELT_BASEHSO) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASEHROW) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-%-h2.c: $(melt_make_source_dir)/warmelt-%.melt $(melt_make_gencdeps) \
	 $(WARMELT_BASEHDSO)  empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=$(WARMELT_BASEHDROW) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c

## we add the ability to translate all the warmelt* files at once.
## this could help some delicate changes, like removing a field from
## an inner class of the translator. The generated file is huge, so we
## don't want to do that often, and the generated C file should be
## compiled without any optimisation, otherwise the C compiler suffers
## too much..

warmeltbig.1.c: $(WARMELT_SRCFILES) warmelt0.modlis $(melt_make_gencdeps)  $(WARMELT_BASE0SO)  warmelt-predef.melt  empty-file-for-melt.c melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASE0ROW) \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST) \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmeltbig.2.c: $(WARMELT_SRCFILES) warmeltbig.1.so $(melt_make_gencdeps) empty-file-for-melt.c warmelt-predef.melt $(WARMELT_SRCFILES)  empty-file-for-melt.c melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)=warmeltbig.1 \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST) \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmeltbig.3.c: $(WARMELT_SRCFILES) warmeltbig.2.so $(melt_make_gencdeps) empty-file-for-melt.c warmelt-predef.melt $(WARMELT_SRCFILES)  empty-file-for-melt.c melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)=warmeltbig.2 \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST) \
	      $(meltarg_output)=$@  empty-file-for-melt.c



.PHONY: warmeltallbig

### this is only useful rarely ... it regenerated the warmelt*0.c
### files using the big monolithic version of the translator... to be
### used carefully when desperate!
warmeltallbig: $(melt_make_gencdeps) $(melt_compile_script)  warmeltbig.1.c warmeltbig.3.so
	wc warmeltbig*.c
	$(MELTCCINIT1) $(meltarg_init)=warmeltbig.3 \
	      $(meltarg_arg)=$(melt_make_source_dir)/warmelt-first.melt  \
	      $(meltarg_output)=warmelt-first.0.c  empty-file-for-melt.c
	for f in $(WARMELT_SRCRESTFILES); do \
	   $(MELTCCFILE1) \
	     $(meltarg_init)=warmeltbig.3 \
	      $(meltarg_arg)=$$f \
	      $(meltarg_output)=`basename $$f .melt`.0.c  empty-file-for-melt.c ; \
	done


warmelth2.modlis: $(WARMELT_BASEH2SO)
	date +"#$@ generated %c" > $@-tmp
	for f in  $(WARMELT_BASEH2); do echo $$f.d >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

warmelt-base.1.c: $(melt_make_source_dir)/warmelt-base.melt $(melt_make_gencdeps) \
      warmelt-first.1.so  \
      warmelt-base.0.d.so  \
      warmelt-debug.0.d.so  \
      warmelt-macro.0.d.so  \
      warmelt-normal.0.d.so \
      warmelt-normatch.0.d.so \
      warmelt-genobj.0.d.so \
      warmelt-outobj.0.d.so \
      empty-file-for-melt.c melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency) 
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first.1:warmelt-base.0.d:warmelt-debug.0.d:warmelt-macro.0.d:warmelt-normal.0.d:warmelt-normatch.0.d:warmelt-genobj.0.d:warmelt-outobj.0.d \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmelt-debug.1.c: $(melt_make_source_dir)/warmelt-debug.melt $(melt_make_gencdeps) \
      warmelt-first.1.so  \
      warmelt-base.1.so  \
      warmelt-debug.0.d.so  \
      warmelt-macro.0.d.so  \
      warmelt-normal.0.d.so \
      warmelt-normatch.0.d.so \
      warmelt-genobj.0.d.so \
      warmelt-outobj.0.d.so \
      empty-file-for-melt.c melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency) 
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first.1:warmelt-base.1:warmelt-debug.0.d:warmelt-macro.0.d:warmelt-normal.0.d:warmelt-normatch.0.d:warmelt-genobj.0.d:warmelt-outobj.0.d \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-macro.1.c: $(melt_make_source_dir)/warmelt-macro.melt $(melt_make_gencdeps) \
     warmelt-first.1.so  \
     warmelt-base.1.so  \
     warmelt-debug.1.so  \
     warmelt-macro.0.d.so  \
     warmelt-normal.0.d.so \
     warmelt-normatch.0.d.so \
     warmelt-genobj.0.d.so \
     warmelt-outobj.0.d.so \
     empty-file-for-melt.c warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency) 
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first.1:warmelt-base.1:warmelt-debug.1:warmelt-macro.0.d:warmelt-normal.0.d:warmelt-normatch.0.d:warmelt-genobj.0.d:warmelt-outobj.0.d \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-normal.1.c: $(melt_make_source_dir)/warmelt-normal.melt $(melt_make_gencdeps) \
    warmelt-first.1.so  \
    warmelt-base.1.so  \
    warmelt-debug.1.so  \
    warmelt-macro.1.so  \
    warmelt-normal.0.d.so \
    warmelt-normatch.0.d.so \
    warmelt-genobj.0.d.so \
    warmelt-outobj.0.d.so \
    empty-file-for-melt.c warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency) 
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first.1:warmelt-base.1:warmelt-debug.1:warmelt-macro.1:warmelt-normal.0.d:warmelt-normatch.0.d:warmelt-genobj.0.d:warmelt-outobj.0.d \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-normatch.1.c: $(melt_make_source_dir)/warmelt-normatch.melt $(melt_make_gencdeps) \
    warmelt-first.1.so  \
    warmelt-base.1.so  \
    warmelt-debug.1.so  \
    warmelt-macro.1.so  \
    warmelt-normal.1.so \
    warmelt-normatch.0.d.so \
    warmelt-genobj.0.d.so \
    warmelt-outobj.0.d.so \
    empty-file-for-melt.c warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first.1:warmelt-base.1:warmelt-debug.1:warmelt-macro.1:warmelt-normal.1:warmelt-normatch.0.d:warmelt-genobj.0.d:warmelt-outobj.0.d \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmelt-genobj.1.c: $(melt_make_source_dir)/warmelt-genobj.melt $(melt_make_gencdeps) \
    warmelt-first.1.so  \
    warmelt-base.1.so  \
    warmelt-debug.1.so  \
    warmelt-macro.1.so  \
    warmelt-normal.1.so \
    warmelt-normatch.1.so \
    warmelt-genobj.0.d.so \
    warmelt-outobj.0.d.so \
    empty-file-for-melt.c warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first.1:warmelt-base.1:warmelt-debug.1:warmelt-macro.1:warmelt-normal.1:warmelt-normatch.1:warmelt-genobj.0.d:warmelt-outobj.0.d \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24)\
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmelt-outobj.1.c: $(melt_make_source_dir)/warmelt-outobj.melt $(melt_make_gencdeps) \
    warmelt-first.1.so  \
    warmelt-base.1.so  \
    warmelt-debug.1.so  \
    warmelt-macro.1.so  \
    warmelt-normal.1.so \
    warmelt-normatch.1.so \
    warmelt-genobj.1.so \
    warmelt-outobj.0.d.so \
    empty-file-for-melt.c warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first.1:warmelt-base.1:warmelt-debug.1:warmelt-macro.1:warmelt-normal.1:warmelt-normatch.1:warmelt-genobj.1:warmelt-outobj.0.d \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24)\
	      $(meltarg_output)=$@  empty-file-for-melt.c

####
warmelt2.modlis: $(WARMELT_BASE2SO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE2); do echo $$f >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

warmelt2n.modlis: $(WARMELT_BASE2NSO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE2N); do echo $$f >> $@-tmp; done
	echo "# end $@" >> $@-tmp
	$(melt_make_move) $@-tmp $@

warmelt-first.2.c: $(melt_make_source_dir)/warmelt-first.melt warmelt1.modlis $(WARMELT_BASE1SO) $(melt_make_gencdeps) \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCINIT1) $(meltarg_init)="@warmelt1" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@ empty-file-for-melt.c



warmelt-%.2.c: $(melt_make_source_dir)/warmelt-%.melt warmelt1.modlis $(WARMELT_BASE1SO)  $(melt_make_gencdeps) \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	$(MELTCCFILE1) $(meltarg_init)="@warmelt1" \
	        -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c
####

warmelt2b.modlis: warmeltbig.1.so
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE2); do printf "%sb\n" $$f >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

warmelt-first.2b.c: $(melt_make_source_dir)/warmelt-first.melt warmeltbig.1.so  $(melt_make_gencdeps)
	$(MELTCCINIT1) $(meltarg_init)="warmeltbig.1" \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@



warmelt-%.2b.c: $(melt_make_source_dir)/warmelt-%.melt warmeltbig.1.so   $(melt_make_gencdeps)  empty-file-for-melt.c
	$(MELTCCFILE1) $(meltarg_init)="warmeltbig.1" \
	        -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c

.PHONY: warmelt2b
warmelt2b: $(patsubst %,%b.so, $(WARMELT_BASE2))

###
warmelt-normal.2.c: warmelt-predef.melt
warmelt-normal.2b.c: warmelt-predef.melt
warmelt-normal.c: warmelt-predef.melt

## apparently, these dependencies are explicitly needed... because
## melt.encap target is making explicitly these
warmelt-normatch.2.so: warmelt-normatch.2.c
warmelt-outobj.so: warmelt-outobj.c 


####
warmelt-%.c: $(melt_make_source_dir)/warmelt-%.melt  warmelt2.modlis $(WARMELT_BASE2SO)  $(melt_make_gencdeps) \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c
	ls -l $@
warmelt.modlis: $(WARMELT_BASE2SO)
	date +"#$@ generated %c" > $@-tmp
	for f in  $(WARMELT_BASE); do echo $$f >> $@-tmp; done
	$(SHELL) $(srcdir)/../move-if-change $@-tmp $@

warmelt-first.c: $(melt_make_source_dir)/warmelt-first.melt warmelt2.modlis $(WARMELT_BASE2SO) $(melt_make_gencdeps) \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	-rm -f $@ 
	@echo generating $@ using $(WARMELT_BASE2SO)
	$(MELTCCINIT1) $(meltarg_init)="@warmelt2" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c
	ls -l $@


####
diff-warmelt.2.: $(WARMELT_BASESO)
	echo WARMELT_BASEROW= $(WARMELT_BASEROW)
	for f in $(WARMELT_BASE); do; \
	  diff $$f.2.c $$f.c || true; \
	done


####
xtramelt-ana-base.c: $(melt_make_source_dir)/xtramelt-ana-base.melt  warmelt2.modlis $(WARMELT_BASE2SO)  $(melt_make_gencdeps) \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@   empty-file-for-melt.c
	ls -l $@

xtramelt-ana-simple.c:  $(melt_make_source_dir)/xtramelt-ana-simple.melt  warmelt2.modlis $(WARMELT_BASE2SO)  xtramelt-ana-base.so   $(melt_make_gencdeps)  \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2:xtramelt-ana-base" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@   empty-file-for-melt.c
	ls -l $@

xtramelt-parse-infix-syntax.c:  $(melt_make_source_dir)/xtramelt-parse-infix-syntax.melt  warmelt2.modlis $(WARMELT_BASE2SO)  xtramelt-ana-base.so xtramelt-ana-simple.so  $(melt_make_gencdeps)  \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2:xtramelt-ana-base:xtramelt-ana-simple" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@   empty-file-for-melt.c
	ls -l $@

xtramelt-opengpu.c:  $(melt_make_source_dir)/xtramelt-opengpu.melt  warmelt2.modlis $(WARMELT_BASE2SO)  xtramelt-ana-base.so xtramelt-ana-simple.so  xtramelt-parse-infix-syntax.so $(melt_make_gencdeps)  \
  empty-file-for-melt.c  warmelt-predef.melt melt-predef.h melt-run.h melt-runtime.h $(melt_make_cc1_dependency)
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2:xtramelt-ana-base:xtramelt-ana-simple:xtramelt-parse-infix-syntax" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@   empty-file-for-melt.c
	ls -l $@

####
#### the default list of modules
$(melt_default_modules_list).modlis:  $(WARMELT_BASESO) $(XTRAMELT_BASESO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE); do echo $$f >> $@-tmp; done
	for f in  $(XTRAMELT_BASE); do echo $$f >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

### generated melt documentation
meltgendoc.texi: $(melt_default_modules_list).modlis $(WARMELT_SRCFILES) $(XTRAMELT_SRCFILES) empty-file-for-melt.c $(melt_make_gencdeps)
	$(melt_make_cc1) $(melt_make_cc1flags) -Wno-shadow $(meltarg_mode)=makedoc  \
	      $(meltarg_module_path)=.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=.:$(melt_make_source_dir):$(melt_source_dir) \
	      $(meltarg_compile_script)=$(melt_make_compile_script) \
	      $(meltarg_tempdir)=.  $(MELT_DEBUG) \
	      $(meltarg_init)="@$(melt_default_modules_list)" \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST),$(XTRAMELT_SRCARGLIST)  \
	      $(meltarg_output)=$@   empty-file-for-melt.c


#eof melt-make.mk
