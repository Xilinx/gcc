/* Calculate branch probabilities, and basic block execution counts.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 1996, 1997, 1998, 1999,
   2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2010
   Free Software Foundation, Inc.
   Contributed by James E. Wilson, UC Berkeley/Cygnus Support;
   based on some ideas from Dain Samples of UC Berkeley.
   Further mangling by Bob Manson, Cygnus Support.
   Converted to use trees by Dale Johannesen, Apple Computer.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* Generate basic block profile instrumentation and auxiliary files.
   Tree-based version.  See profile.c for overview.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "flags.h"
#include "target.h"
#include "output.h"
#include "regs.h"
#include "function.h"
#include "basic-block.h"
#include "diagnostic-core.h"
#include "coverage.h"
#include "tree.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "tree-pass.h"
#include "timevar.h"
#include "value-prof.h"
#include "cgraph.h"
#include "output.h"
#include "params.h"
#include "profile.h"
#include "l-ipo.h"
#include "params.h"
#include "profile.h"
#include "target.h"
#include "output.h"

/* Number of statements inserted for each edge counter increment.  */
#define EDGE_COUNTER_STMT_COUNT 3

static GTY(()) tree gcov_type_node;
static GTY(()) tree gcov_type_tmp_var;
static GTY(()) tree tree_interval_profiler_fn;
static GTY(()) tree tree_pow2_profiler_fn;
static GTY(()) tree tree_one_value_profiler_fn;
static GTY(()) tree tree_indirect_call_profiler_fn;
static GTY(()) tree tree_indirect_call_topn_profiler_fn;
static GTY(()) tree tree_direct_call_profiler_fn;
static GTY(()) tree tree_average_profiler_fn;
static GTY(()) tree tree_ior_profiler_fn;


static GTY(()) tree ic_void_ptr_var;
static GTY(()) tree ic_gcov_type_ptr_var;
static GTY(()) tree dc_void_ptr_var;
static GTY(()) tree dc_gcov_type_ptr_var;
static GTY(()) tree ptr_void;
static GTY(()) tree gcov_info_decl;

/* When -D__KERNEL__ is in the option list, we assume this is a
   compilation for Linux Kernel.  */ 
bool is_kernel_build;

/* Do initialization work for the edge profiler.  */

/* Add code:
   // if flag_dyn_ipa
   extern gcov*	__gcov_indirect_call_topn_counters; // pointer to actual counter
   extern void*	__gcov_indirect_call_topn_callee; // actual callee address

   // else
   static gcov*	__gcov_indirect_call_counters; // pointer to actual counter
   static void*	__gcov_indirect_call_callee; // actual callee address
*/
static void
init_ic_make_global_vars (void)
{
  tree  gcov_type_ptr;

  ptr_void = build_pointer_type (void_type_node);

  if (flag_dyn_ipa)
    {
      ic_void_ptr_var 
	= build_decl (UNKNOWN_LOCATION, VAR_DECL, 
		      get_identifier ("__gcov_indirect_call_topn_callee"), 
		      ptr_void);
      TREE_PUBLIC (ic_void_ptr_var) = 1;
      DECL_EXTERNAL (ic_void_ptr_var) = 1;
      if (targetm.have_tls && !is_kernel_build)
        DECL_TLS_MODEL (ic_void_ptr_var) =
          decl_default_tls_model (ic_void_ptr_var);

      gcov_type_ptr = build_pointer_type (get_gcov_type ());
      ic_gcov_type_ptr_var 
	= build_decl (UNKNOWN_LOCATION, VAR_DECL, 
		      get_identifier ("__gcov_indirect_call_topn_counters"), 
		      gcov_type_ptr);
      TREE_PUBLIC (ic_gcov_type_ptr_var) = 1;
      DECL_EXTERNAL (ic_gcov_type_ptr_var) = 1;
      if (targetm.have_tls && !is_kernel_build)
        DECL_TLS_MODEL (ic_gcov_type_ptr_var) =
          decl_default_tls_model (ic_gcov_type_ptr_var);
    }
  else
    {
      ic_void_ptr_var 
	= build_decl (UNKNOWN_LOCATION, VAR_DECL, 
		      get_identifier ("__gcov_indirect_call_callee"), 
		      ptr_void);
      TREE_STATIC (ic_void_ptr_var) = 1;
      TREE_PUBLIC (ic_void_ptr_var) = 0;
      DECL_INITIAL (ic_void_ptr_var) = NULL;
      if (targetm.have_tls && !is_kernel_build)
        DECL_TLS_MODEL (ic_void_ptr_var) =
          decl_default_tls_model (ic_void_ptr_var);

      gcov_type_ptr = build_pointer_type (get_gcov_type ());
      ic_gcov_type_ptr_var 
	= build_decl (UNKNOWN_LOCATION, VAR_DECL, 
		      get_identifier ("__gcov_indirect_call_counters"), 
		      gcov_type_ptr);
      TREE_STATIC (ic_gcov_type_ptr_var) = 1;
      TREE_PUBLIC (ic_gcov_type_ptr_var) = 0;
      DECL_INITIAL (ic_gcov_type_ptr_var) = NULL;
      if (targetm.have_tls && !is_kernel_build)
        DECL_TLS_MODEL (ic_gcov_type_ptr_var) =
          decl_default_tls_model (ic_gcov_type_ptr_var);
    }

  DECL_ARTIFICIAL (ic_void_ptr_var) = 1;
  DECL_ARTIFICIAL (ic_gcov_type_ptr_var) = 1;
  if (!flag_dyn_ipa)
    {
      varpool_finalize_decl (ic_void_ptr_var);
      varpool_mark_needed_node (varpool_node (ic_void_ptr_var));
      varpool_finalize_decl (ic_gcov_type_ptr_var);
      varpool_mark_needed_node (varpool_node (ic_gcov_type_ptr_var));
    }
}

/* A pointer-set of the first statement in each block of statements that need to
   be applied a sampling wrapper.  */
static struct pointer_set_t *instrumentation_to_be_sampled = NULL;

/* extern __thread gcov_unsigned_t __gcov_sample_counter  */
static tree gcov_sample_counter_decl = NULL_TREE;

/* extern gcov_unsigned_t __gcov_sampling_rate  */
static tree gcov_sampling_rate_decl = NULL_TREE;

/* Insert STMT_IF around given sequence of consecutive statements in the
   same basic block starting with STMT_START, ending with STMT_END.  */

static void
insert_if_then (gimple stmt_start, gimple stmt_end, gimple stmt_if)
{
  gimple_stmt_iterator gsi;
  basic_block bb_original, bb_before_if, bb_after_if;
  edge e_if_taken, e_then_join;

  gsi = gsi_for_stmt (stmt_start);
  gsi_insert_before (&gsi, stmt_if, GSI_SAME_STMT);
  bb_original = gsi_bb (gsi);
  e_if_taken = split_block (bb_original, stmt_if);
  e_if_taken->flags &= ~EDGE_FALLTHRU;
  e_if_taken->flags |= EDGE_TRUE_VALUE;
  e_then_join = split_block (e_if_taken->dest, stmt_end);
  bb_before_if = e_if_taken->src;
  bb_after_if = e_then_join->dest;
  make_edge (bb_before_if, bb_after_if, EDGE_FALSE_VALUE);
}

