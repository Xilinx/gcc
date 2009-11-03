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

#include "libitm_i.h"


static void
do_memcpy (uintptr_t dst, uintptr_t src, size_t size,
	   gtm_write_lock_fn W, gtm_read_lock_fn R)
{
  uintptr_t dofs = dst & (CACHELINE_SIZE - 1);
  uintptr_t sofs = src & (CACHELINE_SIZE - 1);
  gtm_cacheline *sline;
  gtm_cacheline_mask_pair dpair;

  if (size == 0)
    return;

  dst &= -CACHELINE_SIZE;
  src &= -CACHELINE_SIZE;

  if (dofs == sofs)
    {
      if (sofs != 0)
	{
	  size_t sleft = CACHELINE_SIZE - sofs;
	  size_t min = (size <= sleft ? size : sleft);

	  dpair = W(dst);
	  sline = R(src);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << min) - 1) << sofs;
	  memcpy (&dpair.line->b[sofs], &sline->b[sofs], min);
	  dst += CACHELINE_SIZE;
	  src += CACHELINE_SIZE;
	  size -= min;
	}

      while (size >= CACHELINE_SIZE)
	{
	  dpair = W(dst);
	  sline = R(src);
	  *dpair.mask = -1;
	  gtm_cacheline_copy (dpair.line, sline);
	  dst += CACHELINE_SIZE;
	  src += CACHELINE_SIZE;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  dpair = W(dst);
	  sline = R(src);
	  *dpair.mask |= ((gtm_cacheline_mask)1 << size) - 1;
	  memcpy (dpair.line, sline, size);
	}
    }
  else
    {
      gtm_cacheline c;
      size_t sleft = CACHELINE_SIZE - sofs;

      sline = R(src);
      if (dofs != 0)
	{
	  size_t dleft = CACHELINE_SIZE - dofs;
	  size_t min = (size <= dleft ? size : dleft);

	  dpair = W(dst);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << min) - 1) << dofs;
	  if (min <= sleft)
	    {
	      memcpy (&dpair.line->b[dofs], &sline->b[sofs], min);
	      sofs += min;
	    }
	  else
	    {
	      memcpy (&c, &sline->b[sofs], sleft);
	      src += CACHELINE_SIZE;
	      sline = R(src);
	      sofs = min - sleft;
	      memcpy (&c.b[sleft], sline, sofs);
	      memcpy (&dpair.line->b[dofs], &c, min);
	    }
	  sleft = CACHELINE_SIZE - sofs;

	  dst += CACHELINE_SIZE;
	  size -= min;
	}

      while (size >= CACHELINE_SIZE)
	{
	  memcpy (&c, &sline->b[sofs], sleft);
	  src += CACHELINE_SIZE;
	  sline = R(src);
	  memcpy (&c.b[sleft], sline, sofs);

	  dpair = W(dst);
	  *dpair.mask = -1;
	  gtm_cacheline_copy (dpair.line, &c);

	  dst += CACHELINE_SIZE;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  dpair = W(dst);
	  *dpair.mask |= ((gtm_cacheline_mask)1 << size) - 1;
	  if (size <= sleft)
	    memcpy (dpair.line, &sline->b[sofs], size);
	  else
	    {
	      memcpy (&c, &sline->b[sofs], sleft);
	      src += CACHELINE_SIZE;
	      sline = R(src);
	      memcpy (&c.b[sleft], sline, size - sleft);
	      memcpy (dpair.line, &c, size);
	    }
	}
    }
}

