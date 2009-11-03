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


typedef gtm_word gtm_version;
typedef gtm_word gtm_stmlock;

typedef struct r_entry
{
  gtm_version version;
  volatile gtm_stmlock *lock;
} r_entry_t;

typedef struct r_set
{
  r_entry_t *entries;
  int nb_entries;
  int size;
} r_set_t;

typedef struct w_entry
{
  struct w_entry *next;
  uintptr_t addr;
  volatile gtm_stmlock *lock;
  gtm_cacheline *value;
  gtm_version version;
} w_entry_t;

typedef struct w_set
{
  w_entry_t *entries;
  int nb_entries;
  int size;
  int reallocate;
} w_set_t;

struct gtm_method
{
  gtm_version start;
  gtm_version end;

  r_set_t r_set;
  w_set_t w_set;

  gtm_cacheline_page *cache_page;
  int n_cache_page;
};

/* ??? The locks stuff needs to be moved to common code.  */
#define OWNED_MASK			0x01
#define VERSION_MAX			(~(gtm_version)0 >> 1)

#define LOCK_GET_OWNED(LOCK)		((LOCK) & OWNED_MASK)

#define LOCK_SET_ADDR(A) \
	((gtm_stmlock)(uintptr_t)(A) | OWNED_MASK)
#define LOCK_GET_ADDR(LOCK) \
	((w_entry_t *) ((LOCK) & ~(gtm_stmlock)OWNED_MASK))
#define LOCK_GET_TIMESTAMP(LOCK)	((LOCK) >> 1)
#define LOCK_SET_TIMESTAMP(T)		((T) << 1)
#define LOCK_ARRAY_SIZE			(1ul << 20)
#define LOCK_MASK			(LOCK_ARRAY_SIZE - 1)
#define LOCK_IDX(A) \
	(((uintptr_t)(A) / CACHELINE_SIZE) & LOCK_MASK)
#define GET_LOCK(A)			(locks + LOCK_IDX(A))

static volatile gtm_stmlock locks[LOCK_ARRAY_SIZE];

#define CLOCK				(gclock)
static volatile gtm_version gclock;

#define GET_CLOCK			__sync_add_and_fetch(&CLOCK, 0)
#define FETCH_AND_INC_CLOCK		__sync_add_and_fetch(&CLOCK, 1)

#define RW_SET_SIZE			4096

/* Check if W is one of our write locks.  */
static inline bool
wbetl_local_w_entry_p (struct gtm_method *m, w_entry_t *w)
{
  return (m->w_set.entries <= w
	  && w < m->w_set.entries + m->w_set.nb_entries);
}

/* Check if stripe has been read previously.  */

static inline r_entry_t *
wbetl_has_read (struct gtm_method *m, volatile gtm_word *lock)
{
  r_entry_t *r;
  int i;

  r = m->r_set.entries;
  for (i = m->r_set.nb_entries; i > 0; i--, r++)
    if (r->lock == lock)
      return r;

  return NULL;
}

/* Validate read set, i.e. check if all read addresses are still valid now.  */

static bool
wbetl_validate (struct gtm_method *m)
{
  r_entry_t *r;
  int i;
  gtm_word l;

  __sync_synchronize ();

  r = m->r_set.entries;
  for (i = m->r_set.nb_entries; i > 0; i--, r++)
    {
      l = *r->lock;
      if (LOCK_GET_OWNED (l))
	{
	  w_entry_t *w = LOCK_GET_ADDR (l);

	  if (!wbetl_local_w_entry_p (m, w))
	    return false;
	}
      else
	{
	  if (LOCK_GET_TIMESTAMP (l) != r->version)
	    return false;
	}
    }

  return true;
}

/* Extend the snapshot range.  */

static bool
wbetl_extend (struct gtm_method *m)
{
  gtm_word now = GET_CLOCK;

  if (wbetl_validate (m))
    {
      m->end = now;
      return true;
    }
  return false;
}

/* Acquire a write lock on ADDR.  */

