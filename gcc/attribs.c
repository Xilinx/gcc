/* Functions dealing with attribute handling, used by most front ends.
   Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001,
   2002, 2003, 2004, 2005, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

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
#include "flags.h"
#include "diagnostic-core.h"
#include "ggc.h"
#include "tm_p.h"
#include "cpplib.h"
#include "target.h"
#include "langhooks.h"
#include "hashtab.h"
#include "plugin.h"

static void init_attributes (void);
static void merge_lock_attr_args (tree, tree);

/* Table of the tables of attributes (common, language, format, machine)
   searched.  */
static const struct attribute_spec *attribute_tables[4];

/* Hashtable mapping names (represented as substrings) to attribute specs. */
static htab_t attribute_hash;

/* Substring representation.  */

struct substring
{
  const char *str;
  int length;
};

static bool attributes_initialized = false;

/* Default empty table of attributes.  */

static const struct attribute_spec empty_attribute_table[] =
{
  { NULL, 0, 0, false, false, false, NULL }
};

/* Return base name of the attribute.  Ie '__attr__' is turned into 'attr'.
   To avoid need for copying, we simply return length of the string.  */

static void
extract_attribute_substring (struct substring *str)
{
  if (str->length > 4 && str->str[0] == '_' && str->str[1] == '_'
      && str->str[str->length - 1] == '_' && str->str[str->length - 2] == '_')
    {
      str->length -= 4;
      str->str += 2;
    }
}

/* Simple hash function to avoid need to scan whole string.  */

static inline hashval_t
substring_hash (const char *str, int l)
{
  return str[0] + str[l - 1] * 256 + l * 65536;
}

/* Used for attribute_hash.  */

static hashval_t
hash_attr (const void *p)
{
  const struct attribute_spec *const spec = (const struct attribute_spec *) p;
  const int l = strlen (spec->name);

  return substring_hash (spec->name, l);
}

/* Used for attribute_hash.  */

static int
eq_attr (const void *p, const void *q)
{
  const struct attribute_spec *const spec = (const struct attribute_spec *) p;
  const struct substring *const str = (const struct substring *) q;

  return (!strncmp (spec->name, str->str, str->length) && !spec->name[str->length]);
}

/* Initialize attribute tables, and make some sanity checks
   if --enable-checking.  */

static void
init_attributes (void)
{
  size_t i;
  int k;

  attribute_tables[0] = lang_hooks.common_attribute_table;
  attribute_tables[1] = lang_hooks.attribute_table;
  attribute_tables[2] = lang_hooks.format_attribute_table;
  attribute_tables[3] = targetm.attribute_table;

  /* Translate NULL pointers to pointers to the empty table.  */
  for (i = 0; i < ARRAY_SIZE (attribute_tables); i++)
    if (attribute_tables[i] == NULL)
      attribute_tables[i] = empty_attribute_table;

#ifdef ENABLE_CHECKING
  /* Make some sanity checks on the attribute tables.  */
  for (i = 0; i < ARRAY_SIZE (attribute_tables); i++)
    {
      int j;

      for (j = 0; attribute_tables[i][j].name != NULL; j++)
	{
	  /* The name must not begin and end with __.  */
	  const char *name = attribute_tables[i][j].name;
	  int len = strlen (name);

	  gcc_assert (!(name[0] == '_' && name[1] == '_'
			&& name[len - 1] == '_' && name[len - 2] == '_'));

	  /* The minimum and maximum lengths must be consistent.  */
	  gcc_assert (attribute_tables[i][j].min_length >= 0);

	  gcc_assert (attribute_tables[i][j].max_length == -1
		      || (attribute_tables[i][j].max_length
			  >= attribute_tables[i][j].min_length));

	  /* An attribute cannot require both a DECL and a TYPE.  */
	  gcc_assert (!attribute_tables[i][j].decl_required
		      || !attribute_tables[i][j].type_required);

	  /* If an attribute requires a function type, in particular
	     it requires a type.  */
	  gcc_assert (!attribute_tables[i][j].function_type_required
		      || attribute_tables[i][j].type_required);
	}
    }

  /* Check that each name occurs just once in each table.  */
  for (i = 0; i < ARRAY_SIZE (attribute_tables); i++)
    {
      int j, k;
      for (j = 0; attribute_tables[i][j].name != NULL; j++)
	for (k = j + 1; attribute_tables[i][k].name != NULL; k++)
	  gcc_assert (strcmp (attribute_tables[i][j].name,
			      attribute_tables[i][k].name));
    }
  /* Check that no name occurs in more than one table.  */
  for (i = 0; i < ARRAY_SIZE (attribute_tables); i++)
    {
      size_t j, k, l;

      for (j = i + 1; j < ARRAY_SIZE (attribute_tables); j++)
	for (k = 0; attribute_tables[i][k].name != NULL; k++)
	  for (l = 0; attribute_tables[j][l].name != NULL; l++)
	    gcc_assert (strcmp (attribute_tables[i][k].name,
				attribute_tables[j][l].name));
    }
#endif

  attribute_hash = htab_create (200, hash_attr, eq_attr, NULL);
  for (i = 0; i < ARRAY_SIZE (attribute_tables); i++)
    for (k = 0; attribute_tables[i][k].name != NULL; k++)
      {
        register_attribute (&attribute_tables[i][k]);
      }
  invoke_plugin_callbacks (PLUGIN_ATTRIBUTES, NULL);
  attributes_initialized = true;
}

