/* os_mutex.h                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2009-2012 
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

#include <cilk/common.h>
#include "rts-common.h"

__CILKRTS_BEGIN_EXTERN_C

typedef struct os_mutex os_mutex;
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

/**
 * Acquire the global os_mutex for exclusive use.  The global os_mutex
 * will be initialized the first time this function is called in a
 * thread-safe manner.
 */
COMMON_SYSDEP void global_os_mutex_lock();

/**
 * Release the global os_mutex.  global_os_mutex_lock() must have been
 * called first.
 */
COMMON_SYSDEP void global_os_mutex_unlock();


#ifdef _MSC_VER

/**
 * @brief Create the global OS mutex - Windows only.
 *
 * On Windows we use DllMain() to create the global OS mutex when cilkrts20.dll
 * is loaded. As opposed to Linux/MacOS where we use pthread_once to implement
 * a singleton since there are no guarantees about constructor or destructor
 * ordering between shared objects.
 */
NON_COMMON void global_os_mutex_create();

/**
 * @brief Destroy the global OS mutex - Windows only
 *
 * On Windows we use DllMain() to destroy the global OS mutex when
 * cilkrts20.dll is unloaded.  As opposed to Linux/MacOS where we cannot
 * know when it's safe to destroy the global OS mutex since there are no
 * guarantees about constructor or destructor ordering.
 */
NON_COMMON void global_os_mutex_destroy();

#endif  // _MSC_VER

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_OS_MUTEX_DOT_H)
