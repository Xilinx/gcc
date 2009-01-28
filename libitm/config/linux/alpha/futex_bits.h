/* Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libitm is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with libitm; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you link this library with other files, some
   of which are compiled with GCC, to produce an executable, this library
   does not by itself cause the resulting executable to be covered by the
   GNU General Public License.  This exception does not however invalidate
   any other reasons why the executable file might be covered by the GNU
   General Public License.  */

/* Provide target-specific access to the futex system call.  */

#ifndef SYS_futex
#define SYS_futex               394
#endif

static inline long
sys_futex0 (int *addr, long op, long val)
{
  register long sc_0 __asm__("$0");
  register long sc_16 __asm__("$16");
  register long sc_17 __asm__("$17");
  register long sc_18 __asm__("$18");
  register long sc_19 __asm__("$19");
  long res;

  sc_0 = SYS_futex;
  sc_16 = (long) addr;
  sc_17 = op;
  sc_18 = val;
  sc_19 = 0;
  __asm volatile ("callsys"
		  : "=r" (sc_0), "=r"(sc_19)
		  : "0"(sc_0), "r" (sc_16), "r"(sc_17), "r"(sc_18), "1"(sc_19)
		  : "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8",
		    "$22", "$23", "$24", "$25", "$27", "$28", "memory");

  res = sc_0;
  if (__builtin_expect (sc_19, 0))
    res = -res;
  return res;
}
