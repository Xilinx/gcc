/* Folding of trees in SSA form.
   Copyright (C) 2004, 2005, 2007, 2008, 2009, 2010, 2011
   Free Software Foundation, Inc.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "tm_p.h"
#include "basic-block.h"
#include "timevar.h"
#include "gimple-pretty-print.h"
#include "tree-flow.h"
#include "tree-pass.h"
#include "tree-dump.h"
#include "langhooks.h"
#include "flags.h"
#include "gimple.h"
#include "expr.h"


/* Get the statement we can propagate from into NAME skipping
   trivial copies.  Returns the statement which defines the
   propagation source or NULL_TREE if there is no such one.
   If SINGLE_USE_ONLY is set considers only sources which have
   a single use chain up to NAME.  If SINGLE_USE_P is non-null,
   it is set to whether the chain to NAME is a single use chain
   or not.  SINGLE_USE_P is not written to if SINGLE_USE_ONLY is set.  */

static gimple
get_prop_source_stmt (tree name, bool single_use_only, bool *single_use_p)
{
  bool single_use = true;

  do {
    gimple def_stmt = SSA_NAME_DEF_STMT (name);

    if (!has_single_use (name))
      {
	single_use = false;
	if (single_use_only)
	  return NULL;
      }

    /* If name is defined by a PHI node or is the default def, bail out.  */
    if (!is_gimple_assign (def_stmt))
      return NULL;

    /* If def_stmt is not a simple copy, we possibly found it.  */
    if (!gimple_assign_ssa_name_copy_p (def_stmt))
      {
	tree rhs;

	if (!single_use_only && single_use_p)
	  *single_use_p = single_use;

	/* We can look through pointer conversions in the search
	   for a useful stmt for the comparison folding.  */
	rhs = gimple_assign_rhs1 (def_stmt);
	if (CONVERT_EXPR_CODE_P (gimple_assign_rhs_code (def_stmt))
	    && TREE_CODE (rhs) == SSA_NAME
	    && POINTER_TYPE_P (TREE_TYPE (gimple_assign_lhs (def_stmt)))
	    && POINTER_TYPE_P (TREE_TYPE (rhs)))
	  name = rhs;
	else
	  return def_stmt;
      }
    else
      {
	/* Continue searching the def of the copy source name.  */
	name = gimple_assign_rhs1 (def_stmt);
      }
  } while (1);
}

/* Checks if the destination ssa name in DEF_STMT can be used as
   propagation source.  Returns true if so, otherwise false.  */

static bool
can_propagate_from (gimple def_stmt)
{
  gcc_assert (is_gimple_assign (def_stmt));

  /* If the rhs has side-effects we cannot propagate from it.  */
  if (gimple_has_volatile_ops (def_stmt))
    return false;

  /* If the rhs is a load we cannot propagate from it.  */
  if (TREE_CODE_CLASS (gimple_assign_rhs_code (def_stmt)) == tcc_reference
      || TREE_CODE_CLASS (gimple_assign_rhs_code (def_stmt)) == tcc_declaration)
    return false;

  /* Constants can be always propagated.  */
  if (gimple_assign_single_p (def_stmt)
      && is_gimple_min_invariant (gimple_assign_rhs1 (def_stmt)))
    return true;

  /* We cannot propagate ssa names that occur in abnormal phi nodes.  */
  if (stmt_references_abnormal_ssa_name (def_stmt))
    return false;

  /* If the definition is a conversion of a pointer to a function type,
     then we can not apply optimizations as some targets require
     function pointers to be canonicalized and in this case this
     optimization could eliminate a necessary canonicalization.  */
  if (CONVERT_EXPR_CODE_P (gimple_assign_rhs_code (def_stmt)))
    {
      tree rhs = gimple_assign_rhs1 (def_stmt);
      if (POINTER_TYPE_P (TREE_TYPE (rhs))
          && TREE_CODE (TREE_TYPE (TREE_TYPE (rhs))) == FUNCTION_TYPE)
        return false;
    }

  return true;
}

/* Return the rhs of a gimple_assign STMT in a form of a single tree,
   converted to type TYPE.

   This should disappear, but is needed so we can combine expressions and use
   the fold() interfaces. Long term, we need to develop folding and combine
   routines that deal with gimple exclusively . */

static tree
rhs_to_tree (tree type, gimple stmt)
{
  location_t loc = gimple_location (stmt);
  enum tree_code code = gimple_assign_rhs_code (stmt);
  if (get_gimple_rhs_class (code) == GIMPLE_TERNARY_RHS)
    return fold_build3_loc (loc, code, type, gimple_assign_rhs1 (stmt),
			    gimple_assign_rhs2 (stmt),
			    gimple_assign_rhs3 (stmt));
  else if (get_gimple_rhs_class (code) == GIMPLE_BINARY_RHS)
    return fold_build2_loc (loc, code, type, gimple_assign_rhs1 (stmt),
			gimple_assign_rhs2 (stmt));
  else if (get_gimple_rhs_class (code) == GIMPLE_UNARY_RHS)
    return build1 (code, type, gimple_assign_rhs1 (stmt));
  else if (get_gimple_rhs_class (code) == GIMPLE_SINGLE_RHS)
    return gimple_assign_rhs1 (stmt);
  else
    gcc_unreachable ();
}

/* Combine OP0 CODE OP1 in the context of a COND_EXPR.  Returns
   the folded result in a form suitable for COND_EXPR_COND or
   NULL_TREE, if there is no suitable simplified form.  If
   INVARIANT_ONLY is true only gimple_min_invariant results are
   considered simplified.  */

static tree
combine_cond_expr_cond (gimple stmt, enum tree_code code, tree type,
			tree op0, tree op1, bool invariant_only)
{
  tree t;

  gcc_assert (TREE_CODE_CLASS (code) == tcc_comparison);

  fold_defer_overflow_warnings ();

  /* Swap the operands so that fold_binary will return NULL if we
     really did not do any folding. */
  if (tree_swap_operands_p (op0, op1, true))
    {
      t = op0;
      op0 = op1;
      op1 = t;
      code = swap_tree_comparison (code);
    }

  t = fold_binary_loc (gimple_location (stmt), code, type, op0, op1);
  if (!t)
    {
      fold_undefer_overflow_warnings (false, NULL, 0);
      return NULL_TREE;
    }

  /* Require that we got a boolean type out if we put one in.  */
  gcc_assert (TREE_CODE (TREE_TYPE (t)) == TREE_CODE (type));

  /* Bail out if we required an invariant but didn't get one.  */
  if (!t || (invariant_only && !is_gimple_min_invariant (t)))
    {
      fold_undefer_overflow_warnings (false, NULL, 0);
      return NULL_TREE;
    }

  fold_undefer_overflow_warnings (!gimple_no_warning_p (stmt), stmt, 0);

  return t;
}

/* Combine the comparison OP0 CODE OP1 at LOC with the defining statements
   of its operand.  Return a new comparison tree or NULL_TREE if there
   were no simplifying combines.  */

static tree
forward_propagate_into_comparison_1 (gimple stmt,
				     enum tree_code code, tree type,
				     tree op0, tree op1)
{
  tree tmp = NULL_TREE;
  tree rhs0 = NULL_TREE, rhs1 = NULL_TREE;
  tree rhs01 = NULL_TREE, rhs11 = NULL_TREE;
  bool single_use0_p = false, single_use1_p = false;
  bool single_use01_p = false, single_use11_p = false;

  /* FIXME: this really should not be using combine_cond_expr_cond (fold_binary)
     but matching the patterns directly.  */

  /* For comparisons use the first operand, that is likely to
     simplify comparisons against constants.  */
  if (TREE_CODE (op0) == SSA_NAME)
    {
      gimple def_stmt = get_prop_source_stmt (op0, false, &single_use0_p);
      if (def_stmt && can_propagate_from (def_stmt))
	{
	  rhs0 = rhs_to_tree (TREE_TYPE (op1), def_stmt);
	  tmp = combine_cond_expr_cond (stmt, code, type,
					rhs0, op1, !single_use0_p);
	  if (tmp)
	    return tmp;
	  /* If we have a conversion, try to combine with what we have before.  */
	  if (CONVERT_EXPR_P (rhs0)
	      && TREE_CODE (TREE_OPERAND (rhs0, 0)) == SSA_NAME)
	    {
	      gimple def_stmt1 = get_prop_source_stmt (TREE_OPERAND (rhs0, 0),
						       false, &single_use01_p);
	      if (def_stmt1 && can_propagate_from (def_stmt1))
		{
		  rhs01 = rhs_to_tree (TREE_TYPE (TREE_OPERAND (rhs0, 0)),
					  def_stmt1);
		  rhs01 = fold_convert (TREE_TYPE (op0), rhs01);
		  single_use01_p &= single_use0_p;
		  tmp = combine_cond_expr_cond (stmt, code, type,
						rhs01, op1, !single_use01_p);
		  if (tmp)
		    return tmp;
		}
	    }
	}
    }

  /* If that wasn't successful, try the second operand.  */
  if (TREE_CODE (op1) == SSA_NAME)
    {
      gimple def_stmt = get_prop_source_stmt (op1, false, &single_use1_p);
      if (def_stmt && can_propagate_from (def_stmt))
	{
	  rhs1 = rhs_to_tree (TREE_TYPE (op0), def_stmt);
	  tmp = combine_cond_expr_cond (stmt, code, type,
					op0, rhs1, !single_use1_p);
	  if (tmp)
	    return tmp;
	  /* If we have a conversion, try to combine with what we have before.  */
	  if (CONVERT_EXPR_P (rhs1)
	      && TREE_CODE (TREE_OPERAND (rhs1, 0)) == SSA_NAME)
	    {
	      gimple def_stmt1 = get_prop_source_stmt (TREE_OPERAND (rhs1, 0),
						       false, &single_use11_p);
	      if (def_stmt1 && can_propagate_from (def_stmt1))
		{
		  rhs11 = rhs_to_tree (TREE_TYPE (TREE_OPERAND (rhs1, 0)),
					  def_stmt1);
		  rhs11 = fold_convert (TREE_TYPE (op0), rhs11);
		  single_use11_p &= single_use1_p;
		  tmp = combine_cond_expr_cond (stmt, code, type,
						op0, rhs11, !single_use01_p);
		  if (tmp)
		    return tmp;
		}
	    }
	}
    }

  /* If that wasn't successful either, try both operands.  */
  if (rhs0 != NULL_TREE
      && rhs1 != NULL_TREE)
    {
      tmp = combine_cond_expr_cond (stmt, code, type,
				    rhs0, rhs1,
				    !(single_use0_p && single_use1_p));
      if (tmp)
	return tmp;
    }
  if (rhs01 != NULL_TREE
      && rhs1 != NULL_TREE)
    {
      tmp = combine_cond_expr_cond (stmt, code, type,
				    rhs01, rhs1,
				    !(single_use01_p && single_use1_p));
      if (tmp)
	return tmp;
    }
  if (rhs0 != NULL_TREE
      && rhs11 != NULL_TREE)
    {
      tmp = combine_cond_expr_cond (stmt, code, type,
				    rhs0, rhs11,
				    !(single_use0_p && single_use11_p));
      if (tmp)
	return tmp;
    }
  if (rhs01 != NULL_TREE
      && rhs11 != NULL_TREE)
    {
      tmp = combine_cond_expr_cond (stmt, code, type,
				    rhs01, rhs11,
				    !(single_use01_p && single_use11_p));
      if (tmp)
	return tmp;
    }


  return tmp;
}

