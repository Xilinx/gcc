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

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
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
#include "coverage.h"
#include "gcov-io.h"

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
bool parser_parsing_start = false;
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
                sizeof (struct tree_function_decl)
                - sizeof (struct tree_decl_common));

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

  if (!parser_parsing_start)
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
  size_t i;
  tree decl;

  for (i = 0;
       VEC_iterate (tree, module_scope->module_decls, i, decl);
       ++i)
    lang_hooks.l_ipo.clear_global_name_bindings (
        get_type_or_decl_name (decl));
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
  size_t i;
  tree nm;
  for (i = 0;
       VEC_iterate (tree, referenced_asm_ids, i, nm);
       ++i)
    TREE_SYMBOL_REFERENCED (nm) = 1;
}

/* Set up the module scope before the parsing of the
   associated source file.  */

void
push_module_scope (void)
{
  struct saved_module_scope *prev_module_scope;

  if (!flag_dyn_ipa || !L_IPO_COMP_MODE)
    {
      parser_parsing_start = true;
      return;
    }

  prev_module_scope = current_module_scope;
  if (L_IPO_IS_PRIMARY_MODULE)
    {
      gcc_assert (!prev_module_scope);
      lang_hooks.l_ipo.save_built_in_decl_pre_parsing ();
      parser_parsing_start = true;
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

  if (L_IPO_IS_PRIMARY_MODULE)
    primary_module_last_loc = input_location;

  at_eof = 1;
  lang_hooks.l_ipo.process_pending_decls (input_location);
  lang_hooks.l_ipo.clear_deferred_fns ();
  at_eof = 0;

  is_last = is_last_module (current_module_id);

  lang_hooks.l_ipo.save_built_in_decl_post_module_parsing ();
  /* Save primary module state if needed (when module group
     size > 1)  */
  if (L_IPO_IS_PRIMARY_MODULE && num_in_fnames > 1)
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
    gcc_assert (L_IPO_IS_PRIMARY_MODULE && num_in_fnames == 1);
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

/* Hash function for the type table.  */

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

/* Equality function for type hash table.  */

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
  return (hashval_t) (long) entry->type;
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
               || TREE_CODE (ctx1) == BLOCK))
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
get_norm_type (const_tree type)
{
  tree cano_type = TYPE_MAIN_VARIANT (type);
  if (TYPE_CANONICAL (cano_type))
    cano_type = TYPE_CANONICAL (cano_type);

  return cano_type;
}

/* Return 1 if type T1 and T2 are equivalent. Struct/union/class
   types are compared using qualified name ids.  Alias sets of
   equivalent types will be merged. Client code may choose to do
   structural equivalence check for sanity.  Note the difference
   between the types_compatible_p (and its langhooks subroutines)
   and this interface. The former is mainly used to remove useless
   type conversion and value numbering computation. It returns 1
   only when it is sure and should not be used in contexts where
   erroneously returning 0 causes problems. This interface
   lipo_cmp_type behaves differently - it returns 1 when it is not
   sure -- as the primary purpose of the interface is for alias
   set computation.  */

