/* pedigrees.h                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2009-2012 
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
 **************************************************************************/

#ifndef INCLUDED_PEDIGREES_DOT_H
#define INCLUDED_PEDIGREES_DOT_H


#include <cilk/common.h>
#include <internal/abi.h>

#include "rts-common.h"
#include "global_state.h"
#include "os.h"

__CILKRTS_BEGIN_EXTERN_C

/**
 * @file pedigrees.h
 *
 * @brief pedigrees.h declares common routines related to pedigrees
 * and the pedigree API.
 */


/**
 * @brief Sets the leaf pedigree node for the current user thread.
 *
 * A typical implementation stores this pedigree node in thread-local
 * storage.
 *
 * Preconditions:
 *  - Current thread should be a user thread.
 *
 * @param leaf The pedigree node to store as a leaf.
 */
COMMON_PORTABLE
void __cilkrts_set_pedigree_leaf(__cilkrts_pedigree* leaf);


/**
 * Load the pedigree leaf node from thread-local storage into the
 * current user worker.  This method should execute as a part of
 * binding the user thread to a worker.
 *
 * Preconditions:
 *  
 *  - w should be the worker for the current thread 
 *  - w should be a user thread.
 */
COMMON_PORTABLE
void load_pedigree_leaf_into_user_worker(__cilkrts_worker *w);

/**
 * Save the pedigree leaf node from the worker into thread-local
 * storage.  This method should execute as part of unbinding a user
 * thread from a worker.
 *
 * Preconditions:
 *  
 *  - w should be the worker for the current thread 
 *  - w should be a user thread.
 */
COMMON_PORTABLE
void save_pedigree_leaf_from_user_worker(__cilkrts_worker *w);



/**
 * Update pedigree for a worker when leaving a frame.
 *
 * If this is the frame of a spawn helper (indicated by the
 *  CILK_FRAME_DETACHED flag) we must update the pedigree.  The
 *  pedigree points to nodes allocated on the stack.  Failing to
 *  update it will result in a accvio/segfault if the pedigree is
 *  walked.  This must happen for all spawn helper frames, even if
 *  we're processing an exception.
 */ 
COMMON_PORTABLE
inline void update_pedigree_on_leave_frame(__cilkrts_worker *w,
					   __cilkrts_stack_frame *sf) 
{
    // Update the worker's pedigree information if this is an ABI 1 or later
    // frame
    if (CILK_FRAME_VERSION_VALUE(sf->flags) >= 1)
    {
	w->pedigree.rank = sf->spawn_helper_pedigree.rank + 1;
	w->pedigree.parent = sf->spawn_helper_pedigree.parent;
    }
}



__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_PEDIGREES_DOT_H)
