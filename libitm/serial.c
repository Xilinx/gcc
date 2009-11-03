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


static gtm_cacheline_mask gtm_cacheline_mask_sink;

gtm_cacheline *
GTM_null_read_lock (uintptr_t ptr)
{
  return (gtm_cacheline *) ptr;
}

gtm_cacheline_mask_pair
GTM_null_write_lock (uintptr_t ptr)
{
  gtm_cacheline_mask_pair pair;
  pair.line = (gtm_cacheline *) ptr;
  pair.mask = &gtm_cacheline_mask_sink;
  return pair;
}

static bool
serial_trycommit (void)
{
  return true;
}

static void
serial_rollback (void)
{
  abort ();
}

static void
serial_init (bool first UNUSED)
{
}

static void
serial_fini (void)
{
}

const static struct gtm_dispatch serial_dispatch = 
{
  .R = GTM_null_read_lock,
  .RaR = GTM_null_read_lock,
  .RaW = GTM_null_read_lock,
  .RfW = GTM_null_read_lock,

  .W = GTM_null_write_lock,
  .WaR = GTM_null_write_lock,
  .WaW = GTM_null_write_lock,

  .trycommit = serial_trycommit,
  .rollback = serial_rollback,
  .init = serial_init,
  .fini = serial_fini,
};


/* Put the transaction into serial mode.  */

void
GTM_serialmode (bool initial, bool irrevokable)
{
  struct gtm_transaction *tx = gtm_tx();
  const struct gtm_dispatch *old_disp;

  if (tx->state & STATE_SERIAL)
    {
      if (irrevokable)
	tx->state |= STATE_IRREVOKABLE;
      return;
    }

  old_disp = gtm_disp ();
  set_gtm_disp (&serial_dispatch);

  if (initial)
    gtm_rwlock_write_lock (&gtm_serial_lock);
  else
    {
      gtm_rwlock_write_upgrade (&gtm_serial_lock);
      if (old_disp->trycommit ())
	old_disp->fini ();
      else
	{
	  tx->state = STATE_SERIAL;
	  GTM_restart_transaction (RESTART_VALIDATE_COMMIT);
	}
    }

  tx->state = STATE_SERIAL | (irrevokable ? STATE_IRREVOKABLE : 0);
}


void ITM_REGPARM
_ITM_changeTransactionMode (_ITM_transactionState state)
{
  assert (state == modeSerialIrrevocable);
  GTM_serialmode (false, true);
}
