/* GCC MELT GENERATED FILE warmelt-infixsyntax.0.c - DO NOT EDIT */

#ifndef MELTGCC_DYNAMIC_OBJSTRUCT
/* version string of the gcc executable generating this file: */
const char genversionstr_melt[]=
 "4.5.0 20100104 (experimental)";
#endif

#include "run-melt.h"

/**** warmelt-infixsyntax.0.c declarations ****/
#define MELT_HAS_INITIAL_ENVIRONMENT 1

/**!!** ***
    Copyright 2009 Free Software Foundation, Inc.
    Contributed by Basile Starynkevitch <basile@starynkevitch.net>

    This file is part of GCC.

    GCC is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3, or (at your option)
    any later version.

    GCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GCC; see the file COPYING3.  If not see
    <http://www.gnu.org/licenses/>.
*****!!**/


void* start_module_melt(void*);
void mark_module_melt(void*);
void* initial_frame_melt;


/**** warmelt-infixsyntax.0.c implementations ****/


typedef 
 struct frame_start_module_melt_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct meltclosure_st *noinitialclos;
  struct excepth_melt_st *exh;
  struct callframe_melt_st *prev;
#define CURFRAM_NBVARPTR 9
  void* varptr[9];
/*no varnum*/
#define CURFRAM_NBVARNUM /*none*/0
/*others*/
  long _spare_; }
 initial_frame_st;


static void initialize_module_meltdata(initial_frame_st *iniframp__, char predefinited[])
{
#define curfram__  (*iniframp__)
 
 struct cdata_st {
 long spare_;
} *cdat = NULL;
 cdat = (struct cdata_st*) meltgc_allocate(sizeof(*cdat),0);
  melt_prohibit_garbcoll = TRUE;
 /*initial routine predef*/
 /*initial routine fill*/
  cdat = NULL;
 melt_prohibit_garbcoll = FALSE;

#undef curfram__
} /*end initialize_module_meltdata*/