/* Transform:

   ORIGINAL CODE

   Into:

   __gcov_sample_counter++;
   if (__gcov_sample_counter >= __gcov_sampling_rate)
     {
       __gcov_sample_counter = 0;
       ORIGINAL CODE
     }

   The original code block starts with STMT_START, is made of STMT_COUNT
   consecutive statements in the same basic block.  */

static void
add_sampling_wrapper (gimple stmt_start, gimple stmt_end)
{
  tree zero, one, tmp_var, tmp1, tmp2, tmp3;
  gimple stmt_inc_counter1, stmt_inc_counter2, stmt_inc_counter3;
  gimple stmt_reset_counter, stmt_assign_rate, stmt_if;
  gimple_stmt_iterator gsi;

  tmp_var = create_tmp_reg (get_gcov_unsigned_t (), "PROF_sample");
  tmp1 = make_ssa_name (tmp_var, NULL);
  tmp2 = make_ssa_name (tmp_var, NULL);

  /* Create all the new statements needed.  */
  stmt_inc_counter1 = gimple_build_assign (tmp1, gcov_sample_counter_decl);
  one = build_int_cst (get_gcov_unsigned_t (), 1);
  stmt_inc_counter2 = gimple_build_assign_with_ops (
      PLUS_EXPR, tmp2, tmp1, one);
  stmt_inc_counter3 = gimple_build_assign (gcov_sample_counter_decl, tmp2);
  zero = build_int_cst (get_gcov_unsigned_t (), 0);
  stmt_reset_counter = gimple_build_assign (gcov_sample_counter_decl, zero);
  tmp3 = make_ssa_name (tmp_var, NULL);
  stmt_assign_rate = gimple_build_assign (tmp3, gcov_sampling_rate_decl);
  stmt_if = gimple_build_cond (GE_EXPR, tmp2, tmp3, NULL_TREE, NULL_TREE);

  /* Insert them for now in the original basic block.  */
  gsi = gsi_for_stmt (stmt_start);
  gsi_insert_before (&gsi, stmt_inc_counter1, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt_inc_counter2, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt_inc_counter3, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt_assign_rate, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt_reset_counter, GSI_SAME_STMT);

  /* Insert IF block.  */
  insert_if_then (stmt_reset_counter, stmt_end, stmt_if);
}

/* Return whether STMT is the beginning of an instrumentation block to be
   applied sampling.  */

static bool
is_instrumentation_to_be_sampled (gimple stmt)
{
  return pointer_set_contains (instrumentation_to_be_sampled, stmt);
}

/* Add sampling wrappers around edge counter code in current function.  */

void
add_sampling_to_edge_counters (void)
{
  gimple_stmt_iterator gsi;
  basic_block bb;

  FOR_EACH_BB_REVERSE (bb)
    for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
      {
        gimple stmt = gsi_stmt (gsi);
        if (is_instrumentation_to_be_sampled (stmt))
          {
            gimple stmt_end;
            int i;
            /* The code for edge counter increment has EDGE_COUNTER_STMT_COUNT
               gimple statements. Advance that many statements to find the
               last statement.  */
            for (i = 0; i < EDGE_COUNTER_STMT_COUNT - 1; i++)
              gsi_next (&gsi);
            stmt_end = gsi_stmt (gsi);
            gcc_assert (stmt_end);
            add_sampling_wrapper (stmt, stmt_end);
            break;
          }
      }

  /* Free the bitmap.  */
  if (instrumentation_to_be_sampled)
    {
      pointer_set_destroy (instrumentation_to_be_sampled);
      instrumentation_to_be_sampled = NULL;
    }
}

static void
gimple_init_instrumentation_sampling (void)
{
  if (!gcov_sampling_rate_decl)
    {
      /* Define __gcov_sampling_rate regardless of -fprofile-generate-sampling.
         Otherwise the extern reference to it from libgcov becomes unmatched.
      */
      gcov_sampling_rate_decl = build_decl (
          UNKNOWN_LOCATION,
          VAR_DECL,
          get_identifier ("__gcov_sampling_rate"),
          get_gcov_unsigned_t ());
      TREE_PUBLIC (gcov_sampling_rate_decl) = 1;
      DECL_ARTIFICIAL (gcov_sampling_rate_decl) = 1;
      DECL_COMDAT_GROUP (gcov_sampling_rate_decl)
          = DECL_ASSEMBLER_NAME (gcov_sampling_rate_decl);
      TREE_STATIC (gcov_sampling_rate_decl) = 1;
      DECL_INITIAL (gcov_sampling_rate_decl) = build_int_cst (
          get_gcov_unsigned_t (),
          PARAM_VALUE (PARAM_PROFILE_GENERATE_SAMPLING_RATE));
      assemble_variable (gcov_sampling_rate_decl, 0, 0, 0);
    }

  if (flag_profile_generate_sampling && !instrumentation_to_be_sampled)
    {
      instrumentation_to_be_sampled = pointer_set_create ();
      gcov_sample_counter_decl = build_decl (
          UNKNOWN_LOCATION,
          VAR_DECL,
          get_identifier ("__gcov_sample_counter"),
          get_gcov_unsigned_t ());
      TREE_PUBLIC (gcov_sample_counter_decl) = 1;
      DECL_EXTERNAL (gcov_sample_counter_decl) = 1;
      DECL_ARTIFICIAL (gcov_sample_counter_decl) = 1;
      if (targetm.have_tls && !is_kernel_build)
        DECL_TLS_MODEL (gcov_sample_counter_decl) =
            decl_default_tls_model (gcov_sample_counter_decl);
      assemble_variable (gcov_sample_counter_decl, 0, 0, 0);
    }
}

