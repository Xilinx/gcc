/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains the CilkPlus Intrinsics
   Copyright (C) 2011, 2012  Free Software Foundation, Inc.
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
#include "output.h"
#include "dwarf2out.h"

tree cilk_trees[(int) CILK_TI_MAX];

static HOST_WIDE_INT worker_tail_offset;

zca_data *zca_head;

/* This function will add a function of type "name" to a chain of functions.  */

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

/* This function will add a field to a defined builtin structure.  */

static tree
add_field (const char *name, tree type, tree fields)
{
  tree  t = get_identifier (name);
  tree field = build_decl (BUILTINS_LOCATION, FIELD_DECL, t, type);
  TREE_CHAIN (field) = fields;
  return field;
}

/* This function will define a builtin function.  */

static tree
install_builtin (const char *name, tree fntype, enum built_in_function code,
		 bool publish)
{
  tree fndecl = build_fn_decl (name, fntype);
  DECL_BUILT_IN_CLASS (fndecl) = BUILT_IN_NORMAL;
  DECL_FUNCTION_CODE (fndecl) = code;
  if (publish)
    { 
      tree t = lang_hooks.decls.pushdecl (fndecl);
      if (t)
	fndecl = t;
    }
  set_builtin_decl (code, fndecl, true);
  return fndecl;
}

/* This function will add the cold attribute to a builtin function.  */

static void
mark_cold (tree fndecl)
{
  DECL_ATTRIBUTES (fndecl) = tree_cons (get_identifier ("cold"), NULL_TREE,
					DECL_ATTRIBUTES (fndecl));
}