int
lipo_cmp_type (tree t1, tree t2)
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
      return lipo_cmp_type (TREE_TYPE (t1), TREE_TYPE (t2));
    case ARRAY_TYPE:
      return (lipo_cmp_type (TYPE_DOMAIN (t1), TYPE_DOMAIN (t2))
              && lipo_cmp_type (TREE_TYPE (t1), TREE_TYPE (t2)));
    case METHOD_TYPE:
      return lipo_cmp_type (TYPE_METHOD_BASETYPE (t1),
                            TYPE_METHOD_BASETYPE (t2));
    case FUNCTION_TYPE:
      {
        tree arg1, arg2;
        for (arg1 = TYPE_ARG_TYPES (t1), arg2 = TYPE_ARG_TYPES (t2);
             arg1 && arg2;
             arg1 = TREE_CHAIN (arg1), arg2 = TREE_CHAIN (arg2))
          if (!lipo_cmp_type (TREE_VALUE (arg1),
                              TREE_VALUE (arg2)))
            return 0;
        if (arg1 || arg2)
          return 0;
        return 1;
      }
    case OFFSET_TYPE:
      return lipo_cmp_type (TYPE_OFFSET_BASETYPE (t1),
                            TYPE_OFFSET_BASETYPE (t2));
    case ENUMERAL_TYPE:
      return lipo_cmp_type (TREE_TYPE (t1), TREE_TYPE (t2));
    case REAL_TYPE:
    case FIXED_POINT_TYPE:
    case INTEGER_TYPE:
      return (TYPE_PRECISION (t1) == TYPE_PRECISION (t2)
              && TYPE_MODE (t1) == TYPE_MODE (t2)
              && TYPE_MIN_VALUE (t1) == TYPE_MIN_VALUE (t2)
              && TYPE_MAX_VALUE (t1) == TYPE_MAX_VALUE (t2));
    case VECTOR_TYPE:
      return (TYPE_VECTOR_SUBPARTS (t1) == TYPE_VECTOR_SUBPARTS (t2)
              && lipo_cmp_type (TREE_TYPE (t1), TREE_TYPE (t2)));
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
            tree field;

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
            else
              return NULL_TREE; /* Or use walk tree without dups.  */

            context = TYPE_CONTEXT (cano_type);
            if (context && TYPE_P (context))
              walk_tree (&context, find_struct_types, NULL, NULL);

            /* Instantiate a nested work as the tree walker does not
               get to the fields.  */
            if (TYPE_BINFO (cano_type))
	      {
                int i;
                tree binfo, base_binfo;

                for (binfo = TYPE_BINFO (cano_type), i = 0;
                     BINFO_BASE_ITERATE (binfo, i, base_binfo); i++)
                  walk_tree (&BINFO_TYPE (base_binfo), find_struct_types,
                             NULL, NULL);
              }
            for (field = TYPE_FIELDS (cano_type);
                 field != 0;
                 field = TREE_CHAIN (field))
              walk_tree (&TREE_TYPE (field), find_struct_types,
                         NULL, NULL);
            return NULL_TREE;
          }
        default:
          return NULL_TREE;
        }
    }
  else if (DECL_P (*tp))
    /* walk tree does not walk down decls, so do a nested walk here.  */
    walk_tree (&(TREE_TYPE (*tp)), find_struct_types, NULL, NULL);

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
equivalent_struct_types_for_tbaa (const_tree t1, const_tree t2)
{
  struct type_ent key, *tent1, *tent2,  **slot;

  if (!l_ipo_type_tab)
    return -1;

  t1 = get_norm_type (t1);
  t2 = get_norm_type (t2);

  key.type = (tree) (long) t1;
  slot = (struct type_ent **)
      htab_find_slot (l_ipo_type_tab, &key, NO_INSERT);
  if (!slot || !*slot)
    return -1;
  tent1 = *slot;

  key.type = (tree) (long) t2;
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
  unsigned i;
  alias_set_type alias_set, ptr_alias_set = -1;
  tree rep_type, type;
  VEC(tree, heap) *eq_types;
  struct type_ec ** te = (struct type_ec **)slot;
  bool zero_set = false, ptr_zero_set = false;
  struct type_ent **slot2, key, *tent;


  rep_type = (*te)->rep_type;
  eq_types = (*te)->eq_types;
  alias_set = get_alias_set (rep_type);

  for (i = 0;
       VEC_iterate (tree, eq_types, i, type);
       ++i)
    {
      alias_set_type als, ptr_als = -1;
      tree type_ptr = TYPE_POINTER_TO (type);;

      als = get_alias_set (type);
      if (als == 0)
        zero_set = true;

      if (alias_set && als && alias_set != als)
        record_alias_subset (alias_set, als);

      if (type_ptr)
        {
          ptr_als = get_alias_set (type_ptr);
          if (ptr_als == 0)
            ptr_zero_set = true;

          if (ptr_alias_set == -1)
            ptr_alias_set = ptr_als;
          else
            {
              if (!ptr_zero_set && ptr_alias_set != ptr_als)
                record_alias_subset (ptr_alias_set, ptr_als);
            }
        }
    }

  /* Now propagate back.  */
  for (i = 0;
       VEC_iterate (tree, eq_types, i, type);
       ++i)
    {
      alias_set_type als, ptr_als;
      tree ptr_type = TYPE_POINTER_TO (type);

      als = get_alias_set (type);

      if (zero_set)
        TYPE_ALIAS_SET (type) = 0;
      else if (alias_set != als)
        record_alias_subset (als, alias_set);

      if (ptr_type)
        {
          ptr_als = get_alias_set (ptr_type);
          if (ptr_zero_set)
            TYPE_ALIAS_SET (ptr_type) = 0;
          else if (ptr_alias_set != ptr_als)
            record_alias_subset (ptr_als, ptr_alias_set);
        }
    }


  /* Now populate the type table.  */
  l_ipo_eq_id++;
  for (i = 0;
       VEC_iterate (tree, eq_types, i, type);
       ++i)
    {
      key.type = type;
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
  struct varpool_node *pv;

  if (!L_IPO_COMP_MODE)
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

  for (pv = varpool_nodes; pv; pv = pv->next)
    walk_tree (&pv->decl, find_struct_types, NULL, NULL); 

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

/* Return true if NODE->decl from an auxiliary module has external
   definition (and therefore is not needed for expansion).  */

bool
cgraph_is_aux_decl_external (struct cgraph_node *node)
{
  tree decl = node->decl;

  if (!L_IPO_COMP_MODE)
    return false;

  if (!cgraph_is_auxiliary (decl))
    return false;

  /* Versioned clones from auxiliary moduels are not
     external.  */
  if (node->is_versioned_clone)
    return false;

  /* virtual functions won't be deleted in the primary module.  */
  if (DECL_VIRTUAL_P (decl))
    return true;

  /* Comdat or weak functions in aux modules are not external --
     there is no guarantee that the definitition will be emitted
     in the primary compilation of this auxiliary module.  */
  if (DECL_COMDAT (decl) || DECL_WEAK (decl))
    return false;

  /* The others from aux modules are external. */
  return true;
}

/* Linked function symbol (cgraph node)  table.  */
static GTY((param_is (struct cgraph_sym))) htab_t cgraph_symtab;

/* This is true when global linking is needed and performed (for C++).
   For C, symbol linking is performed on the fly during parsing, and
   the cgraph_symtab is used only for keeping additional information
   for any already merged symbol if needed.  */

static bool global_link_performed = 0;

/* For an external (non-defined) function DECL, return the primary
   module id (even though when the declaration is declared in an aux
   module). For a defined function DECL, return the module id in which
   it is defined.  */

unsigned
cgraph_get_module_id (tree decl)
{
  struct function *func = DECL_STRUCT_FUNCTION (decl);
  /* Not defined.  */
  if (!func)
    return primary_module_id;
  return FUNC_DECL_MODULE_ID (func);
}

/* Return true if function decl is defined in an auxiliary module.  */

bool
cgraph_is_auxiliary (tree decl)
{
  return (cgraph_get_module_id (decl) != primary_module_id);
}

/* Return the hash value for cgraph_sym pointed to by P. The
   hash value is computed using function's assembler name.  */ 

static hashval_t
hash_sym_by_assembler_name (const void *p)
{
  const struct cgraph_sym *n = (const struct cgraph_sym *) p;
  return (hashval_t) decl_assembler_name_hash (n->assembler_name);
}

/* Return nonzero if P1 and P2 are equal.  */

static int
eq_assembler_name (const void *p1, const void *p2)
{
  const struct cgraph_sym *n1 = (const struct cgraph_sym *) p1;
  const_tree name = (const_tree) p2;
  return (decl_assembler_name_equal (n1->rep_decl, name));
}

/* Return the cgraph_sym for function declaration DECL.  */

static struct cgraph_sym **
cgraph_sym (tree decl)
{
  struct cgraph_sym **slot;
  tree name;

  if (!cgraph_symtab)
    {
      gcc_assert (!global_link_performed);
      return NULL;
    }

  name = DECL_ASSEMBLER_NAME (decl);
  slot = (struct cgraph_sym **)
      htab_find_slot_with_hash (cgraph_symtab, name,
                                decl_assembler_name_hash (name),
                                NO_INSERT);
  return slot;
}

/* Return the representative declaration for assembler name
   ASM_NAME.  */

tree
cgraph_find_decl (tree asm_name)
{
  struct cgraph_sym **slot;
  if (!L_IPO_COMP_MODE)
    return NULL;
  if (!cgraph_symtab || !global_link_performed)
    return NULL;

  slot = (struct cgraph_sym **)
      htab_find_slot_with_hash (cgraph_symtab, asm_name,
                                decl_assembler_name_hash (asm_name),
                                NO_INSERT);
  if (!slot || !*slot)
    return NULL;

  return (*slot)->rep_node->decl;
}

/* Return true if function declaration DECL is originally file scope
   static, which is promoted to global scope.  */

bool
cgraph_is_promoted_static_func (tree decl)
{
  struct cgraph_sym ** sym;
  gcc_assert (L_IPO_COMP_MODE);

  /* cgraph_symtab will be created when any symbol got
     promoted.  */
  if (!cgraph_symtab)
    return false;

  sym = cgraph_sym (decl);
  if (!sym)
    return false;
  return (*sym)->is_promoted_static;
}

/* Hash function for module information table. ENT
   is a pointer to a cgraph_module_info.  */

static hashval_t
htab_sym_hash (const void *ent)
{
  const struct cgraph_mod_info * const mi
      = (const struct cgraph_mod_info * const ) ent;
  return (hashval_t) mi->module_id;
}

/* Hash equality function for module information table.  */

static int
htab_sym_eq (const void *ent1, const void *ent2)
{
  const struct cgraph_mod_info * const mi1
      = (const struct cgraph_mod_info * const ) ent1;
  const struct cgraph_mod_info * const mi2
      = (const struct cgraph_mod_info * const ) ent2;
  return (mi1->module_id == mi2->module_id);
}

/* cgraph_sym SYM may be defined in more than one source modules.
   Add declaration DECL's definiting module to SYM.  */

static void
add_define_module (struct cgraph_sym *sym, tree decl)
{
  unsigned module_id;
  struct cgraph_mod_info **slot;
  struct cgraph_mod_info mi;

  struct function *f = DECL_STRUCT_FUNCTION (decl);
  if (!f)
    return;
  module_id = FUNC_DECL_MODULE_ID (f);

  if (!sym->def_module_hash)
    sym->def_module_hash
        = htab_create_ggc (10, htab_sym_hash, htab_sym_eq, NULL);

  mi.module_id = module_id;
  slot = (struct cgraph_mod_info **)htab_find_slot (sym->def_module_hash,
                                                    &mi, INSERT);
  if (!*slot)
    {
      *slot = GGC_CNEW (struct cgraph_mod_info);
      (*slot)->module_id = module_id;
    }
  else
    gcc_assert ((*slot)->module_id == module_id);
}

/* Return true if the symbol associated with DECL is defined in module
   MODULE_ID.  This interface is used by the inliner to make sure profile-gen
   and profile-use pass (L-IPO mode) make consistent inline decision.  */

bool
cgraph_is_inline_body_available_in_module (tree decl, unsigned module_id)
{
  struct cgraph_sym **sym;
  void **slot;
  struct cgraph_mod_info mi;

  gcc_assert (L_IPO_COMP_MODE);

  if (DECL_BUILT_IN (decl))
    return true;

  /* TODO: revisit this.  */
  if (DECL_IN_SYSTEM_HEADER (decl) && DECL_DECLARED_INLINE_P (decl))
    return true;

  gcc_assert (TREE_STATIC (decl) || DECL_DECLARED_INLINE_P (decl));

  if (cgraph_get_module_id (decl) == module_id)
    return true;

  sym = cgraph_sym (decl);
  if (!sym || !(*sym)->def_module_hash)
    return false;

  mi.module_id = module_id;
  slot = htab_find_slot ((*sym)->def_module_hash, &mi, NO_INSERT);
  if (slot)
    {
      gcc_assert (((struct cgraph_mod_info*)*slot)->module_id == module_id);
      return true;
    }
  return false;
}

/* Return the linked cgraph node using DECL's assembler name.  DO_ASSERT
   is a flag indicating that a non null link target must be returned.  */

struct cgraph_node *
cgraph_lipo_get_resolved_node_1 (tree decl, bool do_assert)
{
  struct cgraph_sym **slot;

  slot = cgraph_sym (decl);

  if (!slot || !*slot)
    {
      if (!do_assert)
        return NULL;
      else
        {
          /* Nodes that are indirectly called are not 'reachable' in
             the callgraph. If they are not needed (comdat, inline
             extern etc), they may be removed from the link table
             before direct calls to them are exposed (via indirect
             call promtion by const folding etc). When this happens,
             the node will be to be relinked. A probably better fix
             is to modify the callgraph so that they are not eliminated
             in the first place -- this will allow inlining to happen.  */

          struct cgraph_node *n = cgraph_node (decl);
          if (!n->analyzed)
            {
              gcc_assert (DECL_EXTERNAL (decl)
                          || cgraph_is_aux_decl_external (n)
                          || DECL_VIRTUAL_P (decl));
              gcc_assert ((!n->reachable && !n->needed)
                          /* This is the case for explicit extern instantiation,
                             when cgraph node is not created before link.  */
                          || DECL_EXTERNAL (decl));
              cgraph_link_node (n);
              return n;
            }
          else
            gcc_unreachable ();
        }
    }
  else
    {
      struct cgraph_sym *sym = *slot;
      return sym->rep_node;
    }
}

/* Return the cgraph_node of DECL if decl has definition; otherwise return
   the cgraph node of the representative decl, which is the declaration DECL
   is resolved to after linking/symbol resolution.  */

struct cgraph_node *
cgraph_lipo_get_resolved_node (tree decl)
{
  struct cgraph_node *node = NULL;

  gcc_assert (L_IPO_COMP_MODE && global_link_performed);
  gcc_assert (cgraph_symtab);

  /* Never merged.  */
  if (!TREE_PUBLIC (decl) || DECL_ARTIFICIAL (decl)
      /* builtin function decls are shared across modules, but 'linking'
         is still performed for them to keep track of the set of defining
         modules. Skip the real resolution here to avoid merging '__builtin_xxx'
         with 'xxx'.  */
      || DECL_BUILT_IN (decl))
    return cgraph_node (decl);

  /* if (gimple_has_body_p (decl)) */
  if (TREE_STATIC (decl))
    return cgraph_node (decl);

  node = cgraph_lipo_get_resolved_node_1 (decl, true);
  return node;
}

/* When NODE->decl is dead function eliminated,
   remove the entry in the link table.  */

void
cgraph_remove_link_node (struct cgraph_node *node)
{
  tree name, decl;

  if (!L_IPO_COMP_MODE || !cgraph_symtab)
    return;

  decl = node->decl;

  /* Skip nodes that are not in the link table.  */
  if (!TREE_PUBLIC (decl) || DECL_ARTIFICIAL (decl))
    return;

  /* Skip if node is an inline clone or if the node has
     defintion that is not really resolved to the merged node.  */
  if (cgraph_lipo_get_resolved_node_1 (decl, false) != node)
    return;

  name = DECL_ASSEMBLER_NAME (decl);
  htab_remove_elt_with_hash (cgraph_symtab, name,
                             decl_assembler_name_hash (name));
}

/* Return true if the function body for DECL has profile information.  */

static bool
has_profile_info (tree decl)
{
  gcov_type *ctrs = NULL;
  unsigned n;
  struct function* f = DECL_STRUCT_FUNCTION (decl);

  ctrs = get_coverage_counts_no_warn (f, GCOV_COUNTER_ARCS, &n);
  if (ctrs)
    {
      unsigned i;
      for (i = 0; i < n; i++)
        if (ctrs[i])
          return true;
    }

  return false;
}

/* Resolve delaration NODE->decl for function symbol *SLOT.  */

static void
resolve_cgraph_node (struct cgraph_sym **slot, struct cgraph_node *node)
{
  tree decl1, decl2;
  int decl1_defined = 0;
  int decl2_defined = 0;

  decl1 = (*slot)->rep_decl;
  decl2 = node->decl;

  /* Can not use gimple_has_body_p because there is no
     guarantee functions are gimplified at this point.  */
  decl1_defined = TREE_STATIC (decl1);
  decl2_defined = TREE_STATIC (decl2);

  if (decl1_defined && !decl2_defined)
    return;

  if (!decl1_defined && decl2_defined)
    {
      (*slot)->rep_node = node;
      (*slot)->rep_decl = decl2;
      add_define_module (*slot, decl2);
      return;
    }

  if (decl2_defined)
    {
      bool has_prof1 = false;
      bool has_prof2 = false;
      gcc_assert (decl1_defined);
      add_define_module (*slot, decl2);

      has_prof1 = has_profile_info (decl1);
      if (has_prof1)
        return;
      has_prof2 = has_profile_info (decl2);
      if (has_prof2)
        {
          (*slot)->rep_node = node;
          (*slot)->rep_decl = decl2;
        }
      return;
    }
  return;
}


/* Resolve NODE->decl in the function symbol table.  */

struct cgraph_sym *
cgraph_link_node (struct cgraph_node *node)
{
  void **slot;
  tree name;

  if (!L_IPO_COMP_MODE)
    return NULL;

  if (!cgraph_symtab)
    cgraph_symtab
        = htab_create_ggc (10, hash_sym_by_assembler_name,
                           eq_assembler_name, NULL);

  /* Skip the cases when the  defintion can be locally resolved, and
     when we do not need to keep track of defining modules.  */
  if (!TREE_PUBLIC (node->decl) || DECL_ARTIFICIAL (node->decl))
    return NULL;

  name = DECL_ASSEMBLER_NAME (node->decl);
  slot = htab_find_slot_with_hash (cgraph_symtab, name,
                                   decl_assembler_name_hash (name),
                                   INSERT);
  if (*slot)
    resolve_cgraph_node ((struct cgraph_sym **) slot, node);
  else
    {
      struct cgraph_sym *sym = GGC_CNEW (struct cgraph_sym);
      sym->rep_node = node;
      sym->rep_decl = node->decl;
      sym->assembler_name = name;
      add_define_module (sym, node->decl);
      *slot = sym;
    }
  return (struct cgraph_sym *) *slot;
}

/* Perform cross module linking of function declarations.  */

void
cgraph_do_link (void)
{
  struct cgraph_node *node;

  if (!L_IPO_COMP_MODE)
    return;

  global_link_performed = 1;

  if (!cgraph_symtab)
    cgraph_symtab
        = htab_create_ggc (10, hash_sym_by_assembler_name,
                           eq_assembler_name, NULL);

  for (node = cgraph_nodes; node; node = node->next)
    {
      gcc_assert (!node->global.inlined_to);
      cgraph_link_node (node);
    }
}

struct promo_ent
{
  char* assemb_name;
  int seq;
};

/* Hash function for promo_ent table.  */

static hashval_t
promo_ent_hash (const void *ent)
{
  const struct promo_ent *const entry
      = (const struct promo_ent *) ent;

  return htab_hash_string (entry->assemb_name);
}

/* Hash_eq function for promo_ent table.  */

static int
promo_ent_eq (const void *ent1, const void *ent2)
{
  const struct promo_ent *const entry1
      = (const struct promo_ent *) ent1;
  const struct promo_ent *const entry2
      = (const struct promo_ent *) ent2;
  if (!strcmp (entry1->assemb_name, entry2->assemb_name))
    return 1;
  return 0;
}

/* Delete function for promo_ent hash table.  */

static void
promo_ent_del (void *ent)
{
  struct promo_ent *const entry
      = (struct promo_ent *) ent;

  free (entry->assemb_name);
  free (entry);
}

static htab_t promo_ent_hash_tab = NULL;

/* Return a unique sequence number for NAME. This is needed to avoid
   name conflict -- function scope statics may have identical names.

   This function returns a zero sequence number if it is called with
   a particular NAME for the first time, and non-zero otherwise.
   This fact is used to keep track of unseen weak variables.  */

static int
get_name_seq_num (const char *name)
{
  struct promo_ent **slot;
  struct promo_ent ent;
  ent.assemb_name = xstrdup (name);
  ent.seq = 0;

  slot = (struct promo_ent **)
      htab_find_slot (promo_ent_hash_tab, &ent, INSERT);

  if (!*slot)
    {
      *slot = XCNEW (struct promo_ent);
      (*slot)->assemb_name = ent.assemb_name;
    }
  else
    {
      (*slot)->seq++;
      free (ent.assemb_name);
    }
  return (*slot)->seq;
}

/* Promote DECL to be global. MODULE_ID is the id of the module where
   DECL is defined. IS_EXTERN is a flag indicating if externalization
   is needed.  */

static void
promote_static_var_func (unsigned module_id, tree decl, bool is_extern)
{
  tree id, assemb_id;
  char *assembler_name;
  const char *name;
  struct  function *context = NULL;
  tree alias;
  int seq = 0;

  /* No need to promote symbol alias.  */
  alias = lookup_attribute ("alias", DECL_ATTRIBUTES (decl));
  if (alias)
    return;

  /* Function decls in C++ may contain characters not taken by assembler.
     Similarly, function scope static variable has UID as the assembler name
     suffix which is not consistent across modules.  */

  if (DECL_ASSEMBLER_NAME_SET_P (decl)
      && TREE_CODE (decl) == FUNCTION_DECL)
    cgraph_remove_assembler_hash_node (cgraph_node (decl));

  if (TREE_CODE (decl) == FUNCTION_DECL)
    {
      if (!DECL_CONTEXT (decl)
          || TREE_CODE (DECL_CONTEXT (decl)) == TRANSLATION_UNIT_DECL)
        {
          id = DECL_NAME (decl);
          /* if (IDENTIFIER_OPNAME_P (id))  */
          if (TREE_LANG_FLAG_2 (id))
            id = DECL_ASSEMBLER_NAME (decl);
        }
      else
        id = DECL_ASSEMBLER_NAME (decl);
    }
  else
    {
      if (!DECL_CONTEXT (decl))
        id = DECL_NAME (decl);
      else if (TREE_CODE (DECL_CONTEXT (decl)) == NAMESPACE_DECL)
        id = DECL_ASSEMBLER_NAME (decl);
      else if (TREE_CODE (DECL_CONTEXT (decl)) == FUNCTION_DECL)
        {
          id = DECL_NAME (decl);
          context = DECL_STRUCT_FUNCTION (DECL_CONTEXT (decl));
        }
      else
        /* file scope context */
        id = DECL_NAME (decl);
    }

  name = IDENTIFIER_POINTER (id);
  if (context)
    {
      char *n;
      unsigned fno =  FUNC_DECL_FUNC_ID (context);
      n = (char *)alloca (strlen (name) + 15);
      sprintf (n, "%s_%u", name, fno);
      name = n;
    }

  assembler_name = (char*) alloca (strlen (name) + 30);
  sprintf (assembler_name, "%s_cmo_%u", name, module_id);
  seq = get_name_seq_num (assembler_name);
  if (seq)
    sprintf (assembler_name, "%s_%d", assembler_name, seq);

  assemb_id = get_identifier (assembler_name);
  SET_DECL_ASSEMBLER_NAME (decl, assemb_id);
  TREE_PUBLIC (decl) = 1;
  DECL_VISIBILITY (decl) = VISIBILITY_HIDDEN;
  DECL_VISIBILITY_SPECIFIED (decl) = 1;

  if (TREE_CODE (decl) == FUNCTION_DECL)
    {
      struct cgraph_sym *resolved_sym = NULL;
      struct cgraph_node *node = cgraph_node (decl);
      cgraph_add_assembler_hash_node (node);
      /* incremental update the link table -- or
         can introduce a flag in cgraph node to indicate
         non global origin.  */
      resolved_sym = cgraph_link_node (node);
      gcc_assert (resolved_sym);
      resolved_sym->is_promoted_static = 1;
    }
  else
    {
      struct varpool_node *node = varpool_node (decl);
      varpool_link_node (node);
    }

  if (is_extern)
    {
      if (TREE_CODE (decl) == VAR_DECL)
        {
          TREE_STATIC (decl) = 0;
          DECL_EXTERNAL (decl) = 1;
          DECL_INITIAL (decl) = 0;
	  DECL_CONTEXT (decl) = 0;
        }
      /* else
         Function body will be deleted later before expansion.  */
    }
  else
    TREE_STATIC (decl) = 1;
}

/* Externalize global variables from aux modules and promote
   static variables.
   WEAK variables need special treatment. If there is a definition in the
   primary module, all the definitions of this weak variable in auxiliary
   modules are externalized. Otherwise, all but one definition in the
   auxiliary modules is externalized. For this reason, before the first
   call to this function, we must have a record of all the weak variables
   emitted from the primary module.  */

static void
process_module_scope_static_var (struct varpool_node *vnode)
{
  tree decl = vnode->decl;

  if (varpool_is_auxiliary (vnode))
    {
      gcc_assert (vnode->module_id != primary_module_id);
      if (TREE_PUBLIC (decl))
        {
          /* Externalize non-weak variables, and those weak variables that
	     we have seen one non-external copy of.  */
	  if (!DECL_WEAK (decl)
	      || (!DECL_EXTERNAL (decl)
		  && get_name_seq_num (IDENTIFIER_POINTER (DECL_NAME (decl)))))
	    {
	      DECL_EXTERNAL (decl) = 1;
	      TREE_STATIC (decl) = 0;
	      DECL_INITIAL (decl) = NULL;
	      DECL_CONTEXT (decl) = NULL;
	    }
        }
      else
        {
          /* Promote static vars to global.  */
          if (vnode->module_id)
            promote_static_var_func (vnode->module_id, decl,
                                     varpool_is_auxiliary (vnode));
        }
    }
  else
    {
      if (PRIMARY_MODULE_EXPORTED && !TREE_PUBLIC (decl))
        promote_static_var_func (vnode->module_id, decl,
                                 varpool_is_auxiliary (vnode));
    }
}

/* Promote static function CNODE->decl to be global.  */

static void
process_module_scope_static_func (struct cgraph_node *cnode)
{
  tree decl = cnode->decl;

  if (TREE_PUBLIC (decl)
      || !TREE_STATIC (decl)
      || DECL_EXTERNAL (decl)
      || DECL_ARTIFICIAL (decl))
    return;

  if (cgraph_is_auxiliary (cnode->decl))
    {
      gcc_assert (cgraph_get_module_id (cnode->decl)
                  != primary_module_id);
      /* Promote static function to global.  */
      if (cgraph_get_module_id (cnode->decl))
        promote_static_var_func (cgraph_get_module_id (cnode->decl), decl, 1);
    }
  else
    {
      if (PRIMARY_MODULE_EXPORTED
          /* skip static_init routines.  */
          && !DECL_ARTIFICIAL (decl))
        {
          promote_static_var_func (cgraph_get_module_id (cnode->decl), decl, 0);
          cgraph_mark_if_needed (decl);
        }
    }
}

/* Process var_decls, func_decls with static storage.  */

void
cgraph_process_module_scope_statics (void)
{
  struct cgraph_node *pf;
  struct varpool_node *pv;

  if (!L_IPO_COMP_MODE)
    return;

  promo_ent_hash_tab = htab_create (10, promo_ent_hash,
                                    promo_ent_eq, promo_ent_del);

  /* Process variable first.  */
  /* Keep track of weak variables emitted from the primary module (see
     comment for the process_module_scope_static_var function for why
     this needs to be done). We use the get_name_seq_name function for
     this.  */
  for (pv = varpool_nodes_queue; pv; pv = pv->next_needed)
    if (!varpool_is_auxiliary (pv) && DECL_WEAK (pv->decl) && !DECL_EXTERNAL (pv->decl))
      get_name_seq_num (IDENTIFIER_POINTER (DECL_NAME (pv->decl)));
  for (pv = varpool_nodes_queue; pv; pv = pv->next_needed)
    process_module_scope_static_var (pv);

  for (pf = cgraph_nodes; pf; pf = pf->next)
    process_module_scope_static_func (pf);

  htab_delete (promo_ent_hash_tab);
}

static GTY((param_is (struct varpool_node))) htab_t varpool_symtab;

/* Hash function for varpool node.  */

static hashval_t
hash_node_by_assembler_name (const void *p)
{
  const struct varpool_node *n = (const struct varpool_node *) p;
  return (hashval_t) decl_assembler_name_hash (DECL_ASSEMBLER_NAME (n->decl));
}

/* Returns nonzero if P1 and P2 are equal.  */

static int
eq_node_assembler_name (const void *p1, const void *p2)
{
  const struct varpool_node *n1 = (const struct varpool_node *) p1;
  const_tree name = (const_tree)p2;
  return (decl_assembler_name_equal (n1->decl, name));
}

/* Return true if NODE's decl is declared in an auxiliary module.  */

bool
varpool_is_auxiliary (struct varpool_node *node)
{
  return (node->module_id
          && node->module_id != primary_module_id);
}

/* Return the varpool_node to which DECL is resolved to during linking.
   This method can not be used after static to global promotion happens.  */

struct varpool_node *
real_varpool_node (tree decl)
{
  void **slot;
  tree name;

  if (!L_IPO_COMP_MODE || !varpool_symtab)
    return varpool_node (decl);

  if (!TREE_PUBLIC (decl) || DECL_ARTIFICIAL (decl))
    return varpool_node (decl);

  name = DECL_ASSEMBLER_NAME (decl);
  slot = htab_find_slot_with_hash (varpool_symtab, name,
                                   decl_assembler_name_hash (name),
                                   NO_INSERT);
  gcc_assert (slot && *slot);
  return (struct varpool_node *)*slot;
}

/* Remove NODE from the link table.  */

void
varpool_remove_link_node (struct varpool_node *node)
{
  tree name;
  tree decl;

  if (!L_IPO_COMP_MODE || !varpool_symtab)
    return;

  decl = node->decl;

  if (!TREE_PUBLIC (decl) || DECL_ARTIFICIAL (decl))
    return;

  if (real_varpool_node (decl) != node)
    return;

  name = DECL_ASSEMBLER_NAME (decl);
  htab_remove_elt_with_hash (varpool_symtab, name,
                             decl_assembler_name_hash (name));
}

/* Merge the addressable attribute from DECL2 to DECL1.  */

static inline void
merge_addressable_attr (tree decl1, tree decl2)
{
  if (TREE_ADDRESSABLE (decl2))
    TREE_ADDRESSABLE (decl1) = 1;
}

/* Resolve NODE->decl to symbol table entry *SLOT.  */

static void
resolve_varpool_node (struct varpool_node **slot, struct varpool_node *node)
{
  tree decl1, decl2;

  decl1 = (*slot)->decl;
  decl2 = node->decl;

  /* Take the decl with the complete type. */
  if (COMPLETE_TYPE_P (TREE_TYPE (decl1))
      && !COMPLETE_TYPE_P (TREE_TYPE (decl2)))
    {
      merge_addressable_attr (decl1, decl2);
      return;
    }
  if (!COMPLETE_TYPE_P (TREE_TYPE (decl1))
      && COMPLETE_TYPE_P (TREE_TYPE (decl2)))
    {
      *slot = node;
      merge_addressable_attr (decl2, decl1);
      return;
    }

  /* Either all complete or neither's type is complete. Just
     pick the primary module's decl.  */
  if (!varpool_is_auxiliary (*slot))
    {
      merge_addressable_attr (decl1, decl2);
      return;
    }

  if (!varpool_is_auxiliary (node))
    {
      *slot = node;
      merge_addressable_attr (decl2, decl1);
      return;
    }

  merge_addressable_attr (decl1, decl2);
  return;
}

/* Link NODE into var_decl symbol table.  */

void
varpool_link_node (struct varpool_node *node)
{
  tree name;
  void **slot;

  if (!L_IPO_COMP_MODE || !varpool_symtab)
    return;

  if (!TREE_PUBLIC (node->decl) || DECL_ARTIFICIAL (node->decl))
    return;

  name = DECL_ASSEMBLER_NAME (node->decl);
  slot = htab_find_slot_with_hash (varpool_symtab, name,
                                   decl_assembler_name_hash (name),
                                   INSERT);
  if (*slot)
    resolve_varpool_node ((struct varpool_node **) slot, node);
  else
    *slot = node;
}

/* Perform cross module linking for var_decls.  */

void
varpool_do_link (void)
{
  struct varpool_node *node;

  if (!L_IPO_COMP_MODE)
    return;

  varpool_symtab
      = htab_create_ggc (10, hash_node_by_assembler_name,
                         eq_node_assembler_name, NULL);
  for (node = varpool_nodes; node; node = node->next)
    varpool_link_node (node);
}

/* Get the list of assembler name ids with reference bit set.  */

void
varpool_get_referenced_asm_ids (VEC(tree, gc) ** ids)
{
  struct varpool_node *node;
  for (node = varpool_nodes; node; node = node->next)
    {
      tree asm_id = NULL;
      tree decl = node->decl;
      if (DECL_ASSEMBLER_NAME_SET_P (decl))
        {
          asm_id = DECL_ASSEMBLER_NAME (decl);
          VEC_safe_push (tree, gc, *ids, asm_id);
        }
    }
}

/* Clear the referenced bit in all assembler ids.  */

void
varpool_clear_asm_id_reference_bit (void)
{
  struct varpool_node *node;
  for (node = varpool_nodes; node; node = node->next)
    {
      tree asm_id = NULL;
      tree decl = node->decl;
      if (DECL_ASSEMBLER_NAME_SET_P (decl))
        {
          asm_id = DECL_ASSEMBLER_NAME (decl);
          TREE_SYMBOL_REFERENCED (asm_id) = 0;
        }
    }
}


#include "gt-l-ipo.h"
