/* cilk-ittnotify.h                  -*-C++-*-
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

#ifndef INCLUDED_CILK_ITTNOTIFY_DOT_H
#define INCLUDED_CILK_ITTNOTIFY_DOT_H

#ifdef __INTEL_COMPILER
#define USE_ITTNOTIFY 1
#endif
#include <stdio.h>

#ifdef USE_ITTNOTIFY
#include <ittnotify.h>

#ifdef _WIN32
# define ITT_SYNC_CREATE(_address, _description)        \
    __itt_sync_createA(_address,                        \
                       "Intel Cilk Plus " _description, \
                       "",                              \
                       __itt_attr_barrier)
#else
# define ITT_SYNC_CREATE(_address, _description)        \
    __itt_sync_create(_address,                         \
                      "Intel Cilk Plus " _description,  \
                      "",                               \
                      __itt_attr_barrier)
#endif

#define ITT_SYNC_PREPARE(_address) __itt_sync_prepare(_address)
#define ITT_SYNC_ACQUIRED(_address) __itt_sync_acquired(_address)
#define ITT_SYNC_RELEASING(_address) __itt_sync_releasing(_address)
#define ITT_SYNC_DESTROY(_address) __itt_sync_destroy(_address)
// Note that we subtract 5 from the return address to find the CALL instruction
// to __cilkrts_sync
#if 1   // Disable renaming for now.  Piersol isn't ready yet
#define ITT_SYNC_SET_NAME_AND_PREPARE(_address, _sync_ret_address) __itt_sync_prepare(_address)
#else
#define ITT_SYNC_SET_NAME_AND_PREPARE(_address, _sync_ret_address) \
    if (NULL != __itt_sync_prepare_ptr) {   \
        if (0 == _sync_ret_address) \
            __itt_sync_renameA(_address, "");  \
        else    \
        {   \
            char buf[128];  \
            sprintf_s(buf, 128, "IP:0x%p", (DWORD_PTR)_sync_ret_address - 5); \
            __itt_sync_renameA(_address, buf); \
            _sync_ret_address = 0;  \
         }  \
        __itt_sync_prepare(_address);  \
    }
#endif
#else   // USE_ITTNOTIFY not defined, compile out all calls
#define ITT_SYNC_CREATE(_address, _description)
#define ITT_SYNC_PREPARE(_address)
#define ITT_SYNC_ACQUIRED(_address)
#define ITT_SYNC_RELEASING(_addresss)
#define ITT_SYNC_DESTROY(_address)
#define ITT_SYNC_SET_NAME_AND_PREPARE(_sync_address, _wait_address)
#endif

#endif // ! defined(INCLUDED_CILK_ITTNOTIFY_DOT_H)
