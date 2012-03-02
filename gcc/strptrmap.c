/* A string to string mapping.

   Copyright (C) 2012 Free Software Foundation, Inc.
   Contributed by Lawrence Crowl <crowl@google.com>.

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

#include "libiberty.h"
#include "hashtab.h"

#include "strptrmap.h"

/* The hash table slot type.  */

typedef struct strptrhashslot_d
{
  const char *key;
  void *val;
} strptrhashslot_t;

/* Returns a hash code for the slot v.
   Function type is typedef hashval_t (*htab_hash) (const void *);  */

static hashval_t
strptrhashslot_hash (const void *v)
{
  const strptrhashslot_t *s = (const strptrhashslot_t *) v;
  return htab_hash_string (s->key);
}

/* Compares two slots, V1 and V2, for equality.
   Function type is typedef int (*htab_eq) (const void *, const void *);  */

static int
strptrhashslot_eq (const void *v1, const void *v2)
{
  const strptrhashslot_t *s1 = (const strptrhashslot_t *) v1;
  const strptrhashslot_t *s2 = (const strptrhashslot_t *) v2;
  return strcmp (s1->key, s2->key) == 0;
}

/* Free the slot V.  */

static void
strptrhashslot_free (void *v)
{
  strptrhashslot_t *s = (strptrhashslot_t *) v;
  free (CONST_CAST (void *, (const void *) s->key));
  free (v);
}

/* The type for the hash table itself.  */

struct strptrmap_d
{
  htab_t tab;
};

/* Create a hash table.  */

strptrmap_t*
strptrmap_create (void)
{
  strptrmap_t *tbl;
  htab_t tab;
  tbl = XCNEW (strptrmap_t);
  gcc_assert (tbl != NULL);
  tab = htab_create (37, strptrhashslot_hash,
                     strptrhashslot_eq, strptrhashslot_free);
  gcc_assert (tab != NULL);
  tbl->tab = tab;
  return tbl;
}

/* Destroy the hash table TBL.  */

void
strptrmap_destroy (strptrmap_t *tbl)
{
  htab_delete (tbl->tab);
  free (tbl);
}

/* Insert a mapping from KEY to VAL into hash table TBL.
   All parameters must not be NULL.
   If the KEY is new, the insert returns NULL.
   If the KEY is not new, the insert returns the old VAL,
   which has NOT been replaced.  */

void *
strptrmap_insert (strptrmap_t *tbl, const char *key, void *val)
{
  strptrhashslot_t query;
  strptrhashslot_t **spot;
  void* result;

  gcc_assert (tbl != NULL);
  gcc_assert (key != NULL);

  query.key = key;
  query.val = val;
  spot = (strptrhashslot_t **) htab_find_slot (tbl->tab, &query, INSERT);

  if (*spot == NULL)
    {
      /* We found no instance of key in the table.  */
      strptrhashslot_t *entry
          = (strptrhashslot_t *) xmalloc (sizeof (strptrhashslot_t));
      entry->key = xstrdup (key);
      entry->val = val;
      *spot = entry;
      result = NULL;
    }
  else
    {
      /* We found an instance of key already in the table.  */
      strptrhashslot_t *entry = (strptrhashslot_t *)*spot;
      result = entry->val;
    }

  return result;
}

/* Lookup a mapping for KEY in hash table TBL.
   All parameters must not be NULL.
   If the KEY is new, the insert returns NULL.
   If the KEY is not new, the insert returns the old VAL,
   which has been replaced.  */

void*
strptrmap_lookup (strptrmap_t* tbl, const char* key)
{
  strptrhashslot_t query;
  strptrhashslot_t **spot;
  void* result;

  gcc_assert (tbl != NULL);
  gcc_assert (key != NULL);

  query.key = key;
  spot = (strptrhashslot_t **) htab_find_slot (tbl->tab, &query, NO_INSERT);

  if (spot == NULL)
    {
      /* We found no instance of key in the table.  */
      result = NULL;
    }
  else
    {
      /* We found an instance of key in the table.  */
      strptrhashslot_t *entry = (strptrhashslot_t *)*spot;
      gcc_assert (entry != NULL);
      result = entry->val;
    }

  return result;
}
