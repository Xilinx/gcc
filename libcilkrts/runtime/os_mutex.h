/* os_mutex.h                  -*-C++-*-
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
 * @file os_mutex.h
 *
 * @brief Portable interface to operating-system mutexes.
 *
 * Do not confuse os_mutex with Cilk runtime-specific spinlock mutexes.
 */

#ifndef INCLUDED_OS_MUTEX_DOT_H
#define INCLUDED_OS_MUTEX_DOT_H

#include "rts-common.h"

typedef struct os_mutex os_mutex;

/**
 * __cilkrts_global_os_mutex is used to serialize accesses that may change data
 * in the global state.
 *
 * Specifically, when binding and unbinding threads, and when initializing and
 * shutting down the runtime.
 */
COMMON_SYSDEP os_mutex *__cilkrts_global_os_mutex;

/**
 * Allocate and initialize an os_mutex
 *
 * @return A pointer to the initialized os_mutex
 */
COMMON_SYSDEP os_mutex* __cilkrts_os_mutex_create(void);

/**
 * Acquire the os_mutex for exclusive use
 *
 * @param m The os_mutex that is to be acquired.
 */
COMMON_SYSDEP void __cilkrts_os_mutex_lock(os_mutex *m);

/*COMMON_SYSDEP int __cilkrts_os_mutex_trylock(os_mutex *m);*/

/**
 * Release the os_mutex
 *
 * @param m The os_mutex that is to be released.
 */
COMMON_SYSDEP void __cilkrts_os_mutex_unlock(os_mutex *m);

/**
 * Release any resources and deallocate the os_mutex.
 *
 * @param m The os_mutex that is to be deallocated.
 */
COMMON_SYSDEP void __cilkrts_os_mutex_destroy(os_mutex *m);

#endif // ! defined(INCLUDED_OS_MUTEX_DOT_H)
