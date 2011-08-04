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

/* Return true if PPH has been enabled.  */
static inline bool
pph_enabled_p (void)
{
  return pph_out_file != NULL || query_have_pph_map ();
}

/* Global state.  FIXME pph, get rid of these.  */

/* Log file where PPH analysis is written to.  Controlled by
   -fpph-logfile.  If this flag is not given, stdout is used.  */
extern FILE *pph_logfile;

/* In pph.c  */
extern void pph_init (void);
extern void pph_finish (void);
extern void pph_dump_tree_name (FILE *file, tree t, int flags);
extern void pph_dump_namespace (FILE *, tree ns);

#endif  /* GCC_CP_PPH_H  */