void
gimple_init_edge_profiler (void)
{
  tree interval_profiler_fn_type;
  tree pow2_profiler_fn_type;
  tree one_value_profiler_fn_type;
  tree gcov_type_ptr;
  tree ic_profiler_fn_type;
  tree ic_topn_profiler_fn_type;
  tree dc_profiler_fn_type;
  tree average_profiler_fn_type;

  gimple_init_instrumentation_sampling ();

  if (!gcov_type_node)
    {
      char name_buf[32];
      gcov_type_node = get_gcov_type ();
      gcov_type_ptr = build_pointer_type (gcov_type_node);

      ASM_GENERATE_INTERNAL_LABEL (name_buf, "LPBX", 0);
      gcov_info_decl = build_decl (UNKNOWN_LOCATION, VAR_DECL,
                                   get_identifier (name_buf),
                                   get_gcov_unsigned_t ());
      DECL_EXTERNAL (gcov_info_decl) = 1;
      TREE_ADDRESSABLE (gcov_info_decl) = 1;

      /* void (*) (gcov_type *, gcov_type, int, unsigned)  */
      interval_profiler_fn_type
	      = build_function_type_list (void_type_node,
					  gcov_type_ptr, gcov_type_node,
					  integer_type_node,
					  unsigned_type_node, NULL_TREE);
      tree_interval_profiler_fn
	      = build_fn_decl ("__gcov_interval_profiler",
				     interval_profiler_fn_type);
      TREE_NOTHROW (tree_interval_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_interval_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_interval_profiler_fn));

      /* void (*) (gcov_type *, gcov_type)  */
      pow2_profiler_fn_type
	      = build_function_type_list (void_type_node,
					  gcov_type_ptr, gcov_type_node,
					  NULL_TREE);
      tree_pow2_profiler_fn = build_fn_decl ("__gcov_pow2_profiler",
						   pow2_profiler_fn_type);
      TREE_NOTHROW (tree_pow2_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_pow2_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_pow2_profiler_fn));

      /* void (*) (gcov_type *, gcov_type)  */
      one_value_profiler_fn_type
	      = build_function_type_list (void_type_node,
					  gcov_type_ptr, gcov_type_node,
					  NULL_TREE);
      tree_one_value_profiler_fn
	      = build_fn_decl ("__gcov_one_value_profiler",
				     one_value_profiler_fn_type);
      TREE_NOTHROW (tree_one_value_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_one_value_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_one_value_profiler_fn));

      init_ic_make_global_vars ();

      /* void (*) (gcov_type *, gcov_type, void *, void *)  */
      ic_profiler_fn_type
          = build_function_type_list (void_type_node,
                                      gcov_type_ptr, gcov_type_node,
                                      ptr_void,
                                      ptr_void, NULL_TREE);
      tree_indirect_call_profiler_fn
	      = build_fn_decl ("__gcov_indirect_call_profiler",
				     ic_profiler_fn_type);
      TREE_NOTHROW (tree_indirect_call_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_indirect_call_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_indirect_call_profiler_fn));

      /* void (*) (void *, void *, gcov_unsigned_t)  */
      ic_topn_profiler_fn_type
	= build_function_type_list (void_type_node, ptr_void, ptr_void,
				    get_gcov_unsigned_t (), NULL_TREE);
      tree_indirect_call_topn_profiler_fn
	      = build_fn_decl ("__gcov_indirect_call_topn_profiler",
                               ic_topn_profiler_fn_type);
      TREE_NOTHROW (tree_indirect_call_topn_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_indirect_call_topn_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_indirect_call_topn_profiler_fn));

      /* void (*) (void *, void *, gcov_unsigned_t)  */
      dc_profiler_fn_type
	= build_function_type_list (void_type_node, ptr_void, ptr_void,
				    get_gcov_unsigned_t (), NULL_TREE);
      tree_direct_call_profiler_fn
	= build_fn_decl ("__gcov_direct_call_profiler",
			 dc_profiler_fn_type);
      TREE_NOTHROW (tree_direct_call_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_direct_call_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_direct_call_profiler_fn));

      /* void (*) (gcov_type *, gcov_type)  */
      average_profiler_fn_type
	      = build_function_type_list (void_type_node,
					  gcov_type_ptr, gcov_type_node, NULL_TREE);
      tree_average_profiler_fn
	      = build_fn_decl ("__gcov_average_profiler",
				     average_profiler_fn_type);
      TREE_NOTHROW (tree_average_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_average_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_average_profiler_fn));
      tree_ior_profiler_fn
	      = build_fn_decl ("__gcov_ior_profiler",
				     average_profiler_fn_type);
      TREE_NOTHROW (tree_ior_profiler_fn) = 1;
      DECL_ATTRIBUTES (tree_ior_profiler_fn)
	= tree_cons (get_identifier ("leaf"), NULL,
		     DECL_ATTRIBUTES (tree_ior_profiler_fn));

      /* LTO streamer needs assembler names.  Because we create these decls
         late, we need to initialize them by hand.  */
      DECL_ASSEMBLER_NAME (tree_interval_profiler_fn);
      DECL_ASSEMBLER_NAME (tree_pow2_profiler_fn);
      DECL_ASSEMBLER_NAME (tree_one_value_profiler_fn);
      DECL_ASSEMBLER_NAME (tree_indirect_call_profiler_fn);
      DECL_ASSEMBLER_NAME (tree_average_profiler_fn);
      DECL_ASSEMBLER_NAME (tree_ior_profiler_fn);
    }
}

/* Output instructions as GIMPLE trees to increment the edge
   execution count, and insert them on E.  We rely on
   gsi_insert_on_edge to preserve the order.  */

void
gimple_gen_edge_profiler (int edgeno, edge e)
{
  tree ref, one;
  gimple stmt1, stmt2, stmt3;

  /* We share one temporary variable declaration per function.  This
     gets re-set in tree_profiling.  */
  if (gcov_type_tmp_var == NULL_TREE)
    gcov_type_tmp_var = create_tmp_reg (gcov_type_node, "PROF_edge_counter");
  ref = tree_coverage_counter_ref (GCOV_COUNTER_ARCS, edgeno);
  one = build_int_cst (gcov_type_node, 1);
  stmt1 = gimple_build_assign (gcov_type_tmp_var, ref);
  gimple_assign_set_lhs (stmt1, make_ssa_name (gcov_type_tmp_var, stmt1));
  stmt2 = gimple_build_assign_with_ops (PLUS_EXPR, gcov_type_tmp_var,
					gimple_assign_lhs (stmt1), one);
  gimple_assign_set_lhs (stmt2, make_ssa_name (gcov_type_tmp_var, stmt2));
  stmt3 = gimple_build_assign (unshare_expr (ref), gimple_assign_lhs (stmt2));

  if (flag_profile_generate_sampling)
    pointer_set_insert (instrumentation_to_be_sampled, stmt1);

  gsi_insert_on_edge (e, stmt1);
  gsi_insert_on_edge (e, stmt2);
  gsi_insert_on_edge (e, stmt3);
}

/* Emits code to get VALUE to instrument at GSI, and returns the
   variable containing the value.  */

static tree
prepare_instrumented_value (gimple_stmt_iterator *gsi, histogram_value value)
{
  tree val = value->hvalue.value;
  if (POINTER_TYPE_P (TREE_TYPE (val)))
    val = fold_convert (sizetype, val);
  return force_gimple_operand_gsi (gsi, fold_convert (gcov_type_node, val),
				   true, NULL_TREE, true, GSI_SAME_STMT);
}

/* Output instructions as GIMPLE trees to increment the interval histogram
   counter.  VALUE is the expression whose value is profiled.  TAG is the
   tag of the section for counters, BASE is offset of the counter position.  */

void
gimple_gen_interval_profiler (histogram_value value, unsigned tag, unsigned base)
{
  gimple stmt = value->hvalue.stmt;
  gimple_stmt_iterator gsi = gsi_for_stmt (stmt);
  tree ref = tree_coverage_counter_ref (tag, base), ref_ptr;
  gimple call;
  tree val;
  tree start = build_int_cst_type (integer_type_node,
				   value->hdata.intvl.int_start);
  tree steps = build_int_cst_type (unsigned_type_node,
				   value->hdata.intvl.steps);

  ref_ptr = force_gimple_operand_gsi (&gsi,
				      build_addr (ref, current_function_decl),
				      true, NULL_TREE, true, GSI_SAME_STMT);
  val = prepare_instrumented_value (&gsi, value);
  call = gimple_build_call (tree_interval_profiler_fn, 4,
			    ref_ptr, val, start, steps);
  gsi_insert_before (&gsi, call, GSI_NEW_STMT);
}

/* Output instructions as GIMPLE trees to increment the power of two histogram
   counter.  VALUE is the expression whose value is profiled.  TAG is the tag
   of the section for counters, BASE is offset of the counter position.  */

