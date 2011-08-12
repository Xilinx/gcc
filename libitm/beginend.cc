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
gtm_transaction *GTM::gtm_transaction::list_of_tx = 0;

gtm_stmlock GTM::gtm_stmlock_array[LOCK_ARRAY_SIZE];
gtm_version GTM::gtm_clock;

/* ??? Move elsewhere when we figure out library initialization.  */
uint64_t GTM::gtm_spin_count_var = 1000;

static _ITM_transactionId_t global_tid;

// Provides a on-thread-exit callback used to release per-thread data.
static pthread_key_t tx_release_key;
static pthread_once_t tx_release_once = PTHREAD_ONCE_INIT;


/* Allocate a transaction structure.  */

void *
GTM::gtm_transaction::operator new (size_t s)
{
  void *tx;

  assert(s == sizeof(gtm_transaction));

  tx = xmalloc (sizeof (gtm_transaction), true);
  memset (tx, 0, sizeof (gtm_transaction));

  return tx;
}

/* Free the given transaction. Raises an error if the transaction is still
   in use.  */

void
GTM::gtm_transaction::operator delete(void *tx)
{
  free(tx);
}

static void
thread_exit_handler(void *dummy __attribute__((unused)))
{
  gtm_transaction *tx = gtm_tx();
  if (tx)
    {
      if (tx->nesting > 0)
        GTM_fatal("Thread exit while a transaction is still active.");

      // Deregister this transaction.
      gtm_transaction::serial_lock.write_lock ();
      gtm_transaction **prev = &gtm_transaction::list_of_tx;
      for (; *prev; prev = &(*prev)->next_tx)
        {
          if (*prev == tx)
            {
              *prev = (*prev)->next_tx;
              break;
            }
        }
      gtm_transaction::serial_lock.write_unlock ();

      delete tx;
      set_gtm_tx(NULL);
    }
  if (pthread_setspecific(tx_release_key, NULL))
    GTM_fatal("Setting tx release TLS key failed.");
}

static void
thread_exit_init()
{
  if (pthread_key_create(&tx_release_key, thread_exit_handler))
    GTM_fatal("Creating tx release TLS key failed.");
}


#ifndef HAVE_64BIT_SYNC_BUILTINS
static pthread_mutex_t global_tid_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static inline uint32_t choose_code_path(uint32_t prop, abi_dispatch *disp)
{
  if ((prop & pr_uninstrumentedCode) && disp->can_run_uninstrumented_code())
    return a_runUninstrumentedCode;
  else
    return a_runInstrumentedCode;
}

