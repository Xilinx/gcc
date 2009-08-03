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

#include "libitm.h"
#include <endian.h>


typedef unsigned long word;

typedef struct r_entry
{
  word version;
  volatile word *lock;
} r_entry_t;

typedef struct r_set
{
  r_entry_t *entries;
  int nb_entries;
  int size;
} r_set_t;

typedef struct w_entry
{
  volatile word *addr;
  word value;
  word version;
  volatile word *lock;
  struct w_entry *next;
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
  word start;
  word end;
  r_set_t r_set;
  w_set_t w_set;
};

#define OWNED_MASK			0x01
#define VERSION_MAX			(~(word)0 >> 1)

#define LOCK_GET_OWNED(LOCK)		((LOCK) & OWNED_MASK)

#define LOCK_SET_ADDR(A)		((A) | OWNED_MASK)
#define LOCK_GET_ADDR(LOCK)		((LOCK) & ~(word)OWNED_MASK)
#define LOCK_GET_TIMESTAMP(LOCK)	((LOCK) >> 1)
#define LOCK_SET_TIMESTAMP(T)		((T) << 1)
#define LOCK_ARRAY_SIZE			(1ul << 20)
#define LOCK_MASK			(LOCK_ARRAY_SIZE - 1)
#define LOCK_SHIFT			((sizeof(word) == 4) ? 2 : 3)
#define LOCK_IDX(A)		(((word)(A) >> LOCK_SHIFT) & LOCK_MASK)
#define GET_LOCK(A)			(locks + LOCK_IDX(A))

static volatile word locks[LOCK_ARRAY_SIZE];

#define CLOCK				(gclock)
static volatile word gclock;

#define GET_CLOCK			__sync_add_and_fetch(&CLOCK, 0)
#define FETCH_AND_INC_CLOCK		__sync_add_and_fetch(&CLOCK, 1)

#define RW_SET_SIZE			4096



/* Check if stripe has been read previously.  */

static inline r_entry_t *
wbetl_has_read(struct gtm_method *m, volatile word *lock)
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
wbetl_validate(struct gtm_method *m)
{
  r_entry_t *r;
  int i;
  word l;

  __sync_synchronize ();

  r = m->r_set.entries;
  for (i = m->r_set.nb_entries; i > 0; i--, r++)
    {
      l = *r->lock;
      if (LOCK_GET_OWNED(l))
	{
	  w_entry_t *w = (w_entry_t *)LOCK_GET_ADDR(l);

	  if (!(m->w_set.entries <= w
		&& w < m->w_set.entries + m->w_set.nb_entries))
	    return false;
	}
      else
	{
	  if (LOCK_GET_TIMESTAMP(l) != r->version)
	    return false;
	}
    }

  return true;
}

/* Extend the snapshot range.  */

static bool
wbetl_extend(struct gtm_method *m)
{
  word now = GET_CLOCK;

  if (wbetl_validate (m))
    {
      m->end = now;
      return true;
    }
  return false;
}

/* Store a word-sized value.  */

