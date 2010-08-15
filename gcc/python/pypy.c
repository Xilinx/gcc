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
#include "opts.h"
#include "tree.h"
#include "tree-iterator.h"
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

#include "gpy.h"
#include "opcodes.def"
#include "symbols.h"
#include "pypy-tree.h"
#include "runtime.h"

static VEC( gpy_sym,gc ) * gpy_decls;
VEC(gpy_ctx_t,gc) * gpy_ctx_table;
VEC(tree,gc) * global_decls;

/*
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

 Convert into seperate operations

           =
          / \
         z   +
            / \
           +   2
          / \
         2   2

->         =
          / \
         y   z

->
           =
          / \
         x   y

*/
static
gpy_symbol_obj * gpy_process_AST_Split_Asigns( gpy_symbol_obj ** sym )
{
  VEC(gpy_sym,gc) * operations = VEC_alloc(gpy_sym,gc,0);
  gpy_symbol_obj *nn = NULL;
  gpy_symbol_obj *retval = NULL;
  gpy_symbol_obj *i = NULL;
  gpy_symbol_obj *it = retval;

  if( (*sym)->next )
    {
      nn = (*sym)->next;
      (*sym)->next = NULL;
    }
  retval = (*sym);

  if( (*sym)->exp == OP_EXPRESS )
    {
      it = retval;
      while( it->type == OP_ASSIGN_EVAL )
	{
	  if( it->op_b.symbol_table->type == OP_ASSIGN_EVAL )
	    {
	      gpy_symbol_obj * op = it;
	      op->op_b.symbol_table =
		gpy_symbol_obj_clone( it->op_b.symbol_table->op_a.symbol_table,
				      false );

	      gcc_assert( op->op_b.symbol_table->type == SYMBOL_REFERENCE );

	      it = it->op_b.symbol_table;
	      VEC_safe_push( gpy_sym, gc, operations, op );
	    }
	  else
	    break;
	}
      retval = it;
    }
  
  if( VEC_length(gpy_sym,operations) > 0 )
    {
      while( (i = VEC_pop(gpy_sym,operations)) )
	{
	  it->next = i;
	  it = i;
	}
    }

  if( nn )
    {
      it->next = nn;
    }
  (*sym) = retval;

  return retval;
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
gpy_symbol_obj * gpy_process_AST_Align( gpy_symbol_obj ** sym )
{
  gpy_symbol_obj *nn = NULL;
  gpy_symbol_obj *retval = NULL;
  if( (*sym)->next )
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

gpy_symbol_obj * gpy_symbol_obj_clone( gpy_symbol_obj * sym, bool cnext )
{
  gpy_symbol_obj * retval = NULL;
  if( sym )
    {
      Gpy_Symbol_Init( retval );
      
      retval->identifier = xstrdup( sym->identifier );
      retval->exp = sym->exp;
      retval->type = sym->type;
      retval->op_a_t = sym->op_a_t;
      retval->op_b_t = sym->op_b_t;

      switch( retval->op_a_t )
	{
	case TYPE_INTEGER:
	  retval->op_a.integer = sym->op_a.integer;
	  break;

	case TYPE_STRING:
	  retval->op_a.string = xstrdup( sym->op_a.string );
	  break;

	case TYPE_SYMBOL:
	  retval->op_a.symbol_table = gpy_symbol_obj_clone( sym->op_a.symbol_table,
							    cnext );
	  break;
	  
	default:
	  fatal_error("unhandled symbol clone operand A <0x%x>!\n",
		      retval->op_a_t );
	  break;
	}

      switch( retval->op_b_t )
	{
	  case TYPE_INTEGER:
	  retval->op_b.integer = sym->op_b.integer;
	  break;

	case TYPE_STRING:
	  retval->op_b.string = xstrdup( sym->op_b.string );
	  break;

	case TYPE_SYMBOL:
	  retval->op_b.symbol_table = gpy_symbol_obj_clone( sym->op_b.symbol_table,
							    cnext );
	  break;
	  
	default:
	  fatal_error("unhandled symbol clone operand B <0x%x>!\n",
		      retval->op_b_t );
	  break;
	}

      if( cnext )
	retval->next = gpy_symbol_obj_clone( sym->next, cnext );
    }

  return retval;
}

void gpy_process_decl( gpy_symbol_obj * sym )
{
  /* Push the declaration! */
  VEC_safe_push( gpy_sym, gc, gpy_decls, sym );
  debug("decl <%p> was pushed!\n", (void*)sym );
}

VEC(tree,gc) * gpy_process_expression( const gpy_symbol_obj * const sym )
{
  VEC(tree,gc) * retval = NULL;
  if( sym->type == SYMBOL_PRIMARY )
    {
      retval = VEC_alloc(tree,gc,0);
      VEC(tree,gc) *primitive = gpy_fold_primitive( sym );
      gcc_assert( VEC_length(tree,primitive) == 1 );
      VEC_safe_push( tree,gc,retval,VEC_index(tree,primitive,0) );
    }
  else if( sym->type == SYMBOL_REFERENCE )
    {
      gcc_assert( sym->op_a_t == TYPE_STRING );
      tree decl = gpy_ctx_lookup_decl( sym->op_a.string, VAR );
      if( decl )
	{
	  retval = VEC_alloc(tree,gc,0);
	  debug("tree reference <%s>!\n", sym->op_a.string);
	  VEC_safe_push( tree,gc,retval,decl );
	}
      else
	{
	  error("undeclared symbol reference <%s>!\n",
		sym->op_a.string );
	}
    }
  else
    {
      gpy_symbol_obj *opa= NULL, *opb= NULL; VEC(tree,gc) *res = NULL;
      debug("expression evalution <0x%x>!\n", sym->type );

      opa= sym->op_a.symbol_table;
      opb= sym->op_b.symbol_table;

      debug( "opa->type = <0x%x>, opb->type = <0x%x>!\n",
	     opa->type, opb->type );

      switch( sym->type )
	{
	case OP_ASSIGN_EVAL:
	  res = gpy_process_assign( &opa, &opb );
	  break;

	case OP_BIN_ADDITION:
	  res = gpy_process_bin_expression( &opa, &opb, sym->type );
	  break;

	default:
	  fatal_error( "invalid expression evaluation symbol type <0x%x>!\n",
		       sym->type );
	  break;
	}
      if( res ) { retval = res; }
      else { fatal_error("error evaluating expression!\n"); }
    }

  return retval;
}

VEC(tree,gc) * gpy_process_functor( const gpy_symbol_obj * const  functor,
				    const char * base_ident )
{
  VEC(tree,gc) * retval_vec = VEC_alloc( tree,gc,0 );

  gpy_symbol_obj * o = functor->op_a.symbol_table;
  tree fntype = build_function_type(void_type_node, void_list_node);
  tree retval = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
			    get_identifier( functor->identifier ),
			    fntype );

  tree declare_vars = NULL_TREE;
  tree bind = NULL_TREE;
  tree block = alloc_stmt_list( );
  tree resdecl = NULL_TREE;
  tree restype = TREE_TYPE(retval);

  unsigned int idx = 0;
  gpy_context_branch *co = NULL;
  gpy_ident it = NULL;

  if( base_ident )
    {
      size_t len = strlen( functor->identifier ) + strlen( base_ident );
      size_t idx = 0, idy = 0;
      char * buffer = (char *) xmalloc( (sizeof(char) * len)+2 );
      for( ; idx<strlen( base_ident ); ++idx )
	{
	  buffer[ idy ] = base_ident[ idx ];
	  idy++;
	}
      buffer[ idy ] = '.'; idy++;
      for( idx=0; idx<strlen( functor->identifier ); ++idx )
	{
	  buffer[ idy ] = functor->identifier[ idx ];
	  idy++;
	}
      buffer[idy] = '\0';
      debug("buffer = <%s>!\n", buffer );
      SET_DECL_ASSEMBLER_NAME( retval, get_identifier( buffer ) );
      free( buffer );
    }
  else
    SET_DECL_ASSEMBLER_NAME( retval, get_identifier(functor->identifier) );

  TREE_PUBLIC(retval) = 1;
  TREE_STATIC(retval) = 1;

  resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			restype );
  DECL_CONTEXT(resdecl) = retval;
  DECL_RESULT(retval) = resdecl;

  DECL_INITIAL(retval) = block;

  /* push a new context for local symbols */
  co = (gpy_context_branch *)
    xmalloc( sizeof(gpy_context_branch) );
  co->var_decls = NULL;
  co->fnc_decls = NULL;
  gpy_init_ctx_branch( &co );
  VEC_safe_push( gpy_ctx_t, gc, gpy_ctx_table, co );

  while( o )
    {
      /* looping over the gpy_symbol_obj block of function statements
	 and getting the respective tree's and creating the GENERIC block
       */
      VEC(tree,gc) * x = gpy_get_tree( o );
      int itx = 0; tree xt;
      for( ; VEC_iterate(tree,x,itx,xt); ++itx )
	{
	  gcc_assert( xt );
	  append_to_statement_list( xt, &block );
	}
      o = o->next;
    }
  
  for( ; VEC_iterate( gpy_ident,co->var_decl_t, idx, it ); ++idx )
    {
      /* get all block var_decls */
      tree x = gpy_ctx_lookup_decl( it->ident, VAR );
      gcc_assert( TREE_CODE( x ) == VAR_DECL );
      debug("got var decl <%p>:<%s> within func <%s>!\n", (void*)x,
	    it->ident, functor->identifier );
      TREE_CHAIN( x ) = declare_vars;
      declare_vars = x;
    }
 
  if( declare_vars != NULL_TREE )
    {
      tree bl = make_node(BLOCK);
      BLOCK_SUPERCONTEXT(bl) = retval;
      DECL_INITIAL(retval) = bl;
      BLOCK_VARS(bl) = declare_vars;
      TREE_USED(bl) = 1;
      bind = build3(BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		    NULL_TREE, bl);
      TREE_SIDE_EFFECTS(bind) = 1;
    }
  BIND_EXPR_BODY(bind) = block;
  block = bind;
  DECL_SAVED_TREE(retval) = block;
   
  VEC_pop( gpy_ctx_t, gpy_ctx_table );

  gimplify_function_tree( retval );

  cgraph_add_new_function(retval, false);
  cgraph_finalize_function(retval, true);

  VEC_safe_push( tree,gc,retval_vec,retval );
    
  return retval_vec;
}

