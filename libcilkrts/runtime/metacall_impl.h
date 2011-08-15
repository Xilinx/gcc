/* metacall_impl.h                  -*-C++-*-
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

/**
 * @file metacall_impl.h
 *
 * @brief Meta-function calls to be used within the Cilk runtime system.
 * 
 * These differ from the macros in cilkscreen.h and cilkview.h because they go
 * through the __cilkrts_metacall interface, which ensures that the operation
 * is performed even when instrumentation is disabled.
 */

#ifndef INCLUDED_CILKRTS_METACALL_H
#define INCLUDED_CILKRTS_METACALL_H

#include "rts-common.h"
#include <internal/metacall.h>
#include <cilk/common.h>

__CILKRTS_BEGIN_EXTERN_C

/**
 * This function is effectively an unconditional call from the runtime into
 * a tool.  It is used for operations that must be performed by the tool,
 * even when the tool is not instrumenting.  For example, Cilkscreen always
 * recognizes the address of this function and performs the action specified
 * in the contained metadata.
 *
 * Note that this function MUST NOT BE INLINED within the runtime.  This must
 * be the ONLY instance of the cilkscreen_metacall metadata.
 */
CILK_API_VOID
__cilkrts_metacall(unsigned int tool, unsigned int code, void *data);

/**
 * Return non-zero if running under Cilkscreen or Cilkview
 */
COMMON_PORTABLE
int __cilkrts_running_under_sequential_ptool(void);

/**
 * Disable Cilkscreen implementation
 */
#define __cilkrts_cilkscreen_disable_instrumentation() \
    __cilkrts_metacall(METACALL_TOOL_SYSTEM, HYPER_DISABLE_INSTRUMENTATION, 0)

/**
 * Enable Cilkscreen implementation
 */
#define __cilkrts_cilkscreen_enable_instrumentation() \
    __cilkrts_metacall(METACALL_TOOL_SYSTEM, HYPER_ENABLE_INSTRUMENTATION, 0)

/**
 * Set the worker on entering runtime.
 *
 * @attention Deprecated in favor of __cilkrts_cilkscreen_ignore_block.  The
 * begin/enter pairs in the current metadata mean Cilkscreen no longer has to
 * have improper knowledge of the __cilkrts_worker or __cilkrts_stack_frame
 * structures.
 */
#define __cilkrts_cilkscreen_establish_worker(w) \
    __cilkrts_metacall(METACALL_TOOL_SYSTEM, HYPER_ESTABLISH_WORKER, w)

/**
 * Notify Cilkscreen of the extent of the stack.
 *
 * @param in begin Start (low address) of stack
 * @param in end   One past high address of stack
 */
void __cilkrts_cilkscreen_establish_c_stack(char *begin, char *end);

/**
 * Tell tools to ignore a block of memory - currently the global state and
 * memory allocated for workers.
 */
#define __cilkrts_cilkscreen_ignore_block(_begin, _end) \
{                                                       \
    void *block[2] = {_begin, _end};                    \
    __cilkrts_metacall(METACALL_TOOL_SYSTEM,            \
                       HYPER_IGNORE_MEMORY_BLOCK,       \
                       block);                          \
}

__CILKRTS_END_EXTERN_C

#endif /* ! defined(INCLUDED_CILKRTS_METACALL_H) */
