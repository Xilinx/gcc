/* Loop streamization.
   Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Antoniu Pop  <antoniu.pop@gmail.com>
   and Sebastian Pop <sebastian.pop@amd.com>.

This file is part of GCC.
   
GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.
   
GCC is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.
   
You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* This pass performs loop streamization: for example, the loop

   |DO I = 2, N
   |    A(I) = B(I) + C
   |    D(I) = A(I) * E
   |ENDDO

   is distributed across two threads executing in parallel:

   thread_1:
   |DOALL I = 2, N
   |   A(I) = B(I) + C
   |   push (stream, A(I))
   |ENDDO

   thread_2:
   |DOALL I = 2, N
   |  pop (stream, x)
   |  D(I) = x * E
   |ENDDO

*/

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "target.h"

#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "expr.h"
#include "optabs.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "lambda.h"
#include "langhooks.h"
#include "tree-vectorizer.h"
#include "cfgloop.h"

#if 0
/* Check that PRODUCER and CONSUMERS are in LOOP.  */

static bool
check_producer_consumers (tree producer, VEC (tree, heap) *consumers,
			  struct loop *loop)
{
  unsigned i;
  tree consumer;

  if (bb_for_stmt (producer)->loop_father != loop)
    return false;

  for (i = 0; VEC_iterate (tree, consumers, i, consumer); i++)
    if (bb_for_stmt (consumer)->loop_father != loop)
      return false;

  return true;
}

/* Scalar expansion.  */

static void
scalar_expansion (tree variable, tree producer, VEC (tree, heap) *consumers,
		  struct loop *loop)
{
  unsigned i;
  bool insert_after;
  ssa_op_iter iter;
  use_operand_p use_p;
  def_operand_p def_p;
  block_stmt_iterator bsi;
  tree ivvar, base, elt_type, iv, array_type, niter, index_type, array;
  tree consumer;

  edge exit = single_exit (loop);

  gcc_assert (exit);
  gcc_assert (check_producer_consumers (producer, consumers, loop));

  niter = find_loop_niter (loop, &exit);
  gcc_assert (niter != chrec_dont_know);

  index_type = build_index_type (niter);
  elt_type = TREE_TYPE (variable);
  array_type = build_array_type (elt_type, index_type);

  base = create_tmp_var (array_type, "Atmp");
  ivvar = create_tmp_var_raw (elt_type, "scivtmp");
  add_referenced_var (base);
  add_referenced_var (ivvar);

  standard_iv_increment_position (loop, &bsi, &insert_after);
  create_iv (build_int_cst (elt_type, 0),
	     build_int_cst (elt_type, 1),
	     ivvar, loop, &bsi, insert_after, &iv,
	     NULL);

  array = build4 (ARRAY_REF, elt_type, base, iv, NULL_TREE, NULL_TREE);

  FOR_EACH_SSA_DEF_OPERAND (def_p, producer, iter, SSA_OP_DEF)
    {
      SET_DEF (def_p, array);
    }

  for (i = 0; VEC_iterate (tree, consumers, i, consumer); i++)
    {
      FOR_EACH_SSA_USE_OPERAND (use_p, consumer, iter, SSA_OP_USE)
	{
	  SET_USE (use_p, array);
	}
    }
}
#endif

typedef struct stream
{
  int producer;
  int consumer;
  tree channel_name;
} *stream;

static hashval_t
stream_hash (const PTR p)
{
  return (hashval_t) (((const struct stream *) p)->producer);
}

static int
stream_eq (const PTR p1, const PTR p2)
{
  int producer1 = ((const struct stream *) p1)->producer;
  int producer2 = ((const struct stream *) p2)->producer;

  return producer1 == producer2;
}

/* Generate the alignment loop for the producer side.  For example, in
   the loop:

   | for (i=1; i < 10; i++)
   |  {
   |    a[i] += c[i];
   |    b[i] = a[i - k] + 1;
   |  }

   we generate 

   | for (i = 0; i < k; i++)
   |   push (stream, a[i])
   | for (i=1; i < 10; i++)
   |  {
   |    a[i] += c[i];
   |    push (stream, a[i]);
   |  }
   | eos (stream);
   |
   | for (i = k; i < 0; i++)
   |   pop (stream);
   | for (i=1; i < 10; i++)
   |  {
   |    b[i] = head (stream) + 1;
   |    pop (stream);
   |  }
   | close (stream);
 */

