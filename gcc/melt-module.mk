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

### should be invoked, perhaps from melt-runtime.c, with 
#### make -f melt-module.mk \
####    GCCMELT_MODULE_SOURCE=srcdir/foo.c \
####    GCCMELT_MODULE_BINARY=moduledir/foo \
####    GCCMELT_MODULE_WORKSPACE=/tmp 
#### to make moduledir/foo.so 

### See also routine compile_gencsrc_to_binmodule of melt-runtime.c

ifndef GCCMELT_MODULE_SOURCE
$(error GCCMELT_MODULE_SOURCE not defined)
endif

ifndef GCCMELT_MODULE_BINARY
$(error GCCMELT_MODULE_BINARY not defined)
endif

ifneq ($(filter %.so, $(GCCMELT_MODULE_BINARY)),)
$(error GCCMELT_MODULE_BINARY= $(GCCMELT_MODULE_BINARY) should not contain .so)
endif

ifndef GCCMELT_MODULE_WORKSPACE
GCCMELT_MODULE_WORKSPACE=.
endif

MELTMODULE_BASENAME:=$(basename $(GCCMELT_MODULE_SOURCE))
MELTMODULE_PLAIN:=$(notdir $(MELTMODULE_BASENAME))
MELTMODULE_SRCDIR:=$(patsubst %/, %, $(dir $(GCCMELT_MODULE_SOURCE)))


## these flags should only affect the quality of the generated
## binaries. No preprocessor flags please here!
GCCMELT_OPTIMIZED_FLAGS ?= -O1
GCCMELT_QUICKLYBUILT_FLAGS ?= -O0 -g1
GCCMELT_DYNAMIC_FLAGS ?= -O0
GCCMELT_NOLINE_FLAGS ?= -g

## The .d.so & .n.so suffixes are wired in melt-runtime.c!

MELTMODULE_DYNAMIC:= \
  $(patsubst %, %.d.so, $(GCCMELT_MODULE_BINARY))

MELTMODULE_OPTIMIZED:= \
  $(patsubst %, %.so, $(GCCMELT_MODULE_BINARY))

MELTMODULE_NOLINE:= \
  $(patsubst %, %.n.so, $(GCCMELT_MODULE_BINARY))

MELTMODULE_QUICKLYBUILT:= \
  $(patsubst %, %.q.so, $(GCCMELT_MODULE_BINARY))

MELTSTAMP:=$(GCCMELT_MODULE_WORKSPACE)/$(MELTMODULE_PLAIN)-stamp.c
MELTSTAMPOLD=$(MELTSTAMP)~
MELTMODULE_CFILES:=$(sort $(wildcard $(MELTMODULE_BASENAME).c $(MELTMODULE_BASENAME)+*.c))
MELTMODULE_OBJPICFILES:=$(patsubst %.c, $(GCCMELT_MODULE_WORKSPACE)/%.pic.o, $(notdir $(MELTMODULE_CFILES)))
MELTMODULE_OBJDYNPICFILES:=$(patsubst %.c, $(GCCMELT_MODULE_WORKSPACE)/%.dynpic.o, $(notdir $(MELTMODULE_CFILES)))
MELTMODULE_OBJNOLPICFILES:=$(patsubst %.c, $(GCCMELT_MODULE_WORKSPACE)/%.nolpic.o, $(notdir $(MELTMODULE_CFILES)))
MELTMODULE_OBJQUIPICFILES:=$(patsubst %.c, $(GCCMELT_MODULE_WORKSPACE)/%.quipic.o, $(notdir $(MELTMODULE_CFILES)))

ifndef GCCMELT_CC
GCCMELT_CC=gcc
endif

GCCMELT_CFLAGS ?= -O 


RM=rm -f
MD5SUM=md5sum
.PHONY: melt_module melt_module_dynamic  melt_module_rawdynamic melt_module_withoutline melt_clean

melt_module: $(MELTMODULE_OPTIMIZED)

melt_module_quicklybuilt: $(MELTMODULE_QUICKLYBUILT)
melt_module_dynamic: $(MELTMODULE_DYNAMIC)
melt_module_rawdynamic: $(MELTMODULE_DYNAMIC)
# melt_module_rawdynamic: override GCCMELT_CFLAGS +=  -DMELTGCC_DYNAMIC_OBJSTRUCT

melt_module_withoutline: $(MELTMODULE_NOLINE)
melt_module_rawwithoutline: $(GCCMELT_MODULE_BINARY)

$(MELTMODULE_OPTIMIZED): $(MELTMODULE_OBJPICFILES) $(MELTSTAMP)
	$(RM)  $(MELTSTAMPOLD)
	$(GCCMELT_CC) -DMELTGCC_MODULE_OPTIMIZED  $(GCCMELT_CFLAGS) $(GCCMELT_OPTIMIZED_FLAGS) -fPIC -shared \
	    $(MELTMODULE_OBJPICFILES) $(MELTSTAMP) -o $@
	mv $(MELTSTAMP) $(MELTSTAMPOLD)

