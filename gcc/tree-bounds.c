/* Bounds checking.
   Copyright (C) 2007 Free Software Foundation, Inc.
   Contributed by Alexander Lamaison <awl03@doc.ic.ac.uk>
   based on Mudflap.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "target.h"

#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-bounds.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "expr.h"
#include "optabs.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "varray.h"
#include "lambda.h"
#include "diagnostic.h"
#include <demangle.h>
#include "langhooks.h"
#include "toplev.h"

/* Build a reference to a literal string.  */

static tree
bounds_build_string (const char *string)
{
  size_t len = strlen (string);
  tree result = bounds_mark_seen (build_string (len + 1, string));

  TREE_TYPE (result) = build_array_type
    (char_type_node, build_index_type (build_int_cst (NULL_TREE, len)));
  TREE_CONSTANT (result) = 1;
  TREE_READONLY (result) = 1;
  TREE_STATIC (result) = 1;

  result = build1 (ADDR_EXPR, build_pointer_type (char_type_node), result);

  return bounds_mark_seen (result);
}

/* Build a pretty-printed identifier string: file:line (function)
   identifier.  */

static tree
bounds_varname_tree (tree decl)
{
  static pretty_printer buf_rec;
  static int initialized = 0;
  pretty_printer *buf = & buf_rec;
  const char *buf_contents;
  tree result;

  gcc_assert (decl);

  if (!initialized)
    {
      pp_construct (buf, /* prefix */ NULL, /* line-width */ 0);
      initialized = 1;
    }
  pp_clear_output_area (buf);

  /* Add FILENAME[:LINENUMBER[:COLUMNNUMBER]].  */
  {
    expanded_location xloc = expand_location (DECL_SOURCE_LOCATION (decl));
    const char *sourcefile;
    unsigned sourceline = xloc.line;
    unsigned sourcecolumn = 0;
    sourcefile = xloc.file;
    if (sourcefile == NULL && current_function_decl != NULL_TREE)
      sourcefile = DECL_SOURCE_FILE (current_function_decl);
    if (sourcefile == NULL)
      sourcefile = "<unknown file>";

    pp_string (buf, sourcefile);

    if (sourceline != 0)
      {
        pp_string (buf, ":");
        pp_decimal_int (buf, sourceline);

        if (sourcecolumn != 0)
          {
            pp_string (buf, ":");
            pp_decimal_int (buf, sourcecolumn);
          }
      }
  }

  if (current_function_decl != NULL_TREE)
    {
      /* Add (FUNCTION).  */
      pp_string (buf, " (");
      {
        const char *funcname = NULL;
        if (DECL_NAME (current_function_decl))
          funcname = lang_hooks.decl_printable_name (current_function_decl, 1);
        if (funcname == NULL)
          funcname = "anonymous fn";

        pp_string (buf, funcname);
      }
      pp_string (buf, ") ");
    }
  else
    pp_string (buf, " ");

  /* Add <variable-declaration>, possibly demangled.  */
  {
    const char *declname = NULL;

    if (DECL_NAME (decl) != NULL)
      {
	if (strcmp ("GNU C++", lang_hooks.name) == 0)
	  {
	    /* The gcc/cp decl_printable_name hook doesn't do as good a job as
	       the libiberty demangler.  */
	    declname = cplus_demangle (IDENTIFIER_POINTER (DECL_NAME (decl)),
				       DMGL_AUTO | DMGL_VERBOSE);
	  }
	if (declname == NULL)
	  declname = lang_hooks.decl_printable_name (decl, 3);
      }
    if (declname == NULL)
      declname = "<unnamed variable>";

    pp_string (buf, declname);
  }

  /* Return the lot as a new STRING_CST.  */
  buf_contents = pp_base_formatted_text (buf);
  result = bounds_build_string (buf_contents);
  pp_clear_output_area (buf);

  return result;
}

/* And another friend, for producing a simpler message.  */

static tree
bounds_file_function_line_tree (location_t location)
{
  expanded_location xloc = expand_location (location);
  const char *file = NULL, *colon, *line, *op, *name, *cp;
  char linecolbuf[30]; /* Enough for two decimal numbers plus a colon.  */
  char *string;
  tree result;

  /* Add FILENAME[:LINENUMBER[:COLUMNNUMBER]].  */
  file = xloc.file;
  if (file == NULL && current_function_decl != NULL_TREE)
    file = DECL_SOURCE_FILE (current_function_decl);
  if (file == NULL)
    file = "<unknown file>";

  if (xloc.line > 0)
    {
      sprintf (linecolbuf, "%d", xloc.line);
      colon = ":";
      line = linecolbuf;
    }
  else
    colon = line = "";

  /* Add (FUNCTION).  */
  name = lang_hooks.decl_printable_name (current_function_decl, 1);
  if (name)
    {
      op = " (";
      cp = ")";
    }
  else
    op = name = cp = "";

  string = concat (file, colon, line, op, name, cp, NULL);
  result = bounds_build_string (string);
  free (string);

  return result;
}

/* ------------------------------------------------------------------------ */
/* Global tree objects for global variables and functions exported by
   mudflap runtime library.  bounds_init must be called
   before using these.  */

/* uintptr_t (usually "unsigned long") */
static GTY (()) tree bounds_uintptr_type;

/* extern void __bounds_check (void *ptr, size_t sz, int type, const char *); */
static GTY (()) tree bounds_check_fndecl;

/* extern void *__bounds_arith (void *base, void *addr, const char *); */
static GTY (()) tree bounds_arith_fndecl;

/* extern int __bounds_comp (void *left, void *right, int op_type, const char *); */
static GTY (()) tree bounds_comp_fndecl;

/* extern int __bounds_cast (void *ptr, const char *); */
static GTY (()) tree bounds_cast_fndecl;

/* extern void __bounds_register (void *ptr, size_t sz, int type, const char *); */
static GTY (()) tree bounds_register_fndecl;

/* extern void __bounds_unregister (void *ptr, size_t sz, int type); */
static GTY (()) tree bounds_unregister_fndecl;

/* extern void __bounds_init (); */
static GTY (()) tree bounds_init_fndecl;

/* extern void __bounds_create_oob (); */
static GTY (()) tree bounds_create_oob_fndecl;

#define build_function_type_0(rtype) \
  build_function_type (rtype, void_list_node)

#define build_function_type_1(rtype, arg1) \
  build_function_type (rtype, tree_cons (0, arg1, void_list_node))

#define build_function_type_2(rtype, arg1, arg2) \
  build_function_type (rtype, tree_cons (0, arg1, \
                              tree_cons (0, arg2, void_list_node)))

#define build_function_type_3(rtype, arg1, arg2, arg3) \
  build_function_type (rtype, tree_cons (0, arg1, \
                              tree_cons (0, arg2, \
                              tree_cons (0, arg3, void_list_node))))

