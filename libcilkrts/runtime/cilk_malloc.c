/* cilk_malloc.c                  -*-C-*-
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

#include "cilk_malloc.h"

#include <stdlib.h>
#if defined _WIN32 || defined _WIN64 || defined __linux__
#include <malloc.h>
#define HAS_MEMALIGN 1
#endif

#define PREFERRED_ALIGNMENT 64  /* try to keep runtime system data
                                   structures within one cache line */

void *__cilkrts_malloc(size_t size)
{
    /* TODO: check for out of memory */
#ifdef _WIN32
    return _aligned_malloc(size, PREFERRED_ALIGNMENT);
#elif defined HAS_MEMALIGN
    return memalign(PREFERRED_ALIGNMENT, size);
#else
    return malloc(size);
#endif
}

void *__cilkrts_realloc(void *ptr, size_t size)
{
#ifdef _WIN32
    return _aligned_realloc(ptr, size, PREFERRED_ALIGNMENT);
#else
    return realloc(ptr, size);
#endif
}

void __cilkrts_free(void *ptr)
{
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

/* End cilk_malloc.c */
