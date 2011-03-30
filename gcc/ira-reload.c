/* Integrated Register Allocator (IRA) reloading .
   Copyright (C) 2009, 2010, 2011
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
static bitmap regs_to_load_alt;
static bitmap regs_to_store;
static bitmap regs_to_store_alt;
static int *pseudo_nuses;
static int *pseudo_nsets;
static rtx *reg_map;
static rtx *alt_reg_map;

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
  if ((reg_equivs && VEC_index (reg_equivs_t, reg_equivs, regno)->constant)
      || (reg_equivs && VEC_index (reg_equivs_t, reg_equivs, regno)->invariant))
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
identify_singleton_uses (rtx use, bitmap subregs_to_decompose, int df_flags)
{
  if (GET_CODE (use) == SUBREG)
    use = SUBREG_REG (use);

  if (GET_CODE (use) != REG)
    return;

  /* If this reference is an implicit use via a subreg and the referenced
     subreg is marked for decomposition, then we can ignore the implicit
     use as it will disappear when the subreg is decomposed.  */
  if (((df_flags & (DF_REF_SUBREG | DF_REF_READ_WRITE))
       == (DF_REF_SUBREG | DF_REF_READ_WRITE))
      && bitmap_bit_p (subregs_to_decompose, REGNO (use)))
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

/* Return true if there are no uses of DEST between (START, STOP].  */

static bool
no_uses_after_this_set (rtx dest, rtx start, rtx stop)
{
  rtx insn;
	  
  for (insn = NEXT_INSN (start);
       insn != NEXT_INSN (BB_END (BLOCK_FOR_INSN (stop)));
       insn = NEXT_INSN (insn))
    {
      df_ref *use_rec;
      rtx use = NULL_RTX;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	{
	  use = DF_REF_REG (*use_rec);
	  if (GET_CODE (use) == SUBREG)
	    use = SUBREG_REG (use);

	  if (use == dest)
	    return false;
	}
    }
  return true;
}

/* Collect (into REGS_TO_LOAD) USE if it is a pseudo marked for
   localization.  */

static void
collect_loads (rtx use, bitmap subregs_to_decompose, int df_flags)
{
  rtx orig = use;
  bool alt = false;

  if (GET_CODE (use) == SUBREG)
    use = SUBREG_REG (use);

  if (GET_CODE (use) != REG)
    return;

  /* If this reference is an implicit use via a subreg and the referenced
     subreg is marked for decomposition, then we can ignore the implicit
     use as it will disappear when the subreg is decomposed.  */
  if (((df_flags & (DF_REF_SUBREG | DF_REF_READ_WRITE))
       == (DF_REF_SUBREG | DF_REF_READ_WRITE))
      && bitmap_bit_p (subregs_to_decompose, REGNO (use)))
    return;

  /* If this is the high word access of a double word pseudo that was marked
     for decomposition, then use the _alt arrays as needed.  */
  if (GET_CODE (orig) == SUBREG
      && GET_MODE_SIZE (GET_MODE (orig)) < GET_MODE_SIZE (GET_MODE (use))
      && bitmap_bit_p (subregs_to_decompose, REGNO (use))
      && SUBREG_BYTE (orig) != 0)
   alt = true;

  if (bitmap_bit_p (pseudos_to_localize, REGNO (use)))
    bitmap_set_bit ((alt ? regs_to_load_alt : regs_to_load), REGNO (use));

  return;
}

/* If USE refers to a pseudo marked in REGS_TO_LOAD, emit a load of
   the pseudo before INSN.  */

static int
emit_localizing_loads (rtx use,
		       rtx insn,
		       bitmap subregs_to_decompose,
		       int df_flags)
{
  rtx orig = use;
  bool alt = false;
  bool decompose = false;

  if (GET_CODE (use) == SUBREG)
    use = SUBREG_REG (use);

  if (GET_CODE (use) != REG)
    return 0;

  /* If this reference is an implicit use via a subreg and the referenced
     subreg is marked for decomposition, then we can ignore the implicit
     use as it will disappear when the subreg is decomposed.  */
  if (((df_flags & (DF_REF_SUBREG | DF_REF_READ_WRITE))
       == (DF_REF_SUBREG | DF_REF_READ_WRITE))
      && bitmap_bit_p (subregs_to_decompose, REGNO (use)))
    return 0;

  /* If this is word access of a double word pseudo that was marked for
     decomposition, then decompose the double-word access to single word
     accesses.  */
  if (GET_CODE (orig) == SUBREG
      && GET_MODE_SIZE (GET_MODE (orig)) < GET_MODE_SIZE (GET_MODE (use))
      && bitmap_bit_p (subregs_to_decompose, REGNO (use)))
    decompose = true;

  /* If this is the high word access of a double word pseudo that was marked
     for decomposition, then use the _alt arrays as needed.  */
  if (decompose && SUBREG_BYTE (orig) != 0)
   alt = true;

  if (bitmap_bit_p (pseudos_to_localize, REGNO (use)))
    {
      /* If this pseudo still needs a load, emit it.  */
      if (bitmap_bit_p ((alt ? regs_to_load_alt : regs_to_load), REGNO (use)))
	{
	  rtx insns, temp;
	  rtx mem
	    = VEC_index (reg_equivs_t, reg_equivs, REGNO (use))->memory_loc;
	  int nuses = pseudo_nuses[REGNO (use)];
	  int nsets = pseudo_nsets[REGNO (use)];
	  int occurrences = count_occurrences (PATTERN (insn), use, 0);

	  mem = copy_rtx (mem);

	  /* If we're decomposing a SUBREG, then the memory address needs
 	     adjustment.  */
	  if (decompose)
	    mem = adjust_address_nv (mem, GET_MODE (orig), SUBREG_BYTE (orig));

	  /* validate_replace_rtx internally calls df_insn_rescan, which is
	     unsafe as our caller is iterating over the existing DF info.  So
	     we have to turn off insn rescanning temporarily.  */
	  df_set_flags (DF_NO_INSN_RESCAN);

	  /* If this insn has all the uses of a pseudo we want to localize
	     and the pseudo is never set, then try to replace the pseudo
	     with its equivalent memory location.  */
	  if (nsets == 0
	      && (occurrences == nuses || nuses == 2)
	      && validate_replace_rtx ((decompose ? orig : use), mem, insn))
	    {
	      df_clear_flags (DF_NO_INSN_RESCAN);
	    }
	  else
	    {
	      /* Create a new pseudo and record it in our map.  */
	      if ((alt ? alt_reg_map[(REGNO (use))] : reg_map [(REGNO (use))])
		   == NULL)
		{
		  if (alt)
		    alt_reg_map [REGNO (use)] = gen_reg_rtx (GET_MODE (orig));
		  else
		    reg_map [REGNO (use)]= gen_reg_rtx ((decompose
							 ? GET_MODE (orig) 
							 : GET_MODE (use)));
		}

	      df_clear_flags (DF_NO_INSN_RESCAN);
	      start_sequence ();
	      emit_move_insn ((alt
			       ? alt_reg_map [REGNO (use)] 
			       : reg_map [REGNO (use)]),
			       mem);
	      insns = get_insns();
	      end_sequence ();
	      emit_insn_before (insns, insn);

	      /* Inform the DF framework about the new insns.  */
	      for (temp = insns; temp != insn; temp = NEXT_INSN (insns))
	        df_insn_rescan (temp);

	      /* Note it is no longer necessary to load this pseudo.  */
	      bitmap_clear_bit ((alt ? regs_to_load_alt : regs_to_load),
				REGNO (use));
	    }
	}

      /* Replace the original pseudo with the new one.  */
      if ((alt ? alt_reg_map [REGNO (use)] : reg_map [REGNO (use)]))
	replace_rtx (insn,
		     (decompose ? orig : use),
		     (alt ? alt_reg_map [REGNO (use)] : reg_map [REGNO (use)]));
      return 1;
    }
  return 0;
}

