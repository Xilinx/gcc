/* GIMPLE parser.

   Copyright 2010 Free Software Foundation, Inc.
   Contributed by Sandeep Soni and Diego Novillo.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "diagnostic.h"
#include "toplev.h"
#include "timevar.h"
#include "tree.h"
#include "gimple.h"
#include "parser.h"
#include "ggc.h"

/* The GIMPLE parser.  Note: do not use this variable directly.  It is
   declared here only to serve as a root for the GC machinery.  The
   parser pointer should be passed as a parameter to every function
   that needs to access it.  */
static GTY(()) gimple_parser *parser_gc_root__;

/* Consumes a token if the EXPECTED_TOKEN_TYPE is exactly the one we 
   are looking for. The token is obtained by reading it from the reader P.  */
 
static const gimple_token * 
gimple_parse_expect_token (cpp_reader *p, enum cpp_ttype expected_token_type)
{
  const gimple_token *next_token;

  next_token = cpp_peek_token (p, 0);

  /* If the token type does not match then we must report an error,
     otherwise consume the token.  */

  /* FIXME The error reported should be more precise to help 
     diagnostics similar to that reported by other front ends in
     the same case.  */

  if (next_token->type != expected_token_type)
    error ("expected token type %s instead of %s",
	    cpp_type2name (expected_token_type, 0),
	    cpp_type2name (next_token->type, 0));
  else
    next_token = cpp_get_token_with_location (p, &input_location);

  return next_token;
}


/* Helper for gimple_parse_assign_stmt and gimple_parse_cond_stmt.
   Peeks a token by reading from reader P and looks it up to match 
   against the tree codes.  */

static void
gimple_parse_expect_subcode (cpp_reader *p)
{
  const gimple_token *next_token;
  const char *text;
  int i;

  gimple_parse_expect_token (p, CPP_LESS);

  /* Peeks a token and looks it up for a match.  */
 
  next_token = cpp_peek_token (p, 0);
  text = (const char *) cpp_token_as_text (p, next_token);
  for (i = ERROR_MARK; i < LAST_AND_UNUSED_TREE_CODE; i++)
    if (strcasecmp (text, tree_code_name[i]) == 0)
      break;

  /* If none of the tree codes match, then report an error. Otherwise
     consume this token.  */
  if (i == LAST_AND_UNUSED_TREE_CODE)
    error ("Expected token should be one of the tree codes");
  else
    next_token = cpp_get_token (p);

  gimple_parse_expect_token (p, CPP_COMMA);

  /* FIXME From this function we should return the tree code since it
     can be used by the other helper functions to recognize precisely.  */
}

/* Helper for gimple_parse_assign_stmt. The token read from reader P should 
   be the lhs of the tuple.  */

static void 
gimple_parse_expect_lhs (cpp_reader *p)
{  
  const gimple_token *next_token;

  /* Just before the name of the identifier we might get the symbol 
     of dereference too. If we do get it then consume that token, else
     continue recognizing the name.  */
  next_token = cpp_peek_token (p, 0);
  if (next_token->type == CPP_MULT)
    next_token = cpp_get_token (p);

  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
}

/* Helper for gimple_parse_assign_stmt. The token read from reader P should 
   be the first operand in rhs of the tuple.  */

static void 
gimple_parse_expect_rhs1 (cpp_reader *p)
{
  const gimple_token *next_token;
  next_token = cpp_peek_token (p, 0);

  /* Currently there is duplication in the following blocks but there
     would be more stuff added here as we go on.  */

  /* ??? Can there be more possibilities than these ?  */
  switch (next_token->type)
    {
    case CPP_MULT:
    case CPP_AND:
      next_token = cpp_get_token (p);
      gimple_parse_expect_token (p, CPP_NAME);
      break;

    case CPP_NAME:
    case CPP_NUMBER:
    case CPP_STRING:
      next_token = cpp_get_token (p);
      break;

    default:
      break;
    }

  gimple_parse_expect_token (p, CPP_COMMA); 
}


/* Helper for gimple_parse_assign_stmt. The token read from reader P should 
   be the second operand in rhs of the tuple.  */

