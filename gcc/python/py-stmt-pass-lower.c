/* This file is part of GCC.

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

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "coretypes.h"
#include "tm.h"
#include "opts.h"
#include "tree.h"
#include "tree-iterator.h"
#include "tree-pass.h"
#include "gimple.h"
#include "toplev.h"
#include "debug.h"
#include "options.h"
#include "flags.h"
#include "convert.h"
#include "diagnostic-core.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "target.h"
#include "cgraph.h"

#include <gmp.h>
#include <mpfr.h>

#include "vec.h"
#include "hashtab.h"

#include "gpython.h"
#include "py-dot.h"
#include "py-vec.h"
#include "py-tree.h"
#include "py-builtins.h"

static VEC(tree,gc) * gpy_stmt_pass_lower_genericify (gpy_hash_tab_t *, VEC(gpydot,gc) *);
static tree gpy_stmt_pass_lower_get_module_type (const char *, gpy_hash_tab_t *);
static void gpy_stmt_pass_lower_gen_toplevl_context (tree, tree, gpy_hash_tab_t *);

static tree gpy_stmt_pass_lower_gen_concat_identifier (const char *, const char *);
static tree gpy_stmt_pass_lower_gen_main (tree);

static
tree gpy_stmt_pass_lower_gen_concat_identifier (const char * s1,
						const char * s2)
{
  debug ("s1 = <%s> s2 = <%s>!\n", s1, s2);
  size_t s1len = strlen (s1);
  size_t s2len = strlen (s2);
  size_t tlen = s1len + s2len;

  size_t idx = 0, idy = 0;
  char buffer[tlen+3];
  strncpy (buffer,s1,s1len);

  buffer[s1len] = '.';
  for (idx = s1len+1; idx<tlen+2; ++idx)
    {
      buffer[idx] = s2[idy];
      ++idy;
    }
  buffer[tlen+1] = '\0';
  debug ("buffer = <%s>!\n", buffer);

  return get_identifier (buffer);
}

static
tree gpy_stmt_pass_lower_get_module_type (const char * s, 
					  gpy_hash_tab_t * modules)
{
  tree retval = error_mark_node;

  gpy_hashval_t h = gpy_dd_hash_string (s);
  gpy_hash_entry_t * e = gpy_dd_hash_lookup_table (modules, h);
  if (e)
    {
      if (e->data)
	retval = (tree) e->data;
    }

  return retval;
}

static
void gpy_stmt_pass_lower_gen_toplevl_context (tree module, tree param_decl,
					      gpy_hash_tab_t * context)
{
  if (module == error_mark_node)
    return;
  else
    {
      tree field = TYPE_FIELDS (module);
      do {
	gcc_assert (TREE_CODE (field) == FIELD_DECL);

	debug ("generating refernence to <%s>!\n", IDENTIFIER_POINTER(DECL_NAME (field)));
	gpy_hashval_t h = gpy_dd_hash_string (IDENTIFIER_POINTER(DECL_NAME (field)));
	tree ref = build3 (COMPONENT_REF, TREE_TYPE (field), build_fold_indirect_ref(param_decl),
			   field, NULL_TREE);
	void ** e = gpy_dd_hash_insert (h, ref, context);
	
	debug_tree (ref);
	if (e)
	  fatal_error ("problem inserting component reference into context!\n");
      } while ((field = DECL_CHAIN (field)));
    }
}

/* 
   Creates a DECL_CHAIN of stmts to fold the scalar 
   with the last tree being the address of the primitive 
*/
tree gpy_stmt_decl_lower_scalar (gpy_dot_tree_t * decl, tree * cblock)
{
  tree retval = error_mark_node;

  gcc_assert (DOT_TYPE (decl) == D_PRIMITIVE);
  gcc_assert (DOT_lhs_T (decl) == D_TD_COM);

  switch (DOT_lhs_TC (decl)->T)
    {
    case D_T_INTEGER:
      {
	retval = build_decl (UNKNOWN_LOCATION, VAR_DECL,
			     create_tmp_var_name ("P"),
			     gpy_object_type_ptr);
	append_to_statement_list (build2 (MODIFY_EXPR, gpy_object_type_ptr, retval,
					  gpy_builtin_get_fold_int_call (DOT_lhs_TC (decl)->o.integer)),
				  cblock);
      }
      break;

    default:
      error ("invalid scalar type!\n");
      break;
    }

  return retval;
}

