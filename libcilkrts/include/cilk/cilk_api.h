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

/**
 * @if public_doc
 * @mainpage
 * @section intro_sec Introduction
 *
 * In addition to the Cilk Plus keywords, Intel Cilk Plus provides an API to
 * allow users to query and control the Intel Cilk Plus runtime.
 * @endif
 */

/**
 * @file cilk_api.h
 *
 * @brief Defines the documented API exposed by the Intel Cilk Plus for use
 * by applications.
 */

/**
 * @page API
 * Cilk API -- Functions callable by the user to modify the operation of the
 * Cilk scheduler.
 */

#ifndef INCLUDED_CILK_API_H
#define INCLUDED_CILK_API_H

#ifndef CILK_STUB

#include <cilk/common.h>

#ifdef __cplusplus
#   include <cstddef>  /* Defines size_t */
#else
#   include <stddef.h> /* Defines size_t */
#endif

#ifdef _WIN32
#   ifndef IN_CILK_RUNTIME
/* Ensure the library is brought if any of these functions are being called. */
#       pragma comment(lib, "cilkrts")
#   endif

#   ifndef __cplusplus
#       include <wchar.h>
#   endif
#endif /* _WIN32 */

__CILKRTS_BEGIN_EXTERN_C

/** @brief Return values from __cilkrts_set_param() and __cilkrts_set_param_w() */
enum {
    __CILKRTS_SET_PARAM_SUCCESS = 0, /**< Success - parameter set */
    __CILKRTS_SET_PARAM_UNIMP   = 1, /**< Unimplemented parameter */
    __CILKRTS_SET_PARAM_XRANGE  = 2, /**< Parameter value out of range */
    __CILKRTS_SET_PARAM_INVALID = 3, /**< Invalid parameter value */
    __CILKRTS_SET_PARAM_LATE    = 4  /**< Too late to change parameter value */
};

/**
 * @brief Set user controllable parameters
 *
 * @param param - string specifying parameter to be set
 * @param value - string specifying new value
 * @returns One of: __CILKRTS_SET_PARAM_SUCCESS ( = 0),
 *    __CILKRTS_SET_PARAM_UNIMP, __CILKRTS_SET_PARAM_XRANGE,
 *    __CILKRTS_SET_PARAM_INVALID, or __CILKRTS_SET_PARAM_LATE.
 *
 * @attention The wide character function __cilkrts_set_param_w() is available
 * only on Windows.
 *
 * Allowable parameter names:
 *
 * - "nworkers" - number of processors that should run Cilk code.
 *   The value is a string of digits to be parsed by strtol.
 *
 * - "force reduce" - test reducer callbacks by allocating new views
 *   for every spawn within which a reducer is accessed.  This can
 *   significantly reduce performance.  The value is "1" or "true"
 *   to enable, "0" or "false" to disable.
 */
CILK_API(int) __cilkrts_set_param(const char *param, const char *value);

#ifdef _WIN32
/**
 * Set user controllable parameters using wide strings
 *
 * @copydetails __cilkrts_set_param
 */
CILK_API(int) __cilkrts_set_param_w(const wchar_t *param, const wchar_t *value);
#endif

/**
 * Shut down and deallocate all Cilk state.  The runtime will abort
 * if Cilk is still in use by this thread.  Otherwise the runtime
 * will wait for all other threads using Cilk to exit.
 */
CILK_API(void) __cilkrts_end_cilk(void);

/**
 * Allocate Cilk data structures, starting the runtime.
 */
CILK_API(void) __cilkrts_init(void);

/**
 * Return the number of worker threads that this instance of Cilk
 * will attempt to use.
 */
CILK_API(int) __cilkrts_get_nworkers(void);

/**
 *Return the number of worker threads allocated.
 */
CILK_API(int) __cilkrts_get_total_workers(void);

/**
 * Return a small integer indicating which Cilk worker the function is
 * currently running on.  Each thread started by the Cilk runtime library
 * (system worker) has a unique worker number in the range 1..P-1, where P is
 * the valued returned by __cilkrts_get_nworkers().  All threads started by
 * the user or by other libraries (user workers) share the worker number 0.
 * Therefore, the worker number is not unique across multiple user threads.
 */
