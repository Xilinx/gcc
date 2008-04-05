/* Mudflap: narrow-pointer bounds-checking by tree rewriting.
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


#include "config.h"

#ifndef HAVE_SOCKLEN_T
#define socklen_t int
#endif


/* These attempt to coax various unix flavours to declare all our
   needed tidbits in the system headers.  */
#if !defined(__FreeBSD__)  && !defined(__APPLE__)
#define _POSIX_SOURCE
#endif /* Some BSDs break <sys/socket.h> if this is defined. */
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#define _BSD_TYPES
#define __EXTENSIONS__
#define _ALL_SOURCE
#define _LARGE_FILE_API
#define _XOPEN_SOURCE_EXTENDED 1

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

#include "bounds-runtime.h"
#include "bounds-impl.h"

#ifdef __BOUNDS_CHECKING_ON
#error "Do not compile this file with -fbounds-checking!"
#endif


/* Memory allocation related hook functions.  Some of these are
   intercepted via linker wrapping or symbol interposition.  Others
   use plain macros in bounds-runtime.h.  */


#if PIC
/* A special bootstrap variant. */
void *
__bounds_0fn_malloc (size_t c)
{
  enum foo { BS = 4096, NB=10 };
  static char bufs[NB][BS];
  static unsigned bufs_used[NB];
  unsigned i;

  for (i=0; i<NB; i++)
    {
      if (! bufs_used[i] && c < BS)
	{
	  bufs_used[i] = 1;
	  return & bufs[i][0];
	}
    }
  return NULL;
}
#endif


#undef malloc
WRAPPER(void *, malloc, size_t c)
{
  DECLARE(void *, malloc, size_t c);
  void *result;
  BEGIN_PROTECT (malloc, c);

  BEGIN_MALLOC_PROTECT ();
  result = (char *) CALL_REAL (malloc, CLAMPSZ (c,0));
  END_MALLOC_PROTECT ();

  if (LIKELY(result))
      __bounds_register (result, c, __MF_TYPE_HEAP, "malloc region");

  return result;
}


#ifdef PIC
/* A special bootstrap variant. */
void *
__bounds_0fn_calloc (size_t c, size_t n)
{
  return __bounds_0fn_malloc (c * n);
}
#endif


#undef calloc
WRAPPER(void *, calloc, size_t c, size_t n)
{
  size_t size;
  DECLARE(void *, calloc, size_t, size_t);
  DECLARE(void *, malloc, size_t);
  DECLARE(void *, memset, void *, int, size_t);
  char *result;
  BEGIN_PROTECT (calloc, c, n);

  size = CLAMPSZ((c * n),0); /* XXX: CLAMPMUL */
  BEGIN_MALLOC_PROTECT ();
  result = (char *) CALL_REAL (malloc, size);
  END_MALLOC_PROTECT ();

  if (LIKELY(result))
    memset (result, 0, size);

  if (LIKELY(result))
      __bounds_register (result, c*n /* XXX: clamp */, __MF_TYPE_HEAP_I, "calloc region");

  return result;
}


#if PIC
/* A special bootstrap variant. */
void *
__bounds_0fn_realloc (void *buf, size_t c)
{
  return NULL;
}
#endif


#undef realloc
WRAPPER(void *, realloc, void *buf, size_t c)
{
  DECLARE(void * , realloc, void *, size_t);
  char *base = buf;
  char *result;
  BEGIN_PROTECT (realloc, buf, c);

  BEGIN_MALLOC_PROTECT ();
  result = (char *) CALL_REAL (realloc, base, CLAMPSZ (c,0));
  END_MALLOC_PROTECT ();

  LOCKTH ();
  __bounds_set_state (reentrant);

  if (LIKELY(buf))
    __bounds_ulock_unregister (buf, 0, __MF_TYPE_HEAP_I);
  /* NB: underlying region may have been __MF_TYPE_HEAP. */

  if (LIKELY(result))
    __bounds_ulock_register (result, c, __MF_TYPE_HEAP_I, "realloc region");

  __bounds_set_state (active);
  UNLOCKTH ();

  return result;
}


