/* C++ Functions to handle Intel(R) Cilk(TM) Plus Specific functions.
   Copyright (C) 2011  Free Software Foundation, Inc.
   Written by Balaji V. Iyer <balaji.v.iyer@intel.com>,
   Intel Corporation.

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
#include "tree-iterator.h"
#include "gimple.h"
#include "tree-inline.h"
#include "cilk.h"
#include "langhooks.h"
#include "cp-tree.h"
#include "output.h"
#include "rtl.h"
#include "insn-flags.h"
#include "cgraph.h"

int called_cilk_init_builtin=0;
int cilk_worker_regno=CILK_WORKER_INVALID;

HOST_WIDE_INT cilk_field_offsets[CILK_TI_MAX];
static GTY(()) rtx cilk_dynamic_libfunc;

tree cilk_wrappers;

enum add_variable_type {
  ADD_READ,	/* reference to previously-defined variable */
  ADD_BIND,	/* definition of new variable in inner scope */
  ADD_WRITE	/* write to possibly previously-defined variable */
};
enum add_variable_context {
  CILK_BLOCK_CALL = 30,
  CILK_BLOCK_BLOCK,
  CILK_BLOCK_RUN,
  CILK_BLOCK_FOR
};

/* ****************************************************************
 * cilk_for
 **************************************************************** */

/* This structure is not tagged for GC because it should not be
   hold values across a GC pass.  GC is disabled during generation
   of nested functions. */
struct cilk_for_desc
{
  /* May the loop use a true nested function with static chain? */
  bool nested_ok;
  /* True if the body function can't be demoted. */
  bool no_demote;
  /* Is the loop control variable a RECORD_TYPE? */
  bool iterator;
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
     END_VAR is a VAR_DECL holding the value, if computation of
     the value has side effects. */
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
  tree grain;
  /* Context argument to generated function, if not (fdesc fn 1).  */
  tree ctx_arg;
  /* The number of loop iterations, in case the generated function
     needs to know. */
  tree count;
  /* Variables of the generated function */
  tree ctx_parm, min_parm, max_parm;
  tree var2;

  /* Replacements for variables in loop body. */
  /* splay_tree var_map; */ /* replace splays with pointer maps */

  struct pointer_map_t *decl_map;
};

void gimplify_cilk_for_stmt(tree *for_p, gimple_seq *pre_p);
static tree compute_loop_var (struct cilk_for_desc *, tree, tree);
static bool cp_extract_for_fields (struct cilk_for_desc *cfd, tree for_stmt);

void cilk_outline (tree outer_fn, tree inner_fn, tree *stmt_p,
		   struct pointer_map_t *decl_map,
		   enum add_variable_context ctx);
bool in_cilk_function(void);
bool cilk_validate_for(tree c_for_stmt);
bool is_cilk_function_type(tree fntype ATTRIBUTE_UNUSED);
void cilk_erase_for(tree c_for_stmt);
tree cilk_get_worker_tree (bool for_call);
enum tls_model cilk_tls_model (tree decl ATTRIBUTE_UNUSED);

bool is_cilk_function_type(tree fntype ATTRIBUTE_UNUSED)
{
  return true;
}

bool in_cilk_function(void)
{
  if (current_function_decl==NULL_TREE)
    return false;
  else
    return is_cilk_function_type(current_function_decl);
}


static tree
cilk_c_declare_looper (const char *name, tree type)
{
  tree cb, ft, fn;

  /*push_namespace(get_identifier("cilk")); */
  
  cb = build_function_type_list (void_type_node,
                                 ptr_type_node, type, type,
                                 NULL_TREE);
  cb = build_pointer_type (cb);

  ft = build_function_type_list (void_type_node,
                                 cb, ptr_type_node, type,
                                 integer_type_node, NULL_TREE);
  fn = build_fn_decl (name, ft);
  TREE_NOTHROW (fn) = 0;

  /* pop_namespace(); */
  return fn;
}




static void initialize_cilk_for_desc (struct cilk_for_desc *cfd)
{
  /* gcc seems to assume that memset initializes pointers to NULL. */
  memset (cfd, 0, sizeof *cfd);

  cfd->decl_map= pointer_map_create();

}

static void release_cilk_for_desc (struct cilk_for_desc *cfd)
{
  pointer_map_destroy(cfd->decl_map);
  /*c fd->var_map = 0; */
  cfd->decl_map=NULL;
}

