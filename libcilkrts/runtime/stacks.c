/* stacks.c                  -*-C-*-
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

#include "stacks.h"
#include "sysdep.h"
#include "local_state.h"
#include "frame_malloc.h"
#include "cilk-tbb-interop.h"

static void move_to_global(__cilkrts_worker *w, unsigned int until)
{
    __cilkrts_stack_cache *local = &w->l->stack_cache;
    __cilkrts_stack_cache *global = &w->g->stack_cache;

    /* If the global cache appears to be full do not take out the lock. */
    if (global->n >= global->size)
        return;

    __cilkrts_mutex_lock(w, &global->lock);
    while (global->n < global->size && local->n > until) {
        global->stacks[global->n++] = local->stacks[--local->n];
    }
    __cilkrts_mutex_unlock(w, &global->lock);
}

static void push(__cilkrts_worker *w, __cilkrts_stack *sd)
{
    __cilkrts_stack_cache *local = &w->l->stack_cache;
    const unsigned int local_size = local->size;

    if (local->n < local_size) {
        local->stacks[local->n++] = sd;
        return;
    }

    if (local_size == 0) {
        __cilkrts_free_stack(w->g, sd);
        return;
    }

    /* push half (round up) of the free stacks */
    move_to_global(w, local_size / 2);

    while (local->n > local_size / 2)
        __cilkrts_free_stack(w->g, local->stacks[--local->n]);

    local->stacks[local->n++] = sd;
    return;
}

static __cilkrts_stack *pop(__cilkrts_worker *w)
{
    __cilkrts_stack_cache *local = &w->l->stack_cache;
    __cilkrts_stack_cache *global = &w->g->stack_cache;
    __cilkrts_stack *sd = 0;
    if (local->n > 0)
        return local->stacks[--local->n];
    if (global->n > 0) {
        __cilkrts_mutex_lock(w, &global->lock);
        if (global->n > 0)
            sd = global->stacks[--global->n];
        __cilkrts_mutex_unlock(w, &global->lock);
    }
    return sd;
}

#ifdef _WIN32
#   include "stacks-win.h"
#   define okay_to_release(stack) (0 == (stack)->outstanding_references)
#else
#   define okay_to_release(stack) (1)
#endif  // _WIN32

void __cilkrts_release_stack(__cilkrts_worker *w,
                             __cilkrts_stack *sd)
{
    START_INTERVAL(w, INTERVAL_FREE_STACK);
    if (sd && okay_to_release(sd)) {
        __cilkrts_invoke_stack_op(w, CILK_TBB_STACK_RELEASE,sd);
        push(w, sd);
    }
    STOP_INTERVAL(w, INTERVAL_FREE_STACK);
    return;
}

__cilkrts_stack *__cilkrts_get_stack(__cilkrts_worker *w)
{
    __cilkrts_stack *sd;

    START_INTERVAL(w, INTERVAL_ALLOC_STACK);
    sd = pop (w);
    if (sd == NULL)
        sd = __cilkrts_make_stack(w);
    else
        __cilkrts_sysdep_reset_stack(sd);
    STOP_INTERVAL(w, INTERVAL_ALLOC_STACK);
    return sd;
}

static void flush(global_state_t *g,
                  __cilkrts_stack_cache *c)
{
    /*START_INTERVAL(w, INTERVAL_FREE_STACK);*/
    while (c->n > 0)
        __cilkrts_free_stack(g, c->stacks[--c->n]);
    /*STOP_INTERVAL(w, INTERVAL_FREE_STACK);*/
}

void __cilkrts_init_stack_cache(__cilkrts_worker *w,
                                __cilkrts_stack_cache *c,
                                unsigned int size)
{
    __cilkrts_mutex_init(&c->lock);
    c->size = size;
    c->n = 0;
    c->stacks = __cilkrts_frame_malloc(w, size * sizeof(__cilkrts_stack *));
#if 0 /* Causes problems on Linux due to generated call to intel_fast_memset */
    {
      unsigned int i;
      /* Not really needed -- only indices < n are valid */
      for (i = 0; i < size; i++)
        c->stacks[i] = 0;
    }
#else
    if (size > 0)
      c->stacks[0] = 0;
#endif
}

void __cilkrts_destroy_stack_cache(__cilkrts_worker *w,
                                   global_state_t *g,
                                   __cilkrts_stack_cache *c)
{
    flush(g, c);
    __cilkrts_frame_free(w, c->stacks, c->size * sizeof(__cilkrts_stack *));
    c->stacks = 0;
    c->n = 0;
    c->size = 0;
    __cilkrts_mutex_destroy(w, &c->lock);
}

/* Free all but one local stack, returning to the global pool if possible. */

void __cilkrts_trim_stack_cache(__cilkrts_worker *w)
{
    __cilkrts_stack_cache *local = &w->l->stack_cache;

    if (local->n <= 1)
        return;

    START_INTERVAL(w, INTERVAL_FREE_STACK);

    move_to_global(w, 1);

    while (local->n > 1)
        __cilkrts_free_stack(w->g, local->stacks[--local->n]);

    STOP_INTERVAL(w, INTERVAL_FREE_STACK);
}

/* End stacks.c */
