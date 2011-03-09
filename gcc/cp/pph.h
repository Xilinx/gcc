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

/* Number of bytes in an MD5 signature.  */
#define DIGEST_LEN	16

struct pth_image;
typedef struct pth_image *pth_image_ptr;
DEF_VEC_P (pth_image_ptr);
DEF_VEC_ALLOC_P (pth_image_ptr,gc);

DEF_VEC_ALLOC_I(char,gc);

/* Description of a #include command.  This records the #include type
   used in the original source and the image included.  It is used
   to restore a file from its character stream in case that the
   associated image is found to be tainted.  */

typedef struct GTY(()) pth_include
{
  /* The image to include.  */
  struct pth_image *image;

  /* The #include command used: #include, #include_next or #import.  */
  enum include_type itype;

  /* Nonzero if this include used angle brackets.  */
  unsigned int angle_brackets : 1;

  /* Name used in the command used in this #include command.  */
  const char * GTY((skip)) iname;

  /* Directory where INAME can be found.  This is not necessarily
     the same as lbasename (IMAGE->FNAME), but it is always true that
     DNAME/INAME == IMAGE->FNAME.  */
  const char * GTY((skip)) dname;
} pth_include;

typedef struct pth_include *pth_include_ptr;
DEF_VEC_P (pth_include_ptr);
DEF_VEC_ALLOC_P (pth_include_ptr,gc);

/* An entry in the incremental cache.  Each entry represents
   a single file in the translation unit.  Tokens are saved in a linked
   list of lexers, separated by file change events in the pre-processor.
   Every time the preprocessor signals a file change, a new lexer for
   this entry is created.  */

typedef struct GTY(()) pth_image
{
  /* Full path name.  */
  const char * GTY((skip)) fname;

  /* MD5 sum for the contents of the original path name.  */
  unsigned char digest[DIGEST_LEN];

  /* Vector of token hunks corresponding to this file, and this file
     only. If this file includes other files, their tokens will not be
     stored here.  Each entry in this vector is a hunk of tokens
     delimited by file change events.  For instance, if a file is 
     laid out like this:

      [ t1 t2 ... tN ]
      #include "foo.h"
      [ p1 p2 ... pM ]

     The field token_caches will have two entries.  One for the hunk
     [t1, tN], the second one for the hunk [p1, pM].  */
  VEC(cp_token_hunk_ptr,gc) *token_hunks;

  /* Vector of files included by this file.  */
  VEC(pth_include_ptr,gc) *includes;

  /* Sequencing string that describes how the elements from
     TOKEN_HUNKS and INCLUDES are interleaved.  Each character of this
     string is either 'H' (hunk) or 'I' (include).  This is used in
     pth_image_to_lexer to know in what order should the hunks
     and include files be processed.
     
     FIXME pph, using a character string is heavy handed.  A bitmap
     would suffice, but for now this is easier and the space consumed
     should not be too significant.  */
  VEC(char,gc) *ih_sequence;

  /* True if this image needs to be flushed out to disk.  */
  unsigned int save_p : 1;

  /* True if this image has been loaded from an image file.  */
  unsigned int loaded_p: 1;

  /* True if this image has been incorporated into the current
     compilation context.  FIXME pph, should we keep track of this
     at the hunk level?  */
  unsigned int used_p : 1;

  /* True if we have computed the MD5 digest for FNAME.  */
  unsigned int digest_computed_p : 1;

  /* Index into the lexer buffer where the next token hunk to be
     created should start.  This is managed by the file changing
     logic in pth_file_change.  */
  unsigned hunk_start_ix;

  /* libcpp buffer associated with IMAGE->FNAME.  */
  cpp_buffer * GTY((skip)) buffer;
} pth_image;