#define build_function_type_4(rtype, arg1, arg2, arg3, arg4) \
  build_function_type (rtype, tree_cons (0, arg1, \
                              tree_cons (0, arg2, \
                              tree_cons (0, arg3, \
                              tree_cons (0, arg4, void_list_node)))))

#define build_function_type_5(rtype, arg1, arg2, arg3, arg4, arg5) \
  build_function_type (rtype, tree_cons (0, arg1, \
                              tree_cons (0, arg2, \
                              tree_cons (0, arg3, \
                              tree_cons (0, arg4, \
                              tree_cons (0, arg5, void_list_node))))))

/* Helper for bounds_init: construct a decl with the given category,
   name, and type, mark it an external reference, and pushdecl it.  */

static inline tree
bounds_make_builtin (enum tree_code category, const char *name, tree type)
{
  tree decl = bounds_mark_seen (build_decl (category, get_identifier (name), type));
  TREE_PUBLIC (decl) = 1;
  DECL_EXTERNAL (decl) = 1;
  lang_hooks.decls.pushdecl (decl);
  return decl;
}

/* Initialize the global tree nodes that correspond to bounds-runtime.h
   declarations.  */

void
bounds_init (void)
{
  static bool done = false;
  tree bounds_const_string_type;
  tree bounds_register_fntype;
  tree bounds_check_fntype;
  tree bounds_arith_fntype;
  tree bounds_comp_fntype;
  tree bounds_cast_fntype;
  tree bounds_unregister_fntype;
  tree bounds_init_fntype;
  tree bounds_create_oob_fntype;

  if (done)
    return;
  done = true;

  bounds_uintptr_type = lang_hooks.types.type_for_mode (ptr_mode, /*unsignedp=*/true);

  bounds_const_string_type =
    build_pointer_type (build_qualified_type (char_type_node, TYPE_QUAL_CONST));

  bounds_register_fntype =
    build_function_type_4 (void_type_node, ptr_type_node, size_type_node,
                           integer_type_node, bounds_const_string_type);
  bounds_check_fntype =
    build_function_type_5 (void_type_node, ptr_type_node, ptr_type_node, size_type_node,
                           integer_type_node, bounds_const_string_type);
  bounds_arith_fntype =
    build_function_type_3 (ptr_type_node, 
                           ptr_type_node, ptr_type_node, bounds_const_string_type);
  bounds_comp_fntype =
    build_function_type_4 (integer_type_node, 
                           ptr_type_node, ptr_type_node, integer_type_node,
                           bounds_const_string_type);
  bounds_cast_fntype =
    build_function_type_2 (ptr_type_node, ptr_type_node, bounds_const_string_type);

  bounds_unregister_fntype =
    build_function_type_3 (void_type_node, ptr_type_node, size_type_node,
                           integer_type_node);
  bounds_init_fntype =
    build_function_type_0 (void_type_node);
  bounds_create_oob_fntype = 
    build_function_type_3 (ptr_type_node, ptr_type_node, ptr_type_node, 
                           bounds_const_string_type);

  bounds_check_fndecl = 
    bounds_make_builtin (FUNCTION_DECL, "__bounds_check", bounds_check_fntype);
  bounds_arith_fndecl = 
    bounds_make_builtin (FUNCTION_DECL, "__bounds_arith", bounds_arith_fntype);
  bounds_comp_fndecl = 
    bounds_make_builtin (FUNCTION_DECL, "__bounds_comp", bounds_comp_fntype);
  bounds_cast_fndecl = 
    bounds_make_builtin (FUNCTION_DECL, "__bounds_cast", bounds_cast_fntype);
  bounds_register_fndecl = 
    bounds_make_builtin (FUNCTION_DECL, "__bounds_register", bounds_register_fntype);
  bounds_unregister_fndecl = 
    bounds_make_builtin (FUNCTION_DECL, "__bounds_unregister", bounds_unregister_fntype);
  bounds_init_fndecl =
    bounds_make_builtin (FUNCTION_DECL, "__bounds_init", bounds_init_fntype);
  bounds_create_oob_fndecl =
    bounds_make_builtin (FUNCTION_DECL, "__bounds_create_oob", bounds_create_oob_fntype);
}
#undef build_function_type_5
#undef build_function_type_4
#undef build_function_type_3
#undef build_function_type_2
#undef build_function_type_1
#undef build_function_type_0

/*  */

static void
bounds_build_check_statement_for (tree base, tree addr, tree limit,
				  block_stmt_iterator *instr_bsi,
				  location_t *locus, tree dirflag)
{
  tree_stmt_iterator head, tsi;
  block_stmt_iterator bsi;
  tree t, u;
  tree bounds_base;
  tree bounds_addr;
  tree bounds_limit;

  /* Build our local variables.  */
  bounds_base = create_tmp_var (bounds_uintptr_type, "__bounds_base");
  bounds_addr = create_tmp_var (bounds_uintptr_type, "__bounds_addr");
  bounds_limit = create_tmp_var (bounds_uintptr_type, "__bounds_limit");

  /* Build: __bounds_base = (uintptr_t) <base address expression>.  */
  t = build_gimple_modify_stmt (bounds_base,
				fold_convert (bounds_uintptr_type,
					      unshare_expr (base)));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  head = tsi_start (t);
  tsi = tsi_last (t);

  /* Build: __bounds_addr = (uintptr_t) <target address expression>.  */
  t = build_gimple_modify_stmt (bounds_addr,
				fold_convert (bounds_uintptr_type,
					      unshare_expr (addr)));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  tsi_link_after (&tsi, t, TSI_CONTINUE_LINKING);

  /* Build: __bounds_limit = (uintptr_t) <limit address expression>.  */
  t = build_gimple_modify_stmt (bounds_limit,
				fold_convert (bounds_uintptr_type,
					      unshare_expr (limit)));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  tsi_link_after (&tsi, t, TSI_CONTINUE_LINKING);

  /* Insert these statements into the block.  */
  bsi = *instr_bsi;
  tsi = head;
  if (!tsi_end_p (tsi))
    {
      /* Insert first new statement before current point.  */
      bsi_insert_before (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);
	
      /* Insert remaining statements after.  */
      for (tsi_next (&tsi); !tsi_end_p (tsi); tsi_next (&tsi))
	bsi_insert_after (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);
    }

  /*  Now build up the body of the check handling: __bounds_check(); */
  u = tree_cons (NULL_TREE,
                 bounds_file_function_line_tree (locus == NULL ? UNKNOWN_LOCATION
						 : *locus),
                 NULL_TREE);
  u = tree_cons (NULL_TREE, dirflag, u);
  /* NB: we pass the overall [addr..limit] range to bounds_check.  */
  u = tree_cons (NULL_TREE, 
                 fold_build2 (PLUS_EXPR, integer_type_node,
			      fold_build2 (MINUS_EXPR, bounds_uintptr_type, bounds_limit, bounds_addr),
			      integer_one_node),
                 u);
  u = tree_cons (NULL_TREE, bounds_addr, u);
  u = tree_cons (NULL_TREE, bounds_base, u); /* referent object pointer */
  t = build_function_call_expr (bounds_check_fndecl, u);
  gimplify_to_stmt_list (&t);
  head = tsi_start (t);

  /* Insert the check code in the CHECK block.  */
  for (tsi = head; ! tsi_end_p (tsi); tsi_next (&tsi))
    bsi_insert_after (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);

  /* bsi_next (instr_bsi); */
}

