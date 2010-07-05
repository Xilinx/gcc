# Process this file with autoconf to produce a configure script, like so:
# aclocal && autoconf && autoheader && automake

AC_PREREQ(2.64)
AC_INIT([GNU Python Runtime Library], 0.1,,[libgpy])
AC_CONFIG_HEADER(config.h)
GCC_TOPLEV_SUBDIRS

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AM_SANITY_CHECK

AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_CHECK_PROGS(AR, ar aal, ar)
AC_PROG_CC_C99
AC_PROG_CXX

AC_PROG_INSTALL

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

AM_PROG_CC_C_O

AC_C_BIGENDIAN
AC_C_INLINE
AC_C_VOLATILE

AC_CANONICAL_HOST

# a lang needs math functions...
AC_CHECK_LIB([m], [pow], , 
  [AC_MSG_ERROR([System Math library is required!])])

# used for constant folding and rounding
AC_CHECK_LIB([gmp], [__gmpz_init], ,
  [AC_MSG_ERROR([GNU MP not found, see http://gmplib.org/])])
AC_CHECK_LIB([mpfr],[mpfr_init] , ,
  [AC_MSG_ERROR([MPFR not found, see http://www.mpfr.org/])])
AC_CHECK_SIZEOF(mp_limb_t, , [#include <gmp.h>])

VL_LIB_READLINE
AS_IF([test "x$vl_cv_lib_readline" = "xno"],
      AC_MSG_ERROR([libreadline is required for Crules. \
On Debian this can be found in libreadline5-dev. On RedHat \
this can be found in readline-devel.]))
AC_CHECK_FUNCS(dlopen)

gl_VISIBILITY

AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)

AC_PATH_PROG(MV, mv, mv)
AC_PATH_PROG(RM, rm, rm)
AC_PATH_PROG(CP, cp, cp)
AC_PATH_PROG(SED, sed, sed)
AC_PATH_PROG(CMP, cmp, cmp)
AC_PATH_PROG(CHMOD, chmod, chmod)
AC_PATH_PROG(HOSTNAME, hostname, hostname)

AM_INIT_AUTOMAKE

AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(void*)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(long int)
AC_CHECK_SIZEOF(long long)

# Checks for header files.
AC_HEADER_SYS_WAIT
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h stdarg.h unistd.h \
		  stdint.h stdio.h getopt.h \
		  assert.h sys/types.h signal.h \
		  fcntl.h pthread.h sys/wait.h \
		  gmp.h mpfr.h pcre.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_FUNCS([popen fopen fclose sprintf fprintf strdup \
		strlen strcpy strcmp getopt_long \
		memcpy calloc system sysconf atoi \
		getpid execl fork wait exit atof \
		vfprintf memcmp getc fgets pipe \
		waitpid fdopen])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
dnl libtoolize scans configure.ac  and needs to see some text
m4_define([LIBTOOLIZE_AC_INIT], [])