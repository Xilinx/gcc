/* reducer_impl.h                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2009-2011 
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

/**
 * @file reducer_impl.h
 *
 * @brief Functions to implement reducers in the runtime.
 */

#ifndef INCLUDED_REDUCER_IMPL_DOT_H
#define INCLUDED_REDUCER_IMPL_DOT_H

#include <cilk/common.h>
#include <internal/abi.h>
#include "rts-common.h"

__CILKRTS_BEGIN_EXTERN_C

typedef struct cilkred_map cilkred_map;

/**
 * Construct an empty reducer map from the memory pool associated with the
 * given worker.  This reducer map must be destroyed before the worker's
 * associated global context is destroyed.
 *
 * @param w __cilkrts_worker the cilkred_map is being created for.
 *
 * @return Pointer to the initialized cilkred_map.
 */
COMMON_SYSDEP
cilkred_map *__cilkrts_make_reducer_map(__cilkrts_worker *w);

/**
 * Destroy a reducer map.  The map must have been allocated from the worker's
 * global context and should have been allocated from the same worker.
 *
 * @param w __cilkrts_worker the cilkred_map was created for.
 * @param h The cilkred_map to be deallocated.
 */
COMMON_SYSDEP
void __cilkrts_destroy_reducer_map(__cilkrts_worker *w,
                                   cilkred_map *h);

/**
 * Set the specified reducer map as the leftmost map if is_leftmost is true,
 * otherwise, set it to not be the leftmost map.
 *
 * @param h The cilkred_map to be modified.
 * @param is_leftmost true if the reducer map is leftmost.
 */
COMMON_SYSDEP
void __cilkrts_set_leftmost_reducer_map(cilkred_map *h,
                                        int is_leftmost);

/**
 * Merge reducer map RIGHT_MAP into LEFT_MAP and return the result of the
 * merge.  Both maps must be allocated from the global context associated
 * with the specified worker.  The  returned reducer map must be destroyed
 * before the worker's associated global context is destroyed.
 *
 * If two cilkred_maps are specified, one will be destroyed and the other
 * one will be returned as the merged cilkred_map.
 *
 * @param w __cilkrts_worker that owns the cilkred_maps.
 * @param left_map The left cilkred_map.
 * @param right_map The right cilkred_map.
 *
 * @return pointer to merged cilkred_map.
 */
extern
cilkred_map *__cilkrts_merge_reducer_maps(__cilkrts_worker *w,
                                          cilkred_map *left_map,
                                          cilkred_map *right_map);

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_REDUCER_IMPL_DOT_H)
