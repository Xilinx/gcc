/* frame_malloc.c                  -*-C-*-
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

#include "frame_malloc.h"
#include "bug.h"
#include "local_state.h"
#include "cilk_malloc.h"

#include <memory.h>

/* #define USE_MMAP 1 */
#if USE_MMAP
#define __USE_MISC 1
#include <sys/mman.h>
#include <errno.h>
#endif

// Define to fill the stack frame header with the fill character when pushing
// it on a free list.  Note that this should be #ifdef'd out when checked in!

#ifdef _DEBUG
#define HEADER_FILL_CHAR 0xbf
#endif

// HEADER_FILL_CHAR should not be defined when checked in, so put out a warning
// message if this is a release build

#if defined(NDEBUG) && defined (HEADER_FILL_CHAR)
#pragma message ("Warning: HEADER_FILL_CHAR defined for a release build")
#endif

static void allocate_batch(__cilkrts_worker *w, int bucket, size_t size);

#ifndef _WIN32

const unsigned short __cilkrts_bucket_sizes[FRAME_MALLOC_NBUCKETS] =
{
    64, 128, 256, 512, 1024, 2048
};

#define FRAME_MALLOC_BUCKET_TO_SIZE(bucket) __cilkrts_bucket_sizes[bucket]

/* threshold above which we use slow malloc */
#define FRAME_MALLOC_MAX_SIZE 2048

#else // _WIN32

/* Note that this must match the implementation of framesz_to_bucket in
 * asmilator/layout.ml! */
#define FRAME_MALLOC_BUCKET_TO_SIZE(bucket) ((size_t)(64 << (bucket)))

/* threshold above which we use slow malloc */
#define FRAME_MALLOC_MAX_SIZE                                   \
    FRAME_MALLOC_BUCKET_TO_SIZE(FRAME_MALLOC_NBUCKETS - 1)

#endif // _WIN32

/* utility procedures */
static void push(struct free_list **b, struct free_list *p)
{
#ifdef HEADER_FILL_CHAR
    memset (p, HEADER_FILL_CHAR, FRAME_MALLOC_BUCKET_TO_SIZE(0));
#endif
    /* cons! onto free list */
    p->cdr = *b;
    *b = p;
}

static struct free_list *pop(struct free_list **b)
{
    struct free_list *p = *b;
    if (p) 
        *b = p->cdr;
    return p;
}

/*************************************************************
  global allocator:
*************************************************************/
/* request slightly less than 2^K from the OS, which after malloc
   overhead and alignment should end up filling each VM page almost
   completely.  128 is a guess of the total malloc overhead and cache
   line alignment */
#define FRAME_MALLOC_CHUNK (32 * 1024 - 128)

/** Implements linked list of frames */
struct pool_cons {
    char *p;                /**< This element of the list */
    struct pool_cons *cdr;  /**< Remainder of the list */
};

static void extend_global_pool(global_state_t *g)
{
    /* FIXME: memalign to a cache line? */
    struct pool_cons *c = (struct pool_cons *)__cilkrts_malloc(sizeof(*c));
    g->frame_malloc.pool_begin = 
        (char *)__cilkrts_malloc((size_t)FRAME_MALLOC_CHUNK);
    g->frame_malloc.pool_end = 
        g->frame_malloc.pool_begin + FRAME_MALLOC_CHUNK;
    g->frame_malloc.allocated_from_os += FRAME_MALLOC_CHUNK;
    c->p = g->frame_malloc.pool_begin;
    c->cdr = g->frame_malloc.pool_list;
    g->frame_malloc.pool_list = c;
}

/* the size is already canonicalized at this point */
static struct free_list *global_alloc(global_state_t *g, int bucket)
{
    struct free_list *mem;
    size_t size;

    CILK_ASSERT(bucket < FRAME_MALLOC_NBUCKETS);
    size = FRAME_MALLOC_BUCKET_TO_SIZE(bucket);
    g->frame_malloc.allocated_from_global_pool += size;

    if (!(mem = pop(&g->frame_malloc.global_free_list[bucket]))) {

        CILK_ASSERT(g->frame_malloc.pool_begin <= g->frame_malloc.pool_end);
        if (g->frame_malloc.pool_begin + size > g->frame_malloc.pool_end) {
            /* We waste the fragment of pool. */
            g->frame_malloc.wasted +=
                g->frame_malloc.pool_end - g->frame_malloc.pool_begin;
            extend_global_pool(g);
        }
        mem = (struct free_list *)g->frame_malloc.pool_begin;
        g->frame_malloc.pool_begin += size;
    }