void* start_module_melt(void* modargp_) {

 char predefinited[MELTGLOB__LASTGLOB+8];
 
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

initial_frame_st     curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 9;
 curfram__.prev = (struct callframe_melt_st *) melt_topframe;
 melt_topframe = (struct callframe_melt_st *) &curfram__;
/**initial routine prologue**/
 initial_frame_melt = (void*) &curfram__;
 /**COMMENT: get previous environment **/;
 
 /*_.PREVENV___V4*/ curfptr[3] = modargp_;
 /**COMMENT: compute boxloc **/;
 
 /*cond*/ if (/*_.CONTENV___V2*/ curfptr[1] || melt_object_length((melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA))<FSYSDAT_BOX_FRESH_ENV) /*then*/ {
 } else {
  /*com.block:compute fresh module environment*/{
   /**COMMENT: start computing boxloc **/;
   ;
   { melt_ptr_t slot=0;
    melt_object_get_field(slot,(melt_ptr_t)(((void*)(MELT_PREDEF(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.FRESHENV___V3*/ curfptr[2] = slot; };
   ;
   
    if ((/*_.PREVENV___V4*/ curfptr[3]) && melt_magic_discr((melt_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2])) != OBMAG_CLOSURE) warning(0, "bad MELT fresh_env @%p in system data <%s:%d>", /*_.FRESHENV___V3*/ curfptr[2], __FILE__, __LINE__);;;
   /*apply*/{
    /*_.CONTENV___V2*/ curfptr[1] =  melt_apply ((meltclosure_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2]), (melt_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (""), (union meltparam_un*)0, "", (union meltparam_un*)0);
    }
   ;}/*com.end block:compute fresh module environment*/
  ;
  }
 
 /**COMMENT: get symbols & keywords **/;
 
 /**initial routine cdata initializer**/

 memset(predefinited, 0, sizeof(predefinited));
 initialize_module_meltdata(&curfram__, predefinited);
 /**initial routine body**/
/**COMMENT: start of oinibody **/;
 
 /**COMMENT: before toplevel body **/;
 
 MELT_LOCATION("warmelt-infixsyntax.melt:3:/ block");
 /*block*/{
  MELT_LOCATION("warmelt-infixsyntax.melt:3:/ cond");
  /*cond*/ if (
   /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1]) /*then*/ {
  MELT_LOCATION("warmelt-infixsyntax.melt:3:/ cond.then");
  /*_.OR___V7*/ curfptr[6] = 
    /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1];;
  } else {MELT_LOCATION("warmelt-infixsyntax.melt:3:/ cond.else");
  
   MELT_LOCATION("warmelt-infixsyntax.melt:3:/ block");
   /*block*/{
    MELT_LOCATION("warmelt-infixsyntax.melt:3:/ getslot");
    { melt_ptr_t slot=0;
     melt_object_get_field(slot,(melt_ptr_t)(((void*)(MELT_PREDEF(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.SYSDATA_CONT_FRESH_ENV__V8*/ curfptr[7] = slot; };
    ;
    MELT_LOCATION("warmelt-infixsyntax.melt:3:/ apply");
    /*apply*/{
     union meltparam_un argtab[1];
     memset(&argtab, 0, sizeof(argtab));
     MELT_LOCATION("warmelt-infixsyntax.melt:3:/ apply.arg");
     argtab[0].bp_cstring =  "warmelt-infixsyntax";
     /*_.FUN___V9*/ curfptr[8] =  melt_apply ((meltclosure_ptr_t)(/*_.SYSDATA_CONT_FRESH_ENV__V8*/ curfptr[7]), (melt_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (BPARSTR_CSTRING ""), argtab, "", (union meltparam_un*)0);
     }
    ;
    /*_.OR___V7*/ curfptr[6] = /*_.FUN___V9*/ curfptr[8];;
    /*epilog*/
    MELT_LOCATION("warmelt-infixsyntax.melt:3:/ clear");
    /*clear*/ /*_.SYSDATA_CONT_FRESH_ENV__V8*/ curfptr[7] = 0 ;
    MELT_LOCATION("warmelt-infixsyntax.melt:3:/ clear");
    /*clear*/ /*_.FUN___V9*/ curfptr[8] = 0 ;}
   ;
   }
  ;
  MELT_LOCATION("warmelt-infixsyntax.melt:3:/ block");
  /*com.block:upd.cur.mod.env.cont : at very start*/{}/*com.end block:upd.cur.mod.env.cont : at very start*/
  ;
  /*epilog*/
  MELT_LOCATION("warmelt-infixsyntax.melt:3:/ clear");
  /*clear*/ /*_.OR___V7*/ curfptr[6] = 0 ;}
 
 /**COMMENT: after toplevel body **/;
 
 /**COMMENT: compute boxloc again **/;
 
 /*cond*/ if (/*_.CONTENV___V2*/ curfptr[1] || melt_object_length((melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA))<FSYSDAT_BOX_FRESH_ENV) /*then*/ {
 } else {
  /*com.block:compute fresh module environment*/{
   /**COMMENT: start computing boxloc **/;
   ;
   { melt_ptr_t slot=0;
    melt_object_get_field(slot,(melt_ptr_t)(((void*)(MELT_PREDEF(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.FRESHENV___V3*/ curfptr[2] = slot; };
   ;
   
    if ((/*_.PREVENV___V4*/ curfptr[3]) && melt_magic_discr((melt_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2])) != OBMAG_CLOSURE) warning(0, "bad MELT fresh_env @%p in system data <%s:%d>", /*_.FRESHENV___V3*/ curfptr[2], __FILE__, __LINE__);;;
   /*apply*/{
    /*_.CONTENV___V2*/ curfptr[1] =  melt_apply ((meltclosure_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2]), (melt_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (""), (union meltparam_un*)0, "", (union meltparam_un*)0);
    }
   ;}/*com.end block:compute fresh module environment*/
  ;
  }
 
 /**COMMENT: intern symbols **/;
 
 /**COMMENT: set retinit from boxloc **/;
 
 /*_.RETINIT___V1*/ curfptr[0] = 
  /*final freshenv get*/ melt_container_value((melt_ptr_t)(/*_.CONTENV___V2*/ curfptr[1]));
 /**COMMENT: end the initproc **/;
 
 ;
 goto labend_rout;
labend_rout:  melt_topframe = (struct callframe_melt_st *) curfram__.prev;
initial_frame_melt = (void*)0;
 return /*_.RETINIT___V1*/ curfptr[0];
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR

} /* end start_module_melt */

void mark_module_melt(void*fp)
{
 int ix=0;
 initial_frame_st* framptr_= (initial_frame_st*)fp;
 for(ix=0; ix<9; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_melt_un (framptr_->varptr[ix]);
   
} /* end mark_module_melt */


/* exported field offsets */

/* exported class lengths */


/**** end of warmelt-infixsyntax.0.c ****/
