/* Copyright (C) 2009. Free Software Foundation, Inc.
   Contributed by Xinliang David Li (davidxl@google.com) and
                  Raksit Ashok  (raksit@google.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "c-common.h"
#include "toplev.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "diagnostic.h"
#include "debug.h"
#include "gimple.h"
#include "cgraph.h"
#include "l-ipo.h"

struct GTY(()) saved_module_scope
{
  VEC(tree, gc) *module_decls;
  unsigned module_id;
};

static GTY (()) struct saved_module_scope *current_module_scope;
static GTY ((param_is (struct saved_module_scope))) htab_t saved_module_scope_map;
static int primary_module_last_fundef_no = 0;
static location_t primary_module_last_loc;
/* Primary module pending templates.  */
/* Referenced asm ids in primary module.  */
static GTY (()) VEC(tree, gc) *referenced_asm_ids = NULL;
bool parsing_start = false;
/* Nonzero if we're done parsing and into end-of-file activities.  */
int at_eof;

static int aggr_has_equiv_id (tree t1, tree t2);

/* Module scope hash function.  */

static hashval_t
htab_module_scope_hash (const void *ent)
{
  const struct saved_module_scope *const entry
      = (const struct saved_module_scope *) ent;
  return (hashval_t) entry->module_id;
}

/* Module scope equality function.  */

static int
htab_module_scope_eq (const void *ent1, const void *ent2)
{
  const struct saved_module_scope *const entry1
      = (const struct saved_module_scope *) ent1;
  const struct saved_module_scope *const entry2
      = (const struct saved_module_scope *) ent2;

  return entry1->module_id == entry2->module_id;
}

/* Returns the module scope given a module id MOD_ID.  */

static struct saved_module_scope *
get_module_scope (unsigned mod_id)
{
  struct saved_module_scope **slot, key, *module_scope;

  gcc_assert (mod_id);

  if (saved_module_scope_map == NULL)
    saved_module_scope_map = htab_create_ggc (10, htab_module_scope_hash,
                                              htab_module_scope_eq, NULL);
  key.module_id = mod_id;
  slot = (struct saved_module_scope **)
      htab_find_slot (saved_module_scope_map, &key, INSERT);
  module_scope = *slot;
  if (!module_scope)
    {
      module_scope = GGC_CNEW (struct saved_module_scope);
      module_scope->module_id = mod_id;
      *slot = module_scope;
    }
  return module_scope;
}

/* Allocate memory for struct lang_decl for tree T.  */

static struct lang_decl *
alloc_lang_decl (tree t)
{
  size_t size;
  size = lang_hooks.l_ipo.get_lang_decl_size (t);
  return GGC_CNEWVAR (struct lang_decl, size);
}

/* Return a cloned copy of tree SRC.  */

tree
lipo_save_decl (tree src)
{
  tree saved = copy_node (src);
  enum tree_code tc = TREE_CODE (src);
  if (TREE_CODE_CLASS (tc) == tcc_declaration)
    {
      struct lang_decl *ls = NULL;
      struct function *func = NULL;
      DECL_CONTEXT (saved) = DECL_CONTEXT (src);
      if (DECL_LANG_SPECIFIC (src))
        {
          ls = alloc_lang_decl (src);
          memcpy (ls, DECL_LANG_SPECIFIC (src),
                  lang_hooks.l_ipo.get_lang_decl_size (src));
        }
      DECL_LANG_SPECIFIC (saved) = ls;
      if (tc == FUNCTION_DECL && DECL_STRUCT_FUNCTION (src))
        {
          func = GGC_CNEW (struct function);
          *func = *(DECL_STRUCT_FUNCTION (src));
          DECL_STRUCT_FUNCTION (saved) = func;
        }
    }
  else
    {
      gcc_assert (TREE_CODE_CLASS (tc) == tcc_type &&
                  TYPE_MAIN_VARIANT (src) == src);
      TYPE_CONTEXT (saved) = TYPE_CONTEXT (src);
      lang_hooks.l_ipo.dup_lang_type (src, saved);
    }

  return saved;
}

