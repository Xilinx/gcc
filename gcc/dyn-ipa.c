/* Compile this one with gcc.  */
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

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "tconfig.h"
#include "tsystem.h"
#include "coretypes.h"
#include "tm.h"

#if defined(inhibit_libc)
#define IN_LIBGCOV (-1)
#else
#undef NULL /* Avoid errors if stdio.h and our stddef.h mismatch.  */
#include <stdio.h>
#include <stdlib.h>
#define IN_LIBGCOV 1
#if defined(L_gcov)
#define GCOV_LINKAGE /* nothing */
#endif
#endif
#include "gcov-io.h"

struct dyn_pointer_set;
struct dyn_vect;

#define XNEWVEC(type,ne) (type *)malloc(sizeof(type) * (ne))
#define XNEW(type) (type *)malloc(sizeof(type))
#define XDELETEVEC(p) free(p)
#define XDELETE(p) free(p)

struct dyn_cgraph_node
{
  struct dyn_cgraph_edge *callees;
  struct dyn_cgraph_edge *callers;
  struct dyn_pointer_set *imported_modules;

  gcov_type guid;
  gcov_unsigned_t visited;
};

struct dyn_cgraph_edge
{
  struct dyn_cgraph_node *caller;
  struct dyn_cgraph_node *callee;
  struct dyn_cgraph_edge *next_caller;
  struct dyn_cgraph_edge *next_callee;
  gcov_type count;
};

struct dyn_module_info
{
  struct dyn_pointer_set *imported_modules;
  gcov_unsigned_t max_func_ident;
};

struct dyn_cgraph
{
  struct dyn_cgraph_node **call_graph_nodes;
  struct gcov_info **modules;
  /* supplement module information  */
  struct dyn_module_info *sup_modules;
  const struct gcov_fn_info ***functions;
  unsigned num_modules;
};

struct dyn_pointer_set
{
  size_t log_slots;
  size_t n_slots;		/* n_slots = 2^log_slots */
  size_t n_elements;

  const void **slots;
};


#if defined(inhibit_libc)
__gcov_build_callgraph (void) {}
#else

void __gcov_compute_module_groups (void) ATTRIBUTE_HIDDEN;
void __gcov_finalize_dyn_callgraph (void) ATTRIBUTE_HIDDEN;
static void gcov_dump_callgraph (gcov_type);
static void gcov_dump_cgraph_node_short (struct dyn_cgraph_node *node);
static void gcov_dump_cgraph_node (struct dyn_cgraph_node *node,
                                  unsigned m, unsigned f);
static void
gcov_dump_cgraph_node_dot (struct dyn_cgraph_node *node,
                           unsigned m, unsigned f,
                           gcov_type cutoff_count);
static void
pointer_set_destroy (struct dyn_pointer_set *pset);

static struct dyn_cgraph the_dyn_call_graph;

static void
init_dyn_cgraph_node (struct dyn_cgraph_node *node, gcov_type guid)
{
  node->callees = 0;
  node->callers = 0;
  node->imported_modules = 0;
  node->guid = guid;
  node->visited = 0;
}

/* Return (module_id - 1). FUNC_GUID is the global unique id.  */

static inline gcov_unsigned_t
get_module_idx_from_func_glob_uid (gcov_type func_guid)
{
  return EXTRACT_MODULE_ID_FROM_GLOBAL_ID (func_guid) - 1;
}

/* Return (module_id - 1) for MODULE_INFO.  */

static inline gcov_unsigned_t
get_module_idx (const struct gcov_info *module_info)
{
  return module_info->mod_info->ident - 1;
}

/* Return intra-module function id given function global unique id
   FUNC_GUID.  */

static inline gcov_unsigned_t
get_intra_module_func_id (gcov_type func_guid)
{
  return EXTRACT_FUNC_ID_FROM_GLOBAL_ID (func_guid);
}

/* Return the pointer to the dynamic call graph node for FUNC_GUID.  */

static inline struct dyn_cgraph_node *
get_cgraph_node (gcov_type func_guid)
{
  gcov_unsigned_t mod_id, func_id;

  mod_id = get_module_idx_from_func_glob_uid (func_guid);

  /* This is to workaround: calls in __static_initialization_and_destruction
     should not be instrumented as the module id context for the callees have
     not setup yet -- this leads to mod_id == (unsigned) (0 - 1). Multithreaded
     programs may also produce insane func_guid in the profile counter.  */
  if (mod_id >= the_dyn_call_graph.num_modules)
    return 0;

  func_id = get_intra_module_func_id (func_guid);
  if (func_id > the_dyn_call_graph.sup_modules[mod_id].max_func_ident)
    return 0;

  return &the_dyn_call_graph.call_graph_nodes[mod_id][func_id];
}

/* Return the gcov_info pointer for module with id MODULE_ID.  */

static inline struct gcov_info *
get_module_info (gcov_unsigned_t module_id)
{
  return the_dyn_call_graph.modules[module_id];
}

struct gcov_info *__gcov_list ATTRIBUTE_HIDDEN;

/* Initialize dynamic call graph.  */