tree gpy_stmt_decl_lower_modify (gpy_dot_tree_t * decl, tree * cblock,
				 VEC(gpy_ctx_t,gc) * context)
{
  tree retval = error_mark_node;
  gpy_dot_tree_t * lhs = DOT_lhs_TT (decl);
  gpy_dot_tree_t * rhs = DOT_rhs_TT (decl);

  /*
    We dont handle full target lists yet
    all targets are in the lhs tree.
   
    To implment a target list such as:
    x,y,z = 1

    The lhs should be a DOT_CHAIN of identifiers!
    So we just iterate over them and deal with it as such!
   */

  gcc_assert (DOT_TYPE (lhs) == D_IDENTIFIER);

  tree addr = gpy_ctx_lookup_decl (context, DOT_IDENTIFIER_POINTER (lhs));
  if (!addr)
    {
      /* since not previously declared we need to declare the variable! */
      gpy_hash_tab_t * current_context = VEC_index (gpy_ctx_t, context,
						    (VEC_length (gpy_ctx_t, context)
						     - 1)
						    );
      addr = build_decl (UNKNOWN_LOCATION, VAR_DECL,
			 get_identifier (DOT_IDENTIFIER_POINTER (lhs)),
			 gpy_object_type_ptr);

      if (!gpy_ctx_push_decl (addr, DOT_IDENTIFIER_POINTER (lhs), 
			      current_context))
	fatal_error ("error pushing decl <%q+E>!\n", addr);
    }
  gcc_assert (addr != error_mark_node);

  tree addr_rhs_tree  = gpy_stmt_decl_lower_expr (rhs, cblock, context);

  append_to_statement_list (build2 (MODIFY_EXPR, gpy_object_type_ptr, addr, addr_rhs_tree), cblock);
  append_to_statement_list (gpy_builtin_get_incr_ref_call (addr), cblock);
  retval = addr;
  
  return retval;
}

tree gpy_stmt_decl_lower_binary_op (gpy_dot_tree_t * decl, tree * cblock,
				    VEC(gpy_ctx_t,gc) * context)
{
  tree retval = error_mark_node;

  gcc_assert (DOT_T_FIELD (decl) == D_D_EXPR);

  gpy_dot_tree_t * lhs = DOT_lhs_TT (decl);
  gpy_dot_tree_t * rhs = DOT_rhs_TT (decl);

  tree lhs_eval = gpy_stmt_decl_lower_expr (lhs, cblock, context);
  tree rhs_eval = gpy_stmt_decl_lower_expr (rhs, cblock, context);

  tree op = error_mark_node;
  switch (DOT_TYPE (decl))
    {
    case D_ADD_EXPR:
      op = gpy_builtin_get_eval_expression_call (lhs_eval, rhs_eval, DOT_TYPE (decl));
      break;

      // ....

    default:
      error ("unhandled binary operation type!\n");
      break;
    }
  gcc_assert (op != error_mark_node);

  
  tree retaddr = build_decl (UNKNOWN_LOCATION, VAR_DECL, create_tmp_var_name("T"),
			     gpy_object_type_ptr);
  append_to_statement_list (build2 (MODIFY_EXPR, gpy_object_type_ptr, retaddr, op), cblock);
  retval = retaddr;

  return retval;
}

tree gpy_stmt_decl_lower_expr (gpy_dot_tree_t * decl, tree * cblock,
			       VEC(gpy_ctx_t,gc) * context)
{
  tree retval = error_mark_node;

  switch (DOT_TYPE (decl))
    {
    case D_PRIMITIVE:
      retval = gpy_stmt_decl_lower_scalar (decl, cblock);
      break;

    case D_IDENTIFIER:
      retval = gpy_ctx_lookup_decl (context,
				       DOT_IDENTIFIER_POINTER (decl));
      break;

    default:
      {
	switch (DOT_TYPE (decl))
	  {
	  case D_MODIFY_EXPR:
	    retval = gpy_stmt_decl_lower_modify (decl, cblock, context);
	    break;

	  case D_ADD_EXPR:
	    retval = gpy_stmt_decl_lower_binary_op (decl, cblock, context);
	    break;

	    // ... the rest of the binary operators!

	  default:
	    error ("unhandled operation type!\n");
	    break;
	  }
      }
      break;
    }

  return retval;
}

