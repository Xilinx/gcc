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


#define ITM_READ(T, LOCK) \
_ITM_TYPE_##T ITM_REGPARM _ITM_##LOCK##T(const _ITM_TYPE_##T *ptr)	\
{									\
  uintptr_t iptr = (uintptr_t) ptr;					\
  uintptr_t iline = iptr & -CACHELINE_SIZE;				\
  uintptr_t iofs = iptr & (CACHELINE_SIZE - 1);				\
  gtm_cacheline *line = gtm_disp()->LOCK (iline);			\
  _ITM_TYPE_##T ret;							\
									\
  if (STRICT_ALIGNMENT							\
      ? (iofs & (sizeof (ret) - 1)) == 0				\
      : iofs + sizeof(ret) <= CACHELINE_SIZE)				\
    {									\
      return *(_ITM_TYPE_##T *)&line->b[iofs];				\
    }									\
  else if (STRICT_ALIGNMENT && iofs + sizeof(ret) <= CACHELINE_SIZE)	\
    {									\
      memcpy (&ret, &line->b[iofs], sizeof (ret));			\
    }									\
  else									\
    {									\
      uintptr_t ileft = CACHELINE_SIZE - iofs;				\
      memcpy (&ret, &line->b[iofs], ileft);				\
      line = gtm_disp()->LOCK (iline + CACHELINE_SIZE);			\
      memcpy ((char *)&ret + ileft, line, sizeof(ret) - ileft);		\
    }									\
  return ret;								\
}

#define ITM_WRITE(T, LOCK) \
void ITM_REGPARM _ITM_##LOCK##T(_ITM_TYPE_##T *ptr, _ITM_TYPE_##T val)	\
{									\
  uintptr_t iptr = (uintptr_t) ptr;					\
  uintptr_t iline = iptr & -CACHELINE_SIZE;				\
  uintptr_t iofs = iptr & (CACHELINE_SIZE - 1);				\
  gtm_cacheline_mask m = ((gtm_cacheline_mask)1 << sizeof(val)) - 1;	\
  gtm_cacheline_mask_pair pair = gtm_disp()->LOCK (iline);		\
									\
  if (STRICT_ALIGNMENT							\
      ? (iofs & (sizeof (val) - 1)) == 0				\
      : iofs + sizeof(val) <= CACHELINE_SIZE)				\
    {									\
      *(_ITM_TYPE_##T *)&pair.line->b[iofs] = val;			\
      *pair.mask |= m << iofs;						\
    }									\
  else if (STRICT_ALIGNMENT && iofs + sizeof(val) <= CACHELINE_SIZE)	\
    {									\
      memcpy (&pair.line->b[iofs], &val, sizeof (val));			\
      *pair.mask |= m << iofs;						\
    }									\
  else									\
    {									\
      uintptr_t ileft = CACHELINE_SIZE - iofs;				\
      memcpy (&pair.line->b[iofs], &val, ileft);			\
      *pair.mask |= m << iofs;						\
      pair = gtm_disp()->LOCK (iline + CACHELINE_SIZE);			\
      memcpy (pair.line, (char *)&val + ileft, sizeof(val) - ileft);	\
      *pair.mask |= m >> ileft;						\
    }									\
}

#define ITM_BARRIERS(T)		\
  ITM_READ(T, R)		\
  ITM_READ(T, RaR)		\
  ITM_READ(T, RaW)		\
  ITM_READ(T, RfW)		\
  ITM_WRITE(T, W)		\
  ITM_WRITE(T, WaR)		\
  ITM_WRITE(T, WaW)

ITM_BARRIERS(U1)
ITM_BARRIERS(U2)
ITM_BARRIERS(U4)
ITM_BARRIERS(U8)
ITM_BARRIERS(F)
ITM_BARRIERS(D)
ITM_BARRIERS(E)
ITM_BARRIERS(CF)
ITM_BARRIERS(CD)
ITM_BARRIERS(CE)
