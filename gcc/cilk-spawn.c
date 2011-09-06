/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains cilk functions for C language support
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
#include "tree.h"
#include "tree-iterator.h"
#include "tree-inline.h"
#include "ggc.h"
#include "gimple.h"
#include "cilk.h"
#include "cgraph.h" /* XXX needs too many other headers*/
#include "target.h"
#include "flags.h"
#include "toplev.h" /* warning() */
#include "function.h"
#include "langhooks.h"
#include "tree-pretty-print.h"
#include "pointer-set.h"

/* Cilk works with C-like languages  */
#include "c-family/c-common.h"  

/* C only for now because of cilk_for */
#include "c-tree.h"



enum add_variable_type  {
    ADD_READ,	/* reference to previously-defined variable */
    ADD_BIND,	/* definition of new variable in inner scope */
    ADD_WRITE	/* write to possibly previously-defined variable */
};

enum cilk_block_type {
    CILK_BLOCK_SPAWN = 30,
    CILK_BLOCK_BLOCK,
    CILK_BLOCK_FOR
};

struct superset_decl_maps
{
  struct pointer_map_t *decl_maps;
  struct superset_decl_maps *ptr_next;
};


  


struct wrapper_data
{
  /* Kind of function to be created. */
  enum cilk_block_type type;
  /* Signature of helper function. */
  tree fntype;
  /* Containing function */
  tree context;
  /* Disposition of all variables in the inner statement. */
  struct pointer_map_t *decl_map;
  /* True if this function needs a static chain */
  bool nested;
  /* Arguments to be passed to wrapper function, currently a list. */
  tree arglist;
  /* Argument types, a list */
  tree argtypes;
  /* Incoming parameters */
  tree parms;
  /* Outer BLOCK object */
  tree block;
};

struct cilk_for_desc;

static void extract_free_variables (tree, struct wrapper_data *,
				    enum add_variable_type);
static void print_node_simple (FILE *fp, const_tree var);
static void race_warning (tree, const_tree, enum cilk_block_type);
static tree build_cilk_wrapper_body (tree, struct wrapper_data *, const char *);
static void cilk_outline (tree inner_fn, tree *, struct wrapper_data *);
static tree copy_decl_for_cilk (tree decl, copy_body_data *id);
static tree check_outlined_calls (tree *, int *, void *);
static tree build_cilk_wrapper (tree, tree *);
static void install_body_with_frame_cleanup (tree, tree);
static bool var_mentioned_p (tree exp, tree var);

extern tree build_unary_op (location_t location, enum tree_code code,
			    tree xarg, int flag);

/* Trying to get cfun right */
static void
pop_cfun_to (tree outer)
{
  pop_cfun ();
  current_function_decl = outer;
  gcc_assert (cfun == DECL_STRUCT_FUNCTION (current_function_decl));
  gcc_assert (cfun->decl == current_function_decl);
}

/* This function is whatever is necessary to make the compiler
   emit a newly generated function if it is needed.  */
static void
cg_hacks (tree fndecl, struct wrapper_data *wd)
{
  const tree outer = current_function_decl;
  struct function *f = DECL_STRUCT_FUNCTION (fndecl);

  gcc_assert (TREE_CODE (fndecl) == FUNCTION_DECL);

  f->is_cilk_function = 1;
  /* gimplify_body may garbage collect.  Save a root. */
  cilk_trees[CILK_TI_PENDING_FUNCTIONS] =
    tree_cons (NULL_TREE, fndecl, cilk_trees[CILK_TI_PENDING_FUNCTIONS]);
  
  f->curr_properties = cfun->curr_properties;

  gcc_assert (cfun == DECL_STRUCT_FUNCTION (outer)); 
  gcc_assert (cfun->decl == outer);

  push_cfun (f); 

  /* If this is a genuine nested function, the nested function
     handling will deal with it.  If this is not a nested function
     it must be handled now or the compiler will crash in a
     mysterious way later. */
  if ((!DECL_STATIC_CHAIN (fndecl) || !wd->nested))
    {
      gimplify_function_tree (fndecl); 
    }
  
  cgraph_add_new_function (fndecl, false);
  
  /* Calling cgraph_finalize_function now seems to be the only way to
     prevent a crash due to cgraph becoming confused over whether the
     function is needed. */
  cgraph_finalize_function (fndecl, true); 

  pop_cfun_to (outer);

  gcc_assert (TREE_VALUE (cilk_trees[CILK_TI_PENDING_FUNCTIONS]) == fndecl);
  cilk_trees[CILK_TI_PENDING_FUNCTIONS] = 
    TREE_CHAIN (cilk_trees[CILK_TI_PENDING_FUNCTIONS]);
}

/* **************************************************************** */
/* Recognizing spawn */

/* Return true if this is a tree which is allowed to contain
   a spawn as operand 0.

   A spawn call may be wrapped in a series of unary operations such
   as conversions.  These conversions need not be "useless"
   to be disregarded because they are retained in the spawned
   statement.  They are bypassed only to look for a spawn
   within.

   A comparison to constant is simple enough to allow, and
   is used to convert to bool.  */
static bool
cilk_ignorable_spawn_rhs_op (tree exp)
{
  enum tree_code code = TREE_CODE (exp);
  switch (TREE_CODE_CLASS (code))
    {
    case tcc_expression:
      return code == ADDR_EXPR;
    case tcc_comparison:
      /* We need the spawn as operand 0 for now.   That's where it
	 appears in the only case we really care about, conversion
	 to bool. */
      return (TREE_CODE (TREE_OPERAND (exp, 1)) == INTEGER_CST);
    case tcc_unary:
    case tcc_reference:
      return true;
    default:
      return false;
    }
}

static bool
cilk_spawnable_constructor (tree exp)
{
  if (TREE_CODE (exp) != ADDR_EXPR)
    return false;
  exp = TREE_OPERAND (exp, 0);
  if (TREE_CODE (exp) != FUNCTION_DECL)
    return false;
  if (DECL_BUILT_IN_CLASS (exp) == BUILT_IN_NORMAL)
    return DECL_FUNCTION_CODE (exp) == BUILT_IN_MEMCPY;
  return lang_hooks.cilk.spawnable_constructor (exp);
}

static bool
recognize_spawn (tree exp)
{
  if (TREE_CODE (exp) != CALL_EXPR)
    return lang_hooks.cilk.recognize_spawn (exp);
  if (!SPAWN_CALL_P (exp))
    return false;
  SPAWN_CALL_P (exp) = 0;
  SPAWN_DETACH_POINT (exp) = 1;
  return true;
}

static tree
is_spawn (tree *tp, int *walk_subtrees, void *data ATTRIBUTE_UNUSED)
{
  tree t = *tp;

  if (TYPE_P (t))
    *walk_subtrees = 0;

  if (TREE_CODE (t) != CALL_EXPR)
    return 0;

  if (SPAWN_CALL_P (t))
    return t;

  return 0;
}

static bool
contains_spawn (tree exp)
{
  return (walk_tree (&exp, is_spawn, 0, 0) != 0);
}

/* See if this is a recognized form of spawn.  Spawns that
   are not recognized here will generate a warning later
   when gimplify runs into the SPAWN_CALL_P or
   AGGR_INIT_VIA_SPAWN_P bit that is turned off when a
   spawn is translated.

   Recognized forms are, after conversion to void, a
   call expression at outer level or an assignment
   at outer level with the right hand side being a
   spawned call.  Note that `=' in C++ may turn into
   a CALL_EXPR rather than a MODIFY_EXPR.

   If this function returns true it has cleared the
   SPAWN_CALL_P or AGGR_INIT_VIA_SPAWN_P flag on the
   call to which the spawn keyword was attached and
   set the SPAWN_DETACH_POINT or AGGR_INIT_DETACH
   flag instead.
*/
bool
cilk_valid_spawn (tree exp0)
{
  tree exp = exp0;
  bool warn;

  if (! TREE_SIDE_EFFECTS (exp))
    return false;

  /* shortcut: if the function contains no Cilk code, this isn't a spawn */
  /* XXX This is wrong when a cilk_for body function contains a spawn. */
  if (!cfun->cilk_frame_decl)
    return false;

  /* Strip off any conversion to void.  It does not affect
     whether spawn is supported here. */
  if (TREE_CODE (exp) == CONVERT_EXPR && VOID_TYPE_P (TREE_TYPE (exp)))
    exp = TREE_OPERAND (exp, 0);

  if (TREE_CODE (exp) == MODIFY_EXPR || TREE_CODE (exp) == INIT_EXPR)
    exp = TREE_OPERAND (exp, 1);

  while (cilk_ignorable_spawn_rhs_op (exp))
    exp = TREE_OPERAND (exp, 0);

  if (TREE_CODE (exp) == TARGET_EXPR)
    exp = TARGET_EXPR_INITIAL (exp);

  if (exp == NULL_TREE)
    return false; /* happens with C++ TARGET_EXPR */

  /* Now we have a call, or this isn't a valid spawn. */
  /* XXX This will reject any outer non-spawn AGGR_INIT_EXPR
     that is valid because of a spawn inside.  Are there any
     such? */
  if (recognize_spawn (exp))
    return true;

  if (TREE_CODE (exp) != CALL_EXPR)
    return false;

  /* This may be a call that is not a spawn itself but contains a spawn.
     In that case the call should be a constructor.

     x = spawn f();

     may expand to

     (call operator= (&var1, (convert &(target var2 (aggr_init/spawn ...))))

     operator= may be a function or a call to __builtin_memcpy (which
     will have one more argument, the size).

     What we specifically support is the address of the value
     initialized by a spawning AGGR_INIT_EXPR being passed as
     the second argument to a function.

     Maybe we should ensure that the function is a constructor
     or builtin memcpy?
  */

  warn = !cilk_spawnable_constructor (CALL_EXPR_FN (exp));

  /* The function address of a call may not be computed via a spawn.
     Look at the arglist only, and only the second argument which
     is the RHS of any plausible assignment or copy.  The first
     argument is the LHS.  A third argument could be a size for
     memcpy.  This path supports op= in addition to =, only because
     it is easy to do so. */
  if (call_expr_nargs (exp) < 2)
    return false;

  exp = CALL_EXPR_ARG (exp, 0);

  {
    extern bool tree_ssa_useless_type_conversion (tree);
    STRIP_USELESS_TYPE_CONVERSION (exp);
  }

  if (TREE_CODE (exp) == ADDR_EXPR)
    exp = TREE_OPERAND (exp, 0);

  if (TREE_CODE (exp) == TARGET_EXPR)
    exp = TARGET_EXPR_INITIAL (exp);

  if (!exp || !recognize_spawn (exp))
    return false;

  if (warn)
    {
      if (EXPR_HAS_LOCATION (exp0))
	{
	  warning (0, "suspicious use of spawn");
	}
      else
	warning (0, "suspicious use of spawn");
    }

  return true;
}

/* Given a statement to be spawned generate the surrounding code
   to make the runtime treat the expression as a spawn. */
