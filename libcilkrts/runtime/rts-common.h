/* rts-common.h                  -*-C++-*-
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

#ifndef INCLUDED_RTS_COMMON_DOT_H
#define INCLUDED_RTS_COMMON_DOT_H

/* Abbreviations API functions returning different types.  By using these
 * abbreviations instead of using CILK_API(ret) directly, etags and other
 * tools can more easily recognize function signatures.
 */
#define CILK_API_VOID        CILK_API(void)
#define CILK_API_VOID_PTR    CILK_API(void*)
#define CILK_API_INT         CILK_API(int)
#define CILK_API_TBB_RETCODE CILK_API(__cilk_tbb_retcode)

/* Abbreviations ABI functions returning different types.  By using these
 * abbreviations instead of using CILK_ABI(ret) directly, etags and other
 * tools can more easily recognize function signatures.
 */
#define CILK_ABI_VOID        CILK_ABI(void)
#define CILK_ABI_WORKER_PTR  CILK_ABI(__cilkrts_worker_ptr)
#define CILK_ABI_THROWS_VOID CILK_ABI_THROWS(void)

/* documentation aid to identify portable vs. nonportable
   parts of the runtime.  See README for definitions. */
#define COMMON_PORTABLE extern
#define COMMON_SYSDEP extern
#define NON_COMMON extern

#if !(defined __GNUC__ || defined __ICC)
#   define __builtin_expect(a_, b_) a_
#endif

#ifdef __cplusplus
#   define cilk_nothrow throw()
#else
#   define cilk_nothrow /*empty in C*/
#endif

#ifdef __GNUC__
#   define NORETURN void __attribute__((noreturn))
#else
#   define NORETURN void __declspec(noreturn)
#endif

#ifdef __GNUC__
#   define NOINLINE __attribute__((noinline))
#else
#   define NOINLINE __declspec(noinline)
#endif

#ifndef __GNUC__
#   define __attribute__(X)
#endif

/* Microsoft CL accepts "inline" for C++, but not for C.  It accepts 
 * __inline for both.  Intel ICL accepts inline for C of /Qstd=c99
 * is set.  The Cilk runtime is assumed to be compiled with /Qstd=c99
 */
#if defined(_MSC_VER) && ! defined(__INTEL_COMPILER)
#   error define inline
#   define inline __inline
#endif

#endif // ! defined(INCLUDED_RTS_COMMON_DOT_H)
