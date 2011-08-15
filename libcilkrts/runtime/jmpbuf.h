/* jmpbuf.h                  -*-C++-*-
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

/**
 * @file jmpbuf.h
 *
 * @brief Macros and functions to access the _JUMP_BUFFER initialized by a 
 * call to CILK_SETJMP before a cilk_spawn or cilk_sync.  The definition of
 * CILK_SETJMP and CILK_LONGJMP are OS dependent and in abi.h
 *
 */

#ifndef INCLUDED_JMPBUF_DOT_H
#define INCLUDED_JMPBUF_DOT_H

#include <cilk/common.h>
#include <internal/abi.h>
#include <stddef.h>
#include <setjmp.h>

#if 0 /* defined CILK_USE_C_SETJMP && defined JB_RSP */
#define SP(SF) (SF)->ctx[0].__jmpbuf[JB_RSP]
#define FP(SF) (SF)->ctx[0].__jmpbuf[JB_RBP]
#define PC(SF) (SF)->ctx[0].__jmpbuf[JB_PC]
#elif 0 /* defined CILK_USE_C_SETJMP && defined JB_SP */
#define SP(SF) (SF)->ctx[0].__jmpbuf[JB_SP]
#define FP(SF) (SF)->ctx[0].__jmpbuf[JB_BP]
#define PC(SF) (SF)->ctx[0].__jmpbuf[JB_PC]
#elif defined _WIN64
#define SP(SF) ((_JUMP_BUFFER*)(&(SF)->ctx))->Rsp
#define FP(SF) ((_JUMP_BUFFER*)(&(SF)->ctx))->Rbp
#define PC(SF) ((_JUMP_BUFFER*)(&(SF)->ctx))->Rip
#elif defined _WIN32
/** Fetch stack pointer from a __cilkrts_stack_frame */
#define SP(SF) SF->ctx.Esp
/** Fetch frame pointer from a __cilkrts_stack_frame */
#define FP(SF) SF->ctx.Ebp
/** Fetch program counter from a __cilkrts_stack_frame */
#define PC(SF) SF->ctx.Eip
#else /* defined __GNUC__ || defined __ICC */
/* word 0 is frame address
   word 1 is resume address
   word 2 is stack address */
#define FP(SF) (SF)->ctx[0]
#define PC(SF) (SF)->ctx[1]
#define SP(SF) (SF)->ctx[2]
#endif

__CILKRTS_BEGIN_EXTERN_C

/**
 * Fetch the stack pointer from a __cilkrts_stack_frame.  The jmpbuf was
 * initialized before a cilk_spawn or cilk_sync.
 *
 * @param sf __cilkrts_stack_frame containing the jmpbuf.
 *
 * @return the stack pointer from the ctx.
 */
inline char *__cilkrts_get_sp(__cilkrts_stack_frame *sf)
{
    return (char *)SP(sf);
}

/**
 * Calculate the frame size from __cilkrts_stack_frame.  The jmpbuf was
 * initialized before a cilk_spawn or cilk_sync.
 *
 * @warning Returning an arbitrary value on Windows!
 *
 * @param sf __cilkrts_stack_frame containing the jmpbuf.
 *
 * @return the stack pointer from the ctx.
 */
inline ptrdiff_t __cilkrts_get_frame_size(__cilkrts_stack_frame *sf)
{
#ifdef _WIN32
    if (0 == SP(sf))
        return 256;         // Arbitrary!
#endif
    return (ptrdiff_t)FP(sf) - (ptrdiff_t)SP(sf);
}

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_JMPBUF_DOT_H)
