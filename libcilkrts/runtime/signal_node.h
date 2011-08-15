/* signal_node.h                  -*-C++-*-
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
 * @file signal_node.h
 *
 * @brief Signal nodes allow coordinated waking and sleeping of the runtime
 * without hammering on a single location in memory.
 *
 * The workers are logically arranged in a binary tree and propagate messages
 * leaf-ward.  User workers notify the root about waking and sleeping, so only
 * that one node need share a cache line with a user worker.
 */

#ifndef INCLUDED_SIGNAL_NODE_DOT_H
#define INCLUDED_SIGNAL_NODE_DOT_H

#include "rts-common.h"

/* Opaque type. */
typedef struct signal_node_t signal_node_t;

/**
 * Allocate and initialize a signal_node_t
 *
 * @return The initialized signal_node_t
 */
COMMON_SYSDEP
signal_node_t *signal_node_create(void);

/**
 * Free any resources and deallocate a signal_node_t
 *
 * @param node The node to be deallocated.
 */
COMMON_SYSDEP void signal_node_destroy(signal_node_t *node);

/**
 * Test whether the node thinks the worker should go to sleep
 *
 * @param node The node to be tested.
 *
 * @return 1 If the worker should go to sleep
 * @return 0 If the worker should not go to sleep
 */
COMMON_SYSDEP
unsigned int signal_node_should_wait(signal_node_t *node);

/**
 * Specify whether the worker should go to sleep
 *
 * @param node The node to be set.
 * @param msg The value to be set.  Valid values are:
 * - 0 - the worker should go to sleep
 * - 1 - the worker should stay active
 */
COMMON_SYSDEP
void signal_node_msg(signal_node_t *node, unsigned int msg);


/**
 * Wait for the node to be set
 *
 * @param node The node to wait on
 */
COMMON_SYSDEP
void signal_node_wait(signal_node_t *node);

#endif // ! defined(INCLUDED_SIGNAL_NODE_DOT_H)
