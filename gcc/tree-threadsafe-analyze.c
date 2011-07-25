/* Thread Safety Annotations and Analysis.
   Copyright (C) 2007, 2008, 2009, 2010, 2011 Free Software Foundation, Inc.
   Contributed by Le-Chun Wu <lcwu@google.com>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */


/* This file contains an analysis pass that uses the thread safety attributes
   to identify and warn about potential issues that could result in data
   races and deadlocks. The thread safety attributes currently support only
   lock-based synchronization. They help developers document the locks that
   need to be used to safely read and write shared variables and also the
   order in which they intend to acquire locks. Here is the list of the
   attributes that this analysis pass uses:

     __attribute__ ((lockable))
     __attribute__ ((scoped_lockable))
     __attribute__ ((guarded_by(x)))
     __attribute__ ((guarded))
     __attribute__ ((point_to_guarded_by(x)))
     __attribute__ ((point_to_guarded))
     __attribute__ ((acquired_after(__VA_ARGS__)))
     __attribute__ ((acquired_before(__VA_ARGS__)))
     __attribute__ ((exclusive_lock(__VA_ARGS__)))
     __attribute__ ((shared_lock(__VA_ARGS__)))
     __attribute__ ((exclusive_trylock(__VA_ARGS__)))
     __attribute__ ((shared_trylock(__VA_ARGS__)))
     __attribute__ ((unlock(__VA_ARGS__)))
     __attribute__ ((exclusive_locks_required(__VA_ARGS__)))
     __attribute__ ((shared_locks_required(__VA_ARGS__)))
     __attribute__ ((locks_excluded(__VA_ARGS__)))
     __attribute__ ((lock_returned(x)))
     __attribute__ ((no_thread_safety_analysis))
     __attribute__ ((ignore_reads_begin))
     __attribute__ ((ignore_reads_end))
     __attribute__ ((ignore_writes_begin))
     __attribute__ ((ignore_writes_end))
     __attribute__ ((unprotected_read))

   If multi-threaded code is annotated with these attributes, this analysis
   pass can detect the following potential thread safety issues:

     * Accesses to shared variables and function calls are not guarded by
       proper (read or write) locks
     * Locks are not acquired in the specified order
     * A cycle in the lock acquisition order
     * Try to acquire a lock that is already held by the same thread
       - Useful when locks are non-reentrant 
     * Locks are not acquired and released in the same routine (or in the
       control-equivalent blocks)
       - Having critical sections starting and ending in the same routine
         is a better practice
   
   The analysis pass uses a single-pass (or single iteration) data-flow
   analysis to maintain live lock sets at each program point, using the
   attributes to decide when to add locks to the live sets and when to
   remove them from the sets. With the live lock sets and the attributes
   attached to shared variables and functions, we are able to check whether
   the variables and functions are well protected. Note that the reason why
   we don't need iterative data flow analysis is because critical sections
   across back edges are considered a bad practice.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "c-family/c-common.h"
#include "toplev.h"
#include "input.h"
#include "diagnostic.h"
#include "intl.h"
#include "basic-block.h"
#include "timevar.h"
#include "tree-flow.h"
#include "tree-pass.h"
#include "tree-dump.h"
#include "langhooks.h"
#include "pointer-set.h"
#include "tree-pretty-print.h"
#include "tree-threadsafe-analyze.h"
#include "tree-ssa-propagate.h"


/* A per-BB data structure used for topological traversal and data flow
   analysis.  */
struct bb_threadsafe_info
{
  /* Indicating whether the BB has been visited in the analysis.  */
  bool visited;

  /* Flags indicating whether we should ignore reads or writes in the
     analysis. The flags are set and unset during the analysis when seeing
     function calls annotated with ignore_{reads|writes}_{begin|end}
     attributes.  */
  bool reads_ignored;
  bool writes_ignored;

  /* Number of predecessors visited. Used for topological traversal of BBs.  */
  unsigned int n_preds_visited;

  /* Live out exclusive/shared lock sets.  */
  struct pointer_set_t *liveout_exclusive_locks;
  struct pointer_set_t *liveout_shared_locks;

  /* Locks released by the Release routine of a scoped lock (e.g.
     std::unique_lock::Release()). When a lock is released by such routines
     on certain control-flow paths but not all, we consider it weakly
     released and keep track of it in this set so that later when we encounter
     the destructor of the scoped lock (which is also an UNLOCK function),
     we will not emit a bogus warning.  */
  struct pointer_set_t *weak_released_locks;

  /* Working live lock sets. These sets are used and updated during the
     analysis of statements.  */
  struct pointer_set_t *live_excl_locks;
  struct pointer_set_t *live_shared_locks;

  /* The outgoing edge that a successful trylock call takes.  */
  edge trylock_live_edge;

  /* Sets of live-out locks acquired by a successful trylock.  */
  struct pointer_set_t *edge_exclusive_locks;
  struct pointer_set_t *edge_shared_locks;
};


/* This data structure is created when we see a function call that is a
   trylock. A map entry that maps the trylock call to its associated
   trylock_info structure is inserted to trylock_info_map (see below).
   The map (and the trylock_info structure) will later be used when we
   analyze a block-ending if-statement whose condition expression is
   fed by a trylock.  */
struct trylock_info
{
  /* The set of locks the trylock call tries to acquire.  */
  struct pointer_set_t *locks;

  /* Indicating whether the locks acquired by trylock is exclusive
     (i.e. writer lock) or not.  */
  bool is_exclusive;

  /* Specify the trylock return value on a successful lock acquisition.  */
  int succ_retval;
};

/* Access mode used for indicating whether an access to a shared variable
   is a read or a write.  */
enum access_mode
{
  TSA_READ,
  TSA_WRITE
};

/* True if the parser is currently parsing a lock attribute.  */
bool parsing_lock_attribute = false;

/* A map of which each entry maps a lock, say A, to a set of locks that
   lock A should be acquired after. This map is first populated when we
   parse the lock declarations that are annotated with "acquired_after"
   or "acquired_before" attributes. Later at the beginning of the thread
   safety analysis (see build_transitive_acquired_after_sets()), we
   calculate the transitive closures of the acquired_after sets for the
   locks and modify the map. For example, if we have global variables
   declarations like the following:

     Mutex mu1;
     Mutex mu2 __attribute__ ((acquired_after(mu1)));
     Mutex mu3 __attribute__ ((acquired_after(mu2)));

   After parsing, the contents of the map is shown below:

     lock    acquired_after set
     --------------------------
     mu2  ->  { mu1 }
     mu3  ->  { mu2 }

   After we call build_transitive_acquired_after_sets(), the map would be
   modified as shown below:

     lock    acquired_after set
     --------------------------
     mu2  ->  { mu1 }
     mu3  ->  { mu2, mu1 }  */
struct pointer_map_t *lock_acquired_after_map = NULL;

/* This flag is used for indicating whether transitive acquired_after sets
   for the locks have been built so that we only build them once per
   compilation unit.  */
static bool transitive_acq_after_sets_built = false;

/* These two variables are used during the process of building acquired_after
   transitive closures. A lock is considered finalized (and then added to the
   finalized_locks set) when every member of its current acquired_after set
     (1) is finalized, or
     (2) doesn't have an acquired_after set (i.e. a root in the partial order
         of the acquired_after relations)

   Once a lock is finalized, we never have to calculate its acquired_after set
   again during the transitive closure building process. This helps make the
   calculation converge faster.

   The reason why we needed to use global variables (instead of passing them
   in as parameters) is because we use pointer_set_traverse routine to visit
   set members, and the routine only take one additional parameter (besides
   the set and the applied function).  */
static struct pointer_set_t *finalized_locks;
static bool finalized = true;

/* This map contains the locks specified in attributes that couldn't be bound
   to any decl tree in scope when they were parsed. We would try to bind them
   during the analysis.  */
struct pointer_map_t *unbound_lock_map = NULL;

/* A map of which each entry maps a scoped lock to the lock it acquires
   at construction. An entry is created and added to the map when we see
   the constructor of a scoped lock. It is later used when we see the
   destructor of the scoped lock because the destructor doesn't take an
   argument that specifies the lock.  */
static struct pointer_map_t *scopedlock_to_lock_map;

/* Each entry maps a lock to the source location at which it was last
   acquired.  */
static struct pointer_map_t *lock_locus_map;

/* Each entry maps a lock to its canonicalized expression (see
   get_canonical_lock_expr()).  */
static GTY((param_is (union tree_node))) htab_t lock_expr_tab = NULL;

/* Each entry is a gimple call statement. Calls to the same function with
   symbolically identical arguments will hash to the same entry.  */
static htab_t gimple_call_tab = NULL;

/* Each entry maps a trylock call expr to its trylock_info.  */
static struct pointer_map_t *trylock_info_map;

/* Source location of the currently processed expression. In our analysis,
   we actually tried to pass the source location around through function
   parameters. However, in the cases where we need to use pointer_set_traverse
   or pointer_map_traverse, this global variable is used.  */
static const location_t *current_loc;

/* Buffer for pretty print the lock expression in the warning messages.  */
static pretty_printer pp_buf;

/* Forward declaration */
static void analyze_expr (tree, tree, bool, struct pointer_set_t *,
                          struct pointer_set_t *, const location_t *,
                          enum access_mode);


/* This function hashes an expr tree to a hash value by doing the following:
   - for a decl, returns the pointer hash of the tree,
   - for an integer constant, returns the sum of low and high parts,
   - for other expressions, sums up the hash values of all operands and
     multiplies it by the opcode,
   - for all other trees, returns 0.  */

static hashval_t
lock_expr_hash (const void *exp)
{
  const_tree expr = (const_tree) exp;

  STRIP_NOPS (expr);

  if (DECL_P (expr))
    return htab_hash_pointer (expr);
  else if (TREE_CODE (expr) == INTEGER_CST)
    return (hashval_t) (TREE_INT_CST_LOW (expr) + TREE_INT_CST_HIGH (expr));
  else if (EXPR_P (expr))
    {
      int nops = TREE_OPERAND_LENGTH (expr);
      int i;
      hashval_t sum = 0;
      for (i = 0; i < nops; i++)
        {
          tree op = TREE_OPERAND (expr, i);
          if (op != 0)
            sum += lock_expr_hash (op);
        }
      sum *= (hashval_t) TREE_CODE (expr);
      return sum;
    }
  else
    return 0;
}

/* Given two lock expressions/trees, determine whether they are equal.
   This is basically a wrapper around operand_equal_p so please see its
   comments for how two expression trees are considered equal
   (in fold-const.c).  */

static int
lock_expr_eq (const void *exp1, const void* exp2)
{
  const_tree expr1 = (const_tree) exp1;
  const_tree expr2 = (const_tree) exp2;

  return operand_equal_p (expr1, expr2, OEP_PURE_SAME);
}

/* This function hashes a gimple call statement to a hash value.
   Calls to the same function would be hashed to the same value.  */

static hashval_t
call_gs_hash (const void *call)
{
  const_gimple call_gs = (const_gimple) call;
  tree fdecl = gimple_call_fndecl (call_gs);
  if (fdecl)
    return htab_hash_pointer (fdecl);
  else
    {
      tree fn_ptr = gimple_call_fn (call_gs);
      return lock_expr_hash (get_canonical_lock_expr (fn_ptr, NULL_TREE, true,
                                                      NULL_TREE));
    }
}

/* Given two gimple call statements, determine whether they are equal.
   Two calls are consider equal if they call the same function with the
   same arguments (which is determined using operand_equal_p). This is
   a helper function used by gimple_call_tab hash table.  */

static int
call_gs_eq (const void *call1, const void* call2)
{
  const_gimple call_gs1 = (const_gimple) call1;
  const_gimple call_gs2 = (const_gimple) call2;
  tree fdecl1 = gimple_call_fndecl (call_gs1);
  tree fdecl2 = gimple_call_fndecl (call_gs2);
  unsigned i, num_args1, num_args2;

  if (call_gs1 == call_gs2)
    return 1;

  if (fdecl1 != fdecl2)
    return 0;

  if (!fdecl1)
    {
      tree fn_ptr1 = get_canonical_lock_expr (gimple_call_fn (call_gs1),
                                              NULL_TREE, true, NULL_TREE);
      tree fn_ptr2 = get_canonical_lock_expr (gimple_call_fn (call_gs2),
                                              NULL_TREE, true, NULL_TREE);
      if (!operand_equal_p (fn_ptr1, fn_ptr2, OEP_PURE_SAME))
        return 0;
    }

  num_args1 = gimple_call_num_args (call_gs1);
  num_args2 = gimple_call_num_args (call_gs2);

  if (num_args1 != num_args2)
    return 0;

  for (i = 0; i < num_args1; ++i)
    {
      tree arg1 = get_canonical_lock_expr (gimple_call_arg (call_gs1, i),
                                           NULL_TREE, true, NULL_TREE);
      tree arg2 = get_canonical_lock_expr (gimple_call_arg (call_gs2, i),
                                           NULL_TREE, true, NULL_TREE);
      if (!operand_equal_p (arg1, arg2, OEP_PURE_SAME))
        return 0;
    }

  return 1;
}

/* This is a helper function passed in (as a parameter) to the
   pointer_set_traverse routine when we traverse the acquired_after set
   of a lock, say lock A, to populate the transitive closure. It should
   not be called by other functions. Parameter LOCK is a member of lock A's
   acquired_after set and TRANSITIVE_LOCKS is the set of locks that will
   eventually be added to lock A's acquired_after set.  */ 

static bool
add_transitive_locks (const void *lock, void *transitive_locks)
{
  void **entry = pointer_map_contains (lock_acquired_after_map, lock);

  if (!entry)
    return true;

  /* Add LOCK's acquired_after set to lock A's transitive closure.  */
  pointer_set_union_inplace ((struct pointer_set_t *) transitive_locks,
                             (struct pointer_set_t *) *entry);

  /* If LOCK, which is a member of lock A's acquired_after set, is not
     finalized, lock A is not finalized.  */
  if (!pointer_set_contains (finalized_locks, lock))
    finalized = false;

  return true;
}

/* This is a helper function passed in (as a parameter) to the
   pointer_map_traverse routine when we traverse lock_acquired_after_map
   to update the acquired_after set for each lock. It should not be
   called by other functions.

   This function iterates over members of LOCK's acquired_after set
   (i.e. ACQ_AFTER_SET) and adds their acquired_after sets to
   "transitive_lock", which is then union-ed with ACQ_AFTER_SET.
   If there is any new member added to the ACQ_AFTER_SET, we need to
   set *UPDATED to true so that the main loop that calculates the transitive
   closures will iterate again (see build_transitive_acquired_after_sets()).
   Also if every member of ACQ_AFTER_SET is finalized, LOCK is also finalized
   and added to the finalized_locks set.  */

static bool
update_acquired_after (const void *lock, void **acq_after_set,
                       void *updated)
{
  struct pointer_set_t *transitive_locks;
  size_t old_num_elements;
  size_t new_num_elements;

  /* Skip locks whose acquired_after set is already finalized.  */
  if (pointer_set_contains (finalized_locks, lock))
    return true;

  transitive_locks = pointer_set_create();

  /* Before we traverse the acq_after_set, set finalized to true. If any
     of acq_after_set's members is not finalized, the flag will be set to
     false.  */
  finalized = true;

  pointer_set_traverse ((struct pointer_set_t *) *acq_after_set,
                        add_transitive_locks, transitive_locks);

  /* Before we union transitive_locks with acq_after_set, get the original
     member number of acq_after_set.  */
  old_num_elements =
      pointer_set_cardinality ((struct pointer_set_t *) *acq_after_set);

  pointer_set_union_inplace ((struct pointer_set_t *) *acq_after_set,
                             transitive_locks);

  new_num_elements =
      pointer_set_cardinality ((struct pointer_set_t *) *acq_after_set);

  gcc_assert (new_num_elements >= old_num_elements);

  /* If new member number is greater than the original, which means some new
     members (locks) were added to acq_after_set, set *update to true.  */
  if (new_num_elements > old_num_elements)
    {
      *((bool *)updated) = true;
      if (finalized)
        pointer_set_insert (finalized_locks, lock);
    }
  else
    /* If no new locks were added to ACQ_AFTER_SET, LOCK is also finalized.  */
    pointer_set_insert (finalized_locks, lock);

  pointer_set_destroy (transitive_locks);

  return true;
}

/* This function builds transitive acquired_after sets (i.e. transitive
   closures) for locks and updates the lock_acquired_after_map. It iteratively
   traverses the lock_acquired_after_map, updating the acquired_after sets
   until the transitive closures converge. This function is called at most
   once per compilation unit.  */

