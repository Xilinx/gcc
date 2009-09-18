## file melt-make.mk
## a -*- Makefile -*- fragment for GNU make. 

# Copyright (C) 2009 Free Software Foundation, Inc.
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
### melt_private_include_dir - header files for MELT generated C code
### melt_source_dir - directory containing *.melt (& corresponding *.c) file
### melt_module_dir - directory containing *.so MELT module files
### melt_compile_script - script file to compile MELT generated *.c files
### melt_make_compile_script - likewise when making MELT
### melt_make_source_dir - directory containing the *.melt files when making MELT
### melt_make_module_dir - directory containing the *.so files when making MELT
### melt_default_modules_list - basename of the default module list
### melt_cc1 - cc1 program with MELT (or loading MELT plugin)
### melt_is_plugin - should be non empty in plugin mode
### melt_make_move - a copy or move command for files

## the various arguments to MELT - avoid spaces in them!
meltarg_mode=$(if $(melt_is_plugin),-fplugin-arg-melt-mode,-fmelt)
meltarg_init=$(if $(melt_is_plugin),-fplugin-arg-melt-init,-fmelt-init)
meltarg_module_path=$(if $(melt_is_plugin),-fplugin-arg-melt-module-path,-fmelt-module-path)
meltarg_source_path=$(if $(melt_is_plugin),-fplugin-arg-melt-source-path,-fmelt-source-path)
meltarg_tempdir=$(if $(melt_is_plugin),-fplugin-arg-melt-tempdir,-fmelt-tempdir)
meltarg_compile_script=$(if $(melt_is_plugin),-fplugin-arg-melt-compile-script,-fmelt-compile-script)
meltarg_arg=$(if $(melt_is_plugin),-fplugin-arg-melt-arg,-fmelt-arg)
meltarg_arglist=$(if $(melt_is_plugin),-fplugin-arg-melt-arglist,-fmelt-arglist)
meltarg_output=$(if $(melt_is_plugin),-fplugin-arg-melt-output,-fmelt-output)

## MELT_DEBUG could be set to -fmelt-debug or -fplugin-arg-melt-debug
## the invocation to translate the very first initial MELT file
MELTCCINIT1=$(melt_cc1) $(melt_cc1flags) -Wno-shadow $(meltarg_mode)=translateinit  \
	      $(meltarg_module_path)=.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=.:$(melt_make_source_dir):$(melt_source_dir) \
	      $(meltarg_compile_script)=$(melt_make_compile_script) \
	      $(meltarg_tempdir)=.  $(MELT_DEBUG)

## the invocation to translate the other files
MELTCCFILE1=$(melt_cc1)  $(melt_cc1flags) -Wno-shadow $(meltarg_mode)=translatefile  \
	      $(meltarg_module_path)=.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=.:$(melt_make_source_dir):$(melt_source_dir) \
	      $(meltarg_compile_script)=$(melt_make_compile_script) \
	      $(meltarg_tempdir)=.  $(MELT_DEBUG)


vpath %.so $(melt_make_module_dir) . 
vpath %.c $(melt_make_source_dir) . $(melt_source_dir)
vpath %.melt $(melt_make_source_dir) . $(melt_source_dir)

## all the warm*.so need ./built-melt-cc-script to be built, but we
## don't add a dependency to avoid them being rebuilt at install time.
##
warmelt-%-0.d.so: warmelt-%-0.c 
	$(melt_make_compile_script) -d $< $@
warmelt-%-h.d.so: warmelt-%-h.c 
	$(melt_make_compile_script) -d $< $@
warmelt-%-h2.d.so: warmelt-%-h2.c 
	$(melt_make_compile_script) -d $< $@
warm%.n.so: warm%.c 
	$(melt_make_compile_script) -n $< $@
## warmeltbig*.c is so big that it can only be compiled with -O0
warmeltbig-%.so: warmeltbig-%.c 
	$(melt_make_compile_script) -O0 -d $< $@
warm%.so: warm%.c 
	$(melt_make_compile_script) $< $@