static void 
gimple_parse_expect_rhs2 (cpp_reader *p)
{
  const gimple_token *next_token;
  next_token = cpp_peek_token (p, 0);

  /* ??? Can there be more possibilities than these ?  */

  switch (next_token->type)
    {
    case CPP_NAME:
      /* Handle a special case, this can be NULL too.  */

    case CPP_NUMBER:
      next_token = cpp_get_token (p);
      break;

    default:
      break;
    }

  gimple_parse_expect_token (p, CPP_GREATER);  
}

/* Parse a gimple_assign tuple that is read from the reader P. For now we 
   only recognize the tuple. Refer gimple.def for the format of this tuple.  */

static void 
gimple_parse_assign_stmt (cpp_reader *p)
{
  gimple_parse_expect_subcode (p);
  gimple_parse_expect_lhs (p);
  gimple_parse_expect_rhs1 (p);
  gimple_parse_expect_rhs2 (p);
}

/* Helper for gimple_parse_cond_stmt. The token read from reader P should
   be the first operand in the tuple.  */
static void
gimple_parse_expect_op1 (cpp_reader *p)
{
  const gimple_token *next_token;
  next_token = cpp_peek_token (p, 0);

  switch (next_token->type)
    {
    case CPP_NAME:
    case CPP_NUMBER:
      next_token = cpp_get_token (p);
      break;

    default:
      break;
    }

  gimple_parse_expect_token (p, CPP_COMMA);  
}

/* Helper for gimple_parse_cond_stmt. The token read from reader P should
   be the second operand in the tuple.  */

static void
gimple_parse_expect_op2 (cpp_reader *p)
{
  const gimple_token *next_token;
  next_token = cpp_peek_token (p, 0);

  switch (next_token->type)
    {
    case CPP_NAME:
    case CPP_NUMBER:
    case CPP_STRING:
      next_token = cpp_get_token (p);
      break;

    case CPP_AND:
      next_token = cpp_get_token (p);
      gimple_parse_expect_token (p, CPP_NAME);
      break;

    default:
      break;
    }

  gimple_parse_expect_token (p, CPP_COMMA);  
}

/* Helper for gimple_parse_cond_stmt. The token read from reader P should
   be the true label in the tuple that means the label where the control
   jumps if the condition evaluates to true.  */

static void
gimple_parse_expect_true_label (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_GREATER);
  gimple_parse_expect_token (p, CPP_COMMA);  
}

/* Helper for gimple_parse_cond_stmt. The token read from reader P should
   be the false label in the tuple that means the label where the control
   jumps if the condition evaluates to false.  */

static void
gimple_parse_expect_false_label (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_RSHIFT);
}

/* Parse a gimple_cond tuple that is read from the reader P. For now we only 
   recognize the tuple. Refer gimple.def for the format of this tuple.  */

static void
gimple_parse_cond_stmt (cpp_reader *p)
{
  gimple_parse_expect_subcode (p);
  gimple_parse_expect_op1 (p);
  gimple_parse_expect_op2 (p);
  gimple_parse_expect_true_label (p);
  gimple_parse_expect_false_label (p);
}

/* Parse a gimple_goto tuple that is read from the reader P. For now we only 
   recognize the tuple. Refer gimple.def for the format of this tuple.  */

static void
gimple_parse_goto_stmt (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_LSHIFT);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_RSHIFT);
}

/* Parse a gimple_label tuple that is read from the reader P. For now we only 
   recognize the tuple. Refer gimple.def for the format of this tuple.  */

static void
gimple_parse_label_stmt (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_LSHIFT);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_RSHIFT);  
}

/* Parse a gimple_switch tuple that is read from the reader P. For now we only 
   recognize the tuple. Refer gimple.def for the format of this tuple.  */