void
gimplify_cilk_spawn (tree *spawn_p, gimple_seq *before ATTRIBUTE_UNUSED,
		     gimple_seq *after ATTRIBUTE_UNUSED)
{
  tree expr = *spawn_p;
  tree function, call1, call2, new_args;
  tree ii_args = NULL_TREE;
  int total_args = 0, ii = 0;
  tree *arg_array;
  tree setjmp_cond_expr = NULL_TREE;
  
  /* This is a statement that cilk_valid_spawn has said yes to.
     It might be a call, modify, or init. */

  cfun->calls_spawn = 1;
  cfun->is_cilk_function = 1;
  

  /* Convert this statement into a nested function, using capture
     by value when that is equivalent but faster. */
  if (!flag_enable_cilk)
    {
      sorry ("spawn is not implemented");
      *spawn_p = build_empty_stmt (UNKNOWN_LOCATION);
      return;
    }

  new_args = NULL;
  function = build_cilk_wrapper (expr, &new_args);

  /* thsi should give the number of parameters */
  total_args = list_length (new_args);
  arg_array = XNEWVEC (tree, total_args);
  /* gcc_assert(arg_array != NULL_TREE); */
  

  ii_args = new_args;
  for (ii = 0; ii < total_args; ii++)
    {
      arg_array[ii] = TREE_VALUE (ii_args);
      ii_args=TREE_CHAIN (ii_args);
    }
  
  
  /* A spawn wrapper has void type. */
  /* XXX static chain */
  TREE_USED (function) = 1;

  rest_of_decl_compilation (function, 0, 0);

  call1 = cilk_call_setjmp (cfun->cilk_frame_decl);
  
  if (*arg_array == NULL_TREE)
    call2 = build_call_expr (function,0);
  else
    {
      call2 = build_call_expr_loc_array (UNKNOWN_LOCATION,
					 function, total_args, arg_array);
    }
  setjmp_cond_expr = fold_build2 (EQ_EXPR, TREE_TYPE (call1), call1,
				  build_int_cst (TREE_TYPE (call1), 0));
  
  *spawn_p = fold_build3 (COND_EXPR, void_type_node, setjmp_cond_expr, call2,
			  build_empty_stmt (EXPR_LOCATION(call1)));
  /* tree-nested.c will set the static chain */

  /* XXX  cgraph_update_edges_for_call_stmt ? */
}

/* **************************************************************** */
/* Generating nested functions */

/* The old version used a splay_tree with a deterministic comparator.
   Pointer sets are not reproducible from run to run.  These functions
   extract the contents into an array with predictable behavior. */

static int
sort_decl (const void *p1, const void *p2)
{
  const_tree t1 = (const_tree)p1;
  const_tree t2 = (const_tree)p2;
  if (t1 == t2)
    return 0;
  if (TREE_CODE (t1) != TREE_CODE (t2))
    return t1 - t2;
  if (DECL_P (t1))
    return (int)DECL_UID (t1) - (int)DECL_UID (t2);
  if (TYPE_P (t1))
    return (int)TYPE_UID (t1) - (int)TYPE_UID (t2);
  if (TREE_CODE (t1) == INTEGER_CST)
    return tree_int_cst_compare (t1, t2);
  return 0;
}



/* Given a variable in an expression to be extracted into
   a helper function, declare the helper function parameter
   to receive it.

   On entry the value of the (key, value) pair may be

   (*, error_mark_node) -- Variable is private to helper function,
   do nothing.

   (var, var) -- Reference to outer scope (function or global scope).

   (var, integer 0) -- Capture by value, save newly-declared PARM_DECL
   for value in value slot.

   (var, integer 1) -- Capture by reference, declare pointer to type
   as new PARM_DECL and store (spawn_stmt (indirect_ref (parm)).
   
   (var, ???) -- Pure output argument, handled similarly to above.
*/
static bool
declare_one_free_variable (const void *var0, void **map0,
			   void *data ATTRIBUTE_UNUSED)
{
  const_tree var = (const_tree)var0;
  tree map = (tree)*map0;
  tree var_type = TREE_TYPE (var), arg_type;
  bool by_reference;
  tree parm;

  gcc_assert (DECL_P (var));

  /* Ignore truly local variables. */
  if (map == error_mark_node)
    return true;
  /* Ignore references to the parent function */
  if (map == var)
    return true;

  gcc_assert (TREE_CODE (map) == INTEGER_CST);

  /* A value is passed by reference if:

     1. It is addressable, so that a copy may not be made.
     2. It is modified in the spawned statement.
     In the future this function may want to arrange
     a warning if the spawned statement is a loop body
     because an output argument would indicate a race.
     Note: Earlier passes must have marked the variable addressable.
     3. It is expensive to copy. */
  by_reference =
    (TREE_ADDRESSABLE (var_type)
     /* Arrays must be passed by reference.  This is required for C
	semantics -- arrays are not first class objects.  Other
	aggregate types can and should be passed by reference if
	they are not passed to the spawned function.  We aren't yet
	distinguishing safe uses in argument calculation from unsafe
	uses as outgoing function arguments, so we make a copy to
	stabilize the value. */
     || TREE_CODE (var_type) == ARRAY_TYPE
     || (tree) map == integer_one_node);

  if (by_reference)
    var_type = build_qualified_type (build_pointer_type (var_type),
				     TYPE_QUAL_RESTRICT);
  gcc_assert (!TREE_ADDRESSABLE (var_type));

  /* Maybe promote to int */
  if (INTEGRAL_TYPE_P (var_type)
      && COMPLETE_TYPE_P (var_type)
      && targetm.calls.promote_prototypes (var_type)
      && INT_CST_LT_UNSIGNED (TYPE_SIZE (var_type),
			      TYPE_SIZE (integer_type_node)))
    arg_type = integer_type_node;
  else
    arg_type = var_type;

  /* See cp/decl.2.c: build_artificial_parm and cp_build_parm_decl. */
  parm = build_decl (UNKNOWN_LOCATION, PARM_DECL, NULL_TREE, var_type);
  DECL_ARG_TYPE (parm) = arg_type;
  DECL_ARTIFICIAL (parm) = 0;
  TREE_READONLY (parm) = 1;
  
  if (by_reference)
    {
      parm = build1 (INDIRECT_REF, TREE_TYPE (var_type), parm);
      parm = build1 (PAREN_EXPR, void_type_node, parm);
    }
  *map0 = parm;
  return true;
}

static void
declare_free_variables (struct wrapper_data *wd)
{
  pointer_map_traverse_ordered (wd->decl_map, declare_one_free_variable,
				sort_decl, wd);
}

/* Helper for wrapped_function_name.  Find a call embedded within an
   expression.  Hopefully it is the spawned call.  This function is
   allowed to be wrong.  It exists to make generated code slightly
   more understandable on average. */
static tree
find_call (tree exp)
{
  enum tree_code code = TREE_CODE (exp);
  if (code == CALL_EXPR)
    return exp;

  /* Only the right hand side of an assignment can be spawned. */
  if ((code == MODIFY_EXPR) || (code == INIT_EXPR))
    return find_call (TREE_OPERAND (exp, 1));

  if (TREE_CODE_CLASS (code) == tcc_unary)
    return find_call (TREE_OPERAND (exp, 0));

  /* Don't try too hard yet. */
  return NULL_TREE;
}


/* If a function is being called try to figure out which function.
   This is for the benefit of humans. */
static const char *
wrapped_function_name (tree exp)
{
  tree fn = find_call (exp);
  if (fn == NULL_TREE)
    return NULL;
  fn = get_callee_fndecl (fn);
  if (fn == NULL_TREE)
    return NULL;

  /* Spawn of an expression returning a struct by value
     can appear as a spawn of __builtin_memcpy.  Don't
     confuse matters by putting that name in assembly. */
  if (DECL_BUILT_IN (fn))
    return NULL;
  if (DECL_NAME (fn))
    return IDENTIFIER_POINTER (DECL_NAME (fn));
  return NULL;
}

static void
init_wd (struct wrapper_data *wd, enum cilk_block_type type)
{
  wd->type = type;
  wd->fntype = NULL_TREE;
  wd->context = current_function_decl;
  wd->decl_map = pointer_map_create ();
  /* For bodies are always nested.  Others start off as normal functions. */
  wd->nested = (type == CILK_BLOCK_FOR);
  wd->arglist = NULL_TREE;
  wd->argtypes = NULL_TREE;
  wd->block = NULL_TREE;
}

static void
free_wd (struct wrapper_data *wd)
{
  pointer_map_destroy (wd->decl_map);
  wd->nested = false;
  wd->arglist = NULL_TREE;
  wd->argtypes = NULL_TREE;
  wd->parms = NULL_TREE;
}

static tree
build_cilk_wrapper (tree exp, tree *args_out)
{
  struct wrapper_data wd;
  tree fndecl;
  const char *fname = wrapped_function_name (exp);

  init_wd (&wd, CILK_BLOCK_SPAWN);

  if (TREE_CODE (exp) == CONVERT_EXPR)
    {
      exp = TREE_OPERAND(exp,0);
    }
  
  /* Special handling for top level INIT_EXPR.
     Usually INIT_EXPR means the variable is defined
     in the spawned expression and can be private to
     the spawn helper.  At top level INIT_EXPR defines
     a variable to be initialized by spawn and the
     variable must remain in the outer function. */
  if (TREE_CODE (exp) == INIT_EXPR)
    {
      extract_free_variables (TREE_OPERAND (exp, 0), &wd, ADD_WRITE);
      extract_free_variables (TREE_OPERAND (exp, 1), &wd, ADD_READ);
      /* TREE_TYPE should be void.  Be defensive. */
      if (TREE_TYPE (exp) != void_type_node)
	extract_free_variables (TREE_TYPE (exp), &wd, ADD_READ);
    }
  else
    {
      extract_free_variables (exp, &wd, ADD_READ);
    }
  declare_free_variables (&wd);
  wd.block = TREE_BLOCK (exp);
  if (!wd.block)
    wd.block = DECL_INITIAL (current_function_decl);

  /* ??? add_decl_expr for scope? */

  /* Now fvars maps old variable to incoming variable.  Update
     the expression and arguments to refer to the new names. */

  fndecl = build_cilk_wrapper_body (exp, &wd, fname);
  *args_out = wd.arglist;
  /* TREE_TYPE(*args_out)=wd.argtypes;  */
  
  free_wd (&wd);

  /* XXX This now returns the function where Cilk++ 1.x returned a
     pointer to the function. */
  return fndecl;
}

/* **************************************************************** */
/* build variable map */

static void
print_node_simple (FILE *fp, const_tree var)
{
  enum tree_code code = TREE_CODE (var);
  switch (code)
    {
    case VAR_DECL:
    case PARM_DECL:
      if (DECL_NAME (var))
	fprintf (fp, "%s.%d", IDENTIFIER_POINTER (DECL_NAME (var)),
		 DECL_UID (var));
      else
	fprintf (fp, "D.%d", DECL_UID (var));
      break;
    case RESULT_DECL:
      fprintf (fp, "RESULT.%d", DECL_UID (var));
      break;
    case INDIRECT_REF:
      fputs ("*", fp);
      print_node_simple (fp, TREE_OPERAND (var, 0));
      break;
    case ADDR_EXPR:
      fputs ("&", fp);
      print_node_simple (fp, TREE_OPERAND (var, 0));
      break;
    case BLOCK:
      fputs ("BLOCK", fp);
      break;
    case INTEGER_CST:
      dump_double_int (fp, TREE_INT_CST (var), TYPE_UNSIGNED (TREE_TYPE (var)));
      break;
    default:
      fputs (tree_code_name[(int) code], fp);
      if (TREE_CODE_CLASS (code) == tcc_unary)
	{
	  fputc (' ', fp);
	  print_node_simple (fp, TREE_OPERAND (var, 0));
	  break;
	}
      break;
    }
}

static bool
dump_replacement_map_1 (const void *k, void **vp, void *data)
{
  FILE *fp = (FILE *)data;
  const_tree var = (const_tree)k;
  tree val = (tree)*vp;

  print_node_simple (fp, var);
  fputs (" = ", fp);

  if (val == error_mark_node)
    fputs ("(private)", fp);
  else if (val == var)
    fputs ("(nested)", fp);
  else if (val == integer_zero_node)
    fputs ("(read-only)", fp);
  else if (val == integer_one_node)
    fputs ("(modified)", fp);
  else if (val == integer_minus_one_node)
    fputs ("(loop variable)", fp);
  else
    print_node_simple (fp, val);
  fputc ('\n', fp);
  return true;
}