static void
init_dyn_call_graph (void)
{
  unsigned num_modules = 0;
  struct gcov_info *gi_ptr;

  the_dyn_call_graph.call_graph_nodes = 0;
  the_dyn_call_graph.modules = 0;
  the_dyn_call_graph.functions = 0;

  gi_ptr = __gcov_list;

  for (; gi_ptr; gi_ptr = gi_ptr->next)
    num_modules++;

  the_dyn_call_graph.num_modules = num_modules;

  the_dyn_call_graph.modules
    = XNEWVEC (struct gcov_info *, num_modules);

  the_dyn_call_graph.sup_modules
    = XNEWVEC (struct dyn_module_info, num_modules);
  memset (the_dyn_call_graph.sup_modules, 0,
          num_modules * sizeof (struct dyn_module_info));

  the_dyn_call_graph.functions
    = XNEWVEC (const struct gcov_fn_info **, num_modules);

  the_dyn_call_graph.call_graph_nodes
    = XNEWVEC (struct dyn_cgraph_node *, num_modules);

  gi_ptr = __gcov_list;

  for (; gi_ptr; gi_ptr = gi_ptr->next)
    {
      unsigned c_ix = 0, t_ix, j, mod_id, fi_stride, max_func_ident = 0;
      struct dyn_cgraph_node *node;

      mod_id = get_module_idx (gi_ptr);

      the_dyn_call_graph.modules[mod_id] = gi_ptr;

      the_dyn_call_graph.functions[mod_id]
          = XNEWVEC (const struct gcov_fn_info *, gi_ptr->n_functions);

      for (t_ix = 0; t_ix < GCOV_COUNTERS; t_ix++)
	if ((1 << t_ix) & gi_ptr->ctr_mask)
	    c_ix++;

      fi_stride = sizeof (struct gcov_fn_info) + c_ix * sizeof (unsigned);
      if (__alignof__ (struct gcov_fn_info) > sizeof (unsigned))
	{
	  fi_stride += __alignof__ (struct gcov_fn_info) - 1;
	  fi_stride &= ~(__alignof__ (struct gcov_fn_info) - 1);
	}

      for (j = 0; j < gi_ptr->n_functions; j++)
	{
          const struct gcov_fn_info *fi_ptr = (const struct gcov_fn_info *)
	    ((const char *) gi_ptr->functions + j * fi_stride);
          the_dyn_call_graph.functions[mod_id][j] = fi_ptr;
          if (fi_ptr->ident > max_func_ident)
            max_func_ident = fi_ptr->ident;
        }


      the_dyn_call_graph.call_graph_nodes[mod_id]
          = XNEWVEC (struct dyn_cgraph_node, max_func_ident + 1);

      the_dyn_call_graph.sup_modules[mod_id].max_func_ident = max_func_ident;

      for (j = 0; j < max_func_ident + 1; j++)
        init_dyn_cgraph_node (&the_dyn_call_graph.call_graph_nodes[mod_id][j], 0);

      for (j = 0; j < gi_ptr->n_functions; j++)
	{
          const struct gcov_fn_info *fi_ptr 
              = the_dyn_call_graph.functions[mod_id][j];

	  node = &the_dyn_call_graph.call_graph_nodes[mod_id][fi_ptr->ident];
	  init_dyn_cgraph_node (node, GEN_FUNC_GLOBAL_ID (gi_ptr->mod_info->ident, 
                                                          fi_ptr->ident));
	}
    }
}

/* Free up memory allocated for dynamic call graph.  */

void
__gcov_finalize_dyn_callgraph (void)
{
  unsigned i;
  struct gcov_info *gi_ptr;

  for (i = 0; i < the_dyn_call_graph.num_modules; i++)
    {
      gi_ptr = the_dyn_call_graph.modules[i];
      const struct gcov_fn_info *fi_ptr;
      unsigned f_ix;
      for (f_ix = 0; f_ix < gi_ptr->n_functions; f_ix++)
	{
	  struct dyn_cgraph_node *node;
          struct dyn_cgraph_edge *callees, *next_callee;
          fi_ptr = the_dyn_call_graph.functions[i][f_ix];
          node = &the_dyn_call_graph.call_graph_nodes[i][fi_ptr->ident];
          callees = node->callees;

          if (!callees)
            continue;
          while (callees != 0)
            {
              next_callee = callees->next_callee;
              XDELETE (callees);
              callees = next_callee;
            }
	  if (node->imported_modules)
	    pointer_set_destroy (node->imported_modules);
        }
      if (the_dyn_call_graph.call_graph_nodes[i])
        XDELETEVEC (the_dyn_call_graph.call_graph_nodes[i]);
      if (the_dyn_call_graph.functions[i])
        XDELETEVEC (the_dyn_call_graph.functions[i]);
      /* Now delete sup modules */
      if (the_dyn_call_graph.sup_modules[i].imported_modules)
        pointer_set_destroy (the_dyn_call_graph.sup_modules[i].imported_modules);
    }
  XDELETEVEC (the_dyn_call_graph.call_graph_nodes);
  XDELETEVEC (the_dyn_call_graph.functions);
  XDELETEVEC (the_dyn_call_graph.sup_modules);
  XDELETEVEC (the_dyn_call_graph.modules);
}

/* Add outgoing edge OUT_EDGE for caller node CALLER.  */