static bool
add_incr (tree incr)
{
  switch (TREE_CODE (incr))
    {
    case PLUS_EXPR:
    case PREINCREMENT_EXPR:
    case POSTINCREMENT_EXPR:
      return true;
    case MINUS_EXPR:
    case PREDECREMENT_EXPR:
    case POSTDECREMENT_EXPR:
      return false;
    case CLEANUP_POINT_EXPR:
    case NOP_EXPR:
      return add_incr (TREE_OPERAND (incr, 0));
    case MODIFY_EXPR:
      return add_incr (TREE_OPERAND (incr, 1));
    default:
      gcc_unreachable ();
    }
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



static bool
for_local_cb (const void *k_v, void **vp, void *p)
{
  tree k = (tree) k_v;
  tree v = (tree) *vp;


  if (k == v)
    return true;
  
  if (v == error_mark_node)
    *vp = copy_decl_no_change(k,(copy_body_data *)p);
  
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


static bool
wrapper_local_cb (const void *k_v, void **vp, void *data)
{
  copy_body_data *id = (copy_body_data *)data;
  tree key = (tree)k_v;
  tree val = (tree)*vp;

  if (val == error_mark_node)
    *vp=copy_decl_for_cilk(key,id);
  
  return true;
}


/* Alter a tree STMT from OUTER_FN to form the body of INNER_FN. */
void
cilk_outline (tree outer_fn, tree inner_fn, tree *stmt_p,
	      struct pointer_map_t *decl_map, enum add_variable_context ctx)
{
  const bool nested = (ctx == CILK_BLOCK_FOR || ctx == CILK_BLOCK_RUN);
  copy_body_data id;
  bool throws;
  tree block=NULL_TREE;

  memset (&id, 0, sizeof (id));

  id.src_fn = outer_fn; /* Copy FROM the function containing the spawn... */
  id.dst_fn = inner_fn; /* ...TO the wrapper */
  id.src_cfun = DECL_STRUCT_FUNCTION (outer_fn);

  id.retvar = 0; /* should be no RETURN in spawn */
  id.decl_map = decl_map;
  id.copy_decl = nested ? copy_decl_no_change : copy_decl_for_cilk;
  id.block = 0;
  id.transform_lang_insert_block = NULL; /* ? */
  /* This runs before cgraph and EH. */
  id.src_node = 0;
  id.dst_node = 0;
  id.eh_region = -1;
  id.eh_region_offset = 0;
  id.transform_new_cfg = true;
  id.transform_call_graph_edges = CB_CGE_DUPLICATE;
  id.remap_var_for_cilk = true;



  insert_decl_map(&id,block,DECL_INITIAL(inner_fn));
  pointer_map_traverse(decl_map, nested ? for_local_cb : wrapper_local_cb,
		       &id);

  /* bviyer: FIX THIS!! */
  walk_tree (stmt_p, copy_tree_body_r, &id, NULL);
  

  /* See if this function can throw or calls something that should
     not be spawned.  The exception part is only necessary if
     flag_exceptions && !flag_non_call_exceptions. */
  throws = cp_function_chain->can_throw;
  (void)walk_tree_without_duplicates (stmt_p, check_outlined_calls, &throws);
  cp_function_chain->can_throw = throws;

  /* When a call is spawned gimplification will insert a detach at the
     appropriate place.  When a statement is spawned,
     build_cilk_wrapper_body inserts a detach at the start of the function. */
}



static void
declare_for_loop_variables (struct cilk_for_desc *cfd, tree fndecl)
{
  tree ro_count = build_qualified_type (cfd->count_type, TYPE_QUAL_CONST);
  /* splay_tree_node n; */
  tree sc_parm, min_parm, max_parm;
  tree var2;
  void **mapped;
  tree low_var,high_var,sc_var;
  tree contextMapper=NULL_TREE;
  tree t = NULL_TREE;
  
  low_var=get_identifier("__low");
  high_var=get_identifier("__high");
  sc_var=get_identifier("__sc_var");
  max_parm = build_decl (UNKNOWN_LOCATION, PARM_DECL, high_var, ro_count);
  DECL_ARG_TYPE (max_parm) = cfd->count_type;
  DECL_ARTIFICIAL (max_parm) = 1; /* "Yes, please optimize me." */
  TREE_READONLY (max_parm) = 1;
  cfd->max_parm = max_parm;

  min_parm = build_decl (UNKNOWN_LOCATION,PARM_DECL, low_var, ro_count);
  DECL_ARG_TYPE (min_parm) = cfd->count_type;
  DECL_ARTIFICIAL (min_parm) = 1;
  TREE_READONLY (min_parm) = 1;
  TREE_CHAIN (min_parm) = max_parm;
  cfd->min_parm = min_parm;

  sc_parm = cfd->ctx_parm;
  if (sc_parm == NULL_TREE)
    {
      sc_parm = build_decl (UNKNOWN_LOCATION, PARM_DECL, sc_var,
			    ptr_type_node);
      DECL_ARG_TYPE (sc_parm) = ptr_type_node;
      DECL_ARTIFICIAL (sc_parm) = 1;
      TREE_READONLY (sc_parm) = 1;
      cfd->ctx_parm = sc_parm;
    }
  TREE_CHAIN (sc_parm) = min_parm;

  var2 = build_decl (EXPR_LOCATION(cfd->var), VAR_DECL,
		     DECL_NAME(cfd->var), cfd->var_type);
  DECL_CONTEXT (var2) = fndecl;
  cfd->var2 = var2;
 
  mapped = pointer_map_contains(cfd->decl_map,cfd->var);
  gcc_assert(mapped);

  t = (const tree)*mapped;
  /* The loop control variable may appear as mapped to itself
     or mapped to integer_one_node depending on its type and
     how it was modified. */
  if (TREE_CODE (t) != INTEGER_CST ||  t == integer_one_node)
    {
      tree save_function = current_function_decl;
      current_function_decl = DECL_CONTEXT (cfd->var);
      warning (0, "loop body modifies control variable %qD", t);
      current_function_decl = save_function;
    }
  /*
    n->value = (splay_tree_value) var2;
  */
  *mapped = (void *)var2;


  contextMapper = sc_parm;
  DECL_ARGUMENTS(fndecl)=contextMapper;

  do {
    DECL_CONTEXT(contextMapper)=fndecl;
    contextMapper=TREE_CHAIN(contextMapper);
  } while (contextMapper != NULL_TREE);
  
}


static tree
cp_build_cilk_for_body (struct cilk_for_desc *cfd)
{
  tree outer = current_function_decl;
  tree loop_body;
  char *name=NULL;
  static int counter;
  tree fndecl, fntype;
  tree body;
  tree fn_args;
  tree lower_bound;
  tree loop_var;
  tree cleanup;
  tree count_type;
  tree pre, hack = NULL_TREE;
  gimple_seq pre_seq=NULL;
  enum function_linkage linkage;
  struct gimplify_ctx gctx;
  expanded_location file_location;
  char *function_name;
  char *cc=NULL;
  char *dd=NULL;
  /* tree local_lower_bound = NULL_TREE; */
  tree loop_end_comp = NULL_TREE;
  /*tree new_body = NULL_TREE; */
  
  /* see also synthesize_method */
  /* bviyer: removed the outer and chagned pushed_function_context_to */
  push_function_context ();

  fn_args = tree_cons (cfd->min_parm, long_unsigned_type_node, void_list_node);
  fn_args = tree_cons (cfd->max_parm, long_unsigned_type_node, fn_args);
  fn_args = tree_cons (NULL_TREE, ptr_type_node, fn_args);

  /* Manually demote to C++ if the function does not use Cilk.
     The loop body is not a local function that may be demoted
     later. */
  if (cfd->no_demote || cp_tree_uses_cilk (cfd->body))
    linkage = linkage_cilk;
  else
    linkage = linkage_native;


  fntype = build_function_type(void_type_node, fn_args);
  SET_FUNCTION_TYPE_LINKAGE(fntype,linkage);

  if (IDENTIFIER_POINTER(DECL_NAME(outer)) != NULL)
    {
      function_name = (char *)xmalloc(sizeof(char) *
				      (strlen(IDENTIFIER_POINTER(
								 DECL_NAME(outer)))+1));
      strcpy(function_name, IDENTIFIER_POINTER(DECL_NAME(outer)));
    }
  else
    {
      function_name =(char *)xmalloc(sizeof(char) * (strlen("no_name")+1));
      strcpy(function_name,"no_name");
    }

  file_location=expand_location(DECL_SOURCE_LOCATION(outer));
  

  name = (char *)xcalloc(72,sizeof(char));

  sprintf (name, "cilk_loop_line_%d_%d",
	   file_location.line,
	   ++counter);

  /* we do this because sometimes there are spaces at the end of the function
   * name,This loop will get rid of it
   */

  cc=name;
  dd=name;

  while (*cc)
    {
      if (*cc==' ')
	{
	  ++cc;
	} 
      else if (!ISIDNUM(*cc))
	{
	  *cc = '_';
	  *dd++ = *cc++; 
	}
      else
	{
	  *dd++ = *cc++;
	}
    }
  *dd=0;
  
  fndecl = build_lang_decl (FUNCTION_DECL, get_identifier (name), fntype);
  if (cfd->nested_ok)
    DECL_CONTEXT (fndecl) = current_function_decl;
  else
    /* This makes the loop function appear like a static member
       function.  DECL_CONTEXT = 0 would work too.  */
    DECL_CONTEXT(fndecl) = DECL_CONTEXT (current_function_decl);

  
  /* All this means is "don't mangle the name of this function."
     It does not actually change the calling convention. */
  SET_DECL_LANGUAGE (fndecl, lang_c);

  start_preparsed_function (fndecl, NULL_TREE, SF_PRE_PARSED);

  declare_for_loop_variables (cfd, fndecl);

  body = begin_function_body (); /* begin_compound_stmt (BCS_FN_BODY); ? */

  push_gimplify_context (&gctx);

  gimple_add_tmp_var (cfd->var2);

  /* If begin_compound_stmt is used, then:
     cilk_init_frame_descriptor (cfun, true); */

  /* Get the loop lower bound into a variable, unless it is constant
     or a value that can't be copied.  In the latter case we reference
     the uncopyable value in the outer frame. */

  cfun->is_cilk_function=1;
  pre = 0;
  lower_bound = cfd->lower_bound;
  if (lower_bound == 0)
    {
      lower_bound = cfd->var;

      hack = build_decl (UNKNOWN_LOCATION, VAR_DECL,
			 get_identifier("cilk_lower_bound_local_copy"),
			 TREE_TYPE (lower_bound));
      DECL_CONTEXT (hack) = DECL_CONTEXT (lower_bound);

      *pointer_map_insert(cfd->decl_map,hack)=lower_bound;
      lower_bound = hack;
    
    }
  
  loop_var = build_decl (UNKNOWN_LOCATION,VAR_DECL,
			 /* DECL_NAME(lower_bound) */ NULL_TREE,
			 TREE_TYPE(cfd->min_parm));

  
  DECL_CONTEXT(loop_var)=fndecl;
  
  loop_var = get_initialized_tmp_var (cfd->min_parm, &pre_seq, 0);
  
  pre=build2(INIT_EXPR, void_type_node, loop_var, cfd->min_parm);
  add_stmt (pre); 

  count_type = cfd->count_type;
  
  
  gcc_assert (TYPE_MAIN_VARIANT(TREE_TYPE (loop_var)) ==
	      TYPE_MAIN_VARIANT(count_type)); 

  /* The new loop body is

     var2 = (T)((control variable) * INCR + (lower bound));

  */

  loop_body = 0;
  /* Concatenate the control variable initialization with the
     loop body.  Do not call gimplify_and_add to append to list
     because we need to wrap the entire list in a cleanup point
     expr to delay destruction of the control variable to the
     end of the loop if it is an iterator. */
  loop_end_comp = compute_loop_var (cfd, loop_var, lower_bound);
  append_to_statement_list (loop_end_comp, &loop_body);
  cleanup = cxx_maybe_build_cleanup (cfd->var2, tf_none);

  if (cleanup != NULL)
    {
      append_to_statement_list (cfd->body, &loop_body);
      append_to_statement_list (cleanup, &loop_body); 
    } 
  else
    {
      append_to_statement_list (cfd->body, &loop_body);
    }
  loop_body = fold_build_cleanup_point_expr (void_type_node, loop_body);
  DECL_SEEN_IN_BIND_EXPR_P(cfd->var2)=1;

  cilk_outline (outer, fndecl, &loop_body, cfd->decl_map, CILK_BLOCK_FOR);
  
  loop_body =
    build5 (FOR_STMT, void_type_node, loop_var,
	    build2 (LT_EXPR, boolean_type_node, loop_var, cfd->max_parm),
	    build2 (MODIFY_EXPR, void_type_node,
		    loop_var,
		    build2 (PLUS_EXPR, count_type,
			    loop_var,
			    build_int_cst (count_type, 1))),
	    loop_body, NULL_TREE);

 
  add_stmt (loop_body);

  DECL_INITIAL(fndecl) = make_node(BLOCK);
  TREE_USED(DECL_INITIAL(fndecl)) = 1;
  BLOCK_VARS(DECL_INITIAL(fndecl)) = loop_var;
  TREE_CHAIN(loop_var) = cfd->var2;

  DECL_STATIC_CHAIN(fndecl) = 1;
  body = build3(BIND_EXPR,void_type_node,loop_var,body,
		DECL_INITIAL(fndecl));
  DECL_CONTEXT(cfd->var2) = fndecl;
  
  DECL_STATIC_CHAIN(fndecl) = 1;
  pop_gimplify_context (0);

  finish_function_body (body);

  
  
  /* A nested function can not be expand_or_defer-ed until
     its parent is done, so don't call expand_or_defer_fn (fndecl);
     here.  A non-nested function msut be expand_or_defer-ed here.  */

  if (!cfd->nested_ok)
    expand_or_defer_fn (fndecl);


  pop_function_context(); 

  
  return fndecl;
}


static tree
compute_loop_var (struct cilk_for_desc *cfd, tree loop_var, tree lower_bound)
{
  tree incr = cfd->incr;
  tree count_type = cfd->count_type;
  tree scaled, adjusted;
  int incr_sign = cfd->incr_sign;
  enum tree_code add_op = incr_sign >= 0 ? PLUS_EXPR : MINUS_EXPR;

  gcc_assert (TYPE_MAIN_VARIANT(TREE_TYPE (loop_var)) ==
	      TYPE_MAIN_VARIANT(count_type)); 

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
      exp = build_new_op (add_op, 0, low, loop_var, NULL_TREE, 0, 0);
      gcc_assert (exp != error_mark_node);
 
      exp = build_modify_expr (UNKNOWN_LOCATION, cfd->var2,
			       TREE_TYPE(cfd->var2), INIT_EXPR,
			       UNKNOWN_LOCATION, exp, TREE_TYPE(cfd->var2));
      if (exp == error_mark_node)
	{
	  fnotice (stderr, "confused by earlier errors, bailing out\n");
	  exit(ICE_EXIT_CODE);
	}

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



bool
cp_spawnable_constructor (tree fn)
{
  return (DECL_CONSTRUCTOR_P (fn) ||
	  DECL_OVERLOADED_OPERATOR_P (fn) != ERROR_MARK);
}


bool
cp_recognize_spawn (tree exp)
{
  if (TREE_CODE (exp) != AGGR_INIT_EXPR)
    return false;

  if (!AGGR_INIT_VIA_SPAWN_P (exp))
    return false;

  AGGR_INIT_VIA_SPAWN_P (exp) = 0;
  AGGR_INIT_DETACH (exp) = 1;
  return true;
}


/* Return the innermost Cilk block enclosing the current scope, if any.  */
struct cp_binding_level *
in_cilk_block (void)
{
  struct cp_binding_level *b = current_binding_level;
  while (b)
    {
      switch (b->kind)
	{
	case sk_function_parms:
	  return 0;
	case sk_cilk_for:
	case sk_cilk_block:
	  return b;
	default:
	  break;
	}
      b = b->level_chain;
    }
  return 0;
}


tree
cp_make_cilk_frame (void)
{
  tree decl = cfun->cilk_frame_decl;

  cfun->is_cilk_function=1;
  
  if (decl == NULL_TREE)
    {
      tree addr, body, ctor, dtor, obody;
      tree *saved_tree = &DECL_SAVED_TREE (current_function_decl);

      decl = make_cilk_frame (current_function_decl);

      push_local_binding (DECL_NAME (decl), decl, 0);
      cp_finish_decl (decl, NULL_TREE, false, NULL_TREE, 0); 

      addr = build1 (ADDR_EXPR, cilk_frame_ptr_type_decl, decl);

      ctor = build_call_expr (cilk_enter_fndecl, 1, addr);
      dtor = build_cilk_function_exit (decl, false,true);

      /* The new body will be
	 ctor
	 try old body finally dtor
      */
      body = alloc_stmt_list ();
      obody = *saved_tree;
      /* Some inner block has a chain pointing to obody.
	 obody must point to the new body and remain as
	 a separate statement list. */
      gcc_assert (TREE_CODE (obody) == STATEMENT_LIST);

      append_to_statement_list_force (ctor, &body);
      append_to_statement_list_force (build_stmt (UNKNOWN_LOCATION,
						  TRY_FINALLY_EXPR,
						  obody, dtor),
				      &body);

      TREE_CHAIN (obody) = body;
      *saved_tree = body;
    }

  return decl;
}

void
cilk_erase_for(tree c_for_stmt)
{
  FOR_INIT_STMT(c_for_stmt) = error_mark_node;

  FOR_COND(c_for_stmt) = boolean_false_node;
  FOR_BODY(c_for_stmt) = error_mark_node;
  FOR_EXPR(c_for_stmt) = error_mark_node;
  CILK_FOR_GRAIN(c_for_stmt) = NULL_TREE;
  CILK_FOR_VAR(c_for_stmt) = NULL_TREE;
  TREE_SET_CODE(c_for_stmt, FOR_STMT);
}



static tree
callable(enum tree_code code, tree op0, tree op1, const char *what, bool cry)
{
  tree exp=NULL_TREE;
  int flags=0;
  const char *op=operator_name_info[(int)code].name;

  if (code==INIT_EXPR)
    {

      VEC(tree,gc) *op1_vec=make_tree_vector_single(op1);
      return build_special_member_call(NULL_TREE,
				       complete_ctor_identifier,
				       &op1_vec,
				       TYPE_MAIN_VARIANT(TREE_TYPE(op1)),
				       0, CALL_NORMAL,
				       tf_warning_or_error);
    }

  if (code == PSEUDO_DTOR_EXPR)
    {

      VEC(tree,gc) *op1_vec=make_tree_vector_single(op1);
      return build_special_member_call(NULL_TREE,
				       complete_dtor_identifier,
				       &op1_vec,
				       TYPE_MAIN_VARIANT(TREE_TYPE(op1)),
				       0, CALL_NORMAL,
				       tf_warning_or_error);
    }

  flags = LOOKUP_PROTECT | LOOKUP_CONSTRUCTOR_CALLABLE | LOOKUP_ONLYCONVERTING;
  if (cry == true)
    {
      flags |=LOOKUP_COMPLAIN;
    }

  exp = build_new_op(code, flags, op0, op1, NULL_TREE, NULL, 0); 
  
  if ((exp != NULL_TREE) &&
      (exp != error_mark_node))
    {
      return exp;
    }

  if (what != NULL)
    {
      const char *explain = cry ?"" : " accessible,unambiguous";

      if (op1 != NULL)
	{
	  error("No%s operator%s(%T,%T) for Cilk for loop%s",
		explain, op, TREE_TYPE(op0), TREE_TYPE(op1),what);
	}
      else
	{
	  error("No%s operator%s(%T) for Cilk for loop%s",
		explain, op, TREE_TYPE(op0), what);
	}
    }
  return NULL_TREE;
}

static tree
check_limit_record (tree cond, tree var, int *direction)
{
  int dir = 0;

  if ((TREE_CODE(cond) == LT_EXPR) ||
      (TREE_CODE(cond) == LE_EXPR))
    {
      dir = 1;
    }
  else if ((TREE_CODE(cond)==ERROR_MARK) ||
	   (TREE_CODE(cond)==NE_EXPR))
    {
      dir = 0;
    }
  else if ((TREE_CODE(cond)==GE_EXPR) ||
	   (TREE_CODE(cond)==GT_EXPR))
    {
      dir = -1;
    }
  else
    {
      gcc_unreachable();
    }

  if (TREE_OPERAND(cond,0) == var)
    {
      *direction = dir;
      return TREE_OPERAND(cond,1);
    }
  if (TREE_OPERAND(cond,1) == var)
    {
      *direction = -1 * dir;
      return TREE_OPERAND(cond,0);
    }

  error("loop cnodition is not a simple comparison of the loop variable");
  return NULL;
}



static tree
var_mentioned_p_cb(tree *t, int *walk_subtrees, void *var)
{
  if (*t == (tree)var)
    return *t;
  else if (TREE_CODE_CLASS(TREE_CODE(*t)) == tcc_type)
    *walk_subtrees = 0;
  return NULL_TREE;
}


static bool
var_mentioned_p(tree exp, tree var)
{
  return (walk_tree(&exp,var_mentioned_p_cb, var, 0) != NULL_TREE);
} 


static bool
check_incr(tree var, tree arith_type, tree incr)
{
  bool modify = false;
  tree exp_incr = NULL_TREE;
  enum tree_code incr_code;
  
  if (TREE_SIDE_EFFECTS(incr) == 0)
    {
      error("Cilk for loop increment has no side effects.\n");
      return false;
    }

  if (TREE_CODE(incr) == CLEANUP_POINT_EXPR)
    {
      incr=TREE_OPERAND(incr,0);
    }

  if (TREE_CODE(incr) == CONVERT_EXPR)
    {
      incr=TREE_OPERAND(incr,0);
    }

  incr_code = TREE_CODE(incr);
  
  if (TREE_CODE(incr) == MODIFY_EXPR)
    {
      modify = true;
      if (TREE_OPERAND(incr,0) != var)
	{
	  error("Cilk for increment does not modify the loop variable.\n");
	  return false;
	}
      incr = TREE_OPERAND(incr,1);
      incr_code = TREE_CODE(incr);
      gcc_assert(TREE_OPERAND(incr, 0) == var);
    
    }
  else if (TREE_OPERAND(incr,0) != var)
    {
      error("Cilk for increment does not modify the loop variable.");
      return false;
    }
  incr = TREE_OPERAND(incr,1);
  if (incr == NULL_TREE)
    {
      gcc_assert((incr_code >= PREDECREMENT_EXPR) &&
		 (incr_code <= POSTINCREMENT_EXPR));
    }
  else
    {
      if (TREE_CODE(TREE_TYPE(incr))==ERROR_MARK)
	{
	  gcc_assert(errorcount > 0);
	  return false;
	}
      else if ((TREE_CODE(TREE_TYPE(incr)) == POINTER_TYPE))
	{
	  error("invalid type %qT for loop increment",TREE_TYPE(incr));
	  return false;
	}
      else if ((TREE_CODE(TREE_TYPE(incr)) != INTEGER_TYPE) &&
	       (TREE_CODE(TREE_TYPE(incr)) != ENUMERAL_TYPE) &&
	       (TREE_CODE(TREE_TYPE(incr)) != BOOLEAN_TYPE))
	{
	  error("invalid type %qT for loop increment",TREE_TYPE(incr));
	  return false;
	}

      if (TREE_CODE(TREE_TYPE(var)) == POINTER_TYPE)
	arith_type = TREE_TYPE(var);
      else if(TREE_CODE(TREE_TYPE(var)) == RECORD_TYPE)
	;
      else
	{
	  arith_type = type_after_usual_arithmetic_conversions(TREE_TYPE(var),
							       TREE_TYPE(incr));
    
	}
      gcc_assert(arith_type !=error_mark_node);
    }

  if ((incr != NULL_TREE) &&
      (TYPE_UNSIGNED(arith_type)) &&
      (TYPE_UNSIGNED(TREE_TYPE(incr)) == 0) &&
      (tree_int_cst_sgn(incr) >= 0))
    {
      warning(OPT_Wcilk_for,"signed increment implicitly converted to unsigned");
    }

  exp_incr = callable(incr_code,var,incr,"increment",true);
  if (exp_incr == NULL_TREE)
    {
      return false;
    }

  if ((TREE_CODE(TREE_TYPE(var)) == RECORD_TYPE) &&
      (modify == true))
    {
      if ((same_type_p(TYPE_MAIN_VARIANT(TREE_TYPE(exp_incr)),
		       TYPE_MAIN_VARIANT(TREE_TYPE(var))) == 0) &&
	  (can_convert_arg(TREE_TYPE(var),TYPE_MAIN_VARIANT(TREE_TYPE(exp_incr)),
			   TREE_TYPE(exp_incr), 0) == 0))
	{
	  error("loop increment expression is not convertable to type loop var.\n");
	  return false;
	}
    }
  if (incr == NULL_TREE)
    return true;

  if ((integer_zerop(incr)))
    {
      error("Cilk for increment is '0'.\n");
      return false;
    }
  if (var_mentioned_p(incr,var) == true)
    {
      error("Cilk for increment depends on loop variable.\n");
      return false;
    }

  return true;
}





static bool check_limit_scalar(tree var, tree cond)
{
  tree limit = NULL_TREE,op0 = NULL_TREE,op1 = NULL_TREE;


  if (TREE_SIDE_EFFECTS(cond))
    {
      warning(OPT_Wcilk_for,"Cilk for loop condition  has side effects.\n");
    }

  switch (TREE_CODE(cond))
    {
    case NE_EXPR:
    case LT_EXPR:
    case LE_EXPR:
    case GT_EXPR:
    case GE_EXPR:
      break;
    case EQ_EXPR:
      error ("Cilk for condition may not use equal exprression.\n");
      return false;
    case ERROR_MARK:
      return false;
    default:
      error("Cilk for condition doesn't appear to be a condition.\n");
      return false;
    }

  op1=TREE_OPERAND(cond,1);
  op0=TREE_OPERAND(cond,0);

  if (op0==var)
    {
      limit=op1;
      if (var_mentioned_p(op1,var))
	{
	  error("loop condition references variable on both sides.\n");
	  return false;
	}
    }
  else if (op1 != var)
    {
      while (TREE_CODE(op0) == CONVERT_EXPR)
	{
	  op0=TREE_OPERAND(op0,0);
	}
      while (TREE_CODE(op1) == CONVERT_EXPR)
	{
	  op1 = TREE_OPERAND(op1,0);
	}

      if ((op0 == var) ||
	  (op1 == var))
	{
	  error("Loop condition applies type conversion to loop variable.\n");
	}

      if (((DECL_P(op0)) &&
	   (TREE_CONSTANT(op1))) ||
	  ((DECL_P(op1)) &&
	   (TREE_CONSTANT(op0))))
	{
	  error("loop condition is not a simple comparison of the loop variables.");
	}

      return false;
    }
  else
    {
      if (var_mentioned_p (op0, var))
	{
	  error("loop condition variable references variable on both sides");
	  return false;
	}
      limit = op0;
    }

  if ((TREE_CODE(TREE_TYPE(limit)) != INTEGER_TYPE)  &&
      (TREE_CODE(TREE_TYPE(limit)) != POINTER_TYPE)  &&
      (TREE_CODE(TREE_TYPE(limit)) != BOOLEAN_TYPE)  &&
      (TREE_CODE(TREE_TYPE(limit)) != ENUMERAL_TYPE) &&
      (TREE_CODE(TREE_TYPE(limit)) != FUNCTION_TYPE) &&
      (TREE_CODE(TREE_TYPE(limit)) != ARRAY_TYPE))
    {
      error("loop limit has invalid type %qT", TREE_TYPE(limit));
      return false;
    }

  if ((TYPE_PRECISION(TREE_TYPE(limit)) > TYPE_PRECISION(TREE_TYPE(var))) &&
      ((TREE_CODE(limit) != INTEGER_CST) ||
       (int_fits_type_p (limit, TREE_TYPE(var)) == 0)))
    {
      warning(OPT_Wcilk_for,"loop condition compares loop var. to wider type.");
    }
  return true;
}

/* returns the count type of a CIlk for loop */
static tree
check_loop_difference_type(tree type)
{
  if (TREE_CODE(type) != INTEGER_TYPE)
    {
      error("loop variable difference type %qT is not integral",type);
      return NULL_TREE;
    }
  else if (TYPE_PRECISION(type) > TYPE_PRECISION(long_long_unsigned_type_node))
    {
      /* overflow occured */
      error("loop variable difference type is bigger than long long ");
      return NULL_TREE;
    }
  else if ((TYPE_PRECISION(type) > TYPE_PRECISION(long_unsigned_type_node)) ||
	   (same_type_p(type, long_long_integer_type_node)) ||
	   (same_type_p(type, long_long_unsigned_type_node)))
    {
      return long_long_unsigned_type_node;
    }
  else
    {
      return long_unsigned_type_node;
    }
}


static bool
validate_for_scalar(tree c_for_stmt, tree var)
{
  tree type = TREE_TYPE(var);
  
  if (TREE_CODE(TREE_TYPE(var)) == POINTER_TYPE)
    type = ptrdiff_type_node;
  
  if (check_loop_difference_type (type) == NULL_TREE)
    {
      return false;
    } 

  if (check_incr(var, type, FOR_EXPR(c_for_stmt)) == false)
    {
      return false;
    }

  if (check_limit_scalar(var, FOR_COND(c_for_stmt)) == false)
    {
      return false;
    }

  return true;
}

static bool
validate_for_record(tree c_for_stmt, tree var)
{
  tree exp_up = NULL_TREE,exp_down = NULL_TREE,exp_plus = NULL_TREE;
  tree exp_cond = NULL_TREE;
  tree l_type = NULL_TREE, d_type = NULL_TREE, d_type_up = NULL_TREE;
  tree d_type_down = NULL_TREE;
  tree var_type = NULL_TREE,cond = NULL_TREE,limit = NULL_TREE,hack = NULL_TREE;
  int direction=0;
  
  cond = FOR_COND(c_for_stmt);
  
  limit = check_limit_record(cond,var,&direction);
  if(limit == NULL_TREE)
    {
      return false;
    }

  var_type = TREE_TYPE(var);

  l_type = TREE_TYPE(limit);

  hack = build_decl(UNKNOWN_LOCATION, VAR_DECL,get_identifier("loop_bound"),
		    build_qualified_type(l_type,TYPE_QUAL_CONST));

  if (direction >= 0)
    {
      exp_up = callable(MINUS_EXPR,hack,var," control variable",true);
      if (exp_up == NULL_TREE)
	{
	  return false;
	}
      d_type_up = TYPE_MAIN_VARIANT(TREE_TYPE(exp_up));
      d_type = d_type_up;
    }

  if (direction <= 0)
    {
      exp_down = callable(MINUS_EXPR,var,hack," control variable",true);
      if (exp_down == NULL_TREE)
	{
	  return false;
	}
      d_type_down = TYPE_MAIN_VARIANT(TREE_TYPE(exp_down));
      d_type = d_type_down;
    }
  if ((direction == 0) &&
      (d_type_up != d_type_down))
    {
      error("Ambiguous operator - return type.");
      return false;
    }

  if (check_loop_difference_type (d_type) == 0)
    {
      return false;
    }

  exp_plus=callable(add_incr (FOR_EXPR(c_for_stmt)) ? PLUS_EXPR : MINUS_EXPR,
		    var, build_int_cst(d_type,1)," variable calculation",
		    false);
  if (exp_plus == NULL_TREE)
    {
      return false;
    }

  if ((TYPE_MAIN_VARIANT(TREE_TYPE(exp_plus)) != var_type) &&
      (can_convert_arg(var_type,TREE_TYPE(exp_plus),exp_plus,0) == 0))
    {
      error("result of operation%c(%T,%T) not convertable to type of loop var.",
	    (direction >= 0) ? '+' : '-', var_type, d_type);
    }

  if ((cp_tree_uses_cilk (exp_plus)) ||
      (cp_tree_uses_cilk (callable (INIT_EXPR, NULL_TREE, var, 0, false))) ||
      (cp_tree_uses_cilk (callable (PSEUDO_DTOR_EXPR, NULL_TREE, var, 0,
				    false))))
    {
      CILK_FOR_NO_DEMOTE(c_for_stmt) = 1;
    }
  cond=FOR_COND(c_for_stmt);

  exp_cond = callable(TREE_CODE(cond), TREE_OPERAND(cond,0),
		      TREE_OPERAND(cond,1), " condition", true);
  if (exp_cond == NULL_TREE)
    {
      return false;
    }

  if (can_convert_arg (boolean_type_node, TREE_TYPE(exp_cond), exp_cond,
		       LOOKUP_NORMAL) == 0)
    {
      return false;
    }

  if (check_incr (var, d_type, FOR_EXPR (c_for_stmt)) == false)
    {
      return false;
    }

  return true;
}


bool
cilk_validate_for(tree c_for_stmt)
{
  tree var= CILK_FOR_VAR(c_for_stmt), grain = CILK_FOR_GRAIN(c_for_stmt);
  
  tree grainType = NULL_TREE;
  
  if (var == error_mark_node)
    return false;


  if ((var == NULL) ||
      (DECL_P(var) == 0))
    {
      /* this means the Cilk for loop does not have a loop var declaration */
      error("Cilk for loop does not have a loop-variable declaration.\n");
      return false;
    }

  if ((grain != NULL_TREE)  &&
      (grain != error_mark_node) )
    {
      grainType = TREE_TYPE(grain);
      gcc_assert (grainType !=NULL);

      if ((TREE_CODE(grainType) != INTEGER_TYPE) &&
	  (TREE_CODE(grainType) != ENUMERAL_TYPE))
	{
	  error ("Pragma grainsize argument must be an integer.\n");
	  CILK_FOR_GRAIN(c_for_stmt)=NULL_TREE;
	  grain = NULL_TREE;
	  return false;
	}
      else if ((TYPE_UNSIGNED(grainType) == 0) &&
	       (TREE_CODE(grainType) == INTEGER_TYPE) &&
	       (TREE_INT_CST_HIGH(grain) < 0))
	{
	  warning(OPT_Wcilk_for,
		  "Pragma grainsize should be positive.\nIgnoring this value.");
	  CILK_FOR_GRAIN(c_for_stmt) = NULL_TREE; 
	  grain = NULL_TREE;
	  return false;
	}
      else if (TREE_SIDE_EFFECTS(grain))
	{
	  while ((TREE_CODE(grain) == CONVERT_EXPR) ||
		 (TREE_CODE(grain) == NOP_EXPR))
	    {
	      grain = TREE_OPERAND(grain, 0);
	      if (TREE_CODE(grain) != CALL_EXPR)
		{
		  warning(OPT_Wcilk_for, "pragma cilk grainsize has side effects.\n");
		}
	    }
	}
    }

  if ((TREE_CODE(TREE_TYPE(var)) == INTEGER_TYPE) ||
      (TREE_CODE(TREE_TYPE(var)) == POINTER_TYPE))
    {
      return validate_for_scalar (c_for_stmt,var);
    }
  else if (TREE_CODE(TREE_TYPE(var)) == RECORD_TYPE)
    {
      return validate_for_record (c_for_stmt,var);
    }
  else
    {
      error("Cilk for loop variable must be of type integer, pointer or class.");
      return false;
    }

  /* should never get here */
  return false;
}

static void
race_warning (tree var ATTRIBUTE_UNUSED, tree loc_expr ATTRIBUTE_UNUSED,
              enum add_variable_context where ATTRIBUTE_UNUSED)
{
#if 0 /* We will come back and fix this later */
  if (TREE_NO_WARNING (var))
    return;
  TREE_NO_WARNING (var) = 1;

  if (loc_expr && EXPR_HAS_LOCATION (loc_expr))
    {
      location_t loc = EXPR_LOCATION (loc_expr);
      if (where == CILK_BLOCK_FOR)
	warning (OPT_Wcilk_for,
		 "%Hwrites to %qD in loop body may race",
		 &loc, var);
      else
	warning (OPT_Wcilk_block,
		 "%Houter scope variable %qD written in spawned statement",
		 &loc, var);
      return;
    }
  if (where == CILK_BLOCK_FOR)
    warning (OPT_Wcilk_for, "writes to %qD in loop body may race", var);
  else
    warning (OPT_Wcilk_block,
	     "outer scope variable %qD written in spawned statement",
	     var);
#endif
  return;
}


static void
add_variable (struct pointer_map_t *decl_map, tree var,
	      enum add_variable_type how,
	      enum add_variable_context ctx)
{
  void **valp;

  valp=pointer_map_contains(decl_map,(void*)var);
 
  if (valp != 0)
    {
      /* If the variable is local, do nothing. */
      if ((tree)*valp == error_mark_node)
	return;
      /* If the variable was entered with itself as value,
	 meaning it belongs to an outer scope, do not alter
	 the value. */
      if ((tree) *valp == var)
	{
	  if (ctx != CILK_BLOCK_CALL && ctx != CILK_BLOCK_RUN
	      && how == ADD_WRITE)
	    race_warning (var, NULL_TREE, ctx);
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
      if (ctx != CILK_BLOCK_CALL)
	*valp = (void *)var;
      else
	*valp = (void *)integer_one_node;
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
      if (how != ADD_BIND
	  && TREE_CODE (var) == VAR_DECL
	  && !DECL_EXTERNAL (var)
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
	  switch (ctx)
	    {
	    case CILK_BLOCK_FOR:
	    case CILK_BLOCK_RUN:
	      val = var;
	      break;
	    case CILK_BLOCK_BLOCK:
	      if (TREE_ADDRESSABLE (var))
		val = var;
	      else
		val = integer_zero_node;
	      break;
	    case CILK_BLOCK_CALL:
	      val = integer_zero_node;
	      break;
	    }
	  break;
	case ADD_WRITE:
	  switch (ctx)
	    {
	    case CILK_BLOCK_BLOCK:
	    case CILK_BLOCK_FOR:
	      race_warning (var, NULL_TREE, ctx);
	      val = var;
	      break;
	    case CILK_BLOCK_RUN:
	      val = var;
	      break;
	    case CILK_BLOCK_CALL:
	      val = integer_one_node;
	      break;
	    }
	}
      *pointer_map_insert(decl_map, (void *)var)=val;
    }
}


static void
extract_free_variables (tree t,
			/* splay_tree */ struct pointer_map_t *decl_map,
			enum add_variable_type how,
			enum add_variable_context ctx)
{
  enum tree_code code;
  location_t where;

#define SUBTREE(EXP)  extract_free_variables(EXP, decl_map, ADD_READ, ctx)
#define MODIFIED(EXP) extract_free_variables(EXP, decl_map, ADD_WRITE, ctx)
#define INITIALIZED(EXP) extract_free_variables(EXP, decl_map, ADD_BIND, ctx)

  /* Skip empty subtrees.  */
  if (t == NULL_TREE)
    return;

  code = TREE_CODE (t);
  switch (code)
    {
    case ERROR_MARK:
    case IDENTIFIER_NODE:
    case INTEGER_CST:
    case REAL_CST:
    case VECTOR_CST:
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
      extract_free_variables (SSA_NAME_VAR (t), decl_map, how, ctx);
      return;

    case LABEL_DECL:
      /* This might be a reference to a label outside the Cilk block,
	 which is an error, or a reference to a label in the Cilk block
	 that we haven't seen yet.  We can't tell.  Ignore it.  An
	 invalid use will cause an error later in copy_decl_for_cilk. */
      return;

    case RESULT_DECL:
      if (ctx != CILK_BLOCK_CALL)
	TREE_ADDRESSABLE (t) = 1;
    case VAR_DECL:
    case PARM_DECL:
      if (!TREE_STATIC (t) && !DECL_EXTERNAL (t))
	add_variable (decl_map, t, how, ctx);
      /* A variable size array, for example, may contain variable
	 references. */
      SUBTREE (TREE_TYPE (t));
      return;

    case NON_LVALUE_EXPR:
    case CONVERT_EXPR:
    case NOP_EXPR:
      SUBTREE (TREE_TYPE (t));
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
	 is should be printed. */
      if (ctx != CILK_BLOCK_CALL)
	SUBTREE (TREE_OPERAND (t, 0));
      else
	MODIFIED (TREE_OPERAND (t, 0));
      SUBTREE (TREE_TYPE (t)); /* redundant? */
      return;

    case ARRAY_REF:
      /* Treating ARRAY_REF and BIT_FIELD_REF identically may
	 mark the array as written but the end result is correct
	 because the array is passed by pointer anyway. */
    case BIT_FIELD_REF:
      /* Propagate the access type to the object part of which
	 is being accessed here.  As for ADDR_EXPR, don't do this
	 in a nested loop, unless the access is to a fixed index. */
      if (ctx != CILK_BLOCK_FOR || TREE_CONSTANT (TREE_OPERAND (t, 1)))
	extract_free_variables (TREE_OPERAND (t, 0), decl_map, how, ctx);
      else
	SUBTREE (TREE_OPERAND (t, 0));
      SUBTREE (TREE_OPERAND (t, 1));
      SUBTREE (TREE_OPERAND (t, 2));
      SUBTREE (TREE_TYPE (t)); /* redundant? */
      return;

    case TREE_LIST:
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

    case COMPLEX_CST:
      SUBTREE (TREE_REALPART (t));
      SUBTREE (TREE_IMAGPART (t));
      SUBTREE (TREE_TYPE (t));
      return;

    case CONSTRUCTOR:
      {
	unsigned HOST_WIDE_INT idx;
	constructor_elt *ce;

	for (idx = 0;
	     VEC_iterate(constructor_elt, CONSTRUCTOR_ELTS (t), idx, ce);
	     idx++)
	  SUBTREE (ce->value);
	SUBTREE (TREE_TYPE (t));
	return;
      }

    case SAVE_EXPR:
      SUBTREE (TREE_OPERAND (t, 0));
      SUBTREE (TREE_TYPE (t));
      return;

    case BIND_EXPR:
      {
	tree decl;
	for (decl = BIND_EXPR_VARS (t); decl; decl = TREE_CHAIN (decl))
	  {
	    add_variable (decl_map, decl, ADD_BIND, ctx);
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
      where = EXPR_LOCATION (t); 
      error_at (where, "spawn of return statement"); /* bviyer: CHECK THIS */
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
      return;

    case ARRAY_TYPE:
      SUBTREE (TREE_TYPE (t));
      SUBTREE (TYPE_DOMAIN (t));
      return;

    case AGGR_INIT_EXPR:
      {
	int len = 0;
	int ii = 0;

	if (TREE_CODE(TREE_OPERAND(t,0)) == INTEGER_CST)
	  {
	    len = TREE_INT_CST_LOW(TREE_OPERAND(t,0));

	    for (ii = 0; ii < len; ii++)
	      {
		SUBTREE(TREE_OPERAND(t,ii));
	      }
	    SUBTREE(TREE_TYPE(t));
	  }
	break;
      }
    case CALL_EXPR:
      {
	int len = 0;
	int ii = 0;
	if (TREE_CODE(TREE_OPERAND(t,0)) == INTEGER_CST)
	  {
	    len = TREE_INT_CST_LOW(TREE_OPERAND(t,0));

	    for (ii = 0; ii < len; ii++) 
	      {
		SUBTREE(TREE_OPERAND(t,ii));
	      }
	    SUBTREE(TREE_TYPE(t));
	  }
	break;
      }
  
    default:
      if (IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (code)))
	{
	  int i, len;

	  /* Walk over all the sub-trees of this operand.  */
	  len = TREE_CODE_LENGTH (code);

	  /* Go through the subtrees.  We need to do this in forward order so
	     that the scope of a FOR_EXPR is handled properly.  */
	  for (i = 0; i < len; ++i)
	    SUBTREE (TREE_OPERAND (t, i));

	  SUBTREE (TREE_TYPE (t));
	}
      return;
    }
}


static void
cg_hacks (tree fndecl, bool is_nested)
{
  const tree outer = current_function_decl;
  struct function *f = DECL_STRUCT_FUNCTION (fndecl);

  gcc_assert (TREE_CODE (fndecl) == FUNCTION_DECL);

  /* gimplify_body may garbage collect.  Save a root. */
  cilk_trees[CILK_TI_PENDING_FUNCTIONS] =
    tree_cons (NULL_TREE, fndecl, cilk_trees[CILK_TI_PENDING_FUNCTIONS]);

  gcc_assert (cfun == DECL_STRUCT_FUNCTION (outer));
  gcc_assert (cfun->decl == outer);

  push_cfun (f);
  current_function_decl = fndecl;

  /* If this is a genuine nested function, the nested function
     handling will deal with it.  If this is not a nested function
     it must be handled now or the compiler will crash in a
     mysterious way later.

     XXX This is not working right for the deeply nested loop+spawn
     test, bug603.  NO_STATIC_CHAIN is not set but gimplify_function_tree
     must be called. */
  if (!DECL_STATIC_CHAIN (fndecl) || is_nested==true)
    {
      gimplify_function_tree (fndecl);
    }
  cgraph_add_new_function (fndecl, false);
  /* Calling cgraph_finalize_function now seems to be the only way to
     prevent a crash due to cgraph becoming confused over whether the
     function is needed. */
  cgraph_finalize_function (fndecl, true);

  pop_cfun ();

  gcc_assert (TREE_VALUE (cilk_trees[CILK_TI_PENDING_FUNCTIONS]) == fndecl);
  cilk_trees[CILK_TI_PENDING_FUNCTIONS] =
    TREE_CHAIN (cilk_trees[CILK_TI_PENDING_FUNCTIONS]);
}



static tree
gimplify_cilk_for_stmt_1 (struct cilk_for_desc *cfd, gimple_seq *pre_p)
{
  tree var=NULL_TREE, incr=NULL_TREE;
  tree fn=NULL_TREE;
  /* tree lambda_fn=NULL_TREE; */
  /* tree x = NULL_TREE; */

  
  var = cfd->var; /* a VAR_DECL */
  DECL_CONTEXT (var)=current_function_decl;
  

  /* If the loop increment is not an integer constant and is not
     a DECL (e.g. it is a type conversion of a variable), copy it
     to a temporary.  If the DECL is modified during the loop the
     behavior is undefined; races could be avoided by copying any
     non-const DECL.  */
  incr = cfd->incr;
  if (TREE_CODE (incr) != INTEGER_CST && !DECL_P (incr))
    cfd->incr = incr = get_formal_tmp_var (incr, pre_p);

  if (DECL_P (incr) && !TREE_STATIC (incr) && !DECL_EXTERNAL (incr))
    /* This variable is accessed as-is */

    *pointer_map_insert(cfd->decl_map,incr)=incr;

  /* Map the loop variable to integer_minus_one_node if we won't really
     be passing it to the loop body and integer_zero_node otherwise.

     If the map ends up integer_one_node then somebody wrote to the loop
     variable and that's a user error.
     The correct map will be installed in declare_for_loop_variables. */

  *pointer_map_insert(cfd->decl_map,var) = 
    (void *) (cfd->lower_bound ? integer_minus_one_node : integer_zero_node);
  extract_free_variables (cfd->body, cfd->decl_map, ADD_READ, CILK_BLOCK_FOR);
  /* Note that variables are not extracted from the loop condition
     and increment.  They are evaluated, to the extent they are
     evaluated, in the context containing the for loop. */

  fn = cp_build_cilk_for_body (cfd);


  
  if (cfd->nested_ok)
    {
      DECL_STATIC_CHAIN (fn) = 1;
      DECL_EXPLICIT_STATIC_CHAIN (fn) = 1;
    }
  DECL_UNINLINABLE (fn) = 1;
  DECL_STATIC_CHAIN(fn)=1;
  cg_hacks(fn,false);
  
  
  return fn;
}



/* This is mostly a subset of cp_build_cilk_for_body */
static tree
build_cilk_run_body (tree stmt, tree rv)
{
  tree outer = current_function_decl;
  char name[40];
  static int counter;
  tree fndecl, fntype;
  tree body;
  tree x;
  struct pointer_map_t *decl_map;
  struct gimplify_ctx gctx;

  decl_map=pointer_map_create();

  /* Force the return value to be entered as a nested reference
     even though it it set with INIT_EXPR. */
  if (rv)
    add_variable (decl_map, rv, ADD_WRITE, CILK_BLOCK_RUN);

  extract_free_variables (stmt, decl_map, ADD_READ, CILK_BLOCK_RUN);

  push_function_context();

  x = tree_cons (NULL_TREE, ptr_type_node, void_list_node);

  fntype = build_function_type (void_type_node, x);
  SET_FUNCTION_TYPE_LINKAGE(fntype,linkage_cilk);

  sprintf (name, "__cilk_run_%03d", ++counter);

  fndecl = build_lang_decl (FUNCTION_DECL, get_identifier (name), fntype);
  /* cilk_run bodies are always nested functions */
  DECL_CONTEXT (fndecl) = current_function_decl;
  DECL_STATIC_CHAIN (fndecl) = 1;
  DECL_EXPLICIT_STATIC_CHAIN (fndecl) = 1;

  /* All this means is "don't mangle the name of this function."
     It does not actually change the calling convention. */

  start_preparsed_function (fndecl, NULL_TREE, SF_PRE_PARSED);

  {
    tree parm = build_decl (UNKNOWN_LOCATION, PARM_DECL, NULL_TREE,
			    ptr_type_node);
    DECL_ARG_TYPE (parm) = ptr_type_node;
    /* DECL_IGNORED_P means "you may optimize register allocation
       of this compiler-generated variable because the user does
       not expect to look at it with a debugger." */
    DECL_IGNORED_P (parm) = 1;
    DECL_ARTIFICIAL (parm) = 1;
    TREE_READONLY (parm) = 1;
  }

  body = begin_function_body (); /* begin_compound_stmt (BCS_FN_BODY); ? */

  push_gimplify_context (&gctx);

  cilk_outline (outer, fndecl, &stmt, decl_map, CILK_BLOCK_RUN);

  if (cp_function_chain->can_throw)
    {
      /* TODO: Deposit the exception in the parent to be rethrown. */
      warning (0, "%<cilk_run%> expression may throw exception");
      stmt = build1 (MUST_NOT_THROW_EXPR, void_type_node, stmt);
    }

  add_stmt (fold_build_cleanup_point_expr (void_type_node, stmt));

  pop_gimplify_context (0);

  finish_function_body (body);

  finish_function (SF_CILK_NESTED);

  pop_function_context();

  /* splay_tree_delete (var_map); */
  pointer_map_destroy(decl_map);

  return fndecl;
}

void
cilk_gimplify_run (tree *expr_p,
		   gimple_seq *before ATTRIBUTE_UNUSED,
		   gimple_seq *after ATTRIBUTE_UNUSED)
{
  tree expr = *expr_p;
  tree inner = TREE_OPERAND (expr, 0);
  tree type;
  tree fn, fnaddr, ctx, args;
  tree var, var_ret;

  if (in_cilk_function ())
    {
      /* cilk_run in a Cilk++ function does nothing */
      *expr_p = inner;
      return;
    }

  /* If this operation is initializing a particular addressable
     variable, preserve the identity of that variable. */
  if (TREE_CODE (inner) == AGGR_INIT_EXPR)
    {
      var = TREE_OPERAND (inner, 2);
      var_ret = NULL_TREE;
      type = TREE_TYPE (var);
    }
  else
    {
      type = TREE_TYPE (expr);
      gcc_assert (type == TREE_TYPE (inner));
      var = NULL_TREE;
      var_ret = NULL_TREE;
    }

  if (var == NULL_TREE && ! VOID_TYPE_P (type))
    {
      /* We shouldn't get here with an addressable type.
	 If we need to handle that, replace create_tmp_var
	 with create_tmp_var_raw and gimple_add_tmp_var. */
      var = create_tmp_var (type, "cilk_run");
      /* This used to simply build a MODIFY_EXPR.
	 It may happen that INIT_EXPR is better. */
      inner = build_modify_expr (UNKNOWN_LOCATION, var, TREE_TYPE(var),
				 INIT_EXPR,UNKNOWN_LOCATION, var,
				 TREE_TYPE(inner));
      var_ret = var;
    }

  fn = build_cilk_run_body (inner, var);
  fnaddr = build2 (FDESC_EXPR, build_pointer_type (TREE_TYPE (fn)),
		   fn, integer_zero_node);
  /* TREE_INVARIANT (fnaddr) = 1; */
  TREE_CONSTANT (fnaddr) = 1;
  ctx = build2 (FDESC_EXPR, ptr_type_node, fn, integer_one_node);

  args = NULL_TREE;
  args = tree_cons (NULL_TREE, ctx, args);
  args = tree_cons (NULL_TREE, fnaddr, args);

  gimplify_and_add (build3 (CALL_EXPR, void_type_node,
			    build_address (cilk_run_fndecl),
			    args, NULL_TREE),
		    before);

  *expr_p = var_ret; /* may be NULL_TREE */

  return;
}

tree
build_cilk_run (tree expr)
{
  if (TREE_CODE (expr) == TARGET_EXPR)
    {

      /* Can we really modify this in-place? */
      TARGET_EXPR_INITIAL (expr) = build_cilk_run (TARGET_EXPR_INITIAL (expr));
      return expr;
    }
  return build1 (CILK_RUN_EXPR, TREE_TYPE (expr), expr);
}

static tree
mangle_count (tree count, enum tree_code op, tree incr, bool negate, tree type)
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

     XXX Should change the width of INCR if int_fits_type_p */
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
  /* If END_EXPR has been evaluated into a variable, use the
     variable.  Otherwise use the expression, which should have
     no interesting side effects. */
  tree high = cfd->end_var ? cfd->end_var : cfd->end_expr ;
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
      /* If the loop is going up the loop count is

	 operator-(high, low);

	 If the loop is going down the loop count is

	 operator-(low, high);

	 Note that these are in general different functions.
	 The result must be positive if the loop condition is true,
	 or zero if the loop is inclusive of its upper bound. */

      /* bviyer: the terms low and high are deceptive.. Here is what I meant
       * by these two vars:
       * cilk_for (x = 5; x >= 2 ; x--)
       *
       * the value before the first semi colon (5) is the "low" and the value
       * after the 2nd semi-colon (2), is the "high"
       */
      if (direction >= 0)
	{
	  if (TREE_CODE(high) == TARGET_EXPR)
	    {
	      high = TARGET_EXPR_INITIAL(high);
	    }

	  if (TREE_CODE(low) == TARGET_EXPR)
	    {
	      low = TARGET_EXPR_INITIAL(low);
	    }
 
	  /* count_up=build2(MINUS_EXPR,TREE_TYPE(high),high, low); */
	  count_up = build_new_op (MINUS_EXPR, 0, high, low, NULL_TREE, 0,
				   /* NULL */ 0);
	  /* We should have already failed if the operator is not callable. */
	  gcc_assert (count_up != error_mark_node);
	}
      else if (direction <= 0)
	{
	  if (TREE_CODE(high) == TARGET_EXPR)
	    {
	      high = TARGET_EXPR_INITIAL (high);
	    }

	  if (TREE_CODE(low) == TARGET_EXPR)
	    {
	      low = TARGET_EXPR_INITIAL (low);
	    }
  
	  /* count_down = build2 (MINUS_EXPR, TREE_TYPE(high), low, high); */
	  count_down = build_new_op (MINUS_EXPR, 0, low, high, NULL_TREE, 0, 0);
	  gcc_assert (count_down != error_mark_node);
	}
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
	  sub_type = type_after_usual_arithmetic_conversions (low_type, high_type);

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
      (type_after_usual_arithmetic_conversions
       (TREE_TYPE (cfd->var), TREE_TYPE (incr)),
       incr);

  /* Now separately divide each count by +/-INCR yielding
     a value with type TYPE. */
  count_up = mangle_count (count_up, div_op, incr, incr_sign < 0, type);
  count_down = mangle_count (count_down, div_op, incr, incr_sign > 0, type);

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


static tree
cilk_simplify_incr (tree incr)
{
  extern bool tree_ssa_useless_type_conversion (tree);

  if (TREE_CODE (incr) == CLEANUP_POINT_EXPR)
    incr = TREE_OPERAND (incr, 0);
  if (TREE_CODE (incr) == CONVERT_EXPR && VOID_TYPE_P (TREE_TYPE (incr)))
    incr = TREE_OPERAND (incr, 0);
  STRIP_USELESS_TYPE_CONVERSION (incr);

  return incr;
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

static bool
cp_extract_for_fields (struct cilk_for_desc *cfd, tree for_stmt)
{
  tree var = CILK_FOR_VAR (for_stmt);
  tree cond = FOR_COND (for_stmt);
  tree init = CILK_FOR_INIT (for_stmt);
  tree incr = cilk_simplify_incr (FOR_EXPR (for_stmt));
  enum tree_code incr_op;
  bool no_demote = CILK_FOR_NO_DEMOTE (for_stmt);
  bool iterator, inclusive, exactly_one;
  tree limit;
  int cond_direction, incr_direction, implied_direction, direction;
  bool negate_incr;
  tree var_type, difference_type, count_type;
  tree body, grain;

  gcc_assert (var != NULL_TREE);


  
  /* The parser requires an explicit comparison operation,
     not something like (bool)x. */
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
    default:
      gcc_unreachable ();
      break;
    }

  if (TREE_OPERAND (cond, 0) == var)
    {
      limit = decay_conversion (TREE_OPERAND (cond, 1));

    }
  else if (TREE_OPERAND (cond, 1) == var)
    {
      limit = decay_conversion (TREE_OPERAND (cond, 0));
      cond_direction = -cond_direction;
    }
  else
    gcc_unreachable ();

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
      difference_type = type_promotes_to (TREE_TYPE (var));
      break;
    case ENUMERAL_TYPE:
    case BOOLEAN_TYPE:
      iterator = false;
      difference_type = type_promotes_to (var_type);
      break;
    case RECORD_TYPE:
    case UNION_TYPE:
      {
	tree exp = NULL_TREE, hack = NULL_TREE;
 
	{
	  hack = build_decl (UNKNOWN_LOCATION, VAR_DECL,
			     NULL_TREE,
			     TREE_TYPE (limit));
	}
	if (cond_direction >= 0)
	  exp = callable (MINUS_EXPR, hack, var, NULL, false);
	else
	  exp = callable (MINUS_EXPR, var, hack, NULL, false);
	/* We already checked this. */
	gcc_assert (exp);
	difference_type = TYPE_MAIN_VARIANT (TREE_TYPE (exp));
	iterator = true;
      } 
      break;
    default:
      gcc_unreachable ();
      break;
    }

  count_type = check_loop_difference_type (difference_type);

  /* Before the switch incr is an expression modifying VAR.
     After the switch incr is the modification to VAR with
     the sign factored out. */
  incr_op = TREE_CODE (incr);
  switch (incr_op)
    {
    case PREINCREMENT_EXPR:
    case POSTINCREMENT_EXPR:
      negate_incr = false;
      incr_direction = 1; /* exact */
      implied_direction = 1;
      incr = TREE_OPERAND (incr, 1);
      if (incr == NULL_TREE)
	{
	  if ((TREE_CODE (var_type) == POINTER_TYPE))
	    incr = size_in_bytes ((var_type));
	  else
	    incr = integer_one_node;
	}
      exactly_one = integer_onep (incr);
      break;

    case PREDECREMENT_EXPR:
    case POSTDECREMENT_EXPR:
      negate_incr = true;  /* we store +1 and subtract it */
      incr_direction = -1; /* exact */
      implied_direction = -1;
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
      if (TREE_CODE (incr) == PLUS_EXPR)
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
	  implied_direction = 1;
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
      
	  if (TREE_CODE (var_type) == POINTER_TYPE)
	    {
	      tree size = size_in_bytes (TREE_TYPE (var_type));
	      if (!integer_onep (size))
		{
		  exactly_one = 0;
		  /* cilk_for (int *p = a; p < b; p += (char)c) ;
		     We need to do the math in a type wider than c.
		     build_binary_op will do default conversions,
		     which should be enough if SIZE is size_t. */
		  incr = cp_build_binary_op (UNKNOWN_LOCATION, MULT_EXPR, incr, size,
					     true);
		}
	    }
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
	  implied_direction = -1;
	  incr_direction = -compute_incr_direction (incr);
	  /* Subtracting a negative number treated as unsigned
	     is adding a large positive number. */
	  if (TYPE_UNSIGNED (difference_type) && incr_direction > 0)
	    incr_direction = -2;
	  exactly_one = incr_direction == -1;
	  if (TREE_CODE (var_type) == POINTER_TYPE)
	    {
	      tree size = size_in_bytes (TREE_TYPE (var_type));
	      if (!integer_onep (size))
		{
		  exactly_one = 0;
		  incr = cp_build_binary_op (UNKNOWN_LOCATION, MULT_EXPR, incr, size,
					     true);
		}
	    }
	}
      else
	gcc_unreachable ();
      break;

    default:
      gcc_unreachable ();
    }

  /* We can handle loops with direction not known at compile time,
     and it's undefined behavior at runtime if the user does something
     silly.  We can't handle loops with inconsistent direction. */
  if ((incr_direction < 0 && cond_direction > 0)
      || (incr_direction > 0 && cond_direction < 0))
    {
      error ("Cilk for loop increment and condition are inconsistent");
      return false;
    }


  if (TREE_CODE (incr) != INTEGER_CST && var_mentioned_p (incr, var))
    warning (OPT_Wcilk_for, "loop increment references loop variable");

  if (incr_direction == 0 && cond_direction == 0)
    warning (OPT_Wcilk_for_direction,
	     "Cilk for loop using != comparison not determined by form of increment");
  else if (implied_direction < 0 && cond_direction > 0)
    warning (OPT_Wcilk_for,
	     "Cilk for loop mixes less than comparison with -= operation");
  else if (implied_direction > 0 && cond_direction < 0)
    warning (OPT_Wcilk_for,
	     "Cilk for loop mixes greater than comparison with += operation");

  /* If we had a != comparison we know the range is exact
     because we don't allow wrap around. */
  if (TREE_CODE (cond) == NE_EXPR && incr_direction == 2)
    direction = 1;
  else if (TREE_CODE (cond) == NE_EXPR && incr_direction == -2)
    direction = -1;
  /* Otherwise incr_direction is more accurate. */
  else if (incr_direction != 0)
    direction = incr_direction;
  else
    direction = cond_direction;

  body = FOR_BODY (for_stmt);
  grain = CILK_FOR_GRAIN (for_stmt);

  cfd->iterator = iterator;
  cfd->no_demote = no_demote;
  cfd->inclusive = inclusive;
  cfd->incr_sign = negate_incr ? -1 : 1;
  cfd->exactly_one = exactly_one;
  cfd->direction = direction;
  cfd->end_expr = limit;
  cfd->end_var = NULL_TREE;
  cfd->var = var;
  cfd->var_type = var_type;
  cfd->difference_type = difference_type;
  cfd->count_type = count_type;
  cfd->lower_bound = init;
  cfd->incr = incr;
  cfd->cond = cond;
  cfd->body = body;
  cfd->grain = grain;

  return true;
}


void
gimplify_cilk_for_stmt(tree *for_p, gimple_seq *pre_p)
{
  tree parent_function = current_function_decl;
  tree t = *for_p,new_for = NULL_TREE, fn = NULL_TREE, ctx = NULL_TREE;
  tree grain = NULL_TREE;
  tree libfun = NULL_TREE, count = NULL_TREE, cond = NULL_TREE;
  tree ii_tree = NULL_TREE;
  bool order_variable = false;
  struct cilk_for_desc cfd; 
  enum tree_code ecode;
  tree op0 = NULL_TREE, op1 = NULL_TREE;
  gimple_seq inner_seq = NULL;
  
  initialize_cilk_for_desc (&cfd);
 
  if (cp_extract_for_fields(&cfd,*for_p) == 0)
    {
      *for_p = build_empty_stmt(UNKNOWN_LOCATION);
      release_cilk_for_desc (&cfd);
      return;
    }

  gcc_assert (cfd.var != error_mark_node);

  cfd.nested_ok= !DECL_MAYBE_IN_CHARGE_CONSTRUCTOR_P(current_function_decl);
  
  if (cfd.grain != NULL_TREE)
    {
      grain = cfd.grain;
    }
  else 
    {
      grain = integer_zero_node;
    }

  gimplify_and_add(FOR_INIT_STMT(t),pre_p);

  if (TREE_SIDE_EFFECTS(cfd.end_expr) != 0)
    {
      ecode = TREE_CODE(cfd.end_expr);

      if ((ecode == INIT_EXPR) ||
	  (ecode == MODIFY_EXPR))
	{
	  cfd.end_var = TREE_OPERAND(cfd.end_expr,0);
	}
      else if (ecode == TARGET_EXPR)
	{
	  cfd.end_var = TARGET_EXPR_INITIAL(cfd.end_expr);
	  if (TREE_CODE(cfd.end_var) == AGGR_INIT_EXPR)
	    {
	      cfd.end_var = TARGET_EXPR_SLOT(cfd.end_expr);
	    }
	  else
	    {
	      cfd.end_var = get_formal_tmp_var(cfd.end_var,pre_p);
	    }
	}
      else if (ecode == CALL_EXPR)
	{
	  cfd.end_var = cfd.end_expr;
	}
      else
	{
	  ii_tree = cfd.end_expr;
	  while (TREE_CODE_CLASS(TREE_CODE(ii_tree)) == tcc_unary)
	    {
	      ii_tree = TREE_OPERAND(ii_tree, 0);
	    }
	  ecode = TREE_CODE(ii_tree);
	  cfd.end_var = cfd.end_expr;
	  cfd.end_var = get_formal_tmp_var(cfd.end_expr,pre_p); 
	  order_variable = true;
	}
    }
      
  if (TREE_SIDE_EFFECTS(cfd.incr))
    cfd.incr = get_formal_tmp_var(cfd.incr,&inner_seq);

  cond = cfd.cond;

  op1 = TREE_OPERAND(cond,1);
  op0 = TREE_OPERAND(cond,0);

  if ((order_variable == true) &&
      (op1 == cfd.end_expr))
    {
      op1 = cfd.end_var;
    }
  else if ((order_variable == true) &&
	   (op0 == cfd.end_expr))
    {
      op0 = cfd.end_var;
    }

  cond = callable(TREE_CODE(cond),op0,op1,NULL,false);
  gcc_assert(cond != NULL_TREE);

  if (TREE_CODE(TREE_TYPE(cond)) != BOOLEAN_TYPE)
    {
      cond = perform_implicit_conversion(boolean_type_node, cond,
					 tf_warning_or_error);
    }

  count = compute_loop_count(&cfd);
  if (TREE_CONSTANT(count) == 0)
    {
      count = fold_build_cleanup_point_expr(TREE_TYPE(count),count);
    }

  fn = gimplify_cilk_for_stmt_1(&cfd,pre_p);

  
  /* we have to restore the current function name */
  current_function_decl =  parent_function;
  

  switch (TYPE_PRECISION(cfd.count_type))
    {
    case 32:
      libfun=cilk_c_declare_looper("__cilkrts_cilk_for_32",
				   unsigned_intSI_type_node);
      cilk_for_32_fndecl=libfun;
    
      break;
    case 64:
      libfun=cilk_c_declare_looper("__cilkrts_cilk_for_64",
				   unsigned_intDI_type_node);
      cilk_for_64_fndecl=libfun;
      break;
    default:
      gcc_unreachable();
    }
  
  if (cfd.ctx_arg)
    {
      ctx = cfd.ctx_arg;
      if (TREE_TYPE(ctx) != ptr_type_node)
	{
	  ctx = build1(NOP_EXPR,ptr_type_node,ctx);
	}
      if (DECL_P(ctx) == 0)
	{
	  ctx = get_formal_tmp_var(ctx,&inner_seq);
	}
      fn = build1 (ADDR_EXPR, build_pointer_type(TREE_TYPE(fn)),fn);
    }
  else
    {
      gcc_assert(fn != NULL_TREE);
      ctx = build1(ADDR_EXPR,ptr_type_node,fn /* ,integer_one_node */);
      ctx = get_formal_tmp_var(ctx,&inner_seq);
      fn = build1(ADDR_EXPR,build_pointer_type(TREE_TYPE(fn)),
		  fn /* , integer_zero_node */);
    }
  TREE_CONSTANT(fn) = 1;
  fn = get_formal_tmp_var(fn,&inner_seq);

  /* we have to restore the current function name */
  current_function_decl=parent_function;

  if (grain == NULL_TREE)
    {
      grain = integer_zero_node;
    }
  else if (TYPE_MAIN_VARIANT(TREE_TYPE(grain)) !=
	   TYPE_MAIN_VARIANT(cfd.count_type))
    {
      grain = convert(cfd.count_type,grain);
    }

  
  new_for = fold_build_cleanup_point_expr 
    (void_type_node, build3 (COND_EXPR,void_type_node, cond,
			     build_call_expr(libfun, 4, fn, ctx, count, grain),
			     build_empty_stmt(UNKNOWN_LOCATION)));

  
  gimplify_and_add(new_for,&inner_seq);

  gimple_seq_add_seq(pre_p,inner_seq);

  release_cilk_for_desc(&cfd);


  
  return;
}
#if 0
static bool
declare_one_free_variable (const void *var0, void **map0, 
                           void *p ATTRIBUTE_UNUSED)
{
  tree d = (const tree)var0; /* a VAR_DECL or PARM_DECL */
  tree map=(tree)*map0;
  tree var_type = TREE_TYPE (d), arg_type;
  bool by_reference;
  tree parm;

  if (map == error_mark_node)
    return true;
  if (map == d)
    return true;
  

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
  arg_type = type_passed_as (var_type);

  /* See cp/decl.2.c: build_artificial_parm and cp_build_parm_decl. */
  parm = build_decl (UNKNOWN_LOCATION , PARM_DECL, NULL_TREE, var_type);
  DECL_ARG_TYPE (parm) = arg_type;
  DECL_ARTIFICIAL (parm) = 1;
  TREE_READONLY(parm)=1;
  /*DECL_SOURCE_LOCATION (parm) = input_location;
    DECL_CONTEXT (parm) = current_function_decl;*/
  /* TREE_PURPOSE holds the expression to be substiuted.
     For an output parameter this is an indirect_ref. */
  if (by_reference) {
    parm = build_indirect_ref (0, parm, /* "Cilk wrapper generation" */ 
			       RO_CILK_WRAPPER_GENERATION);
    parm = build1 (PAREN_EXPR, void_type_node,parm);
  }
  else
    TREE_READONLY (parm) = 1;
  if (by_reference)
    parm = build1 (SPAWN_STMT, void_type_node, parm);

  *map0=parm;
  
  return true;
}




static void
declare_free_variables (struct pointer_map_t *fvars)
{
  /* splay_tree_foreach (fvars, declare_one_free_variable, 0); */
  pointer_map_traverse(fvars,declare_one_free_variable,0);
}
    
static tree
find_call (tree exp)
{
  enum tree_code code = TREE_CODE (exp);
  if (code == CALL_EXPR)
    return exp;

  /* Only the right hand side of an assignment can be spawned. */
  if (code == MODIFY_EXPR || code == INIT_EXPR)
    return find_call (TREE_OPERAND (exp, 1));

  if (TREE_CODE_CLASS (code) == tcc_unary)
    return find_call (TREE_OPERAND (exp, 0));

  /* Don't try too hard yet. */
  return NULL_TREE;
}

static const char *
wrapped_function_name (tree exp)
{
  tree fn = find_call (exp);
  if (fn == NULL_TREE)
    return NULL;
  fn = TREE_OPERAND (fn, 0);
  if (TREE_CODE (fn) != ADDR_EXPR)
    return NULL;
  fn = TREE_OPERAND (fn, 0);
  if (TREE_CODE (fn) == FUNCTION_DECL)
    {
      enum tree_code op = DECL_OVERLOADED_OPERATOR_P (fn);
      if (op != ERROR_MARK)
	return operator_name_info[op].mangled_name;
      if (DECL_CONSTRUCTOR_P (fn))
	return "ctor";
      if (DECL_DESTRUCTOR_P (fn))
	return "dtor";
      /* Spawn of an expression returning a struct by value
	 can appear as a spawn of __builtin_memcpy.  Don't
	 confuse matters by putting that name in assembly. */
      if (DECL_BUILT_IN (fn))
	return NULL;
      if (DECL_NAME (fn))
	return IDENTIFIER_POINTER (DECL_NAME (fn));
    }
  return NULL;
}
#endif

struct wrapper_data
{
  /* Types and variables associated with the generated wrapper
     function. */
  tree wrapper_parms, wrapper_args, wrapper_types;
};


static int cilk_wrapper_cnt;
#if 0
static tree
build_cilk_wrapper_decl (tree fntype, const char *wrapped)
{
  char name[48];
  tree fndecl;
  
  /* Make a name for this wrapper.
     12 characters plus <~6 wrapper counter plus up to 24 characters
     called function name to aid human debugging. */
  if (wrapped && strlen (wrapped) < 24)
    sprintf (name, "__cilk_spawn_%s_%03d", wrapped, ++cilk_wrapper_cnt);
  else
    sprintf (name, "__cilk_spawn_%03d", ++cilk_wrapper_cnt);

  /* Build the new function type.  In general, the argument list
     has changed. */

  fndecl = build_lang_decl (FUNCTION_DECL, get_identifier (name), fntype);

  TREE_PUBLIC (fndecl) = 0;
  TREE_STATIC (fndecl) = 1;
  TREE_USED (fndecl) = 1;
  DECL_ARTIFICIAL (fndecl) = 1;
  /* XXX INTERFACE_KNOWN may be wrong if this function later
     turns out not to be needed? */
  DECL_INTERFACE_KNOWN (fndecl) = 1;
  /* A call to this function is a knot, and detaches. */
  DECL_SET_KNOT (fndecl, 1);
  /* The caller of this function may be stolen and lose its
     registers. */
  DECL_KILLS_REGISTERS (fndecl) = ~0;

  /* Inlining would defeat the purpose of this wrapper.
     Either it secretly switches stack frames or it allocates
     a stable stack frame to hold function arguments even if
     the parent stack frame is stolen. */
  DECL_UNINLINABLE (fndecl) = 1;

  /*DECL_ARGUMENTS (fndecl) = wrapper_parms;*/

  /* All this means is "don't mangle the name of this function."
     It does not actually change the calling convention. */
  SET_DECL_LANGUAGE (fndecl, lang_c);

  return fndecl;
}

static bool
wrapper_parm_cb (const void *key0, void **val0, void *data)
{
  struct wrapper_data *wd = (struct wrapper_data *)data;
  tree arg = (tree)key0;
  tree val = (tree)*val0;
  tree parm;

  if (val == error_mark_node || val == arg)
    {
      return false;
    }

  if (TREE_CODE (val) == SPAWN_STMT)
    {
      /* We should not reach here with a register receiver.
	 We may see a register variable modified in the
	 argument list.  Because register variables are
	 worker-local we don't need to work hard to support
	 them in code that spawns. */
      if (TREE_CODE (arg) == VAR_DECL && DECL_HARD_REGISTER (arg))
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
	  arg = build_address (arg);
	}
      val = TREE_OPERAND (val, 0);
      *val0=val;
      gcc_assert (TREE_CODE (val) == INDIRECT_REF);
      parm = TREE_OPERAND (val, 0);
      STRIP_NOPS (parm);
    }
  else
    {
      parm = val;
    }
  TREE_CHAIN (parm) = wd->wrapper_parms;
  wd->wrapper_parms = parm;
  wd->wrapper_types = tree_cons (NULL_TREE, TREE_TYPE (parm), wd->wrapper_types);
  wd->wrapper_args = tree_cons (NULL_TREE, arg, wd->wrapper_args);
  return true;
}


tree
cilk_get_worker_tree (bool for_call)
{
  switch (cilk_worker_regno)
    {
    case CILK_WORKER_TLS:
      /* XXX Should this refer to the function scope copy of DECL? */
      return cilk_worker_var_decl;

    case CILK_WORKER_PARM:
      /* The function call form has the advantage of suppressing CSE.
	 The function could be marked as returning a strand-specific
	 value. */
      return build_function_call (UNKNOWN_LOCATION,cilk_trees[CILK_TI_F_WORKER], NULL_TREE);

    default:
      return for_call ? NULL_TREE : cilk_worker_var_decl;
    }
}




/* Generate the body of a wrapper function that assigns the
   result of the expression RHS into RECEIVER.  RECEIVER must
   be NULL if this is not a spawn -- the wrapper will return
   a value.  If this is a spawn the wrapper will return void. */

static tree
build_cilk_wrapper_body (tree stmt,
			 struct pointer_map_t *decl_map,
			 tree *args_out, tree frame,
			 const char *fname,
			 bool delayed)
{
  struct wrapper_data wd;
  tree outer_fndecl = current_function_decl;
  tree wrapper_fntype;
  tree fndecl;
  tree frame_parm = NULL_TREE;
  tree body;

  /* It's not clear exactly where this should go.  Moving it early
     prevents expressions from appearing to belong in the outer
     function. */
  push_to_top_level ();

  /* Build the type of the wrapper and its argument list from the
     variables that it requires.  */

  wd.wrapper_args = NULL_TREE;
  wd.wrapper_parms = NULL_TREE;
  wd.wrapper_types = void_list_node;

  /* Generate the parameter for the parent frame to be pushed onto
     the spawn stack.  cilk_insert_detach requires this to be the
     last parameter to distinguish it from any other anonymous
     parameters that happen to have the same type. */
  if (frame != NULL)
    {
      /* Debugging tip:
	 Accidental use of FRAME rather than FRAME_PARM will cause
	 a crash during gimplification due to the frame variable
	 being used in a context other than the function it was
	 declared in.  This causes its address not to be considered
	 invariant when it originally was invariant. */
      frame_parm = build_decl (UNKNOWN_LOCATION, PARM_DECL, NULL_TREE, TREE_TYPE (frame));
      DECL_ARG_TYPE (frame_parm) = type_passed_as (TREE_TYPE (frame));
      DECL_ARTIFICIAL (frame_parm) = 1;
      TREE_READONLY (frame_parm) = 1;
      gcc_assert (wd.wrapper_parms == NULL_TREE);
      TREE_CHAIN (frame_parm) = NULL_TREE /*wd.wrapper_parms*/;
      wd.wrapper_args = tree_cons (NULL_TREE, frame, wd.wrapper_args);
      wd.wrapper_parms = frame_parm;
      wd.wrapper_types = tree_cons (NULL_TREE, TREE_TYPE (frame), wd.wrapper_types);
    }
  /*
    splay_tree_foreach (decl_map, wrapper_parm_cb, &wd);
  */
  pointer_map_traverse(decl_map, wrapper_parm_cb, &wd);
  
  if (!delayed)
    {
      tree ctx = build_decl (UNKNOWN_LOCATION, PARM_DECL, NULL_TREE,
			     ptr_type_node);
      DECL_ARG_TYPE (ctx) = ptr_type_node;
      DECL_ARTIFICIAL (ctx) = 1;
      TREE_READONLY (ctx) = 1;
      TREE_CHAIN (ctx) = wd.wrapper_parms;
      wd.wrapper_args = tree_cons (NULL_TREE, error_mark_node, wd.wrapper_args);
      wd.wrapper_parms = ctx;
      wd.wrapper_types = tree_cons (NULL_TREE, ptr_type_node, wd.wrapper_types);
    }

  /* Now build a function.

     Its return type is void (all side effects are via explicit parameters).

     Its parameters are WRAPPER_PARMS with type WRAPPER_TYPES.

     Actual arguments in the caller are WRAPPER_ARGS.

  */

  wrapper_fntype = build_function_type(void_type_node, wd.wrapper_types);
  SET_FUNCTION_TYPE_LINKAGE(wrapper_fntype,linkage_cilk);

  /* We are committed to building a wrapper.  Start working on it.

     Emit a function that takes WRAPPER_PARMS incoming and applies
     ARGS (modified) to the wrapped function.  Return the wrapper
     and modified ARGS to the caller to generate a function call. */

  fndecl = build_cilk_wrapper_decl (wrapper_fntype, fname);

  if (!delayed)
    {
      DECL_CONTEXT (fndecl) = outer_fndecl;
      DECL_STATIC_CHAIN (fndecl) = 1;
      DECL_EXPLICIT_STATIC_CHAIN (fndecl) = 1;
      gcc_assert (TREE_VALUE (wd.wrapper_args) == error_mark_node);
      TREE_VALUE (wd.wrapper_args) = build2 (FDESC_EXPR, ptr_type_node,
					     fndecl, integer_one_node);
    }
  DECL_UNINLINABLE (fndecl) = 1;

  /*push_to_top_level ();*/
  /* Language context must be Cilk to be calling spawn, so
     there is no need to touch it here. */
  /* XXX Suppressed for now because a cilk function not declared
     in extern "Cilk++" has language name C++.  This needs to be
     fixed? */
  /*gcc_assert (current_lang_name == lang_name_cilk);*/

  start_preparsed_function (fndecl, NULL_TREE, SF_PRE_PARSED);

  cfun->detaches = 1;
  cfun->is_cilk_function = 1;

  /* This must be begin_compound_stmt (BCS_FN_BODY) rather than
     begin_function_body () in order to create a scope to hold the
     VAR_DECL used to hold the wrapped function return value. */
  body = begin_compound_stmt (BCS_FN_BODY);

  /* If this is a Cilk function, initialize the frame descriptor.
     Ordinarily this would be done in begin_function_body. */
  if (cfun->linkage == linkage_cilk)
    cilk_init_frame_descriptor (cfun, true);

  /* If this is a spawned statement, detach immediately.
     If this is a spawned call the detach point is somewhere
     deep inside. */
  if (! delayed)
    add_stmt (cilk_detach (cilk_get_worker_tree (false)));

  /* cp_genericize will do the invisible reference fixup */

  /* Any wrapper parameters that are used after the detach
     need to be copied if they may be passed in memory.

     Any wrapper parameters that are passed by hidden reference
     contrary to the pass-by-value appearing in the C code,
     need to be copied here using a copy constructor if any
     use follows the detach.  (This interacts with constructor
     elision, which may need to be forced on temporarily.)

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
  cilk_outline (outer_fndecl, fndecl, &stmt, decl_map,
		delayed ? CILK_BLOCK_BLOCK : CILK_BLOCK_CALL);
  
  add_stmt (fold_build_cleanup_point_expr (void_type_node, stmt));

  finish_function_body (body);

  defer_mark_used_calls=false;
  expand_or_defer_fn (finish_function (SF_CILK_NESTED));

  pop_from_top_level ();

  *args_out = wd.wrapper_args;

  return fndecl;
}
	
static tree
build_cilk_wrapper (tree exp, tree *args_out, bool delayed)
{
  /* Free variables of EXP and ARGS.  This is a map.
     The key holds the *_DECL of the environment.
     The value holds one of:
     error_mark_node, if this variable is local to the spawned statement;
     integer_zero_node, during construction if this variable is not (yet)
     modified;
     integer_one_node, during construction if this variable is modified;
     PARM_DECL, after construction
  */
  struct pointer_map_t *fvars;
  tree fndecl;
  tree parent_frame;
  const char *fname = delayed ? wrapped_function_name (exp) : "block";

  /* The parent frame descriptor is needed for the detach
     operation.  It is passed as an argument.  It could
     also be computed in the child. */
  parent_frame = build_address (cfun->cilk_frame_decl);

  cfun->is_cilk_function = 1;
  fvars = pointer_map_create();
  extract_free_variables (exp, fvars, ADD_READ,
			  delayed ? CILK_BLOCK_CALL : CILK_BLOCK_BLOCK);
  declare_free_variables (fvars);

  /* ??? add_decl_expr for scope? */

  /* Now fvars maps old variable to incoming variable.  Update
     the expression and arguments to refer to the new names. */

  fndecl = build_cilk_wrapper_body (exp, fvars, args_out,
				    parent_frame, fname,
				    delayed);

  /* splay_tree_delete (fvars); */
  pointer_map_destroy(fvars);

  return build_addr_func (fndecl);
}
#endif




#if 0
static void
print_node_simple (FILE *fp, tree var)
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
#endif 
#if 0
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
      return TREE_CODE (exp) == ADDR_EXPR;
    case tcc_comparison:
      /* We need the spawn as operand 0 for now.   That's where it
	 appears in the only case we really care about, conversion
	 to bool. */
      return TREE_CODE (TREE_OPERAND (exp, 1)) == INTEGER_CST;
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
  enum built_in_class b;
  if (TREE_CODE (exp) != ADDR_EXPR)
    return false;
  exp = TREE_OPERAND (exp, 0);
  if (TREE_CODE (exp) != FUNCTION_DECL)
    return false;
  b = DECL_BUILT_IN_CLASS (exp);
  if (b == BUILT_IN_NORMAL)
    return DECL_FUNCTION_CODE (exp) == BUILT_IN_MEMCPY;
  if (b == NOT_BUILT_IN)
    return (DECL_CONSTRUCTOR_P (exp) ||
	    (DECL_OVERLOADED_OPERATOR_P (exp) != ERROR_MARK));
  return false;
}
#endif

static tree
look_for_spawn (tree *tp,
		int *walk_subtrees ATTRIBUTE_UNUSED,
		void *data)
{
  tree t = *tp;
  enum tree_code c = TREE_CODE (t);
  enum tree_code_class cc = TREE_CODE_CLASS (c);
  if ((c == CALL_EXPR && SPAWN_CALL_P (t))
      || (c == AGGR_INIT_EXPR && AGGR_INIT_VIA_SPAWN_P (t)))
    return t;
  if (cc == tcc_constant || cc == tcc_type
      /* If data is set, this call does not want to look inside spawned
	 subblocks. */
      || (data != NULL && (c == SPAWN_STMT || c == CILK_FOR_STMT)))
    *walk_subtrees = 0;
  return 0;
}


void
mark_receiver_addressable (tree lhs, tree rhs)
{
  if (current_function_decl
      && !is_cilk_function_type (TREE_TYPE (current_function_decl)))
    return;

  if (walk_tree_without_duplicates (&rhs, look_for_spawn, 0) == 0)
    return;

  if (TREE_CODE (lhs) == VAR_DECL && DECL_HARD_REGISTER (lhs))
    {
      error ("explicit register variable %qD may not be assigned with spawn",
	     lhs);
      return;
    }
  cxx_mark_addressable (lhs);
  return;
}
#if 0
/* Given a statement to be spawned generate the surrounding code
   to make the runtime treat the expression as a spawn. */
void
cp_gimplify_cilk_spawn (tree *spawn_p,
			gimple_seq *before ATTRIBUTE_UNUSED,
			gimple_seq *after ATTRIBUTE_UNUSED)
{
  bool delayed;
  tree expr = *spawn_p;
  tree function, new_args;

  gcc_assert (TREE_CODE (expr) == SPAWN_STMT);
  gcc_assert (VOID_TYPE_P (TREE_TYPE (expr)));

  if (cfun->cilk_frame_decl == NULL_TREE)
    {
      location_t where = EXPR_LOCATION (expr);
      gcc_assert (cfun->linkage != linkage_cilk);
      error_at (where, "Cilk spawn in non-Cilk function");
      *spawn_p = TREE_OPERAND (expr, 0);
      return;
    }

  cfun->calls_spawn = 1;

  delayed = SPAWN_DELAYED_DETACH_P (expr);
#if ENABLE_TREE_CHECKING
  if (! delayed)
    /* Make sure we won't try to detach twice. */
    {
      tree subspawn = walk_tree_without_duplicates (&TREE_OPERAND (expr, 0), look_for_spawn, expr);
      gcc_assert (subspawn == NULL_TREE);
    }
#endif
  expr = TREE_OPERAND (expr, 0);

  if (TREE_CODE (expr) == CONVERT_EXPR && VOID_TYPE_P (TREE_TYPE (expr)))
    expr = TREE_OPERAND (expr, 0);

  new_args = NULL;
  function = build_cilk_wrapper (expr, &new_args, delayed);
  /* A spawn wrapper has void type. */
  *spawn_p = build3 (CALL_EXPR, void_type_node, function, new_args, NULL_TREE);
}
#endif
#if 0
static tree
add_field (const char *name, tree type, tree fields)
{
  tree field = build_decl (UNKNOWN_LOCATION,FIELD_DECL,
			   get_identifier (name), type);
  TREE_CHAIN (field) = fields;
  return field;
}
#endif
#if 0 /* we do not support Cilkscreen in this release */
static tree
cilk_declare_looper (tree type,
		     enum function_linkage linkage ATTRIBUTE_UNUSED)
{
  /* namespace cilk {
     int cilk_for_loop(void (*)(void *, T, T), void *, T, T); }
  */
  tree cb, ft, fn;

  push_namespace (get_identifier ("cilk"));

  cb = build_function_type_list (void_type_node,
				 ptr_type_node, type, type,
				 NULL_TREE);
  SET_FUNCTION_TYPE_LINKAGE(cb,linkage_cilk);
  cb = build_pointer_type (cb);

  ft = build_function_type_list(void_type_node,
				cb, ptr_type_node, type,
				type, NULL_TREE);
  SET_FUNCTION_TYPE_LINKAGE(ft,linkage_cilk);
  fn = build_cp_library_fn_ptr ("cilk_for_loop", ft);
  TREE_NOTHROW (fn) = 0;

  pop_namespace ();

  return fn;
}

static tree
cilk_declare_runner (void)
{
  /* namespace cilk { void run_cilk_fn(void (*)(void *), void *); } */
  tree cb, ft, fn;
 
  push_namespace (get_identifier ("cilk"));

  cb = build_function_type_list (void_type_node,
				 ptr_type_node, NULL_TREE);
  SET_FUNCTION_TYPE_LINKAGE(cb,linkage_cilk);
  cb = build_pointer_type (cb);

  ft = build_function_type_list ( void_type_node,
				  cb, ptr_type_node, NULL_TREE);
  SET_FUNCTION_TYPE_LINKAGE(ft,linkage_native);
  fn = build_cp_library_fn_ptr ("run_cilk_fn", ft);

  pop_namespace ();

  return fn;
}

/* Declare the user-visible builtin Cilkscreen functions */
static void
declare_cilkscreen_functions (void)
{
  tree i_list = tree_cons (NULL_TREE, integer_type_node, void_list_node);
  tree p_list = tree_cons (NULL_TREE, ptr_type_node, void_list_node);
  tree pp_list = tree_cons (NULL_TREE, ptr_type_node, p_list);
  tree ftype_v_v = build_function_type (void_type_node, void_list_node);
  tree x;
  tree metadata_fndecl;
  tree hypercall_fndecl;
  tree clean_fndecl, write_fndecl;
  tree enable_checking_fndecl;
  tree disable_checking_fndecl;
  tree acquire_lock_fndecl;
  tree release_lock_fndecl;

  /* extern void __cilkscreen_metadata(int, int) */
  x = tree_cons (NULL_TREE, integer_type_node, i_list);
  metadata_fndecl =
    build_library_fn_ptr ("__cilkrts_metadata",
			  build_function_type (void_type_node, x));
  DECL_BUILT_IN_CLASS (metadata_fndecl) = BUILT_IN_FRONTEND;
  DECL_FUNCTION_CODE (metadata_fndecl) = BUILT_IN_CILK_METADATA;
  pushdecl (metadata_fndecl);
  /* Make declaration available for internal use too. */
  cilk_metadata_fndecl = metadata_fndecl;

  /* extern void __cilkscreen_hypercall(int, void *) */
  x = tree_cons (NULL_TREE, integer_type_node, p_list);
  hypercall_fndecl =
    build_library_fn_ptr ("__cilkscreen_hypercall",
			  build_function_type (void_type_node, x));
  /* Not a builtin function because identity matters. */
  pushdecl (hypercall_fndecl);

  /* extern void __cilkscreen_clean(void *, void *) */
  clean_fndecl =
    build_library_fn_ptr ("__cilkscreen_clean",
			  build_function_type (void_type_node, pp_list));
  DECL_BUILT_IN_CLASS (clean_fndecl) = BUILT_IN_FRONTEND;
  DECL_FUNCTION_CODE (clean_fndecl) = BUILT_IN_CILK_CLEAN;
  pushdecl (clean_fndecl);

  /* extern void __cilkscreen_write(void *, void *) */
  write_fndecl =
    build_library_fn_ptr ("__cilkscreen_write",
			  build_function_type (void_type_node, pp_list));
  DECL_BUILT_IN_CLASS (write_fndecl) = BUILT_IN_FRONTEND;
  DECL_FUNCTION_CODE (write_fndecl) = BUILT_IN_CILK_WRITE;
  pushdecl (write_fndecl);

  /* extern void ___cilkscreen_disable_checking (void) */
  /* extern void ___cilkscreen_enable_checking (void) */
  enable_checking_fndecl =
    build_library_fn_ptr ("__cilkscreen_enable_checking", ftype_v_v);
  DECL_BUILT_IN_CLASS (enable_checking_fndecl) = BUILT_IN_FRONTEND;
  DECL_FUNCTION_CODE (enable_checking_fndecl) = BUILT_IN_CILK_ENABLE;
  pushdecl (enable_checking_fndecl);
  disable_checking_fndecl =
    build_library_fn_ptr ("__cilkscreen_disable_checking", ftype_v_v);
  DECL_BUILT_IN_CLASS (disable_checking_fndecl) = BUILT_IN_FRONTEND;
  DECL_FUNCTION_CODE (disable_checking_fndecl) = BUILT_IN_CILK_DISABLE;
  pushdecl (disable_checking_fndecl);

  /* extern void __cilkscreen_acquire_lock (void *) */
  acquire_lock_fndecl =
    build_library_fn_ptr ("__cilkscreen_acquire_lock",
			  build_function_type (void_type_node, p_list));
  DECL_BUILT_IN_CLASS (acquire_lock_fndecl) = BUILT_IN_FRONTEND;
  DECL_FUNCTION_CODE (acquire_lock_fndecl) = BUILT_IN_CILK_ACQUIRE;
  pushdecl (acquire_lock_fndecl);

  /* extern void __cilkscreen_release_lock (void *) */
  release_lock_fndecl =
    build_library_fn_ptr ("__cilkscreen_release_lock",
			  build_function_type (void_type_node, p_list));
  DECL_BUILT_IN_CLASS (release_lock_fndecl) = BUILT_IN_FRONTEND;
  DECL_FUNCTION_CODE (release_lock_fndecl) = BUILT_IN_CILK_RELEASE;
  pushdecl (release_lock_fndecl);
  return;
}
#endif

/* How is thread-local storage for Cilk runtime objects accessed?
   By default they must be in the initial executable.  In the
   common method for accessing thread local storage an access
   will require two memory references in PIC mode and one reference
   otherwise.

   On some targets it may not be possible to use the most general
   model (e.g. shared library referencing a TLS symbol defined by
   another shared library).  Runtime relocation is performed
   invisibly by a C function that requires the C stack to be valid.

   In the Windows version of Cilk adding a level of indirection
   to worker access was observed to have a significant effect
   on performance. */
enum tls_model
cilk_tls_model (tree decl ATTRIBUTE_UNUSED)
{
#ifdef CILK_TLS_MODEL
  return CILK_TLS_MODEL(decl);
#else
  /* If the variable reference OR the variable is in a shared library,
     the slower local-exec mode must be used.  In 1.0 the Cilk thread
     local variables were forced to be linked into the initial
     executable. */
  /*
    if (!flag_cilk_pic)
    return TLS_MODEL_LOCAL_EXEC;
  */
  return TLS_MODEL_INITIAL_EXEC;
#endif
}


void
cilk_init_common (void)
{
  int i;
  for (i = 0; i < CILK_TI_MAX; i++)
    {
      tree field = cilk_trees[i];
      if (field == NULL_TREE || TREE_CODE (field) != FIELD_DECL)
	{
	  cilk_field_offsets[i] = -1;
	  continue;
	}
      else
	{
	  tree off1 = DECL_FIELD_OFFSET (field); /* bytes */
	  tree off2 = DECL_FIELD_BIT_OFFSET (field); /* bits */
	  cilk_field_offsets[i] = tree_low_cst (off1, 0) + tree_low_cst (off2, 0) / BITS_PER_UNIT;
	}
    }

  cilk_dynamic_libfunc = gen_rtx_SYMBOL_REF (Pmode, "__cilkrts_allocate_dynamic");
  /* This is not currently used:
     cilk_save_ra = GET_CODE (INCOMING_RETURN_ADDR_RTX) == MEM;*/

  if (flag_cilkscreen)
    cilk_metadata_default_section = get_section (".clkmtdt", SECTION_DEBUG|SECTION_NOTYPE, 0);
}

#if 0
void
cilk_update_worker_decl (void)
{
  int regno = cilk_worker_regno;

  if (regno >= 0 && call_used_regs[regno])
    {
      error ("Invalid Cilk register %s (must be call-saved)", reg_names[regno]);
      cilk_worker_regno = regno = CILK_WORKER_PARM;
      return;
    }

  if (regno == CILK_WORKER_TLS)
    {
      
      /* DECL_TLS_MODEL (cilk_worker_var_decl) = cilk_tls_model (cilk_worker_var_decl); */
      DECL_EXTERNAL (cilk_worker_var_decl) = 1;
      TREE_PUBLIC (cilk_worker_var_decl) = 1;
      cilk_worker_var_decl = pushdecl (cilk_worker_var_decl);

      cilk_c_stack_decl = build_decl (UNKNOWN_LOCATION, VAR_DECL,
				      get_identifier ("__cilkrts_c_stack"),
				      build_qualified_type (ptr_type_node,
							    TYPE_QUAL_VOLATILE));
      DECL_TLS_MODEL (cilk_c_stack_decl) = cilk_tls_model (cilk_c_stack_decl);
      DECL_EXTERNAL (cilk_c_stack_decl) = 1;
      TREE_PUBLIC (cilk_c_stack_decl) = 1;
      /* The Debian toolchain as of May, 2009 does not support
	 TLS with either (1) @nobits sections, or (2) .gnu.linkonce
	 to merge sections.  The tools silently generate incorrect
	 relocations. */
#ifdef LINUX_LINKER_WORKS
      {
	/* XXX We assume GNU linker if TLS is used.  Is this safe? */
	const char *sname = ".gnu.linkonce.tb.__cilkrts_worker";
	tree sname_s = build_string (strlen (sname), sname);
	DECL_SECTION_NAME (cilk_worker_var_decl) = sname_s;
	if (cilk_c_stack_decl)
	  DECL_SECTION_NAME (cilk_c_stack_decl) = sname_s;
      }
#endif
      return;
    }
  else if (0)
    {
      /* The worker is NULL outside of Cilk functions. */
      tree zero;
      zero = build_decl (UNKNOWN_LOCATION,CONST_DECL,
			 get_identifier ("__cilkrts_worker"),
			 build_qualified_type (ptr_type_node, TYPE_QUAL_CONST));
      TREE_READONLY (zero) = 1;
      TREE_CONSTANT (zero) = 1;
      /* TREE_INVARIANT (zero) = 1; */
      DECL_INITIAL (zero) = null_pointer_node;
      pushdecl (zero);
    }

  if (cilk_worker_var_decl && regno >= 0)
    {
      DECL_REGISTER (cilk_worker_var_decl) = 1;
      DECL_HARD_REGISTER (cilk_worker_var_decl) = 1;
      set_user_assembler_name (cilk_worker_var_decl, reg_names[regno]);
    }

  /* (*__cilkrts_worker).stack */
  /*cilk_c_stack_decl = NULL_TREE;*/
  /*build3 (COMPONENT_REF, ptr_type_node,
    build1 (INDIRECT_REF,
    TREE_TYPE (TREE_TYPE (cilk_worker_var_decl)),
    cilk_worker_var_decl),
    cilk_trees[CILK_TI_WORKER_STACK],
    NULL_TREE);
  */
  return;
}
#endif

static tree
cilk_find_parent_frame (void)
{
  tree parm;
  /* The current function must have an argument list.
     The parent frame argument is currently always at
     the end.  (Alternatively, we could compute it using
     a builtin function.) */
  parm = DECL_ARGUMENTS (current_function_decl);
  gcc_assert (parm != 0);
  while (TREE_CHAIN (parm))
    parm = TREE_CHAIN (parm);
  gcc_assert (DECL_ARTIFICIAL (parm) && TREE_READONLY (parm));
  gcc_assert (TREE_CODE (TREE_TYPE (parm)) == POINTER_TYPE);
  return parm;
}




tree
cilk_detach (tree worker_ptr)
{
  tree parent_frame;
  tree call;
  tree detacher;
  tree pre = 0;
  gimple_seq pre_seq;

  parent_frame = cilk_find_parent_frame ();
  detacher = cilk_detach_fndecl;

  worker_ptr = get_formal_tmp_var (worker_ptr, &pre_seq);

  call = build3 (CALL_EXPR, void_type_node,
                 build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (detacher)), detacher),
                 tree_cons (NULL_TREE, worker_ptr,
                            tree_cons (NULL_TREE, parent_frame, NULL_TREE)),
                 NULL_TREE);

  gimplify_and_add (call, &pre_seq);

  return pre;
}



