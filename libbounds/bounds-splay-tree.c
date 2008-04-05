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

#include <stdio.h>
#include "bounds-splay-tree.h"
#include "bounds-impl.h"

/* Adapted splay tree code, originally from libiberty.  It has been
   specialized for libmudflap as requested by RMS.  */

static void
boundssplay_tree_free (void *p)
{
  DECLARE (void, free, void *p);
  CALL_REAL (free, p);
}

static void *
boundssplay_tree_xmalloc (size_t s)
{
  DECLARE (void *, malloc, size_t s);
  return CALL_REAL (malloc, s);
}

static void boundssplay_tree_splay (boundssplay_tree, boundssplay_tree_key);
static boundssplay_tree_node boundssplay_tree_splay_helper (boundssplay_tree,
                                                boundssplay_tree_key,
                                                boundssplay_tree_node *,
                                                boundssplay_tree_node *,
                                                boundssplay_tree_node *);


/* Help splay SP around KEY.  PARENT and GRANDPARENT are the parent
   and grandparent, respectively, of NODE.  */

static boundssplay_tree_node
boundssplay_tree_splay_helper (boundssplay_tree sp,
                         boundssplay_tree_key key,
                         boundssplay_tree_node * node,
                         boundssplay_tree_node * parent,
                         boundssplay_tree_node * grandparent)
{
  boundssplay_tree_node *next;
  boundssplay_tree_node n;
  int comparison;

  n = *node;

  if (!n)
    return *parent;

  comparison = ((key > n->key) ? 1 : ((key < n->key) ? -1 : 0));

  if (comparison == 0)
    /* We've found the target.  */
    next = 0;
  else if (comparison < 0)
    /* The target is to the left.  */
    next = &n->left;
  else
    /* The target is to the right.  */
    next = &n->right;

  if (next)
    {
      /* Check whether our recursion depth is too high.  Abort this search,
         and signal that a rebalance is required to continue.  */
      if (sp->depth > sp->max_depth)
        {
          sp->rebalance_p = 1;
          return n;
         }

      /* Continue down the tree.  */
      sp->depth ++;
      n = boundssplay_tree_splay_helper (sp, key, next, node, parent);
      sp->depth --;

      /* The recursive call will change the place to which NODE
         points.  */
      if (*node != n || sp->rebalance_p)
        return n;
    }

  if (!parent)
    /* NODE is the root.  We are done.  */
    return n;

  /* First, handle the case where there is no grandparent (i.e.,
   *PARENT is the root of the tree.)  */
  if (!grandparent)
    {
      if (n == (*parent)->left)
        {
          *node = n->right;
          n->right = *parent;
        }
      else
        {
          *node = n->left;
          n->left = *parent;
        }
      *parent = n;
      return n;
    }

  /* Next handle the cases where both N and *PARENT are left children,
     or where both are right children.  */
  if (n == (*parent)->left && *parent == (*grandparent)->left)
    {
      boundssplay_tree_node p = *parent;

      (*grandparent)->left = p->right;
      p->right = *grandparent;
      p->left = n->right;
      n->right = p;
      *grandparent = n;
      return n;
    }
  else if (n == (*parent)->right && *parent == (*grandparent)->right)
    {
      boundssplay_tree_node p = *parent;

      (*grandparent)->right = p->left;
      p->left = *grandparent;
      p->right = n->left;
      n->left = p;
      *grandparent = n;
      return n;
    }

  /* Finally, deal with the case where N is a left child, but *PARENT
     is a right child, or vice versa.  */
  if (n == (*parent)->left)
    {
      (*parent)->left = n->right;
      n->right = *parent;
      (*grandparent)->right = n->left;
      n->left = *grandparent;
      *grandparent = n;
      return n;
    }
  else
    {
      (*parent)->right = n->left;
      n->left = *parent;
      (*grandparent)->left = n->right;
      n->right = *grandparent;
      *grandparent = n;
      return n;
    }
}



static int
boundssplay_tree_rebalance_helper1 (boundssplay_tree_node n, void *array_ptr)
{
  boundssplay_tree_node **p = array_ptr;
  *(*p) = n;
  (*p)++;
  return 0;
}


static boundssplay_tree_node
boundssplay_tree_rebalance_helper2 (boundssplay_tree_node * array, unsigned low,
                              unsigned high)
{
  unsigned middle = low + (high - low) / 2;
  boundssplay_tree_node n = array[middle];

  /* Note that since we're producing a balanced binary tree, it is not a problem
     that this function is recursive.  */
  if (low + 1 <= middle)
    n->left = boundssplay_tree_rebalance_helper2 (array, low, middle - 1);
  else
    n->left = NULL;

  if (middle + 1 <= high)
    n->right = boundssplay_tree_rebalance_helper2 (array, middle + 1, high);
  else
    n->right = NULL;

  return n;
}