/* Insert a single ATTR into the attribute table.  */

void
register_attribute (const struct attribute_spec *attr)
{
  struct substring str;
  void **slot;

  str.str = attr->name;
  str.length = strlen (str.str);
  slot = htab_find_slot_with_hash (attribute_hash, &str,
				   substring_hash (str.str, str.length),
				   INSERT);
  gcc_assert (!*slot);
  *slot = (void *) CONST_CAST (struct attribute_spec *, attr);
}

/* Return the spec for the attribute named NAME.  */

const struct attribute_spec *
lookup_attribute_spec (const_tree name)
{
  struct substring attr;

  attr.str = IDENTIFIER_POINTER (name);
  attr.length = IDENTIFIER_LENGTH (name);
  extract_attribute_substring (&attr);
  return (const struct attribute_spec *)
    htab_find_with_hash (attribute_hash, &attr,
			 substring_hash (attr.str, attr.length));
}

/* Process the attributes listed in ATTRIBUTES and install them in *NODE,
   which is either a DECL (including a TYPE_DECL) or a TYPE.  If a DECL,
   it should be modified in place; if a TYPE, a copy should be created
   unless ATTR_FLAG_TYPE_IN_PLACE is set in FLAGS.  FLAGS gives further
   information, in the form of a bitwise OR of flags in enum attribute_flags
   from tree.h.  Depending on these flags, some attributes may be
   returned to be applied at a later stage (for example, to apply
   a decl attribute to the declaration rather than to its type).  */

