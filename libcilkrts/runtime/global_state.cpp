/* global_state.cpp                  -*-C++-*-
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

#include "global_state.h"
#include "os.h"
#include "bug.h"
#include "metacall_impl.h"
#include "stats.h"
#include "cilk/cilk_api.h"

#include <algorithm>  // For max()
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cerrno>

#ifdef _WIN32
#   include <wchar.h>
#endif

// Set to true if the user settable values portion of the global state
// singleton is initialized, even if the rest of the singleton is not
// initialized.
int cilkg_user_settable_values_initialized = false;

// Pointer to the global state singleton.  
extern "C" global_state_t *cilkg_singleton_ptr = NULL;  

// __cilkrts_global_state is exported and referenced by the debugger
// It is always equal to cilkg_singleton_ptr.
extern "C" global_state_t *__cilkrts_global_state = NULL;

namespace {

// Single copy of the global state.  Zero-filled until
// cilkg_get_user_settable_values() is called and partially-zero-filled until
// cilkg_init_global_state() is called.
global_state_t global_state_singleton = { };

// Returns true if 'a' and 'b' are equal null-terminated strings
inline bool strmatch(const char* a, const char* b)
{
    return 0 == std::strcmp(a, b);
}

// Returns the integer value represented by the null-terminated string at 's'.
inline long to_long(const char* s)
{
    char *end;

    errno = 0;
    return std::strtol(s, &end, 0);
}

#ifdef _WIN32
// Returns true if 'a' and 'b' are equal null-terminated wide-char strings
inline bool strmatch(const wchar_t* a, const wchar_t* b)
{
    return 0 == wcscmp(a, b);
}

// Returns true if the multi-byte character string at 'a' represents the same
// character sequence as the wide-character string at 'b'.  The behavior is
// undefined if 'a' contains more than 30 multi-byte characters.
bool strmatch(const char* a, const wchar_t* b)
{
    // Convert 'a' to wide-characters, then compare.
    wchar_t wa[31];
    std::size_t count;
    errno_t err = mbstowcs_s(&count, wa, a, 30);
    CILK_ASSERT(0 == err);
    if (err) return false;
    return strmatch(wa, b);
}

// Returns true if the wide-character string at 'a' represents the same
// character sequence as the multi-byte character string at 'b'.  The behavior
// id undefined if 'b' contains more than 30 multi-byte characters.
inline
bool strmatch(const wchar_t* a, const char* b)
{
    return strmatch(b, a);
}


// Returns the integer value represented by the null-terminated wide-char
// string at 's'.
inline long to_long(const wchar_t* s)
{
    wchar_t *end;

    errno = 0;
    return wcstol(s, &end, 0);
}
#endif

// Check if Cilkscreen or other sequential ptool wants to force reducers.
bool always_force_reduce()
{
    // Metacall *looks* like a no-op.  volatile needed to keep compiler from
    // optimizing away variable.
    volatile char not_force_reduce = '\377';
    __cilkrts_metacall(METACALL_TOOL_SYSTEM, HYPER_ZERO_IF_FORCE_REDUCE,
                       const_cast<char*>(&not_force_reduce));
    return ! not_force_reduce;
}

// Stores the boolean value represented by the null-terminated string at 'val'
// into the integer object at 'out'.  Returns '__CILKRTS_SET_PARAM_SUCCESS' if
// 'val' is "true", "false", "0" or "1" and '__CILKRTS_SET_PARAM_INVALID'
// otherwise.
template <typename INT_T, typename CHAR_T>
int store_bool(INT_T *out, const CHAR_T *val)
{
    static const char* const s_zero  = "0";
    static const char* const s_one   = "1";
    static const char* const s_true  = "true";
    static const char* const s_false = "false";
    
    if (val == 0)
        return __CILKRTS_SET_PARAM_INVALID;

    if (strmatch(s_false, val) || strmatch(s_zero, val)) { 
        *out = 0;
        return __CILKRTS_SET_PARAM_SUCCESS;
    }

    if (strmatch(s_true, val) || strmatch(s_one, val)) { 
        *out = 1;
        return __CILKRTS_SET_PARAM_SUCCESS;
    }

    return __CILKRTS_SET_PARAM_INVALID;
}

// Stores the integer value represented by the null-terminated string at 'val'
// into the integer object at 'out', restricting the result to the range 'min'
// to 'max', inclusive.  Returns '__CILKRTS_SET_PARAM_SUCCESS' if the conversion
// succeeds and is in range, '__CILKRTS_SET_PARAM_XRANGE' if the conversion
// succeeds but is out of range, and '__CILKRTS_SET_PARAM_INVALID' otherwise.  In
// the case of any error, '*out' is unchanged.
template <typename INT_T, typename CHAR_T>
int store_int(INT_T *out, const CHAR_T *val, INT_T min, INT_T max)
{
    errno = 0;
    char *end = 0;
    long val_as_long = to_long(val);
    if (val_as_long == 0 && errno != 0)
        return __CILKRTS_SET_PARAM_INVALID;
    if (val_as_long < min || val_as_long == LONG_MIN)
        return __CILKRTS_SET_PARAM_XRANGE;
    else if (val_as_long > max || val_as_long == LONG_MAX)
        return __CILKRTS_SET_PARAM_XRANGE;

    *out = val_as_long;
    return __CILKRTS_SET_PARAM_SUCCESS;
}

// Implementaton of cilkg_set_param templatized on character type.
// Windows will instantiate with both char and wchar_t.
// Note that g must have its user settable values set, but need not be fully
// initialized.
template <class CHAR_T>
int set_param_imp(global_state_t* g, const CHAR_T* param, const CHAR_T* value)
{
    static const char* const s_force_reduce     = "force reduce";
    static const char* const s_nworkers         = "nworkers";
    static const char* const s_max_user_workers = "max user workers";
    static const char* const s_local_stacks     = "local stacks";
    static const char* const s_shared_stacks    = "shared stacks";
    static const char* const s_nstacks          = "nstacks";

    // We must have a parameter and a value
    if (0 == param)
        return __CILKRTS_SET_PARAM_INVALID;
    if (0 == value)
        return __CILKRTS_SET_PARAM_INVALID;

    if (strmatch(param, s_force_reduce))
    {
        // Sets whether we force a reduce operation at every sync.  Useful for
        // debugging reducers.  Off by default.  Overridden by Cilkscreen
        //
        // Documented in cilk_api_<os>.h
        if (always_force_reduce())
            // Force reduce is set by cilkscreen.  User cannot change it.
            return __CILKRTS_SET_PARAM_LATE;

        return store_bool(&g->force_reduce, value);
    }
    else if (strmatch(param, s_nworkers))
    {
        // Set the total number of workers.  Overrides count of cores we get
        // from the OS and the setting of the CILK_NWORKERS environment
        // variable.  Setting to 0 indicates that the default worker count
        // should be used.
        //
        // Documented in cilk_api_<os>.h
        if (cilkg_singleton_ptr)
            return __CILKRTS_SET_PARAM_LATE;

        int hardware_cpu_count = __cilkrts_hardware_cpu_count();
        int max_cpu_count = 16 * hardware_cpu_count;
        if (__cilkrts_running_under_sequential_ptool())
        {
            hardware_cpu_count = 1;
            max_cpu_count = 1;
        }
        // Allow a value of 0, which means "set to hardware thread count".
        int ret = store_int(&g->P, value, 0, max_cpu_count);
        if (0 == g->P)
            g->P = hardware_cpu_count;
        return ret;
    }
    else if (strmatch(param, s_max_user_workers))
    {
        // ** UNDOCUMENTED **
        //
        // Sets the number of slots allocated for user worker threads
        int hardware_cpu_count = __cilkrts_hardware_cpu_count();
        return store_int(&g->max_user_workers, value, 1,
                         16 * hardware_cpu_count);
    }
    else if (strmatch(param, s_local_stacks))
    {
        // ** UNDOCUMENTED **
        //
        // Number of stacks we'll hold in the per-worker stack cache.  Maximum
        // value is 42.  See __cilkrts_make_global_state for details.
        return store_int(&g->stack_cache_size, value, 0, 42);
    }
    else if (strmatch(param, s_shared_stacks))
    {
        // ** UNDOCUMENTED **
        //
        // Maximum number of stacks we'll hold in the global stack
        // cache. Maximum value is 42.  See __cilkrts_make_global_state for
        // details.
        return store_int(&g->global_stack_cache_size, value, 0, 42);
    }
    else if (strmatch(param, s_nstacks))
    {
        // Sets the maximum number of stacks permitted at one time.  If the
        // runtime reaches this maximum, it will cease to allocate stacks and
        // the app will lose parallelism.  0 means unlimited.  Default is
        // unlimited.  Minimum is twice the number of worker threads, though
        // that cannot be tested at this time.
        //
        // Undocumented at this time, though there are plans to expose it.
        // The current implentation is for Linux debugging only and is not
        // robust enough for users.
        return store_int<long>(&g->max_stacks, value, 0, INT_MAX);
    }

    // If got here, then didn't match any of the strings
    return __CILKRTS_SET_PARAM_UNIMP;
}

} // end unnamed namespace

__CILKRTS_BEGIN_EXTERN_C

/**
 * @brief Returns the global state object.  If called for the first time,
 * initializes the user-settable values in the global state, but does not
 * initialize the rest of the structure.
 */
