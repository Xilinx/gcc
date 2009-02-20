/* Gimple Represented as Polyhedra.
   Copyright (C) 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@inria.fr>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* This pass converts GIMPLE to GRAPHITE, performs some loop
   transformations and then converts the resulting representation back
   to GIMPLE.  

   An early description of this pass can be found in the GCC Summit'06
   paper "GRAPHITE: Polyhedral Analyses and Optimizations for GCC".
   The wiki page http://gcc.gnu.org/wiki/Graphite contains pointers to
   the related work.  

   One important document to read is CLooG's internal manual:
   http://repo.or.cz/w/cloog-ppl.git?a=blob_plain;f=doc/cloog.texi;hb=HEAD
   that describes the data structure of loops used in this file, and
   the functions that are used for transforming the code.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "toplev.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "domwalk.h"
#include "value-prof.h"
#include "pointer-set.h"
#include "gimple.h"
#include "sese.h"

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "ppl_c.h"
#include "graphite-ppl.h"
#include "graphite.h"
#include "graphite-poly.h"
#include "graphite-scop-detection.h"
#include "graphite-data-ref.h"

/* Converts a GMP constant V to a tree and returns it.  */

static tree
gmp_cst_to_tree (tree type, Value v)
{
  return build_int_cst (type, value_get_si (v));
}

/* Debug the list of old induction variables for this SCOP.  */

void
debug_oldivs (sese region)
{
  int i;
  name_tree oldiv;

  fprintf (stderr, "Old IVs:");

  for (i = 0; VEC_iterate (name_tree, SESE_OLDIVS (region), i, oldiv); i++)
    {
      fprintf (stderr, "(");
      print_generic_expr (stderr, oldiv->t, 0);
      fprintf (stderr, ", %s, %d)\n", oldiv->name, oldiv->loop->num);
    }
  fprintf (stderr, "\n");
}

/* Debug the loops around basic block GB.  */

void
debug_loop_vec (poly_bb_p pbb)
{
  int i;
  loop_p loop;

  fprintf (stderr, "Loop Vec:");

  for (i = 0; VEC_iterate (loop_p, PBB_LOOPS (pbb), i, loop); i++)
    fprintf (stderr, "%d: %d, ", i, loop ? loop->num : -1);

  fprintf (stderr, "\n");
}

/* Returns true if stack ENTRY is a constant.  */

static bool
iv_stack_entry_is_constant (iv_stack_entry *entry)
{
  return entry->kind == iv_stack_entry_const;
}

/* Returns true if stack ENTRY is an induction variable.  */

static bool
iv_stack_entry_is_iv (iv_stack_entry *entry)
{
  return entry->kind == iv_stack_entry_iv;
}

/* Push (IV, NAME) on STACK.  */

static void 
loop_iv_stack_push_iv (loop_iv_stack stack, tree iv, const char *name)
{
  iv_stack_entry *entry = XNEW (iv_stack_entry);
  name_tree named_iv = XNEW (struct name_tree);

  named_iv->t = iv;
  named_iv->name = name;

  entry->kind = iv_stack_entry_iv;
  entry->data.iv = named_iv;

  VEC_safe_push (iv_stack_entry_p, heap, *stack, entry);
}

/* Inserts a CONSTANT in STACK at INDEX.  */

static void
loop_iv_stack_insert_constant (loop_iv_stack stack, int index,
			       tree constant)
{
  iv_stack_entry *entry = XNEW (iv_stack_entry);
  
  entry->kind = iv_stack_entry_const;
  entry->data.constant = constant;

  VEC_safe_insert (iv_stack_entry_p, heap, *stack, index, entry);
}

/* Pops and frees an element out of STACK.  */

static void
loop_iv_stack_pop (loop_iv_stack stack)
{
  iv_stack_entry_p entry = VEC_pop (iv_stack_entry_p, *stack);

  free (entry->data.iv);
  free (entry);
}

/* Get the IV at INDEX in STACK.  */

static tree
loop_iv_stack_get_iv (loop_iv_stack stack, int index)
{
  iv_stack_entry_p entry = VEC_index (iv_stack_entry_p, *stack, index);
  iv_stack_entry_data data = entry->data;

  return iv_stack_entry_is_iv (entry) ? data.iv->t : data.constant;
}

/* Get the IV from its NAME in STACK.  */

static tree
loop_iv_stack_get_iv_from_name (loop_iv_stack stack, const char* name)
{
  int i;
  iv_stack_entry_p entry;

  for (i = 0; VEC_iterate (iv_stack_entry_p, *stack, i, entry); i++)
    {
      name_tree iv = entry->data.iv;
      if (!strcmp (name, iv->name))
	return iv->t;
    }

  return NULL;
}

/* Prints on stderr the contents of STACK.  */

void
debug_loop_iv_stack (loop_iv_stack stack)
{
  int i;
  iv_stack_entry_p entry;
  bool first = true;

  fprintf (stderr, "(");

  for (i = 0; VEC_iterate (iv_stack_entry_p, *stack, i, entry); i++)
    {
      if (first) 
	first = false;
      else
	fprintf (stderr, " ");

      if (iv_stack_entry_is_iv (entry))
	{
	  name_tree iv = entry->data.iv;
	  fprintf (stderr, "%s:", iv->name);
	  print_generic_expr (stderr, iv->t, 0);
	}
      else 
	{
	  tree constant = entry->data.constant;
	  print_generic_expr (stderr, constant, 0);
	  fprintf (stderr, ":");
	  print_generic_expr (stderr, constant, 0);
	}
    }

  fprintf (stderr, ")\n");
}

/* Frees STACK.  */

static void
free_loop_iv_stack (loop_iv_stack stack)
{
  int i;
  iv_stack_entry_p entry;

  for (i = 0; VEC_iterate (iv_stack_entry_p, *stack, i, entry); i++)
    {
      free (entry->data.iv);
      free (entry);
    }

  VEC_free (iv_stack_entry_p, heap, *stack);
}

/* Structure containing the mapping between the CLooG's induction
   variable and the type of the old induction variable.  */
typedef struct ivtype_map_elt
{
  tree type;
  const char *cloog_iv;
} *ivtype_map_elt;

/* Print to stderr the element ELT.  */

static void
debug_ivtype_elt (ivtype_map_elt elt)
{
  fprintf (stderr, "(%s, ", elt->cloog_iv);
  print_generic_expr (stderr, elt->type, 0);
  fprintf (stderr, ")\n");
}

/* Helper function for debug_ivtype_map.  */

static int
debug_ivtype_map_1 (void **slot, void *s ATTRIBUTE_UNUSED)
{
  struct ivtype_map_elt *entry = (struct ivtype_map_elt *) *slot;
  debug_ivtype_elt (entry);
  return 1;
}

/* Print to stderr all the elements of MAP.  */

void
debug_ivtype_map (htab_t map)
{
  htab_traverse (map, debug_ivtype_map_1, NULL);
}

/* Constructs a new SCEV_INFO_STR structure for VAR and INSTANTIATED_BELOW.  */

static inline ivtype_map_elt
new_ivtype_map_elt (const char *cloog_iv, tree type)
{
  ivtype_map_elt res;
  
  res = XNEW (struct ivtype_map_elt);
  res->cloog_iv = cloog_iv;
  res->type = type;

  return res;
}

/* Computes a hash function for database element ELT.  */

static hashval_t
ivtype_map_elt_info (const void *elt)
{
  return htab_hash_pointer (((const struct ivtype_map_elt *) elt)->cloog_iv);
}

/* Compares database elements E1 and E2.  */

static int
eq_ivtype_map_elts (const void *e1, const void *e2)
{
  const struct ivtype_map_elt *elt1 = (const struct ivtype_map_elt *) e1;
  const struct ivtype_map_elt *elt2 = (const struct ivtype_map_elt *) e2;

  return (elt1->cloog_iv == elt2->cloog_iv);
}

/* Given a CLOOG_IV, returns the type that it should have in GCC land.
   If the information is not available, i.e. in the case one of the
   transforms created the loop, just return integer_type_node.  */

static tree
gcc_type_for_cloog_iv (const char *cloog_iv, gimple_bb_p gbb)
{
  struct ivtype_map_elt tmp;
  PTR *slot;

  tmp.cloog_iv = cloog_iv;
  slot = htab_find_slot (GBB_CLOOG_IV_TYPES (gbb), &tmp, NO_INSERT);

  if (slot && *slot)
    return ((ivtype_map_elt) *slot)->type;

  return integer_type_node;
}

/* Inserts constants derived from the USER_STMT argument list into the
   STACK.  This is needed to map old ivs to constants when loops have
   been eliminated.  */

static void 
loop_iv_stack_patch_for_consts (loop_iv_stack stack,
				struct clast_user_stmt *user_stmt)
{
  struct clast_stmt *t;
  int index = 0;
  CloogStatement *cs = user_stmt->statement;
  poly_bb_p pbb = (poly_bb_p) cloog_statement_usr (cs);

  for (t = user_stmt->substitutions; t; t = t->next) 
    {
      struct clast_expr *expr = (struct clast_expr *) 
	((struct clast_assignment *)t)->RHS;
      struct clast_term *term = (struct clast_term *) expr;

      /* FIXME: What should be done with expr_bin, expr_red?  */
      if (expr->type == expr_term
	  && !term->var)
	{
	  loop_p loop = pbb_loop_at_index (pbb, index);
	  tree oldiv = oldiv_for_loop (SCOP_REGION (PBB_SCOP (pbb)), loop);
	  tree type = oldiv ? TREE_TYPE (oldiv) : integer_type_node;
	  tree value = gmp_cst_to_tree (type, term->val);
	  loop_iv_stack_insert_constant (stack, index, value);
	}
      index = index + 1;
    }
}

/* Removes all constants in the iv STACK.  */

static void
loop_iv_stack_remove_constants (loop_iv_stack stack)
{
  int i;
  iv_stack_entry *entry;
  
  for (i = 0; VEC_iterate (iv_stack_entry_p, *stack, i, entry);)
    {
      if (iv_stack_entry_is_constant (entry))
	{
	  free (VEC_index (iv_stack_entry_p, *stack, i));
	  VEC_ordered_remove (iv_stack_entry_p, *stack, i);
	}
      else
	i++;
    }
}

/* Converts the graphite scheduling function into a cloog scattering
   polyhedron.  The scattering polyhedron is used to add constraints
   that limit the possible cloog output to valid programs in respect
   to the scheduling function.

   SCATTERING_DIMENSIONS specifies the number of scattering
   dimensions.  CLooG 0.15.0 and previous versions require, that all
   scattering functions of one CloogProgram have the same number of
   scattering dimensions, therefore we allow to specify it.  This
   restriction should be removed in future versions of CLooG.

   The scattering polyhedron consists of these dimensions: scattering,
   loop_iterators, parameters.

   Example:

   | scattering_dimensions = 5
   | used_scattering_dimensions = 3
   | nb_iterators = 1 
   | scop_nb_params = 2
   |
   | Schedule:
   |   i
   | 4 5
   |
   | Scattering polyhedron:
   |
   | scattering: {s1, s2, s3, s4, s5}
   | loop_iterators: {i}
   | parameters: {p1, p2}
   |
   | s1  s2  s3  s4  s5  i   p1  p2  1 
   | 1   0   0   0   0   0   0   0  -4  = 0
   | 0   1   0   0   0  -1   0   0   0  = 0
   | 0   0   1   0   0   0   0   0  -5  = 0  */

static ppl_Polyhedron_t
schedule_to_scattering (poly_bb_p pbb, int scattering_dimensions) 
{
  int i;
  scop_p scop = PBB_SCOP (pbb);
  int nb_iterators = pbb_nb_loops (pbb);
  int used_scattering_dimensions = nb_iterators * 2 + 1;
  int nb_params = scop_nb_params (scop);
  int col_iter_offset = scattering_dimensions;
  ppl_Polyhedron_t ph;
  ppl_Coefficient_t c;
  ppl_dimension_type dim = scattering_dimensions + nb_iterators + nb_params;
  Value v;

  gcc_assert (scattering_dimensions >= used_scattering_dimensions);

  value_init (v);
  ppl_new_Coefficient (&c);
  ppl_new_NNC_Polyhedron_from_space_dimension (&ph, dim, 0);

  for (i = 0; i < scattering_dimensions; i++)
    {
      ppl_Constraint_t cstr;
      ppl_Linear_Expression_t expr;

      ppl_new_Linear_Expression_with_dimension (&expr, dim);
      value_set_si (v, 1);
      ppl_assign_Coefficient_from_mpz_t (c, v);
      ppl_Linear_Expression_add_to_coefficient (expr, i, c);

      /* Textual order inside this loop.  */
      if (i < used_scattering_dimensions
	  && (i % 2) == 0)
	{
	  ppl_Linear_Expression_coefficient (PBB_STATIC_SCHEDULE (pbb), i / 2, c);
	  ppl_Coefficient_to_mpz_t (c, v);
	  value_oppose (v, v);
	  ppl_assign_Coefficient_from_mpz_t (c, v);
	  ppl_Linear_Expression_add_to_inhomogeneous (expr, c);
	}

      /* Iterations of this loop.  */
      if (i < used_scattering_dimensions
	  && (i % 2) == 1)
	{
	  int loop = (i - 1) / 2;
	  value_set_si (v, -1);
	  ppl_assign_Coefficient_from_mpz_t (c, v);
	  ppl_Linear_Expression_add_to_coefficient (expr,
						    col_iter_offset + loop, c);
	}
      
      ppl_new_Constraint (&cstr, expr, PPL_CONSTRAINT_TYPE_EQUAL);
      ppl_Polyhedron_add_constraint (ph, cstr);
      ppl_delete_Linear_Expression (expr);
      ppl_delete_Constraint (cstr);
    }

  value_clear (v);
  ppl_delete_Coefficient (c);
  return ph;
}

/* Pretty print all SCoPs in DOT format and mark them with different colors.
   If there are not enough colors, paint later SCoPs gray.
   Special nodes:
   - "*" after the node number: entry of a SCoP,
   - "#" after the node number: exit of a SCoP,
   - "()" entry or exit not part of SCoP.  */