static void
generate_stream_alignment (basic_block entry_bb, basic_block exit_bb,
			   struct graph *rdg, int index, htab_t channels,
			   int is_producer)
{
  struct stream stream;
  struct stream *slot;
  struct data_reference *dr;
  ddr_p ddr;
  lambda_vector dist;
  int k;
  tree count, call, address;
  block_stmt_iterator bsi;

  if (is_producer)
    {
      struct graph_edge *e = rdg->vertices[index].succ;

      while (!(ddr = RDGE_RELATION (e)))
	e = e->succ_next;

      /* The channel should always be for a flow dependence.  Anti
	 dependences should not be split in different tasks.  */
      gcc_assert (RDGE_TYPE (e) == flow_dd);

      /* The producer side.  */
      gcc_assert (!DR_IS_READ (DDR_A (ddr)));

      stream.producer = index;
      dr = DDR_A (ddr);
    }
  else
    {
      struct graph_edge *e = rdg->vertices[index].pred;

      while (!(ddr = RDGE_RELATION (e)))
	e = e->pred_next;

      /* The channel should always be for a flow dependence.  Anti
	 dependences should not be split in different tasks.  */
      gcc_assert (RDGE_TYPE (e) == flow_dd);

      /* The consumer side.  */
      gcc_assert (DR_IS_READ (DDR_B (ddr)));

      stream.producer = e->src;
      dr = DDR_B (ddr);
    }
  slot = (struct stream *) htab_find (channels, &stream);
  address = DR_BASE_ADDRESS (dr);

  /* FIXME: not yet implemented for multiple dist vectors.  */
  gcc_assert (DDR_NUM_DIST_VECTS (ddr) == 1);
  /* FIXME: not yet implemented for nested loops.  */
  gcc_assert (DDR_NB_LOOPS (ddr) == 1);
  /* FIXME: not yet implemented for non unit stride loops.  */
  /* gcc_assert (integer_onep (DR_STEP (dr))); */

  dist = DDR_DIST_VECT (ddr, 0);
  k = dist[0];
  count = build_int_cst (integer_type_node, k);

  if (is_producer) 
    {
      tree stmt = RDG_STMT (rdg, index);
      tree ref = GIMPLE_STMT_OPERAND (stmt, 0);
      tree size = TYPE_SIZE_UNIT (TREE_TYPE (ref));
      tree expr = fold_build2 (MINUS_EXPR, sizetype, DR_INIT (dr),
			       fold_build2 (MULT_EXPR, integer_type_node,
					    count, size));
      tree base_ptr = fold_build2 (POINTER_PLUS_EXPR, TREE_TYPE (address),
				   address, expr);

      call = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_ALIGN_PUSH],
			      3, slot->channel_name, base_ptr, count);
      bsi = bsi_start (entry_bb);
      bsi_insert_after (&bsi, call, BSI_NEW_STMT);
      update_stmt(call);

      call = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_SET_EOS],
			      1, slot->channel_name);
      bsi = bsi_start (exit_bb);
      bsi_insert_after (&bsi, call, BSI_NEW_STMT);
      update_stmt(call);
    }
  else
    {
      /* We could just not bother realigning the stream if there are unused
	 elements left in the channel, and just close the stream.  */

      call = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_ALIGN_POP],
			      2, slot->channel_name, count);
      bsi = bsi_start (exit_bb);
      bsi_insert_after (&bsi, call, BSI_NEW_STMT);
      update_stmt(call);

      call = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_DESTROY],
			      1, slot->channel_name);
      bsi_insert_after (&bsi, call, BSI_NEW_STMT);
      update_stmt(call);
    }
}

/* Create code for the producer side of the stream, and place it at
   BSI.  Update BSI to point after these statements.  X is the RDG
   node that writes to memory.  CONSUMERS and CHANNELS are initialized
   in this function with the RDG nodes that consume the information
   and with the map (consumer, open channel).  */

