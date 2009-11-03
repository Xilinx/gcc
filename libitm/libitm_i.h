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

/* The following are internal implementation functions and definitions.
   To distinguish them from those defined by the Intel ABI, they all
   begin with GTM/gtm.  */

#ifndef LIBITM_I_H
#define LIBITM_I_H 1

#include "libitm.h"

#include "config.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unwind.h>

#define UNUSED		__attribute__((unused))

/* Control how gtm_copy_cacheline_mask operates.  If set, we use byte masking
   to update D, which *does* write to bytes not affected by the mask.  It's
   unclear if this optimization is correct.  */
#define ALLOW_UNMASKED_STORES 0

#ifdef HAVE_ATTRIBUTE_VISIBILITY
# pragma GCC visibility push(hidden)
#endif

#include "target.h"
#include "rwlock.h"
#include "aatree.h"

/* A gtm_cacheline_mask stores a modified bit for every modified byte
   in the cacheline with which it is associated.  */
#if CACHELINE_SIZE == 8
typedef uint8_t gtm_cacheline_mask;
#elif CACHELINE_SIZE == 16
typedef uint16_t gtm_cacheline_mask;
#elif CACHELINE_SIZE == 32
typedef uint32_t gtm_cacheline_mask;
#elif CACHELINE_SIZE == 64
typedef uint64_t gtm_cacheline_mask;
#else
#error "Unsupported cacheline size"
#endif

typedef unsigned int gtm_word __attribute__((mode (word)));

/* A cacheline.  The smallest unit with which locks are associated.  */
typedef union gtm_cacheline
{
  /* Byte access to the cacheline.  */
  unsigned char b[CACHELINE_SIZE] __attribute__((aligned(CACHELINE_SIZE)));

  /* Larger sized access to the cacheline.  */
  uint16_t u16[CACHELINE_SIZE / sizeof(uint16_t)];
  uint32_t u32[CACHELINE_SIZE / sizeof(uint32_t)];
  uint64_t u64[CACHELINE_SIZE / sizeof(uint64_t)];
  gtm_word w[CACHELINE_SIZE / sizeof(gtm_word)];

#if defined(__i386__) || defined(__x86_64__)
  /* ??? The definitions of gtm_cacheline_copy{,_mask} require all three
     of these types depending on the implementation, making it difficult
     to hide these inside the target header file.  */
# ifdef __SSE__
  __m128 m128[CACHELINE_SIZE / sizeof(__m128)];
# endif
# ifdef __SSE2__
  __m128i m128i[CACHELINE_SIZE / sizeof(__m128i)];
# endif
# ifdef __AVX__
  __m256 m256[CACHELINE_SIZE / sizeof(__m256)];
# endif
#endif
} gtm_cacheline;

/* A "page" worth of saved cachelines plus modification masks.  This
   arrangement is intended to minimize the overhead of alignment.  The
   PAGE_SIZE defined by the target must be a constant for this to work,
   which means that this definition may not be the same as the real
   system page size.  */

#define CACHELINES_PER_PAGE \
	((PAGE_SIZE - sizeof(void *)) \
	 / (CACHELINE_SIZE + sizeof(gtm_cacheline_mask)))

typedef struct gtm_cacheline_page
{
  gtm_cacheline lines[CACHELINES_PER_PAGE] __attribute__((aligned(PAGE_SIZE)));
  gtm_cacheline_mask masks[CACHELINES_PER_PAGE];
  struct gtm_cacheline_page *prev;
} gtm_cacheline_page;

static inline gtm_cacheline_page *
gtm_page_for_line (gtm_cacheline *c)
{
  return (gtm_cacheline_page *)((uintptr_t)c & -PAGE_SIZE);
}

static inline gtm_cacheline_mask *
gtm_mask_for_line (gtm_cacheline *c)
{
  gtm_cacheline_page *p = gtm_page_for_line (c);
  size_t index = c - &p->lines[0];
  return &p->masks[index];
}

/* A read lock function locks a cacheline.  PTR must be cacheline aligned.
   The return value is the cacheline address (equal to PTR for a write-through
   implementation, and something else for a write-back implementation).  */
typedef gtm_cacheline *(*gtm_read_lock_fn)(uintptr_t cacheline);

/* A write lock function locks a cacheline.  PTR must be cacheline aligned.
   The return value is a pair of the cacheline address and a mask that must
   be updated with the bytes that are subsequently modified.  We hope that
   the target implements small structure return efficiently so that this
   comes back in a pair of registers.  If not, we're not really worse off
   than returning the second value via a second argument to the function.  */

