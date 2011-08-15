/* cilk_malloc.h                  -*-C++-*-
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
 * @file cilk_malloc.h
 *
 * @brief Provides replacement memory allocation functions to allocate
 * (and free) memory on cache line boundaries, if supported by the OS.
 *
 * If aligned memory functions are not provided by the OS, the calls just
 * pass through to the standard memory allocation functions.
 */

#ifndef INCLUDED_CILK_MALLOC_DOT_H
#define INCLUDED_CILK_MALLOC_DOT_H

#include <cilk/common.h>
#include <stddef.h>

#include "rts-common.h"

__CILKRTS_BEGIN_EXTERN_C

/**
 * malloc replacement function to allocate memory aligned on a cache line
 * boundary if aligned memory allocations are supported by the OS.
 *
 * @param size Number of bytes to allocate.
 *
 * @return pointer to memory block allocated, or NULL if unsuccessful.
 */
COMMON_PORTABLE void *__cilkrts_malloc(size_t size);

/**
 * realloc replacement function to allocate memory aligned on a cache line
 * boundary if aligned memory allocations are supported by the OS.
 *
 * @param ptr Block to be reallocated.
 * @param size Number of bytes to allocate.
 *
 * @return pointer to memory block allocated, or NULL if unsuccessful.
 */
COMMON_PORTABLE void *__cilkrts_realloc(void *ptr, size_t size);

/**
 * free replacement function to deallocate memory aligned on a cache line
 * boundary if aligned memory allocations are supported by the OS.
 *
 * @param ptr Block to be freed.
 */
COMMON_PORTABLE void __cilkrts_free(void *ptr);

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_CILK_MALLOC_DOT_H)
