/* Out-of-bounds pointers 
   Copyright (C) 2007, 2008 Free Software Foundation, Inc.
   Contributed by Alexander Lamaison <awl03@doc.ic.ac.uk>.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   In addition to the permissions in the GNU General Public License, the
   Free Software Foundation gives you unlimited permission to link the
   compiled version of this file into combinations with other programs,
   and to distribute those combinations without any restriction coming
   from the use of this file.  (The General Public License restrictions
   do apply in other respects; for example, they cover modification of
   the file, and distribution when not linked into a combine
   executable.)

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include <stdio.h>
#include "bounds-oob.h"
#include "bounds-impl.h"
#include "bounds-splay-tree.h"

static boundssplay_tree
__bounds_oob_tree ()
{
  static boundssplay_tree tree;
  
  if (UNLIKELY (tree == NULL))
    tree = boundssplay_tree_new ();

  return tree;
}

/* ------------------------------------------------------------------------ */
/* Splay tree manipulation functions.  */

/* Creates a new out-of-bound object and returns it.  */

void *
__bounds_create_oob (void *ptr, void *old_ptr, const char *location)
{
  DECLARE (void *, malloc, size_t s);
  __bounds_oob_ptr_t* oob = CALL_REAL (malloc, sizeof(__bounds_oob_ptr_t));

  oob->ptr = (uintptr_t) ptr;
  oob->last_valid = (uintptr_t) old_ptr;
  oob->arith_location = location;

  boundssplay_tree_insert (__bounds_oob_tree (), 
			   (boundssplay_tree_key) oob,
			   (boundssplay_tree_value) oob);

  TRACE ("Created oob pointer at %p (ptr=%p, last_valid=%p, arith_location=%s)\n",
	 oob, (void *) oob->ptr, (void *) oob->last_valid, oob->arith_location);

  return (void *) oob;
}

/* Looks for associated OOB-pointer object for given pointer value if
   one exists.  Returns NULL otherwise.  */

__bounds_oob_ptr_t *
__bounds_get_oob (void *ptr)
{
  boundssplay_tree_key k = (boundssplay_tree_key) ptr;
  boundssplay_tree_node n = boundssplay_tree_lookup (__bounds_oob_tree (), k);

  TRACE ("getting oob pointer object at ptr=%p\n", ptr);

  if (n != NULL)
    return (__bounds_oob_ptr_t *) n->value;

  return NULL;
}

/* Frees associated OOB-pointer object at given pointer if one exists.  */

void
__bounds_destroy_oob (void *ptr)
{
  boundssplay_tree_key k = (boundssplay_tree_key) ptr;
  boundssplay_tree_node n = boundssplay_tree_lookup (__bounds_oob_tree (), k);
  DECLARE (void, free, void *p);

  TRACE ("destroying oob pointer object at ptr=%p\n", ptr);

  if (n != NULL)
    CALL_REAL (free, n->value);

  return;
}