void dump_replacement_map (FILE *fp, struct pointer_map_t *decl_map);
void debug_replacement_map (struct pointer_map_t *decl_map);

void
dump_replacement_map (FILE *fp, struct pointer_map_t *decl_map)
{
  pointer_map_traverse (decl_map, dump_replacement_map_1, fp);
}

void
debug_replacement_map (struct pointer_map_t *decl_map)
{
  dump_replacement_map (stderr, decl_map);
}

static void
add_variable (struct wrapper_data *wd, tree var, enum add_variable_type how)
{
  void **valp;
  
  valp = pointer_map_contains (wd->decl_map, (void *) var);
  if (valp)
    {
      tree val = (tree) *valp;
      /* If the variable is local, do nothing. */
      if (val == error_mark_node)
	return;
      /* If the variable was entered with itself as value,
	 meaning it belongs to an outer scope, do not alter
	 the value. */
      if (val == var)
	{
	  if (wd->type != CILK_BLOCK_SPAWN && how == ADD_WRITE)
	    race_warning (var, NULL_TREE, wd->type);
	  return;
	}
      /* A statement expression may cause a variable to be
	 bound twice, once in BIND_EXPR and again in a
	 DECL_EXPR.  That case caused a return in the 
	 test above.  Any other duplicate definition is
	 an error.*/
      gcc_assert (how != ADD_BIND);
      if (how != ADD_WRITE)
	return;
      /* This variable might have been entered as read
	 but is now written. */
      *valp = (void *) var;
      wd->nested = true;
      return;
    }
  else
    {
      tree val = NULL_TREE;

      /* Nested function rewriting silently discards hard register
	 assignments for function scope variables, and they wouldn't
	 work anyway.  Warn here.  This misses one case: if the
	 register variable is used as the loop bound or increment it
	 has already been added to the map. */
      if ((how != ADD_BIND)
	  && (TREE_CODE (var) == VAR_DECL) && !DECL_EXTERNAL (var)
	  && DECL_HARD_REGISTER (var))
	warning (0,
		 "register assignment ignored for %qD used in Cilk block",
		 var);

      switch (how)
	{
	  /* ADD_BIND means always make a fresh new variable */
	case ADD_BIND:
	  val = error_mark_node;
	  break;
	  /* ADD_READ means
	     1. For cilk_for, refer to the outer scope definition as-is
	     2. For a spawned block, take a scalar in an argument
	     and otherwise refer to the outer scope definition as-is
	     3. For a spawned call, take a scalar in an argument */
	case ADD_READ:
	  switch (wd->type)
	    {
	    case CILK_BLOCK_FOR:
	      val = var;
	      break;
	    case CILK_BLOCK_BLOCK:
	    case CILK_BLOCK_SPAWN:
	      if (TREE_ADDRESSABLE (var))
		{
		  val = var;
		  wd->nested = true;
		  break;
		}
	      val = integer_zero_node;
	      break;
	    }
	  break;
	case ADD_WRITE:
	  switch (wd->type)
	    {
	    case CILK_BLOCK_BLOCK:
	    case CILK_BLOCK_FOR:
	      race_warning (var, NULL_TREE, wd->type);
	      val = var;
	      wd->nested = true;
	      break;
	    case CILK_BLOCK_SPAWN:
	      if (TREE_ADDRESSABLE (var))
		val = integer_one_node;
	      else
		{
		  val = var;
		  wd->nested = true;
		}
	      break;
	    }
	}
      *pointer_map_insert (wd->decl_map, (void *)var) = val;
    }
}

/* Find the variables referenced in an expression T. This does not
   avoid duplicates because a variable may be read in one context
   and written in another.

   HOW describes the context in which the reference is seen.

   If NESTED is true a nested function is being generated and
   variables in the original context should not be remapped. */
static void
extract_free_variables (tree t, struct wrapper_data *wd,
			enum add_variable_type how)
{
  enum tree_code code;
  bool is_expr;

#define SUBTREE(EXP)  extract_free_variables(EXP, wd, ADD_READ)
#define MODIFIED(EXP) extract_free_variables(EXP, wd, ADD_WRITE)
#define INITIALIZED(EXP) extract_free_variables(EXP, wd, ADD_BIND)

  /* Skip empty subtrees.  */
  if (t == NULL_TREE)
    return;

  code = TREE_CODE (t);
  is_expr = IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (code));
  if (is_expr)
    SUBTREE (TREE_TYPE (t));

  switch (code)
    {
    case ERROR_MARK:
    case IDENTIFIER_NODE:
    case INTEGER_CST:
    case REAL_CST:
    case FIXED_CST:
    case STRING_CST:
    case BLOCK:
    case PLACEHOLDER_EXPR:
    case FIELD_DECL:
    case VOID_TYPE:
    case REAL_TYPE:
      /* These do not contain variable references. */
      return;

    case SSA_NAME:
      /* Currently we don't see SSA_NAME. */
      extract_free_variables (SSA_NAME_VAR (t), wd, how);
      return;

    case LABEL_DECL:
      /* This might be a reference to a label outside the Cilk block,
	 which is an error, or a reference to a label in the Cilk block
	 that we haven't seen yet.  We can't tell.  Ignore it.  An
	 invalid use will cause an error later in copy_decl_for_cilk. */
      return;

    case RESULT_DECL:
      if (wd->type != CILK_BLOCK_SPAWN)
	TREE_ADDRESSABLE (t) = 1;
    case VAR_DECL:
    case PARM_DECL:
      if (!TREE_STATIC (t) && !DECL_EXTERNAL (t))
	add_variable (wd, t, how);
      return;

    case NON_LVALUE_EXPR:
    case CONVERT_EXPR:
    case NOP_EXPR:
      SUBTREE (TREE_OPERAND (t, 0));
      return;

    case INIT_EXPR:
      INITIALIZED (TREE_OPERAND (t, 0));
      SUBTREE (TREE_OPERAND (t, 1));
      return;

    case MODIFY_EXPR:
    case PREDECREMENT_EXPR:
    case PREINCREMENT_EXPR:
    case POSTDECREMENT_EXPR:
    case POSTINCREMENT_EXPR:
      /* These write their result. */
      MODIFIED (TREE_OPERAND (t, 0));
      SUBTREE (TREE_OPERAND (t, 1));
      return;

    case ADDR_EXPR:
      /* This might modify its argument, and the value needs to be
	 passed by reference in any case to preserve identity and
	 type if is a promoting type.  In the case of a nested loop
	 just notice that we touch the variable.  It will already
	 be addressable, and marking it modified will cause a spurious
	 warning about writing the control variable.

	 XXX Conversion to a const pointer should not result in a
	 modified warning.

	 XXX If the variable is not the control variable, the warning
	 should be printed. */
      if (wd->type != CILK_BLOCK_SPAWN)
	SUBTREE (TREE_OPERAND (t, 0));
      else
	MODIFIED (TREE_OPERAND (t, 0));
      return;

    case ARRAY_REF:
      /* Treating ARRAY_REF and BIT_FIELD_REF identically may
	 mark the array as written but the end result is correct
	 because the array is passed by pointer anyway. */
    case BIT_FIELD_REF:
      /* Propagate the access type to the object part of which
	 is being accessed here.  As for ADDR_EXPR, don't do this
	 in a nested loop, unless the access is to a fixed index. */
      if (wd->type != CILK_BLOCK_FOR || TREE_CONSTANT (TREE_OPERAND (t, 1)))
	extract_free_variables (TREE_OPERAND (t, 0), wd, how);
      else
	SUBTREE (TREE_OPERAND (t, 0));
      SUBTREE (TREE_OPERAND (t, 1));
      SUBTREE (TREE_OPERAND (t, 2));
      return;

    case TREE_LIST:
      SUBTREE (TREE_PURPOSE (t));
      SUBTREE (TREE_VALUE (t));
      SUBTREE (TREE_CHAIN (t));
      return;

    case TREE_VEC:
      {
	int len = TREE_VEC_LENGTH (t);
	int i;
	for (i = 0; i < len; i++)
	  SUBTREE (TREE_VEC_ELT (t, i));
	return;
      }

    case VECTOR_CST:
      SUBTREE (TREE_VECTOR_CST_ELTS (t));
      break;

    case COMPLEX_CST:
      SUBTREE (TREE_REALPART (t));
      SUBTREE (TREE_IMAGPART (t));
      return;

    case CONSTRUCTOR:
      {
	unsigned HOST_WIDE_INT idx;
	constructor_elt *ce;

	for (idx = 0;
	     VEC_iterate(constructor_elt, CONSTRUCTOR_ELTS (t), idx, ce);
	     idx++)
	  SUBTREE (ce->value);
	return;
      }

    case BIND_EXPR:
      {
	tree decl;
	for (decl = BIND_EXPR_VARS (t); decl; decl = TREE_CHAIN (decl))
	  {
	    add_variable (wd, decl, ADD_BIND);
	    /* A self-referential initialization is no problem because
	       we already entered the variable into the map as local. */
	    SUBTREE (DECL_INITIAL (decl));
	    SUBTREE (DECL_SIZE (decl));
	    SUBTREE (DECL_SIZE_UNIT (decl));
	  }
	SUBTREE (BIND_EXPR_BODY (t));
	return;
      }

    case STATEMENT_LIST:
      {
	tree_stmt_iterator i;
	for (i = tsi_start (t); !tsi_end_p (i); tsi_next (&i))
	  SUBTREE (*tsi_stmt_ptr (i));
	return;
      }

    case OMP_PARALLEL:
    case OMP_TASK:
    case OMP_FOR:
    case OMP_SINGLE:
    case OMP_SECTION:
    case OMP_SECTIONS:
    case OMP_MASTER:
    case OMP_ORDERED:
    case OMP_CRITICAL:
    case OMP_ATOMIC:
    case OMP_CLAUSE:
      error ("OMP construct used within Cilk construct");
      break;

    case TARGET_EXPR:
      {
	INITIALIZED (TREE_OPERAND (t, 0));
	SUBTREE (TREE_OPERAND (t, 1));
	SUBTREE (TREE_OPERAND (t, 2));
	if (TREE_OPERAND (t, 3) != TREE_OPERAND (t, 1))
	  SUBTREE (TREE_OPERAND (t, 3));
	return;
      }

    case RETURN_EXPR:
      if (TREE_NO_WARNING (t))
	{
	  gcc_assert (errorcount);
	  return;
	}
      error ("spawn of return statement");
      return;

    case DECL_EXPR:
      if (TREE_CODE (DECL_EXPR_DECL (t)) != TYPE_DECL)
	INITIALIZED (DECL_EXPR_DECL (t));
      return;

    case INTEGER_TYPE:
    case ENUMERAL_TYPE:
    case BOOLEAN_TYPE:
      SUBTREE (TYPE_MIN_VALUE (t));
      SUBTREE (TYPE_MAX_VALUE (t));
      return;

    case POINTER_TYPE:
      SUBTREE (TREE_TYPE (t));
      break;

    case ARRAY_TYPE:
      SUBTREE (TREE_TYPE (t));
      SUBTREE (TYPE_DOMAIN (t));
      return;

    case RECORD_TYPE:
      SUBTREE(TYPE_FIELDS(t));
      return;
    
    case METHOD_TYPE:
      SUBTREE(TYPE_ARG_TYPES(t));
      SUBTREE(TYPE_METHOD_BASETYPE(t));
      return;

    case AGGR_INIT_EXPR:
    case CALL_EXPR:
      {
	int len = 0;
	int ii = 0;
	if (TREE_CODE (TREE_OPERAND (t, 0)) == INTEGER_CST)
	  {
	    len = TREE_INT_CST_LOW (TREE_OPERAND(t, 0));

	    for (ii = 0; ii < len; ii++)
	      {
		SUBTREE (TREE_OPERAND (t,ii));
	      }
	    SUBTREE (TREE_TYPE (t));
	  }
	break;
      }

    default:
      if (is_expr)
	{
	  int i, len;

	  /* Walk over all the sub-trees of this operand.  */
	  len = TREE_CODE_LENGTH (code);

	  /* Go through the subtrees.  We need to do this in forward order so
	     that the scope of a FOR_EXPR is handled properly.  */
	  for (i = 0; i < len; ++i)
	    SUBTREE (TREE_OPERAND (t, i));
	}
      return;
    }

  /* TREE_CHAIN? */
}

