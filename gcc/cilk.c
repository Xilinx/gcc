/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains the CilkPlus Intrinsics
   Copyright (C) 2011  Free Software Foundation, Inc.
   Contributed by Balaji V. Iyer <balaji.v.iyer@intel.com>,
   Intel Corporation

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "langhooks.h"
#include "expr.h"
#include "optabs.h" /* for sync_lock_release in detach */
#include "recog.h"
#include "cilk.h"
#include "gimple.h"
#include "tree-iterator.h"
#include "c-family/c-common.h"
#include "toplev.h" /* only correct declaration of warning() */

tree cilk_trees[(int) CILK_TI_MAX];

static HOST_WIDE_INT worker_tail_offset;

static tree
cilk_declare_looper (const char *name, tree type)
{
  tree cb, ft, fn;

  cb = build_function_type_list (void_type_node,
				 ptr_type_node, type, type,
				 NULL_TREE);
  cb = build_pointer_type (cb);

  ft = build_function_type_list (void_type_node,
				 cb, ptr_type_node, type,
				 integer_type_node, NULL_TREE);
  fn = build_fn_decl (name, ft);
  TREE_NOTHROW (fn) = 0;

  return fn;
}


static tree
add_field (const char *name, tree type, tree fields)
{
  tree  t = get_identifier(name);
  tree field = build_decl (EXPR_LOCATION(t), FIELD_DECL, t, type);
  TREE_CHAIN (field) = fields;
  return field;
}

static tree
install_builtin (const char *name, tree fntype, enum built_in_function code,
		 bool publish)
{
  tree fndecl = build_fn_decl (name, fntype);
  DECL_BUILT_IN_CLASS (fndecl) = BUILT_IN_NORMAL;
  DECL_FUNCTION_CODE (fndecl) = code;
  if (publish)
    fndecl = lang_hooks.decls.pushdecl (fndecl);
  implicit_built_in_decls[code] = fndecl;
  return fndecl;
}

static void
mark_cold (tree fndecl)
{
  DECL_ATTRIBUTES (fndecl) = tree_cons (get_identifier ("cold"), NULL_TREE,
					DECL_ATTRIBUTES (fndecl));
}