    return mem;
}

static void global_free(global_state_t *g, void *mem, int bucket)
{
    size_t size;

    CILK_ASSERT(bucket < FRAME_MALLOC_NBUCKETS);
    size = FRAME_MALLOC_BUCKET_TO_SIZE(bucket);
    g->frame_malloc.allocated_from_global_pool -= size;

    push(&g->frame_malloc.global_free_list[bucket], mem);
}

void __cilkrts_frame_malloc_global_init(global_state_t *g)
{
    int i;

    __cilkrts_mutex_init(&g->frame_malloc.lock); 
    g->frame_malloc.check_for_leaks = 1;
    g->frame_malloc.pool_list = 0;
    g->frame_malloc.pool_begin = 0;
    g->frame_malloc.pool_end = 0;
    g->frame_malloc.batch_size = 8000;
    g->frame_malloc.potential_limit = 4 * g->frame_malloc.batch_size;
    g->frame_malloc.allocated_from_os = 0;
    g->frame_malloc.allocated_from_global_pool = 0;
    g->frame_malloc.wasted = 0;
    g->frame_malloc.in_buckets_at_end_of_execution = 0;
    for (i = 0; i < FRAME_MALLOC_NBUCKETS; ++i) 
        g->frame_malloc.global_free_list[i] = 0;
}

void __cilkrts_frame_malloc_global_cleanup(global_state_t *g)
{
    struct pool_cons *c;

    while ((c = g->frame_malloc.pool_list)) {
        g->frame_malloc.pool_list = c->cdr;
        __cilkrts_free(c->p);
        __cilkrts_free(c);
    }

    __cilkrts_mutex_destroy(0, &g->frame_malloc.lock);

    if (g->frame_malloc.check_for_leaks
        && (g->frame_malloc.allocated_from_global_pool !=
            g->frame_malloc.in_buckets_at_end_of_execution))
    {
        __cilkrts_bug("\n"
                      "---------------------------" "\n"
                      "  MEMORY LEAK DETECTED!!!  " "\n"
                      "---------------------------" "\n"
                      "\n"
            );
    }
}

/*************************************************************
  per-worker allocator
*************************************************************/
/* allocate a batch of frames of size SIZE from the global pool and
   store them in the worker's free list */
static void allocate_batch(__cilkrts_worker *w, int bucket, size_t size)
{
    global_state_t *g = w->g;

    __cilkrts_mutex_lock(w, &g->frame_malloc.lock); {
#if USE_MMAP
        char *p = mmap(0, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED)
            __cilkrts_bug("mmap failed %d", errno);
        assert(size < 4096);
        assert(p != MAP_FAILED);
        mprotect(p, 4096, PROT_NONE);
        mprotect(p + 8192, 4096, PROT_NONE);
        w->l->bucket_potential[bucket] += size;
        push(&w->l->free_list[bucket], (struct free_list *)(p + 8192 - size));
#else
        size_t bytes_allocated = 0;
        do {
            w->l->bucket_potential[bucket] += size;
            bytes_allocated += size;
            push(&w->l->free_list[bucket], global_alloc(g, bucket));
        } while (bytes_allocated < g->frame_malloc.batch_size);
#endif
    } __cilkrts_mutex_unlock(w, &g->frame_malloc.lock);
}

static void gc_bucket(__cilkrts_worker *w, int bucket, size_t size)
{
    struct free_list *p, *q;
    global_state_t *g = w->g;
    size_t pot = w->l->bucket_potential[bucket];
    size_t newpot;

    /* Keep up to POT/2 elements in the free list.  The cost of
       counting up to POT/2 is amortized against POT. */
    newpot = 0;
    for (newpot = 0, p = w->l->free_list[bucket]; p && 2 * newpot < pot; 
         p = p->cdr, newpot += size)
        ;
    w->l->bucket_potential[bucket] = newpot;

    if (p) {
        /* free the rest of the list.  The cost of grabbing the lock
           is amortized against POT/2; the cost of traversing the rest
           of the list is amortized against the free operation that
           puts the element on the list. */
        __cilkrts_mutex_lock(w, &g->frame_malloc.lock); {
            while ((q = pop(&p->cdr)))
#if USE_MMAP
                munmap((char *)q + size - 8192, 12288);
#else
                global_free(g, q, bucket);
#endif
        } __cilkrts_mutex_unlock(w, &g->frame_malloc.lock);
    }
}

