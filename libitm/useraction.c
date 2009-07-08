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

  for (tx = gtm_tx(); tx->id != tid; tx = tx->prev)
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
  struct gtm_transaction *tx = gtm_tx();
  struct gtm_user_action *a;

  a = malloc (sizeof (*a));
  a->next = tx->undo_actions;
  a->fn = fn;
  a->arg = arg;
  tx->undo_actions = a;
}
