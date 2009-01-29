/* Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libitm is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with libitm; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you link this library with other files, some
   of which are compiled with GCC, to produce an executable, this library
   does not by itself cause the resulting executable to be covered by the
   GNU General Public License.  This exception does not however invalidate
   any other reasons why the executable file might be covered by the GNU
   General Public License.  */

#include "libitm.h"

static gtm_rwlock table_lock;

struct clone_entry
{
  void *orig, *clone;
};

struct clone_table
{
  struct clone_entry *table;
  size_t size;
  struct clone_table *next;
};

static struct clone_table *all_tables;

static void * REGPARM
GTM_find_clone (void *ptr)
{
  struct clone_table *table;
  void *ret = NULL;

  gtm_rwlock_read_lock (&table_lock);

  for (table = all_tables; table ; table = table->next)
    {
      struct clone_entry *t = table->table;
      size_t lo = 0, hi = table->size, i;

      /* Quick test for whether PTR is present in this table.  */
      if (ptr < t[0].orig || ptr > t[hi - 1].orig)
	continue;

      /* Otherwise binary search.  */
      while (lo < hi)
	{
	  i = (lo + hi) / 2;
	  if (ptr < t[i].orig)
	    hi = i;
	  else if (ptr > t[i].orig)
	    lo = i + 1;
	  else
	    {
	      ret = t[i].clone;
	      goto found;
	    }
	}

      /* Given the quick test above, if we don't find the entry in
	 this table then it doesn't exist.  */
      break;
    }

 found:
  gtm_rwlock_read_unlock (&table_lock);
  return ret;
}


void * REGPARM
_ITM_getTMCloneOrIrrevokable (void *ptr)
{
  void *ret = GTM_find_clone (ptr);
  if (ret)
    return ret;

  GTM_serialmode (false, true);
  return ptr;
}

void * REGPARM
_ITM_getTMCloneSafe (void *ptr)
{
  void *ret = GTM_find_clone (ptr);
  if (ret == NULL)
    abort ();
  return ret;
}

static int
clone_entry_compare (const void *a, const void *b)
{
  const struct clone_entry *aa = (const struct clone_entry *)a;
  const struct clone_entry *bb = (const struct clone_entry *)b;

  if (aa->orig < bb->orig)
    return -1;
  else if (aa->orig < bb->orig)
    return 1;
  else
    return 0;
}

void
_ITM_registerTMCloneTable (void *t, size_t size)
{
  struct clone_table *old, *table = malloc (sizeof (*table));

  table->table = t;
  table->size = size;

  qsort (t, size, sizeof (struct clone_entry), clone_entry_compare);

  old = all_tables;
  do
    {
      table->next = old;
      old = __sync_val_compare_and_swap (&all_tables, old, table);
    }
  while (old != table);
}

void
_ITM_deregisterTMCloneTable (void *t)
{
  struct clone_table **pprev = &all_tables;

  gtm_rwlock_write_lock (&table_lock);

  for (pprev = &all_tables; (*pprev)->table != t; pprev = &(*pprev)->next)
    continue;
  *pprev = (*pprev)->next;

  gtm_rwlock_write_unlock (&table_lock);
}