/* A Cilk function needs a frame descriptor if it or its
   children invoke spawn.

   Now: Every Cilk function gets a frame descriptor as
   begin_function_body calls cilk_init_frame descriptor.

   Later: Cilk functions which call other Cilk functions
   get frame descriptors during the tree rewriting pass.
   If anybody has set cilk_frame_decl non-NULL, insert a
   declaration and initialization at the start of the
   function.

   Much later: Interprocedural analysis can mark Cilk
   functions as never calling spawn and propagate that
   information up the call graph.

   Currently the frame descriptor is marked volatile because
   otherwise it will be optimized out because it is not
   visibly used.  Marking an implicit use by calls would
   fix this -- no calls means no frame descriptor.  */

tree
cilk_make_frame_descriptor (struct function *fdesc)
{
  tree frame_v_type;
  tree frame;

  frame_v_type = build_qualified_type (cilk_frame_type_decl, TYPE_QUAL_VOLATILE);
  TREE_ADDRESSABLE (frame_v_type) = 1;
  layout_type (frame_v_type);

  frame = build_decl (UNKNOWN_LOCATION,VAR_DECL, get_identifier ("__cilkrts_frame"), frame_v_type);  
  DECL_IGNORED_P (frame) = 1;
  DECL_ARTIFICIAL (frame) = 1;
  /* It doesn't really make sense for this not to be marked addressable
     because it can't be scalarized and it might have its address taken. */
  TREE_ADDRESSABLE (frame) = 1;
  TREE_THIS_VOLATILE (frame) = 1;
  TREE_SIDE_EFFECTS (frame) = 1;

  /* Lie to gimplify and say we saw this in a BIND_EXPR.
     But see comment near end of gimplify_decl_expr. */
  DECL_CONTEXT (frame) = fdesc->decl;
  DECL_SEEN_IN_BIND_EXPR_P (frame) = 1;

  layout_decl (frame, 0);



  return frame;
}