###

ana%.n.so: ana%.c 
	$(melt_make_compile_script) -n $< $@
ana%.d.so: ana%.c 
	$(melt_make_compile_script) -d $< $@
ana%.so: ana%.c 
	$(melt_make_compile_script)  $< $@

## the warmelt files - order is important!
WARMELT_RESTFILES= \
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
WARMELT_BASELIST:=$(shell echo $(WARMELT_BASE)|sed 's: :,:g')

WARMELT_BASE0= $(patsubst %, %-0, $(WARMELT_BASE))
WARMELT_BASE0C= $(patsubst %, %-0.c, $(WARMELT_BASE))
WARMELT_BASE0SO= $(patsubst %, %-0.so, $(WARMELT_BASE))
WARMELT_BASE0DSO= $(patsubst %, %-0.d.so, $(WARMELT_BASE))
WARMELT_BASE0ROW:=$(shell echo $(WARMELT_BASE0)|sed 's/ /:/g')
WARMELT_BASE0DROW:=$(shell echo $(patsubst %, %-0.d, $(WARMELT_BASE))|sed 's/ /:/g')
##
WARMELT_BASEH= $(patsubst %, %-h, $(WARMELT_BASE))
WARMELT_BASEHSO= $(patsubst %, %-h.so, $(WARMELT_BASE))
WARMELT_BASEHDSO= $(patsubst %, %-h.d.so, $(WARMELT_BASE))
WARMELT_BASEHC= $(patsubst %, %-h.c, $(WARMELT_BASE))
WARMELT_BASEHROW:=$(shell echo $(WARMELT_BASEH)|sed 's/ /:/g')
WARMELT_BASEHDROW:=$(shell echo $(patsubst %, %-h.d, $(WARMELT_BASE))|sed 's/ /:/g')
##
##
WARMELT_BASEH2= $(patsubst %, %-h2, $(WARMELT_BASE))
WARMELT_BASEH2SO= $(patsubst %, %-h2.so, $(WARMELT_BASE))
WARMELT_BASEH2DSO= $(patsubst %, %-h2.d.so, $(WARMELT_BASE))
WARMELT_BASEH2C= $(patsubst %, %-h2.c, $(WARMELT_BASE))
WARMELT_BASEH2ROW:=$(shell echo $(WARMELT_BASEH2)|sed 's/ /:/g')
##
WARMELT_BASE1= $(patsubst %, %-1, $(WARMELT_BASE))
WARMELT_BASE1SO= $(patsubst %, %-1.so, $(WARMELT_BASE))
WARMELT_BASE1NSO= $(patsubst %, %-1.n.so, $(WARMELT_BASE))
WARMELT_BASE1C= $(patsubst %, %-1.c, $(WARMELT_BASE))
WARMELT_BASE1ROW:=$(shell echo $(WARMELT_BASE1)|sed 's/ /:/g')
##
WARMELT_BASE2= $(patsubst %, %-2, $(WARMELT_BASE))
WARMELT_BASE2SO= $(patsubst %, %-2.so, $(WARMELT_BASE))
WARMELT_BASE2NSO= $(patsubst %, %-2.n.so, $(WARMELT_BASE))
WARMELT_BASE2C= $(patsubst %, %-2.c, $(WARMELT_BASE))
WARMELT_BASE2ROW:=$(shell echo $(WARMELT_BASE2)|sed 's/ /:/g')
##
WARMELT_BASE3= $(patsubst %, %-3, $(WARMELT_BASE))
WARMELT_BASE3SO= $(patsubst %, %-3.so, $(WARMELT_BASE))
WARMELT_BASE3C= $(patsubst %, %-3.c, $(WARMELT_BASE))
WARMELT_BASE3ROW:=$(shell echo $(WARMELT_BASE3)|sed 's/ /:/g')

## force a dependency
$(WARMELT_BASE0SO): empty-file-for-melt.c run-melt.h melt-runtime.h


