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
#include "py-dot-codes.def"
#include "py-dot.h"
#include "py-vec.h"
#include "py-tree.h"
#include "py-types.h"
#include "py-runtime.h"

static VEC(gpy_sym,gc) * gpy_decls;

static gpy_symbol_obj * gpy_stmt_process_AST_Align (gpy_symbol_obj **);

static tree gpy_stmt_pass_1 (VEC(gpy_sym,gc) * const);
static VEC(tree,gc) * gpy_stmt_pass_2 (VEC(gpy_sym,gc) * const, tree);	    

static tree gpy_stmt_create_module_type (VEC(tree,gc) * const, const char *);

static tree gpy_stmt_create_main_fndecl (tree, tree);
static tree gpy_stmt_create_module_initilizer (VEC(tree,gc) *, VEC(tree,gc) *,
					       const char *);

void gpy_stmt_process_decl (gpy_symbol_obj * const sym)
{
  /* Push the declaration! */
  VEC_safe_push( gpy_sym, gc, gpy_decls, sym );
  debug("decl <%p> was pushed!\n", (void*)sym );
}

/**
 * Fairly Confusing Function to read.
 *
 * example:
 *    >>> x = y = z = 2 + 2 + 2;
 *
 *    --- Currently Yacc parses that expression into this Tree:

                      +
                     / \
                    +   2
		   /
                  =
		 / \
		x   =
		   / \
		  y   =
		     / \
		    z   2

  -- Is converted into the procedure:

  1. z = 2 + 2 + 2;
  2. y = z;
  3. x = y;

  -- Tree structure as so:

                 =
                / \
               x   =
                  / \
                 y   =
		    / \
                   z   +
                      / \
                     +   2
                    / \
                   2   2
 **/
static
gpy_symbol_obj * gpy_stmt_process_AST_Align (gpy_symbol_obj ** sym)
{
  gpy_symbol_obj *nn = NULL;
  gpy_symbol_obj *retval = NULL;
  if ((*sym)->next)
    {
      nn = (*sym)->next;
      (*sym)->next = NULL;
    }
  retval = (*sym);

  if( (*sym)->exp == OP_EXPRESS )
    {
      debug("Processing Expression AST!\n");
      if( retval->type != OP_ASSIGN_EVAL )
	{
	  gpy_symbol_obj *o= retval;
	  gpy_symbol_obj *h= NULL;
	  while( o )
	    {
	      if( o->op_a_t == TYPE_SYMBOL )
		{
		  if( o->op_a.symbol_table->type == OP_ASSIGN_EVAL )
		    {
		      h = o;
		      break;
		    }
		  else
		    {
		      o = o->op_a.symbol_table;
		    }
		}
	      else break;
	    }
	  if( h )
	    {
	      gpy_symbol_obj *head = h->op_a.symbol_table;
	      if( head->op_b.symbol_table->type == OP_ASSIGN_EVAL )
		{
		  gpy_symbol_obj *t = head, *m = NULL;
		  while( t )
		    {
		      if( t->op_b_t == TYPE_SYMBOL )
			{
			  if( t->op_b.symbol_table->type != OP_ASSIGN_EVAL )
			    {
			      m = t;
			      break;
			    }
			  else
			    {
			      t = t->op_b.symbol_table;
			    }
			}
		      else break;
		    }
		  if( m )
		    {
		      h->op_a.symbol_table = m->op_b.symbol_table;
		      m->op_b.symbol_table = retval;
		    }
		  else
		    {
		      fatal_error("error processing the expression AST!\n");
		    }
		}
	      else
		{
		  h->op_a.symbol_table = head->op_b.symbol_table;
		  head->op_b.symbol_table = retval;
		}
	      retval = head;
	    }
	}
    }

  if( nn )
    {
      retval->next = nn;
    }
  (*sym) = retval;

  return retval;
}

