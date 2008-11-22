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


void REGPARM
GTM_run_actions (struct gtm_user_action **list)
{
  struct gtm_user_action *a = *list;

  if (a == NULL)
    return;
  *list = NULL;

  do
    {
      struct gtm_user_action *n = a->next;
      a->fn (a->arg);
      free (a);
      a = n;
    }
  while (a);
}


void REGPARM
GTM_free_actions (struct gtm_user_action **list)
{
  struct gtm_user_action *a = *list;

  if (a == NULL)
    return;
  *list = NULL;

  do
    {
      struct gtm_user_action *n = a->next;
      free (a);
      a = n;
    }
  while (a);
}


void REGPARM
_ITM_addUserCommitAction(_ITM_userCommitFunction fn,
			 _ITM_transactionId_t tid, void *arg)
{
  struct gtm_transaction *tx;
  struct gtm_user_action *a;

  for (tx = gtm_thr.tx; tx->id != tid; tx = tx->prev)
    continue;

  a = malloc (sizeof (*a));
  a->next = tx->commit_actions;
  a->fn = fn;
  a->arg = arg;
  tx->commit_actions = a;
}


void REGPARM
_ITM_addUserUndoAction(_ITM_userUndoFunction fn, void * arg)
{
  struct gtm_transaction *tx = gtm_thr.tx;
  struct gtm_user_action *a;

  a = malloc (sizeof (*a));
  a->next = tx->undo_actions;
  a->fn = fn;
  a->arg = arg;
  tx->undo_actions = a;
}