/* Copy tree SAVED to tree DEST.  */

void
lipo_restore_decl (tree dest, tree saved)
{
  enum tree_code tc;
  unsigned old_uid;
  struct lang_decl *oldls;

  tc = TREE_CODE (saved);
  if (TREE_CODE_CLASS (tc) == tcc_declaration)
    {
      struct function *oldfunc = NULL;
      old_uid = DECL_UID (dest);
      oldls = DECL_LANG_SPECIFIC (dest);
      oldfunc
          = (tc == FUNCTION_DECL ? DECL_STRUCT_FUNCTION (dest) : NULL);

      memcpy ((char *) dest + sizeof (struct tree_common),
              (char *) saved + sizeof (struct tree_common),
              sizeof (struct tree_decl_common) - sizeof (struct tree_common));

      if (tc == FUNCTION_DECL)
        memcpy ((char *) dest + sizeof (struct tree_decl_common),
                (char *) saved + sizeof (struct tree_decl_common),
                sizeof (struct tree_function_decl) - sizeof (struct tree_decl_common));

      DECL_UID (dest) = old_uid;
      if (DECL_LANG_SPECIFIC (saved))
        {
          if (!oldls)
            oldls = alloc_lang_decl (dest);
          memcpy (oldls, DECL_LANG_SPECIFIC (saved),
                  lang_hooks.l_ipo.get_lang_decl_size (saved));
          DECL_LANG_SPECIFIC (dest) = oldls;
        }
      else
        DECL_LANG_SPECIFIC (dest) = NULL;

      if (tc == FUNCTION_DECL)
        {
          if (DECL_STRUCT_FUNCTION (saved))
            {
              if (!oldfunc)
                oldfunc = GGC_CNEW (struct function);
              *oldfunc = *(DECL_STRUCT_FUNCTION (saved));
              DECL_STRUCT_FUNCTION (dest) = oldfunc;
            }
          else
            DECL_STRUCT_FUNCTION (dest) = NULL;
        }
    }
  else
    {
      gcc_assert (TREE_CODE_CLASS (tc) == tcc_type);
      lang_hooks.l_ipo.copy_lang_type (saved, dest);
    }
}


/* Return the name for tree TD which is either a decl or type.  */

tree
get_type_or_decl_name (tree td)
{
  tree id;

  if (DECL_P (td))
    id = DECL_NAME (td);
  else
    {
      id = TYPE_NAME (td);
      if (DECL_P (id))
        id = DECL_NAME (id);
    }
  return id;
}

/* For a DECL (a type or a decl) in SCOPE, check to see if it is in
   global or namespace scope. If yes, add it to the current module scope.  */

void
add_decl_to_current_module_scope (tree decl, void *scope)
{
  struct saved_module_scope *module_scope;
  tree id;

  if (!flag_dyn_ipa)
    return;

  if (!parsing_start)
    {
      /* The source file may contains only global variable declations
         -- there is no module grouping data associated with it, so
         neither primary_module_id nor current_module_id is set.  */
      lang_hooks.l_ipo.add_built_in_decl (decl);
      return;
    }

  if (!L_IPO_COMP_MODE)
    return;

  if (!lang_hooks.l_ipo.has_global_name (decl, scope))
    return;

  /* Unlike C++ where names are attached to type decls, for C, the type name
     is identifier node. Thus we need to track type names as well.  */
  id = get_type_or_decl_name (decl);
  if (!id)
    return;

  module_scope = current_module_scope;
  gcc_assert (module_scope && module_scope->module_id == current_module_id);
  VEC_safe_push (tree, gc, module_scope->module_decls, decl);
}

/* Clear name bindings for all decls created in MODULE_SCOPE.  */