typedef struct gtm_cacheline_mask_pair
{
  gtm_cacheline *line;
  gtm_cacheline_mask *mask;
} gtm_cacheline_mask_pair;

typedef gtm_cacheline_mask_pair (*gtm_write_lock_fn)(uintptr_t cacheline);

/* A dispatch table parameterizes the implementation of the STM.  */
typedef struct gtm_dispatch
{
  gtm_read_lock_fn R;
  gtm_read_lock_fn RaR;
  gtm_read_lock_fn RaW;
  gtm_read_lock_fn RfW;

  gtm_write_lock_fn W;
  gtm_write_lock_fn WaR;
  gtm_write_lock_fn WaW;

  bool (*trycommit) (void);
  void (*rollback) (void);
  void (*init) (bool);
  void (*fini) (void);
} gtm_dispatch;


/* These values define a mask used in gtm_transaction.state.  */
#define STATE_READONLY		0x0001
#define STATE_SERIAL		0x0002
#define STATE_IRREVOKABLE	0x0004
#define STATE_ABORTING		0x0008

/* These values are given to GTM_restart_transaction and indicate the
   reason for the restart.  The reason is used to decide what STM 
   implementation should be used during the next iteration.  */
typedef enum gtm_restart_reason
{
  RESTART_REALLOCATE,
  RESTART_LOCKED_READ,
  RESTART_LOCKED_WRITE,
  RESTART_VALIDATE_READ,
  RESTART_VALIDATE_WRITE,
  RESTART_VALIDATE_COMMIT,
  NUM_RESTARTS
} gtm_restart_reason;


/* This type is private to local.c.  */
struct gtm_local_undo;

/* This type is private to useraction.c.  */
struct gtm_user_action;

/* This type is private to the STM implementation.  */
struct gtm_method;


/* All data relevant to a single transaction.  */
typedef struct gtm_transaction
{
  /* The jump buffer by which GTM_longjmp restarts the transaction.
     This field *must* be at the beginning of the transaction.  */
  gtm_jmpbuf jb;

  /* Data used by local.c for the local memory undo log.  */
  struct gtm_local_undo **local_undo;
  size_t n_local_undo;
  size_t size_local_undo;

  /* Data used by alloc.c for the malloc/free undo log.  */
  aa_tree alloc_actions;

  /* Data used by useraction.c for the user defined undo log.  */
  struct gtm_user_action *commit_actions;
  struct gtm_user_action *undo_actions;

  /* Data used by the STM implementation.  */
  struct gtm_method *m;

  /* A pointer to the "outer" transaction.  */
  struct gtm_transaction *prev;

  /* A numerical identifier for this transaction.  */
  _ITM_transactionId_t id;

  /* The _ITM_codeProperties of this transaction as given by the compiler.  */
  uint32_t prop;

  /* The nesting depth of this transaction.  */
  uint32_t nesting;

  /* A mask of bits indicating the current status of the transaction.  */
  uint32_t state;

  /* Data used by eh_cpp.c for managing exceptions within the transaction.  */
  uint32_t cxa_catch_count;
  void *cxa_unthrown;
  void *eh_in_flight;

  /* Data used by retry.c for deciding what STM implementation should
     be used for the next iteration of the transaction.  */
  uint32_t restart_reason[NUM_RESTARTS];
  uint32_t restart_total;
} gtm_transaction;

/* The maximum number of free gtm_transaction structs to be kept.
   This number must be greater than 1 in order for transaction abort
   to be handled properly.  */
#define MAX_FREE_TX	8

/* All thread-local data required by the entire library.  */
typedef struct gtm_thread
{
#ifndef HAVE_ARCH_GTM_THREAD_TX
  /* The currently active transaction.  Elided if the target provides
     some efficient mechanism for storing this.  */
  gtm_transaction *tx;
#endif
#ifndef HAVE_ARCH_GTM_THREAD_DISP
  /* The dispatch table for the STM implementation currently in use.  Elided
     if the target provides some efficient mechanism for storing this.  */
  const gtm_dispatch *disp;
#endif

  /* A queue of free gtm_transaction structs.  */
  gtm_transaction *free_tx[MAX_FREE_TX];
  unsigned free_tx_idx, free_tx_count;

  /* The value returned by _ITM_getThreadnum to identify this thread.  */
  /* ??? At present, this is densely allocated beginning with 1 and
     we don't bother filling in this value until it is requested.
     Which means that the value returned is, as far as the user is
     concerned, essentially arbitrary.  We wouldn't need this at all
     if we knew that pthread_t is integral and fits into an int.  */
  /* ??? Consider using gettid on Linux w/ NPTL.  At least that would
     be a value meaningful to the user.  */
  int thread_num;
} gtm_thread;

