/* cilk-abi-cilk-for.cpp                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2011 
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

/* Implementation of cilk_for ABI.
 *
 * This file must be C++, not C, in order to handle C++ exceptions correctly
 * from within the body of the cilk_for loop
 */

#include "internal/abi.h"
#include "metacall_impl.h"
#include "global_state.h"

// Icky macros to determine if we're compiled with optimization.  Based on
// the declaration of __CILKRTS_ASSERT in common.h
#if defined(_WIN32)
# if defined (_DEBUG)
#   define CILKRTS_OPTIMIZED 0    // Assumes /MDd is always used with /Od
# else
#   define CILKRTS_OPTIMIZED 1
# endif // defined(_DEBUG)
#else
# if defined(__OPTIMIZE__)
#   define CILKRTS_OPTIMIZED 1
# else
#   define CILKRTS_OPTIMIZED 0
# endif
#endif

template <typename count_t>
static inline int grainsize(long req, count_t iter)
{
    
    if (req > 0)
    {
        // This could be if req > INT_MAX return INT_MAX but limits.h is
        // broken on some Linux's.  A limit this high risks losing
        // parallelism, but the user told us what they want for grainsize.
        // Who are we to argue?
        if (req > 0x7fffffff)
            return 0x7fffffff;
        return (int)req;
    }

    global_state_t* g = cilkg_get_global_state();
    if (g->under_ptool)
    {
        // Grainsize = 1, when running under PIN, and when the grainsize has not
        // explicitly been set by the user.
        return 1;
    }
    else
    {
        count_t n = iter / (8 * g->P) + 1;
        // 2K should be enough to amortize the cost of the cilk_for and any
        // larger grainsize risks losing parallelism
        if (n > 2048)
            return 2048;
        return (int)n;
    }
}

/*
 * call_cilk_for_loop_body
 *
 * Centralizes the code to call the loop body.  The compiler should be
 * inlining this code
 *
 * low   - Low loop index we're considering in this portion of the algorithm
 * high  - High loop index we're considering in this portion of the algorithm
 * body  - lambda function for the cilk_for loop body
 * data  - data used by the lambda function
 * w     - __cilkrts_worker we're currently executing on
 * loop_root_pedigree - __cilkrts_pedigree node we generated for the root of
 *         the cilk_for loop to flatten out the internal nodes
 */
template <typename count_t, typename F>
inline static
void call_cilk_for_loop_body(count_t low, count_t high,
                             F body, void *data,
                             struct __cilkrts_worker *w,
                             struct __cilkrts_pedigree *loop_root_pedigree)
{
    // The worker is only valid until the first spawn.  Fetch the
    // __cilkrts_stack_frame out of the worker, since it will be stable across
    // steals
    volatile struct __cilkrts_stack_frame *sf = w->current_stack_frame;

    // Save the pedigree node pointer to by the worker.  We'll need to restore
    // that when we exit since the spawn helpers in the cilk_for call tree
    // will assume that it's valid
    struct __cilkrts_pedigree *saved_next_pedigree_node = w->pedigree.next;

    // Add the leaf pedigree node to the chain. The parent is the root node
    // to flatten the tree regardless of the DAG branches in the cilk_for
    // divide-and-conquer recursion.
    //
    // The rank is initailized to the low index - 1, because the user is
    // expected to call __cilkrts_bump_loop_rank at the start of the cilk_for
    // loop body.  This is because it's less error-prone to do it once at the
    // top of the loop body rather than multiple places if the loop includes
    // branches
    struct __cilkrts_pedigree loop_leaf_pedigree;

    loop_leaf_pedigree.rank = (uint64_t)low - 1;
    loop_leaf_pedigree.next = loop_root_pedigree;

    // The worker's pedigree always starts with a rank of 0
    w->pedigree.rank = 0;
    w->pedigree.next = &loop_leaf_pedigree;

    // Call the compiler generated cilk_for loop body lambda function
    body(data, low, high);

    // Restore the pedigree chain. It must be valid because the spawn helpers
    // generated by the cilk_for implementation will access it.
    //
    // Remember that the loop body may have included spawns, so we must
    // refetch the worker from the __cilkrts_stack_frame which is stable
    // regardless of which worker we're executing on.
    sf->worker->pedigree.next = saved_next_pedigree_node;
}

/*
 * cilk_for_recursive
 *
 * Templatized function to implement the recursive divide-and-conquer
 * algorithm that's how we implement a cilk_for.
 *
 * low   - Low loop index we're considering in this portion of the algorithm
 * high  - High loop index we're considering in this portion of the algorithm
 * body  - lambda function for the cilk_for loop body
 * data  - data used by the lambda function
 * grain - grain size (0 if it should be computed)
 * w     - __cilkrts_worker we're currently executing on
 * loop_root_pedigree - __cilkrts_pedigree node we generated for the root of
 *         the cilk_for loop to flatten out the internal nodes
 */
