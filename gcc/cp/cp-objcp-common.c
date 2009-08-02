/* Some code common to C++ and ObjC++ front ends.
   Copyright (C) 2004, 2007, 2008, 2009 Free Software Foundation, Inc.
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
#include "c-common.h"
#include "toplev.h"
#include "cgraph.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "diagnostic.h"
#include "debug.h"
#include "cxx-pretty-print.h"
#include "cp-objcp-common.h"
#include "l-ipo.h"

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

/* Langhook for expr_size: Tell the back end that the value of an expression
   of non-POD class type does not include any tail padding; a derived class
   might have allocated something there.  */

tree
cp_expr_size (const_tree exp)
{
  tree type = TREE_TYPE (exp);

  if (CLASS_TYPE_P (type))
    {
      /* The back end should not be interested in the size of an expression
	 of a type with both of these set; all copies of such types must go
	 through a constructor or assignment op.  */
      if (!TYPE_HAS_COMPLEX_INIT_REF (type)
	  || !TYPE_HAS_COMPLEX_ASSIGN_REF (type)
	  /* But storing a CONSTRUCTOR isn't a copy.  */
	  || TREE_CODE (exp) == CONSTRUCTOR
	  /* And, the gimplifier will sometimes make a copy of
	     an aggregate.  In particular, for a case like:

		struct S { S(); };
		struct X { int a; S s; };
		X x = { 0 };

	     the gimplifier will create a temporary with
	     static storage duration, perform static
	     initialization of the temporary, and then copy
	     the result.  Since the "s" subobject is never
	     constructed, this is a valid transformation.  */
	  || CP_AGGREGATE_TYPE_P (type))
	/* This would be wrong for a type with virtual bases.  */
	return (is_really_empty_class (type)
		? size_zero_node
		: CLASSTYPE_SIZE_UNIT (type));
      else
	return NULL_TREE;
    }
  else
    /* Use the default code.  */
    return lhd_expr_size (exp);
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
  pretty_printer *base = context->printer;
  cxx_pretty_printer *pp = XNEW (cxx_pretty_printer);
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
	  && DECL_NONCONVERTING_P (decl));
}

/* LIPO support  */

typedef struct GTY (()) sb
{
  tree decl;
  tree decl_init_copy; /* copy at the start of file parsing.  */
  tree decl_fini_copy; /* copy at the end of module_scope.  */
  tree id;
  cxx_binding *binding;
  /* binding->value may get overwritten during parsing due to
     an incompatible delcaration in the same scope (e.g. clog),
     so we need to expliclitly save the binding value.  */
  tree binding_value;
  /* The binding value in the previous scope: std namespace.  */
  tree std_binding_value;
  tree real_type_value;
} saved_builtin;

DEF_VEC_O(saved_builtin);
DEF_VEC_ALLOC_O(saved_builtin,gc);

static GTY (()) VEC(saved_builtin, gc) *saved_builtins = NULL;

/* Return true if the type is not user defined.  */

bool
cp_is_compiler_generated_type (tree t)
{
  if (TYPE_PTRMEMFUNC_P (t))
    return true;
  return false;
}

/* Clear symbol binding for name ID. */

void
cp_clear_global_name_bindings (tree id)
{
  if (id)
    IDENTIFIER_NAMESPACE_BINDINGS (id) = NULL;
}

/* Return true if DECL is scoped in global/namespace scope, otherwise
   return false.  This is a langhook method that is used to select declarations
   that needs to be explicitly popped out the global/namespace scope
   at the end of parsing the file.  */

bool
cp_is_non_sharable_global_decl (tree decl, void *scope)
{
  cxx_scope *global_scope, *cur_scope;

  cur_scope = (cxx_scope *) scope;
  global_scope = NAMESPACE_LEVEL (global_namespace);
  if (cur_scope->kind != sk_namespace && cur_scope != global_scope)
    return false;

  /* Type info objects are compiler created -- allow such
     decls to be shared (treated as other builtins) across modules.  */
  if (TREE_CODE (decl) == VAR_DECL && DECL_TINFO_P (decl))
    return false;

  return true;
}

/* Duplicate language specific type information from SRC
   to DEST.  */

