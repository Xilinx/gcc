/* Hash tables.
   Copyright (C) 2000, 2001, 2003, 2004, 2008, 2009
   Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

#include "config.h"
#include "system.h"
#include "symtab.h"
#include "internal.h"

/* The code below is a specialization of Vladimir Makarov's expandable
   hash tables (see libiberty/hashtab.c).  The abstraction penalty was
   too high to continue using the generic form.  This code knows
   intrinsically how to calculate a hash value, and how to compare an
   existing entry with a potential new one.  */

static void ht_expand (hash_table *);
static double approx_sqrt (double);

/* A deleted entry.  */
#define DELETED ((hashnode) -1)

/* Calculate the hash of the string STR of length LEN.  */

unsigned int
ht_calc_hash (const unsigned char *str, size_t len)
{
  size_t n = len;
  unsigned int r = 0;

  while (n--)
    r = HT_HASHSTEP (r, *str++);

  return HT_HASHFINISH (r, len);
}

/* Initialize an identifier hashtable.  */

hash_table *
ht_create (unsigned int order)
{
  unsigned int nslots = 1 << order;
  hash_table *table;

  table = XCNEW (hash_table);

  /* Strings need no alignment.  */
  _obstack_begin (&table->stack, 0, 0,
		  (void *(*) (long)) xmalloc,
		  (void (*) (void *)) free);

  obstack_alignment_mask (&table->stack) = 0;

  table->entries = XCNEWVEC (hashnode, nslots);
  table->entries_owned = true;
  table->nslots = nslots;
  return table;
}

/* Frees all memory associated with a hash table.  */

void
ht_destroy (hash_table *table)
{
  obstack_free (&table->stack, NULL);
  if (table->entries_owned)
    free (table->entries);
  free (table);
}

/* Returns the hash entry for the a STR of length LEN.  If that string
   already exists in the table, returns the existing entry.  If the
   identifier hasn't been seen before, and INSERT is CPP_NO_INSERT,
   returns NULL.  Otherwise insert and returns a new entry.  A new
   string is allocated.  */
hashnode
ht_lookup (hash_table *table, const unsigned char *str, size_t len,
	   enum ht_lookup_option insert)
{
  return ht_lookup_with_hash (table, str, len, ht_calc_hash (str, len),
			      insert);
}

hashnode
ht_lookup_with_hash (hash_table *table, const unsigned char *str,
		     size_t len, unsigned int hash,
		     enum ht_lookup_option insert)
{
  unsigned int hash2;
  unsigned int index;
  unsigned int deleted_index = table->nslots;
  size_t sizemask;
  hashnode node;

  sizemask = table->nslots - 1;
  index = hash & sizemask;
  table->searches++;

  node = table->entries[index];

  if (node != NULL)
    {
      if (node == DELETED)
	deleted_index = index;
      else if (node->hash_value == hash
	       && HT_LEN (node) == (unsigned int) len
	       && !memcmp (HT_STR (node), str, len))
	return node;

      /* hash2 must be odd, so we're guaranteed to visit every possible
	 location in the table during rehashing.  */
      hash2 = ((hash * 17) & sizemask) | 1;

      for (;;)
	{
	  table->collisions++;
	  index = (index + hash2) & sizemask;
	  node = table->entries[index];
	  if (node == NULL)
	    break;

	  if (node == DELETED)
	    {
	      if (deleted_index != table->nslots)
		deleted_index = index;
	    }
	  else if (node->hash_value == hash
		   && HT_LEN (node) == (unsigned int) len
		   && !memcmp (HT_STR (node), str, len))
	    return node;
	}
    }

  if (insert == HT_NO_INSERT)
    return NULL;

  /* We prefer to overwrite the first deleted slot we saw.  */
  if (deleted_index != table->nslots)
    index = deleted_index;

  node = (*table->alloc_node) (table);
  table->entries[index] = node;

  HT_LEN (node) = (unsigned int) len;
  node->hash_value = hash;

  if (table->alloc_subobject)
    {
      char *chars = (char *) table->alloc_subobject (len + 1);
      memcpy (chars, str, len);
      chars[len] = '\0';
      HT_STR (node) = (const unsigned char *) chars;
    }
  else
    HT_STR (node) = (const unsigned char *) obstack_copy0 (&table->stack,
							   str, len);

