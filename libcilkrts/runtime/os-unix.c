/* os-unix.c                  -*-C-*-
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

#ifdef __linux__
    // define _GNU_SOURCE before *any* #include.
    // Even <stdint.h> will break later #includes if this macro is not
    // already defined when it is #included.
#   define _GNU_SOURCE
#endif

#include "os.h"
#include "bug.h"
#include <internal/abi.h>

#if defined __linux__
#   include <sys/sysinfo.h>
#   include <sys/syscall.h>
#elif defined __APPLE__
#   include <sys/sysctl.h>
    // Uses sysconf(_SC_NPROCESSORS_ONLN) in verbose output
#elif defined  __FreeBSD__
#else
#   error Unsupported OS
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>


// /* Thread-local storage */
// #ifdef _WIN32
// typedef unsigned cilkos_tls_key_t;
// #else
// typedef pthread_key_t cilkos_tls_key_t;
// #endif
// cilkos_tls_key_t cilkos_allocate_tls_key();
// void cilkos_set_tls_pointer(cilkos_tls_key_t key, void* ptr);
// void* cilkos_get_tls_pointer(cilkos_tls_key_t key);

#if !defined CILK_WORKER_TLS
static int cilk_keys_defined;
static pthread_key_t worker_key, reducer_key, tbb_interop_key;
static void *serial_worker;

void __cilkrts_init_tls_variables(void)
{
    int status;
    /* This will be called once in serial execution before any
       Cilk parallelism so we do not need to worry about races
       on cilk_keys_defined. */
    if (cilk_keys_defined)
        return;
    status = pthread_key_create(&worker_key, 0);
    CILK_ASSERT (status == 0);
    status = pthread_key_create(&reducer_key, 0);
    CILK_ASSERT (status == 0);
    status = pthread_key_create(&tbb_interop_key, 0);
    CILK_ASSERT (status == 0);
    cilk_keys_defined = 1;
    return;
}

CILK_ABI_WORKER_PTR __cilkrts_get_tls_worker()
{
    if (__builtin_expect(cilk_keys_defined, 1))
        return (__cilkrts_worker *)pthread_getspecific(worker_key);
    else
        return serial_worker;
}

CILK_ABI_WORKER_PTR __cilkrts_get_tls_worker_fast()
{
  return (__cilkrts_worker *)pthread_getspecific(worker_key);
}

COMMON_SYSDEP struct cilkred_map *__cilkrts_get_tls_reducer(void)
{
    if (__builtin_expect(cilk_keys_defined, 1))
        return (struct cilkred_map *)pthread_getspecific(reducer_key);
    else
        return 0;
}

COMMON_SYSDEP
__cilk_tbb_stack_op_thunk *__cilkrts_get_tls_tbb_interop(void)
{
    if (__builtin_expect(cilk_keys_defined, 1))
        return (__cilk_tbb_stack_op_thunk *)
            pthread_getspecific(tbb_interop_key);
    else
        return 0;
}

COMMON_SYSDEP
void __cilkrts_set_tls_worker(__cilkrts_worker *w)
{
    if (__builtin_expect(cilk_keys_defined, 1)) {
        int status;
        status = pthread_setspecific(worker_key, w);
        CILK_ASSERT (status == 0);
        return;
    }
    else
    {
        serial_worker = w;
    }
}

COMMON_SYSDEP void __cilkrts_set_tls_reducer(struct cilkred_map *r)
{
    if (__builtin_expect(cilk_keys_defined, 1)) {
        int status;
        status = pthread_setspecific(reducer_key, r);
        CILK_ASSERT (status == 0);
        return;
    }
    abort();
}

COMMON_SYSDEP
void __cilkrts_set_tls_tbb_interop(__cilk_tbb_stack_op_thunk *t)
{
    if (__builtin_expect(cilk_keys_defined, 1)) {
        int status;
        status = pthread_setspecific(tbb_interop_key, t);
        CILK_ASSERT (status == 0);
        return;
    }
    abort();
}
#else
void __cilkrts_init_tls_variables(void)
{
}
#endif

#if defined __linux__
/*
 * Get the thread id, rather than the pid. In the case of MIC offload, it's
 * possible that we have multiple threads entering Cilk, and each has a
 * different affinity.
 */
static pid_t linux_gettid(void)
{
    return syscall(SYS_gettid);
}

/*
 * On Linux we look at the thread affinity mask and restrict ourself to one
 * thread for each of the hardware contexts to which we are bound.
 * Therefore if user does
 * % taskset 0-1 cilkProgram
 *       # restrict execution to hardware contexts zero and one
 * the Cilk program will only use two threads even if it is running on a
 * machine that has 32 hardware contexts.
 * This is the right thing to do, because the threads are restricted to two
 * hardware contexts by the affinity mask set by taskset, and if we were to
 * create extra threads they would simply oversubscribe the hardware resources
 * we can use.
 * This is particularly important on MIC in offload mode, where the affinity
 * mask is set by the offload library to force the offload code away from
 * cores that have offload support threads running on them.
 */