void
cp_lipo_dup_lang_type (tree src, tree dest)
{
  struct lang_type *lang_type_clone = 0;
  /* TODO check size.  */
  lang_type_clone = GGC_CNEW (struct lang_type);
  *lang_type_clone = *TYPE_LANG_SPECIFIC (src);
  TYPE_LANG_SPECIFIC (dest) = lang_type_clone;

  TYPE_CACHED_VALUES_P (dest) = TYPE_CACHED_VALUES_P (src);
  if (TYPE_CACHED_VALUES_P (src))
    TYPE_CACHED_VALUES (dest) = TYPE_CACHED_VALUES (src);
  /* Main variant's clone's main variant should be itself. */
  TYPE_MAIN_VARIANT (dest) = dest;
  /* Now copy the subdecl.
     Do not reorder this with previous statement -- it
     depends on the result of previous one.  */
  TYPE_MAIN_DECL(dest) = TYPE_MAIN_DECL (src);
}


/* Copy DEST into SRC.  */

void
cp_lipo_copy_lang_type (tree src, tree dest)
{
  struct lang_type *old_ls;
  unsigned old_uid;

  old_ls = TYPE_LANG_SPECIFIC (dest);
  *old_ls = *(TYPE_LANG_SPECIFIC (src));
  old_uid = TYPE_UID (dest);
  memcpy (dest, src, tree_size (dest));

  TYPE_UID (dest) = old_uid;
  TYPE_LANG_SPECIFIC (dest) = old_ls;
  /* recover main variant. */
  TYPE_MAIN_VARIANT (dest) = dest;
  TYPE_MAIN_DECL (dest) = TYPE_MAIN_DECL (src);
}

/* Return the actual size of the lang_decl struct for
   decl T.  */

int
cp_get_lang_decl_size (tree t)
{
  size_t size;
  if (TREE_CODE (t) == FUNCTION_DECL)
    size = sizeof (struct lang_decl_fn);
  else if (TREE_CODE (t) == NAMESPACE_DECL)
    size = sizeof (struct lang_decl_ns);
  else if (TREE_CODE (t) == PARM_DECL)
    size = sizeof (struct lang_decl_parm);
  else if (LANG_DECL_HAS_MIN (t))
    size = sizeof (struct lang_decl_min);
  else
    gcc_unreachable ();

  return (int) size;
}

/* Return 1 if template arguments TA1 and TA2 is compatible.
   Return 0 otherwise.  */

static int
cmp_templ_arg (tree ta1, tree ta2)
{
  if (ARGUMENT_PACK_P (ta1))
    {
      int n, i;
      if (!ARGUMENT_PACK_P (ta2))
        return 0;
      n = TREE_VEC_LENGTH (ta1);
      if (n != TREE_VEC_LENGTH (ta2))
        return 0;
      for (i = 0; i < n ; i++)
        {
          if (!cmp_templ_arg (TREE_VEC_ELT (ta1, i),
                              TREE_VEC_ELT (ta2, i)))
            return 0;
        }
      return 1;
    }
  else if (TYPE_P (ta1))
    {
      if (!TYPE_P (ta2))
        return 0;

      return lipo_cmp_type (ta1, ta2);
    }
  else if (TREE_CODE (ta1) == TEMPLATE_DECL)
    {
      if (TREE_CODE (ta2) != TEMPLATE_DECL)
        return 0;

      /* compare name -- need context comparison:  */
      return !strcmp (IDENTIFIER_POINTER (DECL_NAME (ta1)),
                      IDENTIFIER_POINTER (DECL_NAME (ta2)));
    }
  else /* integer expression  */
    {
      if (TREE_CODE (ta1) != TREE_CODE (ta2))
        return 0;
      if (TREE_CODE (ta1) == INTEGER_CST)
        return (TREE_INT_CST_HIGH (ta1) == TREE_INT_CST_HIGH (ta2)
                && TREE_INT_CST_LOW (ta1) == TREE_INT_CST_LOW (ta2));
      else if (TREE_CODE (ta1) == ADDR_EXPR)
        {
          tree td1, td2;

          td1 = TREE_OPERAND (ta1, 0);
          td2 = TREE_OPERAND (ta2, 0);
          if (TREE_CODE (td1) != TREE_CODE (td2))
            return 0;
          if (TREE_CODE (td1) == FUNCTION_DECL)
            return (cgraph_lipo_get_resolved_node (td1)
                    == cgraph_lipo_get_resolved_node (td2));
          else
            {
              gcc_assert (TREE_CODE (td1) == VAR_DECL);
              return real_varpool_node (td1) == real_varpool_node (td2);
            }
        }
      else
        /* Be conservative (from aliasing point of view) for now (TODO)  */
        return 1;
    }
}