  if (++table->nelements * 4 >= table->nslots * 3)
    /* Must expand the string table.  */
    ht_expand (table);

  return node;
}

/* Double the size of a hash table, re-hashing existing entries.  */

static void
ht_expand (hash_table *table)
{
  hashnode *nentries, *p, *limit;
  unsigned int size, sizemask;

  size = table->nslots * 2;
  nentries = XCNEWVEC (hashnode, size);
  sizemask = size - 1;

  p = table->entries;
  limit = p + table->nslots;
  do
    if (*p && *p != DELETED)
      {
	unsigned int index, hash, hash2;

	hash = (*p)->hash_value;
	index = hash & sizemask;

	if (nentries[index])
	  {
	    hash2 = ((hash * 17) & sizemask) | 1;
	    do
	      {
		index = (index + hash2) & sizemask;
	      }
	    while (nentries[index]);
	  }
	nentries[index] = *p;
      }
  while (++p < limit);

  if (table->entries_owned)
    free (table->entries);
  table->entries_owned = true;
  table->entries = nentries;
  table->nslots = size;
}

/* For all nodes in TABLE, callback CB with parameters TABLE->PFILE,
   the node, and V.  */
void
ht_forall (hash_table *table, ht_cb cb, const void *v)
{
  hashnode *p, *limit;

  p = table->entries;
  limit = p + table->nslots;
  do
    if (*p && *p != DELETED)
      {
	if ((*cb) (table->pfile, *p, v) == 0)
	  break;
      }
  while (++p < limit);
}

/* Like ht_forall, but a nonzero return from the callback means that
   the entry should be removed from the table.  */
void
ht_purge (hash_table *table, ht_cb cb, const void *v)
{
  hashnode *p, *limit;

  p = table->entries;
  limit = p + table->nslots;
  do
    if (*p && *p != DELETED)
      {
	if ((*cb) (table->pfile, *p, v))
	  *p = DELETED;
      }
  while (++p < limit);
}

/* Restore the hash table.  */
void
ht_load (hash_table *ht, hashnode *entries,
	 unsigned int nslots, unsigned int nelements,
	 bool own)
{
  if (ht->entries_owned)
    free (ht->entries);
  ht->entries = entries;
  ht->nslots = nslots;
  ht->nelements = nelements;
  ht->entries_owned = own;
}

/* Dump allocation statistics to stderr.  */

void
ht_dump_statistics (hash_table *table)
{
  size_t nelts, nids, overhead, headers;
  size_t total_bytes, longest, deleted = 0;
  double sum_of_squares, exp_len, exp_len2, exp2_len;
  hashnode *p, *limit;

#define SCALE(x) ((unsigned long) ((x) < 1024*10 \
		  ? (x) \
		  : ((x) < 1024*1024*10 \
		     ? (x) / 1024 \
		     : (x) / (1024*1024))))
#define LABEL(x) ((x) < 1024*10 ? ' ' : ((x) < 1024*1024*10 ? 'k' : 'M'))

  total_bytes = longest = sum_of_squares = nids = 0;
  p = table->entries;
  limit = p + table->nslots;
  do
    if (*p == DELETED)
      ++deleted;
    else if (*p)
      {
	size_t n = HT_LEN (*p);

	total_bytes += n;
	sum_of_squares += (double) n * n;
	if (n > longest)
	  longest = n;
	nids++;
      }
  while (++p < limit);

  nelts = table->nelements;
  overhead = obstack_memory_used (&table->stack) - total_bytes;
  headers = table->nslots * sizeof (hashnode);

  fprintf (stderr, "\nString pool\nentries\t\t%lu\n",
	   (unsigned long) nelts);
  fprintf (stderr, "identifiers\t%lu (%.2f%%)\n",
	   (unsigned long) nids, nids * 100.0 / nelts);
  fprintf (stderr, "slots\t\t%lu\n",
	   (unsigned long) table->nslots);
  fprintf (stderr, "deleted\t\t%lu\n",
	   (unsigned long) deleted);
  fprintf (stderr, "bytes\t\t%lu%c (%lu%c overhead)\n",
	   SCALE (total_bytes), LABEL (total_bytes),
	   SCALE (overhead), LABEL (overhead));
  fprintf (stderr, "table size\t%lu%c\n",
	   SCALE (headers), LABEL (headers));

  exp_len = (double)total_bytes / (double)nelts;
  exp2_len = exp_len * exp_len;
  exp_len2 = (double) sum_of_squares / (double) nelts;

  fprintf (stderr, "coll/search\t%.4f\n",
	   (double) table->collisions / (double) table->searches);
  fprintf (stderr, "ins/search\t%.4f\n",
	   (double) nelts / (double) table->searches);
  fprintf (stderr, "avg. entry\t%.2f bytes (+/- %.2f)\n",
	   exp_len, approx_sqrt (exp_len2 - exp2_len));
  fprintf (stderr, "longest entry\t%lu\n",
	   (unsigned long) longest);
