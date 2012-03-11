/*
 * Copyright (C) 2009-2010 
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
 ******************************************************************************
 *
 * cilk_undocumented.h
 *
 * This file defines exported functions that are not included in the standard
 * documentation.
 */

#ifndef INCLUDED_CILK_UNDOCUMENTED_H
#define INCLUDED_CILK_UNDOCUMENTED_H

#include <cilk/common.h>

#ifndef CILK_STUB

__CILKRTS_BEGIN_EXTERN_C

/*
 * __cilkrts_synched
 *
 * Allows an application to determine if there are any outstanding
 * children at this instant.  This function will examine the current
 * full frame to determine this.
 */

CILK_EXPORT __CILKRTS_NOTHROW
int __cilkrts_synched(void);

/*
 * __cilkrts_cilkscreen_puts
 *
 * Allows an application to write a string to the Cilkscreen log.
 * The standard error stream will be flushed after the write.
 */

CILK_EXPORT __CILKRTS_NOTHROW
void __cilkrts_cilkscreen_puts(const char *);

/*
 * __cilkrts_get_sf
 *
 * A debugging aid that allows an application to get the __cilkrts_stack_frame
 * for the current function.  Only compiled into the DLL in debug builds.
 */

CILK_EXPORT __CILKRTS_NOTHROW
void *__cilkrts_get_sf(void);

struct __cilkrts_worker;

CILK_EXPORT __CILKRTS_NOTHROW
void __cilkrts_init_worker_sysdep(struct __cilkrts_worker *w);

CILK_EXPORT __CILKRTS_NOTHROW
int __cilkrts_irml_version(void);

struct __cilk_tbb_unwatch_thunk;
struct __cilk_tbb_stack_op_thunk;

CILK_EXPORT __CILKRTS_NOTHROW
int __cilkrts_watch_stack(struct __cilk_tbb_unwatch_thunk *u,
                          struct __cilk_tbb_stack_op_thunk o);

#ifdef _WIN32
/* Do not use CILK_API because __cilkrts_worker_stub must be __stdcall */
CILK_EXPORT unsigned __CILKRTS_NOTHROW __stdcall
__cilkrts_worker_stub(void *arg);
#else
/* Do not use CILK_API because __cilkrts_worker_stub have defauld visibility */
__attribute__((visibility("default")))
void* __CILKRTS_NOTHROW __cilkrts_worker_stub(void *arg);
#endif

__CILKRTS_END_EXTERN_C

#else /* CILK_STUB */

/* Stubs for the api functions */

#define __cilkrts_synched() (1)

#endif /* CILK_STUB */

#endif /* INCLUDED_CILK_UNDOCUMENTED_H */
