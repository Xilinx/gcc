/* reducer_impl.cpp                  -*-C++-*-
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

/*
 * reducer_impl.cpp
 *
 * Copyright (c) 2007, 20011 Cilk Arts, Inc. 55 Cambridge Street, Burlington, MA
 * 01803.  Patents pending.  All rights reserved. Use, duplication or
 * disclosure is subject to the terms and conditions of of the Cilk Arts
 * Public License, version 1.0 (2008), or an applicable license agreement
 * with Cilk Arts, Inc. Use, duplication or disclosure by the U.S.
 * Government is further subject to restrictions set forth in FAR
 * 52.227-14, FAR 52.227-19(c)(1.2) (June 1987) or DFARS
 * 252.227-7013(c)(1)(ii) (Oct 1988), as applicable.
 *
 * Support for reducers
 */

// ICL: Don't complain about conversion from pointer to same-sized integral type
// in hashfun.  That's why we're using size_t
#ifdef _WIN32
#   pragma warning(disable: 1684)
#endif

#include "reducer_impl.h"
#include "scheduler.h"
#include "bug.h"
#include "global_state.h"
#include "frame_malloc.h"

#include "cilk/hyperobject_base.h"
#include "cilktools/cilkscreen.h"
#include "internal/abi.h"

#define DBG if(0) // if(1) enables some internal checks

#if JFC_DEBUG
volatile int reducer_map_count = 1;
#endif

/// Helper class to disable and re-enable Cilkscreen
struct DisableCilkscreen
{
    DisableCilkscreen () { __cilkscreen_disable_checking(); }
    ~DisableCilkscreen () { __cilkscreen_enable_checking(); }
};

/// Helper class to enable and re-disable Cilkscreen
struct EnableCilkscreen
{
    EnableCilkscreen () { __cilkscreen_enable_checking(); }
    ~EnableCilkscreen () { __cilkscreen_disable_checking(); }
};

/** Element for a hyperobject */
struct elem {
    /// Key for this strand?
    __cilkrts_hyperobject_base *key;

    /// A view for the hyperobject
    void *val;

    /// Destructor for an instance of this hyperobject
    void destroy();
};

/** Bucket containing at most NMAX elements */
struct bucket {
    /// Size of the array of elements for this bucket
    size_t nmax;

    /**
     * We use the ``struct hack'' to allocate an array of variable
     * dimension at the end of the struct.  However, we allocate a
     * total of NMAX+1 elements instead of NMAX.  The last one always
     * has key == 0, which we use as a termination criterion
     */
    elem el[1];
};

/**
 * Class that implements the map for reducers so we can find the
 * view for a strand.
 */
struct cilkred_map {
    /** Handy pointer to the global state */
    global_state_t *g;

    /** Number of elements in table */
    size_t nelem;

    /** Number of buckets */
    size_t nbuckets;

    /** Array of pointers to buckets */
    bucket **buckets;

    /** Set true if merging (for debugging purposes) */
    bool merging;

    /** Set true for leftmost reducer map */
    bool is_leftmost;

    /** Return element mapped to 'key' or null if not found. */
    elem *lookup(__cilkrts_hyperobject_base *key);

    /**
     * Insert key/value element into hash map without rehashing. Does not
     * check for duplicate key.
     */
    elem *insert_no_rehash(__cilkrts_worker *, __cilkrts_hyperobject_base *key, void *value);

    /**
     * Insert key/value element into hash map, rehashing if necessary. Does not
     * check for duplicate key.
     */
    inline elem *rehash_and_insert(__cilkrts_worker *, 
                                   __cilkrts_hyperobject_base *key, void *value);

    /** Grow bucket by one element, reallocating bucket if necessary */
    static elem *grow(__cilkrts_worker *w, bucket **bp);

    /** Rehash a worker's reducer map */
    void rehash(__cilkrts_worker *);

    /**
     * Returns true if a rehash is needed due to the number of elements that
     * have been inserted.
     */
    inline bool need_rehash_p() const;

    /** Allocate and initialize the buckets */
    void make_buckets(__cilkrts_worker *w, size_t nbuckets);

