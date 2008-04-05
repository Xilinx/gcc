/* Splay tree implimentation for libbounds
   Copyright (C) 2002, 2003, 2004, 2005 Free Software Foundation, Inc.
   Contributed by Frank Ch. Eigler <fche@redhat.com>
   and Graydon Hoare <graydon@redhat.com>
   Splay Tree code originally by Mark Mitchell <mark@markmitchell.com>,
   adapted from libiberty.

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

/* This is largely taken from the libmudflap splay tree */

#ifndef _BOUNDSSPLAY_TREE_H
#define _BOUNDSSPLAY_TREE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
typedef unsigned int uintptr_t __attribute__ ((__mode__ (__pointer__)));
#endif

/* ------------------------------------------------------------------------ */
/* Splay-tree member types */

/* Use typedefs for the key and data types to facilitate changing
   these types, if necessary.  These types should be sufficiently wide
   that any pointer or scalar can be cast to these types, and then
   cast back, without loss of precision.  */
typedef uintptr_t boundssplay_tree_key;
typedef void *boundssplay_tree_value;

/* Forward declaration for a node in the tree.  */
typedef struct boundssplay_tree_node_s *boundssplay_tree_node;

/* The type of a function used to iterate over the tree.  */
typedef int (*boundssplay_tree_foreach_fn) (boundssplay_tree_node, void *);

/* ------------------------------------------------------------------------ */
/* Splay-tree structure */

/* The nodes in the splay tree.  */
struct boundssplay_tree_node_s
{
  /* Data.  */
  boundssplay_tree_key key;
  boundssplay_tree_value value;
  /* Children.  */
  boundssplay_tree_node left;
  boundssplay_tree_node right;
  /* XXX: The addition of a parent pointer may eliminate some recursion.  */
};

/* The splay tree itself.  */
struct boundssplay_tree_s
{
  /* The root of the tree.  */
  boundssplay_tree_node root;

  /* The last key value for which the tree has been splayed, but not
     since modified.  */
  boundssplay_tree_key last_splayed_key;
  int last_splayed_key_p;

  /* Statistics.  */
  unsigned num_keys;

  /* Traversal recursion control flags.  */
  unsigned max_depth;
  unsigned depth;
  unsigned rebalance_p;
};
typedef struct boundssplay_tree_s *boundssplay_tree;

/* ------------------------------------------------------------------------ */
/* Splay-tree API */

extern boundssplay_tree 
boundssplay_tree_new (void);

extern boundssplay_tree_node 
boundssplay_tree_insert (boundssplay_tree, boundssplay_tree_key, boundssplay_tree_value);

extern void 
boundssplay_tree_remove (boundssplay_tree, boundssplay_tree_key);

extern boundssplay_tree_node 
boundssplay_tree_lookup (boundssplay_tree, boundssplay_tree_key);

extern boundssplay_tree_node 
boundssplay_tree_predecessor (boundssplay_tree, boundssplay_tree_key);

extern boundssplay_tree_node 
boundssplay_tree_successor (boundssplay_tree, boundssplay_tree_key);

extern int 
boundssplay_tree_foreach (boundssplay_tree, boundssplay_tree_foreach_fn, void *);

extern void 
boundssplay_tree_rebalance (boundssplay_tree sp);

#endif /* _SPLAY_TREE_H */