static tree
expand_possible_comparison (tree tmp, gimple_stmt_iterator *gsi,
			    bool *reversed_edges)
{
  tree tmp1;
  gimple stmt = gsi_stmt (*gsi);
  tree op0, op1;
  if (!tmp)
    return NULL;

  if (TREE_CODE (tmp) == TRUTH_NOT_EXPR
       || TREE_CODE (tmp) == BIT_NOT_EXPR)
    {
      *reversed_edges ^= true;
      tmp = TREE_OPERAND (tmp, 0);
    }

  tmp1 = canonicalize_cond_expr_cond (tmp);
  if (tmp1)
    return tmp1;

  if (!COMPARISON_CLASS_P (tmp))
    return NULL;
  op0 = TREE_OPERAND (tmp, 0);
  op1 = TREE_OPERAND (tmp, 1);
  if (!is_gimple_val (op1))
    return NULL;
  if (UNARY_CLASS_P (op0)
      && is_gimple_val (TREE_OPERAND (op0, 0)))
    {
      tree var = create_tmp_reg (TREE_TYPE (op0), NULL);
      gimple newop;
      newop = gimple_build_assign_with_ops (TREE_CODE (op0),
					    var, TREE_OPERAND (op0, 0), NULL); 
      var = make_ssa_name (var, newop);
      gimple_assign_set_lhs (newop, var);
      gimple_set_location (newop, gimple_location (stmt));
      gsi_insert_before (gsi, newop, GSI_SAME_STMT);
      return fold_build2_loc (gimple_location (stmt), TREE_CODE (tmp),
			      TREE_TYPE (tmp), var, op1);
    }
  if (BINARY_CLASS_P (op0)
      && is_gimple_val (TREE_OPERAND (op0, 0))
      && is_gimple_val (TREE_OPERAND (op0, 1)))
    {
      tree var = create_tmp_reg (TREE_TYPE (op0), NULL);
      gimple newop;
      newop = gimple_build_assign_with_ops (TREE_CODE (op0),
					    var, TREE_OPERAND (op0, 0),
					    TREE_OPERAND (op0, 1)); 
      var = make_ssa_name (var, newop);
      gimple_assign_set_lhs (newop, var);
      gimple_set_location (newop, gimple_location (stmt));
      gsi_insert_before (gsi, newop, GSI_SAME_STMT);
      return fold_build2_loc (gimple_location (stmt), TREE_CODE (tmp),
			      TREE_TYPE (tmp), var, op1);
    }
  return NULL;
}

/* Propagate from the ssa name definition statements of the assignment
   from a comparison at *GSI into the conditional if that simplifies it.
   Returns true if the stmt was modified therwise returns false.  */

static bool 
forward_propagate_into_comparison (gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree tmp;
  tree type = TREE_TYPE (gimple_assign_lhs (stmt));
  tree rhs1 = gimple_assign_rhs1 (stmt);
  tree rhs2 = gimple_assign_rhs2 (stmt);
  enum tree_code code = gimple_assign_rhs_code (stmt);
  tree tmp1 = NULL_TREE;
  bool reversed_edges = false;

  /* Combine the comparison with defining statements.  */
  do {
    tmp = forward_propagate_into_comparison_1 (stmt,
					       code,
					       type, rhs1, rhs2);
    if (!tmp)
      break;
    if (!useless_type_conversion_p (type, TREE_TYPE (tmp)))
      break;
    tmp = expand_possible_comparison (tmp, gsi, &reversed_edges);
    if (!tmp)
      break;
    tmp1 = tmp;
    gimple_cond_get_ops_from_tree (tmp, &code, &rhs1, &rhs2);
  } while (tmp);

  if (reversed_edges && TREE_CODE (tmp1) == SSA_NAME)
    {
      tmp1 = fold_build1 (BIT_NOT_EXPR, TREE_TYPE (tmp1), tmp1);
      reversed_edges = false;
    }
  /* We cannot handle reversing the edges as we have no edges to reverse
     here. */
  if (!reversed_edges && tmp1 && useless_type_conversion_p (type, TREE_TYPE (tmp1)))
    {
      gimple_assign_set_rhs_from_tree (gsi, tmp1);
      fold_stmt (gsi);
      update_stmt (gsi_stmt (*gsi));

      return true;
    }

  return false;
}

/* Propagate from the ssa name definition statements of COND_EXPR
   in GIMPLE_COND statement STMT into the conditional if that simplifies it.
   Returns zero if no statement was changed, one if there were
   changes and two if cfg_cleanup needs to run.

   This must be kept in sync with forward_propagate_into_cond.  */

static bool
forward_propagate_into_gimple_cond (gimple_stmt_iterator *gsi, gimple stmt)
{
  tree tmp, tmp1 = NULL_TREE;
  enum tree_code code = gimple_cond_code (stmt);
  tree rhs1 = gimple_cond_lhs (stmt);
  tree rhs2 = gimple_cond_rhs (stmt);
  bool reversed_edges = false;

  /* We can do tree combining on SSA_NAME and comparison expressions.  */
  if (TREE_CODE_CLASS (gimple_cond_code (stmt)) != tcc_comparison)
    return 0;
  do
    {
      tmp = forward_propagate_into_comparison_1 (stmt, code,
						 boolean_type_node,
						 rhs1, rhs2);
      if (!tmp)
	break;
      tmp = expand_possible_comparison (tmp, gsi, &reversed_edges);
      if (!tmp)
	break;
      tmp1 = tmp;
      gimple_cond_get_ops_from_tree (tmp, &code, &rhs1, &rhs2);
    } while (tmp);

  if (tmp1)
    {
      if (dump_file && tmp1)
	{
	  fprintf (dump_file, "  Replaced '");
	  print_gimple_expr (dump_file, stmt, 0, 0);
	  fprintf (dump_file, "' with '");
	  print_generic_expr (dump_file, tmp1, 0);
	  fprintf (dump_file, "'\n");
	}

      gimple_cond_set_condition_from_tree (stmt, unshare_expr (tmp1));
      /* Switch around the edges if expand_poosible_comparison tells us
         we need to. */
      if (reversed_edges)
        {
          basic_block bb = gimple_bb (stmt);
          EDGE_SUCC (bb, 0)->flags ^= (EDGE_TRUE_VALUE|EDGE_FALSE_VALUE);
          EDGE_SUCC (bb, 1)->flags ^= (EDGE_TRUE_VALUE|EDGE_FALSE_VALUE);
	}
      update_stmt (stmt);

      return true;
    }

  /* Canonicalize _Bool == 0 and _Bool != 1 to _Bool != 0 by swapping edges.  */
  if ((TREE_CODE (TREE_TYPE (rhs1)) == BOOLEAN_TYPE
       || (INTEGRAL_TYPE_P (TREE_TYPE (rhs1))
	   && TYPE_PRECISION (TREE_TYPE (rhs1)) == 1))
      && ((code == EQ_EXPR
	   && integer_zerop (rhs2))
	  || (code == NE_EXPR
	      && integer_onep (rhs2))))
    {
      basic_block bb = gimple_bb (stmt);
      gimple_cond_set_code (stmt, NE_EXPR);
      gimple_cond_set_rhs (stmt, build_zero_cst (TREE_TYPE (rhs1)));
      EDGE_SUCC (bb, 0)->flags ^= (EDGE_TRUE_VALUE|EDGE_FALSE_VALUE);
      EDGE_SUCC (bb, 1)->flags ^= (EDGE_TRUE_VALUE|EDGE_FALSE_VALUE);
      return true;
    }

  return false;
}

/* Propagate from the ssa name definition statements of COND_EXPR
   in the rhs of statement STMT into the conditional if that simplifies it.
   Returns true zero if the stmt was changed.  */