static void
gcov_add_out_edge (struct dyn_cgraph_node *caller,
		   struct dyn_cgraph_edge *out_edge)
{
  if (!caller->callees)
    caller->callees = out_edge;
  else
    {
      out_edge->next_callee = caller->callees;
      caller->callees = out_edge;
    }
}

/* Add incoming edge IN_EDGE for callee node CALLEE.  */

static void
gcov_add_in_edge (struct dyn_cgraph_node *callee,
		  struct dyn_cgraph_edge *in_edge)
{
  if (!callee->callers)
    callee->callers = in_edge;
  else
    {
      in_edge->next_caller = callee->callers;
      callee->callers = in_edge;
    }
}

/* Add a call graph edge between caller CALLER and callee CALLEE.
   The edge count is COUNT.  */

static void
gcov_add_cgraph_edge (struct dyn_cgraph_node *caller,
		      struct dyn_cgraph_node *callee,
		      gcov_type count)
{
  struct dyn_cgraph_edge *new_edge = XNEW (struct dyn_cgraph_edge);
  new_edge->caller = caller;
  new_edge->callee = callee;
  new_edge->count = count;
  new_edge->next_caller = 0;
  new_edge->next_callee = 0;

  gcov_add_out_edge (caller, new_edge);
  gcov_add_in_edge (callee, new_edge);
}

/* Add call graph edges from direct calls for caller CALLER. DIR_CALL_COUNTERS
   is the array of call counters. N_COUNTS is the number of counters.  */

static void
gcov_build_callgraph_dc_fn (struct dyn_cgraph_node *caller,
                            gcov_type *dir_call_counters,
                            unsigned n_counts)
{
  unsigned i;

  for (i = 0; i < n_counts; i += 2)
    {
      struct dyn_cgraph_node *callee;
      gcov_type count;
      gcov_type callee_guid = dir_call_counters[i];

      count = dir_call_counters[i + 1];
      if (count == 0)
        continue;
      callee = get_cgraph_node (callee_guid);
      if (!callee)
        continue;
      gcov_add_cgraph_edge (caller, callee, count);
    }
}

/* Add call graph edges from indirect calls for caller CALLER. ICALL_COUNTERS
   is the array of icall counters. N_COUNTS is the number of counters.  */

static void
gcov_build_callgraph_ic_fn (struct dyn_cgraph_node *caller,
                            gcov_type *icall_counters,
                            unsigned n_counts)
{
  unsigned i, j;

  for (i = 0; i < n_counts; i += GCOV_ICALL_TOPN_NCOUNTS)
    {
      gcov_type *value_array = &icall_counters[i + 1];
      for (j = 0; j < GCOV_ICALL_TOPN_NCOUNTS - 1; j += 2)
        {
          struct dyn_cgraph_node *callee;
          gcov_type count;
          gcov_type callee_guid = value_array[j];

          count = value_array[j + 1];
          if (count == 0)
            continue;
          callee = get_cgraph_node (callee_guid);
          if (!callee)
            continue;
          gcov_add_cgraph_edge (caller, callee, count);
        }
    }
}

/* Build the dynamic call graph.  */

static void
gcov_build_callgraph (void)
{
  struct gcov_info *gi_ptr;
  unsigned t_ix, m_ix;

  init_dyn_call_graph ();

  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      const struct gcov_fn_info *fi_ptr;
      unsigned c_ix, f_ix, n_counts, dp_cix = 0, ip_cix = 0;
      gcov_type *dcall_profile_values, *icall_profile_values;

      gi_ptr = the_dyn_call_graph.modules[m_ix];

      dcall_profile_values = 0;
      icall_profile_values = 0;
      c_ix = 0;
      for (t_ix = 0; t_ix < GCOV_COUNTERS; t_ix++)
	if ((1 << t_ix) & gi_ptr->ctr_mask)
	  {
	    if (t_ix == GCOV_COUNTER_DIRECT_CALL)
	      {
		dcall_profile_values = gi_ptr->counts[c_ix].values;
		dp_cix = c_ix;
	      }
	    if (t_ix == GCOV_COUNTER_ICALL_TOPNV)
	      {
		icall_profile_values = gi_ptr->counts[c_ix].values;
		ip_cix = c_ix;
	      }
	    c_ix++;
	  }

      if (dcall_profile_values == 0 && icall_profile_values == 0)
        continue;

      for (f_ix = 0; f_ix < gi_ptr->n_functions; f_ix++)
        {
          struct dyn_cgraph_node *caller;
          fi_ptr = the_dyn_call_graph.functions[m_ix][f_ix];
          caller = &the_dyn_call_graph.call_graph_nodes[m_ix][fi_ptr->ident];
          if (dcall_profile_values)
            {
              n_counts = fi_ptr->n_ctrs[dp_cix];
              gcov_build_callgraph_dc_fn (caller, dcall_profile_values, n_counts);
              dcall_profile_values += n_counts;
            }
          if (icall_profile_values)
            {
              n_counts = fi_ptr->n_ctrs[ip_cix];
              gcov_build_callgraph_ic_fn (caller, icall_profile_values, n_counts);
              icall_profile_values += n_counts;
            }
        }
    }

}