    /**
     * Specify behavior when the same key is present in both maps passed
     * into merge().
     */
    enum merge_kind
    {
        MERGE_UNORDERED, ///< Assertion fails
        MERGE_INTO_LEFT, ///< Merges the argument from the right into the left
        MERGE_INTO_RIGHT ///< Merges the argument from the left into the right
    };

    /**
     * Merge another reducer map into this one, destroying the other map in
     * the process.
     */
    void merge(__cilkrts_worker *, cilkred_map *, __cilkrts_worker *, enum merge_kind);

    /** check consistency of a reducer map */
    void check(bool allow_null_val);

    /** Move a reducer map from one memory allocation context to another. */
    static cilkred_map *move(cilkred_map *r, __cilkrts_worker *from, __cilkrts_worker *to);

    /** Test whether the cilkred_map is empty */
    bool is_empty() { return nelem == 0; }
};

static size_t sizeof_bucket(size_t nmax)
{
    bucket *b = 0;
    return (sizeof(*b) + nmax * sizeof(b->el[0]));
}

static bucket *alloc_bucket(__cilkrts_worker *w, size_t nmax)
{
    bucket *b = (bucket *)
        __cilkrts_frame_malloc(w, sizeof_bucket(nmax));
    b->nmax = nmax;
    return b;
}

static void free_bucket(__cilkrts_worker *w, bucket **bp)
{
    bucket *b = *bp;
    if (b) {
        __cilkrts_frame_free(w, b, sizeof_bucket(b->nmax));
        *bp = 0;
    }
}

/* round up nmax to fill a memory allocator block completely */
static size_t roundup(size_t nmax)
{
    size_t sz = sizeof_bucket(nmax);

    /* round up size to a full malloc block */
    sz = __cilkrts_frame_malloc_roundup(sz);

    /* invert sizeof_bucket() */
    nmax = ((sz - sizeof(bucket)) / sizeof(elem));
     
    return nmax;
}

static bool is_power_of_2(size_t n)
{
    return (n & (n - 1)) == 0;
}

void cilkred_map::make_buckets(__cilkrts_worker *w, 
                               size_t new_nbuckets)
{     
    nbuckets = new_nbuckets;

    CILK_ASSERT(is_power_of_2(nbuckets));
#if defined __GNUC__ && defined __ICC 
    /* bug workaround -- suppress calls to _intel_fast_memset */
    bucket *volatile*new_buckets = (bucket *volatile*)
#else
    bucket **new_buckets = (bucket **)
#endif
        __cilkrts_frame_malloc(w, nbuckets * sizeof(*(buckets)));

    for (size_t i = 0; i < new_nbuckets; ++i)
        new_buckets[i] = 0;
#if defined __GNUC__ && defined __ICC 
    buckets = (bucket **)new_buckets;
#else
    buckets = new_buckets;
#endif
    nelem = 0;
}

static void free_buckets(__cilkrts_worker *w, 
                         bucket **buckets,
                         size_t nbuckets)
{
    size_t i;
    for (i = 0; i < nbuckets; ++i)
        free_bucket(w, buckets + i);
    __cilkrts_frame_free(w, buckets, nbuckets * sizeof(*buckets));
}

static size_t minsz(size_t nelem)
{
    return 1U + nelem + nelem / 8U;
}

static size_t nextsz(size_t nelem)
{
    return 2 * nelem;
}

bool cilkred_map::need_rehash_p() const
{
    return minsz(nelem) > nbuckets;
}

static inline size_t hashfun(const cilkred_map *h, 
                             __cilkrts_hyperobject_base *key)
{
    size_t k = (size_t) key;

    k ^= k >> 21;
    k ^= k >> 8;
    k ^= k >> 3;

    return k & (h->nbuckets - 1);
}

static inline void* get_leftmost_view(__cilkrts_hyperobject_base *key)
{
    return reinterpret_cast<char*>(key) + key->__view_offset;
}

/* debugging support: check consistency of a reducer map */
void cilkred_map::check(bool allow_null_val)
{
    size_t count = 0;

    CILK_ASSERT(buckets);
    for (size_t i = 0; i < nbuckets; ++i) {
        bucket *b = buckets[i];
        if (b) 
            for (elem *el = b->el; el->key; ++el) {
                CILK_ASSERT(allow_null_val || el->val);
                ++count;
            }
    }
    CILK_ASSERT(nelem == count);
    /*global_reducer_map::check();*/
}             

