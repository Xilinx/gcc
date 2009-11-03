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

typedef struct gtm_jmpbuf
{
  unsigned long pc;
  unsigned long s[7];
  unsigned long cfa;
  unsigned long f[8];
} gtm_jmpbuf;

/* The "cacheline" as defined by the STM need not be the same as the
   cacheline defined by the processor.  It ought to be big enough for
   any of the basic types to be stored (aligned) in one line.  It ought
   to be small enough for efficient manipulation of the modification mask.  */
#define CACHELINE_SIZE 64

/* Alpha requires strict alignment for the basic types.  */
#define STRICT_ALIGNMENT 1

/* Alpha generally uses a fixed page size of 8K.  */
#define PAGE_SIZE	8192
#define FIXED_PAGE_SIZE	1