tree gpy_stmt_pass_lower_functor (gpy_dot_tree_t * decl,
				  gpy_hash_tab_t * modules)
{
  tree block = alloc_stmt_list ();

  gpy_hash_tab_t toplvl, topnxt;
  gpy_dd_hash_init_table (&toplvl);
  gpy_dd_hash_init_table (&topnxt);

  tree main_init_module = gpy_stmt_pass_lower_get_module_type ("main.main",
							       modules);
  tree pdecl_t = build_pointer_type (main_init_module);
  tree fntype = build_function_type_list (void_type_node, pdecl_t,
					  /* 
					     handle function parameters
					     ... 
					   */
					  NULL_TREE);
  tree concat_ident = gpy_stmt_pass_lower_gen_concat_identifier ("main.main",
								 DOT_IDENTIFIER_POINTER (DOT_FIELD (decl)));
  tree fndecl = build_decl (BUILTINS_LOCATION, FUNCTION_DECL, concat_ident, fntype);

  DECL_EXTERNAL (fndecl) = 0;
  TREE_PUBLIC (fndecl) = 1;
  TREE_STATIC (fndecl) = 1;
  tree arglist = NULL_TREE;

  tree result_decl = build_decl (BUILTINS_LOCATION, RESULT_DECL, NULL_TREE,
				 integer_type_node);
  DECL_RESULT (fndecl) = result_decl;

  SET_DECL_ASSEMBLER_NAME (fndecl, concat_ident);

  tree self_parm_decl = build_decl (BUILTINS_LOCATION, PARM_DECL,
				    get_identifier ("__self__"),
				    pdecl_t);

  DECL_CONTEXT (self_parm_decl) = fndecl;
  DECL_ARG_TYPE (self_parm_decl) = TREE_VALUE (TYPE_ARG_TYPES (TREE_TYPE (fndecl)));
  TREE_READONLY (self_parm_decl) = 1;
  arglist = chainon (arglist, self_parm_decl);

  TREE_USED (self_parm_decl) = 1;
  DECL_ARGUMENTS (fndecl) = arglist;

  gpy_stmt_pass_lower_gen_toplevl_context (main_init_module, self_parm_decl,
					   &toplvl);

  VEC(gpy_ctx_t,gc) * toplevl_context = VEC_alloc (gpy_ctx_t, gc, 0);
  VEC_safe_push (gpy_ctx_t, gc, toplevl_context, &toplvl);
  VEC_safe_push (gpy_ctx_t, gc, toplevl_context, &topnxt);
  
  DECL_INITIAL(fndecl) = block;
  gpy_dot_tree_t * idtx = DOT_rhs_TT (decl);
   /*
    Iterating over the DOT IL to lower/generate the GENERIC code
    required to compile the stmts and decls
   */
  do {
    if (DOT_T_FIELD (idtx) ==  D_D_EXPR)
	{
	  // append to stmt list as this goes into the module initilizer...
	  gpy_stmt_decl_lower_expr (idtx, &block, toplevl_context);
	  continue;
	}

      switch (DOT_TYPE (idtx))
	{
	default:
	  fatal_error ("unhandled dot tree code <%i>!\n", DOT_TYPE (idtx));
	  break;
	}
  } while ((idtx = DOT_CHAIN (idtx)));

  tree bl = make_node(BLOCK);
  BLOCK_SUPERCONTEXT(bl) = fndecl;
  DECL_INITIAL(fndecl) = bl;
  BLOCK_VARS(bl) = NULL_TREE;
  TREE_USED(bl) = 1;
  tree bind = build3(BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		     NULL_TREE, bl);
  TREE_SIDE_EFFECTS(bind) = 1;
  
  BIND_EXPR_BODY(bind) = block;
  block = bind;
  DECL_SAVED_TREE(fndecl) = block;

  gimplify_function_tree (fndecl);

  cgraph_add_new_function (fndecl, false);
  cgraph_finalize_function (fndecl, true);

  return fndecl;
}

