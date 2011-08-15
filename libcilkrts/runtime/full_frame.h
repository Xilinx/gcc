/* full_frame.h                  -*-C++-*-
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

#ifndef INCLUDED_FULL_FRAME_DOT_H
#define INCLUDED_FULL_FRAME_DOT_H

#include "rts-common.h"
#include "worker_mutex.h"

#include <cilk/common.h>

#include <stddef.h>

__CILKRTS_BEGIN_EXTERN_C

// Forwarded declarations
typedef struct __cilkrts_stack_frame __cilkrts_stack_frame;
typedef struct __cilkrts_stack __cilkrts_stack;

/** Magic numbers for full_frame, used for debugging */
typedef unsigned long long ff_magic_t;

/* COMMON_SYSDEP */ struct pending_exception_info;  /* opaque */
/* COMMON_SYSDEP */ struct __cilkrts_stack;         /* opaque */

/*************************************************************
  Full frames
*************************************************************/

/**
 * @file full_frame.h
 * @brief A full frame includes additional information such as a join
 * counter and parent frame.
 * @defgroup FullFrames Full Frames
 * A full frame includes additional information such as a join
 * counter and parent frame.
 * @{
 */

/**
 * Convenience typedef so we don't have to specify "struct full_frame"
 * all over the code.  Putting it before the structure definition allows
 * us to use the typedef within the structure itself
 */
typedef struct full_frame full_frame;

/**
 * @brief A full frame includes additional information such as a join
 * counter and parent frame.
 *
 * The frame at the top of a worker's stack is promoted into a "full"
 * frame, which carries additional information, such as join counter
 * and parent frame.  Full frames can be suspended at a sync, in which
 * case they lie somewhere in memory and do not belong to any
 * worker. 
 *
 * Full frames are in contrast to the entries in the worker's deque which
 * are only represented by a pointer to their __cilkrts_stack_frame. */

/* COMMON_PORTABLE */ 
struct full_frame
{
    /**
     * Value to detect writes off the beginning of a full_frame.
     */
#   define FULL_FRAME_MAGIC_0 ((ff_magic_t)0x361e710b9597d553ULL)

    /**
     * Field to detect writes off the beginning of a full_frame.  Must be
     * FULL_FRAME_MAGIC_0.
     */
    ff_magic_t full_frame_magic_0;

    /** Used to serialize access to this full_frame */
    struct mutex lock;

    /** Count of outstanding children running in parallel */
    int join_counter;

    /**
     * If TRUE: frame was called by the parent.
     * If FALSE: frame was spawned by parent.
     */
    int is_call_child;

    /**
     * TRUE if this frame is the loot of a simulated steal.
     *
     * This situation never happens in normal execution.  However,
     * when running under cilkscreen, a worker may promote frames and
     * then immediately suspend them, in order to simulate an
     * execution on an infinite number of processors where all spawns
     * are stolen.  In this case, the frame is marked as the loot of a fake
     * steal.
     */
    int simulated_stolen;

    /** Caller of this full_frame */
    full_frame *parent;

    /**
     * Doubly-linked list of children.  The serial execution order is
     * by definition from left to right.  Because of how we do work
     * stealing, the parent is always to the right of all its
     * children.
     */
    full_frame *left_sibling;

    /** @copydoc left_sibling */
    full_frame *right_sibling;

    /** Pointer to rightmost child */
    full_frame *rightmost_child;

    /**
     * Call stack associated with this frame.
     * Set and reset in make_unrunnable and make_runnable
     */
    __cilkrts_stack_frame *call_stack;

    /**
     * Stable copy of call stack associated with this frame - never NULLed.
     */
    __cilkrts_stack_frame *stable_call_stack;

    /** Map from reducer names to reducer values */
    struct cilkred_map *reducer_map;

    /** Accumulated reducers of children */
    struct cilkred_map *children_reducer_map;

    /**
     * Accumulated reducers of right siblings that have already
     * terminated
     */
    struct cilkred_map *right_reducer_map;

    /** Exception that needs to be pass to our parent */
    struct pending_exception_info *pending_exception;

    /** Exception from one of our children */
    struct pending_exception_info *child_pending_exception;

    /** Exception from any right siblings */
    struct pending_exception_info *right_pending_exception;

    /** Stack pointer to restore on sync. */
    char *sync_sp;

#ifdef _WIN32
    /** Stack pointer to restore on exception. */
    char *exception_sp;

    /** Exception trylevel at steal */
    unsigned long trylevel;

    /** Exception registration head pointer to restore on sync. */
    unsigned long registration;
#endif

    /** Size of frame to match sync sp */
    ptrdiff_t frame_size;

    /**
     * Allocated stacks that need to be freed.  The stacks work
     * like a reducer.  The leftmost frame may have stack_self
     * null and owner non-null.
     */
    __cilkrts_stack *stack_self;

    /**
     * Allocated stacks that need to be freed.  The stacks work
     * like a reducer.  The leftmost frame may have stack_self
     * null and owner non-null.
     */
    __cilkrts_stack *stack_child;

    /**
     * If the sync_master is set, this function can only be sync'd by the team
     * leader, who first entered Cilk.  This is set by the first worker to steal
     * from the user worker.
     */
    __cilkrts_worker *sync_master;

    /**
     * Value to detect writes off the end of a full_frame.
     */
#   define FULL_FRAME_MAGIC_1 ((ff_magic_t)0x189986dcc7aee1caULL)

    /**
     * Field to detect writes off the end of a full_frame.  Must be
     * FULL_FRAME_MAGIC_1.
     */
    ff_magic_t full_frame_magic_1;
};

