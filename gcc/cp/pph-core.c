/* Factored pre-parsed header (PPH) support for C++.
   Common routines for streaming PPH data.

   Copyright (C) 2012 Free Software Foundation, Inc.
   Contributed by Lawrence Crowl <crowl@google.com> and
   Diego Novillo <dnovillo@google.com>.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */


#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "pph.h"
#include "cpplib.h"
#include "toplev.h"
#include "tree.h"
#include "cp-tree.h"
#include "langhooks.h"
#include "timevar.h"
#include "tree-iterator.h"
#include "tree-pretty-print.h"
#include "lto-streamer.h"
#include "pph-streamer.h"
#include "pointer-set.h"
#include "fixed-value.h"
#include "md5.h"
#include "tree-pass.h"
#include "tree-dump.h"
#include "tree-inline.h"
#include "tree-pretty-print.h"
#include "cxx-pretty-print.h"
#include "parser.h"
#include "version.h"
#include "cppbuiltin.h"
#include "streamer-hooks.h"


/* Mapping between a name string and the registry index for the
   corresponding PPH image.  */
struct pph_name_stream_map {
  const char *name;
  unsigned ix;
};

/* Registry of all the PPH images opened during this compilation.
   This registry is used in three different ways:

    1- To associate header pathnames (HEADER_NAME field in
       pph_stream) and images.  Used for looking up PPH streams from
       header names (e.g., pph_out_line_table_and_includes).

    2- To associate PPH file names (NAME field in pph_stream) and
       images.  Used for deciding whether a PPH file has been
       opened already (e.g., pph_stream_open).

    3- To look for symbols and types in the pickle cache of opened
       PPH streams (e.g., pph_cache_lookup_in_includes).

   This registry is needed because images opened during #include
   processing and opened from pph_in_includes cannot be closed
   immediately after reading, because the pickle cache contained in
   them may be referenced from other images.  We delay closing all of
   them until the end of parsing (when pph_streamer_finish is called).  */
struct pph_stream_registry_d {
  /* Map between names and images.  */
  htab_t name_ix;

  /* Index of all registered PPH images.  */
  struct pointer_map_t *image_ix;

  /* List of registered PPH images.  */
  VEC(pph_stream_ptr, heap) *v;
};

static struct pph_stream_registry_d pph_stream_registry;


/* List of all the trees added to PPH pickle caches.

   FIXME pph, the *only* purpose of this list is to act as a GC root
   to avoid these trees from being garbage collected.

   The parser will invoke garbage collection, which may clobber some
   trees in the pickle caches (because the pickle caches are stored on
   the heap).  Ideally, we would put all these data structures on GC
   memory.  */
static GTY(()) VEC(tree,gc) *pph_cached_trees;



/*************************************************************** pph logging */


/* Log file where PPH analysis is written to.  Controlled by
   -fpph_logfile.  If this flag is not given, stdout is used.  */
FILE *pph_logfile = NULL;


/* Convert a checked tree_code CODE to a string.  */

const char *
pph_tree_code_text (enum tree_code code)
{
  gcc_assert (code < MAX_TREE_CODES);
  return tree_code_name[code];
}


/* Dump a location LOC to FILE.  */

void
pph_dump_location (FILE *file, location_t loc)
{
  expanded_location xloc = expand_location (loc);
  fprintf (file, "%s:%d", xloc.file, xloc.line);
}


/* Dump a complicated name for tree T to FILE using FLAGS.
   See TDF_* in tree-pass.h for flags.  */

void
pph_dump_tree_name (FILE *file, tree t, int flags)
{
  enum tree_code code = TREE_CODE (t);
  const char *text = pph_tree_code_text (code);
  fprintf (file, "%p %s ", (void *)t, text);
  if (DECL_P (t))
    fprintf (file, "%s\n", decl_as_string (t, flags));
  else if (TYPE_P (t))
    fprintf (file, "%s\n", type_as_string (t, flags));
  else if (EXPR_P (t))
    fprintf (file, "%s\n", expr_as_string (t, flags));
  else
    {
      print_generic_expr (file, t, flags);
      fprintf (file, "\n");
    }
}


/* Dump a list of overloaded function names to FILE starting with T
   using FLAGS.  */

static void
pph_dump_overload_names (FILE *file, tree t, int flags)
{
  for (; t; t = OVL_NEXT (t))
    {
      tree u = OVL_CURRENT (t);
      enum tree_code code = TREE_CODE (t);
      const char *text = pph_tree_code_text (code);
      fprintf (file, "  binding value: ");
      fprintf (file, "%p %s ", (void *)t, text);
      pph_dump_tree_name (file, u, flags);
    }
}


/* Dump one cxx_binding B to FILE.  */

static void
pph_dump_one_binding (FILE *file, cxx_binding *b)
{
  if (b->scope)
    {
      fprintf (file, "  binding scope: ");
      pph_dump_tree_name (file, b->scope->this_entity, 0);
    }
  if (b->value)
    {
      if (TREE_CODE (b->value) == OVERLOAD)
	pph_dump_overload_names (file, b->value, 0);
      else
	{
	  fprintf (file, "  binding value: ");
	  pph_dump_tree_name (file, b->value, 0);
	}
    }
  if (b->type)
    {
      fprintf (file, "  binding type: ");
      pph_dump_tree_name (file, b->type, 0);
    }
  fprintf (file, "  binding is_local: %d\n", b->is_local);
  fprintf (file, "  binding value_is_inherited: %d\n", b->value_is_inherited);
}


/* Dump all the cxx_bindings for and identifier ID to FILE.  */

static void
pph_dump_bindings_for_id (FILE *file, tree id)
{
  cxx_binding *b = IDENTIFIER_NAMESPACE_BINDINGS (id);
  if (b)
    for (; b != NULL; b = b->previous)
      pph_dump_one_binding (file, b);
  else
    fprintf (file, "  binding: NO BINDING\n");
}