tree
decl_attributes (tree *node, tree attributes, int flags)
{
  tree a;
  tree returned_attrs = NULL_TREE;

  if (TREE_TYPE (*node) == error_mark_node)
    return NULL_TREE;

  if (!attributes_initialized)
    init_attributes ();

  /* If this is a function and the user used #pragma GCC optimize, add the
     options to the attribute((optimize(...))) list.  */
  if (TREE_CODE (*node) == FUNCTION_DECL && current_optimize_pragma)
    {
      tree cur_attr = lookup_attribute ("optimize", attributes);
      tree opts = copy_list (current_optimize_pragma);

      if (! cur_attr)
	attributes
	  = tree_cons (get_identifier ("optimize"), opts, attributes);
      else
	TREE_VALUE (cur_attr) = chainon (opts, TREE_VALUE (cur_attr));
    }

  if (TREE_CODE (*node) == FUNCTION_DECL
      && optimization_current_node != optimization_default_node
      && !DECL_FUNCTION_SPECIFIC_OPTIMIZATION (*node))
    DECL_FUNCTION_SPECIFIC_OPTIMIZATION (*node) = optimization_current_node;

  /* If this is a function and the user used #pragma GCC target, add the
     options to the attribute((target(...))) list.  */
  if (TREE_CODE (*node) == FUNCTION_DECL
      && current_target_pragma
      && targetm.target_option.valid_attribute_p (*node, NULL_TREE,
						  current_target_pragma, 0))
    {
      tree cur_attr = lookup_attribute ("target", attributes);
      tree opts = copy_list (current_target_pragma);

      if (! cur_attr)
	attributes = tree_cons (get_identifier ("target"), opts, attributes);
      else
	TREE_VALUE (cur_attr) = chainon (opts, TREE_VALUE (cur_attr));
    }

  /* A "naked" function attribute implies "noinline" and "noclone" for
     those targets that support it.  */
  if (TREE_CODE (*node) == FUNCTION_DECL
      && lookup_attribute_spec (get_identifier ("naked"))
      && lookup_attribute ("naked", attributes) != NULL)
    {
      if (lookup_attribute ("noinline", attributes) == NULL)
	attributes = tree_cons (get_identifier ("noinline"), NULL, attributes);

      if (lookup_attribute ("noclone", attributes) == NULL)
	attributes = tree_cons (get_identifier ("noclone"),  NULL, attributes);
    }

  targetm.insert_attributes (*node, &attributes);

  for (a = attributes; a; a = TREE_CHAIN (a))
    {
      tree name = TREE_PURPOSE (a);
      tree args = TREE_VALUE (a);
      tree *anode = node;
      const struct attribute_spec *spec = lookup_attribute_spec (name);
      bool no_add_attrs = 0;
      int fn_ptr_quals = 0;
      tree fn_ptr_tmp = NULL_TREE;

      if (spec == NULL)
	{
	  warning (OPT_Wattributes, "%qE attribute directive ignored",
		   name);
	  continue;
	}
      else if (list_length (args) < spec->min_length
	       || (spec->max_length >= 0
		   && list_length (args) > spec->max_length))
	{
	  error ("wrong number of arguments specified for %qE attribute",
		 name);
	  continue;
	}
      gcc_assert (is_attribute_p (spec->name, name));

      /* If this is a lock attribute and the purpose field of the args is
         an error_mark_node, the attribute arguments have not been parsed yet
         (as we delay the parsing of the attribute arguments until after the
         whole class has been parsed). So don't handle this attribute now
         but simply replace the error_mark_node with the current decl node
         (which we will need when we call this routine again later).  */
      if (args
          && TREE_PURPOSE (args) == error_mark_node
          && is_lock_attribute_with_args (name))
        {
          TREE_PURPOSE (args) = *node;
          continue;
        }

      if (spec->decl_required && !DECL_P (*anode))
	{
	  if (flags & ((int) ATTR_FLAG_DECL_NEXT
		       | (int) ATTR_FLAG_FUNCTION_NEXT
		       | (int) ATTR_FLAG_ARRAY_NEXT))
	    {
	      /* Pass on this attribute to be tried again.  */
	      returned_attrs = tree_cons (name, args, returned_attrs);
	      continue;
	    }
	  else
	    {
	      warning (OPT_Wattributes, "%qE attribute does not apply to types",
		       name);
	      continue;
	    }
	}

      /* If we require a type, but were passed a decl, set up to make a
	 new type and update the one in the decl.  ATTR_FLAG_TYPE_IN_PLACE
	 would have applied if we'd been passed a type, but we cannot modify
	 the decl's type in place here.  */
      if (spec->type_required && DECL_P (*anode))
	{
	  anode = &TREE_TYPE (*anode);
	  /* Allow ATTR_FLAG_TYPE_IN_PLACE for the type's naming decl.  */
	  if (!(TREE_CODE (*anode) == TYPE_DECL
		&& *anode == TYPE_NAME (TYPE_MAIN_VARIANT
					(TREE_TYPE (*anode)))))
	    flags &= ~(int) ATTR_FLAG_TYPE_IN_PLACE;
	}

      if (spec->function_type_required && TREE_CODE (*anode) != FUNCTION_TYPE
	  && TREE_CODE (*anode) != METHOD_TYPE)
	{
	  if (TREE_CODE (*anode) == POINTER_TYPE
	      && (TREE_CODE (TREE_TYPE (*anode)) == FUNCTION_TYPE
		  || TREE_CODE (TREE_TYPE (*anode)) == METHOD_TYPE))
	    {
	      /* OK, this is a bit convoluted.  We can't just make a copy
		 of the pointer type and modify its TREE_TYPE, because if
		 we change the attributes of the target type the pointer
		 type needs to have a different TYPE_MAIN_VARIANT.  So we
		 pull out the target type now, frob it as appropriate, and
		 rebuild the pointer type later.

		 This would all be simpler if attributes were part of the
		 declarator, grumble grumble.  */
	      fn_ptr_tmp = TREE_TYPE (*anode);
	      fn_ptr_quals = TYPE_QUALS (*anode);
	      anode = &fn_ptr_tmp;
	      flags &= ~(int) ATTR_FLAG_TYPE_IN_PLACE;
	    }
	  else if (flags & (int) ATTR_FLAG_FUNCTION_NEXT)
	    {
	      /* Pass on this attribute to be tried again.  */
	      returned_attrs = tree_cons (name, args, returned_attrs);
	      continue;
	    }

	  if (TREE_CODE (*anode) != FUNCTION_TYPE
	      && TREE_CODE (*anode) != METHOD_TYPE)
	    {
	      warning (OPT_Wattributes,
		       "%qE attribute only applies to function types",
		       name);
	      continue;
	    }
	}

      if (TYPE_P (*anode)
	  && (flags & (int) ATTR_FLAG_TYPE_IN_PLACE)
	  && TYPE_SIZE (*anode) != NULL_TREE)
	{
	  warning (OPT_Wattributes, "type attributes ignored after type is already defined");
	  continue;
	}

      if (spec->handler != NULL)
        {
          tree ret_attr = (*spec->handler) (anode, name, args,
                                            flags, &no_add_attrs);
          if (ret_attr)
            {
              /* For the lock attributes whose arguments (i.e. locks) are not
                 supported or the names are not in scope, we would demote the
                 attributes. For example, if 'foo' is not in scope in the
                 attribute "guarded_by(foo->lock), the attribute would be
                 downgraded to a "guarded" attribute. And in this case, the
                 handler would return the new, demoted attribute which is
                 appended to the current one so that it is handled in the next
                 iteration.  */
              if (is_lock_attribute_with_args (name))
                {
                  gcc_assert (no_add_attrs);
                  TREE_CHAIN (ret_attr) = TREE_CHAIN (a);
                  TREE_CHAIN (a) = ret_attr;
                  continue;
                }
              else
                returned_attrs = chainon (ret_attr, returned_attrs);
            }
        }

      /* Layout the decl in case anything changed.  */
      if (spec->type_required && DECL_P (*node)
	  && (TREE_CODE (*node) == VAR_DECL
	      || TREE_CODE (*node) == PARM_DECL
	      || TREE_CODE (*node) == RESULT_DECL))
	relayout_decl (*node);

      if (!no_add_attrs)
	{
	  tree old_attrs;
	  tree a;

	  if (DECL_P (*anode))
	    old_attrs = DECL_ATTRIBUTES (*anode);
	  else
	    old_attrs = TYPE_ATTRIBUTES (*anode);

	  for (a = lookup_attribute (spec->name, old_attrs);
	       a != NULL_TREE;
	       a = lookup_attribute (spec->name, TREE_CHAIN (a)))
	    {
	      if (simple_cst_equal (TREE_VALUE (a), args) == 1)
		break;
              /* If a lock attribute of the same kind is already on the decl,
                 don't add this one again. Instead, merge the arguments.  */
              if (is_lock_attribute_with_args (name))
                {
                  merge_lock_attr_args (a, args);
                  break;
                }
	    }

	  if (a == NULL_TREE)
	    {
	      /* This attribute isn't already in the list.  */
	      if (DECL_P (*anode))
		DECL_ATTRIBUTES (*anode) = tree_cons (name, args, old_attrs);
	      else if (flags & (int) ATTR_FLAG_TYPE_IN_PLACE)
		{
		  TYPE_ATTRIBUTES (*anode) = tree_cons (name, args, old_attrs);
		  /* If this is the main variant, also push the attributes
		     out to the other variants.  */
		  if (*anode == TYPE_MAIN_VARIANT (*anode))
		    {
		      tree variant;
		      for (variant = *anode; variant;
			   variant = TYPE_NEXT_VARIANT (variant))
			{
			  if (TYPE_ATTRIBUTES (variant) == old_attrs)
			    TYPE_ATTRIBUTES (variant)
			      = TYPE_ATTRIBUTES (*anode);
			  else if (!lookup_attribute
				   (spec->name, TYPE_ATTRIBUTES (variant)))
			    TYPE_ATTRIBUTES (variant) = tree_cons
			      (name, args, TYPE_ATTRIBUTES (variant));
			}
		    }
		}
	      else
		*anode = build_type_attribute_variant (*anode,
						       tree_cons (name, args,
								  old_attrs));
	    }
	}

      if (fn_ptr_tmp)
	{
	  /* Rebuild the function pointer type and put it in the
	     appropriate place.  */
	  fn_ptr_tmp = build_pointer_type (fn_ptr_tmp);
	  if (fn_ptr_quals)
	    fn_ptr_tmp = build_qualified_type (fn_ptr_tmp, fn_ptr_quals);
	  if (DECL_P (*node))
	    TREE_TYPE (*node) = fn_ptr_tmp;
	  else
	    {
	      gcc_assert (TREE_CODE (*node) == POINTER_TYPE);
	      *node = fn_ptr_tmp;
	    }
	}
    }

  return returned_attrs;
}