/* In order to check that the arithmetic is valid (and if it is not, to
   replace its results with the address of a new OOB object) statements
   of the form:
       p = q + i
   are transformed into the following:
       __bounds_base = q;
       __bounds_addr = i + q;
       p = __bounds_arith (__bounds_base, __bounds_addr, "foo.c:33");
*/

static void
bounds_build_arithmetic_check_for (tree lhs, tree base, tree addr,
				   block_stmt_iterator *instr_bsi,
				   location_t *locus)
{
  tree_stmt_iterator head, tsi;
  block_stmt_iterator bsi;
  tree t, u;

  /* Build our local variables */
  /* If we start doing calculation on these vars here, these should change
   * to uintptr_t 
   * XXX: Maybe we should do this anyway as it would stop us trying to
   * intercept our own arithmetic below and therefore remove the need
   * for bounds_mark_seen */
  tree bounds_base = create_tmp_var (ptr_type_node, "__bounds_base");
  tree bounds_addr = create_tmp_var (ptr_type_node, "__bounds_addr");

  /* Build: __bounds_base = <referent object pointer>.  */
  t = build_gimple_modify_stmt (bounds_base, 
				bounds_mark_seen (unshare_expr (base)));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  head = tsi_start (t);
  tsi = tsi_last (t);

  /* Build: __bounds_addr = <result of arithmetic>.  */
  t = build_gimple_modify_stmt (bounds_addr, 
				bounds_mark_seen (unshare_expr (addr)));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  tsi_link_after (&tsi, t, TSI_CONTINUE_LINKING);

  /* Build up the body of the check handling: 
   * p = __bounds_arith (void* base, void* addr, char* location); */
  u = tree_cons (NULL_TREE,
                 bounds_file_function_line_tree (locus == NULL ? UNKNOWN_LOCATION
						 : *locus), NULL_TREE);
  u = tree_cons (NULL_TREE, bounds_addr, u);
  u = tree_cons (NULL_TREE, bounds_base, u);
  t = build_gimple_modify_stmt (lhs, 
				build_function_call_expr (bounds_arith_fndecl, u));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  tsi_link_after (&tsi, t, TSI_CONTINUE_LINKING);

  /* Replace arithmetic statement with call to bounds_arith function.  */
  bsi = *instr_bsi;
  tsi = head;
  if (!tsi_end_p (tsi))
    {
      /* Insert first new statement as a replacement.  */
      bsi_replace (&bsi, tsi_stmt (tsi), 1);
	
      /* Insert remaining statements after the first.  */
      for (tsi_next (&tsi); !tsi_end_p (tsi); tsi_next (&tsi))
	bsi_insert_after (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);
    }

  /*    bsi_next (instr_bsi); */
}

/* When a comparison is made between pointers they do not have to
   be in-bounds and, as such, may have their real valued stored in 
   an OOB.  In order to perform the comparison correctly, all 
   conditionals must be intercepted and the actual comparison must
   be performed by the libbounds library.  Statements of the form:
       if (p > q) goto <L0>; else goto <L2>; 
   are transformed into the following:
       __bounds_cond = __bounds_comp (p, q, __BOUNDS_COND_GT, "foo.c:33");
       if (__bounds_cond) goto <L0>; else goto <L2>;
*/

static void
bounds_build_conditional_for (tree left, tree right, tree conditional, tree op_type,
                              block_stmt_iterator *instr_bsi,
                              location_t *locus)
{
  tree_stmt_iterator head, tsi;
  block_stmt_iterator bsi;
  tree t, u;

  /* Build our local variables.  */
  tree bounds_cond = create_tmp_var (boolean_type_node, "__bounds_cond");

  /* Build up the body of the check handling: 
     __bounds_cond = __bounds_comp (void* left, void* right, 
                                    int op_type, char* location); */
  u = tree_cons (NULL_TREE,
                 bounds_file_function_line_tree (locus == NULL ? UNKNOWN_LOCATION
						 : *locus), NULL_TREE);
  u = tree_cons (NULL_TREE, bounds_mark_seen (unshare_expr (op_type)), u);
  u = tree_cons (NULL_TREE, bounds_mark_seen (unshare_expr (right)), u);
  u = tree_cons (NULL_TREE, bounds_mark_seen (unshare_expr (left)), u);
  t = build_gimple_modify_stmt (bounds_cond, 
				build_function_call_expr (bounds_comp_fndecl, u));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  head = tsi_start (t);
  tsi = tsi_last (t);

  /* Insert library call before conditional.  */
  bsi = *instr_bsi;
  tsi = head;
  if (!tsi_end_p (tsi))
    {
      /* Insert first new statement before current point.  */
      bsi_insert_before (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);
    
      /* Insert remaining statements after.  */
      for (tsi_next (&tsi); !tsi_end_p (tsi); tsi_next (&tsi))
	bsi_insert_after (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);
    }

  /* Change conditional expression:
   * if (__bounds_cond) then_branch; else_branch; */
  TREE_OPERAND (conditional, 0) = bounds_cond;

  /*    bsi_next (instr_bsi); */
}

/* When the difference of two pointer is taken (i = p - q) the
   pointers are coerced to integers and then subtracted.  As it 
   is possible that one or both of these pointers have their
   real value stored in an OOB, we must intercept coersion (casting)
   of pointers and retrieve their value from the OOB if necessary.
   Statements of the form:
       i = (int) p;
   are transformed into the following:
       __bounds_cast_ptr = __bounds_cast (p, "foo.c:33");
       i = (int) __bounds_cast_ptr;
*/

