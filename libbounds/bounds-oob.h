/* Out-of-bounds pointer representation for libbounds.
   Copyright (C) 2007, 2008 Free Software Foundation, Inc.
   Contributed by Alexander Lamaison <awl03@doc.ic.ac.uk>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#ifndef _BOUNDS_OOB_H
#define _BOUNDS_OOB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
typedef unsigned int uintptr_t __attribute__ ((__mode__ (__pointer__)));
#endif

/* Out-of-bounds object.  */
typedef struct __bounds_oob_ptr
{
  uintptr_t ptr;		/* Current pointer value. */
  uintptr_t last_valid;		/* Value of pointer before going out-of-bounds.  */
  const char *arith_location;	/* Location where pointer went out-of-bounds.  */

} __bounds_oob_ptr_t;

extern void *__bounds_create_oob (void *, void *, const char *);
extern void __bounds_destroy_oob (void *);
extern __bounds_oob_ptr_t *__bounds_get_oob (void *);

#endif /* _BOUNDS_OOB_H */

