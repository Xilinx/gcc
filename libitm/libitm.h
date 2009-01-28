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

/* The external interface of this library follows the specification described
   in version 1 of http://www.intel.com/some/path/here.pdf.  */

#ifndef LIBITM_H
#define LIBITM_H 1

#include "config.h"
#include "target.h"

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef REGPARM
# define REGPARM
#endif

#define NORETURN	__attribute__((noreturn))
#define UNUSED		__attribute__((unused))

#ifndef _ITM_ALL_TARGET_TYPES
# define _ITM_ALL_TARGET_TYPES(M)
#endif

#ifndef _ITM_TYPE_ATTR
# define _ITM_TYPE_ATTR(T)
#endif

/* The following are externally visible definitions and functions, though
   only very few of these should be called by user code.  */
/* ??? Move declarations usable by user code to another header; figure out
   how to unobtrusively define REGPARM there.  */

/* Values used as arguments to abort. */
typedef enum {
    userAbort = 1,
    userRetry = 2,
    TMConflict= 4,
    exceptionBlockAbort = 8
} _ITM_abortReason;

/* Arguments to changeTransactionMode */
typedef enum
{
    modeSerialIrrevocable,
} _ITM_transactionState;

/* Results from inTransaction */
typedef enum
{
    outsideTransaction = 0,    /* So "if (inTransaction(td))" works */
    inRetryableTransaction,
    inIrrevocableTransaction
} _ITM_howExecuting;

/* Values to describe properties of code, passed in to startTransaction */
typedef enum
{
   pr_instrumentedCode    = 0x0001,
   pr_uninstrumentedCode  = 0x0002,
   pr_multiwayCode        = pr_instrumentedCode | pr_uninstrumentedCode,
   pr_hasNoXMMUpdate      = 0x0004,
   pr_hasNoAbort          = 0x0008,
   pr_hasNoRetry          = 0x0010,
   pr_hasNoIrrevocable    = 0x0020,
   pr_doesGoIrrevocable   = 0x0040,
   pr_hasNoSimpleReads    = 0x0080,
   pr_aWBarriersOmitted   = 0x0100,
   pr_RaRBarriersOmitted  = 0x0200,
   pr_undoLogCode         = 0x0400,
   pr_preferUninstrumented=0x0800,
   pr_exceptionBlock      = 0x1000,
   pr_hasElse             = 0x2000,
} _ITM_codeProperties;

/* Result from startTransaction that describes what actions to take.  */
typedef enum
{
   a_runInstrumentedCode       = 0x01,
   a_runUninstrumentedCode     = 0x02,
   a_saveLiveVariables         = 0x04,
   a_restoreLiveVariables      = 0x08,
   a_abortTransaction          = 0x10,
} _ITM_actions;

typedef struct
{
    uint32_t reserved_1;
    uint32_t flags;
    uint32_t reserved_2;
    uint32_t reserved_3;
    const char *psource;
} _ITM_srcLocation;

/* ??? Under discussion whether we should have these.  */
#if 0
# define _ITM_SRCLOCATION_DECL_1	const _ITM_srcLocation *
# define _ITM_SRCLOCATION_DECL_2	, const _ITM_srcLocation *
# define _ITM_SRCLOCATION_DEFN_1	const _ITM_srcLocation * loc UNUSED
# define _ITM_SRCLOCATION_DEFN_2	, const _ITM_srcLocation * loc UNUSED
#else
# define _ITM_SRCLOCATION_DECL_1	void
# define _ITM_SRCLOCATION_DECL_2
# define _ITM_SRCLOCATION_DEFN_1	void
# define _ITM_SRCLOCATION_DEFN_2
#endif

typedef void (* _ITM_userUndoFunction)(void *);
typedef void (* _ITM_userCommitFunction) (void *);

#define _ITM_VERSION "0.90 (Feb 29 2008)"
#define _ITM_VERSION_NO 90

extern int _ITM_versionCompatible (int) REGPARM;
extern const char * _ITM_libraryVersion (void) REGPARM;

void _ITM_error(const _ITM_srcLocation *, int errorCode) REGPARM NORETURN;
 
extern _ITM_howExecuting _ITM_inTransaction(void) REGPARM;

typedef uint32_t _ITM_transactionId_t;	/* Transaction identifier */
#define _ITM_noTransactionId 1		/* Id for non-transactional code. */

