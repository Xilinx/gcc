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

%token NEWLINE
%token INDENT
%token DEDENT

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

%start declarations

%left '-' '+'
%left '*' '/'
%left LESS LESS_EQUAL
%left GREATER GREATER_EQUAL
%left NOT_EQUAL EQUAL
%left AND OR
%right '^' '='
%nonassoc UMINUS

%%

declarations: declarations statement
            {
	      if( $2 )
		gpy_process_decl( $2 );
	    }
            | NEWLINE
            ;

compound_stmt: funcdef
             ;

funcdef: DEF IDENTIFIER "(" ")" ":" suite
       { $$=NULL; }
       ;

suite: stmt_list NEWLINE
     { $$=NULL; }
     | NEWLINE suite_statement_list DEDENT
     { $$=NULL; }
     ;

suite_statement_list: suite_statement_list indent_stmt
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
           ;

expression_stmt: expression_list
          ;

assignment_stmt: target_list "=" expression_list
               ;
  
target_list: target_list "," target
           | target
           ;
  
target: IDENTIFIER
      { $$=NULL; }
      ;

expression_list: expression_list "," expression
               | expression
               ;

expression: conditional_expression
          ;
  
conditional_expression: or_test ["if" or_test "else" expression]
  
or_test: and_test
       | or_test OR and_test
       ;
  
and_test: not_test
        | and_test AND not_test
        ;
  
not_test: comparison
        | NOT not_test
        ;

u_expr: power
       | "-" u_expr
       | "+" u_expr
       ;

m_expr: u_expr
      | m_expr "*" u_expr
      | m_expr "/" u_expr
      ;
  
a_expr: m_expr
      | a_expr "+" m_expr
      | a_expr "-" m_expr
      ;

shift_expr: a_expr
          ;

and_expr: shift_expr
        | and_expr "\;SPMamp;" shift_expr
        ;
  
xor_expr: and_expr
        | xor_expr "\textasciicircum" and_expr
        ;
  
or_expr: xor_expr
       | or_expr "|" xor_expr
       ;

comparison_comp: comp_operator or_expr
               ;

comparison_list: comparison_list comparison_comp
               | comparison_comp
               ;

comparison: or_expr
          | or_expr comparison_list
          ;

comp_operator: "<"
             | ">"
             | "=="
             | ">="
             | "<="
             ;
%%

void yyerror( const char *msg )
{
  error( "syntax error :: line %i:'%s'\n",
	 yylineno, msg );
}