void
cilk_init_frame_descriptor (struct function *fdesc,
			    bool is_wrapper ATTRIBUTE_UNUSED)
{
  tree frame;

  frame = cilk_make_frame_descriptor (fdesc);
  /* Calling pushdecl makes the frame descriptor appear used.
     See if we can do without. */
  /*frame = pushdecl(frame);*/

  /* This should be called exactly once per Cilk function. */
  gcc_assert (fdesc->cilk_frame_decl == NULL_TREE);
  fdesc->cilk_frame_decl = frame;
  /*finish_decl (frame, NULL_TREE, NULL_TREE);*/
  add_decl_expr (frame);

  /* It is currently the responsibility of machine-dependent
     code to make __cilkrts_frame.stack_pointer correct when
     it needs to be.  The stack pointer should be constant in
     Cilk functions so the machine-independent code could also
     store it into the frame. */
}




/* FNDECL has been promoted.  Fix up its struct function. */
void
cilk_promote_struct_function (tree fndecl)
{
  struct function *f = DECL_STRUCT_FUNCTION (fndecl);
  tree fntype = TREE_TYPE (fndecl);
  tree return_type = TREE_TYPE (fntype);

  if (f == 0)
    return;

  gcc_assert (FUNCTION_TYPE_LINKAGE (fntype) == linkage_cilk);

  /* This function probably must return in memory. */
  f->returns_struct = aggregate_value_p (return_type, fntype);

  /* These shouldn't be set yet. */
  gcc_assert (!f->has_cilk_dynamic);
  gcc_assert (!f->calls_cilk_alloca);

  f->cilk_frame_decl = cilk_make_frame_descriptor (f);
  f->linkage = linkage_cilk;
}