/* This function wil initialize/create all the builtin cilk plus functions */
void
cilk_init_builtins (void)
{
  tree worker, frame, fields;

  tree wptr_type;

  /* The C declaration is
     struct stack_frame *volatile *volatile tail;

     The field type is volatile.  It points to a volatile type.
     The underlying stack_frame object is NOT volatile. */
  tree fptr_type; /* pointer to cilk_frame_type */
  tree fptr_v_type; /* above type, volatile */
  tree fptr_v_ptr_type; /* pointer to above type */
  tree fptr_v_ptr_v_type; /* above type, volatile */
  tree fptr_arglist; /* (frame *) */
  tree fptr_fun; /* void(frame *) */
  tree frame_pred;
  tree s_type_node;

  /* Make the frame and worker tags first because they reference each other. */
  worker = lang_hooks.types.make_type (RECORD_TYPE);
  wptr_type = build_pointer_type (worker);
  frame = lang_hooks.types.make_type (RECORD_TYPE);
  fptr_type = build_pointer_type (frame);
  fptr_arglist = tree_cons (NULL_TREE, fptr_type, void_list_node);
  fptr_fun = build_function_type (void_type_node, fptr_arglist); 
  frame_pred = build_function_type (boolean_type_node, fptr_arglist);

  /*
    typedef struct __cilkrts_stack frame {
    uint32_t flags;
    uint32_t spare;
    struct __cilkrts_frame_t *parent;
    (type) *worker;
    (type) *exception;
    void   *ctx[5];
    } __cilkrts_frame_t;
  */

  fields = NULL_TREE;
  fields = add_field ("flags", unsigned_type_node, fields);
  cilk_trees[CILK_TI_FRAME_FLAGS] = fields;
  fields = add_field ("size", integer_type_node, fields);
  fields = add_field ("call_parent", fptr_type, fields);
  cilk_trees[CILK_TI_FRAME_PARENT] = fields;
  fields = add_field ("worker", wptr_type, fields);
  /* The type may need to be void * volatile to ensure
     it is not cached across calls to Cilk functions. */
  cilk_trees[CILK_TI_FRAME_WORKER] = fields;
  fields = add_field ("except_data", ptr_type_node, fields);
  cilk_trees[CILK_TI_FRAME_EXCEPTION] = fields;

  s_type_node = build_int_cst (size_type_node, 4);
  fields =
    add_field ("ctx", 
	       build_array_type (ptr_type_node,
				 build_index_type (s_type_node)), fields);
  cilk_trees[CILK_TI_FRAME_CONTEXT] = fields;
  /* ??? */
  TYPE_ALIGN (frame) = PREFERRED_STACK_BOUNDARY;
  TREE_ADDRESSABLE (frame) = 1;
  /* XXX Is this right? */
  finish_builtin_struct (frame, "__cilkrts_stack_frame", fields, NULL_TREE);
  /* XXX C++ only SET_CLASS_TYPE_P(record, 1); */
  /* XXX C++ only xref_basetypes (record, NULL_TREE); */
  cilk_frame_type_decl = frame;
  lang_hooks.types.register_builtin_type (frame, "__cilkrts_frame_t");

  cilk_frame_ptr_type_decl = build_qualified_type (fptr_type,
						   TYPE_QUAL_RESTRICT);

  /* object could be named __cilk_frame_var for compatibility */

  fptr_v_type = build_qualified_type (fptr_type, TYPE_QUAL_VOLATILE);
  fptr_v_ptr_type = build_pointer_type (fptr_v_type);
  fptr_v_ptr_v_type = build_qualified_type (fptr_v_ptr_type,
					    TYPE_QUAL_VOLATILE);
  /* frame * volatile * volatile, the type of the worker fields */

  cilk_trees[CILK_TI_F_LOOP_32] =
    cilk_declare_looper ("__cilkrts_cilk_for_32", unsigned_intSI_type_node);
  cilk_trees[CILK_TI_F_LOOP_64] =
    cilk_declare_looper ("__cilkrts_cilk_for_64", unsigned_intDI_type_node);

  /* Now the worker.  Has bidirectional reference to frame. */
  fields = add_field ("tail", fptr_v_ptr_v_type, NULL_TREE);
  cilk_trees[CILK_TI_WORKER_TAIL] = fields;
  fields = add_field ("head", fptr_v_ptr_v_type, fields);
  cilk_trees[CILK_TI_WORKER_HEAD] = fields;
  fields = add_field ("exc", fptr_v_ptr_v_type, fields);
  cilk_trees[CILK_TI_WORKER_EXC] = fields;
  fields = add_field ("protected_tail", fptr_v_ptr_v_type, fields);
  cilk_trees[CILK_TI_WORKER_PTAIL] = fields;
  /* Note that ltq_limit is not volatile. */
  fields = add_field ("ltq_limit", fptr_v_ptr_type, fields);
  cilk_trees[CILK_TI_WORKER_LTQ_LIMIT] = fields;
  fields = add_field ("self", unsigned_type_node, fields);
  cilk_trees[CILK_TI_WORKER_SELF] = fields;
  {
    tree g = lang_hooks.types.make_type (RECORD_TYPE);
    finish_builtin_struct (g, "__cilkrts_global_state", NULL_TREE, NULL_TREE);
    fields = add_field ("g", build_pointer_type (g), fields);
  }
  {
    tree l = lang_hooks.types.make_type (RECORD_TYPE);
    finish_builtin_struct (l, "__cilkrts_local_state", NULL_TREE, NULL_TREE);
    fields = add_field ("l", build_pointer_type (l), fields);
  }
  fields = add_field ("reducer_map", ptr_type_node, fields);
  fields = add_field ("current_stack_frame", fptr_type, fields);
  cilk_trees[CILK_TI_WORKER_CUR] = fields;
  fields = add_field ("saved_protected_tail", fptr_v_ptr_type, fields);
  {
    tree sysdep_t = lang_hooks.types.make_type (RECORD_TYPE);
    finish_builtin_struct (sysdep_t, "__cilkrts_worker_sysdep_state",
			   NULL_TREE, NULL_TREE);
    fields = add_field ("sysdep", build_pointer_type (sysdep_t), fields);
  }

  DECL_ALIGN (fields) = BIGGEST_ALIGNMENT;

  finish_builtin_struct (worker, "__cilkrts_worker_t", fields, NULL_TREE);

  {
    tree fn;

    fn = build_fn_decl ("__cilkrts_current_worker_id",
			build_function_type (integer_type_node,
					     void_list_node));
    lang_hooks.decls.pushdecl (fn);
    /* XXX set attributes */

    fn = build_fn_decl ("__cilkrts_current_worker",
			build_function_type (wptr_type, void_list_node));
    lang_hooks.decls.pushdecl (fn);
  }

  cilk_detach_fndecl
    = install_builtin ("__cilkrts_detach", fptr_fun, BUILT_IN_CILK_DETACH, 1);

  /* The detach function does not detach in the sense meant by
     DECL_DETACHES_P.  Control flow past a detach is smooth and
     worker-related values remain unchanged.  The only change is
     that the parent function might be stolen.
     DECL_DETACHES_P (cilk_detach_fndecl) = 1;*/


  cilk_enter_fndecl = build_fn_decl ("__cilkrts_enter_frame", fptr_fun);
  mark_cold (cilk_enter_fndecl);
  cilk_enter_fndecl = lang_hooks.decls.pushdecl (cilk_enter_fndecl);

  cilk_pop_fndecl = install_builtin ("__cilkrts_pop_frame", fptr_fun,
				     BUILT_IN_CILK_POP_FRAME, false);
 
  cilk_leave_fndecl = build_fn_decl ("__cilkrts_leave_frame", fptr_fun);
  mark_cold (cilk_leave_fndecl);
  cilk_leave_fndecl = lang_hooks.decls.pushdecl (cilk_leave_fndecl);

  cilk_enter_fndecl = build_fn_decl ("__cilkrts_enter_frame", fptr_fun);
  mark_cold (cilk_enter_fndecl);
  cilk_enter_fndecl = lang_hooks.decls.pushdecl (cilk_enter_fndecl);



  cilk_pop_fndecl = install_builtin ("__cilkrts_pop_frame", fptr_fun,
				     BUILT_IN_CILK_POP_FRAME, false);

  cilk_leave_fndecl = build_fn_decl ("__cilkrts_leave_frame", fptr_fun);
  mark_cold (cilk_leave_fndecl);
  cilk_leave_fndecl = lang_hooks.decls.pushdecl (cilk_leave_fndecl);


  cilk_sync_fndecl = build_fn_decl ("__cilkrts_sync", fptr_fun);
  mark_cold (cilk_leave_fndecl);
  /* Unlike ordinary library functions cilk_sync can throw. */

  /* extern void __cilkrts_sync(void) */
  cilk_sync_fndecl = build_fn_decl ("__cilkrts_sync", fptr_fun);
  mark_cold (cilk_leave_fndecl);
  /* Unlike ordinary library functions cilk_sync can throw.
     Exceptions from spawns earlier in the same spawn scope
     may be deferred until a sync. */
  TREE_NOTHROW (cilk_sync_fndecl) = 0;
  
  /* A call to __cilkrts_sync is a knot, but not a detach. */
  DECL_SET_KNOT (cilk_sync_fndecl, 2);
  cilk_sync_fndecl = lang_hooks.decls.pushdecl (cilk_sync_fndecl);

  cilk_stolen_fndecl = install_builtin ("__cilkrts_was_stolen", frame_pred,
					BUILT_IN_CILK_STOLEN, 1);

  {
    tree x = tree_cons (NULL_TREE, ptr_type_node, void_list_node);
    x = tree_cons (NULL_TREE, fptr_type, x);
    x = build_function_type (void_type_node, x);
    cilk_free_dynamic_fndecl = build_fn_decl ("__cilkrts_free_dynamic", x);
  }

  {
    tree x = tree_cons (NULL_TREE, wptr_type, void_list_node);
    x = build_function_type (void_type_node, x);
    cilk_overflow_fndecl = build_fn_decl ("__cilkrts_ltq_overflow", x);
    TREE_THIS_VOLATILE (cilk_overflow_fndecl) = 1;
  }

  {
    tree tail = cilk_trees[CILK_TI_WORKER_TAIL];
    tree off1 = DECL_FIELD_OFFSET (tail); /* bytes */
    tree off2 = DECL_FIELD_BIT_OFFSET (tail); /* bits */
    worker_tail_offset = tree_low_cst (off1, 0) +
      tree_low_cst (off2, 0) / BITS_PER_UNIT;
  }
}

