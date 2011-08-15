/* except.h                  -*-C++-*-
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
 * @file except.h
 *
 * @brief Common definitions for the various implementations of exception
 * handling.
 */

#ifndef INCLUDED_EXCEPT_DOT_H
#define INCLUDED_EXCEPT_DOT_H

#include <cilk/common.h>
#include <internal/abi.h>
#include "full_frame.h"

__CILKRTS_BEGIN_EXTERN_C

/**
 * OS-dependent information about an exception that's being moved between
 * strands.
 */
typedef struct pending_exception_info pending_exception_info;

/**
 * Merge the right exception record into the left.  The left is logically
 * earlier.
 *
 * On entry the left state is synched and can not have an unresolved
 * exception.  The merge may result in an unresolved exception.
 *
 * If there is both a right and left exception, the right exception will
 * be disposed of in preference to the left exception, destructing the
 * exception object.
 *
 * @param w The worker that is preparing to resume execution.
 * @param left_exception The exception that would have happened earlier
 * if the code executed serially.  Can be NULL if the left strand has not
 * raised an exception.
 * @param right_exception The exception that would have happened later
 * if the code executed serially.  Can be NULL if the right strand has not
 * raised an exception.
 *
 * @return NULL if there both the right and left exception are NULL. This
 * indicates that there are no pending exceptions.
 * @return The pending exception that is to be raised to continue searching
 * for a catch block to handle the exception.
 */
COMMON_SYSDEP
struct pending_exception_info *__cilkrts_merge_pending_exceptions(
    __cilkrts_worker *w,
    pending_exception_info *left_exception,
    pending_exception_info *right_exception);

/**
 * Move the exception information from the worker to the full_frame.
 *
 * @param w The worker which is suspending work on a full_frame.
 * @param f The full_frame which is being suspended.
 */
COMMON_SYSDEP
void __cilkrts_save_exception_state(__cilkrts_worker *w,
                                    full_frame *f);

/**
 * Function to delete pending exception.  This will delete the
 * exception object and then free the stack/fiber.
 *
 * @param w The worker we're running on.
 * @param pei The pending exception to be delete
 * @param delete_object Unused.  Should always be 1.
 */
void delete_exception_obj (__cilkrts_worker *w,
                           struct pending_exception_info *pei,
                           int delete_object);

#ifndef _WIN32
/* gcc-style exception handling */
NON_COMMON NORETURN __cilkrts_c_sync_except(__cilkrts_worker *w,
                                            __cilkrts_stack_frame *sf);
NON_COMMON void __attribute__((nonnull))
__cilkrts_gcc_rethrow(__cilkrts_stack_frame *sf);
#endif

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_EXCEPT_DOT_H)