VEC(tree,gc) * gpy_process_print( gpy_symbol_obj *sym )
{
  VEC(tree,gc) * retval = NULL;
  gcc_assert( sym->op_a_t == TYPE_SYMBOL );
  gpy_symbol_obj * argument_list = sym->op_a.symbol_table;

  if( argument_list )
    {
      int len = 0;
      gpy_symbol_obj * t = argument_list;
      while( t )
	{
	  len++;
	  t=t->next;
	}
     
      int idx = 0; t = argument_list;
      VEC(tree,gc) * args_stmt_vec = VEC_alloc(tree,gc,0);
      for( ; idx<len; ++idx )
	{
	  VEC(tree,gc) * x = gpy_get_tree( t );
	  int itx = 0; tree xt;
	  for( ; VEC_iterate(tree,x,itx,xt); ++itx )
	    {
	      gcc_assert( xt );
	      VEC_safe_push(tree,gc,retval,xt);
	    }
	  t = t->next;
	}

      tree * args = XNEWVEC( tree, VEC_length(tree,args_stmt_vec) );
      tree it = NULL_TREE; int idy = 0;
      for( idx=0; VEC_iterate(tree,args_stmt_vec,idx,it); ++idx )
	{
	  args[ idy ] = it;
	  idy++;
	}
      
      VEC_safe_push( tree,gc,retval,gpy_builtin_get_print_call( len, args ) );
    }
  else
    error("print call without any arguments!\n");

  return retval;
}

