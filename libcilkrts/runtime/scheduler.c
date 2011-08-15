/* scheduler.c                  -*-C-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2007-2011 
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

/*
 * Cilk scheduler
 */

#include "scheduler.h"
#include "bug.h"
#include "os.h"
#include "os_mutex.h"
#include "local_state.h"
#include "signal_node.h"
#include "full_frame.h"
#include "stacks.h"
#include "sysdep.h"
#include "except.h"
#include "cilk_malloc.h"

#include <string.h> /* memcpy */
#include <stdio.h>  // sprintf
#include <stdlib.h> // malloc, free, abort

#ifdef _WIN32
#   pragma warning(disable:1786)   // disable warning: sprintf is deprecated
#   include "sysdep-win.h"
#endif  // _WIN32


// ICL: Don't complain about conversion from pointer to same-sized integral
// type in __cilkrts_put_stack.  That's why we're using ptrdiff_t
#ifdef _WIN32
#   pragma warning(disable: 1684)
#endif

#include "cilk/cilk_api.h"
#include "frame_malloc.h"
#include "metacall_impl.h"
#include "reducer_impl.h"
#include "cilk-tbb-interop.h"
#include "cilk-ittnotify.h"
#include "stats.h"
// ICL: Don't complain about loss of precision in myrand
// I tried restoring the warning after the function, but it didn't
// suppress it
#ifdef _WIN32
#   pragma warning(disable: 2259)
#endif

#ifndef _WIN32
#   include <unistd.h>
#endif

// Options for the scheduler.
enum schedule_t { SCHEDULE_RUN,
                  SCHEDULE_WAIT,
                  SCHEDULE_EXIT };

static enum schedule_t worker_runnable(__cilkrts_worker *w);

// Scheduling-stack functions:
static void do_return_from_spawn (__cilkrts_worker *w,
                                  full_frame *f,
                                  __cilkrts_stack_frame *sf);
static void do_migrate_exception (__cilkrts_worker *w,
                                  full_frame *f,
                                  __cilkrts_stack_frame *sf);
static void do_sync (__cilkrts_worker *w,
                     full_frame *f,
                     __cilkrts_stack_frame *sf);

#ifndef _WIN32
    // TBD: definition of max() for Linux.
#   define max(a, b) ((a) < (b) ? (b) : (a))
#endif

static void __cilkrts_dump_stats_to_stderr(global_state_t *g)
{
#ifdef CILK_PROFILE
    int i;
    for (i = 0; i < g->nworkers; ++i)
        __cilkrts_accum_stats(&g->stats, &g->workers[i]->l->stats);
    dump_stats_to_file(stderr, &g->stats);
#endif
    fprintf(stderr,
            "\nCILK++ RUNTIME MEMORY USAGE: %lld bytes",
            (long long)g->frame_malloc.allocated_from_os);
    if (g->stats.stack_hwm)
        fprintf(stderr, ", %ld stacks\n", g->stats.stack_hwm);
    else
        fputc('\n', stderr);
}

static void validate_worker(__cilkrts_worker *w)
{
    /* check the magic numbers, for debugging purposes */
    if (w->l->worker_magic_0 != WORKER_MAGIC_0 ||
        w->l->worker_magic_1 != WORKER_MAGIC_1)
        abort_because_rts_is_corrupted();
}

static void double_link(full_frame *left, full_frame *right)
{
    if (left)
        left->right_sibling = right;
    if (right)
        right->left_sibling = left;
}

/* add CHILD to the right of all children of PARENT */
static void push_child(full_frame *parent, full_frame *child)
{
    double_link(parent->rightmost_child, child);
    double_link(child, 0);
    parent->rightmost_child = child;
}

/* unlink CHILD from the list of all children of PARENT */
static void unlink_child(full_frame *parent, full_frame *child)
{
    double_link(child->left_sibling, child->right_sibling);

    if (!child->right_sibling) {
        /* this is the rightmost child -- update parent link */
        CILK_ASSERT(parent->rightmost_child == child);
        parent->rightmost_child = child->left_sibling;
    }
    child->left_sibling = child->right_sibling = 0; /* paranoia */
}

static void incjoin(full_frame *f)
{
    ++f->join_counter;
}

static int decjoin(full_frame *f)
{
    CILK_ASSERT(f->join_counter > 0);
    return (--f->join_counter);
}

/*
 * Pseudo-random generator defined by the congruence S' = 69070 * S
 * mod (2^32 - 5).  Marsaglia (CACM July 1993) says on page 107 that
 * this is a ``good one''.  There you go.
 *
 * The literature makes a big fuss about avoiding the division, but
 * for us it is not worth the hassle.
 */
static const unsigned RNGMOD = ((1ULL << 32) - 5);
static const unsigned RNGMUL = 69070U;

static unsigned myrand(__cilkrts_worker *w)
{
    unsigned state = w->l->rand_seed;
    state = (unsigned)((RNGMUL * (unsigned long long)state) % RNGMOD);
    w->l->rand_seed = state;
    return state;
}

static void mysrand(__cilkrts_worker *w, unsigned seed)
{
    seed %= RNGMOD;
    seed += (seed == 0); /* 0 does not belong to the multiplicative
                            group.  Use 1 instead */
    w->l->rand_seed = seed;
}

/* W grabs its own lock */
void __cilkrts_worker_lock(__cilkrts_worker *w)
{
    validate_worker(w);
    CILK_ASSERT(w->l->do_not_steal == 0);

    /* tell thieves to stay out of the way */
    w->l->do_not_steal = 1;
    __cilkrts_fence(); /* probably redundant */

    __cilkrts_mutex_lock(w, &w->l->lock);
}

void __cilkrts_worker_unlock(__cilkrts_worker *w)
{
    __cilkrts_mutex_unlock(w, &w->l->lock);
    CILK_ASSERT(w->l->do_not_steal == 1);
    /* The fence is probably redundant.  Use a release
       operation when supported (gcc and compatibile);
       that is faster on x86 which serializes normal stores. */
#if defined __GNUC__ && (__GNUC__ * 10 + __GNUC_MINOR__ > 43 || __ICC >= 1110)
    __sync_lock_release(&w->l->do_not_steal);
#else
    w->l->do_not_steal = 0;
    __cilkrts_fence(); /* store-store barrier, redundant on x86 */
#endif
}

/* try to acquire the lock of some *other* worker */
static int worker_trylock_other(__cilkrts_worker *w,
                                __cilkrts_worker *other)
{
    int success = 0;

    validate_worker(other);

    /* This protocol guarantees that, after setting the DO_NOT_STEAL
       flag, worker W can enter its critical section after waiting for
       the thief currently in the critical section (if any) and at
       most one other thief.  

       This requirement is overly paranoid, but it should protect us
       against future nonsense from OS implementors.
    */

    /* compete for the right to disturb OTHER */
    if (__cilkrts_mutex_trylock(w, &other->l->steal_lock)) {
        if (other->l->do_not_steal) {
            /* leave it alone */
        } else {
            success = __cilkrts_mutex_trylock(w, &other->l->lock);
        }
        __cilkrts_mutex_unlock(w, &other->l->steal_lock);
    }


    return success;
}

static void worker_unlock_other(__cilkrts_worker *w,
                                __cilkrts_worker *other)
{
    __cilkrts_mutex_unlock(w, &other->l->lock);
}


/* Lock macro Usage:
    BEGIN_WITH_WORKER_LOCK(w) {
        statement;
        statement;
        BEGIN_WITH_FRAME_LOCK(w, f) {
            statement;
            statement;
        } END_WITH_FRAME_LOCK(w, f);
    } END_WITH_WORKER_LOCK(w);
 */
#define BEGIN_WITH_WORKER_LOCK(w) __cilkrts_worker_lock(w); do
#define END_WITH_WORKER_LOCK(w)   while (__cilkrts_worker_unlock(w), 0)

#define BEGIN_WITH_FRAME_LOCK(w, f)                                     \
    do { full_frame *_locked = f; __cilkrts_frame_lock(w, _locked); do

#define END_WITH_FRAME_LOCK(w, f)                       \
    while (__cilkrts_frame_unlock(w, _locked), 0); } while (0)

/* W becomes the owner of F and F can be stolen from W */
static void make_runnable(__cilkrts_worker *w, full_frame *f)
{
    w->l->frame = f;

    /* CALL_STACK is invalid (the information is stored implicitly in W) */
    f->call_stack = 0;
}

static void make_unrunnable(__cilkrts_worker *w,
                            full_frame *f,
                            __cilkrts_stack_frame *sf,
                            int state_valid,
                            const char *why)
{
    /* CALL_STACK becomes valid again */
    f->call_stack = sf;

    /* stable_call_stack has to be valid */
    if (f->stable_call_stack && sf)
        CILK_ASSERT(f->stable_call_stack == sf);
    f->stable_call_stack = sf;

    if (sf) {
#if CILK_LIB_DEBUG
        if (__builtin_expect(sf->flags & CILK_FRAME_EXITING, 0))
            __cilkrts_bug("W%d suspending exiting frame %p/%p\n", w->self, f, sf);
#endif
        sf->flags |= CILK_FRAME_STOLEN | CILK_FRAME_SUSPENDED;
        sf->worker = 0;

        if (state_valid)
            __cilkrts_put_stack(f, sf);

        /* perform any system-dependent action, such as saving the
           state of the stack */
        __cilkrts_make_unrunnable_sysdep(w, f, sf, state_valid, why);
    }
}