static void
dot_all_scops_1 (FILE *file, VEC (scop_p, heap) *scops)
{
  basic_block bb;
  edge e;
  edge_iterator ei;
  scop_p scop;
  const char* color;
  int i;

  /* Disable debugging while printing graph.  */
  int tmp_dump_flags = dump_flags;
  dump_flags = 0;

  fprintf (file, "digraph all {\n");

  FOR_ALL_BB (bb)
    {
      int part_of_scop = false;

      /* Use HTML for every bb label.  So we are able to print bbs
         which are part of two different SCoPs, with two different
         background colors.  */
      fprintf (file, "%d [label=<\n  <TABLE BORDER=\"0\" CELLBORDER=\"1\" ",
                     bb->index);
      fprintf (file, "CELLSPACING=\"0\">\n");

      /* Select color for SCoP.  */
      for (i = 0; VEC_iterate (scop_p, scops, i, scop); i++)
	if (bb_in_sese_p (bb, SCOP_REGION (scop))
	    || (SCOP_EXIT (scop) == bb)
	    || (SCOP_ENTRY (scop) == bb))
	  {
	    switch (i % 17)
	      {
	      case 0: /* red */
		color = "#e41a1c";
		break;
	      case 1: /* blue */
		color = "#377eb8";
		break;
	      case 2: /* green */
		color = "#4daf4a";
		break;
	      case 3: /* purple */
		color = "#984ea3";
		break;
	      case 4: /* orange */
		color = "#ff7f00";
		break;
	      case 5: /* yellow */
		color = "#ffff33";
		break;
	      case 6: /* brown */
		color = "#a65628";
		break;
	      case 7: /* rose */
		color = "#f781bf";
		break;
	      case 8:
		color = "#8dd3c7";
		break;
	      case 9:
		color = "#ffffb3";
		break;
	      case 10:
		color = "#bebada";
		break;
	      case 11:
		color = "#fb8072";
		break;
	      case 12:
		color = "#80b1d3";
		break;
	      case 13:
		color = "#fdb462";
		break;
	      case 14:
		color = "#b3de69";
		break;
	      case 15:
		color = "#fccde5";
		break;
	      case 16:
		color = "#bc80bd";
		break;
	      default: /* gray */
		color = "#999999";
	      }

	    fprintf (file, "    <TR><TD WIDTH=\"50\" BGCOLOR=\"%s\">", color);
        
	    if (!bb_in_sese_p (bb, SCOP_REGION (scop)))
	      fprintf (file, " ("); 

	    if (bb == SCOP_ENTRY (scop)
		&& bb == SCOP_EXIT (scop))
	      fprintf (file, " %d*# ", bb->index);
	    else if (bb == SCOP_ENTRY (scop))
	      fprintf (file, " %d* ", bb->index);
	    else if (bb == SCOP_EXIT (scop))
	      fprintf (file, " %d# ", bb->index);
	    else
	      fprintf (file, " %d ", bb->index);

	    if (!bb_in_sese_p (bb, SCOP_REGION (scop)))
	      fprintf (file, ")");

	    fprintf (file, "</TD></TR>\n");
	    part_of_scop  = true;
	  }

      if (!part_of_scop)
        {
          fprintf (file, "    <TR><TD WIDTH=\"50\" BGCOLOR=\"#ffffff\">");
          fprintf (file, " %d </TD></TR>\n", bb->index);
        }

      fprintf (file, "  </TABLE>>, shape=box, style=\"setlinewidth(0)\"]\n");
    }

  FOR_ALL_BB (bb)
    {
      FOR_EACH_EDGE (e, ei, bb->succs)
	      fprintf (file, "%d -> %d;\n", bb->index, e->dest->index);
    }

  fputs ("}\n\n", file);

  /* Enable debugging again.  */
  dump_flags = tmp_dump_flags;
}

/* Display all SCoPs using dotty.  */

void
dot_all_scops (VEC (scop_p, heap) *scops)
{
  /* When debugging, enable the following code.  This cannot be used
     in production compilers because it calls "system".  */
#if 1
  FILE *stream = fopen ("/tmp/allscops.dot", "w");
  gcc_assert (stream);

  dot_all_scops_1 (stream, scops);
  fclose (stream);

  system ("dotty /tmp/allscops.dot");
#else
  dot_all_scops_1 (stderr, scops);
#endif
}

/* Returns the outermost loop in SCOP that contains BB.  */

static struct loop *
outermost_loop_in_sese (sese region, basic_block bb)
{
  struct loop *nest;

  nest = bb->loop_father;
  while (loop_outer (nest)
	 && loop_in_sese_p (loop_outer (nest), region))
    nest = loop_outer (nest);

  return nest;
}

/* Returns true when BB will be represented in graphite.  Return false
   for the basic blocks that contain code eliminated in the code
   generation pass: i.e. induction variables and exit conditions.  */

static bool
graphite_stmt_p (sese region, basic_block bb,
		 VEC (data_reference_p, heap) *drs)
{
  gimple_stmt_iterator gsi;
  loop_p loop = bb->loop_father;

  if (VEC_length (data_reference_p, drs) > 0)
    return true;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);

      switch (gimple_code (stmt))
        {
          /* Control flow expressions can be ignored, as they are
             represented in the iteration domains and will be
             regenerated by graphite.  */
	case GIMPLE_COND:
	case GIMPLE_GOTO:
	case GIMPLE_SWITCH:
	  break;

	case GIMPLE_ASSIGN:
	  {
	    tree var = gimple_assign_lhs (stmt);
	    var = analyze_scalar_evolution (loop, var);
	    var = instantiate_scev (block_before_sese (region), loop, var);

	    if (chrec_contains_undetermined (var))
	      return true;

	    break;
	  }

	default:
	  return true;
        }
    }

  return false;
}

/* Store the GRAPHITE representation of BB.  */

static gimple_bb_p
new_gimple_bb (basic_block bb, VEC (data_reference_p, heap) *drs)
{
  struct gimple_bb *gbb;

  gbb = XNEW (struct gimple_bb);
  bb->aux = gbb;
  GBB_BB (gbb) = bb;
  GBB_DATA_REFS (gbb) = drs;
  GBB_CONDITIONS (gbb) = NULL;
  GBB_CONDITION_CASES (gbb) = NULL;
  GBB_CLOOG_IV_TYPES (gbb) = NULL;
 
  return gbb;
}

/* Frees GBB.  */

static void
free_gimple_bb (struct gimple_bb *gbb)
{
  if (GBB_CLOOG_IV_TYPES (gbb))
    htab_delete (GBB_CLOOG_IV_TYPES (gbb));

  /* FIXME: free_data_refs is disabled for the moment, but should be
     enabled.

     free_data_refs (GBB_DATA_REFS (gbb)); */

  VEC_free (gimple, heap, GBB_CONDITIONS (gbb));
  VEC_free (gimple, heap, GBB_CONDITION_CASES (gbb));
  GBB_BB (gbb)->aux = 0;
  XDELETE (gbb);
}

/* Structure containing the mapping between the old names and the new
   names used after block copy in the new loop context.  */
typedef struct rename_map_elt
{
  tree old_name, new_name;
} *rename_map_elt;


/* Print to stderr the element ELT.  */

static void
debug_rename_elt (rename_map_elt elt)
{
  fprintf (stderr, "(");
  print_generic_expr (stderr, elt->old_name, 0);
  fprintf (stderr, ", ");
  print_generic_expr (stderr, elt->new_name, 0);
  fprintf (stderr, ")\n");
}

/* Helper function for debug_rename_map.  */

static int
debug_rename_map_1 (void **slot, void *s ATTRIBUTE_UNUSED)
{
  struct rename_map_elt *entry = (struct rename_map_elt *) *slot;
  debug_rename_elt (entry);
  return 1;
}

/* Print to stderr all the elements of MAP.  */

void
debug_rename_map (htab_t map)
{
  htab_traverse (map, debug_rename_map_1, NULL);
}

/* Constructs a new SCEV_INFO_STR structure for VAR and INSTANTIATED_BELOW.  */

static inline rename_map_elt
new_rename_map_elt (tree old_name, tree new_name)
{
  rename_map_elt res;
  
  res = XNEW (struct rename_map_elt);
  res->old_name = old_name;
  res->new_name = new_name;

  return res;
}

/* Computes a hash function for database element ELT.  */

static hashval_t
rename_map_elt_info (const void *elt)
{
  return htab_hash_pointer (((const struct rename_map_elt *) elt)->old_name);
}

/* Compares database elements E1 and E2.  */

static int
eq_rename_map_elts (const void *e1, const void *e2)
{
  const struct rename_map_elt *elt1 = (const struct rename_map_elt *) e1;
  const struct rename_map_elt *elt2 = (const struct rename_map_elt *) e2;

  return (elt1->old_name == elt2->old_name);
}

/* Returns the new name associated to OLD_NAME in MAP.  */

static tree
get_new_name_from_old_name (htab_t map, tree old_name)
{
  struct rename_map_elt tmp;
  PTR *slot;

  tmp.old_name = old_name;
  slot = htab_find_slot (map, &tmp, NO_INSERT);

  if (slot && *slot)
    return ((rename_map_elt) *slot)->new_name;

  return old_name;
}

/* Deletes all gimple bbs in SCOP.  */

static void
remove_gbbs_in_scop (scop_p scop)
{
  int i;
  poly_bb_p pbb;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    free_gimple_bb (PBB_BLACK_BOX (pbb));
}

/* Deletes all scops in SCOPS.  */

void
free_scops (VEC (scop_p, heap) *scops)
{
  int i;
  scop_p scop;

  for (i = 0; VEC_iterate (scop_p, scops, i, scop); i++)
    {
      remove_gbbs_in_scop (scop);
      free_sese (SCOP_REGION (scop));
      free_scop (scop);
    }

  VEC_free (scop_p, heap, scops);
}

/* Free and compute again all the dominators information.  */

static inline void
recompute_all_dominators (void)
{
  mark_irreducible_loops ();
  free_dominance_info (CDI_DOMINATORS);
  free_dominance_info (CDI_POST_DOMINATORS);
  calculate_dominance_info (CDI_DOMINATORS);
  calculate_dominance_info (CDI_POST_DOMINATORS);
}

/* Verifies properties that GRAPHITE should maintain during translation.  */

static inline void
graphite_verify (void)
{
#ifdef ENABLE_CHECKING
  verify_loop_structure ();
  verify_dominators (CDI_DOMINATORS);
  verify_dominators (CDI_POST_DOMINATORS);
  verify_ssa (false);
  verify_loop_closed_ssa ();
#endif
}

/* Generates a polyhedral black box only if the bb contains interesting
   information.  */

static void
try_generate_gimple_bb (scop_p scop, basic_block bb)
{
  sese region = SCOP_REGION (scop); 
  VEC (data_reference_p, heap) *drs = VEC_alloc (data_reference_p, heap, 5);
  struct loop *nest = outermost_loop_in_sese (region, bb);
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    find_data_references_in_stmt (nest, gsi_stmt (gsi), &drs);

  if (!graphite_stmt_p (region, bb, drs))
    {
      free_data_refs (drs);
      return;
    }

  new_poly_bb (scop, new_gimple_bb (bb, drs));
}

/* Gather the basic blocks belonging to the SCOP.  */

void
build_scop_bbs (scop_p scop)
{
  basic_block *stack = XNEWVEC (basic_block, n_basic_blocks + 1);
  sbitmap visited = sbitmap_alloc (last_basic_block);
  int sp = 0;

  sbitmap_zero (visited);
  stack[sp++] = SCOP_ENTRY (scop);

  while (sp)
    {
      basic_block bb = stack[--sp];
      int depth = loop_depth (bb->loop_father);
      int num = bb->loop_father->num;
      edge_iterator ei;
      edge e;

      /* Scop's exit is not in the scop.  Exclude also bbs, which are
	 dominated by the SCoP exit.  These are e.g. loop latches.  */
      if (TEST_BIT (visited, bb->index)
	  || dominated_by_p (CDI_DOMINATORS, bb, SCOP_EXIT (scop))
	  /* Every block in the scop is dominated by scop's entry.  */
	  || !dominated_by_p (CDI_DOMINATORS, bb, SCOP_ENTRY (scop)))
	continue;

      try_generate_gimple_bb (scop, bb);

      SET_BIT (visited, bb->index);

      /* First push the blocks that have to be processed last.  Note
	 that this means that the order in which the code is organized
	 below is important: do not reorder the following code.  */
      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && (int) loop_depth (e->dest->loop_father) < depth)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && (int) loop_depth (e->dest->loop_father) == depth
	    && e->dest->loop_father->num != num)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && (int) loop_depth (e->dest->loop_father) == depth
	    && e->dest->loop_father->num == num
	    && EDGE_COUNT (e->dest->preds) > 1)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && (int) loop_depth (e->dest->loop_father) == depth
	    && e->dest->loop_father->num == num
	    && EDGE_COUNT (e->dest->preds) == 1)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && (int) loop_depth (e->dest->loop_father) > depth)
	  stack[sp++] = e->dest;
    }

  free (stack);
  sbitmap_free (visited);
}

/* Calculate the number of loops around LOOP in the SCOP.
   FIXME: The same as sese_loop_depth.  */

static inline int
nb_loops_around_loop_in_sese (struct loop *l, sese region)
{
  int d = 0;

  for (; loop_in_sese_p (l, region); d++, l = loop_outer (l));

  return d;
}

/* Calculate the number of loops around GB in the current SCOP.  */

static int
nb_loops_around_pbb (poly_bb_p pbb)
{
  return nb_loops_around_loop_in_sese (gbb_loop (PBB_BLACK_BOX (pbb)),
				       SCOP_REGION (PBB_SCOP (pbb)));
}

/* Returns the dimensionality of an enclosing loop iteration domain
   with respect to enclosing SCoP for a given data reference REF.  The
   returned dimensionality is homogeneous (depth of loop nest + number
   of SCoP parameters + const).  */

int
ref_nb_loops (data_reference_p ref, sese region)
{
  loop_p loop = loop_containing_stmt (DR_STMT (ref));

  return nb_loops_around_loop_in_sese (loop, region)
    + sese_nb_params (region) + 2;
}

/* Returns the number of loops that are identical at the beginning of
   the vectors A and B.  */

static int
compare_prefix_loops (VEC (loop_p, heap) *a, VEC (loop_p, heap) *b)
{
  int i;
  loop_p ea;
  int lb;

  if (!a || !b)
    return 0;

  lb = VEC_length (loop_p, b);

  for (i = 0; VEC_iterate (loop_p, a, i, ea); i++)
    if (i >= lb
	|| ea != VEC_index (loop_p, b, i))
      return i;

  return 0;
}

/* Build for BB the static schedule.

   The STATIC_SCHEDULE is defined like this:

   A
   for (i: ...)
     {
       for (j: ...)
         {
           B
           C 
         }

       for (k: ...)
         {
           D
           E 
         }
     }
   F

   Static schedules for A to F:

     DEPTH
     0 1 2 
   A 0
   B 1 0 0
   C 1 0 1
   D 1 1 0
   E 1 1 1 
   F 2
*/

