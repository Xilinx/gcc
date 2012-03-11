/* full_frame.c                  -*-C++-*-
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

#include "full_frame.h"
#include "stats.h"
#include "os.h"
#include "bug.h"
#include "jmpbuf.h"
#include "frame_malloc.h"

COMMON_PORTABLE
full_frame *__cilkrts_make_full_frame(__cilkrts_worker *w,
                                      __cilkrts_stack_frame *sf)
{
    full_frame *f;

    START_INTERVAL(w, INTERVAL_ALLOC_FULL_FRAME) {
        f = (full_frame *)__cilkrts_frame_malloc(w, sizeof(*f));
        __cilkrts_mutex_init(&f->lock);

        f->full_frame_magic_0 = FULL_FRAME_MAGIC_0;
        f->join_counter = 0;
        f->parent = 0;
        f->rightmost_child = 0;
        f->left_sibling = f->right_sibling = 0;
        f->call_stack = sf;
        f->stable_call_stack = sf;
        f->is_call_child = 0;
        f->simulated_stolen = 0;
        f->reducer_map = f->children_reducer_map = f->right_reducer_map = 0;
        f->pending_exception = 
            f->child_pending_exception = 
            f->right_pending_exception = NULL;

        f->sync_sp = 0;
#ifdef _WIN32
        f->exception_sp = 0;
        f->trylevel = (unsigned long)-1;
        f->registration = 0;
#endif
//        f->exception_sp_offset = 0;
//        f->eh_kind = EH_NONE;
        f->stack_self = 0;
        f->stack_child = 0;

        f->sync_master = 0;

        /*__cilkrts_init_full_frame_sysdep(w, f);*/
        f->full_frame_magic_1 = FULL_FRAME_MAGIC_1;
    } STOP_INTERVAL(w, INTERVAL_ALLOC_FULL_FRAME);
    return f;
}

COMMON_PORTABLE void __cilkrts_put_stack(full_frame *f,
                                         __cilkrts_stack_frame *sf)
{
    /* When suspending frame f prior to stealing it, __cilkrts_put_stack is
     * used to store the stack pointer for eventual sync.  When suspending
     * frame f prior to a sync, __cilkrts_put_stack is called to re-establish
     * the sync stack pointer, offsetting it by any change in the stack depth
     * that occured between the spawn and the sync.
     * Although it is not usually meaningful to add two pointers, the value of
     * f->sync_sp at the time of this call is really an integer, not a
     * pointer.
     */
    ptrdiff_t sync_sp_i = (ptrdiff_t) f->sync_sp;
    char* sp = (char*) __cilkrts_get_sp(sf);

    f->sync_sp = sp + sync_sp_i;

    DBGPRINTF("%d-                __cilkrts_put_stack - adjust (+) sync "
              "stack of full frame %p (+sp: %p) to %p\n",
              __cilkrts_get_tls_worker()->self, f, sp, f->sync_sp);
}

COMMON_PORTABLE void __cilkrts_take_stack(full_frame *f, void *sp)
{
    /* When resuming the parent after a steal, __cilkrts_take_stack is used to
     * subtract the new stack pointer from the current stack pointer, storing
     * the offset in f->sync_sp.  When resuming after a sync,
     * __cilkrts_take_stack is used to subtract the new stack pointer from
     * itself, leaving f->sync_sp at zero (null).  Although the pointers being
     * subtracted are not part of the same contiguous chunk of memory, the
     * flat memory model allows us to subtract them and get a useable offset.
     */
    ptrdiff_t sync_sp_i = f->sync_sp - (char*) sp;

    f->sync_sp = (char *) sync_sp_i;

    DBGPRINTF("%d-                __cilkrts_take_stack - adjust (-) sync "
              "stack of full frame %p to %p (-sp: %p)\n",
              __cilkrts_get_tls_worker()->self, f, f->sync_sp, sp);
}

COMMON_PORTABLE
void __cilkrts_destroy_full_frame(__cilkrts_worker *w, full_frame *f)
{
    validate_full_frame(f);
    CILK_ASSERT(f->reducer_map == 0);
    CILK_ASSERT(f->children_reducer_map == 0);
    CILK_ASSERT(f->right_reducer_map == 0);
    CILK_ASSERT(NULL == f->pending_exception);
    CILK_ASSERT(NULL == f->child_pending_exception);
    CILK_ASSERT(NULL == f->right_pending_exception);
    __cilkrts_mutex_destroy(w, &f->lock);
    __cilkrts_frame_free(w, f, sizeof(*f));
}

COMMON_PORTABLE void validate_full_frame(full_frame *f)
{
    /* check the magic numbers, for debugging purposes */
    if (f->full_frame_magic_0 != FULL_FRAME_MAGIC_0 ||
        f->full_frame_magic_1 != FULL_FRAME_MAGIC_1)
        abort_because_rts_is_corrupted();
}

void __cilkrts_frame_lock(__cilkrts_worker *w, full_frame *f)
{
    validate_full_frame(f);
    __cilkrts_mutex_lock(w, &f->lock);
}

void __cilkrts_frame_unlock(__cilkrts_worker *w, full_frame *f)
{
    __cilkrts_mutex_unlock(w, &f->lock);
}

/* End full_frame.c */