static void
clear_module_scope_bindings (struct saved_module_scope *module_scope)
{
  size_t i, len;

  len = VEC_length (tree, module_scope->module_decls);
  for (i = 0; i < len; i++)
    {
      tree decl;

      decl = VEC_index (tree, module_scope->module_decls, i);
      lang_hooks.l_ipo.clear_global_name_bindings (
          get_type_or_decl_name (decl));
    }
}

/* The referenced attribute of a decl is not associated with the
   decl itself but with the assembler name. Remember the referenced
   bits before clearing them.  */

static void
save_assembler_name_reference_bit (void)
{
  varpool_get_referenced_asm_ids (&referenced_asm_ids);
}

/* Clear the reference bits for assembler names before closing the
   module scope.  */

static void
clear_assembler_name_reference_bit (void)
{
  varpool_clear_asm_id_reference_bit ();
}

/* Restore the reference bits for assembler names.  */

static void
restore_assembler_name_reference_bit (void)
{
  size_t i, len;
  len = VEC_length (tree, referenced_asm_ids);
  for (i = 0; i < len; i++)
    TREE_SYMBOL_REFERENCED (VEC_index (tree, referenced_asm_ids, i)) = 1;
}

/* Set up the module scope before the parsing of the
   associated source file.  */

void
push_module_scope (void)
{
  struct saved_module_scope *prev_module_scope;

  if (!flag_dyn_ipa || !L_IPO_COMP_MODE)
    {
      parsing_start = true;
      return;
    }

  prev_module_scope = current_module_scope;
  if (IS_PRIMARY_MODULE)
    {
      gcc_assert (!prev_module_scope);
      lang_hooks.l_ipo.save_built_in_decl_pre_parsing ();
      parsing_start = true;
    }

  gcc_assert (current_module_id);

  /* Set up the module scope.  */
  current_module_scope = get_module_scope (current_module_id);
  return;
}

/* Restore the shared decls to their post parsing states.  */

static void
restore_post_parsing_states (void)
{
  current_module_id = primary_module_id;
  current_module_scope = get_module_scope (primary_module_id);
  set_funcdef_no (primary_module_last_fundef_no);
  input_location = primary_module_last_loc;

  restore_assembler_name_reference_bit ();
  lang_hooks.l_ipo.restore_built_in_decl_post_module_parsing ();
}

/* Pop the current module scope (by clearing name bindings etc.)
   and prepare for parsing of the next module.  In particular,
   built-in decls need to be restored to the state before file
   parsing starts.  */

void
pop_module_scope (void)
{
  bool is_last = false;
  if (!flag_dyn_ipa || !L_IPO_COMP_MODE)
    return;

  gcc_assert (current_module_id && current_module_scope);

  if (IS_PRIMARY_MODULE)
    primary_module_last_loc = input_location;

  at_eof = 1;
  lang_hooks.l_ipo.process_pending_decls (input_location);
  lang_hooks.l_ipo.clear_deferred_fns ();
  at_eof = 0;

  is_last = is_last_module (current_module_id);

  lang_hooks.l_ipo.save_built_in_decl_post_module_parsing ();
  /* Save primary module state if needed (when module group
     size > 1)  */
  if (IS_PRIMARY_MODULE && num_in_fnames > 1)
    {
      save_assembler_name_reference_bit ();
      primary_module_last_fundef_no = get_current_funcdef_no ();
    }

  if (!is_last)
    {
      /* More aux modules are anticipated, clear
         the parsing state.  */
      gcc_assert (num_in_fnames > 1);
      clear_assembler_name_reference_bit ();
      clear_module_scope_bindings (current_module_scope);
      /* Restore symtab bindings for builtins  */
      lang_hooks.l_ipo.restore_built_in_decl_pre_parsing ();
      /* The map can not be cleared because the names of operator
         decls are used to store the information about the conversion
         target type. This forces the coversion operator ids to be
         incremented across different modules, and assember id must
         be used for checksum computation.  */
      /* cp_clear_conv_type_map (); */
    }
  else if (num_in_fnames > 1)
    restore_post_parsing_states ();
  else
    gcc_assert (IS_PRIMARY_MODULE && num_in_fnames == 1);
}