static
VEC(tree,gc) * gpy_stmt_pass_lower_genericify (gpy_hash_tab_t * modules,
					       VEC(gpydot,gc) * decls)
{
  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  
  tree block = alloc_stmt_list ();
  // tree declvars = NULL_TREE;

  gpy_hash_tab_t toplvl, topnxt;
  gpy_dd_hash_init_table (&toplvl);
  gpy_dd_hash_init_table (&topnxt);

  tree main_init_module = gpy_stmt_pass_lower_get_module_type ("main.main", modules);
  tree pdecl_t = build_pointer_type (main_init_module);

  tree main_init_fntype = build_function_type_list (void_type_node, pdecl_t, NULL_TREE);
  tree main_init_fndecl = build_decl (BUILTINS_LOCATION, FUNCTION_DECL,
				      get_identifier ("main.main.init"),
				      main_init_fntype);
  DECL_EXTERNAL (main_init_fndecl) = 0;
  TREE_PUBLIC (main_init_fndecl) = 1;
  TREE_STATIC (main_init_fndecl) = 1;

  tree result_decl = build_decl (BUILTINS_LOCATION, RESULT_DECL, NULL_TREE,
				 integer_type_node);
  DECL_RESULT (main_init_fndecl) = result_decl;

  tree arglist = NULL_TREE;

  SET_DECL_ASSEMBLER_NAME (main_init_fndecl, get_identifier("main.main.init"));
  tree self_parm_decl = build_decl (BUILTINS_LOCATION, PARM_DECL,
				    get_identifier ("__self__"),
				    pdecl_t);
  DECL_CONTEXT (self_parm_decl) = main_init_fndecl;
  DECL_ARG_TYPE (self_parm_decl) = TREE_VALUE (TYPE_ARG_TYPES (TREE_TYPE (main_init_fndecl)));
  TREE_READONLY (self_parm_decl) = 1;
  arglist = chainon (arglist, self_parm_decl);

  TREE_USED (self_parm_decl) = 1;
  DECL_ARGUMENTS (main_init_fndecl) = arglist;

  gpy_stmt_pass_lower_gen_toplevl_context (main_init_module, self_parm_decl,
					   &toplvl);

  VEC(gpy_ctx_t,gc) * toplevl_context = VEC_alloc (gpy_ctx_t, gc, 0);
  VEC_safe_push (gpy_ctx_t, gc, toplevl_context, &toplvl);
  VEC_safe_push (gpy_ctx_t, gc, toplevl_context, &topnxt);

  DECL_INITIAL(main_init_fndecl) = block;
  
  int idx = 0;
  gpy_dot_tree_t * idtx = NULL_DOT;
  /*
    Iterating over the DOT IL to lower/generate the GENERIC code
    required to compile the stmts and decls
   */
  for (idx = 0; VEC_iterate (gpydot, decls, idx, idtx); ++idx)
    {
      if (DOT_T_FIELD (idtx) ==  D_D_EXPR)
	{
	  // append to stmt list as this goes into the module initilizer...
	  gpy_stmt_decl_lower_expr (idtx, &block, toplevl_context);
	  continue;
	}

      switch (DOT_TYPE (idtx))
	{
	case D_STRUCT_METHOD:
	  {
	    /* 
	       They are self contained decls so we just pass them to the 
	       return vector for gimplification
	    */
	    debug ("lowering function toplevel!\n");
	    VEC_safe_push (tree, gc, retval, 
			   gpy_stmt_pass_lower_functor (idtx, modules)
			   );
	    debug ("lowered function toplevel!\n");
	  }
	  break;

	default:
	  fatal_error ("unhandled dot tree code <%i>!\n", DOT_TYPE (idtx));
	  break;
	}
    }

  tree bl = make_node(BLOCK);
  BLOCK_SUPERCONTEXT(bl) = main_init_fndecl;
  DECL_INITIAL(main_init_fndecl) = bl;
  BLOCK_VARS(bl) = NULL_TREE;
  TREE_USED(bl) = 1;
  tree bind = build3(BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		     NULL_TREE, bl);
  TREE_SIDE_EFFECTS(bind) = 1;
  
  BIND_EXPR_BODY(bind) = block;
  block = bind;
  DECL_SAVED_TREE(main_init_fndecl) = block;

  gimplify_function_tree (main_init_fndecl);

  cgraph_add_new_function (main_init_fndecl, false);
  cgraph_finalize_function (main_init_fndecl, true);

  VEC_safe_push (tree,gc,retval, main_init_fndecl);

  return retval;
}