static void
build_transitive_acquired_after_sets (void)
{
  bool updated = false;

  finalized_locks = pointer_set_create();

  while (1)
    {
      pointer_map_traverse (lock_acquired_after_map, update_acquired_after,
                            &updated);
      if (!updated)
        return;

      updated = false;
    }

  pointer_set_destroy (finalized_locks);
}

/* A helper function used by pointer_map_traverse to destroy ACQ_AFTER_SET
   when deleting the lock_acquired_after_map.  */

static bool
destroy_acquired_after_set (const void * ARG_UNUSED (lock),
                            void **acq_after_set, void * ARG_UNUSED (data))
{
  pointer_set_destroy ((struct pointer_set_t *) *acq_after_set);
  return true;
}

/* Function to delete the lock_expr_tab, lock_acquired_after_map, and
   unbound_lock_map. This is called at the end of a compilation unit.
   (See toplev.c)  */

void
clean_up_threadsafe_analysis (void)
{
  /* lock_expr_tab is garbage collected. */
  lock_expr_tab = NULL;

  /* Free the lock acquired_after map and the sets */
  if (lock_acquired_after_map)
    {
      pointer_map_traverse (lock_acquired_after_map,
                            destroy_acquired_after_set, NULL);
      pointer_map_destroy (lock_acquired_after_map);
      lock_acquired_after_map = NULL;
    }

  transitive_acq_after_sets_built = false;

  /* Free the unbound lock map */
  if (unbound_lock_map)
    {
      pointer_map_destroy (unbound_lock_map);
      unbound_lock_map = NULL;
    }
}

/* Given a BASE object of a field access (i.e. base->a or base->foo()),
   this function tells whether BASE is a this pointer (i.e. this->a or
   this->foo()).  */

static bool
is_base_object_this_pointer (tree base)
{
  tree this_ptr;

  if (TREE_CODE (base) != INDIRECT_REF && TREE_CODE (base) != MEM_REF)
    return false;

  this_ptr = TREE_OPERAND (base, 0);

  if (TREE_CODE (this_ptr) == SSA_NAME)
    this_ptr = SSA_NAME_VAR (this_ptr);

  if (TREE_CODE (this_ptr) == PARM_DECL
      && DECL_NAME (this_ptr) == maybe_get_identifier ("this"))
    return true;
  else
    return false;
}

/* Given a CALL gimple statment, check if its function decl is annotated
   with "lock_returned" attribute. If so, return the lock specified in
   the attribute. Otherise, return NULL_TREE.  */

static tree
get_lock_returned_by_call (gimple call)
{
  tree fdecl = gimple_call_fndecl (call);
  tree attr = (fdecl
               ? lookup_attribute ("lock_returned", DECL_ATTRIBUTES (fdecl))
               : NULL_TREE);
  if (attr)
    {
      gcc_assert (TREE_VALUE (attr) && TREE_VALUE (TREE_VALUE (attr)));
      return TREE_VALUE (TREE_VALUE (attr));
    }
  else
    return NULL_TREE;
}

/* Given a lock expression (LOCKABLE), this function returns the
   var/field/parm decl part of the lockable. For example, if the lockable 
   is a[2].foo->mu, it returns the decl tree of mu.  */

static tree
get_lockable_decl (tree lockable)
{
  switch (TREE_CODE (lockable))
    {
      case VAR_DECL:
      case FIELD_DECL:
      case PARM_DECL:
        {
          /* If the lockable is a compiler-generated temp variable that
             has a debug expr specifying the original var decl (see
             lookup_tmp_var() in gimplify.c), return the original var decl.  */
          if (DECL_ARTIFICIAL (lockable)
              && (DECL_DEBUG_EXPR_IS_FROM (lockable)
                  && DECL_DEBUG_EXPR (lockable)))
            {
              lockable = DECL_DEBUG_EXPR (lockable);
              gcc_assert (DECL_P (lockable));
            }
          return lockable;
        }
      case ADDR_EXPR:
        /* Handle the case of mu.Lock(), i.e. Lock(&mu).  */
        return get_lockable_decl (TREE_OPERAND (lockable, 0));
      case SSA_NAME:
        {
          /* If the lockable is an SSA_NAME of a temp variable (with or
             without a name), we get to get the original variable decl
             by back-tracing its SSA def (as shown in the following example).
               D.2_1 = &this->mu;
               Lock (D.2_1);
             Note that the SSA name doesn't always have a def statement
             (e.g. "this" pointer).  */
          tree vdecl = SSA_NAME_VAR (lockable);
          if (DECL_ARTIFICIAL (vdecl)
              && !gimple_nop_p (SSA_NAME_DEF_STMT (lockable)))
            {
              gimple def_stmt = SSA_NAME_DEF_STMT (lockable);
              if (is_gimple_assign (def_stmt)
                  && (get_gimple_rhs_class (gimple_assign_rhs_code (def_stmt))
                      == GIMPLE_SINGLE_RHS))
                return get_lockable_decl (gimple_assign_rhs1 (def_stmt));
              else if (is_gimple_call (def_stmt))
                return get_lock_returned_by_call (def_stmt);
              else
                return get_lockable_decl (vdecl);
            }
          else
            return get_lockable_decl (vdecl);
        }
      case COMPONENT_REF:
        /* Handle the case of Foo.mu.Lock() or Foo->mu.Lock() */
        return get_lockable_decl (TREE_OPERAND (lockable, 1));
      case ARRAY_REF:
        return get_lockable_decl (TREE_OPERAND (lockable, 0));
      default:
        return NULL_TREE;
    }
}

/* Build a fully-qualified name of a lock that is a class member with the
   given BASE object tree and the LOCK_FIELD tree. This helper function is
   usually used when handling lock_returned, lock, and unlock attributes.
   For example, given the following code

   class Bar {
     public:
       bool MyLock() __attributes__ ((exclusive_lock(mu1_)));
       void MyUnlock() __attributes__ ((unlock(mu1_)));
       int a_ __attribute__ ((guarded_by(mu1_)));
     private:
       Mutex mu1_;
   };

   Bar *b1, *b2;

   void func()
   {
     b1->MyLock();    // S1
     b1->a_ = 5;      // S2
     b2->a_ = 3;      // S3
     b1->MyUnlock();  // S4
   }

   When analyzing statement S1, instead of adding "mu1_" to the live lock
   set, we need to build the fully-qualified name, b1->mu1, first and add
   the fully-qualified name to the live lock set. The same goes for the unlock
   statement in S4. Without using the fully-qualified lock names, we won't
   be able to tell the lock requirement difference between S2 and S3.  */

static tree
build_fully_qualified_lock (tree lock_field, tree base)
{
  tree lock;
  tree canon_base = get_canonical_lock_expr (base, NULL_TREE,
                                             true /* is_temp_expr */,
                                             NULL_TREE);

  /* When the base is a pointer, i.e. b1->MyLock() (or MyLock(base)
     internally), we need to create a new base that is INDIRECT_REF so that
     we could form a correct fully-qualified lock expression with the
     lock_field (e.g. b1->lock_field). On the other hand, if the base is an
     address_taken operation (i.e. base.foo() or foo(&base)), we need to get
     rid of the ADDR_EXPR operator before we form the new lock expression.  */
  if (TREE_CODE (canon_base) != ADDR_EXPR)
    {
      /* Note that if the base object is neither an ADDR_EXPR, nor a pointer,
         most likely we have done IPA-SRA optimization and the DECL is a
         cloned method, where reference parameters are changed to be passed
         by value. So in this case, we don't need to do anything.  */
      if (POINTER_TYPE_P (TREE_TYPE (canon_base)))
        canon_base = build1 (INDIRECT_REF, TREE_TYPE (TREE_TYPE (canon_base)),
                             canon_base);
    }
  else
    canon_base = TREE_OPERAND (canon_base, 0);

  lock = get_canonical_lock_expr (lock_field, canon_base,
                                  false /* is_temp_expr */, NULL_TREE);

  return lock;
}

/* Given a lock expression, this function returns a canonicalized
   expression (for matching locks in the analysis). Basically this
   function does the following to canonicalize the expression:

   - Fold temp variables. e.g.

           D.1 = &q->mu;  ==>   foo (&q->mu);
           foo (D.1);

   - Fold SSA names. e.g.

           q.0_1 = q;     ==>   q->mu;
           q.0_1->mu;

   - Replace function calls that return a lock with the actual lock returned
     (if it is annotated with "lock_returned" attribute).

   - Subexpressions of the lock are canonicalized recursively.

   When matching two expressions, we currently only do it symbolically.
   That is, if two different pointers, say p and q, point to the same
   location, they will not map to the same canonical expr.

   This function can also be called to get a canonical form of an expression
   which may not be a lock. For example, when trying to canonicalize the base
   object expression. And in that case, IS_TEMP_EXPR is set to true so that
   the expression will not be inserted into the LOCK_EXPR_TAB.

   When this function is called with a non-null NEW_LEFTMOST_BASE_VAR and the
   lefmost base of LOCK is a PARM_DECL, we are trying to replace a formal
   parameter with an actual argument (i.e. NEW_LEFTMOST_BASE_VAR). This
   function will return an expression whose leftmost base/operands is replaced
   with the given new base var. For example, if LOCK is a->b[3]->c and
   NEW_LEFTMOST_BASE_VAR is x, the function will return (the canonical form
   of) x->b[3]->c.  */

tree
get_canonical_lock_expr (tree lock, tree base_obj, bool is_temp_expr,
                         tree new_leftmost_base_var)
{
  hashval_t hash;
  tree canon_lock;
  void **slot;

  switch (TREE_CODE (lock))
    {
      case PARM_DECL:
        if (new_leftmost_base_var)
          return new_leftmost_base_var;
        /* Fall through to the following case.  */
      case VAR_DECL:
        {
          /* If the lock is a compiler-generated temp variable that
             has a debug expr specifying the original var decl (see
             lookup_tmp_var() in gimplify.c), return the original var decl.  */
          if (DECL_ARTIFICIAL (lock)
              && (DECL_DEBUG_EXPR_IS_FROM (lock)
                  && DECL_DEBUG_EXPR (lock)))
            {
              lock = DECL_DEBUG_EXPR (lock);
              gcc_assert (DECL_P (lock));
            }
          return lock;
        }
      case FIELD_DECL:
        {
          /* If the LOCK is a field decl and BASE_OBJ is not NULL, build a
             component_ref expression for the canonical lock.  */
          if (base_obj)
            {
              tree full_lock = build3 (COMPONENT_REF, TREE_TYPE (lock),
                                       base_obj, lock, NULL_TREE);
              lock = get_canonical_lock_expr (full_lock, NULL_TREE,
                                              true /* is_temp_expr */,
                                              NULL_TREE);
            }
          return lock;
        }
      case SSA_NAME:
        {
          /* If the lock is an SSA_NAME of a temp variable (with or
             without a name), we can possibly get the original variable decl
             by back-tracing its SSA def (as shown in the following example).

               D.2_1 = &this->mu;
               Lock (D.2_1);

             Note that the SSA name doesn't always have a def statement
             (e.g. "this" pointer).  */
          tree vdecl = SSA_NAME_VAR (lock);
          if (DECL_ARTIFICIAL (vdecl)
              && !gimple_nop_p (SSA_NAME_DEF_STMT (lock)))
            {
              gimple def_stmt = SSA_NAME_DEF_STMT (lock);
              if (is_gimple_assign (def_stmt)
                  && (get_gimple_rhs_class (gimple_assign_rhs_code (def_stmt))
                      == GIMPLE_SINGLE_RHS))
                return get_canonical_lock_expr (gimple_assign_rhs1 (def_stmt),
                                                base_obj, is_temp_expr,
                                                NULL_TREE);
              else if (is_gimple_call (def_stmt))
                {
                  tree fdecl = gimple_call_fndecl (def_stmt);
                  tree real_lock = get_lock_returned_by_call (def_stmt);
                  if (real_lock)
                    {
                      gcc_assert (fdecl);
                      if (TREE_CODE (TREE_TYPE (fdecl)) == METHOD_TYPE)
                        {
                          tree base = gimple_call_arg (def_stmt, 0);
                          lock = build_fully_qualified_lock (real_lock, base);
                        }
                      else
                        lock = real_lock;
                      break;
                    }
                  /* We deal with a lockable object wrapped in a smart pointer
                     here. For example, given the following code

                       auto_ptr<Mutex> mu;
                       mu->Lock();

                     We would like to ignore the "operator->" (or "operator.")
                     and simply return mu. We also treat the "get" method of
                     a smart pointer the same as operator->. And we only do it
                     when LOCK is indeed a lock expr, not some temp expr.  */
                  else if (fdecl
                           && ((DECL_NAME (fdecl)
                                == maybe_get_identifier ("operator->"))
                               || (DECL_NAME (fdecl)
                                   == maybe_get_identifier ("operator."))
                               || (DECL_NAME (fdecl)
                                   == maybe_get_identifier ("get")))
                           && !is_temp_expr
                           && POINTER_TYPE_P (TREE_TYPE (lock))
                           && lookup_attribute ("lockable", TYPE_ATTRIBUTES (
                                                TREE_TYPE (TREE_TYPE (lock)))))
                    {
                      tree arg = gimple_call_arg (def_stmt, 0);
                      tree canon_arg = get_canonical_lock_expr (
                          arg, base_obj, false /* is_temp_expr */, NULL_TREE);
                      if (TREE_CODE (canon_arg) == ADDR_EXPR)
                        lock = TREE_OPERAND (canon_arg, 0);
                      break;
                    }

                  /* For a gimple call statement not annotated with
                     "lock_returned" attr, try to get the canonical lhs of
                     the statement.  */
                  hash = call_gs_hash (def_stmt);
                  if (hash)
                    {
                      gimple canon_call = (gimple) htab_find_with_hash (
                          gimple_call_tab, def_stmt, hash);
                      if (!canon_call)
                        {
                          slot = htab_find_slot_with_hash (gimple_call_tab,
                                                           def_stmt, hash,
                                                           INSERT);
                          *slot = def_stmt;
                          canon_call = def_stmt;
                        }
                      lock = gimple_call_lhs (canon_call);
                      break;
                    }
                }
            }
          return get_canonical_lock_expr (vdecl, base_obj, is_temp_expr,
                                          NULL_TREE);
        }
      case ADDR_EXPR:
        {
          tree base = TREE_OPERAND (lock, 0);
          tree canon_base;
          /* When the expr is a pointer to a lockable type (i.e. mu.Lock()
             or Lock(&mu) internally), we don't need the address-taken
             operator (&).  */
          if (lookup_attribute("lockable", TYPE_ATTRIBUTES (TREE_TYPE (base))))
            return get_canonical_lock_expr (base, base_obj,
                                            false /* is_temp_expr */,
                                            new_leftmost_base_var);
          canon_base = get_canonical_lock_expr (base, NULL_TREE,
                                                true /* is_temp_expr */,
                                                new_leftmost_base_var);
          if (base != canon_base)
            lock = build1 (ADDR_EXPR, TREE_TYPE (lock), canon_base);
          break;
        }
      case COMPONENT_REF:
        {
          /* Handle the case of Foo.mu.Lock() or Foo->mu.Lock().
             If the base is "this" pointer or a base class, get the component
             only.  */
          tree base = TREE_OPERAND (lock, 0);
          tree component = TREE_OPERAND (lock, 1);
          tree canon_base;
          if (is_base_object_this_pointer (base))
            return get_canonical_lock_expr (component, NULL_TREE, is_temp_expr,
                                            NULL_TREE);

          canon_base = get_canonical_lock_expr (base, base_obj,
                                                true /* is_temp_expr */,
                                                new_leftmost_base_var);

          /* If either the base or the component is a compiler-generated base
             object field, skip it. For example, if a lock expressions is
             foo->D.2801.mu, where D.2801 is the base field in foo which is
             a derived class, we want the canonical form of the lock to be
             foo->mu.  */
          if (lang_hooks.decl_is_base_field (canon_base))
            return get_canonical_lock_expr (component, NULL_TREE, is_temp_expr,
                                            NULL_TREE);

          if (lang_hooks.decl_is_base_field (component))
            return canon_base;

          if (base != canon_base)
            lock = build3 (COMPONENT_REF, TREE_TYPE (component),
                           canon_base, component, NULL_TREE);
          break;
        }
      case ARRAY_REF:
        {
          tree array = TREE_OPERAND (lock, 0);
          tree canon_array = get_canonical_lock_expr (array, base_obj,
                                                      true /* is_temp_expr */,
                                                      new_leftmost_base_var);
          tree index = TREE_OPERAND (lock, 1);
          tree canon_index = (TREE_CODE (index) == INTEGER_CST
                              ? index
                              : get_canonical_lock_expr (index, NULL_TREE,
                                                      true /* is_temp_expr */,
                                                      NULL_TREE));
          if (array != canon_array || index != canon_index)
            lock = build4 (ARRAY_REF, TREE_TYPE (lock), canon_array,
                           canon_index, TREE_OPERAND (lock, 2),
                           TREE_OPERAND (lock, 3));
          break;
        }
      case INDIRECT_REF:
      case MEM_REF:
        {
          tree base = TREE_OPERAND (lock, 0);
          tree canon_base = get_canonical_lock_expr (base, base_obj,
                                                     true /* is_temp_expr */,
                                                     new_leftmost_base_var);
          
          /* If CANON_BASE is an ADDR_EXPR (e.g. &a), doing an indirect or
             memory reference on top of it is equivalent to accessing the
             variable itself. That is, *(&a) == a. So if that's the case,
             simply return the variable. Otherwise, build an indirect ref
             expression.  */
          if (TREE_CODE (canon_base) == ADDR_EXPR)
            lock = TREE_OPERAND (canon_base, 0);
          else
            lock = build1 (INDIRECT_REF,
                           TREE_TYPE (TREE_TYPE (canon_base)), canon_base);
          break;
        }
      default:
        break;
    }

  hash = lock_expr_hash (lock);

  /* Return the original lock expr if the lock expr is not something we can
     handle now.  */
  if (hash == 0)
    return lock;

  /* Now that we have built a canonical lock expr, check whether it's already
     in the lock_expr_tab. If so, grab and return it. Otherwise, insert the
     new lock expr to the map.  */
  if (lock_expr_tab == NULL)
    lock_expr_tab = htab_create_ggc (10, lock_expr_hash, lock_expr_eq, NULL);

  canon_lock = (tree) htab_find_with_hash (lock_expr_tab, lock, hash);
  if (canon_lock)
    return canon_lock;

  /* If the lock is created temporarily (e.g. to form a full-path
     lock name), don't insert it in the lock_expr_tab as the lock
     tree will be manually freed later.  */
  if (!is_temp_expr)
    {
      slot = htab_find_slot_with_hash (lock_expr_tab, lock, hash, INSERT);
      *slot = lock;
    }

  return lock;
}

