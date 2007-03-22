/* Condate language for tree/CFG checks.
  Copyright (C) 2006 Free Software Foundation, Inc.
  Contributed by Nic Volanschi <nic.volanschi@free.fr>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

/* The Condate language for expressing user-defined properties
   of a program. These properties, called "condates"
   blend control-flow, data-flow, syntactic and semantic information.
*/

%{
  #define YYSTYPE void *
  #include "config.h"
  #include "system.h"
  #include "coretypes.h"
  #include "tm.h"
  #include "tree.h"
  #include "rtl.h"
  #include "tm_p.h"
  #include "hard-reg-set.h"
  #include "basic-block.h"
  #include "output.h"
  #include "errors.h"
  #include "flags.h"
  #include "function.h"
  #include "expr.h"
  #include "diagnostic.h"
  #include "tree-flow.h"
  #include "timevar.h"
  #include "tree-dump.h"
  #include "tree-pass.h"
  #include "toplev.h"
  #include "tree-match.h"
  int yylex (void);
  void yyerror (char const *);

%}

/* Bison declarations.  */
%name-prefix="condate_"
%token CONDATE
%token FROM
%token TO
%token AVOID
%token IDENT
%right OR
%token STR
%token CCODE
%token WARNING

%% /* The grammar follows.  */
start: /* empty */
| start condate {/* print_cond($2); */
		 normalize_condate($2);
		 name_condate($2);
		 add_condate($2);};

condate: CONDATE IDENT '{' crq '}' 
         WARNING '(' STR ')' ';' {$$ = $4; 
	   ((condate)$$)->name = $2; ((condate)$$)->msg = $8;}
| crq ';' {$$ = $1;};

/* Constraint Reachability Queries */
crq: patexp {$$ = mkcond(NULL, $1, NULL, NULL, NULL, NULL);}
| FROM patexp TO patexp {$$ = mkcond(NULL, $2, $4, NULL, NULL, NULL);}
| FROM patexp TO patexp AVOID patexp {$$ = mkcond(NULL, $2, $4, $6, NULL, NULL);};

patexp: edgepat {$$ = $1;}
| patexp OR patexp {$$ = pat_or($1, $3);}
| '(' patexp ')' {$$ = $2;};

edgepat: pat {$$ = $1; ((pattern)$$)->sign = 0;}
| '+' pat {$$ = $2; ((pattern)$$)->sign = +1;}
| '-' pat {$$ = $2; ((pattern)$$)->sign = -1;};

pat: STR sempat {$$ = mkpat($1); free($1);};

sempat: /* empty */
| '|' CCODE {fprintf(stderr, "%s: warning: semantic patterns NYI: {%s}\n",
		     tree_check_file, (char *)$2);};

%%

/* The lexical analyzer returns a double floating point
   number on the stack and the token NUM, or the numeric code
   of the character read if not a number.  It skips all blanks
   and tabs, and returns 0 for end-of-input.  */

#include <ctype.h>

/* The folowing should be bigger than all of the folowing:
 - the maximal keyword length 
 - the maximal pattern length
 - the maximal length of a CCODE block. 
 Note: this ugly limit should be eliminated by writing the lexer in Flex.
*/
#define MAX_KEYWORD_LEN 1024