static void
build_scop_canonical_schedules (scop_p scop)
{
  int i;
  poly_bb_p pbb;
  ppl_Linear_Expression_t static_schedule;
  VEC (loop_p, heap) *loops_previous = NULL;
  ppl_Coefficient_t c;
  Value v;

  value_init (v);
  ppl_new_Coefficient (&c);
  ppl_new_Linear_Expression (&static_schedule);

  /* We have to start schedules at 0 on the first component and
     because we cannot compare_prefix_loops against a previous loop,
     prefix will be equal to zero, and that index will be
     incremented before copying.  */
  value_set_si (v, -1);
  ppl_assign_Coefficient_from_mpz_t (c, v);
  ppl_Linear_Expression_add_to_coefficient (static_schedule, 0, c);

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    {
      ppl_Linear_Expression_t common;
      int prefix = compare_prefix_loops (loops_previous, PBB_LOOPS (pbb));
      int nb = pbb_nb_loops (pbb);

      loops_previous = PBB_LOOPS (pbb);
      ppl_new_Linear_Expression_with_dimension (&common, prefix + 1);
      ppl_assign_Linear_Expression_from_Linear_Expression (common, static_schedule);

      value_set_si (v, 1);
      ppl_assign_Coefficient_from_mpz_t (c, v);
      ppl_Linear_Expression_add_to_coefficient (common, prefix, c);
      ppl_assign_Linear_Expression_from_Linear_Expression (static_schedule, common);

      ppl_new_Linear_Expression_with_dimension (&PBB_STATIC_SCHEDULE (pbb),
						nb + 1);
      ppl_assign_Linear_Expression_from_Linear_Expression
	(PBB_STATIC_SCHEDULE (pbb), common);
      ppl_delete_Linear_Expression (common);
    }

  value_clear (v);
  ppl_delete_Coefficient (c);
  ppl_delete_Linear_Expression (static_schedule);
}

/* Build the LOOPS vector for all bbs in SCOP.  */

static void
build_bb_loops (scop_p scop)
{
  poly_bb_p pbb;
  int i;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    {
      loop_p loop;
      int depth; 

      depth = nb_loops_around_pbb (pbb) - 1; 

      PBB_LOOPS (pbb) = VEC_alloc (loop_p, heap, 3);
      VEC_safe_grow_cleared (loop_p, heap, PBB_LOOPS (pbb), depth + 1);

      loop = GBB_BB (PBB_BLACK_BOX (pbb))->loop_father;  

      while (sese_contains_loop (SCOP_REGION(scop), loop))
        {
          VEC_replace (loop_p, PBB_LOOPS (pbb), depth, loop);
          loop = loop_outer (loop);
          depth--;
        }
    }
}

/* When SUBTRACT is true subtract or when SUBTRACT is false add the
   value K to the dimension D of the linear expression EXPR.  */

static void
add_value_to_dim (ppl_dimension_type d, ppl_Linear_Expression_t expr, 
		  Value k, bool subtract)
{
  Value val;
  ppl_Coefficient_t coef;

  ppl_new_Coefficient (&coef);
  ppl_Linear_Expression_coefficient (expr, d, coef);
  value_init (val);
  ppl_Coefficient_to_mpz_t (coef, val);

  if (subtract)
    value_subtract (val, val, k);
  else
    value_addto (val, val, k);

  ppl_assign_Coefficient_from_mpz_t (coef, val);
  ppl_Linear_Expression_add_to_coefficient (expr, d, coef);
  value_clear (val);
  ppl_delete_Coefficient (coef);
}

/* In the context of scop S, scan E, the right hand side of a scalar
   evolution function in loop VAR, and translate it to a linear
   expression EXPR.  When SUBTRACT is true the linear expression
   corresponding to E is subtracted from the linear expression EXPR.  */

static void
scan_tree_for_params_right_scev (sese s, tree e, int var,
				 ppl_Linear_Expression_t expr,
				 bool subtract)
{
  if (expr)
    {
      loop_p loop = get_loop (var);
      ppl_dimension_type l = sese_loop_depth (s, loop);
      Value val;

      gcc_assert (TREE_CODE (e) == INTEGER_CST);

      value_init (val);
      value_set_si (val, int_cst_value (e));
      add_value_to_dim (l, expr, val, subtract);
      value_clear (val);
    }
}

/* Scan the integer constant CST, and if SUBTRACT is false add it or
   if SUBTRACT is true subtract it from the inhomogeneous part of the
   linear expression EXPR.  */

static void
scan_tree_for_params_int (tree cst, ppl_Linear_Expression_t expr, bool subtract)
{
  Value val;
  ppl_Coefficient_t coef;
  int v = int_cst_value (cst);

  if (v < 0)
    {
      v = -v;
      subtract = subtract ? false : true;
    }

  value_init (val);
  value_set_si (val, 0);
  if (subtract)
    value_sub_int (val, val, v);
  else
    value_add_int (val, val, v);

  ppl_new_Coefficient (&coef);
  ppl_assign_Coefficient_from_mpz_t (coef, val);
  ppl_Linear_Expression_add_to_inhomogeneous (expr, coef);
  value_clear (val);
  ppl_delete_Coefficient (coef);
}

/* In the context of sese S, scan the expression E and translate it to
   a linear expression C.  When parsing a symbolic multiplication, K
   represents the constant multiplier of an expression containing
   parameters.  When SUBTRACT is true the linear expression
   corresponding to E is subtracted from the linear expression C.  */

static void
scan_tree_for_params (sese s, tree e, ppl_Linear_Expression_t c,
		      Value k, bool subtract)
{
  if (e == chrec_dont_know)
    return;

  switch (TREE_CODE (e))
    {
    case POLYNOMIAL_CHREC:
      scan_tree_for_params_right_scev (s, CHREC_RIGHT (e), CHREC_VARIABLE (e),
				       c, subtract);
      scan_tree_for_params (s, CHREC_LEFT (e), c, k, subtract);
      break;

    case MULT_EXPR:
      if (chrec_contains_symbols (TREE_OPERAND (e, 0)))
	{
	  if (c)
	    {
	      Value val;
	      gcc_assert (host_integerp (TREE_OPERAND (e, 1), 0));
	      value_init (val);
	      value_set_si (val, int_cst_value (TREE_OPERAND (e, 1)));
	      value_multiply (k, k, val);
	      value_clear (val);
	    }
	  scan_tree_for_params (s, TREE_OPERAND (e, 0), c, k, subtract);
	}
      else
	{
	  if (c)
	    {
	      Value val;
	      gcc_assert (host_integerp (TREE_OPERAND (e, 0), 0));
	      value_init (val);
	      value_set_si (val, int_cst_value (TREE_OPERAND (e, 0)));
	      value_multiply (k, k, val);
	      value_clear (val);
	    }
	  scan_tree_for_params (s, TREE_OPERAND (e, 1), c, k, subtract);
	}
      break;

    case PLUS_EXPR:
    case POINTER_PLUS_EXPR:
      scan_tree_for_params (s, TREE_OPERAND (e, 0), c, k, subtract);
      scan_tree_for_params (s, TREE_OPERAND (e, 1), c, k, subtract);
      break;

    case MINUS_EXPR:
      scan_tree_for_params (s, TREE_OPERAND (e, 0), c, k, subtract);
      scan_tree_for_params (s, TREE_OPERAND (e, 1), c, k, !subtract);
      break;

    case NEGATE_EXPR:
      scan_tree_for_params (s, TREE_OPERAND (e, 0), c, k, !subtract);
      break;

    case SSA_NAME:
      {
	ppl_dimension_type p = parameter_index_in_region (e, s);
	if (c)
	  {
	    ppl_dimension_type dim;
	    ppl_Linear_Expression_space_dimension (c, &dim);
	    p += dim - sese_nb_params (s);
	    add_value_to_dim (p, c, k, subtract);
	  }
	break;
      }

    case INTEGER_CST:
      if (c)
	scan_tree_for_params_int (e, c, subtract);
      break;

    CASE_CONVERT:
    case NON_LVALUE_EXPR:
      scan_tree_for_params (s, TREE_OPERAND (e, 0), c, k, subtract);
      break;

    default:
      gcc_unreachable ();
      break;
    }
}

/* Data structure for idx_record_params.  */

struct irp_data
{
  struct loop *loop;
  sese sese;
};

/* For a data reference with an ARRAY_REF as its BASE, record the
   parameters occurring in IDX.  DTA is passed in as complementary
   information, and is used by the automatic walker function.  This
   function is a callback for for_each_index.  */

static bool
idx_record_params (tree base, tree *idx, void *dta)
{
  struct irp_data *data = (struct irp_data *) dta;

  if (TREE_CODE (base) != ARRAY_REF)
    return true;

  if (TREE_CODE (*idx) == SSA_NAME)
    {
      tree scev;
      sese sese = data->sese;
      struct loop *loop = data->loop;
      Value one;

      scev = analyze_scalar_evolution (loop, *idx);
      scev = instantiate_scev (block_before_sese (sese), loop, scev);

      value_init (one);
      value_set_si (one, 1);
      scan_tree_for_params (sese, scev, NULL, one, false);
      value_clear (one);
    }

  return true;
}

/* Find parameters with respect to SESE in BB. We are looking in memory
   access functions, conditions and loop bounds.  */

static void
find_params_in_bb (sese sese, gimple_bb_p gbb)
{
  int i;
  data_reference_p dr;
  gimple stmt;
  loop_p father = GBB_BB (gbb)->loop_father;

  for (i = 0; VEC_iterate (data_reference_p, GBB_DATA_REFS (gbb), i, dr); i++)
    {
      struct irp_data irp;

      irp.loop = father;
      irp.sese = sese;
      for_each_index (&dr->ref, idx_record_params, &irp);
    }

  /* Find parameters in conditional statements.  */ 
  for (i = 0; VEC_iterate (gimple, GBB_CONDITIONS (gbb), i, stmt); i++)
    {
      Value one;
      loop_p loop = father;

      tree lhs, rhs;

      lhs = gimple_cond_lhs (stmt);
      lhs = analyze_scalar_evolution (loop, lhs);
      lhs = instantiate_scev (block_before_sese (sese), loop, lhs);

      rhs = gimple_cond_rhs (stmt);
      rhs = analyze_scalar_evolution (loop, rhs);
      rhs = instantiate_scev (block_before_sese (sese), loop, rhs);

      value_init (one);
      value_set_si (one, 1);
      scan_tree_for_params (sese, lhs, NULL, one, false);
      scan_tree_for_params (sese, rhs, NULL, one, false);
      value_clear (one);
    }
}

/* Saves in NV the name of variable P->T.  */

static void
save_var_name (char **nv, int i, name_tree p)
{
  const char *name = get_name (SSA_NAME_VAR (p->t));

  if (name)
    {
      int len = strlen (name) + 16;
      nv[i] = XNEWVEC (char, len);
      snprintf (nv[i], len, "%s_%d", name, SSA_NAME_VERSION (p->t));
    }
  else
    {
      nv[i] = XNEWVEC (char, 16);
      snprintf (nv[i], 2 + 16, "T_%d", SSA_NAME_VERSION (p->t));
    }

  p->name = nv[i];
}

/* Initialize Cloog's parameter names from the names used in GIMPLE.
   Initialize Cloog's iterator names, using 'graphite_iterator_%d'
   from 0 to scop_nb_loops (scop).  */

static void
initialize_cloog_names (scop_p scop, CloogProgram *prog)
{
  sese region = SCOP_REGION (scop);
  int i, nb_params = VEC_length (name_tree, SESE_PARAMS (region));
  char **params = XNEWVEC (char *, nb_params);
  int nb_iterators = scop_max_loop_depth (scop);
  int nb_scattering = cloog_program_nb_scattdims (prog);
  char **iterators = XNEWVEC (char *, nb_iterators * 2);
  char **scattering = XNEWVEC (char *, nb_scattering);
  name_tree p;

  cloog_program_set_names (prog, cloog_names_malloc ());

  for (i = 0; VEC_iterate (name_tree, SESE_PARAMS (region), i, p); i++)
    save_var_name (params, i, p);

  cloog_names_set_nb_parameters (cloog_program_names (prog),
				 nb_params);
  cloog_names_set_parameters (cloog_program_names (prog),
			      params);

  for (i = 0; i < nb_iterators; i++)
    {
      int len = 18 + 16;
      iterators[i] = XNEWVEC (char, len);
      snprintf (iterators[i], len, "graphite_iterator_%d", i);
    }

  cloog_names_set_nb_iterators (cloog_program_names (prog),
				nb_iterators);
  cloog_names_set_iterators (cloog_program_names (prog),
			     iterators);

  for (i = 0; i < nb_scattering; i++)
    {
      int len = 2 + 16;
      scattering[i] = XNEWVEC (char, len);
      snprintf (scattering[i], len, "s_%d", i);
    }

  cloog_names_set_nb_scattering (cloog_program_names (prog),
				 nb_scattering);
  cloog_names_set_scattering (cloog_program_names (prog),
			      scattering);
}

/* Record the parameters used in the SCOP.  A variable is a parameter
   in a scop if it does not vary during the execution of that scop.  */

static void
find_scop_parameters (scop_p scop)
{
  poly_bb_p pbb;
  unsigned i;
  sese region = SCOP_REGION (scop);
  struct loop *loop;
  Value one;

  value_init (one);
  value_set_si (one, 1);

  /* Find the parameters used in the loop bounds.  */
  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST (scop), i, loop); i++)
    {
      tree nb_iters = number_of_latch_executions (loop);

      if (!chrec_contains_symbols (nb_iters))
	continue;

      nb_iters = analyze_scalar_evolution (loop, nb_iters);
      nb_iters = instantiate_scev (block_before_sese (SCOP_REGION (scop)), loop,
						      nb_iters);
      scan_tree_for_params (region, nb_iters, NULL, one, false);
    }

  value_clear (one);

  /* Find the parameters used in data accesses.  */
  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    find_params_in_bb (region, PBB_BLACK_BOX (pbb));

  SESE_ADD_PARAMS (region) = false;
}

/* Build the context constraints for SCOP: constraints and relations
   on parameters.  */

static void
build_scop_context (scop_p scop, CloogProgram *prog)
{
  ppl_Polyhedron_t ph;
  CloogDomain *dom;

  ppl_new_NNC_Polyhedron_from_space_dimension (&ph, scop_nb_params (scop), 0);
  dom = new_Cloog_Domain_from_ppl_Polyhedron (ph);
  cloog_program_set_context (prog, dom);
  ppl_delete_Polyhedron (ph);
}

/* Returns a gimple_bb from BB.  */

static inline gimple_bb_p
gbb_from_bb (basic_block bb)
{
  return (gimple_bb_p) bb->aux;
}

/* Builds the constraint polyhedra for LOOP in SCOP.  OUTER_PH gives
   the constraints for the surrounding loops.  */

static void
build_loop_iteration_domains (scop_p scop, struct loop *loop,
                              ppl_Polyhedron_t outer_ph, int nb)