static
tree gpy_stmt_pass_lower_gen_main (tree module)
{
  tree main_fn_type = build_function_type_list (integer_type_node, NULL_TREE);
  tree main_fn_decl = build_decl (BUILTINS_LOCATION, FUNCTION_DECL,
				  get_identifier("main"), main_fn_type);

  DECL_CONTEXT (main_fn_decl) = NULL_TREE;
  TREE_STATIC (main_fn_decl) = 1;
  TREE_PUBLIC (main_fn_decl) = 1;
  DECL_ARGUMENTS (main_fn_decl) = NULL_TREE;

  /* Define the return type (represented by RESULT_DECL) for the main functin */
  tree main_ret = build_decl (BUILTINS_LOCATION, RESULT_DECL,
			      NULL_TREE, TREE_TYPE(main_fn_type));
  DECL_CONTEXT(main_ret) = main_fn_decl;
  DECL_ARTIFICIAL(main_ret) = 1;
  DECL_IGNORED_P(main_ret) = 1;
  DECL_RESULT(main_fn_decl) = main_ret;

  tree main_art_block = build_block(NULL_TREE, NULL_TREE, NULL_TREE, NULL_TREE);
  DECL_INITIAL(main_fn_decl) = main_art_block;

  tree declare_vars = NULL_TREE;
  tree main_stmts = alloc_stmt_list ();

  append_to_statement_list (gpy_builtin_get_init_call(), &main_stmts);
  tree mod_decl = build_decl (BUILTINS_LOCATION, VAR_DECL, create_tmp_var_name ("I"),
			      module);
  declare_vars = mod_decl;
  tree module_init_fndecl = build_decl (BUILTINS_LOCATION, FUNCTION_DECL,
					get_identifier ("main.main.init"),
					build_function_type_list (void_type_node,
								  build_pointer_type (module), NULL_TREE)
					);
  append_to_statement_list (build_call_expr (module_init_fndecl, 1, build_fold_addr_expr(mod_decl)),
			    &main_stmts);
  append_to_statement_list ( gpy_builtin_get_cleanup_final_call (), &main_stmts);

  tree main_set_ret = build2 (MODIFY_EXPR, TREE_TYPE(main_ret),
			      main_ret, build_int_cst(integer_type_node, 0));
  tree main_ret_expr = build1 (RETURN_EXPR, void_type_node, main_set_ret);
  append_to_statement_list (main_ret_expr, &main_stmts);

  tree bind = NULL_TREE;
  tree bl = make_node(BLOCK);
  BLOCK_SUPERCONTEXT(bl) = main_fn_decl;
  DECL_INITIAL(main_fn_decl) = bl;
  BLOCK_VARS(bl) = declare_vars;
  TREE_USED(bl) = 1;
  bind = build3( BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		 NULL_TREE, bl );
  TREE_SIDE_EFFECTS(bind) = 1;

  BIND_EXPR_BODY(bind) = main_stmts;
  main_stmts = bind;
  DECL_SAVED_TREE(main_fn_decl) = main_stmts;

  gimplify_function_tree (main_fn_decl);
  cgraph_finalize_function (main_fn_decl, false);

  return main_fn_decl;
}

/* Now we start to iterate over the dot IL to lower it down to */
/* a vector of GENERIC decls */

/* Which is then passed over to the pass manager for any other */
/* defined passes which can be placed into the queue but arent */
/* nessecary for now. */
VEC(tree,gc) * gpy_stmt_pass_lower (VEC(tree,gc) *modules,
				    VEC(gpydot,gc) *decls)
{
  VEC(tree,gc) * retval;

  gpy_hash_tab_t module_ctx;
  gpy_dd_hash_init_table (&module_ctx);

  int idx = 0;
  tree itx = NULL_TREE;

  for (; VEC_iterate (tree, modules, idx, itx); ++idx)
    {
      debug ("hashing module name <%s>!\n", IDENTIFIER_POINTER (TYPE_NAME(itx)));
      gpy_hashval_t h = gpy_dd_hash_string (IDENTIFIER_POINTER (TYPE_NAME(itx)));
      void ** e = gpy_dd_hash_insert (h, itx, &module_ctx);

      if (e)
	fatal_error ("module <%q+E> is already defined!\n", itx);
    }

  retval =  gpy_stmt_pass_lower_genericify (&module_ctx, decls);
  VEC_safe_push (tree, gc, retval,
		 gpy_stmt_pass_lower_gen_main (gpy_stmt_pass_lower_get_module_type ("main.main",
										    &module_ctx)
					       )
		 );
  // free (context.array);

  return retval;
}
