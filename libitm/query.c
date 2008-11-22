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
  struct gtm_transaction *tx = gtm_thr.tx;
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
  struct gtm_transaction *tx = gtm_thr.tx;
  return tx ? tx->id : _ITM_noTransactionId;
}


int REGPARM
_ITM_getThreadnum (void)
{
  static int global_num;
  int num = gtm_thr.thread_num;

  if (num == 0)
    {
      num = __sync_add_and_fetch (&global_num, 1);
      gtm_thr.thread_num = num;
    }

  return num;
}


void REGPARM NORETURN
_ITM_error (const _ITM_srcLocation * loc UNUSED, int errorCode UNUSED)
{
  abort ();
}