static inline size_t
hash1 (const void *p, unsigned long max, unsigned long logmax)
{
  const unsigned long long A = 0x9e3779b97f4a7c16ull;
  const unsigned long long shift = 64 - logmax;

  return ((A * (unsigned long) p) >> shift) & (max - 1);
}

/* Allocate an empty pointer set.  */

static struct dyn_pointer_set *
pointer_set_create (void)
{
  struct dyn_pointer_set *result = XNEW (struct dyn_pointer_set);

  result->n_elements = 0;
  result->log_slots = 8;
  result->n_slots = (size_t) 1 << result->log_slots;

  result->slots = XNEWVEC (const void *, result->n_slots);
  memset (result->slots, 0, sizeof (const void *) * result->n_slots);
  return result;
}

/* Reclaim all memory associated with PSET.  */

static void
pointer_set_destroy (struct dyn_pointer_set *pset)
{
  XDELETEVEC (pset->slots);
  XDELETE (pset);
}

/* Subroutine of pointer_set_insert.  Return the insertion slot for P into
   an empty element of SLOTS, an array of length N_SLOTS.  */
static inline size_t
insert_aux (const void *p, const void **slots, size_t n_slots, size_t log_slots)
{
  size_t n = hash1 (p, n_slots, log_slots);
  while (1)
    {
      if (slots[n] == p || slots[n] == 0)
	return n;
      else
	{
	  ++n;
	  if (n == n_slots)
	    n = 0;
	}
    }
}

/* Insert P into PSET if it wasn't already there.  Returns nonzero
   if it was already there. P must be nonnull.  */

static int
pointer_set_insert (struct dyn_pointer_set *pset, const void *p)
{
  size_t n;

  /* For simplicity, expand the set even if P is already there.  This can be
     superfluous but can happen at most once.  */
  if (pset->n_elements > pset->n_slots / 4)
    {
      size_t new_log_slots = pset->log_slots + 1;
      size_t new_n_slots = pset->n_slots * 2;
      const void **new_slots = XNEWVEC (const void *, new_n_slots);
      memset (new_slots, 0, sizeof (const void*) * new_n_slots);
      size_t i;

      for (i = 0; i < pset->n_slots; ++i)
        {
	  const void *value = pset->slots[i];
	  n = insert_aux (value, new_slots, new_n_slots, new_log_slots);
	  new_slots[n] = value;
	}

      XDELETEVEC (pset->slots);
      pset->n_slots = new_n_slots;
      pset->log_slots = new_log_slots;
      pset->slots = new_slots;
    }

  n = insert_aux (p, pset->slots, pset->n_slots, pset->log_slots);
  if (pset->slots[n])
    return 1;

  pset->slots[n] = p;
  ++pset->n_elements;
  return 0;
}

/* Pass each pointer in PSET to the function in FN, together with the fixed
   parameter DATA.  If FN returns false, the iteration stops.  */

static void
pointer_set_traverse (const struct dyn_pointer_set *pset,
                      int (*fn) (const void *, void *), void *data)
{
  size_t i;
  for (i = 0; i < pset->n_slots; ++i)
    if (pset->slots[i] && !fn (pset->slots[i], data))
      break;
}

/* Callback function to propagate import module set (VALUE) from callee to
   caller (DATA).  */
static int
gcov_propagate_imp_modules (const void *value, void *data)
{
  struct dyn_pointer_set *receiving_set
      = (struct dyn_pointer_set *) data;

  pointer_set_insert (receiving_set, value);
  return 1;
}

static int
sort_by_count (const void *pa, const void *pb)
{
  const struct dyn_cgraph_edge *edge_a = *(struct dyn_cgraph_edge * const *)pa;
  const struct dyn_cgraph_edge *edge_b = *(struct dyn_cgraph_edge * const *)pb;

  /* This can overvlow.  */
  /* return edge_b->count - edge_a->count;  */
  if (edge_b->count > edge_a->count)
    return 1;
  else if (edge_b->count == edge_a->count)
    return 0;
  else
    return -1;
}

/* Compute the hot callgraph edge threhold.  */

static gcov_type
gcov_compute_cutoff_count (void)
{
  unsigned m_ix, capacity, i;
  unsigned num_edges = 0;
  gcov_type cutoff_count;
  double total, cum, cum_cutoff;
  struct dyn_cgraph_edge **edges;
  struct gcov_info *gi_ptr;
  char *cutoff_str;
  char *num_perc_str;
  unsigned cutoff_perc;
  unsigned num_perc;
  int do_dump;

  capacity = 100;
  /* allocate an edge array */
  edges = XNEWVEC (struct dyn_cgraph_edge*, capacity);
  /* First count the number of edges.  */
  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      const struct gcov_fn_info *fi_ptr;
      unsigned f_ix;

      gi_ptr = the_dyn_call_graph.modules[m_ix];

      for (f_ix = 0; f_ix < gi_ptr->n_functions; f_ix++)
	{
	  struct dyn_cgraph_node *node;
          struct dyn_cgraph_edge *callees;

	  fi_ptr = the_dyn_call_graph.functions[m_ix][f_ix];

	  node = &the_dyn_call_graph.call_graph_nodes[m_ix][fi_ptr->ident];

          callees = node->callees;
          while (callees != 0)
            {
              num_edges++;
              if (num_edges < capacity)
                edges[num_edges - 1] = callees;
              else
                {
                  capacity = capacity + (capacity >> 1);
                  edges = (struct dyn_cgraph_edge **)realloc (edges, sizeof (void*) * capacity);
                  edges[num_edges - 1] = callees;
                }
              callees = callees->next_callee;
            }
	}
    }

  /* Now sort */
 qsort (edges, num_edges, sizeof (void *), sort_by_count);