tree gpy_stmt_process_functor_1 (gpy_symbol_obj * const functor, const char * prefix,
				 VEC(gpy_ctx_t,gc) * context)
{
  gpy_symbol_obj * o = functor->op_a.symbol_table;
  tree fndecl = build_decl (BUILTINS_LOCATION, FUNCTION_DECL,
			    get_identifier (functor->identifier),
			    build_function_type (gpy_object_type_ptr,
						 void_list_node));
  tree declare_vars = NULL_TREE;
  tree bind = NULL_TREE;
  tree block = alloc_stmt_list ();
  tree resdecl = NULL_TREE;
  tree restype = TREE_TYPE (fndecl);

  int idx = 0;

  tree check = gpy_ctx_lookup_decl (context, functor->identifier);
  if (check)
    {
      error("name <%s> is already defined, dynamic re-defintion not implemented yet!\n",
	    functor->identifier);
      fndecl = error_mark_node;
    }
  else
    {
      TREE_PUBLIC(fndecl) = 1;
      TREE_STATIC(fndecl) = 1;

      resdecl = build_decl (BUILTINS_LOCATION, RESULT_DECL, NULL_TREE,
			    restype);
      DECL_CONTEXT(resdecl) = fndecl;
      DECL_RESULT(fndecl) = resdecl;
      DECL_INITIAL(fndecl) = block;

      while (o)
	{
	  /* looping over the gpy_symbol_obj block of function statements
	     and getting the respective tree's and creating the GENERIC block
	  */
	  VEC(tree,gc) * x = gpy_stmt_get_tree_1 (o,context);
	  tree xt;
	  for(idx = 0; VEC_iterate(tree,x,idx,xt); ++idx )
	    {
	      gcc_assert (xt);
	      append_to_statement_list (xt, &block);
	    }
	  o = o->next;
	}
  
      gpy_hash_tab_t * ctx = VEC_index (gpy_ctx_t, context,
					VEC_length (gpy_ctx_t,context)-1);
      int size = ctx->size;
      gpy_hash_entry_t *array= ctx->array;

      for (idx = 0; idx<size; ++idx)
	{
	  if (array[idx].data)
	    {
	      tree x = (tree) array[idx].data;
	      gcc_assert (TREE_CODE(x) == VAR_DECL);
	      debug("got decl <%p>:<%s> within func <%s>!\n", (void*)x,
		    IDENTIFIER_POINTER (DECL_NAME(x)), functor->identifier);
	      TREE_CHAIN( x ) = declare_vars;
	      declare_vars = x;
	    }
	}
 
      tree bl = make_node(BLOCK);
      BLOCK_SUPERCONTEXT(bl) = fndecl;
      DECL_INITIAL(fndecl) = bl;
      BLOCK_VARS(bl) = declare_vars;
      TREE_USED(bl) = 1;
      bind = build3(BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		    NULL_TREE, bl);
      TREE_SIDE_EFFECTS(bind) = 1;
      
      BIND_EXPR_BODY(bind) = block;
      block = bind;
      DECL_SAVED_TREE(fndecl) = block;

      gpy_ctx_t x = VEC_index (gpy_ctx_t, context, 0);
      if (!gpy_ctx_push_decl (fndecl, functor->identifier, x))
	fatal_error("error pushing decl <%s>!\n", functor->identifier );
    }
  
  return fndecl;
}
	    