static void
generate_channel (struct graph *rdg, int x, htab_t channels)
{
  tree stmt = RDG_STMT (rdg, x);
  tree ref = GIMPLE_STMT_OPERAND (stmt, 0);
  tree size = TYPE_SIZE_UNIT (TREE_TYPE (ref));
  tree count = build_int_cst (unsigned_type_node, 16);
  tree call = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_CREATE], 2,
			       size, count);
  tree decl_stream = create_tmp_var (ptr_type_node, "stream");
  stream stream = XNEW (struct stream);
  struct stream **slot;
  tree modify, channel_name;
  block_stmt_iterator bsi;

  add_referenced_var (decl_stream);
  modify = build_gimple_modify_stmt (decl_stream, call);
  channel_name = make_ssa_name (decl_stream, modify);
  GIMPLE_STMT_OPERAND (modify, 0) = channel_name;

  stream->channel_name = channel_name;
  stream->producer = x;

  slot = (struct stream **) htab_find_slot (channels, stream, INSERT);
  gcc_assert (!*slot);
  *slot = stream;

  bsi = bsi_start (ENTRY_BLOCK_PTR->next_bb);
  bsi_insert_after (&bsi, modify, BSI_NEW_STMT);
  update_stmt (modify);

  if (0)
    fprintf (stdout, "generated channel\n");
}

/* Create code for the producer side of the stream, and place it at
   BSI.  Update BSI to point after these statements.  X is the RDG
   node that writes to memory.  CONSUMERS and CHANNELS are initialized
   in this function with the RDG nodes that consume the information
   and with the map (consumer, open channel).  */

static void
generate_push (block_stmt_iterator *bsi, int producer, htab_t channels)
{
  tree push;
  tree ptr = TREE_OPERAND (GIMPLE_STMT_OPERAND (bsi_stmt (*bsi), 0), 0);
  struct stream stream;
  struct stream **slot;

  stream.producer = producer;
  slot = (struct stream **) htab_find_slot (channels, &stream, NO_INSERT);
  stream = **slot;

  push = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_PUSH], 2,
			  stream.channel_name, ptr);

  bsi_insert_after (bsi, push, BSI_NEW_STMT);
  update_stmt (push);
}


/* */

static void
generate_pop (block_stmt_iterator *bsi, struct graph *rdg, int consumer,
	      htab_t channels)
{
  tree pop;
  struct stream stream;
  struct stream *slot;
  tree head, head_call, value_head, head_name;
  tree decl_head = create_tmp_var (ptr_type_node, "head");
  struct graph_edge *e = rdg->vertices[consumer].pred;
  int producer = e->src;

  while (!RDG_MEM_WRITE_STMT (rdg, producer))
    {
      e = e->pred_next;
      gcc_assert (e);
      producer = e->src;
    }

  stream.producer = producer;
  slot = (struct stream *) htab_find (channels, &stream);

  head_call = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_HEAD], 1,
			       (slot)->channel_name);

  add_referenced_var (decl_head);
  head = build_gimple_modify_stmt (decl_head, head_call);
  head_name = make_ssa_name (decl_head, head);
  GIMPLE_STMT_OPERAND (head, 0) = head_name;

  value_head = build1 (INDIRECT_REF,
		       TREE_TYPE (GIMPLE_STMT_OPERAND (bsi_stmt (*bsi), 1)),
		       head_name);
  pop = build_call_expr (built_in_decls[BUILT_IN_GOMP_STREAM_POP], 1,
			 (slot)->channel_name);

  /* Replace in the consumer the memory read with a read in the
     stream.  */
  GIMPLE_STMT_OPERAND (bsi_stmt (*bsi), 1) = value_head;

  update_stmt (bsi_stmt (*bsi));

  /* Insert the read in the stream before the consumer.  */
  bsi_insert_before (bsi, head, BSI_SAME_STMT);

  /* Insert the pop after the consumer has used the information.  */
  bsi_insert_after (bsi, pop, BSI_NEW_STMT);

  update_stmt (head);
  update_stmt (pop);
}

typedef struct section_region
{
  basic_block entry;
  basic_block exit;
} *section_region;

