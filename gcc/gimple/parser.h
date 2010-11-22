/* GIMPLE parser declarations

   Copyright 2010 Free Software Foundation, Inc.
   Contributed by Sandeep Soni and Diego Novillo

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GIMPLE_PARSER_H
#define GIMPLE_PARSER_H

#include "cpplib.h"
#include "vec.h"

struct gimple_parser;
typedef cpp_token gimple_token;

DEF_VEC_O (gimple_token);
DEF_VEC_ALLOC_O (gimple_token, gc);

/* The GIMPLE lexer.  */

typedef struct GTY(()) gimple_lexer {
  /* Associated parser.  */
  struct gimple_parser *parser;

  /* Path to the main input file name.  */
  const char *filename;

  /* The cpp reader to get pre-processed tokens.  */
  struct GTY((skip)) cpp_reader *reader;

  /* The array of tokens read by the lexer.  */
  VEC(gimple_token, gc) *tokens;

  /* Token to be consumed by the parser.  */
  size_t cur_token_ix;
} gimple_lexer;


/* The GIMPLE parser.  */

typedef struct GTY(()) gimple_parser {
  /* Reader we use for lexing.  */
  gimple_lexer *lexer;

  /* Non-zero if '-dy' is enabled (dump debugging information during
     parsing).  */
  unsigned int debug_p : 1;

  /* Line table.  */
  struct line_maps *line_table;

  /* Identifier table.  */
  struct GTY((skip)) ht *ident_hash;
} gimple_parser;


/* In parser.c  */
extern void gimple_main (int);
 
#endif /* GIMPLE_PARSER_H  */
