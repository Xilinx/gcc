/* OpencCL code generation for GRAPHITE-OpenCL.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.

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

/* OpenCL code genration for GRAPHITE-OpenCL.  This file implements
   OpenCL kernel code generation from CLAST structures.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "toplev.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "domwalk.h"
#include "value-prof.h"
#include "pointer-set.h"
#include "gimple.h"
#include "sese.h"
#include "output.h"
#include <sys/time.h>
#include "hashtab.h"
#include "gimple-pretty-print.h"
#include "tree.h"

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "ppl_c.h"
#include "graphite-ppl.h"
#include "graphite.h"
#include "graphite-poly.h"
#include "graphite-scop-detection.h"
#include "graphite-clast-to-gimple.h"
#include "graphite-dependences.h"
#include "dyn-string.h"
#include "graphite-opencl.h"


/* These functions implement code generation from different clast
   structures.  */
static void opencl_print_stmt_list (struct clast_stmt *, opencl_main, int);
static void opencl_print_for (struct clast_for *, opencl_main, int);
static void opencl_print_guard (struct clast_guard *, opencl_main, int);
static void opencl_print_equation (struct clast_equation *, opencl_main);
static void opencl_print_expr (struct clast_expr *, opencl_main);
static void opencl_add_variable (const char *, tree,  opencl_main);
static void opencl_print_term (struct clast_term *, opencl_main);
static void opencl_print_reduction (struct clast_reduction *, opencl_main);
static void opencl_print_sum (struct clast_reduction *, opencl_main);
static void opencl_print_binary (struct clast_binary *, opencl_main);
static void opencl_print_minmax_c (struct clast_reduction *, opencl_main);

/* These function implement code generation from different gimple
   objects.  */
static void opencl_print_bb (basic_block, opencl_main);
static void opencl_print_gimple_assign_operation (gimple, opencl_main);
static void opencl_print_gimple_assign (gimple, opencl_main);
static void opencl_print_gimple (gimple, opencl_main);
static int opencl_print_operand (tree, bool, opencl_main);


static void opencl_print_local_vars (const char *, const char *, const char *,
                                     opencl_main);
static void opencl_try_variable (opencl_main, tree);
static const char *opencl_get_var_name (tree);
static void opencl_build_defines (tree, opencl_main);
static void opencl_expand_scalar_vars (opencl_main, gimple);
static void opencl_add_function_arg (opencl_main, tree, const char *);
static void opencl_add_data_refs_pbb (poly_bb_p, opencl_main);
static void opencl_add_non_scalar_type_decl (tree, dyn_string_t, const char *);
static const char *opencl_print_function_arg_with_type (const char *, tree);
static bool check_and_mark_arg (opencl_main, const char *, bool);


/* Compare two clast names based on their indexes.  */

static int
opencl_cmp_scat (const char *scat1, const char *scat2)
{
  int len_1 = strlen (scat1);
  int len_2 = strlen (scat2);

  if (len_1 > len_2)
    return 1;

  if (len_1 < len_2)
    return -1;

  return strcmp (scat1, scat2);
}

/* This function implements !strcmp (STR1, STR2) call.  */

static int
opencl_cmp_str (const void *str1, const void *str2)
{
  const char *c_str1 = (const char *) str1;
  const char *c_str2 = (const char *) str2;
  return !strcmp (c_str1, c_str2);
}

/* Get body of generating kernel function from CODE_GEN.  */

static dyn_string_t
opencl_get_current_body (opencl_main code_gen)
{
  return code_gen->current_body->body;
}

/* Get header of generating kernel function from CODE_GEN.  */

static dyn_string_t
opencl_get_current_header (opencl_main code_gen)
{
  return code_gen->current_body->header;
}

/* Appends a string STR to the header of the generating kernel
   function from CODE_GEN.  */

static void
opencl_append_string_to_header (const char *str, opencl_main code_gen)
{
  dyn_string_t tmp = opencl_get_current_header (code_gen);
  dyn_string_append_cstr (tmp, str);
}

/* Appends a string STR to the body of the generating kernel function
   from CODE_GEN.  */

static void
opencl_append_string_to_body (const char *str, opencl_main code_gen)
{
  dyn_string_t tmp = opencl_get_current_body (code_gen);
  dyn_string_append_cstr (tmp, str);
}

/* Appends an integer NUM to the string STR following FORMAT.  */

static void
opencl_append_int_to_str (dyn_string_t str, long num, const char *format)
{
  char tmp[100];
  sprintf (tmp, format, num);
  dyn_string_append_cstr (str, tmp);
}

/* Appends an integer NUM to the header of the generating kernel
   function from CODE_GEN following FORMAT.  */

static void
opencl_append_num_to_header (opencl_main code_gen, long num,
			     const char *format)
{
  dyn_string_t tmp = opencl_get_current_header (code_gen);
  opencl_append_int_to_str (tmp, num, format);
}

/* Appends an integer NUM to the body of the generating kernel
   function from CODE_GEN following FORMAT.  */

static void
opencl_append_num_to_body (opencl_main code_gen, long num, const char *format)
{
  dyn_string_t tmp = opencl_get_current_body (code_gen);
  opencl_append_int_to_str (tmp, num, format);
}

/* Get TYPE scalar (base) part.  */

static tree
opencl_get_main_type (tree type)
{
  while (TREE_CODE (type) == ARRAY_TYPE
	 || TREE_CODE (type) == POINTER_TYPE)
    type = TREE_TYPE (type);
  return build_pointer_type (type);
}


/* Create the base part of FUNCTION declaration, similar to this:
   "__global void __opencl_function_0".  */

static void
opencl_create_function_code (opencl_body function)
{
  static int opencl_function_counter = 0;
  dyn_string_t dest = function->header;
  dyn_string_append_cstr (dest, "__kernel void");
  dyn_string_append_cstr (dest, " ");
  dyn_string_append_cstr (dest, "opencl_auto_function_");
  opencl_append_int_to_str (dest, opencl_function_counter, "%ld");
  dyn_string_append_cstr (dest, " (");
  sprintf (function->name, "%s%d","opencl_auto_function_",
           opencl_function_counter++);
}

/* Create new instance of opencl_body.  */

static opencl_body
opencl_body_create (void)
{
  opencl_body tmp = XNEW (struct graphite_opencl_kernel_body);

  tmp->body = dyn_string_new (OPENCL_INIT_BUFF_SIZE);
  tmp->pre_header = dyn_string_new (OPENCL_INIT_BUFF_SIZE);
  tmp->header = dyn_string_new (OPENCL_INIT_BUFF_SIZE);
  tmp->non_scalar_args = dyn_string_new (OPENCL_INIT_BUFF_SIZE);

  tmp->num_of_data_writes = 0;
  tmp->function_args = VEC_alloc (tree, heap, OPENCL_INIT_BUFF_SIZE);
  tmp->function_args_to_pass = VEC_alloc (tree, heap, OPENCL_INIT_BUFF_SIZE);
  tmp->data_refs = VEC_alloc (opencl_data, heap, OPENCL_INIT_BUFF_SIZE);
  opencl_create_function_code (tmp);

  return tmp;
}

/* Check whether clast expression EXPT is constant in current loop nest.
   FIRST_SCAT is the iterator of outermost loop in current loop nest.  */

static bool
opencl_constant_expression_p (struct clast_expr *expr, const char *first_scat)
{
  switch (expr->type)
    {
    case expr_term:
      {
	struct clast_term *term = (struct clast_term *) expr;
	if (!(term->var))
	  return true;
	{
	  const char *name = term->var;
	  if (strstr (name, "scat_") != name)
            return true;

          return (opencl_cmp_scat (first_scat, name) == 1);
	}
      }
    case expr_red:
      {
	struct clast_reduction *red = (struct clast_reduction *) expr;
	int i;
	for (i = 0; i < red->n; i++)
          if (!opencl_constant_expression_p (red->elts [i], first_scat))
            return false;

	return true;
      }
    case expr_bin:
      {
	struct clast_binary *bin = (struct clast_binary *) expr;
	return opencl_constant_expression_p (bin->LHS, first_scat);
      }
    default:
      gcc_unreachable ();
      return false;
    }
}