static bool
forward_propagate_into_cond (gimple_stmt_iterator *gsi_p)
{
  gimple stmt = gsi_stmt (*gsi_p);
  tree tmp = NULL_TREE;
  tree cond = gimple_assign_rhs1 (stmt);
  bool swap = false;

  /* We can do tree combining on SSA_NAME and comparison expressions.  */
  if (COMPARISON_CLASS_P (cond))
    {
      enum tree_code code = TREE_CODE (cond);
      tree rhs1 = TREE_OPERAND (cond, 0);
      tree rhs2 = TREE_OPERAND (cond, 1);
      tree tmp1 = NULL_TREE;
       do
	{
          tmp = forward_propagate_into_comparison_1 (stmt, code,
						     boolean_type_node,
						     rhs1, rhs2);
	  if (!tmp)
	    break;
	  tmp = expand_possible_comparison (tmp, gsi_p, &swap);
	  if (!tmp)
	    break;
	  tmp1 = tmp;
          gimple_cond_get_ops_from_tree (tmp, &code, &rhs1, &rhs2);
	} while (tmp);
      tmp = tmp1;
    }
  else if (TREE_CODE (cond) == SSA_NAME)
    {
      enum tree_code code;
      tree name = cond;
      gimple def_stmt = get_prop_source_stmt (name, true, NULL);
      if (!def_stmt || !can_propagate_from (def_stmt))
	return false;

      code = gimple_assign_rhs_code (def_stmt);
      if (TREE_CODE_CLASS (code) == tcc_comparison)
	tmp = fold_build2_loc (gimple_location (def_stmt),
			       code,
			       boolean_type_node,
			       gimple_assign_rhs1 (def_stmt),
			       gimple_assign_rhs2 (def_stmt));
      else if ((code == BIT_NOT_EXPR
		&& TYPE_PRECISION (TREE_TYPE (cond)) == 1)
	       || (code == BIT_XOR_EXPR
		   && integer_onep (gimple_assign_rhs2 (def_stmt))))
	{
	  tmp = gimple_assign_rhs1 (def_stmt);
	  swap = true;
	}
    }

  if (tmp
      && is_gimple_condexpr (tmp))
    {
      if (dump_file && tmp)
	{
	  fprintf (dump_file, "  Replaced '");
	  print_generic_expr (dump_file, cond, 0);
	  fprintf (dump_file, "' with '");
	  print_generic_expr (dump_file, tmp, 0);
	  fprintf (dump_file, "'\n");
	}

      if (integer_onep (tmp))
	gimple_assign_set_rhs_from_tree (gsi_p, gimple_assign_rhs2 (stmt));
      else if (integer_zerop (tmp))
	gimple_assign_set_rhs_from_tree (gsi_p, gimple_assign_rhs3 (stmt));
      else
	{
	  gimple_assign_set_rhs1 (stmt, unshare_expr (tmp));
	  if (swap)
	    {
	      tree t = gimple_assign_rhs2 (stmt);
	      gimple_assign_set_rhs2 (stmt, gimple_assign_rhs3 (stmt));
	      gimple_assign_set_rhs3 (stmt, t);
	    }
	}
      stmt = gsi_stmt (*gsi_p);
      update_stmt (stmt);

      return true;
    }

  return false;
}

/* If we have lhs = ~x (STMT), look and see if earlier we had x = ~y.
   If so, we can change STMT into lhs = y which can later be copy
   propagated.  Similarly for negation.

   This could trivially be formulated as a forward propagation
   to immediate uses.  However, we already had an implementation
   from DOM which used backward propagation via the use-def links.

   It turns out that backward propagation is actually faster as
   there's less work to do for each NOT/NEG expression we find.
   Backwards propagation needs to look at the statement in a single
   backlink.  Forward propagation needs to look at potentially more
   than one forward link.

   Returns true when the statement was changed.  */

static bool 
simplify_not_neg_expr (gimple_stmt_iterator *gsi_p)
{
  gimple stmt = gsi_stmt (*gsi_p);
  tree rhs = gimple_assign_rhs1 (stmt);
  gimple rhs_def_stmt = SSA_NAME_DEF_STMT (rhs);

  /* See if the RHS_DEF_STMT has the same form as our statement.  */
  if (is_gimple_assign (rhs_def_stmt)
      && gimple_assign_rhs_code (rhs_def_stmt) == gimple_assign_rhs_code (stmt))
    {
      tree rhs_def_operand = gimple_assign_rhs1 (rhs_def_stmt);

      /* Verify that RHS_DEF_OPERAND is a suitable SSA_NAME.  */
      if (TREE_CODE (rhs_def_operand) == SSA_NAME
	  && ! SSA_NAME_OCCURS_IN_ABNORMAL_PHI (rhs_def_operand))
	{
	  gimple_assign_set_rhs_from_tree (gsi_p, rhs_def_operand);
	  stmt = gsi_stmt (*gsi_p);
	  update_stmt (stmt);
	  return true;
	}
    }

  return false;
}

/* STMT is a SWITCH_EXPR for which we attempt to find equivalent forms of
   the condition which we may be able to optimize better.  */

static bool
simplify_gimple_switch (gimple stmt)
{
  tree cond = gimple_switch_index (stmt);
  tree def, to, ti;
  gimple def_stmt;

  /* The optimization that we really care about is removing unnecessary
     casts.  That will let us do much better in propagating the inferred
     constant at the switch target.  */
  if (TREE_CODE (cond) == SSA_NAME)
    {
      def_stmt = SSA_NAME_DEF_STMT (cond);
      if (is_gimple_assign (def_stmt))
	{
	  if (gimple_assign_rhs_code (def_stmt) == NOP_EXPR)
	    {
	      int need_precision;
	      bool fail;

	      def = gimple_assign_rhs1 (def_stmt);

	      /* ??? Why was Jeff testing this?  We are gimple...  */
	      gcc_checking_assert (is_gimple_val (def));

	      to = TREE_TYPE (cond);
	      ti = TREE_TYPE (def);

	      /* If we have an extension that preserves value, then we
		 can copy the source value into the switch.  */

	      need_precision = TYPE_PRECISION (ti);
	      fail = false;
	      if (! INTEGRAL_TYPE_P (ti))
		fail = true;
	      else if (TYPE_UNSIGNED (to) && !TYPE_UNSIGNED (ti))
		fail = true;
	      else if (!TYPE_UNSIGNED (to) && TYPE_UNSIGNED (ti))
		need_precision += 1;
	      if (TYPE_PRECISION (to) < need_precision)
		fail = true;

	      if (!fail)
		{
		  gimple_switch_set_index (stmt, def);
		  update_stmt (stmt);
		  return true;
		}
	    }
	}
    }

  return false;
}

/* For pointers p2 and p1 return p2 - p1 if the
   difference is known and constant, otherwise return NULL.  */

static tree
constant_pointer_difference (tree p1, tree p2)
{
  int i, j;
#define CPD_ITERATIONS 5
  tree exps[2][CPD_ITERATIONS];
  tree offs[2][CPD_ITERATIONS];
  int cnt[2];

  for (i = 0; i < 2; i++)
    {
      tree p = i ? p1 : p2;
      tree off = size_zero_node;
      gimple stmt;
      enum tree_code code;

      /* For each of p1 and p2 we need to iterate at least
	 twice, to handle ADDR_EXPR directly in p1/p2,
	 SSA_NAME with ADDR_EXPR or POINTER_PLUS_EXPR etc.
	 on definition's stmt RHS.  Iterate a few extra times.  */
      j = 0;
      do
	{
	  if (!POINTER_TYPE_P (TREE_TYPE (p)))
	    break;
	  if (TREE_CODE (p) == ADDR_EXPR)
	    {
	      tree q = TREE_OPERAND (p, 0);
	      HOST_WIDE_INT offset;
	      tree base = get_addr_base_and_unit_offset (q, &offset);
	      if (base)
		{
		  q = base;
		  if (offset)
		    off = size_binop (PLUS_EXPR, off, size_int (offset));
		}
	      if (TREE_CODE (q) == MEM_REF
		  && TREE_CODE (TREE_OPERAND (q, 0)) == SSA_NAME)
		{
		  p = TREE_OPERAND (q, 0);
		  off = size_binop (PLUS_EXPR, off,
				    double_int_to_tree (sizetype,
							mem_ref_offset (q)));
		}
	      else
		{
		  exps[i][j] = q;
		  offs[i][j++] = off;
		  break;
		}
	    }
	  if (TREE_CODE (p) != SSA_NAME)
	    break;
	  exps[i][j] = p;
	  offs[i][j++] = off;
	  if (j == CPD_ITERATIONS)
	    break;
	  stmt = SSA_NAME_DEF_STMT (p);
	  if (!is_gimple_assign (stmt) || gimple_assign_lhs (stmt) != p)
	    break;
	  code = gimple_assign_rhs_code (stmt);
	  if (code == POINTER_PLUS_EXPR)
	    {
	      if (TREE_CODE (gimple_assign_rhs2 (stmt)) != INTEGER_CST)
		break;
	      off = size_binop (PLUS_EXPR, off, gimple_assign_rhs2 (stmt));
	      p = gimple_assign_rhs1 (stmt);
	    }
	  else if (code == ADDR_EXPR || code == NOP_EXPR)
	    p = gimple_assign_rhs1 (stmt);
	  else
	    break;
	}
      while (1);
      cnt[i] = j;
    }

  for (i = 0; i < cnt[0]; i++)
    for (j = 0; j < cnt[1]; j++)
      if (exps[0][i] == exps[1][j])
	return size_binop (MINUS_EXPR, offs[0][i], offs[1][j]);

  return NULL_TREE;
}

/* *GSI_P is a GIMPLE_CALL to a builtin function.
   Optimize
   memcpy (p, "abcd", 4);
   memset (p + 4, ' ', 3);
   into
   memcpy (p, "abcd   ", 7);
   call if the latter can be stored by pieces during expansion.  */