/* Push the next full frame to be made active in this worker and increment its
 * join counter.  __cilkrts_push_next_frame and pop_next_frame work on a
 * one-element queue.  This queue is used to communicate across the runtime
 * from the code that wants to activate a frame to the code that can actually
 * begin execution on that frame.  They are asymetrical in that push
 * increments the join counter but pop does not decrement it.  Rather, a
 * single push/pop combination makes a frame active and increments its join
 * counter once. */
void __cilkrts_push_next_frame(__cilkrts_worker *w, full_frame *f)
{
    CILK_ASSERT(f);
    CILK_ASSERT(!w->l->next_frame);
    incjoin(f);

    w->l->next_frame = f;
}

/* Get the next full-frame to be made active in this worker.  The join count
 * of the full frame will have been incremented by the corresponding push
 * event.  See __cilkrts_push_next_frame, above.
 */
static full_frame *pop_next_frame(__cilkrts_worker *w)
{
    full_frame *f;
    f = w->l->next_frame;
    // Remove the frame from the next_frame field.
    //
    // If this is a user worker, then there is a chance that another worker
    // from our team could push work into our next_frame (if it is the last
    // worker doing work for this team).  The other worker's setting of the
    // next_frame could race with our setting of next_frame to NULL.  This is
    // the only possible race condition on next_frame.  However, if next_frame
    // has a non-NULL value, then it means the team still has work to do, and
    // there is no chance of another team member populating next_frame.  Thus,
    // it is safe to set next_frame to NULL, if it was populated.  There is no
    // need for an atomic op.
    if (NULL != f) {
        w->l->next_frame = NULL;
    }
    return f;
}

/*
 * Identify the single worker that is allowed to cross a sync in this frame.  A
 * thief should call this function when it is the first to steal work from a
 * user worker.  "First to steal work" may mean that there has been parallelism
 * in the user worker before, but the whole team sync'd, and this is the first
 * steal after that.
 *
 * This should happen while holding the worker and frame lock.
 */
static void set_sync_master(__cilkrts_worker *w, full_frame *f)
{
    w->l->last_full_frame = f;
    f->sync_master = w;
}

/*
 * The sync that ends all parallelism for a particular user worker is about to
 * be crossed.  Decouple the worker and frame.
 *
 * No locks need to be held since the user worker isn't doing anything, and none
 * of the system workers can steal from it.  But unset_sync_master() should be
 * called before the user worker knows about this work (i.e., before it is
 * inserted into the w->l->next_frame is set).
 */
static void unset_sync_master(__cilkrts_worker *w, full_frame *f)
{
    CILK_ASSERT(WORKER_USER == w->l->type);
    CILK_ASSERT(f->sync_master == w);
    f->sync_master = NULL;
    w->l->last_full_frame = NULL;
}

/*************************************************************
   THE protocol:
*************************************************************/
/*
  This is a protocol for work stealing that minimize the
  overhead on the victim.

  The protocol uses three shared pointes into the victim's deque: T
  (the ``tail''), H (the ``head'') and E (the ``exception''),
  with H <= E, H <= T.  (NB: "exception," in this case has nothing to do with
  C++ throw-catch exceptions -- it refers only to a non-normal return, i.e., a
  steal or similar scheduling exception.)

  Stack frames P, where H <= E < T, are available for stealing. 

  The victim operates on the T end of the stack.  The frame being
  worked on by the victim is not on the stack.  To push, the victim
  stores *T++=frame.  To pop, it obtains frame=*--T.

  After decrementing T, the condition E > T signals to the victim that
  it should invoke the runtime system ``THE'' exception handler.  The
  pointer E can become INFINITY, in which case the victim must invoke
  the THE exception handler as soon as possible.

  See "The implementation of the Cilk-5 multithreaded language", PLDI 1998,
  http://portal.acm.org/citation.cfm?doid=277652.277725, for more information
  on the THE protocol.
*/

/* the infinity value of E */
#define EXC_INFINITY  ((__cilkrts_stack_frame **) (-1))

static void increment_E(__cilkrts_worker *victim)
{
    __cilkrts_stack_frame *volatile *tmp;

    /* ASSERT: we own victim->lock */

    tmp = victim->exc;
    if (tmp != EXC_INFINITY) {
        /* On most x86 this pair of operations would be slightly faster
           as an atomic exchange due to the implicit memory barrier in
           an atomic instruction. */
        victim->exc = tmp + 1;
        __cilkrts_fence();
    }
}

static void decrement_E(__cilkrts_worker *victim)
{
    __cilkrts_stack_frame *volatile *tmp;

    /* ASSERT: we own victim->lock */

    tmp = victim->exc;
    if (tmp != EXC_INFINITY) {
        /* On most x86 this pair of operations would be slightly faster
           as an atomic exchange due to the implicit memory barrier in
           an atomic instruction. */
        victim->exc = tmp - 1;
        __cilkrts_fence(); /* memory fence not really necessary */
    }
}

#if 0
/* for now unused, will be necessary if we implement abort */
static void signal_THE_exception(__cilkrts_worker *wparent)
{
    wparent->exc = EXC_INFINITY;
    __cilkrts_fence();
}
#endif

static void reset_THE_exception(__cilkrts_worker *w)
{
    w->exc = w->head;
    __cilkrts_fence();
}

/* conditions under which victim->head can be stolen: */
static int can_steal_from(__cilkrts_worker *victim)
{
#if JFC_DEBUG
    if ((unsigned long)(victim->tail + 1) - (unsigned long)victim->head > 1000)
        __cilkrts_bug("can steal from: victim %d head %p tail %p\n",
                      victim->self, victim->head, victim->tail);
#endif
    return ((victim->head < victim->tail) && 
            (victim->head < victim->protected_tail));
}

/* Return TRUE if the frame can be stolen, false otherwise */
static int dekker_protocol(__cilkrts_worker *victim)
{
    /* ASSERT(E >= H); */

    increment_E(victim);

    /* ASSERT(E >= H + 1); */
    if (can_steal_from(victim)) {
        /* success, we can steal victim->head and set H <- H + 1
           in detach() */
        return 1;
    } else {
        /* failure, restore previous state */
        decrement_E(victim);
        return 0;    
    }
}

/* Link PARENT and CHILD in the spawn tree */
static full_frame *make_child(__cilkrts_worker *w, 
                              full_frame *parent,
                              __cilkrts_stack_frame *child_sf,
                              __cilkrts_stack *sd)
{
    full_frame *child = __cilkrts_make_full_frame(w, child_sf);

    child->parent = parent;
    push_child(parent, child);

    //DBGPRINTF("%d-          make_child - child_frame: %p, parent_frame: %p, child_sf: %p\n"
    //    "            parent - parent: %p, left_sibling: %p, right_sibling: %p, rightmost_child: %p\n"
    //    "            child  - parent: %p, left_sibling: %p, right_sibling: %p, rightmost_child: %p\n",
    //          w->self, child, parent, child_sf,
    //          parent->parent, parent->left_sibling, parent->right_sibling, parent->rightmost_child,
    //          child->parent, child->left_sibling, child->right_sibling, child->rightmost_child);

    CILK_ASSERT(parent->call_stack);
    child->is_call_child = (sd == NULL);

    /* PLACEHOLDER_STACK is used as non-null marker indicating that
       child should be treated as a spawn child even though we have not
       yet assigned a real stack to its parent. */
    if (sd == PLACEHOLDER_STACK)
        sd = NULL; /* Parent actually gets a null stack, for now */

    /* perform any system-dependent actions, such as capturing
       parameter passing information */
    /*__cilkrts_make_child_sysdep(child, parent);*/

    /* Child gets reducer map and stack of parent.
       Parent gets a new map and new stack. */
    child->reducer_map = parent->reducer_map;
    child->stack_self = parent->stack_self;
    child->sync_master = NULL;

    if (child->is_call_child) {
        /* Cause segfault on any attempted access.  The parent gets
           the child map and stack when the child completes. */
        parent->reducer_map = 0;
        parent->stack_self = 0;
    } else {
        parent->stack_self = sd;
        parent->reducer_map = __cilkrts_make_reducer_map(w);
        __cilkrts_bind_stack(parent,
                             __cilkrts_stack_to_pointer(parent->stack_self, child_sf),
                             child->stack_self,
                             child->sync_master);
    }

    incjoin(parent);
    return child;
}

static inline __cilkrts_stack_frame *__cilkrts_advance_frame(__cilkrts_stack_frame *sf)
{
    __cilkrts_stack_frame *p = sf->call_parent;
    sf->call_parent = 0;
    return p;
}