/* grow bucket by one element, reallocating bucket if necessary */
elem *cilkred_map::grow(__cilkrts_worker *w, 
                        bucket          **bp)
{
    size_t i, nmax, nnmax;
    bucket *b, *nb;

    b = *bp;
    if (b) {
        nmax = b->nmax;
        /* find empty element if any */
        for (i = 0; i < nmax; ++i) 
            if (b->el[i].key == 0) 
                return &(b->el[i]);
        /* do not use the last one even if empty */
    } else {
        nmax = 0;
    }
     
    /* allocate a new bucket */
    nnmax = roundup(2 * nmax);
    nb = alloc_bucket(w, nnmax);

    /* copy old bucket into new */
    for (i = 0; i < nmax; ++i)
        nb->el[i] = b->el[i];
     
    free_bucket(w, bp); *bp = nb;

    /* zero out extra elements */
    for (; i < nnmax; ++i)
        nb->el[i].key = 0;

    /* zero out the last one */
    nb->el[i].key = 0;
  
    return &(nb->el[nmax]);
}

elem *cilkred_map::insert_no_rehash(__cilkrts_worker *w, 
                                    __cilkrts_hyperobject_base *key, void *val)
{
    CILK_ASSERT((w == 0 && g == 0) || w->g == g);
    CILK_ASSERT(key != 0);
    CILK_ASSERT(val != 0);

    elem *el = grow(w, &(buckets[hashfun(this, key)]));
    el->key = key;
    el->val = val;
    ++nelem;

    return el;
}

void cilkred_map::rehash(__cilkrts_worker *w)
{
    CILK_ASSERT((w == 0 && g == 0) || w->g == g);

    size_t onbuckets = nbuckets;
    size_t onelem = nelem;
    bucket **obuckets = buckets;
    size_t i;
    bucket *b;

    make_buckets(w, nextsz(nbuckets));
     
    for (i = 0; i < onbuckets; ++i) {
        b = obuckets[i];
        if (b) {
            elem *oel;
            for (oel = b->el; oel->key; ++oel) 
                insert_no_rehash(w, oel->key, oel->val);
        }
    }

    CILK_ASSERT(nelem == onelem);

    free_buckets(w, obuckets, onbuckets);
}

elem *cilkred_map::rehash_and_insert(__cilkrts_worker *w, 
                                     __cilkrts_hyperobject_base *key, void *val)
{
    if (need_rehash_p())
        rehash(w);

    return insert_no_rehash(w, key, val);
}


elem *cilkred_map::lookup(__cilkrts_hyperobject_base *key)
{
    bucket *b = buckets[hashfun(this, key)];

    if (b) {
        elem *el;
        for (el = b->el; el->key; ++el) {
            if (el->key == key) {
                CILK_ASSERT(el->val);
                return el;
            }
        }
    }

    return 0;
}

void elem::destroy()
{
    const cilk_c_monoid &m = key->__c_monoid;
    // Call destroy_fn and deallocate_fn on all but the leftmost value
    if (val != get_leftmost_view(key))
    {
        m.destroy_fn(key, val);
        m.deallocate_fn(key, val);
    }
    val = 0;
}

/* remove the reducer from the current reducer map.  If the reducer
   exists in maps other than the current one, the behavior is
   undefined. */
extern "C"
CILK_EXPORT void __CILKRTS_STRAND_STALE(
    __cilkrts_hyper_destroy(__cilkrts_hyperobject_base *key))
{
    // Disable Cilkscreen for the duration of this call.  The destructor for
    // this class will re-enable Cilkscreen when the method returns.  This
    // will prevent Cilkscreen from reporting apparent races in reducers
    DisableCilkscreen x;

    __cilkrts_worker* w = __cilkrts_get_tls_worker();
    if (! w) {
        // If no worker, then Cilk is not running and there is no reducer
        // map.  Do nothing.  The reducer's destructor will take care of
        // destroying the leftmost view.
        return;
    }

    cilkred_map* h = w->reducer_map;
    CILK_ASSERT(h);

    if (h->merging)
        __cilkrts_bug("User error: hyperobject used by another hyperobject");
    elem *el = h->lookup(key);
    if (el) {
        /* found. */

        /* Destroy view and remove element from bucket. */
        el->destroy();

        /* Shift all subsequent elements.  Do not bother
           shrinking the bucket */
        do {
            el[0] = el[1];
            ++el;
        } while (el->key);

        --h->nelem;
    }
}

