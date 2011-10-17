/* Some code common to C++ and ObjC++ front ends.
   Copyright (C) 2004, 2007, 2008, 2009, 2010, 2011
   Free Software Foundation, Inc.
   Contributed by Ziemowit Laski  <zlaski@apple.com>

This file is part of GCC.

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
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "cp-tree.h"
#include "c-family/c-common.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "diagnostic.h"
#include "debug.h"
#include "cxx-pretty-print.h"
#include "cp-objcp-common.h"

/* Special routine to get the alias set for C++.  */

alias_set_type
cxx_get_alias_set (tree t)
{
  if (IS_FAKE_BASE_TYPE (t))
    /* The base variant of a type must be in the same alias set as the
       complete type.  */
    return get_alias_set (TYPE_CONTEXT (t));

  /* Punt on PMFs until we canonicalize functions properly.  */
  if (TYPE_PTRMEMFUNC_P (t)
      || (POINTER_TYPE_P (t)
	  && TYPE_PTRMEMFUNC_P (TREE_TYPE (t))))
    return 0;

  return c_common_get_alias_set (t);
}

/* Called from check_global_declarations.  */

bool
cxx_warn_unused_global_decl (const_tree decl)
{
  if (TREE_CODE (decl) == FUNCTION_DECL && DECL_DECLARED_INLINE_P (decl))
    return false;
  if (DECL_IN_SYSTEM_HEADER (decl))
    return false;

  /* Const variables take the place of #defines in C++.  */
  if (TREE_CODE (decl) == VAR_DECL && TREE_READONLY (decl))
    return false;

  return true;
}

/* Langhook for tree_size: determine size of our 'x' and 'c' nodes.  */
size_t
cp_tree_size (enum tree_code code)
{
  switch (code)
    {
    case PTRMEM_CST:		return sizeof (struct ptrmem_cst);
    case BASELINK:		return sizeof (struct tree_baselink);
    case TEMPLATE_PARM_INDEX:	return sizeof (template_parm_index);
    case DEFAULT_ARG:		return sizeof (struct tree_default_arg);
    case DEFERRED_NOEXCEPT:	return sizeof (struct tree_deferred_noexcept);
    case OVERLOAD:		return sizeof (struct tree_overload);
    case STATIC_ASSERT:         return sizeof (struct tree_static_assert);
    case TYPE_ARGUMENT_PACK:
    case TYPE_PACK_EXPANSION:
      return sizeof (struct tree_common);

    case NONTYPE_ARGUMENT_PACK:
    case EXPR_PACK_EXPANSION:
      return sizeof (struct tree_exp);

    case ARGUMENT_PACK_SELECT:
      return sizeof (struct tree_argument_pack_select);

    case TRAIT_EXPR:
      return sizeof (struct tree_trait_expr);

    case LAMBDA_EXPR:           return sizeof (struct tree_lambda_expr);

    case TEMPLATE_INFO:         return sizeof (struct tree_template_info);

    default:
      gcc_unreachable ();
    }
  /* NOTREACHED */
}

/* Returns true if T is a variably modified type, in the sense of C99.
   FN is as passed to variably_modified_p.
   This routine needs only check cases that cannot be handled by the
   language-independent logic in tree.c.  */

bool
cp_var_mod_type_p (tree type, tree fn)
{
  /* If TYPE is a pointer-to-member, it is variably modified if either
     the class or the member are variably modified.  */
  if (TYPE_PTR_TO_MEMBER_P (type))
    return (variably_modified_type_p (TYPE_PTRMEM_CLASS_TYPE (type), fn)
	    || variably_modified_type_p (TYPE_PTRMEM_POINTED_TO_TYPE (type),
					 fn));

  /* All other types are not variably modified.  */
  return false;
}

/* Construct a C++-aware pretty-printer for CONTEXT.  It is assumed
   that CONTEXT->printer is an already constructed basic pretty_printer.  */
