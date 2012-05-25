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
#include "hashtab.h"
#include "ggc.h"

/* The GIMPLE parser.  Note: do not use this variable directly.  It is
   declared here only to serve as a root for the GC machinery.  The
   parser pointer should be passed as a parameter to every function
   that needs to access it.  */
static GTY(()) gimple_parser *parser_gc_root__;

/* Declare debugging functions to make them available in the debugger.  */
extern void gl_dump_token (FILE *, gimple_token *);
extern void gl_dump (FILE *, gimple_lexer *); 
extern void gp_dump (FILE *, gimple_parser *);

/* EOF token.  */
static gimple_token gl_eof_token = { CPP_EOF, 0, 0, 0 };

/* Gimple symbol table.  */

static GTY ((if_marked ("gimple_symtab_entry_marked_p"),
	     param_is (struct gimple_symtab_entry_def)))
  htab_t gimple_symtab;

/* Return the hash value of the declaration name of a gimple_symtab_entry_def
   object pointed by ENTRY.  */

static hashval_t
gimple_symtab_entry_hash (const void *entry)
{
  const struct gimple_symtab_entry_def *base =
    (const struct gimple_symtab_entry_def *)entry;
  return IDENTIFIER_HASH_VALUE (base->id);
}

/* Returns non-zero if ENTRY1 and ENTRY2 point to gimple_symtab_entry_def
   objects corresponding to the same declaration.  */

static int
gimple_symtab_eq_hash (const void *entry1, const void *entry2)
{
  const struct gimple_symtab_entry_def *base1 =
    (const struct gimple_symtab_entry_def *)entry1;
  const struct gimple_symtab_entry_def *base2 =
    (const struct gimple_symtab_entry_def *)entry2;

  return (base1->id == base2->id);
}

/* Returns non-zero if P points to an gimple_symtab_entry_def struct that needs
   to be marked for GC.  */

static int
gimple_symtab_entry_marked_p (const void *p)
{
  const struct gimple_symtab_entry_def *base =
     (const struct gimple_symtab_entry_def *) p;

  /* Keep this only if the common IDENTIFIER_NODE of the symtab chain
     is marked which it will be if at least one of the DECLs in the
     chain is marked.  */
  return ggc_marked_p (base->id);
}


/* Lazily initialize hash tables.  */

static void
gimple_symtab_maybe_init_hash_table (void)
{
  if (gimple_symtab)
    return;

  gimple_symtab =
    htab_create_ggc (1021, gimple_symtab_entry_hash,
		     gimple_symtab_eq_hash, NULL);
}

/* Registers DECL with the gimple symbol table as having identifier ID.  */

static void
gimple_symtab_register_decl (tree decl, tree id)
{
  gimple_symtab_entry_t new_entry;
  void **slot;

  new_entry = ggc_alloc_cleared_gimple_symtab_entry_def ();
  new_entry->id = id;
  new_entry->decl = decl;

  gimple_symtab_maybe_init_hash_table ();
  slot = htab_find_slot (gimple_symtab, new_entry, INSERT); 
  if (*slot == NULL)
    *slot = new_entry;
}

/* Return the string representation of token TOKEN.  */

static const char *
gl_token_as_text (const gimple_token *token)
{
  switch (token->type)
    {
    case CPP_NAME:
      return IDENTIFIER_POINTER (token->value);

    case CPP_STRING:
    case CPP_STRING16:
    case CPP_STRING32:
    case CPP_WSTRING:
    case CPP_UTF8STRING:
      return TREE_STRING_POINTER (token->value);
      break;

    default:
      return cpp_type2name (token->type, token->flags);
    }
}

/* Helper function to register the variable declaration having token NAME_TOKEN
   in the global gimple symbol table.  */

static void
gimple_register_var_decl_in_symtab (const gimple_token *name_token)
{
  const char *name = gl_token_as_text (name_token);
  tree id = get_identifier (name);
  tree decl = build_decl (name_token->location, VAR_DECL,
			  get_identifier(name), void_type_node);
  gimple_symtab_register_decl (decl,id);
}

/* Return true if we have reached the end of LEXER's token buffer.  */

static bool
gl_at_eof (gimple_lexer *lexer)
{
  return lexer->cur_token_ix >= VEC_length (gimple_token, lexer->tokens);
}


/* Peek into the next token from LEXER.  */

static gimple_token *
gl_peek_token (gimple_lexer *lexer)
{
  if (gl_at_eof (lexer))
    return &gl_eof_token;
  return VEC_index (gimple_token, lexer->tokens, lexer->cur_token_ix);
}


/* Consume the next token from LEXER.  */

static gimple_token *
gl_consume_token (gimple_lexer *lexer)
{
  gimple_token *tok = gl_peek_token (lexer);
  lexer->cur_token_ix++;
  return tok;
}


/* Return the tree code for TOKEN.  Returns LAST_AND_UNUSED_TREE_CODE
   if no match is found.  */

static enum tree_code
gl_tree_code_for_token (const gimple_token *token)
{
  size_t code;
  const char *s;

  /* FIXME.  Expensive linear scan, convert into a string->code map.  */
  s = gl_token_as_text (token);
  for (code = ERROR_MARK; code < LAST_AND_UNUSED_TREE_CODE; code++)
    if (strcasecmp (s, tree_code_name[code]) == 0)
      break;

  return (enum tree_code) code;
}