static void
do_memmove (uintptr_t dst, uintptr_t src, size_t size,
	    gtm_write_lock_fn W, gtm_read_lock_fn R)
{
  uintptr_t dleft, sleft, sofs, dofs;
  gtm_cacheline *sline;
  gtm_cacheline_mask_pair dpair;
  
  if (size == 0)
    return;

  /* The co-aligned memmove below doesn't work for DST == SRC, so filter
     that out.  It's tempting to just return here, as this is a no-op move.
     However, our caller has the right to expect the locks to be acquired
     as advertized.  */
  if (__builtin_expect (dst == src, 0))
    {
      const gtm_dispatch *disp = gtm_disp();

      /* If the write lock is already acquired, nothing to do.  */
      if (W == disp->WaW)
	return;
      /* If the destination is protected, acquire a write lock.  */
      if (W != GTM_null_write_lock)
	R = disp->RfW;
      /* Notice serial mode, where we don't acquire locks at all.  */
      if (R == GTM_null_read_lock)
	return;

      dst = src + size;
      for (src &= -CACHELINE_SIZE; src < dst; src += CACHELINE_SIZE)
	R(src);
      return;
    }

  /* Fall back to memcpy if the implementation above can handle it.  */
  if (dst < src || src + size <= dst)
    {
      do_memcpy (dst, src, size, W, R);
      return;
    }

  /* What remains requires a backward copy from the end of the blocks.  */
  dst += size;
  src += size;
  dofs = dst & (CACHELINE_SIZE - 1);
  sofs = src & (CACHELINE_SIZE - 1);
  dleft = CACHELINE_SIZE - dofs;
  sleft = CACHELINE_SIZE - sofs;
  dst &= -CACHELINE_SIZE;
  src &= -CACHELINE_SIZE;
  if (dofs == 0)
    dst -= CACHELINE_SIZE;
  if (sofs == 0)
    src -= CACHELINE_SIZE;

  if (dofs == sofs)
    {
      /* Since DST and SRC are co-aligned, and we didn't use the memcpy
	 optimization above, that implies that SIZE > CACHELINE_SIZE.  */
      if (sofs != 0)
	{
	  dpair = W(dst);
	  sline = R(src);
	  *dpair.mask |= ((gtm_cacheline_mask)1 << sleft) - 1;
	  memcpy (dpair.line, sline, sleft);
	  dst -= CACHELINE_SIZE;
	  src -= CACHELINE_SIZE;
	  size -= sleft;
	}

      while (size >= CACHELINE_SIZE)
	{
	  dpair = W(dst);
	  sline = R(src);
	  *dpair.mask = -1;
	  gtm_cacheline_copy (dpair.line, sline);
	  dst -= CACHELINE_SIZE;
	  src -= CACHELINE_SIZE;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  size_t ofs = CACHELINE_SIZE - size;
	  dpair = W(dst);
	  sline = R(src);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << size) - 1) << ofs;
	  memcpy (&dpair.line->b[ofs], &sline->b[ofs], size);
	}
    }
  else
    {
      gtm_cacheline c;

      sline = R(src);
      if (dofs != 0)
	{
	  size_t min = (size <= dofs ? size : dofs);

	  if (min <= sofs)
	    {
	      sofs -= min;
	      memcpy (&c, &sline->b[sofs], min);
	    }
	  else
	    {
	      size_t min_ofs = min - sofs;
	      memcpy (&c.b[min_ofs], sline, sofs);
	      src -= CACHELINE_SIZE;
	      sline = R(src);
	      sofs = CACHELINE_SIZE - min_ofs;
	      memcpy (&c, &sline->b[sofs], min_ofs);
	    }

	  dofs = dleft - min;
	  dpair = W(dst);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << min) - 1) << dofs;
	  memcpy (&dpair.line->b[dofs], &c, min);

	  sleft = CACHELINE_SIZE - sofs;
	  dst -= CACHELINE_SIZE;
	  size -= min;
	}

      while (size >= CACHELINE_SIZE)
	{
	  memcpy (&c.b[sleft], sline, sofs);
	  src -= CACHELINE_SIZE;
	  sline = R(src);
	  memcpy (&c, &sline->b[sofs], sleft);

	  dpair = W(dst);
	  *dpair.mask = -1;
	  gtm_cacheline_copy (dpair.line, &c);

	  dst -= CACHELINE_SIZE;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  dofs = CACHELINE_SIZE - size;

	  memcpy (&c.b[sleft], sline, sofs);
	  if (sleft > dofs)
	    {
	      src -= CACHELINE_SIZE;
	      sline = R(src);
	      memcpy (&c, &sline->b[sofs], sleft);
	    }

	  dpair = W(dst);
	  *dpair.mask |= (gtm_cacheline_mask)-1 << dofs;
	  memcpy (&dpair.line->b[dofs], &c.b[dofs], size);
	}
    }
}

#define ITM_MEM_DEF(NAME, READ, WRITE) \
void ITM_REGPARM _ITM_memcpy##NAME(void *dst, const void *src, size_t size)  \
{									     \
  const gtm_dispatch *disp = gtm_disp();				     \
  do_memcpy ((uintptr_t)dst, (uintptr_t)src, size, WRITE, READ);	     \
}									     \
void ITM_REGPARM _ITM_memmove##NAME(void *dst, const void *src, size_t size) \
{									     \
  const gtm_dispatch *disp = gtm_disp();				     \
  do_memmove ((uintptr_t)dst, (uintptr_t)src, size, WRITE, READ);	     \
}

ITM_MEM_DEF(RnWt,	GTM_null_read_lock,	disp->W)
ITM_MEM_DEF(RnWtaR,	GTM_null_read_lock,	disp->WaR)
ITM_MEM_DEF(RnWtaW,	GTM_null_read_lock,	disp->WaW)

ITM_MEM_DEF(RtWn,	disp->R,		GTM_null_write_lock)
ITM_MEM_DEF(RtWt,	disp->R,		disp->W)
ITM_MEM_DEF(RtWtaR,	disp->R,		disp->WaR)
ITM_MEM_DEF(RtWtaW,	disp->R,		disp->WaW)

ITM_MEM_DEF(RtaRWn,	disp->RaR,		GTM_null_write_lock)
ITM_MEM_DEF(RtaRWt,	disp->RaR,		disp->W)
ITM_MEM_DEF(RtaRWtaR,	disp->RaR,		disp->WaR)
ITM_MEM_DEF(RtaRWtaW,	disp->RaR,		disp->WaW)

ITM_MEM_DEF(RtaWWn,	disp->RaW,		GTM_null_write_lock)
ITM_MEM_DEF(RtaWWt,	disp->RaW,		disp->W)
ITM_MEM_DEF(RtaWWtaR,	disp->RaW,		disp->WaR)
ITM_MEM_DEF(RtaWWtaW,	disp->RaW,		disp->WaW)
