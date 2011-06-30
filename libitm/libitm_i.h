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

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <unwind.h>
#include <type_traits>

#define UNUSED		__attribute__((unused))
#define ALWAYS_INLINE	__attribute__((always_inline))
#ifdef HAVE_ATTRIBUTE_VISIBILITY
# define HIDDEN		__attribute__((visibility("hidden")))
#else
# define HIDDEN
#endif

#define likely(X)	__builtin_expect((X) != 0, 1)
#define unlikely(X)	__builtin_expect((X), 0)

namespace GTM HIDDEN {

using namespace std;

// A helper template for accessing an unsigned integral of SIZE bytes.
template<size_t SIZE> struct sized_integral { };
template<> struct sized_integral<1> { typedef uint8_t type; };
template<> struct sized_integral<2> { typedef uint16_t type; };
template<> struct sized_integral<4> { typedef uint32_t type; };
template<> struct sized_integral<8> { typedef uint64_t type; };

typedef unsigned int gtm_word __attribute__((mode (word)));

// Locally defined protected allocation functions.
//
// To avoid dependency on libstdc++ new/delete, as well as to not
// interfere with the wrapping of the global new/delete we wrap for
// the user in alloc_cpp.cc, use class-local versions that defer
// to malloc/free.  Recall that operator new/delete does not go through
// normal lookup and so we cannot simply inject a version into the
// GTM namespace.

extern void * xmalloc (size_t s) __attribute__((malloc, nothrow));
extern void * xrealloc (void *p, size_t s) __attribute__((malloc, nothrow));

} // namespace GTM

#include "target.h"
#include "rwlock.h"
#include "aatree.h"
#include "cacheline.h"
#include "cachepage.h"
#include "stmlock.h"

namespace GTM HIDDEN {

// A dispatch table parameterizes the implementation of the STM.
struct abi_dispatch
{
 public:
  enum lock_type { NOLOCK, R, RaR, RaW, RfW, W, WaR, WaW };

  struct mask_pair
  {
    gtm_cacheline *line;
    gtm_cacheline_mask *mask;

    mask_pair() = default;
    mask_pair(gtm_cacheline *l, gtm_cacheline_mask *m) : line(l), mask(m) { }
  };

 private:
  // Disallow copies
  abi_dispatch(const abi_dispatch &) = delete;
  abi_dispatch& operator=(const abi_dispatch &) = delete;

 public:
  // The default version of these is pass-through.  This merely gives the
  // a single location to instantiate the base class vtable.
  virtual const gtm_cacheline *read_lock(const gtm_cacheline *, lock_type);
  virtual mask_pair write_lock(gtm_cacheline *, lock_type);

  virtual bool trycommit() = 0;
  virtual void rollback() = 0;
  virtual void reinit() = 0;
  virtual bool trydropreference(void *, size_t) = 0;

  // Use fini instead of dtor to support a static subclasses that uses
  // a unique object and so we don't want to destroy it from common code.
  virtual void fini() = 0;

  bool read_only () const { return m_read_only; }
  bool write_through() const { return m_write_through; }

  static void *operator new(size_t s) { return xmalloc (s); }
  static void operator delete(void *p) { free (p); }

 protected:
  const bool m_read_only;
  const bool m_write_through;
  abi_dispatch(bool ro, bool wt) : m_read_only(ro), m_write_through(wt) { }

  static gtm_cacheline_mask mask_sink;
};

// These values are given to GTM_restart_transaction and indicate the
// reason for the restart.  The reason is used to decide what STM
// implementation should be used during the next iteration.
enum gtm_restart_reason
{
  RESTART_REALLOCATE,
  RESTART_LOCKED_READ,
  RESTART_LOCKED_WRITE,
  RESTART_VALIDATE_READ,
  RESTART_VALIDATE_WRITE,
  RESTART_VALIDATE_COMMIT,
  RESTART_SERIAL_IRR,
  RESTART_NOT_READONLY,
  NUM_RESTARTS
};

// This type is private to alloc.c, but needs to be defined so that
// the template used inside gtm_transaction can instantiate.
struct gtm_alloc_action
{
  void (*free_fn)(void *);
  bool allocated;
};

// This type is private to local.c.
struct gtm_local_undo;

// This type is private to useraction.c.
struct gtm_user_action;

// All data relevant to a single transaction.
struct gtm_transaction
{
  // The jump buffer by which GTM_longjmp restarts the transaction.
  // This field *must* be at the beginning of the transaction.
  gtm_jmpbuf jb;

