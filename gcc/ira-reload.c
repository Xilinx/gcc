/* Integrated Register Allocator (IRA) reloading .
   Copyright (C) 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "regs.h"
#include "rtl.h"
#include "tm_p.h"
#include "target.h"
#include "flags.h"
#include "obstack.h"
#include "bitmap.h"
#include "hard-reg-set.h"
#include "basic-block.h"
#include "expr.h"
#include "recog.h"
#include "params.h"
#include "timevar.h"
#include "tree-pass.h"
#include "output.h"
#include "except.h"
#include "reload.h"
#include "errors.h"
#include "integrate.h"
#include "df.h"
#include "ggc.h"
#include "ira-int.h"


static bitmap pseudos_to_localize;
static bitmap regs_to_load;
static bitmap regs_to_store;
static int *pseudo_nuses;
static int *pseudo_nsets;
static rtx *reg_map;

/* Return true if REG is a pseudo which should be localized, return false
   otherwise.  */

static bool
localize_pseudo_p (unsigned int regno) 
{
  /* If this pseudo got a hard register, then we obviously do not want to
     localize it.  */
  if (reg_renumber [regno] != -1)
    return false;

  /* Avoid localizing a pseudo which can be rematerialized.
     ?!? I think this is a holdover from ancient code and may no longer
     be necessary.  */
  if ((reg_equiv_constant && reg_equiv_constant[regno])
      || (reg_equiv_invariant && reg_equiv_invariant[regno]))
    return false;

  /* If we don't know what register class to use for the psuedo, then
     we don't try to localize it.
     ?!? ISTM we should go ahead and localize as the localized pseudo
     should be easier for reload to handle.  */
  if (reg_preferred_class (regno) == NO_REGS)
    return false;

  /* If the pseudo is already local to a block, then there's no point
     in localizing it.  */
  if (REG_BASIC_BLOCK (regno) != REG_BLOCK_GLOBAL
      && REG_BASIC_BLOCK (regno) != REG_BLOCK_UNKNOWN)
    return false;

  return true;
}


/* Alter each pseudo-reg rtx to contain its hard reg number.  Assign
   stack slots to the pseudos that lack hard regs or equivalents.
   Do not touch virtual registers. 

   ?!? This assigns for local pseudos too, which might be wasteful
   as we get another chance to allocate those.  */
static void
assign_stack_slots (void)
{
  unsigned i, n, max_regno = max_reg_num ();
  int *temp_pseudo_reg_arr;

  /* ?!? This should be cleaned up or go away.  */
  ira_spilled_reg_stack_slots_num = 0;


  temp_pseudo_reg_arr = XNEWVEC (int, max_regno - LAST_VIRTUAL_REGISTER - 1);
  n = 0;
  for (n = 0, i = LAST_VIRTUAL_REGISTER + 1; i < max_regno; i++)
    temp_pseudo_reg_arr[n++] = i;
  
  /* Ask IRA to order pseudo-registers for better stack slot
     sharing.  */
  ira_sort_regnos_for_alter_reg (temp_pseudo_reg_arr, n, reg_max_ref_width);

  for (i = 0; i < n; i++)
    alter_reg (temp_pseudo_reg_arr[i], -1, false);

  free (temp_pseudo_reg_arr);
}


/* Count uses of USE (into pseudo_nuses) if USE is a register marked for
   localization.  */

static void
identify_singleton_uses (rtx use)
{
  if (GET_CODE (use) == SUBREG)
    use = SUBREG_REG (use);

  if (GET_CODE (use) != REG)
    return;

  if (bitmap_bit_p (pseudos_to_localize, REGNO (use)))
    pseudo_nuses[REGNO (use)]++;
}

/* Count assignments to DEST (into pseudo_nsets) if DEST is a register marked
   for localization.  */

static void
identify_singleton_sets (rtx dest)
{
  if (GET_CODE (dest) == SUBREG)
    dest = SUBREG_REG (dest);

  if (GET_CODE (dest) != REG)
    return;

  /* If DEST isn't maked for spilling, then there is nothing to do.  */
  if (bitmap_bit_p (pseudos_to_localize, REGNO (dest)))
    pseudo_nsets[REGNO (dest)]++;
}

/* Collect (into REGS_TO_LOAD) USE if it is a pseudo marked for
   localization.  */