/* PTH state.  This holds all the data needed to manage the PTH cache.  */
typedef struct GTY(()) pth_state
{
  /* The cache of pre-tokenized content.  */
  VEC(pth_image_ptr,gc) *cache;

  /* Pointer map for speeding up cache lookups.  */
  htab_t GTY((param_is (struct pth_image))) cache_dir;

  /* The current cache image being modified.  */
  pth_image *cur_image;

  /* Attributes for the most recent #include command found by
     pth_include_handler.  */
  enum include_type new_itype;
  bool new_angle_brackets;
  const char *new_iname;

  /* The lexer used to pre-process the file.  */
  cp_lexer *lexer;

  /* Previously registered file change callback.  */
  void (*file_change_prev) (cpp_reader *, const struct line_map *);
} pth_state;


/* Statistics on PTH.  */

struct pth_stats_d
{
  /* Number of files processed as a regular #include.  */
  size_t included_files;

  /* Number of valid images.  */
  size_t valid_images;

  /* Number of token hunks seen.  */
  size_t hunks;

  /* Number of valid hunks copied from images.  */
  size_t valid_hunks;

  /* Number of hunks that failed dependency checks.  */
  size_t invalid_hunks;

  /* Number of verified hunks.  */
  size_t verified_hunks;

  /* Number of verified identifiers.  */
  size_t verified_identifiers;
};

extern struct pth_stats_d pth_stats;

#define PTH_STATS_INCR(CNT,N)			\
  	do {					\
	    if (flag_pth_stats)			\
	      pth_stats.CNT += (N);		\
	} while (0)


/* Statistics on PPH.  */

struct pph_stats_d
{
  /* Number of tokens parsed in this TU.  */
  size_t parsed_tokens;

  /* Number of tokens in the lexer buffer.  */
  size_t lexed_tokens;

  /* Number of declarations copied into the parser cache.  */
  size_t cached_decls;

  /* Number of declarations restored from the parser cache.  */
  size_t restored_decls;

  /* Number of references rebound when going in/out of the cache.  */
  size_t cached_refs;

  /* Number of name lookups done by the parser.  */
  size_t name_lookups;

  /* Number of decl lookups that were changed to something weird.  */
  size_t bad_lookups;
};

extern struct pph_stats_d pph_stats;
  
#define PPH_STATS_INCR(CNT,N)			\
  	do {					\
	    if (flag_pph_stats)			\
	      pph_stats.CNT += (N);		\
	} while (0)


/* Maps for tracking decl dependencies.  For each *_DECL tree intercepted
   during parsing, we store the trees on which the node depends for
   its declaration.  Two maps are kept, one for the head of the declaration
   and another for its body.  */
struct pph_decl_deps_d
{
  /* Symbol dependencies on the declaration header.  */
  struct pointer_map_t *header;

  /* Symbol dependencies on the declaration body.  */
  struct pointer_map_t *body;
};

/* Global state.  FIXME pph, get rid of these.  */

/* Log file where PPH analysis is written to.  Controlled by
   -fpph_logfile.  If this flag is not given, stdout is used.  */
extern FILE *pph_logfile;

#define PPH_POP_TIMEVAR_AND_RETURN(TV, T)				\
  do {									\
    pph_catch_name_lookup (T);						\
    POP_TIMEVAR_AND_RETURN(TV, T);					\
  } while (0)


/* In pph.c  */
extern void pth_init (cp_lexer *);
extern void pth_finish (void);
extern void pth_debug_state (void);
extern void pth_debug_token_hunks (pth_image *);
extern pth_image *pth_image_lookup (pth_state *, const char *, cpp_reader *);
extern pth_state *pth_get_state (void);
extern void pth_print_stats (FILE *, cp_lexer *);
extern cp_token *pph_start_exposed (cp_parser *);
extern void pph_stop_exposed (cp_parser *, cp_token *);
extern void pph_init (void);
extern void pph_finish (void);
extern void pph_catch_tree (tree);
extern void pph_uncatch_tree (tree);
extern void pph_catch_name_lookup (tree);

#endif  /* GCC_CP_PPH_H  */