static bool
simplify_builtin_call (gimple_stmt_iterator *gsi_p, tree callee2)
{
  gimple stmt1, stmt2 = gsi_stmt (*gsi_p);
  tree vuse = gimple_vuse (stmt2);
  if (vuse == NULL)
    return false;
  stmt1 = SSA_NAME_DEF_STMT (vuse);

  switch (DECL_FUNCTION_CODE (callee2))
    {
    case BUILT_IN_MEMSET:
      if (gimple_call_num_args (stmt2) != 3
	  || gimple_call_lhs (stmt2)
	  || CHAR_BIT != 8
	  || BITS_PER_UNIT != 8)
	break;
      else
	{
	  tree callee1;
	  tree ptr1, src1, str1, off1, len1, lhs1;
	  tree ptr2 = gimple_call_arg (stmt2, 0);
	  tree val2 = gimple_call_arg (stmt2, 1);
	  tree len2 = gimple_call_arg (stmt2, 2);
	  tree diff, vdef, new_str_cst;
	  gimple use_stmt;
	  unsigned int ptr1_align;
	  unsigned HOST_WIDE_INT src_len;
	  char *src_buf;
	  use_operand_p use_p;

	  if (!host_integerp (val2, 0)
	      || !host_integerp (len2, 1))
	    break;
	  if (is_gimple_call (stmt1))
	    {
	      /* If first stmt is a call, it needs to be memcpy
		 or mempcpy, with string literal as second argument and
		 constant length.  */
	      callee1 = gimple_call_fndecl (stmt1);
	      if (callee1 == NULL_TREE
		  || DECL_BUILT_IN_CLASS (callee1) != BUILT_IN_NORMAL
		  || gimple_call_num_args (stmt1) != 3)
		break;
	      if (DECL_FUNCTION_CODE (callee1) != BUILT_IN_MEMCPY
		  && DECL_FUNCTION_CODE (callee1) != BUILT_IN_MEMPCPY)
		break;
	      ptr1 = gimple_call_arg (stmt1, 0);
	      src1 = gimple_call_arg (stmt1, 1);
	      len1 = gimple_call_arg (stmt1, 2);
	      lhs1 = gimple_call_lhs (stmt1);
	      if (!host_integerp (len1, 1))
		break;
	      str1 = string_constant (src1, &off1);
	      if (str1 == NULL_TREE)
		break;
	      if (!host_integerp (off1, 1)
		  || compare_tree_int (off1, TREE_STRING_LENGTH (str1) - 1) > 0
		  || compare_tree_int (len1, TREE_STRING_LENGTH (str1)
					     - tree_low_cst (off1, 1)) > 0
		  || TREE_CODE (TREE_TYPE (str1)) != ARRAY_TYPE
		  || TYPE_MODE (TREE_TYPE (TREE_TYPE (str1)))
		     != TYPE_MODE (char_type_node))
		break;
	    }
	  else if (gimple_assign_single_p (stmt1))
	    {
	      /* Otherwise look for length 1 memcpy optimized into
		 assignment.  */
    	      ptr1 = gimple_assign_lhs (stmt1);
	      src1 = gimple_assign_rhs1 (stmt1);
	      if (TREE_CODE (ptr1) != MEM_REF
		  || TYPE_MODE (TREE_TYPE (ptr1)) != TYPE_MODE (char_type_node)
		  || !host_integerp (src1, 0))
		break;
	      ptr1 = build_fold_addr_expr (ptr1);
	      callee1 = NULL_TREE;
	      len1 = size_one_node;
	      lhs1 = NULL_TREE;
	      off1 = size_zero_node;
	      str1 = NULL_TREE;
	    }
	  else
	    break;

	  diff = constant_pointer_difference (ptr1, ptr2);
	  if (diff == NULL && lhs1 != NULL)
	    {
	      diff = constant_pointer_difference (lhs1, ptr2);
	      if (DECL_FUNCTION_CODE (callee1) == BUILT_IN_MEMPCPY
		  && diff != NULL)
		diff = size_binop (PLUS_EXPR, diff,
				   fold_convert (sizetype, len1));
	    }
	  /* If the difference between the second and first destination pointer
	     is not constant, or is bigger than memcpy length, bail out.  */
	  if (diff == NULL
	      || !host_integerp (diff, 1)
	      || tree_int_cst_lt (len1, diff))
	    break;

	  /* Use maximum of difference plus memset length and memcpy length
	     as the new memcpy length, if it is too big, bail out.  */
	  src_len = tree_low_cst (diff, 1);
	  src_len += tree_low_cst (len2, 1);
	  if (src_len < (unsigned HOST_WIDE_INT) tree_low_cst (len1, 1))
	    src_len = tree_low_cst (len1, 1);
	  if (src_len > 1024)
	    break;

	  /* If mempcpy value is used elsewhere, bail out, as mempcpy
	     with bigger length will return different result.  */
	  if (lhs1 != NULL_TREE
	      && DECL_FUNCTION_CODE (callee1) == BUILT_IN_MEMPCPY
	      && (TREE_CODE (lhs1) != SSA_NAME
		  || !single_imm_use (lhs1, &use_p, &use_stmt)
		  || use_stmt != stmt2))
	    break;

	  /* If anything reads memory in between memcpy and memset
	     call, the modified memcpy call might change it.  */
	  vdef = gimple_vdef (stmt1);
	  if (vdef != NULL
	      && (!single_imm_use (vdef, &use_p, &use_stmt)
		  || use_stmt != stmt2))
	    break;

	  ptr1_align = get_pointer_alignment (ptr1);
	  /* Construct the new source string literal.  */
	  src_buf = XALLOCAVEC (char, src_len + 1);
	  if (callee1)
	    memcpy (src_buf,
		    TREE_STRING_POINTER (str1) + tree_low_cst (off1, 1),
		    tree_low_cst (len1, 1));
	  else
	    src_buf[0] = tree_low_cst (src1, 0);
	  memset (src_buf + tree_low_cst (diff, 1),
		  tree_low_cst (val2, 1), tree_low_cst (len2, 1));
	  src_buf[src_len] = '\0';
	  /* Neither builtin_strncpy_read_str nor builtin_memcpy_read_str
	     handle embedded '\0's.  */
	  if (strlen (src_buf) != src_len)
	    break;
	  rtl_profile_for_bb (gimple_bb (stmt2));
	  /* If the new memcpy wouldn't be emitted by storing the literal
	     by pieces, this optimization might enlarge .rodata too much,
	     as commonly used string literals couldn't be shared any
	     longer.  */
	  if (!can_store_by_pieces (src_len,
				    builtin_strncpy_read_str,
				    src_buf, ptr1_align, false))
	    break;

	  new_str_cst = build_string_literal (src_len, src_buf);
	  if (callee1)
	    {
	      /* If STMT1 is a mem{,p}cpy call, adjust it and remove
		 memset call.  */
	      if (lhs1 && DECL_FUNCTION_CODE (callee1) == BUILT_IN_MEMPCPY)
		gimple_call_set_lhs (stmt1, NULL_TREE);
	      gimple_call_set_arg (stmt1, 1, new_str_cst);
	      gimple_call_set_arg (stmt1, 2,
				   build_int_cst (TREE_TYPE (len1), src_len));
	      update_stmt (stmt1);
	      unlink_stmt_vdef (stmt2);
	      gsi_remove (gsi_p, true);
	      release_defs (stmt2);
	      if (lhs1 && DECL_FUNCTION_CODE (callee1) == BUILT_IN_MEMPCPY)
		release_ssa_name (lhs1);
	      return true;
	    }
	  else
	    {
	      /* Otherwise, if STMT1 is length 1 memcpy optimized into
		 assignment, remove STMT1 and change memset call into
		 memcpy call.  */
	      gimple_stmt_iterator gsi = gsi_for_stmt (stmt1);

	      if (!is_gimple_val (ptr1))
		ptr1 = force_gimple_operand_gsi (gsi_p, ptr1, true, NULL_TREE,
						 true, GSI_SAME_STMT);
	      gimple_call_set_fndecl (stmt2,
				      builtin_decl_explicit (BUILT_IN_MEMCPY));
	      gimple_call_set_arg (stmt2, 0, ptr1);
	      gimple_call_set_arg (stmt2, 1, new_str_cst);
	      gimple_call_set_arg (stmt2, 2,
				   build_int_cst (TREE_TYPE (len2), src_len));
	      unlink_stmt_vdef (stmt1);
	      gsi_remove (&gsi, true);
	      release_defs (stmt1);
	      update_stmt (stmt2);
	      return false;
	    }
	}
      break;
    default:
      break;
    }
  return false;
}

/* Checks if expression has type of one-bit precision, or is a known
   truth-valued expression.  */
static bool
truth_valued_ssa_name (tree name)
{
  gimple def;
  tree type = TREE_TYPE (name);

  if (!INTEGRAL_TYPE_P (type))
    return false;
  /* Don't check here for BOOLEAN_TYPE as the precision isn't
     necessarily one and so ~X is not equal to !X.  */
  if (TYPE_PRECISION (type) == 1)
    return true;
  def = SSA_NAME_DEF_STMT (name);
  if (is_gimple_assign (def))
    return truth_value_p (gimple_assign_rhs_code (def));
  return false;
}

/* Helper routine for simplify_bitwise_binary_1 function.
   Return for the SSA name NAME the expression X if it mets condition
   NAME = !X. Otherwise return NULL_TREE.
   Detected patterns for NAME = !X are:
     !X and X == 0 for X with integral type.
     X ^ 1, X != 1,or ~X for X with integral type with precision of one.  */