/* Dump all the cxx_bindings for and identifier ID to FILE.  */

static void
pph_dump_bindings_for_decl (FILE *file, tree decl)
{
  /* FIXME pph: The declarations often have the same identifier, and
  hence the same bbindings.  The output is hence redundant.  We should
  probably just collect the ids an print them in a separate table. */

  tree id = DECL_NAME (decl);
  if (id)
    pph_dump_bindings_for_id (file, id);
  else
    fprintf (file, "  binding: NO NAME\n" );
}


/* Forward declaration.  */

static void
pph_dump_binding (FILE *file, cp_binding_level *level);


/* Dump namespace NS to FILE.  */

static void
pph_dump_namespace (FILE *file, tree ns)
{
  fprintf (file, "namespace ");
  pph_dump_tree_name (file, ns, 0);
  fprintf (file, "{\n");
  pph_dump_binding (file, NAMESPACE_LEVEL (ns));
  fprintf (file, "}\n");
}


/* Dump a CHAIN for PPH.  */

static void
pph_dump_chain (FILE *file, tree chain)
{
  tree t;
  for (t = chain; t; t = DECL_CHAIN (t))
    {
      if (!DECL_IS_BUILTIN (t) || flag_pph_debug >= 5)
	{
	  pph_dump_tree_name (file, t, 0);
	  pph_dump_bindings_for_decl (file, t);
	}
    }
}


/* Dump cp_binding_level LEVEL for PPH.  */

static void
pph_dump_binding (FILE *file, cp_binding_level *level)
{
  tree t;
  pph_dump_chain (file, level->names);
  for (t = level->namespaces; t; t = DECL_CHAIN (t))
    {
      if (!DECL_IS_BUILTIN (t) || flag_pph_debug >= 5)
        pph_dump_namespace (file, t);
    }
}


/* Dump a tree vec v for PPH.  */

void
pph_dump_vec_tree (FILE *file, VEC(tree,gc) *v)
{
  unsigned i;
  tree t;
  FOR_EACH_VEC_ELT (tree, v, i, t)
    pph_dump_tree_name (file, t, 0);
}



/* Dump global symbol information for PPH.  */

void
pph_dump_global_state (FILE *file, const char *msg)
{
  fprintf (file, "\nPPH: BEGIN GLOBAL STATE ");
  if (msg)
    fprintf (file, "%s", msg);
  fprintf (file, "\n");
  pph_dump_namespace (file, global_namespace);
  fprintf (file, "\nPPH: static_aggregates\n");
  pph_dump_chain (file, static_aggregates);
  fprintf (file, "\nPPH: keyed_classes\n");
  pph_dump_chain (file, keyed_classes);
  fprintf (file, "\nPPH: unemitted_tinfo_decls\n");
  pph_dump_vec_tree (file, unemitted_tinfo_decls);
  fprintf (file, "\nPPH: END GLOBAL STATE\n\n");
  pph_dump_decl2_hidden_state (file);
}


/* Trace a record MARKER and TAG.  */

static const char *marker_strings[] =
{
  "PPH_RECORD_START",
  "PPH_RECORD_START_NO_CACHE",
  "PPH_RECORD_START_MUTATED",
  "PPH_RECORD_START_MERGE_KEY",
  "PPH_RECORD_START_MERGE_BODY",
  "PPH_RECORD_END",
  "PPH_RECORD_IREF",
  "PPH_RECORD_XREF",
  "PPH_RECORD_PREF"
};

static const char *tag_strings[] =
{
  "PPH_any_tree",
  "PPH_binding_entry",
  "PPH_binding_table",
  "PPH_cp_binding_level",
  "PPH_cp_class_binding",
  "PPH_cp_label_binding",
  "PPH_cxx_binding",
  "PPH_function",
  "PPH_lang_decl",
  "PPH_lang_type",
  "PPH_language_function",
  "PPH_sorted_fields_type"
};


/* Trace print a MARKER and TAG.  */


void
pph_trace_marker (enum pph_record_marker marker, enum pph_tag tag)
{
  fprintf (pph_logfile, "PPH: marker ");
  if (PPH_RECORD_START <= marker && marker <= PPH_RECORD_PREF)
    fprintf (pph_logfile, "%s", marker_strings[marker - PPH_RECORD_START]);
  else
    fprintf (pph_logfile, "unknown");
  fprintf (pph_logfile, " tag ");
  if (tag == PPH_null)
    fprintf (pph_logfile, "PPH_null\n");
  else if (tag < PPH_any_tree)
    fprintf (pph_logfile, "%s\n", tree_code_name[tag]);
  else if (tag < PPH_NUM_TAGS)
    fprintf (pph_logfile, "%s\n", tag_strings[tag - PPH_any_tree]);
  else
    fprintf (pph_logfile, "unknown\n");
}


/* Print tracing information for a possibly MERGEABLE tree T.  */

void
pph_trace_tree (tree t, const char *name,
		enum pph_trace_end end, enum pph_trace_kind kind)
{
  char end_char, kind_char, decl_char;
  bool is_merge, is_decl, is_type;
  bool emit = false;

  switch (kind)
    {
      case pph_trace_key_out:
	kind_char = 'K';
	is_merge = true;
	break;
      case pph_trace_unmerged_key:
	kind_char = 'U';
	is_merge = true;
	break;
      case pph_trace_merged_key:
	kind_char = 'M';
	is_merge = true;
	break;
      case pph_trace_merge_body:
	kind_char = 'B';
	is_merge = true;
	break;
      case pph_trace_mutate:
	kind_char = '=';
	is_merge = true;
	break;
      case pph_trace_normal:
	kind_char = '.';
	is_merge = false;
	break;
      default:
	kind_char = '?';
	is_merge = false;
    }