static void
bounds_build_cast_for (tree ptr, tree cast_expr, block_stmt_iterator *instr_bsi,
                       location_t *locus)
{
  tree_stmt_iterator head, tsi;
  block_stmt_iterator bsi;
  tree t, u;

  /* Build our local variable.  */
  tree bounds_cast_ptr = create_tmp_var (ptr_type_node, "__bounds_cast_ptr");

  /* Build up the body of the interception: 
   * __bounds_cast_ptr = __bounds_cast (void* ptr, char* location); */
  u = tree_cons (NULL_TREE,
                 bounds_file_function_line_tree (locus == NULL ? UNKNOWN_LOCATION
						 : *locus), NULL_TREE);
  u = tree_cons (NULL_TREE, bounds_mark_seen (unshare_expr (ptr)), u);
  t = build_gimple_modify_stmt (bounds_cast_ptr, 
				build_function_call_expr (bounds_cast_fndecl, u));
  SET_EXPR_LOCUS (t, locus);
  gimplify_to_stmt_list (&t);
  head = tsi_start (t);
  tsi = tsi_last (t);

  /* Insert library call before cast.  */
  bsi = *instr_bsi;
  tsi = head;
  if (!tsi_end_p (tsi))
    {
      /* Insert first new statement before current point.  */
      bsi_insert_before (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);
    
      /* Insert remaining statements after.  */
      for (tsi_next (&tsi); !tsi_end_p (tsi); tsi_next (&tsi))
	bsi_insert_after (&bsi, tsi_stmt (tsi), BSI_CONTINUE_LINKING);
    }

  /* Change cast expression:
   * i = (type) __bounds_cast_ptr; */
  TREE_OPERAND (cast_expr, 0) = bounds_cast_ptr;

  /*    bsi_next (instr_bsi); */
}

/* Check whether the given decl, generally a VAR_DECL or PARM_DECL, is
   eligible for instrumentation.  For the mudflap1 pass, this implies
   that it should be registered with the libmudflap runtime.  For the
   mudflap2 pass this means instrumenting an indirection operation with
   respect to the object.
*/

static int
bounds_decl_eligible_p (tree decl)
{
  return (
	  /* Deal with variables and parameters.  */
	  ((TREE_CODE (decl) == VAR_DECL || TREE_CODE (decl) == PARM_DECL)
	   /* The decl must have its address taken.  In the case of
	      arrays, this flag is also set if the indexes are not
	      compile-time known valid constants.  */
	   && TREE_ADDRESSABLE (decl)    /* XXX: not sufficient: return-by-value structs! */
	   /* The type of the variable must be complete.  */
	   && COMPLETE_OR_VOID_TYPE_P (TREE_TYPE (decl))
	   /* The decl hasn't been decomposed somehow.  */
	   && !DECL_HAS_VALUE_EXPR_P (decl))
	  /* Deal with return values.  */
	  || (TREE_CODE (decl) == RESULT_DECL
	      && TREE_ADDRESSABLE (decl)    /* XXX: not sufficient: return-by-value structs! */
	      /* The type of the variable must be complete.  */
	      && COMPLETE_OR_VOID_TYPE_P (TREE_TYPE (decl))));
}


/* ------------------------------------------------------------------------ */

/* This struct is passed between bounds_transform_decls to store state needed
   during the traversal searching for objects that have their
   addresses taken.  */
struct bounds_xform_decls_data
{
  tree param_decls;
};

/* Synthesize a CALL_EXPR and a TRY_FINALLY_EXPR, for this chain of
   _DECLs if appropriate.  Arrange to call the __bounds_register function
   now, and the __bounds_unregister function later for each.  */
static void
mx_register_decls (tree decl, tree *stmt_list)
{
  tree finally_stmts = NULL_TREE;
  tree_stmt_iterator initially_stmts = tsi_start (*stmt_list);

  while (decl != NULL_TREE)
    {
      /* Insert register/unregister calls to try/finally.  */
      if (bounds_decl_eligible_p (decl) 
          /* Not already processed.  */
          && ! bounds_marked_seen_p (decl)
          /* Automatic variable.  */
          && ! DECL_EXTERNAL (decl)
          && ! TREE_STATIC (decl))
        {
	  tree var;
          tree size = NULL_TREE, variable_name;
          tree unregister_fncall, unregister_fncall_params;
          tree register_fncall, register_fncall_params;

          size = fold_convert (size_type_node,
			       TYPE_SIZE_UNIT (TREE_TYPE (decl)));

          /* (& VARIABLE, sizeof (VARIABLE), __MF_TYPE_STACK) */
	  var = bounds_mark_seen (build1 (ADDR_EXPR,
					  build_pointer_type (TREE_TYPE (decl)),
					  decl));
          unregister_fncall_params =
            tree_cons (NULL_TREE,
                       fold_convert (ptr_type_node, var),
                       tree_cons (NULL_TREE, 
                                  size,
                                  tree_cons (NULL_TREE,
					     /* __MF_TYPE_STACK */
                                             build_int_cst (NULL_TREE, 3),
                                             NULL_TREE)));
          /* __bounds_unregister (...) */
          unregister_fncall = build_function_call_expr (bounds_unregister_fndecl,
                                                        unregister_fncall_params);

          /* (& VARIABLE, sizeof (VARIABLE), __MF_TYPE_STACK, "name") */
          variable_name = bounds_varname_tree (decl);
	  var = bounds_mark_seen (build1 (ADDR_EXPR,
					  build_pointer_type (TREE_TYPE (decl)),
					  decl));
          register_fncall_params =
            tree_cons (NULL_TREE,
		       fold_convert (ptr_type_node, var),
                       tree_cons (NULL_TREE,
                                  size,
                                  tree_cons (NULL_TREE,
					     /* __MF_TYPE_STACK */
                                             build_int_cst (NULL_TREE, 3),
                                             tree_cons (NULL_TREE,
                                                        variable_name,
                                                        NULL_TREE))));

          /* __bounds_register (...) */
          register_fncall = build_function_call_expr (bounds_register_fndecl,
                                                      register_fncall_params);

          /* Accumulate the two calls.  */
          /* ??? Set EXPR_LOCATION.  */
          gimplify_stmt (&register_fncall);
          gimplify_stmt (&unregister_fncall);

          /* Add the __mf_register call at the current appending point.  */
          if (tsi_end_p (initially_stmts))
            warning (0, "mudflap cannot track %qs in stub function",
                     DECL_NAME (decl) ? IDENTIFIER_POINTER (DECL_NAME (decl)) : "<unknown>");
          else
	    {
	      tsi_link_before (&initially_stmts, register_fncall, TSI_SAME_STMT);

	      /* Accumulate the FINALLY piece.  */
	      append_to_statement_list (unregister_fncall, &finally_stmts);
	    }
          bounds_mark_seen (decl);
        }

      /* Initialise any pointers with an out-of-bounds pointer object.  */
      if (POINTER_TYPE_P (TREE_TYPE (decl))
          /* Should be an automatic (stack) variable? */
          && ! DECL_EXTERNAL (decl)
          && ! TREE_STATIC (decl)
	  /* Must not be a compiler-generated intermediate.  */
	  /* XXX: && ! DECL_ARTIFICIAL (decl) */
	  /* Must be a variable declaration.  */
          && (TREE_CODE (decl) == VAR_DECL)
	  /* The type of the variable must be complete.  */
	  && COMPLETE_OR_VOID_TYPE_P (TREE_TYPE (decl))
	  /* The decl hasn't been decomposed somehow.  */
	  && !DECL_HAS_VALUE_EXPR_P (decl))
	{
	  tree bounds_create_oob_fncall_params, bounds_create_oob_fncall;
	  tree assign_oob_expr;

	  /* Build function parameters (all are null) */
	  /* (void* ptr, void* old_ptr, const char* location) */
	  bounds_create_oob_fncall_params = 
	    tree_cons (NULL_TREE, null_pointer_node,
                       tree_cons (NULL_TREE, null_pointer_node, 
				  tree_cons (NULL_TREE, null_pointer_node, NULL_TREE)));

	  /* Build function call __bounds_create_oob (...) */
	  bounds_create_oob_fncall =
	    build_function_call_expr (bounds_create_oob_fndecl,
				      bounds_create_oob_fncall_params);

	  assign_oob_expr = 
	    build_gimple_modify_stmt (decl, bounds_create_oob_fncall);

	  gimplify_stmt (&assign_oob_expr);

	  /* Add the call at the current appending point.  */
	  if (tsi_end_p (initially_stmts))
	    warning (0, "mudflap cannot track %qs in stub function",
		     IDENTIFIER_POINTER (DECL_NAME (decl)));
	  else
	    {
	      tsi_link_before (&initially_stmts, assign_oob_expr, TSI_SAME_STMT);

	      /* Accumulate the FINALLY piece.  */
	      /* XXX: FREE? append_to_statement_list (unregister_fncall, &finally_stmts); */
	    }
	}
      decl = TREE_CHAIN (decl);
    }

  /* Actually, (initially_stmts!=NULL) <=> (finally_stmts!=NULL) */
  if (finally_stmts != NULL_TREE)
    {
      tree t = build2 (TRY_FINALLY_EXPR, void_type_node,
		       *stmt_list, finally_stmts);
      *stmt_list = NULL;
      append_to_statement_list (t, stmt_list);
    }
}