/* Type merging support for LIPO  */

struct type_ec
{
  tree rep_type;
  VEC(tree, heap) *eq_types;
};

static VEC(tree, heap) *pending_types = NULL;
static struct pointer_set_t *type_set = NULL;
static htab_t type_hash_tab = NULL;

/* hash function for the type table.  */

static hashval_t
type_hash_hash (const void *ent)
{
  tree type, name;
  const struct type_ec *const entry
      = (const struct type_ec *) ent;

  type = entry->rep_type;
  name = TYPE_NAME (type);
  if (DECL_P (name))
    name = DECL_NAME (name);

  return htab_hash_string (IDENTIFIER_POINTER (name));
}

/* equality function for type hash table.  */

static int
type_hash_eq (const void *ent1, const void *ent2)
{
  tree type1, type2;
  const struct type_ec *const entry1
      = (const struct type_ec *) ent1;
  const struct type_ec *const entry2
      = (const struct type_ec *) ent2;

  type1 = entry1->rep_type;
  type2 = entry2->rep_type;

  return aggr_has_equiv_id (type1, type2);
}

/* Function to delete type hash entries.  */

static void
type_hash_del (void *ent)
{
  struct type_ec *const entry
      = (struct type_ec *) ent;

  VEC_free (tree, heap, entry->eq_types);
  free (entry);
}

struct GTY(()) type_ent
{
  tree type;
  unsigned eq_id;
};

static GTY ((param_is (struct type_ent))) htab_t l_ipo_type_tab = 0;
static unsigned l_ipo_eq_id = 0;

/* Address hash function for struct type_ent.  */

static hashval_t
type_addr_hash (const void *ent)
{
  const struct type_ent *const entry
      = (const struct type_ent *) ent;
  return (hashval_t) entry->type;
}

/* Address equality function for type_ent.  */

static int
type_addr_eq (const void *ent1, const void *ent2)
{
  const struct type_ent *const entry1
      = (const struct type_ent *) ent1;
  const struct type_ent *const entry2
      = (const struct type_ent *) ent2;
  return entry1->type == entry2->type;
}

/* Returns 1 if NS1 and NS2 refer to the same namespace.  */

static int
is_ns_equiv (tree ns1, tree ns2)
{
  tree n1, n2;
  if (ns1 == NULL && ns2 == NULL)
    return 1;

  if ((!ns1 && ns2) || (ns1 && !ns2))
    return 0;

  gcc_assert (DECL_P (ns1) && DECL_P (ns2));

  if (!is_ns_equiv (DECL_CONTEXT (ns1),
                    DECL_CONTEXT (ns2)))
      return 0;

  n1 = DECL_NAME (ns1);
  n2 = DECL_NAME (ns2);
  if (n1 == 0 && n2 == 0)
    /* Conservative (which can happen when two NSes are from
       different modules but with same UID) quivalence is allowed.  */
    return DECL_UID (ns1) == DECL_UID (ns2);
  if (!n1 || !n2)
    return 0;

  if (!strcmp (IDENTIFIER_POINTER (n1),
               IDENTIFIER_POINTER (n2)))
    return 1;

  return 0;
}

/* Returns 1 if aggregate type T1 and T2 have equivalent qualified
   ids.  */

