/* Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.
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


using namespace GTM;

__thread gtm_thread GTM::_gtm_thr;
gtm_rwlock GTM::gtm_transaction::serial_lock;

gtm_stmlock GTM::gtm_stmlock_array[LOCK_ARRAY_SIZE];
gtm_version GTM::gtm_clock;

/* ??? Move elsewhere when we figure out library initialization.  */
uint64_t GTM::gtm_spin_count_var = 1000;

static _ITM_transactionId_t global_tid;

/* Allocate a transaction structure.  Reuse an old one if possible.  */

void *
GTM::gtm_transaction::operator new (size_t s)
{
  gtm_thread *thr = gtm_thr ();
  void *tx;

  assert(s == sizeof(gtm_transaction));

  if (thr->free_tx_count == 0)
    tx = xmalloc (sizeof (gtm_transaction));
  else
    {
      thr->free_tx_count--;
      tx = thr->free_tx[thr->free_tx_idx];
      thr->free_tx_idx = (thr->free_tx_idx + 1) % gtm_thread::MAX_FREE_TX;
    }
  memset (tx, 0, sizeof (gtm_transaction));

  return tx;
}

/* Queue a transaction structure for freeing.  We never free the given
   transaction immediately -- this is a requirement of abortTransaction
   as the jmpbuf is used immediately after calling this function.  Thus
   the requirement that this queue be per-thread.  */

void
GTM::gtm_transaction::operator delete(void *tx)
{
  gtm_thread *thr = gtm_thr ();
  unsigned idx
    = (thr->free_tx_idx + thr->free_tx_count) % gtm_thread::MAX_FREE_TX;

  if (thr->free_tx_count == gtm_thread::MAX_FREE_TX)
    {
      thr->free_tx_idx = (thr->free_tx_idx + 1) % gtm_thread::MAX_FREE_TX;
      free (thr->free_tx[idx]);
    }
  else
    thr->free_tx_count++;

  thr->free_tx[idx] = tx;
}

#ifndef HAVE_64BIT_SYNC_BUILTINS
static pthread_mutex_t global_tid_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

uint32_t
GTM::gtm_transaction::begin_transaction (uint32_t prop, const gtm_jmpbuf *jb)
{
  static const _ITM_transactionId_t tid_block_size = 1 << 16;

  gtm_transaction *tx;
  abi_dispatch *disp;
  uint32_t ret;

  gtm_thread *thr = setup_gtm_thr ();

  tx = new gtm_transaction;

  tx->prop = prop;
  tx->prev = gtm_tx();
  if (tx->prev)
    tx->nesting = tx->prev->nesting + 1;

  // As long as we have not exhausted a previously allocated block of TIDs,
  // we can avoid an atomic operation on a shared cacheline.
  if (thr->local_tid & (tid_block_size - 1))
    tx->id = thr->local_tid++;
  else
    {
#ifdef HAVE_64BIT_SYNC_BUILTINS
      tx->id = __sync_add_and_fetch (&global_tid, tid_block_size);
      thr->local_tid = tx->id + 1;
#else
      pthread_mutex_lock (&global_tid_lock);
      global_tid += tid_block_size;
      tx->id = global_tid;
      thr->local_tid = tx->id + 1;
      pthread_mutex_unlock (&global_tid_lock);
#endif
    }

  tx->jb = *jb;

  set_gtm_tx (tx);

  // ??? pr_undoLogCode is not properly defined in the ABI. Are barriers
  // omitted because they are not necessary (e.g., a transaction on thread-
  // local data) or because the compiler thinks that some kind of global
  // synchronization might perform better?
  if (unlikely(prop & pr_undoLogCode))
    GTM_fatal("pr_undoLogCode not supported");

  if ((prop & pr_doesGoIrrevocable) || !(prop & pr_instrumentedCode))
    tx->state = (STATE_SERIAL | STATE_IRREVOCABLE);

  else
    disp = tx->decide_begin_dispatch ();

  if (tx->state & STATE_SERIAL)
    {
      serial_lock.write_lock ();

      if (tx->state & STATE_IRREVOCABLE)
        disp = dispatch_serialirr ();
      else
        disp = dispatch_serial ();

      ret = a_runUninstrumentedCode;
      if ((prop & pr_multiwayCode) == pr_instrumentedCode)
	ret = a_runInstrumentedCode;
    }
  else
    {
      serial_lock.read_lock ();
      ret = a_runInstrumentedCode | a_saveLiveVariables;
    }

  set_abi_disp (disp);

  return ret;
}

void
GTM::gtm_transaction::rollback ()
{
  abi_disp()->rollback ();
  rollback_local ();

  rollback_user_actions();
  commit_allocations (true);
  revert_cpp_exceptions ();

  if (this->eh_in_flight)
    {
      _Unwind_DeleteException ((_Unwind_Exception *) this->eh_in_flight);
      this->eh_in_flight = NULL;
    }
}

void ITM_REGPARM
_ITM_abortTransaction (_ITM_abortReason reason)
{
  gtm_transaction *tx = gtm_tx();

  assert (reason == userAbort);
  assert ((tx->prop & pr_hasNoAbort) == 0);

  if (tx->state & gtm_transaction::STATE_IRREVOCABLE)
    abort ();

  tx->rollback ();
  abi_disp()->fini ();

  if (tx->state & gtm_transaction::STATE_SERIAL)
    gtm_transaction::serial_lock.write_unlock ();
  else
    gtm_transaction::serial_lock.read_unlock ();

  set_gtm_tx (tx->prev);
  delete tx;

  GTM_longjmp (&tx->jb, a_abortTransaction | a_restoreLiveVariables, tx->prop);
}

bool
GTM::gtm_transaction::trycommit ()
{
  if (abi_disp()->trycommit ())
    {
      commit_local ();
      commit_user_actions();
      commit_allocations (false);
      return true;
    }
  return false;
}

bool
GTM::gtm_transaction::trycommit_and_finalize ()
{
  if (trycommit ())
    {
      abi_disp()->fini ();
      set_gtm_tx (this->prev);
      delete this;
      if (this->state & gtm_transaction::STATE_SERIAL)
	gtm_transaction::serial_lock.write_unlock ();
      else
	gtm_transaction::serial_lock.read_unlock ();
      return true;
    }
  return false;
}

void ITM_NORETURN
GTM::gtm_transaction::restart (gtm_restart_reason r)
{
  uint32_t actions;

  rollback ();
  decide_retry_strategy (r);

  actions = a_runInstrumentedCode | a_restoreLiveVariables;
  if ((this->prop & pr_uninstrumentedCode)
      && (this->state & gtm_transaction::STATE_IRREVOCABLE))
    actions = a_runUninstrumentedCode | a_restoreLiveVariables;

  GTM_longjmp (&this->jb, actions, this->prop);
}

void ITM_REGPARM
_ITM_commitTransaction(void)
{
  gtm_transaction *tx = gtm_tx();
  if (!tx->trycommit_and_finalize ())
    tx->restart (RESTART_VALIDATE_COMMIT);
}

void ITM_REGPARM
_ITM_commitTransactionEH(void *exc_ptr)
{
  gtm_transaction *tx = gtm_tx();
  if (!tx->trycommit_and_finalize ())
    {
      tx->eh_in_flight = exc_ptr;
      tx->restart (RESTART_VALIDATE_COMMIT);
    }
}