static tree
lookup_logical_inverted_value (tree name)
{
  tree op1, op2;
  enum tree_code code;
  gimple def;

  /* If name has none-intergal type, or isn't a SSA_NAME, then
     return.  */
  if (TREE_CODE (name) != SSA_NAME
      || !INTEGRAL_TYPE_P (TREE_TYPE (name)))
    return NULL_TREE;
  def = SSA_NAME_DEF_STMT (name);
  if (!is_gimple_assign (def))
    return NULL_TREE;

  code = gimple_assign_rhs_code (def);
  op1 = gimple_assign_rhs1 (def);
  op2 = NULL_TREE;

  /* Get for EQ_EXPR or BIT_XOR_EXPR operation the second operand.
     If CODE isn't an EQ_EXPR, BIT_XOR_EXPR, or BIT_NOT_EXPR, then return.  */
  if (code == EQ_EXPR || code == NE_EXPR
      || code == BIT_XOR_EXPR)
    op2 = gimple_assign_rhs2 (def);

  switch (code)
    {
    case BIT_NOT_EXPR:
      if (truth_valued_ssa_name (name))
	return op1;
      break;
    case EQ_EXPR:
      /* Check if we have X == 0 and X has an integral type.  */
      if (!INTEGRAL_TYPE_P (TREE_TYPE (op1)))
	break;
      if (integer_zerop (op2))
	return op1;
      break;
    case NE_EXPR:
      /* Check if we have X != 1 and X is a truth-valued.  */
      if (!INTEGRAL_TYPE_P (TREE_TYPE (op1)))
	break;
      if (integer_onep (op2) && truth_valued_ssa_name (op1))
	return op1;
      break;
    case BIT_XOR_EXPR:
      /* Check if we have X ^ 1 and X is truth valued.  */
      if (integer_onep (op2) && truth_valued_ssa_name (op1))
	return op1;
      break;
    default:
      break;
    }

  return NULL_TREE;
}

/* Optimize ARG1 CODE ARG2 to a constant for bitwise binary
   operations CODE, if one operand has the logically inverted
   value of the other.  */
static tree
simplify_bitwise_binary_1 (enum tree_code code, tree type,
			   tree arg1, tree arg2)
{
  tree anot;

  /* If CODE isn't a bitwise binary operation, return NULL_TREE.  */
  if (code != BIT_AND_EXPR && code != BIT_IOR_EXPR
      && code != BIT_XOR_EXPR)
    return NULL_TREE;

  /* First check if operands ARG1 and ARG2 are equal.  If so
     return NULL_TREE as this optimization is handled fold_stmt.  */
  if (arg1 == arg2)
    return NULL_TREE;
  /* See if we have in arguments logical-not patterns.  */
  if (((anot = lookup_logical_inverted_value (arg1)) == NULL_TREE
       || anot != arg2)
      && ((anot = lookup_logical_inverted_value (arg2)) == NULL_TREE
	  || anot != arg1))
    return NULL_TREE;

  /* X & !X -> 0.  */
  if (code == BIT_AND_EXPR)
    return fold_convert (type, integer_zero_node);
  /* X | !X -> 1 and X ^ !X -> 1, if X is truth-valued.  */
  if (truth_valued_ssa_name (anot))
    return fold_convert (type, integer_one_node);

  /* ??? Otherwise result is (X != 0 ? X : 1).  not handled.  */
  return NULL_TREE;
}

static inline bool
valid_simple_gimple (tree newexpr)
{
  if (is_gimple_val (newexpr))
    return true;
  if (UNARY_CLASS_P (newexpr)
     && is_gimple_val (TREE_OPERAND (newexpr, 0)))
    return true;
  if (BINARY_CLASS_P (newexpr)
      && is_gimple_val (TREE_OPERAND (newexpr, 0))
      && is_gimple_val (TREE_OPERAND (newexpr, 1)))
    return true;
  return false;
}

static tree extract_simple_gimple (location_t loc, gimple_stmt_iterator *gsi,
				   tree expr);
static tree build_simple_gimple (location_t loc, gimple_stmt_iterator *gsi, tree expr)
{
  gimple newop;
  tree op1, tem;
  if (is_gimple_val (expr))
    return expr;
  op1 = extract_simple_gimple (loc, gsi, expr);
  tem = create_tmp_reg (TREE_TYPE (op1), NULL);
  newop = gimple_build_assign (tem, op1);
  tem = make_ssa_name (tem, newop);
  gimple_assign_set_lhs (newop, tem);
  gimple_set_location (newop, loc);
  gsi_insert_before (gsi, newop, GSI_SAME_STMT);
  return tem;
}

static tree
extract_simple_gimple (location_t loc, gimple_stmt_iterator *gsi, tree expr)
{
  enum tree_code code = TREE_CODE (expr);
  if (valid_simple_gimple (expr))
    return expr;
  if (UNARY_CLASS_P (expr))
    {
      tree op1 = build_simple_gimple (loc, gsi, TREE_OPERAND (expr, 0));
      if (op1 == NULL_TREE)
	return NULL_TREE;
      return build1 (code, TREE_TYPE (expr), op1);
    }
  /* For comparisons, create a tmp variable to hold the comparison
     if the type is not bool. */
  if (COMPARISON_CLASS_P (expr)
      && TREE_CODE (TREE_TYPE (expr)) != BOOLEAN_TYPE)
    {
      tree tmp = build2 (code, boolean_type_node, TREE_OPERAND (expr, 0),
			 TREE_OPERAND (expr, 1));
      tmp = build_simple_gimple (loc, gsi, tmp);
      return build1 (NOP_EXPR, TREE_TYPE (expr), tmp);
    }
  if (BINARY_CLASS_P (expr) || COMPARISON_CLASS_P (expr))
    {
      tree op1 = build_simple_gimple (loc, gsi, TREE_OPERAND (expr, 0));
      tree op2 = build_simple_gimple (loc, gsi, TREE_OPERAND (expr, 1));
      if (op1 == NULL_TREE)
	return NULL_TREE;
      if (op2 == NULL_TREE)
	return NULL_TREE;
      return build2 (code, TREE_TYPE (expr), op1, op2);
    }
  return NULL_TREE;
}

/* Given a ssa_name in NAME see if it was defined by an assignment and
   set CODE to be the code and ARG1 to the first operand on the rhs and ARG2
   to the second operand on the rhs. */
static inline void
defcodefor_name (tree name, enum tree_code *code, tree *arg1, tree *arg2)
{
  gimple def;
  gcc_assert (TREE_CODE (name) == SSA_NAME);
  def = SSA_NAME_DEF_STMT (name);
  if (is_gimple_assign (def))
    {
      *code = gimple_assign_rhs_code (def);
      *arg1 = gimple_assign_rhs1 (def);
      if (arg2)
        *arg2 = gimple_assign_rhs2 (def);
    }
  else
    {
      *code = SSA_NAME;
      *arg1 = name;
      if (arg2)
        *arg2 = NULL;
    }
}

static tree simplify_bitwise_binary_3 (gimple_stmt_iterator *gsi,
				       nonzerobits_t nonzerobitsp,
			   	       tree arg1, tree arg2,
				       enum tree_code code);
/* Simplify bitwise binary operations.
   Return the tree of what the code was transformed into.  */