/* Check whether the clast_for LOOP has constant bounds.  FIRST_SCAT
   is the iterator of outermost loop in current loop nest.  */

static bool
opencl_constant_loop_bound_p (struct clast_for *loop, const char *first_scat)
{
  return opencl_constant_expression_p (loop->UB, first_scat)
    && opencl_constant_expression_p (loop->LB, first_scat);
}

/* If clast loop PARENT has only one child and it's a loop too, return
   this child.  Otherwise return NULL.  */

static struct clast_for *
opencl_get_single_loop_child (struct clast_for *parent)
{
  struct clast_stmt *body = parent->body;

  if (body->next
      || !CLAST_STMT_IS_A (body, stmt_for))
    return NULL;

  return (struct clast_for *) body;
}

/* Calculate the maximal depth of a perfect nested loop nest with LOOP
   as outermost loop.  META holds meta information for loop LOOP,
   DEPTH is the depth of LOOP in current loop nest, FIRST_SCAT is the
   iterator of outermost loop in current loop nest.  CODE_GEN holds
   information related to OpenCL code generation.  */

static int
opencl_get_perfect_nested_loop_depth (opencl_main code_gen,
                                      opencl_clast_meta meta,
                                      struct clast_for *loop,
                                      int depth, const char *first_scat)
{
  struct clast_for *child;
  if (dependency_in_clast_loop_p (code_gen, meta, loop, depth))
    return 0;

  child = opencl_get_single_loop_child (loop);

  if (!child
      || !opencl_constant_loop_bound_p (child, first_scat))
    return 1;

  return 1 + opencl_get_perfect_nested_loop_depth (code_gen, meta->body, child,
                                                   depth + 1, first_scat);
}

/* Get the type of the loop iterator for loop, represented by STMT.
   LEVEL is the depth of this loop in current loop nest.  CODE_GEN
   holds information related to OpenCL code generation.  */

static tree
opencl_get_loop_iter_type (struct clast_for *stmt, opencl_main code_gen,
                           int level)
{
  tree lb_type = gcc_type_for_clast_expr (stmt->LB, code_gen->region,
                                          code_gen->newivs,
					  code_gen->newivs_index,
                                          code_gen->params_index);
  tree ub_type = gcc_type_for_clast_expr (stmt->UB, code_gen->region,
                                          code_gen->newivs,
					  code_gen->newivs_index,
                                          code_gen->params_index);
  tree type = gcc_type_for_iv_of_clast_loop (stmt, level, lb_type, ub_type);

  return type;
}

static const char *data_type;

/* Simplified version of C-style type printing from c-aux-info.c.  */

static const char *
gen_type_1 (const char *ret_val, tree t)
{
  switch (TREE_CODE (t))
    {
    case POINTER_TYPE:
      if (TYPE_READONLY (t))
	ret_val = concat ("const ", ret_val, NULL);
      if (TYPE_VOLATILE (t))
	ret_val = concat ("volatile ", ret_val, NULL);

      ret_val = concat ("*", ret_val, NULL);

      if (TREE_CODE (TREE_TYPE (t)) == ARRAY_TYPE
	  || TREE_CODE (TREE_TYPE (t)) == FUNCTION_TYPE)
	ret_val = concat ("(", ret_val, ")", NULL);

      ret_val = gen_type_1 (ret_val, TREE_TYPE (t));

      return ret_val;

    case ARRAY_TYPE:
      if (!COMPLETE_TYPE_P (t) || TREE_CODE (TYPE_SIZE (t)) != INTEGER_CST)
	ret_val = gen_type_1 (concat (ret_val, "[]", NULL), TREE_TYPE (t));
      else if (int_size_in_bytes (t) == 0)
	ret_val = gen_type_1 (concat (ret_val, "[0]", NULL), TREE_TYPE (t));
      else
	{
	  int size = int_size_in_bytes (t) / int_size_in_bytes (TREE_TYPE (t));
	  char buff[10];
	  sprintf (buff, "[%d]", size);
	  ret_val = gen_type_1 (concat (ret_val, buff, NULL), TREE_TYPE (t));
	}
      break;

    case IDENTIFIER_NODE:
      data_type = IDENTIFIER_POINTER (t);
      break;

    case TYPE_DECL:
      data_type = IDENTIFIER_POINTER (DECL_NAME (t));
      break;

    case INTEGER_TYPE:
    case FIXED_POINT_TYPE:
      switch (TYPE_PRECISION (t))
	{
	case 8: data_type = "char"; break;
	case 16: data_type = "short"; break;
	case 32: data_type = "int"; break;
	case 64: data_type = "long"; break;
	default: gcc_unreachable ();
	}
      if (TYPE_UNSIGNED (t))
	data_type = concat ("unsigned ", data_type, NULL);
      break;

    case REAL_TYPE:
      switch (TYPE_PRECISION (t))
	{
	case 32: data_type = "float"; break;
	case 64: data_type = "double"; break;
	default: gcc_unreachable ();
	}
      break;

    case VOID_TYPE:
      data_type = "void";
      break;

    default:
      gcc_unreachable ();
    }

  if (TYPE_READONLY (t))
    ret_val = concat ("const ", ret_val, NULL);
  if (TYPE_VOLATILE (t))
    ret_val = concat ("volatile ", ret_val, NULL);
  if (TYPE_RESTRICT (t))
    ret_val = concat ("restrict ", ret_val, NULL);
  return ret_val;
}

/* Generate a string representation of a declaration of varable named
   NAME with type T.  */

static const char *
gen_type_with_name (const char *name, tree t)
{
  const char *type_part = gen_type_1 (name, t);
  return concat (data_type, " ", type_part, NULL);
}

/* Replace perfect nested loop nest represented by F with opencl kernel.
   For example, loop nest like this

   | for (scat_1 = 0; scat_1 < M; i ++)
   |   for (scat_2 = 0; scat_2 < N; j ++)
   |     for (scat_3 = 0; scat_3 < L; k ++)
   |       stmt (i, j, k)

   will be replased by  kernel, where scat_1, scat_2, scat_3
   depends on thread global id.  Number of threads for this kernel
   will be M * N * L.  DEPTH is the depth of F in current loop nest.
   CODE_GEN holds information related to OpenCL code generation.  BODY holds
   information current OpenCL kernel.  */