static void
race_warning (tree var,
	      const_tree loc_expr,
	      enum cilk_block_type where)
{
  if (TREE_NO_WARNING (var))
    return;
  TREE_NO_WARNING (var) = 1;

  if (loc_expr && EXPR_HAS_LOCATION (loc_expr))
    {
      if (where == CILK_BLOCK_FOR)
	warning (OPT_Wcilk_for,
		 "writes to loop body may race");
      else
	warning (OPT_Wcilk_block,
		 "outer scope variable written in spawned statement");
      return;
    }
  if (where == CILK_BLOCK_FOR)
    warning (OPT_Wcilk_for, "writes to  in loop body may race");
  else
    warning (OPT_Wcilk_block,
	     "outer scope variable written in spawned statement");
  return;
}

/* **************************************************************** */
/* Make me a function */

static int cilk_wrapper_cnt;

static tree
build_cilk_helper_decl (struct wrapper_data *wd, const char *wrapped)
{
  char name[48];
  tree fndecl;
  tree block;
  tree t;
  char *cc = NULL, *dd = NULL;
  
  /* Make a name for this wrapper.
     12 characters plus <~6 wrapper counter plus up to 24 characters
     called function name to aid human debugging. */
  if (wd->type == CILK_BLOCK_FOR)
    sprintf (name, "__cilk_for_%03d", ++cilk_wrapper_cnt);
  else if (wrapped && strlen (wrapped) < 24)
    sprintf (name, "__cilk_spawn_%03d",++cilk_wrapper_cnt);
  else
    sprintf (name, "__cilk_spawn_%03d", ++cilk_wrapper_cnt);


  /* this is here so that we can get rid of spaces in teh file name */

  cc = name;
  dd = name;

  while (*cc)
    {
      if (*cc == ' ')
	++cc;
      else if (!ISIDNUM(*cc))
	{
	  *cc = '_';
	  *dd++ = *cc++;
	}
      else
	*dd++ = *cc++;
    }
  

  
  t = get_identifier(name);
  fndecl = build_decl (EXPR_LOCATION(t), FUNCTION_DECL, t, wd->fntype);

  TREE_PUBLIC (fndecl) = 0;
  TREE_STATIC (fndecl) = 1;
  TREE_USED (fndecl) = 1;
  DECL_ARTIFICIAL (fndecl) = 0;
  DECL_IGNORED_P (fndecl) = 0; /* ??? what OMP does */
  DECL_EXTERNAL (fndecl) = 0;

  if (wd->nested)
    {
      DECL_CONTEXT (fndecl) = wd->context;
      DECL_EXPLICIT_STATIC_CHAIN (fndecl) = (wd->type == CILK_BLOCK_FOR);
    }
  else
    {
      /* In C++, copying the outer function's context makes the loop
	 function appear like a static member function.
	 DECL_CONTEXT = 0 would work too.  */
      DECL_CONTEXT (fndecl) = DECL_CONTEXT (wd->context);
      /* This is wrong because the block could be inside
	 a cilk_for even if it isn't one.  However, the
	 crash noted in cg_hacks (or a similar one) will
	 occur if NO_STATIC_CHAIN is cleared. */
      DECL_STATIC_CHAIN (fndecl) = DECL_STATIC_CHAIN (wd->context);
    }

  /* This is what OMP does. */
  block = make_node (BLOCK);
  DECL_INITIAL (fndecl) = block;
  TREE_USED (block) = 1;
  gcc_assert (!DECL_SAVED_TREE (fndecl));

  if (wd->type == CILK_BLOCK_SPAWN)
    /* A call to this function is a knot, and detaches. */
    DECL_SET_KNOT (fndecl, 1);

  /* Inlining would defeat the purpose of this wrapper.
     Either it secretly switches stack frames or it allocates
     a stable stack frame to hold function arguments even if
     the parent stack frame is stolen. */
  DECL_UNINLINABLE (fndecl) = 1;

  /* OMP makes a result, so why don't we do the same?
     Cilk++ 1.x did not need one. */
  {
    tree r = build_decl (UNKNOWN_LOCATION,RESULT_DECL, NULL_TREE,
			 void_type_node);
    DECL_ARTIFICIAL (r) = 0;
    DECL_IGNORED_P (r) = 1;
    DECL_CONTEXT (r) = fndecl;
    DECL_RESULT (fndecl) = r;
  }

  /* XXX OMP beats on the struct function here */
  
  return fndecl;
}

static bool
wrapper_parm_cb (const void *key0, void **val0, void *data)
{
  struct wrapper_data *wd = (struct wrapper_data *)data;
  tree arg = (const tree)key0;
  tree val = (tree)*val0;
  tree parm;

  if (val == error_mark_node || val == arg)
    {
      return true;
    }

  if (TREE_CODE (val) == PAREN_EXPR)
    {
      /* We should not reach here with a register receiver.
	 We may see a register variable modified in the
	 argument list.  Because register variables are
	 worker-local we don't need to work hard to support
	 them in code that spawns. */
      if ((TREE_CODE (arg) == VAR_DECL) && DECL_HARD_REGISTER (arg))
	{
	  error ("explicit register variable %qD may not be modified in spawn",
		 arg);
	  arg = null_pointer_node;
	}
      else
	{
	  /* This assertion is commented out because it trips over
	     the target of an assignment spawn.  But that target
	     does need to live in memory, so there is a bug.
	     gcc_assert (!DECL_P (arg) || TREE_ADDRESSABLE (arg));*/
	  arg = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (arg)), arg);
	}
      val = TREE_OPERAND (val, 0);
      *val0 = val;
      gcc_assert (TREE_CODE (val) == INDIRECT_REF);
      parm = TREE_OPERAND (val, 0);
      STRIP_NOPS (parm);
    }
  else
    {
      parm = val;
    }
  TREE_CHAIN (parm) = wd->parms;
  wd->parms = parm;
  wd->argtypes = tree_cons (NULL_TREE, TREE_TYPE (parm), wd->argtypes); 
  wd->arglist = tree_cons (NULL_TREE, arg, wd->arglist); 
  return true;
}

static void
build_wrapper_type (struct wrapper_data *wd)
{
  wd->arglist = NULL_TREE;
  wd->parms = NULL_TREE;
  wd->argtypes = void_list_node;

  /* Cilk++ 1.x code to put parent frame at end of list removed. */

  pointer_map_traverse_ordered(wd->decl_map, wrapper_parm_cb, sort_decl, wd);

  gcc_assert (wd->type != CILK_BLOCK_FOR);

  /* Now build a function.

     Its return type is void (all side effects are via explicit parameters).

     Its parameters are WRAPPER_PARMS with type WRAPPER_TYPES.

     Actual arguments in the caller are WRAPPER_ARGS.

  */

  wd->fntype = build_function_type (void_type_node, wd->argtypes);
}


/* Generate the body of a wrapper function that assigns the
   result of the expression RHS into RECEIVER.  RECEIVER must
   be NULL if this is not a spawn -- the wrapper will return
   a value.  If this is a spawn the wrapper will return void.

   The Cilk++ 1.x version of this function explicitly used the
   parent frame.  Now the use is implicit because the a pointer
   to the parent frame is stored in the current frame. */
static tree
build_cilk_wrapper_body (tree stmt,
			 struct wrapper_data *wd,
			 const char *fname)
{
  const tree outer = current_function_decl;
  tree fndecl;
  tree p;

   /* Build the type of the wrapper and its argument list from the
     variables that it requires.  */
  build_wrapper_type (wd);

  /* We are committed to building a wrapper.  Start working on it.

     Emit a function that takes WRAPPER_PARMS incoming and applies
     ARGS (modified) to the wrapped function.  Return the wrapper
     and modified ARGS to the caller to generate a function call. */

  fndecl = build_cilk_helper_decl (wd, fname);

  push_struct_function (fndecl);

  if (wd->nested && (wd->type == CILK_BLOCK_FOR))
    {
      DECL_EXPLICIT_STATIC_CHAIN (fndecl) = 1;
      gcc_assert (TREE_VALUE (wd->arglist) == NULL_TREE);
      TREE_VALUE (wd->arglist) = build2 (FDESC_EXPR, ptr_type_node ,
					 fndecl, integer_one_node);
    }
  else
    {
      DECL_EXPLICIT_STATIC_CHAIN (fndecl) = 0;
    }

  DECL_ARGUMENTS (fndecl) = wd->parms;

  for (p = wd->parms; p; p = TREE_CHAIN (p))
    DECL_CONTEXT (p) = fndecl;


  /* Any wrapper parameters that are passed by hidden reference
     contrary to the pass-by-value appearing in the C code,
     need to be copied here using a copy constructor if any
     use follows the detach.

     It's not clear whether this can happen.  Currently arrays
     are always passed by reference, which is correct C semantics
     because arrays are not first class objects.  POD structures
     are not passed by reference because in general a copy needs
     to be made to stabilize the value. */

  /* Now install the body of the function here.
     
     If the detach is at the point of a spawned call it
     is added during gimplification of the wrapped function.
     That serves two related purposes: AGGR_INIT has been
     converted to CALL and evaluation of arguments with
     side effects has been moved out of the call. */
  cilk_outline (fndecl, &stmt, wd);
  stmt = fold_build_cleanup_point_expr (void_type_node, stmt);
  gcc_assert (!DECL_SAVED_TREE (fndecl));
  install_body_with_frame_cleanup (fndecl, stmt);
  gcc_assert (DECL_SAVED_TREE (fndecl));

  pop_cfun_to (outer);

  /* Apparently we need to gimplify now because we can't leave
     non-GIMPLE functions lying around. */
  cg_hacks (fndecl, wd);

  return fndecl;
}


static bool
for_local_cb (const void *k_v, void **vp, void *p)
{
  tree k = (const tree) k_v; /* const cast */
  tree v = (tree) *vp;

  if (v == error_mark_node)
    *vp = copy_decl_no_change (k, (copy_body_data *)p);
  return true;
}

static bool
wrapper_local_cb (const void *k_v, void **vp, void *data)
{
  copy_body_data *id = (copy_body_data *)data;
  tree key = (const tree) k_v;
  tree val = (tree) *vp;

  if (val == error_mark_node)
    *vp = copy_decl_for_cilk (key, id);

  return true;
}

/* Each DECL in the source code (spawned statement)
   is passed to this function once.  Each instance
   of the DECL is replaced with the result of this
   function.

   The parameters of the wrapper should have been
   entered into the map already.  This function
   only deals with variables with scope limited
   to the spawned expression. */
static tree
copy_decl_for_cilk (tree decl, copy_body_data *id)
{
  switch (TREE_CODE (decl))
    {
    case VAR_DECL:
      return copy_decl_no_change (decl, id);

    case LABEL_DECL:
      error ("Invalid use of label %q+D in spawn", decl);
      return error_mark_node;

    case RESULT_DECL:
      /* PARM_DECL has already been entered into the map. */
    case PARM_DECL:
      /* PARM_DECL has already been entered into the map. */
    default:
      gcc_unreachable ();
      return error_mark_node;
    }
}