void
cilk_promote (tree fndecl)
{
  tree type = TREE_TYPE (fndecl);
  tree ntype = build_function_linkage_variant (type, linkage_cilk);
  SET_FUNCTION_TYPE_LINKAGE(type,linkage_cilk);

#if 0 /* this may not be necessary; see call in instantiate_class_template */
  if (TREE_CODE (fndecl) == TEMPLATE_DECL)
    {
      TREE_TYPE (fndecl) = ntype;
      cilk_promote (DECL_TEMPLATE_RESULT (fndecl));
      return;
    }
#endif

  gcc_assert (TREE_CODE (fndecl) == FUNCTION_DECL);

  if (type != ntype)
    {
      TREE_TYPE (fndecl) = ntype;
      cilk_promote_struct_function (fndecl);
    }
}

static bool
is_cp_cilk_tree (tree t)
{
  switch (TREE_CODE (t))
    {
    case CILK_FOR_STMT:
      return true;
    case AGGR_INIT_EXPR:
      return AGGR_INIT_VIA_SPAWN_P (t);
    default:
      return false;
    case CALL_EXPR:
      if (SPAWN_CALL_P (t))
	return true;
      t = get_callee_fndecl (t);
      if (!t ||
	  !DECL_BUILT_IN (t) ||
	  (DECL_BUILT_IN_CLASS (t) != BUILT_IN_FRONTEND))
	return false;
      switch (DECL_FUNCTION_CODE (t))
	{
	case BUILT_IN_CILK_DETACH:
	case BUILT_IN_CILK_FRAME:
	  return true;
	default:
	  return false;
	}
    }
}