static void
opencl_perfect_nested_to_kernel (opencl_main code_gen, struct clast_for *f,
				 opencl_body body, int depth)
{
  VEC (tree, heap) *mod = VEC_alloc (tree, heap, OPENCL_INIT_BUFF_SIZE);

  VEC (tree, heap) *function_args = body->function_args;
  const int perfect_depth
    = opencl_get_perfect_nested_loop_depth (code_gen, code_gen->curr_meta, f,
					    depth, f->iterator);
  struct clast_for *curr = f;
  int counter = perfect_depth;
  tree curr_base = integer_one_node;
  basic_block calc_block = opencl_create_bb (code_gen);
  opencl_append_string_to_body
    ("size_t opencl_global_id = get_global_id (0);\n", code_gen);

  body->first_iter = curr->iterator;

  /* Iterate through all loops, which form perfect loop nest.  */
  while (counter--)
    {
      tree iv;
      sese region = code_gen->region;
      VEC (tree, heap) *newivs = code_gen->newivs;
      htab_t newivs_index = code_gen->newivs_index;
      htab_t params_index = code_gen->params_index;
      const char *tmp;
      const char *decl;

      tree type = opencl_get_loop_iter_type (curr, code_gen, depth);

      const char *type_str = gen_type_with_name (" ", type);

      tree low_bound = clast_to_gcc_expression (type, curr->LB, region,
                                                newivs, newivs_index,
                                                params_index);

      tree up_bound = clast_to_gcc_expression (type, curr->UB, region,
                                               newivs, newivs_index,
                                               params_index);
      long stride = 1;
      tree t_stride;
      tree curr_loop_size;
      int num = perfect_depth - counter - 1;

      gcc_assert (curr->LB);
      gcc_assert (curr->UB);

      body->last_iter = curr->iterator;

      low_bound = opencl_tree_to_var (calc_block, low_bound);
      up_bound = opencl_tree_to_var (calc_block, up_bound);

      if (mpz_cmp_si (curr->stride, 1) > 0)
	stride = mpz_get_si (curr->stride);

      t_stride = build_int_cst (NULL_TREE, stride);

      curr_loop_size = build2 (MINUS_EXPR, type,
			       up_bound, low_bound);

      curr_loop_size = build2 (PLUS_EXPR, type,
			       curr_loop_size,
			       fold_convert (type, integer_one_node));

      if (stride != 1)
	curr_loop_size
	  = build2 (TRUNC_DIV_EXPR, type,
		    curr_loop_size, t_stride);

      curr_loop_size = opencl_tree_to_var (calc_block, curr_loop_size);

      VEC_safe_push (tree, heap, mod, curr_loop_size);

      iv = create_tmp_var (type, "scat_tmp_iter");

      /* Declare loop iterator as local variable.  */
      opencl_append_string_to_body (type_str, code_gen);
      opencl_append_string_to_body (curr->iterator, code_gen);

      /* Calculate the value of the iterator of current loop based of the
	 number of current thread and numbers of iterators of inner loops.  */
      opencl_append_string_to_body (" = ((opencl_global_id / ", code_gen);
      opencl_append_num_to_body (code_gen, num, "opencl_base_%d");
      opencl_append_string_to_body (") % ", code_gen);
      opencl_append_num_to_body (code_gen, num, "opencl_mod_%d");
      opencl_append_string_to_body (") * ", code_gen);
      opencl_append_num_to_body (code_gen, stride, "%d");
      opencl_append_num_to_body (code_gen, num, " + opencl_first_%d;\n");

      opencl_append_string_to_header (type_str, code_gen);
      opencl_append_num_to_header (code_gen, num, "opencl_mod_%d, ");
      opencl_append_string_to_header (type_str, code_gen);
      opencl_append_num_to_header (code_gen, num, "opencl_first_%d, ");

      /* Declare old loop iterator.  */
      tmp = opencl_get_var_name (iv);
      check_and_mark_arg (code_gen, tmp, false);
      decl = opencl_print_function_arg_with_type (tmp, type);
      opencl_append_string_to_body (decl, code_gen);
      opencl_append_string_to_body (" = ", code_gen);
      opencl_append_string_to_body (curr->iterator, code_gen);
      opencl_append_string_to_body (";\n", code_gen);

      save_clast_name_index (code_gen->newivs_index, curr->iterator,
                             VEC_length (tree, code_gen->newivs));
      VEC_safe_push (tree, heap, code_gen->newivs, iv);

      /* Save number of iterations for loop.  */
      VEC_safe_push (tree, heap, function_args, curr_loop_size);
      VEC_safe_push (tree, heap, function_args, low_bound);

      body->clast_body = curr->body;
      curr = opencl_get_single_loop_child (curr);
      depth ++;
    }

  counter = perfect_depth;

  /* Store number of iteration of inner loops for each loop in perfect
     nest.  */
  while (counter --)
    {
      tree type = TREE_TYPE (curr_base);
      const char *type_str = gen_type_with_name (" ", type);
      tree curr = VEC_index (tree, mod, counter);

      opencl_append_string_to_header (type_str, code_gen);
      opencl_append_num_to_header (code_gen, counter, "opencl_base_%d, ");

      VEC_safe_push (tree, heap, function_args, curr_base);

      curr_base = build2 (MULT_EXPR, type, curr_base,
                          build1 (CONVERT_EXPR, type, curr));
      curr_base = opencl_tree_to_var (calc_block, curr_base);
    }

  body->num_of_exec = fold_convert (integer_type_node, curr_base);
  VEC_free (tree, heap, mod);
}

/* Generate code for loop statement F.  DEPTH is the depth of F in
   current loop nest.  CODE_GEN holds information related to OpenCL
   code generation.  */

static opencl_body
opencl_print_loop (struct clast_for *f, opencl_main code_gen, int depth)
{
  opencl_body current_body = code_gen->current_body;

  code_gen->global_defined_vars
    = htab_create (10, htab_hash_string, opencl_cmp_str, NULL);

  opencl_perfect_nested_to_kernel (code_gen, f, current_body, depth);

  /* Define local loop iterators.  */
  opencl_print_local_vars (current_body->first_iter,
			   current_body->last_iter,
			   "unsigned int", code_gen);

  /* Generate code for kernel body.  */
  opencl_print_stmt_list (current_body->clast_body, code_gen, depth + 1);
  opencl_append_string_to_body ("}\n", code_gen);

  if (current_body->num_of_data_writes)
    {
      dyn_string_t header = current_body->header;
      int offset;

      dyn_string_append (header, current_body->non_scalar_args);
      offset = dyn_string_length (header) - 2;

      if (*(dyn_string_buf (header) + offset) == ',')
        *(dyn_string_buf (header) + offset) = ' ';

      opencl_append_string_to_header (")\n{\n", code_gen);
    }

  return current_body;
}

/* Generate OpenCL code for clast_assignment A.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_print_assignment (struct clast_assignment *a, opencl_main code_gen)
{
  /* Real assignment.  */
  if (a->LHS)
    {
      opencl_append_string_to_body (a->LHS, code_gen);
      opencl_append_string_to_body (" = ", code_gen);
    }

  /* Just expression.  */
  opencl_print_expr (a->RHS, code_gen);
}

/* Return tree with variable, corresponging to given clast name NAME.
   CODE_GEN holds information related to OpenCL code generation.  */

static tree
opencl_clast_name_to_tree (opencl_main code_gen, const char *name)
{
  return clast_name_to_gcc (name, code_gen->region, code_gen->newivs,
                            code_gen->newivs_index, code_gen->params_index);
}

/* For a given clast name return that name, if it's local name in
   kernel body or, otherwise, name of gimple variable created for this
   scat_i in gimple.  CODE_GEN holds information related to OpenCL
   code generation.  */

static const char *
opencl_get_scat_real_name (opencl_main code_gen, const char *name)
{
  /* NAME > FIRST_ITER */
  if (opencl_cmp_scat (name, code_gen->current_body->first_iter) >= 0)
    return name;

  return
    opencl_get_var_name (opencl_clast_name_to_tree (code_gen, name));
}

/* Add clast variable (scat_i) as kernel argument.  NAME is a new name
   of loop iterator (scat_*), REAL_NAME is an old (origin) name of
   loop iterator.  CODE_GEN holds information related to OpenCL code
   generation.  */

static void
opencl_add_scat_as_arg (opencl_main code_gen, const char *name,
			const char *real_name)
{
  tree var;
  if (!check_and_mark_arg (code_gen, real_name, false))
    return;
  var = opencl_clast_name_to_tree (code_gen, name);
  opencl_add_function_arg (code_gen, var, real_name);
}