static full_frame *unroll_call_stack(__cilkrts_worker *w, 
                                     full_frame *f, 
                                     __cilkrts_stack_frame *const sf0)
{
    __cilkrts_stack_frame *sf = sf0;
    __cilkrts_stack_frame *rev = 0;
    __cilkrts_stack_frame *t;

    CILK_ASSERT(sf);
    /*CILK_ASSERT(sf->call_parent != sf);*/

    /* The leafmost frame is unsynched. */
    if (sf->worker != w)
        sf->flags |= CILK_FRAME_UNSYNCHED;

    /* Reverse the call stack to make a linked list ordered from parent
       to child.  sf->call_parent points to the child of SF instead of
       the parent.  */
    do {
        t = (sf->flags & (CILK_FRAME_DETACHED|CILK_FRAME_STOLEN|CILK_FRAME_LAST))? 0 : sf->call_parent;
        sf->call_parent = rev;
        rev = sf;
        sf = t;
    } while (sf);
    sf = rev;

    /* Promote each frame in order from parent to child, following the
       reversed list we just built. */
    make_unrunnable(w, f, sf, sf == sf0, "steal 1");

    /* T is the *child* of SF, because we have reversed the list */
    for (t = __cilkrts_advance_frame(sf); t;
         sf = t, t = __cilkrts_advance_frame(sf)) {
        f = make_child(w, f, t, NULL);
        make_unrunnable(w, f, t, t == sf0, "steal 2");
    }

    /* XXX What if the leafmost frame does not contain a sync
       and this steal is from promote own deque? */
    /*sf->flags |= CILK_FRAME_UNSYNCHED;*/

    CILK_ASSERT(!sf->call_parent);
    return f;
}

/* detach the top of the deque frame from the VICTIM and install a new
   CHILD frame in its place */
static void detach(__cilkrts_worker *w,
                   __cilkrts_worker *victim,
                   __cilkrts_stack *sd)
{
    /* ASSERT: we own victim->lock */

    full_frame *parent, *child, *loot;
    __cilkrts_stack_frame *volatile *h;
    __cilkrts_stack_frame *sf;

    w->l->team = victim->l->team;

    CILK_ASSERT(w->l->frame == 0 || w == victim);

    h = victim->head;

    CILK_ASSERT(*h);

#if JFC_DEBUG
    fprintf(stderr, "detach %u tail %p head <- %p { %p %p ... }\n",
            victim->self, (void *)victim->tail, (void *)(h+1), h[0], h[1]);
#endif
    victim->head = h + 1;

    parent = victim->l->frame;
    BEGIN_WITH_FRAME_LOCK(w, parent) {
        /* parent no longer referenced by victim */
        decjoin(parent);

        /* obtain the victim call stack */
        sf = *h;

        /* perform system-dependent normalizations */
        /*__cilkrts_normalize_call_stack_on_steal(sf);*/

        /* unroll PARENT with call stack SF, adopt the youngest
           frame LOOT */
        loot = unroll_call_stack(w, parent, sf);

        if (WORKER_USER == victim->l->type &&
            NULL == victim->l->last_full_frame) {
            // Mark this looted frame as special: only the original user worker
            // may cross the sync.
            set_sync_master(victim, loot);
        }

        /* LOOT is the next frame that the thief W is supposed to
           run, unless the thief is stealing from itself, in which
           case the thief W == VICTIM executes CHILD and nobody
           executes LOOT. */
        if (w == victim) {
            /* Pretend that frame has been stolen */
            loot->call_stack->flags |= CILK_FRAME_UNSYNCHED;
            loot->simulated_stolen = 1;
        }
        else
            __cilkrts_push_next_frame(w, loot);

        child = make_child(w, loot, 0, sd);

        BEGIN_WITH_FRAME_LOCK(w, child) {
            /* install child in the victim's work queue, taking
               the parent's place */
            /* child is referenced by victim */
            incjoin(child);
            make_runnable(victim, child);
            CILK_ASSERT(victim->reducer_map == child->reducer_map);
        } END_WITH_FRAME_LOCK(w, child);
    } END_WITH_FRAME_LOCK(w, parent);
}

static void random_steal(__cilkrts_worker *w)
{
    __cilkrts_worker *victim;
    __cilkrts_stack *sd;
    int n;
    int success = 0;

    // Nothing's been stolen yet. When true, this will flag
    // setup_for_execution_pedigree to increment the pedigree
    w->l->work_stolen = 0;

    /* If the user has disabled stealing (using the debugger) we fail */
    if (__builtin_expect(w->g->stealing_disabled, 0))
        return;

    CILK_ASSERT(w->l->type == WORKER_SYSTEM || w->l->team == w);

    /* If there is only one processor work can still be stolen.
       There must be only one worker to prevent stealing. */
    CILK_ASSERT(w->g->nworkers > 1);

    /* Verify that we can get a stack.  If not, no need to continue. */
    sd = __cilkrts_get_stack(w);
    if (NULL == sd) {
        return;
    }

    /* pick random *other* victim */
    n = myrand(w) % (w->g->nworkers - 1); if (n >= w->self) ++n;
    victim = w->g->workers[n];

    /* do not steal from self */
    CILK_ASSERT (victim != w);

    /* Execute a quick check before engaging in the THE protocol.
       Avoid grabbing locks if there is nothing to steal. */
    if (!can_steal_from(victim)) {
        NOTE_INTERVAL(w, INTERVAL_STEAL_FAIL_EMPTYQ);
        __cilkrts_release_stack(w, sd);
        return;
    }

    /* Attempt to steal work from the victim */
    if (worker_trylock_other(w, victim)) {
        if (w->l->type == WORKER_USER && victim->l->team != w) {

            // Fail to steal if this is a user worker and the victim is not
            // on this team.  If a user worker were allowed to steal work
            // descended from another user worker, the former might not be
            // done with its work by the time it was needed to resume and
            // unbind.  Therefore, user workers are not permitted to change
            // teams.

            // There is no race on the victim's team because the victim cannot
            // change its team until it runs out of work to do, at which point
            // it will try to take out its own lock, and this worker already
            // holds it.
            NOTE_INTERVAL(w, INTERVAL_STEAL_FAIL_USER_WORKER);

        } else if (victim->l->frame) {
            if (dekker_protocol(victim)) {
                START_INTERVAL(w, INTERVAL_STEAL_SUCCESS) {
                    success = 1;
                    detach(w, victim, sd);
                    DBGPRINTF ("%d-%p: Stealing work from worker %d\n"
                               "            sf: %p, call parent: %p\n",
                               w->self, GetCurrentFiber(), victim->self,
                               w->l->next_frame->call_stack,
                               w->l->next_frame->call_stack->call_parent);
                } STOP_INTERVAL(w, INTERVAL_STEAL_SUCCESS);
            } else {
                NOTE_INTERVAL(w, INTERVAL_STEAL_FAIL_DEKKER);
            }
        } else {
            NOTE_INTERVAL(w, INTERVAL_STEAL_FAIL_EMPTYQ);
        }
        worker_unlock_other(w, victim);
    } else {
        NOTE_INTERVAL(w, INTERVAL_STEAL_FAIL_LOCK);
    }

    // Record whether work was stolen.  When true, this will flag
    // setup_for_execution_pedigree to increment the pedigree
    w->l->work_stolen = success;

    if (0 == success) {
        // failed to steal work.  Return the stack to the pool.
        __cilkrts_release_stack(w, sd);
    }
}

/* at a provably good steal, incorporate the accumulated reducers of
   children into the parent's map */
static void provably_good_steal_reducers(__cilkrts_worker *w,
                                         full_frame       *f)
{
    f->reducer_map = 
        __cilkrts_merge_reducer_maps(
            w, f->children_reducer_map, f->reducer_map);
    f->children_reducer_map = 0;
}

/* at a provably good steal, incorporate the accumulated exceptions of
   children into the parent's exception */
static void provably_good_steal_exceptions(__cilkrts_worker *w, 
                                           full_frame *f)
{
    f->pending_exception =
        __cilkrts_merge_pending_exceptions(
            w, f->child_pending_exception, f->pending_exception);
    f->child_pending_exception = NULL;
}

/* At sync discard the frame's old stack and take the leftmost child's. */
static void provably_good_steal_stacks(__cilkrts_worker *w, full_frame *f)
{
    __cilkrts_stack *s;

#if JFC_DEBUG
    fprintf(stderr, "W%d sync stacks %p/%p free %p owner %p/%p\n", w->self,
            f, f->call_stack, f->stack_self, f->owner, f->owner_child);
#endif
    s = f->stack_self;
    f->stack_self = f->stack_child;
    f->stack_child = NULL;
    if (s)
        __cilkrts_release_stack(w, s);

    /* We don't have a stack to bind right now, so use the
       BIND_PROVABLY_GOOD_STACK magic number, instead */
    __cilkrts_bind_stack(f, f->sync_sp, BIND_PROVABLY_GOOD_STACK, NULL);
}

static void __cilkrts_mark_synched(full_frame *f)
{
    f->call_stack->flags &= ~CILK_FRAME_UNSYNCHED;
    f->simulated_stolen = 0;
}

static int provably_good_steal(__cilkrts_worker *w,
                               full_frame       *f)
{
    int abandoned = 1;  // True if we can't make any more progress on this
                        // thread and are going to attempt to steal work from
                        // someone else

    START_INTERVAL(w, INTERVAL_PROVABLY_GOOD_STEAL) {
        if (decjoin(f) == 0) {
            provably_good_steal_reducers(w, f);
            provably_good_steal_exceptions(w, f);
            provably_good_steal_stacks(w, f);
            __cilkrts_mark_synched(f);

            /* If the original owner wants this frame back (to resume
               it on its original thread) pass it back now. */
            if (NULL != f->sync_master) {
                // The frame wants to go back and be executed by the original
                // user thread.  We can throw caution to the wind and push the
                // frame straight onto its queue because the only way we have
                // gotten to this point of being able to continue execution of
                // the frame is if the original user worker is spinning without
                // work.
                unset_sync_master(w->l->team, f);
                __cilkrts_push_next_frame(w->l->team, f);

                // If this is the team leader we're not abandoning the work
                if (w == w->l->team)
                    abandoned = 0;
            } else {
                __cilkrts_push_next_frame(w, f);
                abandoned = 0;  // Continue working on this thread
            }
        }
    } STOP_INTERVAL(w, INTERVAL_PROVABLY_GOOD_STEAL);

    return abandoned;
}

