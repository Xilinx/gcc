// -*- C++ -*-

/*
 * Copyright (C) 2009-2010 
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
 ******************************************************************************
 *
 * metacall.h
 *
 * This is an internal header file defining part of the metacall
 * interface used by Cilkscreen.  It is not a stable API and is
 * subject to change without notice.
 */

// Provides the enum of metacall kinds.  This is used by Cilkscreen and the
// runtime, and will probably be used by any future ptools.

#pragma once

///////////////////////////////////////////////////////////////////////////////

enum
{
    // Notify Cilkscreen to stop/start instrumenting code
    HYPER_DISABLE_INSTRUMENTATION = 0,
    HYPER_ENABLE_INSTRUMENTATION = 1,

    // Write 0 in *(char *)arg if the p-tool is sequential.  The Cilk runtime
    // system invokes this metacall to know whether to spawn worker threads.
    HYPER_ZERO_IF_SEQUENTIAL_PTOOL = 2,

    // Write 0 in *(char *)arg if the runtime must force reducers to
    // call the reduce() method even if no actual stealing occurs.
    HYPER_ZERO_IF_FORCE_REDUCE = 3,

    // Inform cilkscreen about the current stack pointer.
    HYPER_ESTABLISH_C_STACK = 4,

    // Inform Cilkscreen about the current worker
    HYPER_ESTABLISH_WORKER = 5,

    // Tell tools to ignore a block of memory.  Parameter is a 2 element
    // array: void *block[2] = {_begin, _end};  _end is 1 beyond the end
    // of the block to be ignored.  Essentially, if p is a pointer to an
    // array, _begin = &p[0], _end = &p[max]
    HYPER_IGNORE_MEMORY_BLOCK = 6

    // If you add metacalls here, remember to update BOTH workspan.cpp AND
    // cilkscreen-common.cpp!
};

typedef struct
{
    unsigned int tool;  // Specifies tool metacall is for
                        // (eg. system=0, cilkscreen=1, cilkview=2).
                        // All tools should understand system codes.
                        // Tools should ignore all other codes, except
                        // their own.

    unsigned int code;  // Tool-specific code specifies what to do and how to
                        // interpret data

    void        *data;
} metacall_data_t;

#define METACALL_TOOL_SYSTEM 0

///////////////////////////////////////////////////////////////////////////////
