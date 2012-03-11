/* c_reducers.c                  -*-C-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2010 
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
 *
 **************************************************************************/

/* Implementation of C reducers */

#include <cilk/reducer_opadd.h>
#include <cilk/reducer_opand.h>
#include <cilk/reducer_opor.h>
#include <cilk/reducer_opxor.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <limits.h>
#include <math.h> /* HUGE_VAL */

#ifndef _MSC_VER
# include <stdint.h> /* WCHAR_MIN */
#else
# include <wchar.h> /* WCHAR_MIN */
#endif

/* Floating-point constants */
#ifndef HUGE_VALF
  static const unsigned int __huge_valf[] = {0x7f800000};
# define HUGE_VALF (*((const float *)__huge_valf))
#endif

#ifndef HUGE_VALL
  static const unsigned int __huge_vall[] = {0, 0, 0x00007f80, 0};
# define HUGE_VALL (*((const long double *)__huge_vall))
#endif

// Disable warning about integer conversions losing significant bits.
// The code is correct as is.
#pragma warning(disable:2259)

CILK_C_REDUCER_OPADD_IMP(char,char)
CILK_C_REDUCER_OPADD_IMP(unsigned char,uchar)
CILK_C_REDUCER_OPADD_IMP(signed char,schar)
CILK_C_REDUCER_OPADD_IMP(wchar_t,wchar_t)
CILK_C_REDUCER_OPADD_IMP(short,short)
CILK_C_REDUCER_OPADD_IMP(unsigned short,ushort)
CILK_C_REDUCER_OPADD_IMP(int,int)
CILK_C_REDUCER_OPADD_IMP(unsigned int,uint)
CILK_C_REDUCER_OPADD_IMP(unsigned int,unsigned) // alternate name
CILK_C_REDUCER_OPADD_IMP(long,long)
CILK_C_REDUCER_OPADD_IMP(unsigned long,ulong)
CILK_C_REDUCER_OPADD_IMP(long long,longlong)
CILK_C_REDUCER_OPADD_IMP(unsigned long long,ulonglong)
CILK_C_REDUCER_OPADD_IMP(float,float)
CILK_C_REDUCER_OPADD_IMP(double,double)
CILK_C_REDUCER_OPADD_IMP(long double,longdouble)

CILK_C_REDUCER_OPAND_IMP(char,char)
CILK_C_REDUCER_OPAND_IMP(unsigned char,uchar)
CILK_C_REDUCER_OPAND_IMP(signed char,schar)
CILK_C_REDUCER_OPAND_IMP(wchar_t,wchar_t)
CILK_C_REDUCER_OPAND_IMP(short,short)
CILK_C_REDUCER_OPAND_IMP(unsigned short,ushort)
CILK_C_REDUCER_OPAND_IMP(int,int)
CILK_C_REDUCER_OPAND_IMP(unsigned int,uint)
CILK_C_REDUCER_OPAND_IMP(unsigned int,unsigned) // alternate name
CILK_C_REDUCER_OPAND_IMP(long,long)
CILK_C_REDUCER_OPAND_IMP(unsigned long,ulong)
CILK_C_REDUCER_OPAND_IMP(long long,longlong)
CILK_C_REDUCER_OPAND_IMP(unsigned long long,ulonglong)

CILK_C_REDUCER_OPOR_IMP(char,char)
CILK_C_REDUCER_OPOR_IMP(unsigned char,uchar)
CILK_C_REDUCER_OPOR_IMP(signed char,schar)
CILK_C_REDUCER_OPOR_IMP(wchar_t,wchar_t)
CILK_C_REDUCER_OPOR_IMP(short,short)
CILK_C_REDUCER_OPOR_IMP(unsigned short,ushort)
CILK_C_REDUCER_OPOR_IMP(int,int)
CILK_C_REDUCER_OPOR_IMP(unsigned int,uint)
CILK_C_REDUCER_OPOR_IMP(unsigned int,unsigned) // alternate name
CILK_C_REDUCER_OPOR_IMP(long,long)
CILK_C_REDUCER_OPOR_IMP(unsigned long,ulong)
CILK_C_REDUCER_OPOR_IMP(long long,longlong)
CILK_C_REDUCER_OPOR_IMP(unsigned long long,ulonglong)

CILK_C_REDUCER_OPXOR_IMP(char,char)
CILK_C_REDUCER_OPXOR_IMP(unsigned char,uchar)
CILK_C_REDUCER_OPXOR_IMP(signed char,schar)
CILK_C_REDUCER_OPXOR_IMP(wchar_t,wchar_t)
CILK_C_REDUCER_OPXOR_IMP(short,short)
CILK_C_REDUCER_OPXOR_IMP(unsigned short,ushort)
CILK_C_REDUCER_OPXOR_IMP(int,int)
CILK_C_REDUCER_OPXOR_IMP(unsigned int,uint)
CILK_C_REDUCER_OPXOR_IMP(unsigned int,unsigned) // alternate name
CILK_C_REDUCER_OPXOR_IMP(long,long)
CILK_C_REDUCER_OPXOR_IMP(unsigned long,ulong)
CILK_C_REDUCER_OPXOR_IMP(long long,longlong)
CILK_C_REDUCER_OPXOR_IMP(unsigned long long,ulonglong)