static void
collect_loads (rtx use)
{
  if (GET_CODE (use) == SUBREG)
    use = SUBREG_REG (use);

  if (GET_CODE (use) != REG)
    return;

  if (bitmap_bit_p (pseudos_to_localize, REGNO (use)))
    bitmap_set_bit (regs_to_load, REGNO (use));

  return;
}

/* If USE refers to a pseudo marked in REGS_TO_LOAD, emit a load of
   the pseudo before INSN.  */

static int
emit_localizing_loads (rtx use, rtx insn)
{
  if (GET_CODE (use) == SUBREG)
    use = SUBREG_REG (use);

  if (GET_CODE (use) != REG)
    return 0 ;

  if (bitmap_bit_p (pseudos_to_localize, REGNO (use)))
    {
      /* Create a new pseudo and record it in our map.  */
      if (reg_map [(REGNO (use))] == NULL)
	reg_map [REGNO (use)] = gen_reg_rtx (GET_MODE (use));


      /* If this pseudo still needs a load, emit it.  */
      if (bitmap_bit_p (regs_to_load, REGNO (use)))
	{
	  rtx insns, temp;
	  rtx mem = copy_rtx (reg_equiv_memory_loc[REGNO (use)]);
	  int nuses = pseudo_nuses[REGNO (use)];
	  int nsets = pseudo_nsets[REGNO (use)];
	  int occurrences = count_occurrences (PATTERN (insn), use, 0);

	  /* validate_replace_rtx internally calls df_insn_rescan, which is
	     unsafe as our caller is iterating over the existing DF info.  So
	     we have to turn off insn rescanning temporarily.  */
	  df_set_flags (DF_NO_INSN_RESCAN);

	  /* If this insn has all the uses of a pseudo we want to localize
	     and the pseudo is never set, then try to replace the pseudo
	     with its equivalent memory location.  */
	  if (nsets == 0
	      && (occurrences == nuses || nuses == 2)
	      && validate_replace_rtx (use, mem, insn))
	    {
	      df_clear_flags (DF_NO_INSN_RESCAN);
	    }
	  else
	    {
	      df_clear_flags (DF_NO_INSN_RESCAN);
	      start_sequence ();
	      emit_move_insn (reg_map [REGNO (use)], mem);
	      insns = get_insns();
	      end_sequence ();
	      emit_insn_before (insns, insn);

	      /* Inform the DF framework about the new insns.  */
	      for (temp = insns; temp != insn; temp = NEXT_INSN (insns))
	        df_insn_rescan (temp);

	      /* Note it is no longer necessary to load this pseudo.  */
	      bitmap_clear_bit (regs_to_load, REGNO (use));
	    }
	}

      /* Replace the original pseudo with the new one.  */
      replace_rtx (insn, use, reg_map [REGNO (use)]);
      return 1;
    }
  return 0;
}

/* DEST is an output for INSN (passed in DATA).  If the output is marked
   for localizing, then we need to rename it to the new block-local
   pseudo.  This finishes the localization of unallocated globals.  */

static int
rename_sets (rtx dest, rtx insn)
{
  rtx orig = dest;

  if (GET_CODE (dest) == SUBREG)
    dest = SUBREG_REG (dest);

  if (GET_CODE (dest) != REG)
    return 0;

  /* If DEST isn't maked for spilling, then there is nothing to do.  */
  if (! bitmap_bit_p (pseudos_to_localize, REGNO (dest)))
    return 0;

  /* A store can be viewed as a store followed by a load, so we can clear DEST
     from REGS_TO_LOAD, but not if this was a partial store.  */
  if (GET_CODE (orig) == STRICT_LOW_PART)
    {
      /* This must be treated as a USE too.
         ?!? Does this need to integrated with the use processing?  */
      emit_localizing_loads (dest, insn);
    }
  else if (GET_CODE (orig) == SUBREG
	   && (GET_MODE_SIZE (GET_MODE (orig)))
	       < GET_MODE_SIZE (GET_MODE (dest)))
    {
      /* This must be treated as a USE too.
         ?!? Does this need to integrated with the use processing?  */
      emit_localizing_loads (dest, insn);
    }

  /* ?!? I'm not entirely sure this can still happen.  */
  if (reg_map [(REGNO (dest))] == NULL)
    reg_map [REGNO (dest)] = gen_reg_rtx (GET_MODE (dest));


  replace_rtx (insn, dest, reg_map [REGNO (dest)]);
  return 1;
}