static tree
simplify_bitwise_binary_2 (gimple_stmt_iterator *gsi, nonzerobits_t nonzerobitsp,
			   tree arg1, tree arg2, enum tree_code code)
{
  gimple stmt = gsi_stmt (*gsi);
  tree res;
  tree def1_arg1, def1_arg2 = NULL_TREE, def2_arg1, def2_arg2 = NULL_TREE;
  enum tree_code def1_code, def2_code;
  tree type = TREE_TYPE (arg1);
  location_t loc = gimple_location (stmt);

  gcc_assert (code == BIT_AND_EXPR
	      || code == BIT_XOR_EXPR
	      || code == BIT_IOR_EXPR);

  def1_code = TREE_CODE (arg1);
  def1_arg1 = arg1;

  if (def1_code == SSA_NAME)
    defcodefor_name (arg1, &def1_code, &def1_arg1, &def1_arg2);

  def2_code = TREE_CODE (arg2);
  def2_arg1 = arg2;

  if (def2_code == SSA_NAME)
    defcodefor_name (arg2, &def2_code, &def2_arg1, &def2_arg2);

  /* Try to optimize away the AND based on the nonzero bits info. */
  if (code == BIT_AND_EXPR)
    {
      double_int nzop1 = nonzerobitsp (arg1);
      double_int nzop2;
      if (TREE_CODE (arg2) == INTEGER_CST)
	{
	  double_int val2 = tree_to_double_int (arg2);
	  if (double_int_zero_p (double_int_and_not (nzop1, val2)))
	    return arg1;
        }
        nzop2 = nonzerobitsp (arg2);
        /* If we are clearing all the nonzero bits, the result is zero.  */
        if (double_int_zero_p (double_int_and (nzop1, nzop2)))
	  return fold_convert (TREE_TYPE (arg1), integer_zero_node);
    }

  /* A | C is C if all bits of A that might be nonzero are on in C.  */
  if (code == BIT_IOR_EXPR
      && TREE_CODE (arg2) == INTEGER_CST
      && double_int_zero_p (double_int_and_not (nonzerobitsp (arg1),
						tree_to_double_int (arg2))))
						
    return arg2;

  /* Try to fold (type) X op CST -> (type) (X op ((type-x) CST)).  */
  if (TREE_CODE (arg2) == INTEGER_CST
      && CONVERT_EXPR_CODE_P (def1_code)
      && INTEGRAL_TYPE_P (TREE_TYPE (def1_arg1))
      && int_fits_type_p (arg2, TREE_TYPE (def1_arg1)))
    {
      tree tmp, cst;
      cst = fold_convert_loc (loc, TREE_TYPE (def1_arg1), arg2);
      tmp = simplify_bitwise_binary_3 (gsi, nonzerobitsp, def1_arg1, cst, code);
      /* Don't use fold here since it undos this conversion.  */
      return build1_loc (loc, NOP_EXPR, TREE_TYPE (arg1), tmp);
    }

  /* For bitwise binary operations apply operand conversions to the
     binary operation result instead of to the operands.  This allows
     to combine successive conversions and bitwise binary operations.  */
  if (CONVERT_EXPR_CODE_P (def1_code)
      && CONVERT_EXPR_CODE_P (def2_code)
      && types_compatible_p (TREE_TYPE (def1_arg1), TREE_TYPE (def2_arg1))
      /* Make sure that the conversion widens the operands, or has same
	 precision,  or that it changes the operation to a bitfield
	 precision.  */
      && ((TYPE_PRECISION (TREE_TYPE (def1_arg1))
	   <= TYPE_PRECISION (TREE_TYPE (arg1)))
	  || (GET_MODE_CLASS (TYPE_MODE (TREE_TYPE (arg1)))
	      != MODE_INT)
	  || (TYPE_PRECISION (TREE_TYPE (arg1))
	      != GET_MODE_PRECISION (TYPE_MODE (TREE_TYPE (arg1))))))
    {
      tree tmp;
      tmp = simplify_bitwise_binary_3 (gsi, nonzerobitsp, def1_arg1, def2_arg1, code);
      return fold_convert_loc (loc, type, tmp);
    }

  /* (a | CST1) & CST2  ->  (a & CST2) | (CST1 & CST2).  */
  if (code == BIT_AND_EXPR
      && def1_code == BIT_IOR_EXPR
      && TREE_CODE (arg2) == INTEGER_CST
      && TREE_CODE (def1_arg2) == INTEGER_CST)
    {
      tree cst = fold_build2 (BIT_AND_EXPR, type,
			      arg2, def1_arg2);
      tree tem;
      tem = simplify_bitwise_binary_3 (gsi, nonzerobitsp, def1_arg1, arg2, code);
      if (integer_zerop (cst))
	return tem;
      return fold_build2_loc (loc, def1_code, type, tem, cst);
    }

  /* Combine successive equal operations with constants.  */
  if (def1_code == code 
      && TREE_CODE (arg2) == INTEGER_CST
      && TREE_CODE (def1_arg2) == INTEGER_CST)
    {
      tree cst = fold_build2 (code, type, arg2, def1_arg2);
      return simplify_bitwise_binary_3 (gsi, nonzerobitsp, def1_arg1, cst, code);
    }

   /* Fold (A OP1 B) OP0 (C OP1 B) to (A OP0 C) OP1 B. */
   if (def1_code == def2_code
       && (def1_code == BIT_AND_EXPR
	   || def1_code == BIT_XOR_EXPR
	   || def1_code == BIT_IOR_EXPR)
       && operand_equal_for_phi_arg_p (def1_arg2,
				       def2_arg2))
    {
      tree inner = simplify_bitwise_binary_3 (gsi, nonzerobitsp, def1_arg1, def2_arg1, code);
      if (integer_zerop (inner))
	{
	  if (def1_code == BIT_AND_EXPR)
	    return inner;
	  else
	    return def1_arg2;
	}
      else
      	return fold_build2 (def1_code, type, inner, def1_arg2);
    }

  /* Canonicalize X ^ ~0 to ~X.  */
  if (code == BIT_XOR_EXPR
      && TREE_CODE (arg2) == INTEGER_CST
      && integer_all_onesp (arg2))
    return fold_build1 (BIT_NOT_EXPR, type, arg1);

  /* Fold (X ^ Y) & Y as ~X & Y.  */
  if (code == BIT_AND_EXPR
      && def1_code == BIT_XOR_EXPR
      && operand_equal_for_phi_arg_p (def1_arg2, arg2))
    {
      tree tem;
      tem = fold_build1 (BIT_NOT_EXPR, type, def1_arg1);
      return fold_build2 (code, type, arg2, tem);
    }

  /* Fold (X ^ Y) & X as ~Y & X.  */
  if (code == BIT_AND_EXPR
      && def1_code == BIT_XOR_EXPR
      && operand_equal_for_phi_arg_p (def1_arg1, arg2))
    {
      tree tem;
      tem = fold_build1 (BIT_NOT_EXPR, type, def1_arg2);
      return fold_build2 (code, type, arg2, tem);
    }

  /* Fold Y & (X ^ Y) as Y & ~X.  */
  if (code == BIT_AND_EXPR
      && def2_code == BIT_XOR_EXPR
      && operand_equal_for_phi_arg_p (def2_arg2, arg1))
    {
      tree tem;
      tem = fold_build1 (BIT_NOT_EXPR, type, def2_arg1);
      return fold_build2 (code, type, arg1, tem);
    }
    

  /* Fold X & (X ^ Y) as X & ~Y.  */
  if (code == BIT_AND_EXPR
      && def2_code == BIT_XOR_EXPR
      && operand_equal_for_phi_arg_p (def2_arg1, arg1))
    {
      tree tem;
      tem = fold_build1 (BIT_NOT_EXPR, type, def2_arg2);
      return fold_build2 (code, type, arg1, tem);
    }

  /* Fold ~X & N into X ^ N if X's nonzerobits is equal to N. */
  if (code == BIT_AND_EXPR
      && def1_code == BIT_NOT_EXPR
      && TREE_CODE (arg2) == INTEGER_CST
      && double_int_equal_p (tree_to_double_int (arg2),
			     nonzerobitsp (def1_arg1)))
    return simplify_bitwise_binary_3 (gsi, nonzerobitsp, def1_arg1, arg2, BIT_XOR_EXPR);

  /* Try simple folding for X op !X, and X op X.  */
  res = simplify_bitwise_binary_1 (code, TREE_TYPE (arg1), arg1, arg2);
  if (res != NULL_TREE)
    return res;

  if (code == BIT_AND_EXPR || code == BIT_IOR_EXPR)
    {
      enum tree_code ocode = code == BIT_AND_EXPR ? BIT_IOR_EXPR : BIT_AND_EXPR;
      if (def1_code == ocode)
	{
	  tree x = arg2;
	  /* ( X | Y) & X -> X */
	  /* ( X & Y) | X -> X */
	  if (x == def1_arg1
	      || x == def1_arg2)
	    return x;
	  if (TREE_CODE (def1_arg1) == SSA_NAME)
	    {
	      enum tree_code coden;
	      tree a1, a2;
	      defcodefor_name (def1_arg1, &coden, &a1, &a2);
	      /* (~X | Y) & X -> X & Y */
	      /* (~X & Y) | X -> X | Y */
	      if (coden == BIT_NOT_EXPR && a1 == x)
		return simplify_bitwise_binary_3 (gsi, nonzerobitsp, def1_arg2, x, code);
	    }
	  if (TREE_CODE (def1_arg2) == SSA_NAME)
	    {
	      enum tree_code coden;
	      tree a1, a2;
	      defcodefor_name (def1_arg2, &coden, &a1, &a2);
	      /* (Y | ~X) & X -> X & Y */
	      /* (Y & ~X) | X -> X | Y */
	      if (coden == BIT_NOT_EXPR && a1 == x)
		return fold_build2 (code, type, x, def1_arg1);
	    }
	}
      if (def2_code == ocode)
	{
	  tree x = arg1;
	  /* X & ( X | Y) -> X */
	  /* X | ( X & Y) -> X */
	  if (x == def2_arg1
	      || x == def2_arg2)
	    return x;
	  if (TREE_CODE (def2_arg1) == SSA_NAME)
	    {
	      enum tree_code coden;
	      tree a1;
	      defcodefor_name (def2_arg1, &coden, &a1, NULL);
	      /* (~X | Y) & X -> X & Y */
	      /* (~X & Y) | X -> X | Y */
	      if (coden == BIT_NOT_EXPR && a1 == x)
		return fold_build2 (code, type, x, def2_arg2);
	    }
	  if (TREE_CODE (def2_arg2) == SSA_NAME)
	    {
	      enum tree_code coden;
	      tree a1 = NULL;
	      defcodefor_name (def2_arg2, &coden, &a1, NULL);
	      /* (Y | ~X) & X -> X & Y */
	      /* (Y & ~X) | X -> X | Y */
	      if (coden == BIT_NOT_EXPR && a1 == x)
		return fold_build2 (code, type, x, def2_arg1);
	    }
	}
    }

  return NULL;
}

static tree
simplify_bitwise_binary_3 (gimple_stmt_iterator *gsi, nonzerobits_t nonzerobitsp,
			   tree arg1, tree arg2, enum tree_code code)
{
  tree tmp;
  tmp = simplify_bitwise_binary_2 (gsi, nonzerobitsp, arg1, arg2, code);
  if (tmp)
    return tmp;
  return build2 (code, TREE_TYPE (arg1), arg1, arg2);
}

static tree
simplify_bitwise_binary (gimple_stmt_iterator *gsi, nonzerobits_t nonzerobitsp)
{
  gimple stmt = gsi_stmt (*gsi);
  tree arg1 = gimple_assign_rhs1 (stmt);
  tree arg2 = gimple_assign_rhs2 (stmt);
  enum tree_code code = gimple_assign_rhs_code (stmt);
  return simplify_bitwise_binary_2 (gsi, nonzerobitsp, arg1, arg2,
				    code);
}


/* Perform re-associations of the plus or minus statement STMT that are
   always permitted.  Returns true if the CFG was changed.  */