CILK_C_REDUCER_MAX_IMP(char,char,CHAR_MIN)
CILK_C_REDUCER_MAX_IMP(unsigned char,uchar,0)
CILK_C_REDUCER_MAX_IMP(signed char,schar,SCHAR_MIN)
CILK_C_REDUCER_MAX_IMP(wchar_t,wchar_t,WCHAR_MIN)
CILK_C_REDUCER_MAX_IMP(short,short,SHRT_MIN)
CILK_C_REDUCER_MAX_IMP(unsigned short,ushort,0)
CILK_C_REDUCER_MAX_IMP(int,int,INT_MIN)
CILK_C_REDUCER_MAX_IMP(unsigned int,uint,0)
CILK_C_REDUCER_MAX_IMP(unsigned int,unsigned,0) // alternate name
CILK_C_REDUCER_MAX_IMP(long,long,LONG_MIN)
CILK_C_REDUCER_MAX_IMP(unsigned long,ulong,0)
CILK_C_REDUCER_MAX_IMP(long long,longlong,LLONG_MIN)
CILK_C_REDUCER_MAX_IMP(unsigned long long,ulonglong,0)
CILK_C_REDUCER_MAX_IMP(float,float,-HUGE_VALF)
CILK_C_REDUCER_MAX_IMP(double,double,-HUGE_VAL)
CILK_C_REDUCER_MAX_IMP(long double,longdouble,-HUGE_VALL)
CILK_C_REDUCER_MAX_INDEX_IMP(char,char,CHAR_MIN)
CILK_C_REDUCER_MAX_INDEX_IMP(unsigned char,uchar,0)
CILK_C_REDUCER_MAX_INDEX_IMP(signed char,schar,SCHAR_MIN)
CILK_C_REDUCER_MAX_INDEX_IMP(wchar_t,wchar_t,WCHAR_MIN)
CILK_C_REDUCER_MAX_INDEX_IMP(short,short,SHRT_MIN)
CILK_C_REDUCER_MAX_INDEX_IMP(unsigned short,ushort,0)
CILK_C_REDUCER_MAX_INDEX_IMP(int,int,INT_MIN)
CILK_C_REDUCER_MAX_INDEX_IMP(unsigned int,uint,0)
CILK_C_REDUCER_MAX_INDEX_IMP(unsigned int,unsigned,0) // alternate name
CILK_C_REDUCER_MAX_INDEX_IMP(long,long,LONG_MIN)
CILK_C_REDUCER_MAX_INDEX_IMP(unsigned long,ulong,0)
CILK_C_REDUCER_MAX_INDEX_IMP(long long,longlong,LLONG_MIN)
CILK_C_REDUCER_MAX_INDEX_IMP(unsigned long long,ulonglong,0)
CILK_C_REDUCER_MAX_INDEX_IMP(float,float,-HUGE_VALF)
CILK_C_REDUCER_MAX_INDEX_IMP(double,double,-HUGE_VAL)
CILK_C_REDUCER_MAX_INDEX_IMP(long double,longdouble,-HUGE_VALL)

CILK_C_REDUCER_MIN_IMP(char,char,CHAR_MAX)
CILK_C_REDUCER_MIN_IMP(unsigned char,uchar,CHAR_MIN)
CILK_C_REDUCER_MIN_IMP(signed char,schar,SCHAR_MAX)
CILK_C_REDUCER_MIN_IMP(wchar_t,wchar_t,WCHAR_MAX)
CILK_C_REDUCER_MIN_IMP(short,short,SHRT_MAX)
CILK_C_REDUCER_MIN_IMP(unsigned short,ushort,USHRT_MAX)
CILK_C_REDUCER_MIN_IMP(int,int,INT_MAX)
CILK_C_REDUCER_MIN_IMP(unsigned int,uint,UINT_MAX)
CILK_C_REDUCER_MIN_IMP(unsigned int,unsigned,UINT_MAX) // alternate name
CILK_C_REDUCER_MIN_IMP(long,long,LONG_MAX)
CILK_C_REDUCER_MIN_IMP(unsigned long,ulong,ULONG_MAX)
CILK_C_REDUCER_MIN_IMP(long long,longlong,LLONG_MAX)
CILK_C_REDUCER_MIN_IMP(unsigned long long,ulonglong,ULLONG_MAX)
CILK_C_REDUCER_MIN_IMP(float,float,HUGE_VALF)
CILK_C_REDUCER_MIN_IMP(double,double,HUGE_VAL)
CILK_C_REDUCER_MIN_IMP(long double,longdouble,HUGE_VALL)
CILK_C_REDUCER_MIN_INDEX_IMP(char,char,CHAR_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(unsigned char,uchar,CHAR_MIN)
CILK_C_REDUCER_MIN_INDEX_IMP(signed char,schar,SCHAR_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(wchar_t,wchar_t,WCHAR_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(short,short,SHRT_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(unsigned short,ushort,USHRT_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(int,int,INT_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(unsigned int,uint,UINT_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(unsigned int,unsigned,UINT_MAX) // alternate name
CILK_C_REDUCER_MIN_INDEX_IMP(long,long,LONG_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(unsigned long,ulong,ULONG_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(long long,longlong,LLONG_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(unsigned long long,ulonglong,ULLONG_MAX)
CILK_C_REDUCER_MIN_INDEX_IMP(float,float,HUGE_VALF)
CILK_C_REDUCER_MIN_INDEX_IMP(double,double,HUGE_VAL)
CILK_C_REDUCER_MIN_INDEX_IMP(long double,longdouble,HUGE_VALL)

/* End reducer_opadd.c */