/* Return true if IDENTIFIER is the name of a lock attribute that takes
   arguments, as listed in the if-statement below.  */

bool
is_lock_attribute_with_args (const_tree identifier)
{
  gcc_assert (TREE_CODE (identifier) == IDENTIFIER_NODE);

  if (is_attribute_p ("guarded_by", identifier)
      || is_attribute_p ("point_to_guarded_by", identifier)
      || is_attribute_p ("acquired_after", identifier)
      || is_attribute_p ("acquired_before", identifier)
      || is_attribute_p ("exclusive_lock", identifier)
      || is_attribute_p ("shared_lock", identifier)
      || is_attribute_p ("exclusive_trylock", identifier)
      || is_attribute_p ("shared_trylock", identifier)
      || is_attribute_p ("unlock", identifier)
      || is_attribute_p ("exclusive_locks_required", identifier)
      || is_attribute_p ("shared_locks_required", identifier)
      || is_attribute_p ("locks_excluded", identifier)
      || is_attribute_p ("lock_returned", identifier))
    return true;
  else
    return false;
}

/* Return true if IDENTIFIER is the name of a lock attribute.  */

static bool
is_lock_attribute_p (const_tree identifier)
{
  gcc_assert (TREE_CODE (identifier) == IDENTIFIER_NODE);

  if (is_lock_attribute_with_args (identifier)
      || is_attribute_p ("no_thread_safety_analysis", identifier)
      || is_attribute_p ("ignore_reads_begin", identifier)
      || is_attribute_p ("ignore_reads_end", identifier)
      || is_attribute_p ("ignore_writes_begin", identifier)
      || is_attribute_p ("ignore_writes_end", identifier)
      || is_attribute_p ("unprotected_read", identifier)
      || is_attribute_p ("guarded", identifier)
      || is_attribute_p ("point_to_guarded", identifier)
      || is_attribute_p ("lockable", identifier)
      || is_attribute_p ("scoped_lockable", identifier))
    return true;
  else
    return false;
}