static int
aggr_has_equiv_id (tree t1, tree t2)
{
  int ctx_match;
  tree ctx1, ctx2, tn1, tn2;
  gcc_assert (TYPE_P (t1) && TYPE_P (t2));

  ctx1 = TYPE_CONTEXT (t1);
  ctx2 = TYPE_CONTEXT (t2);

  if ((ctx1 && !ctx2) || (!ctx1 && ctx2))
    return 0;
  if (ctx1 && TREE_CODE (ctx1) != TREE_CODE (ctx2))
    return 0;

  if (ctx1 && (TREE_CODE (ctx1) == FUNCTION_DECL
               || TREE_CODE (ctx2) == FUNCTION_DECL))
    return 0;

  if (!ctx1)
    {
      ctx_match = 1;
      gcc_assert (!ctx2);
    }
  else if (TREE_CODE (ctx1) == NAMESPACE_DECL)
    ctx_match = is_ns_equiv (ctx1, ctx2);
  else if (TYPE_P (ctx1))
    ctx_match = aggr_has_equiv_id (ctx1, ctx2);
  else
  {
    gcc_assert (TREE_CODE (ctx1) == TRANSLATION_UNIT_DECL);
    ctx_match = 1;
  }

  if (!ctx_match)
    return 0;

  /* Now compare the name of the types.  */
  tn1 = TYPE_NAME (t1);
  tn2 = TYPE_NAME (t2);
  if ((tn1 && !tn2) || !(tn1 && tn2))
    return 0;
  else if (!tn1 && !tn2)
    /* Be conservative on unamed types.  */
    return 1;

  if (DECL_P (tn1))
    tn1 = DECL_NAME (tn1);
  if (DECL_P (tn2))
    tn2 = DECL_NAME (tn2);
  if (strcmp (IDENTIFIER_POINTER (tn1),
              IDENTIFIER_POINTER (tn2)))
    return 0;

  return lang_hooks.l_ipo.cmp_lang_type (t1, t2);
}

/* Return the canonical type of the type's main variant.  */
static inline tree
get_norm_type (tree type)
{
  tree cano_type = TYPE_MAIN_VARIANT (type);
  if (TYPE_CANONICAL (cano_type))
    cano_type = TYPE_CANONICAL (cano_type);

  return cano_type;
}

/* Return 1 if type T1 and T2 are equivalent. Struct/union/class
   types are compared using qualified name ids.  Alias sets of
   equivalent types will be merged. Client code may choose to do
   structural equivalence check for sanity.  */

int
cmp_type_arg (tree t1, tree t2)
{
  if (TREE_CODE (t1) != TREE_CODE (t2))
    return 0;
  if (TYPE_READONLY (t1) != TYPE_READONLY (t2))
    return 0;
  if (TYPE_VOLATILE (t1) != TYPE_VOLATILE (t2))
    return 0;

  t1 = get_norm_type (t1);
  t2 = get_norm_type (t2);

  switch (TREE_CODE (t1))
    {
    case RECORD_TYPE:
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
      return aggr_has_equiv_id (t1, t2);

    case POINTER_TYPE:
    case REFERENCE_TYPE:
    case COMPLEX_TYPE:
      return cmp_type_arg (TREE_TYPE (t1), TREE_TYPE (t2));
    case ARRAY_TYPE:
      return cmp_type_arg (TYPE_DOMAIN (t1), TYPE_DOMAIN (t2))
          && cmp_type_arg (TREE_TYPE (t1), TREE_TYPE (t2));
    case METHOD_TYPE:
      return cmp_type_arg (TYPE_METHOD_BASETYPE (t1),
                          TYPE_METHOD_BASETYPE (t2));
    case FUNCTION_TYPE:
      {
        tree arg1, arg2;
        for (arg1 = TYPE_ARG_TYPES (t1), arg2 = TYPE_ARG_TYPES (t2);
             arg1 && arg2;
             arg1 = TREE_CHAIN (arg1), arg2 = TREE_CHAIN (arg2))
          if (!cmp_type_arg (TREE_VALUE (arg1),
                             TREE_VALUE (arg2)))
            return 0;
        if (arg1 || arg2)
          return 0;
        return 1;
      }
    case OFFSET_TYPE:
      return cmp_type_arg (TYPE_OFFSET_BASETYPE (t1),
                           TYPE_OFFSET_BASETYPE (t2));
    case ENUMERAL_TYPE:
      return cmp_type_arg (TREE_TYPE (t1), TREE_TYPE (t2));
    case REAL_TYPE:
    case FIXED_POINT_TYPE:
    case INTEGER_TYPE:
      return (TYPE_PRECISION (t1) == TYPE_PRECISION (t2)
              && TYPE_MODE (t1) == TYPE_MODE (t2)
              && TYPE_MIN_VALUE (t1) == TYPE_MIN_VALUE (t2)
              && TYPE_MAX_VALUE (t1) == TYPE_MAX_VALUE (t2));
    case VECTOR_TYPE:
      return (TYPE_VECTOR_SUBPARTS (t1) == TYPE_VECTOR_SUBPARTS (t2)
              && cmp_type_arg (TREE_TYPE (t1), TREE_TYPE (t2)));
    case VOID_TYPE:
    case BOOLEAN_TYPE:
      return 1;
    default:
      gcc_unreachable ();
    }
}