void
cxx_initialize_diagnostics (diagnostic_context *context)
{
  pretty_printer *base;
  cxx_pretty_printer *pp;

  c_common_initialize_diagnostics (context);

  base = context->printer;
  pp = XNEW (cxx_pretty_printer);
  memcpy (pp_base (pp), base, sizeof (pretty_printer));
  pp_cxx_pretty_printer_init (pp);
  context->printer = (pretty_printer *) pp;

  /* It is safe to free this object because it was previously malloc()'d.  */
  free (base);
}

/* This compares two types for equivalence ("compatible" in C-based languages).
   This routine should only return 1 if it is sure.  It should not be used
   in contexts where erroneously returning 0 causes problems.  */

int
cxx_types_compatible_p (tree x, tree y)
{
  return same_type_ignoring_top_level_qualifiers_p (x, y);
}

/* Return true if DECL is explicit member function.  */

bool
cp_function_decl_explicit_p (tree decl)
{
  return (decl
	  && FUNCTION_FIRST_USER_PARMTYPE (decl) != void_list_node
	  && DECL_LANG_SPECIFIC (STRIP_TEMPLATE (decl))
	  && DECL_NONCONVERTING_P (decl));
}

/* Stubs to keep c-opts.c happy.  */
void
push_file_scope (void)
{
}

void
pop_file_scope (void)
{
}

/* c-pragma.c needs to query whether a decl has extern "C" linkage.  */
bool
has_c_linkage (const_tree decl)
{
  return DECL_EXTERN_C_P (decl);
}

static GTY ((if_marked ("tree_decl_map_marked_p"), param_is (struct tree_decl_map)))
     htab_t shadowed_var_for_decl;

/* Lookup a shadowed var for FROM, and return it if we find one.  */

tree
decl_shadowed_for_var_lookup (tree from)
{
  struct tree_decl_map *h, in;
  in.base.from = from;

  h = (struct tree_decl_map *)
      htab_find_with_hash (shadowed_var_for_decl, &in, DECL_UID (from));
  if (h)
    return h->to;
  return NULL_TREE;
}

/* Insert a mapping FROM->TO in the shadowed var hashtable.  */

void
decl_shadowed_for_var_insert (tree from, tree to)
{
  struct tree_decl_map *h;
  void **loc;

  h = ggc_alloc_tree_decl_map ();
  h->base.from = from;
  h->to = to;
  loc = htab_find_slot_with_hash (shadowed_var_for_decl, h, DECL_UID (from),
				  INSERT);
  *(struct tree_decl_map **) loc = h;
}

void
init_shadowed_var_for_decl (void)
{
  shadowed_var_for_decl = htab_create_ggc (512, tree_decl_map_hash,
					   tree_decl_map_eq, 0);
}