void
gimple_gen_pow2_profiler (histogram_value value, unsigned tag, unsigned base)
{
  gimple stmt = value->hvalue.stmt;
  gimple_stmt_iterator gsi = gsi_for_stmt (stmt);
  tree ref_ptr = tree_coverage_counter_addr (tag, base);
  gimple call;
  tree val;

  ref_ptr = force_gimple_operand_gsi (&gsi, ref_ptr,
				      true, NULL_TREE, true, GSI_SAME_STMT);
  val = prepare_instrumented_value (&gsi, value);
  call = gimple_build_call (tree_pow2_profiler_fn, 2, ref_ptr, val);
  gsi_insert_before (&gsi, call, GSI_NEW_STMT);
}

/* Output instructions as GIMPLE trees for code to find the most common value.
   VALUE is the expression whose value is profiled.  TAG is the tag of the
   section for counters, BASE is offset of the counter position.  */

void
gimple_gen_one_value_profiler (histogram_value value, unsigned tag, unsigned base)
{
  gimple stmt = value->hvalue.stmt;
  gimple_stmt_iterator gsi = gsi_for_stmt (stmt);
  tree ref_ptr = tree_coverage_counter_addr (tag, base);
  gimple call;
  tree val;

  ref_ptr = force_gimple_operand_gsi (&gsi, ref_ptr,
				      true, NULL_TREE, true, GSI_SAME_STMT);
  val = prepare_instrumented_value (&gsi, value);
  call = gimple_build_call (tree_one_value_profiler_fn, 2, ref_ptr, val);
  gsi_insert_before (&gsi, call, GSI_NEW_STMT);
}


/* Output instructions as GIMPLE trees for code to find the most
   common called function in indirect call.
   VALUE is the call expression whose indirect callee is profiled.
   TAG is the tag of the section for counters, BASE is offset of the
   counter position.  */

void
gimple_gen_ic_profiler (histogram_value value, unsigned tag, unsigned base)
{
  tree tmp1;
  gimple stmt1, stmt2, stmt3;
  gimple stmt;
  gimple_stmt_iterator gsi;
  tree ref_ptr;

  /* TODO add option -- only disble for topn icall profiling.  */
  if (DECL_STATIC_CONSTRUCTOR (current_function_decl) 
      || DECL_STATIC_CONSTRUCTOR (current_function_decl))
    return;
 
  stmt = value->hvalue.stmt;
  gsi = gsi_for_stmt (stmt);
  ref_ptr = tree_coverage_counter_addr (tag, base);
  ref_ptr = force_gimple_operand_gsi (&gsi, ref_ptr,
				      true, NULL_TREE, true, GSI_SAME_STMT);

  /* Insert code:

    __gcov_indirect_call_counters = get_relevant_counter_ptr ();
    __gcov_indirect_call_callee = (void *) indirect call argument;
   */

  tmp1 = create_tmp_reg (ptr_void, "PROF");
  stmt1 = gimple_build_assign (ic_gcov_type_ptr_var, ref_ptr);
  stmt2 = gimple_build_assign (tmp1, unshare_expr (value->hvalue.value));
  gimple_assign_set_lhs (stmt2, make_ssa_name (tmp1, stmt2));
  stmt3 = gimple_build_assign (ic_void_ptr_var, gimple_assign_lhs (stmt2));

  gsi_insert_before (&gsi, stmt1, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt2, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt3, GSI_SAME_STMT);
}


/* Output instructions as GIMPLE trees for code to find the most
   common called function in indirect call. Insert instructions at the
   beginning of every possible called function.
  */

void
gimple_gen_ic_func_profiler (void)
{
  struct cgraph_node * c_node = cgraph_node (current_function_decl);
  gimple_stmt_iterator gsi;
  gimple stmt1, stmt2;
  tree tree_uid, cur_func, counter_ptr, ptr_var, void0;

  if (cgraph_only_called_directly_p (c_node))
    return;

  gimple_init_edge_profiler ();

  gsi = gsi_after_labels (single_succ (ENTRY_BLOCK_PTR));

  cur_func = force_gimple_operand_gsi (&gsi,
				       build_addr (current_function_decl,
						   current_function_decl),
				       true, NULL_TREE,
				       true, GSI_SAME_STMT);
  counter_ptr = force_gimple_operand_gsi (&gsi, ic_gcov_type_ptr_var,
					  true, NULL_TREE, true,
					  GSI_SAME_STMT);
  ptr_var = force_gimple_operand_gsi (&gsi, ic_void_ptr_var,
				      true, NULL_TREE, true,
				      GSI_SAME_STMT);
  tree_uid = build_int_cst (gcov_type_node, current_function_funcdef_no);
  stmt1 = gimple_build_call (tree_indirect_call_profiler_fn, 4,
			     counter_ptr, tree_uid, cur_func, ptr_var);
  gsi_insert_before (&gsi, stmt1, GSI_SAME_STMT);

  /* Set __gcov_indirect_call_callee to 0,
     so that calls from other modules won't get misattributed
     to the last caller of the current callee. */
  void0 = build_int_cst (build_pointer_type (void_type_node), 0);
  stmt2 = gimple_build_assign (ic_void_ptr_var, void0);
  gsi_insert_before (&gsi, stmt2, GSI_SAME_STMT);
}

/* Output instructions as GIMPLE trees for code to find the most
   common called function in indirect call. Insert instructions at the
   beginning of every possible called function.
  */

static void
gimple_gen_ic_func_topn_profiler (void)
{
  gimple_stmt_iterator gsi;
  gimple stmt1;
  tree cur_func, gcov_info, cur_func_id;

  if (DECL_STATIC_CONSTRUCTOR (current_function_decl)
      || DECL_STATIC_CONSTRUCTOR (current_function_decl)
      || DECL_NO_INSTRUMENT_FUNCTION_ENTRY_EXIT (current_function_decl))
    return;

  gimple_init_edge_profiler ();

  gsi = gsi_after_labels (single_succ (ENTRY_BLOCK_PTR));

  cur_func = force_gimple_operand_gsi (&gsi,
				       build_addr (current_function_decl,
						   current_function_decl),
				       true, NULL_TREE,
				       true, GSI_SAME_STMT);
  gcov_info = build_fold_addr_expr (gcov_info_decl);
  cur_func_id = build_int_cst (get_gcov_unsigned_t (),
			       FUNC_DECL_FUNC_ID (cfun));
  stmt1 = gimple_build_call (tree_indirect_call_topn_profiler_fn, 
			     3, cur_func, gcov_info, cur_func_id);
  gsi_insert_before (&gsi, stmt1, GSI_SAME_STMT);
}


/* Output instructions as GIMPLE trees for code to find the number of
   calls at each direct call site.
   BASE is offset of the counter position, CALL_STMT is the direct call
   whose call-count is profiled.  */

