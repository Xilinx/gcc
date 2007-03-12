/* Tree pattern matching and checking using concrete syntax.
  Copyright (C) 2006
  Free Software Foundation, Inc.
  Contributed by Nic Volanschi <nic.volanschi@free.fr>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* At the statement level, a node in the CFG has the following type: */
typedef struct tree_statement_list_node *cfg_node;

DEF_VEC_P (cfg_node);
DEF_VEC_ALLOC_P (cfg_node, heap);

/* Accessor for a CFG node */
static inline tree
cfg_node_stmt (cfg_node node)
{
  return node->stmt;
}

/* Map a statement iterator to a CFG node */
static inline cfg_node
bsi_cfg_node (block_stmt_iterator bsi)
{
  return bsi.tsi.ptr;
}

/* Get 1st CFG node in a basic block */
static inline cfg_node
bb_1st_cfg_node (basic_block bb)
{
  return STATEMENT_LIST_HEAD (bb->stmt_list);
}

/* Patterns are of 2 kinds:
   1. with named holes, as in: "lock(%X, %Y)"
   2. with anonymous holes, as in: tree_scanf(t, "lock(%t, %t)", &t1, &t2)
 */
typedef struct patt_info_s {
  const char *format_spec;
  va_list *args_ptr; /* only used for anomymous holes */
  struct patt_info_s *next;
} patt_info;

typedef patt_info *pattern;

/* Atomic pattern constructor */
static inline pattern 
mkpat (const char *str) 
{
  pattern res = xmalloc (sizeof (patt_info));
  res->format_spec = xstrdup (str);
  res->next = NULL;
  res->args_ptr = NULL;
  return res;
}

/* Disjunctive pattern constructor */
static inline pattern 
pat_or (pattern p1, pattern p2) 
{
  p1->next = p2;
  return p1;
}

/* Pattern destructor */
static inline void 
rmpat (pattern p) 
{
  if (p->next)
    rmpat (p->next);
  free ((char *) p->format_spec);
  free (p);
}

/* Display a pattern to stderr */
static inline void 
pat_print (pattern p) 
{
  if (!p)
    return;

  if (!p->next) 
    fprintf (stderr, "\"%s\"", p->format_spec);
  else
    {
      fprintf (stderr, "\"%s\" or ", p->format_spec);
      pat_print (p->next);
    }
}

/* A "hole" is a pattern variable (aka meta-variable). It contains a
   tree (which is NULL when the var is unistantiated), and a CFG
   "context" node which points to the stmt containing the tree. The
   context is useful to interpret temporary variables, or other
   dataflow data.
 */
typedef struct hole_s {
  tree tree;
  cfg_node ctx;
} hole;

typedef hole *hole_p;

DEF_VEC_P (hole_p);
DEF_VEC_ALLOC_P (hole_p, heap);

/* Named local holes are noted by a single lowercase letter */
#define LOCAL_MAX 26
/* Named global holes are noted by a single capital letter */
#define GLOBAL_MAX 26
/* Named local holes are used locally within each pattern. Thus, a
   variable %x may have different values in different patterns. */
extern hole local_holes[LOCAL_MAX];
/* Named global holes are shared between all the patterns in a same
   property. Thus, a variable %X has the same value in different
   patterns. */
extern hole global_holes[GLOBAL_MAX];

extern bool is_global_hole (char c);
extern hole *get_hole_named (char c);
extern void reset_local_holes (void);
extern void reset_global_holes (void);
extern hole *save_global_holes (void);
extern void restore_global_holes (hole *saved);
extern bool eq_global_holes (hole *holes1, hole *holes2);
extern void print_local_holes (void);
extern void print_global_holes (void);

/* User interface to AST pattern matching */
extern bool tree_scanf (tree t, const char *fmt, cfg_node ctx_node, ...);
extern bool tree_match (tree t, const char *fmt, cfg_node ctx_node);
extern bool tree_match_disj (tree t, pattern fmt, cfg_node ctx_node);

/* A "condate" is a program property involving CONtrol, DATa, and
   other aspects such as syntactic and semantic information. For
   example, '[there is a path] from "lock(%X)" to "unlock(%X)"
   avoiding "return" or "return %_"' is a condate specifying
   lock-unlock bugs.  Thus, condates are built upon patterns, CFG and
   dataflow information.
 */
typedef struct condate_s {
  char *name;    /* Used to identify the condate in warning messages. */
  pattern from;  /* Paths start at nodes matching this pattern. */
  pattern to;    /* Paths end at nodes matching this pattern. */
  pattern avoid; /* Paths must avoid nodes matching: this pattern, */
  pattern avoid_then; /* ... successful conditions matching this pattern, */
  pattern avoid_else; /* ... and unsuccessful conditions mathing this one. */
} *condate;

/* Condate constructor */
static inline condate 
mkcond (const char *name, pattern from, pattern to, pattern avoid, 
	pattern avoid_then, pattern avoid_else) 
{ 
  condate cond = xmalloc (sizeof (struct condate_s));
  if (name)
    cond->name = xstrdup (name);
  else 
    cond->name = NULL;
  cond->from = from; 
  cond->to = to; 
  cond->avoid = avoid; 
  cond->avoid_then = avoid_then; 
  cond->avoid_else = avoid_else;
  return cond;
}

/* Condate destructor */
static inline void 
rmcond (condate cond) 
{
  if (cond->name)
    free (cond->name);
  rmpat (cond->from);
  rmpat (cond->to);
  rmpat (cond->avoid);
  rmpat (cond->avoid_then);
  rmpat (cond->avoid_else);
  free (cond);
}

extern void print_cond (condate cond);

/* Tracing levels & macros */
enum trace_level {
  TRACE_ALWAYS = 0,
  TRACE_CHECK,
  TRACE_CHECK_STEPS,
  TRACE_MATCH,
  TRACE_MATCH_STEPS
};

/* current tracing level */
#define pp_trace_level TRACE_ALWAYS
/* use this macro to conditionally execute a tracing action */
#define PP_TRACE(lev, stmt) if (lev <= pp_trace_level) stmt;
