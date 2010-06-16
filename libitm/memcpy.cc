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

using namespace GTM;

static void
do_memcpy (uintptr_t idst, uintptr_t isrc, size_t size,
	   gtm_dispatch::lock_type W, gtm_dispatch::lock_type R)
{
  gtm_dispatch *disp = gtm_disp();
  uintptr_t dofs = idst & (CACHELINE_SIZE - 1);
  uintptr_t sofs = isrc & (CACHELINE_SIZE - 1);
  const gtm_cacheline *src
    = reinterpret_cast<const gtm_cacheline *>(isrc & -CACHELINE_SIZE);
  gtm_cacheline *dst
    = reinterpret_cast<gtm_cacheline *>(idst & -CACHELINE_SIZE);
  const gtm_cacheline *sline;
  gtm_dispatch::mask_pair dpair;

  if (size == 0)
    return;

  if (dofs == sofs)
    {
      if (sofs != 0)
	{
	  size_t sleft = CACHELINE_SIZE - sofs;
	  size_t min = (size <= sleft ? size : sleft);

	  dpair = disp->write_lock(dst, W);
	  sline = disp->read_lock(src, R);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << min) - 1) << sofs;
	  memcpy (&dpair.line->b[sofs], &sline->b[sofs], min);
	  dst++;
	  src++;
	  size -= min;
	}

      while (size >= CACHELINE_SIZE)
	{
	  dpair = disp->write_lock(dst, W);
	  sline = disp->read_lock(src, R);
	  *dpair.mask = -1;
	  *dpair.line = *sline;
	  dst++;
	  src++;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  dpair = disp->write_lock(dst, W);
	  sline = disp->read_lock(src, R);
	  *dpair.mask |= ((gtm_cacheline_mask)1 << size) - 1;
	  memcpy (dpair.line, sline, size);
	}
    }
  else
    {
      gtm_cacheline c;
      size_t sleft = CACHELINE_SIZE - sofs;

      sline = disp->read_lock(src, R);
      if (dofs != 0)
	{
	  size_t dleft = CACHELINE_SIZE - dofs;
	  size_t min = (size <= dleft ? size : dleft);

	  dpair = disp->write_lock(dst, W);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << min) - 1) << dofs;
	  if (min <= sleft)
	    {
	      memcpy (&dpair.line->b[dofs], &sline->b[sofs], min);
	      sofs += min;
	    }
	  else
	    {
	      memcpy (&c, &sline->b[sofs], sleft);
	      sline = disp->read_lock(++src, R);
	      sofs = min - sleft;
	      memcpy (&c.b[sleft], sline, sofs);
	      memcpy (&dpair.line->b[dofs], &c, min);
	    }
	  sleft = CACHELINE_SIZE - sofs;

	  dst++;
	  size -= min;
	}

      while (size >= CACHELINE_SIZE)
	{
	  memcpy (&c, &sline->b[sofs], sleft);
	  sline = disp->read_lock(++src, R);
	  memcpy (&c.b[sleft], sline, sofs);

	  dpair = disp->write_lock(dst, W);
	  *dpair.mask = -1;
	  *dpair.line = c;

	  dst++;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  dpair = disp->write_lock(dst, W);
	  *dpair.mask |= ((gtm_cacheline_mask)1 << size) - 1;
	  if (size <= sleft)
	    memcpy (dpair.line, &sline->b[sofs], size);
	  else
	    {
	      memcpy (&c, &sline->b[sofs], sleft);
	      sline = disp->read_lock(++src, R);
	      memcpy (&c.b[sleft], sline, size - sleft);
	      memcpy (dpair.line, &c, size);
	    }
	}
    }
}

