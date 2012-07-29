#! /bin/bash
## file contrib/make-melt-source-tar.sh of the MELT branch of GCC
## building the plugin source distribution
## first argument is GCC MELT source tree
## second argument is the basename of the tar ball
## following optional arguments are gengtype -r gengtype.state ...
##
##    Middle End Lisp Translator = MELT
##
##    Copyright (C)  2010, 2011, 2012 Free Software Foundation, Inc.
##    Contributed by Basile Starynkevitch <basile@starynkevitch.net>
## 
## This file is part of GCC.
## 
## GCC is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3, or (at your option)
## any later version.
## 
## GCC is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with GCC; see the file COPYING3.   If not see
## <http://www.gnu.org/licenses/>.

## the first argument of this script is the source tree of the GCC
## MELT branch from which is extracted the MELT plugin source, for
## instance /usr/src/Lang/gcc-melt
gccmelt_source_tree=$1

## the second argument of this script is a temporary directory
## basename for the plugin source, for instance /tmp/gcc-melt-plugin
## and this script will then make a directory /tmp/gcc-melt-plugin and
## a gzipped tar archive /tmp/gcc-melt-plugin.tgz
gccmelt_tarbase=$2

shift 2
## the optional other arguments are used to invoke gengtype, for instance
## $(gcc-4.6 -print-file-name=gengtype) -v -r $(gcc-4.6 -print-file-name=gtype.state)
gengtype_prog=$1

if [ -n "$gengtype_prog" ]; then
    case $gengtype_prog in
	*gengtype*) 
	    shift 1; 
	    gengtype_args="$@";;
	*) 
	    echo $0: Bad optional gengtype $gengtype_prog >&2
	    exit 1
    esac
fi

if [ ! -f $gccmelt_source_tree/gcc/melt-runtime.h ]; then
    echo $0: Bad first argument for GCC MELT source tree $gccmelt_source_tree >&2
    exit 1
fi

if [ -z $gccmelt_tarbase ]; then
    echo $0 Bad second argument for GCC MELT tar ball base $gccmelt_tarbase >&2
    exit 1
fi

rm -rf $gccmelt_tarbase

mkdir -p $gccmelt_tarbase/melt/generated

date +"source tar timestamp %c" > $gccmelt_tarbase/GCCMELT-SOURCE-DATE

copymelt() {
    if [ -f $gccmelt_source_tree/$1 ]; then
	    cp -av $gccmelt_source_tree/$1 $gccmelt_tarbase/$2
    fi
}

## just a reminder
echo $0: You should have recently run in gcc/ of build tree: make upgrade-warmelt 

copymelt COPYING3
copymelt move-if-change
copymelt gcc/DATESTAMP GCCMELT-DATESTAMP
copymelt gcc/REVISION GCCMELT-REVISION

copymelt gcc/doc/include/fdl.texi 
copymelt gcc/doc/include/funding.texi 
copymelt gcc/doc/gnu.texi 
copymelt gcc/doc/include/gpl.texi 
copymelt gcc/doc/include/gpl_v3.texi 
copymelt gcc/doc/include/texinfo.tex

copymelt gcc/doc/melt.texi

copymelt contrib/melt-mv-if-changed.c 
copymelt contrib/meltplugin.texi 
copymelt contrib/meltpluginapi.texi

copymelt contrib/MELT-Plugin-Makefile
(cd $gccmelt_tarbase/; ln -s MELT-Plugin-Makefile Makefile)


## copy the non MELT files which are in MISSINGMELT_PLUGIN_HEADERS, that is
## the files which really should be available in the plugin/include/
## directory but are not yet.  Keep in sync with gcc/Makefile.in
copymelt gcc/realmpfr.h realmpfr.h
copymelt gcc/gimple-pretty-print.h gimple-pretty-print.h
copymelt gcc/tree-pretty-print.h tree-pretty-print.h
copymelt gcc/make-melt-predefh.awk make-melt-predefh.awk
copymelt gcc/make-warmelt-predef.awk make-warmelt-predef.awk
copymelt gcc/melt-build-script.tpl gcc/melt-build-script.def gcc/melt-build-script.sh


for mf in $gccmelt_source_tree/gcc/melt/*.melt ; do 
    cp -av $mf  $gccmelt_tarbase/melt/
done

for mf in $gccmelt_source_tree/gcc/melt/generated/*.[ch] ; do 
    cp -av $mf  $gccmelt_tarbase/melt/generated/
done


for mf in $gccmelt_source_tree/gcc/*melt*  ; do
    case $mf in
	*~) ;;
	*) copymelt gcc/$(basename $mf) ;;
    esac
done

for cf in  $gccmelt_source_tree/contrib/*melt*.sh $gccmelt_source_tree/contrib/pygmentize-melt ; do
    copymelt contrib/$(basename $cf) 
    chmod a+x $gccmelt_tarbase/$(basename $cf)
done

for cf in   $gccmelt_source_tree/contrib/simplemelt-gtkmm-probe.cc ; do
    copymelt contrib/$(basename $cf) 
done

copymelt INSTALL/README-MELT-PLUGIN
copymelt libmeltopengpu/meltopengpu-runtime.c

if [ -n "$gengtype_prog" ]; then
    gengtype_version=$($gengtype_prog -V | head -1 | awk '{print $NF}' 2>/dev/null)
    $gengtype_prog $gengtype_args -P $gccmelt_tarbase/gt-melt-runtime-$gengtype_version-plugin.h $gccmelt_tarbase/melt-runtime.h  $gccmelt_tarbase/melt/generated/meltrunsup.h
fi

tar -cvf $gccmelt_tarbase-tmp.tar \
   --exclude-backups --exclude='*~' --exclude='*%' \
   -C $(dirname $gccmelt_tarbase) $(basename $gccmelt_tarbase)

## we use tardy http://tardy.sourceforge.net/ to remove our name and
## group.. and make a tarball owned by melt/gcc

tardy -User_NAme melt -Group_NAme gcc $gccmelt_tarbase-tmp.tar $gccmelt_tarbase.tar 

rm -f $gccmelt_tarbase-tmp.tar 
gzip -v9 $gccmelt_tarbase.tar
