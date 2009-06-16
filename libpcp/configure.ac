# configure.ac for libpcp			-*- Autoconf -*-
# Process this file with autoconf to generate a configuration script.

# Copyright 2009 Free Software Foundation, Inc.

# This file is part of GCC.

# GCC is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2, or (at your option) any
# later #version.

# GCC is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License #for more details.

# You should have received a copy of the GNU General Public License
# along with GCC; see the file COPYING.  If not, write to the Free
# Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

AC_PREREQ(2.59)
AC_INIT(libpcp, [Polyhedral Compilation Package Library ], gcc-bugs@gcc.gnu.org, libpcp)
AC_CONFIG_SRCDIR(pcp.h)
AC_CONFIG_HEADER(auto-host.h:config.in)

# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_RANLIB

MISSING=`cd $ac_aux_dir && ${PWDCMD-pwd}`/missing
AC_CHECK_PROGS([ACLOCAL], [aclocal], [$MISSING aclocal])
AC_CHECK_PROGS([AUTOCONF], [autoconf], [$MISSING autoconf])
AC_CHECK_PROGS([AUTOHEADER], [autoheader], [$MISSING autoheader])

# Figure out what compiler warnings we can enable.
# See config/warnings.m4 for details.

ACX_PROG_CC_WARNING_OPTS([-W -Wall -Wwrite-strings -Wstrict-prototypes \
			  -Wmissing-prototypes -Wold-style-definition \
			  -Wmissing-format-attribute -Wcast-qual])
ACX_PROG_CC_WARNING_ALMOST_PEDANTIC([-Wno-long-long])

# Only enable with --enable-werror-always until existing warnings are
# corrected.
ACX_PROG_CC_WARNINGS_ARE_ERRORS([manual])

# Checks for header files.
AC_CHECK_HEADERS(stddef.h string.h stdio.h)
GCC_HEADER_STDINT(gstdint.h)

AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable rules only needed by maintainers],,
enable_maintainer_mode=no)

if test "x$enable_maintainer_mode" = xno; then
  MAINT='#'
else
  MAINT=
fi
AC_SUBST(MAINT)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_PROG_CXX
AC_HEADER_STDBOOL
AC_CHECK_FUNCS([floor])


AC_ARG_VAR(GMPLIBS,[How to link GMP])
AC_ARG_VAR(GMPINC,[How to find GMP include files])

AC_ARG_VAR(PPLLIBS,[How to link PPL])
AC_ARG_VAR(PPLINC,[How to find PPL include files])

AC_ARG_VAR(CLOOGLIBS,[How to link CLOOG])
AC_ARG_VAR(CLOOGINC,[How to find CLOOG include files])
if test x${CLOOGLIBS} = x ; then 
   AC_MSG_ERROR([cannot build libPCP without CLooG])
fi

# Output.

AC_CONFIG_HEADERS(config.h:config.in, [echo timestamp > stamp-h1])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