/* Dump the LOCK name/expr in char string to OUT_BUF. If LOCK is a
   simple decl, we just use the identifier node of the lock. Otherwise,
   we use the tree pretty print mechanism to do that.  */

const char*
dump_expr_tree (tree lock, char *out_buf)
{
  if (DECL_P (lock) && DECL_NAME (lock))
    snprintf(out_buf, LOCK_NAME_LEN, "'%s'",
             IDENTIFIER_POINTER (DECL_NAME (lock)));
  else
    {
      pp_clear_output_area (&pp_buf);
      dump_generic_node (&pp_buf, lock, 0, TDF_DIAGNOSTIC, false);
      snprintf(out_buf, LOCK_NAME_LEN, "'%s'",
               pp_base_formatted_text (&pp_buf));
    }
  return out_buf;
}

/* A helper function that checks if the left-most operand of
   EXPR is a field decl, and if so, returns true. For example, if EXPR is
   'a.b->c[2]', it will check if 'a' is a field decl.  */

static bool
leftmost_operand_is_field_decl (tree expr)
{
  if (TREE_CODE (get_leftmost_base_var (expr)) == FIELD_DECL)
    return true;
  else
    return false;
}

/* Check whether the given LOCK is a member of LOCK_SET and return the lock
   contained in the set if so. This check is more complicated than just
   calling pointer_set_contains with LOCK and LOCKSET because we need to
   get the canonical form of the lock. Also the LOCK_SET may contain the
   universal lock (i.e. error_mark_node). IGNORE_UNIVERSAL_LOCK indicates
   whether to ignore it. In order to be conservative (not to emit false
   positives), we don't want to ignore the universal lock when checking for
   locks required, but should ignore it when checking for locks excluded.  */

static tree
lock_set_contains (const struct pointer_set_t *lock_set, tree lock,
                   tree base_obj, bool ignore_universal_lock)
{
  /* If the universal lock is in the LOCK_SET and it is not to be ignored,
     just assume the LOCK is in the LOCK_SET and returns it.  */
  if (!ignore_universal_lock
      && pointer_set_contains (lock_set, error_mark_node))
    return lock;
  
  /* If the lock is a field and the base is not 'this' pointer nor a base
     class, we need to check the lock set with the fully-qualified lock name.
     Otherwise, we could be confused by the same lock field of a different
     object.  */
  if (leftmost_operand_is_field_decl (lock)
      && base_obj != NULL_TREE
      && !is_base_object_this_pointer (base_obj)
      && !lang_hooks.decl_is_base_field (base_obj))
    {
      /* canonical lock is a fully-qualified name. */
      tree canonical_lock = get_canonical_lock_expr (lock, base_obj,
                                                     true /* is_temp_expr */,
                                                     NULL_TREE);
      tree result = (pointer_set_contains (lock_set, canonical_lock)
                     ? canonical_lock : NULL_TREE);
      return result;
    }
  /* Check the lock set with the given lock directly as it could already be
     in canonical form.  */
  else if (pointer_set_contains (lock_set, lock))
    return lock;
  /* If the lock is not yet bound to a decl, try to bind it now.  */
  else if (TREE_CODE (lock) == IDENTIFIER_NODE)
    {
      void **entry;
      /* If there is any unbound lock in the attribute, the unbound lock map
         must not be null.  */
      gcc_assert (unbound_lock_map);
      entry = pointer_map_contains (unbound_lock_map, lock);
      gcc_assert (entry);
      if (*entry)
        {
          tree lock_decl = (tree) *entry;
          gcc_assert (TREE_CODE (lock_decl) == VAR_DECL
                      || TREE_CODE (lock_decl) == PARM_DECL
                      || TREE_CODE (lock_decl) == FIELD_DECL);
          if (pointer_set_contains (lock_set, lock_decl))
            return lock_decl;
          else
            return NULL_TREE;
        }
      else
        return NULL_TREE;
    }
  else
    return NULL_TREE;
}

/* This function checks whether LOCK is in the current live lock sets
   (EXCL_LOCKS and SHARED_LOCKS) and emits warning message if it's not.
   This function is called when analyzing the expression that either accesses
   a shared variable or calls a function whose DECL is annotated with
   guarded_by, point_to_guarded_by, or {exclusive|shared}_locks_required
   attributes.

   IS_INDIRECT_REF indicates whether the (variable) access is indirect or not.

   LOCUS specifies the source location of the expression that accesses the
   shared variable or calls the guarded function.

   MODE indicates whether the access is a read or a write.  */

static void
check_lock_required (tree lock, tree decl, tree base_obj, bool is_indirect_ref,
                     const struct pointer_set_t *excl_locks,
                     const struct pointer_set_t *shared_locks,
                     const location_t *locus, enum access_mode mode)
{
  const char *msg;
  char dname[LOCK_NAME_LEN], lname[LOCK_NAME_LEN];

  if (TREE_CODE (decl) == FUNCTION_DECL)
    {
      gcc_assert (!is_indirect_ref);
      msg = G_("Calling function");
      /* When the base obj tree is not an ADDR_EXPR, which means it is a
         pointer (i.e. base->foo(), or foo(base) internally), we will need
         to create a new base that is INDIRECT_REF so that we would be able
         to form a correct full expression for a lock later. On the other hand,
         if the base obj is an ADDR_EXPR (i.e. base.foo(), or foo(&base)
         internally), we need to remove the address-taken operation. Note
         that this is an issue only for class member functions. If DECL
         is a class field, the base_obj is good.  */
      if (base_obj)
        {
          tree canon_base = get_canonical_lock_expr (base_obj, NULL_TREE,
                                                     true /* is_temp_expr */,
                                                     NULL_TREE);
          if (TREE_CODE (canon_base) != ADDR_EXPR)
            {
              if (POINTER_TYPE_P (TREE_TYPE (canon_base)))
                base_obj = build1 (INDIRECT_REF,
                                   TREE_TYPE (TREE_TYPE (canon_base)),
                                   canon_base);
              /* If the base object is neither an ADDR_EXPR, nor a pointer,
                 and DECL is a cloned method, most likely we have done IPA-SRA
                 optimization, where reference parameters are changed to be
                 passed by value. So in this case, just use the CANON_BASE.  */
              else if (DECL_ABSTRACT_ORIGIN (decl))
                base_obj = canon_base;
              else
                gcc_unreachable ();
            }
          else
            base_obj = TREE_OPERAND (canon_base, 0);
        }
    }
  else
    {
      if (mode == TSA_READ)
        msg = G_("Reading variable");
      else
        msg = G_("Writing to variable");
    }

  /* We want to use fully-qualified expressions (i.e. including base_obj
     if any) for DECL when emitting warning messages.  */
  if (TREE_CODE (decl) != FUNCTION_DECL)
    {
      if (base_obj)
        {
          tree full_decl = build3 (COMPONENT_REF, TREE_TYPE (decl),
                                   base_obj, decl, NULL_TREE);
          decl = get_canonical_lock_expr (full_decl, NULL_TREE,
                                          true /* is_temp_expr */, NULL_TREE);
        }
    }
  else
    /* If DECL is a function, call DECL_ORIGIN first in case it is a clone so
       that we can avoid using the cloned name in the warning messages.  */
    decl = DECL_ORIGIN (decl);

  if (!lock)
    {
      /* If LOCK is NULL, either the attribute is a "guarded" attribute that
         doesn't specify a particular lock, or the lock name/expression
         is not supported. Just check whether there is any live lock at this
         point.  */
      if (pointer_set_cardinality (excl_locks) == 0)
        {
          if (pointer_set_cardinality (shared_locks) == 0)
            {
              if (is_indirect_ref)
                warning_at (*locus, OPT_Wthread_safety,
                            G_("Access to memory location pointed to by"
                               " variable %s requires a lock"),
                            dump_expr_tree (decl, dname));
              else
                warning_at (*locus,
                            OPT_Wthread_safety, G_("%s %s requires a lock"),
                            msg, dump_expr_tree (decl, dname));
            }
          else
            {
              if (mode == TSA_WRITE)
                {
                  if (is_indirect_ref)
                    warning_at (*locus, OPT_Wthread_safety,
                                G_("Writing to memory location pointed to by"
                                   " variable %s requires an exclusive lock"),
                                dump_expr_tree (decl, dname));
                  else
                    warning_at (*locus, OPT_Wthread_safety,
                                G_("%s %s requires an exclusive lock"),
                                msg, dump_expr_tree (decl, dname));
                }
            }
        }
      return;
    }

  if (!DECL_P (lock))
    lock = get_canonical_lock_expr (lock, NULL_TREE, false /* is_temp_expr */,
                                    NULL_TREE);

  if (!lock_set_contains(excl_locks, lock, base_obj, false))
    {
      if (!lock_set_contains(shared_locks, lock, base_obj, false))
        {
          /* We want to use fully-qualified expressions (i.e. including
             base_obj if any) for LOCK when emitting warning
             messages.  */
          if (base_obj)
            {
              if (TREE_CODE (lock) == FIELD_DECL)
                {
                  tree full_lock = build3 (COMPONENT_REF, TREE_TYPE (lock),
                                           base_obj, lock, NULL_TREE);
                  /* Get the canonical lock tree */
                  lock = get_canonical_lock_expr (full_lock, NULL_TREE,
                                                  true /* is_temp_expr */,
                                                  NULL_TREE);
                }
            }
          if (is_indirect_ref)
            warning_at (*locus, OPT_Wthread_safety,
                        G_("Access to memory location pointed to by"
                           " variable %s requires lock %s"),
                        dump_expr_tree (decl, dname),
                        dump_expr_tree (lock, lname));
          else
            warning_at (*locus, OPT_Wthread_safety,
                        G_("%s %s requires lock %s"),
                        msg, dump_expr_tree (decl, dname),
                        dump_expr_tree (lock, lname));
        }
      else
        {
          if (mode == TSA_WRITE)
            {
              if (base_obj)
                {
                  /* We want to use fully-qualified expressions (i.e.
                     including base_obj if any) for LOCK when
                     emitting warning messages.  */
                  if (TREE_CODE (lock) == FIELD_DECL)
                    {
                      tree full_lock = build3 (COMPONENT_REF, TREE_TYPE (lock),
                                               base_obj, lock, NULL_TREE);
                      /* Get the canonical lock tree */
                      lock = get_canonical_lock_expr (full_lock, NULL_TREE,
                                                      true /* is_temp_expr */,
                                                      NULL_TREE);
                    }
                }
              if (is_indirect_ref)
                warning_at (*locus, OPT_Wthread_safety,
                            G_("Writing to memory location pointed to by"
                               " variable %s requires exclusive lock %s"),
                            dump_expr_tree (decl, dname),
                            dump_expr_tree (lock, lname));
              else
                warning_at (*locus, OPT_Wthread_safety,
                            G_("%s %s requires exclusive lock %s"),
                            msg, dump_expr_tree (decl, dname),
                            dump_expr_tree (lock, lname));
            }
        }
    }
}

/* This data structure is created to overcome the limitation of the
   pointer_set_traverse routine which only takes one data pointer argument.
   Unfortunately when we are trying to decide whether a lock (with an optional
   base object) is in a set or not, we will need 2 input parameters and 1
   output parameter. Therefore we use the following data structure.  */

struct lock_match_info
{
  /* The lock which we want to check if it is in the acquired_after set.  */ 
  tree lock;

  /* The base object of the lock if lock is a class member.  */
  tree base;

  /* Whether we find a match or not.  */
  bool match;
};

/* This is a helper function passed in (as a parameter) to the
   pointer_set_traverse routine we invoke to traverse the acquired_after
   set to find a match for the lock recorded in the match info
   (parameter INFO). This function should not be called by other functions.
   Parameter LOCK is a member of the acquired_after set.
   If LOCK is a class field, we would reconstruct the LOCK name by
   combining it with the base object (recorded in INFO) and do a match.
   If we find a match, record the result in INFO->match and return false
   so that pointer_set_traverse would stop iterating through the rest of
   the set. Also see the comments for function acquired_after_set_contains()
   below.  */

static bool
match_locks (const void *lock, void *info)
{
  struct lock_match_info *match_info = (struct lock_match_info *)info;
  tree acq_after_lock = CONST_CAST_TREE ((const_tree) lock);
  bool result = true;

  if (TREE_CODE (acq_after_lock) == FIELD_DECL)
    {
      tree tmp_lock;
      gcc_assert (match_info->base);
      tmp_lock = build3 (COMPONENT_REF, TREE_TYPE (acq_after_lock),
                         match_info->base, acq_after_lock, NULL_TREE);
      if (lock_expr_eq (tmp_lock, match_info->lock))
        {
          match_info->match = true;
          result = false;
        }
      /* Since we know tmp_lock is not going to be used any more, we might
         as well free it even though it's not necessary.  */
      ggc_free (tmp_lock);
    }

  return result;
}

/* Check if the LOCK is in the ACQ_AFTER_SET. This check is more complicated
   than simply calling pointer_set_contains to see whether ACQ_AFTER_SET
   contains LOCK because the ACQ_AFTER_SET could only contains the "bare"
   name of the LOCK. For example, suppose we have the following code:

     class Foo {
       public:
         Mutex mu1;
         Mutex mu2 attribute__ ((acquired_after(mu1)));
         ...
     };

     main()
     {
       Foo *foo = new Foo();
       ...
       foo->mu1.Lock();
       ...
       foo->mu2.Lock();
       ...
     }

   The lock_acquired_after_map would be

     lock    acquired_after set
     --------------------------
     mu2  ->  { mu1 }

   In our analysis, when we look at foo->mu2.Lock() and want to know whether
   foo->mu1 (which was acquired earlier) is in mu2's acquired_after set
   (in this case, ACQ_AFTER_SET = { mu1 }, LOCK = foo->mu1, BASE = foo),
   a call to pointer_set_contains(mu2_acquired_after_set, foo->mu1) would
   return false as it is "mu1", not "foo->mu1", in mu2's acquired_after set.
   Therefore we will need to iterate through each member of mu2's
   acquired_after set, reconstructing the lock name with the BASE (which is
   foo in this example), and check again.  */

static bool
acquired_after_set_contains (const struct pointer_set_t *acq_after_set,
                             tree lock, tree base)
{
  struct lock_match_info *info;
  bool result;

  if (pointer_set_contains (acq_after_set, lock))
    return true;
  else if (base == NULL_TREE)
    return false;

  /* Now that a simple call to pointer_set_contains returns false and
     the LOCK appears to be a class member (as BASE is not null),
     we need to look at each element of ACQ_AFTER_SET, reconstructing
     their names, and check again.  */
  info = XNEW (struct lock_match_info);
  info->lock = lock;
  info->base = base;
  info->match = false;

  pointer_set_traverse (acq_after_set, match_locks, info);

  result = info->match;

  XDELETE (info);

  return result;
}