static void unconditional_steal(__cilkrts_worker *w,
                                full_frame *f)
{
    START_INTERVAL(w, INTERVAL_UNCONDITIONAL_STEAL) {
        decjoin(f);
        __cilkrts_push_next_frame(w, f);
    } STOP_INTERVAL(w, INTERVAL_UNCONDITIONAL_STEAL);
}

/* CHILD is about to die.  Give its reducers to a sibling
   or to the parent. */
static void splice_reducers(__cilkrts_worker *w, 
                            full_frame *parent,
                            full_frame *child)
{
    /* ASSERT: we own parent->lock */
    CILK_ASSERT(!child->children_reducer_map);

    if (child->is_call_child) {
        /* if CHILD is attached to PARENT, then CHILD cannot have
           right siblings */
        CILK_ASSERT(child->right_reducer_map == 0);

        /* merge own map with map of parent, which is guaranteed not
           to be running because the child is attached to it */
        CILK_ASSERT(child == parent->rightmost_child);

        /* if the child is attached to the parent, then the parent
           cannot have accumulated any updates in its reducer
           map. */
        CILK_ASSERT(parent->reducer_map == 0);

        parent->reducer_map = child->reducer_map;
        child->reducer_map = 0;        
    } else {
        full_frame *left;

        child->reducer_map = __cilkrts_merge_reducer_maps(
            w, child->reducer_map, child->right_reducer_map);
        child->right_reducer_map = 0;

        if ((left = child->left_sibling)) {
            /* merge own map with right map of left sibling */
            left->right_reducer_map = __cilkrts_merge_reducer_maps(
                w, left->right_reducer_map, child->reducer_map);
            child->reducer_map = 0;
        } else {
            /* merge own map with children map in the parent */
            parent->children_reducer_map = __cilkrts_merge_reducer_maps(
                w, parent->children_reducer_map, child->reducer_map);
            child->reducer_map = 0;
        }
    }
}

/* CHILD is about to die.  Give its exceptions to a sibling or to the
   parent.  */
static void splice_exceptions(__cilkrts_worker *w, 
                              full_frame *parent,
                              full_frame *child)
{
    if (child->is_call_child) {
        CILK_ASSERT(NULL == child->right_pending_exception);
        CILK_ASSERT(NULL == parent->pending_exception);
        parent->pending_exception = child->pending_exception;
        child->pending_exception = 0;
    } else {
        full_frame *left;

        /* If any exceptions occurred logically after us, merge that
           exception with ours */
        child->pending_exception =
            __cilkrts_merge_pending_exceptions(
                w, child->pending_exception, child->right_pending_exception);
        child->right_pending_exception = 0;

        if ((left = child->left_sibling)) {
            /* If there's a sibling to our left, which means they're
               logically executing "before" us.  Merge our pending
               exception into their right_pending_exception */
            left->right_pending_exception =
                __cilkrts_merge_pending_exceptions(
                    w, left->right_pending_exception, child->pending_exception);
            child->pending_exception = 0;
        } else {
            /* We are the left-most sibling.  Merge our pending
               exception into our parent */
            parent->child_pending_exception =
                __cilkrts_merge_pending_exceptions(
                    w, parent->child_pending_exception, 
                    child->pending_exception);
            child->pending_exception = 0;
        }
    }
}

static void splice_stacks(__cilkrts_worker *w, 
                          full_frame *parent,
                          full_frame *child)
{
#if CILK_LIB_DEBUG
    if (parent->call_stack)
        CILK_ASSERT(!(parent->call_stack->flags & CILK_FRAME_MBZ));
    CILK_ASSERT(!(parent->stack_child && parent->owner_child));
#endif

    /* A synched frame does not have accumulated child reducers. */
    CILK_ASSERT(!child->stack_child);

    if (child->is_call_child) {
#if JFC_DEBUG
        fprintf(stderr, "W%d return %p to %p/%p stack %p owner %p/%p\n",
                w->self,
                child, parent, parent->call_stack,
                child->stack_self ? __cilkrts_stack_to_pointer(child->stack_self, 0) : 0,
                parent->owner, child->owner);
#endif
        /* An attached parent has no self stack.  It may have
           accumulated child stacks or child owners, which should be
           ignored until sync. */
        CILK_ASSERT(!parent->stack_self);
        parent->stack_self = child->stack_self;
        child->stack_self = NULL;
    } else {
        __cilkrts_stack *s = child->stack_self;
#if JFC_DEBUG
        fprintf(stderr, "W%d unspawn %p to %p/%p %s %p/%p\n", w->self,
                child, parent, parent->call_stack,
                (child->left_sibling || parent->stack_child) ? "free" : "keep",
                s, s ? __cilkrts_stack_to_pointer(s, 0) : 0);
#endif
        /* This assertion is no longer valid when the leftmost thread
           has no allocated stack. CILK_ASSERT(s); */
        child->stack_self = NULL;
        /* A spawned child deposits a stack or an owner into the
           parent's child slot. */
        if (child->left_sibling || parent->stack_child) {
            __cilkrts_release_stack(w, s);
        } else {
            parent->stack_child = s;
            if (s)
                CILK_ASSERT(parent->stack_self != s);
        }
    }
#if CILK_LIB_DEBUG
    CILK_ASSERT(!(parent->stack_child && parent->owner_child));
#endif
}

static void finalize_child(__cilkrts_worker *w, 
                           full_frame *parent,
                           full_frame *child)
{
    START_INTERVAL(w, INTERVAL_FINALIZE_CHILD) {
        CILK_ASSERT(child->join_counter == 0);
        CILK_ASSERT(!child->rightmost_child);

        START_INTERVAL(w, INTERVAL_SPLICE_REDUCERS) {
            splice_reducers(w, parent, child);
        } STOP_INTERVAL(w, INTERVAL_SPLICE_REDUCERS);
        START_INTERVAL(w, INTERVAL_SPLICE_EXCEPTIONS) {
            splice_exceptions(w, parent, child);
        } STOP_INTERVAL(w, INTERVAL_SPLICE_EXCEPTIONS);
        START_INTERVAL(w, INTERVAL_SPLICE_STACKS) {
            splice_stacks(w, parent, child);
        } STOP_INTERVAL(w, INTERVAL_SPLICE_STACKS);

        /* remove CHILD from list of children of PARENT */
        unlink_child(parent, child);

        if (child->is_call_child) {
            /* continue with the parent. */
            unconditional_steal(w, parent);
        }
        else if (parent->simulated_stolen) {
            /* continue with the parent. */
            unconditional_steal(w, parent);
        }
        else {
            /* continue with the parent, maybe */
            provably_good_steal(w, parent);
        }

        __cilkrts_destroy_full_frame(w, child);
    } STOP_INTERVAL(w, INTERVAL_FINALIZE_CHILD);
}


static void setup_for_execution_reducers(__cilkrts_worker *w, 
                                         full_frame *f)
{
    w->reducer_map = f->reducer_map;
}

static void setup_for_execution_exceptions(__cilkrts_worker *w, 
                                           full_frame *f)
{
    CILK_ASSERT(NULL == w->l->pending_exception);
    w->l->pending_exception = f->pending_exception;
    f->pending_exception = NULL;
}

#if 0 /* unused */
static void setup_for_execution_stack(__cilkrts_worker *w, 
                                      full_frame *f)
{
#if JFC_DEBUG
    fprintf(stderr, "W%d setup_for_execution_stack %p stack %p\n",
            w->self, f, f->stack_self);
#endif
}
#endif

/*
 * setup_for_execution_pedigree
 *
 * Copies the pedigree information from the frame we're resuming to the
 * worker.  Increments the pedigree if this is work that has been stolen
 * to match the increment on a return from a spawn helper.
 */
static void setup_for_execution_pedigree(__cilkrts_worker *w)
{
    __cilkrts_stack_frame *sf = w->current_stack_frame;

    CILK_ASSERT(NULL != w->current_stack_frame);

    // Update the worker's pedigree information if this is an ABI 1 or later
    // frame.  If we're just marshalling onto this worker, do not increment
    // the rank since that wouldn't happen in a sequential execution
    if (CILK_FRAME_VERSION_VALUE(sf->flags) >= 1)
    {
        if (w->l->work_stolen)
            w->pedigree.rank = w->current_stack_frame->parent_pedigree.rank + 1;
        else
            w->pedigree.rank = w->current_stack_frame->parent_pedigree.rank;
        w->pedigree.next = w->current_stack_frame->parent_pedigree.next;
        w->l->work_stolen = 0;
    }
}