/* Return the gimple code for TOKEN.  Returns LAST_AND_UNUSED_GIMPLE_CODE
   if no match is found.  */

static enum gimple_code
gl_gimple_code_for_token (const gimple_token *token)
{
  size_t code;
  const char *s;

  /* FIXME.  Expensive linear scan, convert into a string->code map.  */
  s = gl_token_as_text (token);
  for (code = GIMPLE_ERROR_MARK; code < LAST_AND_UNUSED_GIMPLE_CODE; code++)
    if (strcasecmp (s, gimple_code_name[code]) == 0)
      break;

  return (enum gimple_code) code;
}


/* Return true if TOKEN is the start of a declaration.  */

static bool
gl_token_starts_decl (gimple_token *token)
{
  enum tree_code code = gl_tree_code_for_token (token);
  return code == VAR_DECL;
}


/* Return true if TOKEN is the start of a type declaration.  */

static bool
gl_token_starts_type (gimple_token *token)
{
  enum tree_code code = gl_tree_code_for_token (token);
  return code == RECORD_TYPE || code == UNION_TYPE || code == ENUMERAL_TYPE;
}


/* Dump TOKEN to FILE.  If FILE is NULL, stderr is used.  */

void
gl_dump_token (FILE *file, gimple_token *token)
{
  if (file == NULL)
    file = stderr;

  fprintf (file, "%s", gl_token_as_text (token));

  if (token->type == CPP_CLOSE_BRACE || token->type == CPP_SEMICOLON)
    fprintf (file, "\n");
  else
    {
      fprintf (file, " ");
      fflush (file);
    }
}


/* Dump debugging information about LEXER to FILE.  If FILE is NULL,
   stderr is used.  */

void
gl_dump (FILE *file, gimple_lexer *lexer)
{
  unsigned i;
  gimple_token *token;

  if (file == NULL)
    file = stderr;

  fprintf (file, "%s: %u tokens, current token index: %u\n",
	   lexer->filename, VEC_length (gimple_token, lexer->tokens),
	   lexer->cur_token_ix);

  for (i = 0; VEC_iterate (gimple_token, lexer->tokens, i, token); i++)
    {
      if (i == lexer->cur_token_ix)
	fprintf (file, "[[[ ");
      gl_dump_token (file, token);
      if (i == lexer->cur_token_ix)
	fprintf (file, "]]]");
    }

  fprintf (file, "\n");
}


/* Dump debugging information about PARSER to FILE.  If FILE is
   NULL, stderr is used.  */

void
gp_dump (FILE *file, gimple_parser *parser)
{
  gl_dump (file, parser->lexer);
}


/* Returns the next token from LEXER if its type is the same as 
   EXPECTED.  Otherwise, it issues an error message.  */
 
static const gimple_token * 
gl_consume_expected_token (gimple_lexer *lexer, enum cpp_ttype expected)
{
  const gimple_token *next_token = gl_consume_token (lexer);
  if (next_token->type != expected && !errorcount)
    error_at (next_token->location,
	      "token '%s' is not of the expected type '%s'",
	      gl_token_as_text (next_token), cpp_type2name (expected, 0));

  return next_token;
}


/* Helper for gp_parse_assign_stmt and gp_parse_cond_stmt.
   Consumes a token by reading from reader PARSER and looks it up to match
   against the tree codes.  Returns the tree code or emits a diagnostic
   if no valid tree code was found.  Additionally, if TOKEN_P is not
   NULL, it returns the read token in *TOKEN_P.  */

static enum tree_code
gp_parse_expect_subcode (gimple_parser *parser, gimple_token **token_p)
{
  gimple_token *next_token;
  enum tree_code code;

  gl_consume_expected_token (parser->lexer, CPP_LESS);

  /* Peeks a token and looks it up for a match.  */
  next_token = gl_consume_token (parser->lexer);
  code = gl_tree_code_for_token (next_token);

  /* If none of the tree codes match, then report an error. Otherwise
     consume this token.  */
  if (code == LAST_AND_UNUSED_TREE_CODE)
    error_at (next_token->location, 
	      "Unmatched tree code for token '%s'",
	      gl_token_as_text (next_token));

  gl_consume_expected_token (parser->lexer, CPP_COMMA);

  if (token_p)
    *token_p = next_token;

  return code;
}


/* Helper for gp_parse_assign_stmt. The token read from reader PARSER should 
   be the lhs of the tuple.  */

static void 
gp_parse_expect_lhs (gimple_parser *parser)
{  
  const gimple_token *next_token;

  /* Just before the name of the identifier we might get the symbol 
     of dereference too. If we do get it then consume that token, else
     continue recognizing the name.  */
  next_token = gl_peek_token (parser->lexer);
  if (next_token->type == CPP_MULT)
    next_token = gl_consume_token (parser->lexer);

  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
}


/* Helper for gp_parse_assign_stmt. The token read from reader PARSER should 
   be the first operand in rhs of the tuple.  */

static void 
gp_parse_expect_rhs_op (gimple_parser *parser)
{
  const gimple_token *next_token;

  next_token = gl_peek_token (parser->lexer);

  /* Currently there is duplication in the following blocks but there
     would be more stuff added here as we go on.  */

  /* ??? Can there be more possibilities than these ?  */
  switch (next_token->type)
    {
    case CPP_MULT:
    case CPP_AND:
      gl_consume_expected_token (parser->lexer, CPP_NAME);
      break;

    case CPP_NAME:
    case CPP_NUMBER:
    case CPP_STRING:
      next_token = gl_consume_token (parser->lexer);
      break;

    default:
      break;
    }
}


