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

#include "libitm.h"


#define SERIAL(T) \
static _ITM_TYPE_##T REGPARM _ITM_TYPE_ATTR(T)				\
serial_R##T(const _ITM_TYPE_##T *ptr)					\
{									\
  return *ptr;								\
}									\
static void REGPARM _ITM_TYPE_ATTR(T)					\
serial_W##T(_ITM_TYPE_##T *ptr, _ITM_TYPE_##T val)			\
{									\
  *ptr = val;								\
}

_ITM_ALL_TYPES (SERIAL)

#undef SERIAL

static void REGPARM serial_memcpy (void *dst, const void *src, size_t len)
{
  memcpy (dst, src, len);
}

static void REGPARM serial_memmove (void *dst, const void *src, size_t len)
{
  memmove (dst, src, len);
}

static void REGPARM serial_memset (void *dst, int src, size_t len)
{
  memset (dst, src, len);
}

static bool serial_trycommit (void)
{
  return true;
}

static void serial_rollback (void)
{
  abort ();
}

static void REGPARM serial_init (bool first UNUSED)
{
}

static void serial_fini (void)
{
}

const static struct gtm_dispatch serial_dispatch = 
{
#define _ITM_READ(R, T)		.R##T = serial_R##T,
#define _ITM_WRITE(W, T)	.W##T = serial_W##T,
  _ITM_ALL_TYPES (_ITM_ALL_READS)
  _ITM_ALL_TYPES (_ITM_ALL_WRITES)
#undef _ITM_READ
#undef _ITM_WRITE

#define _ITM_MCPY_RW(FN, R, W)	.FN##R##W = serial_##FN,
  _ITM_MCPY(memcpy)
  _ITM_MCPY(memmove)
#undef _ITM_MCPY_RW

#define _ITM_MSET_W(FN, W)	.FN##W = serial_##FN,
  _ITM_MSET(memset)
#undef _ITM_MSET_W

  .trycommit = serial_trycommit,
  .rollback = serial_rollback,
  .init = serial_init,
  .fini = serial_fini,
};


/* Put the transaction into serial mode.  */

void REGPARM
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


void REGPARM
_ITM_changeTransactionMode (_ITM_transactionState state)
{
  assert (state == modeSerialIrrevocable);
  GTM_serialmode (false, true);
}