/* Generate OpenCL code for user statement U.  Code will be generated
   from basic block, related to U.  Also induction variables mapping
   to old variables must be calculated to process basic block.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_print_user_stmt (struct clast_user_stmt *u, opencl_main code_gen)
{
  CloogStatement * cs;
  poly_bb_p pbb;
  gimple_bb_p gbbp;
  basic_block bb;
  int i;
  int nb_loops = number_of_loops ();
  code_gen->iv_map = VEC_alloc (tree, heap, nb_loops);

  for (i = 0; i < nb_loops; i++)
    VEC_safe_push (tree, heap, code_gen->iv_map, NULL_TREE);
  build_iv_mapping (code_gen->iv_map, code_gen->region,
                    code_gen->newivs,
                    code_gen->newivs_index, u,
                    code_gen->params_index);

  code_gen->defined_vars
    = htab_create (10, htab_hash_string, opencl_cmp_str, NULL);
  opencl_append_string_to_body ("{\n", code_gen);

  cs = u->statement;
  pbb = (poly_bb_p) cloog_statement_usr (cs);
  gbbp = PBB_BLACK_BOX (pbb);
  bb = GBB_BB (gbbp);
  code_gen->context_loop = bb->loop_father;

  opencl_add_data_refs_pbb (pbb, code_gen);
  opencl_print_bb (bb, code_gen);
  opencl_append_string_to_body ("}\n", code_gen);
  htab_delete (code_gen->defined_vars);
  code_gen->defined_vars = NULL;
  VEC_free (tree, heap, code_gen->iv_map);
}

/* If tree node NODE defined in current sese build and insert define
   statements for it, otherwise mark node as external (parameter for
   kernel).  If tree defined in current sese, also recursively build
   defines for all trees in definition expression.  */

static void
opencl_build_defines (tree node, opencl_main code_gen)
{
  enum tree_code code = TREE_CODE (node);
  switch (code)
    {
    case SSA_NAME:
      {
	const char *tmp = opencl_get_var_name (node);
	gimple def_stmt;

	/* If name defined in other sese it is kernel's parameter.  */
	if (!defined_in_sese_p (node, code_gen->region))
          return;

	/*  Bail out if this name was defined earlier either in this
            or other region.  */
        if (*(const char **)htab_find_slot (code_gen->defined_vars,
                                            tmp, INSERT))
          return;

        /* Get definition statement.  */
	def_stmt = SSA_NAME_DEF_STMT (node);
	opencl_expand_scalar_vars (code_gen, def_stmt);
	opencl_print_gimple (def_stmt, code_gen);
	return;
      }
    case ARRAY_REF:
      {
	tree arr = TREE_OPERAND (node, 0);
	tree offset = TREE_OPERAND (node, 1);
	opencl_build_defines (arr, code_gen);
	opencl_build_defines (offset, code_gen);
	return;
      }
    default:
      gcc_unreachable ();
    }
}

/* For a given gimple statement STMT build definition for all names,
   used in this stament.  If name has been defined in other sese, mark
   it as kernel parameter.  CODE_GEN holds information related to
   OpenCL code generation.  */

static void
opencl_expand_scalar_vars (opencl_main code_gen, gimple stmt)
{
  ssa_op_iter iter;
  use_operand_p use_p;
  FOR_EACH_SSA_USE_OPERAND (use_p, stmt, iter, SSA_OP_ALL_USES)
    {
      tree use = USE_FROM_PTR (use_p);
      if (!is_gimple_reg (use))
	continue;
      opencl_build_defines (use, code_gen);
    }
}

/* Generate code for a single basic block BB.  CODE_GEN holds
   information related to OpenCL code generation.  */

static void
opencl_print_bb (basic_block bb, opencl_main code_gen)
{
  gimple_stmt_iterator gsi;
  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);
      opencl_expand_scalar_vars (code_gen, stmt);
      opencl_print_gimple (stmt, code_gen);
    }
}

/* Print operation simbol (`+' `-' `*') for assignment operation GMA.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_print_gimple_assign_operation (gimple gmp, opencl_main code_gen)
{
  opencl_append_string_to_body
    (op_symbol_code (gimple_assign_rhs_code (gmp)), code_gen);
}

/* Print pointer expression represented by EXPR.  TYPE_SIZE represents
   size of the base type for EXPR.  CODE_GEN holds information related
   to OpenCL code generation.  */

static void
opencl_print_addr_operand (tree expr, tree type_size, opencl_main code_gen)
{
  if (TREE_CODE (TREE_TYPE (expr)) != POINTER_TYPE)
    {
      opencl_append_string_to_body ("(", code_gen);
      opencl_print_operand (expr, false, code_gen);
      opencl_append_string_to_body ("/", code_gen);
      opencl_print_operand (type_size, false, code_gen);
      opencl_append_string_to_body (")", code_gen);
    }
  else
    opencl_print_operand (expr, false, code_gen);

}

/* Print unary gimple operation GMP.  CODE_GEN holds information
   related to OpenCL code generation.  */

static void
opencl_print_unary (gimple gmp, opencl_main code_gen)
{
  switch (gimple_assign_rhs_code (gmp))
    {
    case BIT_NOT_EXPR:
      opencl_append_string_to_body ("~", code_gen);
      return;
    case TRUTH_NOT_EXPR:
      opencl_append_string_to_body ("!", code_gen);
      return;
    case NEGATE_EXPR:
      opencl_append_string_to_body ("-", code_gen);
      return;
    case MODIFY_EXPR:
    default:
      return;
    }
}

/* Generate code for min or max gimple operand GMP.  CODE_GEN holds
   information related to OpenCL code generation.  */

static void
opencl_print_max_min_assign (gimple gmp, opencl_main code_gen)
{
  tree lhs = gimple_assign_lhs (gmp);
  tree rhs1 = gimple_assign_rhs1 (gmp);
  tree rhs2 = gimple_assign_rhs2 (gmp);
  bool max = gimple_assign_rhs_code (gmp) == MAX_EXPR;

  opencl_print_operand (lhs, true, code_gen);
  opencl_append_string_to_body (max?" = fmax (":"= fmin (", code_gen);
  opencl_print_operand (rhs1, false, code_gen);
  opencl_append_string_to_body (",", code_gen);
  opencl_print_operand (rhs2, false, code_gen);
  opencl_append_string_to_body (");\n", code_gen);

}

/* Generate code for gimple assignment statement GMP.  CODE_GEN holds
   information related to OpenCL code generation.  */

static void
opencl_print_gimple_assign (gimple gmp, opencl_main code_gen)
{
  int num_of_ops = gimple_num_ops (gmp);
  tree lhs;
  tree rhs1;
  tree rhs2;
  bool addr_expr;
  int result;
  tree result_size = NULL;

  if (gimple_assign_rhs_code (gmp) == MAX_EXPR
      || gimple_assign_rhs_code (gmp) == MIN_EXPR)
    {
      opencl_print_max_min_assign (gmp, code_gen);
      return;
    }
  gcc_assert (num_of_ops == 2 || num_of_ops == 3);
  lhs = gimple_assign_lhs (gmp);

  addr_expr = (TREE_CODE (TREE_TYPE (lhs)) == POINTER_TYPE);
  if (addr_expr)
    result_size = TYPE_SIZE_UNIT (TREE_TYPE (TREE_TYPE (lhs)));

  rhs1 = gimple_assign_rhs1 (gmp);
  rhs2 = gimple_assign_rhs2 (gmp);
  result = opencl_print_operand (lhs, true, code_gen);
  if (result != 0)
    return;
  opencl_append_string_to_body (" = ", code_gen);

  if (addr_expr)
    opencl_print_addr_operand (rhs1, result_size, code_gen);
  else
    {
      if (rhs2 == NULL)
        opencl_print_unary (gmp, code_gen);
      opencl_print_operand (rhs1, false, code_gen);
    }
  if (rhs2 != NULL_TREE)
    {
      opencl_print_gimple_assign_operation (gmp, code_gen);
      if (addr_expr)
	opencl_print_addr_operand (rhs2, result_size, code_gen);
      else
	opencl_print_operand (rhs2, false, code_gen);
    }
  opencl_append_string_to_body (";\n",code_gen);
}