/* this function will call the value in a structure. eg. x.y */
static tree
dot (tree frame, int field_number, bool volatil)
{
  tree field = cilk_trees[field_number];
  field = build3 (COMPONENT_REF, TREE_TYPE (field), frame, field, NULL_TREE);
  if (volatil)
    TREE_THIS_VOLATILE (field) = 1;
  return field;
}

/* this function will call the address in a structure. e.g. (&x)->y */
static tree
arrow (tree fptr, int field_number, bool volatil)
{
  return dot (build1 (INDIRECT_REF, TREE_TYPE (TREE_TYPE (fptr)), fptr),
	      field_number, volatil);
}

tree
cilk_call_setjmp (tree frame)
{
  tree c;

  c = dot(frame, CILK_TI_FRAME_CONTEXT, false);
  c = build1 (ADDR_EXPR, build_pointer_type (ptr_type_node), c);
  return build_call_expr (implicit_built_in_decls[BUILT_IN_SETJMP], 1, c);
}

static rtx
cilk_test_flag (tree fptr, enum tree_code code, int bit)
{
  tree field = arrow (fptr, CILK_TI_FRAME_FLAGS, 0);

  field = build2 (BIT_AND_EXPR, TREE_TYPE (field), field,
		  build_int_cst (TREE_TYPE (field), bit));

  field = build2 (code, integer_type_node, field, integer_zero_node);

  return expand_expr (field, NULL_RTX, VOIDmode, EXPAND_NORMAL);
}

