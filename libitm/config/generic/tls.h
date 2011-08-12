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

#ifndef LIBITM_TLS_H
#define LIBITM_TLS_H 1

namespace GTM HIDDEN {

// All thread-local data required by the entire library.
struct gtm_thread
{
#ifndef HAVE_ARCH_GTM_THREAD_TX
  // The currently active transaction.  Elided if the target provides
  // some efficient mechanism for storing this.
  gtm_transaction *tx;
#endif
#ifndef HAVE_ARCH_GTM_THREAD_DISP
  // The dispatch table for the STM implementation currently in use.  Elided
  // if the target provides some efficient mechanism for storing this.
  abi_dispatch *disp;
#endif

  // The maximum number of free gtm_transaction structs to be kept.
  // This number must be greater than 1 in order for transaction abort
  // to be handled properly.
  static const unsigned MAX_FREE_TX = 8;

  // A queue of free gtm_transaction structs.
  void *free_tx[MAX_FREE_TX];
  unsigned free_tx_idx, free_tx_count;

  // The value returned by _ITM_getThreadnum to identify this thread.
  // ??? At present, this is densely allocated beginning with 1 and
  // we don't bother filling in this value until it is requested.
  // Which means that the value returned is, as far as the user is
  // concerned, essentially arbitrary.  We wouldn't need this at all
  // if we knew that pthread_t is integral and fits into an int.
  // ??? Consider using gettid on Linux w/ NPTL.  At least that would
  // be a value meaningful to the user.
  int thread_num;
};

// Don't access this variable directly; use the functions below.
extern __thread gtm_thread _gtm_thr;

#ifndef HAVE_ARCH_GTM_THREAD
// If the target does not provide optimized access to the thread-local
// data, simply access the TLS variable defined above.
static inline gtm_thread *setup_gtm_thr() { return &_gtm_thr; }
static inline gtm_thread *gtm_thr() { return &_gtm_thr; }
#endif

#ifndef HAVE_ARCH_GTM_THREAD_TX
// If the target does not provide optimized access to the currently
// active transaction, simply access via GTM_THR.
static inline gtm_transaction * gtm_tx() { return gtm_thr()->tx; }
static inline void set_gtm_tx(gtm_transaction *x) { gtm_thr()->tx = x; }
#endif

#ifndef HAVE_ARCH_GTM_THREAD_DISP
// If the target does not provide optimized access to the currently
// active dispatch table, simply access via GTM_THR.
static inline abi_dispatch * abi_disp() { return gtm_thr()->disp; }
static inline void set_abi_disp(abi_dispatch *x) { gtm_thr()->disp = x; }
#endif

} // namespace GTM

#endif // LIBITM_TLS_H