/* Generate code for arguments for gimple call statement GMP.
   CODE_GEN hold information related to OpenCL code generation.  */

static void
opencl_print_gimple_call_args (opencl_main code_gen, gimple gmp)
{
  size_t len = gimple_call_num_args (gmp);
  size_t i;
  opencl_append_string_to_body (" (",code_gen);
  for (i = 0; i < len; i++)
    {
      opencl_print_operand (gimple_call_arg (gmp, i), false, code_gen);
      if (i < len - 1)
	opencl_append_string_to_body (", ",code_gen);
    }
  opencl_append_string_to_body (")",code_gen);
}

/* Replace some function names.  */

static const char *
opencl_get_function_name (tree function)
{
  const char *gimple_name = IDENTIFIER_POINTER (DECL_NAME (function));
  if (!strcmp (gimple_name, "__builtin_powf"))
    return "pow";
  return gimple_name;
}

/* Generate code for gimple call statement GMP.  CODE_GEN holds information
   related to OpenCL code generation.  */

static void
opencl_print_gimple_call (opencl_main code_gen, gimple gmp)
{
  tree lhs = gimple_call_lhs (gmp);
  tree function = gimple_call_fn (gmp);
  opencl_print_operand (lhs, true, code_gen);
  opencl_append_string_to_body (" = ", code_gen);

  while (TREE_CODE (function) == ADDR_EXPR
	 || TREE_CODE (function) == INDIRECT_REF)
    function = TREE_OPERAND (function, 0);
  opencl_append_string_to_body (opencl_get_function_name (function), code_gen);
  opencl_print_gimple_call_args (code_gen, gmp);
  opencl_append_string_to_body (";\n",code_gen);
}

/* Generate code for gimple statment SMP.  Now only assignment
   operation are supported, but it seems enough for clast translation.
   GIMPLE_COND statements are loop bound conditions and can be safely
   ignored.  CODE_GEN holds information related to OpenCL code
   generation.  */

static void
opencl_print_gimple (gimple gmp, opencl_main code_gen)
{
  if (!gmp)
    return;

  switch (gimple_code (gmp))
    {
    case GIMPLE_ASSIGN:
      opencl_print_gimple_assign (gmp, code_gen);
      break;
    case GIMPLE_COND:
      break;
    case GIMPLE_PHI:
      break;
    case GIMPLE_CALL:
      opencl_print_gimple_call (code_gen, gmp);
      break;
    case GIMPLE_DEBUG:
      break;
    case GIMPLE_LABEL:
      {
	tree label = gimple_label_label (gmp);
	opencl_print_operand (label, false, code_gen);
	opencl_append_string_to_body (": ", code_gen);
      }
      break;
    default:
      debug_gimple_stmt (gmp);
      gcc_unreachable ();
    }
}

/* Get name of the variable, represented by tree NODE.  If variable is
   temporary, generate name for it.  */

static const char *
opencl_get_var_name (tree node)
{
  bool ssa_name = TREE_CODE (node) == SSA_NAME;
  tree name;
  int num = 0;
  if (ssa_name)
    {
      num = SSA_NAME_VERSION (node);
      node = SSA_NAME_VAR (node);
    }
  name = DECL_NAME (node);
  if (name)
    {
      if (!ssa_name)
	return identifier_to_locale (IDENTIFIER_POINTER (name));
      else
	{
	  const char *base = identifier_to_locale (IDENTIFIER_POINTER (name));
	  char *buff = XNEWVEC (char, strlen (base) + 5);
	  sprintf (buff, "%s_%d", base, num);
	  return buff;
	}
    }
  else
    {
      int tmp_var_uid = DECL_UID (node);
      char *tmp = XNEWVEC (char, 30);
      sprintf (tmp, "opencl_var_%d_%d", tmp_var_uid, num);
      return tmp;
    }
}

/* Append variable name NAME to function body.  Differs from appending
   string by replacing `.' by `_'. CODE_GEN holds information related
   to OpenCL code generation.  */

static void
opencl_append_var_name (const char *name, opencl_main code_gen)
{
  int len = strlen (name);
  char *tmp = XNEWVEC (char, len + 1);
  int i;
  for (i = 0; i <= len; i++)
    {
      char tt = name[i];
      if (tt == '.')
	tt = '_';
      tmp[i] = tt;
    }
  opencl_append_string_to_body (tmp, code_gen);
  free (tmp);
}

/* If variable VAR_DECL is not defined and it is not marked as a
   parameter, mark it as a parameter and add it to parameters list.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_try_variable (opencl_main code_gen, tree var_decl)
{
  const char *name = opencl_get_var_name (var_decl);
  gcc_assert (code_gen->defined_vars);

  if (check_and_mark_arg (code_gen, name, false))
    opencl_add_function_arg (code_gen, var_decl, name);
}

/* Define non scalar variable, represented be DATA as either local
   variable or kernel argument.  CODE_GEN holds information related to
   OpenCL code generation.  */

static void
opencl_add_non_scalar_function_arg (opencl_main code_gen,
                                    opencl_data data)
{
  const char *decl;
  static int counter = 0;
  opencl_body body = code_gen->current_body;
  tree var = data->exact_object;
  const char *name = opencl_get_var_name (var);
  tree type = TREE_TYPE (var);

  /* Check whether given variable can be privatized.  */
  if (data->privatized)
    {
      /* Define variable as local variable.  */
      gcc_assert (TREE_CODE (type) == ARRAY_TYPE);
      decl = opencl_print_function_arg_with_type (name, type);
      dyn_string_append_cstr (body->pre_header, decl);
      dyn_string_append_cstr (body->pre_header, ";\n");
      return;
    }
  else
    {
      /* Define variable as kernel argument.  */
      char decl_name [30];
      tree main_type = opencl_get_main_type (type);
      sprintf (decl_name, "oclFTmpArg%d", counter++);
      decl = opencl_print_function_arg_with_type (decl_name, main_type);
      dyn_string_append_cstr (body->non_scalar_args, "__global ");
      opencl_add_non_scalar_type_decl (var, body->pre_header, decl_name);
      dyn_string_append_cstr (body->non_scalar_args, decl);
      dyn_string_append_cstr (body->non_scalar_args, ", ");
      VEC_safe_push (opencl_data, heap, body->data_refs, data);
    }
}

/* Register data reference REF to variable DATA.  Do nothing, if it
   has already been registered.  CODE_GEN holds information related to
   OpenCL code generation.  */

static void
opencl_try_data_ref (opencl_main code_gen, data_reference_p ref,
                     opencl_data data)
{
  tree var = dr_outermost_base_object (ref);
  const char *name = opencl_get_var_name (var);
  const char ** slot;
  gcc_assert (code_gen->defined_vars);

  slot = (const char **)htab_find_slot (code_gen->global_defined_vars,
                                        name, INSERT);
  if (*slot)
    return;
  *slot = name;
  opencl_add_non_scalar_function_arg (code_gen, data);
}

/* Register data reference D_REF in current kernel.  CODE_GEN hold
   information related to OpenCL code generation.  */

static void
opencl_add_data_ref (opencl_main code_gen, data_reference_p d_ref)
{
  opencl_data tmp = opencl_get_data_by_data_ref (code_gen, d_ref);

  gcc_assert (tmp);
  if (!DR_IS_READ (d_ref))
    {
      bitmap_set_bit (code_gen->curr_meta->modified_on_device, tmp->id);
      tmp->written_in_current_body = true;
      tmp->ever_written_on_device = true;
      code_gen->current_body->num_of_data_writes ++;
    }
  else
    {
      tmp->read_in_current_body = true;
      tmp->ever_read_on_device = true;
    }
  if (!tmp->privatized)
    tmp->used_on_device = true;

  opencl_try_data_ref (code_gen, d_ref, tmp);
}