static tree get_frame_arg (tree call)
{
  tree arg, argtype;

  if (call_expr_nargs (call) < 1)
    return NULL_TREE;

  arg = CALL_EXPR_ARG (call, 0);
  argtype = TREE_TYPE (arg);
  if (TREE_CODE (argtype) != POINTER_TYPE)
    return NULL_TREE;

  argtype = TREE_TYPE (argtype);
  if (!lang_hooks.types_compatible_p (argtype, cilk_frame_type_decl))
    return NULL_TREE;

  return arg;
}

rtx
expand_builtin_cilk_detach (tree exp)
{

  tree worker, parent, tail, flags;
  rtx wreg, preg, treg, tmem0, tmem1;
  enum insn_code icode;
  rtx insn;
  tree fptr = get_frame_arg (exp);

  if (fptr == NULL_TREE)
    return NULL_RTX;

  worker = arrow (fptr, CILK_TI_FRAME_WORKER, 0);
  tail = arrow (worker, CILK_TI_WORKER_TAIL, 1);
  parent = arrow (fptr, CILK_TI_FRAME_PARENT, 0);

  wreg = expand_expr (worker, NULL_RTX, Pmode, EXPAND_NORMAL);
  if (GET_CODE (wreg) != REG)
    wreg = copy_to_reg (wreg);
  preg = expand_expr (parent, NULL_RTX, Pmode, EXPAND_NORMAL);

  /* TMP <- WORKER.TAIL
   *TMP <- PARENT
   TMP <- TMP + 1
   WORKER.TAIL <- TMP */

  tmem0 = gen_rtx_MEM (Pmode, plus_constant (wreg, worker_tail_offset));
  set_mem_attributes (tmem0, tail, 0);
  MEM_NOTRAP_P (tmem0) = 1;
  gcc_assert (MEM_VOLATILE_P (tmem0));
  treg = copy_to_mode_reg (Pmode, tmem0);
  tmem1 = gen_rtx_MEM (Pmode, treg);
  set_mem_attributes (tmem1, TREE_TYPE (TREE_TYPE (tail)), 0);
  MEM_NOTRAP_P (tmem1) = 1;
  emit_move_insn (tmem1, preg);
  emit_move_insn (treg, plus_constant (treg, GET_MODE_SIZE (Pmode)));

  /* There is a release barrier (st8.rel, membar #StoreStore,
     sfence, lwsync, etc.) between the two stores.  On x86
     normal volatile stores have proper semantics; the sfence
     would only be needed for nontemporal stores (which we
     could generate using the storent optab, for no benefit
     in this case).

     The predicate may return false even for a REG if this is
     the limited release operation that only stores 0. */
  /* icode = sync_lock_release[Pmode];  */
  icode =direct_optab_handler(sync_lock_release_optab,Pmode); 
  if (icode != CODE_FOR_nothing
      && insn_data[icode].operand[1].predicate (treg, Pmode)
      && (insn = GEN_FCN (icode) (tmem0, treg)) != NULL_RTX)
    emit_insn (insn);
  else
    emit_move_insn (tmem0, treg);

  /* sf->flags |= CILK_FRAME_DETACHED

     The memory barrier inserted above should not prevent
     the load of flags from being moved before the stores,
     but in practice it does because it is implemented with
     unspec_volatile.  In-order RISC machines should
     explicitly load flags earlier. */

  flags = arrow (fptr, CILK_TI_FRAME_FLAGS, 0);
  expand_expr (build2 (MODIFY_EXPR, void_type_node, flags,
		       build2 (BIT_IOR_EXPR, TREE_TYPE (flags), flags,
			       build_int_cst (TREE_TYPE (flags),
					      CILK_FRAME_DETACHED))),
	       const0_rtx, VOIDmode, EXPAND_NORMAL);

  return const0_rtx;
}