/* Alter a tree STMT from OUTER_FN to form the body of INNER_FN. */
void
cilk_outline (tree inner_fn,
	      tree *stmt_p,
	      struct wrapper_data *wd)
{
  const tree outer_fn = wd->context;	      
  /* XXX This may change */
  const bool nested = (wd->type == CILK_BLOCK_FOR);
  copy_body_data id;
  bool throws;

  DECL_STATIC_CHAIN (outer_fn) = 1;

  memset (&id, 0, sizeof (id));

  id.src_fn = outer_fn; /* Copy FROM the function containing the spawn... */
  id.dst_fn = inner_fn; /* ...TO the wrapper */
  id.src_cfun = DECL_STRUCT_FUNCTION (outer_fn);

  id.retvar = 0; /* should be no RETURN in spawn */
  id.decl_map = wd->decl_map;
  id.copy_decl = nested ? copy_decl_no_change : copy_decl_for_cilk;
  id.block = DECL_INITIAL (inner_fn);
  id.transform_lang_insert_block = NULL; /* ? */

  /* This runs before EH. */ /* XXX maybe not any more? */
  id.eh_region = 0;
  id.eh_region_offset = 0;
  id.transform_new_cfg = true;
  id.transform_call_graph_edges = CB_CGE_MOVE;
  id.remap_var_for_cilk = true;
  id.regimplify = true; /* unused? */

  insert_decl_map (&id, wd->block, DECL_INITIAL (inner_fn));

  /* We don't want the private variables any more. */
  pointer_map_traverse (wd->decl_map,
			nested ? for_local_cb : wrapper_local_cb,
			&id);

  walk_tree (stmt_p, copy_tree_body_r, &id, NULL);

  /* See if this function can throw or calls something that should
     not be spawned.  The exception part is only necessary if
     flag_exceptions && !flag_non_call_exceptions. */
  throws = false ;
  (void)walk_tree_without_duplicates (stmt_p, check_outlined_calls, &throws);

  /* When a call is spawned gimplification will insert a detach at the
     appropriate place.  When a statement is spawned,
     build_cilk_wrapper_body inserts a detach at the start of the function. */
}

static tree
check_outlined_calls (tree *tp,
		      int *walk_subtrees ATTRIBUTE_UNUSED,
		      void *data)
{
  bool *throws = (bool *)data;
  tree t = *tp;
  int flags;

  if (TREE_CODE (t) != CALL_EXPR)
    return 0;
  flags = call_expr_flags (t);

  if (! (flags & ECF_NOTHROW) && flag_exceptions)
    *throws = true;
  if (flags & ECF_RETURNS_TWICE)
    error ("Can not spawn call to function that returns twice");
  return 0;
}

static void
install_body_with_frame_cleanup (tree fndecl, tree body)
{
  tree list;
  tree frame = make_cilk_frame (fndecl);
  tree addr = build1 (ADDR_EXPR, cilk_frame_ptr_type_decl, frame);
  tree ctor = build_call_expr (cilk_enter_fndecl, 1, addr);
  tree dtor = build_cilk_function_exit (frame, false, false);
  

  add_local_decl (cfun,frame);

  DECL_SAVED_TREE (fndecl) = (list = alloc_stmt_list ());

  append_to_statement_list_force (ctor, &list);
  append_to_statement_list_force (build_stmt (EXPR_LOCATION(body), 
					      TRY_FINALLY_EXPR, body, dtor),
				  &list);
}

struct cilk_for_desc
{
  struct wrapper_data wd;
  /* Does the loop body trigger undefined behavior at runtime? */
  bool invalid;
  /* Is the loop control variable a RECORD_TYPE? */
  bool iterator /* = false */;
  /* Does the loop range include its upper bound? */
  bool inclusive;
  /* Does the loop control variable, after converting pointer to
     machine address and taking into account sizeof pointed to
     type, increment or decrement by (plus or minus) one? */
  bool exactly_one;
  /* Is the increment stored in this structure to be added (+1)
     or subtracted (-1)? */
  signed char incr_sign;
  /* Direction is +/-1 if the increment is known to be exactly one
     in the user-visible units, +/-2 if the sign is known but the
     value is not known to be one, and zero if the sign is not known
     at compile time. */
  signed char direction;
  /* Loop upper bound.  END_EXPR is the tree for the loop bound.
     END_VAR is either END_EXPR or a VAR_DECL holding the stabilized
     value, if computation of the value has side effects. */
  tree end_expr, end_var;

  /* The originally-declared loop control variable. */
  tree var;

  /* Lower bound of the loop if it is constant enough.
     With a constant lower bound the loop body may not
     need to use the static chain to compute the iterator
     value. */
  tree lower_bound;

  /* Several types:

     The declared type of the loop control variable,
     T1 in the cilk_for spec.

     The type of the loop count and argument to loop body, currently
     always unsigned long.  (If pointers are wider, we will need a
     pointer-sized type.)

     The static type of end, T2 in the cilk_for spec.

     The difference type T3 of T1-T1, which is the same as T1 for
     integral types.  The difference type may not be wider than the
     count type.  For integers subtraction is done in count_type
     in case difference_type can't hold the range.

     If integral, the type of the increment is known to be no wider
     than var_type otherwise the truncation in
     VAR = (shorter)((longer)VAR + INCR)
     would have been rejected. */
  tree var_type, count_type, difference_type;
  tree incr;
  tree cond;

  /* The originally-declared body of the loop */
  tree body;
  /* If the user thinks he is smart, this is how smart
     the user thinks he is. */
  tree grain /* = NULL_TREE */;
  /* Context argument to generated function, if not (fdesc fn 1).  */
  tree ctx_arg;
  /* The number of loop iterations, in case the generated function
     needs to know. */
  tree count;
  /* Variables of the generated function */
  tree ctx_parm, min_parm, max_parm;
  tree var2;
};

static void declare_cilk_for_vars (struct cilk_for_desc *, tree);
static void declare_cilk_for_parms (struct cilk_for_desc *);
static tree compute_loop_var (struct cilk_for_desc *, tree, tree);

static void
init_cfd (struct cilk_for_desc *cfd)
{
  memset (cfd, 0, sizeof *cfd);
  init_wd (&cfd->wd, CILK_BLOCK_FOR);
}


/* Return
   0 if the sign of INCR_DIRECTION is unknown
   +1 if the value is exactly +1
   +2 if the value is known to be positive
   -2 if the value is known to be negative
*/
static int
compute_incr_direction (tree incr)
{
  if (TREE_CODE (incr) != INTEGER_CST)
    return tree_expr_nonnegative_p (incr) ? 2 : 0;
  else if (integer_onep (incr))
    return 1;
  else
    return 2 * tree_int_cst_sgn (incr);
}

/* Return the count type of a Cilk for loop, or NULL_TREE if
   there is no acceptable type.

   This function reports an error and should not be called
   again after returning NULL.

   The types used here need to agree with the runtime library.
   The 1.0 runtime implements only unsigned long and unsigned
   long long because arithmetic on unsigned long is not expected
   to be significantly slower than arithmetic on unsigned int. */
static tree
check_loop_difference_type (tree type)
{
  if (TREE_CODE (type) != INTEGER_TYPE)
    {
      error ("loop variable difference type %qT is not integral", type);
      return NULL_TREE;
    }

  /* The new Cilk runtime ABI supports exactly 32 and exactly 64 bits. */
  if (TYPE_PRECISION (type) > 64)
    {
      error ("loop variable difference type %qT is longer than 64 bits", type);
      return NULL_TREE;
    }

  if ((TYPE_PRECISION (type) > TYPE_PRECISION (long_unsigned_type_node))
      || (TYPE_MAIN_VARIANT (type) == long_long_integer_type_node)
      || (TYPE_MAIN_VARIANT (type) == long_long_unsigned_type_node))
    return long_long_unsigned_type_node;

  return long_unsigned_type_node;
}

static tree
tree_operand_noconv (tree exp, int n)
{
  tree op = TREE_OPERAND (exp, n);

  while (TREE_CODE (op) == NOP_EXPR)
    op = TREE_OPERAND (op, 0);

  return op;
}

static tree
cilk_simplify_tree(tree t)
{
  extern bool tree_ssa_useless_type_conversion (tree);

  if (TREE_CODE(t) == CLEANUP_POINT_EXPR)
    {
      t = TREE_OPERAND (t, 0);
    }
  if ((TREE_CODE(t) == CONVERT_EXPR) &&
      (VOID_TYPE_P (TREE_TYPE (t)) != 0))
    {
      t = TREE_OPERAND (t, 0);
    }

  STRIP_USELESS_TYPE_CONVERSION (t);

  return t;
}