/* This function wil initialize/create all the builtin cilk plus functions.  */

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
  tree metacall_frame, metacall_fptr_type, metacall_fields, metacall_fptr_fun;
  tree metacall_fptr_arglist;
  tree int_list = tree_cons (NULL_TREE, integer_type_node, NULL_TREE);
  tree int_fun = build_function_type (void_type_node, int_list);
  tree ptr_list = tree_cons (NULL_TREE, ptr_type_node, void_list_node);
  tree ptr_fun = build_function_type (void_type_node, ptr_list);
  tree void_fun = build_function_type (void_type_node, void_list_node);
  tree notify_intrinsic_arg = NULL_TREE;
  
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
  
  finish_builtin_struct (frame, "__cilkrts_stack_frame_GCC", fields, NULL_TREE);
  cilk_frame_type_decl = frame;
  lang_hooks.types.register_builtin_type (frame, "__cilkrts_frame_t");

  cilk_frame_ptr_type_decl = build_qualified_type (fptr_type,
						   TYPE_QUAL_RESTRICT);
  /*
    typedef struct __metacall_data_t {
        unsigned int tool;
        unsigned int code;
	void *data;
    } metacall_data_t;
  */
  metacall_frame = lang_hooks.types.make_type (RECORD_TYPE);
  metacall_fptr_type = build_pointer_type (metacall_frame);
  metacall_fptr_arglist =
    tree_cons (NULL_TREE, metacall_fptr_type, void_list_node);
  metacall_fptr_fun = build_function_type (void_type_node,
					   metacall_fptr_arglist);
  metacall_fields = NULL_TREE;
  metacall_fields = add_field ("tool", unsigned_type_node, metacall_fields);
  cilk_trees[CILK_TI_METACALL_TOOL_FLAGS] = metacall_fields;
  metacall_fields = add_field ("code", unsigned_type_node, metacall_fields);
  cilk_trees[CILK_TI_METACALL_CODE_FLAGS] = metacall_fields;
  metacall_fields = add_field ("data", fptr_type, metacall_fields);
  cilk_trees[CILK_TI_METACALL_DATA_FLAGS] = metacall_fields;
  TYPE_ALIGN (metacall_frame) = PREFERRED_STACK_BOUNDARY;
  TREE_ADDRESSABLE (metacall_frame) = 1;
  finish_builtin_struct (metacall_frame, "__metacall_data_s", metacall_fields,
			 NULL_TREE);
  cilk_metacall_frame_type_decl = metacall_frame;
  lang_hooks.types.register_builtin_type (frame, "__metacall_data_t");
  cilk_mcall_frame_ptr_type_decl = build_qualified_type (metacall_fptr_type,
							 TYPE_QUAL_RESTRICT);
  notify_intrinsic_arg = tree_cons (NULL_TREE, ptr_type_node,
				    notify_intrinsic_arg);
  notify_intrinsic_arg = tree_cons
    (NULL_TREE, build_pointer_type (char_type_node), notify_intrinsic_arg);
				    
  fptr_v_type = build_qualified_type (fptr_type, TYPE_QUAL_VOLATILE);
  fptr_v_ptr_type = build_pointer_type (fptr_v_type);
  fptr_v_ptr_v_type = build_qualified_type (fptr_v_ptr_type,
					    TYPE_QUAL_VOLATILE);
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

  tree g = lang_hooks.types.make_type (RECORD_TYPE); 
  finish_builtin_struct (g, "__cilkrts_global_state", NULL_TREE, NULL_TREE); 
  fields = add_field ("g", build_pointer_type (g), fields); 
  tree l = lang_hooks.types.make_type (RECORD_TYPE); 
  finish_builtin_struct (l, "__cilkrts_local_state", NULL_TREE, NULL_TREE); 
  fields = add_field ("l", build_pointer_type (l), fields);
  fields = add_field ("reducer_map", ptr_type_node, fields);
  fields = add_field ("current_stack_frame", fptr_type, fields);
  cilk_trees[CILK_TI_WORKER_CUR] = fields;
  fields = add_field ("saved_protected_tail", fptr_v_ptr_type, fields);
  tree sysdep_t = lang_hooks.types.make_type (RECORD_TYPE); 
  finish_builtin_struct (sysdep_t, "__cilkrts_worker_sysdep_state", NULL_TREE, 
			 NULL_TREE); 
  fields = add_field ("sysdep", build_pointer_type (sysdep_t), fields);
  DECL_ALIGN (fields) = BIGGEST_ALIGNMENT;

  finish_builtin_struct (worker, "__cilkrts_worker_t", fields, NULL_TREE);

  cilk_detach_fndecl = install_builtin ("__cilkrts_detach", fptr_fun, 
					BUILT_IN_CILK_DETACH, 1);

  /* The detach function does not detach in the sense meant by
     DECL_DETACHES_P.  Control flow past a detach is smooth and
     worker-related values remain unchanged.  The only change is
     that the parent function might be stolen.
     DECL_DETACHES_P (cilk_detach_fndecl) = 1;*/


  cilk_enter_fndecl = install_builtin ("__cilkrts_enter_frame", fptr_fun,
				       BUILT_IN_CILK_ENTER_FRAME, true);

  cilk_pop_fndecl = install_builtin ("__cilkrts_pop_frame", fptr_fun,
				     BUILT_IN_CILK_POP_FRAME, false);
 
  cilk_leave_fndecl = build_fn_decl ("__cilkrts_leave_frame", fptr_fun);
  mark_cold (cilk_leave_fndecl);
  cilk_leave_fndecl = lang_hooks.decls.pushdecl (cilk_leave_fndecl);

  cilk_sync_fndecl = build_fn_decl ("__cilkrts_sync", fptr_fun);
  mark_cold (cilk_sync_fndecl);
  /* Unlike ordinary library functions cilk_sync can throw.
     Exceptions from spawns earlier in the same spawn scope
     may be deferred until a sync. */
  TREE_NOTHROW (cilk_sync_fndecl) = 0;
  
  /* A call to __cilkrts_sync is a knot, but not a detach.  */
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

  cilk_enter_begin_fndecl = install_builtin ("cilk_enter_begin", fptr_fun,
					     BUILT_IN_CILK_ENTER_BEGIN, true);
  cilk_enter_h_begin_fndecl = install_builtin
    ("cilk_enter_helper_begin", fptr_fun, BUILT_IN_CILK_ENTER_H_BEGIN, true);
  cilk_enter_end_fndecl = install_builtin ("cilk_enter_end", fptr_fun,
					   BUILT_IN_CILK_ENTER_END, true);
  cilk_spawn_prepare_fndecl = install_builtin
    ("cilk_spawn_prepare", fptr_fun, BUILT_IN_CILK_SPAWN_PREPARE, true);
  cilk_spawn_or_cont_fndecl = install_builtin
    ("cilk_spawn_or_continue", int_fun, BUILT_IN_SPAWN_OR_CONT, true);
  cilk_detach_begin_fndecl = install_builtin ("cilk_detach_begin", fptr_fun,
					      BUILT_IN_CILK_DETACH_BEGIN, true);
  cilk_detach_end_fndecl = install_builtin ("cilk_detach_end", void_fun,
					    BUILT_IN_CILK_DETACH_END, true);
  cilk_sync_begin_fndecl = install_builtin ("cilk_sync_begin", fptr_fun,
					  BUILT_IN_CILK_SYNC_BEGIN, true);
  cilk_sync_end_fndecl = install_builtin ("cilk_sync_end", fptr_fun,
					  BUILT_IN_CILK_SYNC_END, true);
  cilk_leave_begin_fndecl = install_builtin ("cilk_leave_begin", fptr_fun,
					     BUILT_IN_CILK_LEAVE_BEGIN, true);
  cilk_leave_end_fndecl = install_builtin ("cilk_leave_end", void_fun,
					   BUILT_IN_CILK_LEAVE_END, true);
  cilkscreen_metacall_fndecl = install_builtin
    ("cilkscreen_metacall", metacall_fptr_fun, BUILT_IN_CILKSCREEN_METACALL,
     true);
  cilk_resume_fndecl = install_builtin
    ("cilk_resume", fptr_fun, BUILT_IN_CILK_RESUME, true);
  cilk_leave_stolen_fndecl = install_builtin
    ("cilk_leave_stolen", void_fun, BUILT_IN_LEAVE_STOLEN, true);
  cilk_sync_abandon_fndecl = install_builtin ("cilk_sync_abandon", void_fun,
					      BUILT_IN_SYNC_ABANDON, true);
  cilkscreen_disable_instr_fndecl = install_builtin
    ("cilkscreen_disable_instrumentation", void_fun,
     BUILT_IN_CILKSCREEN_DS_INSTR, true);
  cilkscreen_enable_instr_fndecl = install_builtin
    ("cilkscreen_enable_instrumentation", void_fun,
     BUILT_IN_CILKSCREEN_EN_INSTR, true);
  cilkscreen_disable_check_fndecl = install_builtin
    ("cilkscreen_disable_checking", void_fun, BUILT_IN_CILKSCREEN_DS_CHK, true);
  cilkscreen_enable_check_fndecl = install_builtin
    ("cilkscreen_enable_checking", void_fun, BUILT_IN_CILKSCREEN_EN_CHK, true);
  cilkscreen_aquire_lock_fndecl = install_builtin
    ("cilkscreen_aquire_lock", ptr_fun, BUILT_IN_CILKSCREEN_AQUIRE_LOCK, true);
  cilkscreen_release_lock_fndecl = install_builtin
    ("cilkscreen_release_lock", ptr_fun, BUILT_IN_CILKSCREEN_REL_LOCK, true);
  notify_intrinsic_fndecl = install_builtin
    ("__notify_intrinsic",
     build_function_type (void_type_node, notify_intrinsic_arg),
     BUILT_IN_NOTIFY_INTRINSIC, true);
  notify_zc_intrinsic_fndecl = install_builtin
    ("__notify_zc_intrinsic", 
     build_function_type (void_type_node, notify_intrinsic_arg),
			 BUILT_IN_NOTIFY_ZC_INTRINSIC, true);
			 

  zca_head = NULL;
}