DEF_VEC_P (section_region);
DEF_VEC_ALLOC_P (section_region, heap);

/* Generate loops with stream code for channels between partitions.
   The loop is copied when COPY_P is true.  All the statements not
   flagged in the PARTITION bitmap are removed from the loop or from
   its copy.  The statements are indexed in sequence inside a basic
   block, and the basic blocks of a loop are taken in dom order.  */

static struct loop *
generate_streamable_loops_for_partition (struct loop *loop, struct graph *rdg,
					 bitmap partition,
					 bitmap producers, bitmap consumers,
					 htab_t channels,
					 bool copy_p,
					 VEC (section_region, heap) **sections)
{
  unsigned i, x;
  block_stmt_iterator bsi;
  basic_block *bbs;
  section_region s = XNEW (struct section_region);

  if (copy_p)
    {
      loop = copy_loop_before (loop);
      gcc_assert (loop);

      create_preheader (loop, CP_SIMPLE_PREHEADERS);
      create_bb_after_loop (loop);
    }


  s->entry = split_edge (loop_preheader_edge (loop));
  s->exit  = split_edge (single_dom_exit (loop));
  VEC_safe_push (section_region, heap, *sections, s);

  /* Remove stmts not in the PARTITION bitmap.  The order in which we
     visit the phi nodes and the statements is exactly as in
     stmts_from_loop.  */
  bbs = get_loop_body_in_dom_order (loop);

  for (x = 0, i = 0; i < loop->num_nodes; i++)
    {
      basic_block bb = bbs[i];
      tree phi, prev = NULL_TREE, next;

      for (phi = phi_nodes (bb); phi;)
	if (!bitmap_bit_p (partition, x++))
	  {
	    next = PHI_CHAIN (phi);
	    remove_phi_node (phi, prev, true);
	    phi = next;
	  }
	else
	  {
	    prev = phi;
	    phi = PHI_CHAIN (phi);
	  }

      for (bsi = bsi_start (bb); !bsi_end_p (bsi);)
	if (TREE_CODE (bsi_stmt (bsi)) != LABEL_EXPR
	    && !bitmap_bit_p (partition, x++))
	  bsi_remove (&bsi, false);
	else
	  {
	    bsi_next (&bsi);

	    if (bitmap_bit_p (producers, x))
	      {
		generate_channel (rdg, x, channels);
		generate_stream_alignment (s->entry, s->exit, rdg, x, 
					   channels, true);
		generate_push (&bsi, x, channels);
	      }
	    else if (bitmap_bit_p (consumers, x))
	      {
		generate_stream_alignment (s->entry, s->exit, rdg, x,
					   channels, false);
		generate_pop (&bsi, rdg, x, channels);
	      }
	  }

	mark_virtual_ops_in_bb (bb);
    }

  free (bbs);
  return loop;
}

/* Return true when U and V are in the same partition.  */

static bool
in_same_partition (VEC (bitmap, heap) *partitions, int u, int v)
{
  int i;
  bitmap partition;

  for (i = 0; VEC_iterate (bitmap, partitions, i, partition); i++)
    if (bitmap_bit_p (partition, u))
      {
	if (bitmap_bit_p (partition, v))
	  return true;
	else
	  return false;
      }

  return false;
}

/* A memory channel is a flow dependence link between two stmts, a
   stmt writing to memory followed by another stmt in another
   partition reading from that memory.  */

static void
analyze_producers (struct graph *rdg, VEC (bitmap, heap) *partitions,
		   bitmap producers, bitmap consumers)
{
  int i;

  for (i = 0; i < rdg->n_vertices; i++)
    if (RDG_MEM_WRITE_STMT (rdg, i))
      {
	struct vertex *v = &(rdg->vertices[i]);
	struct graph_edge *e = v->succ;

	if (e == NULL
	    || e->succ_next != NULL
	    || !RDG_MEM_READS_STMT (rdg, e->dest)
	    || in_same_partition (partitions, i, e->dest))
	  continue;

	/* if (incompatible_producer_consumer (rdg, i))
	   fuse (partition (i), partition (e->dest));
	*/

	bitmap_set_bit (producers, i);
	bitmap_set_bit (consumers, e->dest);
      }
}

