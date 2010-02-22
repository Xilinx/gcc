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

#ifndef GTM_RWLOCK_H
#define GTM_RWLOCK_H

namespace GTM HIDDEN {

// This datastructure is similar to the POSIX pthread_rwlock_t except
// that we also provide for upgrading a reader->writer lock, with a
// positive indication of failure (another writer acquired the lock
// before we were able to acquire).
//
// In this implementation, rw upgrade is given highest priority access,
// and writers are given priority over readers.

class gtm_rwlock
{
 private:
  // A collection of bits that may be set in SUMMARY:
  static const int s_lock	= 1;	// The strucure as a whole is locked.
  static const int a_writer	= 2;	// An active writer.
  static const int w_writer	= 4;	// The w_writers field != 0
  static const int a_reader	= 8;	// The a_readers field != 0
  static const int w_reader	= 16;	// The w_readers field != 0
  static const int rw_upgrade	= 32;	// A reader waiting for upgrade.

  // All fields must be "int", since they're all given to the futex syscall.
  int summary;
  int a_readers;		// Number of active readers.
  int w_readers;		// Number of waiting readers.
  int w_writers;       		// Number of waiting writers.

  int lock_summary ();

 public:
  // ??? Uncomment if we have non-static users or if constexpr is supported.
  // gtm_rwlock() : summary(0), a_readers(0), w_readers(0), w_writers(0) { }

  void read_lock ();
  void read_unlock ();

  void write_lock ();
  void write_unlock ();

  bool write_upgrade ();
};

} // namespace GTM

#endif // GTM_RWLOCK_H
