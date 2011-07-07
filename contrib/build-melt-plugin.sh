#! /bin/sh
## file contrib/build-melt-plugin.sh of the MELT branch of GCC
## run with -h to get some help.
##
##    Middle End Lisp Translator = MELT
##
##    Copyright (C)  2010, 2011 Free Software Foundation, Inc.
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

progname=$0

## We need the Posix/C locale (ie programmer's English), because we
## parse output of commands like gcc -v

export LANG=C 
export LC_ALL=C

################## our help & exit function
usage() {
    echo "$progname usage:" 
    echo " -h # gives this help" 
    echo " -q # to be quiet unless errors" 
    echo " -s EXPR       # *unsafely* evaluate EXPR, e.g. -s MAKE=gmake sets the MAKE variable" 
    echo " -M meltpath   # sets the GCC-MELT source subtree, e.g. /usr/src/gcc-melt/gcc" 
    echo " -Y your/gt-melt-runtime.h  # force the gengtype generated gt-melt-runtime.h" 
    echo ' -G gengtype     # gives the GCC gengtype utility, e.g. $(gcc-4.6 -print-file-name=gengtype)'
    echo ' -T gtype.state  # gives the GCC gengtype state file, e.g. $(gcc-4.6 -print-file-name=gtype.state)'
    echo " -b  # building MELT"
    echo " -i  # installing MELT"
    echo "$progname is for building MELT as a plugin, not a branch."
    echo "run $progname -h to get help" >&2
    exit 1
}

################## setting the default values of variables
set_default_variables() {
## upper case variables can be gotten from the shell's environment
# GNU make
    MAKE=${MAKE:-$(which gmake || which make)}
# host C compiler used to build the MELT plugin
    HOSTCC=${HOSTCC:-$(which cc || which gcc)}
# host C flags used to build the MELT plugin, could have -g also
    HOSTCFLAGS=${HOSTCFLAGS-"-O -DIN_GCC"}
# host C flags used to compile MELT generated C code
    HOSTMELTCFLAGS=${HOSTMELTCFLAGS-$HOSTCFLAGS}
# host make command to build MELT modules. [Could be $MAKE -j2]
    HOSTMELTMAKE=${HOSTMELTMAKE:-$MAKE}
# host administrator command prefix like sudo or super, or "echo
# should" or : to avoid installation
    HOSTADMINCMD=${HOSTADMINCMD:-}
# host GNU install command
    HOSTINSTALL=${HOSTINSTALL:-$(which install)}
# GCC compiler for which the MELT plugin is built
    GCC=${GCC:-$(which gcc || echo $CC)}
# GNU awk
    GAWK=${GAWK:-$(which gawk || which awk)}
# the GNU realpath command or else readlink
    REALPATH=${REALPATH:-$(which realpath || echo "readlink -m")}
# source tree of the MELT plugin, containing melt-runtime.c etc..
    GCCMELT_SOURCE_TREE=${GCCMELT_SOURCE_TREE:-""}
# gengtype flags
    GCCMELT_GENGTYPE_FLAGS=${GCCMELT_GENGTYPE_FLAGS:-"-v"}
# sleep delay between important steps
    MELTSLEEPDELAY=2
# destination directory DESTDIR could be set
# lower case variables are conventionnally local to this shell script
    gt_melt_runtime_header=""
    gcc_has_plugins=""
    gcc_version=""
    gcc_target=""
    gcc_plugin_directory=""
    gengtype_prog=""
    gengtype_state=""
    quiet=""
    install=""
    build=""
    timestamp_file=build-gccmelt-timestamp
    gtypelist_file=buildmelt-gtype-input.list
}

################ give a verbose message
verbose_echo() {
    if [ -z "$quiet" ]; then
	echo build-melt-plugin: $@ 
    fi
}

############### list verbosely a file with $1 as text
verbose_ls() {
    if [ -z "$quiet" ]; then
	echo build-melt-plugin: $1
	shift
	ls -l $*
    fi
}

################ give an error message & exit
error_echo() {
    echo build-melt-plugin error: $@ >&2
    exit 1
}