#undef SCALE
#undef LABEL
}

/* Return the approximate positive square root of a number N.  This is for
   statistical reports, not code generation.  */
static double
approx_sqrt (double x)
{
  double s, d;

  if (x < 0)
    abort ();
  if (x == 0)
    return 0;

  s = x;
  do
    {
      d = (s * s - x) / (2 * s);
      s -= d;
    }
  while (d > .0001);
  return s;
}


/* Lookaside Identifier Hash Table */

/* This table is implemented as an extensible linear open hash table.
   See http://en.wikipedia.org/wiki/Open_addressing.  */


/* Exchange the DESIRED lookaside table with the existing table in the
   READER.  This operation is an information-preserving assignment. */

cpp_lookaside *
cpp_lt_exchange (cpp_reader *reader, cpp_lookaside *desired)
{
  cpp_lookaside *current = reader->lookaside_table;
  reader->lookaside_table = desired;
  return current;
}

/* Clear the lookaside TABLE statistics.  */

static void
lt_clear_stats (struct cpp_lookaside *table)
{
  table->searches = 0;
  table->comparisons = 0;
  table->strcmps = 0;
  table->collisions = 0;
  table->misses = 0;
  table->insertions = 0;
  table->macrovalue = 0;
  table->resizes = 0;
  table->bumps = 0;
  table->iterations = 0;
  table->empties = 0;
}

/* Create a lookaside table of pow(2,ORDER) entries and set the DEBUG
   level.  This function is a constructor.  */

cpp_lookaside *
cpp_lt_create (unsigned int order, unsigned int debug)
{
  unsigned int slots = 1 << order;
  cpp_lookaside *table = XCNEW (cpp_lookaside);
  table->entries = XCNEWVEC (struct lae, slots);
  table->order = order;
  table->sticky_order = order;
  table->active = 0;

  table->max_ident_len = 0;
  table->max_value_len = 0;
  table->strings = XCNEW (struct obstack);
  /* Strings need no alignment.  */
  _obstack_begin (table->strings, 0, 0,
		  (void *(*) (long)) xmalloc,
		  (void (*) (void *)) free);
  obstack_alignment_mask (table->strings) = 0;

  table->debug_level = debug;
  lt_clear_stats (table);

  return table;
}

/* Print the statistics for the lookaside TABLE.  */

static void
lt_statistics (struct cpp_lookaside *table)
{
  fprintf (stderr, "lookaside ");
  fprintf (stderr, "order=%u, ",	table->order);
  fprintf (stderr, "active=%u, ",	table->active);
  fprintf (stderr, "search=%llu, ",	table->searches);
  fprintf (stderr, "compare=%llu, ",	table->comparisons);
  fprintf (stderr, "strcmp=%llu, ",	table->strcmps);
  fprintf (stderr, "collide=%llu, ",	table->collisions);
  fprintf (stderr, "miss=%llu, ",	table->misses);
  fprintf (stderr, "insert=%llu, ",	table->insertions);
  fprintf (stderr, "macro=%llu, ",	table->macrovalue);
  fprintf (stderr, "resize=%llu, ",	table->resizes);
  fprintf (stderr, "bump=%llu, ",	table->bumps);
  fprintf (stderr, "iterations=%llu, ",	table->iterations);
  fprintf (stderr, "empties=%llu\n",	table->empties);
}