extern "C"
CILK_EXPORT
void __cilkrts_hyper_create(__cilkrts_hyperobject_base *key)
{
    // This function registers the specified key in the current reducer map
    // and registers the initial value of key as the leftmost view of the
    // reducer.
    __cilkrts_worker *w = __cilkrts_get_tls_worker();
    if (! w) {
        // If there is no worker, then there is nothing to do: The iniitial
        // value will automatically be used as the left-most view when we
        // enter Cilk.
        return;
    }

    // Disable Cilkscreen for the duration of this call.  The destructor for
    // this class will re-enable Cilkscreen when the method returns.  This
    // will prevent Cilkscreen from reporting apparent races in reducers
    DisableCilkscreen x;

    void* val = get_leftmost_view(key);
    cilkred_map *h = w->reducer_map;

    /* Must not exist. */
    CILK_ASSERT(h->lookup(key) == NULL);
    if (h->merging)
        __cilkrts_bug("User error: hyperobject used by another hyperobject");

    (void) h->rehash_and_insert(w, key, val);
}

extern "C"
CILK_EXPORT void* __CILKRTS_STRAND_PURE(
    __cilkrts_hyper_lookup(__cilkrts_hyperobject_base *key))
{
    __cilkrts_worker* w = __cilkrts_get_tls_worker_fast();
    if (! w)
        return get_leftmost_view(key);

    // Disable Cilkscreen for the duration of this call.  This will
    // prevent Cilkscreen from reporting apparent races in reducers
    DisableCilkscreen dguard;

    if (__builtin_expect(w->g->force_reduce, 0))
        __cilkrts_promote_own_deque(w);
    cilkred_map* h = w->reducer_map;

    if (h->merging)
        __cilkrts_bug("User error: hyperobject used by another hyperobject");
    elem* el = h->lookup(key);
    if (! el) {
        /* lookup failed; insert a new default element */
        void *rep;

        {
            /* re-enable cilkscreen while calling the constructor */
            EnableCilkscreen eguard;
            if (h->is_leftmost)
                rep = get_leftmost_view(key);
            else
            {
                rep = key->__c_monoid.allocate_fn(key, key->__view_size);
                // TBD: Handle exception on identity function
                key->__c_monoid.identity_fn(key, rep);
            }
        }

        el = h->rehash_and_insert(w, key, rep);
    }

    return el->val;
}

extern "C" CILK_EXPORT
void* __cilkrts_hyperobject_alloc(void* ignore, std::size_t bytes)
{
    return std::malloc(bytes);
}

extern "C" CILK_EXPORT
void __cilkrts_hyperobject_dealloc(void* ignore, void* view)
{
    std::free(view);
}

/* No-op destroy function */
extern "C" CILK_EXPORT
void __cilkrts_hyperobject_noop_destroy(void* ignore, void* ignore2)
{
}

#if JFC_DEBUG
#include <stdio.h>
#endif

cilkred_map *__cilkrts_make_reducer_map(__cilkrts_worker *w)
{
    CILK_ASSERT(w);

    cilkred_map *h;
    size_t nbuckets = 1; /* default value */

    h = (cilkred_map *)__cilkrts_frame_malloc(w, sizeof(*h));

    h->g = w ? w->g : 0;
    h->make_buckets(w, nbuckets);
    h->merging = false;
    h->is_leftmost = false;

#if JFC_DEBUG
    __sync_add_and_fetch(&reducer_map_count, 1);
#if JFC_DEBUG > 1
    if (w)
      fprintf(stderr, "worker %u reducer map = %p\n", w->self, h);
    else
      fprintf(stderr, "global reducer map = %p\n", h);
#endif
#endif

    return h;
}

/* Destroy a reducer map.  The map must have been allocated
   from the worker's global context and should have been
   allocated from the same worker. */