/* This function will call the value in a structure. eg. x.y  */

static tree
dot (tree frame, int field_number, bool volatil)
{
  tree field = cilk_trees[field_number];
  field = build3 (COMPONENT_REF, TREE_TYPE (field), frame, field, NULL_TREE);
  if (volatil)
    TREE_THIS_VOLATILE (field) = 1;
  return field;
}

/* This function will call the address in a structure. e.g. (&x)->y  */

static tree
arrow (tree fptr, int field_number, bool volatil)
{
  return dot (build1 (INDIRECT_REF, TREE_TYPE (TREE_TYPE (fptr)), fptr),
	      field_number, volatil);
}

/* This function will call the correct setjmp for a field in struct.  */

tree
cilk_call_setjmp (tree frame)
{
  tree c;

  c = dot (frame, CILK_TI_FRAME_CONTEXT, false);
  c = build1 (ADDR_EXPR, build_pointer_type (ptr_type_node), c);
  return build_call_expr (builtin_decl_implicit (BUILT_IN_SETJMP), 1, c);
}

/* This function will create a conditional expression that will test a bit.  */

static rtx
cilk_test_flag (tree fptr, enum tree_code code, int bit)
{
  tree field = arrow (fptr, CILK_TI_FRAME_FLAGS, 0);

  field = build2 (BIT_AND_EXPR, TREE_TYPE (field), field,
		  build_int_cst (TREE_TYPE (field), bit));

  field = build2 (code, integer_type_node, field, integer_zero_node);

  return expand_expr (field, NULL_RTX, VOIDmode, EXPAND_NORMAL);
}

/* Get the appropriate frame arguments for a function call.  */

static tree
get_frame_arg (tree call)
{
  tree arg, argtype;

  if (call_expr_nargs (call) < 1)
    return NULL_TREE;

  arg = CALL_EXPR_ARG (call, 0);
  argtype = TREE_TYPE (arg);
  if (TREE_CODE (argtype) != POINTER_TYPE)
    return NULL_TREE;

  argtype = TREE_TYPE (argtype);
  
  if (lang_hooks.types_compatible_p &&
      !lang_hooks.types_compatible_p (argtype, cilk_frame_type_decl))
    return NULL_TREE;

  return arg;
}

/* Expands the cilk_detach function call.  */

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

  tmem0 = gen_rtx_MEM (Pmode, plus_constant (Pmode, wreg, worker_tail_offset));
  set_mem_attributes (tmem0, tail, 0);
  MEM_NOTRAP_P (tmem0) = 1;
  gcc_assert (MEM_VOLATILE_P (tmem0));
  treg = copy_to_mode_reg (Pmode, tmem0);
  tmem1 = gen_rtx_MEM (Pmode, treg);
  set_mem_attributes (tmem1, TREE_TYPE (TREE_TYPE (tail)), 0);
  MEM_NOTRAP_P (tmem1) = 1;
  emit_move_insn (tmem1, preg);
  emit_move_insn (treg, plus_constant (Pmode, treg, GET_MODE_SIZE (Pmode)));

  /* There is a release barrier (st8.rel, membar #StoreStore,
     sfence, lwsync, etc.) between the two stores.  On x86
     normal volatile stores have proper semantics; the sfence
     would only be needed for nontemporal stores (which we
     could generate using the storent optab, for no benefit
     in this case).

     The predicate may return false even for a REG if this is
     the limited release operation that only stores 0. */
  icode = direct_optab_handler (sync_lock_release_optab, Pmode); 
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

/* Expand the cilk_synched function call.  */

rtx
expand_builtin_cilk_synched (tree exp)
{
  tree frame = get_frame_arg (exp);

  if (!frame)
    return NULL_RTX;

  return cilk_test_flag (frame, EQ_EXPR, CILK_FRAME_UNSYNCHED);
}

/* Expand the cilk_stolen function call.  */