int
yylex (void)
{
  int c;
  static char buf[MAX_KEYWORD_LEN + 1]; 
  int len;
  static int afterbar = 0;

  c = getc (checkfile);
  /* Skip white space and comments. */
  do 
    {
      while (c == ' ' || c == '\t' || c == '\n') 
	c = getc (checkfile);
      if(c == '#') 
	{
	  while ((c = getc (checkfile)) != '\n' && c != EOF)
	    ;
	  if (c == '\n') 
	    c = getc (checkfile);
	}
    } while(c == ' ' || c == '\t' || c == '#' || c == '\n');

  /* Return end-of-input. */
  if (c == EOF)
    return 0;

  /* recognize one-character keywords */
  if (c == '+' || c == '-' || c == ';'
      || c == '}' || c == '(' || c == ')')
    return c;
  if (c == '|') 
    {
      afterbar = 1;
      return c;
    }

  /* Process strings. */
  if (c == '"')
    {
      len = 0;
      while ((c = getc (checkfile)) != '"' && c != EOF && len < MAX_KEYWORD_LEN)
	{
	  buf[len++] = c;
	}
      buf[len] = 0;
      if (c == EOF || len == MAX_KEYWORD_LEN)
	return 0;
      yylval = xstrdup(buf);
      return STR;
    }

  /* Meaning of '{' is context-dependent: */
  if (c == '{') 
    {
      if (!afterbar)
	return c;
      else
	{ /* Process C code. */     
	  len = 0;
	  while ((c = getc (checkfile)) != '}' && c != EOF && len < MAX_KEYWORD_LEN)
	    {
	      buf[len++] = c;
	    }
	  if (c == EOF || len == MAX_KEYWORD_LEN)
	    return 0;
	  buf[len] = 0;
	  afterbar = 0;
	  yylval = xstrdup(buf);
	  return CCODE;
	}
    }
  /* Recognize keywords & identifiers */
  if (isalpha(c)) 
    {
      len = 0;
      buf[len++] = c;
      while ((isalnum((c = getc (checkfile))) || c == '_') && len < MAX_KEYWORD_LEN)
	{
	  buf[len++] = c;
	}
      if (c == EOF || len == MAX_KEYWORD_LEN)
	return 0;
      buf[len] = 0;
      ungetc (c, checkfile);

      /* try keywords */
      if (!strcmp (buf, "condate"))
	return CONDATE;
      else if (!strcmp (buf, "from"))
	return FROM;
      else if (!strcmp (buf, "to"))
	return TO;
      else if (!strcmp (buf, "avoid"))
	return AVOID;
      else if (!strcmp (buf, "or"))
	return OR;
      else if (!strcmp (buf, "warning"))
	return WARNING;
      /* identifier */
      yylval = xstrdup (buf);
      return IDENT;
    }

  /* Return a single char. */
  fprintf (stderr, "Illegal character: '%c'\n", c);
  return 0;
}

/* Called by yyparse on error.  */
void
yyerror (char const *s)
{
  char buf[32];
  fprintf (stderr, "%s: %s\n", tree_check_file, s);
  fgets (buf, 32, checkfile);
  fprintf (stderr, "%s: before or near: \"%s\"\n", 
	   tree_check_file, buf);
}

struct split_pattern_s split_pattern(pattern p);

/* Structure to return a pattern splitted in: unsigned, positive, and negative
 edge patterns. */
struct split_pattern_s {pattern p1, p2, p3;};

struct split_pattern_s
split_pattern (pattern p)
{
  struct split_pattern_s sp;
  if (!p)
    sp.p1 = sp.p2 = sp.p3 = NULL;
  else 
    {
      sp = split_pattern(p->next);
      if (p->sign == 0) 
	{
	  p->next = sp.p1; 
	  sp.p1 = p;
	} 
      else if (p->sign > 0) 
	{
	  p->next = sp.p2; 
	  sp.p2 = p;
	} 
      else 
	{
	  p->next = sp.p3; 
	  sp.p3 = p;
	}
    }
  return sp;
}

/* Normalize a condate by separating the avoid patterns into: 
  - avoid (unsigned edge patterns),
  - avoid_then (positive edge patterns), and
  - avoid_else (negative edge patterns).
 Normalization conserves the meaning of a condate, but optimizes its matching. 
 Note: we assume that the initial condate contains only 'avoid' patterns.
*/
void 
normalize_condate (condate cond) 
{
  struct split_pattern_s sp = split_pattern (cond->avoid);
  cond->avoid = sp.p1;
  cond->avoid_then = sp.p2;
  cond->avoid_else = sp.p3;
}

void 
name_condate (condate cond)
{
  if(!cond->name) 
    {
      cond->name = xmalloc (strlen (tree_check_file) + 6);
      strcpy (cond->name, tree_check_file);
      sprintf (cond->name + strlen (tree_check_file), "[%d]", n_conds + 1);
    }
}

void 
add_condate (condate cond) 
{
  static int warned = 0;
  if (n_conds == CONDMAX && !warned)
    {
      fprintf (stderr, "Warning: ignoring checks beyond %d", CONDMAX);
      warned = 1;
      return;
    }
  conds[n_conds++] = cond;
}