{
  int i;
  poly_bb_p pbb;
  Value one, minus_one, val;
  ppl_Polyhedron_t ph;
  ppl_Linear_Expression_t lb_expr, ub_expr;
  ppl_Constraint_t lb, ub;
  ppl_Coefficient_t coef;
  ppl_const_Constraint_System_t pcs;
  tree nb_iters = number_of_latch_executions (loop);
  ppl_dimension_type dim = nb + 1 + scop_nb_params (scop);
  ppl_dimension_type *map;

  value_init (one);
  value_init (minus_one);
  value_init (val);
  value_set_si (one, 1);
  value_set_si (minus_one, -1);

  ppl_new_Linear_Expression_with_dimension (&lb_expr, dim);
  ppl_new_Linear_Expression_with_dimension (&ub_expr, dim);
  ppl_new_NNC_Polyhedron_from_space_dimension (&ph, dim, 0);

  /* 0 <= loop_i */
  ppl_new_Coefficient_from_mpz_t (&coef, one);
  ppl_Linear_Expression_add_to_coefficient (lb_expr, nb, coef);
  ppl_new_Constraint (&lb, lb_expr, PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);
  ppl_delete_Linear_Expression (lb_expr);

  /* loop_i <= nb_iters */
  ppl_assign_Coefficient_from_mpz_t (coef, minus_one);
  ppl_Linear_Expression_add_to_coefficient (ub_expr, nb, coef);

  if (TREE_CODE (nb_iters) == INTEGER_CST)
    {
      value_set_si (val, int_cst_value (nb_iters));
      ppl_assign_Coefficient_from_mpz_t (coef, val);
      ppl_Linear_Expression_add_to_inhomogeneous (ub_expr, coef);
      ppl_new_Constraint (&ub, ub_expr, PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);
    }
  else if (!chrec_contains_undetermined (nb_iters))
    {
      nb_iters = analyze_scalar_evolution (loop, nb_iters);
      nb_iters = instantiate_scev (block_before_sese (SCOP_REGION (scop)), loop,
						      nb_iters);
      scan_tree_for_params (SCOP_REGION (scop), nb_iters, ub_expr, one, false);
      ppl_new_Constraint (&ub, ub_expr, PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);
    }
  else
    gcc_unreachable ();

  ppl_delete_Linear_Expression (ub_expr);
  ppl_Polyhedron_get_constraints (outer_ph, &pcs);
  ppl_Polyhedron_add_constraints (ph, pcs);

  map = (ppl_dimension_type *) XNEWVEC (ppl_dimension_type, dim);
  for (i = 0; i < (int) nb; i++)
    map[i] = i;
  for (i = (int) nb; i < (int) dim - 1; i++)
    map[i] = i + 1;
  map[dim - 1] = nb;

  ppl_Polyhedron_map_space_dimensions (ph, map, dim);
  free (map);
  ppl_Polyhedron_add_constraint (ph, lb);
  ppl_Polyhedron_add_constraint (ph, ub);
  ppl_delete_Constraint (lb);
  ppl_delete_Constraint (ub);

  if (loop->inner && loop_in_sese_p (loop->inner, SCOP_REGION (scop)))
    build_loop_iteration_domains (scop, loop->inner, ph, nb + 1);

  if (nb != 0
      && loop->next
      && loop_in_sese_p (loop->next, SCOP_REGION (scop)))
    build_loop_iteration_domains (scop, loop->next, outer_ph, nb);

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    if (gbb_loop (PBB_BLACK_BOX (pbb)) == loop)
      {
	ppl_delete_Polyhedron (PBB_DOMAIN (pbb));
	ppl_new_NNC_Polyhedron_from_NNC_Polyhedron (&PBB_DOMAIN (pbb), ph);
      }

  ppl_delete_Coefficient (coef);
  ppl_delete_Polyhedron (ph);
  value_clear (one);
  value_clear (minus_one);
  value_clear (val);
}

/* Add conditions to the domain of GB.  */

static void
add_conditions_to_domain (poly_bb_p pbb)
{
  unsigned int i;
  gimple stmt;
  gimple_bb_p gbb = PBB_BLACK_BOX (pbb);
  VEC (gimple, heap) *conditions = GBB_CONDITIONS (gbb);
  scop_p scop = PBB_SCOP (pbb);
  basic_block before_scop = block_before_sese (SCOP_REGION (scop));

  if (VEC_empty (gimple, conditions))
    return;

  /* Add the conditions to the new enlarged domain.  */
  for (i = 0; VEC_iterate (gimple, conditions, i, stmt); i++)
    {
      switch (gimple_code (stmt))
        {
        case GIMPLE_COND:
          {
            Value one;
            tree left, right;
            loop_p loop = GBB_BB (gbb)->loop_father;
	    ppl_Linear_Expression_t expr;
	    ppl_Constraint_t cstr;
	    enum ppl_enum_Constraint_Type type = 0;
            enum tree_code code = gimple_cond_code (stmt);
	    ppl_dimension_type dim;

            /* The conditions for ELSE-branches are inverted.  */
            if (VEC_index (gimple, gbb->condition_cases, i) == NULL)
              code = invert_tree_comparison (code, false);

            switch (code)
              {
              case LT_EXPR:
		type = PPL_CONSTRAINT_TYPE_LESS_THAN;
                break;

              case GT_EXPR:
		type = PPL_CONSTRAINT_TYPE_GREATER_THAN;
                break;

              case LE_EXPR:
		type = PPL_CONSTRAINT_TYPE_LESS_OR_EQUAL;
                break;

              case GE_EXPR:
		type = PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL;
                break;

              default:
                /* NE and EQ statements are not supported right now. */
                gcc_unreachable ();
                break;
              }

	    value_init (one);
	    value_set_si (one, 1);
	    dim = pbb_nb_loops (pbb) + scop_nb_params (scop);
	    ppl_new_Linear_Expression_with_dimension (&expr, dim);

	    left = gimple_cond_lhs (stmt);
	    left = analyze_scalar_evolution (loop, left);
	    left = instantiate_scev (before_scop, loop, left);

	    right = gimple_cond_rhs (stmt);
	    right = analyze_scalar_evolution (loop, right);
	    right = instantiate_scev (before_scop, loop, right);

	    scan_tree_for_params (SCOP_REGION (scop), left, expr, one, true);
	    value_set_si (one, 1);
	    scan_tree_for_params (SCOP_REGION (scop), right, expr, one, false);

	    value_clear (one);
	    ppl_new_Constraint (&cstr, expr, type);
	    ppl_Polyhedron_add_constraint (PBB_DOMAIN (pbb), cstr);
	    ppl_delete_Constraint (cstr);
	    ppl_delete_Linear_Expression (expr);
            break;
          }
        case GIMPLE_SWITCH:
          /* Switch statements are not supported right know.  */
        default:
          gcc_unreachable ();
          break;
        }
    }
}

/* Returns true when PHI defines an induction variable in the loop
   containing the PHI node.  */

static bool
phi_node_is_iv (gimple phi)
{
  loop_p loop = gimple_bb (phi)->loop_father;
  tree scev = analyze_scalar_evolution (loop, gimple_phi_result (phi));

  return tree_contains_chrecs (scev, NULL);
}

/* Returns true when BB contains scalar phi nodes that are not an
   induction variable of a loop.  */

static bool
bb_contains_non_iv_scalar_phi_nodes (basic_block bb)
{
  gimple phi = NULL;
  gimple_stmt_iterator si;

  for (si = gsi_start_phis (bb); !gsi_end_p (si); gsi_next (&si))
    if (is_gimple_reg (gimple_phi_result (gsi_stmt (si))))
      {
	/* Store the unique scalar PHI node: at this point, loops
	   should be in cannonical form, so we expect to see at most
	   one scalar phi node in the loop header.  */
	if (phi || bb != bb->loop_father->header)
	  return true;

	phi = gsi_stmt (si);
      }

  if (!phi || phi_node_is_iv (phi))
    return false;

  return true;
}


/* Check if SCOP contains non scalar phi nodes.  */

static bool
scop_contains_non_iv_scalar_phi_nodes (scop_p scop)
{
  int i;
  poly_bb_p pbb;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    if (bb_contains_non_iv_scalar_phi_nodes (GBB_BB (PBB_BLACK_BOX (pbb))))
      return true;

  return false;
}

/* Helper recursive function.  Record in CONDITIONS and CASES all
   conditions from 'if's and 'switch'es occurring in BB from REGION.  */

static void
build_sese_conditions_1 (VEC (gimple, heap) **conditions,
			 VEC (gimple, heap) **cases, basic_block bb,
			 sese region)
{
  int i, j;
  gimple_bb_p gbb;
  basic_block bb_child, bb_iter;
  VEC (basic_block, heap) *dom;
  gimple stmt;
  
  /* Make sure we are in the SCoP.  */
  if (!bb_in_sese_p (bb, region))
    return;

  gbb = gbb_from_bb (bb);
  if (gbb)
    {
      GBB_CONDITIONS (gbb) = VEC_copy (gimple, heap, *conditions);
      GBB_CONDITION_CASES (gbb) = VEC_copy (gimple, heap, *cases);
    }

  dom = get_dominated_by (CDI_DOMINATORS, bb);

  stmt = last_stmt (bb);
  if (stmt)
    {
      VEC (edge, gc) *edges;
      edge e;

      switch (gimple_code (stmt))
	{
	case GIMPLE_COND:
	  edges = bb->succs;
	  for (i = 0; VEC_iterate (edge, edges, i, e); i++)
	    if ((dominated_by_p (CDI_DOMINATORS, e->dest, bb))
		&& VEC_length (edge, e->dest->preds) == 1)
	      {
		/* Remove the scanned block from the dominator successors.  */
		for (j = 0; VEC_iterate (basic_block, dom, j, bb_iter); j++)
		  if (bb_iter == e->dest)
		    {
		      VEC_unordered_remove (basic_block, dom, j);
		      break;
		    }

		/* Recursively scan the then or else part.  */
		if (e->flags & EDGE_TRUE_VALUE)
		  VEC_safe_push (gimple, heap, *cases, stmt);
		else 
		  {
		    gcc_assert (e->flags & EDGE_FALSE_VALUE);
		    VEC_safe_push (gimple, heap, *cases, NULL);
		  }

		VEC_safe_push (gimple, heap, *conditions, stmt);
		build_sese_conditions_1 (conditions, cases, e->dest, region);
		VEC_pop (gimple, *conditions);
		VEC_pop (gimple, *cases);
	      }
	  break;

	case GIMPLE_SWITCH:
	  {
	    unsigned i;
	    gimple_stmt_iterator gsi_search_gimple_label;

	    for (i = 0; i < gimple_switch_num_labels (stmt); ++i)
	      {
		basic_block bb_iter;
		size_t k;
		size_t n_cases = VEC_length (gimple, *conditions);
		unsigned n = gimple_switch_num_labels (stmt);

		bb_child = label_to_block
		  (CASE_LABEL (gimple_switch_label (stmt, i)));

		for (k = 0; k < n; k++)
		  if (i != k
		      && label_to_block 
		      (CASE_LABEL (gimple_switch_label (stmt, k))) == bb_child)
		    break;

		/* Switches with multiple case values for the same
		   block are not handled.  */
		if (k != n
		    /* Switch cases with more than one predecessor are
		       not handled.  */
		    || VEC_length (edge, bb_child->preds) != 1)
		  gcc_unreachable ();

		/* Recursively scan the corresponding 'case' block.  */
		for (gsi_search_gimple_label = gsi_start_bb (bb_child);
		     !gsi_end_p (gsi_search_gimple_label);
		     gsi_next (&gsi_search_gimple_label))
		  {
		    gimple label = gsi_stmt (gsi_search_gimple_label);

		    if (gimple_code (label) == GIMPLE_LABEL)
		      {
			tree t = gimple_label_label (label);

			gcc_assert (t == gimple_switch_label (stmt, i));
			VEC_replace (gimple, *cases, n_cases, label);
			break;
		      }
		  }

		build_sese_conditions_1 (conditions, cases, bb_child, region);

		/* Remove the scanned block from the dominator successors.  */
		for (j = 0; VEC_iterate (basic_block, dom, j, bb_iter); j++)
		  if (bb_iter == bb_child)
		    {
		      VEC_unordered_remove (basic_block, dom, j);
		      break;
		    }
	      }

	    VEC_pop (gimple, *conditions);
	    VEC_pop (gimple, *cases);
	    break;
	  }

	default:
	  break;
      }
  }

  /* Scan all immediate dominated successors.  */
  for (i = 0; VEC_iterate (basic_block, dom, i, bb_child); i++)

  VEC_free (basic_block, heap, dom);
}

/* Record all conditions in REGION.  */

static void 
build_sese_conditions (sese region)
{
  VEC (gimple, heap) *conditions = NULL;
  VEC (gimple, heap) *cases = NULL;

  build_sese_conditions_1 (&conditions, &cases, SESE_ENTRY (region)->dest,
			     region);

  VEC_free (gimple, heap, conditions);
  VEC_free (gimple, heap, cases);
}

/* Traverses all the GBBs of the SCOP and add their constraints to the
   iteration domains.  */

static void
add_conditions_to_constraints (scop_p scop)
{
  int i;
  poly_bb_p pbb;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    add_conditions_to_domain (pbb);
}

/* Build the iteration domains: the loops belonging to the current
   SCOP, and that vary for the execution of the current basic block.
   Returns false if there is no loop in SCOP.  */

static void 
build_scop_iteration_domain (scop_p scop)
{
  struct loop *loop;
  sese region = SCOP_REGION (scop);
  int i;

  /* Build cloog loop for all loops, that are in the uppermost loop layer of
     this SCoP.  */
  for (i = 0; VEC_iterate (loop_p, SESE_LOOP_NEST (region), i, loop); i++)
    if (!loop_in_sese_p (loop_outer (loop), region)) 
      {
	ppl_Polyhedron_t ph;
	ppl_new_NNC_Polyhedron_from_space_dimension (&ph, 0, 0);
	build_loop_iteration_domains (scop, loop, ph, 0);
	ppl_delete_Polyhedron (ph);
      }
}

/* Initializes an equation CY of the access matrix using the
   information for a subscript from AF, relatively to the loop
   indexes from LOOP_NEST and parameter indexes from PARAMS.  NDIM is
   the dimension of the array access, i.e. the number of
   subscripts.  Returns true when the operation succeeds.  */

static bool
build_access_matrix_with_af (tree af, lambda_vector cy,
			     sese region, int ndim)
{
  int param_col;

  switch (TREE_CODE (af))
    {
    case POLYNOMIAL_CHREC:
      {
        struct loop *outer_loop;
	tree left = CHREC_LEFT (af);
	tree right = CHREC_RIGHT (af);
	int var;

	if (TREE_CODE (right) != INTEGER_CST)
	  return false;

        outer_loop = get_loop (CHREC_VARIABLE (af));
        var = nb_loops_around_loop_in_sese (outer_loop, region);
	cy[var] = int_cst_value (right);

	switch (TREE_CODE (left))
	  {
	  case POLYNOMIAL_CHREC:
	    return build_access_matrix_with_af (left, cy, region, ndim);

	  case INTEGER_CST:
	    cy[ndim - 1] = int_cst_value (left);
	    return true;

	  default:
	    return build_access_matrix_with_af (left, cy, region, ndim);
	  }
      }

    case PLUS_EXPR:
      build_access_matrix_with_af (TREE_OPERAND (af, 0), cy, region, ndim);
      build_access_matrix_with_af (TREE_OPERAND (af, 1), cy, region, ndim);
      return true;
      
    case MINUS_EXPR:
      build_access_matrix_with_af (TREE_OPERAND (af, 0), cy, region, ndim);
      build_access_matrix_with_af (TREE_OPERAND (af, 1), cy, region, ndim);
      return true;

    case INTEGER_CST:
      cy[ndim - 1] = int_cst_value (af);
      return true;

    case SSA_NAME:
      param_col = parameter_index_in_region (af, region);
      cy [ndim - sese_nb_params (region) + param_col - 1] = 1; 
      return true;

    default:
      /* FIXME: access_fn can have parameters.  */
      return false;
    }
}