/* The functions __cilkrts_put_stack and __cilkrts_take_stack keep track of
 * changes in the stack's depth between when the point at which a frame is
 * stolen and when it is resumed at a sync.  A stolen frame typically goes
 * through the following phase changes:
 *
 *   1. Suspend frame while stealing it.
 *   2. Resume stolen frame at begining of continuation
 *   3. Suspend stolen frame at a sync
 *   4. Resume frame (no longer marked stolen) after the sync
 *
 * When the frame is suspended (steps 1 and 3), __cilkrts_put_stack is called to
 * establish the stack pointer for the sync.  When the frame is resumed (steps
 * 2 and 4), __cilkrts_take_stack is called to indicate the stack pointer
 * (which may be on a different stack) at
 * the point of resume.  If the stack pointer changes between steps 2 and 3,
 * e.g., as a result of pushing 4 bytes onto the stack,
 * the offset is reflected in the value of f->sync_sp after step 3 relative to
 * its value after step 1 (e.g., the value of f->sync_sp after step 3 would be
 * 4 less than its value after step 1, for a down-growing stack).
 *
 * Imp detail: The actual call chains for each of these phase-change events is:
 *
 *   1. unroll_call_stack -> make_unrunnable  -> __cilkrts_put_stack
 *   2. do_work           -> __cilkrts_resume -> __cilkrts_take_stack
 *   3. do_sync -> disown -> make_runnable    -> __cilkrts_put_stack
 *   4. __cilkrts_resume                      -> __cilkrts_take_stack
 *
 * (The above is a changeable implementation detail.  The resume, sequence, in
 * particular, is more complex on some operating systems.)
 */

/**
 * Records the stack pointer within the 'sf' stack frame as the current stack
 * pointer at the point of suspending full frame 'f'.
 *
 * Preconditions:
 *   - f->sync_sp must be either null or contain the result of a prior call to
 *     __cilkrts_take_stack().
 *   - If f->sync_sp is not null, then SP(sf) must refer to the same stack as
 *     the 'sp' argument to the prior call to __cilkrts_take_stack().
 * 
 * Postconditions:
 *   - If f->sync_sp was null before the call, then f->sync_sp will be set to
 *     SP(sf). 
 *   - Otherwise, f->sync_sp will be restored to the value it had just prior
 *     to the last call to __cilkrts_take_stack(), except offset by any change
 *     in the stack pointer between the call to __cilkrts_take_stack() and
 *      this call to __cilkrts_put_stack().
 *
 * @param f The full frame that is being suspended.
 * @param sf The __cilkrts_stack_frame that is being suspended.  The stack
 *   pointer will be taken from the jmpbuf contained within this
 *   __cilkrts_stack_frame.
 */
COMMON_PORTABLE void __cilkrts_put_stack(full_frame *f,
                                         __cilkrts_stack_frame *sf);

/**
 * Records the stack pointer 'sp' as the stack pointer at the point of
 * resuming execution on full frame 'f'.  The value of 'sp' may be on a
 * different stack than the original value recorded for the stack pointer
 * using __cilkrts_put_stack().
 *
 * Precondition:
 *   - f->sync_sp must contain a value set by __cilkrts_put_stack().
 *
 * Postcondition:
 *   - f->sync_sp contains an *integer* value used to compute a change in the
 *     stack pointer upon the next call to __cilkrts_take_stack().
 *   - If 'sp' equals f->sync_sp, then f->sync_sp is set to null.
 *
 * @param f The full frame that is being resumed.
 * @param sp The stack pointer for the stack the function is being resumed on.
 */
COMMON_PORTABLE void __cilkrts_take_stack(full_frame *f, void *sp);

/**
 * Allocates and initailizes a full_frame.
 *
 * @param w The memory for the full_frame will be allocated out of the
 * worker's pool.
 * @param sf The __cilkrts_stack_frame which will be saved as the call_stack
 * for this full_frame.
 *
 * @return The newly allocated and initialized full_frame.
 */
COMMON_PORTABLE
full_frame *__cilkrts_make_full_frame(__cilkrts_worker *w,
                                      __cilkrts_stack_frame *sf);

/**
 * Deallocates a full_frame.
 *
 * @param w The memory for the full_frame will be returned to the worker's pool.
 * @param f The full_frame to be deallocated.
 */
COMMON_PORTABLE
void __cilkrts_destroy_full_frame(__cilkrts_worker *w, full_frame *f);

/**
 * Performs sanity checks to check the integrity of a full_frame.
 *
 * @param f The full_frame to be validated.
 */
COMMON_PORTABLE void validate_full_frame(full_frame *f);

/**
 * Locks the mutex contained in a full_frame.  The full_frame is validated
 * before the runtime attempts to lock it.
 *
 * Postcondition:
 *   - f->lock will be owned by w.
 *
 * @param w  The worker that will own the full_frame.  If the runtime is
 * collecting stats, the intervals will be attributed to the worker.
 * @param f The full_frame containing the mutex to be locked.
 */
COMMON_PORTABLE void __cilkrts_frame_lock(__cilkrts_worker *w,
                                          full_frame *f);

/**
 * Unlocks the mutex contained in a full_frame.
 *
 * Precondition:
 *   - f->lock must must be owned by w.
 *
 * @param w  The worker that currently owns the full_frame.
 * @param f The full_frame containing the mutex to be unlocked.
 */
COMMON_PORTABLE void __cilkrts_frame_unlock(__cilkrts_worker *w,
                                            full_frame *f);
/** @} */

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_FULL_FRAME_DOT_H)
