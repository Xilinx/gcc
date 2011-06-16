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

static VEC(gpydot,gc) * gpy_decls;

static gpy_symbol_obj * gpy_stmt_process_AST_Align (gpy_dot_tree_t **);

void gpy_stmt_process_decl (gpy_dot_tree_t * const dot)
{
  /* Push the declaration! */
  VEC_safe_push (gpydot, gc, gpy_decls, dot);
  debug ("decl <%p> was pushed!\n", (void*)dot);
}

/**
 * Fairly Confusing Function to read.
 *
 * example:
 *    >>> x = y = z = 2 + 2 + 2;
 *
 *    --- Currently Yacc parses that expression into this Tree, which is just
          side effect of the way we generate the gpy_dot_tree's the much higher level IR
	  which we bring down to GENERIC:

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
gpy_dot_tree_t * gpy_stmt_process_AST_Align (gpy_dot_tree_t ** dot)
{
  gpy_dot_tree_t *retval = NULL_DOT;
  gpy_dot_tree_t *nn = NULL_DOT;
  if (DECL_CHAIN(*dot))
    {
      nn = DECL_CHAIN(*dot);
      DECL_CHAIN(*dot) = NULL_DOT;
    }
  retval = (*dot);

  if (DOT_TYPE(retval) != D_MODIFY_EXPR)
    {
      gpy_dot_tree_t *o = retval;
      gpy_dot_tree_t *h = NULL_DOT;

      while (o != NULL_DOT)
        {
          if ((DOT_TYPE(o) != D_IDENTIFIER) ||
              (DOT_TYPE(o) != D_PRIMITIVE))
            {
              if (DOT_lhs_T(o) == D_TD_DOT)
                {
                  if (DOT_TYPE(DOT_lhs_TT(o)) == D_MODIFY_EXPR)
                    {
                      h = o; 
		      break;
                    }
                  else
                    {
                      o = DOT_lhs_TT(o);
                    }
                }
              else break;
            }
          else break;
        }
      if (h)
        {
          gpy_dot_tree_t  *head = DOT_lhs_TT(h);
          if (DOT_TYPE(DOT_rhs_TT(head)) == D_MODIFY_EXPR)
            {
              gpy_dot_tree_t  *t = head, *m = NULL_DOT;
              while (t)
                {
                  if ((DOT_TYPE(t) != D_IDENTIFIER) ||
                      (DOT_TYPE(t) != D_PRIMITIVE))
                    {
                      if (DOT_TYPE(DOT_rhs_TT(t)) != D_MODIFY_EXPR)
                        {
                          m = t;
                          break;
                        }
                      else
                        {
                          t = t->opb.t;
                        }
                    }
                  else break;
                }
              
              if( m )
                {
                  DOT_lhs_TT(h) = DOT_rhs_TT(m);
                  DOT_rhs_TT(m) = retval;
                }
              else
                fatal_error ("error processing the expression AST!\n");
            }
          else
            {
              DOT_lhs_TT(h) = DOT_rhs_TT(head);
              DOT_rhs_TT(head) = retval;
            }
          retval = head;
        }
    }
  
  if (nn)
    DOT_CHAIN(retval) = nn;
  (*dot) = retval;

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
  VEC(tree,gc) * module_types = gpy_stmt_pass_1 (gpy_decls);
  VEC(tree,gc) * dot2gen_trees = gpy_stmt_pass_2 (module_types, gpy_decls);

  
}