static void
gimple_gen_dc_profiler (unsigned base, gimple call_stmt)
{
  gimple stmt1, stmt2, stmt3;
  gimple_stmt_iterator gsi = gsi_for_stmt (call_stmt);
  tree tmp1, tmp2, tmp3, callee = gimple_call_fn (call_stmt);
 
  /* Insert code:
     __gcov_direct_call_counters = get_relevant_counter_ptr ();
     __gcov_callee = (void *) callee;
   */
  tmp1 = tree_coverage_counter_addr (GCOV_COUNTER_DIRECT_CALL, base);
  tmp1 = force_gimple_operand_gsi (&gsi, tmp1, true, NULL_TREE,
				   true, GSI_SAME_STMT);
  stmt1 = gimple_build_assign (dc_gcov_type_ptr_var, tmp1);
  tmp2 = create_tmp_var (ptr_void, "PROF_dc");
  add_referenced_var (tmp2);
  stmt2 = gimple_build_assign (tmp2, unshare_expr (callee));
  tmp3 = make_ssa_name (tmp2, stmt2);
  gimple_assign_set_lhs (stmt2, tmp3);
  stmt3 = gimple_build_assign (dc_void_ptr_var, tmp3);
  gsi_insert_before (&gsi, stmt1, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt2, GSI_SAME_STMT);
  gsi_insert_before (&gsi, stmt3, GSI_SAME_STMT);
}


/* Output instructions as GIMPLE trees for code to find the number of
   calls at each direct call site. Insert instructions at the beginning of
   every possible called function.  */

static void
gimple_gen_dc_func_profiler (void)
{
  struct cgraph_node * c_node = cgraph_node (current_function_decl);
  gimple_stmt_iterator gsi;
  gimple stmt1;
  tree cur_func, gcov_info, cur_func_id;

  if (DECL_STATIC_CONSTRUCTOR (current_function_decl) 
      || DECL_STATIC_CONSTRUCTOR (current_function_decl)
      || DECL_NO_INSTRUMENT_FUNCTION_ENTRY_EXIT (current_function_decl))
    return;

  if (!c_node->needed && !c_node->reachable)
    return;

  gimple_init_edge_profiler ();

  gsi = gsi_after_labels (single_succ (ENTRY_BLOCK_PTR));

  cur_func = force_gimple_operand_gsi (&gsi,
				       build_addr (current_function_decl,
						   current_function_decl),
				       true, NULL_TREE,
				       true, GSI_SAME_STMT);
  gcov_info = build_fold_addr_expr (gcov_info_decl);
  cur_func_id = build_int_cst (get_gcov_unsigned_t (),
			       FUNC_DECL_FUNC_ID (cfun));
  stmt1 = gimple_build_call (tree_direct_call_profiler_fn, 3, cur_func,
			     gcov_info, cur_func_id);
  gsi_insert_before (&gsi, stmt1, GSI_SAME_STMT);
}

/* Output instructions as GIMPLE trees for code to find the most common value
   of a difference between two evaluations of an expression.
   VALUE is the expression whose value is profiled.  TAG is the tag of the
   section for counters, BASE is offset of the counter position.  */

void
gimple_gen_const_delta_profiler (histogram_value value ATTRIBUTE_UNUSED,
			       unsigned tag ATTRIBUTE_UNUSED,
			       unsigned base ATTRIBUTE_UNUSED)
{
  /* FIXME implement this.  */
#ifdef ENABLE_CHECKING
  internal_error ("unimplemented functionality");
#endif
  gcc_unreachable ();
}

/* Output instructions as GIMPLE trees to increment the average histogram
   counter.  VALUE is the expression whose value is profiled.  TAG is the
   tag of the section for counters, BASE is offset of the counter position.  */

void
gimple_gen_average_profiler (histogram_value value, unsigned tag, unsigned base)
{
  gimple stmt = value->hvalue.stmt;
  gimple_stmt_iterator gsi = gsi_for_stmt (stmt);
  tree ref_ptr = tree_coverage_counter_addr (tag, base);
  gimple call;
  tree val;

  ref_ptr = force_gimple_operand_gsi (&gsi, ref_ptr,
				      true, NULL_TREE,
				      true, GSI_SAME_STMT);
  val = prepare_instrumented_value (&gsi, value);
  call = gimple_build_call (tree_average_profiler_fn, 2, ref_ptr, val);
  gsi_insert_before (&gsi, call, GSI_NEW_STMT);
}

/* Output instructions as GIMPLE trees to increment the ior histogram
   counter.  VALUE is the expression whose value is profiled.  TAG is the
   tag of the section for counters, BASE is offset of the counter position.  */

void
gimple_gen_ior_profiler (histogram_value value, unsigned tag, unsigned base)
{
  gimple stmt = value->hvalue.stmt;
  gimple_stmt_iterator gsi = gsi_for_stmt (stmt);
  tree ref_ptr = tree_coverage_counter_addr (tag, base);
  gimple call;
  tree val;

  ref_ptr = force_gimple_operand_gsi (&gsi, ref_ptr,
				      true, NULL_TREE, true, GSI_SAME_STMT);
  val = prepare_instrumented_value (&gsi, value);
  call = gimple_build_call (tree_ior_profiler_fn, 2, ref_ptr, val);
  gsi_insert_before (&gsi, call, GSI_NEW_STMT);
}

/* String operation substitution record.  For each operation, e.g., memcpy,
   we keep up to four declarations, e.g., libopt__memcpy__{0,1,2,3}.
   They correspond to memcpy versions in which memory access is nontemporal
   in neither, first, second or both arguments (dst, src) respectively.  */

struct stringop_subst
{
  const char* original_name;  /* E.g., "memcpy".  */
  int num_args;               /* Number of args, 3 for memcpy.  */
  int num_ptr_args;           /* Number of pointer args, 2 for memcpy.  */
  tree instr_fun;             /* E.g., declaration of instrument_memcpy.  */
  tree nt_ops[4];             /* E.g., libopt__memcpy__{0,1,2,3}.  */
};
typedef struct stringop_subst* stringop_subst_t;

/* Substitution database.  TODO: switch to hash table.  */

