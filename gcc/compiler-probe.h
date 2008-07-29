/* Compiler probe.
   Copyright (C) 2007 Free Software Foundation, Inc.
   Contributed by Basile Starynkevitch  <basile@starynkevitch.net>

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
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifndef GCC_COMPILER_PROBE_H
#define GCC_COMPILER_PROBE_H

#define COMPROBE_PROTOCOL_NUMBER 200701

/* the compiler probe */
#if defined(ENABLE_COMPILER_PROBE) && ENABLE_COMPILER_PROBE
/* <unistd.h> and <fcntl.h> and <string.h> have already been included
   and the following includes have been checked by autoconf */
#include <sys/select.h>
#include <signal.h>

/* compiler-probe.c uses the comprobe_ prefix */

struct comprobe_whatpos_st
{
  const char *wp_what;
  const char *wp_file;
  int wp_line;
};

/* handling routine for a request (from probe to compiler) */
typedef void comprobe_requestfun_t (struct comprobe_whatpos_st *wp,
				    char *reqlin, void *data);

/****
 * the interrupt flag, its handler, and the macro to check it 
 ****/
extern volatile sig_atomic_t comprobe_interrupted;

void comprobe_handle_probe (const char *what, const char *file, int lineno);

/* this macro should be put at all the points where we want to permit
   compiler probe interaction; in the common case where
   comprobe_interrupted is cleared, this macro runs quite quickly */
#define comprobe_check(WHAT) do{ if (comprobe_interrupted) { \
      comprobe_handle_probe((WHAT),__FILE__,__LINE__); }} while(0)

/***
 * stop the compiler probe (can be called from action handler)
 ***/
void comprobe_stop (void);

/***
 * forced kill of the compiler probe
 ***/
void comprobe_forced_kill (void);

/* force probing till an integer variable is cleared or the probe ended */
void comprobe_while_probe (const char *what, const char *file, int lineno,
			   int *pvar);
#define comprobe_while(WHAT,PVAR) do{ \
    comprobe_while_probe((WHAT),__FILE__,__LINE__,(PVAR));}while(0)
/***
 * the stream for replying to the probe ; may be NULL so should be tested!
 ***/
extern FILE *comprobe_replf;
#define comprobe_printf(Fmt, ...) do{if(comprobe_replf) {	\
      fprintf(comprobe_replf, (Fmt), ##__VA_ARGS__);		\
      fflush(comprobe_replf);}}while(0)
#define comprobe_puts(S) do{if(comprobe_replf) {	\
      fputs((S), comprobe_replf);			\
      fflush(comprobe_replf);}}while(0)
#define comprobe_flush() do{if(comprobe_replf)	\
      fflush(comprobe_replf); } while(0)