void
cp_common_init_ts (void)
{
  unsigned i;

  for (i = LAST_AND_UNUSED_TREE_CODE; i < MAX_TREE_CODES; i++)
    {
      enum tree_node_structure_enum ts_code;
      enum tree_code code;

      code = (enum tree_code) i;
      ts_code = tree_node_structure_for_code (code);
      if (ts_code != LAST_TS_ENUM)
	{
	  /* All expressions in C++ are typed.  */
	  if (ts_code == TS_EXP)
	    MARK_TS_TYPED (code);
	  mark_ts_structures_for (code, ts_code);
	}
      else
	{
	  /* tree_node_structure_for_code does not recognize language
	     specific nodes (unless they use standard code classes).  */
	  MARK_TS_COMMON (code);
	}
    }

  /* Consistency checks for codes used in the front end.  */
  gcc_assert (tree_contains_struct[NAMESPACE_DECL][TS_DECL_NON_COMMON]);
  gcc_assert (tree_contains_struct[USING_DECL][TS_DECL_NON_COMMON]);
  gcc_assert (tree_contains_struct[TEMPLATE_DECL][TS_DECL_NON_COMMON]);

  gcc_assert (tree_contains_struct[TEMPLATE_TEMPLATE_PARM][TS_COMMON]);
  gcc_assert (tree_contains_struct[TEMPLATE_TYPE_PARM][TS_COMMON]);
  gcc_assert (tree_contains_struct[TEMPLATE_PARM_INDEX][TS_COMMON]);
  gcc_assert (tree_contains_struct[OVERLOAD][TS_COMMON]);
  gcc_assert (tree_contains_struct[TEMPLATE_INFO][TS_COMMON]);
  gcc_assert (tree_contains_struct[TYPENAME_TYPE][TS_COMMON]);
  gcc_assert (tree_contains_struct[TYPEOF_TYPE][TS_COMMON]);
  gcc_assert (tree_contains_struct[UNDERLYING_TYPE][TS_COMMON]);
  gcc_assert (tree_contains_struct[BASELINK][TS_COMMON]);
  gcc_assert (tree_contains_struct[TYPE_PACK_EXPANSION][TS_COMMON]);
  gcc_assert (tree_contains_struct[TYPE_ARGUMENT_PACK][TS_COMMON]);
  gcc_assert (tree_contains_struct[DECLTYPE_TYPE][TS_COMMON]);
  gcc_assert (tree_contains_struct[BOUND_TEMPLATE_TEMPLATE_PARM][TS_COMMON]);
  gcc_assert (tree_contains_struct[UNBOUND_CLASS_TEMPLATE][TS_COMMON]);

  gcc_assert (tree_contains_struct[EXPR_PACK_EXPANSION][TS_TYPED]);
  gcc_assert (tree_contains_struct[SWITCH_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[IF_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[FOR_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[RANGE_FOR_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[AGGR_INIT_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[EXPR_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[EH_SPEC_BLOCK][TS_TYPED]);
  gcc_assert (tree_contains_struct[CLEANUP_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[SCOPE_REF][TS_TYPED]);
  gcc_assert (tree_contains_struct[CAST_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[NON_DEPENDENT_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[MODOP_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[TRY_BLOCK][TS_TYPED]);
  gcc_assert (tree_contains_struct[THROW_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[HANDLER][TS_TYPED]);
  gcc_assert (tree_contains_struct[REINTERPRET_CAST_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[CONST_CAST_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[STATIC_CAST_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[DYNAMIC_CAST_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[TEMPLATE_ID_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[ARROW_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[SIZEOF_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[ALIGNOF_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[AT_ENCODE_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[UNARY_PLUS_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[TRAIT_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[TYPE_ARGUMENT_PACK][TS_TYPED]);
  gcc_assert (tree_contains_struct[NOEXCEPT_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[NONTYPE_ARGUMENT_PACK][TS_TYPED]);
  gcc_assert (tree_contains_struct[WHILE_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[NEW_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[VEC_NEW_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[BREAK_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[MEMBER_REF][TS_TYPED]);
  gcc_assert (tree_contains_struct[DOTSTAR_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[DO_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[DELETE_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[VEC_DELETE_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[CONTINUE_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[TAG_DEFN][TS_TYPED]);
  gcc_assert (tree_contains_struct[PSEUDO_DTOR_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[TYPEID_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[MUST_NOT_THROW_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[STMT_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[OFFSET_REF][TS_TYPED]);
  gcc_assert (tree_contains_struct[OFFSETOF_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[PTRMEM_CST][TS_TYPED]);
  gcc_assert (tree_contains_struct[EMPTY_CLASS_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[VEC_INIT_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[USING_STMT][TS_TYPED]);
  gcc_assert (tree_contains_struct[LAMBDA_EXPR][TS_TYPED]);
  gcc_assert (tree_contains_struct[CTOR_INITIALIZER][TS_TYPED]);
  gcc_assert (tree_contains_struct[IMPLICIT_CONV_EXPR][TS_TYPED]);
}

#include "gt-cp-cp-objcp-common.h"
