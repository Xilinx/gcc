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

static VEC(tree,gc) * gpy_stmt_pass_lower_genericify (gpy_hash_tab_t *, VEC(gpydot,gc) *);

static
VEC(tree,gc) * gpy_stmt_pass_lower_genericify (gpy_hash_tab_t * modules,
					       VEC(gpydot,gc) * decls)
{
  VEC(tree,gc) * retval;

  

  return retval;
}

/* Now we start to iterate over the dot IL to lower it down to */
/* a vector of GENERIC decls */

/* Which is then passed over to the pass manager for any other */
/* defined passes which can be placed into the queue but arent */
/* nessecary for now. */
VEC(tree,gc) * gpy_stmt_pass_lower (VEC(tree,gc) *modules,
				    VEC(gpydot,gc) *decls)
{
  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  gpy_hash_tab_t module_ctx;
  gpy_dd_hash_init_table (&module_ctx);

  int idx = 0;
  gpy_dot_tree_t * idtx = NULL_DOT;
  tree itx = NULL_TREE;

  for (; VEC_iterate (tree, modules, idx, itx); ++idx)
    {
      gcc_assert (TREE_CODE (itx) == TYPE_DECL);

      debug ("hashing module name <%s>!\n", IDENTIFIER_POINTER (DECL_NAME(itx)));
      gpy_hashval_t h = gpy_dd_hash_string (IDENTIFIER_POINTER (DECL_NAME(itx)));
      void ** e = gpy_dd_hash_insert (h, itx, &module_ctx);

      if (e)
	fatal_error ("module <%q+E> is already defined!\n", itx);
    }

  retval =  gpy_stmt_pass_lower_genericify (&module_ctx, decls);
  // free (context.array);

  return retval;
}
