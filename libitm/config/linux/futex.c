/* Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with libgomp; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you link this library with other files, some
   of which are compiled with GCC, to produce an executable, this library
   does not by itself cause the resulting executable to be covered by the
   GNU General Public License.  This exception does not however invalidate
   any other reasons why the executable file might be covered by the GNU
   General Public License.  */

/* Provide access to the futex system call.  */

#include "libitm.h"
#include "futex.h"
#include <errno.h>


#define FUTEX_WAIT		0
#define FUTEX_WAKE		1
#define FUTEX_PRIVATE_FLAG	128L


static long int gtm_futex_wait = FUTEX_WAIT | FUTEX_PRIVATE_FLAG;
static long int gtm_futex_wake = FUTEX_WAKE | FUTEX_PRIVATE_FLAG;


void REGPARM
futex_wait (int *addr, int val)
{
  unsigned long long i, count = gtm_spin_count_var;
  long res;

  for (i = 0; i < count; i++)
    if (__builtin_expect (*addr != val, 0))
      return;
    else
      cpu_relax ();

  res = sys_futex0 (addr, gtm_futex_wait, val);
  if (__builtin_expect (res == -ENOSYS, 0))
    {
      gtm_futex_wait = FUTEX_WAIT;
      gtm_futex_wake = FUTEX_WAKE;
      sys_futex0 (addr, FUTEX_WAIT, val);
    }
}


void REGPARM
futex_wake (int *addr, int count)
{
  long res = sys_futex0 (addr, gtm_futex_wake, count);
  if (__builtin_expect (res == -ENOSYS, 0))
    {
      gtm_futex_wait = FUTEX_WAIT;
      gtm_futex_wake = FUTEX_WAKE;
      sys_futex0 (addr, FUTEX_WAKE, count);
    }
}