/* Extract and return all lock attributes from the given ATTRS list.
   Note that the ATTRS list could be damaged if there is any lock attribute
   in the list so you should not call this function if you expect ATTRS to
   be intact. For example, here is the given ATTRS list:

     attr("locks_excluded") -> attr("pure") -> attr("shared_locks_required")

   This function will return the following attribute list

     attr("shared_locks_required") -> attr("locks_excluded")  */

tree
extract_lock_attributes (tree attrs)
{
  tree lock_attrs = NULL_TREE;
  tree next;

  for ( ; attrs; attrs = next)
    {
      next = TREE_CHAIN (attrs);
      if (is_lock_attribute_p (TREE_PURPOSE (attrs)))
        {
          TREE_CHAIN (attrs) = lock_attrs;
          lock_attrs = attrs;
        }
    }

  return lock_attrs;
}

/* This helper function is called when we see multiple lock attributes of
   the same kind on a decl. ATTR is the first attribute of this kind we've
   encountered and ADDITIONAL_ARGS is the args list of another attribute
   of this kind. This function appends ADDITIONAL_ARGS to the args list
   of ATTR. Note that we don't allow some of the lock attributes to appear
   multiple times on a decl (such as 'guarded_by') and would emit a warning
   if that happens.  */

static void
merge_lock_attr_args (tree attr, tree additional_args)
{
  tree identifier = TREE_PURPOSE (attr);

  if (is_attribute_p ("acquired_after", identifier)
      || is_attribute_p ("acquired_before", identifier)
      || is_attribute_p ("exclusive_lock", identifier)
      || is_attribute_p ("shared_lock", identifier)
      || is_attribute_p ("exclusive_trylock", identifier)
      || is_attribute_p ("shared_trylock", identifier)
      || is_attribute_p ("unlock", identifier)
      || is_attribute_p ("exclusive_locks_required", identifier)
      || is_attribute_p ("shared_locks_required", identifier)
      || is_attribute_p ("locks_excluded", identifier))
    TREE_VALUE (attr) = chainon (TREE_VALUE (attr), additional_args);
  /* We don't allow the following lock attributes to appear multiple times
     on a decl.  */
  else if (is_attribute_p ("guarded_by", identifier)
           || is_attribute_p ("point_to_guarded_by", identifier)
           || is_attribute_p ("lock_returned", identifier))
    warning (OPT_Wattributes, "Additional %qs attribute ignored",
             IDENTIFIER_POINTER (identifier));
}