static int linux_get_affinity_count (int tid) 
{
    cpu_set_t process_mask;

    // Extract the thread affinity mask
    int err = sched_getaffinity (tid, sizeof(process_mask),&process_mask);

    if (0 != err)
    {
        return 0;
    }

    // We have extracted the mask OK, so now we can count the number of threads
    // in it.  This is linear in the maximum number of CPUs available, We
    // could do a logarithmic version, if we assume the format of the mask,
    // but it's not really worth it. We only call this at thread startup
    // anyway.
    int available_procs = 0;
    int i;
    for (i = 0; i < CPU_SETSIZE; i++)
    {
        if (CPU_ISSET(i, &process_mask))
        {
            available_procs++;
        }
    }

    return available_procs;
}
#endif

/*
 * __cilkrts_hardware_cpu_count
 *
 * Returns the number of available CPUs on this hardware.  This is architecture-
 * specific. 
 */

COMMON_SYSDEP int __cilkrts_hardware_cpu_count(void)
{
#if defined __linux__
    int affinity_count = linux_get_affinity_count(linux_gettid());

    return (0 != affinity_count) ? affinity_count : sysconf (_SC_NPROCESSORS_ONLN);
#elif defined __APPLE__
    int count = 0;
    int cmd[2] = { CTL_HW, HW_NCPU };
    size_t len = sizeof count;
    int status = sysctl(cmd, 2, &count, &len, 0, 0);
    assert(status >= 0);
    assert((unsigned)count == count);

    return count;
#elif defined  __FreeBSD__
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

/* timer support */
COMMON_SYSDEP unsigned long long __cilkrts_getticks(void)
{
#if defined __i386__ || defined __x86_64
    unsigned a, d; 
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d)); 
    return ((unsigned long long)a) | (((unsigned long long)d) << 32); 
#else
#   error "unimplemented cycle counter"
#endif
}

COMMON_SYSDEP void __cilkrts_short_pause(void)
{
#if __ICC >= 1110
#   if __MIC__ || __MIC2__
    _mm_delay_32(16); // stall for 16 cycles
#   else
    _mm_pause();
#   endif
#elif defined __i386__ || defined __x86_64
    __asm__("pause");
#else
#   error __cilkrts_short_pause undefined
#endif
}

COMMON_SYSDEP int __cilkrts_xchg(volatile int *ptr, int x)
{
#if defined __i386__ || defined __x86_64
    /* asm statement here works around icc bugs */
    __asm__("xchgl %0,%a1" :"=r" (x) : "r" (ptr), "0" (x) :"memory");
#else
#   error __cilkrts_xchg undefined
#endif
    return x;
}

COMMON_SYSDEP void __cilkrts_sleep(void)
{
    usleep(1);
}

COMMON_SYSDEP void __cilkrts_yield(void)
{
#if __APPLE__ || __FreeBSD__
    sched_yield();
#else
    pthread_yield();
#endif
}

COMMON_SYSDEP __STDNS size_t cilkos_getenv(char* value, __STDNS size_t vallen,
                                           const char* varname)
{
    CILK_ASSERT(value);
    CILK_ASSERT(varname);

    const char* envstr = getenv(varname);
    if (envstr)
    {
        size_t len = strlen(envstr);
        if (len > vallen - 1)
            return len + 1;

        strcpy(value, envstr);
        return len;
    }
    else
    {
        value[0] = '\0';
        return 0;
    }
}

/*
 * Unrecoverable error: Print an error message and abort execution.
 */
COMMON_SYSDEP void cilkos_error(const char *fmt, ...)
{
    va_list l;
    fflush(NULL);
    fprintf(stderr, "Cilk error: ");
    va_start(l, fmt);
    vfprintf(stderr, fmt, l);
    va_end(l);
    fprintf(stderr, "Exiting.\n");
    fflush(stderr);

    abort();
}

/*
 * Print a warning message and return.
 */
COMMON_SYSDEP void cilkos_warning(const char *fmt, ...)
{
    va_list l;
    fflush(NULL);
    fprintf(stderr, "Cilk warning: ");
    va_start(l, fmt);
    vfprintf(stderr, fmt, l);
    va_end(l);
    fflush(stderr);
}

static void __attribute__((constructor)) init_once()
{
    /*__cilkrts_debugger_notification_internal(CILK_DB_RUNTIME_LOADED);*/
    __cilkrts_init_tls_variables();
}

/* End os-unix.c */