/* DEST is an output for INSN.  If the output is marked
   for localizing, then we need to rename it to the new block-local
   pseudo.  This finishes the localization of unallocated globals.  */

static int
rename_sets (rtx dest, rtx insn, bitmap subregs_to_decompose, int df_flags)
{
  rtx orig = dest;
  bool decompose = false;
  bool alt = false;

  if (GET_CODE (dest) == SUBREG)
    dest = SUBREG_REG (dest);

  if (GET_CODE (dest) != REG)
    return 0;

  /* If this is word access of a double word pseudo that was marked for
     decomposition, then decompose the double-word access to single word
     accesses.  */
  if (GET_CODE (orig) == SUBREG
      && GET_MODE_SIZE (GET_MODE (orig)) < GET_MODE_SIZE (GET_MODE (dest))
      && bitmap_bit_p (subregs_to_decompose, REGNO (dest)))
    decompose = true;

  /* If this is the high word access of a double word pseudo that was marked
     for decomposition, then use the _alt arrays as needed.  */
  if (decompose && SUBREG_BYTE (orig) != 0)
    alt = true;

  /* If DEST isn't maked for spilling, then there is nothing to do.  */
  if (! bitmap_bit_p (pseudos_to_localize, REGNO (dest)))
    return 0;

  /* A store can be viewed as a store followed by a load, so we can clear DEST
     from REGS_TO_LOAD, but not if this was a partial store.  */
  if (GET_CODE (orig) == STRICT_LOW_PART)
    {
      /* This must be treated as a USE too.
         ?!? Does this need to integrated with the use processing?  */
      emit_localizing_loads (dest, insn, subregs_to_decompose, df_flags);
    }
  else if (GET_CODE (orig) == SUBREG
	   && (GET_MODE_SIZE (GET_MODE (orig)))
	       < GET_MODE_SIZE (GET_MODE (dest)))
    {
      /* This must be treated as a USE too.
         ?!? Does this need to integrated with the use processing?  */
      if (!decompose)
	emit_localizing_loads (dest, insn, subregs_to_decompose, df_flags);
    }

  /* ?!? I'm not entirely sure this can still happen.  */
  if ((alt ? alt_reg_map [(REGNO (dest))] : reg_map [(REGNO (dest))]) == NULL)
    {
      if (alt)
	alt_reg_map [REGNO (dest)] = gen_reg_rtx (GET_MODE (orig));
      else
        reg_map [REGNO (dest)] = gen_reg_rtx (decompose 
					      ? GET_MODE (orig) 
					      : GET_MODE (dest));
    }

  replace_rtx (insn,
	       (decompose ? orig : dest),
	       (alt ? alt_reg_map [REGNO (dest)] : reg_map [REGNO (dest)]));
  return 1;
}

/* Store each pseudo set by the current insn that is
   marked for localizing into memory after INSN. 

   Return 0 if INSN does not need rescanning.

   Return 1 if INSN needs rescanning.

   Return -1 if INSN should be deleted.  */