rtx
expand_builtin_cilk_stolen (tree ARG_UNUSED (exp))
{
  tree frame = get_frame_arg (exp);

  if (!frame)
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
      mem = expand_expr
	(x, NULL_RTX, TYPE_MODE (TREE_TYPE (cilk_trees[CILK_TI_FRAME_FLAGS])),
			 EXPAND_WRITE);
      emit_move_insn
	(mem, GEN_INT (cfun->always_detaches ? CILK_FRAME_DETACHED : 0));
      return const0_rtx;
    }
  return NULL_RTX;
}

/* this function will explain the __pop frame function call.  */

rtx
expand_builtin_cilk_pop_frame (tree exp)
{
  tree fptr = get_frame_arg (exp);
  tree parent, worker, current, assign;
  rtx x;

 /* This function expands to the following:
      w = sf->worker;
      w->current = sf->parent;
      sf->parent = 0
 */
  if (!fptr)
    return NULL_RTX;

  worker = arrow (fptr, CILK_TI_FRAME_WORKER, 0);
  current = arrow (worker, CILK_TI_WORKER_CUR, 0);
  parent = arrow (fptr, CILK_TI_FRAME_PARENT, 0);
  assign = build2 (MODIFY_EXPR, void_type_node, current, parent);
  x = expand_expr (assign, const0_rtx, VOIDmode, EXPAND_NORMAL);
  gcc_assert (GET_CODE (x) != REG);

  return const0_rtx;
}

/* This function will output the exit conditions for a spawn call.  */

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
  tree leave_begin, leave_end;

  addr = build1 (ADDR_EXPR, cilk_frame_ptr_type_decl, frame);
  
  epi = alloc_stmt_list ();

  if (needs_sync == true)
    {
      sync_expr = build_cilk_sync ();
      /* For now always sync.  The optimizer can delete it later.  */
      append_to_statement_list (sync_expr, &epi);
    }
  
  func_ptr = (addr);
  worker = arrow (func_ptr, CILK_TI_FRAME_WORKER, 0);
  current = arrow (worker, CILK_TI_WORKER_CUR, 0);
  parent = arrow (func_ptr, CILK_TI_FRAME_PARENT, 0);

  /* This should replace the pop_fndecl.  */
  call = build2 (MODIFY_EXPR, void_type_node, current, parent);
   
  
  append_to_statement_list (call, &epi);
  clear_parent = build2 (MODIFY_EXPR, void_type_node, parent,
			 build_int_cst (TREE_TYPE (parent), 0));
  append_to_statement_list (clear_parent, &epi);

  /* Added cilk_leave_begin.  */
  leave_begin = build_call_expr (cilk_leave_begin_fndecl, 1, addr);
  append_to_statement_list (leave_begin, &epi);
  
  call = build_call_expr (cilk_leave_fndecl, 1, addr);
  if (!detaches)
    {
      tree flags_cmp_expr = NULL_TREE;
      tree flags = dot (frame, CILK_TI_FRAME_FLAGS, false);
      flags_cmp_expr = fold_build2 (NE_EXPR, TREE_TYPE (flags), flags,
				    build_int_cst (TREE_TYPE (flags), 0));
      call = fold_build3 (COND_EXPR, void_type_node, flags_cmp_expr,
			  call, build_empty_stmt (EXPR_LOCATION (flags)));
    }
  append_to_statement_list (call, &epi);

  /* Added cilk_leave_end.  */
  leave_end = build_call_expr (cilk_leave_end_fndecl, 0);
  append_to_statement_list (leave_end, &epi);
  
  return epi;
}

static int cilk_frame_name_count;

/* Make the frames necessary for a spawn call.  */

tree
make_cilk_frame (tree fn)
{
  struct function *f = DECL_STRUCT_FUNCTION (fn);
  tree decl;
  tree t;
  char frame_name[80];
  if (f->cilk_frame_decl)
    return f->cilk_frame_decl;


  sprintf (frame_name, "sf_%02d",++cilk_frame_name_count);
  
  t = get_identifier (frame_name);
  decl = build_decl (EXPR_LOCATION (t), VAR_DECL, t, cilk_frame_type_decl);
  DECL_CONTEXT (decl) = fn;
  /* Magic to prevent assert failure.  Easier than doing it right. */
  DECL_SEEN_IN_BIND_EXPR_P (decl) = 1;

  f->cilk_frame_decl = decl;
  return decl;
}


/*
 * This function will expand a cilk_sync call.
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
  tree sync_list, frame_addr;
  tree sync_begin, sync_end;

  /* Cilk_sync becomes the following code:
     if (frame.flags & CILK_FRAME_UNSYNCHED)
      if (!builtin_setjmp (frame.ctx)
            // cilk_enter_begin();
          __cilkrts_sync(&frame);
            // cilk_enter_end();
       else
          <NOTHING> ;
    else
          <NOTHING> ;
  */
  flags = dot (frame, CILK_TI_FRAME_FLAGS, false);
  
  unsynched = fold_build2 (BIT_AND_EXPR, TREE_TYPE (flags), flags,
			   build_int_cst (TREE_TYPE (flags),
					  CILK_FRAME_UNSYNCHED));

  unsynched = fold_build2 (NE_EXPR, TREE_TYPE (unsynched), unsynched,
			   build_int_cst (TREE_TYPE (unsynched), 0));

  frame_addr = build1 (ADDR_EXPR, cilk_frame_ptr_type_decl, frame);
  sync_expr = build_call_expr (cilk_sync_fndecl, 1, frame_addr);
  setjmp_expr = cilk_call_setjmp (frame);
  setjmp_expr = fold_build2 (EQ_EXPR, TREE_TYPE (setjmp_expr), setjmp_expr,
			     build_int_cst (TREE_TYPE (setjmp_expr), 0));
  
  setjmp_expr = fold_build3 (COND_EXPR, void_type_node, setjmp_expr,
			     sync_expr,
			     build_empty_stmt (EXPR_LOCATION (unsynched)));
  
  sync = fold_build3 (COND_EXPR, void_type_node, unsynched, setjmp_expr,
		      build_empty_stmt (EXPR_LOCATION (unsynched)));
  
  sync_begin = build_call_expr (cilk_sync_begin_fndecl, 1, frame_addr);
  sync_end = build_call_expr (cilk_sync_end_fndecl, 1, frame_addr);
  sync_list = alloc_stmt_list ();
  append_to_statement_list_force (sync_begin, &sync_list);
  append_to_statement_list_force (sync, &sync_list);
  append_to_statement_list_force (sync_end, &sync_list);

  return sync_list;
}

