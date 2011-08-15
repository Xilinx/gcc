/* signal_node.c               -*-C-*-
 *
 *************************************************************************
 *
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
 *
 **************************************************************************/

#include "signal_node.h"
#include <stdlib.h>

/* Define cilk_semaphore_t for all of the respective systems. */
#if defined __APPLE__
#   include <mach/mach_init.h>
#   include <mach/semaphore.h>
#   include <mach/task.h>
    typedef semaphore_t cilk_semaphore_t;
#elif defined _WIN32
#   include "windows-clean.h"
    typedef HANDLE cilk_semaphore_t;
#else // Linux/MIC
#   include <errno.h>
#   include <semaphore.h>
#   include <stdio.h>
    typedef sem_t cilk_semaphore_t;
#endif // Linux/MIC

#include "bug.h"
#include "cilk_malloc.h"
#include "signal_node.h"

/**
 * Interface within the tree to notify workers to wait without consuming cycles
 * to expend cycles trying to steal.
 *
 * cilk_semaphore_t is implemented as an auto-reset event on Windows, and
 * as a semaphore_t on Linux and MacOS.
 */
typedef struct signal_node_t
{
    /** 0 if the worker should wait, 1 if it should be running. */
    volatile unsigned int run;

    /** OS-specific semaphore on which the worker can wait. */
    cilk_semaphore_t sem;
} signal_node_t;

/******************************************************************************/
/* Semaphore-abstraction functions                                            */
/******************************************************************************/

/*
 * All of these functions are simple wrappers for the system-specific semaphore
 * functions.  This keeps the rest of the code reasonably clean and readable.
 */

#if defined __APPLE__
static void initialize_cilk_semaphore (cilk_semaphore_t *sem)
{
    kern_return_t kstatus
        = semaphore_create(mach_task_self(), sem, SYNC_POLICY_FIFO, 0);
    assert(kstatus == KERN_SUCCESS);
}
static void deinitialize_cilk_semaphore (cilk_semaphore_t *sem)
{
    kern_return_t kstatus = semaphore_destroy(mach_task_self(), *sem);
    assert(kstatus == KERN_SUCCESS);
}
static void wait_on_cilk_semaphore (cilk_semaphore_t *sem)
{
    kern_return_t kstatus = semaphore_wait(*sem);
    assert(kstatus == KERN_SUCCESS);
}
static void signal_cilk_semaphore (cilk_semaphore_t *sem)
{
    kern_return_t kstatus = semaphore_signal(*sem);
    assert(kstatus == KERN_SUCCESS);
}
#elif defined _WIN32
// Note: Windows only provides counting semaphores, and we don't really
// care about the count. So this is implemented using an auto-reset
// event which will automatically reset after the WaitForSingleObject
// call
static void initialize_cilk_semaphore (cilk_semaphore_t *sem)
{
    // Create an auto-reset event
    *sem = CreateEvent(NULL,    // Security attributes
                       FALSE,   // Manual reset
                       FALSE,   // Initial state (initially reset)
                       NULL);   // Name (anonymous)
    CILK_ASSERT (NULL != *sem);
}

static void deinitialize_cilk_semaphore (cilk_semaphore_t *sem)
{
    BOOL result = CloseHandle(*sem);
    CILK_ASSERT (0 != result);
}

static void wait_on_cilk_semaphore (cilk_semaphore_t *sem)
{
    // WaitForSingleObject will reset the event
    DWORD result = WaitForSingleObject (*sem, INFINITE);
    CILK_ASSERT (WAIT_OBJECT_0 == result);
}
static void signal_cilk_semaphore (cilk_semaphore_t *sem)
{
    BOOL result = SetEvent (*sem);
    CILK_ASSERT (0 != result);
}
#else // Linux/MIC
static void initialize_cilk_semaphore (cilk_semaphore_t *sem)
{
    int status = sem_init(sem, 0, 0);
    assert(0 == status);
}
static void deinitialize_cilk_semaphore (cilk_semaphore_t *sem)
{
    int status = sem_destroy(sem);
    assert(0 == status);
}
static void wait_on_cilk_semaphore (cilk_semaphore_t *sem)
{
    int status;

    do {
        status = sem_wait(sem);
    } while (status != 0 && errno == EINTR);

    if (status != 0) {
        perror("sem_wait");
        abort();
    }
}
static void signal_cilk_semaphore (cilk_semaphore_t *sem)
{
    sem_post(sem);
}
#endif // Linux/MIC

/******************************************************************************/
/* Runtime interface functions                                                */
/******************************************************************************/

/*
 * Return a newly malloc'd and initialized signal_node_t.
 */
COMMON_SYSDEP
signal_node_t *signal_node_create(void)
{
     signal_node_t *node;

    node = ( signal_node_t*)
        __cilkrts_malloc(sizeof( signal_node_t));
    node->run = 0;
    initialize_cilk_semaphore(&node->sem);

    return node;
}

/*
 * Clean and free a signal_node_t.
 */
void signal_node_destroy(signal_node_t *node)
{
    CILK_ASSERT(node);
    deinitialize_cilk_semaphore(&node->sem);
    __cilkrts_free(node);
}

/*
 * Return 1 if the node thinks the worker should go to sleep, 0 otherwise.
 */
unsigned int signal_node_should_wait(signal_node_t *node)
{
    CILK_ASSERT(node);
    return !node->run;
}

/*
 * Send a message to the node that the worker will eventually read.
 */
void signal_node_msg(signal_node_t *node, unsigned int msg)
{
    CILK_ASSERT(node);
    switch (msg) {
    case 0:                    // worker should go to sleep.
        node->run = msg;
        break;
    case 1:                    // worker should be awake.
        node->run = msg;
        signal_cilk_semaphore(&node->sem);
        break;
    default:                   // error.
        CILK_ASSERT(0 == "Bad signal_node_t message.");
    }
}

/*
 * The current worker will wait on the semaphore.
 */
void signal_node_wait(signal_node_t *node)
{
    CILK_ASSERT(node);
    while (signal_node_should_wait(node)) {
        // The loop is here to consume extra semaphore signals that might have
        // accumulated.  No point in passing on the accumulation.
        wait_on_cilk_semaphore(&node->sem);
    }
}
