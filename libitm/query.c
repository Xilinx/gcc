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


int REGPARM
_ITM_versionCompatible (int version)
{
  return version == _ITM_VERSION_NO;
}


const char * REGPARM
_ITM_libraryVersion (void)
{
  return "GNU libitm " _ITM_VERSION;
}


_ITM_howExecuting REGPARM
_ITM_inTransaction (void)
{
  struct gtm_transaction *tx = gtm_tx();
  if (tx)
    {
      if (tx->state & STATE_IRREVOKABLE)
	return inIrrevocableTransaction;
      else
	return inRetryableTransaction;
    }
  return outsideTransaction;
}


_ITM_transactionId_t REGPARM
_ITM_getTransactionId (void)
{
  struct gtm_transaction *tx = gtm_tx();
  return tx ? tx->id : _ITM_noTransactionId;
}


int REGPARM
_ITM_getThreadnum (void)
{
  static int global_num;
  struct gtm_thread *thr = gtm_thr();
  int num = thr->thread_num;

  if (num == 0)
    {
      num = __sync_add_and_fetch (&global_num, 1);
      thr->thread_num = num;
    }

  return num;
}


void REGPARM NORETURN
_ITM_error (const _ITM_srcLocation * loc UNUSED, int errorCode UNUSED)
{
  abort ();
}