VEC(tree,gc) * gpy_process_class( gpy_symbol_obj * const sym )
{
  return NULL;
}

VEC(tree,gc) * gpy_get_tree( gpy_symbol_obj * sym )
{
  VEC(tree,gc) * retval = NULL;
  
  debug( "processing decl of type <0x%X> object <%p>\n",
	 sym->type, (void*) sym );

  if( sym->exp == OP_EXPRESS )
    {
      sym = gpy_process_AST_Align( &sym );
      /*sym = gpy_process_AST_Split_Asigns( &sym );*/
      retval = gpy_process_expression( sym );
    }
  else
    {
      switch( sym->type )
	{
	case STRUCTURE_OBJECT_DEF:
	  retval = gpy_process_class( sym );
	  break;
	  
	case STRUCTURE_FUNCTION_DEF:
	  retval = gpy_process_functor( sym, NULL );
	  break;

	case KEY_PRINT:
	  retval = gpy_process_print( sym );
	  break;
	  
	default:
	  fatal_error("unhandled symbol type <0x%x>\n", sym->type );
	  break;
	}
    }

  return retval;
}

tree gpy_main_method_decl( VEC(tree,gc) * block, gpy_context_branch * co )
{
  tree retval = NULL_TREE; int idx;

  tree main_fn_type = build_function_type_list( integer_type_node, NULL_TREE );
  tree main_fn_decl = build_decl( BUILTINS_LOCATION, FUNCTION_DECL,
				  get_identifier("main"), main_fn_type );

  DECL_CONTEXT(main_fn_decl) = NULL_TREE;
  TREE_STATIC(main_fn_decl) = true;
  TREE_PUBLIC(main_fn_decl) = true;
  DECL_ARGUMENTS(main_fn_decl) = NULL_TREE;

  /* Define the return type (represented by RESULT_DECL) for the main functin */
  tree main_ret = build_decl( BUILTINS_LOCATION, RESULT_DECL,
			      NULL_TREE, TREE_TYPE(main_fn_type) );
  DECL_CONTEXT(main_ret) = main_fn_decl;
  DECL_ARTIFICIAL(main_ret) = true;
  DECL_IGNORED_P(main_ret) = true;
  
  DECL_RESULT(main_fn_decl) = main_ret;

  tree main_art_block = build_block(NULL_TREE, NULL_TREE, NULL_TREE, NULL_TREE);
  DECL_INITIAL(main_fn_decl) = main_art_block;

  tree declare_vars = NULL_TREE;
  tree main_stmts = alloc_stmt_list( );

  append_to_statement_list( gpy_builtin_get_init_call( ), &main_stmts );

  tree it = NULL_TREE;
  for( idx = 0; VEC_iterate(tree,block,idx,it); ++idx )
    {
      gcc_assert( it );
      append_to_statement_list( it, &main_stmts );
    }

  int block_decl_len = 0; gpy_ident vit = NULL;
  for( idx = 0; VEC_iterate( gpy_ident,co->var_decl_t, idx, vit ); ++idx )
    {
      /* get all block var_decls */
      tree x = gpy_ctx_lookup_decl( vit->ident, VAR );
      gcc_assert( TREE_CODE( x ) == VAR_DECL );
      debug("got var decl <%p>:<%s> within func <%s>!\n", (void*)x,
	    vit->ident, "main" );
      TREE_CHAIN( x ) = declare_vars;
      declare_vars = x;
      block_decl_len++;
    }

  if( block_decl_len > 0 )
    {
      tree * block_decl_vec = XNEWVEC( tree, block_decl_len );
      int idy = 0;

      for( idx = 0; VEC_iterate( gpy_ident,co->var_decl_t, idx, vit ); ++idx )
	{
	  tree x = gpy_ctx_lookup_decl( vit->ident, VAR );
	  gcc_assert( TREE_CODE( x ) == VAR_DECL );

	  block_decl_vec[ idy ] = x;
	  idy++;
	}
      // block_decl_vec[ idy ] = main_ret; idy++;

      append_to_statement_list( gpy_builtin_get_finalize_block_call( block_decl_len,
								     block_decl_vec ),
				&main_stmts );
    }
  else
    {
      declare_vars = main_ret;
    }


  append_to_statement_list( gpy_builtin_get_cleanup_final_call( ), &main_stmts );

  tree main_set_ret = build2( MODIFY_EXPR, TREE_TYPE(main_ret),
			      main_ret, build_int_cst(integer_type_node, 0));
  tree main_ret_expr = build1( RETURN_EXPR, void_type_node, main_set_ret );
  append_to_statement_list( main_ret_expr, &main_stmts );

  tree bind = NULL_TREE;
  if( declare_vars != NULL_TREE )
    {
      tree bl = make_node(BLOCK);
      BLOCK_SUPERCONTEXT(bl) = main_fn_decl;
      DECL_INITIAL(main_fn_decl) = bl;
      BLOCK_VARS(bl) = declare_vars;
      TREE_USED(bl) = 1;
      bind = build3( BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		     NULL_TREE, bl );
      TREE_SIDE_EFFECTS(bind) = 1;
    }
  BIND_EXPR_BODY(bind) = main_stmts;
  main_stmts = bind;
  DECL_SAVED_TREE(main_fn_decl) = main_stmts;

  gimplify_function_tree( main_fn_decl );
  cgraph_finalize_function( main_fn_decl, false );

  retval = main_fn_decl;

  return retval;
}

