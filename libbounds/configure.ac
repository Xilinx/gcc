# Process this file with autoconf to produce a configure script, like so:
# aclocal -I .. -I ../config && autoconf && autoheader && automake

AC_PREREQ(2.59)
AC_INIT(libbounds, 1.0)
AC_CONFIG_SRCDIR(bounds-runtime.c)
AC_CANONICAL_SYSTEM
ACX_NONCANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_MSG_CHECKING([for --enable-version-specific-runtime-libs])
AC_ARG_ENABLE(version-specific-runtime-libs,
[  --enable-version-specific-runtime-libs    Specify that runtime libraries should be installed in a compiler-specific directory ],
[case "$enableval" in
 yes) version_specific_libs=yes ;;
 no)  version_specific_libs=no ;;
 *)   AC_MSG_ERROR([Unknown argument to enable/disable version-specific libs]);;
 esac],
[version_specific_libs=no])
AC_MSG_RESULT($version_specific_libs)

AM_MAINTAINER_MODE
AC_EXEEXT

AM_ENABLE_MULTILIB(, ..)

target_alias=${target_alias-$host_alias}
AC_SUBST(target_alias)

AC_CONFIG_HEADERS(config.h)

AC_LANG_C
# The same as in boehm-gc and libstdc++. Have to borrow it from there.
# We must force CC to /not/ be precious variables; otherwise
# the wrong, non-multilib-adjusted value will be used in multilibs.
# As a side effect, we have to subst CFLAGS ourselves.

m4_rename([_AC_ARG_VAR_PRECIOUS],[real_PRECIOUS])
m4_define([_AC_ARG_VAR_PRECIOUS],[])
AC_PROG_CC
m4_rename([real_PRECIOUS],[_AC_ARG_VAR_PRECIOUS])

AC_SUBST(CFLAGS)

if test "x$GCC" != "xyes"; then
  AC_MSG_ERROR([libbounds must be built with GCC])
fi
AC_PROG_CPP

# Some hosts don't have dlsym(RTLD_NEXT, "symbol") for use in
# symbol interposition.  We disable shared libraries for these.
AC_MSG_CHECKING([whether dlsym(RTLD_NEXT,...) is available])
AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <dlfcn.h>
],
[void *foo = dlsym (RTLD_NEXT, "exit");],
[AC_MSG_RESULT(yes)],
[AC_MSG_RESULT(no)
enable_shared=no])

AC_CHECK_HEADERS(stdint.h execinfo.h signal.h dlfcn.h dirent.h pwd.h grp.h \
  netdb.h sys/ipc.h sys/sem.h sys/shm.h sys/wait.h ctype.h mntent.h \
  sys/socket.h netinet/in.h arpa/inet.h dlfcn.h sys/mman.h)

AC_CHECK_FUNCS(backtrace backtrace_symbols gettimeofday signal)
AC_CHECK_FUNCS(fopen64 fseeko64 ftello64 stat64 freopen64)
AC_CHECK_FUNCS(setbuf setbuffer setlinebuf setvbuf)
AC_CHECK_FUNCS(strnlen memrchr strncpy memmem sethostname)
AC_CHECK_FUNCS(__ctype_b_loc __ctype_tolower_loc __ctype_toupper_loc)
AC_CHECK_FUNCS(getlogin cuserid getpwnam getpwuid getpwent getgrnam getgrgid getgrent)
AC_CHECK_FUNCS(getlogin_r getpwnam_r getpwuid_r getgrnam_r getgrgid_r)
AC_CHECK_FUNCS(getservent getservbyname getservbyport getaddrinfo gai_strerror)
AC_CHECK_FUNCS(getprotoent getprotobyname getprotobynumber)
AC_CHECK_FUNCS(getmntent setmntent addmntent)
AC_CHECK_FUNCS(inet_ntoa mmap munmap)
AC_CHECK_FUNCS(__libc_freeres)

AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>],[union semun foo;], [mf_have_semun=1], [mf_have_semun=0])
if test $mf_have_semun = 1
then
  AC_DEFINE(HAVE_UNION_SEMUN, 1, [union semun defined in sys/ipc.h or sys/sem.h])
fi


