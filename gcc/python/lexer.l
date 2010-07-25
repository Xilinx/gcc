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

#include "vec.h"

#include "gpy.h"
#include "symbols.h"
#include "opcodes.def"
#include "y.py.h"

#include <gmp.h>
#include <mpfr.h>

extern int yyparse( void );
%}

%option yylineno

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
in                      {  return IN; }
print                   {  return PRINT; }

if                      {  return IF; }
elif                    {  return ELIF; }
else                    {  return ELSE; }

\[                      {  return '['; }
\]                      {  return ']'; }
\(                      {  return '('; }
\)                      {  return ')'; }
\{                      {  return '{'; }
\}                      {  return '}'; }

";"                     {  return ';'; }
","                     {  return ','; }
"."                     {  return '.'; }
":"                     {  return ':'; }

"="                     { return '='; }
"+"                     { return '+'; }
"-"                     { return '-'; }
"/"                     { return '/'; }
"*"                     { return '*'; }

"=="                    { return EQUAL; }

"!="                    { return NOT_EQUAL; }
"<"                     { return LESS; }
"<="                    { return LESS_EQUAL; }
">"                     { return GREATER; }
">="                    { return GREATER_EQUAL; }

"or"                    { return OR; }
"and"                   { return AND; }

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
                              error("error initilizing integer value <%s>!\n", yytext );
                            }
                          yylval.integer = mpfr_get_si( x, GMP_RNDU );
			  mpfr_clear( x );
                          return INTEGER;
                        }

{ID}                    {
                          yylval.string= xstrdup( yytext );
                          return IDENTIFIER;
                        }

[\n]                    { return DELIMITER; }

%%

int yywrap( void )
{
  return 1;
}

int gpy_lex_parse( const char * gpy_in )
{
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
