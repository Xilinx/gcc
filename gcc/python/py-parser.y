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

/* Grammar largely bassed on
 * - http://docs.python.org/release/2.5.2/ref/grammar.txt
 */

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

#include "gpython.h"
#include "py-dot-codes.def"
#include "py-dot.h"
#include "py-vec.h"
#include "py-tree.h"
#include "py-runtime.h"

static VEC( gpy_sym,gc ) * gpy_symbol_stack;

extern int yylineno;

// yydebug = 1;

extern int yylex( void );
extern void yyerror( const char * );
%}

%union {
  char *string;
  long int integer;
  enum OPERATOR_T op;
  gpy_symbol_obj *symbol;
}

%error-verbose
%start declarations
%debug

%token CLASS "class"
%token DEF "def"
%token BREAK "break"
%token CONTINUE "continue"
%token RETURN "return"
%token FOR "for"
%token WHILE "while"
%token IN "in"
%token PRINT "print"

%token EXCEPT "except"
%token FINALLY "finally"
%token TRY "try"

%token AS "as"
%token ASSERT "assert"
%token DEL "del"
%token EXEC "exec"
%token FROM "from"
%token GLOBAL "global"
%token IMPORT "import"
%token IS "is"
%token LAMBDA "lambda"
%token PASS "pass"
%token RAISE "raise"
%token WITH "with"
%token YIELD "yield"

%token IF "if"
%token ELIF "elif"
%token ELSE "else"

%token OR "or"
%token AND "and"
%token NOT "not"

%token V_TRUE "True"
%token V_FALSE "False"

%token NEWLINE
%token INDENT
%token DEDENT

%token EQUAL_EQUAL
%token NOT_EQUAL
%token LESS
%token GREATER
%token LESS_EQUAL
%token GREATER_EQUAL

%token NONE
%token<string> IDENTIFIER
%token<string> STRING
%token<integer> INTEGER
%token<decimal> DOUBLE

%type<symbol> statement
%type<symbol> compound_stmt
%type<symbol> stmt_list
%type<symbol> simple_stmt
%type<symbol> expression_stmt
%type<symbol> assignment_stmt
%type<symbol> target_list
%type<symbol> target
%type<symbol> expression_list
%type<symbol> funcdef
%type<symbol> classdef
%type<symbol> suite
%type<symbol> suite_statement_list
%type<symbol> indent_stmt
%type<symbol> literal
%type<symbol> m_expr
%type<symbol> a_expr
%type<symbol> u_expr
%type<symbol> atom
%type<symbol> primary
%type<symbol> expression
%type<symbol> conditional_expression
%type<symbol> call
%type<symbol> shift_expr
%type<symbol> comparison
%type<symbol> decl
%type<symbol> argument_list
%type<symbol> argument_list_stmt
%type<symbol> parameter_list
%type<symbol> parameter_list_stmt
%type<symbol> print_stmt
%type<string> funcname
%type<string> classname

%left '-' '+'
%left '*' '/'
%right '='
%nonassoc UMINUS

%%

declarations:
            | declarations decl
            {
	      if( $2 )
		{
		  debug( "passing decl <%p> type <0x%x>!\n",
			 (void*)$2, $2->type );
		  
		  gpy_process_decl( $2 ); 
		}
	    }
            ;

decl: NEWLINE
    { $$ = NULL; }
    | statement
    ;

compound_stmt: funcdef
             | classdef
             ;

classdef: CLASS classname ':' suite
        {
	   gpy_symbol_obj *sym;
	   Gpy_Symbol_Init( sym );

	   sym->identifier = $2;
	   sym->type = STRUCTURE_OBJECT_DEF;
	   sym->op_a_t = TYPE_SYMBOL;
	  
	   sym->op_a.symbol_table= $4;
	   $$= sym;
	}
        ;

classname: IDENTIFIER
         ;

funcname: IDENTIFIER
        ;

parameter_list_stmt:
                   { $$=NULL; }
                   | parameter_list
                   { $$ = VEC_pop( gpy_sym, gpy_symbol_stack ); }
		   ;

parameter_list: parameter_list ',' target
              {
		$1->next = $3;
		$$ = $3;
	      }
              | target
              {
		VEC_safe_push( gpy_sym, gc,
			       gpy_symbol_stack, $1 );
		$$ = $1;
	      }
              ;

funcdef: DEF funcname '(' parameter_list_stmt ')' ':' suite
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );

	 sym->identifier = $2;
	 sym->type = STRUCTURE_FUNCTION_DEF;
	 sym->op_a_t = TYPE_SYMBOL;
	 sym->op_b_t = TYPE_SYMBOL;
	  
	 sym->op_a.symbol_table= $7;
	 sym->op_b.symbol_table = $4;

	 $$= sym;
       }
       ;