/* This function will gimplify the cilk_sync expression.  */

void
gimplify_cilk_sync (tree *expr_p, gimple_seq *pre_p)
{
  tree sync_expr = build_cilk_sync ();
  *expr_p = NULL_TREE;
  gimplify_and_add (sync_expr, pre_p);
}

/* This function will create a label for the metadata section given by name.  */ 
static rtx
create_metadata_label (const char *name)
{
  rtx new_label = NULL_RTX;

  new_label = gen_label_rtx ();
  LABEL_PRESERVE_P (new_label) = 1;
  PUT_CODE (new_label, NOTE); 
  NOTE_KIND (new_label) = NOTE_INSN_DELETED_LABEL;
  NOTE_DELETED_LABEL_NAME (new_label) = name;
  INSN_UID (new_label) = crtl->emit.x_cur_insn_uid++;
  LABEL_NUSES (new_label) = 1;
  return new_label;
}

/* This function will insert zca_entry into a linked list.  */

static void
insert_into_zca_list (zca_data zca_entry)
{
  zca_data *ii_data;
  if (!zca_head)
    {
      zca_head = (zca_data *) xmalloc (sizeof (zca_data));
      gcc_assert (zca_head);
      *zca_head = zca_entry;
      zca_head->ptr_next = NULL;
    }
  else
    {
      for (ii_data = zca_head; ii_data->ptr_next; ii_data = ii_data->ptr_next)
	{
	  ;
	}
      ii_data->ptr_next = (zca_data *) xmalloc (sizeof (zca_data));
      gcc_assert (ii_data->ptr_next);
      *ii_data->ptr_next = zca_entry;
      ii_data->ptr_next->ptr_next = NULL;
    }
  return;
}

/* This function will return an entry number from the linked list.  */

static zca_data *
find_zca_data (int entry_no)
{
  int ii = 0;
  zca_data *ii_data = NULL;
  for (ii_data = zca_head; ii_data; ii_data = ii_data->ptr_next)
    {
      if (ii == entry_no)
	return ii_data;
      ii++;
    }
  return NULL;
}

/* This function returns the number of elements in the ZCA linked list.  */

static int
get_zca_entry_count (void)
{
  int length = 0;
  zca_data *ii_data;

  for (ii_data = zca_head; ii_data; ii_data = ii_data->ptr_next)
    length++;
  
  return length; 
}

/* This function computes the string table size of entire zca list.  */

static int
get_zca_string_table_size (void)
{
  int length = get_zca_entry_count ();
  int ii = 0;
  int str_length = 0;
  zca_data *zca_entry = NULL;

  for (ii = 0; ii < length; ii++)
    {
      zca_entry = find_zca_data (ii);
      str_length += strlen (zca_entry->string) + 1 ;
    }
  return str_length; 
}

/* This function returns the zca list's expression table size.  */

static int
get_zca_exprs_table_size (void)
{
  zca_data zca_entry;
  int length = get_zca_entry_count ();

  return (length * (int) sizeof (zca_entry.dwarf_expr));
}

/* This function will output the entire ZCA table in the appropriate format.  */

static void
output_zca_table (section *s)
{
  int ii = 0;
  zca_data *zca_entry = NULL;
  int length = get_zca_entry_count ();
  int str_table_offset = 0;
  int annotation_table_offset = 0;
  
  switch_to_section (s);

  for (ii = 0; ii < length; ii++)
    {
      zca_entry = find_zca_data (ii);

      /* this outputs the IP  */
      fputs (integer_asm_op (GET_MODE_SIZE (Pmode), 1), asm_out_file);
      output_asm_label (zca_entry->label);
      fputc ('\n', asm_out_file);

      /* This outputs the probspace, currently unused, thus is kept to zero.  */
      assemble_integer (gen_rtx_CONST_INT (BLKmode, 0), 4, 1, 1);

      /* This outputs the offset to the string table.  */
      assemble_integer (gen_rtx_CONST_INT (BLKmode, str_table_offset), 4, 1, 1);
      str_table_offset += strlen (zca_entry->string) + 1;

      /* This outputs the offset to the annotation table. */
      assemble_integer (gen_rtx_CONST_INT (BLKmode, annotation_table_offset),
			4, 1, 1);
      annotation_table_offset += (int) sizeof (zca_entry->dwarf_expr);
    }    
  return;
}