/* Store each pseudo set by the current insn (passed in DATA) that is
   marked for localizing into memory after INSN.  */

static int
emit_localizing_stores (rtx dest, rtx insn)
{
  unsigned int regno;
  int retval = 0;
  rtx insns;
  rtx orig = dest;

  if (GET_CODE (dest) == SUBREG)
    dest = SUBREG_REG (dest);

  /* If the output isn't a register, then there's nothing to do.  */
  if (GET_CODE (dest) != REG)
    return retval;

  regno = REGNO (dest);

  /* If the register isn't marked for localization, then there's nothing
     to do.  */
  if (! bitmap_bit_p (pseudos_to_localize, regno))
    return retval;

  /* DEST is marked for spilling, if we have not emitted a spill store yet for
     DEST, then do so now.  Note we do not change INSN at this time.  */
  if (bitmap_bit_p (regs_to_store, regno))
    {
      int nuses = pseudo_nuses[REGNO (dest)];
      int nsets = pseudo_nsets[REGNO (dest)];
      int occurrences = count_occurrences (PATTERN (insn), dest, 0);

      /* We must copy the memory location to avoid incorrect RTL sharing.  */
      rtx mem = copy_rtx (reg_equiv_memory_loc[regno]);

      /* Note that we have stored this register so that we don't try to
         store it again.  */
      bitmap_clear_bit (regs_to_store, regno);

      /* validate_replace_rtx internally calls df_insn_rescan, which is
	 unsafe as our caller is iterating over the existing DF info.  So
	 we have to turn off insn rescanning temporarily.  */
      df_set_flags (DF_NO_INSN_RESCAN);
      /* If this insn both uses and sets a pseudo we want to localize and
	 contains all the uses and sets, then try to replace the pseudo
	 with its equivalent memory location.  */
      if (nuses
	  && nsets == 1
	  && occurrences == nuses
	  && validate_replace_rtx (dest, mem, insn))
	{
	  df_clear_flags (DF_NO_INSN_RESCAN);
	  retval = 1;
	}
      /* Similarly if this insn sets a pseudo we want to localize and
	 there are no uses, then try to replace the pseudo with its
	 equivalent memory location.  */
      else if (nuses == 0
	  && nsets == 1
	  && validate_replace_rtx (dest, mem, insn))
	{
	  df_clear_flags (DF_NO_INSN_RESCAN);
	  retval = 1;
	}
      else
	{
	  df_clear_flags (DF_NO_INSN_RESCAN);
          start_sequence ();
          emit_move_insn (mem, dest);
          insns = get_insns();
          end_sequence ();

          /* If the pseudo is being set from its equivalent memory location,
             then we don't need to store it back.  */
          if (NEXT_INSN (insns) == 0
              && single_set (insns)
              && single_set (insn)
              && rtx_equal_p (SET_SRC (single_set (insn)),
			  SET_DEST (single_set (insns))))
	    ;
          else
	    {
	      rtx temp;
	      /* Inform the DF framework about the new insns.  */
	      for (temp = insns; temp; temp = NEXT_INSN (temp))
	        df_insn_rescan (temp);

	      emit_insn_after_noloc (insns, insn, NULL);
	    }
	}
    }

  /* A store can be viewed as a store followed by a load, so we can clear DEST
     from REGS_TO_LOAD, but not if this was a partial store.  */
  if (GET_CODE (orig) == STRICT_LOW_PART)
    bitmap_set_bit (regs_to_load, regno);
  else if (GET_CODE (orig) == SUBREG
	   && (GET_MODE_SIZE (GET_MODE (orig))
		< GET_MODE_SIZE (GET_MODE (dest))))
    bitmap_set_bit (regs_to_load, regno);
  else
    bitmap_clear_bit (regs_to_load, REGNO (dest));
  return retval;
}

