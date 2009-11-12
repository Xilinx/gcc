/* Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libitm is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "libitm_i.h"

namespace GTM HIDDEN {

struct gtm_alloc_action
{
  void (*free_fn)(void *);
  size_t size;
  bool allocated;
};


void
gtm_transaction::record_allocation (void *ptr, size_t size,
				    void (*free_fn)(void *))
{
  uintptr_t iptr = (uintptr_t) ptr;

  gtm_alloc_action *a = this->alloc_actions.find(iptr);
  if (a == 0)
    a = this->alloc_actions.insert(iptr);

  a->free_fn = free_fn;
  a->size = size;
  a->allocated = true;
}

void
gtm_transaction::forget_allocation (void *ptr, void (*free_fn)(void *))
{
  uintptr_t iptr = (uintptr_t) ptr;

  gtm_alloc_action *a = this->alloc_actions.find(iptr);
  if (a == 0)
    a = this->alloc_actions.insert(iptr);

  a->free_fn = free_fn;
  a->size = 0;
  a->allocated = false;
}

size_t
gtm_transaction::get_allocation_size (void *ptr)
{
  uintptr_t iptr = (uintptr_t) ptr;

  gtm_alloc_action *a = this->alloc_actions.find(iptr);
  return a ? a->size : 0;
}

static void
commit_allocations_1 (uintptr_t key, gtm_alloc_action *a, void *cb_data)
{
  void *ptr = (void *)key;
  uintptr_t revert_p = (uintptr_t) cb_data;

  if (a->allocated == revert_p)
    a->free_fn (ptr);
}

void
gtm_transaction::commit_allocations (bool revert_p)
{
  this->alloc_actions.traverse (commit_allocations_1,
				(void *)(uintptr_t)revert_p);
  this->alloc_actions.clear ();
}

} // namespace GTM