static struct stringop_subst stringop_decl[] =
{
  {"memcpy",      3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"memset",      3, 1, NULL, {NULL, NULL, NULL, NULL}},
  {"memmove",     3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"memcmp",      3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"bcmp",        3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strlen",      1, 1, NULL, {NULL, NULL, NULL, NULL}},
  {"strcpy",      2, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strncpy",     3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strcat",      2, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strncat",     3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strdup",      1, 1, NULL, {NULL, NULL, NULL, NULL}},
  {"strndup",     2, 1, NULL, {NULL, NULL, NULL, NULL}},
  {"strcmp",      2, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strncmp",     3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strcasecmp",  2, 2, NULL, {NULL, NULL, NULL, NULL}},
  {"strncasecmp", 3, 2, NULL, {NULL, NULL, NULL, NULL}},
  {NULL,          0, 0, NULL, {NULL, NULL, NULL, NULL}}
};

/* Get the corresponding element in STRINGOP_DECL for NAME.  */

static stringop_subst_t
get_stringop_subst (const char* name)
{
  stringop_subst_t it;
  for (it = stringop_decl; it->original_name; it++)
    if (strcmp (name, it->original_name) == 0)
      return it;
  return 0;
}

/* Return the matching substitution if call site STMT is worth replacing.  */

static stringop_subst_t
reusedist_is_interesting_call (gimple stmt)
{
  tree fndecl, name;

  if (gimple_code (stmt) != GIMPLE_CALL)
    return 0;

  fndecl = gimple_call_fndecl (stmt);

  if (fndecl == NULL_TREE)
    return 0;

  name = DECL_NAME (fndecl);

  if (name == NULL_TREE)
    return 0;

  return get_stringop_subst (IDENTIFIER_POINTER (name));
}

/* Make up an instrumentation function name for string operation OP.  */

static void
reusedist_instr_func_name (const char* op, char result[], int size)
{
  int written;

  written = snprintf (result, size, "reusedist_instr_%s", op);

  gcc_assert (written < size);
}

/* Create a declaration for an instr. function if not already done.
   Use TEMPLATE_STMT to figure out argument types.  */

static tree
reusedist_get_instr_decl (gimple template_stmt, stringop_subst_t subst)
{
  if (!subst->instr_fun)
    {
      tree args;
      char name[64];

      if (!ptr_void)
        ptr_void = build_pointer_type (void_type_node);

      reusedist_instr_func_name (subst->original_name, name, 64);

      switch (subst->num_args)
        {
          case 1:
            args = build_function_type_list (
                void_type_node, ptr_void,
                TREE_TYPE (gimple_call_arg (template_stmt, 0)),
                NULL_TREE);
            break;
          case 2:
            args = build_function_type_list (
                void_type_node, ptr_void,
                TREE_TYPE (gimple_call_arg (template_stmt, 0)),
                TREE_TYPE (gimple_call_arg (template_stmt, 1)),
                NULL_TREE);
            break;
          case 3:
            args = build_function_type_list (
                void_type_node, ptr_void,
                TREE_TYPE (gimple_call_arg (template_stmt, 0)),
                TREE_TYPE (gimple_call_arg (template_stmt, 1)),
                TREE_TYPE (gimple_call_arg (template_stmt, 2)),
                NULL_TREE);
            break;
          default:
            gcc_assert (false);
        }
      subst->instr_fun = build_fn_decl (name, args);
    }

  return subst->instr_fun;
}

/* Return call to instrumentation function for string op call site STMT.
   Given a call to memcpy (dst, src, len), it will return a call to
   reusedist_instrument_memcpy (counters, dst, src, len).  */

static gimple
reusedist_make_instr_call (gimple stmt, stringop_subst_t subst, tree counters)
{
  tree profiler_fn;

  if (!subst)
    return 0;

  profiler_fn = reusedist_get_instr_decl (stmt, subst);

 switch (subst->num_args)
   {
     case 1:
       return gimple_build_call (profiler_fn, 1 + subst->num_args, counters,
                                 gimple_call_arg (stmt, 0));
     case 2:
       return gimple_build_call (profiler_fn, 1 + subst->num_args, counters,
                                 gimple_call_arg (stmt, 0),
                                 gimple_call_arg (stmt, 1));
     case 3:
       return gimple_build_call (profiler_fn, 1 + subst->num_args, counters,
                                 gimple_call_arg (stmt, 0),
                                 gimple_call_arg (stmt, 1),
                                 gimple_call_arg (stmt, 2));
     default:
       gcc_assert (false);
   }
}

/* Reuse distance information for a single memory block at a single site.
   For some operations, such as memcpy, there will be two such descriptors,
   one of the source and one for the destination.
   We're keeping the average reuse distance
   (e.g., distance from a MEMCPY call until the memory written is first used).
   We're also keeping the average operation size (e.g., memcpy size).
   These averages are measured over all dynamic invocations of the same
   static site.  We're also storing the dynamic operation count.

   We're also keeping a measure named dist_x_size, which is the sum of
   products (distance * size) across all dynamic instances.  This is meant
   to account for some information loss through aggregation.  For instance,
   consider two scenarios.
   A: 50% of operations have large reuse distance but are very short.
      50% of operations have short reuse distance but are very long.
   B: 50% of operations have large reuse distance and are large.
      50% of operations have short reuse distance and are short.
   Without the dist_x_size measure, these scenarios can't be told apart
   from the other three measures.  With the dist_x_size measure, scenario B
   will look like a better candidate.  */

struct reusedist_t {
  gcov_type mean_dist;    /* Average reuse distance.  */
  gcov_type mean_size;    /* Average size of memory referenced.  */
  gcov_type count;        /* Operation count.  */
  gcov_type dist_x_size;  /* Sum of (distance * size >> 12) across all ops.  */
};

typedef struct reusedist_t reusedist_t;

/* Number of gcov counters for one reuse distance measurement.  */

const int RD_NUM_COUNTERS = sizeof(reusedist_t) / sizeof(gcov_type);

/* Initialize RD from gcov COUNTERS.  */

static void
reusedist_from_counters (const gcov_type* counters,
                         reusedist_t* rd)
{
  memcpy (rd, counters, RD_NUM_COUNTERS * sizeof (gcov_type));
}

/* Instrument current function to collect reuse distance for string ops.
   The heavy lifting is done by an external library.  The interface
   to this library is functions like this:

   void reusedist_instr_memcpy(gcov_type *counters,
                               void *dst, void *src, size_t len);

   This function will measure the reuse distance for the given operations
   DST with offset LEN, and store values in COUNTERS for one or two pointer
   arguments.  E.g., for memcpy 2 * RD_NUM_COUNTERS counters will be set,
   first RD_NUM_COUNTERS for DST and last RD_NUM_COUNTERS for SRC.
   For strlen, only RD_NUM_COUNTERS counters will be allocated thus the
   runtime is expected to set only RD_NUM_COUNTERS counters.
   The counters will record:
   - mean reuse distance
   - mean operation size
   - call count
   - sum(reuse distance * operation size) across all calls
     To avoid overflow, each product is first scaled down by a factor of 2^12.

   All reuse distance measurements for dynamic executions of the same static
   string operation will be aggregated into a single set of counters.
   The reuse distance library uses the passed COUNTERS pointer as index
   in its internal tables.  */

void
gimple_gen_reusedist (void)
{
  basic_block bb;
  gimple_stmt_iterator gsi;

  if (DECL_STATIC_CONSTRUCTOR (current_function_decl))
    return;

  gimple_init_edge_profiler ();

  FOR_EACH_BB (bb)
    for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
      {
        gimple stmt = gsi_stmt (gsi);
        stringop_subst_t subst = reusedist_is_interesting_call (stmt);

        if (subst
            && coverage_counter_alloc (
                GCOV_COUNTER_REUSE_DIST,
                subst->num_ptr_args * RD_NUM_COUNTERS))
          {
            location_t locus;
            tree counters = tree_coverage_counter_addr (
                GCOV_COUNTER_REUSE_DIST, 0);

            counters = force_gimple_operand_gsi (
                &gsi, counters, true, NULL_TREE, true, GSI_SAME_STMT);

            gsi_insert_after (
                &gsi,
                reusedist_make_instr_call (stmt, subst, counters),
                GSI_NEW_STMT);

            locus = (stmt != NULL)
                ? gimple_location (stmt)
                : DECL_SOURCE_LOCATION (current_function_decl);
            inform (locus,
                    "inserted reuse distance instrumentation for %qs, using "
                    "%d gcov counters", subst->original_name,
                    subst->num_ptr_args * RD_NUM_COUNTERS);
          }
      }
}

/* Make up a nontemporal substitution name, e.g., "libopt__memcpy__3".  */

static void
nt_op_name (const char* name, int suffix, char result[], int size)
{
  int written;

  written = snprintf (result, size, "libopt__%s__%d", name, suffix);

  gcc_assert (written < size);
}

/* Get size threshold for reusedist substitution decisions.  */

static gcov_type
reusedist_get_size_threshold (const char* name)
{
  if (!strcmp (name, "memcpy"))
    return (gcov_type)PARAM_VALUE (PARAM_REUSEDIST_MEMCPY_SIZE_THRESH);

  if (!strcmp (name, "memset"))
    return (gcov_type)PARAM_VALUE (PARAM_REUSEDIST_MEMSET_SIZE_THRESH);

  /* Use memcpy threshold as default for unspecified operations.  */
  return (gcov_type)PARAM_VALUE (PARAM_REUSEDIST_MEMCPY_SIZE_THRESH);
}

/* Get distance threshold for reusedist substitution decisions.  */

static gcov_type
reusedist_get_distance_large_threshold (void)
{
  return (gcov_type)PARAM_VALUE (PARAM_REUSEDIST_MEAN_DIST_LARGE_THRESH);
}

/* Get distance threshold for reusedist substitution decisions.  */

static gcov_type
reusedist_get_distance_small_threshold (void)
{
  return (gcov_type)PARAM_VALUE (PARAM_REUSEDIST_MEAN_DIST_SMALL_THRESH);
}

/* Get call count threshold for reusedist substitution decisions.  */

static gcov_type
reusedist_get_count_threshold (void)
{
  return (gcov_type)PARAM_VALUE (PARAM_REUSEDIST_CALL_COUNT_THRESH);
}

/* Return whether switching to nontemporal string operation is worth it.
   NAME is the function name, such as "memcpy".
   COUNTERS is a pointer to gcov counters for this operation site.
   Return 1 if worth it, -1 if not worth it and 0 if not sure.  */

static int
reusedist_nt_is_worth_it (const char* name, const gcov_type* counters)
{
  reusedist_t rd;

  reusedist_from_counters (counters, &rd);

  /* TODO: Need to add check for dist_x_size.  */

  if (rd.mean_size < reusedist_get_size_threshold (name)
      || rd.count < reusedist_get_count_threshold ())
    /* If the size of the operation is small, don't substitute.  */
    return 0;

  if (rd.mean_dist >= reusedist_get_distance_large_threshold ())
    /* Enforce non-temporal.  */
    return 1;
  else if (rd.mean_dist <= reusedist_get_distance_small_threshold ())
    /* Enforce temporal.  */
    return -1;
  else
    /* Not conclusive.  */
    return 0;
}

/* Create a declaration for a nontemporal version if not already done.
   INDEX is the index of the version in list [first, second, both].  */

static tree
reusedist_get_nt_decl (tree template_decl, stringop_subst_t subst, int index)
{
  if (!subst->nt_ops[index])
    {
      char nt_name[256];
      nt_op_name (subst->original_name, index, nt_name, 256);
      subst->nt_ops[index] = build_fn_decl (nt_name,
                                            TREE_TYPE (template_decl));
    }

  return subst->nt_ops[index];
}

/* Issue notes with reuse distance values in COUNTERS for given ARG.  */

static void
maybe_issue_profile_use_note (location_t locus, gcov_type* counters, int arg)
{
  reusedist_t rd;

  reusedist_from_counters (counters, &rd);

  if (rd.count)
    inform (locus, "reuse distance counters for arg %d: %lld %lld %lld %lld",
            arg, (long long int)rd.mean_dist, (long long int)rd.mean_size,
            (long long int)rd.count, (long long int)rd.dist_x_size);
}

/* Substitute with nontemporal version when profitable.  */

static void
reusedist_maybe_replace_with_nt_version (gimple stmt,
                                         gcov_type* counters,
                                         stringop_subst_t subst)
{
  int first, second, suffix;
  tree subst_decl;
  const char* name = subst->original_name;
  location_t locus;

  locus = (stmt != NULL)
      ? gimple_location (stmt)
      : DECL_SOURCE_LOCATION (current_function_decl);

  gcc_assert (1 == subst->num_ptr_args || 2 == subst->num_ptr_args);

  maybe_issue_profile_use_note (locus, counters, 1);
  first = reusedist_nt_is_worth_it (name, counters);

  if (2 == subst->num_ptr_args)
    {
      maybe_issue_profile_use_note (locus, counters + RD_NUM_COUNTERS, 2);
      second = reusedist_nt_is_worth_it (name, counters + RD_NUM_COUNTERS);
    }
  else
      second = 0;

  if (first > 0)
    /* Nontemporal in first arg.  */
    {
      /* The operation on the first arg should be nontemporal.  */
      if (second > 0)
        suffix = 3;
      else
        suffix = 1;
    }
  else if (first < 0)
    /* Temporal in first arg.  */
    {
      if (second > 0)
        suffix = 2;
      else if (second < 0)
        suffix = 0;
      else
        suffix = -1;
    }
  else
    /* Don't know about the first arg.  */
    {
      if (second > 0)
        suffix = 2;
      else
        suffix = -1;
    }

  if (suffix == -1)
    return;

  subst_decl = reusedist_get_nt_decl (gimple_call_fndecl (stmt), subst,
                                      suffix);
  gimple_call_set_fndecl (stmt, subst_decl);
  inform (locus, "replaced %qs with non-temporal %qs",
          subst->original_name,
          IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (subst_decl)));
}