static void
create_new_allocno_for_spilling (int nreg, int oreg)
{
  ira_allocno_t to, from, a;
  ira_allocno_iterator ai;
  ira_allocno_conflict_iterator aci;
  unsigned int conflicts;
  allocno_live_range_t prev, range, r;

  /* Update IRA's datastructures.  */

  /* First create the allocno.  */
  to = ira_create_allocno (nreg, true, ira_curr_loop_tree_node);
  ALLOCNO_REG (to) = regno_reg_rtx[nreg];

  /* Now lengthen the regno->allocno map.  */
  ira_free (ira_regno_allocno_map);
  ira_regno_allocno_map
    = (ira_allocno_t *) ira_allocate (max_reg_num () * sizeof (ira_allocno_t));
  memset (ira_regno_allocno_map, 0, max_reg_num () * sizeof (ira_allocno_t));
  FOR_EACH_ALLOCNO (a, ai)
    {
      unsigned int regno = ALLOCNO_REGNO (a);
      ira_regno_allocno_map[regno] = a;
    }


  /* Copy various fields from the original allocno to the new one.  */
  from = ira_regno_allocno_map [oreg];
#ifdef STACK_REGS
  ALLOCNO_NO_STACK_REG_P (to) = ALLOCNO_NO_STACK_REG_P (from);
  ALLOCNO_TOTAL_NO_STACK_REG_P (to) = ALLOCNO_TOTAL_NO_STACK_REG_P (from);
#endif
  ALLOCNO_NREFS (to) = ALLOCNO_NREFS (from);
  ALLOCNO_FREQ (to) = ALLOCNO_FREQ (from);
  ALLOCNO_CALL_FREQ (to) = ALLOCNO_CALL_FREQ (from);
  ALLOCNO_CALLS_CROSSED_NUM (to) = ALLOCNO_CALLS_CROSSED_NUM (from);
  ALLOCNO_EXCESS_PRESSURE_POINTS_NUM (to)
    = ALLOCNO_EXCESS_PRESSURE_POINTS_NUM (from);
  ALLOCNO_BAD_SPILL_P (to) = ALLOCNO_BAD_SPILL_P (from);

  ALLOCNO_COVER_CLASS (to) = ALLOCNO_COVER_CLASS (from);
  ALLOCNO_COVER_CLASS_COST (to) = ALLOCNO_COVER_CLASS_COST (from);
  ALLOCNO_MEMORY_COST (to) = ALLOCNO_MEMORY_COST (from);
#if 0
  ira_allocate_and_accumulate_costs (&ALLOCNO_HARD_REG_COSTS (to),
				     ALLOCNO_COVER_CLASS (to),
				     ALLOCNO_HARD_REG_COSTS (from));
  ira_allocate_and_accumulate_costs (&ALLOCNO_CONFLICT_HARD_REG_COSTS (to),
				     ALLOCNO_COVER_CLASS (to),
				     ALLOCNO_CONFLICT_HARD_REG_COSTS (from));
#endif

  /* We recompute these fields after we have localized an entire block.  */
  CLEAR_HARD_REG_SET (ALLOCNO_CONFLICT_HARD_REGS (to));
  CLEAR_HARD_REG_SET (ALLOCNO_TOTAL_CONFLICT_HARD_REGS (to));

  /* Count the number of conflicts on the original allocno.  We use that count
     as an estimate for the number of conflicts in the new allocno.  The new
     allocno should have fewer conflicts than the original as the new allocno
     is only live in BB and thus only conflicts with objects live in BB.  */
  conflicts = 0;
  FOR_EACH_ALLOCNO_CONFLICT (from, a, aci)
    conflicts++;

  ALLOCNO_MIN (to) = ALLOCNO_MIN (from);
  ALLOCNO_MAX (to) = ALLOCNO_MAX (from);
  ira_allocate_allocno_conflicts (to, conflicts);

  /* For now we copy the live range from the original allocno to the new
     allocno.  This is very suboptimal.  Consider if we have some allocno A
     which gets split into A0..A100.  A0..A100 will not be able to share
     stack slots because the live ranges conflict (they were copied from A
     verbatim) -- however, in reality each new allocno A0..A100 has a
     distinct, non-conflicting live range.  */
  for (prev = NULL, r = ALLOCNO_LIVE_RANGES (from);
       r != NULL;
       r = r->next, prev = range)
    {
      range = ira_create_allocno_live_range (to, r->start, r->finish, NULL);
      /* ?!? This may not be necessary.  */
      range->start_next = NULL;
      range->finish_next = NULL;
      if (prev)
	prev->next = range;
      else
	ALLOCNO_LIVE_RANGES (to) = range;
    }
}