/* If the body of FNDECL contains calls to Cilk functions,
   promote the entire function to Cilk and return true. */
bool
cilk_maybe_promote (tree fndecl)
{
  gcc_assert (!is_cilk_function_type (TREE_TYPE (fndecl)));

  if (!cp_tree_uses_cilk (DECL_SAVED_TREE (fndecl)))
    return false;

  cilk_promote (fndecl);
  return true;
}

static tree
declare_cilk_void_wrapper (tree fn, int style)
{
  tree fndecl;
  bool has_arg;
  tree parm_type;
  char name[48];
  const char *style_s;

  gcc_assert (VOID_TYPE_P (TREE_TYPE (TREE_TYPE (fn))));

  /* The wrapper function takes an argument of type (void *)
     if it is a destructor wrapper or a finalizer run via
     atexit. */
  has_arg = (style == CILK_WRAP_DTOR || (style == CILK_WRAP_FINI && flag_use_cxa_atexit));
  if (has_arg)
    parm_type = tree_cons (NULL_TREE, ptr_type_node, void_list_node);
  else
    parm_type = void_list_node;

  switch (style)
    {
    case CILK_WRAP_INIT: style_s = "init"; break;
    case CILK_WRAP_FINI: style_s = "fini"; break;
    case CILK_WRAP_DTOR: style_s = "dtor"; break;
    default: gcc_unreachable ();
    }
  sprintf (name, "__cilk_wrap_%s_%03d", style_s, cilk_wrapper_cnt++);

  /* See also decl2.c:start_objects */

  fndecl = build_lang_decl (FUNCTION_DECL, get_identifier (name),
			    build_function_type (void_type_node, parm_type));
  TREE_PUBLIC (fndecl) = 0;
  TREE_USED (fndecl) = 0;
  /* DECL_DECLARED_INLINE (fndecl) = 0;*/ /* '0' is replaced with '1' */ 
  DECL_UNINLINABLE (fndecl) = 1;
  DECL_ARTIFICIAL (fndecl) = 1;

  /* All this means is "don't mangle the name of this function."
     It does not actually change the calling convention. */
  SET_DECL_LANGUAGE (fndecl, lang_c);

  return fndecl;
}


