/* cilkscreen.h                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2010 
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
 *
 **************************************************************************/

#ifndef INCLUDED_CILKSCREEN_H
#define INCLUDED_CILKSCREEN_H

#include <cilk/cilk_api.h>

/*
 * Cilkscreen "functions".  These macros generate metadata in your application
 * to notify Cilkscreen of program state changes
 */

#if ! defined(CILK_STUB) && defined(__INTEL_COMPILER)
#  define __cilkscreen_metacall(annotation,expr) \
    __notify_intrinsic(annotation,expr)
#else
#  define __cilkscreen_metacall(annotation,expr) (annotation, (void) (expr))
#endif

/* Call once when a user thread enters a spawning function */
#define __cilkscreen_enable_instrumentation() \
    __cilkscreen_metacall("cilkscreen_enable_instrumentation", 0)

/* Call once when a user thread exits a spawning function */
#define  __cilkscreen_disable_instrumentation() \
    __cilkscreen_metacall("cilkscreen_disable_instrumentation", 0)

/* Call to temporarily disable cilkscreen instrumentation */
#define __cilkscreen_enable_checking() \
    __cilkscreen_metacall((void*)"cilkscreen_enable_checking", 0)

/* Call to re-enable temporarily-disabled cilkscreen instrumentation */
#define __cilkscreen_disable_checking() \
    __cilkscreen_metacall((void*)"cilkscreen_disable_checking", 0)

/* Inform cilkscreen that memory from begin to end can be reused without
 * causing races (e.g., for memory that comes from a memory allocator) */
#define __cilkscreen_clean(begin, end)                      \
    do {                                                    \
        void *__data[2] = { (begin), (end) };               \
        __cilkscreen_metacall("cilkscreen_clean", &__data); \
    } while(0)

/* Inform cilkscreen that a lock is being acquired.
 * If the lock type is not a handle, then the caller should take its address
 * and pass the pointer to the lock.  Otherwise, the caller should pass the
 * lock handle directly.
 */
#define __cilkscreen_acquire_lock(lock) \
    __cilkscreen_metacall("cilkscreen_acquire_lock", (lock))

#define __cilkscreen_release_lock(lock) \
    __cilkscreen_metacall("cilkscreen_release_lock", (lock))

/*
 * Metacall data
 *
 * A metacall is a way to pass data to a function implemented by a tool.
 * Metacalls are always instrumented when the tool is loaded.
 */

// Tool code for Cilkscreen
#define METACALL_TOOL_CILKSCREEN 1

// Metacall codes implemented by Cilkscreen
#define CS_METACALL_PUTS 0  // Write string to the Cilkscreen log

#define __cilkscreen_puts(text) \
    __cilkrts_metacall(METACALL_TOOL_CILKSCREEN, CS_METACALL_PUTS, text)

#endif /* defined(INCLUDED_CILKSCREEN_H) */