static bool
associate_plusminus (gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree rhs1 = gimple_assign_rhs1 (stmt);
  tree rhs2 = gimple_assign_rhs2 (stmt);
  enum tree_code code = gimple_assign_rhs_code (stmt);
  bool changed;

  /* We can't reassociate at all for saturating types.  */
  if (TYPE_SATURATING (TREE_TYPE (rhs1)))
    return false;

  /* First contract negates.  */
  do
    {
      changed = false;

      /* A +- (-B) -> A -+ B.  */
      if (TREE_CODE (rhs2) == SSA_NAME)
	{
	  gimple def_stmt = SSA_NAME_DEF_STMT (rhs2);
	  if (is_gimple_assign (def_stmt)
	      && gimple_assign_rhs_code (def_stmt) == NEGATE_EXPR
	      && can_propagate_from (def_stmt))
	    {
	      code = (code == MINUS_EXPR) ? PLUS_EXPR : MINUS_EXPR;
	      gimple_assign_set_rhs_code (stmt, code);
	      rhs2 = gimple_assign_rhs1 (def_stmt);
	      gimple_assign_set_rhs2 (stmt, rhs2);
	      gimple_set_modified (stmt, true);
	      changed = true;
	    }
	}

      /* (-A) + B -> B - A.  */
      if (TREE_CODE (rhs1) == SSA_NAME
	  && code == PLUS_EXPR)
	{
	  gimple def_stmt = SSA_NAME_DEF_STMT (rhs1);
	  if (is_gimple_assign (def_stmt)
	      && gimple_assign_rhs_code (def_stmt) == NEGATE_EXPR
	      && can_propagate_from (def_stmt))
	    {
	      code = MINUS_EXPR;
	      gimple_assign_set_rhs_code (stmt, code);
	      rhs1 = rhs2;
	      gimple_assign_set_rhs1 (stmt, rhs1);
	      rhs2 = gimple_assign_rhs1 (def_stmt);
	      gimple_assign_set_rhs2 (stmt, rhs2);
	      gimple_set_modified (stmt, true);
	      changed = true;
	    }
	}
    }
  while (changed);

  /* We can't reassociate floating-point or fixed-point plus or minus
     because of saturation to +-Inf.  */
  if (FLOAT_TYPE_P (TREE_TYPE (rhs1))
      || FIXED_POINT_TYPE_P (TREE_TYPE (rhs1)))
    goto out;

  /* Second match patterns that allow contracting a plus-minus pair
     irrespective of overflow issues.

	(A +- B) - A       ->  +- B
	(A +- B) -+ B      ->  A
	(CST +- A) +- CST  ->  CST +- A
	(A + CST) +- CST   ->  A + CST
	~A + A             ->  -1
	~A + 1             ->  -A 
	A - (A +- B)       ->  -+ B
	A +- (B +- A)      ->  +- B
	CST +- (CST +- A)  ->  CST +- A
	CST +- (A +- CST)  ->  CST +- A
	A + ~A             ->  -1

     via commutating the addition and contracting operations to zero
     by reassociation.  */

  if (TREE_CODE (rhs1) == SSA_NAME)
    {
      gimple def_stmt = SSA_NAME_DEF_STMT (rhs1);
      if (is_gimple_assign (def_stmt) && can_propagate_from (def_stmt))
	{
	  enum tree_code def_code = gimple_assign_rhs_code (def_stmt);
	  if (def_code == PLUS_EXPR
	      || def_code == MINUS_EXPR)
	    {
	      tree def_rhs1 = gimple_assign_rhs1 (def_stmt);
	      tree def_rhs2 = gimple_assign_rhs2 (def_stmt);
	      if (operand_equal_p (def_rhs1, rhs2, 0)
		  && code == MINUS_EXPR)
		{
		  /* (A +- B) - A -> +- B.  */
		  code = ((def_code == PLUS_EXPR)
			  ? TREE_CODE (def_rhs2) : NEGATE_EXPR);
		  rhs1 = def_rhs2;
		  rhs2 = NULL_TREE;
		  gimple_assign_set_rhs_with_ops (gsi, code, rhs1, NULL_TREE);
		  gcc_assert (gsi_stmt (*gsi) == stmt);
		  gimple_set_modified (stmt, true);
		}
	      else if (operand_equal_p (def_rhs2, rhs2, 0)
		       && code != def_code)
		{
		  /* (A +- B) -+ B -> A.  */
		  code = TREE_CODE (def_rhs1);
		  rhs1 = def_rhs1;
		  rhs2 = NULL_TREE;
		  gimple_assign_set_rhs_with_ops (gsi, code, rhs1, NULL_TREE);
		  gcc_assert (gsi_stmt (*gsi) == stmt);
		  gimple_set_modified (stmt, true);
		}
	      else if (TREE_CODE (rhs2) == INTEGER_CST
		       && TREE_CODE (def_rhs1) == INTEGER_CST)
		{
		  /* (CST +- A) +- CST -> CST +- A.  */
		  tree cst = fold_binary (code, TREE_TYPE (rhs1),
					  def_rhs1, rhs2);
		  if (cst && !TREE_OVERFLOW (cst))
		    {
		      code = def_code;
		      gimple_assign_set_rhs_code (stmt, code);
		      rhs1 = cst;
		      gimple_assign_set_rhs1 (stmt, rhs1);
		      rhs2 = def_rhs2;
		      gimple_assign_set_rhs2 (stmt, rhs2);
		      gimple_set_modified (stmt, true);
		    }
		}
	      else if (TREE_CODE (rhs2) == INTEGER_CST
		       && TREE_CODE (def_rhs2) == INTEGER_CST
		       && def_code == PLUS_EXPR)
		{
		  /* (A + CST) +- CST -> A + CST.  */
		  tree cst = fold_binary (code, TREE_TYPE (rhs1),
					  def_rhs2, rhs2);
		  if (cst && !TREE_OVERFLOW (cst))
		    {
		      code = PLUS_EXPR;
		      gimple_assign_set_rhs_code (stmt, code);
		      rhs1 = def_rhs1;
		      gimple_assign_set_rhs1 (stmt, rhs1);
		      rhs2 = cst;
		      gimple_assign_set_rhs2 (stmt, rhs2);
		      gimple_set_modified (stmt, true);
		    }
		}
	    }
	  else if (def_code == BIT_NOT_EXPR
		   && INTEGRAL_TYPE_P (TREE_TYPE (rhs1)))
	    {
	      tree def_rhs1 = gimple_assign_rhs1 (def_stmt);
	      if (code == PLUS_EXPR
		  && operand_equal_p (def_rhs1, rhs2, 0))
		{
		  /* ~A + A -> -1.  */
		  code = INTEGER_CST;
		  rhs1 = build_int_cst_type (TREE_TYPE (rhs2), -1);
		  rhs2 = NULL_TREE;
		  gimple_assign_set_rhs_with_ops (gsi, code, rhs1, NULL_TREE);
		  gcc_assert (gsi_stmt (*gsi) == stmt);
		  gimple_set_modified (stmt, true);
		}
	      else if (code == PLUS_EXPR
		       && integer_onep (rhs1))
		{
		  /* ~A + 1 -> -A.  */
		  code = NEGATE_EXPR;
		  rhs1 = def_rhs1;
		  rhs2 = NULL_TREE;
		  gimple_assign_set_rhs_with_ops (gsi, code, rhs1, NULL_TREE);
		  gcc_assert (gsi_stmt (*gsi) == stmt);
		  gimple_set_modified (stmt, true);
		}
	    }
	}
    }

  if (rhs2 && TREE_CODE (rhs2) == SSA_NAME)
    {
      gimple def_stmt = SSA_NAME_DEF_STMT (rhs2);
      if (is_gimple_assign (def_stmt) && can_propagate_from (def_stmt))
	{
	  enum tree_code def_code = gimple_assign_rhs_code (def_stmt);
	  if (def_code == PLUS_EXPR
	      || def_code == MINUS_EXPR)
	    {
	      tree def_rhs1 = gimple_assign_rhs1 (def_stmt);
	      tree def_rhs2 = gimple_assign_rhs2 (def_stmt);
	      if (operand_equal_p (def_rhs1, rhs1, 0)
		  && code == MINUS_EXPR)
		{
		  /* A - (A +- B) -> -+ B.  */
		  code = ((def_code == PLUS_EXPR)
			  ? NEGATE_EXPR : TREE_CODE (def_rhs2));
		  rhs1 = def_rhs2;
		  rhs2 = NULL_TREE;
		  gimple_assign_set_rhs_with_ops (gsi, code, rhs1, NULL_TREE);
		  gcc_assert (gsi_stmt (*gsi) == stmt);
		  gimple_set_modified (stmt, true);
		}
	      else if (operand_equal_p (def_rhs2, rhs1, 0)
		       && code != def_code)
		{
		  /* A +- (B +- A) -> +- B.  */
		  code = ((code == PLUS_EXPR)
			  ? TREE_CODE (def_rhs1) : NEGATE_EXPR);
		  rhs1 = def_rhs1;
		  rhs2 = NULL_TREE;
		  gimple_assign_set_rhs_with_ops (gsi, code, rhs1, NULL_TREE);
		  gcc_assert (gsi_stmt (*gsi) == stmt);
		  gimple_set_modified (stmt, true);
		}
	      else if (TREE_CODE (rhs1) == INTEGER_CST
		       && TREE_CODE (def_rhs1) == INTEGER_CST)
		{
		  /* CST +- (CST +- A) -> CST +- A.  */
		  tree cst = fold_binary (code, TREE_TYPE (rhs2),
					  rhs1, def_rhs1);
		  if (cst && !TREE_OVERFLOW (cst))
		    {
		      code = (code == def_code ? PLUS_EXPR : MINUS_EXPR);
		      gimple_assign_set_rhs_code (stmt, code);
		      rhs1 = cst;
		      gimple_assign_set_rhs1 (stmt, rhs1);
		      rhs2 = def_rhs2;
		      gimple_assign_set_rhs2 (stmt, rhs2);
		      gimple_set_modified (stmt, true);
		    }
		}
	      else if (TREE_CODE (rhs1) == INTEGER_CST
		       && TREE_CODE (def_rhs2) == INTEGER_CST)
		{
		  /* CST +- (A +- CST) -> CST +- A.  */
		  tree cst = fold_binary (def_code == code
					  ? PLUS_EXPR : MINUS_EXPR,
					  TREE_TYPE (rhs2),
					  rhs1, def_rhs2);
		  if (cst && !TREE_OVERFLOW (cst))
		    {
		      rhs1 = cst;
		      gimple_assign_set_rhs1 (stmt, rhs1);
		      rhs2 = def_rhs1;
		      gimple_assign_set_rhs2 (stmt, rhs2);
		      gimple_set_modified (stmt, true);
		    }
		}
	    }
	  else if (def_code == BIT_NOT_EXPR
		   && INTEGRAL_TYPE_P (TREE_TYPE (rhs2)))
	    {
	      tree def_rhs1 = gimple_assign_rhs1 (def_stmt);
	      if (code == PLUS_EXPR
		  && operand_equal_p (def_rhs1, rhs1, 0))
		{
		  /* A + ~A -> -1.  */
		  code = INTEGER_CST;
		  rhs1 = build_int_cst_type (TREE_TYPE (rhs1), -1);
		  rhs2 = NULL_TREE;
		  gimple_assign_set_rhs_with_ops (gsi, code, rhs1, NULL_TREE);
		  gcc_assert (gsi_stmt (*gsi) == stmt);
		  gimple_set_modified (stmt, true);
		}
	    }
	}
    }

out:
  if (gimple_modified_p (stmt))
    {
      fold_stmt_inplace (gsi);
      update_stmt (stmt);
      if (maybe_clean_or_replace_eh_stmt (stmt, stmt)
	  && gimple_purge_dead_eh_edges (gimple_bb (stmt)))
	return true;
    }

  return false;
}