/* This function will output the whole string table.  */

static void
output_string_table (section *s)
{
  int length = get_zca_entry_count ();
  int ii = 0, jj = 0;
  zca_data *zca_entry;
  
  switch_to_section (s);;

  for (ii = 0; ii < length; ii++)
    {
      zca_entry = find_zca_data (ii);
      for (jj = 0; jj < (int) strlen (zca_entry->string); jj++)
	assemble_integer (gen_rtx_CONST_INT (BLKmode, zca_entry->string[jj]),
			  1, 1, 1);
      assemble_integer (gen_rtx_CONST_INT (BLKmode, 0), 1, 1, 1);
    }  
  return;
}

/* This function will output the expression table.  */

static void
output_expr_table (section *s)
{
  int ii = 0;
  int length = get_zca_entry_count ();
  zca_data *zca_entry = NULL;
  
  switch_to_section (s);

  for (ii = 0; ii < length; ii++)
    {
      zca_entry = find_zca_data (ii);
      assemble_integer (gen_rtx_CONST_INT (BLKmode, zca_entry->dwarf_expr),
			2, 1, 1);
    }
  return;
}

/* This function will clear the zca head.  */

static void
delete_zca_list (void)
{
  zca_head = NULL;
  return;
}

/* The main function to output all the metadata information for Cilk.  */

void
cilk_output_metadata (void)
{
  const char *itt_string = ".itt_notify_tab";
  section *s;
  int ii = 0;
  int entry_count = 0;
  int strings_len = 0;
  int exprs_len = 0;
  rtx st_label = NULL_RTX, str_table_label = NULL_RTX, expr_label = NULL_RTX;

  /* if there are no zca entries, then no reason to output this section */
  if (get_zca_entry_count () == 0)
    return;
  
  /* Create a new zca section (if necessary) and switch to it.  */
  s = get_unnamed_section (0, output_section_asm_op,
			   "\t.section .itt_notify_tab,\"a\"");
  switch_to_section (s);
  assemble_align (BITS_PER_WORD);

  st_label = create_metadata_label ("ZCA_START");
  str_table_label = create_metadata_label ("STRING_TABLE_START");
  expr_label = create_metadata_label ("EXPR_TABLE_START");
  /* Now we emit the start label */
  output_asm_label (st_label);
  fputs (":\n", asm_out_file);
  
  
  /* Here we output the magic number. */
  for (ii = 0; ii < (int) strlen (itt_string); ii++)
    assemble_integer (gen_rtx_CONST_INT (BLKmode, itt_string[ii]), 1, 1, 1);
  assemble_integer (gen_rtx_CONST_INT (BLKmode, 0), 1, 1, 1);
  /* here we output the major and minor version number */
  assemble_integer (gen_rtx_CONST_INT (BLKmode, ZCA_MAJOR_VER_NUMBER), 1, 1, 1);
  assemble_integer (gen_rtx_CONST_INT (BLKmode, ZCA_MINOR_VER_NUMBER), 1, 1, 1);

  entry_count = get_zca_entry_count ();
  assemble_integer (gen_rtx_CONST_INT (BLKmode, entry_count), 2, 1, 1);

  /* Now we output the offet to the string table.  This is done by printing out
   * the label for string_table_start, then a '-' then start_label.  The linker
   * should find out the correct absolute value.
   */
  fputs (integer_asm_op (GET_MODE_SIZE (SImode), 1), asm_out_file);
  output_asm_label (str_table_label);
  fputc ('-', asm_out_file);
  output_asm_label (st_label);
  fputc ('\n', asm_out_file);

  strings_len = get_zca_string_table_size ();
  assemble_integer (gen_rtx_CONST_INT (BLKmode, strings_len), 4, 1, 1);

  /* Now we output the expr table the same way.  */
  fputs (integer_asm_op (GET_MODE_SIZE (SImode), 1), asm_out_file);
  output_asm_label (expr_label);
  fputc ('-', asm_out_file);
  output_asm_label (st_label);
  fputc ('\n', asm_out_file);

  exprs_len = get_zca_exprs_table_size ();
  assemble_integer (gen_rtx_CONST_INT (BLKmode, exprs_len), 4, 1, 1);

  output_zca_table (s);

  output_asm_label (str_table_label);
  fputs (":\n", asm_out_file);
  output_string_table (s);

  output_asm_label (expr_label);
  fputs (":\n", asm_out_file);
  output_expr_table (s);

  delete_zca_list ();
 
  return;
}

/* This will replace pseudo registers with appropriate stack pointer.  */
static rtx
cilk_fix_stack_reg (rtx mem_rtx)
{
  /* This function will go through an RTL of type MEM and then check to see if
   * the register is PSEUDO, if so then we replace it with stack_pointer_rtx.
   * This is mainly used to find the DWARF codes for parameters that are pushed
   * in the stack.
   */
  if (!mem_rtx || !MEM_P (mem_rtx))
    return mem_rtx;

  if (REG_P (XEXP (mem_rtx, 0)))
    {
      if (REGNO (XEXP (mem_rtx, 0)) >= FIRST_PSEUDO_REGISTER)
	XEXP (mem_rtx, 0) = stack_pointer_rtx;
    }
  else if (GET_CODE (XEXP (mem_rtx, 0)) == PLUS)
    {
      rtx tmp_rtx = XEXP (mem_rtx, 0);
      if (REG_P (XEXP (tmp_rtx, 0)))
	if (REGNO (XEXP (tmp_rtx, 0)) >= FIRST_PSEUDO_REGISTER)
	  XEXP (XEXP (mem_rtx, 0), 0) = stack_pointer_rtx;
    }
  return mem_rtx;
}

