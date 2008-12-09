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


void
GTM_commit_local (void)
{
  struct gtm_transaction *tx = gtm_thr.tx;
  struct gtm_local_undo **local_undo = tx->local_undo;
  size_t i, n = tx->n_local_undo;

  if (n > 0)
    {
      for (i = 0; i < n; ++i)
	free (local_undo[i]);
      tx->n_local_undo = 0;
    }
  if (local_undo)
    {
      free (local_undo);
      tx->local_undo = NULL;
      tx->size_local_undo = 0;
    }
}

static void
rollback_local (struct gtm_local_undo *undo)
{
  memcpy (undo->addr, undo->saved, undo->len);
}

void
GTM_rollback_local (void)
{
  struct gtm_transaction *tx = gtm_thr.tx;
  struct gtm_local_undo **local_undo = tx->local_undo;
  size_t i, n = tx->n_local_undo;

  if (n > 0)
    {
      for (i = n; i-- > 0; )
	{
	  rollback_local (local_undo[i]);
	  free (local_undo[i]);
	}
      tx->n_local_undo = 0;
    }
}

static void *
alloc_local (void *addr, size_t len)
{
  struct gtm_transaction *tx = gtm_thr.tx;
  struct gtm_local_undo *undo;

  undo = malloc (sizeof (struct gtm_local_undo) + len);
  undo->addr = addr;
  undo->len = len;

  if (tx->local_undo == NULL)
    {
      tx->size_local_undo = 32;
      tx->local_undo = malloc (sizeof (undo) * tx->size_local_undo);
    }
  else if (tx->n_local_undo == tx->size_local_undo)
    {
      tx->size_local_undo *= 2;
      tx->local_undo = realloc (tx->local_undo,
				sizeof (undo) * tx->size_local_undo);
    }
  tx->local_undo[tx->n_local_undo++] = undo;

  return undo->saved;
}

#define _ITM_LOG_DEF(T)							\
void REGPARM _ITM_TYPE_ATTR(T) _ITM_L##T (const _ITM_TYPE_##T *ptr)	\
{									\
  *(_ITM_TYPE_##T *)alloc_local((void *)ptr, sizeof(_ITM_TYPE_##T)) = *ptr; \
}

_ITM_ALL_TYPES(_ITM_LOG_DEF)

void REGPARM
_ITM_LB (const void *ptr, size_t len)
{
  memcpy(alloc_local ((void *)ptr, len), ptr, len);
}