/* Process every variable mentioned in BIND_EXPRs.  */
static tree
mx_xfn_xform_decls (tree *t, int *continue_p, void *data)
{
  struct bounds_xform_decls_data* d = (struct bounds_xform_decls_data*) data;

  if (*t == NULL_TREE || *t == error_mark_node)
    {
      *continue_p = 0;
      return NULL_TREE;
    }

  *continue_p = 1;

  switch (TREE_CODE (*t))
    {
    case BIND_EXPR:
      {
        /* Process function parameters and return value now (but only once).  */
        mx_register_decls (d->param_decls, &BIND_EXPR_BODY (*t));
        d->param_decls = NULL_TREE;

        mx_register_decls (BIND_EXPR_VARS (*t), &BIND_EXPR_BODY (*t));
      }
      break;

    default:
      break;
    }

  return NULL_TREE;
}

/* Perform the object lifetime tracking mudflap transform on the given function
   tree.  The tree is mutated in place, with possibly copied subtree nodes.

   For every auto variable declared, if its address is ever taken
   within the function, then supply its lifetime to the mudflap
   runtime with the __mf_register and __mf_unregister calls.
*/
static void
bounds_transform_decls (tree fnbody, tree fnparams)
{
  struct bounds_xform_decls_data d;
  d.param_decls = fnparams;
  walk_tree_without_duplicates (&fnbody, mx_xfn_xform_decls, &d);

}

/* ------------------------------------------------------------------------ */

/* */

static void
bounds_xform_derefs_1 (block_stmt_iterator *iter, tree *tp,
		       location_t *locus, tree dirflag)
{
  tree type, base, limit, addr, size, t;

  /* Don't instrument marked nodes.  */
  if (bounds_marked_seen_p (*tp))
    return;

  t = *tp;
  type = TREE_TYPE (t);
  size = TYPE_SIZE_UNIT (type);

  switch (TREE_CODE (t))
    {
    case ARRAY_REF:
    case COMPONENT_REF:
      {
        /* This is trickier than it may first appear.  The reason is
           that we are looking at expressions from the "inside out" at
           this point.  We may have a complex nested aggregate/array
           expression (e.g. "a.b[i].c"), maybe with an indirection as
           the leftmost operator ("p->a.b.d"), where instrumentation
           is necessary.  Or we may have an innocent "a.b.c"
           expression that must not be instrumented.  We need to
           recurse all the way down the nesting structure to figure it
           out: looking just at the outer node is not enough.  */          
        tree var;
        int component_ref_only = (TREE_CODE (t) == COMPONENT_REF);

        /* If we have a bitfield component reference, we must note the
           innermost addressable object in ELT, from which we will
           construct the byte-addressable bounds of the bitfield.  */
        tree elt = NULL_TREE;
        int bitfield_ref_p = (TREE_CODE (t) == COMPONENT_REF
			      && DECL_BIT_FIELD_TYPE (TREE_OPERAND (t, 1)));

        /* Iterate to the top of the ARRAY_REF/COMPONENT_REF
           containment hierarchy to find the outermost VAR_DECL.  */
        var = TREE_OPERAND (t, 0);
        while (1)
	  {
	    if (bitfield_ref_p && elt == NULL_TREE && 
		(TREE_CODE (var) == ARRAY_REF || TREE_CODE (var) == COMPONENT_REF))
	      elt = var;

	    if (TREE_CODE (var) == ARRAY_REF)
	      {
		component_ref_only = 0;
		var = TREE_OPERAND (var, 0);
	      }
	    else if (TREE_CODE (var) == COMPONENT_REF)
	      var = TREE_OPERAND (var, 0);
	    else if (INDIRECT_REF_P (var))
	      {
		base = TREE_OPERAND (var, 0);
		break;
	      }
	    else 
	      {
		gcc_assert (TREE_CODE (var) == VAR_DECL 
			    || TREE_CODE (var) == PARM_DECL
			    || TREE_CODE (var) == RESULT_DECL
			    || TREE_CODE (var) == STRING_CST);
		/* Don't instrument this access if the underlying
		   variable is not "eligible".  This test matches
		   those arrays that have only known-valid indexes,
		   and thus are not labeled TREE_ADDRESSABLE.  */
		if (! bounds_decl_eligible_p (var) || component_ref_only) 
		  return;
		else
		  {
		    /* base = &t */
		    base = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (var)), var);
		    break;
		  }
	      }
	  }

        /* Handle the case of ordinary non-indirection structure
           accesses.  These have only nested COMPONENT_REF nodes (no
           INDIRECT_REF), but pass through the above filter loop.
           Note that it's possible for such a struct variable to match
           the eligible_p test because someone else might take its
           address sometime.  */

        /* We need special processing for bitfield components, because
           their addresses cannot be taken.  */
        if (bitfield_ref_p)
	  {
	    tree field = TREE_OPERAND (t, 1);

	    if (TREE_CODE (DECL_SIZE_UNIT (field)) == INTEGER_CST) 
	      size = DECL_SIZE_UNIT (field);
          
	    if (elt)
	      elt = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (elt)), elt);
            addr = fold_convert (ptr_type_node, elt ? elt : base);
	    addr = fold_build2 (POINTER_PLUS_EXPR, ptr_type_node, addr,
				fold_convert (ptr_type_node, byte_position (field)));
	  }
        else
          /* addr = &t[n]...[m] */
          addr = build1 (ADDR_EXPR, build_pointer_type (type), t);

	/* limit = (addr + sizeof(type t)) - 1 */
        limit = fold_build2 (MINUS_EXPR, bounds_uintptr_type,
			     fold_build2 (PLUS_EXPR, bounds_uintptr_type,
					  fold_convert (bounds_uintptr_type, addr),
					  size),
			     integer_one_node);
      }
      break;

    case INDIRECT_REF: /* e.g.  *p = 1  or  array_p = 1  */
      /* addr = p */
      addr = TREE_OPERAND (t, 0);
      /* base = p */
      base = addr;
      /* limit = (addr + sizeof(type t)) - 1 */
      limit = fold_build2 (POINTER_PLUS_EXPR, ptr_type_node, addr,
                           fold_build2 (MINUS_EXPR, size_type_node, size,
					integer_one_node));
      break;

    case TARGET_MEM_REF:
      addr = tree_mem_ref_addr (ptr_type_node, t);
      base = addr;
      limit = fold_build2 (POINTER_PLUS_EXPR, ptr_type_node, addr,
                           fold_build2 (MINUS_EXPR, size_type_node, size,
					integer_one_node));
      break;
  
    case ARRAY_RANGE_REF:
      warning (0, "bounds-checking not yet implemented for ARRAY_RANGE_REF");
      return;

    case BIT_FIELD_REF:
      /* ??? merge with COMPONENT_REF code above? */
      {
        tree ofs, rem, bpu;

        /* If we're not dereferencing something, then the access
           must be ok.  */
        if (TREE_CODE (TREE_OPERAND (t, 0)) != INDIRECT_REF)
          return;

        bpu = bitsize_int (BITS_PER_UNIT);
        ofs = fold_convert (bitsizetype, TREE_OPERAND (t, 2));
        rem = size_binop (TRUNC_MOD_EXPR, ofs, bpu);
        ofs = size_binop (TRUNC_DIV_EXPR, ofs, bpu);

        size = fold_convert (bitsizetype, TREE_OPERAND (t, 1));
        size = size_binop (PLUS_EXPR, size, rem);
        size = size_binop (CEIL_DIV_EXPR, size, bpu);
        size = fold_convert (sizetype, size);

        addr = TREE_OPERAND (TREE_OPERAND (t, 0), 0);
        addr = fold_convert (ptr_type_node, addr);
        addr = fold_build2 (POINTER_PLUS_EXPR, ptr_type_node, addr, ofs);

        base = addr;
	limit = fold_build2 (POINTER_PLUS_EXPR, ptr_type_node, base,
			     fold_build2 (MINUS_EXPR, size_type_node, size,
					  integer_one_node));
      }
      break;

    default:
      return;
    }

  bounds_build_check_statement_for (base, addr, limit, iter, locus, dirflag);
}