/* Rebalance the entire tree.  Do this by copying all the node
   pointers into an array, then cleverly re-linking them.  */
void
boundssplay_tree_rebalance (boundssplay_tree sp)
{
  boundssplay_tree_node *all_nodes, *all_nodes_1;

  if (sp->num_keys <= 2)
    return;

  all_nodes = boundssplay_tree_xmalloc (sizeof (boundssplay_tree_node) * sp->num_keys);

  /* Traverse all nodes to copy their addresses into this array.  */
  all_nodes_1 = all_nodes;
  boundssplay_tree_foreach (sp, boundssplay_tree_rebalance_helper1,
                      (void *) &all_nodes_1);

  /* Relink all the nodes.  */
  sp->root = boundssplay_tree_rebalance_helper2 (all_nodes, 0, sp->num_keys - 1);

  boundssplay_tree_free (all_nodes);
}


/* Splay SP around KEY.  */
static void
boundssplay_tree_splay (boundssplay_tree sp, boundssplay_tree_key key)
{
  if (sp->root == 0)
    return;

  /* If we just splayed the tree with the same key, do nothing.  */
  if (sp->last_splayed_key_p &&
      (sp->last_splayed_key == key))
    return;

  /* Compute a maximum recursion depth for a splay tree with NUM nodes.
     The idea is to limit excessive stack usage if we're facing
     degenerate access patterns.  Unfortunately such patterns can occur
     e.g. during static initialization, where many static objects might
     be registered in increasing address sequence, or during a case where
     large tree-like heap data structures are allocated quickly.

     On x86, this corresponds to roughly 200K of stack usage.
     XXX: For libmudflapth, this could be a function of __mf_opts.thread_stack.  */
  sp->max_depth = 2500;
  sp->rebalance_p = sp->depth = 0;

  boundssplay_tree_splay_helper (sp, key, &sp->root, NULL, NULL);
  if (sp->rebalance_p)
    {
      boundssplay_tree_rebalance (sp);

      sp->rebalance_p = sp->depth = 0;
      boundssplay_tree_splay_helper (sp, key, &sp->root, NULL, NULL);

      if (sp->rebalance_p)
        abort ();
    }


  /* Cache this splay key. */
  sp->last_splayed_key = key;
  sp->last_splayed_key_p = 1;
}



/* Allocate a new splay tree.  */
boundssplay_tree
boundssplay_tree_new ()
{
  boundssplay_tree sp = boundssplay_tree_xmalloc (sizeof (struct boundssplay_tree_s));
  sp->root = NULL;
  sp->last_splayed_key_p = 0;
  sp->num_keys = 0;

  return sp;
}



/* Insert a new node (associating KEY with DATA) into SP.  If a
   previous node with the indicated KEY exists, its data is replaced
   with the new value.  Returns the new node.  */
boundssplay_tree_node
boundssplay_tree_insert (boundssplay_tree sp, boundssplay_tree_key key, boundssplay_tree_value value)
{
  int comparison = 0;

  boundssplay_tree_splay (sp, key);

  if (sp->root)
    comparison = ((sp->root->key > key) ? 1 :
                  ((sp->root->key < key) ? -1 : 0));

  if (sp->root && comparison == 0)
    {
      /* If the root of the tree already has the indicated KEY, just
         replace the value with VALUE.  */
      sp->root->value = value;
    }
  else
    {
      /* Create a new node, and insert it at the root.  */
      boundssplay_tree_node node;

      node = boundssplay_tree_xmalloc (sizeof (struct boundssplay_tree_node_s));
      node->key = key;
      node->value = value;
      sp->num_keys++;
      if (!sp->root)
        node->left = node->right = 0;
      else if (comparison < 0)
        {
          node->left = sp->root;
          node->right = node->left->right;
          node->left->right = 0;
        }
      else
        {
          node->right = sp->root;
          node->left = node->right->left;
          node->right->left = 0;
        }

      sp->root = node;
      sp->last_splayed_key_p = 0;
    }

  return sp->root;
}

/* Remove KEY from SP.  It is not an error if it did not exist.  */

void
boundssplay_tree_remove (boundssplay_tree sp, boundssplay_tree_key key)
{
  boundssplay_tree_splay (sp, key);
  sp->last_splayed_key_p = 0;
  if (sp->root && (sp->root->key == key))
    {
      boundssplay_tree_node left, right;
      left = sp->root->left;
      right = sp->root->right;
      /* Delete the root node itself.  */
      boundssplay_tree_free (sp->root);
      sp->num_keys--;
      /* One of the children is now the root.  Doesn't matter much
         which, so long as we preserve the properties of the tree.  */
      if (left)
        {
          sp->root = left;
          /* If there was a right child as well, hang it off the
             right-most leaf of the left child.  */
          if (right)
            {
              while (left->right)
                left = left->right;
              left->right = right;
            }
        }
      else
        sp->root = right;
    }
}