/* Replace string operations with equivalent nontemporal, when profitable.  */

void
optimize_reusedist (void)
{
  basic_block bb;
  gimple_stmt_iterator gsi;
  unsigned n_counters;
  unsigned counter_index = 0;
  gcov_type *counters = get_coverage_counts_no_warn (
      DECL_STRUCT_FUNCTION (current_function_decl),
      GCOV_COUNTER_REUSE_DIST, &n_counters);

  if (!n_counters || DECL_STATIC_CONSTRUCTOR (current_function_decl))
    return;

  gcc_assert (!(n_counters % RD_NUM_COUNTERS));

  FOR_EACH_BB (bb)
    for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
      {
        gimple stmt = gsi_stmt (gsi);
        stringop_subst_t subst = reusedist_is_interesting_call (stmt);

        if (subst)
          {
            if (counter_index < n_counters)
              reusedist_maybe_replace_with_nt_version (
                  stmt, &counters[counter_index], subst);
            counter_index += subst->num_ptr_args * RD_NUM_COUNTERS;
          }
      }

  if (counter_index != n_counters)
    {
      warning (0, "coverage mismatch for reuse distance counters "
               "in function %qs", IDENTIFIER_POINTER
               (DECL_ASSEMBLER_NAME (current_function_decl)));
      inform (input_location, "number of counters is %u instead of %u",
              n_counters, counter_index);
    }
}

/* Profile all functions in the callgraph.  */