global_state_t* cilkg_get_user_settable_values()
{
    // Environment variable value.  More than big enough for a 64-bit signed
    // integer.
    char envstr[24];

    // Abbreviating &global_state_singleton as g is not only shorter, it also
    // facilitates grepping for the string "g->", which appears ubiquitously
    // in the runtime code.
    global_state_t* g = &global_state_singleton;

    if (! cilkg_user_settable_values_initialized)
    {
        // All fields will be zero until set.  In particular
        std::memset(g, 0, sizeof(global_state_t));

        int hardware_cpu_count = __cilkrts_hardware_cpu_count();
        bool under_ptool = __cilkrts_running_under_sequential_ptool();
        if (under_ptool)
            hardware_cpu_count = 1;

        g->under_ptool              = under_ptool;
        g->force_reduce             = 0;   // Default Off
        g->P                        = hardware_cpu_count;
        g->max_user_workers         = std::max(3, hardware_cpu_count * 2);
        g->stack_cache_size         = 7;   // Arbitrary default
        g->global_stack_cache_size  = 3;   // Arbitrary default
        g->max_stacks               = 0;   // 0 == unlimited
        g->max_steal_failures       = 128; // TBD: depend on max_workers?

        if (always_force_reduce())
            g->force_reduce = true;
        else if (cilkos_getenv(envstr, sizeof(envstr), "CILK_FORCE_REDUCE"))
            store_bool(&g->force_reduce, envstr);

        if (under_ptool)
            g->P = 1;  // Ignore environment variable if under cilkscreen
        else if (cilkos_getenv(envstr, sizeof(envstr), "CILK_NWORKERS"))
            // Set P to environment variable, but limit to no less than 1
            // and no more than 16 times the number of hardware threads.
            store_int(&g->P, envstr, 1, 16 * hardware_cpu_count);

        if (cilkos_getenv(envstr, sizeof(envstr), "CILK_MAX_USER_WORKERS"))
            // Set max_user_workers to environment variable, but limit to no
            // less than 1 and no more 16 times the number of hardware
            // threads.  If not specified, defaults (somewhat arbitrarily) to
            // the larger of 3 and twice the number of hardware threads.
            store_int(&g->max_user_workers, envstr, 1, 16*hardware_cpu_count);

        if (cilkos_getenv(envstr, sizeof(envstr), "CILK_STEAL_FAILURES"))
            // Set the number of times a worker should fail to steal before
            // it looks to see whether it should suspend itself.
            store_int<unsigned>(&g->max_steal_failures, envstr, 1, INT_MAX);

        // g->max_user_workers (optionally set by the user using
        // __cilkrts_set_param("max user workers")) is is the number of
        // simultaneous user threads that can join the Cilk runtime.  If not
        // set by the user, it (somewhat arbitrarily) defaults to the larger
        // of 3 and twice the number of workers.
        if (g->max_user_workers <= 0)
            g->max_user_workers = std::max(3, g->P * 2);

        // Compute the total number of workers to allocate.  Subract one from
        // nworkers and user workers so that the first user worker isn't
        // factored in twice.
        //
        // nworkers must be computed now to support __cilkrts_get_total_workers
        g->nworkers = g->P + g->max_user_workers - 1;

        cilkg_user_settable_values_initialized = true;
    }

    return g;
}