#define CUM_CUTOFF_PERCENT 95
#define MIN_NUM_EDGE_PERCENT 0
  cutoff_str = getenv ("GCOV_DYN_CGRAPH_CUTOFF");
  if (cutoff_str && strlen (cutoff_str))
    {
      if ((num_perc_str = strchr (cutoff_str, ':')))
        {
          *num_perc_str = '\0';
          num_perc_str++;
        }
      cutoff_perc = atoi (cutoff_str);
      if (num_perc_str)
        num_perc = atoi (num_perc_str);
      else
        num_perc = MIN_NUM_EDGE_PERCENT;
    }
  else
    {
      cutoff_perc = CUM_CUTOFF_PERCENT;
      num_perc = MIN_NUM_EDGE_PERCENT;
    }

  total = 0;
  cum = 0;
  for (i = 0; i < num_edges; i++)
    total += edges[i]->count;

  cum_cutoff = (total * cutoff_perc)/100;
  do_dump = (getenv ("GCOV_DYN_CGRAPH_DUMP") != 0);
  for (i = 0; i < num_edges; i++)
    {
      cum += edges[i]->count;
      if (do_dump)
        fprintf (stderr, "// edge[%d] count = %.0f [%llx --> %llx]\n",
                 i, (double) edges[i]->count,
                 (long long) edges[i]->caller->guid,
                 (long long) edges[i]->callee->guid);
      if (cum >= cum_cutoff && (i * 100 >= num_edges * num_perc))
        {
          cutoff_count = edges[i]->count;
          break;
        }
    }

  if (do_dump)
    fprintf (stderr, "//total = %.0f cum = %.0f cum/total = %.0f%%"
             " cutoff_count = %lld [total edges: %d hot edges: %d perc: %d%%]\n",
             total, cum, (cum * 100)/total, (long long) cutoff_count,
             num_edges, i, (i * 100)/num_edges);

  XDELETEVEC (edges);
  return cutoff_count;
}

/* Return the imported module set for NODE.  */

static struct dyn_pointer_set *
gcov_get_imp_module_set (struct dyn_cgraph_node *node)
{
  if (!node->imported_modules)
    node->imported_modules = pointer_set_create ();

  return node->imported_modules;
}

/* Return the imported module set for MODULE MI.  */

static struct dyn_pointer_set *
gcov_get_module_imp_module_set (struct dyn_module_info *mi)
{
  if (!mi->imported_modules)
    mi->imported_modules = pointer_set_create ();

  return mi->imported_modules;
}

/* Callback function to mark if a module needs to be exported.  */

static int
gcov_mark_export_modules (const void *value, void *data ATTRIBUTE_UNUSED)
{
  const struct gcov_info *module_info
      = (const struct gcov_info *)value;

  module_info->mod_info->is_exported = 1;
  return 1;
}

struct gcov_import_mod_array
{
  const struct gcov_info **imported_modules;
  struct gcov_info *importing_module;
  unsigned len;
};

/* Callback function to compute pointer set size.  */

static int
gcov_compute_pset_size (const void *value ATTRIBUTE_UNUSED,
                        void *data)
{
  unsigned *len = (unsigned *) data;
  (*len)++;
  return 1;
}

/* Callback function to collect imported modules.  */

static int
gcov_collect_imported_modules (const void *value, void *data)
{
  struct gcov_import_mod_array *out_array;
  const struct gcov_info *module_info
      = (const struct gcov_info *)value;

  out_array = (struct gcov_import_mod_array *) data;

  if (module_info != out_array->importing_module)
    out_array->imported_modules[out_array->len++] = module_info;

  return 1;
}

/* Comparitor for sorting imported modules using module ids.  */

static int
sort_by_module_id (const void *pa, const void *pb)
{
  const struct gcov_info *m_a = *(struct gcov_info * const *)pa;
  const struct gcov_info *m_b = *(struct gcov_info * const *)pb;

  return (int) m_a->mod_info->ident - (int) m_b->mod_info->ident;
}

/* Return a dynamic array of imported modules that is sorted for
   the importing module MOD_INFO. The length of the array is returned
   in *LEN.  */

const struct gcov_info **
gcov_get_sorted_import_module_array (struct gcov_info *mod_info,
                                     unsigned *len)
{
  unsigned mod_id;
  struct dyn_module_info *sup_mod_info;
  unsigned array_len = 0;
  struct gcov_import_mod_array imp_array;

  mod_id = get_module_idx (mod_info);
  sup_mod_info = &the_dyn_call_graph.sup_modules[mod_id];

  if (sup_mod_info->imported_modules == 0)
    return 0;

  pointer_set_traverse (sup_mod_info->imported_modules,
                        gcov_compute_pset_size, &array_len);
  imp_array.imported_modules = XNEWVEC (const struct gcov_info *, array_len);
  imp_array.len = 0;
  imp_array.importing_module = mod_info;
  pointer_set_traverse (sup_mod_info->imported_modules,
                        gcov_collect_imported_modules, &imp_array);
  *len = imp_array.len;
  qsort (imp_array.imported_modules, imp_array.len,
         sizeof (void *), sort_by_module_id);
  return imp_array.imported_modules;
}

