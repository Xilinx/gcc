#! /bin/sh
## file contrib/make-melt-source-tar.sh of the MELT branch of GCC
##
##    Middle End Lisp Translator = MELT
##
##    Copyright (C)  2010 Free Software Foundation, Inc.
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

## first argument is GCC MELT source tree
## second argument is the basename of the tar ball

gccmelt_source_tree=$1
gccmelt_tarbase=$2

if [ ! -f $gccmelt_source_tree/gcc/melt-runtime.h ]; then
    echo $0: Bad first argument for GCC MELT source tree $1 >&2
    exit 1
fi

if [ -z $gccmelt_tarbase ]; then
    echo $0 Bad second argument for GCC MELT tar ball base $2 >&2
    exit 1
fi

rm -rf $gccmelt_tarbase

mkdir $gccmelt_tarbase
mkdir $gccmelt_tarbase/melt
mkdir $gccmelt_tarbase/melt/generated

date +"source tar timestamp %c" > $gccmelt_tarbase/GCCMELT-SOURCE-DATE

copymelt() {
    if [ -f $gccmelt_source_tree/$1 ]; then
	    cp -av $gccmelt_source_tree/$1 $gccmelt_tarbase/$2
    fi
}

## just a reminder
echo $0: You should have recently run in gcc/ of build tree: make upgrade-warmelt 

copymelt gcc/DATESTAMP GCCMELT-DATESTAMP
copymelt gcc/REVISION GCCMELT-REVISION

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

for cf in   $gccmelt_source_tree/contrib/gt*melt*.h ; do
    copymelt contrib/$(basename $cf) 
done
copymelt INSTALL/README-MELT-PLUGIN

tar czvf $gccmelt_tarbase.tgz $gccmelt_tarbase