AC_MSG_CHECKING([for socklen_t in sys/socket.h])
AC_TRY_COMPILE([#define _POSIX_PII_SOCKET
#include <sys/types.h>
#include <sys/socket.h>], [socklen_t x = 5;],
  [AC_DEFINE(HAVE_SOCKLEN_T, 1, [Define it socklen_t typedef is in sys/socket.h.])
   AC_MSG_RESULT(yes)],
  [AC_MSG_RESULT(no)])

AC_LIBTOOL_DLOPEN
AM_PROG_LIBTOOL
AC_SUBST(enable_shared)
AC_SUBST(enable_static)

AC_CHECK_HEADER(stdint.h, [MF_HAVE_STDINT_H=1], [MF_HAVE_STDINT_H=0])
AC_SUBST(MF_HAVE_STDINT_H)
if test $MF_HAVE_STDINT_H = 1
then
   MF_HAVE_UINTPTR_T=1
else
   AC_TRY_COMPILE([#include <sys/types.h>], [uintptr_t k = 0;],
	[MF_HAVE_UINTPTR_T=1], [MF_HAVE_UINTPTR_T=0])
fi
AC_SUBST(MF_HAVE_UINTPTR_T)

AC_CHECK_LIB(dl, dlsym)

# Calculate toolexeclibdir
# Also toolexecdir, though it's only used in toolexeclibdir
case ${version_specific_libs} in
  yes)
    # Need the gcc compiler version to know where to install libraries
    # and header files if --enable-version-specific-runtime-libs option
    # is selected.
    toolexecdir='$(libdir)/gcc/$(target_alias)'
    toolexeclibdir='$(toolexecdir)/$(gcc_version)$(MULTISUBDIR)'
    ;;
  no)
    if test -n "$with_cross_host" &&
       test x"$with_cross_host" != x"no"; then
      # Install a library built with a cross compiler in tooldir, not libdir.
      toolexecdir='$(exec_prefix)/$(target_alias)'
      toolexeclibdir='$(toolexecdir)/lib'
    else
      toolexecdir='$(libdir)/gcc-lib/$(target_alias)'
      toolexeclibdir='$(libdir)'
    fi
    multi_os_directory=`$CC -print-multi-os-directory`
    case $multi_os_directory in
      .) ;; # Avoid trailing /.
      *) toolexeclibdir=$toolexeclibdir/$multi_os_directory ;;
    esac
    ;;
esac
AC_SUBST(toolexecdir)
AC_SUBST(toolexeclibdir)

# Figure out whether the compiler supports "-ffunction-sections -fdata-sections",
# similarly to how libstdc++ does it
ac_test_CFLAGS="${CFLAGS+set}"
ac_save_CFLAGS="$CFLAGS"

# Check for -ffunction-sections -fdata-sections
AC_MSG_CHECKING([for gcc that supports -ffunction-sections -fdata-sections])
CFLAGS='-Werror -ffunction-sections -fdata-sections'
AC_TRY_COMPILE(, [int foo;], [ac_fdsections=yes], [ac_fdsections=no])
if test "$ac_test_CFLAGS" = set; then
  CFLAGS="$ac_save_CFLAGS"
else
  # this is the suspicious part
  CFLAGS=""
fi
if test x"$ac_fdsections" = x"yes"; then
  SECTION_FLAGS='-ffunction-sections -fdata-sections'
fi
AC_MSG_RESULT($ac_fdsections)
AC_SUBST(SECTION_FLAGS)


# Check for the name of the symbol used for the entry point.
AC_CACHE_CHECK([for the name of the symbol used for the entry point],
  [mudflap_cv_entry_point], [
for name in _start __start unknown; do
  AC_LINK_IFELSE([AC_LANG_PROGRAM([extern char $name@<:@@:>@;], [$name@<:@0@:>@ = 0;])],
		 [break])
done
mudflap_cv_entry_point="$name"])
if test "$mudflap_cv_entry_point" = unknown; then
  AC_MSG_ERROR([none of the known symbol names works])
fi
AC_DEFINE_UNQUOTED([ENTRY_POINT], [$mudflap_cv_entry_point],
  [Define to the name of the symbol used for the entry point.])


if test ${multilib} = yes; then
  multilib_arg="--enable-multilib"
else
  multilib_arg=
fi

# See if we support thread-local storage.
GCC_CHECK_TLS

AC_CONFIG_FILES([Makefile testsuite/Makefile testsuite/boundsconfig.exp])
AC_OUTPUT