static bitmap live;

static void
maybe_add_conflict (int reg1, int reg2, int limit)
{
  if (reg1 < FIRST_PSEUDO_REGISTER
      && reg2 < FIRST_PSEUDO_REGISTER)
    return;

  /* If neither register was created by localization, then ignore this
     conflict.  */
  if (reg1 < limit && reg2 < limit)
    return;

  if (reg1 < FIRST_PSEUDO_REGISTER)
    {
      ira_allocno_t a = ira_regno_allocno_map[reg2];
      SET_HARD_REG_BIT (ALLOCNO_TOTAL_CONFLICT_HARD_REGS (a), reg1);
      SET_HARD_REG_BIT (ALLOCNO_CONFLICT_HARD_REGS (a), reg1);
      return;
    }

  if (reg2 < FIRST_PSEUDO_REGISTER)
    {
      ira_allocno_t a = ira_regno_allocno_map[reg1];
      SET_HARD_REG_BIT (ALLOCNO_TOTAL_CONFLICT_HARD_REGS (a), reg2);
      SET_HARD_REG_BIT (ALLOCNO_CONFLICT_HARD_REGS (a), reg2);
      return;
    }

  /* If the registers are in different cover classes, then ignore this
     conflict.  */
  if (ira_class_translate[reg_preferred_class (reg1)]
      != ira_class_translate[reg_preferred_class (reg2)])
    return;

  ira_add_allocno_conflict (ira_regno_allocno_map[reg1],
			    ira_regno_allocno_map[reg2]);
}

static void
mark_conflicts (rtx reg, unsigned int limit)
{
  bitmap_iterator bi;
  unsigned int i;
  unsigned int j, nregs;
  unsigned int non_killing_store = 0;

  if (GET_CODE (reg) == SUBREG
      && GET_CODE (SUBREG_REG (reg)) == REG)
    {
      if (GET_MODE_SIZE (GET_MODE (reg))
	  < GET_MODE_SIZE (GET_MODE (SUBREG_REG (reg))))
	non_killing_store = 1;

      reg = SUBREG_REG (reg);
    }

  if (GET_CODE (reg) != REG)
    return;

  if (REGNO (reg) >= FIRST_PSEUDO_REGISTER)
    nregs = 1;
  else
    nregs = HARD_REGNO_NREGS (REGNO (reg), GET_MODE (reg));

  for (j = 0; j < nregs; j++)
    {
      if (!non_killing_store)
	{
	  bitmap_clear_bit (live, REGNO (reg) + j);
	}
      EXECUTE_IF_SET_IN_BITMAP (live, 0, i, bi)
	maybe_add_conflict (i, REGNO (reg) + j, limit);
    }
}

static void
mark_live (rtx reg)
{
  int i, nregs;

  if (GET_CODE (reg) == SUBREG)
    reg = SUBREG_REG (reg);
  if (REGNO (reg) > FIRST_PSEUDO_REGISTER)
    nregs = 1;
  else
    nregs = HARD_REGNO_NREGS (REGNO (reg), GET_MODE (reg));

  for (i = 0; i < nregs; i++)
    bitmap_set_bit (live, REGNO (reg));
}

static void
build_conflicts_for_new_allocnos (basic_block bb,
				  bitmap pseudos_to_localize,
				  int orig_max_reg_num)
{
  rtx insn;


  live = BITMAP_ALLOC (NULL);

  /* Starting state is the live-out pseudos - pseudos we're localizing.  */
  bitmap_and_compl (live, DF_LIVE_OUT (bb), pseudos_to_localize);

  FOR_BB_INSNS_REVERSE (bb, insn)
    {
      df_ref *def_rec, *use_rec;
      int call_p;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      call_p = CALL_P (insn);
      /* Mark conflicts for any values defined in this insn.  */
      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	if (!call_p || !DF_REF_FLAGS_IS_SET (*def_rec, DF_REF_MAY_CLOBBER))
	  mark_conflicts (DF_REF_REG (*def_rec), orig_max_reg_num);

      /* Mark each used value as live.  */
      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	mark_live (DF_REF_REG (*use_rec));
    }

  BITMAP_FREE (live);
}