extern _ITM_transactionId_t _ITM_getTransactionId(void) REGPARM;

extern uint32_t _ITM_beginTransaction(uint32_t _ITM_SRCLOCATION_DECL_2)
	REGPARM;

extern void _ITM_abortTransaction(_ITM_abortReason _ITM_SRCLOCATION_DECL_2)
	REGPARM NORETURN;
extern void _ITM_rollbackTransaction (_ITM_SRCLOCATION_DECL_1) REGPARM;

extern void _ITM_commitTransaction (_ITM_SRCLOCATION_DECL_1) REGPARM;
extern bool _ITM_tryCommitTransaction(_ITM_SRCLOCATION_DECL_1) REGPARM;

extern void _ITM_changeTransactionMode (_ITM_transactionState
					_ITM_SRCLOCATION_DECL_2) REGPARM;

/* The following typedefs exist to make the macro expansions below work
   properly.  They are not part of any API.  */
typedef uint8_t  _ITM_TYPE_U1;
typedef uint16_t _ITM_TYPE_U2;
typedef uint32_t _ITM_TYPE_U4;
typedef uint64_t _ITM_TYPE_U8;
typedef float    _ITM_TYPE_F;
typedef double   _ITM_TYPE_D;
typedef long double _ITM_TYPE_E;
typedef float _Complex _ITM_TYPE_CF;
typedef double _Complex _ITM_TYPE_CD;
typedef long double _Complex _ITM_TYPE_CE;

#define _ITM_READ(R, T) \
  extern _ITM_TYPE_##T _ITM_##R##T (const _ITM_TYPE_##T *) REGPARM;
#define _ITM_WRITE(W, T) \
  extern void _ITM_##W##T (_ITM_TYPE_##T *, _ITM_TYPE_##T) REGPARM;

#define _ITM_ALL_READS(T) \
  _ITM_READ(R, T) _ITM_READ(RaR, T) _ITM_READ(RaW, T) _ITM_READ(RfW, T)
#define _ITM_ALL_WRITES(T) \
  _ITM_WRITE(W, T) _ITM_WRITE(WaR, T) _ITM_WRITE(WaW, T)

#define _ITM_ALL_TYPES(M) \
  M(U1) M(U2) M(U4) M(U8) M(F) M(D) M(E) \
  M(CF) M(CD) M(CE) _ITM_ALL_TARGET_TYPES(M)

_ITM_ALL_TYPES(_ITM_ALL_READS)
_ITM_ALL_TYPES(_ITM_ALL_WRITES)

#define _ITM_LOG(T) \
  extern void _ITM_L##T (const _ITM_TYPE_##T *) REGPARM;

_ITM_ALL_TYPES(_ITM_LOG)

#undef _ITM_READ
#undef _ITM_WRITE

#define _ITM_MCPY_RW(FN, R, W) \
  extern void _ITM_##FN##R##W (void *, const void *, size_t) REGPARM;
#define _ITM_MCPY_Wt(FN, R) \
  _ITM_MCPY_RW(FN,R,Wt) _ITM_MCPY_RW(FN,R,WtaR) _ITM_MCPY_RW(FN,R,WtaW)
#define _ITM_MCPY(FN) \
  _ITM_MCPY_Wt(FN, Rn) \
  _ITM_MCPY_Wt(FN, Rt) \
  _ITM_MCPY_Wt(FN, RtaR) \
  _ITM_MCPY_Wt(FN, RtaW) \
  _ITM_MCPY_RW(FN, Rt, Wn) \
  _ITM_MCPY_RW(FN, RtaR, Wn) \
  _ITM_MCPY_RW(FN, RtaW, Wn)

_ITM_MCPY(memcpy)
_ITM_MCPY(memmove)

#undef _ITM_MCPY_RW

#define _ITM_MSET_W(FN, W) \
  extern void _ITM_##FN##W (void * target, int src, size_t count) REGPARM;
#define _ITM_MSET(FN) \
  _ITM_MSET_W(FN, W) _ITM_MSET_W(FN, WaR) _ITM_MSET_W(FN, WaW)

_ITM_MSET (memset)

#undef _ITM_MSET_W

extern void _ITM_LB (const void *, size_t) REGPARM;

extern void _ITM_addUserCommitAction(_ITM_userCommitFunction,
				     _ITM_transactionId_t, void *) REGPARM;

extern void _ITM_addUserUndoAction(_ITM_userUndoFunction, void *) REGPARM;