static void
extract_for_fields (struct cilk_for_desc *cfd, tree loop)
{
  tree var = CILK_FOR_VAR (loop);
  tree cond = FOR_COND (loop);
  tree init = CILK_FOR_INIT (loop);
  tree incr = cilk_simplify_tree (FOR_EXPR (loop));
  tree grain = CILK_FOR_GRAIN (loop);
  tree body = FOR_BODY (loop);
  
  tree var_type, count_type, difference_type, limit;
  enum tree_code incr_op;
  bool inclusive, iterator, negate_incr, exactly_one;
  int incr_direction, cond_direction, direction;
 
  switch (TREE_CODE (cond))
    {
    case NE_EXPR:
      inclusive = false;
      cond_direction = 0;
      break;
    case GE_EXPR:
      inclusive = true;
      cond_direction = -2;
      break;
    case GT_EXPR:
      inclusive = false;
      cond_direction = -2;
      break;
    case LE_EXPR:
      inclusive = true;
      cond_direction = 2;
      break;
    case LT_EXPR:
      inclusive = false;
      cond_direction = 2;
      break;
    case EQ_EXPR:
      /* The front end rewrites (unsigned)var < 1 to var == 0.  This is a
	 silly loop but not illegal. */
      inclusive = false;
      cond_direction = 2;
      break;
    default:
      gcc_unreachable ();
      break;
    }

  if (tree_operand_noconv (cond, 0) == var)
    {
      limit = TREE_OPERAND (cond, 1);
    }
  else if (tree_operand_noconv (cond, 1) == var)
    {
      limit = TREE_OPERAND (cond, 0);
      cond_direction = -cond_direction;
    }
  else
    gcc_unreachable ();

  gcc_assert (TREE_CODE (TREE_TYPE (limit)) != ARRAY_TYPE);

  /* Leave cond undigested for now in case the loop limit expression
     has side effects. */
  var_type = TREE_TYPE (var);
  switch (TREE_CODE (var_type))
    {
    case POINTER_TYPE:
      iterator = false;
      difference_type = ptrdiff_type_node;
      break;
    case INTEGER_TYPE:
      iterator = false;
      difference_type = lang_hooks.types.type_promotes_to (TREE_TYPE (var));
      break;
    case ENUMERAL_TYPE:
    case BOOLEAN_TYPE:
      iterator = false;
      difference_type = lang_hooks.types.type_promotes_to (TREE_TYPE(var));
      break;
    case RECORD_TYPE:
    case UNION_TYPE:
      iterator = true;
    default:
      error ("cilk_for control variable must have integral or pointer type");
      difference_type = error_mark_node;
      cfd->invalid = true;
      return;
    }

  count_type = check_loop_difference_type (difference_type);
  if (count_type == NULL_TREE)
    {
      cfd->invalid = true;
      return;
    }

  /* Before the switch incr is an expression modifying VAR.
     After the switch incr is the modification to VAR with
     the sign factored out. */
  if (TREE_CODE (incr) == CLEANUP_POINT_EXPR)
    {
      incr = TREE_OPERAND (incr, 0);
    }
  
  incr_op = TREE_CODE (incr);
  switch (incr_op)
    {
    case PREINCREMENT_EXPR:
    case POSTINCREMENT_EXPR:
      negate_incr = false;
      incr_direction = 1; /* exact */
      /* implied_direction = 1; */
      incr = TREE_OPERAND (incr, 1);
      if (incr == NULL_TREE)
	{
	  if (TREE_CODE (var_type) == POINTER_TYPE)
	    incr = size_in_bytes (TREE_TYPE (var_type));
	  else
	    incr = integer_one_node;
	}
      exactly_one = integer_onep (incr);
      break;

    case PREDECREMENT_EXPR:
    case POSTDECREMENT_EXPR:
      negate_incr = true;  /* we store +1 and subtract it */
      incr_direction = -1; /* exact */
      /* implied_direction = -1; */
      incr = TREE_OPERAND (incr, 1);
      if (incr == NULL_TREE)
	{
	  if (TREE_CODE (var_type) == POINTER_TYPE)
	    incr = size_in_bytes (TREE_TYPE (var_type));
	  else
	    incr = integer_one_node;
	}
      exactly_one = integer_onep (incr);
      break;

    case MODIFY_EXPR:
      /* We don't get here unless the expression has the form
	 (modify var (op var incr)) */
      gcc_assert (TREE_OPERAND (incr, 0) == var);
      incr = TREE_OPERAND (incr, 1);
      /* again, should have checked form of increment earlier */
      if ((TREE_CODE (incr) == PLUS_EXPR) ||
	  (TREE_CODE (incr) == POINTER_PLUS_EXPR))
	{
	  tree op0 = TREE_OPERAND (incr, 0);
	  tree op1 = TREE_OPERAND (incr, 1);

	  if (op0 == var)
	    incr = op1;
	  else if (op1 == var)
	    incr = op0;
	  else
	    gcc_unreachable ();
	  negate_incr = false;
	  /* implied_direction = 1; */
	  incr_direction = compute_incr_direction (incr);
	  /* Adding a negative number treated as unsigned is
	     adding a (large) positive number.  We already
	     warned if that could be a surprise. */
	  if (TYPE_UNSIGNED (difference_type) && incr_direction < 0)
	    incr_direction = 2;
	  exactly_one = incr_direction == 1;
	  /* When adding to a pointer, scale by size of pointed-to object.
	     The parser should have generated an error if the type can't
	     be added to.  (But it threw away the tree that did the
	     addition of the scaled increment.) */
	}
      else if (TREE_CODE (incr) == MINUS_EXPR)
	{
	  tree op0 = TREE_OPERAND (incr, 0);
	  tree op1 = TREE_OPERAND (incr, 1);

	  gcc_assert (op0 == var);
	  incr = op1;
	  /* Store the amount to be subtracted.
	     Negating it could overflow. */
	  negate_incr = true;
	  /* implied_direction = -1; */
	  incr_direction = -compute_incr_direction (incr);
	  /* Subtracting a negative number treated as unsigned
	     is adding a large positive number. */
	  if (TYPE_UNSIGNED (difference_type) && incr_direction > 0)
	    incr_direction = -2;
	  exactly_one = incr_direction == -1;
	}
      else
	gcc_unreachable ();
      break;

    default:
      gcc_unreachable ();
    }

  if (TREE_CODE (incr) != INTEGER_CST && var_mentioned_p (incr, var))
    warning (OPT_Wcilk_for, "loop increment references loop variable");

  if (incr_direction == 0 && cond_direction == 0)
    warning (OPT_Wcilk_for_direction,
	     "Cilk for loop using != comparison not determined by form of incrrement");

  direction = cond_direction;

  cfd->invalid = false;
  cfd->iterator = iterator;
  cfd->inclusive = inclusive;
  cfd->exactly_one = exactly_one;
  cfd->incr_sign = negate_incr ? -1 : 1;
  cfd->direction = direction;
  cfd->end_expr = limit;
  cfd->var = var;
  cfd->lower_bound = init;
  cfd->var_type = var_type;
  cfd->count_type = count_type;
  cfd->difference_type = difference_type;
  cfd->incr = incr;
  cfd->cond = cond;
  cfd->body = body;
  cfd->grain = grain;
  cfd->ctx_arg = NULL_TREE;
  cfd->count = NULL_TREE;
}

static tree cilk_loop_convert (tree type, tree exp);

static tree
divide_count (tree count, enum tree_code op, tree incr, bool negate, tree type)
{
  tree ctype, itype, dtype; /* type of count, incr, division operation */

  if (count == NULL_TREE)
    return NULL_TREE;

  ctype = TREE_TYPE (count);
  itype = TREE_TYPE (incr);

  if (op == NOP_EXPR && !negate)
    return cilk_loop_convert (type, count);
  /* Return -(unsigned)count instead of (unsigned)-count in case the
     negate overflows. */     
  if (op == NOP_EXPR && negate)
    return fold_build1 (NEGATE_EXPR, type, cilk_loop_convert (type, count));

  /* We are dividing two positive values or else the user has invoked
     undefined behavior.  That means we can divide in a common narrow
     type and widen after.  This does not work if we must negate signed
     INCR to get a positive value because we could be negating INT_MIN.

     XXX Should change the width of INCR if int_fits_type_p

     XXX Instead of testing for pointer equality of type, should test
     for structural equality -- same bit count and signedness */
  if (ctype != itype || (negate && !TYPE_UNSIGNED (itype)))
    {
      incr = cilk_loop_convert (type, incr);
      count = cilk_loop_convert (type, count);
      dtype = type;
    }
  else
    {
      dtype = ctype;
    }

  if (negate)
    incr = fold_build1 (NEGATE_EXPR, TREE_TYPE (incr), incr);

  count = fold_build2 (op, dtype, count, incr);

  if (dtype != type)
    count = cilk_loop_convert (type, count);

  return count;
}

static tree
compute_loop_count (struct cilk_for_desc *cfd)
{
  /* All arithmetic is done in the unsigned type.  As long as
     ptrdiff_t is no wider than count_type this works for
     pointers too.  (typeck.c:pointer_diff() has the same
     possibility for overflow.) */
  const tree type = cfd->count_type;
  /* Use the initial value in the subtraction if it is
     constant enough to be stored in the control structure. */
  tree low = cfd->lower_bound ? cfd->lower_bound : cfd->var;
  /* END_VAR is a stable (side-effect free) version of END_EXPR. */
  tree high = cfd->end_var;
  const int direction = cfd->direction;
  /* INCR is the expression written on the RHS of the loop increment
     (or a variable holding the result of evaluating that expression).
     It is added or subtracted depending on the value of INCR_SIGN. */
  const int incr_sign = cfd->incr_sign;
  tree incr = cfd->incr;
  /* DIV_OP is one of
     NOP_EXPR -- Dividing by +/- 1
     EXACT_DIV_EXPR -- Loop with exact bounds
     CEIL_DIV_EXPR -- Loop that can overshoot bounds after last increment
  */
  enum tree_code div_op;
  tree count, count_up, count_down;
  tree forward = NULL_TREE;

  if (low == error_mark_node || high == error_mark_node)
    {
      gcc_assert (errorcount || sorrycount);
      return error_mark_node;
    }

  switch (direction)
    {
    case -2:
      forward = boolean_false_node;
      div_op = CEIL_DIV_EXPR;
      break;
    case -1:
      forward = boolean_false_node;
      div_op = EXACT_DIV_EXPR;
      break;
    case 0:
      forward = build2 (incr_sign > 0 ? GE_EXPR : LT_EXPR,
			boolean_type_node, incr, integer_zero_node);
      /* Loops with indeterminate direction use != and are always exact. */
      div_op = EXACT_DIV_EXPR;
      break;
    case 1:
      forward = boolean_true_node;
      div_op = EXACT_DIV_EXPR;
      break;
    case 2:
      forward = boolean_true_node;
      div_op = CEIL_DIV_EXPR;
      break;
    default:
      gcc_unreachable ();
    }

  if (cfd->exactly_one)
    div_op = NOP_EXPR;

  /* XXX We may want to call stabilize_expr, but the limit
     expression should not be complicated.  */

  count_up = NULL_TREE;
  count_down = NULL_TREE;
  if (cfd->iterator)
    {
      gcc_unreachable ();
    }
  else
    {
      tree low_type = TREE_TYPE (low), high_type = TREE_TYPE (high);
      tree sub_type;

      if (TREE_CODE (cfd->var_type) == POINTER_TYPE)
	{
	  sub_type = ptrdiff_type_node;
	}
      else
	{
	  /* We need to compute HIGH-LOW or LOW-HIGH without overflow.
	     We will eventually convert the result to the count type. */
	  sub_type = common_type (low_type, high_type);

	  /* If we are subtracting two signed variables without widening
	     convert them to unsigned.  */
	  if (!TYPE_UNSIGNED (sub_type)
	      && (TYPE_PRECISION (sub_type) == TYPE_PRECISION (low_type)
		  || TYPE_PRECISION (sub_type) == TYPE_PRECISION (low_type)))
	    sub_type = unsigned_type_for (sub_type);
	}

      if (low_type != sub_type)
	low = convert (sub_type, low);
      if (high_type != sub_type)
	high = convert (sub_type, high);

      if (direction <= 0)
	count_down = fold_build2 (MINUS_EXPR, sub_type, low, high);
      if (direction >= 0)
	count_up = fold_build2 (MINUS_EXPR, sub_type, high, low);
    }

  /* If the loop is not exact, add one before dividing.  Otherwise
     add one after dividing.  We assume this can't overflow.
     That would mean the loop range exceeds the range of the
     loop variable or difference type. */
  if (cfd->inclusive && div_op == CEIL_DIV_EXPR)
    {
      if (count_up)
	count_up = fold_build2 (PLUS_EXPR, TREE_TYPE (count_up), count_up,
				build_int_cst (TREE_TYPE (count_up), 1));
      if (count_down)
	count_down = fold_build2 (PLUS_EXPR, TREE_TYPE (count_down), count_down,
				  build_int_cst (TREE_TYPE (count_down), 1));
    }

  /* For unsigned loops we could truncate INCR to the precision
     of the loop variable here to give more undefined loops
     serial semantics.

     cilk_for (unsigned short i = 0; i < 100; i += 65537) ;

     This is wrong for signed loops because the loop increment
     can legitimately exceed the range of the type, as in

     cilk_for (unsigned short i = -32768; i < 1; i += 32769) ;

     and signed loops which need this truncation have undefined
     serial behavior. */
  if (false && TYPE_UNSIGNED (cfd->var_type)
      && TYPE_PRECISION (TREE_TYPE (incr)) > TYPE_PRECISION (cfd->var_type))
    incr = fold_build1 (NOP_EXPR, cfd->var_type, incr);

  /* Serial semantics: INCR is converted to the common type
     of VAR and INCR then the result is converted to the type
     of VAR.  If the second conversion truncates Cilk says the
     behavior is undefined.  Do the first conversion to spec. */

  if (!cfd->iterator && TREE_CODE (TREE_TYPE (cfd->var)) != POINTER_TYPE)
    incr = cilk_loop_convert
      (common_type /*type_after_usual_arithmetic_conversions*/
       (TREE_TYPE (cfd->var), TREE_TYPE (incr)),
       incr);

  /* Now separately divide each count by +/-INCR yielding
     a value with type TYPE. */
  count_up = divide_count (count_up, div_op, incr, incr_sign < 0, type);
  count_down = divide_count (count_down, div_op, incr, incr_sign > 0, type);

  /* Merge the forward and backward counts */
  if (count_up == NULL)
    count = count_down;
  else if (count_down == NULL)
    count = count_up;
  else
    count = fold_build3 (COND_EXPR, type, forward, count_up, count_down);

  /* Add one, maybe */
  if (cfd->inclusive && div_op != CEIL_DIV_EXPR)
    count = fold_build2 (PLUS_EXPR, type, count, build_int_cst (type, 1));

  return count;
}