static unsigned int
tree_profiling (void)
{
  struct cgraph_node *node;

  /* Don't profile functions produced at destruction time, particularly
     the gcov datastructure initializer.  Don't profile if it has been
     already instrumented either (when OpenMP expansion creates
     child function from already instrumented body).  */
  if (cgraph_state == CGRAPH_STATE_FINISHED)
    return 0;

  /* After value profile transformation, artificial edges (that keep
     function body from being deleted) won't be needed.  */

  cgraph_pre_profiling_inlining_done = true;
  /* Now perform link to allow cross module inlining.  */
  cgraph_do_link ();
  varpool_do_link ();
  cgraph_unify_type_alias_sets ();

  init_node_map();

  for (node = cgraph_nodes; node; node = node->next)
    {
      if (!node->analyzed
	  || !gimple_has_body_p (node->decl)
	  || !(!node->clone_of || node->decl != node->clone_of->decl))
	continue;

      /* Don't profile functions produced for builtin stuff.  */
      if (DECL_SOURCE_LOCATION (node->decl) == BUILTINS_LOCATION
	  || DECL_STRUCT_FUNCTION (node->decl)->after_tree_profile)
	continue;

      push_cfun (DECL_STRUCT_FUNCTION (node->decl));
      current_function_decl = node->decl;

      /* Re-set global shared temporary variable for edge-counters.  */
      gcov_type_tmp_var = NULL_TREE;

      branch_prob ();

      if (! flag_branch_probabilities
	  && flag_profile_values
	  && !flag_dyn_ipa)
	gimple_gen_ic_func_profiler ();

      if (flag_branch_probabilities
	  && flag_profile_values
	  && flag_value_profile_transformations)
	gimple_value_profile_transformations ();

      /* The above could hose dominator info.  Currently there is
	 none coming in, this is a safety valve.  It should be
	 easy to adjust it, if and when there is some.  */
      free_dominance_info (CDI_DOMINATORS);
      free_dominance_info (CDI_POST_DOMINATORS);

      current_function_decl = NULL;
      pop_cfun ();
    }

  /* Drop pure/const flags from instrumented functions.  */
  for (node = cgraph_nodes; node; node = node->next)
    {
      if (!node->analyzed
	  || !gimple_has_body_p (node->decl)
	  || !(!node->clone_of || node->decl != node->clone_of->decl))
	continue;

      /* Don't profile functions produced for builtin stuff.  */
      if (DECL_SOURCE_LOCATION (node->decl) == BUILTINS_LOCATION
	  || DECL_STRUCT_FUNCTION (node->decl)->after_tree_profile)
	continue;

      cgraph_set_const_flag (node, false, false);
      cgraph_set_pure_flag (node, false, false);
    }

  /* Update call statements and rebuild the cgraph.  */
  for (node = cgraph_nodes; node; node = node->next)
    {
      basic_block bb;

      if (!node->analyzed
	  || !gimple_has_body_p (node->decl)
	  || !(!node->clone_of || node->decl != node->clone_of->decl))
	continue;

      /* Don't profile functions produced for builtin stuff.  */
      if (DECL_SOURCE_LOCATION (node->decl) == BUILTINS_LOCATION
	  || DECL_STRUCT_FUNCTION (node->decl)->after_tree_profile)
	continue;

      push_cfun (DECL_STRUCT_FUNCTION (node->decl));
      current_function_decl = node->decl;

      FOR_EACH_BB (bb)
	{
	  gimple_stmt_iterator gsi;
	  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
	    {
	      gimple stmt = gsi_stmt (gsi);
	      if (is_gimple_call (stmt))
		update_stmt (stmt);
	    }
	}

      cfun->after_tree_profile = 1;
      update_ssa (TODO_update_ssa);

      rebuild_cgraph_edges ();

      current_function_decl = NULL;
      pop_cfun ();
    }

  del_node_map();
  return 0;
}

/* Return true if tree-based direct-call profiling is in effect, else false.  */

static bool
do_direct_call_profiling (void)
{
  return !flag_branch_probabilities
    && (profile_arc_flag || flag_test_coverage)
    && flag_dyn_ipa;
}

/* Instrument current function to collect direct call profile information.  */

static unsigned int
direct_call_profiling (void)
{
  basic_block bb;
  gimple_stmt_iterator gsi;

  /* Add code:
     extern gcov* __gcov_direct_call_counters; // pointer to actual counter
     extern void* __gcov_direct_call_callee;   // actual callee address
  */
  if (!dc_gcov_type_ptr_var)
    {
      dc_gcov_type_ptr_var
	= build_decl (UNKNOWN_LOCATION, VAR_DECL,
		      get_identifier ("__gcov_direct_call_counters"),
		      build_pointer_type (gcov_type_node));
      DECL_ARTIFICIAL (dc_gcov_type_ptr_var) = 1;
      DECL_EXTERNAL (dc_gcov_type_ptr_var) = 1;
      if (!is_kernel_build)
        DECL_TLS_MODEL (dc_gcov_type_ptr_var) =
	  decl_default_tls_model (dc_gcov_type_ptr_var);

      dc_void_ptr_var =
	build_decl (UNKNOWN_LOCATION, VAR_DECL,
	            get_identifier ("__gcov_direct_call_callee"),
		    ptr_void);
      DECL_ARTIFICIAL (dc_void_ptr_var) = 1;
      DECL_EXTERNAL (dc_void_ptr_var) = 1;
      if (!is_kernel_build)
        DECL_TLS_MODEL (dc_void_ptr_var) =
	  decl_default_tls_model (dc_void_ptr_var);
    }

  add_referenced_var (gcov_info_decl);
  add_referenced_var (dc_gcov_type_ptr_var);
  add_referenced_var (dc_void_ptr_var);

  if (!DECL_STATIC_CONSTRUCTOR (current_function_decl))
    {
      FOR_EACH_BB (bb)
	for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
	  {
	    gimple stmt = gsi_stmt (gsi);
	    /* Check if this is a direct call, and not a builtin call.  */
	    if (gimple_code (stmt) != GIMPLE_CALL
		|| gimple_call_fndecl (stmt) == NULL_TREE
		|| DECL_BUILT_IN (gimple_call_fndecl (stmt))
		|| DECL_IS_BUILTIN (gimple_call_fndecl (stmt)))
	      continue;

	    if (!coverage_counter_alloc (GCOV_COUNTER_DIRECT_CALL, 2))
	      continue;
	    gimple_gen_dc_profiler (0, stmt);
	  }
      coverage_dc_end_function ();
    }

  if (coverage_function_present (FUNC_DECL_FUNC_ID (cfun)))
    {
      gimple_gen_dc_func_profiler ();
      if (! flag_branch_probabilities
          && flag_profile_values)
        gimple_gen_ic_func_topn_profiler ();
    }

  return 0;
}

/* When profile instrumentation, use or test coverage shall be performed.  */

static bool
gate_tree_profile_ipa (void)
{
  return (!in_lto_p
	  && (flag_branch_probabilities || flag_test_coverage
	      || profile_arc_flag || flag_profile_reusedist
              || flag_optimize_locality));
}

struct simple_ipa_opt_pass pass_ipa_tree_profile =
{
 {
  SIMPLE_IPA_PASS,
  "tree_profile_ipa",                  /* name */
  gate_tree_profile_ipa,               /* gate */
  tree_profiling,                      /* execute */
  NULL,                                /* sub */
  NULL,                                /* next */
  0,                                   /* static_pass_number */
  TV_IPA_PROFILE,                      /* tv_id */
  0,                                   /* properties_required */
  0,                                   /* properties_provided */
  0,                                   /* properties_destroyed */
  0,                                   /* todo_flags_start */
  TODO_dump_func                       /* todo_flags_finish */
 }
};

struct gimple_opt_pass pass_direct_call_profile =
{
 {
  GIMPLE_PASS,
  "dc_profile",				/* name */
  do_direct_call_profiling,		/* gate */
  direct_call_profiling,		/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  TV_BRANCH_PROB,			/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,					/* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_update_ssa | TODO_dump_func	/* todo_flags_finish */
 }
};

#include "gt-tree-profile.h"