static word *
wbetl_write(word *addr)
{
  volatile word *lock;
  word l, version;
  w_entry_t *w;
  w_entry_t *prev = NULL;
  struct gtm_method *m;

#if 0
  /* R/O hint should be provided by compiler, and if false we should die.  */
  if (m->ro) {
    *m->ro_hint = 0;
    GTM_restart_transaction (RESTART_VALIDATE_WRITE);
  }
#endif

  m = gtm_tx()->m;
  lock = GET_LOCK(addr);

 restart:
  /* Try to acquire lock.  */
  l = *lock;
  __sync_synchronize ();

  if (LOCK_GET_OWNED(l))
    {
      w = (w_entry_t *)LOCK_GET_ADDR(l);

      /* Did we previously write the same address?  */
      if (m->w_set.entries <= w
	  && w < m->w_set.entries + m->w_set.nb_entries)
	{
	  prev = w;
	  while (1)
	    {
	      if (addr == prev->addr)
		return &prev->value;
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
      version = LOCK_GET_TIMESTAMP(l);

      /* We might have read an older version previously.  */
      if (version > m->end)
	{
	  if (wbetl_has_read(m, lock) != NULL)
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
      if (!__sync_bool_compare_and_swap (lock, l, LOCK_SET_ADDR((word)w)))
	goto restart;
    }

 do_write:
  w->addr = addr;
  w->lock = lock;
  w->value = *addr;
  w->version = version;
  w->next = NULL;
  if (prev != NULL)
    prev->next = w;
  m->w_set.nb_entries++;

  return &w->value;
}

/* Load a word sized value.  */

static word *
wbetl_load(word *addr)
{
  volatile word *lock;
  word l, l2, value, version;
  r_entry_t *r;
  w_entry_t *w;
  struct gtm_method *m;

  m = gtm_tx()->m;
  lock = GET_LOCK(addr);

  l = *lock;
  __sync_synchronize ();

 restart_no_load:
  if (LOCK_GET_OWNED(l))
    {
      w = (w_entry_t *)LOCK_GET_ADDR(l);

      /* Did we previously write the same address?  */
      if (m->w_set.entries <= w
	  && w < m->w_set.entries + m->w_set.nb_entries)
	{
	  while (1)
	    {
	      if (addr == w->addr)
		return &w->value;
	      if (w->next == NULL)
		return addr;
	      w = w->next;
	    }
	}

      GTM_restart_transaction (RESTART_LOCKED_READ);
    }

  /* Not locked.  */
  value = *addr;
  __sync_synchronize ();
  l2 = *lock;
  __sync_synchronize ();
  if (l != l2)
    {
      l = l2;
      goto restart_no_load;
    }

  version = LOCK_GET_TIMESTAMP(l);

  /* If version is no longer valid, re-validate the read set.  */
  if (version > m->end)
    {
      if (/* m->ro || */ !wbetl_extend (m))
	GTM_restart_transaction (RESTART_VALIDATE_READ);

      /* Verify that the version has not yet been overwritten.  The read
	 value has not yet bee added to read set and may not have been
	 checked during the extend.  */
      l = *lock;
      __sync_synchronize ();
      if (l != l2)
	goto restart_no_load;
    }

  /* Add the address and version to the read set.  */
  if (/* !m->ro */ true)
    {
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

  return addr;
}

/* Commit the transaction.  */

static bool
wbetl_trycommit (void)
{
  struct gtm_method *m = gtm_tx()->m;
  w_entry_t *w;
  word t;
  int i;

  if (m->w_set.nb_entries > 0)
    {
      /* Get commit timestamp.  */
      t = FETCH_AND_INC_CLOCK;
      if (t >= VERSION_MAX)
	abort ();

      /* Validate only if a concurrent transaction has started since.  */
      if (m->start != t - 1 && !wbetl_validate(m))
	return false;

      /* Install new versions, drop locks and set new timestamp.  */
      w = m->w_set.entries;
      for (i = m->w_set.nb_entries; i > 0; i--, w++)
	{
	  *w->addr = w->value;

	  /* Drop lock only for last covered address in write set.  */
	  if (w->next == NULL)
	    {
	      __sync_synchronize ();
	     *w->lock = LOCK_SET_TIMESTAMP(t);
	    }
	}
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
    {
      if (i == 1)
	{
	  *w->lock = LOCK_SET_TIMESTAMP(w->version);
	  __sync_synchronize ();
	}
      else if (w->next == NULL)
	*w->lock = LOCK_SET_TIMESTAMP(w->version);
    }
}

static void REGPARM
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
      m = gtm_tx()->m;
      m->r_set.nb_entries = 0;
      m->w_set.nb_entries = 0;
      if (m->w_set.reallocate)
	{
	  m->w_set.reallocate = 0;
	  m->w_set.entries = realloc (m->w_set.entries,
				      m->w_set.size * sizeof(w_entry_t));
	}
    }

  m->start = m->end = GET_CLOCK;
}

static void
wbetl_fini (void)
{
  struct gtm_method *m = gtm_tx()->m;

  free (m->r_set.entries);
  free (m->w_set.entries);
  free (m);
}

static _ITM_TYPE_U1 REGPARM wbetl_RU1 (const _ITM_TYPE_U1 *ptr);
static void REGPARM wbetl_WU1 (_ITM_TYPE_U1 *ptr, _ITM_TYPE_U1 val);

struct unaligned_word { word w; } __attribute__((__packed__)); 

static void REGPARM
wbetl_memcpyRtWn(void *w, const void *r, size_t n)
{
  uint8_t *w8 = (uint8_t *)w;
  uint8_t *r8 = (uint8_t *)r;
  size_t i;

  i = (uintptr_t)r & (sizeof(word) - 1);
  if (i != 0)
    for (i = sizeof(word) - i; i > 0 && n > 0; i--, n--, w8++, r8++)
      *w8 = wbetl_RU1 (r8);

  if (n >= sizeof(word))
    {
      struct unaligned_word *wu = (struct unaligned_word *) w8;
      word *ra = (word *)r8;

      for (; n >= sizeof (word); n -= sizeof(word), wu++, ra++)
	wu->w = *wbetl_load (ra);

      w8 = (uint8_t *)wu;
      r8 = (uint8_t *)ra;
    }

  for (; n > 0; n--, w8++, r8++)
    *w8 = wbetl_RU1 (r8);
}

static void REGPARM
wbetl_memcpyRnWt(void *w, const void *r, size_t n)
{
  uint8_t *w8 = (uint8_t *)w;
  uint8_t *r8 = (uint8_t *)r;
  size_t i;

  i = (uintptr_t)w & (sizeof(word) - 1);
  if (i != 0)
    for (i = sizeof(word) - i; i > 0 && n > 0; i--, n--, w8++, r8++)
      wbetl_WU1 (w8, *r8);

  if (n >= sizeof(word))
    {
      word *wa = (word *)w8;
      struct unaligned_word *ru = (struct unaligned_word *) r8;

      for (; n >= sizeof (word); n -= sizeof(word), wa++, ru++)
	*wbetl_write (wa) = ru->w;

      w8 = (uint8_t *)wa;
      r8 = (uint8_t *)ru;
    }

  for (; n > 0; n--, w8++, r8++)
    wbetl_WU1 (w8, *r8);
}

static void REGPARM
wbetl_memcpyRtWt(void *w, const void *r, size_t n)
{
  uint8_t *w8 = (uint8_t *)w;
  uint8_t *r8 = (uint8_t *)r;

  /* ??? We can really do better than this.  */
  for (; n > 0; n--, w8++, r8++)
    wbetl_WU1 (w8, wbetl_RU1 (r8));
}

#define wbetl_memcpyRtaRWn	wbetl_memcpyRtWn
#define wbetl_memcpyRtaWWn	wbetl_memcpyRtWn
#define wbetl_memcpyRnWtaR	wbetl_memcpyRnWt
#define wbetl_memcpyRnWtaW	wbetl_memcpyRnWt
#define wbetl_memcpyRtWtaR	wbetl_memcpyRtWt
#define wbetl_memcpyRtWtaW	wbetl_memcpyRtWt
#define wbetl_memcpyRtaRWt	wbetl_memcpyRtWt
#define wbetl_memcpyRtaRWtaR	wbetl_memcpyRtWt
#define wbetl_memcpyRtaRWtaW	wbetl_memcpyRtWt
#define wbetl_memcpyRtaWWt	wbetl_memcpyRtWt
#define wbetl_memcpyRtaWWtaR	wbetl_memcpyRtWt
#define wbetl_memcpyRtaWWtaW	wbetl_memcpyRtWt

static void REGPARM
wbetl_memmove (void *w, const void *r, size_t n)
{
  uint8_t *w8 = (uint8_t *)w;
  uint8_t *r8 = (uint8_t *)r;

  /* ??? We can really do better than this.  */
  if (w8 < r8)
    for (; n > 0; n--, w8++, r8++)
      wbetl_WU1 (w8, wbetl_RU1 (r8));
  else
    while (n-- > 0)
      wbetl_WU1 (w8 + n, wbetl_RU1 (r8 + n));
}

static void REGPARM
wbetl_memset (void *w, int val, size_t n)
{
  uint8_t *w8 = (uint8_t *)w;
  size_t i;

  i = (uintptr_t)w & (sizeof(word) - 1);
  if (i != 0)
    for (i = sizeof(word) - i; i > 0 && n > 0; i--, n--, w8++)
      wbetl_WU1 (w8, val);

  if (n >= sizeof(word))
    {
      union { word w; char c[sizeof(word)]; } c;
      word *wa = (word *)w8;
      word valw;

      memset (&c, val, sizeof(word));
      valw = c.w;

      for (; n >= sizeof (word); n -= sizeof(word), wa++)
	*wbetl_write (wa) = valw;

      w8 = (uint8_t *)wa;
    }

  for (; n > 0; n--, w8++)
    wbetl_WU1 (w8, val);
}

#define S(X)	sizeof(word) < sizeof (X) ? 1 : sizeof(word) / sizeof (X)

#define _ITM_READ(T)							\
static _ITM_TYPE_##T REGPARM _ITM_TYPE_ATTR(T)				\
wbetl_R##T (const _ITM_TYPE_##T *addr)					\
{									\
  void *addr1;								\
  if (sizeof(*addr) == sizeof(word))					\
    {									\
      addr1 = wbetl_load((word *)addr);					\
      return *(_ITM_TYPE_##T *)addr1;					\
    }									\
  else if (sizeof(*addr) <= sizeof(word))				\
    {									\
      word ofs;								\
      addr1 = wbetl_load ((word *)((uintptr_t)addr & -sizeof(word)));	\
      ofs = (uintptr_t)addr & (sizeof(word) - 1);			\
      return *(_ITM_TYPE_##T *)(addr1 + ofs);				\
    }									\
  else									\
    {									\
      _ITM_TYPE_##T t;							\
      wbetl_memcpyRtWn (&t, addr, sizeof (*addr));			\
      return t;								\
    }									\
}

#define _ITM_WRITE(T)							\
static void REGPARM _ITM_TYPE_ATTR(T)					\
wbetl_W##T (_ITM_TYPE_##T *addr, _ITM_TYPE_##T val)			\
{									\
  void *addr1;								\
  if (sizeof(val) == sizeof(word))					\
    *(_ITM_TYPE_##T *)wbetl_write ((word *)addr) = val;			\
  else if (sizeof(val) <= sizeof(word))					\
    {									\
      word ofs;								\
      addr1 = wbetl_write ((word *)((uintptr_t)addr & -sizeof(word)));	\
      ofs = (uintptr_t)addr & (sizeof(word) - 1);			\
      *(_ITM_TYPE_##T *)(addr1 + ofs) = val;				\
    }									\
  else									\
    wbetl_memcpyRnWt (addr, &val, sizeof(val));				\
}

_ITM_ALL_TYPES (_ITM_READ)
_ITM_ALL_TYPES (_ITM_WRITE)

#undef _ITM_READ
#undef _ITM_WRITE


const struct gtm_dispatch wbetl_dispatch = {
#define _ITM_READ(R, T)		.R##T = wbetl_R##T,
#define _ITM_WRITE(W, T)	.W##T = wbetl_W##T,
  _ITM_ALL_TYPES (_ITM_ALL_READS)
  _ITM_ALL_TYPES (_ITM_ALL_WRITES)
#undef _ITM_READ
#undef _ITM_WRITE

#define _ITM_MCPY_RW(FN, R, W)	.FN##R##W = wbetl_##FN##R##W,
  _ITM_MCPY(memcpy)
#undef _ITM_MCPY_RW
#define _ITM_MCPY_RW(FN, R, W)	.FN##R##W = wbetl_##FN,
  _ITM_MCPY(memmove)
#undef _ITM_MCPY_RW

#define _ITM_MSET_W(FN, W)	.FN##W = wbetl_##FN,
  _ITM_MSET(memset)
#undef _ITM_MSET_W

  .trycommit = wbetl_trycommit,
  .rollback = wbetl_rollback,
  .init = wbetl_init,
  .fini = wbetl_fini
};