static void
gimple_parse_switch_stmt (cpp_reader *p)
{
  const gimple_token *next_token;

  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COLON);
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);

  for (;;)
    {
      next_token = cpp_peek_token (p, 0);
      
      if (next_token->type == CPP_GREATER)
        {
          next_token = cpp_get_token (p);
          gimple_parse_expect_token (p, CPP_COMMA);
          gimple_parse_expect_token (p, CPP_NAME);
          gimple_parse_expect_token (p, CPP_NUMBER);
          gimple_parse_expect_token (p, CPP_COLON);
          gimple_parse_expect_token (p, CPP_LESS);
          gimple_parse_expect_token (p, CPP_NAME);  
        }
      else if (next_token->type == CPP_RSHIFT)
        {
          next_token = cpp_get_token (p);
          break;
        }
      else
        error ("Incorrect use of the gimple_switch statement");
    }
}

/* Helper for gimple_parse_call_stmt. The token read from reader P should
   be the name of the function called.  */

static void
gimple_parse_expect_function_name (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
}

/* Helper for gimple_parse_call_stmt. The token read from reader P should
   be the identifier in which the value is returned.  */

static void
gimple_parse_expect_return_var (cpp_reader *p)
{
  const gimple_token *next_token;

  next_token = cpp_peek_token (p, 0);

  if (next_token->type == CPP_NAME)
    next_token = cpp_get_token (p);
  
  /* There may be no variable in which the return value is collected.
     In that case this field in the tuple will contain NULL. We need 
     to handle it too.  */
}

/* Helper for gimple_parse_call_stmt. The token read from reader P should
   be the argument in the function call.  */

static void
gimple_parse_expect_argument (cpp_reader *p)
{
  const gimple_token *next_token;

  next_token = cpp_peek_token (p, 0);

  switch (next_token->type)
    {
    case CPP_NUMBER:
    case CPP_NAME:
      next_token = cpp_get_token (p);
      break;

    case CPP_MULT:
      next_token = cpp_get_token (p);
      gimple_parse_expect_token (p, CPP_NAME);
      break;

    default:
      error ("Incorrect way to specify an argument");
      break;
    }
}

/* Parse a gimple_call tuple that is read from the reader P. For now we only 
   recognize the tuple. Refer gimple.def for the format of this tuple.  */

static void
gimple_parse_call_stmt (cpp_reader *p)
{
  const gimple_token *next_token;

  gimple_parse_expect_function_name (p);
  gimple_parse_expect_return_var (p);
  
  for (;;)
    {
      next_token = cpp_peek_token (p, 0);
      if (next_token->type == CPP_GREATER)
        {
          next_token = cpp_get_token (p);
          break;
        }
      else if (next_token->type == CPP_COMMA)
        {
          next_token = cpp_get_token (p);
          gimple_parse_expect_argument (p);
        }
    } 
}

/* Parse a gimple_return tuple that is read from the reader P. For now we only 
   recognize the tuple. Refer gimple.def for the format of this tuple.  */

static void
gimple_parse_return_stmt (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_GREATER);  
}

/* The TOK read from the reader P is looked up for a match.  Calls the 
   corresponding function to do the parsing for the match.  Gets called
   for recognizing the statements in a function body.  */

static void 
gimple_parse_stmt (cpp_reader *p, const gimple_token *tok)
{
  const char *text;
  int i;
  text = (const char *) cpp_token_as_text (p, tok);
  for (i = GIMPLE_ERROR_MARK; i < LAST_AND_UNUSED_GIMPLE_CODE; i++)
    if (strcasecmp (text, gimple_code_name[i]) == 0)
      break;

  if (i == LAST_AND_UNUSED_GIMPLE_CODE)
    error ("Invalid gimple code used"); 
  else
    {
    switch (i)
      {
        case GIMPLE_ASSIGN:
          gimple_parse_assign_stmt (p);
          break;
        case GIMPLE_COND:
          gimple_parse_cond_stmt (p);
          break;
        case GIMPLE_LABEL:
          gimple_parse_label_stmt (p);
          break;
        case GIMPLE_GOTO:
          gimple_parse_goto_stmt (p);
          break;
        case GIMPLE_SWITCH:
          gimple_parse_switch_stmt (p);
          break;
        case GIMPLE_CALL:
          gimple_parse_call_stmt (p);
          break;
        case GIMPLE_RETURN:
          gimple_parse_return_stmt (p);
          break;
        default:
          break;
      }
    }
}