/* Returns the base object if EXPR is a component ref tree,
   NULL_TREE otherwise.

   Note that this routine is different from get_base_address or
   get_base_var in that, for example, if we have an expression x[5].a,
   this routine will return x[5], while the other two routines will
   return x. Also if the expr is b[3], this routine will return NULL_TREE
   while the other two will return b.  */

static tree
get_component_ref_base (tree expr)
{
  if (TREE_CODE (expr) == COMPONENT_REF)
    return TREE_OPERAND (expr, 0);
  else if (TREE_CODE (expr) == ARRAY_REF)
    return get_component_ref_base (TREE_OPERAND (expr, 0));
  else
    return NULL_TREE;
}

/* Given an expression, EXPR, returns the leftmost operand/base of EXPR.
   For example, if EXPR is 'a.b->c[2]', it will return 'a'.  Unlike
   get_base_var, this routine allows the leftmost var to be a field decl.  */

tree
get_leftmost_base_var (tree expr)
{
  while (EXPR_P (expr))
    expr = TREE_OPERAND (expr, 0);
  return expr;
}

/* This is helper function passed in (as a parameter) to pointer_set_traverse
   when we traverse live lock sets to check for acquired_after requirement.
   This function should not be called by other functions. The parameter
   LIVE_LOCK is a member of the live lock set we are traversing, and parameter
   LOCK is the lock we are about to add to the live lock set.
   In this function, we first check if LIVE_LOCK is in the acquired_after
   set of LOCK. If so, that's great (but we will also check whether LOCK is
   in LIVE_LOCK's acquired_after set to see if there is a cycle in the
   after_after relationship). Otherwise, we will emit a warning.  */

static bool
check_acquired_after (const void *live_lock, void *lock)
{
  char lname1[LOCK_NAME_LEN], lname2[LOCK_NAME_LEN];
  tree lock_decl;
  tree base;
  void **entry;
  tree live_lock_tree = CONST_CAST_TREE ((const_tree) live_lock);
  tree live_lock_decl;
  bool live_lock_in_locks_acq_after_set;
  bool lock_in_live_locks_acq_after_set;

  /* If lock_acquired_after_map is never created, which means the user code
     doesn't contain any acquired_after attributes, then simply return.
     This should be changed later if we decide to warn about unspecified
     locking order for two locks held simultaneously by a thread.  */
  if (!lock_acquired_after_map)
    return true;

  /* Since the lock_acquired_after_map is keyed by the decl tree of
     the lock variable (see handle_acquired_after_attribute() in c-common.c),
     we cannot use the full expression of the lock to look up the
     lock_acquired_after_map. Instead, we need to get the lock decl
     component of the expression. e.g. If the lock is a[2].foo->mu,
     we cannot use the whole expression tree. We have to use the decl tree
     of mu.  */
  lock_decl = get_lockable_decl ((tree) lock);
  base = (lock_decl ? get_component_ref_base ((tree) lock) : NULL_TREE);
  entry = (lock_decl
           ? pointer_map_contains (lock_acquired_after_map, lock_decl)
           : NULL);
  /* Check if LIVE_LOCK is in LOCK's acquired_after set.  */
  live_lock_in_locks_acq_after_set = (entry
                                      && acquired_after_set_contains (
                                          (struct pointer_set_t *) *entry,
                                          live_lock_tree, base));

  live_lock_decl = get_lockable_decl (live_lock_tree);
  base = (live_lock_decl ? get_component_ref_base (live_lock_tree)
          : NULL_TREE);
  entry = (live_lock_decl
           ? pointer_map_contains (lock_acquired_after_map, live_lock)
           : NULL);
  /* Check if LOCK is in LIVE_LOCK's acquired_after set.  */
  lock_in_live_locks_acq_after_set = (entry
                                      && acquired_after_set_contains (
                                          (struct pointer_set_t *) *entry,
                                          (tree) lock, base));

  if (!live_lock_in_locks_acq_after_set)
    {
      /* When LIVE_LOCK is not in LOCK's acquired_after set, we will emit
         warning messages only when LIVE_LOCK is annotated as being acquired
         after LOCK. Basically what we are saying here is that if the two
         locks don't have an acquired_after relationship based on the
         annotations (attributes), we will not check for (and warn about)
         their locking order. This is an escape hatch for locks that could
         be held simultaneously but their acquisition order is not expressible
         using the current attribute/annotation scheme.  */
      if (lock_in_live_locks_acq_after_set)
        {
          void **loc_entry = pointer_map_contains (lock_locus_map, live_lock);
          if (loc_entry)
            warning_at (*current_loc, OPT_Wthread_safety,
                        G_("Lock %s is acquired after lock %s (acquired at"
                           " line %d) but is annotated otherwise"),
                        dump_expr_tree ((tree) lock, lname1),
                        dump_expr_tree (live_lock_tree, lname2),
                        LOCATION_LINE (*((location_t *) *loc_entry)));
          else
            warning_at (*current_loc, OPT_Wthread_safety,
                        G_("Lock %s is acquired after lock %s (held at function"
                           " entry) but is annotated otherwise"),
                        dump_expr_tree ((tree) lock, lname1),
                        dump_expr_tree (live_lock_tree, lname2));
        }
      return true;
    }

  if (lock_in_live_locks_acq_after_set)
    warning_at (*current_loc, OPT_Wthread_safety,
                G_("There is a cycle in the acquisition order between locks"
                   " %s and %s"),
                dump_expr_tree (live_lock_tree, lname1),
                dump_expr_tree ((tree) lock, lname2));

  return true;
}

/* Main driver to check the lock acquisition order. LOCK is the lock we are
   about to add to the live lock set. LIVE_EXCL_LOCKS and LIVE_SHARED_LOCKS
   are the current live lock sets. LOCUS is the source location at which LOCK
   is acquired.  */

static void
check_locking_order (tree lock,
                     const struct pointer_set_t *live_excl_locks,
                     const struct pointer_set_t *live_shared_locks,
                     const location_t *locus)
{
  if (!warn_thread_mismatched_lock_order)
    return;

  current_loc = locus;
  pointer_set_traverse (live_excl_locks, check_acquired_after, lock);
  pointer_set_traverse (live_shared_locks, check_acquired_after, lock);
}

/* Given a CALL expr and an integer constant tree POS_ARG that specifies the
   argument position, returns the corresponding argument by iterating
   through the call's actual parameter list.  */

static tree
get_actual_argument_from_position (gimple call, tree pos_arg)
{
  int lock_pos;
  int num_args = gimple_call_num_args (call);

  gcc_assert (TREE_CODE (pos_arg) == INTEGER_CST);

  lock_pos = TREE_INT_CST_LOW (pos_arg);

  gcc_assert (lock_pos >= 1 && lock_pos <= num_args);

  /* The lock position specified in the attributes is 1-based, so we need to
     subtract 1 from it when accessing the call arguments.  */
  return gimple_call_arg (call, lock_pos - 1);
}

/* Given a call (CALL) and its function decl (FDECL), return the actual
   argument that corresponds to the given formal parameter (PARAM_DECL).  */

static tree
get_actual_argument_from_parameter (gimple call, tree fdecl, tree param_decl)
{
  tree parm;
  int parm_pos;

  for (parm = DECL_ARGUMENTS (fdecl), parm_pos = 0;
       parm;
       parm = TREE_CHAIN (parm), ++parm_pos)
    if (DECL_NAME (parm) == DECL_NAME (param_decl))
      return gimple_call_arg (call, parm_pos);

  gcc_unreachable ();
}

/* A helper function that adds the LOCKABLE, acquired by CALL, to the
   corresponding lock sets (LIVE_EXCL_LOCKS or LIVE_SHARED_LOCKS) depending
   on the boolean parameter IS_EXCLUSIVE_LOCK. If the CALL is a trylock call,
   create a trylock_info data structure which will be used later.  */

static void
add_lock_to_lockset (gimple call, tree lockable,
                     bool is_exclusive_lock, bool is_trylock,
                     struct pointer_set_t *live_excl_locks,
                     struct pointer_set_t *live_shared_locks)
{
  void **entry;

  if (!is_trylock)
    {
      /* Insert the lock to either exclusive or shared live lock set.  */
      if (is_exclusive_lock)
        pointer_set_insert(live_excl_locks, lockable);
      else
        pointer_set_insert(live_shared_locks, lockable);
    }
  else
    {
      /* If the primitive is a trylock, create a trylock_info structure and
         insert it to trylock_info_map, which will be used later when we
         analyze the if-statement whose condition is fed by the trylock.  */
      struct trylock_info *tryinfo;
      entry = pointer_map_insert (trylock_info_map, call);
      if (!(*entry))
        {
          tryinfo = XNEW (struct trylock_info);
          tryinfo->is_exclusive = is_exclusive_lock;
          tryinfo->locks = pointer_set_create();
          *entry = tryinfo;
        }
      else
        {
          tryinfo = (struct trylock_info *)*entry;
          gcc_assert (tryinfo->locks
                      && tryinfo->is_exclusive == is_exclusive_lock);
        }
      pointer_set_insert (tryinfo->locks, lockable);
    }
}

/* This function handles function calls that acquire or try to acquire
   locks (i.e. the functions annotated with exclusive_lock, shared_lock,
   exclusive_trylock, or shared_trylock attribute). Besides adding to the
   live lock sets the lock(s) it acquires (except for trylock calls), this
   function also does the following:

   - Checks the lock acquisition order between the lock it acquires and
     existing live locks.

   - Checks if any existing live lock is being acquired again
     (i.e. re-entered).

   - If the function call is a constructor of a scoped lock, adds an entry
     with the acquired lock to scopedlock_to_lock_map.

   - If the function call is a trylock, creates a trylock_info structure and
     inserts it to trylock_info_map.

   - Records the source location of this function call in lock_locus_map
     (as this is where the lock is acquired).

   This function handles one lock at a time, so if a locking primitive
   acquires multiple locks, this function is called multiple times (see
   process_function_attrs() below).

   Besides the call itself (CALL), we also pass in the function decl (FDECL).
   While the function decl of a call can be easily extracted by calling
   gimple_call_fndecl in most cases, it becomes a bit tricky when the function
   is virtual as gimple_call_fndecl will simply return NULL. We will need to
   get the function decl through the reference object in this case.
   Since we have already done all the things necessary to get the function
   decl earlier (see handle_call_gs()), instead of doing the whole dance again
   here, we might as well pass in the function decl that we extracted earlier.

   The lock to be acquired is either the base object (i.e. BASE_OBJ)
   when the primitive is a member function of a lockable class (e.g. "mu" in
   mu->Lock()), or specified by an attribute parameter and passed in as ARG.
   If ARG is an integer constant, it specifies the position of the primitive's
   argument that corresponds to the lock to be acquired.  */

static void
handle_lock_primitive_attrs (gimple call, tree fdecl, tree arg, tree base_obj,
                             bool is_exclusive_lock, bool is_trylock,
                             struct pointer_set_t *live_excl_locks,
                             struct pointer_set_t *live_shared_locks,
                             const location_t *locus)
{
  char lname[LOCK_NAME_LEN];
  void **entry;
  tree lockable;
  tree lockable_type;

  /* If ARG is not NULL, it specifies the lock to acquire. Otherwise,
     BASE_OBJ is the lock.  */
  if (!arg)
    arg = base_obj;
  else if (arg == error_mark_node)
    {
      /* If the arg is the universal lock (represented as the error_mark_node),
         we don't need to do all the checks mentioned in the comments above.
         Just add it to the lock set and return.  */
      add_lock_to_lockset (call, arg, is_exclusive_lock, is_trylock,
                           live_excl_locks, live_shared_locks);
      return;
    }
  /* When ARG is an integer that specifies the position of the
     call's argument corresponding to the lock, or if its leftmost base is
     a formal parameter, we need to grab the corresponding actual argument
     of the call.  */
  else if (TREE_CODE (arg) == INTEGER_CST)
    arg = get_actual_argument_from_position (call, arg);
  else if (TREE_CODE (get_leftmost_base_var (arg)) == PARM_DECL)
    {
      tree new_base
          = get_actual_argument_from_parameter (call, fdecl,
                                                get_leftmost_base_var (arg));
      arg = get_canonical_lock_expr (arg, NULL_TREE, false, new_base);
    }
  else if (base_obj)
    arg = build_fully_qualified_lock (arg, base_obj);

  gcc_assert (arg);

  lockable = get_canonical_lock_expr (arg, NULL_TREE, false /* is_temp_expr */,
                                      NULL_TREE);

  /* If there are unbound locks when the thread safety attributes were parsed,
     we should try to bind them now if we see any lock declaration that
     matches the name of the unbound lock.  */
  if (unbound_lock_map
      && (TREE_CODE (lockable) == VAR_DECL
          || TREE_CODE (lockable) == PARM_DECL
          || TREE_CODE (lockable) == FIELD_DECL))
    {
      tree lock_id = DECL_NAME (lockable);
      void **entry = pointer_map_contains (unbound_lock_map, lock_id);
      if (entry)
        *entry = lockable;
    }

  gcc_assert (fdecl);
  lockable_type = DECL_CONTEXT (fdecl);
  if (lockable_type && !TYPE_P (lockable_type))
    lockable_type = NULL_TREE;

  /* Check if the lock primitive is actually a constructor of a scoped lock.
     If so, insert to scopedlock_to_lock_map the scoped lock object along
     with the lock it acquires.  */
  if (!is_trylock
      && lockable_type
      && lookup_attribute("scoped_lockable", TYPE_ATTRIBUTES (lockable_type)))
    {
      if (TREE_CODE (base_obj) == ADDR_EXPR)
        {
          tree scoped_lock = TREE_OPERAND (base_obj, 0);
          void **entry;
          if (TREE_CODE (scoped_lock) == SSA_NAME)
            scoped_lock = SSA_NAME_VAR (scoped_lock);
          gcc_assert(TREE_CODE (scoped_lock) == VAR_DECL);
          entry = pointer_map_insert (scopedlock_to_lock_map, scoped_lock);
          *entry = lockable;
        }
    }

  /* Check if the lock is already held.  */
  if (pointer_set_contains(live_excl_locks, lockable)
      || pointer_set_contains(live_shared_locks, lockable))
    {
      if (warn_thread_reentrant_lock)
        {
          void **entry = pointer_map_contains (lock_locus_map, lockable);
          if (entry)
            warning_at (*locus, OPT_Wthread_safety,
                        G_("Try to acquire lock %s that is already held"
                           " (previously acquired at line %d)"),
                        dump_expr_tree (lockable, lname),
                        LOCATION_LINE (*((location_t *) *entry)));
          else
            warning_at (*locus, OPT_Wthread_safety,
                        G_("Try to acquire lock %s that is already held"
                           " (at function entry)"),
                        dump_expr_tree (lockable, lname));
        }
      /* Normally when we have detected a lock re-entrant issue here, we can
         simply return. However, if this primitive is a trylock, we still
         need to create an entry in the trylock_info_map (which will happen
         later) regardless. Otherwise, the assertion that every trylock call
         always has an entry in the map will fail later.  */
      if (!is_trylock)
        return;
    }

  /* Check the lock acquisition order.  */
  check_locking_order (lockable, live_excl_locks, live_shared_locks, locus);

  /* Record the source location where the lock is acquired.  */
  entry = pointer_map_insert (lock_locus_map, lockable);
  if (!(*entry))
    *entry = XNEW (location_t);
  *((location_t *) *entry) = *locus;

  add_lock_to_lockset (call, lockable, is_exclusive_lock, is_trylock,
                       live_excl_locks, live_shared_locks);
}

/* A helper function that removes the LOCKABLE from either LIVE_EXCL_LOCKS or
   LIVE_SHARED_LOCKS, and returns the canonical form of LOCKABLE. If LOCKABLE
   does not exist in either lock set, return NULL_TREE.  */

static tree
remove_lock_from_lockset (tree lockable, struct pointer_set_t *live_excl_locks,
                          struct pointer_set_t *live_shared_locks)
{
  tree lock_contained;

  if ((lock_contained = lock_set_contains(live_excl_locks, lockable, NULL_TREE,
                                          false)) != NULL_TREE)
    pointer_set_delete (live_excl_locks, lock_contained);
  else if ((lock_contained = lock_set_contains(live_shared_locks, lockable,
                                               NULL_TREE, false)) != NULL_TREE)
    pointer_set_delete (live_shared_locks, lock_contained);

  return lock_contained;
}

/* This function handles function calls that release locks (i.e. the
   functions annotated with the "unlock" attribute). Besides taking the
   lock out of the live lock set, it also checks whether the user code
   is trying to release a lock that's not currently held. For the
   explanations on parameters FDECL, ARG, and BASE_OBJ, please see the
   comments for handle_lock_primitive_attrs above.  */