/* Destroy (deallocate) a lookaside TABLE.  This function is a destructor.  */

void 
cpp_lt_destroy (cpp_lookaside *table)
{
  if (table->debug_level >= 1)
    lt_statistics (table);
  if (table->strings)
    {
      obstack_free (table->strings, NULL);
      free (table->strings);
    }
  free (table->entries);
  free (table);
}

/* Call the GROK function for all the entries in the lookaside TABLE.
   The cpp_lt_forall function passes PASSTHRU to each invocation of GROK,
   in addition to the STR characters and their LEN
   for each IDENT and MACRO value. */
/* FIXME pph: This code is presently unused, but may prove useful later.  */
#if 0
typedef void (*cpp_lookback) (void *passthru,
                              const char *ident_str, unsigned int ident_len,
                              const char *macro_str, unsigned int macro_len);
void
cpp_lt_forall (cpp_lookaside *table, cpp_lookback grok, void *passthru)
{
  unsigned int slots = 1 << table->order;
  struct lae *entries = table->entries;
  unsigned int index;
  for (index = 0; index < slots ; ++index)
    {
      hashnode node = entries[index].node;
      if (node)
        grok (passthru, (const char *)node->str, node->len,
              entries[index].value, entries[index].length);
    }
}
#endif

/* Query a CPP_NODE for its macro value from READER.  */

static const char *
lt_query_macro (cpp_reader *reader, cpp_hashnode *cpp_node)
{
  const char *definition = NULL;
  if (cpp_is_builtin (cpp_node))
    {
      const char *str = (const char *)cpp_node->ident.str;
      if (   strcmp(str, "__DATE__") == 0
          || strcmp(str, "__TIME__") == 0
          || strcmp(str, "__FILE__") == 0
          || strcmp(str, "__LINE__") == 0)
        definition = str;
      else
        {
          static char *string = 0;
          static unsigned int space = 0;
          unsigned int back, needed;
          const char *value;

          value = (const char *)_cpp_builtin_macro_text (reader, cpp_node);
          back = strlen (value);
          needed = 1 + back + 1;
	  if (space < needed)
            {
              if (string != NULL)
                free (string);
              string = XCNEWVEC (char, needed);
              space = needed;
            }
          string[0] = ' ';
          strcpy (string + 1, value);
	  definition = string;
        }
    }
  else
    {
      char c;
      if ((cpp_node->flags & NODE_BUILTIN) && reader->cb.user_builtin_macro)
        reader->cb.user_builtin_macro (reader, cpp_node);
      definition = (const char *) cpp_macro_definition (reader, cpp_node);
      /* Skip over the macro name within the definition.  */
      c = *definition;
      while (   ('0' <= c && c <= '9')
             || ('A' <= c && c <= 'Z')
             || ('a' <= c && c <= 'z')
             ||             (c == '_'))
        {
          c = *++definition;
        }
    }

  if (reader->lookaside_table->debug_level >= 2)
    fprintf (stderr, "PPH: macro %s is %s\n",
                     (const char *)cpp_node->ident.str,
                     definition);

  return definition;
}

/* Capture the current STRING definition of a macro for the
   libcpp CPP_NODE and store it in the look ASIDE table of the READER. */

static unsigned int
lt_macro_value (const char** string, cpp_lookaside *aside,
                cpp_reader *reader, cpp_hashnode *cpp_node)
{
  const char *definition = lt_query_macro (reader, cpp_node);
  size_t macro_len = strlen (definition);
  *string = (const char *) obstack_copy0 (aside->strings, definition, macro_len);
  if (macro_len > aside->max_value_len)
    aside->max_value_len = macro_len;
  ++aside->macrovalue;
  return macro_len;
}

/* Capture the identifier state in the lookaside table of READER
   and then empty the lookaside table.  */