tree
push_cilk_void_wrapper (tree fn, int style)
{
  tree fndecl;
  tree t;

  /* See if there is already a wrapper. */
  for (t = cilk_wrappers; t != NULL_TREE; t = TREE_CHAIN (t))
    if (TREE_PURPOSE (t) == fn)
      return TREE_VALUE (t);

  fndecl = declare_cilk_void_wrapper (fn, style);
  cilk_wrappers = tree_cons (fn, fndecl, cilk_wrappers);
  return fndecl;
}

static tree
finish_cilk_void_wrapper (tree fndecl, tree wrapped)
{
  tree arglist, parm;
  tree body, fn_var, fn_addr, fnptr_type;
  /* tree outgoing; */
  tree call = NULL_TREE;
  /* tree func_call = NULL_TREE , ft1 = NULL_TREE, ft2 = NULL_TREE */;
  /* tree my_args = NULL_TREE; */

  start_preparsed_function (fndecl, NULL_TREE, SF_PRE_PARSED);

  parm = NULL_TREE;
  arglist = TYPE_ARG_TYPES (TREE_TYPE (fndecl));
  if (arglist != void_list_node)
    {
      gcc_assert (TREE_CHAIN (arglist) == void_list_node);
      parm = build_decl (UNKNOWN_LOCATION,PARM_DECL, NULL_TREE, ptr_type_node);
      DECL_ARG_TYPE (parm) = ptr_type_node;
      /* DECL_IGNORED_P means "you may optimize register allocation
	 of this compiler-generated variable because the user does
	 not expect to look at it with a debugger." */
      DECL_IGNORED_P (parm) = 1;
      DECL_ARTIFICIAL (parm) = 1;
      TREE_READONLY (parm) = 1;
    }

  body = begin_compound_stmt (BCS_FN_BODY);

  fn_addr = build_address (wrapped);
  fnptr_type = TREE_TYPE (fn_addr);

  fn_var = build_decl (UNKNOWN_LOCATION,VAR_DECL, get_identifier ("fn"),
		       fnptr_type);
  TREE_ADDRESSABLE (fn_var) = 1;
  DECL_CONTEXT (fn_var) = fndecl;
  DECL_ARTIFICIAL (fn_var) = 1;
  DECL_INITIAL (fn_var) = fn_addr;
  fn_var = pushdecl (fn_var);

  finish_expr_stmt (build_modify_expr (UNKNOWN_LOCATION,fn_var, 0,
				       INIT_EXPR, UNKNOWN_LOCATION, fn_addr,
				       0));

  finish_expr_stmt (call);

  finish_compound_stmt (body);
  fndecl = finish_function (0);
  expand_or_defer_fn (fndecl);

  return fndecl;
}