static void
handle_unlock_primitive_attr (gimple call, tree fdecl, tree arg, tree base_obj,
                              struct bb_threadsafe_info *bbinfo,
                              const location_t *locus)
{
  struct pointer_set_t *live_excl_locks = bbinfo->live_excl_locks;
  struct pointer_set_t *live_shared_locks = bbinfo->live_shared_locks;
  char lname[LOCK_NAME_LEN];
  tree lockable = NULL_TREE;
  tree lock_released;
  bool is_weak_unlock = false;

  /* Check if the unlock attribute specifies a lock or the position of the
     primitive's argument corresponding to the lock.  */
  if (arg)
    {
      /* When ARG is an integer that specifies the position of the
         call's argument corresponding to the lock, or if its leftmost base is
         a formal parameter, we need to grab the corresponding actual argument
         of the call.  */
      if (TREE_CODE (arg) == INTEGER_CST)
        lockable = get_actual_argument_from_position (call, arg);
      else if (TREE_CODE (get_leftmost_base_var (arg)) == PARM_DECL)
        {
          tree fdecl = gimple_call_fndecl (call);
          tree new_base = get_actual_argument_from_parameter (
              call, fdecl, get_leftmost_base_var (arg));
          lockable = get_canonical_lock_expr (arg, NULL_TREE, false, new_base);
        }
      else if (base_obj)
        lockable = build_fully_qualified_lock (arg, base_obj);
      else
        lockable = arg;
      gcc_assert (lockable);
      lockable = get_canonical_lock_expr (lockable, NULL_TREE,
                                          false /* is_temp_expr */, NULL_TREE);
    }
  else
    {
      gcc_assert (base_obj);

      /* Check if the primitive is an unlock routine (e.g. the destructor or
         a release function) of a scoped_lock. If so, get the lock that is 
         being released from scopedlock_to_lock_map.  */
      if (TREE_CODE (base_obj) == ADDR_EXPR)
        {
          tree scoped_lock = TREE_OPERAND (base_obj, 0);
          if (TREE_CODE (scoped_lock) == SSA_NAME)
            scoped_lock = SSA_NAME_VAR (scoped_lock);
          /* A scoped lock should be a local variable.  */
          if (TREE_CODE (scoped_lock) == VAR_DECL)
            {
              void **entry = pointer_map_contains (scopedlock_to_lock_map,
                                                   scoped_lock);
              if (entry)
                {
                  gcc_assert (fdecl);
                  lockable = (tree) *entry;
                  /* Since this is a scoped lock, if the unlock routine is
                     not the destructor, we assume it is a release function
                     (e.g. std::unique_lock::release()). And therefore the
                     lock is considered weakly released and should be added
                     to the weak released lock set.  */
                  if (!lang_hooks.decl_is_destructor (fdecl))
                    is_weak_unlock = true;
                }
            }
        }
      /* If the function is not a destructor of a scoped_lock, base_obj
         is the lock.  */
      if (!lockable)
        lockable = get_canonical_lock_expr (base_obj, NULL_TREE,
                                            false /* is_temp_expr */,
                                            NULL_TREE);
    }

  /* Remove the lock from the live lock set and, if it is not currently held,
     warn about the issue.  */
  if ((lock_released = remove_lock_from_lockset (lockable, live_excl_locks,
                                                 live_shared_locks))
      != NULL_TREE)
    {
      if (is_weak_unlock)
        {
          gcc_assert (bbinfo->weak_released_locks);
          pointer_set_insert (bbinfo->weak_released_locks, lock_released);
        }
    }
  else if (!is_weak_unlock
           && ((lock_released =
                lock_set_contains (bbinfo->weak_released_locks, lockable,
                                   NULL_TREE, false)) != NULL_TREE))
    {
      /* If the unlock function is not a weak release and the lock is currently
         in the weak release set, we need to remove it from the set as it is
         no longer considered weakly released after this point.  */
      pointer_set_delete (bbinfo->weak_released_locks, lock_released);
    }
  else if (warn_thread_mismatched_lock_acq_rel)
    warning_at (*locus, OPT_Wthread_safety,
                G_("Try to unlock %s that was not acquired"),
                dump_expr_tree (lockable, lname));
}

/* A helper function for handling function "locks_excluded" attribute.
   Check if LOCK is in the current live lock sets and emit warnings if so.

   LOCK: the lock being examined.
   FDECL: function decl of the call.
   BASE_OBJ: base object if FDECL is a method (member function).
   LIVE_EXCL_LOCKS: current live exclusive lock set.
   LIVE_SHARED LOCKS: current live shared lock set.
   LOCUS: location info of the call.  */

static void
check_func_lock_excluded (tree lock, tree fdecl, tree base_obj,
                          const struct pointer_set_t *live_excl_locks,
                          const struct pointer_set_t *live_shared_locks,
                          const location_t *locus)
{
  tree lock_contained;

  /* LOCK could be NULL if the front-end/parser cannot recognize it.
     Simply ignore it and return.  */
  if (!lock)
    return;

  /* When the base obj tree is not an ADDR_EXPR, which means it is a
     pointer (i.e. base->foo() or foo(base)), we will need to create
     a new base that is INDIRECT_REF so that we would be able to form
     a correct full expression for a lock later. On the other hand,
     if the base obj is an ADDR_EXPR (i.e. base.foo() or foo(&base)),
     we need to remove the address-taken operation.  */
  if (base_obj)
    {
      tree canon_base = get_canonical_lock_expr (base_obj, NULL_TREE,
                                                 true /* is_temp_expr */,
                                                 NULL_TREE);
      if (TREE_CODE (canon_base) != ADDR_EXPR)
        {
          if (POINTER_TYPE_P (TREE_TYPE (canon_base)))
            base_obj = build1 (INDIRECT_REF,
                               TREE_TYPE (TREE_TYPE (canon_base)),
                               canon_base);
          /* If the base object is neither an ADDR_EXPR, nor a pointer,
             and DECL is a cloned method, most likely we have done IPA-SRA
             optimization, where reference parameters are changed to be
             passed by value. So in this case, just use the CANON_BASE.  */
          else if (DECL_ABSTRACT_ORIGIN (fdecl))
            base_obj = canon_base;
          else
            gcc_unreachable ();
        }
      else
        base_obj = TREE_OPERAND (canon_base, 0);
    }

  if (!DECL_P (lock))
    lock = get_canonical_lock_expr (lock, NULL_TREE, false /* is_temp_expr */,
                                    NULL_TREE);

  /* Check if the excluded lock is in the live lock sets when the
     function is called. If so, issue a warning.  */
  if ((lock_contained = lock_set_contains (live_excl_locks, lock,
                                           base_obj, true))
      || (lock_contained = lock_set_contains (live_shared_locks, lock,
                                              base_obj, true)))
    {
      char lname[LOCK_NAME_LEN];
      void **entry = pointer_map_contains (lock_locus_map, lock_contained);
      if (entry)
        warning_at (*locus, OPT_Wthread_safety,
                    G_("Cannot call function %qE with lock %s held"
                       " (previously acquired at line %d)"),
                    DECL_NAME (fdecl),
                    dump_expr_tree (lock_contained, lname),
                    LOCATION_LINE (*((location_t *) *entry)));
      else
        warning_at (*locus, OPT_Wthread_safety,
                    G_("Cannot call function %qE with lock %s held"
                       " (at function entry)"),
                    DECL_NAME (fdecl),
                    dump_expr_tree (lock_contained, lname));
    }
}

/* Function lock requirement type.  */

enum FUNC_LOCK_REQ_TYPE {
  FLR_EXCL_LOCK_REQUIRED,
  FLR_SHARED_LOCK_REQUIRED,
  FLR_LOCK_EXCLUDED
};

/* Handle function lock requirement attributes ("exclusive_locks_required",
   "shared_locks_required", and "locks_excluded").

   CALL: function/method call that's currently being examined.
   FDECL: function/method decl of the call.
   BASE_OBJ: base object if FDECL is a method (member function).
   ATTR: attribute of type FUNC_LOCK_REQ_TYPE.
   REQ_TYPE: function lock requirement type.
   LIVE_EXCL_LOCKS: current live exclusive lock set.
   LIVE_SHARED LOCKS: current live shared lock set.
   LOCUS: location info of the call.  */

static void
handle_function_lock_requirement (gimple call, tree fdecl, tree base_obj,
                                  tree attr, enum FUNC_LOCK_REQ_TYPE req_type,
                                  const struct pointer_set_t *live_excl_locks,
                                  const struct pointer_set_t *live_shared_locks,
                                  const location_t *locus)
{
  tree arg;
  tree lock;

  for (arg = TREE_VALUE (attr); arg; arg = TREE_CHAIN (arg))
    {
      tree tmp_base_obj = base_obj;
      lock = TREE_VALUE (arg);
      gcc_assert (lock);
      /* If lock is the error_mark_node, just set it to NULL_TREE so that
         we will reduce the level of checking later. (i.e. Only check whether
         there is any live lock at this point in check_lock_required and
         ignore the lock in check_func_lock_excluded.)  */
      if (lock == error_mark_node)
        lock = NULL_TREE;
      else if (TREE_CODE (lock) == INTEGER_CST)
        {
          lock = get_actual_argument_from_position (call, lock);
          /* If the lock is a function argument, we don't want to
             prepend the base object to the lock name. Set the
             TMP_BASE_OBJ to NULL.  */
          tmp_base_obj = NULL_TREE;
        }
      /* If LOCK's leftmost base is a formal parameter, we need to grab the
         corresponding actual argument of the call and replace the formal
         parameter with the actual argument in LOCK.  */
      else if (TREE_CODE (get_leftmost_base_var (lock)) == PARM_DECL)
        {
          tree new_base = get_actual_argument_from_parameter (
              call, fdecl, get_leftmost_base_var (lock));
          lock = get_canonical_lock_expr (lock, NULL_TREE, false, new_base);
          tmp_base_obj = NULL_TREE;
        }

      if (req_type == FLR_EXCL_LOCK_REQUIRED)
        check_lock_required (lock, fdecl, tmp_base_obj,
                             false /* is_indirect_ref */,
                             live_excl_locks, live_shared_locks,
                             locus, TSA_WRITE);
      else if (req_type == FLR_SHARED_LOCK_REQUIRED)
        check_lock_required (lock, fdecl, tmp_base_obj,
                             false /* is_indirect_ref */,
                             live_excl_locks, live_shared_locks,
                             locus, TSA_READ);
      else
        {
          gcc_assert (req_type == FLR_LOCK_EXCLUDED);
          check_func_lock_excluded (lock, fdecl, tmp_base_obj,
                                    live_excl_locks, live_shared_locks, locus);
        }
    }
}

/* The main routine that handles the thread safety attributes for
   functions. CALL is the call expression being analyzed. FDECL is its
   corresponding function decl tree. LIVE_EXCL_LOCKS and LIVE_SHARED_LOCKS
   are the live lock sets when the control flow reaches this call expression.
   LOCUS is the source location of the call expression.  */

static void
process_function_attrs (gimple call, tree fdecl,
                        struct bb_threadsafe_info *current_bb_info,
                        const location_t *locus)
{
  struct pointer_set_t *live_excl_locks = current_bb_info->live_excl_locks;
  struct pointer_set_t *live_shared_locks = current_bb_info->live_shared_locks;
  tree attr = NULL_TREE;
  tree base_obj = NULL_TREE;
  bool is_exclusive_lock;
  bool is_trylock;

  gcc_assert (is_gimple_call (call));

  /* First check if the function call is annotated with any escape-hatch
     related attributes and set/reset the corresponding flags if so.  */
  if (lookup_attribute("ignore_reads_begin", DECL_ATTRIBUTES (fdecl))
      != NULL_TREE)
    current_bb_info->reads_ignored = true;
  if (lookup_attribute("ignore_reads_end", DECL_ATTRIBUTES (fdecl))
      != NULL_TREE)
    current_bb_info->reads_ignored = false;
  if (lookup_attribute("ignore_writes_begin", DECL_ATTRIBUTES (fdecl))
      != NULL_TREE)
    current_bb_info->writes_ignored = true;
  if (lookup_attribute("ignore_writes_end", DECL_ATTRIBUTES (fdecl))
      != NULL_TREE)
    current_bb_info->writes_ignored = false;

  /* If the function is a class member, the first argument of the function
     (i.e. "this" pointer) would be the base object. Note that here we call
     DECL_ORIGIN on fdecl first before we check whether it's a METHOD_TYPE
     because if fdecl is a cloned method, the TREE_CODE of its type would be
     FUNCTION_DECL instead of METHOD_DECL, which would lead us to not grab
     its base object. One possible situation where fdecl could be a clone is
     when -fipa-sra is enabled. (-fipa-sra is enabled by default at -O2
     starting from GCC-4.5.). The clones could be created as early as when
     constructing SSA. Also note that the parameters of a cloned method could
     be optimized away.  */
  if (TREE_CODE (TREE_TYPE (DECL_ORIGIN (fdecl))) == METHOD_TYPE
      && gimple_call_num_args(call) > 0)
    base_obj = gimple_call_arg (call, 0);

  /* Check whether this is a locking primitive of any kind.  */
  if ((attr = lookup_attribute("exclusive_lock",
                               DECL_ATTRIBUTES (fdecl))) != NULL_TREE)
    {
      is_exclusive_lock = true;
      is_trylock = false;
    }
  else if ((attr = lookup_attribute("exclusive_trylock",
                                    DECL_ATTRIBUTES (fdecl))) != NULL_TREE)
    {
      is_exclusive_lock = true;
      is_trylock = true;
    }
  else if ((attr = lookup_attribute("shared_lock",
                                    DECL_ATTRIBUTES (fdecl))) != NULL_TREE)
    {
      is_exclusive_lock = false;
      is_trylock = false;
    }
  else if ((attr = lookup_attribute("shared_trylock",
                                    DECL_ATTRIBUTES (fdecl))) != NULL_TREE)
    {
      is_exclusive_lock = false;
      is_trylock = true;
    }

  /* Handle locking primitives */
  if (attr)
    {
      if (TREE_VALUE (attr))
        {
          int succ_retval = 0;
          tree arg = TREE_VALUE (attr);
          /* If the locking primitive is a trylock, the first argument of
             the attribute specifies the return value of a successful lock
             acquisition.  */
          if (is_trylock)
            {
              gcc_assert (TREE_CODE (TREE_VALUE (arg)) == INTEGER_CST);
              succ_retval = TREE_INT_CST_LOW (TREE_VALUE (arg));
              arg = TREE_CHAIN (arg);
            }

          /* If the primitive is a trylock, after we consume the first
             argument of the attribute, there might not be any argument
             left. So we need to check if arg is NULL again here.  */
          if (arg)
            {
              /* If the locking primitive attribute specifies multiple locks
                 in its arguments, we iterate through the argument list and
                 handle each of the locks individually.  */
              for (; arg; arg = TREE_CHAIN (arg))
                handle_lock_primitive_attrs (call, fdecl, TREE_VALUE (arg),
                                             base_obj, is_exclusive_lock,
                                             is_trylock, live_excl_locks,
                                             live_shared_locks, locus);
            }
          else
            {
              /* If the attribute does not have any argument left, the lock to
                 be acquired is the base obj (e.g. "mu" in mu->TryLock()).  */
              handle_lock_primitive_attrs (call, fdecl, NULL_TREE, base_obj,
                                           is_exclusive_lock, is_trylock,
                                           live_excl_locks, live_shared_locks,
                                           locus);
            }
          /* If the primitive is a trylock, fill in the return value on
             successful lock acquisition in the trylock_info that was
             created in handle_lock_primitive_attrs.  */
          if (is_trylock)
            {
              struct trylock_info *tryinfo;
              void **entry = pointer_map_contains (trylock_info_map, call);
              gcc_assert (entry);
              tryinfo = (struct trylock_info *)*entry;
              tryinfo->succ_retval = succ_retval;
            }
        }
      else
        {
          /* If the attribute does not have any argument, the lock to be
             acquired is the base obj (e.g. "mu" in mu->Lock()).  */
          gcc_assert (!is_trylock);
          handle_lock_primitive_attrs (call, fdecl, NULL_TREE, base_obj,
                                       is_exclusive_lock, is_trylock,
                                       live_excl_locks, live_shared_locks,
                                       locus);
        }
    }
  /* Handle unlocking primitive */
  else if ((attr = lookup_attribute ("unlock", DECL_ATTRIBUTES (fdecl)))
           != NULL_TREE)
    {
      if (TREE_VALUE (attr))
        {
          /* If the unlocking primitive attribute specifies multiple locks
             in its arguments, we iterate through the argument list and
             handle each of the locks individually.  */
          tree arg;
          for (arg = TREE_VALUE (attr); arg; arg = TREE_CHAIN (arg))
            {
              /* If the unlock arg is an error_mark_node, which means an
                 unsupported lock name/expression was encountered during
                 parsing, the conservative approach to take is not to check
                 the lock acquire/release mismatch issue in the current
                 function by setting the flag to 0. Note that the flag will
                 be restored to its original value after finishing analyzing
                 the current function.  */
              if (TREE_VALUE (arg) == error_mark_node)
                {
                  warn_thread_mismatched_lock_acq_rel = 0;
                  continue;
                }
              handle_unlock_primitive_attr (call, fdecl, TREE_VALUE (arg),
                                            base_obj, current_bb_info, locus);
            }
        }
      else
        /* If the attribute does not have any argument, the lock to be
           released is the base obj (e.g. "mu" in mu->Unlock()).  */
        handle_unlock_primitive_attr (call, fdecl, NULL_TREE, base_obj,
                                      current_bb_info, locus);
    }

  if (warn_thread_unguarded_func)
    {
      /* Handle the attributes specifying the lock requirements of
         functions.  */
      if ((attr = lookup_attribute ("exclusive_locks_required",
                                    DECL_ATTRIBUTES (fdecl))) != NULL_TREE)
         handle_function_lock_requirement (call, fdecl, base_obj, attr,
                                          FLR_EXCL_LOCK_REQUIRED,
                                          live_excl_locks, live_shared_locks,
                                          locus);

      if ((attr = lookup_attribute ("shared_locks_required",
                                    DECL_ATTRIBUTES (fdecl))) != NULL_TREE)
        handle_function_lock_requirement (call, fdecl, base_obj, attr,
                                          FLR_SHARED_LOCK_REQUIRED,
                                          live_excl_locks, live_shared_locks,
                                          locus);

      if ((attr = lookup_attribute ("locks_excluded", DECL_ATTRIBUTES (fdecl)))
          != NULL_TREE)
        handle_function_lock_requirement (call, fdecl, base_obj, attr,
                                          FLR_LOCK_EXCLUDED,
                                          live_excl_locks, live_shared_locks,
                                          locus);
    }
}