verbose_sleep() {
    if [ -z "$quiet" ]; then
	echo build-melt-plugin: sleeping $MELTSLEEPDELAY seconds so you can interrupt with Ctrl-C
	sleep  $MELTSLEEPDELAY ||  error_echo Interrupted while sleeping $MELTSLEEPDELAY seconds
    fi
}
################ parsing the shell program argument
parse_args() {
    progname=$0
    while getopts "hqbis:Y:M:C:G:T:" opt ; do
	case $opt in
	    h) usage;;
	    q) quiet=1;;
            b) 
		    verbose_echo build-melt-plugin: entering build mode
		    build=1;
		    install=""
		;;
            i) 
		    verbose_echo build-melt-plugin: entering install mode
		    build="";
		    install=1
		;;
	    s) 
		verbose_echo Evaluating $OPTARG;
		eval "$OPTARG"
		;;
           C) HOSTCFLAGS+=" $OPTARG"
              HOSTMELTCFLAGS+=" $OPTARG"
               verbose_echo using $OPTARG as host and melt compiler flags
               ;;
	    M) GCCMELT_SOURCE_TREE=$($REALPATH "$OPTARG")
		verbose_echo GCC-MELT source tree is $GCCMELT_SOURCE_TREE
		;;
	    Y) gt_melt_runtime_header=$($REALPATH "$OPTARG")
		verbose_echo Forcing use of generated MELT runtime GTY \
		    header $gt_melt_runtime_header without generating it \
		    thru gengtype

		;;
	    G) gengtype_prog=$($REALPATH "$OPTARG")
	       verbose_echo Using $gengtype_prog as the GCC gengtype utility for GTY and GCC garbage collector marking routines
	       ;;
	    T) gengtype_state=$($REALPATH "$OPTARG")
	       verbose_echo Using $gengtype_state as the gengtype state file
	       ;;
	esac
    done
}


################ sanity checks & retrieval of gcc info
sanity_checks_gcc_info() {
    verbose_echo doing sanity checks

# the GCCMELT source tree should have a melt-runtime.h & a
# melt/warmelt-first.melt file & a melt/generated/warmelt-first-0.c
# file
    if [ ! -f "$GCCMELT_SOURCE_TREE/melt-runtime.h" ]; then
	error_echo missing melt-runtime.h file in GCCMELT source tree $GCCMELT_SOURCE_TREE
    fi
    if [ ! -f "$GCCMELT_SOURCE_TREE/melt/warmelt-first.melt" ]; then
	error_echo missing melt/warmelt-first.melt file in GCCMELT source tree $GCCMELT_SOURCE_TREE
    fi
    if [ ! -f "$GCCMELT_SOURCE_TREE/melt/generated/warmelt-first-0.c" ]; then
	error_echo missing melt/generated/warmelt-first-0.c file in GCCMELT source tree $GCCMELT_SOURCE_TREE
    fi
# if $gt_melt_runtime_header is given, it should contain melt_globarr
# & ggc_root_tab strings
    if [ ! -z "$gt_melt_runtime_header" ]; then
	if [ ! -f  "$gt_melt_runtime_header" -o ! -s "$gt_melt_runtime_header" ]; then
	    error_echo bad forced MELT runtime GTY header $gt_melt_runtime_header
	fi
	if grep -q -e 'melt_globarr|ggc_root_tab' $gt_melt_runtime_header; then
	    error_echo invalid content in MELT runtime GTY header $gt_melt_runtime_header
	fi
    fi
########
    verbose_echo checking our GCC compiler $GCC
    verbose_sleep
## check our $GCC for plugin ability and version
    eval $($GCC -v < /dev/null 2>&1 \
	| $GAWK '/^Target:/{printf " gcc_target=%s", $2}
                 /^Configured with.*--enable-plugin/{print " gcc_has_plugins=yes"}
                 /^gcc version/{printf " gcc_version=%s", $3; split($3,vertab,"."); printf " gcc_version_number=%d\n", strtonum(vertab[1])*1000+strtonum(vertab[2])}')
    if [ "$gcc_has_plugins" != yes ]; then
	error_echo The GCC compiler $GCC does not have plugins enabled
    fi
    case $gcc_version in
	# we only support 4.6 or 4.7
	4.[67].*) verbose_echo MELT supported GCC version $gcc_version
	    ;;
	*) error_echo The GCC compiler $GCC version $gcc_version is incompatible with this MELT release
	    ;;
    esac
    gcc_plugin_directory=$($GCC -print-file-name=plugin)
    if [ -z "$gcc_plugin_directory" -o ! -s "$gcc_plugin_directory/include/gcc-plugin.h" ] ; then
	error_echo The GCC compiler $GCC has a bad plugin directory $gcc_plugin_directory
    fi
    ### If MELT is already installed, there is a conflict between
    ### generated MELT files.  We want the user to remove his previous
    ### installation of MELT before installing the new one
    if [ -s "$gcc_plugin_directory/melt.so" -o -f "$gcc_plugin_directory/include/melt-runtime.h" ] ; then
	error_echo "There is already a MELT plugin installed for $GCC, you should uninstall it before installing this one."
    fi    
    verbose_echo Generating a timestamp file $timestamp_file in $(pwd)
    rm -f $timestamp_file
    if date +"build-melt-plugin start %c%n" > $timestamp_file; then
	verbose_echo Timestamp done in $timestamp_file
    else
	error_echo "Failed to make a time stamp file $timestamp_file; the $(pwd) should be a writable current directory"
    fi
}