void __cilkrts_destroy_reducer_map(__cilkrts_worker *w, cilkred_map *h)
{
    CILK_ASSERT((w == 0 && h->g == 0) || w->g == h->g);

    /* the reducer map is allowed to contain el->val == NULL here (and
       only here).  We set el->val == NULL only when we know that the
       map will be destroyed immediately afterwards. */
    DBG h->check(/*allow_null_val=*/true);

    bucket *b;
    size_t i;

    for (i = 0; i < h->nbuckets; ++i) {
        b = h->buckets[i];
        if (b) {
            elem *el;
            for (el = b->el; el->key; ++el) {
                if (el->val)
                    el->destroy();
            }
        }
    }

    free_buckets(w, h->buckets, h->nbuckets);
    __cilkrts_frame_free(w, h, sizeof(*h));
#if JFC_DEBUG
#if JFC_DEBUG > 1
    if (w)
      fprintf(stderr, "destroy worker %u reducer map %p\n", w->self, h);
    else
      fprintf(stderr, "destroy global reducer map %p\n", h);
#endif
    __sync_sub_and_fetch(&reducer_map_count, 1);
#endif
}

/* Set the specified reducer map as the leftmost map if is_leftmost is true,
   otherwise, set it to not be the leftmost map. */
void __cilkrts_set_leftmost_reducer_map(cilkred_map *h, int is_leftmost)
{
    h->is_leftmost = is_leftmost;
}

/* Make a copy of a reducer map for the indicated worker.
   In general this requires a copy of the hash table (but moving --
   not copying -- the values) to ensure that memory is allocated from
   the worker's Cilk context. */
cilkred_map *cilkred_map::move(cilkred_map *r,
                               __cilkrts_worker *from,
                               __cilkrts_worker *to)
{
    cilkred_map *n = __cilkrts_make_reducer_map(to);

    if (r == NULL)
        return n;

    DBG r->check(/*allow_null_val=*/false);

    for (size_t i = 0; i < r->nbuckets; ++i) {
        bucket *b = r->buckets[i];
        if (b) {
            elem *el;
            for (el = b->el; el->key; ++el) {
                CILK_ASSERT(el->val);

                /* move object from right map to left */
                n->rehash_and_insert(to, el->key, el->val);
                el->val = 0;
            }
        }
    }
    n->is_leftmost = r->is_leftmost;
    __cilkrts_destroy_reducer_map(from, r);
    return n;
}

#if 0 // OBSOLETE: For reference only.  We no longer need to import or export reducer maps.
void __cilkrts_import_reducer_map(__cilkrts_worker *w)
{
    CILK_ASSERT(w->reducer_map == NULL);
    cilkred_map *r = cilkred_map::move(get_and_clear_tls(), 0, w);
    r->is_leftmost = true;
    w->reducer_map = r;
}

/* We do not allow Cilk code to access reducers while executing
   in parallel with non-Cilk code.  The order of operation is
   not defined.  (This will change for splitters, where the
   value need not propagate back out.)

   At global scope some other reducer might have been created,
   so merge the maps with duplicates forbidden.  If the reducer
   map was borrowed from a worker, nobody should have touched it. */
void __cilkrts_export_reducer_map(
    __cilkrts_worker *from, __cilkrts_worker *to, cilkred_map **rf)
{
    cilkred_map *r = *rf;
    CILK_ASSERT(r != NULL);
    *rf = 0;
    if (to == NULL) {
        cilkred_map *g = get_tls_map();
        g->merge(0, r, from, cilkred_map::MERGE_UNORDERED);
        /*destroy_tls_if_empty();*/
    } else {
        cilkred_map *nmap = cilkred_map::move(r, from, to);
        (void)__cilkrts_xchg_reducer(to, nmap);
    }
    return;
}
#endif // End OBSOLETE code

void cilkred_map::merge(__cilkrts_worker *w,
                        cilkred_map *other_map,
                        __cilkrts_worker *other_w,
                        enum merge_kind kind)
{
    // Disable Cilkscreen while the we merge the maps.  The destructor for
    // the guard class will re-enable Cilkscreen when it goes out of scope.
    // This will prevent Cilkscreen from reporting apparent races in between
    // the reduce function and the reducer operations.  The Cilk runtime
    // guarantees that a pair of reducer maps will only be merged when no 
    // other strand will access them.
    DisableCilkscreen guard;