uint32_t
GTM::gtm_transaction::begin_transaction (uint32_t prop, const gtm_jmpbuf *jb)
{
  static const _ITM_transactionId_t tid_block_size = 1 << 16;

  gtm_transaction *tx;
  abi_dispatch *disp;
  uint32_t ret;

  // ??? pr_undoLogCode is not properly defined in the ABI. Are barriers
  // omitted because they are not necessary (e.g., a transaction on thread-
  // local data) or because the compiler thinks that some kind of global
  // synchronization might perform better?
  if (unlikely(prop & pr_undoLogCode))
    GTM_fatal("pr_undoLogCode not supported");

  tx = gtm_tx();
  if (unlikely(tx == NULL))
    {
      tx = new gtm_transaction;
      set_gtm_tx(tx);

      // Register this transaction with the list of all threads' transactions.
      serial_lock.write_lock ();
      tx->next_tx = list_of_tx;
      list_of_tx = tx;
      serial_lock.write_unlock ();

      if (pthread_once(&tx_release_once, thread_exit_init))
        GTM_fatal("Initializing tx release TLS key failed.");
      // Any non-null value is sufficient to trigger releasing of this
      // transaction when the current thread terminates.
      if (pthread_setspecific(tx_release_key, tx))
        GTM_fatal("Setting tx release TLS key failed.");
    }

  if (tx->nesting > 0)
    {
      // This is a nested transaction.
      // Check prop compatibility:
      // The ABI requires pr_hasNoFloatUpdate, pr_hasNoVectorUpdate,
      // pr_hasNoIrrevocable, pr_aWBarriersOmitted, pr_RaRBarriersOmitted, and
      // pr_hasNoSimpleReads to hold for the full dynamic scope of a
      // transaction. We could check that these are set for the nested
      // transaction if they are also set for the parent transaction, but the
      // ABI does not require these flags to be set if they could be set,
      // so the check could be too strict.
      // ??? For pr_readOnly, lexical or dynamic scope is unspecified.

      if (prop & pr_hasNoAbort)
        {
          // We can use flat nesting, so elide this transaction.
          if (!(prop & pr_instrumentedCode))
            {
              if (!(tx->state & STATE_SERIAL) ||
                  !(tx->state & STATE_IRREVOCABLE))
                tx->serialirr_mode();
            }
          // Increment nesting level after checking that we have a method that
          // allows us to continue.
          tx->nesting++;
          return choose_code_path(prop, abi_disp());
        }

      // The transaction might abort, so use closed nesting if possible.
      // pr_hasNoAbort has lexical scope, so the compiler should really have
      // generated an instrumented code path.
      assert(prop & pr_instrumentedCode);

      // Create a checkpoint of the current transaction.
      gtm_transaction_cp *cp = tx->parent_txns.push();
      cp->save(tx);
      new (&tx->alloc_actions) aa_tree<uintptr_t, gtm_alloc_action>();

      // Check whether the current method actually supports closed nesting.
      // If we can switch to another one, do so.
      // If not, we assume that actual aborts are infrequent, and rather
      // restart in _ITM_abortTransaction when we really have to.
      disp = abi_disp();
      if (!disp->closed_nesting())
        {
          // ??? Should we elide the transaction if there is no alternative
          // method that supports closed nesting? If we do, we need to set
          // some flag to prevent _ITM_abortTransaction from aborting the
          // wrong transaction (i.e., some parent transaction).
          abi_dispatch *cn_disp = disp->closed_nesting_alternative();
          if (cn_disp)
            {
              disp = cn_disp;
              set_abi_disp(disp);
            }
        }
    }
  else
    {
      // Outermost transaction
      // TODO Pay more attention to prop flags (eg, *omitted) when selecting
      // dispatch.
      if ((prop & pr_doesGoIrrevocable) || !(prop & pr_instrumentedCode))
        tx->state = (STATE_SERIAL | STATE_IRREVOCABLE);

      else
        disp = tx->decide_begin_dispatch (prop);

      if (tx->state & STATE_SERIAL)
        {
          serial_lock.write_lock ();

          if (tx->state & STATE_IRREVOCABLE)
            disp = dispatch_serialirr ();
          else
            disp = dispatch_serial ();
        }
      else
        {
          serial_lock.read_lock (tx);
        }

      set_abi_disp (disp);
    }

  // Initialization that is common for outermost and nested transactions.
  tx->prop = prop;
  tx->nesting++;

  tx->jb = *jb;

  // As long as we have not exhausted a previously allocated block of TIDs,
  // we can avoid an atomic operation on a shared cacheline.
  if (tx->local_tid & (tid_block_size - 1))
    tx->id = tx->local_tid++;
  else
    {
#ifdef HAVE_64BIT_SYNC_BUILTINS
      tx->id = __sync_add_and_fetch (&global_tid, tid_block_size);
      tx->local_tid = tx->id + 1;
#else
      pthread_mutex_lock (&global_tid_lock);
      global_tid += tid_block_size;
      tx->id = global_tid;
      tx->local_tid = tx->id + 1;
      pthread_mutex_unlock (&global_tid_lock);
#endif
    }

  // Determine the code path to run. Only irrevocable transactions cannot be
  // restarted, so all other transactions need to save live variables.
  ret = choose_code_path(prop, disp);
  if (!(tx->state & STATE_IRREVOCABLE))
    ret |= a_saveLiveVariables;
  return ret;
}


void
GTM::gtm_transaction_cp::save(gtm_transaction* tx)
{
  // Save everything that we might have to restore on restarts or aborts.
  jb = tx->jb;
  local_undo_size = tx->local_undo.size();
  memcpy(&alloc_actions, &tx->alloc_actions, sizeof(alloc_actions));
  user_actions_size = tx->user_actions.size();
  id = tx->id;
  prop = tx->prop;
  cxa_catch_count = tx->cxa_catch_count;
  cxa_unthrown = tx->cxa_unthrown;
  disp = abi_disp();
  nesting = tx->nesting;
}

void
GTM::gtm_transaction_cp::commit(gtm_transaction* tx)
{
  // Restore state that is not persistent across commits. Exception handling,
  // information, nesting level, and any logs do not need to be restored on
  // commits of nested transactions. Allocation actions must be committed
  // before committing the snapshot.
  tx->jb = jb;
  memcpy(&tx->alloc_actions, &alloc_actions, sizeof(alloc_actions));
  tx->id = id;
  tx->prop = prop;
}