template <typename count_t, typename F>
static
void cilk_for_recursive(count_t low, count_t high,
                        F body, void *data, int grain,
                        struct __cilkrts_worker *w,
                        struct __cilkrts_pedigree *loop_root_pedigree)
{
    // The worker is only valid until the first spawn.  Fetch the
    // __cilkrts_stack_frame out of the worker, since it will be stable across
    // steals
    volatile struct __cilkrts_stack_frame *sf = w->current_stack_frame;

tail_recurse:
    count_t count = high - low;
    // Invariant: count > 0, grain >= 1
    if (count > grain)
    {
        // Invariant: count >= 2
        count_t mid = low + count / 2;
        _Cilk_spawn cilk_for_recursive(low, mid, body, data, grain,
                                       sf->worker, loop_root_pedigree);
        low = mid;
        goto tail_recurse;
    }

    // Call the cilk_for loop body lambda function passed in by the compiler to
    // execute one grain
    call_cilk_for_loop_body(low, high, body, data,
                            sf->worker, loop_root_pedigree);

}

/*
 * cilk_for_root
 *
 * Templatized function to implement the top level of a cilk_for loop.
 *
 * body  - lambda function for the cilk_for loop body
 * data  - data used by the lambda function
 * count - trip count for loop
 * grain - grain size (0 if it should be computed)
 */
template <typename count_t, typename F>
static void cilk_for_root(F body, void *data, count_t count, int grain)
{
    struct __cilkrts_pedigree loop_root_pedigree;
    struct __cilkrts_worker *w;
    volatile struct __cilkrts_stack_frame *sf;

    // Fetch the current worker.  From that we can get the current stack frame
    // which will be constant even if we're stolen
    w = __cilkrts_get_tls_worker();
    sf = w->current_stack_frame;

    // Save the current worker pedigree into the loop root
    loop_root_pedigree = w->pedigree;

    // Don't splice the loop_root node in yet.  It will be done when we
    // call the loop body lambda function
//    w->pedigree.rank = 0;
//    w->pedigree.next = &loop_root_pedigree;

    /* Spawn is necessary at top-level to force runtime to start up.
     * Runtime must be started in order to call the grainsize() function.
     */
    int gs = grainsize(grain, count);
    if (count > gs)
    {
        count_t mid = count / 2;
        _Cilk_spawn cilk_for_recursive((count_t) 0, mid, body, data, gs, w,
                                       &loop_root_pedigree);
        cilk_for_recursive(mid, count, body, data, gs, sf->worker,
                                       &loop_root_pedigree);

        // We must sync before touching the worker below
        _Cilk_sync;

        // Need to refetch the worker.  The loop body (or something it calls) may
        // have included spawns, or the continuation after the cilk_spawn above may
        // have been stolen, so we can't assume we're on the same worker
        w = sf->worker;

        // Restore the pedigree in the worker.  The rank will be bumped
        // by the return
        w->pedigree = loop_root_pedigree;
    }
    else
    {
        // Call the cilk_for loop body lambda function to execute over the entire
        // range
        call_cilk_for_loop_body((count_t)0, count, body, data, w,
                                &loop_root_pedigree);
    }

    // If this is an optimized build, then the compiler will have optimized out the
    // increment of the worker's pedigree in the implied sync.  We need to add one
    // to make the pedigree_loop test work correctly
    if (CILKRTS_OPTIMIZED)
    {
        sf->worker->pedigree.rank++;
    }
}

// Use extern "C" to suppress name mangling of __cilkrts_cilk_for_32 and
// __cilkrts_cilk_for_64.
extern "C" {

/*
 * __cilkrts_cilk_for_32
 *
 * Implementation of cilk_for for 32-bit trip counts (regardless of processor
 * word size).  Assumes that the range is 0 - count.
 *
 * body  - lambda function for the cilk_for loop body
 * data  - data used by the lambda function
 * count - trip count for loop
 * grain - grain size (0 if it should be computed)
 */

CILK_ABI_THROWS_VOID __cilkrts_cilk_for_32(__cilk_abi_f32_t body, void *data,
                                            cilk32_t count, int grain)
{
    // Check for an empty range here as an optimization - don't need to do any
    // __cilkrts_stack_fram initialization
    if (count > 0)
        cilk_for_root(body, data, count, grain);
}

/*
 * __cilkrts_cilk_for_64
 *
 * Implementation of cilk_for for 64-bit trip counts (regardless of processor
 * word size).  Assumes that the range is 0 - count.
 *
 * body  - lambda function for the cilk_for loop body
 * data  - data used by the lambda function
 * count - trip count for loop
 * grain - grain size (0 if it should be computed)
 */
CILK_ABI_THROWS_VOID __cilkrts_cilk_for_64(__cilk_abi_f64_t body, void *data,
                                            cilk64_t count, int grain)
{
    // Check for an empty range here as an optimization - don't need to do any
    // __cilkrts_stack_fram initialization
    if (count > 0)
        cilk_for_root(body, data, count, grain);
}

} // end extern "C"

/* End cilk-abi-cilk-for.cpp */