cpp_idents_used
cpp_lt_capture (cpp_reader *reader)
{
  cpp_idents_used used;
  cpp_lookaside *aside = reader->lookaside_table;
  unsigned int num_entries = aside->active;
  unsigned int slots = 1 << aside->order;
  unsigned int table_index;
  unsigned int summary_index = 0;
  
  used.num_entries = aside->active;
  used.entries = XCNEWVEC (cpp_ident_use, num_entries);

  /* Copy the entry information into used identifiers table.  */
  for (table_index = 0; table_index < slots ; ++table_index)
    {
      struct lae *table_entry = aside->entries + table_index;
      hashnode node = table_entry->node;
      if (node)
        {
          cpp_ident_use *summary_entry;
          cpp_hashnode *cpp_node = CPP_HASHNODE (node);

          /* Filter out builtin identifiers.  */
          if (cpp_is_builtin (cpp_node))
            continue;

          summary_entry = used.entries + summary_index++;

          summary_entry->used_by_directive = cpp_node->used_by_directive;
          summary_entry->expanded_to_text = cpp_node->expanded_to_text;
          summary_entry->ident_len = node->len;
          summary_entry->ident_str = (const char *)node->str;
          summary_entry->before_len = table_entry->length;
          summary_entry->before_str = table_entry->value;

          /* Capture any macro value.  */
          if (cpp_node->type == NT_MACRO)
              summary_entry->after_len = lt_macro_value
                  (&summary_entry->after_str, aside, reader, cpp_node);
          /* else .after_str and .after_len are still zero initialized.  */
        }
    }

  /* Take the strings from the table and give to the summary.  */
  used.strings = aside->strings;
  aside->strings = NULL;
  used.max_ident_len = aside->max_ident_len;
  used.max_value_len = aside->max_value_len;

  aside->iterations += slots;
  ++aside->empties;

  /* Do we need to reallocate the table?  */
  if (aside->sticky_order < aside->order - 1)
    {
      /* Allocate a new table.  */
      reader->lookaside_table = cpp_lt_create (aside->sticky_order,
					       aside->debug_level);
      cpp_lt_destroy (aside);  /* May also dump statistics.  */
    }
  else
    {
      /* Reuse the old table.  */

      /* Dump out the statistics.  */
      if (aside->debug_level >= 2)
        {
          lt_statistics (aside);
          lt_clear_stats (aside);
        }

      /* Empty out the entries.  */
      memset (aside->entries, 0, slots * sizeof (struct lae));
      aside->active = 0;

      /* Create a new string table.  */
      aside->max_ident_len = 0;
      aside->max_value_len = 0;
      aside->strings = XCNEW (struct obstack);
      /* Strings need no alignment.  */
      _obstack_begin (aside->strings, 0, 0,
		      (void *(*) (long)) xmalloc,
		      (void (*) (void *)) free);
      obstack_alignment_mask (aside->strings) = 0;

    }

  return used;
}

/* Verify that the INDENTIFIERS have before states that consistent
   with the current identifier definitions in the READER.
   If not, set the BAD_USE and CUR_DEF to indicate the first
   inconsistency.  A null means 'not a macro'.  */

bool
cpp_lt_verify_1 (cpp_reader *reader, cpp_idents_used* identifiers,
                 cpp_ident_use **bad_use, const char **cur_def,
                 int permit_postdef)
{
  unsigned int i;
  unsigned int num_entries = identifiers->num_entries;
  cpp_ident_use *entries = identifiers->entries;

  *bad_use = NULL;
  *cur_def = NULL;

  for (i = 0; i < num_entries; ++i)
    {
      cpp_hashnode *cpp_node;
      cpp_ident_use *entry = entries + i;
      const char *ident_str = entry->ident_str;
      unsigned int ident_len = entry->ident_len;
      const char *before_str = entry->before_str;
      unsigned int before_len = entry->before_len;
      cpp_node = cpp_peek_sym (reader, (const unsigned char *)ident_str,
                               ident_len);
      if (cpp_node == NULL)
	{
	  /* The symbol used to exist, but it doesn't now.  */
	  if (before_str != NULL)
	    {
	      *bad_use = entry;
	      *cur_def = NULL;
	      goto fail;
	    }
	}
      else if (before_len == -1U)
        {
          /* It was not saved as a macro.  */
          if (cpp_node->type == NT_MACRO)
            {
	      /* But it is a macro now!  */
	      const char *definition;
	      definition = (const char*) lt_query_macro (reader, cpp_node);
	      if (permit_postdef)
                {
                /* Check to see if the current value is the after value.  */
                unsigned int after_len = entry->after_len;
                /* strlen is required to avoid the prefix problem.  */
                if (definition == NULL
                    || after_len != strlen (definition)
                    || memcmp (definition, entry->after_str, after_len) != 0)
                  {
                    /* They do not have the same value.  */
                    *bad_use = entry;
                    *cur_def = definition;
                    goto fail;
                  }
                }
	      else
		{
		  *bad_use = entry;
		  *cur_def = definition;
		  goto fail;
		}
            }
          /* Otherwise, both agree it is not a macro.  */
        }
      else
        {
          /* It was saved as a macro.  */
          const char *definition;

          if (cpp_node->type != NT_MACRO)
            {
              /* But it is not a macro now!  */
              *bad_use = entry;
              *cur_def = NULL;
              goto fail;
            }
          /* Otherwise, both agree it is a macro.  */
          definition = lt_query_macro (reader, cpp_node);
          /* strlen is required to avoid the prefix problem.  */
          if (definition == NULL
              || before_len != strlen (definition)
              || memcmp (definition, before_str, before_len) != 0)
            {
              /* They do not have the same value.  */
              *bad_use = entry;
              *cur_def = definition;
              goto fail;
            }
        }
    }
/* pass: */
  *bad_use = NULL;
  *cur_def = NULL;
  return true;

fail:
  return false;
}