    merging = true;
    other_map->merging = true;

    // Merging to the leftmost view is a special case because every leftmost
    // element must be initialized before the merge.
    CILK_ASSERT(!other_map->is_leftmost /* || kind == MERGE_UNORDERED */);
    bool merge_to_leftmost = (this->is_leftmost /* && !other_map->is_leftmost */);

    DBG check(/*allow_null_val=*/false);
    DBG other_map->check(/*allow_null_val=*/false);

    for (size_t i = 0; i < other_map->nbuckets; ++i) {
        bucket *b = other_map->buckets[i];
        if (b) {
            for (elem *other_el = b->el; other_el->key; ++other_el) {
                /* Steal the value from the other map, which will be
                   destroyed at the end of this operation. */
                void *other_val = other_el->val;
                CILK_ASSERT(other_val);

                __cilkrts_hyperobject_base *key = other_el->key;
                elem *this_el = lookup(key);

                if (this_el == 0 && merge_to_leftmost) {
                    /* Initialize leftmost view before merging. */
                    void* leftmost = get_leftmost_view(key);
                    // leftmost == other_val can be true if the initial view
                    // was created in other than the leftmost strand of the
                    // spawn tree, but then made visible to subsequent strands
                    // (E.g., the reducer was allocated on the heap and the
                    // pointer was returned to the caller.)  In such cases,
                    // parallel semantics says that syncing with earlier
                    // strands will always result in 'this_el' being null,
                    // thus propagating the initial view up the spawn tree
                    // until it reaches the leftmost strand.  When synching
                    // with the leftmost strand, leftmost == other_val will be
                    // true and we must avoid reducing the initial view with
                    // itself.
                    if (leftmost != other_val)
                        this_el = rehash_and_insert(w, key, leftmost);
                }

                if (this_el == 0) {
                    /* move object from other map into this one */
                    rehash_and_insert(w, key, other_val);
                    other_el->val = 0;
                    continue; /* No element-level merge necessary */
                }

                /* The same key is present in both maps with values
                   A and B.  Three choices: fail, A OP B, B OP A. */
                switch (kind)
                {
                case MERGE_UNORDERED:
                    __cilkrts_bug("TLS Reducer race");
                    break;
                case MERGE_INTO_RIGHT:
                    /* Swap elements in order to preserve object
                       identity */
                    other_el->val = this_el->val;
                    this_el->val = other_val;
                    /* FALL THROUGH */
                case MERGE_INTO_LEFT: {
                    /* Stealing should be disabled during reduce
                       (even if force-reduce is enabled). */
                    __cilkrts_stack_frame * volatile *saved_protected_tail =
                        __cilkrts_disallow_stealing(w, NULL);

                    /* TBD: if reduce throws an exception we need to stop it
                       here. */
                    key->__c_monoid.reduce_fn(key,this_el->val, other_el->val);

                    /* Restore stealing */
                    __cilkrts_restore_stealing(w, saved_protected_tail);
                   
                  } break;
                }
            }
        }
    }
    this->is_leftmost = this->is_leftmost || other_map->is_leftmost;
    merging = false;
    other_map->merging = false;
    __cilkrts_destroy_reducer_map(other_w, other_map);
}

/* merge RIGHT into LEFT; return whichever map allows for faster
   merge; destroy the other one */
cilkred_map *__cilkrts_merge_reducer_maps(
    __cilkrts_worker *w,
    cilkred_map *left_map,
    cilkred_map *right_map)
{
    if (!left_map)
        return right_map;
    if (!right_map)
        return left_map;

    /* Special case, if left_map is leftmost, then always merge into it.
       For C reducers this forces lazy creation of the leftmost views. */
    if (left_map->is_leftmost || left_map->nelem > right_map->nelem) {
        left_map->merge(w, right_map, w, cilkred_map::MERGE_INTO_LEFT);
        return left_map;
    } else {
        right_map->merge(w, left_map, w, cilkred_map::MERGE_INTO_RIGHT);

        return right_map;
    }
}

/* End reducer_impl.cpp */
