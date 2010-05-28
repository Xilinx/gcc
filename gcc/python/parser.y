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

extern int yylex( void );
extern void yyerror( const char * );
%}

%union {
  char * string;
}

%language "c"
%locations
%error-verbose

%token CLASS
%token DEF
%token BREAK
%token CONTINUE
%token RETURN
%token FOR
%token WHILE
%token print

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
%token IDENTIFIER
%token INTEGER
%token STRING
%token FLOAT
%token FALSE
%token TRUE

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
            | declarations function
            ;

decl: expression DELIMITER
    | loop_while
    ;

function: DEF IDENTIFIER '(' parameters ')' ':'  pblock
        | DEF IDENTIFIER '(' ')' ':' pblock
        ;

loop_while: WHILE expression ':' pblock
          ;

expression: expr
          ;

pblock: statement_block
      ;

statement_block: statement_block decl
               | decl
               ;

expr: accessor '=' expr
    | expr '+' expr
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
    | primary
    ;

accessor: IDENTIFIER
        ;

call: IDENTIFIER '(' arguments ')'
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

primary: INTEGER
       | FLOAT
       | STRING
       | NONE
       | TRUE
       | FALSE
       | accessor
       | call
       ;

%%