/* The main routine that handles the attributes specifying variables' lock
   requirements.  */

static void
process_guarded_by_attrs (tree vdecl, tree base_obj, bool is_indirect_ref,
                          const struct pointer_set_t *excl_locks,
                          const struct pointer_set_t *shared_locks,
                          const location_t *locus, enum access_mode mode)
{
  tree attr;
  tree lockable = NULL_TREE;
  /* A flag indicating whether the attribute is {point_to_}guarded_by with
     a lock specified or simply {point_to_}guarded.  */
  bool lock_specified = true;

  if (!warn_thread_unguarded_var)
    return;

  if (is_indirect_ref)
    {
      attr = lookup_attribute ("point_to_guarded_by", DECL_ATTRIBUTES (vdecl));
      if (!attr)
        {
          attr = lookup_attribute ("point_to_guarded",
                                   DECL_ATTRIBUTES (vdecl));
          lock_specified = false;
        }
    }
  else
    {
      attr = lookup_attribute ("guarded_by", DECL_ATTRIBUTES (vdecl));
      if (!attr)
        {
          attr = lookup_attribute ("guarded", DECL_ATTRIBUTES (vdecl));
          lock_specified = false;
        }
    }

  /* If the variable does not have an attribute specifying that it should
     be protected by a lock, simply return.  */
  if (!attr)
    return;

  /* If the variable is a compiler-created temporary pointer, grab the
     original variable's decl from the debug expr (as we don't want to
     print out the temp name in the warnings. For reasons why we only
     need to do this for pointers, see lookup_tmp_var() in gimplify.c.  */
  if (is_indirect_ref && DECL_ARTIFICIAL (vdecl))
    {
      gcc_assert (DECL_DEBUG_EXPR_IS_FROM (vdecl) && DECL_DEBUG_EXPR (vdecl));
      vdecl = DECL_DEBUG_EXPR (vdecl);
      gcc_assert (DECL_P (vdecl));
    }

  if (lock_specified)
    {
      gcc_assert (TREE_VALUE (attr));
      lockable = TREE_VALUE (TREE_VALUE (attr));
      gcc_assert (lockable);
    }

  check_lock_required (lockable, vdecl, base_obj, is_indirect_ref,
                       excl_locks, shared_locks, locus, mode);
}

/* This routine is called when we see an indirect reference in our
   analysis of expressions. In an indirect reference, the pointer itself
   is accessed as a read. The parameter MODE indicates how the memory
   location pointed to by the PTR is being accessed (either read or write).  */

static void
handle_indirect_ref (tree ptr, struct pointer_set_t *excl_locks,
                     struct pointer_set_t *shared_locks,
                     const location_t *locus, enum access_mode mode)
{
  tree vdecl;

  /* The pointer itself is accessed as a read */
  analyze_expr (ptr, NULL_TREE,  false /* is_indirect_ref */, excl_locks,
                shared_locks, locus, TSA_READ);

  if (TREE_CODE (ptr) == SSA_NAME)
    {
      vdecl = SSA_NAME_VAR (ptr);
      if (!DECL_NAME (vdecl))
        {
          gimple def_stmt = SSA_NAME_DEF_STMT (ptr);
          if (is_gimple_assign (def_stmt)
              && (get_gimple_rhs_class (gimple_assign_rhs_code (def_stmt))
                  == GIMPLE_SINGLE_RHS))
            vdecl = gimple_assign_rhs1 (def_stmt);
        }
    }
  else
    vdecl = ptr;

  if (DECL_P (vdecl))
    process_guarded_by_attrs (vdecl, NULL_TREE, true /* is_indirect_ref */,
                              excl_locks, shared_locks, locus, mode);
  else
    analyze_expr (vdecl, NULL_TREE, true /* is_indirect_ref */,
                  excl_locks, shared_locks, locus, mode);

  return;
}

/* The main routine that handles gimple call statements.  */

static void
handle_call_gs (gimple call, struct bb_threadsafe_info *current_bb_info)
{
  tree fdecl = gimple_call_fndecl (call);
  int num_args = gimple_call_num_args (call);
  int arg_index = 0;
  tree arg_type = NULL_TREE;
  tree arg;
  tree lhs;
  location_t locus;

  if (!gimple_has_location (call))
    locus = input_location;
  else
    locus = gimple_location (call);

  /* If the callee fndecl is NULL, check if it is a virtual function,
     and if so, try to get its decl through the reference object.  */
  if (!fdecl)
    {
      tree callee = gimple_call_fn (call);
      if (TREE_CODE (callee) == OBJ_TYPE_REF)
        {
          tree objtype = TREE_TYPE (TREE_TYPE (OBJ_TYPE_REF_OBJECT (callee)));
          /* Check to make sure objtype is a valid type.
             OBJ_TYPE_REF_OBJECT does not always return the correct static type of the callee.   
             For example:  Given  foo(void* ptr) { ((Foo*) ptr)->doSomething(); }
             objtype will be void, not Foo.  Whether or not this happens depends on the details 
             of how a particular call is lowered to GIMPLE, and there is no easy fix that works 
             in all cases.  For now, we simply rely on gcc's type information; if that information 
             is not accurate, then the analysis will be less precise.
           */
          if (TREE_CODE (objtype) == RECORD_TYPE)
              fdecl = lang_hooks.get_virtual_function_decl (callee, objtype);                    
        }
    }

  /* The callee fndecl could be NULL, e.g., when the function is passed in
     as an argument.  */
  if (fdecl)
    {
      arg_type = TYPE_ARG_TYPES (TREE_TYPE (fdecl));
      if (TREE_CODE (TREE_TYPE (fdecl)) == METHOD_TYPE)
        {
          /* If an object, x, is guarded by a lock, whether or not
             calling x.foo() requires an exclusive lock depends on
             if foo() is const.  */
          enum access_mode rw_mode =
              lang_hooks.decl_is_const_member_func (fdecl) ? TSA_READ
                                                           : TSA_WRITE;

          /* A method should have at least one argument, i.e."this" pointer */
          gcc_assert (num_args);
          arg = gimple_call_arg (call, 0);

          /* If the base object (i.e. "this" object) is a SSA name of a temp
             variable, as shown in the following example (assuming the source
             code is 'base.method_call()'):

             D.5041_2 = &this_1(D)->base;
             result.0_3 = method_call (D.5041_2);

             we will need to get the rhs of the SSA def of the temp variable
             in order for the analysis to work correctly.  */
          if (TREE_CODE (arg) == SSA_NAME)
            {
              tree vdecl = SSA_NAME_VAR (arg);
              if (DECL_ARTIFICIAL (vdecl)
                  && !gimple_nop_p (SSA_NAME_DEF_STMT (arg)))
                {
                  gimple def_stmt = SSA_NAME_DEF_STMT (arg);
                  if (is_gimple_assign (def_stmt)
                      && (get_gimple_rhs_class (gimple_assign_rhs_code (
                          def_stmt)) == GIMPLE_SINGLE_RHS))
                    arg = gimple_assign_rhs1 (def_stmt);
                }
            }

          /* Analyze the base object ("this") if we are not instructed to
             ignore it.  */
          if (!(current_bb_info->reads_ignored && rw_mode == TSA_READ)
              && !(current_bb_info->writes_ignored && rw_mode == TSA_WRITE))
            {
              if (TREE_CODE (arg) == ADDR_EXPR)
                {
                  /* Handle smart/scoped pointers. They are not actual
                     pointers but they can be annotated with
                     "point_to_guarded_by" attribute and have overloaded "->"
                     and "*" operators, so we treat them as normal pointers.  */
                  if ((DECL_NAME (fdecl) == maybe_get_identifier ("operator->"))
                      || (DECL_NAME (fdecl)
                          == maybe_get_identifier ("operator*")))
                    handle_indirect_ref(TREE_OPERAND (arg, 0),
                                        current_bb_info->live_excl_locks,
                                        current_bb_info->live_shared_locks,
                                        &locus, rw_mode);
                  else
                    /* Handle the case of x.foo() or foo(&x) */
                    analyze_expr (TREE_OPERAND (arg, 0), NULL_TREE,
                                  false /* is_indirect_ref */,
                                  current_bb_info->live_excl_locks,
                                  current_bb_info->live_shared_locks, &locus,
                                  rw_mode);
                }
              else
                {
                  /* Handle the case of x->foo() or foo(x) */
                  gcc_assert (POINTER_TYPE_P (TREE_TYPE (arg)));
                  handle_indirect_ref(arg, current_bb_info->live_excl_locks,
                                      current_bb_info->live_shared_locks,
                                      &locus, rw_mode);
                }
            }

          /* Advance to the next argument */
          ++arg_index;
          arg_type = TREE_CHAIN (arg_type);
        }
    }

  /* Analyze the call's arguments if we are not instructed to ignore the
     reads. */
  if (!current_bb_info->reads_ignored
      && (!fdecl
          || !lookup_attribute("unprotected_read", DECL_ATTRIBUTES (fdecl))))
    {
      for ( ; arg_index < num_args; ++arg_index)
        {
          arg = gimple_call_arg (call, arg_index);
          if (!CONSTANT_CLASS_P (arg))
            {
              /* In analyze_expr routine, an address-taken expr (e.g. &x) will
                 be skipped because the variable itself is not actually
                 accessed. However, if an argument which is an address-taken
                 expr, say &x, is passed into a function for a reference
                 parameter, we want to treat it as a use of x because this is
                 what users expect and most likely x will be used in the
                 callee body anyway.  */
              if (arg_type
                  && TREE_CODE (TREE_VALUE (arg_type)) == REFERENCE_TYPE)
                {
                  tree canon_arg = arg;
                  /* The argument could be an SSA_NAME. Try to get the rhs of
                     its SSA_DEF.  */
                  if (TREE_CODE (arg) == SSA_NAME)
                    {
                      tree vdecl = SSA_NAME_VAR (arg);
                      if (DECL_ARTIFICIAL (vdecl)
                          && !gimple_nop_p (SSA_NAME_DEF_STMT (arg)))
                        {
                          gimple def_stmt = SSA_NAME_DEF_STMT (arg);
                          if (is_gimple_assign (def_stmt)
                              && (get_gimple_rhs_class (
                                  gimple_assign_rhs_code (def_stmt))
                                  == GIMPLE_SINGLE_RHS))
                            canon_arg = gimple_assign_rhs1 (def_stmt);
                        }
                    }
                  /* For an argument which is an ADDR_EXPR, say &x, that
                     corresponds to a reference parameter, remove the
                     address-taken operator and only pass 'x' to
                     analyze_expr.  */
                  if (TREE_CODE (canon_arg) == ADDR_EXPR)
                    arg = TREE_OPERAND (canon_arg, 0);
                }

              analyze_expr (arg, NULL_TREE, false /* is_indirect_ref */,
                            current_bb_info->live_excl_locks,
                            current_bb_info->live_shared_locks, &locus,
                            TSA_READ);
            }

          if (arg_type)
            arg_type = TREE_CHAIN (arg_type);
        }
    }

  /* Analyze the call's lhs if it exists and we are not instructed to ignore
     the writes.  */
  lhs = gimple_call_lhs (call);
  if (lhs != NULL_TREE && !current_bb_info->writes_ignored)
    analyze_expr (lhs, NULL_TREE, false /* is_indirect_ref */,
                  current_bb_info->live_excl_locks,
                  current_bb_info->live_shared_locks, &locus, TSA_WRITE);

  /* Process the attributes associated with the callee func decl.
     Note that we want to process the arguments first so that the callee
     func decl attributes have no effects on the arguments.  */
  if (fdecl)
    process_function_attrs (call, fdecl, current_bb_info, &locus);

  return;
}

/* The main routine that handles decls and expressions. It in turn calls
   other helper routines to analyze different kinds of trees.

   EXPR is the expression/decl being analyzed.
   BASE_OBJ is the base component of EXPR if EXPR is a component reference
   (e.g. b.a).
   EXCL_LOCKS and SHARED_LOCKS are the live lock sets when the control flow
   reaches EXPR.
   LOCUS is the source location of EXPR.
   MODE indicates whether the access is a read or a write.  */

static void
analyze_expr (tree expr, tree base_obj,  bool is_indirect_ref,
              struct pointer_set_t *excl_locks,
              struct pointer_set_t *shared_locks,
              const location_t *locus, enum access_mode mode)
{
  tree vdecl;

  if (EXPR_P (expr))
    {
      int nops;
      int i;
      /* For an address-taken expression (i.e. &x), the memory location of
         the operand is not actually accessed. So no thread safe check
         necessary here.  */
      if (TREE_CODE (expr) == ADDR_EXPR)
        return;

      if (TREE_CODE (expr) == INDIRECT_REF || TREE_CODE (expr) == MEM_REF)
        {
          tree ptr = TREE_OPERAND (expr, 0);
          handle_indirect_ref (ptr, excl_locks, shared_locks, locus, mode);
          return;
        }

      /* For a component reference, we need to look at both base and
         component trees.  */
      if (TREE_CODE (expr) == COMPONENT_REF)
        {
          tree base = TREE_OPERAND (expr, 0);
          tree component = TREE_OPERAND (expr, 1);
          analyze_expr (base, NULL_TREE, false /* is_indirect_ref */,
                        excl_locks, shared_locks, locus, mode);
          analyze_expr (component, base, is_indirect_ref,
                        excl_locks, shared_locks, locus, mode);
          return;
        }

      /* For all other expressions, just iterate through their operands
         and call analyze_expr on them recursively */
      nops = TREE_OPERAND_LENGTH (expr);
      for (i = 0; i < nops; i++)
        {
          tree op = TREE_OPERAND (expr, i);
          if (op != 0 && !CONSTANT_CLASS_P (op))
            analyze_expr (op, base_obj, false /* is_indirect_ref */,
                          excl_locks, shared_locks, locus, mode);
        }

      return;
    }

  /* If EXPR is a ssa name, grab its original variable decl.  */
  if (TREE_CODE (expr) == SSA_NAME)
    {
      vdecl = SSA_NAME_VAR (expr);
      /* If VDECL is a nameless temp variable and we are analyzing an indirect
         reference, we will need to grab and analyze the RHS of its SSA def
         because the RHS is the actual pointer that gets dereferenced.
         For example, in the following snippet of gimple IR, when we first
         analyzed S1, we only saw a direct access to foo.a_. Later, when
         analyzing the RHS of S2 (i.e. *D1803_1), which is an indirect
         reference, we need to look at foo.a_ again because what's really
         being referenced is *foo.a_.

         S1:  D.1803_1 = foo.a_;
         S2:  res.1_4 = *D.1803_1;  */
      if (!DECL_NAME (vdecl) && is_indirect_ref)
        {
          gimple def_stmt = SSA_NAME_DEF_STMT (expr);
          if (is_gimple_assign (def_stmt)
              && (get_gimple_rhs_class (gimple_assign_rhs_code (def_stmt))
                  == GIMPLE_SINGLE_RHS))
            vdecl = gimple_assign_rhs1 (def_stmt);
        }
    }
  else if (DECL_P (expr))
    vdecl = expr;
  else
    return;

  if (DECL_P (vdecl))
    process_guarded_by_attrs (vdecl, base_obj, is_indirect_ref,
                              excl_locks, shared_locks, locus, mode);
  else
    analyze_expr (vdecl, base_obj, is_indirect_ref, excl_locks, shared_locks,
                  locus, mode);
}

