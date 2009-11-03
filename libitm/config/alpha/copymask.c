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


#if ALLOW_UNMASKED_STORES || !defined(__alpha_bwx__)
/* If we're allowing unmasked stores, then the ZAP/ZAPNOT instructions
   provide a builtin version of the default bit_to_byte_mask operation.

   If we don't allow unmasked stores, and we don't have byte-word stores,
   then we *must* implement the copymask with ll/sc instructions.  However,
   these are slow enough that we'd rather implement this feature with the
   generic store avoidance algorithm if possible.  */

static void __attribute__((always_inline))
copy_mask_w (gtm_word * __restrict d,
	     const gtm_word * __restrict s,
	     gtm_cacheline_mask m)
{
  gtm_cacheline_mask tm = (1 << sizeof (gtm_word)) - 1;

  if (__builtin_expect (m & tm, tm))
    {
      if (__builtin_expect ((m & tm) == tm, 1))
	*d = *s;
      else if (ALLOW_UNMASKED_STORES)
	{
	  *d = (__builtin_alpha_zap (*d, m)
		| __builtin_alpha_zapnot (*s, m));
	}
      else
	{
	  gtm_word t;
	  asm ("\n0:\t"
		"ldq_l	%[t], %[d]\n\t"
		"zap	%[t], %[m], %[t]\n\t"
		"or	%[s], %[t], %[t]\n\t"
		"stq_c	%[t], %[d]\n\t"
		"beq	%[t], 0b"
		: [d] "+m" (*d), [t] "=&r" (t)
		: [m] "r" (m), [s] "r" (__builtin_alpha_zapnot (*s, m)));
	}
    }
}

void
gtm_cacheline_copy_mask (gtm_cacheline * __restrict d,
			 const gtm_cacheline * __restrict s,
			 gtm_cacheline_mask m)
{
  const size_t n = sizeof (gtm_word);
  size_t i;

  if (m == (gtm_cacheline_mask)-1)
    {
      gtm_cacheline_copy (d, s);
      return;
    }
  if (__builtin_expect (m == 0, 0))
    return;

  for (i = 0; i < CACHELINE_SIZE / n; ++i, m >>= n)
    copy_mask_w (&d->w[i], &s->w[i], m);
}

#else
# include "../../copymask.c"
#endif /* ALLOW_UNMASKED_STORES || !defined(__alpha_bwx__) */