/* Return 1 if template parameters of T1 and T2
   are compatible, returns 0 otherwise.  */

static int
cmp_templ_parms (tree t1, tree t2)
{
  int n_lvl = 1, i;
  tree a1, a2;
  tree args1 = CLASSTYPE_TI_ARGS (t1);
  tree args2 = CLASSTYPE_TI_ARGS (t2);
  if (TMPL_ARGS_HAVE_MULTIPLE_LEVELS (args1)
      && !TMPL_ARGS_HAVE_MULTIPLE_LEVELS (args2))
    return 0;
  if (!TMPL_ARGS_HAVE_MULTIPLE_LEVELS (args1)
      && TMPL_ARGS_HAVE_MULTIPLE_LEVELS (args2))
    return 0;

  if (TREE_VEC_LENGTH (args1) != TREE_VEC_LENGTH (args2))
    return 0;

  if (TMPL_ARGS_HAVE_MULTIPLE_LEVELS (args1))
    n_lvl = TREE_VEC_LENGTH (args1);

  i = 0;
  if (n_lvl == 1)
    {
      a1 = args1;
      a2 = args2;
    }
  else
    {
      a1 = TREE_VEC_ELT (args1, 0);
      a2 = TREE_VEC_ELT (args2, 0);
    }

  while (i < n_lvl)
    {
      int len1, len2, j;

      len1 = TREE_VEC_LENGTH (a1);
      len2 = TREE_VEC_LENGTH (a2);

      if (len1 != len2)
        return 0;

      for (j = 0; j < len1; j++)
        {
          tree ta1, ta2;

          ta1 = TREE_VEC_ELT (a1, j);
          ta2 = TREE_VEC_ELT (a2, j);

          if (!cmp_templ_arg (ta1, ta2))
            return 0;
        }
       i++;
       if (i < n_lvl)
         {
           a1 = TREE_VEC_ELT (args1, i);
           a2 = TREE_VEC_ELT (args2, i);
         }
    }
  return 1;
}

/* Return 1 if type T1 and T2 are compatible. Type comparison
   is based on type kind and name.  */

int
cp_cmp_lang_type (tree t1, tree t2)
{
  int templ1, templ2;

  /* Now check if the type is a template instantiation.  */
  templ1 = (TYPE_LANG_SPECIFIC (t1) && CLASSTYPE_TEMPLATE_INFO (t1));
  templ2 = (TYPE_LANG_SPECIFIC (t2) && CLASSTYPE_TEMPLATE_INFO (t2));

  if ((templ1 && !templ2) || (!templ1 && templ2))
    return 0;
  if (!templ1 && !templ2)
    return 1;

  return cmp_templ_parms (t1, t2);
}

/* Push DECL to the list of builtins declared by the
   frontend.  */

void
cp_add_built_in_decl (tree decl)
{
  saved_builtin *sb;

  if (!flag_dyn_ipa)
    return;

  if (at_eof)
    return;

  if (parser_parsing_start)
    return;

  sb = VEC_safe_push (saved_builtin, gc, saved_builtins, NULL);
  sb->decl = decl;
  sb->decl_init_copy = NULL;
  sb->decl_fini_copy = NULL;
  sb->id = NULL;
  sb->binding = NULL;
  sb->real_type_value = NULL;
  sb->binding_value = NULL;
  sb->std_binding_value = NULL;
}

/* Save SB->decl and its name id's binding values.  */

static void
save_built_in_decl_pre_parsing_1 (saved_builtin *sb)
{
  tree decl = sb->decl;

  sb->decl_init_copy = lipo_save_decl (decl);
  sb->decl_fini_copy = NULL;
  sb->id = NULL;
  sb->binding = NULL;
  sb->real_type_value = NULL;
  sb->binding_value = NULL;
  sb->std_binding_value = NULL;
  if (TREE_CODE_CLASS (TREE_CODE (decl)) != tcc_type)
    sb->id = DECL_NAME (decl);
  else
    {
      tree id;
      id = TYPE_NAME (decl);
      if (TREE_CODE (id) == TYPE_DECL)
        id = DECL_NAME (id);
      sb->id = id;
    }

  if (sb->id)
    {
      sb->real_type_value = REAL_IDENTIFIER_TYPE_VALUE (sb->id);
      sb->binding = IDENTIFIER_NAMESPACE_BINDINGS (sb->id);
      if (sb->binding)
        {
          sb->binding_value = sb->binding->value;

          if (sb->binding->previous)
            sb->std_binding_value = sb->binding->previous->value;
        }
    }
  else
    {
      sb->real_type_value = NULL;
      sb->binding = NULL;
    }

  return;
}