#if PIC
/* A special bootstrap variant. */
void
__bounds_0fn_free (void *buf)
{
  return;
}
#endif

#undef free
WRAPPER(void, free, void *buf)
{
  /* Use a circular queue to delay some number (__BOUNDS_FREEQ_LEN) of free()s.  */
  static void *free_queue [__BOUNDS_FREEQ_MAX];
  static unsigned free_ptr = 0;
  static int freeq_initialized = 0;
  DECLARE(void, free, void *);

  BEGIN_PROTECT (free, buf);

  if (UNLIKELY(buf == NULL))
    return;

  LOCKTH ();
  if (UNLIKELY(!freeq_initialized))
    {
      memset (free_queue, 0,
		     __BOUNDS_FREEQ_MAX * sizeof (void *));
      freeq_initialized = 1;
    }
  UNLOCKTH ();

  __bounds_unregister (buf, 0, __MF_TYPE_HEAP_I);
  /* NB: underlying region may have been __MF_TYPE_HEAP. */

  if (LIKELY(__BOUNDS_FREEQ_LEN > 0))
    {
      char *freeme = NULL;
      LOCKTH ();
      if (free_queue [free_ptr] != NULL)
	    freeme = free_queue [free_ptr];
      free_queue [free_ptr] = buf;
      free_ptr = (free_ptr == (__BOUNDS_FREEQ_LEN-1) ? 0 : free_ptr + 1);
      UNLOCKTH ();
      if (freeme)
	  {
	    VERBOSE_TRACE ("freeing deferred pointer %p\n", (void *) freeme);
		BEGIN_MALLOC_PROTECT ();
		CALL_REAL (free, freeme);
		END_MALLOC_PROTECT ();
	  }
    }
  else
    {
	  VERBOSE_TRACE ("freeing pointer %p\n", (void *) buf);
      BEGIN_MALLOC_PROTECT ();
      CALL_REAL (free, buf);
      END_MALLOC_PROTECT ();
    }
}


/* We can only wrap mmap if the target supports it.  Likewise for munmap.
   We assume we have both if we have mmap.  */
#ifdef HAVE_MMAP

#if PIC
/* A special bootstrap variant. */
void *
__bounds_0fn_mmap (void *start, size_t l, int prot, int f, int fd, off_t off)
{
  return (void *) -1;
}
#endif


#undef mmap
WRAPPER(void *, mmap,
	void  *start,  size_t length, int prot,
	int flags, int fd, off_t offset)
{
  DECLARE(void *, mmap, void *, size_t, int,
			    int, int, off_t);
  void *result;
  BEGIN_PROTECT (mmap, start, length, prot, flags, fd, offset);

  result = CALL_REAL (mmap, start, length, prot,
			flags, fd, offset);

  /*
  VERBOSE_TRACE ("mmap (%08lx, %08lx, ...) => %08lx\n",
		 (uintptr_t) start, (uintptr_t) length,
		 (uintptr_t) result);
  */

  if (result != (void *)-1)
    {
      /* Register each page as a heap object.  Why not register it all
	 as a single segment?  That's so that a later munmap() call
	 can unmap individual pages.  XXX: would __MF_TYPE_GUESS make
	 this more automatic?  */
      size_t ps = getpagesize ();
      uintptr_t base = (uintptr_t) result;
      uintptr_t offset;

      for (offset=0; offset<length; offset+=ps)
	{
	  /* XXX: Unaccessed HEAP pages are reported as leaks.  Is this
	     appropriate for unaccessed mmap pages? */
	  __bounds_register ((void *) CLAMPADD (base, offset), ps,
			 __MF_TYPE_HEAP_I, "mmap page");
	}
    }

  return result;
}


#if PIC
/* A special bootstrap variant. */
int
__bounds_0fn_munmap (void *start, size_t length)
{
  return -1;
}
#endif


