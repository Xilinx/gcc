/* Implementation header for bounds-checking runtime library.
   Copyright (C) 2002, 2003, 2004 Free Software Foundation, Inc.
   Contributed by Frank Ch. Eigler <fche@redhat.com>
   and Graydon Hoare <graydon@redhat.com>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#ifndef __BOUNDS_IMPL_H
#define __BOUNDS_IMPL_H

#ifdef __BOUNDS_CHECKING_ON
#error "Do not compile this file with -fbounds-checking!"
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#else
typedef __bounds_uintptr_t uintptr_t;
#endif

#define TRACE_ON 0
#define VERBOSE_TRACE_ON 0

/* Private definitions related to bounds-runtime.h  */

#define __MF_TYPE_MAX __MF_TYPE_GUESS

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/* Address calculation macros.  */

#define MINPTR ((uintptr_t) 0)
#define MAXPTR (~ (uintptr_t) 0)

/* Clamp the addition/subtraction of uintptr_t's to [MINPTR,MAXPTR] */
#define CLAMPSUB(ptr,offset) \
  (((uintptr_t) ptr) >= (offset) ? \
    ((uintptr_t) ptr)-((uintptr_t) offset) : \
    MINPTR)
#define CLAMPADD(ptr,offset) \
  (((uintptr_t) ptr) <= MAXPTR-(offset) ? \
    ((uintptr_t) ptr)+((uintptr_t) offset) : \
    MAXPTR)
#define CLAMPSZ(ptr,size) \
  ((size) ? \
    (((uintptr_t) ptr) <= MAXPTR-(size)+1 ? \
      ((uintptr_t) ptr)+((uintptr_t) size) - 1 : \
	  MAXPTR) : \
    ((uintptr_t) ptr))

/* ------------------------------------------------------------------------ */
/* Type definitions. */
/* ------------------------------------------------------------------------ */

/* The bounds_state type codes describe recursion and initialization order.

   reentrant means we are inside a bounds-runtime support routine, such as
   __bounds_register, and thus there should be no calls to any wrapped functions,
   such as the wrapped malloc.  This indicates a bug if it occurs.
   in_malloc means we are inside a real malloc call inside a wrapped malloc
   call, and thus there should be no calls to any wrapped functions like the
   wrapped mmap.  This happens on some systems due to how the system libraries
   are constructed.  */

enum __bounds_state_enum { active, reentrant, in_malloc }; 

#ifdef PIC

/* This is a table of dynamically resolved function pointers. */

struct __bounds_dynamic_entry
{
  void *pointer;
  char *name;
  char *version;
};

/* The definition of the array (bounds-runtime.c) must match the enums!  */
extern struct __bounds_dynamic_entry __bounds_dynamic[];
enum __bounds_dynamic_index
{
  dyn_calloc, dyn_free, dyn_malloc, dyn_mmap,
  dyn_munmap, dyn_realloc,
  dyn_INITRESOLVE,  /* Marker for last init-time resolution. */
};

#endif /* PIC */

/* ------------------------------------------------------------------------ */
/* Private global variables. */
/* ------------------------------------------------------------------------ */

#define LOCKTH() do {} while (0)
#define UNLOCKTH() do {} while (0)

extern enum __bounds_state_enum __bounds_state_1;
static inline enum __bounds_state_enum __bounds_get_state (void)
{
  return __bounds_state_1;
}
static inline void __bounds_set_state (enum __bounds_state_enum s)
{
  __bounds_state_1 = s;
}

extern int __bounds_starting_p;

/* ------------------------------------------------------------------------ */
/* Utility macros. */
/* ------------------------------------------------------------------------ */

#define UNLIKELY(e) (__builtin_expect (!!(e), 0))
#define LIKELY(e) (__builtin_expect (!!(e), 1))

#define VERBOSE_TRACE(...) \
  do { if (UNLIKELY (VERBOSE_TRACE_ON)) {  \
      fprintf (stderr, "bounds: "); \
      fprintf (stderr, __VA_ARGS__); \
    } } while (0)
#define TRACE(...) \
  do { if (UNLIKELY (TRACE_ON)) { \
      fprintf (stderr, "bounds: "); \
      fprintf (stderr, __VA_ARGS__); \
    } } while (0)

#define __BOUNDS_FREEQ_MAX 256
#define __BOUNDS_FREEQ_LEN 4