static void
bounds_xform_arithmetic (block_stmt_iterator *iter, 
			 tree *lhs, tree *rhs, location_t *locus)
{
  tree t, var;
  tree base /* R.O. */, addr;

  /* Don't instrument marked nodes.  */
  if (bounds_marked_seen_p (*rhs))
    return;

  t = *rhs;

  switch (TREE_CODE (t))
    {
    case ADDR_EXPR: /* p = &x */
      if (TREE_CODE (TREE_OPERAND (t, 0)) == ARRAY_REF) /* t = &a[n] */
	{
	  /* base = &a */
	  var = TREE_OPERAND (TREE_OPERAND (t, 0), 0); /* var = a */
	  base = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (var)), var);

	  /* addr = &a[n] */
	  addr = t; 
	}
      else return;
      break;

    case POINTER_PLUS_EXPR: 
      /* p = base + offset */
      base = TREE_OPERAND (t, 0);

      /* addr = base + offset */
      addr = t; 
      break;

    default:
      gcc_unreachable();
    }

  bounds_build_arithmetic_check_for (*lhs, base, addr, iter, locus);
}

static void
bounds_xform_comp (block_stmt_iterator *iter, 
		   tree *conditional, location_t *locus)
{
  tree left, right, op_type;
  tree t;

  /* Don't instrument marked nodes.  */
  if (bounds_marked_seen_p (*conditional)) return;

  t = *conditional;

  /* Build opcode based on type of conditional.  */
  switch (TREE_CODE (TREE_OPERAND (t, 0)))
    {
    case LT_EXPR:
      op_type = build_int_cstu (integer_type_node, 0 /* __BOUNDS_COMP_LT */); break;
    case LE_EXPR:
      op_type = build_int_cstu (integer_type_node, 1 /* __BOUNDS_COMP_LE */); break;
    case GT_EXPR:
      op_type = build_int_cstu (integer_type_node, 2 /* __BOUNDS_COMP_GT */); break;
    case GE_EXPR:
      op_type = build_int_cstu (integer_type_node, 3 /* __BOUNDS_COMP_GE */); break;
    case EQ_EXPR:
      op_type = build_int_cstu (integer_type_node, 4 /* __BOUNDS_COMP_EQ */); break;
    case NE_EXPR:
      op_type = build_int_cstu (integer_type_node, 5 /* __BOUNDS_COMP_NE */); break;
    default:
      gcc_unreachable();
    }

  /* Retrieve trees representing LH and RH pointer operands.  */
  left = TREE_OPERAND (TREE_OPERAND (t, 0), 0);
  right = TREE_OPERAND (TREE_OPERAND (t, 0), 1);
  gcc_assert (POINTER_TYPE_P (TREE_TYPE (left)));
  gcc_assert (POINTER_TYPE_P (TREE_TYPE (right)));
  
  bounds_build_conditional_for (left, right, *conditional, op_type, iter, locus);
}

static void
bounds_xform_cast (block_stmt_iterator *iter, tree *cast_expr, location_t *locus)
{
  tree ptr;

  /* Don't instrument marked nodes.  */
  if (bounds_marked_seen_p (*cast_expr))
    return;

  /* Retrieve the pointer being cast from the CONVERT_EXPR.  */
  ptr = TREE_OPERAND (*cast_expr, 0);
  gcc_assert (POINTER_TYPE_P (TREE_TYPE (ptr)));
  
  bounds_build_cast_for (ptr, *cast_expr, iter, locus);
}