static void
do_memmove (uintptr_t idst, uintptr_t isrc, size_t size,
	    gtm_dispatch::lock_type W, gtm_dispatch::lock_type R)
{
  gtm_dispatch *disp = gtm_disp();
  uintptr_t dleft, sleft, sofs, dofs;
  const gtm_cacheline *sline;
  gtm_dispatch::mask_pair dpair;
  
  if (size == 0)
    return;

  /* The co-aligned memmove below doesn't work for DST == SRC, so filter
     that out.  It's tempting to just return here, as this is a no-op move.
     However, our caller has the right to expect the locks to be acquired
     as advertized.  */
  if (__builtin_expect (idst == isrc, 0))
    {
      /* If the write lock is already acquired, nothing to do.  */
      if (W == gtm_dispatch::WaW)
	return;
      /* If the destination is protected, acquire a write lock.  */
      if (W != gtm_dispatch::NOLOCK)
	R = gtm_dispatch::RfW;
      /* Notice serial mode, where we don't acquire locks at all.  */
      if (R == gtm_dispatch::NOLOCK)
	return;

      idst = isrc + size;
      for (isrc &= -CACHELINE_SIZE; isrc < idst; isrc += CACHELINE_SIZE)
	disp->read_lock(reinterpret_cast<const gtm_cacheline *>(isrc), R);
      return;
    }

  /* Fall back to memcpy if the implementation above can handle it.  */
  if (idst < isrc || isrc + size <= idst)
    {
      do_memcpy (idst, isrc, size, W, R);
      return;
    }

  /* What remains requires a backward copy from the end of the blocks.  */
  idst += size;
  isrc += size;
  dofs = idst & (CACHELINE_SIZE - 1);
  sofs = isrc & (CACHELINE_SIZE - 1);
  dleft = CACHELINE_SIZE - dofs;
  sleft = CACHELINE_SIZE - sofs;

  gtm_cacheline *dst
    = reinterpret_cast<gtm_cacheline *>(idst & -CACHELINE_SIZE);
  const gtm_cacheline *src
    = reinterpret_cast<const gtm_cacheline *>(isrc & -CACHELINE_SIZE);
  if (dofs == 0)
    dst--;
  if (sofs == 0)
    src--;

  if (dofs == sofs)
    {
      /* Since DST and SRC are co-aligned, and we didn't use the memcpy
	 optimization above, that implies that SIZE > CACHELINE_SIZE.  */
      if (sofs != 0)
	{
	  dpair = disp->write_lock(dst, W);
	  sline = disp->read_lock(src, R);
	  *dpair.mask |= ((gtm_cacheline_mask)1 << sleft) - 1;
	  memcpy (dpair.line, sline, sleft);
	  dst--;
	  src--;
	  size -= sleft;
	}

      while (size >= CACHELINE_SIZE)
	{
	  dpair = disp->write_lock(dst, W);
	  sline = disp->read_lock(src, R);
	  *dpair.mask = -1;
	  *dpair.line = *sline;
	  dst--;
	  src--;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  size_t ofs = CACHELINE_SIZE - size;
	  dpair = disp->write_lock(dst, W);
	  sline = disp->read_lock(src, R);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << size) - 1) << ofs;
	  memcpy (&dpair.line->b[ofs], &sline->b[ofs], size);
	}
    }
  else
    {
      gtm_cacheline c;

      sline = disp->read_lock(src, R);
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
	      sline = disp->read_lock(--src, R);
	      sofs = CACHELINE_SIZE - min_ofs;
	      memcpy (&c, &sline->b[sofs], min_ofs);
	    }

	  dofs = dleft - min;
	  dpair = disp->write_lock(dst, W);
	  *dpair.mask |= (((gtm_cacheline_mask)1 << min) - 1) << dofs;
	  memcpy (&dpair.line->b[dofs], &c, min);

	  sleft = CACHELINE_SIZE - sofs;
	  dst--;
	  size -= min;
	}

      while (size >= CACHELINE_SIZE)
	{
	  memcpy (&c.b[sleft], sline, sofs);
	  sline = disp->read_lock(--src, R);
	  memcpy (&c, &sline->b[sofs], sleft);

	  dpair = disp->write_lock(dst, W);
	  *dpair.mask = -1;
	  *dpair.line = c;

	  dst--;
	  size -= CACHELINE_SIZE;
	}

      if (size != 0)
	{
	  dofs = CACHELINE_SIZE - size;

	  memcpy (&c.b[sleft], sline, sofs);
	  if (sleft > dofs)
	    {
	      sline = disp->read_lock(--src, R);
	      memcpy (&c, &sline->b[sofs], sleft);
	    }

	  dpair = disp->write_lock(dst, W);
	  *dpair.mask |= (gtm_cacheline_mask)-1 << dofs;
	  memcpy (&dpair.line->b[dofs], &c.b[dofs], size);
	}
    }
}

#define ITM_MEM_DEF(NAME, READ, WRITE) \
void ITM_REGPARM _ITM_memcpy##NAME(void *dst, const void *src, size_t size)  \
{									     \
  gtm_stack_marker marker;						     \
  do_memcpy ((uintptr_t)dst, (uintptr_t)src, size,			     \
	     gtm_dispatch::WRITE, gtm_dispatch::READ);			     \
}									     \
void ITM_REGPARM _ITM_memmove##NAME(void *dst, const void *src, size_t size) \
{									     \
  gtm_stack_marker marker;						     \
  do_memmove ((uintptr_t)dst, (uintptr_t)src, size,			     \
	      gtm_dispatch::WRITE, gtm_dispatch::READ);			     \
}

ITM_MEM_DEF(RnWt,	NOLOCK,		W)
ITM_MEM_DEF(RnWtaR,	NOLOCK,		WaR)
ITM_MEM_DEF(RnWtaW,	NOLOCK,		WaW)

ITM_MEM_DEF(RtWn,	R,		NOLOCK)
ITM_MEM_DEF(RtWt,	R,		W)
ITM_MEM_DEF(RtWtaR,	R,		WaR)
ITM_MEM_DEF(RtWtaW,	R,		WaW)

ITM_MEM_DEF(RtaRWn,	RaR,		NOLOCK)
ITM_MEM_DEF(RtaRWt,	RaR,		W)
ITM_MEM_DEF(RtaRWtaR,	RaR,		WaR)
ITM_MEM_DEF(RtaRWtaW,	RaR,		WaW)

ITM_MEM_DEF(RtaWWn,	RaW,		NOLOCK)
ITM_MEM_DEF(RtaWWt,	RaW,		W)
ITM_MEM_DEF(RtaWWtaR,	RaW,		WaR)
ITM_MEM_DEF(RtaWWtaW,	RaW,		WaW)
