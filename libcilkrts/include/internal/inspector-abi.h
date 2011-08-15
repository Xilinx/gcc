/*
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
 */

#ifndef CILK_INTERNAL_INSPECTOR_ABI_H
#define CILK_INTERNAL_INSPECTOR_ABI_H

#ifdef __cplusplus
#define CILK_INSPECTOR_EXTERN extern "C"
#else
#define CILK_INSPECTOR_EXTERN extern
#endif

#ifdef _WIN32
#ifdef IN_CILK_RUNTIME
#define CILK_INSPECTOR_ABI(TYPE) CILK_INSPECTOR_EXTERN __declspec(dllexport) TYPE __cdecl
#else   // ! IN_CILK_RUNTIME
#define CILK_INSPECTOR_ABI(TYPE) CILK_INSPECTOR_EXTERN __declspec(dllimport) TYPE __cdecl
#endif  // IN_CILK_RUNTIME
#else   // ! _WIN32
#define CILK_INSPECTOR_ABI(TYPE) CILK_INSPECTOR_EXTERN TYPE
#endif // _WIN32

/*
 * inspector_abi.h
 *
 * ABI for functions provided for the Piersol release of Inspector
 */

#ifdef _WIN32
typedef unsigned __cilkrts_thread_id;   // OS-specific thread ID
#else
typedef void *__cilkrts_thread_id;   // really a pointer to a pthread_t
#endif
typedef void * __cilkrts_region_id;

typedef struct __cilkrts_region_properties
{
    unsigned size;              // struct size as sanity check & upward compatibility – must be set before call
    __cilkrts_region_id parent;
    void *stack_base;
    void *stack_limit;
} __cilkrts_region_properties;

/*
 * __cilkrts_get_stack_region_id
 *
 * Returns a __cilkrts_region_id for the stack currently executing on a thread.
 * Returns NULL on failure.
 */

CILK_INSPECTOR_ABI(__cilkrts_region_id)
__cilkrts_get_stack_region_id(__cilkrts_thread_id thread_id);

/*
 * __cilkrts_get_stack_region_properties
 *
 * Fills in the properties for a region_id.
 *
 * Returns false on invalid region_id or improperly sized __cilkrts_region_properties
 */

CILK_INSPECTOR_ABI(int)
__cilkrts_get_stack_region_properties(__cilkrts_region_id region_id,
                                      __cilkrts_region_properties *properties);

#endif  // CILK_INTERNAL_INSPECTOR_ABI_H
