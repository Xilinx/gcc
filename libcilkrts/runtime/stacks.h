/* stacks.h                  -*-C++-*-
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
 * @file stacks.h
 *
 * @brief Cilk uses multiple stacks to implement work-stealing.
 * __cilkrts_stack is the OS-dependent representation of a stack.
 */

#ifndef INCLUDED_STACKS_DOT_H
#define INCLUDED_STACKS_DOT_H

#include <cilk/common.h>
#include "rts-common.h"
#include "global_state.h"

__CILKRTS_BEGIN_EXTERN_C

/**
 * Release a stack back to the cache for this worker.  If the cache is full,
 * this may in turn release the stack to the global cache, or free the memory.
 *
 * @param w __cilkrts_worker that is releasing the stack.
 * @param sd __cilkrts_stack that is to be released.
 */

COMMON_PORTABLE
void __cilkrts_release_stack(__cilkrts_worker *w,
                             __cilkrts_stack *sd);

/**
 * Initialize a __cilkrts_stack_cache structure.
 *
 * @param w __cilkrts_worker that the __cilkrts_stack_cache is to be created
 * for.
 * @param c __cilkrts_stack_cache that is to be initialized.
 * @param size Maximum number of stacks to allow in the cache.
 */
COMMON_PORTABLE
void __cilkrts_init_stack_cache(__cilkrts_worker *w,
                                __cilkrts_stack_cache *c,
                                unsigned int size);

/**
 * Free any cached stacks and the free any memory allocated by the
 * __cilkrts_stack_cache.
 *
 * @param w __cilkrts_worker the __cilkrts_stack_cache was created for.
 * @param g Pointer to the global_state_t.  Used when profiling to count number
 * of stacks outstanding.
 * @param c __cilkrts_stack_cache to be deallocated
 */
COMMON_PORTABLE
void __cilkrts_destroy_stack_cache(__cilkrts_worker *w,
                                   global_state_t *g,
                                   __cilkrts_stack_cache *c);

/**
 * Free all but one local stack, returning stacks to the global pool if
 * possible.
 *
 * @param w __cilkrts_worker holding the stack cache to be trimmed.
 */
COMMON_PORTABLE
void __cilkrts_trim_stack_cache(__cilkrts_worker *w);

/**
 * Allocate a stack, pulling one out of the worker's cache if possible.  If
 * no stacks are available in the local cache, attempt to allocate one from
 * the global cache (requires locking).  If a stack still hasn't be acquired,
 * attempt to create a new one.
 *
 * @return Allocated __cilkrts_stack.
 * @return NULL if a __cilkrts_stack cannot be created, either because we
 * ran out of memory or we hit the limit on the number of stacks that may be
 * created.
 */
COMMON_PORTABLE
__cilkrts_stack *__cilkrts_get_stack(__cilkrts_worker *w);

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_STACKS_DOT_H)