/* Initialize the access matrix in the data reference REF with respect
   to the loop nesting LOOP_NEST.  Return true when the operation
   succeeded.  */

static bool
build_access_matrix (data_reference_p ref, poly_bb_p pbb)
{
  int i, ndim = DR_NUM_DIMENSIONS (ref);
  struct access_matrix *am = GGC_NEW (struct access_matrix);
  sese region = SCOP_REGION (PBB_SCOP (pbb));

  AM_MATRIX (am) = VEC_alloc (lambda_vector, gc, ndim);

  for (i = 0; i < ndim; i++)
    {
      lambda_vector v = lambda_vector_new (ref_nb_loops (ref, region));
      tree af = DR_ACCESS_FN (ref, i);

      if (!build_access_matrix_with_af (af, v, region,
					ref_nb_loops (ref, region)))
	return false;

      VEC_quick_push (lambda_vector, AM_MATRIX (am), v);
    }

  DR_ACCESS_MATRIX (ref) = am;
  return true;
}

/* Build the access matrices for the data references in the SCOP.  */

static void
build_scop_data_accesses (scop_p scop)
{
  int i;
  poly_bb_p pbb;

  /* FIXME: Construction of access matrix is disabled until some
     pass, like the data dependence analysis, is using it.  */
  return;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    {
      int j;
      data_reference_p dr;

      /* Construct the access matrix for each data ref, with respect to
	 the loop nest of the current BB in the considered SCOP.  */
      for (j = 0;
	   VEC_iterate (data_reference_p,
			GBB_DATA_REFS (PBB_BLACK_BOX (pbb)), j, dr); j++)
	{
	  bool res = build_access_matrix (dr, pbb);

	  /* FIXME: At this point the DRs should always have an affine
	     form.  For the moment this fails as build_access_matrix
	     does not build matrices with parameters.  */
	  gcc_assert (res);
	}
    }
}

/* Returns the tree variable from the name NAME that was given in
   Cloog representation.  All the parameters are stored in PARAMS, and
   all the loop induction variables are stored in IVSTACK.

   FIXME: This is a hack, and Cloog should be fixed to not work with
   variable names represented as "char *string", but with void
   pointers that could be casted back to a tree.  The only problem in
   doing that is that Cloog's pretty printer still assumes that
   variable names are char *strings.  The solution would be to have a
   function pointer for pretty-printing that can be redirected to be
   print_generic_stmt in our case, or fprintf by default.
   ???  Too ugly to live.  */

static tree
clast_name_to_gcc (const char *name, VEC (name_tree, heap) *params, 
		   loop_iv_stack ivstack)
{
  int i;
  name_tree t;
  tree iv;

  if (params)
    for (i = 0; VEC_iterate (name_tree, params, i, t); i++)
      if (!strcmp (name, t->name))
	return t->t;

  iv = loop_iv_stack_get_iv_from_name (ivstack, name);
  if (iv)
    return iv;

  gcc_unreachable ();
}

/* Returns the maximal precision type for expressions E1 and E2.  */

static inline tree
max_precision_type (tree e1, tree e2)
{
  tree type1 = TREE_TYPE (e1);
  tree type2 = TREE_TYPE (e2);
  return TYPE_PRECISION (type1) > TYPE_PRECISION (type2) ? type1 : type2;
}

static tree
clast_to_gcc_expression (tree, struct clast_expr *, VEC (name_tree, heap) *,
			 loop_iv_stack);

/* Converts a Cloog reduction expression R with reduction operation OP
   to a GCC expression tree of type TYPE.  PARAMS is a vector of
   parameters of the region, and IVSTACK contains the stack of induction
   variables.  */

static tree
clast_to_gcc_expression_red (tree type, enum tree_code op,
			     struct clast_reduction *r,
			     VEC (name_tree, heap) *params,
			     loop_iv_stack ivstack)
{
  int i;
  tree res = clast_to_gcc_expression (type, r->elts[0], params, ivstack);

  for (i = 1; i < r->n; i++)
    {
      tree t = clast_to_gcc_expression (type, r->elts[i], params, ivstack);
      res = fold_build2 (op, type, res, t);
    }
  return res;
}

/* Converts a Cloog AST expression E back to a GCC expression tree of
   type TYPE.  PARAMS is a vector of parameters of the region, and
   IVSTACK contains the stack of induction variables.  */

static tree
clast_to_gcc_expression (tree type, struct clast_expr *e,
			 VEC (name_tree, heap) *params,
			 loop_iv_stack ivstack)
{
  switch (e->type)
    {
    case expr_term:
      {
	struct clast_term *t = (struct clast_term *) e;

	if (t->var)
	  {
	    if (value_one_p (t->val))
	      {
		tree name = clast_name_to_gcc (t->var, params, ivstack);
		return fold_convert (type, name);
	      }

	    else if (value_mone_p (t->val))
	      {
		tree name = clast_name_to_gcc (t->var, params, ivstack);
		name = fold_convert (type, name);
		return fold_build1 (NEGATE_EXPR, type, name);
	      }
	    else
	      {
		tree name = clast_name_to_gcc (t->var, params, ivstack);
		tree cst = gmp_cst_to_tree (type, t->val);
		name = fold_convert (type, name);
		return fold_build2 (MULT_EXPR, type, cst, name);
	      }
	  }
	else
	  return gmp_cst_to_tree (type, t->val);
      }

    case expr_red:
      {
        struct clast_reduction *r = (struct clast_reduction *) e;

        switch (r->type)
          {
	  case clast_red_sum:
	    return clast_to_gcc_expression_red (type, PLUS_EXPR, r, params, ivstack);

	  case clast_red_min:
	    return clast_to_gcc_expression_red (type, MIN_EXPR, r, params, ivstack);

	  case clast_red_max:
	    return clast_to_gcc_expression_red (type, MAX_EXPR, r, params, ivstack);

	  default:
	    gcc_unreachable ();
          }
        break;
      }

    case expr_bin:
      {
	struct clast_binary *b = (struct clast_binary *) e;
	struct clast_expr *lhs = (struct clast_expr *) b->LHS;
	tree tl = clast_to_gcc_expression (type, lhs, params, ivstack);
	tree tr = gmp_cst_to_tree (type, b->RHS);

	switch (b->type)
	  {
	  case clast_bin_fdiv:
	    return fold_build2 (FLOOR_DIV_EXPR, type, tl, tr);

	  case clast_bin_cdiv:
	    return fold_build2 (CEIL_DIV_EXPR, type, tl, tr);

	  case clast_bin_div:
	    return fold_build2 (EXACT_DIV_EXPR, type, tl, tr);

	  case clast_bin_mod:
	    return fold_build2 (TRUNC_MOD_EXPR, type, tl, tr);

	  default:
	    gcc_unreachable ();
	  }
      }

    default:
      gcc_unreachable ();
    }

  return NULL_TREE;
}

/* Returns the type for the expression E.  */

static tree
gcc_type_for_clast_expr (struct clast_expr *e,
			 VEC (name_tree, heap) *params,
			 loop_iv_stack ivstack)
{
  switch (e->type)
    {
    case expr_term:
      {
	struct clast_term *t = (struct clast_term *) e;

	if (t->var)
	  return TREE_TYPE (clast_name_to_gcc (t->var, params, ivstack));
	else
	  return NULL_TREE;
      }

    case expr_red:
      {
        struct clast_reduction *r = (struct clast_reduction *) e;

	if (r->n == 1)
	  return gcc_type_for_clast_expr (r->elts[0], params, ivstack);
	else 
	  {
	    int i;
	    for (i = 0; i < r->n; i++)
	      {
		tree type = gcc_type_for_clast_expr (r->elts[i], params, ivstack);
		if (type)
		  return type;
	      }
	    return NULL_TREE;
	  }
      }

    case expr_bin:
      {
	struct clast_binary *b = (struct clast_binary *) e;
	struct clast_expr *lhs = (struct clast_expr *) b->LHS;
	return gcc_type_for_clast_expr (lhs, params, ivstack);
      }

    default:
      gcc_unreachable ();
    }

  return NULL_TREE;
}

/* Returns the type for the equation CLEQ.  */

static tree
gcc_type_for_clast_eq (struct clast_equation *cleq,
		       VEC (name_tree, heap) *params,
		       loop_iv_stack ivstack)
{
  tree type = gcc_type_for_clast_expr (cleq->LHS, params, ivstack);
  if (type)
    return type;

  return gcc_type_for_clast_expr (cleq->RHS, params, ivstack);
}

/* Translates a clast equation CLEQ to a tree.  */

static tree
graphite_translate_clast_equation (sese region,
				   struct clast_equation *cleq,
				   loop_iv_stack ivstack)
{
  enum tree_code comp;
  tree type = gcc_type_for_clast_eq (cleq, SESE_PARAMS (region), ivstack);
  tree lhs = clast_to_gcc_expression (type, cleq->LHS, SESE_PARAMS (region),
				      ivstack);
  tree rhs = clast_to_gcc_expression (type, cleq->RHS, SESE_PARAMS (region),
				      ivstack);

  if (cleq->sign == 0)
    comp = EQ_EXPR;

  else if (cleq->sign > 0)
    comp = GE_EXPR;

  else
    comp = LE_EXPR;

  return fold_build2 (comp, type, lhs, rhs);
}

/* Creates the test for the condition in STMT.  */

static tree
graphite_create_guard_cond_expr (sese region, struct clast_guard *stmt, 
				 loop_iv_stack ivstack)
{
  tree cond = NULL;
  int i;

  for (i = 0; i < stmt->n; i++)
    {
      tree eq = graphite_translate_clast_equation (region, &stmt->eq[i], ivstack);

      if (cond)
	cond = fold_build2 (TRUTH_AND_EXPR, TREE_TYPE (eq), cond, eq);
      else
	cond = eq;
    }

  return cond;
}

/* Creates a new if region corresponding to Cloog's guard.  */

static edge 
graphite_create_new_guard (sese region, edge entry_edge,
			   struct clast_guard *stmt, 
			   loop_iv_stack ivstack)
{
  tree cond_expr = graphite_create_guard_cond_expr (region, stmt, ivstack);
  edge exit_edge = create_empty_if_region_on_edge (entry_edge, cond_expr);
  return exit_edge;
}

/* Walks a CLAST and returns the first statement in the body of a
   loop.  */

static struct clast_user_stmt *
clast_get_body_of_loop (struct clast_stmt *stmt)
{
  if (!stmt
      || CLAST_STMT_IS_A (stmt, stmt_user))
    return (struct clast_user_stmt *) stmt;

  if (CLAST_STMT_IS_A (stmt, stmt_for))
    return clast_get_body_of_loop (((struct clast_for *) stmt)->body);

  if (CLAST_STMT_IS_A (stmt, stmt_guard))
    return clast_get_body_of_loop (((struct clast_guard *) stmt)->then);

  if (CLAST_STMT_IS_A (stmt, stmt_block))
    return clast_get_body_of_loop (((struct clast_block *) stmt)->body);

  gcc_unreachable ();
}

/* Returns the induction variable for the loop that gets translated to
   STMT.  */

static tree
gcc_type_for_iv_of_clast_loop (struct clast_for *stmt_for)
{
  struct clast_user_stmt *stmt = clast_get_body_of_loop ((struct clast_stmt *) stmt_for);
  const char *cloog_iv = stmt_for->iterator;
  CloogStatement *cs = stmt->statement;
  poly_bb_p pbb = (poly_bb_p) cloog_statement_usr (cs);

  return gcc_type_for_cloog_iv (cloog_iv, PBB_BLACK_BOX (pbb));
}

/* Creates a new LOOP corresponding to Cloog's STMT.  Inserts an induction 
   variable for the new LOOP.  New LOOP is attached to CFG starting at
   ENTRY_EDGE.  LOOP is inserted into the loop tree and becomes the child
   loop of the OUTER_LOOP.  */

static struct loop *
graphite_create_new_loop (sese region, edge entry_edge,
			  struct clast_for *stmt, loop_iv_stack ivstack,
			  loop_p outer)
{
  tree type = gcc_type_for_iv_of_clast_loop (stmt);
  VEC (name_tree, heap) *params = SESE_PARAMS (region);
  tree lb = clast_to_gcc_expression (type, stmt->LB, params, ivstack);
  tree ub = clast_to_gcc_expression (type, stmt->UB, params, ivstack);
  tree stride = gmp_cst_to_tree (type, stmt->stride);
  tree ivvar = create_tmp_var (type, "graphiteIV");
  tree iv_before;
  loop_p loop = create_empty_loop_on_edge
    (entry_edge, lb, stride, ub, ivvar, &iv_before,
     outer ? outer : entry_edge->src->loop_father);

  add_referenced_var (ivvar);
  loop_iv_stack_push_iv (ivstack, iv_before, stmt->iterator);
  return loop;
}

/* Rename the SSA_NAMEs used in STMT and that appear in IVSTACK.  */

static void 
rename_variables_in_stmt (gimple stmt, htab_t map)
{
  ssa_op_iter iter;
  use_operand_p use_p;

  FOR_EACH_SSA_USE_OPERAND (use_p, stmt, iter, SSA_OP_USE)
    {
      tree use = USE_FROM_PTR (use_p);
      tree new_name = get_new_name_from_old_name (map, use);

      replace_exp (use_p, new_name);
    }

  update_stmt (stmt);
}

/* Returns true if SSA_NAME is a parameter of SESE.  */

static bool
is_parameter (sese region, tree ssa_name)
{
  int i;
  VEC (name_tree, heap) *params = SESE_PARAMS (region);
  name_tree param;

  for (i = 0; VEC_iterate (name_tree, params, i, param); i++)
    if (param->t == ssa_name)
      return true;

  return false;
}

/* Returns true if NAME is an induction variable.  */

static bool
is_iv (tree name)
{
  return gimple_code (SSA_NAME_DEF_STMT (name)) == GIMPLE_PHI;
}

static void expand_scalar_variables_stmt (gimple, basic_block, sese,
					  htab_t);