static void setup_for_execution(__cilkrts_worker *w, 
                                full_frame *f)
{
    setup_for_execution_reducers(w, f);
    setup_for_execution_exceptions(w, f);
    /*setup_for_execution_stack(w, f);*/

    f->call_stack->worker = w;
    w->current_stack_frame = f->call_stack;

    setup_for_execution_pedigree(w);
    __cilkrts_setup_for_execution_sysdep(w, f);

    w->head = w->tail = w->l->ltq;
    reset_THE_exception(w);

    make_runnable(w, f);
}

/* The current stack is about to either be suspended or destroyed.  This
 * function will switch to the stack on which the scheduler is suspended and
 * resume running the scheduler within function do_work().  Upon waking up,
 * the scheduler will run the 'cont' function, using the supplied worker and
 * frame.
 */
static NORETURN longjmp_into_runtime(__cilkrts_worker *w,
                                     scheduling_stack_fcn_t fcn,
                                     __cilkrts_stack_frame *sf)
{
    full_frame *f, *f2;

    CILK_ASSERT(!w->l->post_suspend);
    f = w->l->frame;

    // If we've got only one worker, stealing shouldn't be possible.
    //
    // Assume that this is a steal or return from spawn in a force-reduce case.
    // We don't have a scheduling stack to switch to, so call the continuation
    // function directly.
    if (1 == w->g->P) {
        fcn(w, f, sf);

        /* The call to function c() will have pushed f as the next frame.  If
         * this were a normal (non-forced-reduce) execution, there would have
         * been a pop_next_frame call in a separate part of the runtime.  We
         * must call pop_next_frame here to complete the push/pop cycle. */
        f2 = pop_next_frame(w);

        setup_for_execution(w, f2);
        __cilkrts_resume(w, f2, w->current_stack_frame); /* no return */
        CILK_ASSERT(("returned from __cilkrts_resume", 0));
    }

    w->l->post_suspend = fcn;
    w->l->suspended_stack = sf;

    ITT_SYNC_RELEASING(w);
    ITT_SYNC_PREPARE(w);

    // If this is a user worker, and it's the first time that it's returned to
    // a stolen parent, we need to import the thread.  This will create a
    // scheduling stack or fiber, switch to that, and run the scheduling loop
    // on it
    if ((WORKER_USER == w->l->type) && (0 == w->l->user_thread_imported))
    {
        // We're importing the thread
        w->l->user_thread_imported = 1;

        __cilkrts_sysdep_import_user_thread(w);
        CILK_ASSERT(0); // Should never reach this point.
    }


#ifndef _WIN32

    // Jump to this thread's scheduling stack.
    longjmp(w->l->env, 1);
#else
    DBGPRINTF ("%d-%p: longjmp_into_runtime - "
               "Switching to scheduling fiber - %p\n"
               "           continuation routine: %p, sf: %p\n",
               w->self, GetWorkerFiber(w), w->sysdep->scheduling_fiber,
               fcn, sf);
#ifdef _DEBUG
    SetWorkerThreadName(w, NULL);
#endif
    SwitchToFiber(w->sysdep->scheduling_fiber);

    /* Since we switched away from the fiber on which this function was
     * entered, we will not get here until either the initial fiber is
     * resumed.  If the initial fiber belonged to a thief at a sync, then
     * the longjmp below will re-initialize the fiber for another steal.
     * If this fiber belonged to a victim, then the longjmp below will
     * resume the victim after the sync.
     */
    __cilkrts_resume_after_longjmp_into_runtime();
#endif
}

/*
 * Send a message to the children of the specified worker: run or wait.
 */
static void notify_children(__cilkrts_worker *w, unsigned int msg)
{
    int child_num;
    __cilkrts_worker *child;
    int num_sys_workers = w->g->P - 1;

    // If worker is "n", then its children are 2n + 1, and 2n + 2.
    child_num = (w->self << 1) + 1;
    if (child_num < num_sys_workers) {
        child = w->g->workers[child_num];
        CILK_ASSERT(child->l->signal_node);
        signal_node_msg(child->l->signal_node, msg);
        child_num++;
        if (child_num < num_sys_workers) {
            child = w->g->workers[child_num];
            CILK_ASSERT(child->l->signal_node);
            signal_node_msg(child->l->signal_node, msg);
        }
    }
}

/*
 * Notify this worker's children that they need to wait.
 */
static void notify_children_wait(__cilkrts_worker *w)
{
    notify_children(w, 0);
}

/*
 * Notify this worker's children to run and start trying to steal.
 */
static void notify_children_run(__cilkrts_worker *w)
{
    notify_children(w, 1);
}

static void do_work(__cilkrts_worker *w, full_frame *f)
{
    __cilkrts_stack_frame *sf;

#if JFC_DEBUG
    fprintf(stderr, "do_work %u %p\n", w->self, f);
#endif

#ifndef _WIN32
    cilkbug_assert_no_uncaught_exception();
#endif

    BEGIN_WITH_WORKER_LOCK(w) {
        CILK_ASSERT(!w->l->frame);
        BEGIN_WITH_FRAME_LOCK(w, f) {
            sf = f->call_stack;
            CILK_ASSERT(sf && !sf->call_parent);
            setup_for_execution(w, f);
        } END_WITH_FRAME_LOCK(w, f);
    } END_WITH_WORKER_LOCK(w);

#if CILK_LIB_DEBUG
    if (!(sf->flags & CILK_FRAME_UNSYNCHED))
        CILK_ASSERT(!f->stack_child);
    if (sf->flags & CILK_FRAME_EXITING) {
        __cilkrts_bug("W%d: resuming frame %p/%p suspended in exit\n",
                      w->self, f, sf);
    }
#endif

    /* run it */
    if (setjmp(w->l->env) == 0) {
        __cilkrts_resume(w, f, sf);

        /* unreached---the call to cilk_resume exits through longjmp */
        CILK_ASSERT(0);
    }

    /* This point is reached for three reasons:

       1. Undo-detach finds parent stolen.

       2. Sync suspends frame.

       3. Return from Cilk entry point.

       In the first two cases the frame may be truly suspended or
       may be immediately executed by this worker after provably_good_steal.

       The active frame and call_stack may have changed since _resume.  */
    run_scheduling_stack_fcn(w);

    /* The worker borrowed the full frame's reducer map.
       Clear the extra reference.  Bookkeeping uses the
       copy in the frame, not the worker. */
    w->reducer_map = 0;

#ifndef _WIN32
    cilkbug_assert_no_uncaught_exception();
#endif
}

/*
 * Try to do work.  If there is none available, try to steal some and do it.
 */
static void schedule_work(__cilkrts_worker *w)
{
    full_frame *f;

    f = pop_next_frame(w);

    // If there is no work on the queue, try to steal some.
    if (NULL == f) {
        START_INTERVAL(w, INTERVAL_STEALING) {
            if (w->l->type != WORKER_USER && w->l->team != NULL) {
                // At this point, the worker knows for certain that it has run
                // out of work.  Therefore, it loses its team affiliation.  User
                // workers never change teams, of course.
                __cilkrts_worker_lock(w);
                w->l->team = NULL;
                __cilkrts_worker_unlock(w);
            }
            random_steal(w);
        } STOP_INTERVAL(w, INTERVAL_STEALING);

        // If the steal was successful, then the worker has populated its next
        // frame with the work to resume.
        f = pop_next_frame(w);
        if (NULL == f) {
            // Punish the worker for failing to steal.
            // No quantum for you!
            __cilkrts_yield();
            w->l->steal_failure_count++;
            return;
        } else {
            // Reset steal_failure_count since there is obviously still work to
            // be done.
            w->l->steal_failure_count = 0;
        }
    }
    CILK_ASSERT(f);

    // Do the work that was on the queue or was stolen.
    START_INTERVAL(w, INTERVAL_WORKING) {
        do_work(w, f);
        ITT_SYNC_SET_NAME_AND_PREPARE(w, w->l->sync_return_address);
    } STOP_INTERVAL(w, INTERVAL_WORKING);
}

static void __cilkrts_scheduler(__cilkrts_worker *w)
{
    ITT_SYNC_PREPARE(w);

    START_INTERVAL(w, INTERVAL_IN_SCHEDULER) {

        /* this thread now becomes a worker---associate the thread
           with the worker state */
        __cilkrts_set_tls_worker(w);

        /* Notify tools about the new worker. Inspector needs this, but we
           don't want to confuse Cilkscreen with system threads.  User threads
           do this notification in bind_thread */
        if (! w->g->under_ptool)
            __cilkrts_cilkscreen_establish_worker(w);

        mysrand(w, (w->self + 1));

        if (WORKER_SYSTEM == w->l->type) {
            // Runtime begins in a wait-state and is woken up by the first user
            // worker when the runtime is ready.
            signal_node_wait(w->l->signal_node);
            // ...
            // Runtime is waking up.
            notify_children_run(w);
            w->l->steal_failure_count = 0;
        }

        while (!w->g->work_done) {

            switch (worker_runnable(w))
            {
            case SCHEDULE_RUN:             // do some work.
                schedule_work(w);
                break;

            case SCHEDULE_WAIT:            // go into wait-mode.
                CILK_ASSERT(WORKER_SYSTEM == w->l->type);
                notify_children_wait(w);
                signal_node_wait(w->l->signal_node);
                // ...
                // Runtime is waking up.
                notify_children_run(w);
                w->l->steal_failure_count = 0;
                break;

            case SCHEDULE_EXIT:            // exit the scheduler.
                CILK_ASSERT(WORKER_USER != w->l->type);
                break;

            default:
                CILK_ASSERT(0);
                abort();
            }

        } // while (!w->g->work_done)

    } STOP_INTERVAL(w, INTERVAL_IN_SCHEDULER);

    CILK_ASSERT(WORKER_SYSTEM == w->l->type);
}