bool
cpp_lt_verify (cpp_reader *reader, cpp_idents_used* identifiers,
               cpp_ident_use **bad_use, const char **cur_def)
{
  return cpp_lt_verify_1 (reader, identifiers, bad_use, cur_def, false);
}

/* Produce the macro definition syntax NEEDED by cpp_define from
   the syntax GIVEN by cpp_macro_definition.  */

static void
cpp_lt_define_syntax (char *needed, const char *ident, const char *given)
{
  char c;

  /* Copy over macro identifier.  */
  c = *ident++;
  while (c != '\0')
    {
      *needed++ = c;
      c = *ident++;
    }

  /* Copy over macro definition.  */
  c = *given++;
  if (c == '(')
    {
      /* Copy over parameter list.  */
      while (c != ')')
        {
          *needed++ = c;
          c = *given++;
        }

      /* Copy over trailing parenthesis.  */
      *needed++ = c;
      c = *given++;
    }

  /* Replace definition space by assignment.  */
  /* (c == ' ') */
  *needed++ = '=';

  /* Copy over macro value.  */
  c = *given++;
  while (c != '\0')
    {
      *needed++ = c;
      c = *given++;
    }

  *needed++ = '\0';
}

/* Replay the macro definitions captured by the table of IDENTIFIERS
   into the READER state.  If LOC is non-null, assign *LOC as the
   source_location to all macro definitions replayed.  */

void
cpp_lt_replay (cpp_reader *reader, cpp_idents_used* identifiers,
               source_location *loc)
{
  unsigned int i;
  unsigned int num_entries = identifiers->num_entries;
  cpp_ident_use *entries = identifiers->entries;
  char *buffer = XCNEWVEC (char, identifiers->max_ident_len
                                 + identifiers->max_value_len + 1);

  /* Prevent the lexer from invalidating the tokens we've read so far.  */
  reader->keep_tokens++;

  if (loc)
    cpp_force_token_locations (reader, loc);

  for (i = 0; i < num_entries; ++i)
    {
      cpp_ident_use *entry = entries + i;
      const char *ident_str = entry->ident_str;
      const char *before_str = entry->before_str;
      const char *after_str = entry->after_str;
      if (before_str == NULL)
        {
          if (after_str != NULL)
            {
              cpp_lt_define_syntax (buffer, ident_str, after_str);
              cpp_define (reader, buffer);
            }
          /* else consistently not macros */
        }
      else
        {
          if (after_str == NULL)
            {
              cpp_undef (reader, ident_str);
            }
          else if (strcmp (before_str, after_str) != 0)
            {
              cpp_undef (reader, ident_str);
              cpp_lt_define_syntax (buffer, ident_str, after_str);
              cpp_define (reader, buffer);
            }
          /* else macro with the same definition */
        }
    }

  reader->keep_tokens--;

  if (loc)
    cpp_stop_forcing_token_locations (reader);

  free (buffer);
}