global_state_t* cilkg_init_global_state()
{
    if (cilkg_singleton_ptr)
        return cilkg_singleton_ptr;

    // Get partially-initialized global state.
    global_state_t* g = cilkg_get_user_settable_values();

    int i, max_workers;

    if (g->max_stacks > 0) {

        // nstacks is currently honored on non-Windows systems only.

        // Set an upper bound on the number of stacks that are allocated.  If
        // nstacks is set, each worker gets up to one stack in its cache so that
        // no one worker can hog all of the free stacks and keep work from being
        // stolen by the other workers.

        // nstacks corresponds to the number of stacks that will be allocated by
        // the runtime apart from the initial stack created for each thread by
        // the system.  Therefore, if a user asks for n stacks, and there are
        // p workers created, the total number of stacks is actually n + p.

        // This feature is primarily for MIC which has flat memory
        // instead of virtual addresses and tends to run out really quickly.
        // It is not implemented for Windows and it's non-intuitive
        // interaction with the local stack cache is specifically to help out
        // MIC.

        g->stack_cache_size = 1; // One stack per worker cache.

        if (g->max_stacks < g->P)
            g->max_stacks = g->P;

        g->global_stack_cache_size = g->max_stacks;
    }

    // Number of bytes/address - validation for debugger integration
    g->addr_size = sizeof(void *);

    __cilkrts_init_stats(&g->stats);

    __cilkrts_frame_malloc_global_init(g);

    g->Q = 0;
    g->system_workers = g->P - 1; // system_workers is here for the debugger.
    g->work_done = 0;
    g->running = 0;
    g->stealing_disabled = 0;
    g->exiting_runtime = 0;
    g->ltqsize = 1024; /* FIXME */

    g->stacks = 0;
    g->stack_size = 0;
    g->failure_to_allocate_stack = 0;


    cilkg_singleton_ptr = g;
    __cilkrts_global_state = g;

    return g;
}

void cilkg_deinit_global_state()
{
    cilkg_singleton_ptr = NULL;
    __cilkrts_global_state = NULL;
}

int cilkg_is_initialized(void)
{
    return NULL != cilkg_singleton_ptr;
}

int cilkg_set_param(const char* param, const char* value)
{
    return set_param_imp(cilkg_get_user_settable_values(), param, value);
}

#ifdef _WIN32
int cilkg_set_param_w(const wchar_t* param, const wchar_t* value)
{
    return set_param_imp(cilkg_get_user_settable_values(), param, value);
}
#endif

extern "C++" {
    // C++ scheduler function (that may throw exceptions)
    typedef void cpp_scheduler_t(__cilkrts_worker *w);
}

void __cilkrts_run_scheduler_with_exceptions(__cilkrts_worker *w)
{
    global_state_t* g = cilkg_get_global_state();
    CILK_ASSERT(g->scheduler);

    cpp_scheduler_t* scheduler = (cpp_scheduler_t*) g->scheduler;

    try {
        scheduler(w);
    } catch (...) {
        __cilkrts_bug("Exception escaped Cilk context");
    }
}

__CILKRTS_END_EXTERN_C

/* End global_state.cpp */