################ generate or copy the GTY MELT header file
get_gty_melt_header() {
    rm -f gt-melt-runtime.h
    if [ -s "$gt_melt_runtime_header" ] ; then
	verbose_echo Copying GTY MELT header file $gt_melt_runtime_header
	cp $gt_melt_runtime_header gt-melt-runtime.h
    else
	## generate it using gengtype
	if [ ! -x "$gengtype_prog" ]; then
	    error_echo "GCC Gengtype utility $gengtype_prog not executable"
	fi
	if [ ! -r "$gengtype_state" ]; then
	    error_echo "GCC Gengtype state file $gengtype_state not readable"
	fi
	verbose_echo Running $gengtype_prog with state  $gengtype_state to generate gt-melt-runtime.h
	$gengtype_prog $GCCMELT_GENGTYPE_FLAGS -r $gengtype_state -P gt-melt-runtime.h melt-runtime.h melt/generated/meltrunsup.h melt-runtime.c
    fi
    if [ ! -s gt-melt-runtime.h ]; then
	error_echo "failed to get or build gt-melt-runtime.h"
    fi
}


################ build melt-run.h and melt-run-md5.h
### See gcc/Makefile.in and keep in sync with it.
build_melt_run_headers() {  
    # build the melt-predef.h file
    verbose_echo generating melt-predef.h
    rm -f *.so melt-predef.h
    $GAWK -f $GCCMELT_SOURCE_TREE/make-melt-predefh.awk $GCCMELT_SOURCE_TREE/melt-predef.list > melt-predef.h
    verbose_echo Generated melt-predef.h with $(wc -l melt-predef.h) lines
    # 
    verbose_echo building melt-run.h and computing its md5 signature after preprocessing
    verbose_sleep
    rm -f melt-run.h melt-run-md5.h
    melt_run_md5=`$HOSTCC -C -E -DMELT_IS_PLUGIN -I$gcc_plugin_directory/include -I. -Imelt/generated melt-run.proto.h | grep -v '^#' | md5sum | cut -c 1-32`
    echo  "const char melt_run_preprocessed_md5[]=\"$melt_run_md5\";" > melt-run-md5.h
    sed -e "s,#define *MELT_RUN_HASHMD5 *XX,#define MELT_RUN_HASHMD5 \"$melt_run_md5\"," <  melt-run.proto.h > melt-run.h
    verbose_echo built melt-run.h with $(wc -c melt-run.h | cut -d ' ' -f 1) bytes and md5 $melt_run_md5
    verbose_echo in melt-run.h have $(grep MELT_RUN_HASHMD5 melt-run.h)
}

################ build melt.so with appropriate default settings
build_melt_dot_so() {
    # compile the melt.so file
    # the .../c-family subdirecty is needed for c-pragma.h
    host_full_cflags="$HOSTCFLAGS -fPIC -shared -DMELT_IS_PLUGIN -I. -I$PWD/melt/generated -I$GCCMELT_SOURCE_TREE -I$gcc_plugin_directory/include -I$gcc_plugin_directory/include/c-family -Wall"
    verbose_echo Building melt.so with $HOSTCC $host_full_cflags
    verbose_echo our HOSTMELTCFLAGS are $HOSTMELTCFLAGS
    verbose_sleep
    rm -f melt.so
    $HOSTCC $host_full_cflags \
	-DMELT_GCC_VERSION=$gcc_version_number \
	-DMELT_SOURCE_DIR=\"$gcc_plugin_directory/melt-source\" \
	-DMELT_MODULE_DIR=\"$gcc_plugin_directory/libexec/melt-modules\" \
	-DMELT_MODULE_MAKE_COMMAND=\"$HOSTMELTMAKE\" \
        -DMELT_REVISION=\"`tr -s [:space:] _ < GCCMELT-REVISION`\" \
	-DMELT_MODULE_CFLAGS="\"-DMELT_IS_PLUGIN $HOSTMELTCFLAGS -I$gcc_plugin_directory/include -O -DIN_GCC\"" \
	-DMELT_MODULE_MAKEFILE=\"$gcc_plugin_directory/melt-build-module.mk\" \
	-DMELT_DEFAULT_MODLIS=\"melt-default-modules\" \
	$GCCMELT_SOURCE_TREE/melt-runtime.c -o melt.so
    if [ ! -f melt.so ] ; then
	error_echo failed to build melt.so
	exit 1
    fi
    verbose_echo built melt.so
    # test that the melt.so plugin is loadable without any mode...
    # we need the empty-file-for-melt.c 
    verbose_echo making  empty-file-for-melt.c using $GCCMELT_SOURCE_TREE/melt-make.mk
    $MAKE -w -f $GCCMELT_SOURCE_TREE/melt-build.mk melt_make_move=mv empty-file-for-melt.c
    if $GCC -fplugin=./melt.so -c -o /dev/null empty-file-for-melt.c; then
	verbose_echo melt.so plugin seems to be loadable without mode
    else
	error_echo melt.so plugin dont work
	exit 1
    fi
    verbose_echo tried melt.so
}

