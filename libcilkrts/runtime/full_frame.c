/* full_frame.c                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2010-2011 
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
    full_frame *ff;

    START_INTERVAL(w, INTERVAL_ALLOC_FULL_FRAME) {
        ff = (full_frame *)__cilkrts_frame_malloc(w, sizeof(*ff));
        __cilkrts_mutex_init(&ff->lock);

        ff->full_frame_magic_0 = FULL_FRAME_MAGIC_0;
        ff->join_counter = 0;
        ff->parent = 0;
        ff->rightmost_child = 0;
        ff->left_sibling = ff->right_sibling = 0;
        ff->call_stack = sf;
        ff->is_call_child = 0;
        ff->simulated_stolen = 0;
	ff->children_reducer_map = ff->right_reducer_map = 0;
        ff->pending_exception = 
            ff->child_pending_exception = 
            ff->right_pending_exception = NULL;

        ff->sync_sp = 0;
#ifdef _WIN32
        ff->exception_sp = 0;
        ff->trylevel = (unsigned long)-1;
        ff->registration = 0;
#endif
	ff->frame_size = 0;
//        ff->exception_sp_offset = 0;
//        ff->eh_kind = EH_NONE;
        ff->stack_self = 0;
        ff->stack_child = 0;

        ff->sync_master = 0;

        /*__cilkrts_init_full_frame_sysdep(w, ff);*/
        ff->full_frame_magic_1 = FULL_FRAME_MAGIC_1;
    } STOP_INTERVAL(w, INTERVAL_ALLOC_FULL_FRAME);
    return ff;
}

COMMON_PORTABLE void __cilkrts_put_stack(full_frame *ff,
                                         __cilkrts_stack_frame *sf)
{
    /* When suspending frame ff prior to stealing it, __cilkrts_put_stack is
     * used to store the stack pointer for eventual sync.  When suspending
     * frame ff prior to a sync, __cilkrts_put_stack is called to re-establish
     * the sync stack pointer, offsetting it by any change in the stack depth
     * that occured between the spawn and the sync.
     * Although it is not usually meaningful to add two pointers, the value of
     * ff->sync_sp at the time of this call is really an integer, not a
     * pointer.
     */
    ptrdiff_t sync_sp_i = (ptrdiff_t) ff->sync_sp;
    char* sp = (char*) __cilkrts_get_sp(sf);

    ff->sync_sp = sp + sync_sp_i;

    DBGPRINTF("%d-                __cilkrts_put_stack - adjust (+) sync "
              "stack of full frame %p (+sp: %p) to %p\n",
              __cilkrts_get_tls_worker()->self, ff, sp, ff->sync_sp);
}

COMMON_PORTABLE void __cilkrts_take_stack(full_frame *ff, void *sp)
{
    /* When resuming the parent after a steal, __cilkrts_take_stack is used to
     * subtract the new stack pointer from the current stack pointer, storing
     * the offset in ff->sync_sp.  When resuming after a sync,
     * __cilkrts_take_stack is used to subtract the new stack pointer from
     * itself, leaving ff->sync_sp at zero (null).  Although the pointers being
     * subtracted are not part of the same contiguous chunk of memory, the
     * flat memory model allows us to subtract them and get a useable offset.
     */
    ptrdiff_t sync_sp_i = ff->sync_sp - (char*) sp;

    ff->sync_sp = (char *) sync_sp_i;

    DBGPRINTF("%d-                __cilkrts_take_stack - adjust (-) sync "
              "stack of full frame %p to %p (-sp: %p)\n",
              __cilkrts_get_tls_worker()->self, ff, ff->sync_sp, sp);
}

COMMON_PORTABLE
void __cilkrts_destroy_full_frame(__cilkrts_worker *w, full_frame *ff)
{
    validate_full_frame(ff);
    CILK_ASSERT(ff->children_reducer_map == 0);
    CILK_ASSERT(ff->right_reducer_map == 0);
    CILK_ASSERT(NULL == ff->pending_exception);
    CILK_ASSERT(NULL == ff->child_pending_exception);
    CILK_ASSERT(NULL == ff->right_pending_exception);
    __cilkrts_mutex_destroy(w, &ff->lock);
    __cilkrts_frame_free(w, ff, sizeof(*ff));
}

COMMON_PORTABLE void validate_full_frame(full_frame *ff)
{
    /* check the magic numbers, for debugging purposes */
    if (ff->full_frame_magic_0 != FULL_FRAME_MAGIC_0 ||
        ff->full_frame_magic_1 != FULL_FRAME_MAGIC_1)
        abort_because_rts_is_corrupted();
}

void __cilkrts_frame_lock(__cilkrts_worker *w, full_frame *ff)
{
    validate_full_frame(ff);
    __cilkrts_mutex_lock(w, &ff->lock);
}

void __cilkrts_frame_unlock(__cilkrts_worker *w, full_frame *ff)
{
    __cilkrts_mutex_unlock(w, &ff->lock);
}

/* End full_frame.c */