static int
emit_localizing_stores (rtx dest,
			rtx insn,
			rtx tail,
			bitmap subregs_to_decompose)
{
  unsigned int regno;
  int retval = 0;
  rtx insns;
  rtx orig = dest;
  bool decompose = false;
  bool alt = false;

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

  /* If this is word access of a double word pseudo that was marked for
     decomposition, then decompose the double-word access to single word
     accesses.  */
  if (GET_CODE (orig) == SUBREG
      && GET_MODE_SIZE (GET_MODE (orig)) < GET_MODE_SIZE (GET_MODE (dest))
      && bitmap_bit_p (subregs_to_decompose, regno))
    decompose = true;

  /* If this is the high word access of a double word pseudo that was marked
     for decomposition, then use the _alt arrays as needed.  */
  if (decompose && SUBREG_BYTE (orig) != 0)
    alt = true;

  /* IF this register is marked for decomposition and INSN is a naked
     CLOBBER, then mark INSN for deletion since it's not needed anymore.  */
  if (bitmap_bit_p (subregs_to_decompose, regno)
      && GET_CODE (PATTERN (insn)) == CLOBBER)
    retval = -1;

  /* DEST is marked for spilling, if we have not emitted a spill store yet for
     DEST, then do so now.  Note we do not change INSN at this time.  */
  if (bitmap_bit_p ((alt ? regs_to_store_alt : regs_to_store), regno))
    {
      int nuses = pseudo_nuses[REGNO (dest)];
      int nsets = pseudo_nsets[REGNO (dest)];
      int occurrences = count_occurrences (PATTERN (insn), dest, 0);

      /* We must copy the memory location to avoid incorrect RTL sharing.  */
      rtx mem = VEC_index (reg_equivs_t, reg_equivs, regno)->memory_loc;

      mem = copy_rtx (mem);

      /* If we're decomposing a SUBREG, then the memory address needs
	 adjustment.  */
      if (decompose)
	mem = adjust_address_nv (mem, GET_MODE (orig), SUBREG_BYTE (orig));

      /* Note that we have stored this register so that we don't try to
         store it again.  */
      bitmap_clear_bit ((alt ? regs_to_store_alt : regs_to_store), regno);

      /* validate_replace_rtx internally calls df_insn_rescan, which is
	 unsafe as our caller is iterating over the existing DF info.  So
	 we have to turn off insn rescanning temporarily.  */
      df_set_flags (DF_NO_INSN_RESCAN);

      /* If this insn both uses and sets a pseudo we want to localize and
	 contains all the uses and sets, then try to replace the pseudo
	 with its equivalent memory location.  */
      if (nuses
	  && nsets == 1
	  && (occurrences == nuses
	      || (occurrences == 1
		  && nuses == 2
		  && ! no_uses_after_this_set (dest, insn, tail)))
	  && validate_replace_rtx ((decompose ? orig : dest), mem, insn))
	{
	  pseudo_nsets[REGNO (dest)]--;
	  df_clear_flags (DF_NO_INSN_RESCAN);
	  retval = 1;
	}
      /* Similarly if this insn sets a pseudo we want to localize and
	 there are no uses after this set, then try to replace the pseudo
	 with its equivalent memory location.  */
      else if ((nsets == 1 || nuses == 0)
	       && no_uses_after_this_set (dest, insn, tail)
	       && validate_replace_rtx ((decompose ? orig : dest), mem, insn))
	{
	  pseudo_nsets[REGNO (dest)]--;
	  df_clear_flags (DF_NO_INSN_RESCAN);
	  retval = 1;
	}
      else
	{
	  df_clear_flags (DF_NO_INSN_RESCAN);
          start_sequence ();
          emit_move_insn (mem, (decompose ? orig : dest));
          insns = get_insns();
          end_sequence ();

          /* If the pseudo is being set from its equivalent memory location
	     and is unused from this point until the end of this block, then
             we don't need to store the pseudo back to memory back, we
	     actually want to delete INSN.  */
          if (NEXT_INSN (insns) == 0
              && single_set (insns)
              && single_set (insn)
              && rtx_equal_p (SET_SRC (single_set (insn)),
			  SET_DEST (single_set (insns))))
	    {
	      if (no_uses_after_this_set (dest, insn, tail))
		retval = -1;
	    }
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
    bitmap_set_bit ((alt ? regs_to_load_alt : regs_to_load), regno);
  else if (retval == 0)
    bitmap_clear_bit ((alt ? regs_to_load_alt : regs_to_load), REGNO (dest));
  return retval;
}

static ira_allocno_t
create_new_allocno_for_spilling (int nreg, int oreg)
{
  ira_allocno_t to, from, a;
  ira_allocno_iterator ai;

  /* Update IRA's datastructures.  */

  /* First create the allocno.  */
  to = ira_create_allocno (nreg, true, ira_curr_loop_tree_node);

  /* This must occur before creating objects so that we know how many
     objects to create.  */
  from = ira_regno_allocno_map [oreg];
  ALLOCNO_COVER_CLASS (to) = ALLOCNO_COVER_CLASS (from);

  ira_create_allocno_objects (to);
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

  return to;
}


static void
copy_allocno_for_spilling (int nreg, int oreg)
{
  ira_allocno_t to, from;
  unsigned int conflicts, n, i;
  live_range_t prev, range, r;
  ira_object_conflict_iterator oci;
  ira_object_t o;

  if (ira_dump_file != NULL)
    fprintf (ira_dump_file, "Splitting range for %d using new reg %d.\n",
	     oreg, nreg);

  to = create_new_allocno_for_spilling (nreg, oreg);
  from = ira_regno_allocno_map [oreg];

  /* Copy various fields from the original allocno to the new one.  */
#ifdef STACK_REGS
  ALLOCNO_NO_STACK_REG_P (to) = ALLOCNO_NO_STACK_REG_P (from);
  ALLOCNO_TOTAL_NO_STACK_REG_P (to) = ALLOCNO_TOTAL_NO_STACK_REG_P (from);
#endif
  ALLOCNO_NREFS (to) = ALLOCNO_NREFS (from);
  ALLOCNO_FREQ (to) = REG_FREQ (nreg);
  ALLOCNO_CALL_FREQ (to) = 0;
  ALLOCNO_CALLS_CROSSED_NUM (to) = 0;
  ALLOCNO_EXCESS_PRESSURE_POINTS_NUM (to)
    = ALLOCNO_EXCESS_PRESSURE_POINTS_NUM (from);
  ALLOCNO_BAD_SPILL_P (to) = ALLOCNO_BAD_SPILL_P (from);

  ALLOCNO_COVER_CLASS_COST (to) = ALLOCNO_COVER_CLASS_COST (from);
  ALLOCNO_MEMORY_COST (to) = ALLOCNO_MEMORY_COST (from);
  ALLOCNO_UPDATED_MEMORY_COST (to) = ALLOCNO_UPDATED_MEMORY_COST (from);
  ALLOCNO_NEXT_REGNO_ALLOCNO (to) = NULL;

  n = ALLOCNO_NUM_OBJECTS (to);
  for (i = 0; i < n; i++)
    {
      ira_object_t obj = ALLOCNO_OBJECT (to, i);
      /* We recompute these fields after we have localized an entire block.  */
      COPY_HARD_REG_SET (OBJECT_CONFLICT_HARD_REGS (obj),
			 ira_no_alloc_regs);
      COPY_HARD_REG_SET (OBJECT_TOTAL_CONFLICT_HARD_REGS (obj),
			 ira_no_alloc_regs);
      IOR_COMPL_HARD_REG_SET (OBJECT_CONFLICT_HARD_REGS (obj),
			      reg_class_contents[ALLOCNO_COVER_CLASS (to)]);
      IOR_COMPL_HARD_REG_SET (OBJECT_TOTAL_CONFLICT_HARD_REGS (obj),
			      reg_class_contents[ALLOCNO_COVER_CLASS (to)]);
    }

  /* ?!? This is a hack.
     If the original allocno conflicts will all hard registers, then it must
     have crossed a setjmp call or something similar.  Just copy the conflicts
     in this one case.

     Long term we'll catch this elsewhere as the new allocno may have not
     have the same constraints on allocation that the original allocno had.  */
  for (i = 0; i < n; i++)
    {
      HARD_REG_SET x;
      ira_object_t tobj = ALLOCNO_OBJECT (to, i);
      ira_object_t sobj = ALLOCNO_OBJECT (from, i);

      COMPL_HARD_REG_SET (x, OBJECT_CONFLICT_HARD_REGS (sobj));
      if (hard_reg_set_empty_p (x))
        {
	  COPY_HARD_REG_SET (OBJECT_CONFLICT_HARD_REGS (tobj),
			     OBJECT_CONFLICT_HARD_REGS (sobj));
	  COPY_HARD_REG_SET (OBJECT_TOTAL_CONFLICT_HARD_REGS (tobj),
			     OBJECT_TOTAL_CONFLICT_HARD_REGS (sobj));
        }

      /* Count the number of conflicts on the original allocno.  We use that
	 count as an estimate for the number of conflicts in the new allocno.
	 The new allocno should have fewer conflicts than the original as the
	 new allocno is only live in BB and thus only conflicts with objects	
	 live in BB.  */
      conflicts = 0;
      FOR_EACH_OBJECT_CONFLICT (sobj, o, oci)
        conflicts++;

      OBJECT_MIN (tobj) = OBJECT_MIN (sobj);
      OBJECT_MAX (tobj) = OBJECT_MAX (sobj);
      ira_allocate_object_conflicts (tobj, conflicts);

      /* For now we copy the live range from the original allocno to the new
         allocno.  This is very suboptimal.  Consider if we have some allocno A
         which gets split into A0..A100.  A0..A100 will not be able to share
         stack slots because the live ranges conflict (they were copied from A
         verbatim) -- however, in reality each new allocno A0..A100 has a
         distinct, non-conflicting live range.  */
      for (prev = NULL, r = OBJECT_LIVE_RANGES (sobj);
           r != NULL;
           r = r->next, prev = range)
        {
          range = ira_create_live_range (tobj, r->start, r->finish, NULL);
	  /* ?!? This may not be necessary.  */
          range->start_next = NULL;
          range->finish_next = NULL;
          if (prev)
	    prev->next = range;
          else
	    OBJECT_LIVE_RANGES (tobj) = range;
        }
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
      int i, n;

      n = ALLOCNO_NUM_OBJECTS (a);
      for (i = 0; i < n; i++)
	{
	  SET_HARD_REG_BIT (OBJECT_TOTAL_CONFLICT_HARD_REGS (ALLOCNO_OBJECT (a, i)), reg1);
	  SET_HARD_REG_BIT (OBJECT_CONFLICT_HARD_REGS (ALLOCNO_OBJECT (a, i)), reg1);
	}
      return;
    }

  if (reg2 < FIRST_PSEUDO_REGISTER)
    {
      ira_allocno_t a = ira_regno_allocno_map[reg1];
      int i, n;

      n = ALLOCNO_NUM_OBJECTS (a);
      for (i = 0; i < n; i++)
	{
          SET_HARD_REG_BIT (OBJECT_TOTAL_CONFLICT_HARD_REGS (ALLOCNO_OBJECT (a, i)), reg2);
          SET_HARD_REG_BIT (OBJECT_CONFLICT_HARD_REGS (ALLOCNO_OBJECT (a, i)), reg2);
	}
      return;
    }

  /* If the registers are in different cover classes, then ignore this
     conflict.  */
  if (ira_class_translate[reg_preferred_class (reg1)]
      != ira_class_translate[reg_preferred_class (reg2)])
    return;

  ira_add_conflict (ALLOCNO_OBJECT (ira_regno_allocno_map[reg1], 0),
		    ALLOCNO_OBJECT (ira_regno_allocno_map[reg2], 0));
  if (ALLOCNO_NUM_OBJECTS (ira_regno_allocno_map[reg2]) == 2)
    ira_add_conflict (ALLOCNO_OBJECT (ira_regno_allocno_map[reg1], 0),
		      ALLOCNO_OBJECT (ira_regno_allocno_map[reg2], 1));
  if (ALLOCNO_NUM_OBJECTS (ira_regno_allocno_map[reg1]) == 2)
    ira_add_conflict (ALLOCNO_OBJECT (ira_regno_allocno_map[reg1], 1),
		      ALLOCNO_OBJECT (ira_regno_allocno_map[reg2], 0));
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
fix_allocno_map ( ira_loop_tree_node_t loop_tree_node)
{
  /* Free the old (and inaccurate) map.  */
  free (loop_tree_node->regno_allocno_map);

  /* Now install the correct map.  */
  loop_tree_node->regno_allocno_map = ira_regno_allocno_map;
}

static void
build_conflicts_for_new_allocnos (rtx head, rtx tail,
				  bitmap pseudos_to_localize,
				  int orig_max_reg_num)
{
  rtx insn;
  basic_block bb = NULL;


  live = BITMAP_ALLOC (NULL);

  for (insn = tail; insn != PREV_INSN (head); insn = PREV_INSN (insn))
    {
      df_ref *def_rec, *use_rec;
      int call_p;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      /* Anytime we start processing a block we have to merge in the
         registers live at the end of that block - pseudos_to_localize.  */
      if (bb != BLOCK_FOR_INSN (insn))
	{
	  hard_reg_set_iterator hrsi;
	  unsigned int i;

	  bb = BLOCK_FOR_INSN (insn);
          bitmap_ior_and_compl_into (live, DF_LIVE_OUT (bb), pseudos_to_localize);
	  EXECUTE_IF_SET_IN_HARD_REG_SET (eliminable_regset, 0, i, hrsi)
	    bitmap_clear_bit (live, i);
	  EXECUTE_IF_SET_IN_HARD_REG_SET (ira_no_alloc_regs, 0, i, hrsi)
	    bitmap_clear_bit (live, i);
	}
	
      call_p = CALL_P (insn);

      if (call_p)
	{
	  bitmap_iterator bi;
	  unsigned int i;
	  int freq;

	  freq = REG_FREQ_FROM_BB (BLOCK_FOR_INSN (insn));
	  if (freq == 0)
	    freq = 1;
	  EXECUTE_IF_SET_IN_BITMAP (live, orig_max_reg_num, i, bi)
	    {
	      ira_allocno_t a = ira_regno_allocno_map[i];
	      unsigned int n = ALLOCNO_NUM_OBJECTS (a);
	      unsigned int j;

	      ALLOCNO_CALLS_CROSSED_NUM (a)++;
	      ALLOCNO_CALL_FREQ (a) += freq;

	      /* We want to avoid caller-saves for the new pseudos as
		 the new pseudos are already backed by a memory location.

		 We could split these pseudos at call boundaries and remove
		 this hack.  That would probably get us the best of both
		 worlds in most cases.  */
	      for (j = 0; j < n; j++)
		{
		  ira_object_t obj = ALLOCNO_OBJECT (a, j);
	      
		  IOR_HARD_REG_SET (OBJECT_TOTAL_CONFLICT_HARD_REGS (obj),
				    call_used_reg_set);
		  IOR_HARD_REG_SET (OBJECT_CONFLICT_HARD_REGS (obj),
				    call_used_reg_set);
		}
	    }
	}

      /* Mark conflicts for any values defined in this insn. 
 	 Ideally we'd like to ignore conflicts resulting from simple
	 copies.  Unfortunately that confuses reload because we can have
	 two pseudos assigned the same hard reg with overlapping lifetimes.
	 If the insn where one pseudo dies needs an output reload, then
	 reload (via combine_reloads) may select the dying pseudo's hard reg
	 to hold the output reload, which clobbers the value in the hard
	 reg which is still live.  */
      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	if (!call_p || !DF_REF_FLAGS_IS_SET (*def_rec, DF_REF_MAY_CLOBBER))
	  mark_conflicts (DF_REF_REG (*def_rec), orig_max_reg_num);

      /* Mark each used value as live.  */
      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	mark_live (DF_REF_REG (*use_rec));
    }

  BITMAP_FREE (live);
}

/* REF is a use or a set.  For whatever registers REF refers to set the
   appropriate bit in MAP.  */

static void
record_a_use_or_set (bitmap map, rtx ref)
{
  if (GET_CODE (ref) == SUBREG)
    ref = SUBREG_REG (ref);

  if (GET_CODE (ref) != REG)
    return;

  bitmap_set_bit (map, REGNO (ref));
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
localize_pseudos (basic_block bb, bitmap pseudos_to_localize, bitmap visited)
{
  int orig_max_reg_num = max_reg_num ();
  int i;
  rtx insn, head, tail;
  bitmap subreg = BITMAP_ALLOC (NULL);
  bitmap full = BITMAP_ALLOC (NULL);

  /* Get the head and tail insns of this region.  */
  head = BB_HEAD (bb);
  for (;;)
    {
      edge e;
      edge_iterator ei;
      tail = BB_END (bb);
      if (bb->next_bb == EXIT_BLOCK_PTR
	  || LABEL_P (BB_HEAD (bb->next_bb)))
	break;
      FOR_EACH_EDGE (e, ei, bb->succs)
	if ((e->flags & EDGE_FALLTHRU) != 0)
	  break;
      if (! e)
	break;
      bb = bb->next_bb;
    }

  /* We can decompose some additional double-word SUBREGs here since we're
     looking at a smaller window of insns than lower-subreg.  However, this
     method is also simpler, so in some ways its not as aggresive as
     lower-subreg. 

     For each pseudo, we want to know if the pseudo was used in its full
     mode and if it was used in a partial mode via a subreg.  */
  for (insn = tail; insn != PREV_INSN (head); insn = PREV_INSN (insn))
    {
      df_ref *def_rec, *use_rec;

      /* We want to ignore naked CLOBBERs since they generate no code and
	 would impede decomposing double-word subregs.  */
      if (!NONDEBUG_INSN_P (insn) || GET_CODE (PATTERN (insn)) == CLOBBER)
	continue;

      /* For each def, see if it is a partial subreg store and if the size
	 of the outer mode is half the size of the inner mode and the size
	 of the outer mode is the same as a word.  Note that for stores we
	 see the SUBREG itself which makes this easy.  */
      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	if ((DF_REF_FLAGS (*def_rec) & (DF_REF_SUBREG | DF_REF_PARTIAL))
	     == (DF_REF_SUBREG | DF_REF_PARTIAL)
	    && (GET_MODE_BITSIZE (GET_MODE (DF_REF_REG (*def_rec))) * 2
		== GET_MODE_BITSIZE (GET_MODE (SUBREG_REG (DF_REF_REG (*def_rec)))))
	    && (GET_MODE_BITSIZE (GET_MODE (DF_REF_REG (*def_rec)))
		== GET_MODE_BITSIZE (word_mode)))
	  record_a_use_or_set (subreg, DF_REF_REG (*def_rec));
	else
	  record_a_use_or_set (full, DF_REF_REG (*def_rec));


      /* Similarly for each use, except the use might be implied by a
         write to a SUBREG.  In that case we will not see the SUBREG
	 expression, but instead will see a full word read marked with
	 DF_REF_READ_WRITE.  We want to consider those SUBREG reads
         as they'll disappear if we decompose the SUBREG.  */
      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	if ((DF_REF_FLAGS (*use_rec) & (DF_REF_SUBREG | DF_REF_PARTIAL))
	     == (DF_REF_SUBREG | DF_REF_PARTIAL)
	    && (GET_MODE_BITSIZE (GET_MODE (DF_REF_REG (*use_rec))) * 2
		== GET_MODE_BITSIZE (GET_MODE (SUBREG_REG (DF_REF_REG (*use_rec)))))
	    && (GET_MODE_BITSIZE (GET_MODE (DF_REF_REG (*use_rec)))))
	  record_a_use_or_set (subreg, DF_REF_REG (*use_rec));
	else if ((DF_REF_FLAGS (*use_rec) & (DF_REF_SUBREG | DF_REF_READ_WRITE))
	    == (DF_REF_SUBREG | DF_REF_READ_WRITE)
	   && (GET_MODE_BITSIZE (GET_MODE (DF_REF_REG (*use_rec))) 
	       == 2 * GET_MODE_BITSIZE (word_mode)))
	  record_a_use_or_set (subreg, DF_REF_REG (*use_rec));
	else
	  record_a_use_or_set (full, DF_REF_REG (*use_rec));
    }

  /* Now eliminate any pseudos that were used in their full width
     for the candidates to decompose.  */
  bitmap_and_compl_into (subreg, full);

  /* Eliminate regs not marked for localization from the
     candidates to decompose.  */
  bitmap_and_into (subreg, pseudos_to_localize);

  regs_to_store = BITMAP_ALLOC (NULL);
  regs_to_store_alt = BITMAP_ALLOC (NULL);
  regs_to_load = BITMAP_ALLOC (NULL);
  regs_to_load_alt = BITMAP_ALLOC (NULL);
  pseudo_nuses = (int *) xmalloc (max_reg_num () * sizeof (int));
  memset (pseudo_nuses, 0, max_reg_num () * sizeof (int));
  pseudo_nsets = (int *) xmalloc (max_reg_num () * sizeof (int));
  memset (pseudo_nsets, 0, max_reg_num () * sizeof (int));

  reg_map = (rtx *) xmalloc (sizeof (rtx) * orig_max_reg_num);
  memset (reg_map, 0, sizeof (rtx) * orig_max_reg_num);
  alt_reg_map = (rtx *) xmalloc (sizeof (rtx) * orig_max_reg_num);
  memset (alt_reg_map, 0, sizeof (rtx) * orig_max_reg_num);
   
  bitmap_copy (regs_to_store, pseudos_to_localize);
  bitmap_copy (regs_to_store_alt, pseudos_to_localize);

  /* First walk over the insns in this region and identify singleton
     uses of registers in PSEUDOS_TO_LOCALIZE.  We want to know if a
     use is a singleton so that we can change the use to a MEM.  We
     need this information prior to emitting localizing stores so that
     we can change both the use and set in a single insn to a MEM.  */
  for (insn = tail; insn != PREV_INSN (head); insn = PREV_INSN (insn))
    {
      df_ref *def_rec, *use_rec;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      bitmap_set_bit (visited, BLOCK_FOR_INSN (insn)->index);

      /* If we have traversed into a new basic block, then reset
	 NO_USES_AFTER_LAST_SET for any pseudo we want to localize
	 that is live-out on the edge(s) that we did NOT traverse.  */
      if (bb != BLOCK_FOR_INSN (insn))
	{
	  edge e;
	  edge_iterator ei;

	  bb = BLOCK_FOR_INSN (insn);
	  FOR_EACH_EDGE (e, ei, bb->succs)
	    {
	      bitmap_ior_and_into (regs_to_store,
				   pseudos_to_localize,
				   DF_LIVE_IN (e->dest));
	      bitmap_ior_and_into (regs_to_store_alt,
				   pseudos_to_localize,
				   DF_LIVE_IN (e->dest));
	    }
	}

      /* We don't want CLOBBERS to be counted as they generate no code.  */
      if (GET_CODE (PATTERN (insn)) == CLOBBER)
	continue;

      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	identify_singleton_sets (DF_REF_REG (*def_rec));

      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	identify_singleton_uses (DF_REF_REG (*use_rec),
				 subreg,
				 DF_REF_FLAGS (*use_rec));
    }

  /* Next emit a store after the last assignment of each pseudo in
     PSEUDOS_TO_LOCALIZE within the region.  Collect list of pseudos
     we'll need to load as well.  */
  for (bb = BLOCK_FOR_INSN (tail), insn = tail;
       insn != PREV_INSN (BB_HEAD (BLOCK_FOR_INSN (head)));
       insn = PREV_INSN (insn))
    {
      df_ref *def_rec, *use_rec;
      int status;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      /* If we have traversed into a new basic block, then reset REGS_TO_STORE
	 for any pseudo we want to localize that is live-out on the edge(s)
	 that we did NOT traverse.  */
      if (bb != BLOCK_FOR_INSN (insn))
	{
	  edge e;
	  edge_iterator ei;

	  bb = BLOCK_FOR_INSN (insn);
	  FOR_EACH_EDGE (e, ei, bb->succs)
	    {
	      if ((e->flags & EDGE_FALLTHRU) == 0)
		{
		  bitmap_ior_and_into (regs_to_store,
				       pseudos_to_localize,
				       DF_LIVE_IN (e->dest));
		  bitmap_ior_and_into (regs_to_store_alt,
				       pseudos_to_localize,
				       DF_LIVE_IN (e->dest));
		}
	    }
	}

      status = 0;
      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	status |= emit_localizing_stores (DF_REF_REG (*def_rec),
					  insn, tail, subreg);

      /* A return status of -1 indicates INSN should be removed, including
	 naked CLOBBERS.  Do not delete other assignments that are not
	 simple SET insns.  */
      if (status == -1
	  && (GET_CODE (PATTERN (insn)) == CLOBBER
	      || single_set (insn)))
	{
	  set_insn_deleted (insn);
	  continue;
	}

      /* It is not safe to defer scanning any further as emit_localizing_stores
	 can change uses and defs.  */
      if (status)
	df_insn_rescan (insn);

      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	collect_loads (DF_REF_REG (*use_rec), subreg, DF_REF_FLAGS (*use_rec));
    }

  /* Now walk forward through the region emitting loads before
     the first use of each pseudo that we're localizing and change
     each reference from an unallocated pseudo to a new block local
     spill register.  */
  for (insn = head;
       insn != NEXT_INSN (BB_END (BLOCK_FOR_INSN (tail)));
       insn = NEXT_INSN (insn))
    {
      df_ref *def_rec, *use_rec;
      int need_rescan;

      if (!NONDEBUG_INSN_P (insn))
	continue;

      need_rescan = 0;
      for (use_rec = DF_INSN_USES (insn); *use_rec; use_rec++)
	need_rescan |= emit_localizing_loads (DF_REF_REG (*use_rec),
					      insn,
					      subreg,
					      DF_REF_FLAGS (*use_rec));

      for (def_rec = DF_INSN_DEFS (insn); *def_rec; def_rec++)
	need_rescan |= rename_sets (DF_REF_REG (*def_rec),
				    insn,
				    subreg,
				    DF_REF_FLAGS (*def_rec));

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
      grow_reg_equivs ();
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

	  if (reg_map[i] != 0)
	    {
	      nregno = REGNO (reg_map[i]);
	      setup_reg_classes (nregno,
				 reg_preferred_class (i),
				 reg_alternate_class (i),
				 ira_class_translate [reg_preferred_class (i)]);
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->invariant
		= VEC_index (reg_equivs_t, reg_equivs, i)->invariant;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->constant
		= VEC_index (reg_equivs_t, reg_equivs, i)->constant;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->mem
		= VEC_index (reg_equivs_t, reg_equivs, i)->mem;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->alt_mem_list
		= VEC_index (reg_equivs_t, reg_equivs, i)->alt_mem_list;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->address
		= VEC_index (reg_equivs_t, reg_equivs, i)->address;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->memory_loc
		= VEC_index (reg_equivs_t, reg_equivs, i)->memory_loc;
	      /* ?!? I don't recall why this was originally necessary.
		 Definitely need to retest and understand or delete it.  */
	      VEC_index (reg_equivs_t, reg_equivs, i)->init = NULL;
#if 0
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->init
		= VEC_index (reg_equivs_t, reg_equivs, i)->init;
#endif
	      reg_max_ref_width[nregno] = reg_max_ref_width[i];
	      reg_renumber[nregno] = reg_renumber[i];
	      REG_N_CALLS_CROSSED (nregno) = REG_N_CALLS_CROSSED (i);
	      REG_FREQ (nregno) = ((pseudo_nuses[i] + pseudo_nsets[i])
				   * REG_FREQ_FROM_BB (bb));

	      /* We don't really care other than to be sure there's a set
		 and ref.  */
	      SET_REG_N_SETS (nregno, 1);
	      SET_REG_N_REFS (nregno, 1);

	      /* The new register is always local to this block.  */
	      REG_BASIC_BLOCK (nregno) = REG_BLOCK_GLOBAL;

	      /* Create a new allocno for the new register.  */
	      copy_allocno_for_spilling (nregno, i);
	    }

	  if (alt_reg_map[i] != 0)
	    {
	      nregno = REGNO (alt_reg_map[i]);
	      setup_reg_classes (nregno,
				 reg_preferred_class (i),
				 reg_alternate_class (i),
				 ira_class_translate [reg_preferred_class (i)]);
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->invariant
		= VEC_index (reg_equivs_t, reg_equivs, i)->invariant;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->constant
		= VEC_index (reg_equivs_t, reg_equivs, i)->constant;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->mem
		= VEC_index (reg_equivs_t, reg_equivs, i)->mem;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->alt_mem_list
		= VEC_index (reg_equivs_t, reg_equivs, i)->alt_mem_list;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->address
		= VEC_index (reg_equivs_t, reg_equivs, i)->address;
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->memory_loc
		= VEC_index (reg_equivs_t, reg_equivs, i)->memory_loc;
	      /* ?!? I don't recall why this was originally necessary.
		 Definitely need to retest and understand or delete it.  */
	      VEC_index (reg_equivs_t, reg_equivs, i)->init = NULL;
#if 0
	      VEC_index (reg_equivs_t, reg_equivs, nregno)->init
		= VEC_index (reg_equivs_t, reg_equivs, i)->init;
#endif
	      reg_max_ref_width[nregno] = reg_max_ref_width[i];
	      reg_renumber[nregno] = reg_renumber[i];
	      REG_N_CALLS_CROSSED (nregno) = REG_N_CALLS_CROSSED (i);
	      REG_FREQ (nregno) = ((pseudo_nuses[i] + pseudo_nsets[i])
				   * REG_FREQ_FROM_BB (bb));

	      /* We don't really care other than to be sure there's a set
		 and ref.  */
	      SET_REG_N_SETS (nregno, 1);
	      SET_REG_N_REFS (nregno, 1);

	      /* The new register is always local to this block.  */
	      REG_BASIC_BLOCK (nregno) = REG_BLOCK_GLOBAL;

	      /* Create a new allocno for the new register.  */
	      copy_allocno_for_spilling (nregno, i);
	    }
	}

      /* Now look for any pseudos >= orig_max_reg_num which do not have
	 an associated allocno.  These must have been created as temporaries
	 by emit_move_insn.  We'll need allocnos for them.  */
      for (i = orig_max_reg_num; i < (int)max_regno; i++)
	if (ira_regno_allocno_map[i] == NULL)
	  abort ();


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
      build_conflicts_for_new_allocnos (head,
					tail,
					pseudos_to_localize,
					orig_max_reg_num);

      /* We added new live-range objects, so rebuild the chains.  */
      ira_rebuild_start_finish_chains ();
    }

  free (reg_map);
  reg_map = NULL;
  free (alt_reg_map);
  alt_reg_map = NULL;
  BITMAP_FREE (regs_to_store);
  regs_to_store = NULL;
  BITMAP_FREE (regs_to_store_alt);
  regs_to_store_alt = NULL;
  BITMAP_FREE (regs_to_load);
  regs_to_load = NULL;
  BITMAP_FREE (regs_to_load_alt);
  regs_to_load_alt = NULL;
  free (pseudo_nuses);
  pseudo_nuses = NULL;
  free (pseudo_nsets);
  pseudo_nsets = NULL;
  BITMAP_FREE (subreg);
  subreg = NULL;
  BITMAP_FREE (full);
  full = NULL;
}

void
ira_reload (void)
{
  /* We need to build the various equivalences prior to assigning stack
     slots for unallocated global pseudos.  */
  record_equivalences_for_reload ();

  /* What to do when this isn't true?  */
  if (ira_conflicts_p)
    {
      unsigned int i, j;
      bitmap_iterator bi;
      basic_block bb;
      bitmap visited;
      int orig_max_reg_num = max_reg_num ();

      pseudos_to_localize = BITMAP_ALLOC (NULL);
      max_regno = max_reg_num ();
      visited = BITMAP_ALLOC (NULL);

      /* Collect all the registers we want to localize into a bitmap.
         We don't want to localize pseudos which are contained wholly
         within an EBB, so we look for pseudos which are live at the
         start of the EBB or at the end of the EBB.  */
      FOR_EACH_BB (bb)
        {
	  if (!bitmap_bit_p (visited, bb->index))
	    {
	      /* This collects pseudos live at the start of the EBB.  */
	      EXECUTE_IF_SET_IN_BITMAP (DF_LIVE_IN (bb), FIRST_PSEUDO_REGISTER, i, bi)
		if (localize_pseudo_p (i))
		  bitmap_set_bit (pseudos_to_localize, i);

	      for (;;)
		{
		  edge e;
		  edge_iterator ei;
		  bitmap_set_bit (visited, bb->index);
		  if (bb->next_bb == EXIT_BLOCK_PTR
		      || LABEL_P (BB_HEAD (bb->next_bb)))
		    break;
		  FOR_EACH_EDGE (e, ei, bb->succs)
		    if ((e->flags & EDGE_FALLTHRU) != 0)
		      break;
		  if (! e)
		    break;
		  bb = bb->next_bb;
		}

	      /* This collects pseudos live at the end of the EBB.  */
	      EXECUTE_IF_SET_IN_BITMAP (DF_LIVE_OUT (bb), FIRST_PSEUDO_REGISTER, i, bi)
		if (localize_pseudo_p (i))
		  bitmap_set_bit (pseudos_to_localize, i);
	    }
	}
	  
      /* If an assignment to a pseudo has a REG_EQUIV note attached to it for
	 a non-constant memory address, that memory location can generally be
	 considered the "home" for the pseudo.

	 That works great, except when the memory location contains a reference
	 to a pseudo which we are going to localize.  If we use the equivalence
	 we will introduce an uninitialized use of the pseudo we're localizing.

	 We just remove the equivalence for now; we could do better since we
	 know these regs will be local to a block and thus we can derive the
	 split pseudo's current name and update the note.  */
      for (j = FIRST_PSEUDO_REGISTER; j < (unsigned) max_regno; j++)
	{
	  rtx memloc = VEC_index (reg_equivs_t, reg_equivs, j)->memory_loc;

	  if (!memloc)
	    continue;

	  EXECUTE_IF_SET_IN_BITMAP (pseudos_to_localize, FIRST_PSEUDO_REGISTER,
				    i, bi)
	    {
	      if (reg_mentioned_p (regno_reg_rtx[i], memloc))
		{
		  rtx list;
		  VEC_index (reg_equivs_t, reg_equivs, j)->memory_loc
		    = NULL_RTX;
		  for (list = VEC_index (reg_equivs_t, reg_equivs, j)->init;
		       list;
		       list = XEXP (list, 1))
		    {
		      rtx equiv_insn = XEXP (list, 0);
		      rtx set = single_set (equiv_insn);
		      if (set
			  && REG_P (SET_DEST (set))
			  && SET_DEST (set) == regno_reg_rtx[j])
			{
			  rtx note = find_reg_note (equiv_insn, REG_EQUIV, NULL_RTX);
			  remove_note (equiv_insn, note);
			}
			
		    }
		  VEC_index (reg_equivs_t, reg_equivs, j)->init = NULL_RTX;
		
		  break;
		}
	    }
	}


      /* Assign stack slots for pseudos live at block boundaries which did not
         get hard regs.  This unfortunately turns pseudos into hard regs which
         we will need to undo later.  */
      assign_stack_slots ();

      for (i = FIRST_PSEUDO_REGISTER; i < (unsigned) max_regno; i++)
	if (regno_reg_rtx[i])
	  SET_REGNO (regno_reg_rtx[i], i);

      if (!bitmap_empty_p (pseudos_to_localize))
	{
	  bitmap_clear (visited);
          FOR_EACH_BB (bb)
	    if (!bitmap_bit_p (visited, bb->index))
	      localize_pseudos (bb, pseudos_to_localize, visited);
	}

      /* Recompute DF info, primarily to get accurate death notes which
	 IRA can utilize to give better allocations.

	 We could do this by hand and only in blocks of interest and
	 probably will in the future.  But for now, go with the
	 heavyweight, but clearly simple solution.  */
      df_scan_alloc (NULL);
      df_scan_blocks ();
      df_analyze ();

      /* We want to be able to call back into routines to compute costs and
	 record copies via ira_traverse_loop_tree.  Those routines typically
	 require the regno->allocno map within the loop tree structures to
	 be accurate.  So we first traverse the loop tree to free the old
	 map and set up the correct map.  */
      ira_traverse_loop_tree (true, ira_loop_tree_root,
			      fix_allocno_map, NULL);
  
      /* We may have replaced the source or destination of a "copy" with a new
	 pseudo.  Make sure the new "copies" get recorded.  This includes
	 register shuffles to satisfy constraints.  */
      ira_traverse_loop_tree (true, ira_loop_tree_root, ira_add_copies, NULL);

      /* Get costing information for any newly created pseudos.  */
      ira_costs (orig_max_reg_num);
      ira_tune_allocno_costs_and_cover_classes ();

      /* We may have allocated additional pseudos during spilling, so update
         max_regno.  ?!? Updating max_regno should really occur when we
         allocate new regs.  Or better yet, max it go away completely.  */
      max_regno = max_reg_num ();

      /* Now we want to remove each allocnos associated with the pseudos we
	 localized from the conflicts of every other allocno.  Do this once
	 after localizing in all blocks rather than in each block.  */
      EXECUTE_IF_SET_IN_BITMAP (pseudos_to_localize,
				FIRST_PSEUDO_REGISTER, i, bi)
	{
	  ira_allocno_t a = ira_regno_allocno_map[i];
	  int n = ALLOCNO_NUM_OBJECTS (ira_regno_allocno_map[i]);
	  int j;

	  for (j = 0; j < n; j++)
	    remove_from_all_conflicts (ALLOCNO_OBJECT (a, j));
	  SET_REG_N_REFS (i, 0);
	  SET_REG_N_SETS (i, 0);
	}

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

	ira_reassign_pseudos (x, n, bad_spill_regs, p, p, &z, false);

        if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
	  fprintf (ira_dump_file, "Done reassigning after localization\n");
      }

      BITMAP_FREE (pseudos_to_localize);


      /* Spill code insertion can force creation of new basic blocks.  */
      if (fixup_abnormal_edges ())
	commit_edge_insertions ();

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