void comprobe_begin_big (void);
void comprobe_end_big (void);
void comprobe_outenc_string (const char *s);
#define comprobe_begin_big_printf(Fmt, ...) do{if(comprobe_replf) {	\
      comprobe_begin_big(); fprintf(comprobe_replf, (Fmt), \
				    ##__VA_ARGS__);}}while(0)
/***
 * initialize and finish. called in toplev.c 
 ****/
void comprobe_initialize (void);
void comprobe_finish (void);


/****
 * send a message to be shown
 ****/
void comprobe_show_message (const char *msg);



/***
 * return a unique positive file rank for a file path; or 0 for NULL name
 *  may send a PROB_file request to the probe
 ***/
int comprobe_file_rank (const char *filename);

/***
 * return a unique positive infopoint rank for a file rank and a line
 * number; may send a PROB_file request and a PROB_infopoint request
 * to the probe
 **/
int comprobe_infopoint_rank (int filrank, int lineno);

/***
 * return the linenumber, filerank, and filename of a given infopoint
 * or O|NULL if invalid infopoint rank
 ***/
int comprobe_line_of_infopoint (int infoptrank);
int comprobe_filerank_of_infopoint (int infoptrank);
const char *comprobe_filename_of_infopoint (int infoptrank);

/***
 * infopoint displayer routines
 ***/

/* info displayers are opaque structures managed by the compiler probe */
struct comprobe_infodisplay_st;

typedef void comprobe_infodisplay_fun_t
  (struct comprobe_whatpos_st *wp,
   struct comprobe_infodisplay_st *di,
   HOST_WIDE_INT data, HOST_WIDE_INT navig);

void comprobe_infopoint_add_display (int infoptrank,
				     comprobe_infodisplay_fun_t * dispfun,
				     const char *msg, HOST_WIDE_INT data);

/* displayer routines can add navigators */
void comprobe_display_add_navigator (struct comprobe_infodisplay_st *idi,
				     comprobe_infodisplay_fun_t * navfun,
				     const char *msg, HOST_WIDE_INT data);


/***
 * verb handler registration (the verb and data should be constant or
 * global data, or explicitly allocated at registration and freed
 * after unregistration).
 ***/
void comprobe_register (const char *verb, comprobe_requestfun_t * handler,
			void *data);
/* unregistration is not yet imlemented */
void comprobe_unregister (const char *verb);

typedef HOST_WIDE_INT comprobe_ix_t;

/***************************************************************************
 * additional tree specific routines are declared only if we know
 * about trees because this file has been included after tree.h
 *******/
#ifdef TREE_CODE		/*TREE_CODE is an important macro from tree.h */

/***
 * return true if a (GIMPLE/SSA) tuple G has a position 
 * and in that case fill the PFILENAME and PLINENO
 * if the END flag is set, return the last position
 ***/
enum { POS_START = 0, POS_END };
bool comprobe_get_gimple_position (gimple g, char **pfilename, int *plineno, int end);

/***
 * return a unique positive file rank for the location of a gimple, if
 * any (else 0); may send a PROB_file request to the probe and set the
 * *plineno to the line number
 **/
int comprobe_file_rank_of_gimple (gimple g, int *plineno);

/****
 * we manage a unique mapping between gimples and indexes thru our hash
 * table; to get the index of a tree and vice versa the tree of an
 * index
 ***/
comprobe_ix_t comprobe_unique_index_of_gimple (gimple g);
gimple comprobe_gimple_of_unique_index (comprobe_ix_t ix);

#endif /*TREE_CODE */




/************************************************************************
 * additional basic block specific routines are declared only if
 * we know about basic blocks because "basic-block.h" have been included
 ************************************************************************/
#ifdef BASIC_BLOCK		/* an important macro of basic-block */
/****
 * we manage a unique mapping between basic blocks and indexes thru
 * our hash table; to get the index of a basic block and vice versa
 * the basic block of an index
 ***/
comprobe_ix_t comprobe_unique_index_of_basic_block (basic_block bb);
basic_block comprobe_basic_block_of_unique_index (comprobe_ix_t ix);
#endif

#else /* compiler probe disabled */

/***************** fake stubs when probe disabled *******************/
#define comprobe_check(WHAT) do{}while(0)
#define comprobe_while(WHAT,PVAR)  do{}while(0)
#define comprobe_stop() do{}while(0)
#define comprobe_forced_kill() do{}while(0)
#define comprobe_flush() do{}while(0)
#define comprobe_replf  ((FILE*)0)
#define comprobe_show_message(M) do{if(0) puts(M);}while(0)
#define comprobe_puts(S) do[}while(0)
#define comprobe_printf(Fmt, ...) do{if(0) printf((Fmt),##__VA_ARGS__);}while(0)
#define comprobe_big_printf(Fmt, ...) do{if(0) printf((Fmt),##__VA_ARGS__);}while(0)
#define comprobe_register(Verb,Hdlr,Data) do{}while(0)
#define comprobe_unregister(Verb,Hdlr,Data) do{}while(0)
#define comprobe_begin_big()
#define comprobe_begin_big_printf(Fmt, ...) do{if(0) printf((Fmt),##__VA_ARGS__);}while(0)
#define comprobe_end_big()
#define comprobe_outenc_string(S) do{if (0 && ((S) == (char*)0));}while(0)
#define comprobe_filerank_of_tree(T,P)  (0 && (T) == (tree)0 && (P) == (int*)0)
#define comprobe_unique_index_of_tree(T) (0 && (T) == (tree)0)
#define comprobe_tree_of_unique_index(I) NULL_TREE
#define comprobe_filerank(P)  (0 && (P) == (const char*)0)
#define comprobe_get_gimple_position(G,PF,PL,E) (FALSE \
					  && (G)!=(gimple)0 \
					  && (PF)!=(char**0) \
					  && (PL) != (int*)0 && (E))
#define  comprobe_infopoint_add_displayer(IRK,DMESG,DROUT,DATA) while(0 && \
  (IRK) != (int)0 && (DMESG) != (char*)0 && \
  (DROUT) != (comprobe_infoptdisplayroutine_t*)0 && (DATA) != (void*)0) {}