static gtm_cacheline *
wbetl_write_lock (uintptr_t addr)
{
  volatile gtm_stmlock *lock;
  gtm_stmlock l, l2;
  gtm_version version;
  w_entry_t *w, *prev = NULL;
  struct gtm_method *m;

  m = gtm_tx()->m;
  lock = GET_LOCK (addr);
  l = *lock;

 restart_no_load:
  if (LOCK_GET_OWNED (l))
    {
      w = LOCK_GET_ADDR (l);

      /* Did we previously write the same address?  */
      if (wbetl_local_w_entry_p (m, w))
	{
	  prev = w;
	  while (1)
	    {
	      if (addr == prev->addr)
		return prev->value;
	      if (prev->next == NULL)
		break;
	      prev = prev->next;
	    }

	  /* Get version from previous entry write set.  */
	  version = prev->version;

	  /* If there's not enough entries, we must reallocate the array,
	     which invalidates all pointers to write set entries, which
	     means we have to restart the transaction.  */
	  if (m->w_set.nb_entries == m->w_set.size)
	    {
	      m->w_set.size *= 2;
	      m->w_set.reallocate = 1;
	      GTM_restart_transaction (RESTART_REALLOCATE);
	    }

	  w = &m->w_set.entries[m->w_set.nb_entries];
	  goto do_write;
	}

      GTM_restart_transaction (RESTART_LOCKED_WRITE);
    }
  else
    {
      version = LOCK_GET_TIMESTAMP (l);

      /* We might have read an older version previously.  */
      if (version > m->end)
	{
	  if (wbetl_has_read (m, lock) != NULL)
	    GTM_restart_transaction (RESTART_VALIDATE_WRITE);
	}

      /* Extend write set, aborting to reallocate write set entries.  */
      if (m->w_set.nb_entries == m->w_set.size)
	{
	  m->w_set.size *= 2;
	  m->w_set.reallocate = 1;
	  GTM_restart_transaction (RESTART_REALLOCATE);
	}

      /* Acquire the lock.  */
      w = &m->w_set.entries[m->w_set.nb_entries];
      l2 = LOCK_SET_ADDR (w);
      l = __sync_val_compare_and_swap (lock, l, l2);
      if (l != l2)
	goto restart_no_load;
    }

 do_write:
  m->w_set.nb_entries++;
  w->addr = addr;
  w->lock = lock;
  w->version = version;
  w->next = NULL;
  if (prev != NULL)
    prev->next = w;

  {
    gtm_cacheline_page *page = m->cache_page;
    unsigned index = m->n_cache_page;
    gtm_cacheline *line;

    if (page == NULL || index == CACHELINES_PER_PAGE)
      {
        gtm_cacheline_page *npage = GTM_page_alloc ();
	npage->prev = page;
	m->cache_page = page = npage;
	m->n_cache_page = 1;
	index = 0;
      }
    else
      m->n_cache_page = index + 1;

    w->value = line = &page->lines[index];
    page->masks[index] = 0;
    gtm_cacheline_copy (line, (const gtm_cacheline *) addr);

    return line;
  }
}

/* Acquire a read lock on ADDR.  */

static gtm_cacheline *
wbetl_read_lock (uintptr_t addr, bool after_read)
{
  volatile gtm_stmlock *lock;
  gtm_stmlock l, l2;
  gtm_version version;
  w_entry_t *w;
  struct gtm_method *m;

  m = gtm_tx()->m;
  lock = GET_LOCK (addr);
  l = *lock;

 restart_no_load:
  if (LOCK_GET_OWNED (l))
    {
      w = LOCK_GET_ADDR (l);

      /* Did we previously write the same address?  */
      if (wbetl_local_w_entry_p (m, w))
	{
	  while (1)
	    {
	      if (addr == w->addr)
		return w->value;
	      if (w->next == NULL)
		return (gtm_cacheline *) addr;
	      w = w->next;
	    }
	}

      GTM_restart_transaction (RESTART_LOCKED_READ);
    }

  version = LOCK_GET_TIMESTAMP (l);

  /* If version is no longer valid, re-validate the read set.  */
  if (version > m->end)
    {
      if (!wbetl_extend (m))
	GTM_restart_transaction (RESTART_VALIDATE_READ);

      /* Verify that the version has not yet been overwritten.  The read
	 value has not yet bee added to read set and may not have been
	 checked during the extend.  */
      __sync_synchronize ();
      l2 = *lock;
      if (l != l2)
	{
	  l = l2;
	  goto restart_no_load;
	}
    }

  if (!after_read)
    {
      r_entry_t *r;

      /* Add the address and version to the read set.  */
      if (m->r_set.nb_entries == m->r_set.size)
	{
	  m->r_set.size *= 2;

	  m->r_set.entries = (r_entry_t *)
	    realloc (m->r_set.entries, m->r_set.size * sizeof(r_entry_t));
	  if (m->r_set.entries == NULL)
	    abort ();
	}
      r = &m->r_set.entries[m->r_set.nb_entries++];
      r->version = version;
      r->lock = lock;
    }

  return (gtm_cacheline *) addr;
}

static gtm_cacheline *
wbetl_after_write_lock (uintptr_t addr)
{
  volatile gtm_stmlock *lock;
  gtm_stmlock l;
  w_entry_t *w;
  struct gtm_method *m;

  m = gtm_tx()->m;
  lock = GET_LOCK (addr);

  l = *lock;
  assert (LOCK_GET_OWNED (l));

  w = LOCK_GET_ADDR (l);
  assert (wbetl_local_w_entry_p (m, w));

  while (1)
    {
      if (addr == w->addr)
	return w->value;
      w = w->next;
    }
}