bool
finish_cilk_wrappers (void)
{
  bool rv = cilk_wrappers != NULL_TREE;

  while (cilk_wrappers)
    {
      tree wrapper = TREE_VALUE (cilk_wrappers);
      tree wrapped = TREE_PURPOSE (cilk_wrappers);
      cilk_wrappers = TREE_CHAIN (cilk_wrappers);
      finish_cilk_void_wrapper (wrapper, wrapped);
    }

  return rv;
}

/* Given a FUNCTION_DECL FN generate a C++ function that
   invokes the function via the runtime.  FN takes one
   argument of type (void *) if STYLE is CILK_WRAP_DTOR
   or STYLE is CILK_WRAP_FINI and flag_use_cxa_atexit.
   Otherwise it takes no argument.

   The wrapper function takes an argument of type (void *)
   if STYLE is CILK_WRAP_DTOR, otherwise none. */
tree
build_cilk_void_wrapper (tree fn, int style)
{
  tree fndecl;

  /* See also decl2.c:start_objects */

  fndecl = declare_cilk_void_wrapper (fn, style);

  fndecl = finish_cilk_void_wrapper (fndecl, fn);

  return fndecl;
}

static tree
uses_cilk_aux (tree *tp, int *walk_subtrees, void *data)
{
  tree t = *tp;
  enum tree_code code = TREE_CODE (t);
  union {
    void *vptr;
    bool (*bptr) (tree);
  } u;

  if (TREE_CODE_CLASS (code) == tcc_type
      || TREE_CODE_CLASS (code) == tcc_constant)
    *walk_subtrees = 0;

  /* The language-specific callback recognizes any builtin
     functions that require the caller to be Cilk. */
  if (data)
    {
      u.vptr = data;
      if ((*u.bptr)(t))
	return t;
      #if 0
      bool (**fpp)(tree) = (bool (**)(tree)) &data;
      if ((***fpp)(t))

	typedef bool (*f) (tree);
      f *fpp = (f*)&data;
      if ((*fpp)(t))
	if (data(t))
	  return t;
#endif
	
	  #if 0
	if ((*((bool (**)(tree)) &data))(t))
	  #endif
    }

  switch (code)
    {
    case SPAWN_STMT:
    case SYNC_STMT:
      return t;
    case CALL_EXPR:
      /* This may be a spawn of a C++ function before it was rewritten
	 as a call to a detaching Cilk++ function... */
      if (SPAWN_CALL_P (t))
	return t;
      /* ... or an explicit call to a Cilk++ function. */
      if (is_cilk_function_type (TREE_TYPE (TREE_TYPE (TREE_OPERAND (t, 0)))))
	return t;
      return 0;
    case PARM_DECL:
    case VAR_DECL:
      if (cfun && t == cfun->cilk_frame_decl)
	return t;
      *walk_subtrees = 0;
      return 0;
    default:
      return 0;
    }
}


static bool 
tree_uses_cilk (tree stmt, bool (*lang_fn)(tree))
{
  void *data= (void *)lang_fn;
  if (stmt==NULL_TREE)
    return false;
  else
    return (walk_tree_without_duplicates(&stmt,uses_cilk_aux,data) != 0);
}

bool
cp_tree_uses_cilk (tree t)
{
  return tree_uses_cilk (t, is_cp_cilk_tree);
}


bool
cilkish_type (tree type)
{
  enum tree_code code = TREE_CODE (type);

  /* If the argument is not a type, it is not a Cilk++ type. */
  if (TREE_CODE_CLASS (code) != tcc_type)
    return false;


  /* A Cilk++ type is:

     1. A class, struct, or union declared with Cilk++ language linkage.
     2. A function type with Cilk++ language linkage
     3. A pointer or reference to a Cilk++ type.
     4. A pointer to member of Cilk++ type (but not a pointer
     to C++ member of Cilk++ class) */
  switch (code)
    {
    case RECORD_TYPE:
      /* A pointer to member function is a RECORD_TYPE. */
      if (TYPE_PTRMEMFUNC_P (type))
	return cilkish_type (TYPE_PTRMEMFUNC_FN_TYPE (type));
    case UNION_TYPE:
      return RECORD_IS_CILK (type);

    case FUNCTION_TYPE:
    case METHOD_TYPE:
      return FUNCTION_TYPE_LINKAGE (type) == linkage_cilk;

    case TYPEOF_TYPE:
      return cilkish_type (TYPEOF_TYPE_EXPR (type));

    case ARRAY_TYPE:
    case POINTER_TYPE:
    case REFERENCE_TYPE:
    case OFFSET_TYPE:
    case COMPLEX_TYPE:
    case VECTOR_TYPE:
      return cilkish_type (TREE_TYPE (type));

      /* scalar types are not Cilk types */
    case VOID_TYPE:
    case ENUMERAL_TYPE:
    case BOOLEAN_TYPE:
    case INTEGER_TYPE:
    case REAL_TYPE:
      /* Cilk++ qualified union types are not supported */
    case QUAL_UNION_TYPE:
      /* These do not have a TREE_TYPE */
    case TEMPLATE_TEMPLATE_PARM:
    case TYPENAME_TYPE:
      /* I don't understand these */
    case UNBOUND_CLASS_TEMPLATE:
    case BOUND_TEMPLATE_TEMPLATE_PARM:
      return false;

    case TEMPLATE_TYPE_PARM:
      return cilkish_type (TEMPLATE_TYPE_DECL (type));

    case LANG_TYPE: /* should not appear */
    default:
      return false;
      gcc_unreachable();
      break;
    }
}

/* Does the list of template arguments, a TREE_VEC, contain a Cilk type?
   
   Three items may appear in the list:

   1. A NULL pointer.  Ignore this.

   2. A TREE_VEC.  Apply the function recursively.

   3. Some other tree.  Call cilkish_type to check whether it is
   a type and a Cilk type.
*/

bool
cilkish_template_args (tree args)
{
  int i, nargs;
  gcc_assert (TREE_CODE (args) == TREE_VEC);
  nargs = TREE_VEC_LENGTH (args);
  
  for (i = 0; i < nargs; i++)
    {
      tree arg = TREE_VEC_ELT (args, i);
      if (arg == 0)
	continue;
      switch (TREE_CODE (arg))
	{
	case TREE_VEC:
	  if (cilkish_template_args (arg))
	    return true;
	  break;
	case TEMPLATE_DECL:
	  if (TREE_TYPE (arg) && cilkish_type (TREE_TYPE (arg)))
	    return true;
	  break;
	default:
	  if (cilkish_type (arg))
	    return true;
	  break;
	}
    }
  return false;
}

/* Return the label corresponding to name ID, but only return
   labels valid within the current Cilk block. */
tree
cilk_block_local_label (tree id)
{
  struct cp_binding_level *b;
  tree label;

  if (id == error_mark_node)
    return id;

  label = IDENTIFIER_LABEL_VALUE (id);
  b = current_binding_level;
  while (b)
    {
      if (!VEC_empty(cp_label_binding, b->shadowed_labels))
      {
         cp_label_binding *s_label = VEC_index(cp_label_binding, 
                                              b->shadowed_labels, 0); 
      /* Use the innermost definition of an already-defined label. */
      if (label && 
          value_member (label, s_label->label))
	return label;
      }
      /* Stop searching at the end of the label scope, i.e. the
	 innermost function definition or Cilk block. */
      if (b->kind == sk_cilk_for || b->kind == sk_cilk_block
	  || b->kind == sk_function_parms)
	break;
      b = b->level_chain;
    }
  /* If we reached the outermost scope of the function without
     finding a label, make a new label with function scope. */
  if (b == 0 || b->kind == sk_function_parms)
    return lookup_label (id);
  /* We did not find a usable declaration.  If this label is
     already declared the definition must be invisible here. */
  if (label)
    warning (OPT_Wcilk_scope,
	     "Outer definition of label %qD hidden by Cilk block",
	     label);
  current_binding_level = b;
  return declare_local_label (id);
}



  
  
		   
    
