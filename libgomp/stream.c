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

unsigned
gomp_stream_used_space (gomp_stream s)
{
  return s->last - s->first;
}

/* Returns the number of unused element slots available in the buffer
   of the stream S.  */

unsigned
gomp_stream_free_space (gomp_stream s)
{
  return s->count - (s->last - s->first);
}

/* Wait until the number of elements in the stream S reaches COUNT.  */

void
gomp_stream_wait_used_space (gomp_stream s, unsigned count)
{
  while (gomp_stream_used_space (s) < count)
    sched_yield ();
}

/* Wait until the available space in the stream S reaches COUNT.  */

void
gomp_stream_wait_free_space (gomp_stream s, unsigned count)
{
  while (gomp_stream_free_space (s) < count)
    sched_yield ();
} 

/* Push element ELT to stream S.  */

void
gomp_stream_push (gomp_stream s, void *elt)
{
  gomp_stream_wait_free_space (s, 1);
  memcpy (&(s->buffer[s->last]), elt, s->size);
  s->last = (s->last + 1) % s->count;
} 

/* Returns in RES the first element of the stream S.  */

void
gomp_stream_pop (gomp_stream s, void *res)
{
  gomp_stream_wait_used_space (s, 1);
  memcpy (res, &(s->buffer[s->first]), s->size);
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