################ bootstrap the MELT translator by making three stages

## utility function to make a stage of MELT
do_melt_make () {
    verbose_echo making MELT using $GCCMELT_SOURCE_TREE/melt-build.mk $*
    $MAKE -w -f $GCCMELT_SOURCE_TREE/melt-build.mk \
	melt_source_dir=$gcc_plugin_directory/melt-source \
	melt_module_dir=$gcc_plugin_directory/libexec/melt-modules \
	melt_make_module_makefile=$GCCMELT_SOURCE_TREE/melt-module.mk \
	melt_make_cc1="$GCC -fplugin=./melt.so -c -o /dev/null" \
	melt_make_cc1_dependency="" \
	melt_default_modules_list=melt-default-modules \
	melt_make_gencdeps=$timestamp_file \
	melt_make_source_dir=$GCCMELT_SOURCE_TREE/melt \
	melt_make_module_dir=. \
	melt_make_move=mv \
	melt_installed_cflags="$HOSTMELTCFLAGS -DMELT_IS_PLUGIN -I$gcc_plugin_directory/include" \
	melt_cflags="$HOSTMELTCFLAGS -DMELT_IS_PLUGIN  -I. -Imelt/generated -I$PWD -I$PWD/melt/generated -I$gcc_plugin_directory/include -I$GCCMELT_SOURCE_TREE" \
	melt_is_plugin=1 \
	VPATH=.:$GCCMELT_SOURCE_TREE/melt:$GCCMELT_SOURCE_TREE \
	$*
    if [ $? -ne 0 ]; then
	error_echo doing melt make $* failed
	exit 1
    else
	verbose_echo done melt make MELT $* sucessful
    fi
}
 
bootstrap_melt() {
    verbose_echo Before bootstrapping the MELT translator
    verbose_sleep
    ## generate the warmelt-predef.melt file
    verbose_echo Making warmelt-predef.melt
    rm -f warmelt-predef.melt
    if $GAWK -f $GCCMELT_SOURCE_TREE/make-warmelt-predef.awk $GCCMELT_SOURCE_TREE/melt-predef.list > warmelt-predef.melt; then
	verbose_echo Generated warmelt-predef.melt with $(wc -l warmelt-predef.melt| cut -d ' ' -f 1) lines
    else
	error_echo failed to generate warmelt-predef.melt
    fi
    verbose_echo Start regenerating MELT
    verbose_sleep
    ## zeroth stage
    verbose_echo Starting MELT stage zero
    if do_melt_make warmelt0 ; then
	verbose_echo Did MELT stage zero successfully
    else
	error_echo Failure in MELT stage zero
    fi
    ## first stage
    verbose_echo Starting MELT first stage
    if do_melt_make warmelt1 ; then
	verbose_echo Did MELT first stage successfully
    else
	error_echo Failure in MELT first stage
    fi
    ## second stage
    verbose_echo Starting MELT second stage
    if do_melt_make warmelt2 ; then
	verbose_echo Did MELT second stage successfully
    else
	error_echo Failure in MELT second stage
    fi
    ## third stage
    verbose_echo Starting MELT final stage
    if do_melt_make warmelt ; then
	verbose_echo Did MELT final stage successfully
    else
	error_echo Failure in MELT final stage
    fi
    ## default list of modules & documentation
    verbose_echo Building MELT default modules
    verbose_sleep
    if do_melt_make melt-default-modules.modlis ; then
	verbose_echo Did MELT default modules successfully
    else
	error_echo Failure in MELT defaut modules
    fi
    ### generate the documentation from source code
    verbose_echo Generating MELT documentation in texinfo format
    if do_melt_make meltgendoc.texi; then 
	verbose_echo Did generate MELT documentation
    else
	error_echo Failure in MELT documentation generation
    fi	
}