/* This function will expand the cilk metadata.  */
rtx
expand_builtin_cilk_metadata (const char *annotation, tree exp)
{
  rtx metadata_label = NULL_RTX, call_insn = NULL_RTX;
  rtx expr_list_rtx = NULL_RTX, ii_rtx = NULL_RTX, reg_rtx = NULL_RTX;
  zca_data metadata_info;
  unsigned short size;
  dw_loc_descr_ref loc_ref;
  
  metadata_label  = create_metadata_label (annotation);
  expand_call (exp, NULL_RTX, 1);
  call_insn = get_last_insn ();
  emit_insn_after (metadata_label, get_last_insn ());
  metadata_info.label = metadata_label;
  metadata_info.string = xstrdup (annotation);
  metadata_info.ptr_next = NULL;
  expr_list_rtx = XEXP (call_insn, 8);
  if (expr_list_rtx)
    {
      for (ii_rtx  = expr_list_rtx; ii_rtx ; ii_rtx = XEXP (ii_rtx, 1))
	{
	  /* We have 2 options, either functions with 1 parameter or functions 
	     with 2 parameter.  Either case, you take the last parameter 
	     (1st in the former and 2nd in the latter).  So we do this */
	  reg_rtx = XEXP (ii_rtx, 0);
	  if (reg_rtx)
	    if (GET_CODE (reg_rtx) == USE)
	      {
		reg_rtx = XEXP (reg_rtx, 0);
		if (REG_P (reg_rtx))
		  metadata_info.reg_rtx = reg_rtx;
		else if (MEM_P (reg_rtx)) /* this means we are using stack */
		  metadata_info.reg_rtx = cilk_fix_stack_reg (reg_rtx);
	      }
	  
	}

      loc_ref = loc_descriptor (metadata_info.reg_rtx, VOIDmode,
				VAR_INIT_STATUS_UNKNOWN);
  
      gcc_assert (loc_ref);
      size = (unsigned short) size_of_locs (loc_ref);

      metadata_info.dwarf_expr = (unsigned short)(size & 0xFF) |
	(unsigned short) (loc_ref->dw_loc_opc << 8);
    }  
  else
    {
      /* This means we have no arguments.  */

      metadata_info.dwarf_expr = 1 | (DW_OP_lit0 << 8);
    }
  metadata_info.ptr_next = NULL;
  insert_into_zca_list (metadata_info);
  if (cfun) 
    { 
      cfun->calls_notify_intrinsic = 1;
      cfun->is_cilk_function = 1;
      CILK_FN_P (cfun->decl) = 1;
    }

  return const0_rtx;
}

/* This function will return true if the function is an annotated function.  */

bool
cilk_annotated_function_p (char *name)
{
  if (!name)
    return false;
  else if (!strcmp (name, "cilk_enter_begin")
	   || !strcmp (name, "cilk_enter_helper_begin")
	   || !strcmp (name, "cilk_enter_end")
	   || !strcmp (name, "cilk_spawn_prepare")
	   || !strcmp (name, "cilk_spawn_or_continue")
	   || !strcmp (name, "cilk_detach_begin")
	   || !strcmp (name, "cilk_detach_end")
	   || !strcmp (name, "cilk_sync_begin")
	   || !strcmp (name, "cilk_sync_end")
	   || !strcmp (name, "cilk_leave_begin")
	   || !strcmp (name, "cilk_leave_end")
	   || !strcmp (name, "cilkscreen_metacall")
	   || !strcmp (name, "cilk_resume")
	   || !strcmp (name, "cilk_leave_stolen")
	   || !strcmp (name, "__notify_zc_intrinsic")
	   || !strcmp (name, "__notify_intrinsic")
	   || !strcmp (name, "cilk_sync_abandon"))
    return true;
  else
    return false;
}

/* This function will remove the false annotation functions we inserted.  */
void
cilk_remove_annotated_functions (rtx first)
{
  rtx insn = NULL_RTX;
  rtx set_insn = NULL_RTX, mem_insn = NULL_RTX, call_insn = NULL_RTX;
  rtx symbol_insn = NULL_RTX;
  char *function_name = NULL;
  int ii = 0;
  VEC(rtx,gc) *rtx_delete_list = NULL;
  
  for (insn = first; insn != NULL_RTX; insn = NEXT_INSN (insn))
    {
      if (CALL_P (insn))
	{
	  set_insn = XEXP (insn, 4);
	  if ((set_insn && GET_CODE (set_insn) == SET)
	      /* If there is no return then we will see a CALL.  */
	      || (set_insn && GET_CODE (set_insn) == CALL))
	    {
	      if (set_insn && GET_CODE (set_insn) == SET)
		call_insn = XEXP (set_insn, 1);
	      else
		call_insn = set_insn;
	      if (call_insn && GET_CODE (call_insn) == CALL)
		{
		  mem_insn = XEXP (call_insn, 0);
		  if (mem_insn && GET_CODE (mem_insn) == MEM)
		    {
		      symbol_insn = XEXP (mem_insn, 0);
		      if (symbol_insn && GET_CODE (symbol_insn) == SYMBOL_REF)
			if (XSTR (symbol_insn, 0))
			  {
			    function_name = xstrdup (XSTR (symbol_insn, 0));
			    if (cilk_annotated_function_p (function_name))
			      VEC_safe_push (rtx, gc, rtx_delete_list, insn);
			  }
		    }
		}
	    }
	}
    }
  for (ii = 0; ii < (int) VEC_length (rtx, rtx_delete_list); ii++)
    remove_insn (VEC_index (rtx, rtx_delete_list, ii));
  
  return;
}