$(MELTMODULE_DYNAMIC): $(MELTMODULE_OBJDYNPICFILES) $(MELTSTAMP)
	$(RM)  $(MELTSTAMPOLD)
	$(GCCMELT_CC) -DMELTGCC_MODULE_DYNAMIC  -DMELT_HAVE_DEBUG=1 -DMELTGCC_DYNAMIC_OBJSTRUCT $(GCCMELT_CFLAGS) \
	   $(GCCMELT_DYNAMIC_FLAGS) -fPIC -shared $(MELTMODULE_OBJDYNPICFILES) $(MELTSTAMP) -o $@
	mv $(MELTSTAMP) $(MELTSTAMPOLD)

$(MELTMODULE_NOLINE): $(MELTMODULE_OBJNOLPICFILES) $(MELTSTAMP)
	$(RM)  $(MELTSTAMPOLD)
	$(GCCMELT_CC) -DMELTGCC_MODULE_NOLINE  -DMELT_HAVE_DEBUG=1 -DMELTGCC_NOLINENUMBERING $(GCCMELT_CFLAGS) \
	  $(GCCMELT_NOLINE_FLAGS) -fPIC -shared $(MELTMODULE_OBJNOLPICFILES) $(MELTSTAMP) -o $@
	mv $(MELTSTAMP) $(MELTSTAMPOLD)

$(MELTMODULE_QUICKLYBUILT): $(MELTMODULE_OBJQUIPICFILES) $(MELTSTAMP)
	$(RM)  $(MELTSTAMPOLD)
	$(GCCMELT_CC)  -DMELTGCC_MODULE_QUICKLYBUILT -DMELT_HAVE_DEBUG=1 $(GCCMELT_CFLAGS) \
	   $(GCCMELT_QUICKLYBUILT_FLAGS) -fPIC -shared $(MELTMODULE_OBJQUIPICFILES) $(MELTSTAMP) -o $@
	mv $(MELTSTAMP) $(MELTSTAMPOLD)

$(GCCMELT_MODULE_WORKSPACE)/%.pic.o: $(MELTMODULE_SRCDIR)/%.c
	$(GCCMELT_CC) -DMELTGCC_MODULE_OPTIMIZED  $(GCCMELT_OPTIMIZED_FLAGS) $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<
$(GCCMELT_MODULE_WORKSPACE)/%.dynpic.o: $(MELTMODULE_SRCDIR)/%.c
	$(GCCMELT_CC) -DMELTGCC_MODULE_DYNAMIC  -DMELT_HAVE_DEBUG=1 -DMELTGCC_DYNAMIC_OBJSTRUCT $(GCCMELT_CFLAGS) \
	         $(GCCMELT_DYNAMIC_FLAGS)  -fPIC -c -o $@ $<
$(GCCMELT_MODULE_WORKSPACE)/%.quipic.o: $(MELTMODULE_SRCDIR)/%.c
	$(GCCMELT_CC) -DMELTGCC_MODULE_QUICKLYBUILT -DMELT_HAVE_DEBUG=1 $(GCCMELT_CFLAGS)  $(GCCMELT_QUICKLYBUILT_FLAGS) \
                 -fPIC -c -o $@ $<
$(GCCMELT_MODULE_WORKSPACE)/%.nolpic.o: $(MELTMODULE_SRCDIR)/%.c
	$(GCCMELT_CC)  -DMELTGCC_MODULE_NOLINE -DMELT_HAVE_DEBUG=1 -DMELTGCC_NOLINENUMBERING  $(GCCMELT_CFLAGS) \
              $(GCCMELT_NOLINE_FLAGS) -fPIC -c -o $@ $<

## There is only one make recipe. Trailing backslashes are essential.
## Even a parallel make should run it in sequence!
$(MELTSTAMP): $(MELTMODULE_CFILES)
	echo '/*' generated file $(MELTSTAMP) '*/' > $@-tmp;	\
	echo "const char melt_compiled_timestamp[]" >> $@-tmp;	\
	date "+ =\"%c $(MELTMODULE)\";" >> $@-tmp;		\
	echo "const char melt_md5[]=\"\\" >> $@-tmp;		\
	for f in $(MELTMODULE_CFILES); do			\
	  md5line=`$(MD5SUM) $$f` ;				\
	  printf "%s\\\n" $$md5line >> $@-tmp;			\
	done;							\
	echo "\";" >> $@-tmp;					\
	echo "const char melt_csource[]" >> $@-tmp;		\
	echo "  = \"$(MELTMODULE_CFILES)\";" >> $@-tmp;		\
	mv $@-tmp $@

melt_clean: 
	$(RM) $(MELTMODULE_OBJPICFILES) \
	   $(MELTMODULE_OBJNOLPICFILES) \
	   $(MELTMODULE_OBJDYNPICFILES) \
           $(MELTSTAMP) $(MELTSTAMPOLD) \
	   $(MELTMODULE_PLAIN) \
           $(MELTMODULE_DYNAMIC) \
           $(MELTMODULE_NOLINE)

## eof melt-module.mk

