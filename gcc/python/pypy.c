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

#include "vec.h"

#include "gpy.h"
#include "symbols.h"
#include "opcodes.def"
#include "y.py.h"

#include <gmp.h>
#include <mpfr.h>

typedef gpy_symbol_obj *gpy_sym;

DEF_VEC_P( gpy_sym );
DEF_VEC_ALLOC_P( gpy_sym,gc );
static VEC( gpy_sym,gc ) * gpy_decls;

inline
void gpy_symbol_init_ctx( gpy_symbol_obj * const x )
{
  x->identifier= NULL;
  x->exp = TYPE_SYMBOL_NIL;
  x->type= SYMBOL_PRIMARY;
  x->op_a_t= TYPE_SYMBOL_NIL;
  x->op_b_t= TYPE_SYMBOL_NIL;
  x->op_a.symbol_table= NULL;
  x->op_b.symbol_table= NULL;
  x->next= NULL;
}

/**
 * Fairly Confusing Function to read.
 *
 * Will convert ANY Expression AST into a kind of SSA (Single-Assignment Form)!
 * But preserves any syntax tree except with the assignment operator! @see below
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
gpy_symbol_obj * gpy_process_AST( gpy_symbol_obj **sym )
{
  gpy_symbol_obj *nn = NULL, *retval;
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
		      error("error processing the expression AST!\n");
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

void gpy_process_decl( gpy_symbol_obj * sym )
{
  debug( "processing declaration <%p> of type: '0x%X'\n",
	 (void*)sym, sym->type );

  if( sym->exp == OP_EXPRESS )
    {
      sym = gpy_process_AST( &sym );
    }
  /* Push the declaration! */
  VEC_safe_push( gpy_sym, gc, gpy_decls, sym );
  debug("decl <%p> was pushed!\n", (void*)sym );
}

void gpy_write_globals( void )
{
  /*
    tree globals, decl, *vec;
    int len, i;
    
    This lang hook is dual-purposed, and also finalizes the
    compilation unit.  
    cgraph_finalize_compilation_unit ();
    
    Really define vars that have had only a tentative definition.
    Really output inline functions that must actually be callable
    and have not been output so far.  
    
    globals = lang_hooks.decls.getdecls ();
    len = list_length (globals);
    vec = XNEWVEC (tree, len);
    
    Process the decls in reverse order--earliest first.
    Put them into VEC from back to front, then take out from front.  
    
    for (i = 0, decl = globals; i < len; i++, decl = TREE_CHAIN (decl))
    vec[len - i - 1] = decl;
    
    wrapup_global_declarations (vec, len);
    check_global_declarations (vec, len);
    emit_debug_global_declarations (vec, len);
    
    Clean up.  
    free (vec);
  */

  unsigned decl_len = VEC_length( gpy_sym, gpy_decls );
  unsigned idx = 0; gpy_symbol_obj *it = NULL;

  debug("decl_len <%u>!\n", decl_len);
  for( ; VEC_iterate(gpy_sym,gpy_decls,idx,it); ++idx )
    {
      debug("decl <%p>!\n", (void*)it );
    }

  cgraph_finalize_compilation_unit( );
}