rtx
expand_builtin_cilk_synched (tree exp)
{
  tree frame = get_frame_arg (exp);

  if (frame == NULL_TREE)
    return NULL_RTX;

  return cilk_test_flag (frame, EQ_EXPR, CILK_FRAME_UNSYNCHED);
}

rtx
expand_builtin_cilk_stolen (tree ARG_UNUSED(exp))
{
  tree frame = get_frame_arg (exp);

  if (frame == NULL_TREE)
    return NULL_RTX;

  return cilk_test_flag (frame, NE_EXPR, CILK_FRAME_STOLEN);
}

/* Not clear if this function has any value.  A function call is always
   needed and the function call might as well do all the work.  The only
   way to avoid a function call is via fast TLS access, which doesn't work
   with shared libraries on Linux. */
rtx
expand_builtin_cilk_enter (tree exp)
{
  if (false)
    {
      tree x, fptr;
      rtx mem;

      fptr = get_frame_arg (exp);

      /* w = get worker
	 if (!w)
	 slow path
	 else
	 flags <- 0
	 parent <- w->current
	 w->current <- self
      */
      x = arrow (fptr, CILK_TI_FRAME_FLAGS, false);
      mem = expand_expr (x, NULL_RTX,
			 TYPE_MODE (TREE_TYPE (cilk_trees[CILK_TI_FRAME_FLAGS])),
			 EXPAND_WRITE);
      emit_move_insn (mem,
		      GEN_INT (cfun->always_detaches ? CILK_FRAME_DETACHED : 0));
      return const0_rtx;
    }
  return NULL_RTX;
}

/* this function will explain the __pop frame function call . This function
 * expands to the following:
 * w = sf->worker;
 * w->current = sf->parent;
 * sf->parent = 0
 */
rtx
expand_builtin_cilk_pop_frame (tree exp)
{
  tree fptr = get_frame_arg (exp);
  tree parent, worker, current, assign;
  rtx x;

  if (fptr == NULL_TREE)
    return NULL_RTX;

  worker = arrow (fptr, CILK_TI_FRAME_WORKER, 0);
  current = arrow (worker, CILK_TI_WORKER_CUR, 0);
  parent = arrow (fptr, CILK_TI_FRAME_PARENT, 0);
  assign = build2 (MODIFY_EXPR, void_type_node, current, parent);
  x = expand_expr (assign, const0_rtx, VOIDmode, EXPAND_NORMAL);
  gcc_assert (GET_CODE (x) != REG);

  return const0_rtx;
}