/* Helper for gimple_parse_expect_record_type and
   gimple_parse_expect_union_type. The field_decl's
   are read from cpp_reader P.  */
 
static void
gimple_parse_expect_field_decl (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NUMBER);
  gimple_parse_expect_token (p, CPP_RSHIFT);

}


/* The tuple syntax can be extended to types and declarations. The description
   of how it can be done is provided before each recognizer function.  */ 

/* <RECORD_TYPE<Name,Size,Field1,Field2,...FieldN>>
   where each of the FIELDi is a FIELD_DECL or a VAR_DECL, the representation 
   of which is given below.  

   For Ex: 
   Given a source code declaration as :

   struct some_struct {
   int first_var;
   float second_var;
   };

   The tuple representation is done as :
   RECORD_TYPE <some_struct,8,FIELD_DECL<first_var,INTEGER_TYPE<4>>,FIELD_DECL<second_var,REAL_TYPE<4>>>
*/   

/* Recognizer function for Record declarations. The record tuple is read
   from cpp_reader P.  */
 
static void
gimple_parse_record_type (cpp_reader *p)
{
  const gimple_token *next_token;

  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
  gimple_parse_expect_token (p, CPP_NUMBER);

  for (;;)
    {
      next_token = cpp_peek_token (p, 0);
      if (next_token->type == CPP_GREATER)
        {
          next_token = cpp_get_token (p);
          break;
        }
      else if (next_token->type == CPP_COMMA)
        {
          next_token = cpp_get_token (p);
          gimple_parse_expect_field_decl (p);
        }
    }  
}


/* <UNION_TYPE<Name,Size,Field1,Field2,...FieldN>>
   where each of the FIELDi is a FIELD_DECL or a VAR_DECL, the representation 
   of which is given below.  

   For Ex: 
   Given a source code declaration as :
   union some_union {
   int first_var;
   float second_var;
   };

   The tuple representation is done as :
   UNION_TYPE <some_union,4,FIELD_DECL<first_var,INTEGER_TYPE<4>>,FIELD_DECL<second_var,<REAL_TYPE<4>>>
*/

/* Recognizer function for Union declarations. The union tuple is read
   from cpp_reader P.  */

static void
gimple_parse_union_type (cpp_reader *p)
{
  const gimple_token *next_token;

  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
  gimple_parse_expect_token (p, CPP_NUMBER);

  for (;;)
    {
      next_token = cpp_peek_token (p, 0);
      if (next_token->type == CPP_GREATER)
        {
          next_token = cpp_get_token (p);
          break;
        }
      else if (next_token->type == CPP_COMMA)
        {
          next_token = cpp_get_token (p);
          gimple_parse_expect_field_decl (p);
        }
    }  
}


/* Helper for gimple_parse_enum_type. It is used to recognize a field in the
   enumeral. The field is read from the cpp_reader P.  */  

static void
gimple_parse_expect_const_decl (cpp_reader *p)
{
  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
  gimple_parse_expect_token (p, CPP_NUMBER);
  gimple_parse_expect_token (p, CPP_GREATER);  
}


/* <ENUMERAL_TYPE<Name,Size,Field1,Field2,...FieldN>>
   where each of the FIELDi is itself a pair of the form <Name,Value>.  

   For Ex: 
   Given a source code declaration as :
   enum some_enum {
   FIRST = 1,
   SECOND = 2,
   LAST = 5
   };

   The tuple representation is done as :
   ENUMERAL_TYPE<some_enum,3,<FIRST,1>,<SECOND,2>,<LAST,5>>
*/

static void
gimple_parse_enum_type (cpp_reader *p)
{
  const gimple_token *next_token;

  gimple_parse_expect_token (p, CPP_LESS);
  gimple_parse_expect_token (p, CPP_NAME);
  gimple_parse_expect_token (p, CPP_COMMA);
  gimple_parse_expect_token (p, CPP_NUMBER);

  for (;;)
    {
      next_token = cpp_peek_token (p, 0);
      if (next_token->type == CPP_RSHIFT)
	{
	  next_token = cpp_get_token (p);
	  break;
	}
      else if (next_token->type == CPP_COMMA)
	{
	  next_token = cpp_get_token (p);
	  gimple_parse_expect_const_decl (p);
	}
    }  
}