  end_char = end == pph_trace_front ? '{' : '}';

  is_decl = DECL_P (t);
  is_type = TYPE_P (t);
  if (is_decl)
    decl_char = 'D';
  else if (TYPE_P (t))
    decl_char = 'T';
  else
    decl_char = '.';

  if (is_merge && flag_pph_tracer >= 2)
    emit = true;
  else if ((is_merge || is_decl || is_type) && flag_pph_tracer >= 3)
    emit = true;
  else if (!EXPR_P (t) && flag_pph_tracer >= 4)
    emit = true;

  if (emit)
    {
      fprintf (pph_logfile, "PPH: %c%c%c ", end_char, kind_char, decl_char);
      if (name)
	fprintf (pph_logfile, "%p %s %s\n", (void*)t,
			      pph_tree_code_text (TREE_CODE (t)), name);
      else
        pph_dump_tree_name (pph_logfile, t, 0);
    }
}


/* Print a note into the tracing stream.  */

void
pph_trace_note (const char *type, const char *value)
{
  if ( flag_pph_tracer >= 3 )
    fprintf (pph_logfile, "PPH: %s: %s\n", type, value);
}


/************************************************* pph pointer mapping cache */


/* Initialize an empty pickle CACHE.  */

static void
pph_cache_init (pph_cache *cache)
{
  cache->v = NULL;
  cache->m = pointer_map_create ();
}


/* A cache of pre-loaded common tree nodes.  */
static pph_cache *pph_preloaded_cache;


/* Callback for cp_walk_tree.  Called from pph_cache_add_full_tree.
   Add the sub-tree *TP to the cache pointed to by DATA.  Always set
   WALK_SUBTREES to 1 to traverse every sub-tree.  */

static tree
pph_cache_add_full_tree_r (tree *tp, int *walk_subtrees, void *data)
{
  pph_cache *cache = (pph_cache *) data;
  pph_cache_add (cache, *tp, NULL, pph_tree_code_to_tag (*tp));
  *walk_subtrees = 1;
  return NULL;
}


/* Add tree T and all its children into CACHE.  */

static void
pph_cache_add_full_tree (pph_cache *cache, tree t)
{
  cp_walk_tree (&t, pph_cache_add_full_tree_r, cache, NULL);
}


/* Callback for traverse_nonstandard_integer_type_cache.  Add the
   full tree TYPE to the cache pointed by DATA.  */

static bool
nitc_callback (tree type, void *data)
{
  pph_cache *cache = (pph_cache *) data;
  pph_cache_add_full_tree (cache, type);
  return true;
}

/* Vector of builtin types to register in the preloaded cache.  */
static VEC(tree,gc) *pph_builtin_types;


/* Register a builtin type to be preloaded when we are setting up the
   pickle cache.  This is called from record_builtin_type.  */

void
pph_register_builtin_type (tree type)
{
  VEC_safe_push (tree, gc, pph_builtin_types, type);
}


/* Pre-load all the builtin types declared by the compiler.  */

static void
pph_cache_add_builtin_types (pph_cache *cache)
{
  unsigned i;
  tree type;

  FOR_EACH_VEC_ELT (tree, pph_builtin_types, i, type)
    pph_cache_add_full_tree (cache, type);
}


/* Callback for type_hash_table_traverse.  DATA points to the cache
   where we are preloading trees built by the front end on startup.
   TYPE is the type to preload.  Always return true, so we visit the
   whole table.  */

static bool
pph_cache_add_canonical_type (unsigned long h ATTRIBUTE_UNUSED, tree type,
			      void *data)
{
  pph_cache *cache = (pph_cache *) data;
  pph_cache_add_full_tree (cache, type);
  return true;
}


/* Pre-load common tree nodes into CACHE.  These nodes are always built by the
   front end, so there is no need to pickle them.  */

static void
pph_cache_preload (pph_cache *cache)
{
  unsigned i;

  for (i = itk_char; i < itk_none; i++)
    pph_cache_add_full_tree (cache, integer_types[i]);

  for (i = 0; i < TYPE_KIND_LAST; i++)
    pph_cache_add_full_tree (cache, sizetype_tab[i]);

  /* global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < TI_MAX; i++)
    if (global_trees[i])
      pph_cache_add_full_tree (cache, global_trees[i]);

  /* c_global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < CTI_MAX; i++)
    if (c_global_trees[i])
      pph_cache_add_full_tree (cache, c_global_trees[i]);

  /* cp_global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < CPTI_MAX; i++)
    {
      /* Also skip trees which are generated while parsing.  */
      if (i == CPTI_KEYED_CLASSES)
	continue;

      if (cp_global_trees[i])
	pph_cache_add_full_tree (cache, cp_global_trees[i]);
    }

  /* Pre-load the table of nonstandard integer types.  */
  traverse_nonstandard_integer_type_cache (nitc_callback, (void *) cache);

  /* Pre-load all the builtin types.  */
  pph_cache_add_builtin_types (cache);

  /* Pre-load the table of canonical types.  */
  type_hash_table_traverse (pph_cache_add_canonical_type, cache);

  /* Add other well-known nodes that should always be taken from the
     current compilation context.  */
  pph_cache_add_full_tree (cache, global_namespace);
  pph_cache_add_full_tree (cache, DECL_CONTEXT (global_namespace));
}


/* Initialize the pre-loaded cache.  This contains all the common
   tree nodes built by the compiler on startup.  */

static void
pph_init_preloaded_cache (void)
{
  pph_preloaded_cache = XCNEW (pph_cache);
  pph_cache_init (pph_preloaded_cache);
  pph_cache_preload (pph_preloaded_cache);
}


