#! /bin/bash
# file tgcissue-pv-1run.sh
# to be run from  $GCCMELT_BUILD/gcc
if [ ! -x ./cc1 ]; then
    echo no ./cc1 file in $PWD >&2
    exit 1
fi

if [ ! -d melt-modules -o ! -d melt-sources ]; then
    echo no directories melt-modules or melt-sources in $PWD >&2
    exit 1
fi

if [ ! -d "$GCCMELT_SOURCE" ]; then
    echo Bad GCCMELT_SOURCE= "$GCCMELT_SOURCE" >&2
    exit 1
fi

if [ ! -d "$GCCMELT_BUILD" ]; then
    echo Bad GCCMELT_BUILD= "$GCCMELT_BUILD" >&2
    exit 1
fi

if [ ! -f "$GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1c.c" ]; then
    echo missing $GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1c.c >&2
    exit 1
fi

if [ ! -f "$GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1m.melt" ]; then
    echo missing $GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1m.melt >&2
    exit 1
fi

rm -vf tgcissue-pv-1c.*
rm -vf tgcissue-pv-1m.*

ln -sv  "$GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1c.c" .
ln -sv  "$GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1m.melt" .

echo translation of tgcissue-pv-1m.melt to a MELT module
cc1args=( -fmelt-module-path=melt-modules -fmelt-source-path=melt-sources \
    -fmelt-mode=translatetomodule  -fmelt-arg=tgcissue-pv-1m.melt \
    empty-file-for-melt.c)
echo running ./cc1 ${cc1args[@]}
./cc1 ${cc1args[@]}
if [ ! -f  tgcissue-pv-1m.so ]; then
    echo no generated MELT module tgcissue-pv-1m.so in $PWD >&2
    exit 1
fi

echo after translation of tgcissue-pv-1m.melt


cc1args=(  -fmelt-module-path=.:melt-modules -fmelt-source-path=.:melt-sources \
           -fmelt-init=@@:tgcissue-pv-1m \
           -O2 -v -fmelt-mode=tgcissue \
            tgcissue-pv-1c.c)
echo running ./cc1 ${cc1args[@]}
./cc1 ${cc1args[@]}