/* The TOK read from the reader P is looked up for a match. Calls the
   corresponding function to do the parsing for the match. Gets called
   for recognizing the type and variable declarations. */

static void
gimple_parse_type (cpp_reader *p, const gimple_token *tok)
{
  const char *text;
  int i;

  text = (const char *) cpp_token_as_text (p, tok);

  for (i = ERROR_MARK; i < LAST_AND_UNUSED_TREE_CODE; i++)
    if (strcasecmp (text, tree_code_name[i]) == 0)
      break;

  if (i == LAST_AND_UNUSED_TREE_CODE)
    error ("Invalid type code used");
  else
    {
      switch (i)
	{
	case RECORD_TYPE:
	  gimple_parse_record_type (p);
	  break;

	case UNION_TYPE:
	  gimple_parse_union_type (p);
	  break;

	case ENUMERAL_TYPE:
	  gimple_parse_enum_type (p);
	  break;

	default:
	  break;
	}
    }      
}


/* Initialize the lexer.  */

static gimple_lexer *
gl_init (gimple_parser *p)
{
  gimple_lexer *l;

  l = ggc_alloc_cleared_gimple_lexer ();
  l->parser = p;
  l->filename = main_input_filename;
  l->reader = cpp_create_reader (CLK_GNUC99, p->ident_hash, p->line_table);
  l->filename = cpp_read_main_file (l->reader, l->filename);
  l->cur_token_ix = 0;

  return l;
}


/* Initialize the parser data structures.  */

static gimple_parser *
gp_init (int debug_p)
{
  gimple_parser *p = ggc_alloc_cleared_gimple_parser ();
  p->debug_p = debug_p;
  line_table = p->line_table = ggc_alloc_cleared_line_maps ();
  p->ident_hash = ident_hash;
  linemap_init (p->line_table);
  p->lexer = gl_init (p);

  return p;
}


/* Get all the tokens from the file in LEXER.  */

static void
gl_lex (gimple_lexer *lexer)
{
  const gimple_token *gimple_tok;

  timevar_push (TV_CPP);

  do
    {
      location_t loc;

      gimple_tok = cpp_get_token_with_location (lexer->reader, &loc);
      if (gimple_tok->type != CPP_EOF)
	VEC_safe_push (gimple_token, gc, lexer->tokens, gimple_tok);
    }
  while (gimple_tok->type != CPP_EOF);

  timevar_pop (TV_CPP);
}


/* Consume the next token from PARSER.  */

static gimple_token *
gl_consume_token (gimple_lexer *lexer)
{
  return VEC_index (gimple_token, lexer->tokens, lexer->cur_token_ix++);
}

/* Parse the translation unit in PARSER.  */

static void
gp_parse (gimple_parser *parser)
{
  while (!VEC_empty (gimple_token, parser->lexer->tokens))
    {
      gimple_token *tok = gl_consume_token (parser->lexer);
      if (1)
	gimple_parse_type (parser->lexer->reader, tok);
      else
	gimple_parse_stmt (parser->lexer->reader, tok);
    }
}


/* Finalize parsing and release allocated memory in PARSER.  */

static void
gp_finish (gimple_parser *parser)
{
  cpp_finish (parser->lexer->reader, NULL);
  cpp_destroy (parser->lexer->reader);
  parser_gc_root__ = NULL;
}


/* Main entry point for the GIMPLE front end.  */

void
gimple_main (int debug_p)
{
  gimple_parser *parser;

  parser_gc_root__ = parser = gp_init (debug_p);

  if (parser->lexer->filename == NULL)
    return;

  gl_lex (parser->lexer);
  gp_parse (parser);
  gp_finish (parser);
}

#include "gt-gimple-parser.h"