/* Compute modules that are needed for NODE (for cross module inlining).
   CUTTOFF_COUNT is the call graph edge count cutoff value.  */

static void
gcov_process_cgraph_node (struct dyn_cgraph_node *node,
                          gcov_type cutoff_count)
{
  unsigned mod_id;
  struct dyn_cgraph_edge *callees;
  node->visited = 1;

  callees = node->callees;
  mod_id = get_module_idx_from_func_glob_uid (node->guid);

  while (callees)
    {
      if (!callees->callee->visited)
        gcov_process_cgraph_node (callees->callee,
                                  cutoff_count);
      callees = callees->next_callee;
    }

  callees = node->callees;
  while (callees)
    {
      if (callees->count >= cutoff_count)
        {
          unsigned callee_mod_id;
          struct dyn_pointer_set *imp_modules
              = gcov_get_imp_module_set (node);

          callee_mod_id
              = get_module_idx_from_func_glob_uid (callees->callee->guid);

          if (mod_id != callee_mod_id)
            {
              struct gcov_info *callee_mod_info
                  = get_module_info (callee_mod_id);
              pointer_set_insert (imp_modules, callee_mod_info);
            }
          if (callees->callee->imported_modules)
            pointer_set_traverse (callees->callee->imported_modules,
                                  gcov_propagate_imp_modules,
                                  imp_modules);
        }

      callees = callees->next_callee;
    }
}

/* Compute module grouping using CUTOFF_COUNT as the hot edge
   threshold.  */

static void
gcov_compute_module_groups (gcov_type cutoff_count)
{
  unsigned m_ix;
  struct gcov_info *gi_ptr;

  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      const struct gcov_fn_info *fi_ptr;
      unsigned f_ix;

      gi_ptr = the_dyn_call_graph.modules[m_ix];

      for (f_ix = 0; f_ix < gi_ptr->n_functions; f_ix++)
	{
	  struct dyn_cgraph_node *node;

	  fi_ptr = the_dyn_call_graph.functions[m_ix][f_ix];
	  node = &the_dyn_call_graph.call_graph_nodes[m_ix][fi_ptr->ident];
          if (node->visited)
            continue;

          gcov_process_cgraph_node (node, cutoff_count);
	}
    }

  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      const struct gcov_fn_info *fi_ptr;
      unsigned f_ix;

      gi_ptr = the_dyn_call_graph.modules[m_ix];

      for (f_ix = 0; f_ix < gi_ptr->n_functions; f_ix++)
	{
	  struct dyn_cgraph_node *node;
          unsigned mod_id;
          struct dyn_pointer_set *imp_modules;

	  fi_ptr = the_dyn_call_graph.functions[m_ix][f_ix];
	  node = &the_dyn_call_graph.call_graph_nodes[m_ix][fi_ptr->ident];

          if (!node->imported_modules)
            continue;

          mod_id = get_module_idx_from_func_glob_uid (node->guid);
          gcc_assert (mod_id == m_ix);

          imp_modules
              = gcov_get_module_imp_module_set (
                  &the_dyn_call_graph.sup_modules[mod_id]);

          pointer_set_traverse (node->imported_modules,
                                gcov_propagate_imp_modules,
                                imp_modules);
	}
    }

  /* Now compute the export attribute  */
  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      struct dyn_module_info *mi
          = &the_dyn_call_graph.sup_modules[m_ix];
      if (mi->imported_modules)
        pointer_set_traverse (mi->imported_modules,
                              gcov_mark_export_modules, 0);
    }
}

/* For each module, compute at random, the group of imported modules,
   that is of size at most MAX_GROUP_SIZE.  */

static void
gcov_compute_random_module_groups (unsigned max_group_size)
{
  unsigned m_ix;

  if (max_group_size > the_dyn_call_graph.num_modules)
    max_group_size = the_dyn_call_graph.num_modules;

  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      struct dyn_pointer_set *imp_modules =
	gcov_get_module_imp_module_set (&the_dyn_call_graph.sup_modules[m_ix]);
      int cur_group_size = random () % max_group_size;
      int i = 0;
      while (i < cur_group_size)
	{
	  struct gcov_info *imp_mod_info;
	  unsigned mod_id = random () % the_dyn_call_graph.num_modules;
	  if (mod_id == m_ix)
	    continue;
	  imp_mod_info = get_module_info (mod_id);
	  if (!pointer_set_insert (imp_modules, imp_mod_info))
	    i++;
	}
    }

  /* Now compute the export attribute  */
  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      struct dyn_module_info *mi 
	= &the_dyn_call_graph.sup_modules[m_ix];
      if (mi->imported_modules)
        pointer_set_traverse (mi->imported_modules,
                              gcov_mark_export_modules, 0);
    }
}

