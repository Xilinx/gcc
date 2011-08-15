/* stats.c                  -*-C-*-
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

#include "stats.h"
#include "bug.h"
#include "os.h"
#include "local_state.h"

#include <stdio.h>

#define INVALID_START (0ULL - 1ULL)

#ifdef CILK_PROFILE
/* MSVC does not support designated initializers, grrrr... */
static const char *names[] = {
    /*[INTERVAL_IN_SCHEDULER]*/                 "in scheduler",
    /*[INTERVAL_WORKING]*/                      "  of which: working",
    /*[INTERVAL_STEALING]*/                     "  of which: stealing",
    /*[INTERVAL_USER_WAITING]*/                 "  of which: user thread wait",
    /*[INTERVAL_SYSTEM_WAITING]*/               "  of which: system thread wait",
    /*[INTERVAL_STEAL_SUCCESS]*/                "steal success: detach",
    /*[INTERVAL_STEAL_FAIL_EMPTYQ]*/            "steal fail: empty queue",
    /*[INTERVAL_STEAL_FAIL_LOCK]*/              "steal fail: victim locked",
    /*[INTERVAL_STEAL_FAIL_USER_WORKER]*/       "steal fail: user worker",
    /*[INTERVAL_STEAL_FAIL_DEKKER]*/            "steal fail: dekker",
    /*[INTERVAL_SYNC_CHECK]*/                   "sync check",
    /*[INTERVAL_THE_EXCEPTION_CHECK]*/          "THE exception check",
    /*[INTERVAL_THE_EXCEPTION_CHECK_USELESS]*/  "  of which: useless",
    /*[INTERVAL_RETURNING]*/                    "returning",
    /*[INTERVAL_FINALIZE_CHILD]*/               "finalize child",
    /*[INTERVAL_SPLICE_REDUCERS]*/              "  of which: splice_reducers",
    /*[INTERVAL_SPLICE_EXCEPTIONS]*/            "  of which: splice_exceptions",
    /*[INTERVAL_SPLICE_STACKS]*/                "  of which: splice_stacks",
    /*[INTERVAL_PROVABLY_GOOD_STEAL]*/          "provably good steal",
    /*[INTERVAL_UNCONDITIONAL_STEAL]*/          "unconditional steal",
    /*[INTERVAL_ALLOC_FULL_FRAME]*/             "alloc full frame",
    /*[INTERVAL_FRAME_ALLOC_LARGE]*/            "large frame alloc",
    /*[INTERVAL_FRAME_ALLOC]*/                  "small frame alloc",
    /*[INTERVAL_FRAME_ALLOC_GLOBAL]*/           "  of which: to global pool",
    /*[INTERVAL_FRAME_FREE_LARGE]*/             "large frame free",
    /*[INTERVAL_FRAME_FREE]*/                   "small frame free",
    /*[INTERVAL_FRAME_FREE_GLOBAL]*/            "  of which: to global pool",
    /*[INTERVAL_MUTEX_LOCK]*/                   "mutex lock",
    /*[INTERVAL_MUTEX_LOCK_SPINNING]*/          "  spinning",
    /*[INTERVAL_MUTEX_LOCK_YIELDING]*/          "  yielding",
    /*[INTERVAL_MUTEX_TRYLOCK]*/                "mutex trylock",
    /*[INTERVAL_ALLOC_STACK]*/                  "alloc stack",
    /*[INTERVAL_FREE_STACK]*/                   "free stack",
};
#endif

void __cilkrts_init_stats(statistics *s)
{
    int i;

    for (i = 0; i < INTERVAL_N; ++i) {
        s->start[i] = INVALID_START;
        s->accum[i] = 0;
        s->count[i] = 0;
    }

    s->stack_hwm = 0;
}

void __cilkrts_accum_stats(statistics *to, statistics *from)
{
    int i;

    for (i = 0; i < INTERVAL_N; ++i) { 
        to->accum[i] += from->accum[i];
        to->count[i] += from->count[i];
        from->accum[i] = 0;
        from->count[i] = 0;
    }

    if (from->stack_hwm > to->stack_hwm)
        to->stack_hwm = from->stack_hwm;
    from->stack_hwm = 0;
}

void __cilkrts_note_interval(__cilkrts_worker *w, enum interval i)
{
    if (w) {
        statistics *s = &w->l->stats;
        CILK_ASSERT(s->start[i] == INVALID_START);
        s->count[i]++;
    }
}

void __cilkrts_start_interval(__cilkrts_worker *w, enum interval i)
{
    if (w) {
        statistics *s = &w->l->stats;
        CILK_ASSERT(s->start[i] == INVALID_START);
        s->start[i] = __cilkrts_getticks();
        s->count[i]++;
    }
}

void __cilkrts_stop_interval(__cilkrts_worker *w, enum interval i)
{
    if (w) {
        statistics *s = &w->l->stats;
        CILK_ASSERT(s->start[i] != INVALID_START);
        s->accum[i] += __cilkrts_getticks() - s->start[i];
        s->start[i] = INVALID_START;
    }
}

#ifdef CILK_PROFILE
static void dump_stats_to_file(FILE *f, statistics *s)
{
    int i;
    fprintf(f, "\nCILK++ RUNTIME SYSTEM STATISTICS:\n\n");

    fprintf(f,
            "  %-32s: %15s %10s %12s %10s\n",
            "event",
            "count",
            "ticks",
            "ticks/count",
            "%total"
        );
    for (i = 0; i < INTERVAL_N; ++i) {
        fprintf(f, "  %-32s: %15llu", names[i], s->count[i]);
        if (s->accum[i]) {
            fprintf(f, " %10.3g %12.3g %10.2f", 
                    (double)s->accum[i],
                    (double)s->accum[i] / (double)s->count[i],
                    100.0 * (double)s->accum[i] / 
                    (double)s->accum[INTERVAL_IN_SCHEDULER]);
        }
        fprintf(f, "\n");
    }
}
#endif

/* End stats.c */