/* Insert DATA in CACHE at slot IX.  TAG represents the data structure
   pointed-to by DATA.  As a restriction to prevent stomping on cache
   entries, this will not allow inserting into the same slot more than
   once.  Return the newly added entry.  */

pph_cache_entry *
pph_cache_insert_at (pph_cache *cache, void *data, unsigned ix,
                     enum pph_tag tag)
{
  void **map_slot;
  pph_cache_entry e;

  e.data = data;
  e.tag = tag;
  e.needs_merge_body = false;
  e.crc = 0;
  e.crc_nbytes = 0;

  map_slot = pointer_map_insert (cache->m, data);

  /* We should not be trying to insert the same data more than once.
     This indicates that the same DATA pointer has been given two
     different cache locations.  This almost always points to a
     problem with merging data structures read from different files.  */
  gcc_assert (*map_slot == NULL);

  *map_slot = (void *) (intptr_t) ix;
  if (ix + 1 > VEC_length (pph_cache_entry, cache->v))
    VEC_safe_grow_cleared (pph_cache_entry, heap, cache->v, ix + 1);
  VEC_replace (pph_cache_entry, cache->v, ix, &e);

  /* FIXME pph.  Hack to prevent cached trees from being garbage
     collected.  */
  if ((unsigned) tag <= (unsigned) PPH_any_tree)
    VEC_safe_push (tree, gc, pph_cached_trees, (tree) data);

  return pph_cache_get_entry (cache, ix);
}


/* Add pointer DATA with data type TAG to CACHE.  If IX_P is not NULL,
   on exit *IX_P will contain the slot number where DATA is stored.
   Return the newly added entry.  */

pph_cache_entry *
pph_cache_add (pph_cache *cache, void *data, unsigned *ix_p, enum pph_tag tag)
{
  unsigned ix;
  pph_cache_entry *e;

  e = pph_cache_lookup (cache, data, &ix, tag);
  if (e == NULL)
    {
      ix = VEC_length (pph_cache_entry, cache->v);
      e = pph_cache_insert_at (cache, data, ix, tag);
    }

  if (ix_p)
    *ix_p = ix;

  return e;
}


/* If DATA exists in CACHE, return the cache entry holding it.  If
   IX_P is not NULL, store the cache slot where DATA resides in *IX_P
   (or (unsigned)-1 if DATA is not found). If CACHE is NULL use
   pph_preloaded_cache.

   If a cache hit is found, the data type tag for the entry must match
   TAG.  */

pph_cache_entry *
pph_cache_lookup (pph_cache *cache, void *data, unsigned *ix_p,
                  enum pph_tag tag)
{
  void **map_slot;
  unsigned ix;
  pph_cache_entry *e;

  if (cache == NULL)
    cache = pph_preloaded_cache;

  map_slot = pointer_map_contains (cache->m, data);
  if (map_slot == NULL)
    {
      e = NULL;
      ix = (unsigned) -1;
    }
  else
    {
      intptr_t slot_ix = (intptr_t) *map_slot;
      gcc_assert (slot_ix == (intptr_t)(unsigned) slot_ix);
      ix = (unsigned) slot_ix;
      e = pph_cache_get_entry (cache, ix);

      /* If the caller is looking for a specific TAG, make sure
         it matches the tag we pulled from the cache.  */
      if (tag != PPH_null)
	gcc_assert (tag == e->tag);
    }

  if (ix_p)
    *ix_p = ix;

  return e;
}


/* Return true if DATA is in the pickle cache of one of STREAM's
   included images.  TAG is the expected data type TAG for data.

   If DATA is found:
      - the index for INCLUDE_P into IMAGE->INCLUDES is returned in
	*INCLUDE_IX_P (if INCLUDE_IX_P is not NULL),
      - the cache slot index for DATA into *INCLUDE_P's pickle cache
	is returned in *IX_P (if IX_P is not NULL), and,
      - the function returns true.

   If DATA is not found:
      - *INCLUDE_IX_P is set to -1 (if INCLUDE_IX_P is not NULL),
      - *IX_P is set to -1 (if IX_P is not NULL), and,
      - the function returns false.  */

pph_cache_entry *
pph_cache_lookup_in_includes (pph_stream *stream, void *data,
			      unsigned *include_ix_p, unsigned *ix_p,
			      enum pph_tag tag)
{
  unsigned include_ix, ix;
  pph_stream *include;
  pph_cache_entry *e;

  /* When searching the external caches, do not try to find a match
     for TAG.  Since this is an external cache, the parser may have
     re-allocated the object pointed by DATA (e.g., when merging
     decls).  In this case, TAG will be different from the tag we find
     in the cache, so instead of ICEing, we ignore the match to force
     the caller to pickle DATA.  */
  e = NULL;
  FOR_EACH_VEC_ELT (pph_stream_ptr, stream->includes.v, include_ix, include)
    {
      e = pph_cache_lookup (&include->cache, data, &ix, PPH_null);
      if (e)
	{
	  /* Only consider DATA found if its data type matches TAG.  If
	     not, it means that the object pointed by DATA has changed,
	     so DATA will need to be re-pickled.  */
	  if (e->tag != tag)
	    e = NULL;
	  break;
	}
    }

  if (e == NULL)
    {
      include_ix = ix = (unsigned) -1;
      ix = (unsigned) -1;
    }

  if (include_ix_p)
    *include_ix_p = include_ix;

  if (ix_p)
    *ix_p = ix;

  return e;
}


/*************************************************** tree contents signature */


/* Associate signature CRC with the first NBYTES of the area memory
   pointed to by slot IX of CACHE.  */

void
pph_cache_sign (pph_cache *cache, unsigned ix, unsigned crc, size_t nbytes)
{
  pph_cache_entry *e;

  /* Needed because xcrc32 requires an int to specify the length but
     tree_size returns size_t values.  */
  gcc_assert (nbytes == (size_t) (int) nbytes);

  e = pph_cache_get_entry (cache, ix);
  e->crc = crc;
  e->crc_nbytes = nbytes;
}


