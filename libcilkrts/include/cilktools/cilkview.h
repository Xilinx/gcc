/* cilkview.h                  -*-C++-*-
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

#ifndef INCLUDED_CILKVIEW_H
#define INCLUDED_CILKVIEW_H

#include <cilk/cilk_api.h>

#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WINBASE_
extern unsigned long __stdcall GetTickCount();
#endif

#ifdef __cplusplus
}
#endif

static inline unsigned long long __cilkview_getticks()
{
    // Fetch number of milliseconds that have elapsed since the system started
     return GetTickCount();
}
#endif  // _WIN32

#if defined __unix__ || defined __APPLE__
#include <time.h>
#include <sys/time.h>

static inline unsigned long long __cilkview_getticks()
{
    // Fetch number of milliseconds that have elapsed since the Epoch (1-Jan-1970)
    struct timeval t;
    gettimeofday(&t, 0);
    return t.tv_sec * 1000ULL + t.tv_usec / 1000;
}
#endif  // defined __unix__ || defined __APPLE__

typedef struct
{
    unsigned int        size;           // Size of structure in bytes
    unsigned int        status;         // 1 = success, 0 = failure
    unsigned long long  time;           // Time in milliseconds
    unsigned long long  work;
    unsigned long long  span;
    unsigned long long  burdened_span;
    unsigned long long  spawns;
    unsigned long long  syncs;
    unsigned long long  strands;
    unsigned long long  atomic_ins;
    unsigned long long  frames;
} cilkview_data_t;

typedef struct
{
    cilkview_data_t *start;     // Values at start of interval
    cilkview_data_t *end;       // Values at end of interval
    char *label;                // Name for this interval
    unsigned int flags;         // What to do - see flags below
} cilkview_report_t;

// What __cilkview_report should do.  The flags can be ORed together
enum
{
    CV_REPORT_WRITE_TO_LOG = 1,     // Write parallelism report to the log (xml or text)
    CV_REPORT_WRITE_TO_RESULTS = 2  // Write parallelism data to results file
};

void __cilkview_do_report(cilkview_data_t *start,
                          cilkview_data_t *end,
                          char *label,
                          unsigned int flags);
/*
 * Metacall data
 *
 * A metacall is a way to pass data to a function implemented by a tool.
 * Metacalls are always instrumented when the tool is loaded.
 */

// Tool code for Cilkview
#define METACALL_TOOL_CILKVIEW 2

// Metacall codes implemented by Cilkview
enum
{
    CV_METACALL_PUTS,
    CV_METACALL_QUERY,
    CV_METACALL_START,
    CV_METACALL_STOP,
    CV_METACALL_RESET,
    CV_METACALL_USE_DEFAULT_GRAIN,
    CV_METACALL_CONNECTED,
    CV_METACALL_SUSPEND,
    CV_METACALL_RESUME,
    CV_METACALL_REPORT
};

#if ! defined(CILK_STUB) && defined(__INTEL_COMPILER)
#  define __cilkview_metacall(code,data) \
    __cilkrts_metacall(METACALL_TOOL_CILKVIEW, code, data)
#else
#  define __cilkview_metacall(annotation,expr) (annotation, (void) (expr))
#endif

// Write arbitrary string to the log
#define __cilkview_puts(arg) \
    __cilkview_metacall(CV_METACALL_PUTS, arg)

// Retrieve the Cilkview performance counters.  The parameter must be a 
// cilkview_data_t
#define __cilkview_query(d)                             \
    do {                                                \
        d.size = sizeof(d);                             \
        d.status = 0;                                   \
        __cilkview_metacall(CV_METACALL_QUERY, &d);     \
        if (0 == d.status)                              \
            d.time = __cilkview_getticks();             \
    } while (0)

// Write report to log or results file. If end is NULL, Cilkview will
// use the current values.
#define __cilkview_report(start, end, label, flags) \
    __cilkview_do_report(start, end, label, flags)

// Control the workspan performance counters for the final report
#define __cilkview_workspan_start() \
    __cilkview_metacall(CV_METACALL_START, 0)
#define __cilkview_workspan_stop() \
    __cilkview_metacall(CV_METACALL_STOP, 0)
#define __cilkview_workspan_reset() \
    __cilkview_metacall(CV_METACALL_RESET, 0)
#define __cilkview_workspan_suspend() \
    __cilkview_metacall(CV_METACALL_SUSPEND, 0)
#define __cilkview_workspan_resume() \
    __cilkview_metacall(CV_METACALL_RESUME, 0)

#define __cilkview_use_default_grain_size() \
    __cilkview_metacall(CV_METACALL_USE_DEFAULT, 0)

// Sets the int is_connected to 1 if Cilkview is active
#define __cilkview_connected(is_connected) \
    __cilkview_metacall(CV_METACALL_CONNECTED, &is_connected)


#ifndef CILKVIEW_NO_REPORT

// Stop Microsoft include files from complaining about getenv and fopen
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 1786)	// Suppress warnings that getenv, fopen are deprecated
#endif

static void __cilkview_do_report(cilkview_data_t *start,
                                 cilkview_data_t *end,
                                 char *label,
                                 unsigned int flags)
{
    int under_cilkview = 0;
    unsigned long long elapsed_ms;
    int worker_count = 0;
    char *nworkers;
    char *outfile;
    FILE *f;

    // Check whether we're running under Cilkview
    __cilkview_connected(under_cilkview);

    // If we're running under Cilkview, let it do those things that need
    // to be done
    if (under_cilkview)
    {
        cilkview_report_t d = {start, end, label, flags};
        __cilkview_metacall(CV_METACALL_REPORT, &d);
        return;
    }

    // We're not running under Cilkview.
    //
    // If we weren't asked to write to the results file, we're done.
    if (0 == (flags & CV_REPORT_WRITE_TO_RESULTS))
        return;

    // Calculate the elapse milliseconds
    if (NULL == end)
        elapsed_ms = __cilkview_getticks() - start->time;
    else
        elapsed_ms = end->time - start->time;

    // Determine how many workers we're using for this trial run
    nworkers = getenv("CILK_NWORKERS");
    if (NULL != nworkers)
        worker_count = atoi(nworkers);
    if (0 == worker_count)
        worker_count = 16;

    // Open the output file and write the trial data to it
    outfile = getenv("CILKVIEW_OUTFILE");
    if (NULL == outfile)
        outfile = "cilkview.out";

    f = fopen(outfile, "a");
    if (NULL == f)
        fprintf(stderr, "__cilkview_do_report: unable to append to file %s\n", outfile);
    else
    {
        fprintf(f, "%s trial %d %f\n", label,
                worker_count,
                ((float)elapsed_ms) / 1000.0f);
        fclose(f);
    }
}
#ifdef _WIN32
#pragma warning(pop)
#endif

#endif  // CILKVIEW_NO_REPORT


#endif /* ! defined(INCLUDED_CILKVIEW_H) */