/* Lookup KEY in SP, returning VALUE if present, and NULL
   otherwise.  */

boundssplay_tree_node
boundssplay_tree_lookup (boundssplay_tree sp, boundssplay_tree_key key)
{
  boundssplay_tree_splay (sp, key);
  if (sp->root && (sp->root->key == key))
    return sp->root;
  else
    return 0;
}


/* Return the immediate predecessor KEY, or NULL if there is no
   predecessor.  KEY need not be present in the tree.  */

boundssplay_tree_node
boundssplay_tree_predecessor (boundssplay_tree sp, boundssplay_tree_key key)
{
  int comparison;
  boundssplay_tree_node node;
  /* If the tree is empty, there is certainly no predecessor.  */
  if (!sp->root)
    return NULL;
  /* Splay the tree around KEY.  That will leave either the KEY
     itself, its predecessor, or its successor at the root.  */
  boundssplay_tree_splay (sp, key);
  comparison = ((sp->root->key > key) ? 1 :
                ((sp->root->key < key) ? -1 : 0));

  /* If the predecessor is at the root, just return it.  */
  if (comparison < 0)
    return sp->root;
  /* Otherwise, find the rightmost element of the left subtree.  */
  node = sp->root->left;
  if (node)
    while (node->right)
      node = node->right;
  return node;
}

/* Return the immediate successor KEY, or NULL if there is no
   successor.  KEY need not be present in the tree.  */

boundssplay_tree_node
boundssplay_tree_successor (boundssplay_tree sp, boundssplay_tree_key key)
{
  int comparison;
  boundssplay_tree_node node;
  /* If the tree is empty, there is certainly no successor.  */
  if (!sp->root)
    return NULL;
  /* Splay the tree around KEY.  That will leave either the KEY
     itself, its predecessor, or its successor at the root.  */
  boundssplay_tree_splay (sp, key);
  comparison = ((sp->root->key > key) ? 1 :
                ((sp->root->key < key) ? -1 : 0));
  /* If the successor is at the root, just return it.  */
  if (comparison > 0)
    return sp->root;
  /* Otherwise, find the leftmost element of the right subtree.  */
  node = sp->root->right;
  if (node)
    while (node->left)
      node = node->left;
  return node;
}

/* Call FN, passing it the DATA, for every node in SP, following an
   in-order traversal.  If FN every returns a non-zero value, the
   iteration ceases immediately, and the value is returned.
   Otherwise, this function returns 0.

   This function simulates recursion using dynamically allocated
   arrays, since it may be called from boundssplay_tree_rebalance(), which
   in turn means that the tree is already uncomfortably deep for stack
   space limits.  */
int
boundssplay_tree_foreach (boundssplay_tree st, boundssplay_tree_foreach_fn fn, void *data)
{
  boundssplay_tree_node *stack1;
  char *stack2;
  unsigned sp;
  int val = 0;
  enum s { s_left, s_here, s_right, s_up };

  if (st->root == NULL) /* => num_keys == 0 */
    return 0;

  stack1 = boundssplay_tree_xmalloc (sizeof (boundssplay_tree_node) * st->num_keys);
  stack2 = boundssplay_tree_xmalloc (sizeof (char) * st->num_keys);

  sp = 0;
  stack1 [sp] = st->root;
  stack2 [sp] = s_left;

  while (1)
    {
      boundssplay_tree_node n;
      enum s s;

      n = stack1 [sp];
      s = stack2 [sp];

      /* Handle each of the four possible states separately.  */

      /* 1: We're here to traverse the left subtree (if any).  */
      if (s == s_left)
        {
          stack2 [sp] = s_here;
          if (n->left != NULL)
            {
              sp ++;
              stack1 [sp] = n->left;
              stack2 [sp] = s_left;
            }
        }

      /* 2: We're here to traverse this node.  */
      else if (s == s_here)
        {
          stack2 [sp] = s_right;
          val = (*fn) (n, data);
          if (val) break;
        }

      /* 3: We're here to traverse the right subtree (if any).  */
      else if (s == s_right)
        {
          stack2 [sp] = s_up;
          if (n->right != NULL)
            {
              sp ++;
              stack1 [sp] = n->right;
              stack2 [sp] = s_left;
            }
        }

      /* 4: We're here after both subtrees (if any) have been traversed.  */
      else if (s == s_up)
        {
          /* Pop the stack.  */
          if (sp == 0) break; /* Popping off the root note: we're finished!  */
          sp --;
        }

      else
        abort ();
    }

  boundssplay_tree_free (stack1);
  boundssplay_tree_free (stack2);
  return val;
}
