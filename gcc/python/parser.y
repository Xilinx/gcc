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

/* Grammar largely bassed on http://docs.python.org/release/2.5.2/ref/grammar.txt */

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

enum OPERATOR_T {
  LESS_OP, GREATER_OP, EQ_EQ_OP,
  GREATER_EQ_OP, LESS_EQ_OP
};

extern int yylineno;

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

%token OR "or"
%token AND "and"
%token NOT "not"

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
%type<symbol> or_test
%type<symbol> and_test
%type<symbol> not_test
%type<symbol> or_expr
%type<symbol> xor_expr
%type<symbol> and_expr
%type<symbol> shift_expr
%type<symbol> comparison
%type<symbol> comparison_list
%type<symbol> comparison_comp
%type<op> comp_operator

%start declarations

%%

declarations: declarations statement
            {
	      debug( "passing decl <%p> type <0x%x>!\n",
		     (void*)$2, $2->type );

	      gpy_process_decl( $2 );
	    }
            | NEWLINE
            ;

compound_stmt: funcdef
             ;

funcdef: DEF IDENTIFIER "(" ")" ":" suite
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );

	 sym->identifier = $2;
	 sym->type = STRUCTURE_FUNCTION_DEF;
	 sym->op_a_t = TYPE_SYMBOL;
	 sym->op_b_t = TYPE_SYMBOL_NIL;
	  
	 sym->op_a.symbol_table= $6;
	 $$= sym;
       }
       ;

suite: suite_statement_list DEDENT
     { $$=NULL; }
     ;

suite_statement_list: suite_statement_list indent_stmt
                   { $$ = NULL; }
                   | indent_stmt
                   { $$=NULL; }
                   ;

indent_stmt: INDENT statement
           { $$=NULL; }
           ;
  
statement: stmt_list NEWLINE
         | compound_stmt
         ;

stmt_list: stmt_list ";" simple_stmt
         | simple_stmt 
         ;

simple_stmt: assignment_stmt
           | expression_stmt
           ;

expression_stmt: expression_list
          ;

assignment_stmt: target_list "=" expression_list
               ;
  
target_list: target_list "," target
           | target
           ;
  
target: IDENTIFIER
      {
	gpy_symbol_obj *sym;
	Gpy_Symbol_Init( sym );
	
	sym->type= SYMBOL_PRIMARY;
	sym->op_a_t= TYPE_IDENTIFIER;
	
	sym->op_a.string= $1;
	$$= sym;
      }
      ;

expression_list: expression_list "," expression
               {
		 $1->next = $3;
		 $$ = $3;
               }
               | expression
               { $$ = $1; }
               ;

expression: conditional_expression
          ;
  
conditional_expression: or_test
                      ;
  
or_test: and_test
       | or_test OR and_test
       ;
  
and_test: not_test
        | and_test AND not_test
        ;
  
not_test: comparison
        | NOT not_test
        { $$ = NULL; }
        ;

u_expr: primary
       ;

m_expr: u_expr
      | m_expr "*" u_expr
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
      | m_expr "/" u_expr
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
      | a_expr "+" m_expr
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
      | a_expr "-" m_expr
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

and_expr: shift_expr
        | and_expr AND shift_expr
        ;
  
xor_expr: and_expr
        | xor_expr "^" and_expr
        ;
  
or_expr: xor_expr
       | or_expr "|" xor_expr
       ;

comparison_comp: comp_operator or_expr
               { $$ = $2; }
               ;

comparison_list: comparison_list comparison_comp
               | comparison_comp
               ;

comparison: or_expr comparison_list
          | or_expr
          ;

comp_operator: LESS
             { $$ = LESS_OP; }
             | GREATER
	     { $$ = GREATER_OP; }
             | EQUAL_EQUAL
	     { $$ = EQ_EQ_OP; }
             | GREATER_EQUAL
	     { $$ = GREATER_EQ_OP; }
             | LESS_EQUAL
	     { $$ = LESS_EQ_OP; }
             ;

literal: INTEGER
       {
	 gpy_symbol_obj *sym;
	 Gpy_Symbol_Init( sym );
	 
	 sym->type= SYMBOL_PRIMARY;
	 sym->op_a_t= TYPE_INTEGER;
	 
	 sym->op_a.integer= $1;
	 $$= sym;
       }
       ;

atom: IDENTIFIER
    {
      gpy_symbol_obj *sym;
      Gpy_Symbol_Init( sym );
	 
      sym->type= SYMBOL_PRIMARY;
      sym->op_a_t= TYPE_IDENTIFIER;
	 
      sym->op_a.string= $1;
      $$= sym;
    }
    | literal
    ;

positional_arguments: positional_arguments "," expression
                    | expression
                    ;

argument_list: positional_arguments
             ;

call: IDENTIFIER "(" argument_list ")"
    { $$ = NULL; }
    | IDENTIFIER "(" ")"
    { $$ = NULL; }
    ;

primary: atom
       | call
       ;

%%

void yyerror( const char *msg )
{
  error( "syntax error :: line %i:'%s'\n",
	 yylineno, msg );
}