/* Generate tasks and channels in between from the code in LOOP
   following the PARTITIONS.  */

static void
generate_tasks (struct loop *loop, struct graph *rdg,
		VEC (bitmap, heap) *partitions, bitmap producers,
		bitmap consumers)
{
  bitmap partition;
  int i;
  int nbp = VEC_length (bitmap, partitions);
  htab_t channels = htab_create (10, stream_hash, stream_eq, free);
  basic_block head, bb, switch_bb, para_ret, sections_bb, continue_bb;
  VEC (loop_p, heap) *loops = VEC_alloc (loop_p, heap, 3);
  struct loop *nloop;
  block_stmt_iterator bsi;
  tree t, control_var, cont, ret;
  tree phi, control_name_1, control_name_2, control_name_3;
  VEC (section_region, heap) *sections = VEC_alloc (section_region, heap, 3);
  section_region s;
  basic_block pre_entry = split_edge (loop_preheader_edge (loop));
  basic_block post_exit = split_edge (single_dom_exit (loop));
  edge entry, exit;
  edge e;

  for (i = 0; VEC_iterate (bitmap, partitions, i, partition); i++)
    {
      nloop = generate_streamable_loops_for_partition (loop, rdg, partition,
						       producers, consumers,
						       channels, i < nbp - 1,
						       &sections);
      VEC_safe_push (loop_p, heap, loops, nloop);
    }

  /*  rewrite_into_loop_closed_ssa (NULL, TODO_update_ssa);
      update_ssa (TODO_update_ssa_only_virtuals | TODO_update_ssa);
  */

  free_dominance_info (CDI_DOMINATORS);
  free_dominance_info (CDI_POST_DOMINATORS);
  calculate_dominance_info (CDI_DOMINATORS);
  calculate_dominance_info (CDI_POST_DOMINATORS);

  gcc_assert (single_succ_p (pre_entry));
  gcc_assert (single_pred_p (post_exit));
  entry = single_succ_edge (pre_entry);
  exit = single_pred_edge (post_exit);
  head = create_omp_parallel_region (entry, exit, create_loop_fn (), 2);

  para_ret = exit->dest;

  control_var = create_tmp_var (unsigned_type_node, ".section");
  add_referenced_var (control_var);
  control_name_1 = make_ssa_name (control_var, NULL_TREE);
  control_name_3 = make_ssa_name (control_var, NULL_TREE);

  t = build_omp_clause (OMP_CLAUSE_NUM_THREADS);
  OMP_CLAUSE_NUM_THREADS_EXPR (t)
    = build_int_cst (integer_type_node, VEC_length (bitmap, partitions));
  t = build3 (OMP_SECTIONS, void_type_node, NULL_TREE, t, control_name_1);
  SSA_NAME_DEF_STMT (control_name_1) = t;
  sections_bb = split_edge (single_succ_edge (head->next_bb));
  bsi = bsi_last (sections_bb);
  bsi_insert_after (&bsi, t, BSI_NEW_STMT);

  t = make_node (OMP_SECTIONS_SWITCH);
  switch_bb = split_edge (single_succ_edge (sections_bb));
  bsi = bsi_last (switch_bb);
  bsi_insert_after (&bsi, t, BSI_NEW_STMT);

  e = make_edge (switch_bb, para_ret, 0);
  set_immediate_dominator (CDI_DOMINATORS, para_ret, switch_bb);
  bb = split_edge (e);
  bsi = bsi_last (bb);
  bsi_insert_after (&bsi, make_node (OMP_RETURN), BSI_NEW_STMT);

  continue_bb = NULL;

  for (i = 0; VEC_iterate (section_region, sections, i, s); i++)
    {
      basic_block dummy;
      tree section = build1 (OMP_SECTION, void_type_node, NULL_TREE);

      gcc_assert (single_pred_p (s->entry));
      bb = split_edge (single_pred_edge (s->entry));
      bsi = bsi_last (bb);
      bsi_insert_before (&bsi, section, BSI_SAME_STMT);
      dummy = bb->prev_bb;
      redirect_edge_pred (single_pred_edge (bb), switch_bb);

      if (continue_bb)
	delete_basic_block (dummy);

      gcc_assert (single_succ_p (s->exit));
      bb = split_edge (single_succ_edge(s->exit));
      bsi = bsi_last (bb);
      ret = make_node (OMP_RETURN);
      bsi_insert_before (&bsi, ret, BSI_SAME_STMT);

      if (continue_bb)
	redirect_edge_succ (single_succ_edge (bb), continue_bb);
      else
	{
	  redirect_edge_succ (single_succ_edge (bb), switch_bb);
	  continue_bb = split_edge (single_succ_edge (bb));
	  bsi = bsi_last (continue_bb);
	  phi = create_phi_node (control_var, switch_bb);

	  control_name_2 = PHI_RESULT (phi);

	  add_phi_arg (phi, control_name_3,
		       single_succ_edge (continue_bb));
	  add_phi_arg (phi, control_name_1,
		       single_succ_edge (sections_bb));

	  cont = build2 (OMP_CONTINUE, void_type_node,
			 control_name_3, control_name_2);
	  bsi_insert_after (&bsi, cont, BSI_NEW_STMT);
	}

      free (s);
    }
  VEC_free (section_region, heap, sections);

  for (i = 0; VEC_iterate (edge, switch_bb->succs, i, e); i++)
    e->flags &= ~EDGE_FALLTHRU;

  scev_reset ();

  for (i = 0; VEC_iterate (loop_p, loops, i, nloop); i++)
    cancel_loop_tree (nloop);

  free_dominance_info (CDI_DOMINATORS);
  free_dominance_info (CDI_POST_DOMINATORS);
  calculate_dominance_info (CDI_DOMINATORS);
  calculate_dominance_info (CDI_POST_DOMINATORS);

  omp_expand_local (head);

  VEC_free (loop_p, heap, loops);
  htab_delete (channels);
}

