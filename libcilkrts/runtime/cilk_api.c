/* cilk_api.c                  -*-C-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2009-2012 
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

/*
 * Implementation of functions declared in cilk_api.h
 */

/*
 * Define the COMPILING_CILK_ABI_FUNCTIONS macro, so that
 * compilation of this file generates non-inlined definitions for the
 * functions marked as CILK_EXPORT_AND_INLINE in cilk_api.h.
 *
 * We must deal with these functions differently because we need to
 * continue to ship nonlined versions of these functions.
 *
 *   CILK_EXPORT_AND_INLINE int __cilkrts_get_worker_rank(uint64_t *rank);
 *   CILK_EXPORT_AND_INLINE int __cilkrts_bump_worker_rank();
 *   CILK_EXPORT_AND_INLINE int __cilkrts_bump_loop_rank();
 */
#define COMPILING_CILK_API_FUNCTIONS

#include <internal/abi.h>
#include <cilk/cilk_api.h>

#include "os.h"
#include "os_mutex.h"
#include "bug.h"
#include "global_state.h"
#include "local_state.h"
#include "scheduler.h"
#include "sysdep.h"

CILK_API_VOID __cilkrts_init(void)
{
    // Initialize, but don't start, the cilk runtime.
    __cilkrts_init_internal(0);
}

CILK_API_VOID __cilkrts_end_cilk(void)
{
    // Take out the global OS mutex while we do this to protect against
    // another thread attempting to bind while we do this
    global_os_mutex_lock();

    if (cilkg_is_published()) {
        global_state_t *g = cilkg_get_global_state();
        if (g->Q || __cilkrts_get_tls_worker())
            __cilkrts_bug("Attempt to shut down Cilk while Cilk is still "
                          "running");
        __cilkrts_stop_workers(g);
        __cilkrts_deinit_internal(g);
    }

    global_os_mutex_unlock();
}

CILK_API_INT
__cilkrts_get_nworkers()
{
    return cilkg_get_nworkers();
}

CILK_API_INT
__cilkrts_get_total_workers()
{
    return cilkg_get_total_workers();
}

CILK_API_INT __cilkrts_get_force_reduce(void)
{
    return cilkg_get_force_reduce();
}

CILK_API_INT __cilkrts_set_param(const char* param, const char* value)
{
    return cilkg_set_param(param, value);
}

#ifdef _WIN32
CILK_API_INT __cilkrts_set_param_w(const wchar_t* param, const wchar_t* value)
{
    return cilkg_set_param_w(param, value);
}
#endif // _WIN32

/* Return a small integer indicating which Cilk worker the function is
 * currently running on.  Each thread started by the Cilk runtime library
 * (system worker) has a unique worker number in the range 1..P-1, where P is
 * the valued returned by __cilkrts_get_nworkers().  All threads started by
 * the user or by other libraries (user workers) share the worker number 0.
 * Therefore, the worker number is not unique across multiple user threads.
 *
 * Implementor's note: The value returned from this function is different from
 * the value, w->self, used in most debug messages.
 */
CILK_API_INT
__cilkrts_get_worker_number(void)
{
    __cilkrts_worker *w = __cilkrts_get_tls_worker();

    if (0 == w)
        /* A non-worker always has a worker number of zero. */
        return 0;
    else if (WORKER_USER == w->l->type)
        /* User worker was once a non-worker, so its number should still be
         * zero. */
        return 0;
    else
        /* w->self for a system worker is in range 0..(P-1); adjust to 1..P
         * to avoid conflicting with the user thread's worker number. */
        return w->self + 1;
}

/**
 * Internal definition of the pedigree context.  The size of the
 * structure must match __cilkrts_pedigree_context_t defined in abi.i
 */
typedef struct pedigree_context_t
{
    /** Size of the structure, in bytes */
    size_t size;

    /** Next __cilkrts_pedigree to return */
    const __cilkrts_pedigree *pedigree;

    /** Unused.  Left over from previous implementation */
    void *unused1;

    /** Unused.  Left over from previous implementation */
    void *unused2;

    // // Debugging aid for pedigree-test:
    // __cilkrts_stack_frame *expected_sf;
} pedigree_context_t;

/*
 * __cilkrts_get_pedigree_info
 *
 * Fetch the birthrank for a stack frame.  To initialize the walk, both sf_in
 * and frame_in should be NULL.  parent_sf_ptr and parent_frame_ptr provide
 * context for the stackwalk and should be returned as sf_in and frame_in on
 * the next call.
 *
 * Returns:
 *   0 - Success - birthrank, parent_sf_out and parent_frame_out are valid
 *   >1 - Pedigree walk completed
 *   <1 - Failure - -1: No worker bound to thread, -2: Sanity check failed
 */

#define PEDIGREE_WALK_COMPLETE (__cilkrts_pedigree *)-1

CILK_API_INT
__cilkrts_get_pedigree_info(__cilkrts_pedigree_context_t *external_context,
                            uint64_t *sf_birthrank)
{
    pedigree_context_t *context = (pedigree_context_t *)external_context;

    CILK_ASSERT(sizeof(__cilkrts_pedigree_context_t) ==
                sizeof(pedigree_context_t));
    if (context->size != sizeof(pedigree_context_t))
        return -3;  // Invalid size

    // If the pointer to the last __cilkrts_pedigree is -1, we've
    // finished the walk.  We're still done.
    if (PEDIGREE_WALK_COMPLETE == context->pedigree)
        return 1;

    // The passed in context value contains a pointer to the last
    // __cilkrts_pedigree returned, or NULL if we're starting a
    // new walk
    if (NULL == context->pedigree)
    {
        __cilkrts_worker *w = __cilkrts_get_tls_worker();
	__cilkrts_pedigree* pedigree_node;
        if (NULL != w) {
	    pedigree_node = &w->pedigree;
	}
	else {
	    pedigree_node = __cilkrts_get_tls_pedigree_leaf(1);
	}
	context->pedigree = pedigree_node->parent;
    }
    else
        context->pedigree = context->pedigree->parent;

    // Note: If we want to omit the user root node,
    // stop at context->pedigree->parent instead.
    if (NULL == context->pedigree)
    {
	context->pedigree = PEDIGREE_WALK_COMPLETE;
        return 1;
    }

    *sf_birthrank = context->pedigree->rank;
    return 0;
}

CILK_API_PEDIGREE
__cilkrts_get_pedigree_internal(__cilkrts_worker *w)
{
    if (NULL != w) {
	return w->pedigree;
    }
    else {
	const __cilkrts_pedigree *pedigree =
            __cilkrts_get_tls_pedigree_leaf(1);
	return *pedigree;
    }
}


CILK_API_INT __cilkrts_bump_worker_rank_internal(__cilkrts_worker *w)
{
    __cilkrts_pedigree *pedigree;
    pedigree = (w ? &w->pedigree : __cilkrts_get_tls_pedigree_leaf(1));
    pedigree->rank++;
    return 0;
}

/* End cilk_api.c */