extern int _ITM_getThreadnum(void) REGPARM;

extern void _ITM_dropReferences (const void *, size_t) REGPARM;

/* The following are internal implementation functions and definitions.
   To distinguish them from those defined by the Intel ABI, they all
   begin with GTM/gtm.  */

#ifdef HAVE_ATTRIBUTE_VISIBILITY
# pragma GCC visibility push(hidden)
#endif

#include "rwlock.h"


/* ??? We're talking about storing some of this data in glibc tls slots
   begining at 10, and making this part of the ABI on Linux.  Think about
   how to declare such things.  */
   
struct gtm_dispatch
{
#define _ITM_READ(R, T) \
  _ITM_TYPE_##T (*R##T)(const _ITM_TYPE_##T *) REGPARM;
#define _ITM_WRITE(W, T) \
  void (*W##T)(_ITM_TYPE_##T *, _ITM_TYPE_##T) REGPARM;
  _ITM_ALL_TYPES (_ITM_ALL_READS)
  _ITM_ALL_TYPES (_ITM_ALL_WRITES)
#undef _ITM_READ
#undef _ITM_WRITE

#define _ITM_MCPY_RW(FN, R, W) \
  void (* FN##R##W)(void *, const void *, size_t) REGPARM;
  _ITM_MCPY(memcpy)
  _ITM_MCPY(memmove)
#undef _ITM_MCPY_RW

#define _ITM_MSET_W(FN, W) \
  void (*FN##W) (void *, int, size_t) REGPARM;
  _ITM_MSET(memset)
#undef _ITM_MSET_W

  bool (*trycommit) (void);
  void (*rollback) (void);
  void (*init) (bool) REGPARM;
  void (*fini) (void);
};

struct gtm_local_undo
{
  void *addr;
  size_t len;
  char saved[];
};

struct gtm_user_action
{
  struct gtm_user_action *next;
  _ITM_userCommitFunction fn;
  void *arg;
};

struct gtm_method;

#define STATE_READONLY		0x0001
#define STATE_SERIAL		0x0002
#define STATE_IRREVOKABLE	0x0004
#define STATE_ABORTING		0x0008

enum restart_reason
{
  RESTART_REALLOCATE,
  RESTART_LOCKED_READ,
  RESTART_LOCKED_WRITE,
  RESTART_VALIDATE_READ,
  RESTART_VALIDATE_WRITE,
  RESTART_VALIDATE_COMMIT,
  NUM_RESTARTS
};

struct gtm_transaction
{
  uint32_t prop;
  uint32_t nesting;
  uint32_t state;
  uint32_t id;

  struct gtm_jmpbuf jb;

  struct gtm_local_undo **local_undo;
  size_t n_local_undo;
  size_t size_local_undo;

  struct gtm_user_action *commit_actions;
  struct gtm_user_action *undo_actions;

  struct gtm_method *m;
  struct gtm_transaction *prev;

  uint32_t restarts[NUM_RESTARTS + 1];
};

#define MAX_FREE_TX	8

struct gtm_thread
{
  struct gtm_transaction *tx;
  const struct gtm_dispatch *disp;

  struct gtm_transaction *free_tx[MAX_FREE_TX];
  unsigned free_tx_idx, free_tx_count;

  int thread_num;
};

extern __thread struct gtm_thread gtm_thr;

extern gtm_rwlock gtm_serial_lock;
extern unsigned long long gtm_spin_count_var;

extern uint32_t GTM_begin_transaction(uint32_t, const struct gtm_jmpbuf *)
	REGPARM;

extern uint32_t GTM_longjmp (const struct gtm_jmpbuf *, uint32_t, uint32_t)
	REGPARM NORETURN;

extern void GTM_commit_local (void);
extern void GTM_rollback_local (void);

extern void GTM_serialmode (bool) REGPARM;
extern void GTM_decide_retry_strategy (enum restart_reason) REGPARM;
extern void GTM_restart_transaction (enum restart_reason) NORETURN REGPARM;

extern void GTM_run_actions (struct gtm_user_action **) REGPARM;
extern void GTM_free_actions (struct gtm_user_action **) REGPARM;

extern const struct gtm_dispatch wbetl_dispatch;

#ifdef HAVE_ATTRIBUTE_VISIBILITY
# pragma GCC visibility pop
#endif

#endif /* LIBITM_H */