#endif /*ENABLE_COMPILER_PROBE */

#endif /*GCC_COMPILER_PROBE_H */
/* Compiler probe.
   Copyright (C) 2007 Free Software Foundation, Inc.
   Contributed by Basile Starynkevitch  <basile@starynkevitch.net>

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
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifndef GCC_COMPILER_PROBE_H
#define GCC_COMPILER_PROBE_H

#define COMPROBE_PROTOCOL_NUMBER 200701

/* the compiler probe */
#if defined(ENABLE_COMPILER_PROBE) && ENABLE_COMPILER_PROBE
/* <unistd.h> and <fcntl.h> and <string.h> have already been included
   and the following includes have been checked by autoconf */
#include <sys/select.h>
#include <signal.h>

/* compiler-probe.c uses the comprobe_ prefix */

struct comprobe_whatpos_st
{
  const char *wp_what;
  const char *wp_file;
  int wp_line;
};

/* handling routine for a request (from probe to compiler) */
typedef void comprobe_requestfun_t (struct comprobe_whatpos_st *wp,
				    char *reqlin, void *data);

/****
 * the interrupt flag, its handler, and the macro to check it 
 ****/
extern volatile sig_atomic_t comprobe_interrupted;

void comprobe_handle_probe (const char *what, const char *file, int lineno);

/* this macro should be put at all the points where we want to permit
   compiler probe interaction; in the common case where
   comprobe_interrupted is cleared, this macro runs quite quickly */
#define comprobe_check(WHAT) do{ if (comprobe_interrupted) { \
      comprobe_handle_probe((WHAT),__FILE__,__LINE__); }} while(0)

/***
 * stop the compiler probe (can be called from action handler)
 ***/
void comprobe_stop (void);

/***
 * forced kill of the compiler probe
 ***/
void comprobe_forced_kill (void);

/* force probing till an integer variable is cleared or the probe ended */
void comprobe_while_probe (const char *what, const char *file, int lineno,
			   int *pvar);
#define comprobe_while(WHAT,PVAR) do{ \
    comprobe_while_probe((WHAT),__FILE__,__LINE__,(PVAR));}while(0)
/***
 * the stream for replying to the probe ; may be NULL so should be tested!
 ***/
extern FILE *comprobe_replf;
#define comprobe_printf(Fmt, ...) do{if(comprobe_replf) {	\
      fprintf(comprobe_replf, (Fmt), ##__VA_ARGS__);		\
      fflush(comprobe_replf);}}while(0)
#define comprobe_flush() do{if(comprobe_replf)	\
      fflush(comprobe_replf); } while(0)
