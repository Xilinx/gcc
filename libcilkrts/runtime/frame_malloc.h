/* frame_malloc.h                  -*-C++-*-
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
 * @file frame_malloc.h
 *
 * @brief The frame allocation routines manage memory in a per-worker pool.
 *
 * The name "frame malloc" refers to an earlier implementation of Cilk which
 * allocated frames from the heap using this allocator.
 */

#ifndef INCLUDED_FRAME_MALLOC_DOT_H
#define INCLUDED_FRAME_MALLOC_DOT_H

#include "worker_mutex.h"
#include "rts-common.h"
#include <internal/abi.h>  // __cilkrts_worker

#ifdef __cplusplus
#   include <cstddef>
#else
#   include <stddef.h>
#endif

__CILKRTS_BEGIN_EXTERN_C

/**
 * Number of buckets.  Gives us buckets to hold  64, 128, 256, 512, 1024
 * and 2048 bytes
 */
#define FRAME_MALLOC_NBUCKETS 6

/** Layout of frames when unallocated */
struct free_list {
     /** Pointer to next free frame */
     struct free_list *cdr;
};

/** per-worker memory cache */
struct __cilkrts_frame_cache
{
    /** Mutex to serialize access */
    struct mutex lock;

    /** Linked list of frames */
    struct pool_cons *pool_list;

    /** Low bound of memory in pool */
    char *pool_begin;

    /** High bound of memory in pool */
    char *pool_end;

    /** Global free-list buckets */
    struct free_list *global_free_list[FRAME_MALLOC_NBUCKETS];

    /**
     * How many bytes to obtain at once from the global pool
     * (approximately)
     */
    size_t batch_size;

    /** Garbage-collect a bucket when its potential exceeds the limit */
    size_t potential_limit;

    /** If TRUE, check for memory leaks at the end of execution */
    int check_for_leaks;

    /** Bytes of memory allocated from the OS by the global cache */
    size_t allocated_from_os;

    /** Tracks memory allocated by a chunk that isn't a full bucket size */
    size_t wasted;

    /** Bytes of memory allocated from the global cache */
    size_t allocated_from_global_pool;

    /** Bytes of memory at end of execution */
    size_t in_buckets_at_end_of_execution;
};

/**
 * Allocate memory from the per-worker pool. If the size is too large, or
 * if we're given a NULL worker, the memory is allocated using
 * __cilkrts_malloc().
 *
 * @param w The worker to allocate the memory from.
 * @param size The number of bytes to allocate.
 *
 * @return pointer to allocated memory block.
 */
COMMON_PORTABLE
void *__cilkrts_frame_malloc(__cilkrts_worker *w,
                             size_t size) cilk_nothrow;

/**
 * Return memory to the per-worker pool. If the size is too large, or
 * if we're given a NULL worker, the memory is freed using
 * __cilkrts_free().
 *
 * @param w The worker to allocate the memory from.
 * @param p The memory block to be released.
 * @param size The size of the block, in bytes.
 */
COMMON_PORTABLE
void __cilkrts_frame_free(__cilkrts_worker *w,
                          void*  p,
                          size_t size) cilk_nothrow;

/**
 * Destroy the global cache stored in the global state, freeing all memory
 * to the global heap.  Checks whether any memory has been allocated but
 * not freed.
 *
 * @param g The global state.
 */
COMMON_PORTABLE
void __cilkrts_frame_malloc_global_cleanup(global_state_t *g);

/**
 * Initialize a worker's memory cache.  Initially it is empty.
 *
 * @param w The worker who's memory cache is to be initialized.
 */
COMMON_PORTABLE
void __cilkrts_frame_malloc_per_worker_init(__cilkrts_worker *w);

/**
 * If check_for_leaks is set in the global state's memory cache, free any
 * memory in the worker's memory cache.
 *
 * If check_for_leask is not set, nothing happens.
 *
 * @param w The worker who's memory cache is to be cleaned up.
 */
COMMON_PORTABLE
void __cilkrts_frame_malloc_per_worker_cleanup(__cilkrts_worker *w);

/**
 * Round a number of bytes to the size of the smallest bucket that will
 * hold it.  If the size is bigger than the largest bucket, the value is
 * unchanged.
 *
 * @param size Number of bytes to be rounded up to the nearest bucket size.
 *
 * @return The size of the smallest bucket that will hold the specified bytes.
 */
COMMON_PORTABLE
size_t __cilkrts_frame_malloc_roundup(size_t size) cilk_nothrow;

/**
 * Return the number of bytes that can fit into a bucket.
 *
 * Preconditions:
 *  - The index must be in the range 0 - FRAME_MALLOC_NBUCKETS
 *
 * @param bucket Index of the bucket to be sized.
 */
COMMON_PORTABLE
size_t __cilkrts_size_of_bucket(int bucket) cilk_nothrow;

/**
 * Initialize the global memory cache.
 *
 * @param g The global state.
 */
COMMON_PORTABLE
void __cilkrts_frame_malloc_global_init(global_state_t *g);

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_FRAME_MALLOC_DOT_H)