/* Add base objects of all data references in PBB as arguments to
   current kernel.  CODE_GEN holds information related to OpenCL code
   generation.  */

static void
opencl_add_data_refs_pbb (poly_bb_p pbb, opencl_main code_gen)
{
  VEC (poly_dr_p, heap) *drs = PBB_DRS (pbb);
  int i;
  poly_dr_p curr;

  for (i = 0; VEC_iterate (poly_dr_p, drs, i, curr); i++)
    {
      data_reference_p d_ref = (data_reference_p) PDR_CDR (curr);
      opencl_add_data_ref (code_gen, d_ref);
    }
}

/* Generate operand for tree node NODE.  If LSH is true, generated
   operand must be lvalue, otherwise it's rvalue.  CODE_GEN holds
   information related to OpenCL code generation.  Also generate
   definitions for variables if necessary.  Variable definition is not
   necessary if variable has already been defined or if it has been
   defined in other sese.  */

static int
opencl_print_operand (tree node, bool lhs, opencl_main code_gen)
{
  tree scev = scalar_evolution_in_region (code_gen->region,
                                          code_gen->context_loop,
                                          node);
  tree new_node = chrec_apply_map (scev, code_gen->iv_map);

  if (TREE_CODE (new_node) != SCEV_NOT_KNOWN)
    node = new_node;

  switch (TREE_CODE (node))
    {
    case NOP_EXPR:
      return opencl_print_operand (TREE_OPERAND (node, 0), false, code_gen);
    case PLUS_EXPR:
      {
        if (lhs)
          return -1;
        opencl_append_string_to_body ("(", code_gen);
        opencl_print_operand (TREE_OPERAND (node, 0), false, code_gen);
        opencl_append_string_to_body (" + ", code_gen);
        opencl_print_operand (TREE_OPERAND (node, 1), false, code_gen);
        opencl_append_string_to_body (")", code_gen);
        return 0;
      }
    case MULT_EXPR:
      {
        if (lhs)
          return -1;
        opencl_append_string_to_body ("(", code_gen);
        opencl_print_operand (TREE_OPERAND (node, 0), false, code_gen);
        opencl_append_string_to_body (" * ", code_gen);
        opencl_print_operand (TREE_OPERAND (node, 1), false, code_gen);
        opencl_append_string_to_body (")", code_gen);
        return 0;
      }

    case SSA_NAME:
      {
	/* If rhs just add variable name.  Otherwise
           it may be necessary to add variable definition.  */
	const char *tmp = opencl_get_var_name (node);
	if (lhs)
          opencl_add_variable (tmp, node, code_gen);
	else
          opencl_append_var_name (tmp, code_gen);

	/* This call adds variable declaration as formal
	   parameter in kernel header if it is necessary.  */
	opencl_try_variable (code_gen, node);
	return 0;
      }
    case ARRAY_REF:
      {
	/* <operand>[<operand>].  */
	tree arr = TREE_OPERAND (node, 0);
	tree offset = TREE_OPERAND (node, 1);
	opencl_print_operand (arr, false, code_gen);

        opencl_append_string_to_body ("[", code_gen);
        opencl_print_operand (offset, false, code_gen);
        opencl_append_string_to_body ("]", code_gen);
	return 0;
      }
    case INTEGER_CST:
      {
	/* Just print integer constant.  */
	unsigned HOST_WIDE_INT low = TREE_INT_CST_LOW (node);
        if (lhs)
          return -1;
	if (host_integerp (node, 0))
          opencl_append_num_to_body (code_gen, (long)low, "%ld");
	else
	  {
	    HOST_WIDE_INT high = TREE_INT_CST_HIGH (node);
	    char buff[100];
	    buff[0] = ' ';
	    if (tree_int_cst_sgn (node) < 0)
	      {
		buff[0] = '-';
		high = ~high + !low;
		low = -low;
	      }
	    sprintf (buff + 1, HOST_WIDE_INT_PRINT_DOUBLE_HEX,
		     (unsigned HOST_WIDE_INT) high, low);
	    opencl_append_string_to_body (buff, code_gen);
	  }
	return 0;
      }
    case REAL_CST:
      {
	char buff[100];
	REAL_VALUE_TYPE tmp = TREE_REAL_CST (node);
        if (lhs)
          return -1;
	real_to_decimal (buff, &tmp, sizeof (buff), 0, 1);
	opencl_append_string_to_body (buff, code_gen);
	return 0;
      }
    case FIXED_CST:
      {
	char buff[100];
        if (lhs)
          return -1;
	fixed_to_decimal (buff, TREE_FIXED_CST_PTR (node), sizeof (buff));
	opencl_append_string_to_body (buff, code_gen);
	return 0;
      }
    case STRING_CST:
      {
	opencl_append_string_to_body ("\"", code_gen);
	opencl_append_string_to_body (TREE_STRING_POINTER (node), code_gen);
	opencl_append_string_to_body ("\"", code_gen);
	return 0;
      }
    case VAR_DECL:
    case PARM_DECL:
      {
	tree decl_name = DECL_NAME (node);
	const char *tmp;
	gcc_assert (decl_name);
	tmp = IDENTIFIER_POINTER (decl_name);

	opencl_append_var_name (tmp, code_gen);
	opencl_try_variable (code_gen, node);
	return 0;
      }
    case FIELD_DECL:
      {
	tree decl_name = DECL_NAME (node);
	const char *tmp;
	gcc_assert (decl_name);
	tmp = IDENTIFIER_POINTER (decl_name);
	opencl_append_var_name (tmp, code_gen);
        return 0;
      }
    case LABEL_DECL:
      {
	tree decl_name = DECL_NAME (node);
	if (decl_name)
	  {
	    const char *tmp = IDENTIFIER_POINTER (decl_name);
	    opencl_append_var_name (tmp, code_gen);
	    return 0;
	  }

	if (LABEL_DECL_UID (node) != -1)
	  {
	    opencl_append_num_to_body (code_gen, (int) LABEL_DECL_UID (node),
				       "L%d");
	    return 0;
	  }
	opencl_append_num_to_body (code_gen, (int) DECL_UID (node),
				   "D_%u");
	return 0;
      }
    case INDIRECT_REF:
      {
	opencl_append_string_to_body ("(*", code_gen);
	opencl_print_operand (TREE_OPERAND (node, 0), false, code_gen);
	opencl_append_string_to_body (")", code_gen);
	return 0;
      }
    case ADDR_EXPR:
      {
	opencl_append_string_to_body ("&", code_gen);
	opencl_print_operand (TREE_OPERAND (node, 0), false, code_gen);
	return 0;
      }
    case COMPONENT_REF:
      {
	tree op1 = TREE_OPERAND (node, 0);
	tree op2 = TREE_OPERAND (node, 1);
	opencl_print_operand (op1, false, code_gen);

	if (op1 && TREE_CODE (op1) == INDIRECT_REF)
	  opencl_append_string_to_body ("->", code_gen);
	else
	  opencl_append_string_to_body (".", code_gen);

	opencl_print_operand (op2, false, code_gen);
	return 0;
      }
    default:
      debug_tree (node);
      gcc_unreachable ();
    }

  return 0;
}

/* Append variable VAR with name VAR_NAME to current function body.
   If variable has been defined in current scope, but definition for
   it has not been generated - then generate it's definition and mark
   variable as defined.  CODE_GEN holds information related to OpenCL
   code generation.  */