/*************************************************************
  Scheduler functions that are callable by client code
*************************************************************/
static full_frame *disown(__cilkrts_worker *w,
                          full_frame *f,
                          __cilkrts_stack_frame *sf,
                          const char *why)
{
    CILK_ASSERT(f);
    make_unrunnable(w, f, sf, sf != 0, why);
    w->l->frame = 0;
    return f->parent;
}

NORETURN __cilkrts_c_sync(__cilkrts_worker *w,
                          __cilkrts_stack_frame *sf)
{
    longjmp_into_runtime(w, do_sync, sf);
}

static void do_sync(__cilkrts_worker *w, full_frame *f, __cilkrts_stack_frame *sf)
{
    int abandoned = 1;

    START_INTERVAL(w, INTERVAL_SYNC_CHECK) {
        BEGIN_WITH_WORKER_LOCK(w) {
            f = w->l->frame;
            CILK_ASSERT(f);
            CILK_ASSERT(sf->call_parent == 0);
            CILK_ASSERT(sf->flags & CILK_FRAME_UNSYNCHED);

            /* A frame entering a nontrivial sync always has a
               stack_self.  A topmost frame after a sync does
               not; it is back on the caller's stack. */
            CILK_ASSERT(f->stack_self || f->simulated_stolen);

            // Notify TBB that we're orphaning the stack. We'll reclaim it
            // again if we continue
            __cilkrts_invoke_stack_op(w, CILK_TBB_STACK_ORPHAN, f->stack_self);

            BEGIN_WITH_FRAME_LOCK(w, f) {
#ifdef _WIN32
                __cilkrts_save_exception_state(w, f);
#else
                // Move any pending exceptions into the full frame
                CILK_ASSERT(NULL == f->pending_exception);
                f->pending_exception = w->l->pending_exception;
                w->l->pending_exception = NULL;
#endif
                /* if (f->stack_self) see above comment */ {
                    __cilkrts_stack *s = f->stack_self;
                    f->stack_self = NULL;
                    __cilkrts_release_stack(w, s);
                }
                disown(w, f, sf, "sync"); 

                // Update the frame's pedigree information if this is an ABI 1 or later
                // frame
                if (CILK_FRAME_VERSION_VALUE(sf->flags) >= 1)
                {
                    sf->parent_pedigree.rank = w->pedigree.rank;
                    sf->parent_pedigree.next = w->pedigree.next;
                }

                /* the decjoin() for disown() is in
                   provably_good_steal() */

                abandoned = provably_good_steal(w, f);
            } END_WITH_FRAME_LOCK(w, f);
        } END_WITH_WORKER_LOCK(w);
    } STOP_INTERVAL(w, INTERVAL_SYNC_CHECK);

#ifdef __INTEL_COMPILER
    // If we can't make any further progress on this thread, tell Inspector
    // that we're abandoning the work and will go find something else to do.
    if (abandoned)
    {
        __notify_intrinsic("cilk_sync_abandon", 0);
    }
#endif // defined __INTEL_COMPILER

    return; /* back to scheduler loop */
}

/* worker W completely promotes its own deque, simulating the case
   where the whole deque is stolen.  We use this mechanism to force
   the allocation of new storage for reducers for race-detection
   purposes. */
void __cilkrts_promote_own_deque(__cilkrts_worker *w)
{
#if JFC_DEBUG > 0
    fprintf(stderr, "W%d promote own deque (%ld)\n", w->self, w->tail - w->head);
#endif
    BEGIN_WITH_WORKER_LOCK(w) {
        while (dekker_protocol(w)) {
            /* PLACEHOLDER_STACK is used as non-null marker to tell detach()
               and make_child() that this frame should be treated as a spawn
               parent, even though we have not assigned it a stack. */
            detach(w, w, PLACEHOLDER_STACK);

        }
    } END_WITH_WORKER_LOCK(w);
}



/* the client code calls this function after a spawn when the dekker
   protocol fails.  The function may either return or longjmp
   into the rts */
void __cilkrts_c_THE_exception_check(__cilkrts_worker *w)
{
    full_frame *f;
    int stolen_p;

    START_INTERVAL(w, INTERVAL_THE_EXCEPTION_CHECK);

    BEGIN_WITH_WORKER_LOCK(w) {
        f = w->l->frame;
        CILK_ASSERT(f);

        /* This code is called only upon a normal return and never
           upon an exceptional return.  Assert that this is the
           case. */
        CILK_ASSERT(!w->l->pending_exception);

        reset_THE_exception(w);
        stolen_p = !(w->head < (w->tail + 1)); /* +1 because tail was
                                                  speculatively
                                                  decremented by the
                                                  compiled code */

        if (stolen_p)
            /* XXX This will be charged to THE for accounting purposes */
            __cilkrts_save_exception_state(w, f);
    } END_WITH_WORKER_LOCK(w);

    STOP_INTERVAL(w, INTERVAL_THE_EXCEPTION_CHECK);

    if (stolen_p)
    {
        DBGPRINTF ("%d-%p: longjmp_into_runtime from __cilkrts_c_THE_exception_check\n", w->self, GetWorkerFiber(w));
        longjmp_into_runtime(w, do_return_from_spawn, 0);
        DBGPRINTF ("%d-%p: returned from longjmp_into_runtime from __cilkrts_c_THE_exception_check?!\n", w->self, GetWorkerFiber(w));
    }
    else
    {
        NOTE_INTERVAL(w, INTERVAL_THE_EXCEPTION_CHECK_USELESS);
        return;
    }
}

/* Return an exception to a stolen parent. */
NORETURN __cilkrts_exception_from_spawn(__cilkrts_worker *w)
{
    full_frame *f = w->l->frame;
    __cilkrts_stack_frame *sf;

    longjmp_into_runtime(w, do_return_from_spawn, 0);
    CILK_ASSERT(0);
}

static void do_return_from_spawn(__cilkrts_worker *w,
                                 full_frame *f,
                                 __cilkrts_stack_frame *sf)
{
    full_frame *parent;

    // If we're interoperating with TBB, tell them that we are orphaning the
    // stack.
    // FIXME - can finalize_child call into user-defined code?  If so, we have a
    // problem because that code might be calling TBB.
    // FIXME - having out of line call seems inefficient.  Does Intel compiler
    // inline it across translation units?

    BEGIN_WITH_WORKER_LOCK(w) {
        CILK_ASSERT(!f->is_call_child);
        CILK_ASSERT(f == w->l->frame);

        BEGIN_WITH_FRAME_LOCK(w, f) {
            if( f->stack_self )
            {
                // Notify TBB that we're returning from a spawn and orphaning
                // the stack. We'll re-adopt it if we continue
                __cilkrts_invoke_stack_op(w, CILK_TBB_STACK_ORPHAN,
                                          f->stack_self);
            }
            parent = disown(w, f, 0, "unspawn");
            CILK_ASSERT(parent);
            decjoin(f);
        } END_WITH_FRAME_LOCK(w, f);

        BEGIN_WITH_FRAME_LOCK(w, parent) {
            finalize_child(w, parent, f);
        } END_WITH_FRAME_LOCK(w, parent);
    } END_WITH_WORKER_LOCK(w);

    return;
}

#ifdef _WIN32
/* migrate an exception across fibers.  Call this function when an exception has
 * been thrown and has to traverse across a steal.  The exception has already
 * been wrapped up, so all that remains is to longjmp() into the continuation,
 * sync, and re-raise it.
 */
void __cilkrts_migrate_exception(__cilkrts_stack_frame *sf) {

    __cilkrts_worker *w = sf->worker;
    full_frame *f;

    BEGIN_WITH_WORKER_LOCK(w) {

        f = w->l->frame;
        reset_THE_exception(w);
        /* there is no need to check for a steal because we wouldn't be here if
           there weren't a steal. */
        __cilkrts_save_exception_state(w, f);

    } END_WITH_WORKER_LOCK(w);

    longjmp_into_runtime(w, do_migrate_exception, 0); /* does not return. */
    CILK_ASSERT(! "Shouldn't be here...");
}

static void do_migrate_exception(__cilkrts_worker *w,
                                  full_frame *f,
                                  __cilkrts_stack_frame *sf) {

    full_frame *parent;

    BEGIN_WITH_WORKER_LOCK(w) {
        CILK_ASSERT(!f->is_call_child);
        CILK_ASSERT(f == w->l->frame);

        BEGIN_WITH_FRAME_LOCK(w, f) {
            parent = disown(w, f, 0, "unspawn");
            decjoin(f);
        } END_WITH_FRAME_LOCK(w, f);

        BEGIN_WITH_FRAME_LOCK(w, parent) {
            finalize_child(w, parent, f);
        } END_WITH_FRAME_LOCK(w, parent);

#ifndef _WIN32
        if (!w->current_stack_frame) {
            w->current_stack_frame = parent->call_stack;
        }
        w->current_stack_frame->flags |= CILK_FRAME_EXCEPTING;
#endif

    } END_WITH_WORKER_LOCK(w);

    return;
}
#endif

