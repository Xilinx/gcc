## file melt-module.mk
## a -*- Makefile -*- for GNU make. 

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

### should be invoked, perhaps from melt-runtime.c, with 
#### make -f melt-module.mk \
####    GCCMELT_MODULE_SOURCE=foo.c \
####    GCCMELT_MODULE_BINARY=foo.so \
####    GCCMELT_MODULE_WORKSPACE=/tmp 


ifndef GCCMELT_MODULE_SOURCE
$(error GCCMELT_MODULE_SOURCE not defined)
endif

ifndef GCCMELT_MODULE_BINARY
$(error GCCMELT_MODULE_BINARY not defined)
endif

ifndef GCCMELT_MODULE_WORKSPACE
GCCMELT_MODULE_WORKSPACE=.
endif

MELTMODULE_BASENAME:=$(basename $(GCCMELT_MODULE_SOURCE))
MELTMODULE_PLAIN:=$(notdir $(MELTMODULE_BASENAME))
MELTMODULE_SRCDIR:=$(dir $(GCCMELT_MODULE_SOURCE))
MELTMODULE_DYNAMIC:=$(patsubst %.so, %.d.so, $(GCCMELT_MODULE_BINARY))
MELTMODULE_NOLINE:=$(patsubst %.so, %.n.so, $(GCCMELT_MODULE_BINARY))

MELTSTAMP:=$(GCCMELT_MODULE_WORKSPACE)/$(MELTMODULE_PLAIN)-stamp.c

MELTMODULE_CFILES:=$(wildcard $(MELTMODULE_BASENAME).c $(MELTMODULE_BASENAME)+*.c)
MELTMODULE_OBJPICFILES:=$(patsubst %.c, $(GCCMELT_MODULE_WORKSPACE)/%.pic.o, $(notdir $(MELTMODULE_CFILES)))
MELTMODULE_OBJDYNPICFILES:=$(patsubst %.c, $(GCCMELT_MODULE_WORKSPACE)/%.dynpic.o, $(notdir $(MELTMODULE_CFILES)))
MELTMODULE_OBJNOLPICFILES:=$(patsubst %.c, $(GCCMELT_MODULE_WORKSPACE)/%.nolpic.o, $(notdir $(MELTMODULE_CFILES)))

ifndef GCCMELT_CC
GCCMELT_CC=gcc
endif

ifndef GCCMELT_CFLAGS
GCCMELT_CFLAGS=-O 
endif

RM=rm -f
MD5SUM=md5sum
.PHONY: meltmodule meltmoduledynamic  meltmodulerawdynamic meltmodulewithoutline meltclean

meltmodule: $(GCCMELT_MODULE_BINARY)

meltmoduledynamic: $(MELTMODULE_DYNAMIC)
meltmodulerawdynamic: $(GCCMELT_MODULE_BINARY)
# meltmodulerawdynamic: override GCCMELT_CFLAGS +=  -DMELTGCC_DYNAMIC_OBJSTRUCT

meltmodulewithoutline: $(MELTMODULE_NOLINE)
meltmodulerawwithoutline: $(GCCMELT_MODULE_BINARY)

$(GCCMELT_MODULE_BINARY): $(MELTMODULE_OBJPICFILES) $(MELTSTAMP)
	$(GCCMELT_CC) $(GCCMELT_CFLAGS) -fPIC -shared \
	    $(MELTMODULE_OBJPICFILES) $(MELTSTAMP) -o $@
	$(RM) $(MELTSTAMP)

$(MELTMODULE_DYNAMIC): $(MELTMODULE_OBJDYNPICFILES) $(MELTSTAMP)
	$(GCCMELT_CC) $(GCCMELT_CFLAGS) -DMELTGCC_DYNAMIC_OBJSTRUCT \
	   -fPIC -shared $(MELTMODULE_OBJDYNPICFILES) $(MELTSTAMP) -o $@
	$(RM) $(MELTSTAMP)

$(MELTMODULE_NOLINE): $(MELTMODULE_OBJNOLPICFILES) $(MELTSTAMP)
	$(GCCMELT_CC) $(GCCMELT_CFLAGS) -DMELTGCC_NOLINENUMBERING \
	   -fPIC -shared $(MELTMODULE_OBJNOLPICFILES) $(MELTSTAMP) -o $@
	$(RM) $(MELTSTAMP)

$(GCCMELT_MODULE_WORKSPACE)/%.pic.o: $(MELTMODULE_SRCDIR)/%.c
	$(GCCMELT_CC) $(GCCMELT_CFLAGS) -fPIC -c -o $@ $<
$(GCCMELT_MODULE_WORKSPACE)/%.dynpic.o: $(MELTMODULE_SRCDIR)/%.c
	$(GCCMELT_CC) $(GCCMELT_CFLAGS)  -DMELTGCC_DYNAMIC_OBJSTRUCT -fPIC -c -o $@ $<
$(GCCMELT_MODULE_WORKSPACE)/%.nolpic.o: $(MELTMODULE_SRCDIR)/%.c
	$(GCCMELT_CC) $(GCCMELT_CFLAGS)  -DMELTGCC_NOLINENUMBERING -fPIC -c -o $@ $<

$(MELTSTAMP): $(MELTMODULE_CFILES)
	echo '/*' generated file $(MELTSTAMP) '*/' > $@-tmp
	date "+const char melt_compiled_timestamp[]=\"%c $(MELTMODULE)\";" >> $@-tmp
	echo "const char melt_md5[]=\"\\" >> $@-tmp
	for f in $(MELTMODULE_CFILES); do \
	  md5line=`$(MD5SUM) $$f` ; \
	  printf "%s\\\n" $$md5line >> $@-tmp; \
	done
	echo "\";" >> $@-tmp
	echo "const char melt_csource[]=\"$(MELTMODULE_CFILES)\";" >> $@-tmp
	mv $@-tmp $@

meltclean: 
	$(RM) $(MELTMODULE_OBJPICFILES) \
	   $(MELTMODULE_OBJNOLPICFILES) \
	   $(MELTMODULE_OBJDYNPICFILES) \
           $(MELTSTAMP) \
	   $(GCCMELT_MODULE_BINARY) \
           $(MELTMODULE_DYNAMIC) \
           $(MELTMODULE_NOLINE)

## eof melt-module.mk