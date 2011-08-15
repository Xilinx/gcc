/* symbol_test.c                  -*-C-*-
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

/* simple program to verify that there are no undefined symbols in the runtime.
 * If the runtime uses any symbols that are not defined, compiling this program
 * will cause a linker error.
 */

extern void* __cilkrts_global_state;
void *volatile p;

void foo () { }
int main ()
{
    int i;
    long long j;

    _Cilk_spawn foo();
    _Cilk_for (i = 0; i < 2; ++i)
        foo();
    _Cilk_for (j = 0; j < 2; ++j)
        foo();
    p = __cilkrts_global_state;
    return 0;
}

/* End symbol_test.c */