/* Don't access this variable directly; use the functions below.  */
extern __thread gtm_thread _gtm_thr;

#include "target_i.h"

#ifndef HAVE_ARCH_GTM_THREAD
/* If the target does not provide optimized access to the thread-local
   data, simply access the TLS variable defined above.  */
static inline void setup_gtm_thr(void) { }
static inline gtm_thread *gtm_thr(void) { return &_gtm_thr; }
#endif

#ifndef HAVE_ARCH_GTM_THREAD_TX
/* If the target does not provide optimized access to the currently
   active transaction, simply access via GTM_THR.  */
static inline gtm_transaction * gtm_tx(void) { return gtm_thr()->tx; }
static inline void set_gtm_tx(gtm_transaction *x) { gtm_thr()->tx = x; }
#endif

#ifndef HAVE_ARCH_GTM_THREAD_DISP
/* If the target does not provide optimized access to the currently
   active dispatch table, simply access via GTM_THR.  */
static inline const gtm_dispatch * gtm_disp(void) { return gtm_thr()->disp; }
static inline void set_gtm_disp(const gtm_dispatch *x) { gtm_thr()->disp = x; }
#endif

#ifndef HAVE_ARCH_GTM_CACHELINE_COPY
/* Copy S to D, with S and D both aligned no overlap.  */
static inline void
gtm_cacheline_copy (gtm_cacheline * __restrict d,
		    const gtm_cacheline * __restrict s)
{
  *d = *s;
}
#endif

/* Similarly, but only modify bytes with bits set in M.  */
extern void gtm_cacheline_copy_mask (gtm_cacheline * __restrict d,
				     const gtm_cacheline * __restrict s,
				     gtm_cacheline_mask m);

#ifndef HAVE_ARCH_GTM_CCM_WRITE_BARRIER
/* A write barrier to emit after gtm_copy_cacheline_mask.  */
static inline void
gtm_ccm_write_barrier (void)
{
  atomic_write_barrier ();
}
#endif

/* The lock that provides access to serial mode.  Non-serialized transactions
   acquire read locks; the serialized transaction aquires a write lock.  */
extern gtm_rwlock gtm_serial_lock;

/* An unscaled count of the number of times we should spin attempting to 
   acquire locks before we block the current thread and defer to the OS.
   This variable isn't used when the standard POSIX lock implementations
   are used.  */
extern uint64_t gtm_spin_count_var;

extern uint32_t GTM_begin_transaction(uint32_t, const gtm_jmpbuf *);
extern uint32_t GTM_longjmp (const gtm_jmpbuf *, uint32_t, uint32_t)
	ITM_NORETURN;

extern void GTM_commit_local (void);
extern void GTM_rollback_local (void);
extern void *GTM_alloc_local (void *, size_t);

extern void GTM_serialmode (bool, bool);
extern void GTM_decide_retry_strategy (gtm_restart_reason);
extern void GTM_restart_transaction (gtm_restart_reason) ITM_NORETURN;

extern void GTM_run_actions (struct gtm_user_action **);
extern void GTM_free_actions (struct gtm_user_action **);

extern void GTM_record_allocation (void *, size_t, void (*)(void *));
extern void GTM_forget_allocation (void *, void (*)(void *));
extern size_t GTM_get_allocation_size (void *);
extern void GTM_commit_allocations (bool);

extern void GTM_revert_cpp_exceptions (void);

extern gtm_cacheline_page *GTM_page_alloc (void);
extern void GTM_page_release (gtm_cacheline_page *, gtm_cacheline_page *);

extern gtm_cacheline *GTM_null_read_lock (uintptr_t);
extern gtm_cacheline_mask_pair GTM_null_write_lock (uintptr_t);

extern const gtm_dispatch wbetl_dispatch;

#ifdef HAVE_ATTRIBUTE_VISIBILITY
# pragma GCC visibility pop
#endif

#endif /* LIBITM_I_H */