/* Emit trivial spill code for unallocated pseudos which are live at one or
   more basic block boundaries appearing in BB.

   Effectively we're splitting the range of these pseudos in such a way as
   the new pseudos are live within a single basic block by adding a store
   after the last assignment to the pseudo and a load before the first use
   within BB.

   ?!? We might be able to use extended basic blocks to avoid additional loads
   and stores.  Definitely worth some experimentation.  */

static void
localize_pseudos (basic_block bb, bitmap pseudos_to_localize)
{
  int orig_max_reg_num = max_reg_num ();
  int i;
  rtx insn;

  regs_to_store = BITMAP_ALLOC (NULL);
  regs_to_load = BITMAP_ALLOC (NULL);
  pseudo_nuses = (int *) xmalloc (max_reg_num () * sizeof (int));
  memset (pseudo_nuses, 0, max_reg_num () * sizeof (int));
  pseudo_nsets = (int *) xmalloc (max_reg_num () * sizeof (int));
  memset (pseudo_nsets, 0, max_reg_num () * sizeof (int));

  reg_map = (rtx *) xmalloc (sizeof (rtx) * orig_max_reg_num);
  memset (reg_map, 0, sizeof (rtx) * orig_max_reg_num);
   
  bitmap_copy (regs_to_store, pseudos_to_localize);

  /* First walk over the insns in this region and identify singleton
     uses of registers in PSEUDOS_TO_LOCALIZE.  We want to know if a
     use is a singleton so that we can change the use to a MEM.  We
     need this information prior to emitting localizing stores so that
     we can change both the use and set in a single insn to a MEM.  */
  FOR_BB_INSNS_REVERSE (bb, insn)
    {
      df_ref *def_rec, *use_rec;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	identify_singleton_uses (DF_REF_REG (*use_rec));

      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	identify_singleton_sets (DF_REF_REG (*def_rec));

    }

  /* Next emit a store after the last assignment of each pseudo in
     PSEUDOS_TO_LOCALIZE within the region.  Collect list of pseudos
     we'll need to load as well.  */
  FOR_BB_INSNS_REVERSE (bb, insn)
    {
      df_ref *def_rec, *use_rec;
      int need_rescan;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      need_rescan = 0;
      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	need_rescan |= emit_localizing_stores (DF_REF_REG (*def_rec), insn);

      /* It is not safe to defer scanning any further as emit_localizing_stores
	 can change uses and defs.  */
      if (need_rescan)
	df_insn_rescan (insn);

      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	collect_loads (DF_REF_REG (*use_rec));
    }

  /* Now walk forward through the region emitting loads before
     the first use of each pseudo that we're localizing and change
     each reference from an unallocated pseudo to a new block local
     spill register.  */
  FOR_BB_INSNS (bb, insn)
    {
      df_ref *def_rec, *use_rec;
      int need_rescan;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      need_rescan = 0;
      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	need_rescan |= emit_localizing_loads (DF_REF_REG (*use_rec), insn);

      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	need_rescan |= rename_sets (DF_REF_REG (*def_rec), insn);

      if (need_rescan)
	df_insn_rescan (insn);

    }

  /* If we allocated new pseudos, then we need to expand various arrays and
     update IRA's data structures.  */
  if (orig_max_reg_num != max_reg_num ())
    {
      unsigned int max_regno = max_reg_num ();
      unsigned int nregs = max_regno - orig_max_reg_num;

      /* First expand various data structures.  */
      regstat_reallocate_ri (max_regno);
      expand_reg_info (max_regno - 1);
      reg_equiv_invariant = (rtx *) xrealloc (reg_equiv_invariant,
					      max_regno * sizeof (rtx));
      memset (&reg_equiv_invariant[orig_max_reg_num], 0, nregs * sizeof (rtx));
      reg_equiv_constant = (rtx *) xrealloc (reg_equiv_constant,
					     max_regno * sizeof (rtx));
      memset (&reg_equiv_constant[orig_max_reg_num], 0, nregs * sizeof (rtx));
      reg_equiv_mem = (rtx *) xrealloc (reg_equiv_mem, max_regno * sizeof (rtx));
      memset (&reg_equiv_mem[orig_max_reg_num], 0, nregs * sizeof (rtx));
      reg_equiv_alt_mem_list = (rtx *) xrealloc (reg_equiv_alt_mem_list,
					         max_regno * sizeof (rtx));
      memset (&reg_equiv_alt_mem_list[orig_max_reg_num], 0, nregs * sizeof (rtx));
      reg_equiv_address = (rtx *) xrealloc (reg_equiv_address,
					    max_regno * sizeof (rtx));
      memset (&reg_equiv_address[orig_max_reg_num], 0, nregs * sizeof (rtx));
      VEC_safe_grow_cleared (rtx, gc, reg_equiv_memory_loc_vec, max_regno);
      reg_equiv_memory_loc = VEC_address (rtx, reg_equiv_memory_loc_vec);
      reg_equiv_init
	= (rtx *) ggc_realloc (reg_equiv_init, max_regno * sizeof (rtx));
      memset (&reg_equiv_init[orig_max_reg_num], 0, nregs * sizeof (rtx));
      regstat_n_sets_and_refs
	 = ((struct regstat_n_sets_and_refs_t *)
	    xrealloc (regstat_n_sets_and_refs,
		      (max_regno * sizeof (struct regstat_n_sets_and_refs_t))));
      memset (&regstat_n_sets_and_refs[orig_max_reg_num], 0,
	      nregs * sizeof (struct regstat_n_sets_and_refs_t));
      reg_max_ref_width = (unsigned int *) xrealloc (reg_max_ref_width,
						     max_regno * sizeof (unsigned int));
      memset (&reg_max_ref_width[orig_max_reg_num], 0,
	      nregs * sizeof (unsigned int));

      /* Now copy data from the original register to the new register.  */
      for (i = 0; i < orig_max_reg_num; i++)
	{
	  int nregno;

	  if (reg_map[i] == 0)
	    continue;

	  nregno = REGNO (reg_map[i]);
	  setup_reg_classes (nregno, reg_preferred_class (i), reg_alternate_class (i), ira_class_translate [reg_preferred_class (i)]);
	  reg_equiv_invariant[nregno] = reg_equiv_invariant[i];
	  reg_equiv_constant[nregno] = reg_equiv_constant[i];
	  reg_equiv_mem[nregno] = reg_equiv_mem[i];
	  reg_equiv_alt_mem_list[nregno] = reg_equiv_alt_mem_list[i];
	  reg_equiv_address[nregno] = reg_equiv_address[i];
	  reg_equiv_memory_loc[nregno] = reg_equiv_memory_loc[i];
	  /* ?!? I don't recall why this was originally necessary.  Definitely need
	     to retest and understand or make it go away.  */
          reg_equiv_init[i] = NULL;
	  reg_equiv_init[nregno] = reg_equiv_init[i];
	  reg_max_ref_width[nregno] = reg_max_ref_width[i];
	  reg_renumber[nregno] = reg_renumber[i];
	  REG_N_CALLS_CROSSED (nregno) = REG_N_CALLS_CROSSED (i);

	  /* We don't really care other than to be sure there's a set and ref.  */
	  SET_REG_N_SETS (nregno, 1);
	  SET_REG_N_REFS (nregno, 1);

	  /* The new register is always local to this block.  */
	  REG_BASIC_BLOCK (nregno) = REG_BLOCK_GLOBAL;

	  /* Create a new allocno for the new register.  */
	  create_new_allocno_for_spilling (nregno, i);
	}

      /* Now reallocate a few IRA arrays.  */
      ira_finish_assign ();
      ira_initiate_assign ();

      /* Fill in the sorted_allocnos and priority arrays.  */
      {
	ira_allocno_t a;
        ira_allocno_iterator ai;
	unsigned int num = 0;
	extern ira_allocno_t *sorted_allocnos;

        FOR_EACH_ALLOCNO (a, ai)
	  sorted_allocnos[num++] = a;
        setup_allocno_priorities (sorted_allocnos, num);
      }

      /* We didn't copy the conflicts from the old allocno to the new allocno
	 as typically the new allocno will have fewer conflicts.

	 We wait until after we've created all the new allocnos for this block
	 so that we can update the conflict graph with a single backwards walk
	 through this block.  */
      build_conflicts_for_new_allocnos (bb, pseudos_to_localize, orig_max_reg_num);

      /* We added new live-range objects, so rebuild the chains.  */
      ira_rebuild_start_finish_chains ();
    }

  free (reg_map);
  reg_map = NULL;
  BITMAP_FREE (regs_to_store);
  regs_to_store = NULL;
  BITMAP_FREE (regs_to_load);
  regs_to_load = NULL;
  free (pseudo_nuses);
  pseudo_nuses = NULL;
  free (pseudo_nsets);
  pseudo_nsets = NULL;
}