/* Pop a call stack from TAIL.  Return the call stack, or NULL if the
   queue is empty */
__cilkrts_stack_frame *__cilkrts_pop_tail(__cilkrts_worker *w)
{
    __cilkrts_stack_frame *sf;
    BEGIN_WITH_WORKER_LOCK(w) {
        __cilkrts_stack_frame *volatile *tail = w->tail;
        if (w->head < tail) {
            --tail;
            sf = *tail;
            w->tail = tail;
#if JFC_DEBUG
            fprintf(stderr, "Cilk: %u tail <- %p (restore stealing)\n", w->self, tail);
#endif
        } else {
            sf = 0;
        }
    } END_WITH_WORKER_LOCK(w);
    return sf;
}

/* Return from a call, not a spawn. */
void __cilkrts_return(__cilkrts_worker *w)
{
    full_frame *f, *parent;
    START_INTERVAL(w, INTERVAL_RETURNING);

    BEGIN_WITH_WORKER_LOCK(w) {
        f = w->l->frame;
        CILK_ASSERT(f);
        CILK_ASSERT(f->join_counter == 1);
        /* This path is not used to return from spawn. */
        CILK_ASSERT(f->is_call_child);

        BEGIN_WITH_FRAME_LOCK(w, f) {
            parent = disown(w, f, 0, "return");
            decjoin(f);

#ifdef _WIN32
            __cilkrts_save_exception_state(w, f);
#else
            // Move the pending exceptions into the full frame
            // This should always be NULL if this isn't a
            // return with an exception
            CILK_ASSERT(NULL == f->pending_exception);
            f->pending_exception = w->l->pending_exception;
            w->l->pending_exception = NULL;
#endif  // _WIN32

        } END_WITH_FRAME_LOCK(w, f);

        __cilkrts_fence(); /* redundant */

        CILK_ASSERT(parent);

        BEGIN_WITH_FRAME_LOCK(w, parent) {
            finalize_child(w, parent, f);
        } END_WITH_FRAME_LOCK(w, parent);

        f = pop_next_frame(w);
        /* f will be non-null except when the parent frame is owned
           by another worker.
           CILK_ASSERT(f)
        */
        CILK_ASSERT(!w->l->frame);
        if (f) {
            BEGIN_WITH_FRAME_LOCK(w, f) {
                __cilkrts_stack_frame *sf = f->call_stack;
                CILK_ASSERT(sf && !sf->call_parent);
                setup_for_execution(w, f);
            } END_WITH_FRAME_LOCK(w, f);
        }
    } END_WITH_WORKER_LOCK(w);

    STOP_INTERVAL(w, INTERVAL_RETURNING);

#if JFC_DEBUG
    {
        char var;
        fprintf(stderr, "W%d __cilkrts_return stack ~ %p parent stack %p %p\n",
                w->self, (char *)&var, parent->stack_self,
                parent->stack_child);
    }
#endif            
}

static void __cilkrts_unbind_thread()
{
    int stop_cilkscreen = 0;
    global_state_t *g;

    __cilkrts_os_mutex_lock(__cilkrts_global_os_mutex);
    if (cilkg_is_initialized()) {
        __cilkrts_worker *w = __cilkrts_get_tls_worker();
        if (w) {
            g = w->g;

            // If there's only 1 worker, the counts will be stopped in
            // __cilkrts_scheduler
            if (g->P > 1)
            {
                STOP_INTERVAL(w, INTERVAL_WORKING);
                STOP_INTERVAL(w, INTERVAL_IN_SCHEDULER);
            }

            __cilkrts_sysdep_unbind_thread(w);
            __cilkrts_set_tls_worker(0);

            if (w->self == -1) {
                // This worker is an overflow worker.  I.e., it was created on-
                // demand when the global pool ran out of workers.
                destroy_worker(w);
                __cilkrts_free(w);
            } else {
                // This is a normal user worker and needs to be counted by the
                // global state for the purposes of throttling system workers.
                w->l->type = WORKER_FREE;
                __cilkrts_leave_cilk(g);
            }

            stop_cilkscreen = (0 == g->Q);
        }
    }
    __cilkrts_os_mutex_unlock(__cilkrts_global_os_mutex);

    /* Turn off Cilkscreen.  This needs to be done when we are NOT holding the
     * os mutex. */
    if (stop_cilkscreen)
        __cilkrts_cilkscreen_disable_instrumentation();
}

/* special return from the initial frame */

void __cilkrts_c_return_from_initial(__cilkrts_worker *w)
{
    struct cilkred_map *rm;

    /* This is only called on a user thread worker. */
    CILK_ASSERT(w->l->type == WORKER_USER);

    BEGIN_WITH_WORKER_LOCK(w) {
        full_frame *f = w->l->frame;
        CILK_ASSERT(f);
        CILK_ASSERT(f->join_counter == 1);
        w->l->frame = 0;

        CILK_ASSERT(f->stack_self);
        // Save any TBB interop data for the next time this thread enters Cilk
        tbb_interop_save_info_from_stack(f->stack_self);
        sysdep_destroy_user_stack(f->stack_self);

        /* Save reducer map into global_state object */
        CILK_ASSERT(w->reducer_map == f->reducer_map);
        rm = w->reducer_map;
        w->reducer_map = 0;
        f->reducer_map = 0;

        __cilkrts_destroy_full_frame(w, f);

        /* Work is never done. w->g->work_done = 1; __cilkrts_fence(); */
    } END_WITH_WORKER_LOCK(w);

#if JFC_DEBUG
    fprintf(stderr, "Cilk: %u leave\n", w->self);
#endif

    __cilkrts_destroy_reducer_map(w, rm);

    w = NULL;
    __cilkrts_unbind_thread();

    /* Other workers will stop trying to steal if this was the last worker. */

    return;
}


/*
 * __cilkrts_restore_stealing
 *
 * Restore the protected_tail to a previous state, possibly allowing frames
 * to be stolen.  The dekker_protocol has been extended to steal only if
 * head+1 is < protected_tail.
 */

void __cilkrts_restore_stealing(
    __cilkrts_worker *w,
    __cilkrts_stack_frame *volatile *saved_protected_tail)
{
#if JFC_DEBUG > 1
    fprintf(stderr, "Cilk: %u tail <- %p\n", w->self, saved_protected_tail);
#endif
    /* On most x86 this pair of operations would be slightly faster
       as an atomic exchange due to the implicit memory barrier in
       an atomic instruction. */
    w->protected_tail = saved_protected_tail;
    __cilkrts_fence();
}

/*
 * __cilkrts_disallow_stealing
 *
 * Move the protected_tail to NEW_PROTECTED_TAIL, preventing any
 * frames from being stolen.  If NEW_PROTECTED_TAIL is NULL, prevent
 * stealing from the whole queue.  The dekker_protocol has been
 * extended to only steal if head+1 is also < protected_tail.
 */

__cilkrts_stack_frame *volatile *__cilkrts_disallow_stealing(
    __cilkrts_worker *w,
    __cilkrts_stack_frame *volatile *new_protected_tail)
{
    __cilkrts_stack_frame *volatile *saved_protected_tail = w->protected_tail;

    if (!new_protected_tail)
        new_protected_tail = w->l->ltq;

    if (w->protected_tail > new_protected_tail) {
        w->protected_tail = new_protected_tail;
        /* Issue a store-store barrier.  The update to protected_tail
           here must precede the update to tail in the next spawn.
           On x86 this is probably not needed. */
#if defined __GNUC__ && __ICC >= 1200 && !(__MIC__ ||__MIC2__)
        _mm_sfence();
#else
        __cilkrts_fence();
#endif
    }

    return saved_protected_tail;
}

/*************************************************************
  Initialization and startup 
*************************************************************/

__cilkrts_worker *make_worker(global_state_t *g,
                                     int self, __cilkrts_worker *w)
{
    w->self = self;
    w->g = g;

    w->pedigree.rank = 0;    // Initial rank is 0
    w->pedigree.next = NULL;

    w->l = (local_state *)__cilkrts_malloc(sizeof(*w->l));
    __cilkrts_init_stats(&w->l->stats);

    __cilkrts_frame_malloc_per_worker_init(w);

    w->l->worker_magic_0 = WORKER_MAGIC_0;
    __cilkrts_mutex_init(&w->l->lock);
    __cilkrts_mutex_init(&w->l->steal_lock);
    w->l->do_not_steal = 0;
    w->l->frame = 0;
    w->l->ltq = (__cilkrts_stack_frame **)
        __cilkrts_malloc(g->ltqsize * sizeof(*w->l->ltq));
    w->ltq_limit = w->l->ltq + g->ltqsize;
    w->l->rand_seed = 0; /* the scheduler will overwrite this field */
    w->l->next_frame = 0;
    __cilkrts_init_stack_cache(w, &w->l->stack_cache, g->stack_cache_size);

    w->head = w->tail = w->l->ltq;

    w->reducer_map = NULL;

    w->current_stack_frame = NULL;

    w->l->pending_exception = NULL;
    w->l->worker_magic_1 = WORKER_MAGIC_1;

    w->l->post_suspend = 0;
    w->l->suspended_stack = 0;

    w->l->steal_failure_count = 0;

    w->l->team = NULL;
    w->l->last_full_frame = NULL;

    w->l->scheduler_stack = NULL;

    w->l->signal_node = NULL;

    w->saved_protected_tail = NULL;
    /*w->parallelism_disabled = 0;*/

    /* Allow stealing all frames. */
    __cilkrts_restore_stealing(w, w->ltq_limit);

    w->l->type = WORKER_FREE;
    w->l->user_thread_imported = 0;

    // Nothing's been stolen yet
    w->l->work_stolen = 0;

    __cilkrts_init_worker_sysdep(w);

    reset_THE_exception(w);

    return w;
}