/* Combine two conversions in a row for the second conversion at *GSI.
   Returns true if there were any changes made.  Else it returns 0.  */
 
static bool
combine_conversions (gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  gimple def_stmt;
  tree op0, lhs;
  enum tree_code code = gimple_assign_rhs_code (stmt);

  gcc_checking_assert (CONVERT_EXPR_CODE_P (code)
		       || code == FLOAT_EXPR
		       || code == FIX_TRUNC_EXPR);

  lhs = gimple_assign_lhs (stmt);
  op0 = gimple_assign_rhs1 (stmt);
  if (useless_type_conversion_p (TREE_TYPE (lhs), TREE_TYPE (op0)))
    {
      gimple_assign_set_rhs_code (stmt, TREE_CODE (op0));
      return true;
    }

  if (TREE_CODE (op0) != SSA_NAME)
    return false;

  def_stmt = SSA_NAME_DEF_STMT (op0);
  if (!is_gimple_assign (def_stmt))
    return false;

  if (CONVERT_EXPR_CODE_P (gimple_assign_rhs_code (def_stmt)))
    {
      tree defop0 = gimple_assign_rhs1 (def_stmt);
      tree type = TREE_TYPE (lhs);
      tree inside_type = TREE_TYPE (defop0);
      tree inter_type = TREE_TYPE (op0);
      int inside_int = INTEGRAL_TYPE_P (inside_type);
      int inside_ptr = POINTER_TYPE_P (inside_type);
      int inside_float = FLOAT_TYPE_P (inside_type);
      int inside_vec = TREE_CODE (inside_type) == VECTOR_TYPE;
      unsigned int inside_prec = TYPE_PRECISION (inside_type);
      int inside_unsignedp = TYPE_UNSIGNED (inside_type);
      int inter_int = INTEGRAL_TYPE_P (inter_type);
      int inter_ptr = POINTER_TYPE_P (inter_type);
      int inter_float = FLOAT_TYPE_P (inter_type);
      int inter_vec = TREE_CODE (inter_type) == VECTOR_TYPE;
      unsigned int inter_prec = TYPE_PRECISION (inter_type);
      int inter_unsignedp = TYPE_UNSIGNED (inter_type);
      int final_int = INTEGRAL_TYPE_P (type);
      int final_ptr = POINTER_TYPE_P (type);
      int final_float = FLOAT_TYPE_P (type);
      int final_vec = TREE_CODE (type) == VECTOR_TYPE;
      unsigned int final_prec = TYPE_PRECISION (type);
      int final_unsignedp = TYPE_UNSIGNED (type);

      /* In addition to the cases of two conversions in a row
	 handled below, if we are converting something to its own
	 type via an object of identical or wider precision, neither
	 conversion is needed.  */
      if (useless_type_conversion_p (type, inside_type)
	  && (((inter_int || inter_ptr) && final_int)
	      || (inter_float && final_float))
	  && inter_prec >= final_prec)
	{
	  gimple_assign_set_rhs1 (stmt, unshare_expr (defop0));
	  gimple_assign_set_rhs_code (stmt, TREE_CODE (defop0));
	  update_stmt (stmt);
	  return true;
	}

      /* Likewise, if the intermediate and initial types are either both
	 float or both integer, we don't need the middle conversion if the
	 former is wider than the latter and doesn't change the signedness
	 (for integers).  Avoid this if the final type is a pointer since
	 then we sometimes need the middle conversion.  Likewise if the
	 final type has a precision not equal to the size of its mode.  */
      if (((inter_int && inside_int)
	   || (inter_float && inside_float)
	   || (inter_vec && inside_vec))
	  && inter_prec >= inside_prec
	  && (inter_float || inter_vec
	      || inter_unsignedp == inside_unsignedp)
	  && ! (final_prec != GET_MODE_BITSIZE (TYPE_MODE (type))
		&& TYPE_MODE (type) == TYPE_MODE (inter_type))
	  && ! final_ptr
	  && (! final_vec || inter_prec == inside_prec))
	{
	  gimple_assign_set_rhs1 (stmt, defop0);
	  update_stmt (stmt);
	  return true;
	}

      /* If we have a sign-extension of a zero-extended value, we can
	 replace that by a single zero-extension.  */
      if (inside_int && inter_int && final_int
	  && inside_prec < inter_prec && inter_prec < final_prec
	  && inside_unsignedp && !inter_unsignedp)
	{
	  gimple_assign_set_rhs1 (stmt, defop0);
	  update_stmt (stmt);
	  return true;
	}

      /* Two conversions in a row are not needed unless:
	 - some conversion is floating-point (overstrict for now), or
	 - some conversion is a vector (overstrict for now), or
	 - the intermediate type is narrower than both initial and
	 final, or
	 - the intermediate type and innermost type differ in signedness,
	 and the outermost type is wider than the intermediate, or
	 - the initial type is a pointer type and the precisions of the
	 intermediate and final types differ, or
	 - the final type is a pointer type and the precisions of the
	 initial and intermediate types differ.  */
      if (! inside_float && ! inter_float && ! final_float
	  && ! inside_vec && ! inter_vec && ! final_vec
	  && (inter_prec >= inside_prec || inter_prec >= final_prec)
	  && ! (inside_int && inter_int
		&& inter_unsignedp != inside_unsignedp
		&& inter_prec < final_prec)
	  && ((inter_unsignedp && inter_prec > inside_prec)
	      == (final_unsignedp && final_prec > inter_prec))
	  && ! (inside_ptr && inter_prec != final_prec)
	  && ! (final_ptr && inside_prec != inter_prec)
	  && ! (final_prec != GET_MODE_BITSIZE (TYPE_MODE (type))
		&& TYPE_MODE (type) == TYPE_MODE (inter_type)))
	{
	  gimple_assign_set_rhs1 (stmt, defop0);
	  update_stmt (stmt);
	  return true;
	}

      /* A truncation to an unsigned type should be canonicalized as
	 bitwise and of a mask.  */
      if (final_int && inter_int && inside_int
	  && final_prec == inside_prec
	  && final_prec > inter_prec
	  && inter_unsignedp)
	{
	  tree tem;
	  tem = fold_build2 (BIT_AND_EXPR, inside_type,
			     defop0,
			     double_int_to_tree
			       (inside_type, double_int_mask (inter_prec)));
	  if (!useless_type_conversion_p (type, inside_type))
	    {
	      tem = force_gimple_operand_gsi (gsi, tem, true, NULL_TREE, true,
					      GSI_SAME_STMT);
	      gimple_assign_set_rhs1 (stmt, tem);
	    }
	  else
	    gimple_assign_set_rhs_from_tree (gsi, tem);
	  update_stmt (gsi_stmt (*gsi));
	  return true;
	}
    }

  return false;
}

/* Main entry point for the forward propagation and statement combine
   optimizer.  */

bool
ssa_combine (gimple_stmt_iterator *gsi, nonzerobits_t nonzerobits_p)
{
  bool changed = false;
  tree newexpr = NULL_TREE;

  gimple stmt = gsi_stmt (*gsi);

  switch (gimple_code (stmt))
    {
    case GIMPLE_ASSIGN:
      {
	tree rhs1 = gimple_assign_rhs1 (stmt);
	enum tree_code code = gimple_assign_rhs_code (stmt);

	if ((code == BIT_NOT_EXPR
	     || code == NEGATE_EXPR)
	    && TREE_CODE (rhs1) == SSA_NAME)
	  changed = simplify_not_neg_expr (gsi);
	else if (code == COND_EXPR)
	 /* In this case the entire COND_EXPR is in rhs1. */
	 changed = forward_propagate_into_cond (gsi);
	else if (TREE_CODE_CLASS (code) == tcc_comparison)
	  changed = forward_propagate_into_comparison (gsi);
	else if (code == BIT_AND_EXPR
		 || code == BIT_IOR_EXPR
		 || code == BIT_XOR_EXPR)
	  {
	    newexpr = simplify_bitwise_binary (gsi, nonzerobits_p);
	  }
	else if (code == PLUS_EXPR
		 || code == MINUS_EXPR)
	  changed = associate_plusminus (gsi);
	else if (CONVERT_EXPR_CODE_P (code)
		 || code == FLOAT_EXPR
		 || code == FIX_TRUNC_EXPR)
	  changed = combine_conversions (gsi);
	break;
      }

    case GIMPLE_SWITCH:
      changed = simplify_gimple_switch (stmt);
      break;

    case GIMPLE_COND:
      changed = forward_propagate_into_gimple_cond (gsi, stmt);
      break;

    case GIMPLE_CALL:
      {
	tree callee = gimple_call_fndecl (stmt);
	if (callee != NULL_TREE
	    && DECL_BUILT_IN_CLASS (callee) == BUILT_IN_NORMAL)
	  changed = simplify_builtin_call (gsi, callee);
	break;
      }

    default:;
    }

  if (changed)
    return true;
  if (!newexpr)
    return false;
  /* For right now only handle the assign case. */
  gcc_assert (gimple_code (stmt) == GIMPLE_ASSIGN);
  /* Handle the simple case of folding to one expression. */
  newexpr = extract_simple_gimple (gimple_location (stmt), gsi, newexpr);
  if (newexpr == NULL_TREE)
    return false;
  gimple_assign_set_rhs_from_tree (gsi, newexpr);
  update_stmt (gsi_stmt (*gsi));
  return true;
}


