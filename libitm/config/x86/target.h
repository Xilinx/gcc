/* Copyright (C) 2008, 2009 Free Software Foundation, Inc.
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

#ifdef __LP64__

struct gtm_jmpbuf
{
  unsigned long cfa;
  unsigned long i[7];
};

#else

struct gtm_jmpbuf
{
  unsigned long cfa;
  unsigned long i[5];
};

#define REGPARM		__attribute__((regparm(2)))

#endif

/* ??? The 32-bit compiler still crashes on this.  */
#ifdef __LP64__

/* Define platform-specific additions to the library type list.  */
typedef int _ITM_TYPE_M64
  __attribute__ ((__vector_size__ (8), __may_alias__));
typedef float _ITM_TYPE_M128
  __attribute__ ((__vector_size__ (16), __may_alias__));
typedef float _ITM_TYPE_M256
  __attribute__ ((__vector_size__ (32), __may_alias__));

#define _ITM_ALL_TARGET_TYPES(M)	M(M64) M(M128) M(M256)

#define _ITM_ATTR_U1
#define _ITM_ATTR_U2
#define _ITM_ATTR_U4
#define _ITM_ATTR_U8
#define _ITM_ATTR_F
#define _ITM_ATTR_D
#define _ITM_ATTR_E
#define _ITM_ATTR_CF
#define _ITM_ATTR_CD
#define _ITM_ATTR_CE
#ifdef __MMX__
# define _ITM_ATTR_M64
#else
# define _ITM_ATTR_M64		__attribute__((target("mmx")))
#endif
#ifdef __SSE__
# define _ITM_ATTR_M128
#else
# define _ITM_ATTR_M128		__attribute__((target("sse")))
#endif
#ifdef __AVX__
# define _ITM_ATTR_M256
#else
# define _ITM_ATTR_M256		__attribute__((target("avx")))
#endif

#define _ITM_TYPE_ATTR(T)	_ITM_ATTR_##T

#endif

static inline void
cpu_relax (void)
{
  __asm volatile ("rep; nop" : : : "memory");
}

static inline void
atomic_write_barrier (void)
{
#if defined(__SSE__) || defined(__3dNOW_A__)
  __builtin_ia32_sfence ();
#else
  __sync_synchronize ();
#endif
}
