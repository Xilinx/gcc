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

#include "libitm.h"


#define _ITM_READ(R, T) \
_ITM_TYPE_##T REGPARM _ITM_TYPE_ATTR(T)					\
_ITM_##R##T(const _ITM_TYPE_##T *ptr)					\
{									\
  return gtm_disp()->R##T (ptr);					\
}

#define _ITM_WRITE(W, T) \
void REGPARM _ITM_TYPE_ATTR(T)						\
_ITM_##W##T(_ITM_TYPE_##T *ptr, _ITM_TYPE_##T val)			\
{									\
  gtm_disp()->W##T (ptr, val);						\
}

_ITM_ALL_TYPES (_ITM_ALL_READS)
_ITM_ALL_TYPES (_ITM_ALL_WRITES)

#undef _ITM_READ
#undef _ITM_WRITE

#define _ITM_MCPY_RW(FN, R, W) \
void REGPARM _ITM_##FN##R##W (void *dst, const void *src, size_t len)	\
{									\
  gtm_disp()->FN##R##W (dst, src, len);					\
}

_ITM_MCPY(memcpy)
_ITM_MCPY(memmove)

#undef _ITM_MCPY_RW

#define _ITM_MSET_W(FN, W) \
void REGPARM _ITM_##FN##W (void *dst, int src, size_t len)		\
{									\
  gtm_disp()->FN##W (dst, src, len);					\
}

_ITM_MSET(memset)

#undef _ITM_MSET_W
