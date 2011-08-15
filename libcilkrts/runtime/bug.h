/* bug.h                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2009-2011 
 * Intel Corporation
 * 
 * This file is part of the Intel Cilk Plus Library.  This library is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * Under Section 7 of GPL version 3, you are granted additional
 * permissions described in the GCC Runtime Library Exception, version
 * 3.1, as published by the Free Software Foundation.
 * 
 * You should have received a copy of the GNU General Public License and
 * a copy of the GCC Runtime Library Exception along with this program;
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.
 **************************************************************************/

/**
 * @file bug.h
 *
 * @brief Support for reporting bugs and debugging.
 */

#ifndef INCLUDED_BUG_DOT_H
#define INCLUDED_BUG_DOT_H

#include "rts-common.h"
#include <cilk/common.h>

__CILKRTS_BEGIN_EXTERN_C

/**
 * Flush all output, write error message to stderr and abort the execution.
 * On Windows the error is also written to the debugger.
 *
 * @param fmt printf-style format string.  Any remaining parameters will be
 * be interpreted based on the format string text.
 */
COMMON_PORTABLE NORETURN __cilkrts_bug(const char *fmt,...) cilk_nothrow;

#ifndef CILK_ASSERT

/** Standard text for failed assertion */
COMMON_PORTABLE const char *const __cilkrts_assertion_failed;

/**
 * Macro to assert an invariant that must be true.  If the statement evalutes
 * to false, __cilkrts_bug will be called to report the failure and terminate
 * the application.
 */
#define CILK_ASSERT(ex)                                                 \
    (__builtin_expect((ex) != 0, 1) ? (void)0 :                         \
     __cilkrts_bug(__cilkrts_assertion_failed, __FILE__, __LINE__,  #ex))
#endif  // CILK_ASSERT

/**
 * Assert that there is no uncaught exception.
 */
COMMON_PORTABLE void cilkbug_assert_no_uncaught_exception(void);

/**
 * Call __cilkrts_bug with a standard message that the runtime state is
 * corrupted and the application is being terminated.
 */
COMMON_SYSDEP void abort_because_rts_is_corrupted(void);

// Debugging aids
#ifdef _WIN32

/**
 * Write debugging output.  On windows this is written to the debugger.
 *
 * @param fmt printf-style format string.  Any remaining parameters will be
 * be interpreted based on the format string text.
 */
COMMON_SYSDEP void __cilkrts_dbgprintf(const char *fmt,...) cilk_nothrow;

/**
 * Macro to write debugging output which will be elided if this is not a
 * debug build.  The macro is currently always elided on non-Windows builds.
 *
 * @param _fmt printf-style format string.  Any remaining parameters will be
 * be interpreted based on the format string text.
 */
#   ifdef _DEBUG
#       define DBGPRINTF(_fmt, ...) __cilkrts_dbgprintf(_fmt, __VA_ARGS__)
#   else
#       define DBGPRINTF(_fmt, ...)
#   endif  // _DEBUG

#else
    // Not yet implemented on the Unix side
#   define DBGPRINTF(_fmt, ...)
#endif  // _WIN32

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_BUG_DOT_H)