/* Parse a gimple_assign tuple that is read from the reader PARSER.
   For now we only recognize the tuple. Refer gimple.def for the
   format of this tuple.  */

static void 
gp_parse_assign_stmt (gimple_parser *parser)
{
  gimple_token *optoken;
  enum tree_code opcode;
  enum gimple_rhs_class rhs_class;

  opcode = gp_parse_expect_subcode (parser, &optoken);
  gp_parse_expect_lhs (parser);

  rhs_class = get_gimple_rhs_class (opcode);
  switch (rhs_class)
    {
      case GIMPLE_INVALID_RHS:
	error_at (optoken->location, "Invalid RHS for "
		  "gimple assignment: %s", tree_code_name[opcode]);
	break;

      case GIMPLE_SINGLE_RHS:
      case GIMPLE_UNARY_RHS:
      case GIMPLE_BINARY_RHS:
      case GIMPLE_TERNARY_RHS:
	gp_parse_expect_rhs_op (parser);
	if (rhs_class == GIMPLE_BINARY_RHS || rhs_class == GIMPLE_TERNARY_RHS)
	  {
	    gl_consume_expected_token (parser->lexer, CPP_COMMA);
	    gp_parse_expect_rhs_op (parser);
	  }
	if (rhs_class == GIMPLE_TERNARY_RHS)
	  {
	    gl_consume_expected_token (parser->lexer, CPP_COMMA);
	    gp_parse_expect_rhs_op (parser);
	  }
	break;

      default:
	gcc_unreachable ();
    }

  gl_consume_expected_token (parser->lexer, CPP_GREATER);
}

/* Helper for gp_parse_cond_stmt. The token read from reader PARSER should
   be the first operand in the tuple.  */

static void
gp_parse_expect_op1 (gimple_parser *parser)
{
  const gimple_token *next_token;
  next_token = gl_consume_token (parser->lexer);

  switch (next_token->type)
    {
    case CPP_NAME:
    case CPP_NUMBER:
      break;

    default:
      break;
    }

  gl_consume_expected_token (parser->lexer, CPP_COMMA);  
}

/* Helper for gp_parse_cond_stmt. The token read from reader PARSER should
   be the second operand in the tuple.  */

static void
gp_parse_expect_op2 (gimple_parser *parser)
{
  const gimple_token *next_token;
  next_token = gl_consume_token (parser->lexer);

  switch (next_token->type)
    {
    case CPP_NAME:
    case CPP_NUMBER:
    case CPP_STRING:
      break;

    case CPP_AND:
      next_token = gl_consume_token (parser->lexer);
      gl_consume_expected_token (parser->lexer, CPP_NAME);
      break;

    default:
      break;
    }

  gl_consume_expected_token (parser->lexer, CPP_COMMA);  
}

/* Helper for gp_parse_cond_stmt. The token read from reader PARSER should
   be the true label in the tuple that means the label where the control
   jumps if the condition evaluates to true.  */

static void
gp_parse_expect_true_label (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);  
}

/* Helper for gp_parse_cond_stmt. The token read from reader PARSER should
   be the false label in the tuple that means the label where the control
   jumps if the condition evaluates to false.  */

static void
gp_parse_expect_false_label (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);
}

/* Parse a gimple_cond tuple that is read from the reader PARSER. For
   now we only recognize the tuple. Refer gimple.def for the format of
   this tuple.  */

static void
gp_parse_cond_stmt (gimple_parser *parser)
{
  gimple_token *optoken;
  enum tree_code opcode = gp_parse_expect_subcode (parser, &optoken);
  if (get_gimple_rhs_class (opcode) != GIMPLE_BINARY_RHS)
    error_at (optoken->location, "Unsupported gimple_cond expression");
  gp_parse_expect_op1 (parser);
  gp_parse_expect_op2 (parser);
  gp_parse_expect_true_label (parser);
  gp_parse_expect_false_label (parser);
}

/* Parse a gimple_goto tuple that is read from the reader PARSER. For
   now we only recognize the tuple. Refer gimple.def for the format of
   this tuple.  */

static void
gp_parse_goto_stmt (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);
}

/* Parse a gimple_label tuple that is read from the reader PARSER. For
   now we only recognize the tuple. Refer gimple.def for the format of
   this tuple.  */

static void
gp_parse_label_stmt (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);  
}

/* Parse a gimple_switch tuple that is read from the reader PARSER.
   For now we only recognize the tuple. Refer gimple.def for the
   format of this tuple.  */

static void
gp_parse_switch_stmt (gimple_parser *parser)
{
  const gimple_token *next_token;

  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COLON);
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);

  while (!gl_at_eof (parser->lexer))
    {
      next_token = gl_consume_token (parser->lexer);
      
      if (next_token->type == CPP_COMMA)
        {
          gl_consume_expected_token (parser->lexer, CPP_NAME);
          gl_consume_expected_token (parser->lexer, CPP_NUMBER);
          gl_consume_expected_token (parser->lexer, CPP_COLON);
          gl_consume_expected_token (parser->lexer, CPP_LESS);
          gl_consume_expected_token (parser->lexer, CPP_NAME);
          gl_consume_expected_token (parser->lexer, CPP_GREATER);
        }
      else if (next_token->type == CPP_GREATER)
        break;
      else
        error_at (next_token->location, 
	          "Incorrect use of the gimple_switch statement");
    }
}