void destroy_worker(__cilkrts_worker *w)
{
    CILK_ASSERT (NULL == w->l->pending_exception);

    /* Free any cached stack. */
    __cilkrts_destroy_stack_cache(w, w->g, &w->l->stack_cache);

    if (w->l->scheduler_stack) {
        sysdep_destroy_tiny_stack(w->l->scheduler_stack);
        w->l->scheduler_stack = NULL;
    }
    __cilkrts_destroy_worker_sysdep(w);

    if (w->l->signal_node) {
        CILK_ASSERT(WORKER_SYSTEM == w->l->type);
        signal_node_destroy(w->l->signal_node);
    }

    __cilkrts_free(w->l->ltq);
    __cilkrts_mutex_destroy(0, &w->l->lock);
    __cilkrts_mutex_destroy(0, &w->l->steal_lock);
    __cilkrts_frame_malloc_per_worker_cleanup(w);
    __cilkrts_free(w->l);

    // The caller is responsible for freeing the worker memory
}

/*
 * Make a worker into a system worker.
 */
static void make_worker_system(__cilkrts_worker *w) {
    CILK_ASSERT(WORKER_FREE == w->l->type);
    w->l->type = WORKER_SYSTEM;
    w->l->signal_node = signal_node_create();
}

void __cilkrts_deinit_internal(global_state_t *g)
{
    int i;
    __cilkrts_worker *w;

    // If there's no global state then we're done
    if (NULL == g)
        return;

#ifdef CILK_PROFILE
    __cilkrts_dump_stats_to_stderr(g);
#endif

    w = g->workers[0];
    if (w->l->frame) {
        __cilkrts_destroy_full_frame(w, w->l->frame);
        w->l->frame = 0;
    }

    // Destroy any system dependent global state
    __cilkrts_destroy_global_sysdep(g);

    for (i = 0; i < g->nworkers; ++i)
        destroy_worker(g->workers[i]);

    // Free memory for all worker blocks which were allocated contiguously
    __cilkrts_free(g->workers[0]);

    __cilkrts_free(g->workers);
    __cilkrts_destroy_stack_cache(0, g, &g->stack_cache);
    __cilkrts_frame_malloc_global_cleanup(g);
    cilkg_deinit_global_state();
}

/* install NEWMAP in both W and W->L->FRAME; return the old reducer map */
struct cilkred_map *__cilkrts_xchg_reducer(__cilkrts_worker *w, 
                                           struct cilkred_map *newmap)
{
    struct cilkred_map *oldmap;
    BEGIN_WITH_WORKER_LOCK(w) {
        full_frame *f = w->l->frame;
        BEGIN_WITH_FRAME_LOCK(w, f) {
            oldmap = w->reducer_map;
            CILK_ASSERT(f->reducer_map == oldmap);
            w->reducer_map = newmap;
            f->reducer_map = newmap;
        } END_WITH_FRAME_LOCK(w, f);
    } END_WITH_WORKER_LOCK(w);
    return oldmap;
}

/*
 * Wake the runtime by notifying the system workers that they can steal.  The
 * first user worker into the runtime should call this.
 */
static void wake_runtime(global_state_t *g)
{
    __cilkrts_worker *root;
    if (g->P > 1) {
        // Send a message to the root node.  The message will propagate.
        root = g->workers[0];
        CILK_ASSERT(root->l->signal_node);
        signal_node_msg(root->l->signal_node, 1);
    }
}

/*
 * Put the runtime to sleep.  The last user worker out of the runtime should
 * call this.  Like Dad always said, turn out the lights when nobody's in the
 * room.
 */
static void sleep_runtime(global_state_t *g)
{
    __cilkrts_worker *root;
    if (g->P > 1) {
        // Send a message to the root node.  The message will propagate.
        root = g->workers[0];
        CILK_ASSERT(root->l->signal_node);
        signal_node_msg(root->l->signal_node, 0);
    }
}

/* Called when a user thread joins Cilk.
   Global lock must be held. */
void __cilkrts_enter_cilk(global_state_t *g)
{
    if (g->Q++ == 0) {
        // If this is the first user thread to enter Cilk wake
        // up all the workers.
        wake_runtime(g);
    }
}

/* Called when a user thread leaves Cilk.
   Global lock must be held. */
void __cilkrts_leave_cilk(global_state_t *g)
{
    if (--g->Q == 0) {
        // Put the runtime to sleep.
        sleep_runtime(g);
    }
}

/*
 * worker_runnable
 *
 * Return true if the worker should continue to try to steal.  False, otherwise.
 */

NOINLINE
static enum schedule_t worker_runnable(__cilkrts_worker *w)
{
    global_state_t *g = w->g;

    /* If this worker has something to do, do it.
       Otherwise the work would be lost. */
    if (w->l->next_frame)
        return SCHEDULE_RUN;

    // If Cilk has explicitly (by the user) been told to exit (i.e., by
    // __cilkrts_end_cilk() -> __cilkrts_stop_workers(g)), then return 0.
    if (g->work_done)
        return SCHEDULE_EXIT;

    if (0 == w->self) {
        // This worker is the root node and is the only one that may query the
        // global state to see if there are still any user workers in Cilk.
        if (w->l->steal_failure_count > g->max_steal_failures) {
            if (signal_node_should_wait(w->l->signal_node)) {
                return SCHEDULE_WAIT;
            } else {
                // Reset the steal_failure_count since we have verified that
                // user workers are still in Cilk.
                w->l->steal_failure_count = 0;
            }
        }
    } else if (WORKER_SYSTEM == w->l->type &&
               signal_node_should_wait(w->l->signal_node)) {
        // This worker has been notified by its parent that it should stop
        // trying to steal.
        return SCHEDULE_WAIT;
    }

    return SCHEDULE_RUN;
}

// Initialize the worker structs, but don't start the workers themselves.
static void init_workers(global_state_t *g)
{
    int nworkers = g->nworkers;
    int i;

    struct CILK_ALIGNAS(64) buffered_worker {
        __cilkrts_worker w;
        char buf[64];
    } *workers_memory;

    /* not needed if only one worker */
    __cilkrts_init_stack_cache(0, &g->stack_cache,
                               2*g->nworkers * g->global_stack_cache_size);

    g->workers = (__cilkrts_worker **)
        __cilkrts_malloc(nworkers * sizeof(*g->workers));

    // Allocate 1 block of memory for workers to make life easier for tools
    // like Inspector which run multithreaded and need to know the memory
    // range for all the workers that will be accessed in a user's program
    workers_memory = (struct buffered_worker*)
        __cilkrts_malloc(sizeof(*workers_memory) * nworkers);

    // Notify any tools that care (Cilkscreen and Inspector) that they should
    // ignore memory allocated for the workers
    __cilkrts_cilkscreen_ignore_block(&workers_memory[0],
                                      &workers_memory[nworkers]);

    // Initialize worker structs, including unused worker slots.
    for (i = 0; i < nworkers; ++i)
        g->workers[i] = make_worker(g, i, &workers_memory[i].w);

    // Set the workers in the first P - 1 slots to be system workers.
    // Remaining worker structs already have type == 0.
    for (i = 0; i < g->system_workers; ++i)
        make_worker_system(g->workers[i]);
}

void __cilkrts_init_internal(int start)
{
    int i;
    global_state_t *g = NULL;

    if (! cilkg_is_initialized())
    {
        __cilkrts_os_mutex_lock(__cilkrts_global_os_mutex);
        if (! cilkg_is_initialized())
        {
            // Initialize and retrieve global state
            g = cilkg_init_global_state();

            // Set the scheduler pointer
            g->scheduler = &__cilkrts_scheduler;

            // If we're running under a sequential P-Tool (Cilkscreen or
            // Cilkview) then there's only one worker and we need to tell
            // the tool about the extent of the stack
            if (g->under_ptool)
                __cilkrts_establish_c_stack();

            init_workers(g);

            // Initialize any system dependent global state
            __cilkrts_init_global_sysdep(g);
        }
        __cilkrts_os_mutex_unlock(__cilkrts_global_os_mutex);
    }
    else
        g = cilkg_get_global_state();

    CILK_ASSERT(g);

    if (start && !g->running)
    {
        __cilkrts_os_mutex_lock(__cilkrts_global_os_mutex);
        if (!g->running)
            // Start P - 1 system workers since P includes the first user
            // worker.
            __cilkrts_start_workers(g, g->P - 1);
        __cilkrts_os_mutex_unlock(__cilkrts_global_os_mutex);
    }
}

/*
  Local Variables: **
  c-file-style:"bsd" **
  c-basic-offset:4 **
  indent-tabs-mode:nil **
  End: **
*/