/* Write out MOD_INFO into the gcda file. IS_PRIMARY is a flag
   indicating if the module is the primary module in the group.  */

static void
gcov_write_module_info (const struct gcov_info *mod_info,
                        unsigned is_primary)
{
  gcov_unsigned_t len = 0, filename_len = 0, src_filename_len = 0, i, j;
  gcov_unsigned_t num_strings;
  gcov_unsigned_t *aligned_fname;
  struct gcov_module_info  *module_info = mod_info->mod_info;
  filename_len = (strlen (module_info->da_filename) +
		  sizeof (gcov_unsigned_t)) / sizeof (gcov_unsigned_t);
  src_filename_len = (strlen (module_info->source_filename) +
		      sizeof (gcov_unsigned_t)) / sizeof (gcov_unsigned_t);
  len = filename_len + src_filename_len;
  len += 2; /* each name string is led by a length.  */

  num_strings = module_info->num_quote_paths + module_info->num_bracket_paths +
    module_info->num_cpp_defines + module_info->num_cl_args;
  for (i = 0; i < num_strings; i++)
    {
      gcov_unsigned_t string_len
          = (strlen (module_info->string_array[i]) + sizeof (gcov_unsigned_t))
          / sizeof (gcov_unsigned_t);
      len += string_len;
      len += 1; /* Each string is lead by a length.  */
    }

  len += 8; /* 8 more fields */

  gcov_write_tag_length (GCOV_TAG_MODULE_INFO, len);
  gcov_write_unsigned (module_info->ident);
  gcov_write_unsigned (is_primary);
  gcov_write_unsigned (module_info->is_exported);
  gcov_write_unsigned (module_info->lang);
  gcov_write_unsigned (module_info->num_quote_paths);
  gcov_write_unsigned (module_info->num_bracket_paths);
  gcov_write_unsigned (module_info->num_cpp_defines);
  gcov_write_unsigned (module_info->num_cl_args);

  /* Now write the filenames */
  aligned_fname = (gcov_unsigned_t *) alloca ((filename_len + src_filename_len + 2) *
					      sizeof (gcov_unsigned_t));
  memset (aligned_fname, 0,
          (filename_len + src_filename_len + 2) * sizeof (gcov_unsigned_t));
  aligned_fname[0] = filename_len;
  strcpy ((char*) (aligned_fname + 1), module_info->da_filename);
  aligned_fname[filename_len + 1] = src_filename_len;
  strcpy ((char*) (aligned_fname + filename_len + 2), module_info->source_filename);

  for (i = 0; i < (filename_len + src_filename_len + 2); i++)
    gcov_write_unsigned (aligned_fname[i]);

  /* Now write the string array.  */
  for (j = 0; j < num_strings; j++)
    {
      gcov_unsigned_t *aligned_string;
      gcov_unsigned_t string_len =
	(strlen (module_info->string_array[j]) + sizeof (gcov_unsigned_t)) /
	sizeof (gcov_unsigned_t);
      aligned_string = (gcov_unsigned_t *)
	alloca ((string_len + 1) * sizeof (gcov_unsigned_t));
      memset (aligned_string, 0, (string_len + 1) * sizeof (gcov_unsigned_t));
      aligned_string[0] = string_len;
      strcpy ((char*) (aligned_string + 1), module_info->string_array[j]);
      for (i = 0; i < (string_len + 1); i++)
        gcov_write_unsigned (aligned_string[i]);
    }
}

/* Write out MOD_INFO and its imported modules into gcda file.  */

void
gcov_write_module_infos (struct gcov_info *mod_info)
{
  unsigned mod_id, imp_len = 0;
  const struct gcov_info **imp_mods;

  mod_id = get_module_idx (mod_info);
  gcov_write_module_info (mod_info, 1);

  imp_mods = gcov_get_sorted_import_module_array (mod_info, &imp_len);
  if (imp_mods)
    {
      unsigned i;

      for (i = 0; i < imp_len; i++)
        {
          const struct gcov_info *imp_mod = imp_mods[i];
          gcov_write_module_info (imp_mod, 0);
        }
      free (imp_mods);
    }
}

/* Compute module groups needed for L-IPO compilation.  */

void
__gcov_compute_module_groups (void)
{
  gcov_type cut_off_count;
  char *seed = getenv ("LIPO_RANDOM_GROUPING");
  char *max_group_size = seed ? strchr (seed, ':') : 0;

  if (seed && max_group_size)
    {
      *max_group_size = '\0';
      max_group_size++;
      srandom (atoi (seed));
      init_dyn_call_graph ();
      gcov_compute_random_module_groups (atoi (max_group_size));
      return;
    }

  /* First compute dynamic call graph.  */
  gcov_build_callgraph ();

  cut_off_count = gcov_compute_cutoff_count ();

  gcov_compute_module_groups (cut_off_count);

  gcov_dump_callgraph (cut_off_count);

}

/* Dumper function for NODE.  */
static void
gcov_dump_cgraph_node_short (struct dyn_cgraph_node *node)
{
  unsigned mod_id, func_id;
  struct gcov_info *mod_info;
  mod_id = get_module_idx_from_func_glob_uid (node->guid);
  func_id = get_intra_module_func_id (node->guid);

  mod_info = the_dyn_call_graph.modules[mod_id];

  fprintf (stderr, "NODE(%llx) module(%s) func(%u)",
           (long long)node->guid, 
           mod_info->mod_info->source_filename, func_id);
}