void
ira_reload (void)
{
  /* We need to build the various equivalences prior to assigning stack
     slots for unallocated global pseudos.  */
  record_equivalences_for_reload ();

  if (ira_conflicts_p)
    {
      unsigned int i, j;
      bitmap_iterator bi;
      basic_block bb;

      pseudos_to_localize = BITMAP_ALLOC (NULL);
      max_regno = max_reg_num ();
      /* Collect all the registers we want to localize into a bitmap.  */
      for (j = FIRST_PSEUDO_REGISTER; j < (unsigned) max_regno; j++)
	if (localize_pseudo_p (j))
	  bitmap_set_bit (pseudos_to_localize, j);

      /* Assign stack slots for pseudos live at block boundaries which did not
         get hard regs.  This unfortunately turns pseudos into hard regs which
         we will need to undo later.  */
      assign_stack_slots ();

      for (i = FIRST_PSEUDO_REGISTER; i < (unsigned) max_regno; i++)
	if (regno_reg_rtx[i])
	  SET_REGNO (regno_reg_rtx[i], i);

      if (!bitmap_empty_p (pseudos_to_localize))
	{
          FOR_EACH_BB (bb)
	    localize_pseudos (bb, pseudos_to_localize);
	}

      /* Now we want to remove each allocnos associated with the pseudos we
	 localized from the conflicts of every other allocno.  Do this once
	 after localizing in all blocks rather than in each block.  */
      EXECUTE_IF_SET_IN_BITMAP (pseudos_to_localize,
				FIRST_PSEUDO_REGISTER, i, bi)
	remove_from_all_conflicts (ira_regno_allocno_map[i]);

      /* We may have allocated additional pseudos during spilling, so update
         max_regno.  ?!? Updating max_regno should really occur when we
         allocate new regs.  Or better yet, max it go away completely.  */
      max_regno = max_reg_num ();

      /* Try to assign hard regs to pseudos that didn't get them the
         first time through the allocator.  */
      {
	unsigned int i, n;
	int *x = (int *) xmalloc (max_regno * sizeof (int));
	HARD_REG_SET bad_spill_regs ;
	HARD_REG_SET *p = XNEWVEC (HARD_REG_SET, max_regno);
	regset_head z;


        if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
	  fprintf (ira_dump_file, "Reassigning after localization\n");
	
	INIT_REG_SET (&z);
        CLEAR_REG_SET (&z);
        COPY_HARD_REG_SET (bad_spill_regs, fixed_reg_set);
	memset (x, 0, max_regno * sizeof (int));
        for (n = 0, i = LAST_VIRTUAL_REGISTER + 1; i < (unsigned) max_regno; i++)
	  {
	    CLEAR_HARD_REG_SET (p[i]);
	    if (reg_renumber[i] == -1 && ira_regno_allocno_map[i] && !bitmap_bit_p (pseudos_to_localize, i))
	      x[n++] = i;
	  }

	ira_reassign_pseudos (x, n, bad_spill_regs, p, p, &z);

        if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
	  fprintf (ira_dump_file, "Done reassigning after localization\n");
      }

      BITMAP_FREE (pseudos_to_localize);


      /* Spill code insertion can force creation of new basic blocks.  */
      fixup_abnormal_edges ();

      for (i = FIRST_PSEUDO_REGISTER; i < (unsigned) max_regno; i++)
	if (regno_reg_rtx[i])
	  SET_REGNO (regno_reg_rtx[i], i);

      /* Finally, reset the DF analyzer as we have added new blocks, new
	 insns, modified existing insns, etc etc.  */
      df_scan_alloc (NULL);
      df_scan_blocks ();
      df_analyze ();
    }
}
