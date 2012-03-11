/* except-gcc.h                  -*-C++-*-
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
 * @file except-gcc.h
 *
 * @brief ABI for gcc exception handling.
 *
 * @par Origin
 * The code below is generally copied from the Intel Itanium ABI (Intel
 * download 245370).
 */

#ifndef INCLUDED_EXCEPT_GCC_DOT_H
#define INCLUDED_EXCEPT_GCC_DOT_H

#ifndef __cplusplus
#   error except-gcc.h should be used in C++ code only.
#endif

#include <cilk/common.h>
#include <exception>

struct __cxa_exception;

__CILKRTS_BEGIN_EXTERN_C

/** Unwind reason code (Itanium ABI 6.1.2.1) */
typedef enum _Unwind_Reason_Code {
    _URC_NO_REASON = 0,
    _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
    _URC_FATAL_PHASE2_ERROR = 2,
    _URC_FATAL_PHASE1_ERROR = 3,
    _URC_NORMAL_STOP = 4,
    _URC_END_OF_STACK = 5,
    _URC_HANDLER_FOUND = 6,
    _URC_INSTALL_CONTEXT = 7,
    _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

typedef struct _Unwind_Exception _Unwind_Exception;

/** Exception cleanup function pointer (Itanium ABI 6.1.2.2) */
typedef void (*_Unwind_Exception_Cleanup_Fn)(_Unwind_Reason_Code reason,
                                             _Unwind_Exception *exc);

/**
 * @brief Exception undwinding information
 *
 * This is copied from the Intel Itanium ABI except that the
 * private fields are declared unsigned long for binary
 * compatibility with gcc/g++ on 32 bit machines.
 */
struct _Unwind_Exception
{
    uint64_t                     exception_class;
    _Unwind_Exception_Cleanup_Fn exception_cleanup;
    unsigned long                private_1;
    unsigned long                private_2;
};

/** Throw or rethrow an exception */
_Unwind_Reason_Code
_Unwind_RaiseException(_Unwind_Exception *exception_object);

/** Resume an exception other than by rethrowing it. */
void _Unwind_Resume(_Unwind_Exception *exception_object);

/** Delete an exception object */
void _Unwind_DeleteException(_Unwind_Exception *exception_object);

/**
 * C++ exception ABI.
 *  The following declarations are from
 *
 * http://www.codesourcery.com/public/cxx-abi/abi-eh.html#cxx-abi
 */

struct __cxa_exception {
    std::type_info *        exceptionType;
    void (*exceptionDestructor)(void *); 
    std::unexpected_handler unexpectedHandler;
    std::terminate_handler  terminateHandler;
    __cxa_exception *       nextException;

    int                     handlerCount;
    int                     handlerSwitchValue;
    const char *            actionRecord;
    const char *            languageSpecificData;
    void *                  catchTemp;
    void *                  adjustedPtr;

    _Unwind_Exception       unwindHeader;
};

static inline __cxa_exception *to_cxx(_Unwind_Exception *e)
{
    return ((__cxa_exception *)(e+1)) - 1;
}

typedef struct __cxa_eh_globals {
    __cxa_exception *caughtExceptions;
    unsigned int     uncaughtExceptions;
} __cxa_eh_globals;

__cxa_eh_globals*__cxa_get_globals(void) throw();

__CILKRTS_END_EXTERN_C

#endif // ! defined(INCLUDED_EXCEPT_GCC_DOT_H)