/* Helper for gp_parse_call_stmt. The token read from reader PARSER should
   be the name of the function called.  */

static void
gp_parse_expect_function_name (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
}

/* Helper for gp_parse_call_stmt. The token read from reader PARSER should
   be the identifier in which the value is returned.  */

static void
gp_parse_expect_return_var (gimple_parser *parser)
{
  const gimple_token *next_token;

  next_token = gl_consume_token (parser->lexer);

  if (next_token->type == CPP_NAME)
    next_token = gl_consume_token (parser->lexer);
  
  /* There may be no variable in which the return value is collected.
     In that case this field in the tuple will contain NULL. We need 
     to handle it too.  */
}


/* Helper for gp_parse_call_stmt. The token read from reader PARSER should
   be the argument in the function call.  */

static void
gp_parse_expect_argument (gimple_parser *parser)
{
  const gimple_token *next_token;

  next_token = gl_consume_token (parser->lexer);

  switch (next_token->type)
    {
    case CPP_NUMBER:
    case CPP_NAME:
      break;

    case CPP_MULT:
      next_token = gl_consume_token (parser->lexer);
      gl_consume_expected_token (parser->lexer, CPP_NAME);
      break;

    default:
      error_at (next_token->location, "Incorrect way to specify an argument");
      break;
    }
}


/* Parse a gimple_call tuple that is read from PARSER.  */

static void
gp_parse_call_stmt (gimple_parser *parser)
{
  const gimple_token *next_token;

  gp_parse_expect_function_name (parser);
  gp_parse_expect_return_var (parser);
  
  while (!gl_at_eof (parser->lexer))
    {
      next_token = gl_consume_token (parser->lexer);
      if (next_token->type == CPP_GREATER)
	break;
      else if (next_token->type == CPP_COMMA)
        {
          next_token = gl_consume_token (parser->lexer);
          gp_parse_expect_argument (parser);
        }
    } 
}


/* Parse a gimple_return tuple that is read from PARSER.  */

static void
gp_parse_return_stmt (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);  
}


/* The TOKEN read from the reader PARSER is looked up for a match.  Calls the 
   corresponding function to do the parsing for the match.  Gets called
   for recognizing the statements in a function body.  */

static void 
gp_parse_stmt (gimple_parser *parser, const gimple_token *token)
{
  enum gimple_code code = gl_gimple_code_for_token (token);

  if (code == LAST_AND_UNUSED_GIMPLE_CODE && !errorcount)
    error_at (token->location, "Invalid gimple code used"); 
  else
    {
    switch (code)
      {
        case GIMPLE_ASSIGN:
          gp_parse_assign_stmt (parser);
          break;
        case GIMPLE_COND:
          gp_parse_cond_stmt (parser);
          break;
        case GIMPLE_LABEL:
          gp_parse_label_stmt (parser);
          break;
        case GIMPLE_GOTO:
          gp_parse_goto_stmt (parser);
          break;
        case GIMPLE_SWITCH:
          gp_parse_switch_stmt (parser);
          break;
        case GIMPLE_CALL:
          gp_parse_call_stmt (parser);
          break;
        case GIMPLE_RETURN:
          gp_parse_return_stmt (parser);
          break;
        default:
          break;
      }
    }
}


/* Helper for gp_parse_expect_record_type and
   gp_parse_expect_union_type. The field_decl's
   are read from gimple_parser PARSER.  */
 
static void
gp_parse_expect_field_decl (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NUMBER);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);

}


/* The tuple syntax can be extended to types and declarations. The
   description of how it can be done is provided before each
   recognizer function.  */ 

/* <RECORD_TYPE<Name,Size,Field1,Field2,...FieldN>>
   where each of the FIELDi is a FIELD_DECL or a VAR_DECL, the representation 
   of which is given below.  

   Example:
   Given a source code declaration as :

   struct some_struct {
    int first_var;
    float second_var;
   };

   The gimple representation should read:
   RECORD_TYPE <some_struct, 8,
      FIELD_DECL <first_var, INTEGER_TYPE<4>>,
      FIELD_DECL <second_var,REAL_TYPE<4>>>
*/

/* Recognizer function for structure declarations. The structure tuple
   is read from PARSER.  */

