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

#include "libitm.h"


#define _ITM_READ(R, T) \
_ITM_TYPE_##T REGPARM _ITM_##R##T(const _ITM_TYPE_##T *ptr)		\
{									\
  return gtm_thr.disp->R##T (ptr);					\
}

#define _ITM_WRITE(W, T) \
void REGPARM _ITM_##W##T(_ITM_TYPE_##T *ptr, _ITM_TYPE_##T val)		\
{									\
  gtm_thr.disp->W##T (ptr, val);					\
}

_ITM_ALL_TYPES (_ITM_ALL_READS)
_ITM_ALL_TYPES (_ITM_ALL_WRITES)

#undef _ITM_READ
#undef _ITM_WRITE

#define _ITM_MCPY_RW(FN, R, W) \
void REGPARM _ITM_##FN##R##W (void *dst, const void *src, size_t len)	\
{									\
  gtm_thr.disp->FN##R##W (dst, src, len);				\
}

_ITM_MCPY(memcpy)
_ITM_MCPY(memmove)

#undef _ITM_MCPY_RW

#define _ITM_MSET_W(FN, W) \
void REGPARM _ITM_##FN##W (void *dst, int src, size_t len)		\
{									\
  gtm_thr.disp->FN##W (dst, src, len);					\
}

_ITM_MSET(memset)

#undef _ITM_MSET_W
