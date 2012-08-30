/* Performance monitoring unit (PMU) profiler. If available, use an
   external tool to collect hardware performance counter data and
   write it in the .gcda files.

   Copyright (C) 2011. Free Software Foundation, Inc.
   Contributed by Sharad Singhai <singhai@google.com>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "tconfig.h"
#include "tsystem.h"
#include "coretypes.h"
#include "tm.h"
#if (defined (__x86_64__) || defined (__i386__))
#include "cpuid.h"
#endif

#if defined(inhibit_libc)
#define IN_LIBGCOV (-1)
#else
#include <stdio.h>
#include <stdlib.h>
#define IN_LIBGCOV 1
  #if defined(L_gcov)
  #define GCOV_LINKAGE /* nothing */
  #endif
#endif
#include "gcov-io.h"
#ifdef TARGET_POSIX_IO
  #include <fcntl.h>
  #include <signal.h>
  #include <sys/stat.h>
  #include <sys/types.h>
#endif

#if defined(inhibit_libc)
#else
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#define XNEWVEC(type,ne) (type *)calloc((ne),sizeof(type))
#define XNEW(type) (type *)malloc(sizeof(type))
#define XDELETEVEC(p) free(p)
#define XDELETE(p) free(p)

static unsigned convert_pct_to_unsigned (float pct);
static void *init_pmu_load_latency (void);
static void *init_pmu_branch_mispredict (void);
static void destroy_load_latency_infos (void *info);
static void destroy_branch_mispredict_infos (void *info);
static gcov_unsigned_t gcov_tag_pmu_tool_header_length (gcov_pmu_tool_header_t
                                                        *header);

/* Convert a fractional PCT to an unsigned integer after
   muliplying by 100.  */

static unsigned
convert_pct_to_unsigned (float pct)
{
  return (unsigned)(pct * 100.0f);
}

/* Delete load latency info structures INFO.  */

static void
destroy_load_latency_infos (void *info)
{
  unsigned i;
  ll_infos_t* ll_infos = (ll_infos_t *)info;

  /* delete each element */
  for (i = 0; i < ll_infos->ll_count; ++i)
    XDELETE (ll_infos->ll_array[i]);
  /* delete the array itself */
  XDELETE (ll_infos->ll_array);
  __destroy_pmu_tool_header (ll_infos->pmu_tool_header);
  free (ll_infos->pmu_tool_header);
  ll_infos->ll_array = 0;
  ll_infos->ll_count = 0;
}

/* Delete branch mispredict structure INFO.  */

static void
destroy_branch_mispredict_infos (void *info)
{
  unsigned i;
  brm_infos_t* brm_infos = (brm_infos_t *)info;

  /* delete each element */
  for (i = 0; i < brm_infos->brm_count; ++i)
    XDELETE (brm_infos->brm_array[i]);
  /* delete the array itself */
  XDELETE (brm_infos->brm_array);
  __destroy_pmu_tool_header (brm_infos->pmu_tool_header);
  free (brm_infos->pmu_tool_header);
  brm_infos->brm_array = 0;
  brm_infos->brm_count = 0;
}

/* Allocate and initialize pmu load latency structure.  */

static void *
init_pmu_load_latency (void)
{
  ll_infos_t *load_latency = XNEWVEC (ll_infos_t, 1);
  load_latency->ll_count = 0;
  load_latency->alloc_ll_count = 64;
  load_latency->ll_array = XNEWVEC (gcov_pmu_ll_info_t *,
                                    load_latency->alloc_ll_count);
  return (void *)load_latency;
}

/* Allocate and initialize pmu branch mispredict structure.  */

static void *
init_pmu_branch_mispredict (void)
{
  brm_infos_t *brm_info = XNEWVEC (brm_infos_t, 1);
  brm_info->brm_count = 0;
  brm_info->alloc_brm_count = 64;
  brm_info->brm_array = XNEWVEC (gcov_pmu_brm_info_t *,
                                 brm_info->alloc_brm_count);
  return (void *)brm_info;
}

/* Write the load latency information LL_INFO into the gcda file.  */

void
gcov_write_ll_line (const gcov_pmu_ll_info_t *ll_info)
{
  gcov_write_tag_length (GCOV_TAG_PMU_LOAD_LATENCY_INFO,
                         GCOV_TAG_PMU_LOAD_LATENCY_LENGTH);
  gcov_write_unsigned (ll_info->counts);
  gcov_write_unsigned (ll_info->self);
  gcov_write_unsigned (ll_info->cum);
  gcov_write_unsigned (ll_info->lt_10);
  gcov_write_unsigned (ll_info->lt_32);
  gcov_write_unsigned (ll_info->lt_64);
  gcov_write_unsigned (ll_info->lt_256);
  gcov_write_unsigned (ll_info->lt_1024);
  gcov_write_unsigned (ll_info->gt_1024);
  gcov_write_unsigned (ll_info->wself);
  gcov_write_counter (ll_info->code_addr);
  gcov_write_unsigned (ll_info->line);
  gcov_write_unsigned (ll_info->discriminator);
  gcov_write_unsigned (ll_info->filetag);
}


/* Write the branch mispredict information BRM_INFO into the gcda file.  */

void
gcov_write_branch_mispredict_line (const gcov_pmu_brm_info_t *brm_info)
{
  gcov_write_tag_length (GCOV_TAG_PMU_BRANCH_MISPREDICT_INFO,
                         GCOV_TAG_PMU_BRANCH_MISPREDICT_LENGTH);
  gcov_write_unsigned (brm_info->counts);
  gcov_write_unsigned (brm_info->self);
  gcov_write_unsigned (brm_info->cum);
  gcov_write_counter (brm_info->code_addr);
  gcov_write_unsigned (brm_info->line);
  gcov_write_unsigned (brm_info->discriminator);
  gcov_write_unsigned (brm_info->filetag);
}

/* Write the string table entry ST_ENTRY into the gcda file.  */

void
gcov_write_string_table_entry (const gcov_pmu_st_entry_t *st_entry)
{
  gcov_write_tag_length (GCOV_TAG_PMU_STRING_TABLE_ENTRY,
                         GCOV_TAG_PMU_STRING_TABLE_ENTRY_LENGTH(
                             st_entry->str));
  gcov_write_unsigned(st_entry->index);
  gcov_write_string(st_entry->str);
}

/* Compute TOOL_HEADER length for writing into the gcov file.  */

static gcov_unsigned_t
gcov_tag_pmu_tool_header_length (gcov_pmu_tool_header_t *header)
{
  gcov_unsigned_t len = 0;
  if (header)
    {
      len += gcov_string_length (header->host_cpu);
      len += gcov_string_length (header->hostname);
      len += gcov_string_length (header->kernel_version);
      len += gcov_string_length (header->column_header);
      len += gcov_string_length (header->column_description);
      len += gcov_string_length (header->full_header);
    }
  return len;
}

/* Write tool header into the gcda file. It assumes that the gcda file
   has already been opened and is available for writing.  */

void
gcov_write_tool_header (gcov_pmu_tool_header_t *header)
{
  gcov_unsigned_t len = gcov_tag_pmu_tool_header_length (header);
  gcov_write_tag_length (GCOV_TAG_PMU_TOOL_HEADER, len);
  gcov_write_string (header->host_cpu);
  gcov_write_string (header->hostname);
  gcov_write_string (header->kernel_version);
  gcov_write_string (header->column_header);
  gcov_write_string (header->column_description);
  gcov_write_string (header->full_header);
}

#endif