void comprobe_begin_big (void);
void comprobe_end_big (void);
void comprobe_outenc_string (const char *s);
#define comprobe_begin_big_printf(Fmt, ...) do{if(comprobe_replf) {	\
      comprobe_begin_big(); fprintf(comprobe_replf, (Fmt), \
				    ##__VA_ARGS__);}}while(0)
/***
 * initialize and finish. called in toplev.c 
 ****/
void comprobe_initialize (void);
void comprobe_finish (void);


/****
 * send a message to be shown
 ****/
void comprobe_show_message (const char *msg);


extern struct tree_opt_pass pass_compiler_probe;

/***
 * return a unique positive file rank for a file path; or 0 for NULL name
 *  may send a PROB_file request to the probe
 ***/
int comprobe_file_rank (const char *filename);

/***
 * return a unique positive infopoint rank for a file rank and a line
 * number; may send a PROB_file request and a PROB_infopoint request
 * to the probe
 **/
int comprobe_infopoint_rank (int filrank, int lineno);

/***
 * return the linenumber, filerank, and filename of a given infopoint
 * or O|NULL if invalid infopoint rank
 ***/
int comprobe_line_of_infopoint (int infoptrank);
int comprobe_filerank_of_infopoint (int infoptrank);
const char *comprobe_filename_of_infopoint (int infoptrank);

/***
 * infopoint displayer routines
 ***/

/* info displayers are opaque structures managed by the compiler probe */
struct comprobe_infodisplay_st;

typedef void comprobe_infodisplay_fun_t
  (struct comprobe_whatpos_st *wp,
   struct comprobe_infodisplay_st *di,
   HOST_WIDE_INT data, HOST_WIDE_INT navig);

void comprobe_infopoint_add_display (int infoptrank,
				     comprobe_infodisplay_fun_t * dispfun,
				     const char *msg, HOST_WIDE_INT data);

/* displayer routines can add navigators */
void comprobe_display_add_navigator (struct comprobe_infodisplay_st *idi,
				     comprobe_infodisplay_fun_t * navfun,
				     const char *msg, HOST_WIDE_INT data);


/***
 * verb handler registration (the verb and data should be constant or
 * global data, or explicitly allocated at registration and freed
 * after unregistration).
 ***/
void comprobe_register (const char *verb, comprobe_requestfun_t * handler,
			void *data);
/* unregistration is not yet imlemented */
void comprobe_unregister (const char *verb);

typedef HOST_WIDE_INT comprobe_ix_t;

/***************************************************************************
 * additional tree specific routines are declared only if we know
 * about trees because this file has been included after tree.h
 *******/
#ifdef TREE_CODE		/*TREE_CODE is an important macro from tree.h */

/***
 * return true if a (GIMPLE/SSA) tree TR has a position 
 * and in that case fill the PFILENAME and PLINENO
 * if the END flag is set, return the last position
 ***/
enum { POS_START = 0, POS_END };
bool comprobe_get_gimple_position (gimple g, char **pfilename, int *plineno, int end);

/***
 * return a unique positive file rank for the location of a tree, if
 * any (else 0); may send a PROB_file request to the probe and set the
 * *plineno to the line number
 **/
int comprobe_file_rank_of_tree (tree tr, int *plineno);

/****
 * we manage a unique mapping between trees and indexes thru our hash
 * table; to get the index of a tree and vice versa the tree of an
 * index
 ***/
comprobe_ix_t comprobe_unique_index_of_tree (tree tr);
tree comprobe_tree_of_unique_index (comprobe_ix_t ix);

#endif /*TREE_CODE */




/************************************************************************
 * additional basic block specific routines are declared only if
 * we know about basic blocks because "basic-block.h" have been included
 ************************************************************************/
#ifdef BASIC_BLOCK		/* an important macro of basic-block */
/****
 * we manage a unique mapping between basic blocks and indexes thru
 * our hash table; to get the index of a basic block and vice versa
 * the basic block of an index
 ***/
comprobe_ix_t comprobe_unique_index_of_basic_block (basic_block bb);
basic_block comprobe_basic_block_of_unique_index (comprobe_ix_t ix);
#endif

#else /* compiler probe disabled */

/***************** fake stubs when probe disabled *******************/
#define comprobe_check(WHAT) do{}while(0)
#define comprobe_while(WHAT,PVAR)  do{}while(0)
#define comprobe_stop() do{}while(0)
#define comprobe_forced_kill() do{}while(0)
#define comprobe_flush() do{}while(0)
#define comprobe_replf  ((FILE*)0)
#define comprobe_show_message(M) do{if(0) puts(M);}while(0)
#define comprobe_printf(Fmt, ...) do{if(0) printf((Fmt),##__VA_ARGS__);}while(0)
#define comprobe_big_printf(Fmt, ...) do{if(0) printf((Fmt),##__VA_ARGS__);}while(0)
#define comprobe_register(Verb,Hdlr,Data) do{}while(0)
#define comprobe_unregister(Verb,Hdlr,Data) do{}while(0)
#define comprobe_begin_big()
#define comprobe_begin_big_printf(Fmt, ...) do{if(0) printf((Fmt),##__VA_ARGS__);}while(0)
#define comprobe_end_big()
#define comprobe_outenc_string(S) do{if (0 && ((S) == (char*)0));}while(0)
#define comprobe_filerank_of_tree(T,P)  (0 && (T) == (tree)0 && (P) == (int*)0)
#define comprobe_unique_index_of_tree(T) (0 && (T) == (tree)0)
#define comprobe_tree_of_unique_index(I) NULL_TREE
#define comprobe_filerank(P)  (0 && (P) == (const char*)0)
#define comprobe_get_position(T,PF,PL,E) (FALSE \
					  && (T)!=(tree)0 \
					  && (PF)!=(char**0) \
					  && (PL) != (int*)0 && (E))
#define  comprobe_infopoint_add_displayer(IRK,DMESG,DROUT,DATA) while(0 && \
  (IRK) != (int)0 && (DMESG) != (char*)0 && \
  (DROUT) != (comprobe_infoptdisplayroutine_t*)0 && (DATA) != (void*)0) {}
#endif /*ENABLE_COMPILER_PROBE */

#endif /*GCC_COMPILER_PROBE_H */
