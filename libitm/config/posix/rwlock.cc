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

#include "libitm_i.h"

namespace GTM HIDDEN {

// Initialize a new RW lock.
// ??? Move this back to the header file when constexpr is implemented.

gtm_rwlock::gtm_rwlock()
  : mutex (PTHREAD_MUTEX_INITIALIZER),
    c_readers (PTHREAD_COND_INITIALIZER),
    c_writers (PTHREAD_COND_INITIALIZER),
    c_upgrade (PTHREAD_COND_INITIALIZER),
    summary (0),
    a_readers (0),
    w_readers (0),
    w_writers (0)
{ }

gtm_rwlock::~gtm_rwlock()
{
  pthread_mutex_destroy (&this->mutex);
  pthread_cond_destroy (&this->c_readers);
  pthread_cond_destroy (&this->c_writers);
  pthread_cond_destroy (&this->c_upgrade);
}

// Acquire a RW lock for reading.

void
gtm_rwlock::read_lock ()
{
  pthread_mutex_lock (&this->mutex);

  unsigned int sum = this->summary;

  // If there is a waiting upgrade, or an active writer, we must wait.
  while (sum & (w_upgrade | a_writer | w_writer))
    {
      this->summary = sum | w_reader;
      this->w_readers++;
      pthread_cond_wait (&this->c_readers, &this->mutex);
      sum = this->summary;
      if (--this->w_readers == 0)
	sum &= ~w_reader;
    }

  // Otherwise we can acquire the lock for read.
  this->summary = sum | a_reader;
  this->a_readers++;

  pthread_mutex_unlock(&this->mutex);
}


// Acquire a RW lock for writing.

void
gtm_rwlock::write_lock ()
{
  pthread_mutex_lock (&this->mutex);

  unsigned int sum = this->summary;

  // If there is a waiting upgrade, or an active reader or writer, wait.
  while (sum & (w_upgrade | a_writer | a_reader))
    {
      this->summary = sum | w_writer;
      this->w_writers++;
      pthread_cond_wait (&this->c_writers, &this->mutex);
      sum = this->summary;
      if (--this->w_writers == 0)
	sum &= ~w_writer;
    }

  // Otherwise we can acquire the lock for write.
  this->summary = sum | a_writer;

  pthread_mutex_unlock(&this->mutex);
}


// Upgrade a RW lock that has been locked for reading to a writing lock.
// Do this without possibility of another writer incoming.  Return false
// if this attempt fails (i.e. another thread also upgraded).

bool
gtm_rwlock::write_upgrade ()
{
  pthread_mutex_lock (&this->mutex);

  unsigned int sum = this->summary;

  // If there's already someone trying to upgrade, then we fail.
  if (unlikely (sum & w_upgrade))
    {
      pthread_mutex_unlock (&this->mutex);
      return false;
    }

  // If there are more active readers, then we have to wait.
  if (--this->a_readers > 0)
    {
      this->summary = sum | w_upgrade;
      pthread_cond_wait (&this->c_upgrade, &this->mutex);
      sum = this->summary & ~w_upgrade;
      // We only return from upgrade when we've got it; don't loop.
      assert ((sum & (a_reader | a_writer)) == 0);
    }
  else
    {
      // We were the only reader.
      sum &= ~a_reader;
    }

  // Otherwise we can upgrade to writer.
  this->summary = sum | a_writer;

  pthread_mutex_unlock (&this->mutex);
  return true;
}


// Release a RW lock from reading.

void
gtm_rwlock::read_unlock ()
{
  pthread_mutex_lock (&this->mutex);

  // If there are no more active readers, we may need to wake someone.
  if (--this->a_readers == 0)
    {
      unsigned int sum = this->summary;
      this->summary = sum & ~a_reader;

      // If there is a waiting upgrade, wake it.
      if (unlikely (sum & w_upgrade))
	pthread_cond_signal (&this->c_upgrade);

      // If there is a waiting writer, wake it.
      else if (unlikely (sum & w_writer))
	pthread_cond_signal (&this->c_writers);
    }

  pthread_mutex_unlock (&this->mutex);
}


// Release a RW lock from writing.

void
gtm_rwlock::write_unlock ()
{
  pthread_mutex_lock (&this->mutex);

  unsigned int sum = this->summary;
  this->summary = sum & ~a_writer;

  // If there is a waiting writer, wake it.
  if (unlikely (sum & w_writer))
    pthread_cond_signal (&this->c_writers);

  // If there are waiting readers, wake them.
  else if (unlikely (sum & w_reader))
    pthread_cond_broadcast (&this->c_readers);

  pthread_mutex_unlock (&this->mutex);
}

} // namespace GTM