#ifndef ANON_AGGRNAME_PREFIX
#define ANON_AGGRNAME_PREFIX "__anon_"
#endif
#ifndef ANON_AGGRNAME_P
#define ANON_AGGRNAME_P(ID_NODE) \
  (!strncmp (IDENTIFIER_POINTER (ID_NODE), ANON_AGGRNAME_PREFIX, \
	     sizeof (ANON_AGGRNAME_PREFIX) - 1))
#endif

/* Callback function used in tree walk to find referenced struct types.  */

static tree
find_struct_types (tree *tp,
                   int *walk_subtrees ATTRIBUTE_UNUSED,
                   void *data ATTRIBUTE_UNUSED)
{
  if (!(*tp))
    return NULL_TREE;

  if (TYPE_P (*tp))
    {
      if (lang_hooks.l_ipo.is_compiler_generated_type (*tp))
        return NULL_TREE;

      switch (TREE_CODE (*tp))
        {
        case RECORD_TYPE:
        case UNION_TYPE:
        case QUAL_UNION_TYPE:
          {
            tree cano_type, name;
            tree context;

            cano_type = get_norm_type (*tp);
            name = TYPE_NAME (cano_type);
            if (!name)
              {
                /* the main variant of typedef of unnamed struct
                   has no name, use the orignal type for equivalence.  */
                cano_type = *tp;
                name = TYPE_NAME (cano_type);
              }
            if (!name)
              return NULL_TREE;
            if (DECL_P (name)
                && (DECL_IGNORED_P (name)
                    || ANON_AGGRNAME_P (DECL_NAME (name))))
              return NULL_TREE;

            if (!pointer_set_insert (type_set, cano_type))
              VEC_safe_push (tree, heap, pending_types, cano_type);

            context = TYPE_CONTEXT (cano_type);
            if (context && TYPE_P (context))
              find_struct_types (&context, NULL, NULL);
            return NULL_TREE;
          }
        case POINTER_TYPE:
        case REFERENCE_TYPE:
        case COMPLEX_TYPE:
          find_struct_types (&(TREE_TYPE (*tp)), NULL, NULL);
          return NULL_TREE;
        case ARRAY_TYPE:
          find_struct_types (&(TREE_TYPE (*tp)), NULL, NULL);
          return NULL_TREE;
        case METHOD_TYPE:
          find_struct_types (&(TYPE_METHOD_BASETYPE (*tp)), NULL, NULL);
          return NULL_TREE;
        case FUNCTION_TYPE:
          {
            tree arg;
            for (arg = TYPE_ARG_TYPES (*tp); arg; arg = TREE_CHAIN (arg))
              find_struct_types (&(TREE_VALUE (arg)), NULL, NULL);
            return NULL_TREE;
          }
        default:
          return NULL_TREE;
        }
    }
  else if (DECL_P (*tp))
    find_struct_types (&(TREE_TYPE (*tp)), NULL, NULL);
  return NULL_TREE;
}

/* Collect referenced struct types.  */