static void
opencl_add_variable (const char *var_name, tree var, opencl_main code_gen)
{
  const char ** slot;
  if (htab_find (code_gen->global_defined_vars, var_name))
    {
      opencl_append_var_name (var_name, code_gen);
      return;
    }

  slot = (const char **) htab_find_slot
    (code_gen->defined_vars, var_name, INSERT);

  if (! (*slot) && defined_in_sese_p (var, code_gen->region))
    {
      const char *decl;
      tree type = TREE_TYPE (var);
      *slot = var_name;
      if (TREE_CODE (type) == POINTER_TYPE
          || TREE_CODE (type) == ARRAY_TYPE)
        {
          opencl_add_non_scalar_type_decl (var, code_gen->current_body->body,
                                           NULL);
        }
      else
        {
          var = SSA_NAME_VAR (var);
          decl = opencl_print_function_arg_with_type (var_name, type);
          opencl_append_string_to_body (decl, code_gen);
        }
      return;
    }
  opencl_append_var_name (var_name, code_gen);
}

/* Append list of names of loop iterators from CODE_GEN with same type
   TYPE to current kernel.  FIRST and LAST define outermost and
   innermost iterators to append respectively.  */

static void
opencl_print_local_vars (const char *fist, const char *last,
			 const char *type, opencl_main code_gen)
{
  char ** names = code_gen->root_names->_scattering;
  int len = code_gen->root_names->_nb_scattering;
  int i;
  for (i = 0; i < len; i++)
    {
      const char *tmp = names[i];
      if (opencl_cmp_scat (fist, tmp) <= 0
	  && opencl_cmp_scat (last, tmp) >= 0)
	{
	  const char ** slot =
	    (const char **) htab_find_slot (code_gen->global_defined_vars,
					    tmp, INSERT);
	  *slot = tmp;
	  continue;
	}

      if (opencl_cmp_scat (fist, tmp) > 0)
	continue;

      opencl_append_string_to_body (type, code_gen);
      opencl_append_string_to_body (" ", code_gen);
      opencl_append_string_to_body (tmp, code_gen);
      opencl_append_string_to_body (";\n", code_gen);
      *((const char **)htab_find_slot (code_gen->global_defined_vars,
                                       tmp, INSERT)) = tmp;
    }
}

/*  Replace all dots to underscores in string pointed to by P.  Return P.  */

static char *
filter_dots (char *p)
{
  char *s;
  for (s = p; *s; s++)
    if (*s == '.')
      *s = '_';
  return p;
}

/* Return string with varibale definition.  ARG_NAME is the name of
   the variable and TYPE is it's type.  */

static const char *
opencl_print_function_arg_with_type (const char *arg_name, tree type)
{
  const char *decl = gen_type_with_name (arg_name, type);
  char *ddecl;
  ddecl = xstrdup (decl);
  return filter_dots (ddecl);
}

/* Generate definition for non scalar variable VAR and place it to
   string DEST.  Use DECL_NAME as variable name.  */

static void
opencl_add_non_scalar_type_decl (tree var, dyn_string_t dest,
                                 const char *decl_name)
{
  tree type = TREE_TYPE (var);
  const char *name = opencl_get_var_name (var);
  static int counter = 0;
  char type_name [30];
  char *tmp_name = xstrdup (name);
  const char *new_type;
  tree inner_type = TREE_TYPE (type);

  filter_dots (tmp_name);

  sprintf (type_name, "oclFTmpType%d", counter++);

  new_type = opencl_print_function_arg_with_type (type_name, inner_type);

  dyn_string_append_cstr (dest, "typedef __global ");
  dyn_string_append_cstr (dest, new_type);
  dyn_string_append_cstr (dest, ";\n");

  dyn_string_append_cstr (dest, type_name);
  dyn_string_append_cstr (dest, " *");
  dyn_string_append_cstr (dest, tmp_name);
  if (decl_name != NULL)
    {
      dyn_string_append_cstr (dest, " = (");
      dyn_string_append_cstr (dest, type_name);
      dyn_string_append_cstr (dest, "*)");
      dyn_string_append_cstr (dest, decl_name);
      dyn_string_append_cstr (dest, ";\n");
    }
  free (tmp_name);

}

/* Check whether variable with name NAME has been defined as global or
   local variable and mark it as defined.  This function returns false
   if variable has already been defined, otherwise it returns true.  */

static bool
check_and_mark_arg (opencl_main code_gen, const char *name, bool local)
{
  const char ** slot;
  gcc_assert (code_gen->defined_vars || !local);
  if (code_gen->defined_vars)
    {
      slot = (const char **)htab_find_slot (code_gen->defined_vars,
                                            name, INSERT);
      if (*slot)
        return false;
      if (local)
        *slot = name;
    }

  slot = (const char **)htab_find_slot (code_gen->global_defined_vars,
                                        name, INSERT);
  if (*slot)
    return false;
  if (!local)
    *slot = name;
  return true;
}

/* Add variable VAR with name NAME as function argument.  Append it's
   declaration in finction header and add it as function parameter.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_add_function_arg (opencl_main code_gen, tree var, const char *name)
{
  opencl_body body;
  const char *decl;
  tree type;
  type = TREE_TYPE (var);
  body = code_gen->current_body;
  decl = opencl_print_function_arg_with_type (name, type);
  dyn_string_append_cstr (body->header, decl);
  dyn_string_append_cstr (body->header, ", ");
  VEC_safe_push (tree, heap, body->function_args, var);
}

/* Generate kernel function code for clast for statement F, located on
   depth DEPTH.  CODE_GEN holds information related to OpenCL code
   generation.  */

opencl_body
opencl_clast_to_kernel (struct clast_for * f, opencl_main code_gen,
                        int depth)
{
  opencl_body tmp = opencl_body_create ();
  code_gen->current_body = tmp;
  return opencl_print_loop (f, code_gen, depth);
}

/* Generate code for clast statement S, located on depth DEPTH.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_print_stmt_list (struct clast_stmt *s, opencl_main code_gen, int depth)
{
  for ( ; s; s = s->next) {
    gcc_assert (!CLAST_STMT_IS_A (s, stmt_root));
    if (CLAST_STMT_IS_A (s, stmt_ass))
      {
	opencl_print_assignment ((struct clast_assignment *) s, code_gen);
	opencl_append_string_to_body (";\n", code_gen);
      }
    else if (CLAST_STMT_IS_A (s, stmt_user))
      opencl_print_user_stmt ((struct clast_user_stmt *) s, code_gen);
    else if (CLAST_STMT_IS_A (s, stmt_for))
      opencl_print_for ((struct clast_for *) s, code_gen, depth);
    else if (CLAST_STMT_IS_A (s, stmt_guard))
      opencl_print_guard ((struct clast_guard *) s, code_gen, depth);
    else if (CLAST_STMT_IS_A (s, stmt_block))
      {
	opencl_append_string_to_body ("{\n", code_gen);
	opencl_print_stmt_list (((struct clast_block *)s)->body, code_gen,
				depth);
	opencl_append_string_to_body ("}\n", code_gen);
      }
    else
      gcc_unreachable ();
  }
}

/* Generate code for clast for statement F, locate on depth LEVEL.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_print_for (struct clast_for *f, opencl_main code_gen, int level)
{
  tree iv;
  tree iv_type;
  const char *tmp;
  const char *decl;
  opencl_append_string_to_body ("for (", code_gen);
  if (f->LB)
    {
      opencl_append_string_to_body (f->iterator, code_gen);
      opencl_append_string_to_body ("=", code_gen);
      opencl_print_expr (f->LB, code_gen);
    }
  opencl_append_string_to_body (";", code_gen);

  if (f->UB)
    {
      opencl_append_string_to_body (f->iterator, code_gen);
      opencl_append_string_to_body ("<=", code_gen);
      opencl_print_expr (f->UB, code_gen);
    }
  opencl_append_string_to_body (";", code_gen);

  if (value_gt_si (f->stride, 1))
    {
      opencl_append_string_to_body (f->iterator, code_gen);
      opencl_append_string_to_body ("+=", code_gen);
      opencl_append_num_to_body (code_gen, mpz_get_si (f->stride), "%d)\n{\n");
    }
  else
    {
      opencl_append_string_to_body (f->iterator, code_gen);
      opencl_append_string_to_body ("++", code_gen);
      opencl_append_string_to_body (")\n{\n", code_gen);
    }
  iv_type = opencl_get_loop_iter_type (f, code_gen, level);
  iv = create_tmp_var (iv_type, "scat_tmp_iter");

  tmp = opencl_get_var_name (iv);
  check_and_mark_arg (code_gen, tmp, false);
  decl = opencl_print_function_arg_with_type (tmp, iv_type);
  opencl_append_string_to_body (decl, code_gen);

  opencl_append_string_to_body (" = ", code_gen);
  opencl_append_string_to_body (f->iterator, code_gen);
  opencl_append_string_to_body (";\n", code_gen);

  save_clast_name_index (code_gen->newivs_index, f->iterator,
			 VEC_length (tree, code_gen->newivs));
  VEC_safe_push (tree, heap, code_gen->newivs, iv);

  opencl_print_stmt_list (f->body, code_gen, level + 1);
  opencl_append_string_to_body ("}\n", code_gen);
}

/* Generate code for clast conditional statement G, locate on depth DEPTH.
   CODE_GEN holds information related to OpenCL code generation.  */