/* This function will check if the builtin function is a must expand one.  */

bool
is_cilk_must_expand_fn (enum built_in_function func_code)
{
  if (!flag_enable_cilk)
    return false;
  
  switch (func_code)
    {
    case BUILT_IN_CILK_DETACH:
    case BUILT_IN_CILK_ENTER_FRAME:
    case BUILT_IN_CILK_ENTER_BEGIN:
    case BUILT_IN_CILK_ENTER_H_BEGIN:
    case BUILT_IN_CILK_ENTER_END:
    case BUILT_IN_CILK_SPAWN_PREPARE:
    case BUILT_IN_CILK_POP_FRAME:
    case BUILT_IN_SPAWN_OR_CONT:
    case BUILT_IN_CILK_DETACH_BEGIN:
    case BUILT_IN_CILK_DETACH_END:
    case BUILT_IN_CILK_SYNC_BEGIN:
    case BUILT_IN_CILK_SYNC_END:
    case BUILT_IN_CILK_LEAVE_BEGIN:
    case BUILT_IN_CILK_LEAVE_END:
    case BUILT_IN_CILKSCREEN_METACALL:
    case BUILT_IN_CILK_RESUME:
    case BUILT_IN_CILK_STOLEN:
    case BUILT_IN_SYNC_ABANDON:
    case BUILT_IN_CILKSCREEN_EN_INSTR:
    case BUILT_IN_CILKSCREEN_DS_INSTR:
    case BUILT_IN_CILKSCREEN_EN_CHK:
    case BUILT_IN_CILKSCREEN_AQUIRE_LOCK:
    case BUILT_IN_CILKSCREEN_REL_LOCK:
    case BUILT_IN_NOTIFY_ZC_INTRINSIC:
    case BUILT_IN_NOTIFY_INTRINSIC:
      return true;
    default:
      return false;
    }
}

/* This will return true when name matches an elemental function mask.  */

bool
is_elem_fn_attribute_p (tree name)
{
  if (flag_enable_cilk)
    return false;
  return is_attribute_p ("mask", name)
    || is_attribute_p ("unmask", name)
    || is_attribute_p ("vectorlength", name)
    || is_attribute_p ("vector", name)
    || is_attribute_p ("linear", name)
    || is_attribute_p ("uniform", name);
}

/* This function will compare two function names and see if they are same.  */
static bool
compare_fn (const char *my_string, const char *search_str)
{
  const char *cc;
  const char *dd;
  int search_str_length = 0;
  int str_length = 0;
  
  if ((my_string == NULL) &&
      (search_str != NULL))
    return false;

  if ((my_string != NULL) &&
      (search_str == NULL))
    return false;

  if ((my_string == NULL) &&
      (search_str == NULL))
    return true;

  cc = my_string;
  dd = search_str;

  while (*cc != '\0')
    {
      str_length++;
      cc++;
    }

  while (*dd != '\0')
    {
      search_str_length++;
      dd++;
    }

  if (str_length != search_str_length)
    return false;

  /* now we see if the strings match */
  cc = my_string;
  dd = search_str;

  while (*cc != '\0' &&
	 *dd != '\0')
    {
      if (*cc != *dd)
	return false;
      cc++;
      dd++;
    }

  return true;
}

bool
is_cilk_function_decl (tree olddecl, tree newdecl)
{
  const char *cilkrts_enter_frame_array = "__cilkrts_enter_frame";
  const char *cilkrts_leave_frame_array = "__cilkrts_leave_frame";
  const char *cilkrts_sync_array = "__cilkrts_sync";
  bool found_enter_frame = false;
  bool found_leave_frame = false;
  bool found_sync = false;
  
  if ((DECL_NAME (olddecl) == NULL_TREE) || (DECL_NAME (newdecl) == NULL_TREE))
    return false;
  if (TREE_CODE (DECL_NAME (olddecl)) != IDENTIFIER_NODE)
    return false;
  if (TREE_CODE (DECL_NAME (newdecl)) != IDENTIFIER_NODE)
    return false;
  if (DECL_NAME (newdecl) != DECL_NAME (olddecl))
    return false;

  found_enter_frame = compare_fn (IDENTIFIER_POINTER (DECL_NAME (newdecl)),
				  cilkrts_enter_frame_array);
  found_leave_frame = compare_fn (IDENTIFIER_POINTER (DECL_NAME (newdecl)),
				  cilkrts_leave_frame_array);
  found_sync = compare_fn (IDENTIFIER_POINTER (DECL_NAME (newdecl)),
			   cilkrts_sync_array);
  if (found_sync || found_leave_frame || found_enter_frame)
    return true;
  return false;
}