/* Add builtin types into the list of builtins.  */

static void
add_built_in_type_node (void)
{
  tree type_info_node;

  type_info_node = TYPE_MAIN_VARIANT (const_type_info_type_node);
  cp_add_built_in_decl (type_info_node);
}

/* Save the tree (by making a copy) and binding values for
   builtins before parsing start.  */

void
cp_save_built_in_decl_pre_parsing (void)
{
  size_t i;
  saved_builtin *bi;

  add_built_in_type_node ();

  for (i = 0; VEC_iterate (saved_builtin,
                           saved_builtins, i, bi); ++i)
    save_built_in_decl_pre_parsing_1 (bi);
}

/* Restore builtins and their bindings to their values
   before parsing. */

void
cp_restore_built_in_decl_pre_parsing (void)
{
  size_t i;
  saved_builtin *bi;

  for (i = 0; VEC_iterate (saved_builtin,
                           saved_builtins, i, bi); ++i)
    {
      tree decl = bi->decl;

      lipo_restore_decl (decl, bi->decl_init_copy);

      if (bi->id)
        {
          if (bi->binding)
            {
              bi->binding->value = bi->binding_value;
              if (bi->binding->previous)
                bi->binding->previous->value = bi->std_binding_value;
            }
          IDENTIFIER_NAMESPACE_BINDINGS (bi->id) = bi->binding;
          REAL_IDENTIFIER_TYPE_VALUE (bi->id) = bi->real_type_value;
        }
    }
  DECL_NAMESPACE_USING (global_namespace) = NULL;
}

/* Save the tree (by making a copy) and binding values for
   builtins after parsing of a file.  */

void
cp_save_built_in_decl_post_parsing (void)
{
  size_t i;
  saved_builtin *bi;

  for (i = 0; VEC_iterate (saved_builtin,
                           saved_builtins, i, bi); ++i)
    {
      if (!TREE_STATIC (bi->decl) || DECL_ARTIFICIAL (bi->decl))
        continue;
      /* Remember the defining module.  */
      cgraph_link_node (cgraph_node (bi->decl));
      if (!bi->decl_fini_copy)
        bi->decl_fini_copy = lipo_save_decl (bi->decl);
      else
        gcc_assert (TREE_STATIC (bi->decl_fini_copy));
    }
}

/* Restore builtins and their bindings to their post parsing values.  */

void
cp_restore_built_in_decl_post_parsing (void)
{
  unsigned i;
  saved_builtin *bi;

  for (i = 0; VEC_iterate (saved_builtin,
                           saved_builtins, i, bi); ++i)
    {
      tree decl = bi->decl;
      /* Now restore the decl's state  */
      if (bi->decl_fini_copy)
        lipo_restore_decl (decl, bi->decl_fini_copy);
    }
}

void
push_file_scope (void)
{
  push_module_scope ();
}

void
pop_file_scope (void)
{
  pop_module_scope ();
}

/* c-pragma.c needs to query whether a decl has extern "C" linkage.  */
bool
has_c_linkage (const_tree decl)
{
  return DECL_EXTERN_C_P (decl);
}

static GTY ((if_marked ("tree_map_marked_p"), param_is (struct tree_map)))
     htab_t shadowed_var_for_decl;

/* Lookup a shadowed var for FROM, and return it if we find one.  */

tree
decl_shadowed_for_var_lookup (tree from)
{
  struct tree_map *h, in;
  in.base.from = from;

  h = (struct tree_map *) htab_find_with_hash (shadowed_var_for_decl, &in,
					       htab_hash_pointer (from));
  if (h)
    return h->to;
  return NULL_TREE;
}

/* Insert a mapping FROM->TO in the shadowed var hashtable.  */

void
decl_shadowed_for_var_insert (tree from, tree to)
{
  struct tree_map *h;
  void **loc;

  h = GGC_NEW (struct tree_map);
  h->hash = htab_hash_pointer (from);
  h->base.from = from;
  h->to = to;
  loc = htab_find_slot_with_hash (shadowed_var_for_decl, h, h->hash, INSERT);
  *(struct tree_map **) loc = h;
}

void
init_shadowed_var_for_decl (void)
{
  shadowed_var_for_decl = htab_create_ggc (512, tree_map_hash,
					   tree_map_eq, 0);
}


#include "gt-cp-cp-objcp-common.h"