suite: stmt_list NEWLINE
     | NEWLINE suite_statement_list DEDENT
     {
       $$ = VEC_pop( gpy_sym, gpy_symbol_stack );
       printf("poping suite!\n");
     }
     ;

suite_statement_list: suite_statement_list indent_stmt
                   {
		     $1->next = $2;
		     $$ = $2;
		   }
                   | indent_stmt
                   {
		     VEC_safe_push( gpy_sym, gc,
				    gpy_symbol_stack, $1 );
		     $$=$1;
		   }
                   ;

indent_stmt: INDENT statement
           { $$=$2; }
           ;

statement: stmt_list NEWLINE
         | compound_stmt
         ;

stmt_list: simple_stmt 
         ;

simple_stmt: assignment_stmt
           | expression_stmt
           | print_stmt
           ;

argument_list_stmt:
                  { $$ = NULL; }
                  | argument_list
                  { $$ = VEC_pop( gpy_sym, gpy_symbol_stack ); }
		  ;

argument_list: argument_list ',' expression
             {
	       $1->next = $3;
	       $$ = $3;
	     }
             | expression
             {
	       VEC_safe_push( gpy_sym, gc,
			      gpy_symbol_stack, $1 );
	       $$ = $1;
	     }
             ;

print_stmt: PRINT argument_list_stmt
          {
	    gpy_symbol_obj* sym;
	    Gpy_Symbol_Init( sym );

	    sym->type= KEY_PRINT;
	    sym->op_a_t= TYPE_SYMBOL;
	    
	    sym->op_a.symbol_table= $2;
	    $$= sym;
	  }
	  ;


expression_stmt: expression_list
          ;

assignment_stmt: target_list '=' expression_list
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
	       }
               ;
  
target_list: target
           ;
  
target: IDENTIFIER
      {
	gpy_symbol_obj *sym;
	Gpy_Symbol_Init( sym );
	
	sym->exp = OP_EXPRESS;
	sym->type= SYMBOL_REFERENCE;
	sym->op_a_t= TYPE_STRING;
	
	sym->op_a.string= $1;
	$$= sym;
      }
      ;

expression_list: expression
               ;

expression: conditional_expression
          ;
  
conditional_expression: comparison
                      ;

u_expr: primary
       ;

m_expr: u_expr
      | m_expr '*' u_expr
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
      | m_expr '/' u_expr
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
      ;
  
a_expr: m_expr
      | a_expr '+' m_expr
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
      }
      | a_expr '-' m_expr
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
      ;

shift_expr: a_expr
          ;

comparison: shift_expr
          ;

literal: INTEGER
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );
	 
	 sym->exp = OP_EXPRESS;
	 sym->type= SYMBOL_PRIMARY;
	 sym->op_a_t= TYPE_INTEGER;
	 
	 sym->op_a.integer= $1;
	 $$= sym;
       }
       | STRING
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );
	 
	 sym->exp = OP_EXPRESS;
	 sym->type= SYMBOL_PRIMARY;
	 sym->op_a_t= TYPE_STRING;
	 
	 sym->op_a.string= $1;
	 $$= sym;
       }
       | V_TRUE
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );
	 
	 sym->exp = OP_EXPRESS;
	 sym->type= SYMBOL_PRIMARY;
	 sym->op_a_t= TYPE_BOOLEAN;
	 
	 sym->op_a.boolean= true;
	 $$= sym;
       }
       | V_FALSE
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );
	 
	 sym->exp = OP_EXPRESS;
	 sym->type= SYMBOL_PRIMARY;
	 sym->op_a_t= TYPE_BOOLEAN;
	 
	 sym->op_a.boolean= false;
	 $$= sym;
       }
       ;

atom: target
    | literal
    ;

call: IDENTIFIER '(' argument_list_stmt ')'
    {
      gpy_symbol_obj *sym= NULL;
      Gpy_Symbol_Init( sym );

      sym->exp = OP_EXPRESS;
      sym->type= OP_CALL_GOTO;
      sym->op_a_t= TYPE_STRING;
      sym->op_b_t= TYPE_SYMBOL;

      sym->op_a.string = $1;
      sym->op_b.symbol_table = $3;

      $$= sym;
    }
    ;

primary: atom
       | call
       ;

%%

void yyerror( const char *msg )
{
  error( "%s at line %i\n", msg, yylineno );
}
