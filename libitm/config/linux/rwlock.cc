/* Copyright (C) 2008, 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libitm is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <climits>
#include "libitm_i.h"
#include "futex.h"

namespace GTM HIDDEN {

// Lock the summary bit on LOCK.  Return the contents of the summary
// word (without the summary lock bit included).

int
gtm_rwlock::lock_summary ()
{
  int o;

 restart:
  o = __sync_fetch_and_or (&this->summary, s_lock);
  if (unlikely (o & s_lock))
    {
      do
	cpu_relax ();
      while (this->summary & s_lock);
      goto restart;
    }

  return o;
}


// Acquire a RW lock for reading.

void
gtm_rwlock::read_lock ()
{
 restart:
  int o = lock_summary ();

  // If there is an active or waiting writer, then new readers must wait.
  // Increment the waiting reader count, then wait on the reader queue.
  if (unlikely (o & (a_writer | w_writer | rw_upgrade)))
    {
      int n = ++this->w_readers;
      atomic_write_barrier ();
      this->summary = o | w_reader;
      futex_wait (&this->w_readers, n);
      goto restart;
    }

  // Otherwise, we may become a reader.
  ++this->a_readers;
  atomic_write_barrier ();
  this->summary = o | a_reader;
}


// Acquire a RW lock for writing.

void
gtm_rwlock::write_lock ()
{
  int o, n;

 restart:
  o = this->summary;

  // If anyone is manipulating the summary lock, the rest of the
  // data structure is volatile.
  if (unlikely (o & s_lock))
    {
      cpu_relax ();
      goto restart;
    }

  // If there is an active reader or active writer, then new writers must wait.
  // Increment the waiting writer count, then wait on the writer queue.
  if (unlikely (o & (a_writer | a_reader | rw_upgrade)))
    {
      // Grab the summary lock.  We'll need it for incrementing
      // the waiting reader.
      n = o | s_lock;
      if (!__sync_bool_compare_and_swap (&this->summary, o, n))
	goto restart;

      n = ++this->w_writers;
      atomic_write_barrier ();
      this->summary = o | w_writer;
      futex_wait (&this->w_writers, n);
      goto restart;
    }

  // Otherwise, we may become a writer.
  n = o | a_writer;
  if (unlikely (!__sync_bool_compare_and_swap (&this->summary, o, n)))
    goto restart;
}


// Upgrade a RW lock that has been locked for reading to a writing lock.
// Do this without possibility of another writer incoming.  Return false
// if this attempt fails (i.e. another thread also upgraded).

bool
gtm_rwlock::write_upgrade ()
{
  int o, n;

 restart:
  o = this->summary;

  // If anyone is manipulating the summary lock, the rest of the
  // data structure is volatile.
  if (unlikely (o & s_lock))
    {
      cpu_relax ();
      goto restart;
    }

  // If there's already someone trying to upgrade, then we fail.
  if (unlikely (o & rw_upgrade))
    return false;

  // Grab the summary lock.  We'll need it for manipulating the
  // active reader count or the waiting writer count.
  n = o | s_lock;
  if (unlikely (!__sync_bool_compare_and_swap (&this->summary, o, n)))
    goto restart;

  // If there are more active readers, then we have to wait.
  if (--this->a_readers > 0)
    {
      atomic_write_barrier ();
      o |= rw_upgrade;
      this->summary = o;
      do
	{
	  futex_wait (&this->summary, o);
	  o = this->summary;
	}
      while (o & a_reader);
    }

  atomic_write_barrier ();
  o &= ~(a_reader | rw_upgrade);
  o |= a_writer;
  this->summary = o;
  return true;
}


// Release a RW lock from reading.

void
gtm_rwlock::read_unlock ()
{
  int o = lock_summary ();

  // If there are still active readers, nothing else to do.
  if (--this->a_readers > 0)
    {
      atomic_write_barrier ();
      this->summary = o;
      return;
    }
  o &= ~a_reader;

  // If there is a waiting upgrade, wake it.
  if (unlikely (o & rw_upgrade))
    {
      atomic_write_barrier ();
      this->summary = o;
      futex_wake (&this->summary, 1);
      return;
    }

  // If there is a waiting writer, wake it.
  if (unlikely (o & w_writer))
    {
      if (--this->w_writers == 0)
	o &= ~w_writer;
      atomic_write_barrier ();
      this->summary = o;
      futex_wake (&this->w_writers, 1);
      return;
    }

  atomic_write_barrier ();
  this->summary = o;
}


// Release a RW lock from writing.

void
gtm_rwlock::write_unlock ()
{
  int o = lock_summary ();
  o &= ~a_writer;

  // If there is a waiting writer, wake it.
  if (unlikely (o & w_writer))
    {
      if (--this->w_writers == 0)
	o &= ~w_writer;
      atomic_write_barrier ();
      this->summary = o;
      futex_wake (&this->w_writers, 1);
      return;
    }

  // If there are waiting readers, wake them.
  if (unlikely (o & w_reader))
    {
      this->w_readers = 0;
      atomic_write_barrier ();
      this->summary = o & ~w_reader;
      futex_wake (&this->w_readers, INT_MAX);
      return;
    }

  this->summary = o;
}

} // namespace GTM
