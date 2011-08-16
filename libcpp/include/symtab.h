/* Hash tables.
   Copyright (C) 2000, 2001, 2003, 2004, 2007, 2008, 2009, 2010
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
<http://www.gnu.org/licenses/>.  */

#ifndef LIBCPP_SYMTAB_H
#define LIBCPP_SYMTAB_H

#include "obstack.h"
#include "line-map.h"

#ifndef GTY
#define GTY(x) /* nothing */
#endif

/* This is what each hash table entry points to.  It may be embedded
   deeply within another object.  */
typedef struct ht_identifier ht_identifier;
typedef struct ht_identifier *ht_identifier_ptr;
struct GTY(()) ht_identifier {
  const unsigned char *str;
  unsigned int len;
  unsigned int hash_value;
};

#define HT_LEN(NODE) ((NODE)->len)
#define HT_STR(NODE) ((NODE)->str)

typedef struct ht hash_table;
typedef struct ht_identifier *hashnode;

enum ht_lookup_option {HT_NO_INSERT = 0, HT_ALLOC};

/* An identifier hash table for cpplib and the front ends.  */
struct ht
{
  /* Identifiers are allocated from here.  */
  struct obstack stack;

  hashnode *entries;
  /* Call back, allocate a node.  */
  hashnode (*alloc_node) (hash_table *);
  /* Call back, allocate something that hangs off a node like a cpp_macro.  
     NULL means use the usual allocator.  */
  void * (*alloc_subobject) (size_t);

  unsigned int nslots;		/* Total slots in the entries array.  */
  unsigned int nelements;	/* Number of live elements.  */

  /* Link to reader, if any.  For the benefit of cpplib.  */
  struct cpp_reader *pfile;

  /* Table usage statistics.  */
  unsigned int searches;
  unsigned int collisions;

  /* Should 'entries' be freed when it is no longer needed?  */
  bool entries_owned;
};

/* Initialize the hashtable with 2 ^ order entries.  */
extern hash_table *ht_create (unsigned int order);

/* Frees all memory associated with a hash table.  */
extern void ht_destroy (hash_table *);

extern hashnode ht_lookup (hash_table *, const unsigned char *,
			   size_t, enum ht_lookup_option);
extern hashnode ht_lookup_with_hash (hash_table *, const unsigned char *,
                                     size_t, unsigned int,
                                     enum ht_lookup_option);
#define HT_HASHSTEP(r, c) ((r) * 67 + ((c) - 113));
#define HT_HASHFINISH(r, len) ((r) + (len))

/* For all nodes in TABLE, make a callback.  The callback takes
   TABLE->PFILE, the node, and a PTR, and the callback sequence stops
   if the callback returns zero.  */
typedef int (*ht_cb) (struct cpp_reader *, hashnode, const void *);
extern void ht_forall (hash_table *, ht_cb, const void *);

/* For all nodes in TABLE, call the callback.  If the callback returns
   a nonzero value, the node is removed from the table.  */
extern void ht_purge (hash_table *, ht_cb, const void *);

/* Restore the hash table.  */
extern void ht_load (hash_table *ht, hashnode *entries,
		     unsigned int nslots, unsigned int nelements, bool own);

/* Dump allocation statistics to stderr.  */
extern void ht_dump_statistics (hash_table *);


/* A lookaside identifier table for subsets of the token stream.  */

typedef struct cpp_lookaside cpp_lookaside;

/* A summary of the identifier uses captured by the lookaside table.  */

typedef struct GTY(()) cpp_ident_use
{
  const char *ident_str;
  const char *before_str;
  const char *after_str;
  unsigned int ident_len;
  unsigned int before_len;
  unsigned int after_len;
  bool used_by_directive;
  bool expanded_to_text;
  /* FIX pph: We can reduce the space by shortening ident_len.  */
} cpp_ident_use;

typedef struct GTY(()) cpp_idents_used
{
  unsigned int max_ident_len;
  unsigned int max_value_len;
  unsigned int num_entries;
  cpp_ident_use *entries;
  struct obstack * GTY((skip)) strings;
} cpp_idents_used;

/* Exchange the READER's current lookaside table with a new table.
   To deactivate the lookaside table, set it to NULL.
   The current table is the return value.
   Clients are responsible for creating and destroying the tables.  */
cpp_lookaside *
cpp_lt_exchange (struct cpp_reader *reader, cpp_lookaside *desired);

/* Create the lookaside table of pow(2,ORDER) entries
   and set the DEBUG level.  */
cpp_lookaside *
cpp_lt_create (unsigned int order, unsigned int debug);

/* Frees all memory associated with a lookaside TABLE.  */
void
cpp_lt_destroy (cpp_lookaside *table);

/* Captures the current state of the READER lookaside table,
   together with macro definition state before and after the table,
   and then empties the table.  */
cpp_idents_used
cpp_lt_capture (struct cpp_reader *reader);

/* Verifies that the previously captured IDENTIFIERS
   are consistent with the current state of the READER.
   If not, set BAD_USE and CUR_DEF to indicate the first  
   inconsistency.  A null means 'not a macro'.  */
bool
cpp_lt_verify_1 (struct cpp_reader *reader, cpp_idents_used* identifiers,
                 cpp_ident_use **bad_use, const char **cur_def,
                 int permit_postdef);
bool
cpp_lt_verify (struct cpp_reader *reader, cpp_idents_used* identifiers,
               cpp_ident_use **bad_use, const char **cur_def);

/* Replay the macro definitions captured by the table of IDENTIFIERS
   into the READER state.  */
void
cpp_lt_replay (struct cpp_reader *reader, cpp_idents_used* identifiers,
               source_location *loc);

/* Destroy IDENTIFIERS captured.  */
void
cpp_lt_idents_destroy (cpp_idents_used *identifiers);


#endif /* LIBCPP_SYMTAB_H */