################
## the analysis MELT files. They are not needed to bootstrap the MELT
## translation, but they are needed to make any MELT analysis of a GCC
## compiled source 

## order is important
ANAMELT_FILES= ana-base.melt ana-simple.melt
ANAMELT_SRCFILES= $(patsubst %.melt, $(melt_make_source_dir)/%.melt, $(ANAMELT_FILES))
ANAMELT_SRCARGLIST:=$(shell echo $(realpath $(ANAMELT_SRCFILES))|sed 's: :,:g')
ANAMELT_BASE= $(basename $(ANAMELT_FILES))
ANAMELT_BASELIST:=$(shell echo $(ANAMELT_BASE)|sed 's: :,:g')

ANAMELT_BASESO= $(patsubst %, %.so, $(ANAMELT_BASE))
ANAMELT_BASEC= $(patsubst %, %.c, $(ANAMELT_BASE))
ANAMELT_BASEROW:=$(shell echo $(ANAMELT_BASE)|sed 's/ /:/g')

## keep the generated warm*.c files!
.SECONDARY:$(WARMELT_BASE1C) $(WARMELT_BASE2C) $(WARMELT_BASE3C) $(ANAMELT_BASEC) $(WARMELT_BASEHC) $(WARMELT_BASEH2C)

warmelt0.modlis: $(WARMELT_BASE0DSO)
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

### we need ad hoc rules, since warmelt-first-1 is build using the
### warmelt*0.c files from SVN repository but warmelt-macro-1 is build
### using wamelt-first-1.so
empty-file-for-melt.c:
	date +"/* empty-file-for-melt.c %c */" > $@-tmp
	$(melt_make_move) $@-tmp $@

warmelt-first-1.c: $(melt_make_source_dir)/warmelt-first.melt warmelt0.modlis $(melt_cc1)  $(WARMELT_BASE0DSO) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASE0DROW) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c



warmelt-first-h.c: $(melt_make_source_dir)/warmelt-first.melt warmelt0.modlis $(melt_cc1)  $(WARMELT_BASE0DSO) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASE0DROW) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-%-h.c: $(melt_make_source_dir)/warmelt-%.melt $(melt_cc1) \
	 $(WARMELT_BASE0DSO) warmelt-first-h.d.so  empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=$(subst warmelt-first-0.d,warmelt-first-h.d,$(WARMELT_BASE0DROW)) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelth.modlis: $(WARMELT_BASE0DSO) $(WARMELT_BASEHDSO)
	date +"#$@ generated %c" > $@-tmp
	for f in  $(WARMELT_BASEH); do echo $$f.d >> $@-tmp; done
	$(melt_make_move) $@-tmp $@



warmelt-first-h2.c: $(melt_make_source_dir)/warmelt-first.melt warmelth.modlis $(melt_cc1)  $(WARMELT_BASEHSO) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASEHROW) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-%-h2.c: $(melt_make_source_dir)/warmelt-%.melt $(melt_cc1) \
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

warmeltbig-1.c: $(WARMELT_SRCFILES) warmelt0.modlis $(melt_cc1)  $(WARMELT_BASE0DSO) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=$(WARMELT_BASE0DROW) \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST) \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmeltbig-2.c: $(WARMELT_SRCFILES) warmeltbig-1.so $(melt_cc1) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=warmeltbig-1 \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST) \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmeltbig-3.c: $(WARMELT_SRCFILES) warmeltbig-2.so $(melt_cc1) empty-file-for-melt.c
	$(MELTCCINIT1) $(meltarg_init)=warmeltbig-2 \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST) \
	      $(meltarg_output)=$@  empty-file-for-melt.c



.PHONY: warmeltallbig 

warmeltallbig: warmeltbig-3.so
	wc warmeltbig*.c
	$(MELTCCINIT1) $(meltarg_init)=warmeltbig-3 \
	      $(meltarg_arg)=$(melt_make_source_dir)/warmelt-first.melt  \
	      $(meltarg_output)=warmelt-first-3.c  empty-file-for-melt.c
	for f in $(WARMELT_SRCRESTFILES); do \
	   $(MELTCCFILE1) \
	     $(meltarg_init)=warmeltbig-3 \
	      $(meltarg_arg)=$$f \
	      $(meltarg_output)=`basename $$f .melt`-3.c  empty-file-for-melt.c ; \
	done


