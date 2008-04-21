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

/* Set to L1 line cache size.  */
#define SIZE_LOCAL_BUFFER 64

/* Returns a new stream of COUNT * SIZE_LOCAL_BUFFER elements.  Each
   element is of size SIZE bytes.  Returns NULL when the allocation
   fails or when COUNT is less than 2.  */

gomp_stream
gomp_stream_create (size_t size, unsigned count)
{
  gomp_stream s;

  /* There should be enough place for two sliding windows.  */
  if (count < 2)
    return NULL;

  s = (gomp_stream) gomp_malloc (sizeof (struct gomp_stream));

  if (!s)
    return NULL;

  s->eos_p = false;
  s->read_buffer_index = 0;
  s->write_buffer_index = 0;
  s->write_index = 0;
  s->read_index = 0;
  s->size_elt = size;
  s->size_local_buffer = SIZE_LOCAL_BUFFER;
  s->capacity = count * s->size_local_buffer;
  s->buffer = (char *) gomp_malloc (s->capacity);

  if (!s->buffer)
    {
      free (s);
      return NULL;
    }

  return s;
}

static inline unsigned
next_window (gomp_stream s, unsigned index)
{
  unsigned next = index + s->size_local_buffer;
  return ((next >= s->capacity) ? 0 : next);
}

static inline void 
slide_read_window (gomp_stream s)
{
  unsigned next = next_window (s, s->read_buffer_index);

  s->read_buffer_index = next;
  s->read_index = next;
}

static inline void
slide_write_window (gomp_stream s)
{
  unsigned next = next_window (s, s->write_buffer_index);

  while (s->read_buffer_index == next)
    sched_yield ();

  s->write_buffer_index = next;
  s->write_index = next;
}

/* Returns the number of read elements in the read sliding window of
   stream S.  */

static inline unsigned
read_bytes_in_read_window (gomp_stream s)
{
  return s->read_index - s->read_buffer_index;
}

/* Returns the number of written elements in the write sliding window
   of stream S.  */

static inline unsigned
written_bytes_in_write_window (gomp_stream s)
{
  return s->write_index - s->write_buffer_index;
}

/* Push element ELT to stream S.  */

void
gomp_stream_push (gomp_stream s, char *elt)
{
  if (written_bytes_in_write_window (s) + s->size_elt > s->size_local_buffer)
    slide_write_window (s);

  memcpy (s->buffer + s->write_index, elt, s->size_elt);
  s->write_index += s->size_elt;
}

/* Release from stream S the next element.  */

void
gomp_stream_pop (gomp_stream s)
{
  if (read_bytes_in_read_window (s) + 2 * s->size_elt > s->size_local_buffer)
    slide_read_window (s);
  else
    s->read_index += s->size_elt;
}

/* Wait until the producer has slided the write window in stream S.  */

static inline void
wait_used_space (gomp_stream s)
{
  while (s->read_buffer_index == s->write_buffer_index)
    sched_yield ();
}

/* Returns the first element of the stream S.  Don't remove the
   element: for that, a call to gomp_stream_pop is needed.  */

char *
gomp_stream_head (gomp_stream s)
{
  wait_used_space (s);
  return s->buffer + s->read_index;
}

/* Returns true when there are no more elements to be read from the
   stream S.  */

bool
gomp_stream_eos_p (gomp_stream s)
{
  return (s->eos_p && (s->read_index == s->write_index));
}

/* Producer can set End Of Stream to stream S.  The producer has to
   slide the write window if it wrote something.  */

void
gomp_stream_set_eos (gomp_stream s)
{
  if (written_bytes_in_write_window (s) > 0)
    slide_write_window (s);

  s->eos_p = true;
}

/* Free stream S.  */

void
gomp_stream_destroy (gomp_stream s)
{
  /* No need to synchronize here: the consumer that detects when eos
     is set, and based on that it decides to destroy the stream.  */

  free (s->buffer);
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
      start += s->size_elt;
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