tree
build_cilk_function_exit (tree frame, bool detaches, bool needs_sync)
{
  tree clear_parent = NULL_TREE;
  tree epi, call, addr;
  tree worker = NULL_TREE;
  tree parent = NULL_TREE;
  tree current = NULL_TREE;
  tree func_ptr = NULL_TREE;
  tree sync_expr = NULL_TREE;

  /* tree debug_stmt=NULL_TREE; */
  
  addr = build1 (ADDR_EXPR, cilk_frame_ptr_type_decl, frame);
  
  /* debug_stmt = build_call_expr (cilk_debug_fndecl,1,addr); */
  
  epi = alloc_stmt_list ();

  /* append_to_statement_list(debug_stmt, &epi); */
  if (needs_sync == true)
    {
      sync_expr = build_cilk_sync();
      /* For now always sync.  The optimizer can delete it later. */
      append_to_statement_list (sync_expr, &epi);

    }
  
  func_ptr = (addr);
  worker = arrow(func_ptr,CILK_TI_FRAME_WORKER,0);
  current = arrow(worker,CILK_TI_WORKER_CUR,0);
  parent = arrow(func_ptr,CILK_TI_FRAME_PARENT,0);

  /* this should replace the pop_fndecl */
  call = build2(MODIFY_EXPR,void_type_node, current, parent);
   
  
  append_to_statement_list (call, &epi);
  clear_parent = build2(MODIFY_EXPR,void_type_node, parent,
			build_int_cst (TREE_TYPE (parent), 0));
  append_to_statement_list(clear_parent,&epi);
  /* append_to_statement_list (call, &epi); */
  call = build_call_expr (cilk_leave_fndecl, 1, addr);
  if (detaches==false)
    {
      tree flags_cmp_expr = NULL_TREE;
      tree flags = dot (frame, CILK_TI_FRAME_FLAGS, false);
      flags_cmp_expr = fold_build2 (NE_EXPR, TREE_TYPE(flags), flags,
				    build_int_cst (TREE_TYPE (flags), 0));
      call = fold_build3 (COND_EXPR, void_type_node, flags_cmp_expr,
			  call, build_empty_stmt (EXPR_LOCATION(flags)));
    }
  append_to_statement_list (call, &epi);
  return epi;
}

static int cilk_frame_name_count;

tree
make_cilk_frame (tree fn)
{
  struct function *f = DECL_STRUCT_FUNCTION (fn);
  tree decl;
  tree t;
  char frame_name[80];
  if (f->cilk_frame_decl)
    return f->cilk_frame_decl;


  sprintf(frame_name, "sf_%02d",++cilk_frame_name_count);
  
  t = get_identifier(frame_name);
  decl = build_decl (EXPR_LOCATION(t),VAR_DECL, t, cilk_frame_type_decl);
  DECL_CONTEXT (decl) = fn;
  /* Magic to prevent assert failure.  Easier than doing it right. */
  DECL_SEEN_IN_BIND_EXPR_P (decl) = 1;

  f->cilk_frame_decl = decl;
  return decl;
}


/*
 * This function will expand a cilk_sync call.
 * cilk_sync becomes
 * if (frame.flags & 2)
 *         __cilkrts_sync(&frame);
 * else
 *         <NOTHING> ;
*/

tree
build_cilk_sync (void)
{
  tree frame = cfun->cilk_frame_decl;
  tree flags;
  tree unsynched;
  tree sync;
  tree sync_expr;
  tree setjmp_expr;
  /* tree debug_expr; */
  flags = dot (frame, CILK_TI_FRAME_FLAGS, false);
  
  unsynched = fold_build2 (BIT_AND_EXPR, TREE_TYPE (flags),
			   flags,
			   build_int_cst (TREE_TYPE (flags),
					  CILK_FRAME_UNSYNCHED));


  unsynched = fold_build2 (NE_EXPR, TREE_TYPE(unsynched), unsynched,
			   build_int_cst (TREE_TYPE(unsynched), 0));
  
  sync_expr = build_call_expr (cilk_sync_fndecl, 1,
			       build1 (ADDR_EXPR,
				       cilk_frame_ptr_type_decl,
				       frame));
  setjmp_expr = cilk_call_setjmp(frame);
  setjmp_expr = fold_build2 (EQ_EXPR, TREE_TYPE(setjmp_expr), setjmp_expr,
			     build_int_cst(TREE_TYPE(setjmp_expr),0));
  
  setjmp_expr = fold_build3 (COND_EXPR, void_type_node,
			     setjmp_expr,
			     sync_expr, 
			     build_empty_stmt (EXPR_LOCATION(unsynched)));

  
  sync = fold_build3 (COND_EXPR, void_type_node, unsynched, setjmp_expr,
		      build_empty_stmt (EXPR_LOCATION(unsynched)));
  
  return sync;
}

/* this function will gimplify the cilk_sync expression */
void
gimplify_cilk_sync (tree *expr_p, gimple_seq *pre_p)
{
  
  tree sync_expr = build_cilk_sync();

  *expr_p = NULL_TREE;


  gimplify_and_add (sync_expr, pre_p);
}