void
GTM::gtm_transaction::rollback (gtm_transaction_cp *cp)
{
  abi_disp()->rollback (cp);

  rollback_local (cp ? cp->local_undo_size : 0);
  rollback_user_actions (cp ? cp->user_actions_size : 0);
  commit_allocations (true, (cp ? &cp->alloc_actions : 0));
  revert_cpp_exceptions (cp);

  if (cp)
    {
      // Roll back the rest of the state to the checkpoint.
      jb = cp->jb;
      id = cp->id;
      prop = cp->prop;
      if (cp->disp != abi_disp())
        set_abi_disp(cp->disp);
      memcpy(&alloc_actions, &cp->alloc_actions, sizeof(alloc_actions));
      nesting = cp->nesting;
    }
  else
    {
      // Roll back to the outermost transaction.
      // Restore the jump buffer and transaction properties, which we will
      // need for the longjmp used to restart or abort the transaction.
      if (parent_txns.size() > 0)
        {
          jb = parent_txns[0].jb;
          id = parent_txns[0].id;
          prop = parent_txns[0].prop;
        }
      // Reset the transaction. Do not reset this->state, which is handled by
      // the callers. Note that we reset the transaction to the point after
      // having executed begin_transaction (we will return from it), so the
      // nesting level must be one, not zero.
      nesting = 1;
      parent_txns.clear();
    }

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

  // If the current method does not support closed nesting, we are nested, and
  // we can restart, then restart with a method that supports closed nesting.
  abi_dispatch *disp = abi_disp();
  if (!disp->closed_nesting())
    tx->restart(RESTART_CLOSED_NESTING);

  // Roll back to innermost transaction.
  if (tx->parent_txns.size() > 0)
    {
      // The innermost transaction is a nested transaction.
      gtm_transaction_cp *cp = tx->parent_txns.pop();
      uint32_t longjmp_prop = tx->prop;
      gtm_jmpbuf longjmp_jb = tx->jb;

      tx->rollback (cp);
      abi_disp()->fini ();

      // Jump to nested transaction (use the saved jump buffer).
      GTM_longjmp (&longjmp_jb, a_abortTransaction | a_restoreLiveVariables,
          longjmp_prop);
    }
  else
    {
      // There is no nested transaction, so roll back to outermost transaction.
      tx->rollback ();
      abi_disp()->fini ();

      // Aborting an outermost transaction finishes execution of the whole
      // transaction. Therefore, reset transaction state.
      if (tx->state & gtm_transaction::STATE_SERIAL)
        gtm_transaction::serial_lock.write_unlock ();
      else
        gtm_transaction::serial_lock.read_unlock (tx);
      tx->state = 0;

      GTM_longjmp (&tx->jb, a_abortTransaction | a_restoreLiveVariables,
          tx->prop);
    }
}

bool
GTM::gtm_transaction::trycommit ()
{
  nesting--;

  // Skip any real commit for elided transactions.
  if (nesting > 0 && (parent_txns.size() == 0 ||
      nesting > parent_txns[parent_txns.size() - 1].nesting))
    return true;

  if (nesting > 0)
    {
      // Commit of a closed-nested transaction. Remove one checkpoint and add
      // any effects of this transaction to the parent transaction.
      gtm_transaction_cp *cp = parent_txns.pop();
      commit_allocations(false, &cp->alloc_actions);
      cp->commit(this);
      return true;
    }

  // Commit of an outermost transaction.
  if (abi_disp()->trycommit ())
    {
      commit_local ();
      // FIXME: run after ensuring privatization safety:
      commit_user_actions ();
      commit_allocations (false, 0);
      abi_disp()->fini ();

      // Reset transaction state.
      cxa_catch_count = 0;
      cxa_unthrown = NULL;

      // TODO can release SI mode before committing user actions? If so,
      // we can release before ensuring privatization safety too.
      if (state & gtm_transaction::STATE_SERIAL)
	gtm_transaction::serial_lock.write_unlock ();
      else
	gtm_transaction::serial_lock.read_unlock (this);
      state = 0;

      return true;
    }
  return false;
}

void ITM_NORETURN
GTM::gtm_transaction::restart (gtm_restart_reason r)
{
  // Roll back to outermost transaction. Do not reset transaction state because
  // we will continue executing this transaction.
  rollback ();
  decide_retry_strategy (r);

  GTM_longjmp (&this->jb,
      choose_code_path(prop, abi_disp()) | a_restoreLiveVariables,
      this->prop);
}

void ITM_REGPARM
_ITM_commitTransaction(void)
{
  gtm_transaction *tx = gtm_tx();
  if (!tx->trycommit ())
    tx->restart (RESTART_VALIDATE_COMMIT);
}

void ITM_REGPARM
_ITM_commitTransactionEH(void *exc_ptr)
{
  gtm_transaction *tx = gtm_tx();
  if (!tx->trycommit ())
    {
      tx->eh_in_flight = exc_ptr;
      tx->restart (RESTART_VALIDATE_COMMIT);
    }
}