/* Dumper function for NODE.   M is the module id and F is the function id.  */

static void
gcov_dump_cgraph_node (struct dyn_cgraph_node *node, unsigned m, unsigned f)
{
  unsigned mod_id, func_id;
  struct gcov_info *mod_info;
  struct dyn_cgraph_edge *callers;
  struct dyn_cgraph_edge *callees;

  mod_id = get_module_idx_from_func_glob_uid (node->guid);
  func_id = get_intra_module_func_id (node->guid);
  gcc_assert (mod_id == m && func_id == f);

  mod_info = the_dyn_call_graph.modules[mod_id];

  fprintf (stderr, "NODE(%llx) module(%s) func(%x)\n",
           (long long) node->guid,
           mod_info->mod_info->source_filename, f);

  /* Now dump callers.  */
  callers = node->callers;
  fprintf (stderr, "\t[CALLERS]\n");
  while (callers != 0)
    {
      fprintf (stderr,"\t\t[count=%ld] ", (long)  callers->count);
      gcov_dump_cgraph_node_short (callers->caller);
      fprintf (stderr,"\n");
      callers = callers->next_caller;
    }

  callees = node->callees;
  fprintf (stderr, "\t[CALLEES]\n");
  while (callees != 0)
    {
      fprintf (stderr,"\t\t[count=%ld] ", (long)  callees->count);
      gcov_dump_cgraph_node_short (callees->callee);
      fprintf (stderr,"\n");
      callees = callees->next_callee;
    }
}

/* Dumper function for NODE.   M is the module id and F is the function id.  */

static void
gcov_dump_cgraph_node_dot (struct dyn_cgraph_node *node, 
                           unsigned m, unsigned f,
                           gcov_type cutoff_count)
{
  unsigned mod_id, func_id, imp_len = 0, i;
  struct gcov_info *mod_info;
  const struct gcov_info **imp_mods;
  struct dyn_cgraph_edge *callees;

  mod_id = get_module_idx_from_func_glob_uid (node->guid);
  func_id = get_intra_module_func_id (node->guid);
  gcc_assert (mod_id == m && func_id == f);

  mod_info = the_dyn_call_graph.modules[mod_id];

  fprintf (stderr, "NODE_%llx[label=\"MODULE\\n(%s)\\n FUNC(%x)\\n",
           (long long) node->guid, mod_info->mod_info->source_filename, f);

  imp_mods = gcov_get_sorted_import_module_array (mod_info, &imp_len);
  fprintf (stderr, "IMPORTS:\\n");
  if (imp_mods)
    {
      for (i = 0; i < imp_len; i++)
        fprintf (stderr, "%s\\n", imp_mods[i]->mod_info->source_filename);
      fprintf (stderr, "\"]\n");
      free (imp_mods);
    }
  else
    fprintf (stderr, "\"]\n");

  callees = node->callees;
  while (callees != 0)
    {
      if (callees->count >= cutoff_count)
        fprintf (stderr, "NODE_%llx -> NODE_%llx[label=%lld color=red]\n",
                 (long long) node->guid, (long long) callees->callee->guid,
                 (long long) callees->count);
      else
        fprintf (stderr, "NODE_%llx -> NODE_%llx[label=%lld color=blue]\n",
                 (long long) node->guid, (long long) callees->callee->guid,
                 (long long) callees->count);
      callees = callees->next_callee;
    }
}

/* Dump dynamic call graph.  CUTOFF_COUNT is the computed hot edge threshold.  */

static void
gcov_dump_callgraph (gcov_type cutoff_count)
{
  struct gcov_info *gi_ptr;
  unsigned m_ix;
  const char *dyn_cgraph_dump = 0;

  dyn_cgraph_dump = getenv ("GCOV_DYN_CGRAPH_DUMP");

  if (!dyn_cgraph_dump || !strlen (dyn_cgraph_dump))
      return;

  fprintf (stderr,"digraph dyn_call_graph {\n");
  fprintf (stderr,"node[shape=box]\nsize=\"11,8.5\"\n");

  for (m_ix = 0; m_ix < the_dyn_call_graph.num_modules; m_ix++)
    {
      const struct gcov_fn_info *fi_ptr;
      unsigned f_ix;

      gi_ptr = the_dyn_call_graph.modules[m_ix];

      for (f_ix = 0; f_ix < gi_ptr->n_functions; f_ix++)
	{
	  struct dyn_cgraph_node *node;
	  fi_ptr = the_dyn_call_graph.functions[m_ix][f_ix];

	  node = &the_dyn_call_graph.call_graph_nodes[m_ix][fi_ptr->ident];

          /* skip dead functions  */
          if (!node->callees && !node->callers)
            continue;

          if (dyn_cgraph_dump[0] == '1')
            gcov_dump_cgraph_node (node, m_ix, fi_ptr->ident);
          else
            gcov_dump_cgraph_node_dot (node, m_ix, fi_ptr->ident,
                                       cutoff_count);
	}
    }
  fprintf (stderr,"}\n");
}


#endif
