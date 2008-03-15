/* Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Antoniu Pop <antoniu.pop@gmail.com> 
   and Sebastian Pop <sebastian.pop@amd.com>.

   This file is part of the GNU OpenMP Library (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with libgomp; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you link this library with other files, some
   of which are compiled with GCC, to produce an executable, this library
   does not by itself cause the resulting executable to be covered by the
   GNU General Public License.  This exception does not however invalidate
   any other reasons why the executable file might be covered by the GNU
   General Public License.  */

/* This file handles streams.  */

#include "libgomp.h"
#include <stdlib.h>
#include <string.h>
#include <sched.h>

/* Returns a new stream of COUNT elements of SIZE bytes.  */

gomp_stream
gomp_stream_create (size_t size, unsigned count)
{
  gomp_stream res = (gomp_stream) gomp_malloc (sizeof (struct gomp_stream));

  res->eos_p = false;
  res->first = 0;
  res->last = 0;
  res->size = size;
  res->count = count;
  res->buffer = (char *) gomp_malloc (res->count * res->size);

  return res;
}

/* Returns the number of used elements in the stream S.  */

static inline unsigned
gomp_stream_used_space (gomp_stream s)
{
  return s->last - s->first;
}

/* Returns the number of unused element slots available in the buffer
   of the stream S.  */

static inline unsigned
gomp_stream_free_space (gomp_stream s)
{
  return s->count - (s->last - s->first);
}

/* Wait until the number of elements in the stream S reaches COUNT.  */

static inline void
gomp_stream_wait_used_space (gomp_stream s, unsigned count)
{
  while (gomp_stream_used_space (s) < count)
    sched_yield ();
}

/* Wait until the available space in the stream S reaches COUNT.  */

static inline void
gomp_stream_wait_free_space (gomp_stream s, unsigned count)
{
  while (gomp_stream_free_space (s) < count)
    sched_yield ();
} 

/* Push element ELT to stream S.  */

void
gomp_stream_push (gomp_stream s, char *elt)
{
  gomp_stream_wait_free_space (s, 1);
  memcpy (&(s->buffer[s->last]), elt, s->size);
  s->last = (s->last + 1) % s->count;
} 

/* Returns in RES the first element of the stream S.  This will not
   remove or copy the element, so a call to gomp_stream_release will
   be required.  */

char *
gomp_stream_head (gomp_stream s)
{
  gomp_stream_wait_used_space (s, 1);
  return &(s->buffer[s->first]);
}

/* Release from stream S the next element.  */

void
gomp_stream_pop (gomp_stream s)
{
  s->first = (s->first + 1) % s->count;
}

/* Returns true when producer stopped to write to stream S.  */

bool
gomp_stream_eos_p (gomp_stream s)
{
  return s->eos_p;
}

/* Producer can set End Of Stream to stream S.  */

void
gomp_stream_set_eos (gomp_stream s)
{
  s->eos_p = true;
}

/* Free stream S.  */

void
gomp_stream_destroy (gomp_stream s)
{
  free (s);
}

/* Align the producer and consumer accesses by pushing in the stream
   COUNT successive elements starting at address START.  */

void
gomp_stream_align_push (gomp_stream s, char *start, int count)
{
  int i;

  for (i = 0; i < count; ++i)
    {
      gomp_stream_push (s, start);
      start += s->size;
    }
}

/* Align the producer and consumer accesses by removing from the
   stream COUNT elements.  */

void
gomp_stream_align_pop (gomp_stream s, int count)
{
  int i;

  for (i = 0; i < count; ++i)
    gomp_stream_pop (s);
}

void *
GOMP_stream_create (size_t size, unsigned count)
{
  return gomp_stream_create (size, count);
}

void
GOMP_stream_push (void *s, void *elt)
{
  gomp_stream_push ((gomp_stream) s, (char *) elt);
}

void *
GOMP_stream_head (void *s)
{
  return gomp_stream_head ((gomp_stream) s);
}

void
GOMP_stream_pop (void *s)
{
  gomp_stream_pop ((gomp_stream) s);
}

bool
GOMP_stream_eos_p (void *s)
{
  return gomp_stream_eos_p ((gomp_stream) s);
}

void
GOMP_stream_set_eos (void *s)
{
  gomp_stream_set_eos ((gomp_stream) s);
}

void
GOMP_stream_destroy (void *s)
{
  gomp_stream_destroy ((gomp_stream) s);
}

void
GOMP_stream_align_push (void *s, void *start, int offset)
{
  gomp_stream_align_push ((gomp_stream) s, (char *) start, offset);
}

void
GOMP_stream_align_pop (void *s, int offset)
{
  gomp_stream_align_pop ((gomp_stream) s, offset);
}