make_melt_documentation () {
    ### make the info files
    verbose_echo Making the MELT documentation info files
    makeinfo --force meltplugin.texi
    makeinfo --force  meltpluginapi.texi
    ## make the HTML files
    verbose_echo Making the MELT documentation HTML files
    texi2html meltplugin.texi
    texi2html --split section meltpluginapi.texi
}

################ after build, the installation procedure
install_melt() {
    verbose_echo Before installing MELT inside $gcc_plugin_directory
    verbose_sleep
    verbose_echo Installing the melt.so plugin and makefile
## if $DESTDIR is given, the $gcc_plugin_directory might not exist!
    $HOSTADMINCMD $HOSTINSTALL -d $DESTDIR$gcc_plugin_directory/
## melt.so should be installed inside $gcc_plugin_directory to enable
## using just -fplugin=melt
    $HOSTADMINCMD $HOSTINSTALL -D -m 755  melt.so $DESTDIR$gcc_plugin_directory/melt.so
    verbose_ls "the MELT plugin shared object" $DESTDIR$gcc_plugin_directory/melt.so
    $HOSTADMINCMD $HOSTINSTALL -D -m 755 $GCCMELT_SOURCE_TREE/melt-module.mk $DESTDIR$gcc_plugin_directory/melt-build-module.mk
    verbose_ls "MELT module maker" $DESTDIR$gcc_plugin_directory/melt-build-module.mk
    verbose_echo Installing MELT specific header files.
    $HOSTADMINCMD $HOSTINSTALL -d $DESTDIR$gcc_plugin_directory/include/
    $HOSTADMINCMD $HOSTINSTALL -m 644 $GCCMELT_SOURCE_TREE/melt-runtime.h  melt-predef.h $GCCMELT_SOURCE_TREE/melt/generated/meltrunsup.h melt-run.h melt-run-md5.h \
	$DESTDIR$gcc_plugin_directory/include/
    verbose_echo Installing missing GCC header files needed for MELT
    $HOSTADMINCMD $HOSTINSTALL -D -m 644 \
	$GCCMELT_SOURCE_TREE/gimple-pretty-print.h \
	$GCCMELT_SOURCE_TREE/tree-pretty-print.h \
	$GCCMELT_SOURCE_TREE/realmpfr.h \
	$DESTDIR$gcc_plugin_directory/include/
    verbose_echo Installing the MELT source directory
    $HOSTADMINCMD $HOSTINSTALL -D -m 755 -d $DESTDIR$gcc_plugin_directory/melt-source
    verbose_echo Populating the MELT source directory with MELT files
     $HOSTADMINCMD $HOSTINSTALL -D -m 644 warmelt-predef.melt $GCCMELT_SOURCE_TREE/melt/*.melt $DESTDIR$gcc_plugin_directory/melt-source/
    verbose_echo Populating the MELT source directory with generated C files and module catalog
    $HOSTADMINCMD $HOSTINSTALL -D -m 644 melt-sources/* melt-default-modules.modlis  $DESTDIR$gcc_plugin_directory/melt-source/
    verbose_echo Installing the MELT module directory
    $HOSTADMINCMD $HOSTINSTALL -D -m 755 -d $DESTDIR$gcc_plugin_directory/libexec/melt-modules
    verbose_echo Filling the installed MELT module directory
    $HOSTADMINCMD $HOSTINSTALL -D -m 755 melt-modules/*  $DESTDIR$gcc_plugin_directory/libexec/melt-modules/
}

################################################################
## main body
set_default_variables
parse_args "$@"
#
verbose_echo Building MELT plugin on $(hostname) for GCC compiler $GCC in $(pwd)
verbose_sleep
#
# here we go!
sanity_checks_gcc_info
if [ ! -z "$build" ]; then
	get_gty_melt_header
	build_melt_run_headers
	build_melt_dot_so
	bootstrap_melt
	make_melt_documentation
fi
if [ ! -z "$install" ]; then
	install_melt
fi

verbose_echo MELT plugin building terminated
if [ ! -z "$quiet" ]; then
    time
fi