static void
gp_parse_record_type (gimple_parser *parser)
{
  const gimple_token *next_token;

  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
  gl_consume_expected_token (parser->lexer, CPP_NUMBER);

  while (!gl_at_eof (parser->lexer))
    {
      next_token = gl_consume_token (parser->lexer);
      if (next_token->type == CPP_GREATER)
	break;
      else if (next_token->type == CPP_COMMA)
        {
          next_token = gl_consume_token (parser->lexer);
          gp_parse_expect_field_decl (parser);
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
   UNION_TYPE <some_union,4,
      FIELD_DECL <first_var,INTEGER_TYPE<4>>,
      FIELD_DECL<second_var,<REAL_TYPE<4>>>
*/

/* Recognizer function for Union declarations. The union tuple is read
   from gimple_parser PARSER.  */

static void
gp_parse_union_type (gimple_parser *parser)
{
  const gimple_token *next_token;

  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
  gl_consume_expected_token (parser->lexer, CPP_NUMBER);

  while (!gl_at_eof (parser->lexer))
    {
      next_token = gl_consume_token (parser->lexer);
      if (next_token->type == CPP_GREATER)
	break;
      else if (next_token->type == CPP_COMMA)
        {
          next_token = gl_consume_token (parser->lexer);
          gp_parse_expect_field_decl (parser);
        }
    }  
}


/* Helper for gp_parse_enum_type. It is used to recognize a field in the
   enumeral. The field is read from the gimple_parser PARSER.  */  

static void
gp_parse_expect_const_decl (gimple_parser *parser)
{
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
  gl_consume_expected_token (parser->lexer, CPP_NUMBER);
  gl_consume_expected_token (parser->lexer, CPP_GREATER);  
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
   ENUMERAL_TYPE<some_enum,3,
      <FIRST,1>,
      <SECOND,2>,
      <LAST,5>>
*/

static void
gp_parse_enum_type (gimple_parser *parser)
{
  const gimple_token *next_token;

  gl_consume_expected_token (parser->lexer, CPP_LESS);
  gl_consume_expected_token (parser->lexer, CPP_NAME);
  gl_consume_expected_token (parser->lexer, CPP_COMMA);
  gl_consume_expected_token (parser->lexer, CPP_NUMBER);

  while (!gl_at_eof (parser->lexer))
    {
      next_token = gl_consume_token (parser->lexer);
      if (next_token->type == CPP_GREATER)
	break;
      else if (next_token->type == CPP_COMMA)
	gp_parse_expect_const_decl (parser);
    }  
}


/* The token TOKEN read from the reader PARSER is looked up for a match.
   Calls the corresponding function to do the parsing for the match.
   Gets called for recognizing the type and variable declarations. */

static void
gp_parse_type (gimple_parser *parser, const gimple_token *token)
{
  enum tree_code code = gl_tree_code_for_token (token);
  switch (code)
    {
    case RECORD_TYPE:
      gp_parse_record_type (parser);
      break;

    case UNION_TYPE:
      gp_parse_union_type (parser);
      break;

    case ENUMERAL_TYPE:
      gp_parse_enum_type (parser);
      break;

    default:
      break;
    }
}


/* The Declaration section within a .gimple file can consist of 
   a) Declaration of variables.
   b) Declaration of functions.

   The syntax of a variable declaration is as follows:

   <VAR_DECL<Name, Type>>

   Following are the broad cases for which the syntax of a variable 
   declaration is described:

   Example:

   1. C-like declaration as,
     int var;

   The Corresponding gimple syntax,
     VAR_DECL <var, INTEGER_TYPE <4>>

   In General,any variable of an atomic data type,
     VAR_DECL <var_name, TYPE <size>>

   2. C-like declaration as,
     int array[10];

   The Corresponding gimple syntax,
     VAR_DECL <name, ARRAY_TYPE < 0 , 9 , INTEGER_TYPE <4>>>

   In General,any variable of an array type,
     VAR_DECL <array_name, ARRAY_TYPE < min_index, max_index, TYPE <size>>>

   3. C-like declaration as,
     int *ptr;

   The Corresponding gimple syntax,
     VAR_DECL <ptr,POINTER_TYPE < INTEGER_TYPE <4>>>

   In General,any variable of a pointer type,
     VAR_DECL <pointer_name, POINTER_TYPE <TYPE<size>>>
 
   Note: The Nested Type in the Pointer Type tuple is the type of 
         the element to which the variable points.

   4. C-like declaration as,
    struct A a;

   The Corresponding gimple syntax,
     VAR_DECL <a, RECORD_TYPE <A>>

   In General, any variable of an aggregate type,
     VAR_DECL <var_name, AGGREGATE_TYPE <aggregate_name>>

   Note: 1) Records, Unions and Enumerals are considered as Aggregates.  
	 2) For the aggregates we store only the name of the aggregate.
            The other properties of the aggregate will already be stored 
            from the type declarations parsing and can thus be deduced.   */

/* Recognizer function for variable declarations. The declaration tuple is read
   from gimple_parser PARSER.  */

static void
gp_parse_var_decl (gimple_parser *parser)
{
  const gimple_token *next_token, *name_token;
  enum tree_code code ;
 
  gl_consume_expected_token (parser->lexer, CPP_LESS);
  name_token = gl_consume_expected_token (parser->lexer, CPP_NAME);

  gimple_register_var_decl_in_symtab (name_token);

  gl_consume_expected_token (parser->lexer, CPP_COMMA);
  next_token = gl_consume_token (parser->lexer);
  code = gl_tree_code_for_token (next_token);
  switch (code)
    {
    case INTEGER_TYPE:
    case REAL_TYPE:
      gl_consume_expected_token (parser->lexer, CPP_LESS);
      gl_consume_expected_token (parser->lexer, CPP_NUMBER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      break;

    case ARRAY_TYPE:
      gl_consume_expected_token (parser->lexer, CPP_LESS);
      gl_consume_expected_token (parser->lexer, CPP_NUMBER);
      gl_consume_expected_token (parser->lexer, CPP_COMMA);
      gl_consume_expected_token (parser->lexer, CPP_NUMBER);
      gl_consume_expected_token (parser->lexer, CPP_COMMA);
      
      /*TODO The tree code that we recognize below can be processed further.
	     No action is taken for now.  */

      gl_consume_expected_token (parser->lexer, CPP_NAME);
      gl_consume_expected_token (parser->lexer, CPP_LESS);
      gl_consume_expected_token (parser->lexer, CPP_NUMBER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      break;

    case POINTER_TYPE:
      gl_consume_expected_token (parser->lexer, CPP_LESS);

      /*TODO The tree code that we recognize below can be processed further.
	     No action is taken for now.  */

      gl_consume_expected_token (parser->lexer, CPP_NAME);
      gl_consume_expected_token (parser->lexer, CPP_LESS);
      gl_consume_expected_token (parser->lexer, CPP_NUMBER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      break;

    case RECORD_TYPE:
    case UNION_TYPE:
    case ENUMERAL_TYPE:
      gl_consume_expected_token (parser->lexer, CPP_LESS);
      gl_consume_expected_token (parser->lexer, CPP_NAME);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      gl_consume_expected_token (parser->lexer, CPP_GREATER);
      break;

    default: 
      break;
    }
}


/* The token TOKEN read from the reader PARSER is looked up for a match.
   Calls the corresponding function to do the parsing for the match.
   Gets called for recognizing variable and function declarations. */

static void
gp_parse_decl (gimple_parser *parser, const gimple_token *token)
{
  enum tree_code code = gl_tree_code_for_token (token);
  switch (code)
    {
    case VAR_DECL:
      gp_parse_var_decl (parser);
      break;

    default:
      break;
    }
}


/* Initialize the lexer.  PARSER points to the main parsing object.
   FNAME is the name of the input gimple file being compiled.  */

static gimple_lexer *
gl_init (gimple_parser *parser, const char *fname)
{
  gimple_lexer *lexer;

  lexer = ggc_alloc_cleared_gimple_lexer ();
  lexer->parser = parser;
  lexer->filename = fname;
  /* FIXME.  Choose GNU C99 for now, a better default should be
     used here.  */
  lexer->reader = cpp_create_reader (CLK_GNUC99, parser->ident_hash,
				     parser->line_table);
  lexer->filename = cpp_read_main_file (lexer->reader, lexer->filename);
  lexer->cur_token_ix = 0;

  return lexer;
}


/* A helper function; used as the reallocator function for cpp's line
   table.  */

static void *
realloc_for_line_map (void *ptr, size_t len)
{
  return GGC_RESIZEVAR (void, ptr, len);
}


/* Initialize the parser data structures.  FNAME is the name of the input
   gimple file being compiled.  */

static gimple_parser *
gp_init (const char *fname)
{
  gimple_parser *parser = ggc_alloc_cleared_gimple_parser ();
  line_table = parser->line_table = ggc_alloc_line_maps ();
  linemap_init (parser->line_table);
  parser->line_table->reallocator = realloc_for_line_map;
  parser->line_table->round_alloc_size = ggc_round_alloc_size;
  parser->ident_hash = ident_hash;
  parser->lexer = gl_init (parser, fname);

  return parser;
}


/* FIXME.  Copied over from c-family/c-lex.c.  Adapt to GIMPLE.
   Returns the narrowest unsigned type, starting with the
   minimum specified by cpplib FLAGS, that can fit HIGH:LOW, or
   itk_none if there isn't one.  */

static enum integer_type_kind
narrowest_unsigned_type (unsigned HOST_WIDE_INT low,
			 unsigned HOST_WIDE_INT high,
			 unsigned int flags)
{
  int itk;

  if ((flags & CPP_N_WIDTH) == CPP_N_SMALL)
    itk = itk_unsigned_int;
  else if ((flags & CPP_N_WIDTH) == CPP_N_MEDIUM)
    itk = itk_unsigned_long;
  else
    itk = itk_unsigned_long_long;

  for (; itk < itk_none; itk += 2 /* skip unsigned types */)
    {
      tree upper;

      if (integer_types[itk] == NULL_TREE)
	continue;
      upper = TYPE_MAX_VALUE (integer_types[itk]);

      if ((unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (upper) > high
	  || ((unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (upper) == high
	      && TREE_INT_CST_LOW (upper) >= low))
	return (enum integer_type_kind) itk;
    }

  return itk_none;
}


/* Ditto, but narrowest signed type.
   FIXME.  Copied over from c-family/c-lex.c.  Adapt to GIMPLE.  */

static enum integer_type_kind
narrowest_signed_type (unsigned HOST_WIDE_INT low,
		       unsigned HOST_WIDE_INT high, unsigned int flags)
{
  int itk;

  if ((flags & CPP_N_WIDTH) == CPP_N_SMALL)
    itk = itk_int;
  else if ((flags & CPP_N_WIDTH) == CPP_N_MEDIUM)
    itk = itk_long;
  else
    itk = itk_long_long;


  for (; itk < itk_none; itk += 2 /* skip signed types */)
    {
      tree upper;

      if (integer_types[itk] == NULL_TREE)
	continue;
      upper = TYPE_MAX_VALUE (integer_types[itk]);

      if ((unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (upper) > high
	  || ((unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (upper) == high
	      && TREE_INT_CST_LOW (upper) >= low))
	return (enum integer_type_kind) itk;
    }

  return itk_none;
}

/* Returns true if the type of the TOKEN is equal to EXPECTED.  */

static bool
gl_token_is_of_type (gimple_token *token, enum cpp_ttype expected)
{
  return (token->type == expected);
}

/* Splits the token TOKEN into two tokens FIRST_TOKEN and SECOND_TOKEN.
   Note that the split should work only if the type of the TOKEN is
   either CPP_RSHIFT or CPP_LSHIFT which gets split into two tokens
   of the type CPP_GREATER or CPP_LESS respectively.  */

static void
gl_split_token (gimple_token *token, gimple_token *first_token,
		    gimple_token *second_token)
{
  switch (token->type)
    {
    case CPP_RSHIFT:
      first_token->type = CPP_GREATER;
      second_token->type = CPP_GREATER;
      break;

    case CPP_LSHIFT:
      first_token->type = CPP_LESS;
      second_token->type = CPP_LESS;
      break;

    default:
      gcc_unreachable();
    }

  first_token->location = second_token->location = token->location;
  first_token->flags = second_token->flags = token->flags;
  first_token->value = second_token->value = token->value;
}

/* Interpret TOKEN, an integer with FLAGS as classified by cpplib.  */

static tree
gl_interpret_integer (gimple_lexer *lexer, const cpp_token *token,
		      unsigned int flags)
{
  tree value, type;
  enum integer_type_kind itk;
  cpp_num integer;
  cpp_options *options = cpp_get_options (lexer->reader);

  integer = cpp_interpret_integer (lexer->reader, token, flags);
  integer = cpp_num_sign_extend (integer, options->precision);

  /* The type of a constant with a U suffix is straightforward.  */
  if (flags & CPP_N_UNSIGNED)
    itk = narrowest_unsigned_type (integer.low, integer.high, flags);
  else
    itk = narrowest_signed_type (integer.low, integer.high, flags);

  if (itk == itk_none)
    itk = (flags & CPP_N_UNSIGNED) ? itk_unsigned_int128 : itk_int128;

  type = integer_types[itk];
  value = build_int_cst_wide (type, integer.low, integer.high);

  /* Convert imaginary to a complex type.  */
  if (flags & CPP_N_IMAGINARY)
    value = build_complex (NULL_TREE, build_int_cst (type, 0), value);

  return value;
}


/* Interpret TOKEN, a floating point number with FLAGS as classified
   by cpplib.  */

static tree
gl_interpret_float (const cpp_token *token, unsigned int flags)
{
  tree type;
  tree const_type;
  tree value;
  REAL_VALUE_TYPE real;
  REAL_VALUE_TYPE real_trunc;
  char *copy;
  size_t copylen;

  /* Decode type based on width and properties. */
  if (flags & CPP_N_DFLOAT)
    {
      if ((flags & CPP_N_WIDTH) == CPP_N_LARGE)
	type = dfloat128_type_node;
      else if ((flags & CPP_N_WIDTH) == CPP_N_SMALL)
	type = dfloat32_type_node;
      else
	type = dfloat64_type_node;
    }
  else if ((flags & CPP_N_WIDTH) == CPP_N_LARGE)
    type = long_double_type_node;
  else if ((flags & CPP_N_WIDTH) == CPP_N_SMALL
      || flag_single_precision_constant)
    type = float_type_node;
  else
    type = double_type_node;

  const_type = excess_precision_type (type);
  if (!const_type)
    const_type = type;

  /* Copy the constant to a nul-terminated buffer.  If the constant
     has any suffixes, cut them off; REAL_VALUE_ATOF/ REAL_VALUE_HTOF
     can't handle them.  */
  copylen = token->val.str.len;
  if (flags & CPP_N_DFLOAT)
    copylen -= 2;
  else
    {
      if ((flags & CPP_N_WIDTH) != CPP_N_MEDIUM)
	/* Must be an F or L or machine defined suffix.  */
	copylen--;
      if (flags & CPP_N_IMAGINARY)
	/* I or J suffix.  */
	copylen--;
    }

  copy = (char *) alloca (copylen + 1);
  memcpy (copy, token->val.str.text, copylen);
  copy[copylen] = '\0';

  real_from_string3 (&real, copy, TYPE_MODE (const_type));
  if (const_type != type)
    real_convert (&real_trunc, TYPE_MODE (type), &real);

  /* Create a node with determined type and value.  */
  value = build_real (const_type, real);
  if (flags & CPP_N_IMAGINARY)
    value = build_complex (NULL_TREE, convert (const_type, integer_zero_node),
			   value);

  if (type != const_type)
    value = build1 (EXCESS_PRECISION_EXPR, type, value);

  return value;
}


/* Converts a (possibly wide) character constant TOKEN from LEXER
   into a tree.  */

static tree
gl_lex_charconst (gimple_lexer *lexer, const cpp_token *token)
{
  cppchar_t result;
  tree type, value;
  unsigned int chars_seen;
  int unsignedp = 0;

  result = cpp_interpret_charconst (lexer->reader, token,
				    &chars_seen, &unsignedp);
  type = char_type_node;

  /* Cast to cppchar_signed_t to get correct sign-extension of RESULT
     before possibly widening to HOST_WIDE_INT for build_int_cst.  */
  if (unsignedp || (cppchar_signed_t) result >= 0)
    value = build_int_cst_wide (type, result, 0);
  else
    value = build_int_cst_wide (type, (cppchar_signed_t) result, -1);

  return value;
}


/* Convert a series of STRING, WSTRING, STRING16, STRING32 and/or
   UTF8STRING tokens into a tree, performing string constant
   concatenation.  TOKEN is the first of these.  Return a tree
   representing the string.  */

static tree
gl_lex_string (gimple_lexer *lexer, const cpp_token *token)
{
  tree value;
  size_t concats = 0;
  struct obstack str_ob;
  cpp_string istr;
  enum cpp_ttype type = token->type;

  /* Try to avoid the overhead of creating and destroying an obstack
     for the common case of just one string.  */
  cpp_string str = token->val.str;
  cpp_string *strs = &str;

 retry:
  token = cpp_get_token (lexer->reader);
  switch (token->type)
    {
    case CPP_PADDING:
      goto retry;

    default:
      break;

    case CPP_WSTRING:
    case CPP_STRING16:
    case CPP_STRING32:
    case CPP_UTF8STRING:
      if (type != token->type)
	{
	  if (type == CPP_STRING)
	    type = token->type;
	  else
	    error ("unsupported non-standard concatenation of string literals");
	}

    case CPP_STRING:
      if (!concats)
	{
	  gcc_obstack_init (&str_ob);
	  obstack_grow (&str_ob, &str, sizeof (cpp_string));
	}

      concats++;
      obstack_grow (&str_ob, &token->val.str, sizeof (cpp_string));
      goto retry;
    }

  /* We have read one more token than we want.  */
  _cpp_backup_tokens (lexer->reader, 1);
  if (concats)
    strs = XOBFINISH (&str_ob, cpp_string *);

  if (cpp_interpret_string (lexer->reader, strs, concats + 1, &istr, type))
    {
      value = build_string (istr.len, (const char *) istr.text);
      free (CONST_CAST (unsigned char *, istr.text));
    }
  else
    {
      /* Callers cannot generally handle error_mark_node in this context,
	 so return the empty string instead.  cpp_interpret_string has
	 issued an error.  */
      value = build_string (1, "");
    }

  if (concats)
    obstack_free (&str_ob, 0);

  return value;
}


/* Get a single token from LEXER and save it in *TOKEN.  Return false if
   we read EOF, true otherwise.  */

static bool
gl_lex_token (gimple_lexer *lexer, gimple_token *token)
{
  const cpp_token *cpp_tok;

retry:
  cpp_tok = cpp_get_token_with_location (lexer->reader, &token->location);
  token->type = cpp_tok->type;
  token->flags = cpp_tok->flags;
  token->value = NULL;

  switch (token->type)
    {
    case CPP_PADDING:
      goto retry;

    case CPP_NAME:
      token->value = HT_IDENT_TO_GCC_IDENT (HT_NODE (cpp_tok->val.node.node));
      break;

    case CPP_NUMBER:
      {
	const char *suffix = NULL;
	unsigned int flags = cpp_classify_number (lexer->reader, cpp_tok,
						  &suffix, token->location);

	switch (flags & CPP_N_CATEGORY)
	  {
	  case CPP_N_INVALID:
	    /* cpplib has issued an error.  */
	    token->value = error_mark_node;
	    break;

	  case CPP_N_INTEGER:
	    token->value = gl_interpret_integer (lexer, cpp_tok, flags);
	    break;

	  case CPP_N_FLOATING:
	    token->value = gl_interpret_float (cpp_tok, flags);
	    break;

	  default:
	    gcc_unreachable ();
	  }
      }
      break;

    case CPP_CHAR:
    case CPP_WCHAR:
    case CPP_CHAR16:
    case CPP_CHAR32:
      token->value = gl_lex_charconst (lexer, cpp_tok);
      break;

    case CPP_STRING:
    case CPP_WSTRING:
    case CPP_STRING16:
    case CPP_STRING32:
    case CPP_UTF8STRING:
      token->value = gl_lex_string (lexer, cpp_tok);
      break;

    case CPP_EOF:
      /* We've reached the end.  Stop getting tokens.  */
      return false;

    default:
      break;
    }

  /* Keep getting tokens.  */
  return true;
}


/* Get all the tokens from the file in LEXER.  */

static void
gl_lex (gimple_lexer *lexer)
{
  gimple_token token;
  gimple_token first_token, second_token;

  timevar_push (TV_CPP);

  while (gl_lex_token (lexer, &token))
    {
      if (gl_token_is_of_type (&token,CPP_LSHIFT)
	  || gl_token_is_of_type (&token,CPP_RSHIFT))
	{
	  gl_split_token (&token, &first_token, &second_token);
	  VEC_safe_push (gimple_token, gc, lexer->tokens, &first_token);
	  VEC_safe_push (gimple_token, gc, lexer->tokens, &second_token);
        }
      else 
	VEC_safe_push (gimple_token, gc, lexer->tokens, &token);
    }

  timevar_pop (TV_CPP);
}


/* Parse the translation unit in PARSER.  */

static void
gp_parse (gimple_parser *parser)
{
  while (!gl_at_eof (parser->lexer))
    {
      gimple_token *token = gl_consume_token (parser->lexer);
      if (gl_token_starts_type (token))
	gp_parse_type (parser, token);
      else if (gl_token_starts_decl (token))
	gp_parse_decl (parser, token);
      else
	gp_parse_stmt (parser, token);
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
gimple_main (void)
{
  gimple_parser *parser;

  parser_gc_root__ = parser = gp_init (main_input_filename);
  if (parser->lexer->filename == NULL)
    return;

  gimple_symtab_maybe_init_hash_table ();
  gl_lex (parser->lexer);
  gp_parse (parser);
  gp_finish (parser);
}

#include "gt-gimple-parser.h"