#undef munmap
WRAPPER(int , munmap, void *start, size_t length)
{
  DECLARE(int, munmap, void *, size_t);
  int result;
  BEGIN_PROTECT (munmap, start, length);

  result = CALL_REAL (munmap, start, length);

  /*
  VERBOSE_TRACE ("munmap (%08lx, %08lx, ...) => %08lx\n",
		 (uintptr_t) start, (uintptr_t) length,
		 (uintptr_t) result);
  */

  if (result == 0)
    {
      /* Unregister each page as a heap object.  */
      size_t ps = getpagesize ();
      uintptr_t base = (uintptr_t) start & (~ (ps - 1)); /* page align */
      uintptr_t offset;

      for (offset=0; offset<length; offset+=ps)
	__bounds_unregister ((void *) CLAMPADD (base, offset), ps, __MF_TYPE_HEAP_I);
    }
  return result;
}
#endif /* HAVE_MMAP */


/* This wrapper is a little different, as it's called indirectly from
   __mf_fini also to clean up pending allocations.  */
void *
__bounds_wrap_alloca_indirect (size_t c)
{
  DECLARE (void *, malloc, size_t);
  DECLARE (void, free, void *);

  /* This struct, a linked list, tracks alloca'd objects.  The newest
     object is at the head of the list.  If we detect that we've
     popped a few levels of stack, then the listed objects are freed
     as needed.  NB: The tracking struct is allocated with
     real_malloc; the user data with wrap_malloc.
  */
  struct alloca_tracking { void *ptr; void *stack; struct alloca_tracking* next; };
  static struct alloca_tracking *alloca_history = NULL;

  void *stack = __builtin_frame_address (0);
  void *result;
  struct alloca_tracking *track;

  TRACE ("%s\n", __PRETTY_FUNCTION__);
  VERBOSE_TRACE ("alloca stack level %p\n", (void *) stack);

  /* XXX: thread locking! */

  /* Free any previously alloca'd blocks that belong to deeper-nested functions,
     which must therefore have exited by now.  */

#define DEEPER_THAN < /* XXX: for x86; steal find_stack_direction() from libiberty/alloca.c */

  while (alloca_history &&
	 ((uintptr_t) alloca_history->stack DEEPER_THAN (uintptr_t) stack))
    {
      struct alloca_tracking *next = alloca_history->next;
      __bounds_unregister (alloca_history->ptr, 0, __MF_TYPE_HEAP);
      BEGIN_MALLOC_PROTECT ();
      CALL_REAL (free, alloca_history->ptr);
      CALL_REAL (free, alloca_history);
      END_MALLOC_PROTECT ();
      alloca_history = next;
    }

  /* Allocate new block.  */
  result = NULL;
  if (LIKELY (c > 0)) /* alloca(0) causes no allocation.  */
    {
      BEGIN_MALLOC_PROTECT ();
      track = (struct alloca_tracking *) CALL_REAL (malloc,
						    sizeof (struct alloca_tracking));
      END_MALLOC_PROTECT ();
      if (LIKELY (track != NULL))
	{
	  BEGIN_MALLOC_PROTECT ();
	  result = CALL_REAL (malloc, c);
	  END_MALLOC_PROTECT ();
	  if (UNLIKELY (result == NULL))
	    {
	      BEGIN_MALLOC_PROTECT ();
	      CALL_REAL (free, track);
	      END_MALLOC_PROTECT ();
	      /* Too bad.  XXX: What about errno?  */
	    }
	  else
	    {
	      __bounds_register (result, c, __MF_TYPE_HEAP, "alloca region");
	      track->ptr = result;
	      track->stack = stack;
	      track->next = alloca_history;
	      alloca_history = track;
	    }
	}
    }

  return result;
}


#undef alloca
WRAPPER(void *, alloca, size_t c)
{
  return __bounds_wrap_alloca_indirect (c);
}