static void
bounds_transform_derefs (void) /* was mf_xform_derefs */
{
  basic_block bb, next;
  block_stmt_iterator i;
  int saved_last_basic_block = last_basic_block;

  bb = ENTRY_BLOCK_PTR ->next_bb;
  do
    {
      next = bb->next_bb;
      for (i = bsi_start (bb); !bsi_end_p (i); bsi_next (&i))
        {
          tree s = bsi_stmt (i);

          /* Only a few GIMPLE statements can reference memory.  */
          switch (TREE_CODE (s))
	    {
            case GIMPLE_MODIFY_STMT:
	      {
		tree lhs = GIMPLE_STMT_OPERAND (s, 0);
		tree rhs = GIMPLE_STMT_OPERAND (s, 1);

		/* Pointer arithmetic:
		 * If LHS is a pointer and RHS is arithmetic, intercept
		 * this and insert code to check that the arithmetic
		 * maintains the same referent object */
		/* Eats:
		 *   PLUS_EXPR(POINTER_TYPE)   p = a + b
		 *   MINUS_EXPR(POINTER_TYPE)  p = a - b
		 *   ADDR_EXPR(ARRAY_REF)      p = &a[n]
		 *   (and all the same as below for the lhs)
		 */

		if (POINTER_TYPE_P (TREE_TYPE (rhs)))
		  {
		    gcc_assert (TREE_CODE (rhs) != MINUS_EXPR
				|| TREE_CODE (rhs) != PLUS_EXPR);
		    if (TREE_CODE (rhs) == POINTER_PLUS_EXPR ||
			TREE_CODE (rhs) == ADDR_EXPR)
		      {
			bounds_xform_derefs_1 (&i, &lhs, 
					       EXPR_LOCUS (s), integer_one_node);
			bounds_xform_arithmetic (&i, &lhs, &rhs, EXPR_LOCUS (s));
		      }
		  }

		/* Pointer casts:
		 * If RHS is cast from a pointer to a non-pointer, intercept
		 * this and insert code to retrieve the actual pointer
		 * as this may be contained in an OOB */
		/* Eats:
		 *   CONVERT_EXPR              i = (int) p
		 *   (and all the same as below for the lhs)
		 */
		if (TREE_CODE (rhs) == CONVERT_EXPR &&
		    POINTER_TYPE_P (TREE_TYPE (TREE_OPERAND (rhs, 0))) &&
		    ! POINTER_TYPE_P (TREE_TYPE (lhs)))
		  {
		    bounds_xform_derefs_1 (&i, &lhs, 
					   EXPR_LOCUS (s), integer_one_node);
		    bounds_xform_cast (&i, &rhs, EXPR_LOCUS (s));
		  }

		/* XXX: There should really be another catch here for ADDR_EXPRs
		 * where the RHS is and array reference (e.g. p = &a[10]). At the moment
		 * this is handled by the clause below but this does not check that
		 * the LHS is a pointer. */

		/* Search remaining statements for dereferences and check
		 * them for validity */
		/* Eats:
		 *   ARRAY_REF        i = a[m].b[n]
		 *   COMPONENT_REF    i = a.b[n].c
		 *   INDIRECT_REF     i = *p
		 *   TARGET_MEM_REF
		 *   ARRAY_RANGE_REF                      (not yet implemented?)
		 *   BIT_FIELD_REF
		 */
		else
		  {
		    bounds_xform_derefs_1 (&i, &lhs, EXPR_LOCUS (s),
					   integer_one_node);
		    bounds_xform_derefs_1 (&i, &rhs, EXPR_LOCUS (s),
					   integer_zero_node);
		  }
		break;
	      }

            case RETURN_EXPR:
	      {
		tree return_expr = TREE_OPERAND (s, 0);

		if (return_expr != NULL_TREE)
		  {
		    if (TREE_CODE (return_expr) == GIMPLE_MODIFY_STMT)
		      bounds_xform_derefs_1 (&i, &GIMPLE_STMT_OPERAND (return_expr, 1),
					     EXPR_LOCUS (s), integer_zero_node);
		    else
		      bounds_xform_derefs_1 (&i, &return_expr, EXPR_LOCUS (s),
					     integer_zero_node);
		  }
		break;
	      }

            case COND_EXPR:
	      {
		tree conditional = TREE_OPERAND (s, 0);

		/* If operands are pointers (XXX: both?) intercept
		 * this and insert code to retrive the real pointer values
		 * in the case that they reference OOBs */
		/* Eats:
		 *   COND_EXPR         if (p > q)
		 * Does NOT eat:
		 *   COND_EXPR         if (p)     XXX: maybe it should
		 */
		if ((TREE_CODE (conditional) == LT_EXPR ||
		     TREE_CODE (conditional) == LE_EXPR ||
		     TREE_CODE (conditional) == GT_EXPR ||
		     TREE_CODE (conditional) == GE_EXPR ||
		     TREE_CODE (conditional) == EQ_EXPR ||
		     TREE_CODE (conditional) == NE_EXPR) && 
		    POINTER_TYPE_P (TREE_TYPE (TREE_OPERAND (conditional, 0))) &&
		    POINTER_TYPE_P (TREE_TYPE (TREE_OPERAND (conditional, 1))))
		  {
		    bounds_xform_comp (&i, &s, EXPR_LOCUS (s));
		  }
		break;
	      }

            default:
              ;
	    }
        }
      bb = next;
    }
  while (bb && bb->index <= saved_last_basic_block);
}

/* ------------------------------------------------------------------------ */
/* Externally visible bounds-checking functions.  */

/* Mark and return the given tree node to prevent further bounds-checking
   transforms.  */
static GTY ((param_is (union tree_node))) htab_t marked_trees = NULL;

tree
bounds_mark_seen (tree t) /* was mf_mark */
{
  void **slot;

  if (marked_trees == NULL)
    marked_trees = htab_create_ggc (31, htab_hash_pointer, htab_eq_pointer, NULL);

  slot = htab_find_slot (marked_trees, t, INSERT);
  *slot = t;
  return t;
}

int
bounds_marked_seen_p (tree t) /* was mf_marked_p */
{
  void *entry;

  if (marked_trees == NULL)
    return 0;

  entry = htab_find (marked_trees, t);
  return (entry != NULL);
}

/* Remember given node as a static of some kind: global data,
   function-scope static, or an anonymous constant.  Its assembler
   label is given.  */

/* A list of globals whose incomplete declarations we encountered.
   Instead of emitting the __bounds_register call for them here, it's
   delayed until program finish time.  If they're still incomplete by
   then, warnings are emitted.  */

static GTY (()) VEC(tree,gc) *deferred_static_decls;

/* A list of statements for calling __bounds_register() at startup time.  */
static GTY (()) tree enqueued_call_stmt_chain;

