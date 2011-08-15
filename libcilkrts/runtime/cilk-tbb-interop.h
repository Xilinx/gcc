/* cilk-tbb-interop.h                  -*-C++-*-
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
 * @file cilk-tbb-interop.h
 *
 * @brief Interface between TBB and Cilk to allow TBB to associate it's
 * per-thread data with Cilk workers, and maintain the association as work
 * moves between worker threads.  This handles the case where TBB calls
 * into a Cilk function which may later call back to a function making
 * TBB calls.
 *
 * Each thunk structure has two pointers: \"routine\" and \"data\".
 * The caller of the thunk invokes *routine, passing \"data\" as the void*
 * parameter.
 */

#ifndef INCLUDED_CILK_TBB_INTEROP_DOT_H
#define INCLUDED_CILK_TBB_INTEROP_DOT_H

#include <cilk/common.h>  // for CILK_EXPORT

__CILKRTS_BEGIN_EXTERN_C

/** A return code.  0 indicates success. */
typedef int __cilk_tbb_retcode;

/**
 * Enumeration of reasons that Cilk will call the TBB stack operation
 * function.
 *
 * When a non-empty stack is transfered between threads, the first thread must
 * orphan it and the second thread must adopt it.
 *
 * An empty stack can be transfered similarly, or simply released by the first
 * thread.
 *
 * Here is a summary of the actions as transitions on a state machine.
@verbatim
                       watch                                    ORPHAN
                       -->-->                                   -->--
                      /      \                                 /     \
   (freed empty stack)       (TBB sees stack running on thread)      (stack in limbo)
                      \     /                                  \     / 
                       --<--                                    --<--
                       RELEASE or                               ADOPT
                       unwatch
@endverbatim
 */
typedef enum __cilk_tbb_stack_op {
   /**
    * Disconnecting stack from a thread.
    *
    * The thunk must be invoked on the thread disconnecting itself from the
    * stack.  Must \"happen before\" the stack is adopted elsewhere.
    */
    CILK_TBB_STACK_ORPHAN,

    /**
     * Reconnecting orphaned stack to a thread.
     *
     * The thunk must be invoked on the thread adopting the stack.
     */
    CILK_TBB_STACK_ADOPT,

   /**
    * Releasing stack.
    *
    * The thunk must be invoked on the thread doing the releasing, Must
    * \"happen before\" the stack is used elsewhere.
    */
    CILK_TBB_STACK_RELEASE
} __cilk_tbb_stack_op;

/**
 * Function that will be called by the Cilk runtime to inform TBB of a change
 * in the stack associated with the current thread.
 *
 * It does not matter what stack the thunk runs on.
 * The thread (not fiber) on which the thunk runs is important.
 *
 * @param op Enumerated value indicating what type of change is ocurring.
 * @param data Context value provided by TBB in the __cilkrts_watch_stack
 * call.  This data is opaque to Cilk.
 *
 * @return 0 indicates success.
 */
typedef __cilk_tbb_retcode (*__cilk_tbb_pfn_stack_op)(enum __cilk_tbb_stack_op op,
                                                      void* data);

/**
 * Function that will be called by TBB to inform the Cilk runtime that TBB
 * is no longer interested in watching the stack bound to the current thread.
 *
 * @param data Context value provided to TBB by the __cilkrts_watch_stack
 * call.  This data is opaque to TBB.
 *
 * @return 0 indicates success.
 */
typedef __cilk_tbb_retcode (*__cilk_tbb_pfn_unwatch_stacks)(void *data);

/**
 * Thunk invoked by Cilk to call back to TBB to tell it about a change in
 * the stack bound to the current thread.
 */
typedef struct __cilk_tbb_stack_op_thunk {
    /// Function in TBB the Cilk runtime should call when something
    // "interesting" happens involving a stack
    __cilk_tbb_pfn_stack_op routine;

    /// TBB context data to pass with the call to the stack_op routine
    void* data;
} __cilk_tbb_stack_op_thunk;

/**
 * Thunk invoked by TBB when it is no longer interested in watching the stack
 * bound to the current thread.
 */
typedef struct __cilk_tbb_unwatch_thunk {
    /// Function in Cilk runtime to call when TBB no longer wants to watch
    // stacks
    __cilk_tbb_pfn_unwatch_stacks routine;

    /// Cilk runtime context data to pass with the call to the unwatch_stacks
    /// routine
    void* data;
} __cilk_tbb_unwatch_thunk;

/**
 * Requests that Cilk invoke __cilk_tbb_orphan_thunk when it orphans a stack.
 * Cilk sets *u to a thunk that TBB should call when it is no longer
 * interested in watching the stack.
 *
 * If the thread is not yet bound to the Cilk runtime, the Cilk runtime should
 * save this data in thread-local storage until __cilkrts_bind_thread is called.
 *
 * Called by TBB, defined by Cilk.  This function is exported from the Cilk
 * runtime DLL/shared object.  This declaration also appears in
 * cilk/cilk_undocumented.h -- don't change one declaration without also
 * changing the other.
 *
 * @param u __cilk_tbb_unwatch_thunk.  This structure will be filled in by
 * the Cilk runtime to allow TBB to register that it is no longer interested
 * in watching the stack bound to the current thread.
 * @param o __cilk_tbb_stack_op_thunk.  This structure specifies the routine
 * that the Cilk runtime should call when an "interesting" change in the stack
 * associate with the current worker occurs.
 *
 * @return 0 indicates success.
 */
CILK_EXPORT
__cilk_tbb_retcode __cilkrts_watch_stack(__cilk_tbb_unwatch_thunk* u,
                                         __cilk_tbb_stack_op_thunk o);

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_CILK_TBB_INTEROP_DOT_H)