/* This is a helper function called by handle_cond_gs() to check if
   GS is a trylock call (or a simple expression fed by a trylock
   call that involves only logic "not" operations). And if so, grab and
   return the corresponding trylock_info structure. Otherwise, return NULL.
   In the process, *LOCK_ON_TRUE_PATH is set to indicate whether the true
   (control flow) path should be taken when the lock is successfully
   acquired.  */

static struct trylock_info *
get_trylock_info(gimple gs, bool *lock_on_true_path)
{
  while (1)
    {
      if (is_gimple_assign (gs))
        {
          enum tree_code subcode = gimple_assign_rhs_code (gs);
          if (subcode == SSA_NAME)
            {
              gs = SSA_NAME_DEF_STMT (gimple_assign_rhs1 (gs));
              continue;
            }
          else if (subcode == TRUTH_NOT_EXPR)
            {
              /* If the expr is a logic "not" operation, negate the value
                 pointed to by lock_on_true_apth and continue trace back
                 the expr's operand.  */
              *lock_on_true_path = !*lock_on_true_path;
              gs = SSA_NAME_DEF_STMT (gimple_assign_rhs1 (gs));
              continue;
            }
          else
            return NULL;
        }
      else if (is_gimple_call (gs))
        {
          tree fdecl = gimple_call_fndecl (gs);
          /* The function decl could be null in some cases, e.g.
             a function pointer passed in as a parameter.  */
          if (fdecl
              && (lookup_attribute ("exclusive_trylock",
                                    DECL_ATTRIBUTES (fdecl))
                  || lookup_attribute ("shared_trylock",
                                       DECL_ATTRIBUTES (fdecl))))
            {
              void **entry = pointer_map_contains (trylock_info_map, gs);
              gcc_assert (entry);
              return (struct trylock_info *)*entry;
            }
          else
            return NULL;
        }
      else
        return NULL;
    }

  gcc_unreachable ();

  return NULL;
}

/* This routine determines whether the given condition statment (COND_STMT) is
   fed by a trylock call through expressions involving only "not", "equal"
   "not-equal" operations. Here are several examples where the condition
   statements are fed by trylock calls:

     (1) if (mu.Trylock()) {
           ...
         }

     (2) bool a = mu.Trylock();
         bool b = !a;
         if (b) {
           ...
         }

     (3) int a = pthread_mutex_trylock(mu);
         bool b = (a == 1);
         if (!b) {
           ...
         }

     (4) int a = pthread_mutex_trylock(mu);
         bool b = (a != 0);
         bool c = b;
         if (c == true) {
           ...
         }

   If COND_STMT is determined to be fed by a trylock call, this routine
   populates the data structure pointed to by CURRENT_BB_INFO, and
   sets *LOCK_ON_TRUE_PATH to indicate whether the true (control flow) path
   should be taken when the lock is successfully acquired.  */

static void
handle_cond_gs (gimple cond_stmt, struct bb_threadsafe_info *current_bb_info)
{
  gimple gs = NULL;
  bool lock_on_true_path = true;
  bool is_cond_stmt = true;
  edge true_edge, false_edge;
  basic_block bb = gimple_bb (cond_stmt);
  tree op0 = gimple_cond_lhs (cond_stmt);
  tree op1 = gimple_cond_rhs (cond_stmt);
  enum tree_code subcode = gimple_cond_code (cond_stmt);

  /* We only handle condition expressions with "equal" or "not-equal"
     operations.  */
  if (subcode != EQ_EXPR && subcode != NE_EXPR)
    return;

  /* In the new gimple tuple IR, a single operand if-condition such as

       if (a) {
       }

     would be represented as

       GIMPLE_COND <NE_EXPR, a, 0, TRUE_LABEL, FALSE_LABEL>

     Here we are trying this case and grab the SSA definition of a.  */

  if (TREE_CODE (op0) == SSA_NAME
      && subcode == NE_EXPR
      && TREE_CODE (op1) == INTEGER_CST
      && TREE_INT_CST_LOW (op1) == 0)
    {
      gs = SSA_NAME_DEF_STMT (op0);
      is_cond_stmt = false;
    }

  /* Iteratively back-tracing the SSA definitions to determine if the
     condition expression is fed by a trylock call. If so, record the
     edge that indicates successful lock acquisition.  */
  while (1)
    {
      if (is_cond_stmt || is_gimple_assign (gs))
        {
          if (!is_cond_stmt)
            {
              gcc_assert (gs);
              subcode = gimple_assign_rhs_code (gs);
            }

          if (subcode == SSA_NAME)
            {
              gcc_assert (!is_cond_stmt);
              gs = SSA_NAME_DEF_STMT (gimple_assign_rhs1 (gs));
              continue;
            }
          else if (subcode == TRUTH_NOT_EXPR)
            {
              gcc_assert (!is_cond_stmt);
              lock_on_true_path = !lock_on_true_path;
              gs = SSA_NAME_DEF_STMT (gimple_assign_rhs1 (gs));
              continue;
            }
          else if (subcode == EQ_EXPR || subcode == NE_EXPR)
            {
              struct trylock_info *tryinfo;
              int const_op;
              if (!is_cond_stmt)
                {
                  op0 = gimple_assign_rhs1 (gs);
                  op1 = gimple_assign_rhs2 (gs);
                }
              if (TREE_CODE (op0) == INTEGER_CST
                  && TREE_CODE (op1) == SSA_NAME)
                {
                  const_op = TREE_INT_CST_LOW (op0);
                  tryinfo = get_trylock_info (SSA_NAME_DEF_STMT (op1),
                                              &lock_on_true_path);
                }
              else if (TREE_CODE (op1) == INTEGER_CST
                       && TREE_CODE (op0) == SSA_NAME)
                {
                  const_op = TREE_INT_CST_LOW (op1);
                  tryinfo = get_trylock_info (SSA_NAME_DEF_STMT (op0),
                                              &lock_on_true_path);
                }
              else
                return;

              if (tryinfo)
                {
                  struct pointer_set_t *edge_locks;
                  /* Depending on the operation (eq or neq) and whether the
                     succ_retval of the trylock is the same as the constant
                     integer operand, we might need to toggle the value
                     pointed to bylock_on_true_path. For example, if the
                     succ_retval of TryLock() is 0 and the cond expression is
                     (mu.TryLock() != 0), we need to negate the
                     lock_on_true_path value.  */ 
                  if ((tryinfo->succ_retval == const_op
                       && subcode == NE_EXPR)
                      || (tryinfo->succ_retval != const_op
                          && subcode == EQ_EXPR))
                    lock_on_true_path = !lock_on_true_path;

                  edge_locks = pointer_set_copy (tryinfo->locks);
                  if (tryinfo->is_exclusive)
                    current_bb_info->edge_exclusive_locks = edge_locks;
                  else
                    current_bb_info->edge_shared_locks = edge_locks;
                  break;
                }
              else
                return;
            }
          else
            return;
        }
      else if (is_gimple_call (gs))
        {
          struct trylock_info *tryinfo;
          tryinfo = get_trylock_info (gs, &lock_on_true_path);
          if (tryinfo)
            {
              struct pointer_set_t *edge_locks;
              /* If the succ_retval of the trylock is 0 (or boolean
                 "false"), we need to negate the value pointed to by
                 lock_on_true_path.  */
              if (tryinfo->succ_retval == 0)
                lock_on_true_path = !lock_on_true_path;
              edge_locks = pointer_set_copy (tryinfo->locks);
              if (tryinfo->is_exclusive)
                current_bb_info->edge_exclusive_locks = edge_locks;
              else
                current_bb_info->edge_shared_locks = edge_locks;
              break;
            }
          else
            return;
        }
      else
        return;
    }

  gcc_assert (current_bb_info->edge_exclusive_locks
              || current_bb_info->edge_shared_locks);
  extract_true_false_edges_from_block (bb, &true_edge, &false_edge);
  if (lock_on_true_path)
    current_bb_info->trylock_live_edge = true_edge;
  else
    current_bb_info->trylock_live_edge = false_edge;
}

/* This is a helper function to populate the LOCK_SET with the locks
   specified in ATTR's arguments.  */

static void
populate_lock_set_with_attr (struct pointer_set_t *lock_set, tree attr)
{
  tree arg;

  for (arg = TREE_VALUE (attr); arg; arg = TREE_CHAIN (arg))
    {
      tree lock = TREE_VALUE (arg);
      gcc_assert (lock);
      /* If the lock is an integer specifying the argument position, grab
         the corresponding formal parameter.  */
      if (TREE_CODE (lock) == INTEGER_CST)
        {
          int lock_pos = TREE_INT_CST_LOW (lock);
          int i;
          tree parm;
          for (parm = DECL_ARGUMENTS (current_function_decl), i = 1;
               parm;
               parm = TREE_CHAIN (parm), ++i)
            if (i == lock_pos)
              break;
          gcc_assert (parm);
          lock = parm;
        }

      /* Canonicalize the lock before we add it to the lock set.  */
      if (!DECL_P (lock))
        lock = get_canonical_lock_expr (lock, NULL_TREE,
                                        false /* is_temp_expr */, NULL_TREE);

      /* Add the lock to the lock set.  */
      pointer_set_insert (lock_set, lock);

      /* If there are unbound locks when the thread safety attributes were
         parsed, we should try to bind them now if we see any lock declaration
         that matches the name of the unbound lock.  */
      if (unbound_lock_map
          && (TREE_CODE (lock) == VAR_DECL
              || TREE_CODE (lock) == PARM_DECL
              || TREE_CODE (lock) == FIELD_DECL))
        {
          tree lock_id = DECL_NAME (lock);
          void **entry = pointer_map_contains (unbound_lock_map, lock_id);
          if (entry)
            *entry = lock;
        }
    }
}

/* This is a helper function passed in (as a parameter) to pointer_set_traverse
   when we traverse the set containing locks that are not properly released 
   and emit a warning message for each of them. By improper release, we meant
   the places these locks are released are not control equivalent to where
   they are acquired. The improperly-released lock set was calculated when we
   reach a joint point during the data flow analysis. Any lock that is not
   in all of the preceding basic blocks' live-out sets is considered not
   released locally. REPORTED set contains the locks for which we have
   already printed out a warning message. We use this set to avoid emitting
   duplicate warnings for a lock. Here is an example why duplicate warnings
   could be emitted if we don't keep a REPORTED set.

          B1:
            mu.Lock()

            /     \   \
           /       \   \
      B2:           B3: B4:
        mu.Unlock()
           \        /   /
            \      /   /

              B5:

   When we reach B5, "mu" would be in the live out sets of B3 and B4, but
   not that of B2. If we do a live out set intersection between B2 and B3
   first, and then intersect the resulting set with B4's live out set, we
   could've emitted the warning message for "mu" twice if we had not kept
   a reported set.  */

static bool
warn_locally_unreleased_locks (const void *lock, void *reported)
{
  char lname[LOCK_NAME_LEN];
  void **entry;
  tree lock_tree = CONST_CAST_TREE ((const_tree) lock);
  location_t *loc;
  struct pointer_set_t *reported_unreleased_locks;

  reported_unreleased_locks = (struct pointer_set_t *) reported;

  /* If this unreleased lock has been reported or is a universal lock (i.e.
     error_mark_node), don't emit a warning message for it again.  */
  if (lock != error_mark_node
      && !pointer_set_contains (reported_unreleased_locks, lock))
    {
      entry = pointer_map_contains (lock_locus_map, lock);
      if (entry)
        {
          loc = (location_t *) *entry;
          warning_at (*loc, OPT_Wthread_safety,
                      G_("Lock %s (acquired at line %d) is not released at"
                         " the end of its scope in function %qE"),
                      dump_expr_tree (lock_tree, lname),
                      LOCATION_LINE (*loc),
                      DECL_NAME (current_function_decl));
        }
      else
        warning_at (DECL_SOURCE_LOCATION (current_function_decl),
                    OPT_Wthread_safety,
                    G_("Lock %s (held at entry) is released on some but not all"
                       " control flow paths in function %qE"),
                    dump_expr_tree (lock_tree, lname),
                    DECL_NAME (current_function_decl));

      pointer_set_insert (reported_unreleased_locks, lock);
    }

  return true;
}

/* This is a helper function passed in (as a parameter) to traverse_pointer_set
   when we iterate through the set of locks that are not released at the end
   of a function. A warning message is emitted for each of them unless they
   were not acquired in the current function (i.e. acquired before calling
   the current function).  */

static bool
warn_unreleased_locks (const void *lock, void *locks_at_entry)
{
  /* If the unreleased lock was actually acquired before calling the current
     function, we don't emit a warning for it as the lock is not expected to
     be released in the current function anyway.  Also if the lock is a
     universal lock (i.e. error_mark_node), don't emit a warning either.  */
  if (lock != error_mark_node
      && !pointer_set_contains ((struct pointer_set_t *) locks_at_entry, lock))
    {
      char lname[LOCK_NAME_LEN];
      void **entry = pointer_map_contains (lock_locus_map, lock);
      tree lock_tree = CONST_CAST_TREE ((const_tree) lock);
      location_t *loc;
      gcc_assert (entry);
      loc = (location_t *) *entry;
      warning_at (*loc, OPT_Wthread_safety,
                  G_("Lock %s (acquired at line %d) is not released at the end"
                     " of function %qE"),
                  dump_expr_tree (lock_tree, lname),
                  LOCATION_LINE (*loc),
                  DECL_NAME (current_function_decl));
    }

  return true;
}

/* This is a helper function passed in (as a parameter) to
   pointer_map_traverse when we delete lock_locus_map.  */

static bool
delete_lock_locations (const void * ARG_UNUSED (lock),
                       void **entry, void * ARG_UNUSED (data))
{
  XDELETE (*entry);
  return true;
}

/* This is a helper function passed in (as a parameter) to
   pointer_map_traverse when we delete trylock_info_map.  */

static bool
delete_trylock_info (const void * ARG_UNUSED (call),
                     void **entry, void * ARG_UNUSED (data))
{
  struct trylock_info *tryinfo = (struct trylock_info *)*entry;
  gcc_assert (tryinfo);
  pointer_set_destroy (tryinfo->locks);
  XDELETE (tryinfo);
  return true;
}

/* Helper function for walk_gimple_stmt() that is called on each gimple
   statement. Except for call statements and SSA definitions of namesless
   temp variables, the operands of the statements will be analyzed by
   analyze_op_r().  */

static tree
analyze_stmt_r (gimple_stmt_iterator *gsi, bool *handled_ops,
                struct walk_stmt_info *wi)
{
  gimple stmt = gsi_stmt (*gsi);
  struct bb_threadsafe_info *current_bb_info =
      (struct bb_threadsafe_info *) wi->info;

  if (is_gimple_call (stmt))
    {
      handle_call_gs (stmt, current_bb_info);
      /* The arguments of the call is already analyzed in handle_call_gs.
         Set *handled_ops to true to skip calling analyze_op_r later.  */
      *handled_ops = true;
    }
  else if (gimple_code (stmt) == GIMPLE_COND)
    handle_cond_gs (stmt, current_bb_info);

  return NULL_TREE;
}

/* Helper function for walk_gimple_stmt() that is called on each operand of
   a visited gimple statement.  */

static tree
analyze_op_r (tree *tp, int *walk_subtrees, void *data)
{
  struct walk_stmt_info *wi = (struct walk_stmt_info *) data;
  struct bb_threadsafe_info *current_bb_info =
      (struct bb_threadsafe_info *) wi->info;
  gimple stmt = gsi_stmt (wi->gsi);
  enum access_mode mode = wi->is_lhs ? TSA_WRITE : TSA_READ;
  location_t locus;

  /* Analyze the statement operand if we are not instructed to ignore the
     reads or writes and if it is not a constant.  */
  if (!(current_bb_info->reads_ignored && mode == TSA_READ)
      && !(current_bb_info->writes_ignored && mode == TSA_WRITE)
      && !CONSTANT_CLASS_P (*tp))
    {
      if (!gimple_has_location (stmt))
        locus = input_location;
      else
        locus = gimple_location (stmt);

      analyze_expr(*tp, NULL_TREE, false /* is_indirect_ref */,
                   current_bb_info->live_excl_locks,
                   current_bb_info->live_shared_locks, &locus, mode);
    }

  *walk_subtrees = 0;

  return NULL_TREE;
}

