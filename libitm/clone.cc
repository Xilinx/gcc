/* Copyright (C) 2009 Free Software Foundation, Inc.
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

static gtm_rwlock table_lock;

struct clone_entry
{
  void *orig, *clone;
};

struct clone_table
{
  clone_entry *table;
  size_t size;
  clone_table *next;
};

static clone_table *all_tables;

static void *
find_clone (void *ptr)
{
  clone_table *table;
  void *ret = NULL;

  table_lock.read_lock ();

  for (table = all_tables; table ; table = table->next)
    {
      clone_entry *t = table->table;
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
  table_lock.read_unlock ();
  return ret;
}


void * ITM_REGPARM
_ITM_getTMCloneOrIrrevocable (void *ptr)
{
  void *ret = find_clone (ptr);
  if (ret)
    return ret;

  gtm_tx()->serialirr_mode ();
  return ptr;
}

void * ITM_REGPARM
_ITM_getTMCloneSafe (void *ptr)
{
  void *ret = find_clone (ptr);
  if (ret == NULL)
    abort ();
  return ret;
}

static int
clone_entry_compare (const void *a, const void *b)
{
  const clone_entry *aa = (const clone_entry *)a;
  const clone_entry *bb = (const clone_entry *)b;

  if (aa->orig < bb->orig)
    return -1;
  else if (aa->orig < bb->orig)
    return 1;
  else
    return 0;
}

void
_ITM_registerTMCloneTable (void *xent, size_t size)
{
  clone_entry *ent = static_cast<clone_entry *>(xent);
  clone_table *old, *table;

  table = (clone_table *) xmalloc (sizeof (clone_table));
  table->table = ent;
  table->size = size;

  qsort (ent, size, sizeof (clone_entry), clone_entry_compare);

  old = all_tables;
  do
    {
      table->next = old;
      old = __sync_val_compare_and_swap (&all_tables, old, table);
    }
  while (old != table);
}

void
_ITM_deregisterTMCloneTable (void *xent)
{
  clone_entry *ent = static_cast<clone_entry *>(xent);
  clone_table **pprev = &all_tables;
  clone_table *tab;

  table_lock.write_lock ();

  for (pprev = &all_tables;
       tab = *pprev, tab->table != ent;
       pprev = &tab->next)
    continue;
  *pprev = tab->next;

  table_lock.write_unlock ();

  free (tab);
}
