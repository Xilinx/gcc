/* metacall_impl.c                  -*-C-*-
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

#include "metacall_impl.h"

NOINLINE
CILK_API_VOID
__cilkrts_metacall(unsigned int tool, unsigned int code, void *data)
{
    // The metacall type, code and data are packed together into a single
    // struct which will be interpreted by the tool. This function is the
    // one and only use of a "cilkscreen_metacall" annotation
    metacall_data_t d = { tool, code, data };

#ifdef __INTEL_COMPILER
    __notify_intrinsic("cilkscreen_metacall", &d);
#endif // defined __INTEL_COMPILER
}

int __cilkrts_running_under_sequential_ptool(void)
{
    static int running_under_sequential_ptool = -1;
    volatile char c = ~0;

    // If we haven't been called before, see if we're running under Cilkscreen
    // or Cilkview
    if (-1 == running_under_sequential_ptool)
    {
        // metacall #2 writes 0 in C if we are running under
        // a p-tools that requires serial execution, and is a 
        // no-op otherwise
        //
        // Note that removing the volatile is required to prevent the compiler
        // from assuming that the value has not changed
        __cilkrts_metacall(METACALL_TOOL_SYSTEM,
                           HYPER_ZERO_IF_SEQUENTIAL_PTOOL, (void *)&c);

        running_under_sequential_ptool = (0 == c);
    }

    return running_under_sequential_ptool;
}

/*
 * __cilkrts_cilkscreen_establish_c_stack
 *
 * Notify Cilkscreen of the extent of the stack
 */

void __cilkrts_cilkscreen_establish_c_stack(char *begin, char *end)
{
    char *limits[2] = {begin, end};

    __cilkrts_metacall(METACALL_TOOL_SYSTEM, HYPER_ESTABLISH_C_STACK, limits);
}

#ifdef WORKSPAN // Workspan stuff - remove when we're sure what we can drop

void __cilkview_workspan_start(void) {
  __cilkrts_metacall(HYPER_WORKSPAN_START, 0);
}

void __cilkview_workspan_stop(void) {
  __cilkrts_metacall(HYPER_WORKSPAN_STOP, 0);
}

void __cilkview_workspan_dump(const char *str) {
  __cilkrts_metacall(HYPER_WORKSPAN_DUMP, (void*)str);
}


void __cilkview_workspan_reset(void) {
  __cilkrts_metacall(HYPER_WORKSPAN_RESET, 0);
}


void __cilkview_use_default_grain(void) {
    __cilkrts_metacall(HYPER_USE_DEFAULT_GRAIN, 0);
}

void __cilkview_get_workspan_data(unsigned long long *values, int size)
{
    void *data[2];

    /* reset counters to zero in case we are not running under
       a p-tool */

    values[0] = 0;

    data[0] = (void*) values;
    data[1] = (void*) &size;
     __cilkrts_metacall(HYPER_WORKSPAN_QUERY, &data);
}

void __cilkview_workspan_connected (int *flag) {
  *flag = 0;
  __cilkrts_metacall(HYPER_WORKSPAN_CONNECTED, (void *)flag);
}

void __cilkview_workspan_suspend() {
  __cilkrts_metacall(HYPER_WORKSPAN_SUSPEND, 0);
}

void __cilkview_workspan_resume() {
  __cilkrts_metacall(HYPER_WORKSPAN_RESUME, 0);
}

/* depreciated interfaces */
void __cilkometer_workspan_start(void) {
  __cilkrts_metacall(HYPER_WORKSPAN_START, 0);
}

void __cilkometer_workspan_stop(void) {
  __cilkrts_metacall(HYPER_WORKSPAN_STOP, 0);
}

void __cilkometer_workspan_dump(const char *str) {
  __cilkrts_metacall(HYPER_WORKSPAN_DUMP, (void*)str);
}


void __cilkometer_workspan_reset(void) {
  __cilkrts_metacall(HYPER_WORKSPAN_RESET, 0);
}

#endif // WORKSPAN

/* End metacall_impl.c */