/* Generate parallel code for partitions.  */

static void
streamize_loops (struct loop *loop, struct graph *rdg,
		 VEC (bitmap, heap) *partitions)
{
  bitmap producers = BITMAP_ALLOC (NULL);
  bitmap consumers = BITMAP_ALLOC (NULL);

  analyze_producers (rdg, partitions, producers, consumers);
  generate_tasks (loop, rdg, partitions, producers, consumers);

  BITMAP_FREE (producers);
  BITMAP_FREE (consumers);
}

/* Generate code from STARTING_VERTICES in RDG.  Returns the number of
   distributed loops.  */

static int
streamizer_gen (struct loop *loop, struct graph *rdg,
		VEC (int, heap) *starting_vertices)
{
  int i, nbp;
  VEC (rdgc, heap) *components = VEC_alloc (rdgc, heap, 3);
  VEC (bitmap, heap) *partitions = VEC_alloc (bitmap, heap, 3);
  VEC (int, heap) *other_stores = VEC_alloc (int, heap, 3);
  bitmap partition, processed = BITMAP_ALLOC (NULL);

  bitmap remaining_stmts = BITMAP_ALLOC (NULL);
  bitmap upstream_mem_writes = BITMAP_ALLOC (NULL);

  for (i = 0; i < rdg->n_vertices; i++)
    {
      bitmap_set_bit (remaining_stmts, i);

      /* Save in OTHER_STORES all the memory writes that are not in
	 STARTING_VERTICES.  */
      if (RDG_MEM_WRITE_STMT (rdg, i))
	{
	  int v;
	  unsigned j;
	  bool found = false;

	  for (j = 0; VEC_iterate (int, starting_vertices, j, v); j++)
	    if (i == v)
	      {
		found = true;
		break;
	      }

	  if (!found)
	    VEC_safe_push (int, heap, other_stores, i);
	}
    }

  mark_nodes_having_upstream_mem_writes (rdg, upstream_mem_writes);
  rdg_build_components (rdg, starting_vertices, &components);
  rdg_build_partitions (rdg, components, &other_stores, &partitions,
			processed, remaining_stmts, upstream_mem_writes);
  BITMAP_FREE (processed);
  nbp = VEC_length (bitmap, partitions);

  if (nbp <= 1)
    goto ldist_done;

  if (dump_file && (dump_flags & TDF_DETAILS))
    dump_rdg_partitions (dump_file, partitions);

  streamize_loops (loop, rdg, partitions);

 ldist_done:

  BITMAP_FREE (remaining_stmts);
  BITMAP_FREE (upstream_mem_writes);

  for (i = 0; VEC_iterate (bitmap, partitions, i, partition); i++)
    BITMAP_FREE (partition);

  VEC_free (int, heap, other_stores);
  VEC_free (bitmap, heap, partitions);
  free_rdg_components (components);
  return nbp;
}