/* Return a signature for tree T.  Store the length of the signed area
   in *NBYTES_P.  */

unsigned
pph_get_signature (tree t, size_t *nbytes_p)
{
  tree prev_chain = NULL;
  rtx prev_rtl = NULL;
  int prev_used;
  size_t nbytes;
  unsigned crc;

  nbytes = tree_size (t);
  if (nbytes_p)
    *nbytes_p = nbytes;

  /* Preserve the value of the fields not included in the signature.  */
  prev_chain = (DECL_P (t)) ? DECL_CHAIN (t) : NULL;
  prev_rtl = (HAS_RTL_P (t)) ? DECL_RTL_IF_SET (t) : NULL;
  prev_used = TREE_USED (t);

  /* Clear the fields not included in the signature.  */
  if (DECL_P (t))
    DECL_CHAIN (t) = NULL;
  if (HAS_RTL_P (t))
    SET_DECL_RTL (t, NULL);
  TREE_USED (t) = 0;

  crc = xcrc32 ((const unsigned char *) t, nbytes, -1);

  /* Restore fields we did not include in the signature.  */
  if (DECL_P (t))
    DECL_CHAIN (t) = prev_chain;
  if (HAS_RTL_P (t))
    SET_DECL_RTL (t, prev_rtl);
  TREE_USED (t) = prev_used;

  return crc;
}


/****************************************************** pph include handling */


/* Return true if PPH image NAME can be used at the point of inclusion
   (given by LOC).  */

static bool
pph_is_valid_here (const char *name, location_t loc)
{
  /* If we are inside a scope, reject the image.  We could be inside a
     namespace or a structure which changes the parsing context for
     the original text file.  */
  if (scope_chain->x_brace_nesting > 0)
    {
      warning_at (loc, OPT_Winvalid_pph,
		  "PPH file %s not included at global scope", name);
      return false;
    }

  return true;
}


/* Record a #include or #include_next for PPH.
   READER is the main pre-processor object, LOC is the location where
   the #include is being emitted from, DNAME is the name of the
   #include directive used, NAME is the canonical name of the file being
   included, ANGLE_BRACKETS is non-zero if this #include uses <> and
   TOK_P is a pointer to the current token being pre-processed.  */

static bool
pph_include_handler (cpp_reader *reader,
                     location_t loc,
                     const unsigned char *dname,
                     const char *name,
                     int angle_brackets,
                     const cpp_token **tok_p ATTRIBUTE_UNUSED)
{
  const char *pph_file;
  bool read_text_file_p;

  timevar_start (TV_PPH);

  if (flag_pph_tracer >= 1)
    {
      fprintf (pph_logfile, "PPH: #%s", dname);
      fprintf (pph_logfile, " %c", angle_brackets ? '<' : '"');
      fprintf (pph_logfile, "%s", name);
      fprintf (pph_logfile, "%c\n", angle_brackets ? '>' : '"');
    }

  /* If PPH has been disabled, process the #include as a regular
     text include.  */
  if (!pph_enabled_p ())
    return true;

  /* If we find a #include_next directive in the primary file,
     refuse to generate a PPH image for it.  #include_next cannot
     be resolved from the primary source file, so generating an
     image for it would cause an infinite self-referential loop
     in the line table.  */
  if (cpp_in_primary_file (reader)
      && strcmp ((const char *)dname, "include_next") == 0)
    {
      warning_at (loc, OPT_Winvalid_pph, "#include_next not allowed in a "
		  "PPH file.  PPH generation disabled for %s",
		  LOCATION_FILE (loc));
      pph_disable_output ();
      return true;
    }

  read_text_file_p = true;
  pph_file = query_pph_include_map (name);
  if (pph_file != NULL
      && pph_is_valid_here (name, loc)
      && !cpp_included_before (reader, name, input_location))
    {
      /* The stream we are currently generating is private to
	 the writer.  As an exception, allow access from the
	 include handler.  */
      pph_stream *pph_writer_get_stream (void);
      pph_stream *include = pph_read_file (pph_file, pph_writer_get_stream ());
      if (include)
	read_text_file_p = false;
      else
	warning_at (loc, OPT_Wmissing_pph,
		    "cannot open PPH file %s for reading: %m; "
		    "using original header %s", pph_file, name);
    }

  timevar_stop (TV_PPH);

  return read_text_file_p;
}


/* PPH include tree dumper.  Each entry in this file has the format:

	DEPTH|SYSP|DNAME|CANONICAL-NAME|FULL-NAME|PPH-NAME

  Where:
	DEPTH		is the include depth of the file.
	SYSP		1 for a system header
			2 for a C system header that needs 'extern "C"'
			0 otherwise.
	DNAME		name of the #include directive used.
	CANONICAL-NAME	is the name of the file as specified by the
			#include directive.
	FULL-NAME	is the full path name where the included file
			was found by the pre-processor.
	PPH-NAME	is the name of the associated PPH file.  */

typedef struct {
  /* Name of current #include directive.  */
  const unsigned char *dname;

  /* Canonical name of file being included.  */
  const char *name;

  /* Previous libcpp #include handler.  */
  void (*prev_file_change) (cpp_reader *, const struct line_map *);

  /* Previous libcpp file change handler.  */
  bool (*prev_include) (cpp_reader *, source_location, const unsigned char *,
		        const char *, int, const cpp_token **);
} pph_include_tree_dumper;

static pph_include_tree_dumper tree_dumper;


/* #include handler for libcpp.  READER is the main pre-processor object,
   LOC is the location where the #include is being emitted from, DNAME
   is the name of the #include directive used, NAME is the canonical
   name of the file being included, ANGLE_BRACKETS is non-zero if this
   #include uses <> and TOK_P is a pointer to the current token being
   pre-processed.  */

