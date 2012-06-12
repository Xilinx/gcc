/* pedigrees.c                  -*-C-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2007-2012 
 * Intel Corporation
 * 
 * This file is part of the Intel Cilk Plus Library.  This library is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * Under Section 7 of GPL version 3, you are granted additional
 * permissions described in the GCC Runtime Library Exception, version
 * 3.1, as published by the Free Software Foundation.
 * 
 * You should have received a copy of the GNU General Public License and
 * a copy of the GCC Runtime Library Exception along with this program;
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/

#include "pedigrees.h"
#include "local_state.h"

/*************************************************************
  Pedigree API code.
*************************************************************/

/*
 * C99 requires that every inline function with external linkage have one
 * extern declaration in the program (with the inline definition in scope).
 */
COMMON_PORTABLE
extern void update_pedigree_on_leave_frame(__cilkrts_worker *w,
					   __cilkrts_stack_frame *sf);

void __cilkrts_set_pedigree_leaf(__cilkrts_pedigree *leaf)
{
    __cilkrts_set_tls_pedigree_leaf(leaf);
}

void load_pedigree_leaf_into_user_worker(__cilkrts_worker *w)
{
    __cilkrts_pedigree *pedigree_leaf;
    CILK_ASSERT(w->l->type == WORKER_USER);
    pedigree_leaf = __cilkrts_get_tls_pedigree_leaf(1);
    w->pedigree = *pedigree_leaf;

    // Save a pointer to the old leaf.
    // We'll need to restore it later.
    CILK_ASSERT(w->l->original_pedigree_leaf == NULL);
    w->l->original_pedigree_leaf = pedigree_leaf;
    
    __cilkrts_set_tls_pedigree_leaf(&w->pedigree);
    
    // Check that this new pedigree root has at least two values.
    CILK_ASSERT(w->pedigree.parent);
    CILK_ASSERT(w->pedigree.parent->parent == NULL);
}

void save_pedigree_leaf_from_user_worker(__cilkrts_worker *w)
{
    CILK_ASSERT(w->l->type == WORKER_USER);

    // Existing leaf in tls should be for the current worker.
    // This assert is expensive to check though.
    // CILK_ASSERT(&w->pedigree == __cilkrts_get_tls_pedigree_leaf(0));
    CILK_ASSERT(w->l->original_pedigree_leaf);

    // w should finish with a pedigree node that points to 
    // the same root that we just looked up.

    // TODO: This assert should be valid.
    // But we are removing it now to make exceptions (without pedigrees) work.
    // Currently, reading the pedigree after an exception is caught
    // fails because the pedigree chain not restored correctly. 
    // CILK_ASSERT(w->l->original_pedigree_leaf->next == w->pedigree.parent);
    w->l->original_pedigree_leaf->rank = w->pedigree.rank;

    // Save that leaf pointer back into tls.
    __cilkrts_set_tls_pedigree_leaf(w->l->original_pedigree_leaf);
    // Null out worker's leaf for paranoia.
    w->l->original_pedigree_leaf = NULL;
}



/*
  Local Variables: **
  c-file-style:"bsd" **
  c-basic-offset:4 **
  indent-tabs-mode:nil **
  End: **
*/