/*
  The loop function looks like

  body(void *, unsigned long min, unsigned long max)
  const T start = [outer_context] var;
  T var';
  for (unsigned long i = min; i < max; i++) {
  var' = start + (T)i * (T)incr;
  body(var');
  }

  COMPUTE_LOOP_VAR returns an expression for
  var' = start + i * incr;
  or
  var' = start - i * decr;
  with suitable type conversions.

  If direction is known we know the sign of INCR (or else it's
  undefined behavior) and we can work with positive unsigned
  numbers until the last addition or subtraction.

  If direction is not known then the increment and loop variable
  are signed but the product of the loop count and increment may
  not be representable as a signed value.

  We can't do the last addition or subtraction in C without
  a conditional operation because the conversion of unsigned
  to signed is undefined for "negative" values of the unsigned
  number.  For now we just pretend this isn't a problem.  We
  may fail on targets with signed overflow.

  For iterator loops we require that the difference type have
  enough range and simply pass the value to operator+ or operator-
  based on the static direction of the loop.

  LOOP_VAR has type COUNT_TYPE.
*/

static tree
compute_loop_var (struct cilk_for_desc *cfd, tree loop_var, tree lower_bound)
{
  tree incr = cfd->incr;
  tree count_type = cfd->count_type;
  tree scaled, adjusted;
  int incr_sign = cfd->incr_sign;
  enum tree_code add_op = incr_sign >= 0 ? PLUS_EXPR : MINUS_EXPR;

  gcc_assert (TYPE_MAIN_VARIANT (TREE_TYPE (loop_var)) ==
	      TYPE_MAIN_VARIANT (count_type));

  /* Compute an expression to be added or subtracted.

     We want to add or subtract LOOP_VAR * INCR.  INCR may be negative.
     If the static direction is indeterminate we don't know that at
     compile time.  The code to convert to unsigned and multiply does
     the right thing in the end.  For iterator loops we don't need to
     go to that trouble, but scalar loops can have range that can not
     be represented in the signed loop variable. */
  if (integer_onep (incr))
    {
      scaled = loop_var;
    }
  else
    {
      tree incr = cilk_loop_convert (count_type, cfd->incr);
      scaled = fold_build2 (MULT_EXPR, count_type, loop_var, incr);
    }

  if (cfd->iterator)
    {
      tree low;
      tree exp;
      
      /* convert LOOP_VAR to T3 (difference_type) so that
	 operator+(T1, T3)
	 is preferred over
	 operator+(T1, count_type)

	 operator+ constructs the object if it returns by value.

	 Use operator- if the user wrote -=. */
      if (count_type != cfd->difference_type)
	loop_var = convert (cfd->difference_type, scaled);
      low = lower_bound ? lower_bound : cfd->var;
      exp = build2 (add_op, TREE_TYPE (loop_var), low, loop_var);
      gcc_assert (exp != error_mark_node);
      exp = build_modify_expr (UNKNOWN_LOCATION, cfd->var2, 
			       TREE_TYPE(cfd->var2), INIT_EXPR, 
			       UNKNOWN_LOCATION, exp, TREE_TYPE(exp));
      gcc_assert (exp != error_mark_node);
      return exp;
    }

  /* The scaled count may not be representable in the type of the
     loop variable, e.g. if the loop range is INT_MIN+1 to INT_MAX-1
     the range does not fit in a signed int.  The sum of the lower
     bound and the count is representable.  Do the addition or
     subtraction in the wider type, then narrow. */
  adjusted = fold_build2 (add_op, count_type,
			  cilk_loop_convert (count_type, lower_bound),
			  scaled);

  return build2 (MODIFY_EXPR, void_type_node,
		 cfd->var2, cilk_loop_convert (cfd->var_type, adjusted));
}

static tree
cilk_loop_convert (tree type, tree exp)
{
  enum tree_code code;
  int inprec, outprec;
  if (type == TREE_TYPE (exp))
    return exp;
  inprec = TYPE_PRECISION (TREE_TYPE (exp));
  outprec = TYPE_PRECISION (type);
  if (outprec > inprec && !TYPE_UNSIGNED (TREE_TYPE (exp)))
    code = CONVERT_EXPR;
  else
    code = NOP_EXPR;
  return fold_build1 (code, type, exp);
}

static tree
build_cilk_for_body (struct cilk_for_desc *cfd)
{
  const tree outer = current_function_decl;
  tree loop_body;
  tree fndecl;
  tree body, block;
  tree lower_bound;
  tree loop_var;
  tree count_type;
  tree tempx,tempy;
  declare_cilk_for_parms (cfd);

  cfd->wd.fntype = build_function_type (void_type_node, cfd->wd.argtypes);

  fndecl = build_cilk_helper_decl (&cfd->wd, NULL);

  push_struct_function (fndecl);
  current_function_decl = fndecl;

  declare_cilk_for_vars (cfd, fndecl);

  /* XXX Can't make any gimplify calls here because the function
     must be generated as pure tree code. */

  body = push_stmt_list ();

  /* In Cilk++ 1.1 the loop bound was copied into a variable.
     The separation between TREE and GIMPLE makes that impossible.
     If the lower bound is constant, use it.  Otherwise make an
     uplevel reference to the parent function. */
  lower_bound = cfd->lower_bound;
  if (lower_bound == NULL_TREE)
    {
      tree hack;
      lower_bound = cfd->var;
      /* Any reference to VAR will be remapped to the local
	 copy.  But we need to reference the original to
	 compute the local copy.  So make a fake variable
	 and insert it in the decl map to remap to the
	 original variable, and hope remapping only runs
	 once.

	 This is ugly.
      */
      hack = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
			 TREE_TYPE (lower_bound));
      
      /* Just in case somebody cares */
      DECL_CONTEXT (hack) = DECL_CONTEXT (lower_bound);
      DECL_NAME (hack) = DECL_NAME (lower_bound);
      *pointer_map_insert (cfd->wd.decl_map, hack) = lower_bound;
      lower_bound = hack;
    }
  loop_var = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
			 TREE_TYPE (cfd->min_parm));
  DECL_CONTEXT (loop_var) = fndecl;
  add_stmt (build2 (INIT_EXPR, void_type_node, loop_var, cfd->min_parm));

  count_type = cfd->count_type;
  gcc_assert (TYPE_MAIN_VARIANT (TREE_TYPE (loop_var)) ==
	      TYPE_MAIN_VARIANT (count_type));

  /* The new loop body is

     var2 = (T)((control variable) * INCR + (lower bound));

  */

  loop_body = 0;
  append_to_statement_list (compute_loop_var (cfd, loop_var, lower_bound),
			    &loop_body);
  append_to_statement_list (cfd->body, &loop_body);
  loop_body = fold_build_cleanup_point_expr (void_type_node, loop_body);
  /* Making a BIND_EXPR doesn't work so pretend gimplify saw one. */
  DECL_SEEN_IN_BIND_EXPR_P (cfd->var2) = 1;

  /* Like c_finish_loop, but not using C front end.  Note that
     these loops are always executed at least once.  */
  {
    tree loop = push_stmt_list ();
    /* create_artificial_label would set wrong DECL_CONTEXT. */
    tree lab = build_decl (UNKNOWN_LOCATION,LABEL_DECL, NULL_TREE,
			   void_type_node);
    tree top = build1 (LABEL_EXPR, void_type_node, lab);
    DECL_ARTIFICIAL (lab) = 0;
    DECL_IGNORED_P (lab) = 1;
    DECL_CONTEXT (lab) = fndecl;

    /* top: body
       var = var + 1
       if var < limit goto top */
    add_stmt (top);

    cilk_outline (fndecl, &loop_body, &cfd->wd);

    add_stmt (loop_body);

    tempx = build2 (MODIFY_EXPR, void_type_node, loop_var,
		    build2 (PLUS_EXPR, count_type,
			    loop_var,
			    build_int_cst (count_type, 1)));
    add_stmt(tempx);
    
    tempy = build3 (COND_EXPR, void_type_node,
		    build2 (LT_EXPR, boolean_type_node, loop_var,
			    cfd->max_parm),
		    build1 (GOTO_EXPR, void_type_node, lab),
		    build_empty_stmt (UNKNOWN_LOCATION));

    add_stmt(tempy);
    pop_stmt_list (loop);

    add_stmt (loop);
  }

  body = pop_stmt_list (body);
  block = DECL_INITIAL (fndecl);
  BLOCK_VARS (block) = loop_var;
  body = build3 (BIND_EXPR, void_type_node, loop_var, body, block);
  TREE_CHAIN (loop_var) = cfd->var2;
  if (contains_spawn (body))
    install_body_with_frame_cleanup (fndecl, body);
  else
    DECL_SAVED_TREE (fndecl) = body;

  pop_cfun_to (outer); /* undo push_struct_function */

  return fndecl;
}

static tree
gimplify_cilk_for_2 (struct cilk_for_desc *cfd,
		     gimple_seq *pre_p ATTRIBUTE_UNUSED,
		     gimple_seq *post_p ATTRIBUTE_UNUSED)
{
  tree old_cfd = current_function_decl;
  tree fn;
  tree incr = cfd->incr;
  tree var = cfd->var;
  tree grain = NULL_TREE;
 

  if (POINTER_TYPE_P(TREE_TYPE(var)))
    {
      extract_free_variables (cfd->lower_bound, &cfd->wd, ADD_WRITE);
    }
  else
    {
      extract_free_variables (cfd->lower_bound, &cfd->wd, ADD_READ);
    }

  
  /* The increment expression is read. */
  extract_free_variables (incr, &cfd->wd, ADD_READ);


  if (cfd->grain != NULL_TREE)
    {
      grain = get_formal_tmp_var (cfd->grain, pre_p);
    }
  else
    {
      grain = NULL_TREE;
    }
  cfd->grain = grain;
  
  /* Map the loop variable to integer_minus_one_node if we won't really
     be passing it to the loop body and integer_zero_node otherwise.

     If the map ends up integer_one_node then somebody wrote to the loop
     variable and that's a user error.
     The correct map will be installed in declare_for_loop_variables. */
  *pointer_map_insert (cfd->wd.decl_map, var)
    = (void *) (cfd->lower_bound ? integer_minus_one_node : integer_zero_node);
  extract_free_variables (cfd->body, &cfd->wd, ADD_READ);
  /* Note that variables are not extracted from the loop condition
     and increment.  They are evaluated, to the extent they are
     evaluated, in the context containing the for loop. */

  fn = build_cilk_for_body (cfd);

  DECL_UNINLINABLE (fn) = 1;

  current_function_decl = old_cfd;
  set_cfun (DECL_STRUCT_FUNCTION (current_function_decl));

  cfun->is_cilk_function = 1;
  
  /* Apparently we need to gimplify now because we can't leave
     non-GIMPLE functions lying around. */
  cg_hacks (fn, &cfd->wd); 

  /*debug_function(fn, 0);*/

  /* TODO: This function could be marked "hot."  It's not clear if that
     is worth doing when the rest of the program isn't being profiled. */

  return fn;
}

/* Create or discover the variable to be used in the loop termination
   condition.  Return true if the cfd->end_var should be used in the
   guard test around the runtime call.  Otherwise the guard test uses
   the complex expression, which in C++ may initialize the variable.

   For example, if END_EXPR is

   (target_expr limit (call constructor ...))

   the variable limit is not initialized until the target_expr is
   evaluated. */