warmelth2.modlis: $(WARMELT_BASEH2SO)
	date +"#$@ generated %c" > $@-tmp
	for f in  $(WARMELT_BASEH2); do echo $$f.d >> $@-tmp; done
	$(melt_make_move) $@-tmp $@


warmelt-macro-1.c: $(melt_make_source_dir)/warmelt-macro.melt $(melt_cc1) \
	 warmelt-first-1.so  \
	 warmelt-macro-0.d.so  \
	 warmelt-normal-0.d.so \
	 warmelt-normatch-0.d.so \
	 warmelt-genobj-0.d.so \
	 warmelt-outobj-0.d.so   empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first-1:warmelt-macro-0.d:warmelt-normal-0.d:warmelt-normatch-0.d:warmelt-genobj-0.d:warmelt-outobj-0.d \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-normal-1.c: $(melt_make_source_dir)/warmelt-normal.melt $(melt_cc1) \
	 warmelt-first-1.so  \
	 warmelt-macro-1.so  \
	 warmelt-normal-0.d.so \
	 warmelt-normatch-0.d.so \
	 warmelt-genobj-0.d.so \
	 warmelt-outobj-0.d.so \
	 warmelt-predef.melt  empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first-1:warmelt-macro-1:warmelt-normal-0.d:warmelt-normatch-0.d:warmelt-genobj-0.d:warmelt-outobj-0.d \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c


warmelt-normatch-1.c: $(melt_make_source_dir)/warmelt-normatch.melt $(melt_cc1) \
	 warmelt-first-1.so  \
	 warmelt-macro-1.so  \
	 warmelt-normal-1.so \
	 warmelt-normatch-0.d.so \
	 warmelt-genobj-0.d.so \
	 warmelt-outobj-0.d.so  empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first-1:warmelt-macro-1:warmelt-normal-1:warmelt-normatch-0.d:warmelt-genobj-0.d:warmelt-outobj-0.d \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmelt-genobj-1.c: $(melt_make_source_dir)/warmelt-genobj.melt $(melt_cc1) \
	 warmelt-first-1.so  \
	 warmelt-macro-1.so  \
	 warmelt-normal-1.so \
	 warmelt-normatch-1.so \
	 warmelt-genobj-0.d.so \
	 warmelt-outobj-0.d.so   empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first-1:warmelt-macro-1:warmelt-normal-1:warmelt-normatch-1:warmelt-genobj-0.d:warmelt-outobj-0.d \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmelt-outobj-1.c: $(melt_make_source_dir)/warmelt-outobj.melt $(melt_cc1) \
	 warmelt-first-1.so  \
	 warmelt-macro-1.so  \
	 warmelt-normal-1.so \
	 warmelt-normatch-1.so \
	 warmelt-genobj-1.so \
	 warmelt-outobj-0.d.so   empty-file-for-melt.c
	$(MELTCCFILE1) \
	$(meltarg_init)=warmelt-first-1:warmelt-macro-1:warmelt-normal-1:warmelt-normatch-1:warmelt-genobj-1:warmelt-outobj-0.d \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c

####
warmelt2.modlis: $(WARMELT_BASE2SO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE2); do echo $$f >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

warmelt-first-2.c: $(melt_make_source_dir)/warmelt-first.melt warmelt1.modlis $(WARMELT_BASE1SO) $(melt_cc1)
	$(MELTCCINIT1) $(meltarg_init)="@warmelt1" \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@



warmelt-%-2.c: $(melt_make_source_dir)/warmelt-%.melt warmelt1.modlis $(WARMELT_BASE1SO)  $(melt_cc1) $(WARMELT_BASE1NSO)  empty-file-for-melt.c
	$(MELTCCFILE1) $(meltarg_init)="@warmelt1" \
	        -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c