CILK_API(int) __cilkrts_get_worker_number(void);

/**
 * Return non-zero if force reduce mode is on
 */
CILK_API(int) __cilkrts_get_force_reduce(void);

/**
 * Interact with tools
 */
CILK_API(void)
    __cilkrts_metacall(unsigned int tool, unsigned int code, void *data);

#ifdef _WIN32
typedef struct _EXCEPTION_RECORD _EXCEPTION_RECORD;

/** Callback function signature for Windows exception notification */
typedef void (*__cilkrts_pfn_seh_callback)(const _EXCEPTION_RECORD *exception);

/**
 * Debugging aid for exceptions on Windows.
 *
 * The specified function will be called when a non-C++ exception is caught
 * by the Cilk Plus runtime.  This is illegal since there's no way for the
 * Cilk Plus runtime to know how to unwind the stack across a strand boundary
 * for Structure Exceptions.
 *
 * This function allows an application to do something before the Cilk Plus
 * runtime aborts the application.
 */
CILK_API(int) __cilkrts_set_seh_callback(__cilkrts_pfn_seh_callback pfn);
#endif

/**
 * Context used by __cilkrts_get_pedigree_info.  Callers should initialize the
 * data array to NULL, and set the size to sizeof(__cilkrts_pedigree_context_t
 * before the first call to __cilkrts_get_pedigree_info and should not examine
 * or modify it after.
 */
typedef struct
{
    __STDNS size_t size;    /**< Size of the struct in bytes */
    void *data[3];          /**< Opaque context data */
} __cilkrts_pedigree_context_t;

/**
 * Pedigree API
 *
 * This routine allows code to walk up the stack of Cilk frames to gather
 * the pedigree.
 *
 * Initialize the pedigree walk by filling the pedigree context with NULLs
 * and setting the size field to sizeof(__cilkrts_pedigree_context).
 * Other than initialization to NULL to start the walk, user coder should
 * consider the pedigree context data opaque and should not examine or
 * modify it.
 *
 * Returns:
 *    0 - Success - birthrank is valid
 *   >0 - End of pedigree walk
 *   <0 - Failure - -1: No worker bound to thread,
 *                  -2: Sanity check failed
 *                  -3: Invalid context size
 *                  -4: Internal error - walked off end of chain of frames
 */
CILK_API(int)
__cilkrts_get_pedigree_info(/* In/Out */ __cilkrts_pedigree_context_t *context,
                            /* Out */    uint64_t *sf_birthrank);

/**
 * Pedigree API
 *
 * Fetch the rank from the currently executing worker
 *
 * Returns:
 *    0 - Success - rank is valid
 *   <0 - Failure - -1: No worker bound to thread
 */

CILK_API(int)
__cilkrts_get_worker_rank(uint64_t *rank);


/**
 * Pedigree API
 *
 * Increment the rank of the currently executing worker
 *
 * Returns:
 *    0 - Success - rank was incremented
 *   -1 - Failure - No worker bound to thread
 */

CILK_API(int)
__cilkrts_bump_worker_rank(void);

__CILKRTS_END_EXTERN_C

#else /* CILK_STUB */

/* Stubs for the api functions */
#ifdef _WIN32
#define __cilkrts_set_param_w(name,value) ((value), 0)
#define __cilkrts_set_seh_callback(pfn) (0)
#endif
#define __cilkrts_set_param(name,value) ((value), 0)
#define __cilkrts_end_cilk() ((void) 0)
#define __cilkrts_init() ((void) 0)
#define __cilkrts_get_nworkers() (1)
#define __cilkrts_get_total_workers() (1)
#define __cilkrts_get_worker_number() (0)
#define __cilkrts_get_force_reduce() (0)
#define __cilkrts_metacall(tool,code,data) ((tool), (code), (data), 0)
#define __cilkrts_get_pedigree_info(context, sf_birthrank) (-1)
#define __cilkrts_get_worker_rank(rank) (-1)
#define __cilkrts_bump_worker_rank() (-1)

#endif /* CILK_STUB */

#endif /* INCLUDED_CILK_API_H */