/* ------------------------------------------------------------------------ */
/* Wrapping and redirection                                                 */
/* ------------------------------------------------------------------------ */
/* Mudflap redirects a number of libc functions into itself, for "cheap"
   verification (eg. strcpy, bzero, memcpy) and also to register /
   unregister regions of memory as they are manipulated by the program
   (eg. malloc/free, mmap/munmap).

   There are two methods of wrapping.

   (1) The static method involves a list of -wrap=foo flags being passed to
   the linker, which then links references to "foo" to the symbol
   "__wrap_foo", and links references to "__real_foo" to the symbol "foo".
   When compiled without -DPIC, libbounds.a contains such __wrap_foo
   functions which delegate to __real_foo functions in libc to get their
   work done.

   (2) The dynamic method involves providing a definition of symbol foo in
   libbounds.so and linking it earlier in the compiler command line,
   before libc.so. The function "foo" in libbounds must then call
   dlsym(RTLD_NEXT, "foo") to acquire a pointer to the "real" libc foo, or
   at least the "next" foo in the dynamic link resolution order.

   We switch between these two techniques by the presence of the -DPIC
   #define passed in by libtool when building libmudflap.
*/

#ifdef PIC

extern void __bounds_resolve_single_dynamic (struct __bounds_dynamic_entry *);

#define _GNU_SOURCE
#include <dlfcn.h>

#define WRAPPER(ret, fname, ...)                      \
ret __wrap_ ## fname (__VA_ARGS__)                    \
    __attribute__ (( alias  (#fname)  ));             \
ret __real_ ## fname (__VA_ARGS__)                    \
    __attribute__ (( alias  (#fname)  ));             \
ret fname (__VA_ARGS__)
#define DECLARE(ty, fname, ...)                       \
 typedef ty (*__bounds_fn_ ## fname) (__VA_ARGS__);       \
 extern ty __bounds_0fn_ ## fname (__VA_ARGS__);
#define CALL_REAL(fname, ...)                         \
  ({__bounds_starting_p \
     ? __bounds_0fn_ ## fname (__VA_ARGS__) \
    : (__bounds_resolve_single_dynamic (& __bounds_dynamic[dyn_ ## fname]), \
       (((__bounds_fn_ ## fname)(__bounds_dynamic[dyn_ ## fname].pointer)) (__VA_ARGS__)));})
#define CALL_BACKUP(fname, ...)                       \
  __bounds_0fn_ ## fname(__VA_ARGS__)

#else /* not PIC --> static library */

#define WRAPPER(ret, fname, ...)            \
ret __wrap_ ## fname (__VA_ARGS__)
#define DECLARE(ty, fname, ...)             \
 extern ty __real_ ## fname (__VA_ARGS__)
#define CALL_REAL(fname, ...)               \
 __real_ ## fname (__VA_ARGS__)
#define CALL_BACKUP(fname, ...)             \
  __real_ ## fname(__VA_ARGS__)

#endif /* PIC */

/* WRAPPER2 is for functions intercepted via macros at compile time. */
#define WRAPPER2(ret, fname, ...)                     \
ret __boundswrap_ ## fname (__VA_ARGS__)

/* Utility macros for bounds-hooks*.c */

#define BOUNDS_VALIDATE_EXTENT(value,size,acc,context) \
 do { \
  if (size > 0 && acc == __MF_CHECK_WRITE) \
    __bounds_check ((void *) (value), (void *) (value), (size), acc, "(" context ")"); \
 } while (0)
#define BEGIN_PROTECT(fname, ...)       \
  if (UNLIKELY (__bounds_starting_p)) \
  {                                         \
    return CALL_BACKUP(fname, __VA_ARGS__); \
  }                                         \
  else if (UNLIKELY (__bounds_get_state () == reentrant))   \
  {                                         \
    return CALL_REAL(fname, __VA_ARGS__);   \
  }                                         \
  else if (UNLIKELY (__bounds_get_state () == in_malloc))   \
  {                                         \
    return CALL_REAL(fname, __VA_ARGS__);   \
  }                                         \
  else                                      \
  {                                         \
    TRACE ("%s\n", __PRETTY_FUNCTION__); \
  }

/* There is an assumption here that these will only be called in routines
   that call BEGIN_PROTECT at the start, and hence the state must always
   be active when BEGIN_MALLOC_PROTECT is called.  */
#define BEGIN_MALLOC_PROTECT() \
  __bounds_set_state (in_malloc)

#define END_MALLOC_PROTECT() \
  __bounds_set_state (active)

/* Unlocked variants of main entry points from bounds-runtime.h.  */
extern void __bounds_ulock_check (void *referent, void *ptr, size_t sz, 
                                  int type, const char *location);
extern void *__bounds_ulock_arith (void* base, void* addr, const char* location);
extern int __bounds_ulock_comp (void* left, void* right, 
                                int op_type, const char* location);
extern void __bounds_ulock_register (void *ptr, size_t sz, int type, const char *name);
extern void __bounds_ulock_unregister (void *ptr, size_t sz, int type);
extern void *__bounds_ulock_cast (void *cast, const char *location);

#endif /* __BOUNDS_IMPL_H */