static bool
pph_include_handler_for_map (cpp_reader *reader,
			     location_t loc,
                             const unsigned char *dname,
                             const char *name,
                             int angle_brackets,
                             const cpp_token **tok_p)
{
  bool retval = true;

  if (tree_dumper.prev_include)
    retval &= tree_dumper.prev_include (reader, loc, dname, name,
					angle_brackets, tok_p);
  tree_dumper.dname = dname;
  tree_dumper.name = name;

  return retval;
}


/* Return a copy of NAME with the characters '/' and '.' replaced with
   '_'.  The caller is reponsible for freeing the returned string.  */

static char *
pph_flatten_name (const char *name)
{
  char *str = xstrdup (name);
  size_t i;

  for (i = 0; i < strlen (str); i++)
    if (str[i] == DIR_SEPARATOR || str[i] == '.')
      str[i] = '_';

  return str;
}


/* File change handler for libcpp.  READER is the main pre-processor object,
   MAP is the line map entry for the file that we are entering into.  */

static void
pph_file_change_handler (cpp_reader *reader, const struct line_map *map)
{
  char *flat;

  if (tree_dumper.prev_file_change)
    tree_dumper.prev_file_change (reader, map);

  /* We are only interested in line maps that describe a new file being
     entered.  */
  if (map == NULL || map->reason != LC_ENTER)
    return;

  /* Emit a line to the map file with the format:

	DEPTH|SYSP|DNAME|CANONICAL-NAME|FULL-NAME|PPH-NAME
  */
  flat = pph_flatten_name (map->d.ordinary.to_file);
  fprintf (stderr, "%d|%d|%s|%s|%s|%s.pph\n", line_table->depth,
	   map->d.ordinary.sysp, tree_dumper.dname, tree_dumper.name,
	   map->d.ordinary.to_file, flat);
  free (flat);
  tree_dumper.dname = NULL;
  tree_dumper.name = NULL;
}


/* Initialize the #include tree dumper.  */

void
pph_init_include_tree (void)
{
  cpp_callbacks *cb;

  memset (&tree_dumper, 0, sizeof (tree_dumper));

  if (pph_enabled_p ())
    fatal_error ("do not use -fpph-map-gen with any other PPH flag");

  /* Set up the libcpp handler for file change events.  Each event
     will generate a new entry in the map file.  */
  cb = cpp_get_callbacks (parse_in);

  tree_dumper.prev_file_change = cb->file_change;
  cb->file_change = pph_file_change_handler;

  tree_dumper.prev_include = cb->include;
  cb->include = pph_include_handler_for_map;
}


/* Dump on FILE the include tree for STREAM as recorded in
   STREAM.INCLUDES.  INDENT is the number of spaces to indent before
   printing each entry.  */

void
pph_dump_includes (FILE *file, pph_stream *stream, unsigned indent)
{
  pph_stream *include;
  unsigned i, j;

  if (file == NULL)
    file = stderr;

  FOR_EACH_VEC_ELT (pph_stream_ptr, stream->includes.v, i, include)
    {
      for (j = 0; j < indent; j++)
	fputc (' ', file);
      fprintf (file, "#%u %s (%s)\n", i, include->name, include->header_name);
      pph_dump_includes (file, include, indent + 2);
    }
}


/* Handle work need after PPH files have been read,
   but before parsing starts.  */

void
pph_loaded (void)
{
  pph_set_global_identifier_bindings ();
  if (flag_pph_dump_tree)
    /* FIXME pph: We could probably just dump the identifier bindings.  */
    pph_dump_global_state (pph_logfile, "after identifiers bound");
}


/*********************************************************** stream handling */


/* Return the index into the registry for STREAM.  If STREAM has not been
   registered yet, return -1.  */

static unsigned
pph_stream_registry_ix_for (pph_stream *stream)
{
  void **slot;

  slot = pointer_map_contains (pph_stream_registry.image_ix, stream);
  if (slot == NULL)
    return (unsigned) -1;

  return (unsigned)(intptr_t) *slot;
}


/* Return true if STREAM has been registered.  */

static bool
pph_stream_registered_p (pph_stream *stream)
{
  return pph_stream_registry_ix_for (stream) != -1u;
}


/* Associate string NAME with the registry entry for STREAM.  */

static void
pph_stream_registry_add_name (pph_stream *stream, const char *name)
{
  void **slot;
  struct pph_name_stream_map e;

  /* STREAM should have been registered beforehand.  */
  gcc_assert (pph_stream_registered_p (stream));

  /* Now associate NAME to STREAM.  */
  e.name = name;
  e.ix = pph_stream_registry_ix_for (stream);
  slot = htab_find_slot (pph_stream_registry.name_ix, &e, INSERT);
  gcc_assert (*slot == NULL);
  *slot = (void *) XNEW (struct pph_name_stream_map);
  memcpy ((struct pph_name_stream_map *) *slot, &e, sizeof (e));
}


/* Set NAME to be STREAM's full pathname for the corresponding header
   file.  */

void
pph_stream_set_header_name (pph_stream *stream, const char *name)
{
  stream->header_name = name;
  pph_stream_registry_add_name (stream, name);
}


/* Return the PPH stream associated with NAME.  Return NULL if no such
   mapping exist.  */

pph_stream *
pph_stream_registry_lookup (const char *name)
{
  void **slot;
  intptr_t slot_ix;
  struct pph_name_stream_map e;

  e.name = name;
  e.ix = -1u;
  slot = htab_find_slot (pph_stream_registry.name_ix, &e, NO_INSERT);
  if (slot == NULL)
    return NULL;

  slot_ix = ((struct pph_name_stream_map *) *slot)->ix;
  return VEC_index (pph_stream_ptr, pph_stream_registry.v, slot_ix);
}


