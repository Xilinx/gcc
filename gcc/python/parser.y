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

#include <gmp.h>
#include <mpfr.h>

extern int yylineno;

extern int yylex( void );
extern void yyerror( const char * );
%}

%union {
  char *string;
  long int integer;
  gpy_symbol_obj *symbol;
}

%error-verbose

%token CLASS
%token DEF
%token BREAK
%token CONTINUE
%token RETURN
%token FOR
%token WHILE
%token PRINT

%token IF
%token ELIF
%token ELSE

%token EQUAL
%token NOT_EQUAL
%token LESS
%token LESS_EQUAL
%token GREATER
%token GREATER_EQUAL
%token OR
%token AND

%token DELIMITER
%token NONE
%token<string> IDENTIFIER
%token<integer> INTEGER
%token STRING

%type<symbol> expr
%type<symbol> expression
%type<symbol> symbol_accessor
%type<symbol> function
%type<symbol> loop_while
%type<symbol> accessor
%type<symbol> decl
%type<symbol> primary

%left '-' '+'
%left '*' '/'
%left LESS LESS_EQUAL
%left GREATER GREATER_EQUAL
%left NOT_EQUAL EQUAL
%left AND OR
%right '^' '='
%nonassoc UMINUS

%%

declarations:
            | declarations decl
            {
	      gpy_process_decl( $2 );
	    }
            ;

decl: expression ';'
    | loop_while
    | function
    ;

function: DEF IDENTIFIER '(' parameters ')' ':' '{' pblock '}'
        { $$ = NULL; }
        | DEF IDENTIFIER '(' ')' ':' '{' pblock '}'
	{ $$ = NULL; }
        ;

loop_while: WHILE expression ':' '{' pblock '}'
          { $$ = NULL; }
          ;

expression: expr
          ;

pblock: statement_block
      ;

statement_block: statement_block decl
               | decl
               ;

expr: symbol_accessor '=' expr
    {
      gpy_symbol_obj* sym;
      Gpy_Symbol_Init( sym );

      sym->exp= OP_EXPRESS;
      sym->type= OP_ASSIGN_EVAL;
      sym->op_a_t= TYPE_SYMBOL;
      sym->op_b_t= TYPE_SYMBOL;

      sym->op_a.symbol_table= $1;
      sym->op_b.symbol_table= $3;
      $$= sym;

      debug("accessor = expr!\n");
    }
    | expr '+' expr
    {
      gpy_symbol_obj* sym;
      Gpy_Symbol_Init( sym );

      sym->exp= OP_EXPRESS;
      sym->type= OP_BIN_ADDITION;
      sym->op_a_t= TYPE_SYMBOL;
      sym->op_b_t= TYPE_SYMBOL;

      sym->op_a.symbol_table= $1;
      sym->op_b.symbol_table= $3;
      $$= sym;
      debug("expr + expr!\n");
    }
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr EQUAL expr
    | expr NOT_EQUAL expr
    | expr LESS expr
    | expr LESS_EQUAL expr
    | expr GREATER expr
    | expr GREATER_EQUAL expr
    | expr AND expr
    | expr OR expr
    | '(' expr ')'
    {
      $$ = $2;
    }
    | primary
    ;

symbol_accessor: IDENTIFIER
               {
		 gpy_symbol_obj *sym;
		 Gpy_Symbol_Init( sym );

		 sym->type= SYMBOL_REFERENCE;
		 sym->op_a_t= TYPE_STRING;

		 sym->op_a.string= $1;
		 $$= sym;
	       }
               ;

accessor: symbol_accessor
        | arbitrary_call
        {
	  $$ = NULL;
	}
        ;

arbitrary_call: IDENTIFIER '(' arguments ')'
    | IDENTIFIER '(' ')'
    ;

parameters: parameter_list
          ;

parameter_list: parameter_list ',' IDENTIFIER
              | IDENTIFIER
              ;

arguments: argument_list
         ;

argument_list: argument_list ',' expression
             | expression
             ;

primary: accessor
       | INTEGER
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );

	 sym->type= SYMBOL_PRIMARY;
	 sym->op_a_t= TYPE_INTEGER;
	 
	 sym->op_a.integer= $1;
	 $$= sym;
       }
       | NONE
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );
	 $$= sym;
       }
       ;

%%

void yyerror( const char *msg )
{
  error( "syntax error :: line %i:'%s'\n",
	 yylineno, msg );
}
