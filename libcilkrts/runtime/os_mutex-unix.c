/* os_mutex-unix.c                  -*-C-*-
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

#include "os_mutex.h"
#include "bug.h"

#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

// contains notification macros for VTune.
#include "cilk-ittnotify.h"

/*
 * OS Mutex functions.
 *
 * Not to be confused with the spinlock mutexes implemented in cilk_mutex.c
 */

struct os_mutex {
    pthread_mutex_t mutex;  ///< On Linux, os_mutex is implemented with a pthreads mutex
};

// Unix implementation of the global OS mutex.  This will be created by the
// first call to global_os_mutex_lock() and *NEVER* destroyed.  On gcc-based
// systems there's no way to guarantee the ordering of constructors and
// destructors, so we can't be guaranteed that our destructor for a static
// object will be called *after* any static destructors that may use Cilk
// in the user's application
static struct os_mutex *global_os_mutex = NULL;

/* Sometimes during shared library load malloc doesn't work.
   To handle that case, preallocate space for one mutex. */
static struct os_mutex static_mutex;
static int static_mutex_used;

struct os_mutex *__cilkrts_os_mutex_create(void)
{
    int status;
    struct os_mutex *mutex = (struct os_mutex *)malloc(sizeof(struct os_mutex));
    pthread_mutexattr_t attr;

    ITT_SYNC_CREATE(mutex, "OS Mutex");

    if (!mutex) {
        if (static_mutex_used) {
            __cilkrts_bug("Cilk RTS library initialization failed");
        } else {
            static_mutex_used = 1;
            mutex = &static_mutex;
        }
    }

    status = pthread_mutexattr_init(&attr);
    CILK_ASSERT (status == 0);
#if defined DEBUG || CILK_LIB_DEBUG 
#ifdef PTHREAD_MUTEX_ERRORCHECK
    status = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
#else
    status = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#endif
    CILK_ASSERT (status == 0);
#endif
    status = pthread_mutex_init (&mutex->mutex, &attr);
    CILK_ASSERT (status == 0);
    pthread_mutexattr_destroy(&attr);

    return mutex;
}

void __cilkrts_os_mutex_lock(struct os_mutex *p)
{
    int status;

    status = pthread_mutex_lock (&p->mutex);
    ITT_SYNC_ACQUIRED(p);
    if (__builtin_expect(status, 0) == 0)
        return;
    if (status == EDEADLK)
        __cilkrts_bug("Cilk runtime error: deadlock acquiring mutex %p\n",
                      p);
    else
        __cilkrts_bug("Cilk runtime error %d acquiring mutex %p\n",
                      status, p);
}

#if 0
int __cilkrts_os_mutex_trylock(struct os_mutex *p)
{
    int status;

    status = pthread_mutex_trylock (&p->mutex);
    return (status == 0);
}
#endif

void __cilkrts_os_mutex_unlock(struct os_mutex *p)
{
    int status;

    ITT_SYNC_RELEASING(p);
    status = pthread_mutex_unlock (&p->mutex);
    CILK_ASSERT(status == 0);
}

void __cilkrts_os_mutex_destroy(struct os_mutex *p)
{
    pthread_mutex_destroy (&p->mutex);
    if (p == &static_mutex) {
        static_mutex_used = 0;
    } else {
        free(p);
    }
}

/*
 * create_global_os_mutex
 *
 * Function used with pthread_once to initialize the global OS mutex.  Since
 * pthread_once requires a function which takes no parameters and has no
 * return value, the global OS mutex will be stored in the static (global
 * to the compilation unit) variable "global_os_mutex."
 * 
 * 
 * global_os_mutex will never be destroyed.
 */
static void create_global_os_mutex(void)
{
    CILK_ASSERT(NULL == global_os_mutex);
    global_os_mutex = __cilkrts_os_mutex_create();
}

void global_os_mutex_lock(void)
{
    // pthread_once_t used with pthread_once to guarantee that
    // create_global_os_mutex() is only called once
    static pthread_once_t global_os_mutex_is_initialized = PTHREAD_ONCE_INIT;

    // Execute create_global_os_mutex once in a thread-safe manner
    // Note that create_global_os_mutex returns the mutex in the static
    // (global to the module) variable "global_os_mutex"
    pthread_once(&global_os_mutex_is_initialized,
		 create_global_os_mutex);

    // We'd better have allocated a global_os_mutex
    CILK_ASSERT(NULL != global_os_mutex);
    
    // Acquire the global OS mutex
    __cilkrts_os_mutex_lock(global_os_mutex);
}

void global_os_mutex_unlock(void)
{
    // We'd better have allocated a global_os_mutex.  This means you should
    // have called global_os_mutex_lock() before calling
    // global_os_mutex_unlock(), but this is the only check for it.
    CILK_ASSERT(NULL != global_os_mutex);

    // Release the global OS mutex
    __cilkrts_os_mutex_unlock(global_os_mutex);
}

/* End os_mutex-unix.c */