/* Destroy IDENTIFIERS captured.  */

void
cpp_lt_idents_destroy (cpp_idents_used *identifiers)
{
  obstack_free (identifiers->strings, NULL);
  XDELETEVEC (identifiers->entries);
}

/* Mappings from hash to index.  */
#define LT_MASK(order) (~(~0 << (order)))
#define LT_FIRST(hash, order, mask) (((hash) ^ ((hash) >> (order))) & (mask))
#define LT_NEXT(index, mask) (((index) + 1) & (mask))
/* Linear probing.  */

/* Resize the look ASIDE table from its OLD_ORDER to a NEW_ORDER.
   The NEW_ORDER must hold twice the number of active elements.  */

static void
lt_resize (cpp_lookaside *aside, unsigned int old_order, unsigned int new_order)
{
  unsigned int old_index;
  unsigned int old_slots = 1 << old_order;
  unsigned int new_slots = 1 << new_order;
  unsigned int new_mask = LT_MASK (new_order);
  struct lae *old_entries = aside->entries;
  struct lae *new_entries = XCNEWVEC (struct lae, new_slots);
  for ( old_index = 0; old_index < old_slots; ++old_index )
    {
      hashnode node = old_entries[old_index].node;
      if (node)
        {
          unsigned int hash = old_entries[old_index].hash;
          unsigned int new_index = LT_FIRST (hash, new_order, new_mask);
          hashnode probe = new_entries[new_index].node;
          while (probe)
            {
              new_index = LT_NEXT (new_index, new_mask);
              probe = new_entries[new_index].node;
              ++aside->bumps;
            }
          new_entries[new_index].node = node;
          new_entries[new_index].hash = hash;
          new_entries[new_index].length = old_entries[old_index].length;
          new_entries[new_index].value = old_entries[old_index].value;
        }
    }
  free (old_entries);
  aside->entries = new_entries;
  aside->order = new_order;
  ++aside->resizes;
}

/* Lookup the IDENTIFER of the given LENGTH and HASH value
   in the READER's lookaside table.
   The lookup does not compute a hash.  */

cpp_hashnode *
lt_lookup (cpp_reader *reader,
           const unsigned char *identifier,
           size_t length,
           unsigned int hash)
{
  cpp_lookaside *aside = reader->lookaside_table;
  /* Compress the hash to an index.
     Assume there is sufficient entropy in the lowest 2*order bits.  */
  unsigned int order = aside->order;
  unsigned int mask = LT_MASK (order);
  unsigned int index = LT_FIRST (hash, order, mask);
  cpp_hashnode *cpp_node;

  /* Search the lookaside table.  */
  struct lae *entries = aside->entries;
  hashnode node = entries[index].node;
  ++aside->searches;

  /* Hashes have no sentinel value, so an entry is empty iff there is
     a null node value. */
  while (node)
    {
      if (entries[index].hash == hash)
        {
          ++aside->comparisons;
          if (node->len == length)
            {
              ++aside->strcmps;
	      if (memcmp (node->str, identifier, length) == 0)
                return CPP_HASHNODE (node);
            }
        }

      ++aside->collisions;
      index = LT_NEXT (index, mask);
      node = entries[index].node;
    }

  ++aside->misses;

  node = ht_lookup_with_hash
	(reader->hash_table, identifier, length, hash, HT_ALLOC);
  cpp_node = CPP_HASHNODE(node);

  /* Do not save macro parameter names; they don't affect verification.  */
  if (cpp_node->flags & NODE_MACRO_ARG)
    return cpp_node;

  ++aside->insertions;

  /* Fill out new entry.  */
  ++aside->active;
  entries[index].node = node;
  entries[index].hash = hash;
  if (length > aside->max_ident_len)
    aside->max_ident_len = length;

  /* Capture any macro value.  */
  if (cpp_node->type == NT_MACRO)
    entries[index].length = lt_macro_value
        (&entries[index].value, aside, reader, cpp_node);
  /* else .value and .length are still zero from initialization.  */

  /* Check table load factor.  */
  if (aside->active >= (unsigned)(1 << (order - 1)))
      /* Table is at least half full; double it.  */
      lt_resize (aside, order, order + 1);

  return cpp_node;
}