tree gpy_stmt_process_functor_2 (gpy_symbol_obj * const functor, const char * prefix,
				 tree module, VEC(gpy_ctx_t,gc) * context)
{
  tree fntype = build_function_type_list (gpy_object_type_ptr, build_pointer_type (module),
					  gpy_object_type_ptr_ptr, NULL_TREE);
  tree fndecl = build_decl (functor->loc, FUNCTION_DECL,
			    get_identifier (functor->identifier), fntype);
  DECL_EXTERNAL (fndecl) = 0;
  TREE_PUBLIC (fndecl) = 1;
  TREE_STATIC (fndecl) = 1;

  tree declare_vars = NULL_TREE;
  tree bind = NULL_TREE;
  tree block = alloc_stmt_list ();

  tree result_decl = build_decl (input_location,
				 RESULT_DECL, NULL_TREE, integer_type_node);
  DECL_ARTIFICIAL (result_decl) = 1;
  DECL_IGNORED_P (result_decl) = 1;
  DECL_CONTEXT (result_decl) = fndecl;
  DECL_RESULT (fndecl) = result_decl;

  tree arglist = NULL_TREE;
  tree typelist = TYPE_ARG_TYPES (TREE_TYPE (fndecl));

  tree tmp = TREE_VALUE (typelist);
  tree self = build_decl (functor->loc, PARM_DECL, get_identifier ("self"), tmp);
  DECL_CONTEXT (self) = fndecl;
  DECL_ARG_TYPE (self) = TREE_VALUE (typelist);
  TREE_READONLY (self) = 1;
  arglist = chainon (arglist, self);

  typelist = TREE_CHAIN (typelist);
  tmp = TREE_VALUE (typelist);
  tree __args__ = build_decl (functor->loc, PARM_DECL, get_identifier ("__args__"), tmp);
  DECL_CONTEXT (__args__) = fndecl;
  DECL_ARG_TYPE (__args__) = TREE_VALUE (typelist);
  TREE_READONLY (__args__) = 1;
  arglist = chainon (arglist, __args__);

  TREE_USED (__args__) = 1;
  TREE_USED (self) = 1;

  DECL_ARGUMENTS (fndecl) = arglist;

  gpy_symbol_obj * o = functor->op_a.symbol_table;
  
  int idx;
  while (o)
    {
      /* looping over the gpy_symbol_obj block of function statements
	 and getting the respective tree's and creating the GENERIC block
      */
      VEC(tree,gc) * x = gpy_stmt_get_tree_2 (o, module, context);
      tree xt;
      for(idx = 0; VEC_iterate(tree,x,idx,xt); ++idx )
	{
	  gcc_assert (xt);
	  append_to_statement_list (xt, &block);
	}
      o = o->next;
    }
  
  gpy_hash_tab_t * ctx = VEC_index (gpy_ctx_t, context,
				    VEC_length (gpy_ctx_t,context)-1);
  int size = ctx->size;
  gpy_hash_entry_t *array= ctx->array;
  
  for (idx = 0; idx<size; ++idx)
    {
      if (array[idx].data)
	{
	  tree x = (tree) array[idx].data;
	  gcc_assert (TREE_CODE(x) == VAR_DECL);
	  debug("got decl <%p>:<%s> within func <%s>!\n", (void*)x,
		IDENTIFIER_POINTER (DECL_NAME(x)), functor->identifier);
	  TREE_CHAIN( x ) = declare_vars;
	  declare_vars = x;
	}
    }
  
  tree bl = make_node(BLOCK);
  BLOCK_SUPERCONTEXT(bl) = fndecl;
  DECL_INITIAL(fndecl) = bl;
  BLOCK_VARS(bl) = declare_vars;
  TREE_USED(bl) = 1;
  bind = build3(BIND_EXPR, void_type_node, BLOCK_VARS(bl),
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
	     

VEC(tree,gc) * gpy_stmt_get_tree_1 (gpy_symbol_obj * sym,
				    VEC(gpy_ctx_t,gc) * context)
{
  VEC(tree,gc) * retval = NULL;

  debug ("processing decl of type <0x%X> object <%p>\n",
	 sym->type, (void*) sym);

  if( sym->exp == OP_EXPRESS )
    {
      sym = gpy_stmt_process_AST_Align (&sym);
      retval = gpy_stmt_process_expression (sym, context);
    }
  else
    {
      switch (sym->type)
	{
	case STRUCTURE_FUNCTION_DEF:
	  {
	    gpy_hash_tab_t ctx;
	    gpy_dd_hash_init_table (&ctx);
	    VEC_safe_push (gpy_ctx_t, gc, context, &ctx);
	    
	    retval = VEC_alloc (tree,gc,0);
	    VEC_safe_push (tree,gc,retval,gpy_stmt_process_functor_1 (sym, NULL, context));

	    VEC_pop (gpy_ctx_t, context);
	  }
	  break;

	case KEY_PRINT:
	  {
	    retval = gpy_stmt_process_print (sym, context);
	  }
	  break;
	  
	default:
	  fatal_error("unhandled symbol type <0x%x>\n", sym->type );
	  break;
	}
    }

  return retval;
}

VEC(tree,gc) * gpy_stmt_get_tree_2 (gpy_symbol_obj * sym, tree toplevl,
				    VEC(gpy_ctx_t,gc) * context)
{
  VEC(tree,gc) * retval = NULL;

  debug ("processing decl of type <0x%X> object <%p>\n",
	 sym->type, (void*) sym);

  if( sym->exp == OP_EXPRESS )
    {
      // sym = gpy_stmt_process_AST_Align (&sym);
      retval = gpy_stmt_process_expression (sym, context);
    }
  else
    {
      switch (sym->type)
	{
	case STRUCTURE_FUNCTION_DEF:
	  {
	    gpy_hash_tab_t ctx;
	    gpy_dd_hash_init_table (&ctx);
	    VEC_safe_push (gpy_ctx_t, gc, context, &ctx);

	    retval = VEC_alloc (tree,gc,0);
	    VEC_safe_push (tree,gc,retval,gpy_stmt_process_functor_2 (sym,
								      IDENTIFIER_POINTER(DECL_NAME(toplevl)),
								      toplevl, context));
	    VEC_pop (gpy_ctx_t, context);
	  }
	  break;

	case KEY_PRINT:
	  {
	    retval = gpy_stmt_process_print (sym, context);
	  }
	  break;
	  
	default:
	  fatal_error("unhandled symbol type <0x%x>\n", sym->type );
	  break;
	}
    }

  return retval;
}

tree gpy_stmt_create_main_fndecl (tree module, tree init_fndecl)
{
  return NULL;
}

tree gpy_stmt_create_module_initilizer (VEC(tree,gc) * stmts,
					VEC(tree,gc) * var_decls,
					const char * prefix)
{
  return NULL;
}

tree gpy_stmt_create_module_type (VEC(tree,gc) * const fields,
				  const char * ident)
{
  tree module = make_node (RECORD_TYPE);
  int idx = 0;
  tree itx = NULL_TREE, field = NULL_TREE, last_field = NULL_TREE;

  for (idx = 0; VEC_iterate(tree,fields,idx,itx); ++idx)
    {
      gcc_assert (TREE_CODE(itx) == VAR_DECL);
      tree name = DECL_NAME(itx);
      field = build_decl (BUILTINS_LOCATION, FIELD_DECL, name,
			  gpy_object_type_ptr);
      DECL_CONTEXT(field) = module;
      if (idx>0)
	DECL_CHAIN(last_field) = field;
      else
	TYPE_FIELDS(module) = field;
      last_field = field;
    }

  layout_type (module);

  finish_builtin_struct (module, ident, field, NULL_TREE);
  TYPE_NAME (module) = get_identifier (ident);
  
  return module;
}

tree gpy_stmt_pass_1 (VEC(gpy_sym,gc) * const decls)
{
  VEC (gpy_ctx_t, gc) * context = VEC_alloc (gpy_ctx_t,gc,0);
  gpy_hash_tab_t ctx;
  gpy_dd_hash_init_table (&ctx);

  int idx, idy;
  gpy_symbol_obj * it = NULL;
  tree retval = NULL_TREE, itx = NULL_TREE;

  for (idx = 0; VEC_iterate (gpy_sym,decls,idx,it); ++idx)
    {
      VEC(tree,gc) * x = gpy_stmt_get_tree_1 (it,context);
      gcc_assert (x);
    }

  // all we care about is this toplevel context which
  // should now be populated with VAR_DECL's we can
  // create our module out of.

  VEC(tree,gc) * global_var_decls = VEC_alloc (tree,gc,0);
  
  gpy_hash_tab_t * hctx = VEC_index (gpy_ctx_t, context,
				     VEC_length (gpy_ctx_t,context)-1);
  int size = hctx->size;
  gpy_hash_entry_t *array = hctx->array;
  for (idx = 0; idx < size; ++idx)
    {
      if (array[idx].data)
	{
	  tree x = (tree) array[idx].data;
	  gcc_assert (x);
	  if (TREE_CODE (x) == VAR_DECL)
	    {
	      debug("got decl <%p>:<%s>!\n", (void*)x,
		    IDENTIFIER_POINTER (DECL_NAME(x)));
	      VEC_safe_push (tree, gc, global_var_decls, x);
	    }
	}
    }

  retval = gpy_stmt_create_module_type (global_var_decls, "mangled_module_main__");

  return retval;
}

VEC(tree,gc) * gpy_stmt_pass_2 (VEC(gpy_sym,gc) * const decls,
				tree module)
{
  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  if (module == error_mark_node)
    gcc_unreachable ();
  else
    {
      VEC (gpy_ctx_t, gc) * context = VEC_alloc (gpy_ctx_t,gc,0);
      gpy_hash_tab_t ctx;
      gpy_dd_hash_init_table (&ctx);
      
      int idx, idy;
      gpy_symbol_obj * it = NULL;
      tree itx = NULL_TREE;

      VEC(tree,gc) * functors = VEC_alloc (tree,gc,0);
      VEC(tree,gc) * stmts = VEC_alloc (tree,gc,0);

      for (idx = 0; VEC_iterate (gpy_sym,decls,idx,it); ++idx)
	{
	  VEC(tree,gc) * x = gpy_stmt_get_tree_2 (it, module, context);
	  gcc_assert (x);

	  for (idy = 0; VEC_iterate (tree,x,idy,itx); ++idx)
	    {
	      if (TREE_CODE (itx) == FUNCTION_DECL)
		{
		  VEC_safe_push (tree,gc,functors,itx);
		}
	      else
		{
		  VEC_safe_push (tree,gc,stmts,itx);
		}
	    }
	}

      tree mod_init = gpy_stmt_create_module_initilizer (stmts, NULL, "module.init");
      gcc_assert (TREE_CODE (mod_init) == FUNCTION_DECL);
      VEC_safe_push (tree,gc,functors,mod_init);

      VEC_safe_push (tree,gc,retval,
		     gpy_stmt_create_main_fndecl(module,mod_init));
      GPY_VEC_stmts_append (retval,functors);
    }
  return retval;
}

/**
 * Things are quite complicated from here on and will change frequently
 * We need to do a 1st pass over the code to generate our module.

 Example:

 x = 1
 y = 2
 def foo ():
   return x + y
 print foo ()

 we need to generate out RECORD_TYPE with FIELDS x,y then pass again to generate
 the rest of the code as our first pass will let us generate our TYPE so we know
 how to access each variable in each context on the 2nd pass.

 struct main.main {
  gpy_object_t *x , *y;
 }

 gpy_object_t * main.foo (struct main.main * self, gpy_object_t ** __args)
 {
   T.1 = gpy_rr_bin_op (OP_ADDITION, self->x, self->y);
   return T.1;
 }

 void main.init (struct main.main *self)
 {
   self->x = fold_int (1);
   self->y = fold_int (2);

   T.2 = fold_call (&main.foo, 1, self)
   gpy_rr_print_stmt (1, T.2)
 }

 int main (int argc, char *argv[])
 {
   int retval;
   
   init_runtime ();

   struct main.main P;
   main.init (&P);

   cleanup_runtime ();

   retval = 0;
   return retval;
 }

 **/
void gpy_stmt_write_globals (void)
{
  tree main_module = gpy_stmt_pass_1 (gpy_decls);
  VEC(tree,gc) * globals_decls_vec = gpy_stmt_pass_2 (gpy_decls,main_module);
  
  int idx, idy, global_vec_len = VEC_length (tree, globals_decls_vec);
  tree itx = NULL_TREE;
  tree * global_vec = XNEWVEC (tree, global_vec_len);
  FILE *tu_stream = dump_begin (TDI_tu, NULL);

  idy = 0;
  for (idx = 0; VEC_iterate(tree, globals_decls_vec, idx, itx); ++idx)
    {
      debug_tree (itx);

      if (tu_stream)
	dump_node(itx, 0, tu_stream);

      global_vec[idy] = itx;
      idy++;
    }
  if (tu_stream)
    dump_end(TDI_tu, tu_stream);

  debug("Finished processing!\n\n");
  debug("global_vec len = <%i>!\n", global_vec_len);

  wrapup_global_declarations (global_vec, global_vec_len);
  check_global_declarations (global_vec, global_vec_len);
  emit_debug_global_declarations (global_vec, global_vec_len);
  cgraph_finalize_compilation_unit ();

  debug("finished passing to middle-end!\n\n");
}