/* Perform thread safety analysis using the attributes mentioned above
   (see comments at the beginning of the file).  The analysis pass uses
   a single-pass (or single iteration) data-flow analysis to calculate
   live lock sets at each program point, using the attributes to decide
   when to add locks to the live sets and when to remove them from the
   sets. With the live lock sets and the attributes attached to shared
   variables and functions, we are able to check whether the variables
   and functions are well protected. Note that the reason why we don't
   need iterative data flow analysis is because critical sections across
   back edges are considered a bad practice.

   The single-iteration data flow analysis is performed by visiting
   each basic block only once in a topological order. The topological
   traversal is achieved by maintaining a work list (or ready list) which
   is seeded with the successors of the function's entry block. A basic
   block is added to the work list when all of its predecessors have been
   visited. During the traversal, back edges are ignored.  */

static unsigned int
execute_threadsafe_analyze (void)
{
  size_t append_ptr = 0, visit_ptr = 0;
  basic_block bb;
  edge e;
  edge_iterator ei;
  tree fdecl_attrs;
  struct bb_threadsafe_info *threadsafe_info;
  struct pointer_set_t *live_excl_locks_at_entry;
  struct pointer_set_t *live_shared_locks_at_entry;
  tree attr;
  basic_block *worklist;
  int i;
  int old_mismatched_lock_acq_rel = warn_thread_mismatched_lock_acq_rel;

  /* Skip the compiler-generated functions.  */
  if (DECL_ARTIFICIAL (current_function_decl))
    return 0;

  /* Constructors and destructors should only be accessed by a single
     thread and therefore are ignored here.  */
  if (lang_hooks.decl_is_constructor (current_function_decl)
      || lang_hooks.decl_is_destructor (current_function_decl))
    return 0;

  /* If the current function is annotated as a locking or unlocking primitive,
     or if it is marked to be skipped (with no_thread_safety_analysis
     attribute), ignore it.  */
  fdecl_attrs = DECL_ATTRIBUTES (current_function_decl);
  if (lookup_attribute("exclusive_lock", fdecl_attrs)
      || lookup_attribute("shared_lock", fdecl_attrs)
      || lookup_attribute("exclusive_trylock", fdecl_attrs)
      || lookup_attribute("shared_trylock", fdecl_attrs)
      || lookup_attribute("unlock", fdecl_attrs)
      || lookup_attribute("no_thread_safety_analysis", fdecl_attrs))
    return 0;

  /* If this is the first function of the current compilation unit, we need
     to build the transitive acquired_after sets for the locks.  */
  if (lock_acquired_after_map && !transitive_acq_after_sets_built)
    {
      build_transitive_acquired_after_sets();
      transitive_acq_after_sets_built = true;
    }

  /* Mark the back edges in the cfg so that we can skip them later
     in our (single-iteration) data-flow analysis.  */
  mark_dfs_back_edges ();

  /* Allocate lock-related global maps.  */
  scopedlock_to_lock_map = pointer_map_create ();
  lock_locus_map = pointer_map_create ();
  trylock_info_map = pointer_map_create ();
  gimple_call_tab = htab_create (10, call_gs_hash, call_gs_eq, NULL);

  /* Initialize the pretty printer buffer for warning emitting.  */
  pp_construct (&pp_buf, /* prefix */ NULL, /* line-width */ 0);

  /* Allocate the threadsafe info array.
     Use XCNEWVEC to clear out the info.  */
  threadsafe_info = XCNEWVEC(struct bb_threadsafe_info, last_basic_block);

  /* Since the back/complex edges are not traversed in the analysis,
     mark them as visited.  */
  FOR_EACH_BB (bb)
    {
      FOR_EACH_EDGE (e, ei, bb->preds)
        {
          if (e->flags & (EDGE_DFS_BACK | EDGE_COMPLEX))
            ++threadsafe_info[bb->index].n_preds_visited;
        }
    }

  /* Populate ENTRY_BLOCK's live out sets with "exclusive_locks_required"
     and "shared_locks_required" attributes.  */
  live_excl_locks_at_entry = pointer_set_create();
  live_shared_locks_at_entry = pointer_set_create();

  attr = lookup_attribute("exclusive_locks_required",
                          DECL_ATTRIBUTES (current_function_decl));
  if (attr)
    populate_lock_set_with_attr(live_excl_locks_at_entry, attr);

  attr = lookup_attribute("shared_locks_required",
                          DECL_ATTRIBUTES (current_function_decl));
  if (attr)
    populate_lock_set_with_attr(live_shared_locks_at_entry, attr);

  threadsafe_info[ENTRY_BLOCK_PTR->index].liveout_exclusive_locks =
      live_excl_locks_at_entry;
  threadsafe_info[ENTRY_BLOCK_PTR->index].liveout_shared_locks =
      live_shared_locks_at_entry;

  threadsafe_info[ENTRY_BLOCK_PTR->index].weak_released_locks =
      pointer_set_create ();

  /* Allocate the worklist of BBs for topological traversal, which is
     basically an array of pointers to basic blocks.  */
  worklist = XNEWVEC (basic_block, n_basic_blocks);

  /* Seed the worklist by adding the successors of the entry block
     to the worklist.  */
  FOR_EACH_EDGE (e, ei, ENTRY_BLOCK_PTR->succs)
    {
      worklist[append_ptr++] = e->dest;
    }

  /* The basic blocks in the current function are traversed in a topological
     order. Both "visit_ptr" and "append_ptr" are indices to the worklist
     array and initialized to zero. "append_ptr" is incremented whenever a BB
     is added to the work list, while "visit_ptr" is incremented when we
     visit a BB. When "visit_ptr" catches up with "append_ptr", the traversal
     is done.  */
  while (visit_ptr != append_ptr)
    {
      struct pointer_set_t *reported_unreleased_locks = pointer_set_create();
      struct bb_threadsafe_info *current_bb_info;
      gimple_stmt_iterator gsi;

      bb = worklist[visit_ptr++];
      current_bb_info = &threadsafe_info[bb->index];
      current_bb_info->visited = true;

      /* First create the live-in lock sets for bb by intersecting all of its
         predecessors' live-out sets */
      FOR_EACH_EDGE (e, ei, bb->preds)
        {
          basic_block pred_bb = e->src;
          struct pointer_set_t *unreleased_locks;
          struct pointer_set_t *pred_liveout_excl_locks;
          struct pointer_set_t *pred_liveout_shared_locks;

          /* Skip the back/complex edge.  */
          if (e->flags & (EDGE_DFS_BACK | EDGE_COMPLEX))
            continue;

          /* If this is the first predecessor of bb's, simply copy the
             predecessor's live-out sets and reads/writes_ignored flags
             to bb's live (working) sets and corresponding flags.  */
          if (current_bb_info->live_excl_locks == NULL)
            {
              current_bb_info->reads_ignored =
                  threadsafe_info[pred_bb->index].reads_ignored;
              current_bb_info->writes_ignored =
                  threadsafe_info[pred_bb->index].writes_ignored;
              current_bb_info->live_excl_locks = pointer_set_copy (
                  threadsafe_info[pred_bb->index].liveout_exclusive_locks);
              current_bb_info->live_shared_locks = pointer_set_copy (
                  threadsafe_info[pred_bb->index].liveout_shared_locks);
              current_bb_info->weak_released_locks = pointer_set_copy (
                  threadsafe_info[pred_bb->index].weak_released_locks);
              /* If the pred bb has a trylock call and its edge to the current
                 bb is the one for successful lock acquisition, add the
                 trylock live sets to the bb's live working sets.  */
              if (threadsafe_info[pred_bb->index].trylock_live_edge == e)
                {
                  gcc_assert (
                      threadsafe_info[pred_bb->index].edge_exclusive_locks
                      || threadsafe_info[pred_bb->index].edge_shared_locks);
                  if (threadsafe_info[pred_bb->index].edge_exclusive_locks)
                    pointer_set_union_inplace (
                        current_bb_info->live_excl_locks,
                        threadsafe_info[pred_bb->index].edge_exclusive_locks);
                  if (threadsafe_info[pred_bb->index].edge_shared_locks)
                    pointer_set_union_inplace (
                        current_bb_info->live_shared_locks,
                        threadsafe_info[pred_bb->index].edge_shared_locks);
                }
              continue;
            }

          unreleased_locks = pointer_set_create();
          pred_liveout_excl_locks =
              threadsafe_info[pred_bb->index].liveout_exclusive_locks;
          pred_liveout_shared_locks =
              threadsafe_info[pred_bb->index].liveout_shared_locks;
          
          /* If the pred bb has a trylock call and its edge to the current
             bb is the one for successful lock acquisition, add the
             trylock live sets to the pred bb's live-out sets.  */
          if (threadsafe_info[pred_bb->index].trylock_live_edge == e)
            {
              gcc_assert(threadsafe_info[pred_bb->index].edge_exclusive_locks
                         || threadsafe_info[pred_bb->index].edge_shared_locks);
              /* The following code will clobber the original contents of
                 edge_exclusive_locks set and/or edge_shared_locks set of
                 the pred bb, but that is fine because they will not be
                 used in the future (as this edge is visited only once in
                 our single-iteration data-flow analysis).  */
              if (threadsafe_info[pred_bb->index].edge_exclusive_locks)
                {
                  pred_liveout_excl_locks =
                      threadsafe_info[pred_bb->index].edge_exclusive_locks;
                  pointer_set_union_inplace (pred_liveout_excl_locks,
                      threadsafe_info[pred_bb->index].liveout_exclusive_locks);
                }
                
              if (threadsafe_info[pred_bb->index].edge_shared_locks)
                {
                  pred_liveout_shared_locks =
                      threadsafe_info[pred_bb->index].edge_shared_locks;
                  pointer_set_union_inplace (pred_liveout_shared_locks,
                         threadsafe_info[pred_bb->index].liveout_shared_locks);
                }
            }

          /* Logical-and'ing the current BB's reads/writes_ignored flags with
             predecessor's flags. These flags will be true at the beginning
             of a BB only when they are true at the end of all the
             precedecessors.  */
          current_bb_info->reads_ignored &=
              threadsafe_info[pred_bb->index].reads_ignored;
          current_bb_info->writes_ignored &=
              threadsafe_info[pred_bb->index].writes_ignored;

          /* Intersect the current (working) live set with the predecessor's
             live-out set. Locks that are not in the intersection (i.e.
             complement set) should be reported as improperly released.  */
          pointer_set_intersection_complement (
              current_bb_info->live_excl_locks,
              pred_liveout_excl_locks,
              unreleased_locks);
          pointer_set_intersection_complement (
              current_bb_info->live_shared_locks,
              pred_liveout_shared_locks,
              unreleased_locks);

          /* Take the union of the weak released lock sets of the
             predecessors.  */
          pointer_set_union_inplace (
              current_bb_info->weak_released_locks,
              threadsafe_info[pred_bb->index].weak_released_locks);

          /* If a lock is released by a Release function of a scoped lock on
             some control-flow paths (but not all), the lock would still be
             live on other paths, which is OK as the destructor of the scoped
             lock will eventually release the lock. We don't want to emit
             bogus warnings about the release inconsistency at the
             control-flow join point. To avoid that, we simply add those
             weakly-released locks in the REPORTED_UNRELEASED_LOCKS set.  */
          pointer_set_union_inplace (
              reported_unreleased_locks,
              current_bb_info->weak_released_locks);

          /* Emit warnings for the locks that are not properly released.
             That is, the places they are released are not control
             equivalent to where they are acquired.  */
          if (warn_thread_mismatched_lock_acq_rel)
            pointer_set_traverse (unreleased_locks,
                                  warn_locally_unreleased_locks,
                                  reported_unreleased_locks);

          pointer_set_destroy (unreleased_locks);
        }

      pointer_set_destroy (reported_unreleased_locks);
      gcc_assert (current_bb_info->live_excl_locks != NULL);

      /* Now iterate through each statement of the bb and analyze its
         operands.  */
      for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
        {
          struct walk_stmt_info wi;
          memset (&wi, 0, sizeof (wi));
          wi.info = (void *) current_bb_info;
          walk_gimple_stmt (&gsi, analyze_stmt_r, analyze_op_r, &wi);
        }

      /* Now that we have visited the current bb, check if any of its
         successors can be added to the work list.  */
      FOR_EACH_EDGE (e, ei, bb->succs)
        {
          basic_block succ_bb;
          if (e->flags & (EDGE_DFS_BACK | EDGE_COMPLEX))
            continue;
          succ_bb = e->dest;
          /* Since we skip the back edges, we shouldn't see a visited basic
             block again here.  */
          gcc_assert (!threadsafe_info[succ_bb->index].visited);
          if ((++threadsafe_info[succ_bb->index].n_preds_visited) ==
              EDGE_COUNT(succ_bb->preds))
            worklist[append_ptr++] = succ_bb;
        }

      current_bb_info->liveout_exclusive_locks =
          current_bb_info->live_excl_locks;
      current_bb_info->liveout_shared_locks =
          current_bb_info->live_shared_locks;
    }

  /* If there are still live locks at the end of the function that are held
     at the entry of the function (i.e. not in the function's locks_required
     sets), emit warning messages for them.
     Note that the exit block may not be reachable from the entry (e.g. when
     there are abort() or exit() calls that collectively dominate the exit
     block). We need to check whether its liveout_exclusive_locks and 
     liveout_shared_locks are empty before trying to traverse them.
     TODO: Besides the exit block, we also need to check the basic blocks
     that don't have any successors as they are practically "exit" blocks
     as well.  */
  if (warn_thread_mismatched_lock_acq_rel)
    {
      if (threadsafe_info[EXIT_BLOCK_PTR->index].liveout_exclusive_locks)
        pointer_set_traverse(
            threadsafe_info[EXIT_BLOCK_PTR->index].liveout_exclusive_locks,
            warn_unreleased_locks, live_excl_locks_at_entry);
      if (threadsafe_info[EXIT_BLOCK_PTR->index].liveout_shared_locks)
        pointer_set_traverse(
            threadsafe_info[EXIT_BLOCK_PTR->index].liveout_shared_locks,
            warn_unreleased_locks, live_shared_locks_at_entry);
    }

  /* Free the allocated data structures.  */
  for (i = 0; i < last_basic_block; ++i)
    {
      if (threadsafe_info[i].liveout_exclusive_locks != NULL)
        {
          pointer_set_destroy(threadsafe_info[i].liveout_exclusive_locks);
          pointer_set_destroy(threadsafe_info[i].liveout_shared_locks);
        }
      if (threadsafe_info[i].weak_released_locks != NULL)
        pointer_set_destroy (threadsafe_info[i].weak_released_locks);
      if (threadsafe_info[i].edge_exclusive_locks != NULL)
        pointer_set_destroy (threadsafe_info[i].edge_exclusive_locks);
      if (threadsafe_info[i].edge_shared_locks != NULL)
        pointer_set_destroy (threadsafe_info[i].edge_shared_locks);
    }

  XDELETEVEC (threadsafe_info);
  XDELETEVEC (worklist);

  pp_clear_output_area (&pp_buf);
  pointer_map_destroy (scopedlock_to_lock_map);
  pointer_map_traverse (lock_locus_map, delete_lock_locations, NULL);
  pointer_map_destroy (lock_locus_map);
  pointer_map_traverse (trylock_info_map, delete_trylock_info, NULL);
  pointer_map_destroy (trylock_info_map);
  htab_delete (gimple_call_tab);
  gimple_call_tab = NULL;

  /* The flag that controls the warning of mismatched lock acquire/release
     could be turned off when we see an unlock primitive with an unsupported
     lock name/expression (see process_function_attrs). We need to restore
     the original value of the flag after we finish analyzing the current
     function.  */
  if (old_mismatched_lock_acq_rel != warn_thread_mismatched_lock_acq_rel)
    warn_thread_mismatched_lock_acq_rel = old_mismatched_lock_acq_rel;

  return 0;
}

static bool
gate_threadsafe_analyze (void)
{
  /* FIXME google/main - Annotalysis is currently broken.  */
  return false;
}

struct gimple_opt_pass pass_threadsafe_analyze =
{
  {
    GIMPLE_PASS,
    "threadsafe_analyze",                 /* name */
    gate_threadsafe_analyze,              /* gate */
    execute_threadsafe_analyze,           /* execute */
    NULL,                                 /* sub */
    NULL,                                 /* next */
    0,                                    /* static_pass_number */
    TV_TREE_THREADSAFE,                   /* tv_id */
    PROP_cfg | PROP_ssa,                  /* properties_required */
    0,                                    /* properties_provided */
    0,                                    /* properties_destroyed */
    0,                                    /* todo_flags_start */
    TODO_dump_func                        /* todo_flags_finish */
  }
};

#include "gt-tree-threadsafe-analyze.h"
