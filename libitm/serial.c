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
  const struct gtm_dispatch *old_disp;

  if (gtm_thr.tx->state & STATE_SERIAL)
    {
      if (irrevokable)
	gtm_thr.tx->state |= STATE_IRREVOKABLE;
      return;
    }

  old_disp = gtm_thr.disp;
  gtm_thr.disp = &serial_dispatch;

  if (initial)
    gtm_rwlock_write_lock (&gtm_serial_lock);
  else
    {
      gtm_rwlock_write_upgrade (&gtm_serial_lock);
      if (old_disp->trycommit ())
	old_disp->fini ();
      else
	{
	  gtm_thr.tx->state = STATE_SERIAL;
	  GTM_restart_transaction (RESTART_VALIDATE_COMMIT);
	}
    }

  gtm_thr.tx->state = STATE_SERIAL | (irrevokable ? STATE_IRREVOKABLE : 0);
}


void REGPARM
_ITM_changeTransactionMode (_ITM_transactionState state
			    _ITM_SRCLOCATION_DEFN_2)
{
  assert (state == modeSerialIrrevocable);
  GTM_serialmode (false, true);
}