static void
opencl_print_guard (struct clast_guard *g, opencl_main code_gen, int depth)
{
  int k;
  opencl_append_string_to_body ("if ", code_gen);
  if (g->n > 1)
    opencl_append_string_to_body ("(", code_gen);
  for (k = 0; k < g->n; ++k)
    {
      if (k > 0)
        opencl_append_string_to_body (" && ", code_gen);
      opencl_append_string_to_body ("(", code_gen);
      opencl_print_equation (&g->eq[k], code_gen);
      opencl_append_string_to_body (")", code_gen);
    }
  if (g->n > 1)
    opencl_append_string_to_body (")", code_gen);
  opencl_append_string_to_body (" {\n", code_gen);
  opencl_print_stmt_list (g->then, code_gen, depth);
  opencl_append_string_to_body ("}\n", code_gen);
}


/* Generate code for clast equation EQ.  CODE_GEN holds information
   related to OpenCL code generation.  */

static void
opencl_print_equation (struct clast_equation *eq, opencl_main code_gen)
{
  opencl_print_expr (eq->LHS, code_gen);
  if (eq->sign == 0)
    opencl_append_string_to_body (" == ", code_gen);
  else if (eq->sign > 0)
    opencl_append_string_to_body (" >= ", code_gen);
  else
    opencl_append_string_to_body (" <= ", code_gen);
  opencl_print_expr (eq->RHS, code_gen);
}

/* Generate code for clast expression E.  CODE_GEN holds information
   related to OpenCL code generation.  */

static void
opencl_print_expr (struct clast_expr *e, opencl_main code_gen)
{
  if (!e)
    return;
  switch (e->type)
    {
    case expr_term:
      opencl_print_term ((struct clast_term*) e, code_gen);
      break;
    case expr_red:
      opencl_print_reduction ((struct clast_reduction*) e, code_gen);
      break;
    case expr_bin:
      opencl_print_binary ((struct clast_binary*) e, code_gen);
      break;
    default:
      gcc_unreachable ();
    }
}

/* Generate code for clast term T.  CODE_GEN holds information
   related to OpenCL code generation.  */

static void
opencl_print_term (struct clast_term *t, opencl_main code_gen)
{
  if (t->var)
    {
      const char *real_name = opencl_get_scat_real_name (code_gen, t->var);
      if (value_one_p (t->val))
	opencl_append_var_name (real_name, code_gen);
      else if (value_mone_p (t->val))
	{
	  opencl_append_string_to_body ("-", code_gen);
	  opencl_append_var_name (real_name, code_gen);
	}
      else
	{
	  opencl_append_num_to_body (code_gen, mpz_get_si (t->val), "%d");
	  opencl_append_string_to_body ("*", code_gen);
	  opencl_append_var_name (real_name, code_gen);
	}
      opencl_add_scat_as_arg (code_gen, t->var, real_name);
    }
  else
    opencl_append_num_to_body (code_gen, mpz_get_si (t->val), "%d");
}

/* Generate code for clast reduction statement R.  CODE_GEN holds
   information related to OpenCL code generation.  */

static void
opencl_print_reduction (struct clast_reduction *r, opencl_main  code_gen)
{
  switch (r->type)
    {
    case clast_red_sum:
      opencl_print_sum (r, code_gen);
      break;
    case clast_red_min:
    case clast_red_max:
      if (r->n == 1)
	{
	  opencl_print_expr (r->elts[0], code_gen);
	  break;
	}
      opencl_print_minmax_c (r, code_gen);
      break;
    default:
      gcc_unreachable ();
    }
}

/* Generate code for clast sum statement R.  CODE_GEN holds information
   related to OpenCL code generation.  */

static void
opencl_print_sum (struct clast_reduction *r, opencl_main code_gen)
{
  int i;
  struct clast_term *t;

  gcc_assert (r->n >= 1 && r->elts[0]->type == expr_term);
  t = (struct clast_term *) r->elts[0];
  opencl_print_term (t, code_gen);

  for (i = 1; i < r->n; ++i)
    {
      gcc_assert (r->elts[i]->type == expr_term);
      t = (struct clast_term *) r->elts[i];
      if (value_pos_p (t->val))
	opencl_append_string_to_body ("+", code_gen);
      opencl_print_term (t, code_gen);
    }
}

/* Generate code for clast binary operation B.  CODE_GEN holds
   information related to OpenCL code generation.  */

static void
opencl_print_binary (struct clast_binary *b, opencl_main code_gen)
{
  const char *s1 = NULL, *s2 = NULL, *s3 = NULL;
  bool group = (b->LHS->type == expr_red
		&& ((struct clast_reduction*) b->LHS)->n > 1);

  switch (b->type)
    {
    case clast_bin_fdiv:
      s1 = "floor ((", s2 = ")/(", s3 = "))";
      break;
    case clast_bin_cdiv:
      s1 = "ceil ((", s2 = ")/(", s3 = "))";
      break;
    case clast_bin_div:
      if (group)
	s1 = "(", s2 = ")/", s3 = "";
      else
	s1 = "", s2 = "/", s3 = "";
      break;
    case clast_bin_mod:
      if (group)
	s1 = "(", s2 = ")%", s3 = "";
      else
	s1 = "", s2 = "%", s3 = "";
      break;
    }

  opencl_append_string_to_body (s1, code_gen);
  opencl_print_expr (b->LHS, code_gen);
  opencl_append_string_to_body (s2, code_gen);
  opencl_append_num_to_body (code_gen, mpz_get_si (b->RHS), "%d");
  opencl_append_string_to_body (s3, code_gen);
}

/* Generate code for clast min/max operation R.  CODE_GEN holds
   information related to OpenCL code generation.  */

static void
opencl_print_minmax_c ( struct clast_reduction *r, opencl_main code_gen)
{
  int i;
  for (i = 1; i < r->n; ++i)
    opencl_append_string_to_body (r->type == clast_red_max ? "max (" : "min (",
				  code_gen);
  if (r->n > 0)
    {
      opencl_append_string_to_body ("(unsigned int)(", code_gen);
      opencl_print_expr (r->elts[0], code_gen);
      opencl_append_string_to_body (")", code_gen);
    }
  for (i = 1; i < r->n; ++i)
    {
      opencl_append_string_to_body (",", code_gen);
      opencl_append_string_to_body ("(unsigned int)(", code_gen);
      opencl_print_expr (r->elts[i], code_gen);
      opencl_append_string_to_body ("))", code_gen);
    }
}

#endif
