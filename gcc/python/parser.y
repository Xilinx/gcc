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
#include "line-map.h"

#include <gmp.h>
#include <mpfr.h>

static VEC( gpy_sym,gc ) * gpy_symbol_stack;

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

%token CLASS "class"
%token DEF "def"
%token BREAK "break"
%token CONTINUE "continue"
%token RETURN "return"
%token FOR "for"
%token WHILE "while"
%token IN "in"
%token PRINT "print"

%token IF "if"
%token ELIF "elif"
%token ELSE "else"

%token EQUAL "=="
%token NOT_EQUAL "!="
%token LESS "<"
%token LESS_EQUAL "<="
%token GREATER ">"
%token GREATER_EQUAL ">="
%token OR "or"
%token AND "and"

%token DELIMITER
%token NONE
%token<string> IDENTIFIER
%token<string> STRING
%token<integer> INTEGER

%type<symbol> expr
%type<symbol> expression
%type<symbol> symbol_accessor
%type<symbol> function
%type<symbol> loop_while
%type<symbol> accessor
%type<symbol> decl
%type<symbol> primary
%type<symbol> procedure
%type<symbol> statement_block
%type<symbol> parameter
%type<symbol> parameters
%type<symbol> parameter_list
%type<symbol> key_return
%type<symbol> arguments
%type<symbol> argument_list
%type<symbol> arbitrary_call
%type<symbol> list_symbol

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
            | error
            {
	      fatal_error("malformed declaration!\n");
	    }
            ;

decl: expression ';'
    | loop_while
    | function
    | key_return ';'
    ;

key_return: RETURN expression
          {
	    gpy_symbol_obj *sym;
	    Gpy_Symbol_Init( sym );

	    sym->type = KEY_RETURN;
	    sym->op_a_t = TYPE_SYMBOL;

	    sym->op_a.symbol_table= $2;
	    $$= sym;
	  }
          ;

function: DEF IDENTIFIER '(' parameters ')' ':' '{' procedure '}'
        {
	  gpy_symbol_obj *sym;
	  Gpy_Symbol_Init( sym );

	  sym->identifier= $2;
	  sym->type= STRUCTURE_FUNCTION_DEF;
	  sym->op_a_t= TYPE_SYMBOL;
	  sym->op_b_t= TYPE_PARAMETERS;

	  sym->op_a.symbol_table = $8;
	  sym->op_b.symbol_table = $4;
	  $$= sym;
	}
        | DEF IDENTIFIER '(' ')' ':' '{' procedure '}'
	{
	  gpy_symbol_obj *sym;
	  Gpy_Symbol_Init( sym );

	  sym->identifier = $2;
	  sym->type = STRUCTURE_FUNCTION_DEF;
	  sym->op_a_t = TYPE_SYMBOL;
	  sym->op_b_t = TYPE_SYMBOL_NIL;
	  
	  sym->op_a.symbol_table= $7;
	  $$= sym;
	}
        ;

loop_while: WHILE expression ':' '{' procedure '}'
          {
	    $$ = NULL;
	  }
          ;

expression: expr
          ;

procedure: statement_block
         {
	   $$ = VEC_pop( gpy_sym, gpy_symbol_stack );
	 }
         ;

statement_block: statement_block decl
               {
		 $1->next = $2;
		 $$ = $2;
	       }
               | decl
	       {
		 VEC_safe_push( gpy_sym, gc,
				gpy_symbol_stack, $1 );
		 $$ = $1;
	       }
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
    {
      gpy_symbol_obj* sym;
      Gpy_Symbol_Init( sym );

      sym->exp= OP_EXPRESS;
      sym->type= OP_BIN_SUBTRACTION;
      sym->op_a_t= TYPE_SYMBOL;
      sym->op_b_t= TYPE_SYMBOL;

      sym->op_a.symbol_table= $1;
      sym->op_b.symbol_table= $3;
      $$= sym;
    }
    | expr '*' expr
    {
      gpy_symbol_obj* sym;
      Gpy_Symbol_Init( sym );

      sym->exp= OP_EXPRESS;
      sym->type= OP_BIN_MULTIPLY;
      sym->op_a_t= TYPE_SYMBOL;
      sym->op_b_t= TYPE_SYMBOL;

      sym->op_a.symbol_table= $1;
      sym->op_b.symbol_table= $3;
      $$= sym;
    }
    | expr '/' expr
    {
      gpy_symbol_obj* sym;
      Gpy_Symbol_Init( sym );

      sym->exp= OP_EXPRESS;
      sym->type= OP_BIN_DIVIDE;
      sym->op_a_t= TYPE_SYMBOL;
      sym->op_b_t= TYPE_SYMBOL;

      sym->op_a.symbol_table= $1;
      sym->op_b.symbol_table= $3;
      $$= sym;
    }
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
		 gpy_symbol_obj * sym;
		 Gpy_Symbol_Init( sym );

		 sym->type= SYMBOL_REFERENCE;
		 sym->op_a_t= TYPE_STRING;

		 sym->op_a.string= $1;
		 $$= sym;
	       }
               ;

accessor: symbol_accessor
        | arbitrary_call
        ;

arbitrary_call: IDENTIFIER '(' arguments ')'
              {
		gpy_symbol_obj *sym= NULL;
		Gpy_Symbol_Init( sym );

		sym->exp = OP_EXPRESS;
		sym->type= OP_CALL_GOTO;

		sym->op_a_t= TYPE_STRING;
		sym->op_b_t= TYPE_ARGUMENTS;

		sym->op_a.string= $1;
		sym->op_b.symbol_table= $3;
		$$= sym;
	      }
              | IDENTIFIER '(' ')'
	      {
		gpy_symbol_obj *sym = NULL;
		Gpy_Symbol_Init( sym );
	  
		sym->exp = OP_EXPRESS;
		sym->type = OP_CALL_GOTO;

		sym->op_a_t = TYPE_STRING;
		sym->op_a.string = $1;
		$$= sym;
	      }
              ;

parameter: IDENTIFIER
         {
	    gpy_symbol_obj *sym;
	    Gpy_Symbol_Init( sym );

	    sym->type= TYPE_PARAMETER;
	    sym->op_a_t= TYPE_STRING;

	    sym->op_a.string= $1;
	    $$= sym;
	 }
         ;

parameters: parameter_list
          {
	    $$ = VEC_pop( gpy_sym, gpy_symbol_stack );
	  }
          ;

parameter_list: parameter_list ',' parameter
              {
		$1->next = $3;
		$$ = $3;
	      }
              | parameter
              {
		VEC_safe_push( gpy_sym, gc,
			       gpy_symbol_stack, $1 );
		$$ = $1;
	      }
              ;

arguments: argument_list
         {
	   $$ = VEC_pop( gpy_sym, gpy_symbol_stack );
	 }
         ;

argument_list: argument_list ',' expression
             {
	       $1->next = $3;
	     }
             | expression
             {
	       VEC_safe_push( gpy_sym, gc,
			      gpy_symbol_stack, $1 );
	       $$ = $1;
	     }
             ;

list_symbol: '[' argument_list ']'
           {
	     $$ = NULL;
	   }
           ;

primary: accessor
       | list_symbol
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