static int bucket_of_size(size_t size)
{
    int i;

    for (i = 0; i < FRAME_MALLOC_NBUCKETS; ++i)
        if (size <= FRAME_MALLOC_BUCKET_TO_SIZE(i))
            return i;

    CILK_ASSERT(0 /* can't happen */);
    return -1;
}

size_t __cilkrts_frame_malloc_roundup(size_t size)
{
    if (size > FRAME_MALLOC_MAX_SIZE) {
        /* nothing, leave it alone */
    } else {
        int bucket = bucket_of_size(size);
        size = FRAME_MALLOC_BUCKET_TO_SIZE(bucket);
    }
    return size;
}

size_t __cilkrts_size_of_bucket(int bucket)
{
    CILK_ASSERT(bucket >= 0 && bucket < FRAME_MALLOC_NBUCKETS);
    return FRAME_MALLOC_BUCKET_TO_SIZE(bucket);
}

void *__cilkrts_frame_malloc(__cilkrts_worker *w, size_t size)
{
    int bucket;
    void *mem;

    /* if too large, or if no worker, fall back to __cilkrts_malloc()  */
    if (!w || size > FRAME_MALLOC_MAX_SIZE) {
        NOTE_INTERVAL(w, INTERVAL_FRAME_ALLOC_LARGE);
        return __cilkrts_malloc(size);
    }

    START_INTERVAL(w, INTERVAL_FRAME_ALLOC); {
        bucket = bucket_of_size(size);
        size = FRAME_MALLOC_BUCKET_TO_SIZE(bucket);

        while (!(mem = pop(&w->l->free_list[bucket]))) {
            /* get a batch of frames from the global pool */
            START_INTERVAL(w, INTERVAL_FRAME_ALLOC_GLOBAL) {
                allocate_batch(w, bucket, size);
            } STOP_INTERVAL(w, INTERVAL_FRAME_ALLOC_GLOBAL);
        }
    } STOP_INTERVAL(w, INTERVAL_FRAME_ALLOC);

    return mem;
}

void __cilkrts_frame_free(__cilkrts_worker *w, void *p0, size_t size)
{
    int bucket;
    struct free_list *p = (struct free_list *)p0;

    /* if too large, or if no worker, fall back to __cilkrts_free()  */
    if (!w || size > FRAME_MALLOC_MAX_SIZE) {
        NOTE_INTERVAL(w, INTERVAL_FRAME_FREE_LARGE);
        __cilkrts_free(p);
        return;
    }

#if CILK_LIB_DEBUG
    *(volatile long *)w;
#endif

    START_INTERVAL(w, INTERVAL_FRAME_FREE); {
        bucket = bucket_of_size(size);
        size = FRAME_MALLOC_BUCKET_TO_SIZE(bucket);
        w->l->bucket_potential[bucket] += size;
        push(&w->l->free_list[bucket], p);
        if (w->l->bucket_potential[bucket] >
            w->g->frame_malloc.potential_limit) {
            START_INTERVAL(w, INTERVAL_FRAME_FREE_GLOBAL) {
                gc_bucket(w, bucket, size);
            } STOP_INTERVAL(w, INTERVAL_FRAME_FREE_GLOBAL);
        }
    } STOP_INTERVAL(w, INTERVAL_FRAME_FREE);
}

void __cilkrts_frame_malloc_per_worker_init(__cilkrts_worker *w)
{
    int i;
    local_state *l = w->l;

    for (i = 0; i < FRAME_MALLOC_NBUCKETS; ++i) {
        l->free_list[i] = 0;
        l->bucket_potential[i] = 0;
    }
}

static void collect_buckets(__cilkrts_worker *w, struct free_list **buckets)
{
    global_state_t *g = w->g;
    int i;
    for (i = 0; i < FRAME_MALLOC_NBUCKETS; ++i) {
        struct free_list **b = buckets + i;
        size_t size = FRAME_MALLOC_BUCKET_TO_SIZE(i);
        while (pop(b)) 
            g->frame_malloc.in_buckets_at_end_of_execution += size;
    }
}

void __cilkrts_frame_malloc_per_worker_cleanup(__cilkrts_worker *w)
{
    if (w->g->frame_malloc.check_for_leaks) 
        collect_buckets(w, w->l->free_list);
}


/*
  Local Variables: **
  c-file-style:"bsd" **
  c-basic-offset:4 **
  indent-tabs-mode:nil **
  End: **
*/
