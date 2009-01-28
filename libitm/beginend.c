/* Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

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

#include "libitm.h"


__thread struct gtm_thread gtm_thr;
gtm_rwlock gtm_serial_lock;

/* ??? Move elsewhere when we figure out library initialization.  */
unsigned long long gtm_spin_count_var = 1000;

static uint32_t global_tid;

/* Allocate a transaction structure.  Reuse an old one if possible.  */

static struct gtm_transaction *
alloc_tx (void)
{
  struct gtm_transaction *tx;

  if (gtm_thr.free_tx_count == 0)
    tx = malloc (sizeof (*tx));
  else
    {
      gtm_thr.free_tx_count--;
      tx = gtm_thr.free_tx[gtm_thr.free_tx_idx];
      gtm_thr.free_tx_idx = (gtm_thr.free_tx_idx + 1) % MAX_FREE_TX;
    }

  return tx;
}

/* Queue a transaction structure for freeing.  We never free the given
   transaction immediately -- this is a requirement of abortTransaction
   as the jmpbuf is used immediately after calling this function.  Thus
   the requirement that this queue be per-thread.  */

static void
free_tx (struct gtm_transaction *tx)
{
  unsigned idx = (gtm_thr.free_tx_idx + gtm_thr.free_tx_count) % MAX_FREE_TX;

  if (gtm_thr.free_tx_count == MAX_FREE_TX)
    {
      gtm_thr.free_tx_idx = (gtm_thr.free_tx_idx + 1) % MAX_FREE_TX;
      free (gtm_thr.free_tx[idx]);
    }
  else
    gtm_thr.free_tx_count++;

  gtm_thr.free_tx[idx] = tx;
}


uint32_t REGPARM
GTM_begin_transaction (uint32_t prop, const struct gtm_jmpbuf *jb)
{
  struct gtm_transaction *tx = alloc_tx ();

  memset (tx, 0, sizeof (*tx));

  tx->prop = prop;
  tx->prev = gtm_thr.tx;
  if (tx->prev)
    tx->nesting = tx->prev->nesting + 1;
  tx->id = __sync_add_and_fetch (&global_tid, 1);
  tx->jb = *jb;

  gtm_thr.tx = tx;

  if ((prop & pr_doesGoIrrevocable) || !(prop & pr_instrumentedCode))
    {
      GTM_serialmode (true);
      return (prop & pr_uninstrumentedCode
	      ? a_runUninstrumentedCode : a_runInstrumentedCode);
    }

  /* ??? Probably want some environment variable to choose the default
     STM implementation once we have more than one implemented.  */
  gtm_thr.disp = &wbetl_dispatch;
  gtm_thr.disp->init (true);

  gtm_rwlock_read_lock (&gtm_serial_lock);

  return a_runInstrumentedCode | a_saveLiveVariables;
}

static void
GTM_rollback_transaction (void)
{
  gtm_thr.disp->rollback ();
  GTM_rollback_local ();
  GTM_free_actions (&gtm_thr.tx->commit_actions);
  GTM_run_actions (&gtm_thr.tx->undo_actions);
}

void REGPARM
_ITM_rollbackTransaction (_ITM_SRCLOCATION_DEFN_1)
{
  assert ((gtm_thr.tx->prop & pr_hasNoAbort) == 0);
  assert ((gtm_thr.tx->state & STATE_ABORTING) == 0);

  GTM_rollback_transaction ();
  gtm_thr.tx->state |= STATE_ABORTING;
}

void REGPARM
_ITM_abortTransaction (_ITM_abortReason reason _ITM_SRCLOCATION_DEFN_2)
{
  struct gtm_transaction *tx;

  assert (reason == userAbort);
  assert ((gtm_thr.tx->prop & pr_hasNoAbort) == 0);
  assert ((gtm_thr.tx->state & STATE_ABORTING) == 0);

  GTM_rollback_transaction ();
  gtm_thr.disp->fini ();

  if (gtm_thr.tx->state & STATE_SERIAL)
    gtm_rwlock_write_unlock (&gtm_serial_lock);
  else
    gtm_rwlock_read_unlock (&gtm_serial_lock);

  tx = gtm_thr.tx;
  gtm_thr.tx = tx->prev;
  free_tx (tx);

  GTM_longjmp (&tx->jb, a_abortTransaction | a_restoreLiveVariables, tx->prop);
}

static bool
GTM_trycommit_transaction (void)
{
  if (gtm_thr.disp->trycommit ())
    {
      GTM_commit_local ();
      GTM_free_actions (&gtm_thr.tx->undo_actions);
      GTM_run_actions (&gtm_thr.tx->commit_actions);
      return true;
    }
  return false;
}

bool REGPARM
_ITM_tryCommitTransaction (_ITM_SRCLOCATION_DEFN_1)
{
  assert ((gtm_thr.tx->state & STATE_ABORTING) == 0);
  return GTM_trycommit_transaction ();
}

void REGPARM NORETURN
GTM_restart_transaction (enum restart_reason r)
{
  struct gtm_transaction *tx = gtm_thr.tx;
  uint32_t actions;

  GTM_rollback_transaction ();
  GTM_decide_retry_strategy (r);

  actions = a_runInstrumentedCode | a_restoreLiveVariables;
  if ((tx->prop & (pr_doesGoIrrevocable | pr_uninstrumentedCode))
      == (pr_doesGoIrrevocable | pr_uninstrumentedCode))
    actions = a_runUninstrumentedCode | a_restoreLiveVariables;

  GTM_longjmp (&tx->jb, actions, tx->prop);
}

void REGPARM
_ITM_commitTransaction(_ITM_SRCLOCATION_DEFN_1)
{
  struct gtm_transaction *tx = gtm_thr.tx;

  if ((tx->state & STATE_ABORTING) || GTM_trycommit_transaction ())
    {
      gtm_thr.disp->fini ();
      gtm_thr.tx = tx->prev;
      free_tx (tx);
    }
  else
    GTM_restart_transaction (RESTART_VALIDATE_COMMIT);
}