/* Distributes the code from LOOP in such a way that producer
   statements are placed before consumer statements.  When STMTS is
   NULL, performs the maximal distribution, if STMTS is not NULL,
   tries to separate only these statements from the LOOP's body.
   Returns the number of distributed loops.  */

static int
streamize_loop (struct loop *loop, VEC (tree, heap) *stmts)
{
  bool res = false;
  struct graph *rdg;
  tree s;
  unsigned i;
  VEC (int, heap) *vertices;

  if (loop->num_nodes > 2)
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file,
		 "FIXME: Loop %d not streamized: it has more than two basic blocks.\n",
		 loop->num);

      return res;
    }

  rdg = build_rdg (loop);

  if (!rdg)
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file,
		 "FIXME: Loop %d not streamized: failed to build the RDG.\n",
		 loop->num);

      return res;
    }

  vertices = VEC_alloc (int, heap, 3);

  if (dump_file && (dump_flags & TDF_DETAILS))
    dump_rdg (dump_file, rdg);

  for (i = 0; VEC_iterate (tree, stmts, i, s); i++)
    {
      int v = rdg_vertex_for_stmt (rdg, s);

      if (v >= 0)
	{
	  VEC_safe_push (int, heap, vertices, v);

	  if (dump_file && (dump_flags & TDF_DETAILS))
	    fprintf (dump_file,
		     "streamizer asked to generate code for vertex %d\n", v);
	}
    }

  res = streamizer_gen (loop, rdg, vertices);
  VEC_free (int, heap, vertices);
  free_rdg (rdg);

  return res;
}

/* Distribute all loops in the current function.  */

static unsigned int
tree_loop_streamization (void)
{
  struct loop *loop;
  loop_iterator li;
  int nb_generated_loops = 0;

  FOR_EACH_LOOP (li, loop, 0)
    {
      VEC (tree, heap) *work_list = VEC_alloc (tree, heap, 3);

      /* With the following working list, we're asking distribute_loop
	 to separate the stores of the loop: when dependences allow,
	 it will end on having one store per loop.  */
      stores_from_loop (loop, &work_list);

      nb_generated_loops = streamize_loop (loop, work_list);

      if (0 && nb_generated_loops > 1)
	fprintf (stdout, "streamized to %d\n", nb_generated_loops);

      if (dump_file && (dump_flags & TDF_DETAILS))
	{
	  if (nb_generated_loops > 1)
	    fprintf (dump_file, "Loop %d distributed: split to %d loops.\n",
		     loop->num, nb_generated_loops);
	  else
	    fprintf (dump_file, "Loop %d is the same.\n", loop->num);
	}

      VEC_free (tree, heap, work_list);
    }

  return 0;
}

/* Returns true when asked for loop streamization.  */

static bool
gate_tree_loop_streamize (void)
{
  return flag_streamize_loops != 0;
}

struct gimple_opt_pass pass_loop_streamization =
{
 {
  GIMPLE_PASS,
  "streamize",			/* name */
  gate_tree_loop_streamize,  /* gate */
  tree_loop_streamization,       /* execute */
  NULL,				/* sub */
  NULL,				/* next */
  0,				/* static_pass_number */
  TV_TREE_LOOP_DISTRIBUTION,    /* tv_id */
  PROP_cfg | PROP_ssa,		/* properties_required */
  0,				/* properties_provided */
  0,				/* properties_destroyed */
  0,				/* todo_flags_start */
  TODO_update_ssa
  | TODO_dump_func 
  | TODO_verify_loops           /* todo_flags_finish */
 }
};