warmelt-normal-2.c: warmelt-predef.melt
warmelt-normal-3.c: warmelt-predef.melt

## apparently, these dependencies are explicitly needed... because
## melt.encap target is making explicitly these
warmelt-normatch-2.so: warmelt-normatch-2.c
warmelt-outobj-3.so: warmelt-outobj-3.c

warmelt2n.modlis: $(WARMELT_BASE2NSO)
	date +"#$@ generated %c" > $@-tmp
	for f in  $(WARMELT_BASE2); do echo $$f.n >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

####
warmelt-%-3.c: $(melt_make_source_dir)/warmelt-%.melt  warmelt2.modlis $(WARMELT_BASE2SO)  $(melt_cc1)  empty-file-for-melt.c
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@  empty-file-for-melt.c
	ls -l $@
warmelt3.modlis: $(WARMELT_BASE2SO)
	date +"#$@ generated %c" > $@-tmp
	for f in  $(WARMELT_BASE3); do echo $$f >> $@-tmp; done
	$(SHELL) $(srcdir)/../move-if-change $@-tmp $@

warmelt-first-3.c: $(melt_make_source_dir)/warmelt-first.melt warmelt2.modlis $(WARMELT_BASE2SO) $(melt_cc1)
	-rm -f $@ 
	@echo generating $@ using $(WARMELT_BASE2SO)
	$(MELTCCINIT1) $(meltarg_init)="@warmelt2" \
	      $(meltarg_arg)=$< \
	      $(meltarg_output)=$@  empty-file-for-melt.c
	ls -l $@


####
diff-warmelt-2-3: $(WARMELT_BASE3SO)
	echo WARMELT_BASE3ROW= $(WARMELT_BASE3ROW)
	for f in $(WARMELT_BASE); do; \
	  diff $$f-2.c $$f-3.c || true; \
	done


####
ana-base.c: $(melt_make_source_dir)/ana-base.melt  warmelt2.modlis $(WARMELT_BASE2SO)  $(melt_cc1)   empty-file-for-melt.c
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@   empty-file-for-melt.c
	ls -l $@

ana-simple.c:  $(melt_make_source_dir)/ana-simple.melt  warmelt2.modlis $(WARMELT_BASE2SO)  ana-base.so   $(melt_cc1) 
	@echo generating $@ using $(WARMELT_BASE2SO)
	-rm -f $@
	$(MELTCCFILE1) $(meltarg_init)="@warmelt2:ana-base" \
	      $(meltarg_arg)=$<  -frandom-seed=$(shell md5sum $< | cut -b-24) \
	      $(meltarg_output)=$@   empty-file-for-melt.c
	ls -l $@

####
#### the default list of modules
$(melt_default_modules_list).modlis:  $(WARMELT_BASE3SO) $(ANAMELT_BASESO)
	date +"#$@ generated %F" > $@-tmp
	for f in  $(WARMELT_BASE3); do echo $$f >> $@-tmp; done
	for f in  $(ANAMELT_BASE); do echo $$f >> $@-tmp; done
	$(melt_make_move) $@-tmp $@

### generated melt documentation
meltgendoc.texi: $(melt_default_modules_list).modlis $(WARMELT_SRCFILES) empty-file-for-melt.c
	$(melt_cc1) $(melt_cc1flags) -Wno-shadow $(meltarg_mode)=makedoc  \
	      $(meltarg_module_path)=.:$(melt_make_module_dir) \
	      $(meltarg_source_path)=.:$(melt_make_source_dir):$(melt_source_dir) \
	      $(meltarg_compile_script)=$(melt_make_compile_script) \
	      $(meltarg_tempdir)=.  $(MELT_DEBUG) \
	      $(meltarg_init)="@$(melt_default_modules_list)" \
	      $(meltarg_arglist)=$(WARMELT_SRCARGLIST)  \
	      $(meltarg_output)=$@   empty-file-for-melt.c


#eof melt-make.mk