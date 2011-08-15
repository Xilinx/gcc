/*
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
 *
 */

#ifndef INCLUDED_CILK_HYPEROBJECT_BASE
#define INCLUDED_CILK_HYPEROBJECT_BASE

#ifdef __cplusplus
# include <cstdlib>
# include <cstddef>
#else
# include <stdlib.h>
# include <stddef.h>
#endif

#include <cilk/common.h>

#if defined _WIN32 || defined _WIN64
# if !defined CILK_STUB && !defined IN_CILK_RUNTIME
    /* bring in the Cilk library, which has definitions for some of these
     * functions. */
#   pragma comment(lib, "cilkrts")
# endif
#endif

/* Macro to cache-align a declaration.  Argument(s) comprise either a
 * variable or a struct declaration. */
#define __CILKRTS_CACHE_LINE__ 64 /* Good enough for most architectures */
#if defined(__INTEL_COMPILER) || defined(_WIN32)
# define __CILKRTS_CACHE_ALIGNED(...) \
    __declspec(align(__CILKRTS_CACHE_LINE__)) __VA_ARGS__
#elif defined(__GNUC__)
# define __CILKRTS_CACHE_ALIGNED(...) \
    __VA_ARGS__ __attribute__((__aligned__(__CILKRTS_CACHE_LINE__)))
#else
# define __CILKRTS_CACHE_ALIGNED(...) __VA_ARGS__
#endif

/* The __CILKRTS_STRAND_PURE attribute tells the compiler that the value
 * returned by 'func' for a given argument to 'func' will remain valid until
 * the next strand boundary (spawn or sync) or until the next call to a
 * function with the __CILKRTS_STRAND_STALE attribute using the same function
 * argument.
 */
#if 0 && defined __cilk && (defined __GNUC__ && !defined _WIN32) && defined __cilkartsrev
# define __CILKRTS_STRAND_PURE(func) \
    func __attribute__((__cilk_hyper__("lookup")))
# define __CILKRTS_STRAND_STALE(func) \
    func __attribute__((__cilk_hyper__("flush")))
#else
# define __CILKRTS_STRAND_PURE(func) func
# define __CILKRTS_STRAND_STALE(func) func
#endif

/*****************************************************************************
 * C runtime interface to the hyperobject subsystem
 *****************************************************************************/

__CILKRTS_BEGIN_EXTERN_C

/* Callback function signatures.  The 'r' argument always points to the
 * reducer itself and is commonly ignored. */
typedef void (*cilk_c_reducer_reduce_fn_t)(void* r, void* lhs, void* rhs);
typedef void (*cilk_c_reducer_identity_fn_t)(void* r, void* view);
typedef void (*cilk_c_reducer_destroy_fn_t)(void* r, void* view);
typedef void* (*cilk_c_reducer_allocate_fn_t)(void* r, __STDNS size_t bytes);
typedef void (*cilk_c_reducer_deallocate_fn_t)(void* r, void* view);

/** Representation of the monoid */
typedef struct cilk_c_monoid {
    cilk_c_reducer_reduce_fn_t          reduce_fn;
    cilk_c_reducer_identity_fn_t        identity_fn;
    cilk_c_reducer_destroy_fn_t         destroy_fn;
    cilk_c_reducer_allocate_fn_t        allocate_fn;
    cilk_c_reducer_deallocate_fn_t      deallocate_fn;
} cilk_c_monoid;

/** Base of the hyperobject */
typedef struct __cilkrts_hyperobject_base
{
    cilk_c_monoid       __c_monoid;
    unsigned long long  __flags;
    __STDNS ptrdiff_t   __view_offset;  /* offset (in bytes) to leftmost view */
    __STDNS size_t      __view_size;    /* Size of each view */
} __cilkrts_hyperobject_base;

/* Library functions. */
CILK_EXPORT
    void __cilkrts_hyper_create(__cilkrts_hyperobject_base *key);
CILK_EXPORT void __CILKRTS_STRAND_STALE(
    __cilkrts_hyper_destroy(__cilkrts_hyperobject_base *key));
CILK_EXPORT void* __CILKRTS_STRAND_PURE(
    __cilkrts_hyper_lookup(__cilkrts_hyperobject_base *key));

CILK_EXPORT
    void* __cilkrts_hyperobject_alloc(void* ignore, __STDNS size_t bytes);
CILK_EXPORT
    void __cilkrts_hyperobject_dealloc(void* ignore, void* view);

/* No-op destroy function */
CILK_EXPORT
    void __cilkrts_hyperobject_noop_destroy(void* ignore, void* ignore2);

__CILKRTS_END_EXTERN_C

#endif /* INCLUDED_CILK_HYPEROBJECT_BASE */