static tree
expand_scalar_variables_expr (tree, tree, enum tree_code, tree, basic_block,
			      sese, htab_t, gimple_stmt_iterator *);

/* Copies at GSI all the scalar computations on which the ssa_name OP0
   depends on in the SESE: these are all the scalar variables used in
   the definition of OP0, that are defined outside BB and still in the
   SESE, i.e. not a parameter of the SESE.  The expression that is
   returned contains only induction variables from the generated code:
   MAP contains the induction variables renaming mapping, and is used
   to translate the names of induction variables.  */

static tree
expand_scalar_variables_ssa_name (tree op0, basic_block bb,
				  sese region, htab_t map, 
				  gimple_stmt_iterator *gsi)
{
  tree var0, var1, type;
  gimple def_stmt;
  enum tree_code subcode;
      
  if (is_parameter (region, op0)
      || is_iv (op0))
    return get_new_name_from_old_name (map, op0);
      
  def_stmt = SSA_NAME_DEF_STMT (op0);
      
  if (gimple_bb (def_stmt) == bb)
    {
      /* If the defining statement is in the basic block already
	 we do not need to create a new expression for it, we
	 only need to ensure its operands are expanded.  */
      expand_scalar_variables_stmt (def_stmt, bb, region, map);
      return get_new_name_from_old_name (map, op0);
    }
  else
    {
      if (gimple_code (def_stmt) != GIMPLE_ASSIGN
	  || !bb_in_sese_p (gimple_bb (def_stmt), region))
	return get_new_name_from_old_name (map, op0);

      var0 = gimple_assign_rhs1 (def_stmt);
      subcode = gimple_assign_rhs_code (def_stmt);
      var1 = gimple_assign_rhs2 (def_stmt);
      type = gimple_expr_type (def_stmt);

      return expand_scalar_variables_expr (type, var0, subcode, var1, bb, region,
					   map, gsi);
    }
}

/* Copies at GSI all the scalar computations on which the expression
   OP0 CODE OP1 depends on in the SESE: these are all the scalar
   variables used in OP0 and OP1, defined outside BB and still defined
   in the SESE, i.e. not a parameter of the SESE.  The expression that
   is returned contains only induction variables from the generated
   code: MAP contains the induction variables renaming mapping, and is
   used to translate the names of induction variables.  */

static tree
expand_scalar_variables_expr (tree type, tree op0, enum tree_code code, 
			      tree op1, basic_block bb, sese region, 
			      htab_t map, gimple_stmt_iterator *gsi)
{
  if (TREE_CODE_CLASS (code) == tcc_constant
      || TREE_CODE_CLASS (code) == tcc_declaration)
    return op0;

  /* For data references we have to duplicate also its memory
     indexing.  */
  if (TREE_CODE_CLASS (code) == tcc_reference)
    {
      switch (code)
	{
	case INDIRECT_REF:
	  {
	    tree old_name = TREE_OPERAND (op0, 0);
	    tree expr = expand_scalar_variables_ssa_name
	      (old_name, bb, region, map, gsi);
	    tree new_name = force_gimple_operand_gsi (gsi, expr, true, NULL,
						      true, GSI_SAME_STMT);

	    set_symbol_mem_tag (SSA_NAME_VAR (new_name),
				symbol_mem_tag (SSA_NAME_VAR (old_name)));
	    return fold_build1 (code, type, new_name);
	  }

	case ARRAY_REF:
	  {
	    tree op00 = TREE_OPERAND (op0, 0);
	    tree op01 = TREE_OPERAND (op0, 1);
	    tree op02 = TREE_OPERAND (op0, 2);
	    tree op03 = TREE_OPERAND (op0, 3);
	    tree base = expand_scalar_variables_expr
	      (TREE_TYPE (op00), op00, TREE_CODE (op00), NULL, bb, region,
	       map, gsi);
	    tree subscript = expand_scalar_variables_expr
	      (TREE_TYPE (op01), op01, TREE_CODE (op01), NULL, bb, region,
	       map, gsi);

	    return build4 (ARRAY_REF, type, base, subscript, op02, op03);
	  }

	default:
	  /* The above cases should catch everything.  */
	  gcc_unreachable ();
	}
    }

  if (TREE_CODE_CLASS (code) == tcc_unary)
    {
      tree op0_type = TREE_TYPE (op0);
      enum tree_code op0_code = TREE_CODE (op0);
      tree op0_expr = expand_scalar_variables_expr (op0_type, op0, op0_code,
						    NULL, bb, region, map, gsi);
  
      return fold_build1 (code, type, op0_expr);
    }

  if (TREE_CODE_CLASS (code) == tcc_binary)
    {
      tree op0_type = TREE_TYPE (op0);
      enum tree_code op0_code = TREE_CODE (op0);
      tree op0_expr = expand_scalar_variables_expr (op0_type, op0, op0_code,
						    NULL, bb, region, map, gsi);
      tree op1_type = TREE_TYPE (op1);
      enum tree_code op1_code = TREE_CODE (op1);
      tree op1_expr = expand_scalar_variables_expr (op1_type, op1, op1_code,
						    NULL, bb, region, map, gsi);

      return fold_build2 (code, type, op0_expr, op1_expr);
    }

  if (code == SSA_NAME)
    return expand_scalar_variables_ssa_name (op0, bb, region, map, gsi);

  gcc_unreachable ();
  return NULL;
}

/* Copies at the beginning of BB all the scalar computations on which
   STMT depends on in the SESE: these are all the scalar variables used
   in STMT, defined outside BB and still defined in the SESE, i.e. not a
   parameter of the SESE.  The expression that is returned contains
   only induction variables from the generated code: MAP contains the
   induction variables renaming mapping, and is used to translate the
   names of induction variables.  */
 
static void
expand_scalar_variables_stmt (gimple stmt, basic_block bb, sese region,
			      htab_t map)
{
  ssa_op_iter iter;
  use_operand_p use_p;
  gimple_stmt_iterator gsi = gsi_after_labels (bb);

  FOR_EACH_SSA_USE_OPERAND (use_p, stmt, iter, SSA_OP_USE)
    {
      tree use = USE_FROM_PTR (use_p);
      tree type = TREE_TYPE (use);
      enum tree_code code = TREE_CODE (use);
      tree use_expr = expand_scalar_variables_expr (type, use, code, NULL, bb,
						    region, map, &gsi);
      if (use_expr != use)
	{
	  tree new_use =
	    force_gimple_operand_gsi (&gsi, use_expr, true, NULL,
				      true, GSI_NEW_STMT);
	  replace_exp (use_p, new_use);
	}
    }

  update_stmt (stmt);
}

/* Copies at the beginning of BB all the scalar computations on which
   BB depends on in the SESE: these are all the scalar variables used
   in BB, defined outside BB and still defined in the SESE, i.e. not a
   parameter of the SESE.  The expression that is returned contains
   only induction variables from the generated code: MAP contains the
   induction variables renaming mapping, and is used to translate the
   names of induction variables.  */

static void 
expand_scalar_variables (basic_block bb, sese region, htab_t map)
{
  gimple_stmt_iterator gsi;
  
  for (gsi = gsi_after_labels (bb); !gsi_end_p (gsi);)
    {
      gimple stmt = gsi_stmt (gsi);
      expand_scalar_variables_stmt (stmt, bb, region, map);
      gsi_next (&gsi);
    }
}

/* Rename all the SSA_NAMEs from block BB according to the MAP.  */