static void
bounds_register_call (tree obj, tree object_size, tree varname)
{
  tree arg, args, call_stmt;

  args = tree_cons (NULL_TREE, varname, NULL_TREE);

  arg = build_int_cst (NULL_TREE, 4); /* __MF_TYPE_STATIC */
  args = tree_cons (NULL_TREE, arg, args);

  arg = fold_convert (size_type_node, object_size);
  args = tree_cons (NULL_TREE, arg, args);

  arg = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (obj)), obj);
  arg = fold_convert (ptr_type_node, arg);
  args = tree_cons (NULL_TREE, arg, args);

  call_stmt = build_function_call_expr (bounds_register_fndecl, args);

  append_to_statement_list (call_stmt, &enqueued_call_stmt_chain);
}

void
bounds_enqueue_decl (tree obj)
{
  if (bounds_marked_seen_p (obj))
    return;

  /* We don't need to process variable decls that are internally
     generated extern.  If we did, we'd end up with warnings for them
     during bounds_finish_file ().  That would confuse the user,
     since the text would refer to variables that don't show up in the
     user's source code.  */
  if (DECL_P (obj) && DECL_EXTERNAL (obj) && DECL_ARTIFICIAL (obj))
    return;

  VEC_safe_push (tree, gc, deferred_static_decls, obj);
}

void
bounds_enqueue_constant (tree obj)
{
  tree object_size, varname;

  if (bounds_marked_seen_p (obj))
    return;

  if (TREE_CODE (obj) == STRING_CST)
    object_size = build_int_cst (NULL_TREE, TREE_STRING_LENGTH (obj));
  else
    object_size = size_in_bytes (TREE_TYPE (obj));

  if (TREE_CODE (obj) == STRING_CST)
    varname = bounds_build_string ("string literal");
  else
    varname = bounds_build_string ("constant");

  bounds_register_call (obj, object_size, varname);
}


/* Emit any file-wide instrumentation.  */
void
bounds_finish_file (void)
{
  tree ctor_statements = NULL_TREE;

  /* No need to continue when there were errors.  */
  if (errorcount != 0 || sorrycount != 0)
    return;

  /* Insert a call to __bounds_init.  */
  {
    tree call2_stmt = build_function_call_expr (bounds_init_fndecl, NULL_TREE);
    append_to_statement_list (call2_stmt, &ctor_statements);
  }

  /* Process all enqueued object decls.  */
  if (deferred_static_decls)
    {
      size_t i;
      tree obj;
      for (i = 0; VEC_iterate (tree, deferred_static_decls, i, obj); i++)
        {
          gcc_assert (DECL_P (obj));

          if (bounds_marked_seen_p (obj))
            continue;

          /* Omit registration for static unaddressed objects.  NB:
             Perform registration for non-static objects regardless of
             TREE_USED or TREE_ADDRESSABLE, because they may be used
             from other compilation units.  */
          if (! TREE_PUBLIC (obj) && ! TREE_ADDRESSABLE (obj))
            continue;

          if (! COMPLETE_TYPE_P (TREE_TYPE (obj)))
            {
              warning (0, "bounds-checking cannot track unknown size extern %qs",
                       IDENTIFIER_POINTER (DECL_NAME (obj)));
              continue;
            }
          
          bounds_register_call (obj, 
				size_in_bytes (TREE_TYPE (obj)),
				bounds_varname_tree (obj));
        }

      VEC_truncate (tree, deferred_static_decls, 0);
    }

  /* Append all the enqueued registration calls.  */
  if (enqueued_call_stmt_chain)
    {
      append_to_statement_list (enqueued_call_stmt_chain, &ctor_statements);
      enqueued_call_stmt_chain = NULL_TREE;
    }

  cgraph_build_static_cdtor ('I', ctor_statements, 
                             MAX_RESERVED_INIT_PRIORITY-1);
}

/* EARLY PASS: ADDR_EXPR transforms.  Perform the declaration-related 
   mudflap tree transforms on the current function.

   This is the first part of the mudflap instrumentation.  It works on
   high-level GIMPLE because after lowering, all variables are moved out
   of their BIND_EXPR binding context, and we lose liveness information
   for the declarations we wish to instrument.  */

static unsigned int
tree_bounds_early (void) /* was execute_mudflap_function_decls */
{
  /* Don't instrument functions such as the synthetic constructor
     built during mudflap_finish_file.  */
  if (bounds_marked_seen_p (current_function_decl) ||
      DECL_ARTIFICIAL (current_function_decl))
    return 0;

  push_gimplify_context ();

  bounds_transform_decls (DECL_SAVED_TREE (current_function_decl),
                          DECL_ARGUMENTS (current_function_decl));

  pop_gimplify_context (NULL);
  return 0;
}

/* LATE PASS: Memory reference transforms. Perform the mudflap 
   indirection-related tree transforms on the current function.

   This is the second part of the mudflap instrumentation.  It works on
   low-level GIMPLE using the CFG, because we want to run this pass after
   tree optimizations have been performed, but we have to preserve the CFG
   for expansion from trees to RTL.  */

static unsigned int
tree_bounds_late (void) /* was execute_mudflap_function_ops */
{
  /* Don't instrument functions such as the synthetic constructor
     built during mudflap_finish_file.  */
  if (bounds_marked_seen_p (current_function_decl) ||
      DECL_ARTIFICIAL (current_function_decl))
    return 0;

  push_gimplify_context ();

  bounds_transform_derefs ();

  pop_gimplify_context (NULL);
  return 0;
}

static bool
gate_tree_bounds(void)
{
  return flag_bounds !=0;
}

struct gimple_opt_pass pass_bounds_early =
{
  {
    GIMPLE_PASS,
    "bounds-early",                       /* name */
    gate_tree_bounds,                     /* gate */
    tree_bounds_early,                    /* execute */
    NULL,                                 /* sub */
    NULL,                                 /* next */
    0,                                    /* static_pass_number */
    0,                                    /* tv_id */
    PROP_gimple_any,                      /* properties_required */
    0,                                    /* properties_provided */
    0,                                    /* properties_destroyed */
    0,                                    /* todo_flags_start */
    TODO_dump_func                        /* todo_flags_finish */
  }
};

struct gimple_opt_pass pass_bounds_late =
{
  {
    GIMPLE_PASS,
    "bounds-late",                        /* name */
    gate_tree_bounds,                     /* gate */
    tree_bounds_late,                     /* execute */
    NULL,                                 /* sub */
    NULL,                                 /* next */
    0,                                    /* static_pass_number */
    0,                                    /* tv_id */
    PROP_gimple_leh,                      /* properties_required */
    0,                                    /* properties_provided */
    0,                                    /* properties_destroyed */
    0,                                    /* todo_flags_start */
    TODO_verify_flow | TODO_verify_stmts
    | TODO_dump_func                      /* todo_flags_finish */
  }
};

#include "gt-tree-bounds.h"