  // Data used by local.c for the local memory undo log.
  struct gtm_local_undo **local_undo;
  size_t n_local_undo;
  size_t size_local_undo;

  // Data used by alloc.c for the malloc/free undo log.
  aa_tree<uintptr_t, gtm_alloc_action> alloc_actions;

  // Data used by useraction.c for the user defined undo log.
  struct gtm_user_action *commit_actions;
  struct gtm_user_action *undo_actions;

  // A pointer to the "outer" transaction.
  struct gtm_transaction *prev;

  // A numerical identifier for this transaction.
  _ITM_transactionId_t id;

  // The _ITM_codeProperties of this transaction as given by the compiler.
  uint32_t prop;

  // The nesting depth of this transaction.
  uint32_t nesting;

  // Set if this transaction owns the serial write lock.
  static const uint32_t STATE_SERIAL		= 0x0001;
  // Set if the serial-irrevocable dispatch table is installed.
  // Implies that no logging is being done, and abort is not possible.
  static const uint32_t STATE_IRREVOCABLE	= 0x0002;
  // Set if we're in the process of aborting the transaction.  This is
  // used when _ITM_rollbackTransaction is called to begin the abort
  // and ends with _ITM_commitTransaction.
  static const uint32_t STATE_ABORTING		= 0x0004;

  // A bitmask of the above.
  uint32_t state;

  // Data used by eh_cpp.c for managing exceptions within the transaction.
  uint32_t cxa_catch_count;
  void *cxa_unthrown;
  void *eh_in_flight;

  // Data used by retry.c for deciding what STM implementation should
  // be used for the next iteration of the transaction.
  uint32_t restart_reason[NUM_RESTARTS];
  uint32_t restart_total;

  // The lock that provides access to serial mode.  Non-serialized
  // transactions acquire read locks; a serialized transaction aquires
  // a write lock.
  static gtm_rwlock serial_lock;

  // In alloc.cc
  void commit_allocations (bool);
  void record_allocation (void *, void (*)(void *));
  void forget_allocation (void *, void (*)(void *));
  void drop_references_allocations (const void *ptr)
  {
    this->alloc_actions.erase((uintptr_t) ptr);
  }

  // In beginend.cc
  void rollback ();
  bool trycommit ();
  bool trycommit_and_finalize ();
  void restart (gtm_restart_reason) ITM_NORETURN;

  static void *operator new(size_t);
  static void operator delete(void *);

  // Invoked from assembly language, thus the "asm" specifier on
  // the name, avoiding complex name mangling.
  static uint32_t begin_transaction(uint32_t, const gtm_jmpbuf *)
	__asm__("GTM_begin_transaction") ITM_REGPARM;

  // In eh_cpp.cc
  void revert_cpp_exceptions ();

  // In local.cc
  void commit_local (void);
  void rollback_local (void);
  void drop_references_local (const void *, size_t);

  // In retry.cc
  void decide_retry_strategy (gtm_restart_reason);

  // In method-serial.cc
  void serialirr_mode ();

  // In useraction.cc
  static void run_actions (struct gtm_user_action **);
  static void free_actions (struct gtm_user_action **);
};

} // namespace GTM

#include "tls.h"

namespace GTM HIDDEN {

// An unscaled count of the number of times we should spin attempting to 
// acquire locks before we block the current thread and defer to the OS.
// This variable isn't used when the standard POSIX lock implementations
// are used.
extern uint64_t gtm_spin_count_var;

extern "C" uint32_t GTM_longjmp (const gtm_jmpbuf *, uint32_t, uint32_t)
	ITM_NORETURN ITM_REGPARM;

extern "C" void GTM_LB (const void *, size_t) ITM_REGPARM;

extern void GTM_error (const char *fmt, ...)
	__attribute__((format (printf, 1, 2)));
extern void GTM_fatal (const char *fmt, ...)
	__attribute__((noreturn, format (printf, 1, 2)));

extern abi_dispatch *dispatch_wbetl();
extern abi_dispatch *dispatch_readonly();
extern abi_dispatch *dispatch_serial();

extern gtm_cacheline_mask gtm_mask_stack(gtm_cacheline *, gtm_cacheline_mask);

} // namespace GTM

#endif // LIBITM_I_H