void gpy_write_globals( void )
{
  gpy_context_branch *co = NULL;
  gpy_symbol_obj * it = NULL;
  VEC(tree,gc) * main_stmts_vec = VEC_alloc(tree,gc,0);

   /* push a new context for local symbols */
  co = (gpy_context_branch *)
    xmalloc( sizeof(gpy_context_branch) );
  gpy_init_ctx_branch( &co );
  VEC_safe_push( gpy_ctx_t, gc, gpy_ctx_table, co );

  int idx;
  for( idx= 0; VEC_iterate(gpy_sym,gpy_decls,idx,it); ++idx )
    {
      VEC(tree,gc) * x = gpy_get_tree( it );

      int itx = 0; tree xt;
      for( ; VEC_iterate(tree,x,itx,xt); ++itx )
	{
	  if( TREE_CODE(xt) == FUNCTION_DECL )
	    {
	      VEC_safe_push( tree,gc,global_decls,xt );
	    }
	  else
	    {
	      VEC_safe_push( tree,gc,main_stmts_vec,xt );
	    }
	}
    }

  /* Add in the main method decl! */
  VEC_safe_push( tree,gc,global_decls,gpy_main_method_decl( main_stmts_vec,co ) );

  VEC_pop( gpy_ctx_t, gpy_ctx_table );

  tree itx = NULL_TREE; int idy = 0; int global_vec_len = VEC_length(tree, global_decls);
  tree * global_vec = XNEWVEC( tree, global_vec_len );
  for( idx=0; VEC_iterate(tree,global_decls,idx,itx); ++idx )
    {
      global_vec[ idy ] = itx;
      idy++;
    }

  debug("Finished processing!\n\n");

  wrapup_global_declarations( global_vec, global_vec_len );

  check_global_declarations( global_vec, global_vec_len );
  emit_debug_global_declarations( global_vec, global_vec_len );

  cgraph_finalize_compilation_unit( );

  debug("finished passing to middle-end!\n\n");
}