static void 
rename_variables (basic_block bb, htab_t map)
{
  gimple_stmt_iterator gsi;
  
  for (gsi = gsi_after_labels (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    rename_variables_in_stmt (gsi_stmt (gsi), map);
}

/* Remove condition from BB.  */

static void
remove_condition (basic_block bb)
{
  gimple last = last_stmt (bb);

  if (last && gimple_code (last) == GIMPLE_COND)
    {
      gimple_stmt_iterator gsi = gsi_last_bb (bb);
      gsi_remove (&gsi, true);
    }
}

/* Returns the first successor edge of BB with EDGE_TRUE_VALUE flag set.  */

static edge
get_true_edge_from_guard_bb (basic_block bb)
{
  edge e;
  edge_iterator ei;

  FOR_EACH_EDGE (e, ei, bb->succs)
    if (e->flags & EDGE_TRUE_VALUE) 
      return e;

  gcc_unreachable ();
  return NULL;
}

/* Returns the first successor edge of BB with EDGE_TRUE_VALUE flag cleared.  */

static edge
get_false_edge_from_guard_bb (basic_block bb)
{
  edge e;
  edge_iterator ei;

  FOR_EACH_EDGE (e, ei, bb->succs)
    if (!(e->flags & EDGE_TRUE_VALUE)) 
      return e;

  gcc_unreachable ();
  return NULL;
}

/* Inserts in MAP a tuple (OLD_NAME, NEW_NAME) for the induction
   variables of the loops around GBB in SESE, i.e. GBB_LOOPS.
   NEW_NAME is obtained from IVSTACK.  IVSTACK has the same stack
   ordering as GBB_LOOPS.  */

static void
build_iv_mapping (loop_iv_stack ivstack, htab_t map, poly_bb_p pbb,
		  sese region)
{
  int i;
  name_tree iv;
  PTR *slot;
  gimple_bb_p gbb = PBB_BLACK_BOX (pbb);

  for (i = 0; VEC_iterate (name_tree, SESE_OLDIVS (region), i, iv); i++)
    {
      struct rename_map_elt tmp;

      if (!flow_bb_inside_loop_p (iv->loop, GBB_BB (gbb)))
	continue;

      tmp.old_name = iv->t;
      slot = htab_find_slot (map, &tmp, INSERT);

      if (!*slot)
	{
	  tree new_name = loop_iv_stack_get_iv (ivstack, 
						pbb_loop_index (pbb, iv->loop));

	  *slot = new_rename_map_elt (iv->t, new_name);
	}
    }
}

/* Register in MAP the tuple (old_name, new_name).  */

static void
register_old_and_new_names (htab_t map, tree old_name, tree new_name)
{
  struct rename_map_elt tmp;
  PTR *slot;

  tmp.old_name = old_name;
  slot = htab_find_slot (map, &tmp, INSERT);

  if (!*slot)
    *slot = new_rename_map_elt (old_name, new_name);
}

/* Create a duplicate of the basic block BB.  NOTE: This does not
   preserve SSA form.  */

static void
graphite_copy_stmts_from_block (basic_block bb, basic_block new_bb, htab_t map)
{
  gimple_stmt_iterator gsi, gsi_tgt;

  gsi_tgt = gsi_start_bb (new_bb);
  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      def_operand_p def_p;
      ssa_op_iter op_iter;
      int region;
      gimple stmt = gsi_stmt (gsi);
      gimple copy;

      if (gimple_code (stmt) == GIMPLE_LABEL)
	continue;

      /* Create a new copy of STMT and duplicate STMT's virtual
	 operands.  */
      copy = gimple_copy (stmt);
      gsi_insert_after (&gsi_tgt, copy, GSI_NEW_STMT);
      mark_symbols_for_renaming (copy);

      region = lookup_stmt_eh_region (stmt);
      if (region >= 0)
	add_stmt_to_eh_region (copy, region);
      gimple_duplicate_stmt_histograms (cfun, copy, cfun, stmt);

      /* Create new names for all the definitions created by COPY and
	 add replacement mappings for each new name.  */
      FOR_EACH_SSA_DEF_OPERAND (def_p, copy, op_iter, SSA_OP_DEF)
	{
	  tree old_name = DEF_FROM_PTR (def_p);
	  tree new_name = create_new_def_for (old_name, copy, def_p);
	  register_old_and_new_names (map, old_name, new_name);
	}
    }
}

/* Records in SESE_LIVEOUT_RENAMES the names that are live out of
   the SESE and that appear in the RENAME_MAP.  */

static void
register_sese_liveout_renames (sese region, htab_t rename_map)
{
  int i;

  for (i = 0; i < SESE_NUM_VER (region); i++)
    if (bitmap_bit_p (SESE_LIVEOUT (region), i)
	&& is_gimple_reg (ssa_name (i)))
      {
	tree old_name = ssa_name (i);
	tree new_name = get_new_name_from_old_name (rename_map, old_name);

	register_old_and_new_names (SESE_LIVEOUT_RENAMES (region),
				    old_name, new_name);
      }
}

/* Copies BB and includes in the copied BB all the statements that can
   be reached following the use-def chains from the memory accesses,
   and returns the next edge following this new block.  */
 
static edge
copy_bb_and_scalar_dependences (basic_block bb, sese region,
				edge next_e, htab_t map)
{
  basic_block new_bb = split_edge (next_e);

  next_e = single_succ_edge (new_bb);
  graphite_copy_stmts_from_block (bb, new_bb, map);
  remove_condition (new_bb);
  rename_variables (new_bb, map);
  remove_phi_nodes (new_bb);
  expand_scalar_variables (new_bb, region, map);
  register_sese_liveout_renames (region, map);

  return next_e;
}

/* Helper function for htab_traverse in insert_loop_close_phis.  */

static int
add_loop_exit_phis (void **slot, void *s)
{
  struct rename_map_elt *entry = (struct rename_map_elt *) *slot;
  tree new_name = entry->new_name;
  basic_block bb = (basic_block) s;
  gimple phi = create_phi_node (new_name, bb);
  tree res = create_new_def_for (gimple_phi_result (phi), phi,
				 gimple_phi_result_ptr (phi));

  add_phi_arg (phi, new_name, single_pred_edge (bb));

  entry->new_name = res;
  *slot = entry;
  return 1;
}

/* Iterate over the SESE_LIVEOUT_RENAMES (SESE) and get tuples of the
   form (OLD_NAME, NEW_NAME).  Insert in BB "RES = phi (NEW_NAME)",
   and finally register in SESE_LIVEOUT_RENAMES (region) the tuple
   (OLD_NAME, RES).  */

static void
insert_loop_close_phis (sese region, basic_block bb)
{
  update_ssa (TODO_update_ssa);
  htab_traverse (SESE_LIVEOUT_RENAMES (region), add_loop_exit_phis, bb);
  update_ssa (TODO_update_ssa);
}

/* Helper structure for htab_traverse in insert_guard_phis.  */

struct igp {
  basic_block bb;
  edge true_edge, false_edge;
  htab_t liveout_before_guard;
};

/* Return the default name that is before the guard.  */

static tree
default_liveout_before_guard (htab_t liveout_before_guard, tree old_name)
{
  tree res = get_new_name_from_old_name (liveout_before_guard, old_name);

  if (res == old_name)
    {
      if (is_gimple_reg (res))
	return fold_convert (TREE_TYPE (res), integer_zero_node);
      return gimple_default_def (cfun, res);
    }

  return res;
}

/* Helper function for htab_traverse in insert_guard_phis.  */

static int
add_guard_exit_phis (void **slot, void *s)
{
  struct rename_map_elt *entry = (struct rename_map_elt *) *slot;
  struct igp *i = (struct igp *) s;
  basic_block bb = i->bb;
  edge true_edge = i->true_edge;
  edge false_edge = i->false_edge;
  tree name1 = entry->new_name;
  tree name2 = default_liveout_before_guard (i->liveout_before_guard,
					     entry->old_name);
  gimple phi = create_phi_node (name1, bb);
  tree res = create_new_def_for (gimple_phi_result (phi), phi,
				 gimple_phi_result_ptr (phi));

  add_phi_arg (phi, name1, true_edge);
  add_phi_arg (phi, name2, false_edge);

  entry->new_name = res;
  *slot = entry;
  return 1;
}

/* Iterate over the SESE_LIVEOUT_RENAMES (SESE) and get tuples of the
   form (OLD_NAME, NAME1).  If there is a correspondent tuple of
   OLD_NAME in LIVEOUT_BEFORE_GUARD, i.e. (OLD_NAME, NAME2) then
   insert in BB
   
   | RES = phi (NAME1 (on TRUE_EDGE), NAME2 (on FALSE_EDGE))"

   if there is no tuple for OLD_NAME in LIVEOUT_BEFORE_GUARD, insert

   | RES = phi (NAME1 (on TRUE_EDGE),
   |            DEFAULT_DEFINITION of NAME1 (on FALSE_EDGE))".

   Finally register in SESE_LIVEOUT_RENAMES (region) the tuple
   (OLD_NAME, RES).  */

static void
insert_guard_phis (sese region, basic_block bb, edge true_edge,
		   edge false_edge, htab_t liveout_before_guard)
{
  struct igp i;
  i.bb = bb;
  i.true_edge = true_edge;
  i.false_edge = false_edge;
  i.liveout_before_guard = liveout_before_guard;

  update_ssa (TODO_update_ssa);
  htab_traverse (SESE_LIVEOUT_RENAMES (region), add_guard_exit_phis, &i);
  update_ssa (TODO_update_ssa);
}

/* Helper function for htab_traverse.  */

static int
copy_renames (void **slot, void *s)
{
  struct rename_map_elt *entry = (struct rename_map_elt *) *slot;
  htab_t res = (htab_t) s;
  tree old_name = entry->old_name;
  tree new_name = entry->new_name;
  struct rename_map_elt tmp;
  PTR *x;

  tmp.old_name = old_name;
  x = htab_find_slot (res, &tmp, INSERT);

  if (!*x)
    *x = new_rename_map_elt (old_name, new_name);

  return 1;
}

/* Translates a CLAST statement STMT to GCC representation in the
   context of a SESE.

   - NEXT_E is the edge where new generated code should be attached.
   - CONTEXT_LOOP is the loop in which the generated code will be placed
     (might be NULL).  
   - IVSTACK contains the surrounding loops around the statement to be
     translated.
*/

static edge
translate_clast (sese region, struct loop *context_loop,
		 struct clast_stmt *stmt, edge next_e, loop_iv_stack ivstack)
{
  if (!stmt)
    return next_e;

  if (CLAST_STMT_IS_A (stmt, stmt_root))
    return translate_clast (region, context_loop, stmt->next, next_e, ivstack);

  if (CLAST_STMT_IS_A (stmt, stmt_user))
    {
      gimple_bb_p gbb;
      htab_t map;
      CloogStatement *cs = ((struct clast_user_stmt *) stmt)->statement;
      poly_bb_p pbb = (poly_bb_p) cloog_statement_usr (cs);
      gbb = PBB_BLACK_BOX (pbb);

      if (GBB_BB (gbb) == ENTRY_BLOCK_PTR)
	return next_e;

      map = htab_create (10, rename_map_elt_info, eq_rename_map_elts, free);
      loop_iv_stack_patch_for_consts (ivstack, (struct clast_user_stmt *) stmt);
      build_iv_mapping (ivstack, map, pbb, region);
      next_e = copy_bb_and_scalar_dependences (GBB_BB (gbb), region,
					       next_e, map);
      htab_delete (map);
      loop_iv_stack_remove_constants (ivstack);
      update_ssa (TODO_update_ssa);
      recompute_all_dominators ();
      graphite_verify ();
      return translate_clast (region, context_loop, stmt->next, next_e, ivstack);
    }

  if (CLAST_STMT_IS_A (stmt, stmt_for))
    {
      struct loop *loop
	= graphite_create_new_loop (region, next_e, (struct clast_for *) stmt,
				    ivstack, context_loop ? context_loop
				    : get_loop (0));
      edge last_e = single_exit (loop);

      next_e = translate_clast (region, loop, ((struct clast_for *) stmt)->body,
				single_pred_edge (loop->latch), ivstack);
      redirect_edge_succ_nodup (next_e, loop->latch);

      set_immediate_dominator (CDI_DOMINATORS, next_e->dest, next_e->src);
      loop_iv_stack_pop (ivstack);
      last_e = single_succ_edge (split_edge (last_e));
      insert_loop_close_phis (region, last_e->src);

      recompute_all_dominators ();
      graphite_verify ();
      return translate_clast (region, context_loop, stmt->next, last_e, ivstack);
    }

  if (CLAST_STMT_IS_A (stmt, stmt_guard))
    {
      htab_t liveout_before_guard = htab_create (10, rename_map_elt_info,
						 eq_rename_map_elts, free);
      edge last_e = graphite_create_new_guard (region, next_e,
					       ((struct clast_guard *) stmt),
					       ivstack);
      edge true_e = get_true_edge_from_guard_bb (next_e->dest);
      edge false_e = get_false_edge_from_guard_bb (next_e->dest);
      edge exit_true_e = single_succ_edge (true_e->dest);
      edge exit_false_e = single_succ_edge (false_e->dest);

      htab_traverse (SESE_LIVEOUT_RENAMES (region), copy_renames,
		     liveout_before_guard);

      next_e = translate_clast (region, context_loop, 
				((struct clast_guard *) stmt)->then,
				true_e, ivstack);
      insert_guard_phis (region, last_e->src, exit_true_e, exit_false_e,
			 liveout_before_guard);
      htab_delete (liveout_before_guard);
      recompute_all_dominators ();
      graphite_verify ();

      return translate_clast (region, context_loop, stmt->next, last_e, ivstack);
    }

  if (CLAST_STMT_IS_A (stmt, stmt_block))
    {
      next_e = translate_clast (region, context_loop,
				((struct clast_block *) stmt)->body,
				next_e, ivstack);
      recompute_all_dominators ();
      graphite_verify ();
      return translate_clast (region, context_loop, stmt->next, next_e, ivstack);
    }

  gcc_unreachable ();
}

/* Free the SCATTERING domain list.  */

static void
free_scattering (CloogDomainList *scattering)
{
  while (scattering)
    {
      CloogDomain *dom = cloog_domain (scattering);
      CloogDomainList *next = cloog_next_domain (scattering);

      cloog_domain_free (dom);
      free (scattering);
      scattering = next;
    }
}

/* Build cloog program for SCoP.  */

static void
build_cloog_prog (scop_p scop, CloogProgram *prog)
{
  int i;
  int max_nb_loops = scop_max_loop_depth (scop);
  poly_bb_p pbb;
  CloogLoop *loop_list = NULL;
  CloogBlockList *block_list = NULL;
  CloogDomainList *scattering = NULL;
  int nbs = 2 * max_nb_loops + 1;
  int *scaldims = (int *) xmalloc (nbs * (sizeof (int)));

  build_scop_context (scop, prog);
  cloog_program_set_nb_scattdims (prog, nbs);
  initialize_cloog_names (scop, prog);

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    {
      /* Build new block.  */
      CloogStatement *stmt = cloog_statement_alloc
	(GBB_BB (PBB_BLACK_BOX (pbb))->index);
      CloogBlock *block = cloog_block_alloc (stmt, 0, NULL,
					     nb_loops_around_pbb (pbb));

      cloog_statement_set_usr (stmt, pbb);

      /* Build loop list.  */
      {
        CloogLoop *new_loop_list = cloog_loop_malloc ();
        cloog_loop_set_next (new_loop_list, loop_list);
        cloog_loop_set_domain
	  (new_loop_list,
	   new_Cloog_Domain_from_ppl_Polyhedron (PBB_DOMAIN (pbb)));
        cloog_loop_set_block (new_loop_list, block);
        loop_list = new_loop_list;
      }

      /* Build block list.  */
      {
        CloogBlockList *new_block_list = cloog_block_list_malloc ();

        cloog_block_list_set_next (new_block_list, block_list);
        cloog_block_list_set_block (new_block_list, block);
        block_list = new_block_list;
      }

      /* Build scattering list.  */
      {
        /* XXX: Replace with cloog_domain_list_alloc(), when available.  */
        CloogDomainList *new_scattering
	  = (CloogDomainList *) xmalloc (sizeof (CloogDomainList));
        ppl_Polyhedron_t scat = schedule_to_scattering (pbb, nbs);
	CloogDomain *dom = new_Cloog_Domain_from_ppl_Polyhedron (scat);

        cloog_set_next_domain (new_scattering, scattering);
	
        cloog_set_domain (new_scattering, dom);
        scattering = new_scattering;
        ppl_delete_Polyhedron (scat);
      }
    }

  cloog_program_set_loop (prog, loop_list);
  cloog_program_set_blocklist (prog, block_list);

  for (i = 0; i < nbs; i++)
    scaldims[i] = 0 ;

  cloog_program_set_scaldims (prog, scaldims);

  /* Extract scalar dimensions to simplify the code generation problem.  */
  cloog_program_extract_scalars (prog, scattering);

  /* Apply scattering.  */
  cloog_program_scatter (prog, scattering);
  free_scattering (scattering);

  /* Iterators corresponding to scalar dimensions have to be extracted.  */
  cloog_names_scalarize (cloog_program_names (prog), nbs,
			 cloog_program_scaldims (prog));
  
  /* Free blocklist.  */
  {
    CloogBlockList *next = cloog_program_blocklist (prog);
	
    while (next)
      {
        CloogBlockList *toDelete = next;
        next = cloog_block_list_next (next);
        cloog_block_list_set_next (toDelete, NULL);
        cloog_block_list_set_block (toDelete, NULL);
        cloog_block_list_free (toDelete);
      }
    cloog_program_set_blocklist (prog, NULL);
  }
}

/* Return the options that will be used in GLOOG.  */

static CloogOptions *
set_cloog_options (void)
{
  CloogOptions *options = cloog_options_malloc ();

  /* Change cloog output language to C.  If we do use FORTRAN instead, cloog
     will stop e.g. with "ERROR: unbounded loops not allowed in FORTRAN.", if
     we pass an incomplete program to cloog.  */
  options->language = LANGUAGE_C;

  /* Enable complex equality spreading: removes dummy statements
     (assignments) in the generated code which repeats the
     substitution equations for statements.  This is useless for
     GLooG.  */
  options->esp = 1;

  /* Enable C pretty-printing mode: normalizes the substitution
     equations for statements.  */
  options->cpp = 1;

  /* Allow cloog to build strides with a stride width different to one.
     This example has stride = 4:

     for (i = 0; i < 20; i += 4)
       A  */
  options->strides = 1;

  /* Disable optimizations and make cloog generate source code closer to the
     input.  This is useful for debugging,  but later we want the optimized
     code.

     XXX: We can not disable optimizations, as loop blocking is not working
     without them.  */
  if (0)
    {
      options->f = -1;
      options->l = INT_MAX;
    }

  return options;
}

/* Prints STMT to STDERR.  */

void
debug_clast_stmt (struct clast_stmt *stmt)
{
  CloogOptions *options = set_cloog_options ();

  pprint (stderr, stmt, 0, options);
  cloog_options_free (options);
}

/* Data structure for CLooG program representation.  */

typedef struct cloog_prog_clast {
  CloogProgram *prog;
  struct clast_stmt *stmt;
} cloog_prog_clast;

/* Translate SCOP to a CLooG program and clast.  These two
   representations should be freed together: a clast cannot be used
   without a program.  */

static cloog_prog_clast
scop_to_clast (scop_p scop)
{
  CloogOptions *options = set_cloog_options ();
  cloog_prog_clast pc;

  /* Connect new cloog prog generation to graphite.  */
  pc.prog = cloog_program_malloc ();
  build_cloog_prog (scop, pc.prog);
  pc.prog = cloog_program_generate (pc.prog, options);
  pc.stmt = cloog_clast_create (pc.prog, options);

  cloog_options_free (options);
  return pc;
}

/* Prints to FILE the code generated by CLooG for SCOP.  */

void
print_generated_program (FILE *file, scop_p scop)
{
  CloogOptions *options = set_cloog_options ();
  cloog_prog_clast pc = scop_to_clast (scop);

  fprintf (file, "       (prog: \n");
  cloog_program_print (file, pc.prog);
  fprintf (file, "       )\n");

  fprintf (file, "       (clast: \n");
  pprint (file, pc.stmt, 0, options);
  fprintf (file, "       )\n");
  
  cloog_options_free (options);
  cloog_clast_free (pc.stmt);
  cloog_program_free (pc.prog);
}

/* Prints to STDERR the code generated by CLooG for SCOP.  */

void
debug_generated_program (scop_p scop)
{
  print_generated_program (stderr, scop);
}

typedef struct ifsese {
  sese region;
  sese true_region;
  sese false_region;
} *ifsese;

static inline edge
if_region_entry (ifsese if_region)
{
  return SESE_ENTRY (if_region->region);
}

static inline edge
if_region_exit (ifsese if_region)
{
  return SESE_EXIT (if_region->region);
}

static inline basic_block
if_region_get_condition_block (ifsese if_region)
{
  return if_region_entry (if_region)->dest;
}

static inline void
if_region_set_false_region (ifsese if_region, sese region)
{
  basic_block condition = if_region_get_condition_block (if_region);
  edge false_edge = get_false_edge_from_guard_bb (condition);
  basic_block dummy = false_edge->dest;
  edge entry_region = SESE_ENTRY (region);
  edge exit_region = SESE_EXIT (region);
  basic_block before_region = entry_region->src;
  basic_block last_in_region = exit_region->src;
  void **slot = htab_find_slot_with_hash (current_loops->exits, exit_region,
					  htab_hash_pointer (exit_region),
					  NO_INSERT);

  entry_region->flags = false_edge->flags;
  false_edge->flags = exit_region->flags;

  redirect_edge_pred (entry_region, condition);
  redirect_edge_pred (exit_region, before_region);
  redirect_edge_pred (false_edge, last_in_region);
  redirect_edge_succ (false_edge, single_succ (dummy));
  delete_basic_block (dummy);

  exit_region->flags = EDGE_FALLTHRU;
  recompute_all_dominators ();

  SESE_EXIT (region) = false_edge;
  if_region->false_region = region;

  if (slot)
    {
      struct loop_exit *loop_exit = GGC_CNEW (struct loop_exit);

      memcpy (loop_exit, *((struct loop_exit **) slot), sizeof (struct loop_exit));
      htab_clear_slot (current_loops->exits, slot);

      slot = htab_find_slot_with_hash (current_loops->exits, false_edge,
				       htab_hash_pointer (false_edge),
				       INSERT);
      loop_exit->e = false_edge;
      *slot = loop_exit;
      false_edge->src->loop_father->exits->next = loop_exit;
    }
}

static ifsese
create_if_region_on_edge (edge entry, tree condition)
{
  edge e;
  edge_iterator ei;
  sese sese_region = GGC_NEW (struct sese);
  sese true_region = GGC_NEW (struct sese);
  sese false_region = GGC_NEW (struct sese);
  ifsese if_region = GGC_NEW (struct ifsese);
  edge exit = create_empty_if_region_on_edge (entry, condition);

  if_region->region = sese_region;
  if_region->region->entry = entry;
  if_region->region->exit = exit;

  FOR_EACH_EDGE (e, ei, entry->dest->succs)
    {
      if (e->flags & EDGE_TRUE_VALUE)
	{
	  true_region->entry = e;
	  true_region->exit = single_succ_edge (e->dest);
	  if_region->true_region = true_region;
	}
      else if (e->flags & EDGE_FALSE_VALUE)
	{
	  false_region->entry = e;
	  false_region->exit = single_succ_edge (e->dest);
	  if_region->false_region = false_region;
	}
    }

  return if_region;
}

/* Moves REGION in a condition expression:
   | if (1)
   |   ;
   | else
   |   REGION;
*/

static ifsese
move_sese_in_condition (sese region)
{
  basic_block pred_block = split_edge (SESE_ENTRY (region));
  ifsese if_region = NULL;

  SESE_ENTRY (region) = single_succ_edge (pred_block);
  if_region = create_if_region_on_edge (single_pred_edge (pred_block), integer_one_node);
  if_region_set_false_region (if_region, region);

  return if_region;
}

/* Add exit phis for USE on EXIT.  */

static void
sese_add_exit_phis_edge (basic_block exit, tree use, edge false_e, edge true_e)
{
  gimple phi = create_phi_node (use, exit);

  create_new_def_for (gimple_phi_result (phi), phi,
		      gimple_phi_result_ptr (phi));
  add_phi_arg (phi, use, false_e);
  add_phi_arg (phi, use, true_e);
}

/* Add phi nodes for VAR that is used in LIVEIN.  Phi nodes are
   inserted in block BB.  */

static void
sese_add_exit_phis_var (basic_block bb, tree var, bitmap livein,
			edge false_e, edge true_e)
{
  bitmap def;
  basic_block def_bb = gimple_bb (SSA_NAME_DEF_STMT (var));

  if (is_gimple_reg (var))
    bitmap_clear_bit (livein, def_bb->index);
  else
    bitmap_set_bit (livein, def_bb->index);

  def = BITMAP_ALLOC (NULL);
  bitmap_set_bit (def, def_bb->index);
  compute_global_livein (livein, def);
  BITMAP_FREE (def);

  sese_add_exit_phis_edge (bb, var, false_e, true_e);
}

/* Insert in the block BB phi nodes for variables defined in REGION
   and used outside the REGION.  The code generation moves REGION in
   the else clause of an "if (1)" and generates code in the then
   clause that is at this point empty:

   | if (1)
   |   empty;
   | else
   |   REGION;
*/

static void
sese_insert_phis_for_liveouts (sese region, basic_block bb,
			       edge false_e, edge true_e)
{
  unsigned i;
  bitmap_iterator bi;

  update_ssa (TODO_update_ssa);

  EXECUTE_IF_SET_IN_BITMAP (SESE_LIVEOUT (region), 0, i, bi)
    sese_add_exit_phis_var (bb, ssa_name (i), SESE_LIVEIN_VER (region, i),
			    false_e, true_e);

  update_ssa (TODO_update_ssa);
}

/* Get the definition of NAME before the SESE.  Keep track of the
   basic blocks that have been VISITED in a bitmap.  */

static tree
get_vdef_before_sese (sese region, tree name, sbitmap visited)
{
  unsigned i;
  gimple def_stmt = SSA_NAME_DEF_STMT (name);
  basic_block def_bb = gimple_bb (def_stmt);

  if (!def_bb || !bb_in_sese_p (def_bb, region))
    return name;

  if (TEST_BIT (visited, def_bb->index))
    return NULL_TREE;

  SET_BIT (visited, def_bb->index);

  switch (gimple_code (def_stmt))
    {
    case GIMPLE_PHI:
      for (i = 0; i < gimple_phi_num_args (def_stmt); i++)
	{
	  tree arg = gimple_phi_arg_def (def_stmt, i);
	  tree res = get_vdef_before_sese (region, arg, visited);
	  if (res)
	    return res;
	}
      return NULL_TREE;

    default:
      return NULL_TREE;
    }
}

/* Adjust a virtual phi node PHI that is placed at the end of the
   generated code for SCOP:

   | if (1)
   |   generated code from REGION;
   | else
   |   REGION;

   The FALSE_E edge comes from the original code, TRUE_E edge comes
   from the code generated for the SCOP.  */

static void
sese_adjust_vphi (sese region, gimple phi, edge true_e)
{
  unsigned i;

  gcc_assert (gimple_phi_num_args (phi) == 2);

  for (i = 0; i < gimple_phi_num_args (phi); i++)
    if (gimple_phi_arg_edge (phi, i) == true_e)
      {
	tree true_arg, false_arg, before_scop_arg;
	sbitmap visited;

	true_arg = gimple_phi_arg_def (phi, i);
	if (!SSA_NAME_IS_DEFAULT_DEF (true_arg))
	  return;

	false_arg = gimple_phi_arg_def (phi, i == 0 ? 1 : 0);
	if (SSA_NAME_IS_DEFAULT_DEF (false_arg))
	  return;

	visited = sbitmap_alloc (last_basic_block);
	sbitmap_zero (visited);
	before_scop_arg = get_vdef_before_sese (region, false_arg, visited);
	gcc_assert (before_scop_arg != NULL_TREE);
	SET_PHI_ARG_DEF (phi, i, before_scop_arg);
	sbitmap_free (visited);
      }
}

/* Adjusts the phi nodes in the block BB for variables defined in
   SCOP_REGION and used outside the SCOP_REGION.  The code generation
   moves SCOP_REGION in the else clause of an "if (1)" and generates
   code in the then clause:

   | if (1)
   |   generated code from REGION;
   | else
   |   REGION;

   To adjust the phi nodes after the condition, SCOP_LIVEOUT_RENAMES
   hash table is used: this stores for a name that is part of the
   LIVEOUT of SCOP_REGION its new name in the generated code.  */

static void
sese_adjust_phis_for_liveouts (sese region, basic_block bb, edge false_e,
			       edge true_e)
{
  gimple_stmt_iterator si;

  for (si = gsi_start_phis (bb); !gsi_end_p (si); gsi_next (&si))
    {
      unsigned i;
      unsigned false_i = 0;
      gimple phi = gsi_stmt (si);

      if (!is_gimple_reg (PHI_RESULT (phi)))
	{
	  sese_adjust_vphi (region, phi, true_e);
	  continue;
	}

      for (i = 0; i < gimple_phi_num_args (phi); i++)
	if (gimple_phi_arg_edge (phi, i) == false_e)
	  {
	    false_i = i;
	    break;
	  }

      for (i = 0; i < gimple_phi_num_args (phi); i++)
	if (gimple_phi_arg_edge (phi, i) == true_e)
	  {
	    tree old_name = gimple_phi_arg_def (phi, false_i);
	    tree new_name = get_new_name_from_old_name
	      (SESE_LIVEOUT_RENAMES (region), old_name);

	    gcc_assert (old_name != new_name);
	    SET_PHI_ARG_DEF (phi, i, new_name);
	  }
    }
}

/* Returns the first cloog name used in EXPR.  */

static const char *
find_cloog_iv_in_expr (struct clast_expr *expr)
{
  struct clast_term *term = (struct clast_term *) expr;

  if (expr->type == expr_term
      && !term->var)
    return NULL;

  if (expr->type == expr_term)
    return term->var;

  if (expr->type == expr_red)
    {
      int i;
      struct clast_reduction *red = (struct clast_reduction *) expr;

      for (i = 0; i < red->n; i++)
	{
	  const char *res = find_cloog_iv_in_expr ((red)->elts[i]);

	  if (res)
	    return res;
	}
    }

  return NULL;
}

/* Build for a clast_user_stmt USER_STMT a map between the CLAST
   induction variables and the corresponding GCC old induction
   variables.  This information is stored on each GRAPHITE_BB.  */

static void
compute_cloog_iv_types_1 (poly_bb_p pbb, struct clast_user_stmt *user_stmt)
{
  gimple_bb_p gbb = PBB_BLACK_BOX (pbb);
  struct clast_stmt *t;
  int index = 0;

  for (t = user_stmt->substitutions; t; t = t->next, index++)
    {
      PTR *slot;
      struct ivtype_map_elt tmp;
      struct clast_expr *expr = (struct clast_expr *) 
	((struct clast_assignment *)t)->RHS;

      /* Create an entry (clast_var, type).  */
      tmp.cloog_iv = find_cloog_iv_in_expr (expr);
      if (!tmp.cloog_iv)
	continue;

      slot = htab_find_slot (GBB_CLOOG_IV_TYPES (gbb), &tmp, INSERT);

      if (!*slot)
	{
	  loop_p loop = pbb_loop_at_index (pbb, index);
	  tree oldiv = oldiv_for_loop (SCOP_REGION (PBB_SCOP (pbb)), loop);
	  tree type = oldiv ? TREE_TYPE (oldiv) : integer_type_node;
	  *slot = new_ivtype_map_elt (tmp.cloog_iv, type);
	}
    }
}

/* Walk the CLAST tree starting from STMT and build for each
   clast_user_stmt a map between the CLAST induction variables and the
   corresponding GCC old induction variables.  This information is
   stored on each GRAPHITE_BB.  */

static void
compute_cloog_iv_types (struct clast_stmt *stmt)
{
  if (!stmt)
    return;

  if (CLAST_STMT_IS_A (stmt, stmt_root))
    goto next;

  if (CLAST_STMT_IS_A (stmt, stmt_user))
    {
      CloogStatement *cs = ((struct clast_user_stmt *) stmt)->statement;
      poly_bb_p pbb = (poly_bb_p) cloog_statement_usr (cs);
      gimple_bb_p gbb = PBB_BLACK_BOX (pbb);
      GBB_CLOOG_IV_TYPES (gbb) = htab_create (10, ivtype_map_elt_info,
					      eq_ivtype_map_elts, free);
      compute_cloog_iv_types_1 (pbb, (struct clast_user_stmt *) stmt);
      goto next;
    }

  if (CLAST_STMT_IS_A (stmt, stmt_for))
    {
      struct clast_stmt *s = ((struct clast_for *) stmt)->body;
      compute_cloog_iv_types (s);
      goto next;
    }

  if (CLAST_STMT_IS_A (stmt, stmt_guard))
    {
      struct clast_stmt *s = ((struct clast_guard *) stmt)->then;
      compute_cloog_iv_types (s);
      goto next;
    }

  if (CLAST_STMT_IS_A (stmt, stmt_block))
    {
      struct clast_stmt *s = ((struct clast_block *) stmt)->body;
      compute_cloog_iv_types (s);
      goto next;
    }

  gcc_unreachable ();

 next:
  compute_cloog_iv_types (stmt->next);
}

/* GIMPLE Loop Generator: generates loops from STMT in GIMPLE form for
   the given SCOP.  Return true if code generation succeeded.  */

static bool
gloog (scop_p scop)
{
  edge new_scop_exit_edge = NULL;
  VEC (iv_stack_entry_p, heap) *ivstack = VEC_alloc (iv_stack_entry_p, heap,
						     10);
  loop_p context_loop;
  sese region = SCOP_REGION (scop);
  ifsese if_region = NULL;

  cloog_prog_clast pc = scop_to_clast (scop);

  recompute_all_dominators ();
  graphite_verify ();

  SESE_LIVEOUT_RENAMES (region) = htab_create
    (10, rename_map_elt_info, eq_rename_map_elts, free);

  if_region = move_sese_in_condition (region);
  sese_build_livein_liveouts (region);
  sese_insert_phis_for_liveouts (region,
				 if_region->region->exit->src,
				 if_region->false_region->exit,
				 if_region->true_region->exit);
  recompute_all_dominators ();
  graphite_verify ();
  context_loop = SESE_ENTRY (region)->src->loop_father;
  compute_cloog_iv_types (pc.stmt);

  new_scop_exit_edge = translate_clast (region, context_loop, pc.stmt,
					if_region->true_region->entry,
					&ivstack);
  free_loop_iv_stack (&ivstack);
  cloog_clast_free (pc.stmt);
  cloog_program_free (pc.prog);

  graphite_verify ();
  sese_adjust_phis_for_liveouts (region,
				 if_region->region->exit->src,
				 if_region->false_region->exit,
				 if_region->true_region->exit);

  recompute_all_dominators ();
  graphite_verify ();
  return true;
}

/* Perform a set of linear transforms on the loops of the current
   function.  */

void
graphite_transform_loops (void)
{
  int i;
  scop_p scop;
  bool transform_done = false;
  VEC (scop_p, heap) *scops = NULL;

  if (number_of_loops () <= 1)
    return;

  recompute_all_dominators ();

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "Graphite loop transformations \n");

  initialize_original_copy_tables ();
  cloog_initialize ();
  build_scops (&scops);

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "\nnumber of SCoPs: %d\n",
	     VEC_length (scop_p, scops));

  for (i = 0; VEC_iterate (scop_p, scops, i, scop); i++)
    {
      build_scop_bbs (scop);
      if (!build_sese_loop_nests (SCOP_REGION (scop)))
	continue;

      if (scop_contains_non_iv_scalar_phi_nodes (scop))
	continue;

      build_bb_loops (scop);
      build_sese_conditions (SCOP_REGION (scop));
      find_scop_parameters (scop);
      build_scop_iteration_domain (scop);
      add_conditions_to_constraints (scop);
      build_scop_canonical_schedules (scop);
      build_scop_data_accesses (scop);
      
      if (graphite_apply_transformations (scop))
	transform_done = gloog (scop);
#ifdef ENABLE_CHECKING
      else
	{
	  cloog_prog_clast pc = scop_to_clast (scop);
	  cloog_clast_free (pc.stmt);
	  cloog_program_free (pc.prog);
	}
#endif
    }

  /* Cleanup.  */
  if (transform_done)
    cleanup_tree_cfg ();

  free_scops (scops);
  cloog_finalize ();
  free_original_copy_tables ();
}

#else /* If Cloog is not available: #ifndef HAVE_cloog.  */

void
graphite_transform_loops (void)
{
  sorry ("Graphite loop optimizations cannot be used");
}

#endif
