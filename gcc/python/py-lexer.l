%{
/* This file is part of GCC.

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

/**
 * The lexer the lexical tokens which are important get parsed here!
 *
 * IMPORTANT: We now requite Flex to generate the lexer now!
 **/

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "coretypes.h"
#include "opts.h"
#include "tree.h"
#include "gimple.h"
#include "toplev.h"
#include "debug.h"
#include "options.h"
#include "flags.h"
#include "convert.h"
#include "diagnostic-core.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "target.h"

#include <gmp.h>
#include <mpfr.h>

#include "vec.h"
#include "hashtab.h"

#include "gpy.h"
#include "opcodes.def"
#include "symbols.h"
#include "pypy-tree.h"
#include "runtime.h"

#include "y.py.h"

/* Stack required for INDENT and DEDENT tokens @see
 * - http://docs.python.org/reference/lexical_analysis.html
 */

static VEC(gpy_int,gc) * gpy_indent_stack;

extern int yyparse( void );

static bool dedent = false;
static bool eof = false;
 
#define YY_USER_ACTION						\
  printf("dedent = <%i>!\n", dedent);				\
  if( dedent )							\
    {								\
      printf("user_action - yytext = <%s>!\n", yytext);		\
      int i;							\
      /* Copy yytext because unput() trashes yytext */		\
      char *yycopy = xstrdup( yytext );				\
      for ( i = yyleng - 1; i >= 0; --i )			\
	unput( yycopy[i] );					\
      free( yycopy );						\
								\
      dedent = false;						\
      return DEDENT;						\
    }
  
 
%}

%option yylineno
%option debug

DIGIT    [0-9]
ID       [_a-zA-Z][a-zA_Z0-9_$]*
qstring  \"[^\"\n]*[\"\n]

%%

#.*                     ; /* comment */

class                   {  return CLASS; }
def                     {  return DEF; }

break                   {  return BREAK; }
continue                {  return CONTINUE; }
return                  {  return RETURN; }
for                     {  return FOR; }
while                   {  return WHILE; }
print                   {  return PRINT; }

if                      {  return IF; }
elif                    {  return ELIF; }
else                    {  return ELSE; }

\[                      {  return '['; }
\]                      {  return ']'; }
\(                      {  return '('; }
\)                      {  return ')'; }

";"                     {  return ';'; }
","                     {  return ','; }
"."                     {  return '.'; }
":"                     {  return ':'; }

"="                     { return '='; }
"+"                     { return '+'; }
"-"                     { return '-'; }
"/"                     { return '/'; }
"*"                     { return '*'; }
"|"                     { return '|'; }

"=="                    { return EQUAL_EQUAL; }

"!="                    { return NOT_EQUAL; }
"<"                     { return LESS; }
"<="                    { return LESS_EQUAL; }
">"                     { return GREATER; }
">="                    { return GREATER_EQUAL; }

"or"                    { return OR; }
"and"                   { return AND; }
"not"                   { return NOT; }

"True"                  { return V_TRUE; }
"False"                 { return V_FALSE; }

{qstring}               {
                          yylval.string= xstrdup( (yytext+1) );
                          if( yylval.string[ yyleng-2 ] != '\"' ) {
                            error("Un-termintated character string!\n");
                          }
                          else {
                            yylval.string[yyleng-2] = '\0';
                          }
                          return STRING;
                        }

{DIGIT}+                {
                          mpfr_t x;
			  mpfr_init2( x, 32 );
			  if( mpfr_set_str( x, yytext, 10, GMP_RNDU) )
                            {
                              fatal_error("error initilizing integer value <%s>!\n", yytext );
                            }
                          yylval.integer = mpfr_get_si( x, GMP_RNDU );
			  mpfr_clear( x );
                          return INTEGER;
                        }

{ID}                    {
                          printf("IDENTIFIER!\n");
                          yylval.string= xstrdup( yytext );
                          return IDENTIFIER;
                        }

"\n"                    {
                          printf("newline rule!\n");
                          int chk = yylex( );
			  printf("chk = <%i>!\n", chk );
			  if( (chk != INDENT) && (eof == false ) )
			    {
			      int top_len = VEC_index( gpy_int, gpy_indent_stack,
						       VEC_length( gpy_int, gpy_indent_stack )-1 );
			      printf("whoop not indent top_len = <%i>!\n", top_len );
			      if( top_len != 0 )
				{
				  printf("pop and dedent!\n");
				  VEC_pop( gpy_int, gpy_indent_stack );
				  dedent = true;
				}
			    }
			  else if( chk == INDENT )
			    {
			      int top_len = VEC_index( gpy_int, gpy_indent_stack,
						       VEC_length( gpy_int, gpy_indent_stack )-1 );
			      int indent = yyleng/4;

			      if( top_len <= indent )
				{
				  if( top_len != indent )
				    {
				      VEC_pop( gpy_int, gpy_indent_stack );
				    }
				}
			    }
			  else if( eof == true )
			    {
			      eof = false;
			      if( chk == DEDENT )
				dedent = true;
			    }

			  printf("yytext = <%s>!\n", yytext );

			  int i;
			  /* Copy yytext because unput() trashes yytext */
			  char *yycopy = xstrdup( yytext );
			  for ( i = yyleng - 1; i >= 0; --i )
			    unput( yycopy[i] );
			  free( yycopy );

			  printf("newline!\n");
                          return NEWLINE;
                        }

"    "+                 {
                          int top_len = VEC_index( gpy_int, gpy_indent_stack,
						   VEC_length( gpy_int, gpy_indent_stack )-1 );
			  int indent = yyleng/4;

			  if( top_len <= indent )
			    {
			      if( top_len != indent )
				{
                                  printf("pushing indent <%i>!\n", indent);
				  VEC_safe_push( gpy_int, gc, gpy_indent_stack, indent );
				}
                              printf("INDENT!\n");
			      return INDENT;
			    }
			  else if( top_len > indent )
			    {
                              printf("DEDENT!\n");
			      VEC_pop( gpy_int, gpy_indent_stack );
			      return DEDENT;
			    }
                         }

<<EOF>>                {
                         printf("EOF rule!\n"); eof = true;
                         int top_len = VEC_index( gpy_int, gpy_indent_stack,
						  VEC_length( gpy_int, gpy_indent_stack )-1 );
			 if( top_len != 0 )
			   {
                             printf("DEDENT EOF!\n");
			     VEC_pop( gpy_int, gpy_indent_stack );
			     return DEDENT;
			   }
			 else
			   return 0;
                       }

%%

int yywrap( void )
{
  return 1;
}

int gpy_lex_parse( const char * gpy_in )
{
  VEC_safe_push( gpy_int, gc, gpy_indent_stack, 0 );

  int retval = 1;
  FILE * fd = fopen( gpy_in, "rb" );
  debug("trying to open <%s>!\n", gpy_in);
  if( fd )
    {
      yyin = fd;
      retval = yyparse( );
      fclose( fd );
      yylex_destroy( );
    }
  else
    {
      fprintf(stderr, "error opening <%s>!\n", gpy_in );
      retval = 0;
    }
  return retval;
}