/* Register STREAM in the table of open streams.  */

static void
pph_stream_register (pph_stream *stream)
{
  void **slot;
  unsigned vlen;

  slot = pointer_map_insert (pph_stream_registry.image_ix, stream);

  /* Disallow multpile registration of the same STREAM.  This is overly
     strict, but it prevents some unnecessary overhead.  */
  gcc_assert (*slot == NULL);

  VEC_safe_push (pph_stream_ptr, heap, pph_stream_registry.v, stream);
  vlen = VEC_length (pph_stream_ptr, pph_stream_registry.v);
  *slot = (void *)(intptr_t) (vlen - 1);

  /* Add a mapping between STREAM's PPH file name and STREAM.  */
  pph_stream_registry_add_name (stream, stream->name);
}


/* Unregister STREAM from the table of open streams.  */

static void
pph_stream_unregister (pph_stream *stream)
{
  void **slot;
  unsigned ix;

  slot = pointer_map_contains (pph_stream_registry.image_ix, stream);
  gcc_assert (slot);
  ix = (unsigned)(intptr_t) *slot;

  /* Mark it unregistered in the image index.  */
  *slot = (void *)(intptr_t) -1;

  /* Remove it from the image list.  Note that we do not need to
     remove the index from the name index.  Any further lookups will
     simply return NULL.  */
  VEC_replace (pph_stream_ptr, pph_stream_registry.v, ix, NULL);
}


/* Create a new PPH stream to be stored on the file called NAME.
   MODE is passed to fopen directly.  If NAME could not be opened,
   return NULL to indicate to the caller that it should process NAME
   as a regular text header.  */

pph_stream *
pph_stream_open (const char *name, const char *mode)
{
  pph_stream *stream;
  FILE *f;

  /* If we have already opened a PPH stream named NAME, just return
     its associated stream.  */
  stream = pph_stream_registry_lookup (name);
  if (stream)
    {
      /* In a circular #include scenario, we will eventually try to
	 read from the same PPH image that we are generating.  To
	 avoid that problem, detect circularity and return NULL to
	 force the caller to process NAME as a regular text header.  */
      if (stream->write_p && strchr (mode, 'r') != NULL)
	return NULL;

      /* Otherwise, assert that we have read (or are reading) STREAM
	 and return it.  */
      gcc_assert (stream->in_memory_p);
      return stream;
    }

  f = fopen (name, mode);
  if (!f)
    return NULL;

  stream = XCNEW (pph_stream);
  stream->file = f;
  stream->name = xstrdup (name);
  stream->write_p = (strchr (mode, 'w') != NULL);
  pph_cache_init (&stream->cache);
  stream->preloaded_cache = pph_preloaded_cache;
  stream->includes.m = pointer_set_create ();

  /* Register STREAM in the table of all open streams.  Do it before
     initializing the reader or writer since they may need to look it
     up in the registry.  */
  pph_stream_register (stream);

  if (stream->write_p)
    pph_init_write (stream);
  else
    pph_init_read (stream);

  return stream;
}


/* Close PPH stream STREAM.  If FLUSH_P is true and STREAM was being
   written to, then STREAM's encoding buffers are flushed before
   closing it.  Otherwise, STREAM is closed without flushing internal
   buffers and its associated file is removed.  This is used when an
   exceptional condition occurs that prevents us from generating a PPH
   image.  */

static void
pph_stream_close_1 (pph_stream *stream, bool flush_p)
{
  /* STREAM can be NULL if it could not be properly opened.  An error
     has already been emitted, so avoid crashing here.  */
  if (stream == NULL)
    return;

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "PPH: Closing %s\n", stream->name);

  /* If we were writing to STREAM and the caller tells us to, flush
     all the memory buffers.  This does the actual writing of all the
     pickled data structures.  */
  if (stream->write_p && flush_p)
    pph_flush_buffers (stream);

  fclose (stream->file);

  /* If we were writing but the caller did not want STREAM's buffers
     flushed, remove the PPH file.  */
  if (stream->write_p && !flush_p)
    {
      if (flag_pph_tracer >= 1)
	fprintf (pph_logfile, "PPH: Removing %s\n", stream->name);
      unlink (stream->name);
    }

  /* Deallocate all memory used.  */
  stream->file = NULL;
  VEC_free (pph_cache_entry, heap, stream->cache.v);
  pointer_map_destroy (stream->cache.m);
  VEC_free (pph_replay_entry, heap, stream->replay.v);
  VEC_free (pph_stream_ptr, heap, stream->includes.v);
  pointer_set_destroy (stream->includes.m);

  if (stream->write_p)
    {
      destroy_output_block (stream->encoder.w.ob);
      free (stream->encoder.w.decl_state_stream);
      lto_delete_out_decl_state (stream->encoder.w.out_state);
    }
  else
    {
      unsigned i;

      free (stream->encoder.r.ib);
      lto_data_in_delete (stream->encoder.r.data_in);
      for (i = 0; i < PPH_NUM_SECTIONS; i++)
	free (stream->encoder.r.pph_sections[i]);
      free (stream->encoder.r.pph_sections);
      free (stream->encoder.r.file_data);
    }

  /* Unregister STREAM.  */
  pph_stream_unregister (stream);

  free (stream);
}


/* Close PPH stream STREAM.  If STREAM was being written to, flush its
   encoding buffers.  */

void
pph_stream_close (pph_stream *stream)
{
  pph_stream_close_1 (stream, true);
}


/* Close PPH stream STREAM.  If STREAM was being written to, do not
   flush its encoding buffers and remove the associated file.  */

void
pph_stream_close_no_flush (pph_stream *stream)
{
  pph_stream_close_1 (stream, false);
}

/********************************************************** stream callbacks */


