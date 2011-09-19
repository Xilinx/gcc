/* Factored pre-parsed header (PPH) support for C++
   Copyright (C) 2010 Free Software Foundation, Inc.
   Contributed by Lawrence Crowl <crowl@google.com> and
   Diego Novillo <dnovillo@google.com>.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_CP_PPH_H
#define GCC_CP_PPH_H

/* In order for the format checking to accept the C++ front end
   diagnostic framework extensions, you must include this file before
   diagnostic-core.h, not after.  We override the definition of GCC_DIAG_STYLE
   in c-common.h.  */
#undef GCC_DIAG_STYLE
#define GCC_DIAG_STYLE __gcc_cxxdiag__
#if defined(GCC_DIAGNOSTIC_CORE_H) || defined (GCC_C_COMMON_H)
#error \
In order for the format checking to accept the C++ front end diagnostic \
framework extensions, you must include this file before diagnostic-core.h and \
c-common.h, not after.
#endif

#include "line-map.h"
#include "hashtab.h"
#include "cp/cp-tree.h"
#include "parser.h"
#include "timevar.h"

/* A set of contiguous tokens within a single file.  */
typedef struct GTY(()) cp_token_hunk
{
  /* Captured identifier and macro state.  */
  cpp_idents_used identifiers;

  /* The array of tokens.  */
  VEC(cp_token,gc) *buffer;
} cp_token_hunk;

typedef struct cp_token_hunk *cp_token_hunk_ptr;

DEF_VEC_P (cp_token_hunk_ptr);
DEF_VEC_ALLOC_P (cp_token_hunk_ptr,gc);

/* Global state.  FIXME pph, get rid of these.  */

/* Log file where PPH analysis is written to.  Controlled by
   -fpph-logfile.  If this flag is not given, stdout is used.  */
extern FILE *pph_logfile;

/* In pph.c  */
extern void pph_init (void);
extern void pph_finish (void);
extern const char *pph_tree_code_text (enum tree_code code);
extern void pph_dump_min_decl (FILE *file, tree decl);
extern void pph_dump_tree_name (FILE *file, tree t, int flags);
extern void pph_dump_namespace (FILE *, tree ns);

#endif  /* GCC_CP_PPH_H  */