static bool
cilk_for_end (struct cilk_for_desc *cfd, gimple_seq *pre_p)
{
  tree end = cfd->end_expr;
  if (TREE_SIDE_EFFECTS (end))
    {
      enum tree_code ecode = TREE_CODE (end);
      if (ecode == INIT_EXPR || ecode == MODIFY_EXPR || ecode == TARGET_EXPR)
	{
	  cfd->end_var = TREE_OPERAND (end, 0);
	  return false;
	}
      else
	{
	  /* Copy the result of evaluating the expression into a variable.
	     The compiler will probably crash if there's anything
	     complicated in it -- a complicated value needs to go through
	     the other branch of this IF using an explicit temporary. */

	  /* This will crash if the type is addressable.  The front
	     end should have generated one of the initialization trees
	     handled above. */
	  cfd->end_var = get_formal_tmp_var (end, pre_p);
	  return true;
	}
    }
  cfd->end_var = end;
  return false;
}

static void
gimplify_cilk_for_1 (struct cilk_for_desc *cfd,
		     gimple_seq *pre_p,
		     gimple_seq *post_p ATTRIBUTE_UNUSED)
{
  tree cond, op0, op1, count, ctx;
  tree libfun_expr;
  tree fn, libfun;
  tree grain = NULL_TREE;
  bool end_expr_initializes;

  /* INNER_SEQ contains evaluation of variables holding loop
     increment and count.  These are evaluated inside the loop
     guard.  Due to a conflict between GIMPLE and TREE format
     these statements have to be saved then spliced in where
     they belong. */
  gimple_seq inner_seq = 0;

  /* Control variable _initialization_ has been gimplified.
     TODO: emit _assignment_ here if variable not declared in loop */

  end_expr_initializes = cilk_for_end (cfd, pre_p);

  /* We don't have to evaluate INCR only once, but we do have
     to evaluate it no more times than in the serial loop.
     The naive method evaluates INCR exactly that many times
     except if the static loop direction is indeterminate.

     Storing the increment in a variable is thus mandatory
     if cfd.direction == 0.  It is an optimization otherwise
     and there seems no harm and some benefit in doing it.

     The evaluation is on the inner statement list.  The
     increment can not be referenced prior to the loop test. */
  if (TREE_SIDE_EFFECTS (cfd->incr))
    sorry ("cilk_for increment with side effects");

  /* Now that we have the final form of the loop limit, construct
     the loop condition. */
  cond = cfd->cond;
  op0 = TREE_OPERAND (cond, 0);
  op1 = TREE_OPERAND (cond, 1);
  /* If we created a temporary variable to hold the increment
     expression, use it here.  If we found a previously declared
     temporary variable do not use it here because evaluating the
     expression initializes the variable. */
  if (!end_expr_initializes && cfd->end_var != cfd->end_expr)
    {
      if (op1 == cfd->end_expr)
	op1 = cfd->end_var;
      else if (op0 == cfd->end_expr)
	op0 = cfd->end_var;
    }

  cond = fold_build2 (TREE_CODE (cond), boolean_type_node, op0, op1);

  /* COND will be added to OUTER_LIST later when the loop expression
     is emitted inside a guard.  CFD.END_VAR will be valid inside
     the condition.  Don't gimplify it yet, because it can only
     be gimplified once. */

  /* Compute the count here so the callback can see it, but save
     the initialization in the statement list that is conditional
     on the loop. */
  count = compute_loop_count (cfd);
  if (!TREE_CONSTANT (count))
    {
      /* Note that this will cause double destruction if we
	 accidentally let the TARGET_EXPR for END_EXPR leak
	 out. */
      count = fold_build_cleanup_point_expr (TREE_TYPE (count), count);
      /* We can't call get_formal_tmp_var on count here because
	 COUNT references END_VAR, and we may not have seen the
	 initializer for it yet.  (It won't be seen until the
	 whole big for is gimplified.) */
      count = get_formal_tmp_var (count, &inner_seq);
    }

  fn = gimplify_cilk_for_2 (cfd, pre_p, post_p);

  switch (TYPE_PRECISION (cfd->count_type))
    {
    case 32:
      libfun = cilk_for_32_fndecl;
      break;
    case 64:
      libfun = cilk_for_64_fndecl;
      break;
    default:
      gcc_unreachable ();
    }

  if (cfd->ctx_arg)
    {
      /* "context" is an ordinary pointer */
      ctx = cfd->ctx_arg;
      if (TREE_TYPE (ctx) != ptr_type_node)
	ctx = build1 (NOP_EXPR, ptr_type_node, ctx);
      if (! DECL_P (ctx))
	ctx = get_formal_tmp_var (ctx, &inner_seq);
      fn = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (fn)), fn);
    }
  else
    {
      gcc_assert (fn);
      ctx = build1 (ADDR_EXPR,  build_pointer_type (TREE_TYPE(fn)), fn);
      /* CTX and FN must be copied to variables because the nested
	 function module will not replace FDESC_EXPR inside a CALL_EXPR.  */
      ctx = get_formal_tmp_var (ctx, &inner_seq);
      fn = build1 (ADDR_EXPR, build_pointer_type(TREE_TYPE(fn)), fn);
    }

  TREE_CONSTANT (fn) = 1;
  fn = get_formal_tmp_var (fn, &inner_seq);


  if (cfd->grain == NULL_TREE)
    {
      grain = get_formal_tmp_var (build_int_cst(cfd->count_type, 0), pre_p);
    }
  else if (TYPE_MAIN_VARIANT(TREE_TYPE(cfd->grain)) !=
	   TYPE_MAIN_VARIANT(cfd->count_type))
    {
      grain = convert (cfd->count_type, cfd->grain);
    }

  cfd->grain = grain;


  
  /* Build the condition now, because if the loop limit has side
     effects the variable was not known earlier. */  
  libfun_expr = fold_build_cleanup_point_expr
    (void_type_node,
     build3 (COND_EXPR, void_type_node, cond,
	     build_call_expr (libfun, 4, fn, ctx, count, grain),
	     build_empty_stmt (UNKNOWN_LOCATION)));
  /* Cilk++ used fold_build_cleanup_point_expr to handle cleanups for
     the loop setup.  Unfortunately that handling is impossible with
     the new distinction between TREE and GIMPLE.  For now C++ cilk_for
     will not work if any class objects with destructors are used to
     compute the loop control variable, limit, or increment. */
  gimplify_and_add (libfun_expr, &inner_seq);

  gimple_seq_add_seq (pre_p, inner_seq);

  /* XXX TODO: If loop variable is visible outside the loop, assign
     its final value. */

  /* XXX Add cgraph edge to libfun? */
}

static tree
block_for_loop (tree loop)
{
  tree *block = tree_block (loop);
  if (block)
    return *block;
  return DECL_INITIAL (current_function_decl);
}


void
gimplify_cilk_for (tree *expr_p,
		   gimple_seq *pre_p ATTRIBUTE_UNUSED,
		   gimple_seq *post_p ATTRIBUTE_UNUSED)
{
  struct cilk_for_desc cfd;
  tree init_expr = NULL_TREE;
  tree loop = *expr_p;

  *expr_p = NULL_TREE;

  cfun->is_cilk_function = 1;
  
  init_cfd (&cfd);

  cfd.wd.block = block_for_loop (loop);

  gcc_assert (cfd.wd.context == current_function_decl);

  extract_for_fields (&cfd, loop);

  
  init_expr = build2(MODIFY_EXPR, void_type_node, CILK_FOR_VAR(loop),
		     CILK_FOR_INIT(loop)); 
  gimplify_and_add(init_expr, pre_p);
  
  if (!cfd.invalid)
    gimplify_cilk_for_1 (&cfd, pre_p, post_p);

  return;

}

/* Callback to ensure that the loop control variable is not mentioned
   in the loop condition.  */

static tree
var_mentioned_p_cb (tree *tp, int *walk_subtrees, void *var)
{
  tree t = *tp;
  
  /* VAR is allowed to be mentioned in an unevaluated context.
     No check is needed here.

     sizeof, alignof, and typeof have already been converted to
     integer constants and need not be tested.

     The remaining unevaluated context is typeid on an expression
     that is not an lvalue of non-polymorphic type.  There is
     probably no way to use typeid expression in the increment of
     a legitimate cilk_for loop.  */
  if (t == (tree)var)
    return t;
  if (TREE_CODE_CLASS (TREE_CODE (t)) == tcc_type)
    *walk_subtrees = 0;
  return NULL_TREE;
}

static bool
var_mentioned_p (tree exp, tree var)
{
  return (walk_tree (&exp, var_mentioned_p_cb, var, 0) != NULL_TREE);
}

/* Set up the signature and parameters of the cilk_for body function
   before declaring the function. */
static void
declare_cilk_for_parms (struct cilk_for_desc *cfd)
{
  tree types, t1, t2;
  tree max_parm, min_parm, ctx, ro_count, count_type;

  count_type = cfd->count_type;
  ro_count = build_qualified_type (count_type, TYPE_QUAL_CONST);
  ctx = build_decl (UNKNOWN_LOCATION, PARM_DECL, NULL_TREE, ptr_type_node);
  t1=get_identifier ("__low");
  min_parm = build_decl (EXPR_LOCATION (t1), PARM_DECL, t1, ro_count);
  t2=get_identifier ("__high");
  max_parm = build_decl (EXPR_LOCATION (t2), PARM_DECL, t2, ro_count);

  DECL_ARG_TYPE (max_parm) = count_type;
  DECL_ARTIFICIAL (max_parm) = 1; /* "Yes, please optimize me." */
  TREE_READONLY (max_parm) = 1;

  DECL_ARG_TYPE (min_parm) = count_type;
  DECL_ARTIFICIAL (min_parm) = 1; /* "Yes, please optimize me." */
  TREE_READONLY (min_parm) = 1;

  DECL_ARG_TYPE (ctx) = ptr_type_node;
  DECL_ARTIFICIAL (ctx) = 1;
  TREE_READONLY (ctx) = 1;

  TREE_CHAIN (min_parm) = max_parm;
  TREE_CHAIN (ctx) = min_parm;

  types = tree_cons (NULL_TREE, TREE_TYPE (max_parm), void_list_node);
  types = tree_cons (NULL_TREE, TREE_TYPE (min_parm), types);
  types = tree_cons (NULL_TREE, TREE_TYPE (ctx), types);

  cfd->min_parm = min_parm;
  cfd->max_parm = max_parm;
  cfd->wd.argtypes = types;
  cfd->wd.arglist = NULL_TREE; /* not used -- not called directly */
  cfd->wd.parms = ctx;
}

/* Set up the variable mapping for the cilk_for body function
   and install parameters after declaring the function and
   scanning the loop body's variable use. */
static void
declare_cilk_for_vars (struct cilk_for_desc *cfd, tree fndecl)
{
  void **mapped;
  const_tree t;
  tree p;
  tree var2;

  var2 = build_decl (UNKNOWN_LOCATION, VAR_DECL, get_identifier("Balaji"),
		     cfd->var_type);
  
  DECL_CONTEXT (var2) = fndecl;
  cfd->var2 = var2;

  mapped = pointer_map_contains (cfd->wd.decl_map, cfd->var);
  /* loop control variable must be mapped */
  gcc_assert (mapped);
  t = (const_tree)*mapped;

  /* The loop control variable may appear as mapped to itself
     or mapped to integer_one_node depending on its type and
     how it was modified. */
  if ((TREE_CODE (t) != INTEGER_CST) || (t == integer_one_node))
    {
      tree save_function = current_function_decl;
      current_function_decl = DECL_CONTEXT (cfd->var);
      warning (0, "loop body modifies control variable %qD", cfd->var);
      current_function_decl = save_function;
    }
  *mapped = (void *) var2;

  p = cfd->wd.parms;
  DECL_ARGUMENTS (fndecl) = p;
  do
    {
      DECL_CONTEXT (p) = fndecl;
      p = TREE_CHAIN (p);
    }
  while (p);
}