static void
cgraph_collect_type_referenced (void)
{
  basic_block bb;
  gimple_stmt_iterator gi;

  FOR_EACH_BB (bb)
    {
      for (gi = gsi_start_bb (bb); !gsi_end_p (gi); gsi_next (&gi))
        {
          unsigned i;
	  gimple stmt = gsi_stmt (gi);
	  for (i = 0; i < gimple_num_ops (stmt); i++)
	    walk_tree (gimple_op_ptr (stmt, i), find_struct_types, NULL, NULL);
	}
    }
}

/* Check type equivalence. Returns 1 if T1 and T2 are equivalent
   for tbaa; return 0 if not. -1 is returned if it is unknown.  */

int
equivalent_struct_types_for_tbaa (tree t1, tree t2)
{
  struct type_ent key, *tent1, *tent2,  **slot;

  t1 = get_norm_type (t1);
  t2 = get_norm_type (t2);

  key.type = t1;
  slot = (struct type_ent **)
      htab_find_slot (l_ipo_type_tab, &key, NO_INSERT);
  if (!slot || !*slot)
    return -1;
  tent1 = *slot;

  key.type = t2;
  slot = (struct type_ent **)
      htab_find_slot (l_ipo_type_tab, &key, NO_INSERT);
  if (!slot || !*slot)
    return -1;
  tent2 = *slot;

  return tent1->eq_id == tent2->eq_id;
}

/* Build type hash table.  */

static void
cgraph_build_type_equivalent_classes (void)
{
  unsigned n, i;
  n = VEC_length (tree, pending_types);
  for (i = 0; i < n; i++)
    {
      struct type_ec **slot;
      struct type_ec te;
      te.rep_type  = VEC_index (tree, pending_types, i);
      te.eq_types = NULL;
      slot = (struct type_ec **) htab_find_slot (type_hash_tab,
                                                 &te, INSERT);
      if (!*slot)
        {
          *slot = XCNEW (struct type_ec);
          (*slot)->rep_type = te.rep_type;
        }
      VEC_safe_push (tree, heap, (*slot)->eq_types, te.rep_type);
    }
}

/* Re-propagate component types's alias set to that of TYPE. PROCESSED
   is the pointer set of processed types.  */

static void
re_record_component_aliases (tree type,
                             struct pointer_set_t *processed)
{
  alias_set_type superset = get_alias_set (type);
  tree field;

  if (superset == 0)
    return;

  if (pointer_set_insert (processed, type))
    return;

  switch (TREE_CODE (type))
    {
    case RECORD_TYPE:
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
      /* Recursively record aliases for the base classes, if there are any.  */
      if (TYPE_BINFO (type))
	{
	  int i;
	  tree binfo, base_binfo;

	  for (binfo = TYPE_BINFO (type), i = 0;
	       BINFO_BASE_ITERATE (binfo, i, base_binfo); i++)
            {
              re_record_component_aliases (BINFO_TYPE (base_binfo),
                                           processed);
              record_alias_subset (superset,
                                   get_alias_set (BINFO_TYPE (base_binfo)));
            }
	}
      for (field = TYPE_FIELDS (type); field != 0; field = TREE_CHAIN (field))
	if (TREE_CODE (field) == FIELD_DECL && !DECL_NONADDRESSABLE_P (field))
          {
            re_record_component_aliases (TREE_TYPE (field), processed);
            record_alias_subset (superset, get_alias_set (TREE_TYPE (field)));
          }
      break;

    case COMPLEX_TYPE:
      re_record_component_aliases (TREE_TYPE (type), processed);
      record_alias_subset (superset, get_alias_set (TREE_TYPE (type)));
      break;

    /* VECTOR_TYPE and ARRAY_TYPE share the alias set with their
       element type.  */

    default:
      break;
    }
}

/* The callback function to merge alias sets of equivalent types.  */