static gtm_cacheline *
wbetl_R (uintptr_t addr)
{
  return wbetl_read_lock (addr, false);
}

static gtm_cacheline *
wbetl_RaR (uintptr_t addr)
{
  return wbetl_read_lock (addr, true);
}

static gtm_cacheline_mask_pair
wbetl_W (uintptr_t addr)
{
  gtm_cacheline_mask_pair pair;
  pair.line = wbetl_write_lock (addr);
  pair.mask = gtm_mask_for_line (pair.line);
  return pair;
}

static gtm_cacheline_mask_pair
wbetl_WaW (uintptr_t addr)
{
  gtm_cacheline_mask_pair pair;
  pair.line = wbetl_after_write_lock (addr);
  pair.mask = gtm_mask_for_line (pair.line);
  return pair;
}

/* Commit the transaction.  */

static bool
wbetl_trycommit (void)
{
  struct gtm_method *m = gtm_tx()->m;
  w_entry_t *w;
  gtm_word t;
  int i;

  if (m->w_set.nb_entries > 0)
    {
      /* Get commit timestamp.  */
      t = FETCH_AND_INC_CLOCK;
      if (t >= VERSION_MAX)
	abort ();

      /* Validate only if a concurrent transaction has started since.  */
      if (m->start != t - 1 && !wbetl_validate (m))
	return false;

      /* Install new versions, drop locks and set new timestamp.  */
      w = m->w_set.entries;
      for (i = m->w_set.nb_entries; i > 0; i--, w++)
	gtm_cacheline_copy_mask ((gtm_cacheline *) w->addr,
				 w->value, *gtm_mask_for_line (w->value));

      /* Only emit barrier after all cachelines are copied.  */
      gtm_ccm_write_barrier ();

      w = m->w_set.entries;
      for (i = m->w_set.nb_entries; i > 0; i--, w++)
	if (w->next == NULL)
	  *w->lock = LOCK_SET_TIMESTAMP (t);

      __sync_synchronize ();
    }

  return true;
}

static void
wbetl_rollback (void)
{
  struct gtm_method *m = gtm_tx()->m;
  w_entry_t *w;
  int i;

  /* Drop locks.  */
  w = m->w_set.entries;
  for (i = m->w_set.nb_entries; i > 0; i--, w++)
    if (w->next == NULL)
      *w->lock = LOCK_SET_TIMESTAMP (w->version);

  __sync_synchronize ();
}

static void
wbetl_init (bool first)
{
  struct gtm_method *m;

  if (first)
    {
      gtm_tx()->m = m = calloc (1, sizeof (*m));
      m->r_set.size = RW_SET_SIZE;
      m->r_set.entries = malloc (RW_SET_SIZE * sizeof(r_entry_t));
      m->w_set.size = RW_SET_SIZE;
      m->w_set.entries = malloc (RW_SET_SIZE * sizeof(w_entry_t));
    }
  else
    {
      gtm_cacheline_page *page;

      m = gtm_tx()->m;
      m->r_set.nb_entries = 0;

      m->w_set.nb_entries = 0;
      if (m->w_set.reallocate)
	{
	  m->w_set.reallocate = 0;
	  m->w_set.entries = realloc (m->w_set.entries,
				      m->w_set.size * sizeof(w_entry_t));
	}

      page = m->cache_page;
      if (page)
	{
	  /* Release all but one of the pages of cachelines.  */
	  gtm_cacheline_page *prev = page->prev;
	  if (prev)
	    {
	      gtm_cacheline_page *tail;
	      for (tail = prev; tail->prev; tail = tail->prev)
		continue;
	      page->prev = NULL;
	      GTM_page_release (prev, tail);
	    }
	  /* Start the next cacheline allocation from the beginning.  */
	  m->n_cache_page = 0;
	}
    }

  m->start = m->end = GET_CLOCK;
}

static void
wbetl_fini (void)
{
  struct gtm_method *m = gtm_tx()->m;
  gtm_cacheline_page *page, *tail;

  page = m->cache_page;
  if (page)
    {
      for (tail = page; tail->prev; tail = tail->prev)
	continue;
      GTM_page_release (page, tail);
    }

  free (m->r_set.entries);
  free (m->w_set.entries);
  free (m);
}

const struct gtm_dispatch wbetl_dispatch = {
  .R = wbetl_R,
  .RaR = wbetl_RaR,
  .RaW = wbetl_after_write_lock,
  .RfW = wbetl_write_lock,

  .W = wbetl_W,
  .WaR = wbetl_W,
  .WaW = wbetl_WaW,

  .trycommit = wbetl_trycommit,
  .rollback = wbetl_rollback,
  .init = wbetl_init,
  .fini = wbetl_fini
};