/* Callback for writing ASTs to a stream.  Write EXPR to the PPH stream
   in OB.  */

static void
pph_write_tree (struct output_block *ob, tree expr, bool ref_p ATTRIBUTE_UNUSED,
		bool this_ref_p ATTRIBUTE_UNUSED)
{
  pph_out_tree ((pph_stream *) ob->sdata, expr);
}


/* Callback for reading ASTs from a stream.  Instantiate and return a
   new tree from the PPH stream in DATA_IN.  */

static tree
pph_read_tree (struct lto_input_block *ib ATTRIBUTE_UNUSED,
	       struct data_in *data_in)
{
  return pph_in_tree ((pph_stream *) data_in->sdata);
}


/* Callback for streamer_hooks.input_location.  An offset is applied to
   the location_t read in according to the properties of the merged
   line_table.  IB and DATA_IN are as in lto_input_location.  This function
   should only be called after pph_in_and_merge_line_table was called as
   we expect pph_loc_offset to be set.  */

static location_t
pph_input_location (struct lto_input_block *ib ATTRIBUTE_UNUSED,
                    struct data_in *data_in)
{
  return pph_in_location ((pph_stream *) data_in->sdata);
}


/* Callback for streamer_hooks.output_location.  Output the LOC directly,
   an offset will be applied on input after rebuilding the line_table.
   OB and LOC are as in lto_output_location.  */

static void
pph_output_location (struct output_block *ob, location_t loc)
{
  pph_out_location ((pph_stream *) ob->sdata, loc);
}


/******************************************************** pph initialization */


/* Initialize all the streamer hooks used for streaming ASTs.  */

static void
pph_hooks_init (void)
{
  streamer_hooks_init ();
  streamer_hooks.write_tree = pph_write_tree;
  streamer_hooks.read_tree = pph_read_tree;
  streamer_hooks.input_location = pph_input_location;
  streamer_hooks.output_location = pph_output_location;
}


/* Initialize the streamer.  */

static void
pph_streamer_init (void)
{
  pph_hooks_init ();
  pph_init_preloaded_cache ();
  pph_cached_trees = NULL;
}


/* The initial order of the size of the lexical lookaside table,
   which will accomodate as many as half of its slots in use.  */
static const unsigned int cpp_lt_order = /* 2 to the power of */ 9;


/* Hash and comparison functions for pph_stream_registry.  */

static hashval_t
pph_header_image_hash (const void *p)
{
  return htab_hash_string (((const struct pph_name_stream_map *)p)->name);
}

static int
pph_header_image_eq (const void *p1, const void *p2)
{
  const char *name1 = ((const struct pph_name_stream_map *)p1)->name;
  const char *name2 = ((const struct pph_name_stream_map *)p2)->name;
  return (name1 == name2 || strcmp (name1, name2) == 0);
}


/* Initialize PPH support.  */

void
pph_init (void)
{
  cpp_callbacks *cb;
  cpp_lookaside *table;

  timevar_start (TV_PPH);

  if (flag_pph_logfile)
    {
      pph_logfile = fopen (flag_pph_logfile, "w");
      if (!pph_logfile)
	fatal_error ("Cannot create %s for writing: %m", flag_pph_logfile);
    }
  else
    pph_logfile = stderr;

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "PPH: Initializing.\n");

  /* Set up the libcpp handler for #include.  */
  cb = cpp_get_callbacks (parse_in);
  cb->include = pph_include_handler;

  table = cpp_lt_exchange (parse_in,
                           cpp_lt_create (cpp_lt_order, flag_pph_debug/2));
  gcc_assert (table == NULL);

  /* Set up the PPH registry.  */
  pph_stream_registry.name_ix = htab_create (10, pph_header_image_hash,
				              pph_header_image_eq, NULL);
  pph_stream_registry.image_ix = pointer_map_create ();
  pph_stream_registry.v = NULL;

  pph_streamer_init ();

  /* If we are generating a PPH file, initialize the writer.  */
  if (pph_writer_enabled_p ())
    pph_writer_init ();

  pph_reader_init ();

  timevar_stop (TV_PPH);
}


/********************************************************** pph finalization */


/* Finalize the streamer.  */

static void
pph_streamer_finish (void)
{
  unsigned i;
  pph_stream *image;

  /* Finalize the writer.  */
  if (pph_writer_enabled_p ())
    pph_writer_finish ();

  /* Finalize the reader.  */
  if (pph_reader_enabled_p ())
    pph_reader_finish ();

  /* Close any images read during parsing.  */
  FOR_EACH_VEC_ELT (pph_stream_ptr, pph_stream_registry.v, i, image)
    pph_stream_close (image);

  /* Free all memory used by the stream registry.  */
  VEC_free (pph_stream_ptr, heap, pph_stream_registry.v);
  pointer_map_destroy (pph_stream_registry.image_ix);
  htab_delete (pph_stream_registry.name_ix);

  /* Get rid of all the dead trees we may have had in caches.  */
  pph_cached_trees = NULL;
  ggc_collect ();
}


/* Return true when the main source file is guarded against preprocessor
   multiple inclusions.  */

bool pph_check_main_guarded (void)
{
  const char *offending_file = cpp_main_missing_guard (parse_in);
  if (offending_file == NULL)
    return true;
  error ("header lacks guard for PPH");
  return false;
}


/* Finalize PPH support.  */

void
pph_finish (void)
{
  timevar_start (TV_PPH);

  /* If we found errors during compilation, disable PPH generation.  */
  if (errorcount || sorrycount)
    pph_disable_output ();

  /* Finalize the streamer.  */
  pph_streamer_finish ();

  /* Close log files.  */
  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "PPH: Finishing.\n");

  if (flag_pph_logfile)
    fclose (pph_logfile);

  timevar_stop (TV_PPH);
}

#include "gt-cp-pph-core.h"