static int
type_eq_process (void **slot, void *data ATTRIBUTE_UNUSED)
{
  unsigned i, n;
  alias_set_type alias_set;
  bool zero_set = false;
  tree rep_type;
  VEC(tree, heap) *eq_types;
  struct type_ec ** te = (struct type_ec **)slot;
  struct type_ent **slot2, key, *tent;

  rep_type = (*te)->rep_type;
  eq_types = (*te)->eq_types;
  alias_set = get_alias_set (rep_type);

  n = VEC_length (tree, eq_types);
  /* fprintf (stderr, "%%TYPE EQ:\n"); */
  for (i = 0; i < n; i++)
    {
      alias_set_type als;
      tree type = VEC_index (tree, eq_types, i);

      als = get_alias_set (type);

      if (als == 0)
        zero_set = true;

      if (alias_set && als && alias_set != als)
        record_alias_subset (alias_set, als);
#if 0
      /* Dump */
      {
      tree tn = TYPE_NAME (type);
      fprintf (stderr, "\t");
      print_generic_expr (stderr, type, 0);
      if (DECL_P (tn))
        fprintf (stderr, " @ %s:%d \n", DECL_SOURCE_FILE (tn), DECL_SOURCE_LINE (tn));
      else
        fprintf (stderr, "\n");
      /* End Dump */
      }
#endif
    }
  /* fprintf (stderr, "\n"); */

  /* Now propagate back.  */
  for (i = 0; i < n; i++)
    {
      alias_set_type als;
      tree type = VEC_index (tree, eq_types, i);
      als = get_alias_set (type);

      if (zero_set)
        TYPE_ALIAS_SET (type) = 0;
      else if (alias_set != als)
        record_alias_subset (als, alias_set);
    }

  /* Now populate the type table.  */
  l_ipo_eq_id++;
  for (i = 0; i < n; i++)
    {
      key.type = VEC_index (tree, eq_types, i);
      slot2 = (struct type_ent **)
          htab_find_slot (l_ipo_type_tab, &key, INSERT);
      tent = *slot2;
      gcc_assert (!tent);
      tent = GGC_CNEW (struct type_ent);
      tent->type = key.type;
      tent->eq_id = l_ipo_eq_id;
      *slot2 = tent;
    }

  return 1;
}

/* Regenerate alias set for aggregate types.  */

static void
record_components_for_parent_types (void)
{
  unsigned n, i;
  struct pointer_set_t *processed_types;

  processed_types = pointer_set_create ();
  n = VEC_length (tree, pending_types);
  for (i = 0; i < n; i++)
    {
      tree type = VEC_index (tree, pending_types, i);
      re_record_component_aliases (type, processed_types);
    }

  pointer_set_destroy (processed_types);
}

/* Unify type alias sets for equivalent types.  */

void
cgraph_unify_type_alias_sets (void)
{
  struct cgraph_node *node;

  if (!L_IPO_COMP_MODE || !flag_strict_aliasing)
    return;
  type_set = pointer_set_create ();
  type_hash_tab = htab_create (10, type_hash_hash,
                               type_hash_eq, type_hash_del);
  l_ipo_type_tab = htab_create_ggc (10, type_addr_hash,
                                    type_addr_eq, NULL);

  for (node = cgraph_nodes; node; node = node->next)
    {
      if (node->analyzed && (node->needed || node->reachable))
        {
          push_cfun (DECL_STRUCT_FUNCTION (node->decl));
          current_function_decl = node->decl;
          cgraph_collect_type_referenced ();
          current_function_decl = NULL;
          pop_cfun ();
        }
    }

  /* Compute type equivalent classes.  */
  cgraph_build_type_equivalent_classes ();
  /* Now unify alias sets of equivelent types.  */
  htab_traverse (type_hash_tab, type_eq_process, NULL);
  /* Finally re-populating parent's alias set.  */
  record_components_for_parent_types ();

  pointer_set_destroy (type_set);
  VEC_free (tree, heap, pending_types);
  htab_delete (type_hash_tab);
}

#include "gt-l-ipo.h"
