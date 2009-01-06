/* GCC BASILYS GENERATED FILE warmelt-normatch-3.c - DO NOT EDIT */
/* generated on Tue Jan  6 10:16:53 2009 */

#include "run-basilys.h"

/**** warmelt-normatch-3.c declarations ****/

/**!!** 
Copyright 2008, 2009 Free Software Foundation, Inc.
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
**!!**/




static basilys_ptr_t rout_1_SCANPAT_NIL(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_2_SCANPAT_ANYRECV(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_3_SCANPAT_ANYPATTERN(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_4_SCANPAT_SRCPATOR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_5_SCANPAT_SRCPATAND(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_6_SCANPAT_SRCPATVAR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_7_SCANPAT_SRCPATJOKER(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_8_SCANPAT_SRCPATCONST(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_9_SCANPAT_SRCPATOBJECT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_10_SCANPAT_SRCPATCOMPOSITE(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_11_NORMPAT_ANYRECV(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_12_NORMPAT_ANYPAT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_13_NORMPAT_INSTANCEPAT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_14_NORMPAT_ANYMATCHPAT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_15_NORMEXP_MATCH(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);


void* start_module_basilys(void*);
void mark_module_basilys(void*);
void* initial_frame_basilys;


/**** warmelt-normatch-3.c implementations ****/




static basilys_ptr_t
rout_1_SCANPAT_NIL(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_1_SCANPAT_NIL_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 11
  void* varptr[11];
#define CURFRAM_NBVARNUM 3
  long varnum[3];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<11; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 11;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_NIL", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:59:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:60:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:60:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:60:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:60:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_nil recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:60:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:60:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:60:/ apply.arg");
    argtab[3].bp_long = 60;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:60:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:60:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:60:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:61:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:61:/ block");
  /*block*/{
   /*_#NULL__L2*/ curfnum[0] = 
    ((/*_.RECV__V2*/ curfptr[1]) == NULL);;
   BASILYS_LOCATION("warmelt-normatch.bysl:61:/ cond");
   /*cond*/ if (/*_#NULL__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:61:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:61:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:61:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check nil recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(61)?(61):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:61:/ clear");
   /*clear*/ /*_#NULL__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:61:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:61:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:62:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:62:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(62)?(62):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:62:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:62:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.IFCPP___V10*/ curfptr[8];;
  BASILYS_LOCATION("warmelt-normatch.bysl:59:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_NIL", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_1_SCANPAT_NIL*/





static basilys_ptr_t
rout_2_SCANPAT_ANYRECV(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_2_SCANPAT_ANYRECV_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 14
  void* varptr[14];
#define CURFRAM_NBVARNUM 2
  long varnum[2];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<14; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 14;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_ANYRECV", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:69:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:70:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:70:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_anypattern recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
    argtab[3].bp_long = 70;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:70:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:70:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:70:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:71:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:71:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:71:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:71:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:71:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:71:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(71)?(71):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:71:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:71:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:71:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ block");
 /*block*/{
  /*_.MYCLASS__V11*/ curfptr[10] = 
   (basilys_discr((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1])));;
  BASILYS_LOCATION("warmelt-normatch.bysl:73:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MYCLASS__V11*/ curfptr[10]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[2])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:73:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:73:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MYCLASS__V11*/ curfptr[10]), 1, "NAMED_NAME");/*_.MYCLASSNAME__V12*/ curfptr[11] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:73:/ cond.else");
  
   /*_.MYCLASSNAME__V12*/ curfptr[11] = NULL;;
   }
  ;
  
  basilys_error_str((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2]), ( "unimplemented scan_pattern for any "), (basilys_ptr_t)(/*_.MYCLASSNAME__V12*/ curfptr[11]));
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:76:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:76:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:76:/ cond");
    /*cond*/ if ((/*nil*/NULL)) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:76:/ cond.then");
    /*_.IFELSE___V14*/ curfptr[13] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:76:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:76:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "catchall scan_pattern unimplemented"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(76)?(76):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V14*/ curfptr[13] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V13*/ curfptr[12] = /*_.IFELSE___V14*/ curfptr[13];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:76:/ clear");
    /*clear*/ /*_.IFELSE___V14*/ curfptr[13] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:76:/ cppif.else");
  /*_.IFCPP___V13*/ curfptr[12] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V10*/ curfptr[8] = /*_.IFCPP___V13*/ curfptr[12];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
  /*clear*/ /*_.MYCLASS__V11*/ curfptr[10] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
  /*clear*/ /*_.MYCLASSNAME__V12*/ curfptr[11] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
  /*clear*/ /*_.IFCPP___V13*/ curfptr[12] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V10*/ curfptr[8];;
  BASILYS_LOCATION("warmelt-normatch.bysl:69:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ clear");
 /*clear*/ /*_.LET___V10*/ curfptr[8] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_ANYRECV", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_2_SCANPAT_ANYRECV*/





static basilys_ptr_t
rout_3_SCANPAT_ANYPATTERN(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_3_SCANPAT_ANYPATTERN_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 16
  void* varptr[16];
#define CURFRAM_NBVARNUM 3
  long varnum[3];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<16; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 16;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_ANYPATTERN", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:83:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:84:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:84:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_anypattern recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
    argtab[3].bp_long = 84;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:84:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:84:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:84:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:85:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:85:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_ANY*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:85:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:85:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:85:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:85:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(85)?(85):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:85:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:85:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:85:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:86:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:86:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:86:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:86:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(86)?(86):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:86:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:87:/ block");
 /*block*/{
  /*_.MYCLASS__V13*/ curfptr[12] = 
   (basilys_discr((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1])));;
  BASILYS_LOCATION("warmelt-normatch.bysl:88:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MYCLASS__V13*/ curfptr[12]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[3])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:88:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:88:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MYCLASS__V13*/ curfptr[12]), 1, "NAMED_NAME");/*_.MYCLASSNAME__V14*/ curfptr[13] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:88:/ cond.else");
  
   /*_.MYCLASSNAME__V14*/ curfptr[13] = NULL;;
   }
  ;
  
  basilys_error_str((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2]), ( "unimplemented scan_pattern for pattern "), (basilys_ptr_t)(/*_.MYCLASSNAME__V14*/ curfptr[13]));
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:91:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:91:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:91:/ cond");
    /*cond*/ if ((/*nil*/NULL)) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:91:/ cond.then");
    /*_.IFELSE___V16*/ curfptr[15] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:91:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:91:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "catchall scan_pattern unimplemented"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(91)?(91):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V16*/ curfptr[15] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V15*/ curfptr[14] = /*_.IFELSE___V16*/ curfptr[15];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:91:/ clear");
    /*clear*/ /*_.IFELSE___V16*/ curfptr[15] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:91:/ cppif.else");
  /*_.IFCPP___V15*/ curfptr[14] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFCPP___V15*/ curfptr[14];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:87:/ clear");
  /*clear*/ /*_.MYCLASS__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:87:/ clear");
  /*clear*/ /*_.MYCLASSNAME__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:87:/ clear");
  /*clear*/ /*_.IFCPP___V15*/ curfptr[14] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:83:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_ANYPATTERN", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_3_SCANPAT_ANYPATTERN*/





static basilys_ptr_t
rout_4_SCANPAT_SRCPATOR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_4_SCANPAT_SRCPATOR_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 18
  void* varptr[18];
#define CURFRAM_NBVARNUM 5
  long varnum[5];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<18; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 18;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATOR", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:98:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:99:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:99:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:99:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:99:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpator recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:99:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:99:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:99:/ apply.arg");
    argtab[3].bp_long = 99;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:99:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:99:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:99:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:100:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:100:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:100:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:100:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:100:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:100:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(100)?(100):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:100:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:100:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:100:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:101:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:101:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_OR*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:101:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:101:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:101:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:101:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(101)?(101):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:101:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:101:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:101:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:102:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:102:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:103:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "ORPAT_DISJ");/*_.SDISJ__V14*/ curfptr[13] = slot; };
  ;
  /*citerblock FOREACH_IN_MULTIPLE*/ {
  long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SDISJ__V14*/ curfptr[13]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.DIS__V15*/ curfptr[14] = basilys_multiple_nth((basilys_ptr_t)(/*_.SDISJ__V14*/ curfptr[13]), cit1__EACHTUP_ix);
/*_#IX__L4*/ curfnum[0] = cit1__EACHTUP_ix;

  
   BASILYS_LOCATION("warmelt-normatch.bysl:108:/ msend");
   /*msend*/{
    union basilysparam_un argtab[3];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:108:/ ojbmsend.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
    BASILYS_LOCATION("warmelt-normatch.bysl:108:/ ojbmsend.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V4*/ curfptr[3];
    BASILYS_LOCATION("warmelt-normatch.bysl:108:/ ojbmsend.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
    basilysgc_send((basilys_ptr_t)(/*_.DIS__V15*/ curfptr[14]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[3])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
  } /* end cit1__EACHTUP*/
  
   /*citerepilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:105:/ clear");
   /*clear*/ /*_.DIS__V15*/ curfptr[14] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:105:/ clear");
   /*clear*/ /*_#IX__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:105:/ clear");
   /*clear*/ /*_.SCAN_PATTERN__V16*/ curfptr[15] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:110:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:110:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:110:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:110:/ apply.arg");
     argtab[0].bp_cstring =  "scanpat_srcpator end recv";
     BASILYS_LOCATION("warmelt-normatch.bysl:110:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[4];
     BASILYS_LOCATION("warmelt-normatch.bysl:110:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:110:/ apply.arg");
     argtab[3].bp_long = 110;
     /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V17*/ curfptr[16] = /*_.DEBUG_MSG_FUN__V18*/ curfptr[17];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:110:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:110:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:110:/ cppif.else");
  /*_.IFCPP___V17*/ curfptr[16] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFCPP___V17*/ curfptr[16];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:102:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:102:/ clear");
  /*clear*/ /*_.SDISJ__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:102:/ clear");
  /*clear*/ /*_.IFCPP___V17*/ curfptr[16] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:98:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATOR", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_4_SCANPAT_SRCPATOR*/





static basilys_ptr_t
rout_5_SCANPAT_SRCPATAND(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_5_SCANPAT_SRCPATAND_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 18
  void* varptr[18];
#define CURFRAM_NBVARNUM 5
  long varnum[5];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<18; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 18;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATAND", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:117:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:118:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:118:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:118:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:118:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatand recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:118:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:118:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:118:/ apply.arg");
    argtab[3].bp_long = 118;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:118:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:118:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:118:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:119:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(119)?(119):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:120:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:120:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_AND*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:120:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:120:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:120:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:120:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(120)?(120):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:120:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:120:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:120:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:121:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:121:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:122:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "ANDPAT_CONJ");/*_.SCONJ__V14*/ curfptr[13] = slot; };
  ;
  /*citerblock FOREACH_IN_MULTIPLE*/ {
  long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SCONJ__V14*/ curfptr[13]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.DIS__V15*/ curfptr[14] = basilys_multiple_nth((basilys_ptr_t)(/*_.SCONJ__V14*/ curfptr[13]), cit1__EACHTUP_ix);
/*_#IX__L4*/ curfnum[0] = cit1__EACHTUP_ix;

  
   BASILYS_LOCATION("warmelt-normatch.bysl:127:/ msend");
   /*msend*/{
    union basilysparam_un argtab[3];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:127:/ ojbmsend.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
    BASILYS_LOCATION("warmelt-normatch.bysl:127:/ ojbmsend.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V4*/ curfptr[3];
    BASILYS_LOCATION("warmelt-normatch.bysl:127:/ ojbmsend.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
    basilysgc_send((basilys_ptr_t)(/*_.DIS__V15*/ curfptr[14]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[3])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
  } /* end cit1__EACHTUP*/
  
   /*citerepilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:124:/ clear");
   /*clear*/ /*_.DIS__V15*/ curfptr[14] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:124:/ clear");
   /*clear*/ /*_#IX__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:124:/ clear");
   /*clear*/ /*_.SCAN_PATTERN__V16*/ curfptr[15] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:129:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:129:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply.arg");
     argtab[0].bp_cstring =  "scanpat_srcpatand end recv";
     BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[4];
     BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply.arg");
     argtab[3].bp_long = 129;
     /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V17*/ curfptr[16] = /*_.DEBUG_MSG_FUN__V18*/ curfptr[17];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:129:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:129:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:129:/ cppif.else");
  /*_.IFCPP___V17*/ curfptr[16] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFCPP___V17*/ curfptr[16];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:121:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:121:/ clear");
  /*clear*/ /*_.SCONJ__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:121:/ clear");
  /*clear*/ /*_.IFCPP___V17*/ curfptr[16] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:117:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATAND", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_5_SCANPAT_SRCPATAND*/





static basilys_ptr_t
rout_6_SCANPAT_SRCPATVAR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_6_SCANPAT_SRCPATVAR_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 25
  void* varptr[25];
#define CURFRAM_NBVARNUM 6
  long varnum[6];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<25; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 25;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATVAR", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:136:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:137:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:137:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:137:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:137:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatvar recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:137:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:137:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:137:/ apply.arg");
    argtab[3].bp_long = 137;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:137:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:137:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:137:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:138:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:138:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:138:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:138:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:138:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:138:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(138)?(138):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:138:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:138:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:138:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:139:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:139:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_VARIABLE*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:139:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:139:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:139:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:139:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(139)?(139):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:139:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:139:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:139:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:140:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:140:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:141:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SPAT_VAR");/*_.SVAR__V14*/ curfptr[13] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:142:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 3, "PCTN_MAPATVAR");/*_.MAPVAR__V15*/ curfptr[14] = slot; };
  ;
  /*_.MVAR__V16*/ curfptr[15] = 
   (basilys_get_mapobjects((basilysmapobjects_ptr_t)(/*_.MAPVAR__V15*/ curfptr[14]), (basilysobject_ptr_t)(/*_.SVAR__V14*/ curfptr[13])));;
  /*_#NULL__L4*/ curfnum[0] = 
   ((/*_.MVAR__V16*/ curfptr[15]) == NULL);;
  BASILYS_LOCATION("warmelt-normatch.bysl:145:/ cond");
  /*cond*/ if (/*_#NULL__L4*/ curfnum[0]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:145:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:145:/ block");
   /*block*/{
    
     basilysgc_put_mapobjects( (basilysmapobjects_ptr_t) (/*_.MAPVAR__V15*/ curfptr[14]), (basilysobject_ptr_t) (/*_.SVAR__V14*/ curfptr[13]), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]));
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:147:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:147:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:147:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:147:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatvar return new svar";
       BASILYS_LOCATION("warmelt-normatch.bysl:147:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[4];
       BASILYS_LOCATION("warmelt-normatch.bysl:147:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:147:/ apply.arg");
       argtab[3].bp_long = 147;
       /*_.DEBUG_MSG_FUN__V19*/ curfptr[18] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.SVAR__V14*/ curfptr[13]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V18*/ curfptr[17] = /*_.DEBUG_MSG_FUN__V19*/ curfptr[18];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:147:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:147:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V19*/ curfptr[18] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:147:/ cppif.else");
    /*_.IFCPP___V18*/ curfptr[17] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:148:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:148:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:145:/ block");
    /*block*/{
     /*_.PROGN___V21*/ curfptr[20] = /*_.RETURN___V20*/ curfptr[18];;}
    ;
    /*_.IFELSE___V17*/ curfptr[16] = /*_.PROGN___V21*/ curfptr[20];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:145:/ clear");
    /*clear*/ /*_.IFCPP___V18*/ curfptr[17] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:145:/ clear");
    /*clear*/ /*_.RETURN___V20*/ curfptr[18] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:145:/ clear");
    /*clear*/ /*_.PROGN___V21*/ curfptr[20] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:145:/ cond.else");
  
   BASILYS_LOCATION("warmelt-normatch.bysl:145:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:151:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:151:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L6*/ curfnum[4] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:151:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:151:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatvar return found mvar";
       BASILYS_LOCATION("warmelt-normatch.bysl:151:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L6*/ curfnum[4];
       BASILYS_LOCATION("warmelt-normatch.bysl:151:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:151:/ apply.arg");
       argtab[3].bp_long = 151;
       /*_.DEBUG_MSG_FUN__V23*/ curfptr[18] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.MVAR__V16*/ curfptr[15]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V22*/ curfptr[17] = /*_.DEBUG_MSG_FUN__V23*/ curfptr[18];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L6*/ curfnum[4] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V23*/ curfptr[18] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:151:/ cppif.else");
    /*_.IFCPP___V22*/ curfptr[17] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:152:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:152:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:150:/ block");
    /*block*/{
     /*_.PROGN___V25*/ curfptr[18] = /*_.RETURN___V24*/ curfptr[20];;}
    ;
    /*_.IFELSE___V17*/ curfptr[16] = /*_.PROGN___V25*/ curfptr[18];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:145:/ clear");
    /*clear*/ /*_.IFCPP___V22*/ curfptr[17] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:145:/ clear");
    /*clear*/ /*_.RETURN___V24*/ curfptr[20] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:145:/ clear");
    /*clear*/ /*_.PROGN___V25*/ curfptr[18] = 0 ;}
   ;
   }
  ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFELSE___V17*/ curfptr[16];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:140:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:140:/ clear");
  /*clear*/ /*_.SVAR__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:140:/ clear");
  /*clear*/ /*_.MAPVAR__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:140:/ clear");
  /*clear*/ /*_.MVAR__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:140:/ clear");
  /*clear*/ /*_#NULL__L4*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:140:/ clear");
  /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:136:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATVAR", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_6_SCANPAT_SRCPATVAR*/





static basilys_ptr_t
rout_7_SCANPAT_SRCPATJOKER(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_7_SCANPAT_SRCPATJOKER_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 12
  void* varptr[12];
#define CURFRAM_NBVARNUM 3
  long varnum[3];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<12; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 12;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATJOKER", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:160:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:161:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:161:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:161:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:161:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatjoker recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:161:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:161:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:161:/ apply.arg");
    argtab[3].bp_long = 161;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:161:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:161:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:161:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:162:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:162:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:162:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:162:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:162:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:162:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(162)?(162):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:162:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:162:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:162:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:163:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:163:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_JOKERVAR*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:163:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:163:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:163:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:163:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(163)?(163):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:163:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:163:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:163:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:165:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = NULL;;
  BASILYS_LOCATION("warmelt-normatch.bysl:165:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.RETURN___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:160:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ clear");
 /*clear*/ /*_.RETURN___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATJOKER", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_7_SCANPAT_SRCPATJOKER*/





static basilys_ptr_t
rout_8_SCANPAT_SRCPATCONST(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_8_SCANPAT_SRCPATCONST_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 46
  void* varptr[46];
#define CURFRAM_NBVARNUM 13
  long varnum[13];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<46; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 46;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATCONST", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:173:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:174:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:174:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:174:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:174:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatconst recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:174:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:174:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:174:/ apply.arg");
    argtab[3].bp_long = 174;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:174:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:174:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:174:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:175:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:175:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:175:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:175:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:175:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:175:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(175)?(175):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:175:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:175:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:175:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:176:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:176:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_CONSTANT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:176:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:176:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:176:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:176:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(176)?(176):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:176:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:176:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:176:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:177:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SPAT_CONSTX");/*_.SCONST__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:178:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V14*/ curfptr[13] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:179:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 0, "PCTN_NORMCTXT");/*_.NCX__V15*/ curfptr[14] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:180:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 4, "PCTN_MAPATCST");/*_.MAPCST__V16*/ curfptr[15] = slot; };
  ;
  /*_#IS_OBJECT__L4*/ curfnum[0] = 
    (basilys_magic_discr((basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12])) == OBMAG_OBJECT);;
  BASILYS_LOCATION("warmelt-normatch.bysl:181:/ cond");
  /*cond*/ if (/*_#IS_OBJECT__L4*/ curfnum[0]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:181:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:181:/ block");
   /*block*/{
    /*_.MAPOBJECT_GET__V18*/ curfptr[17] = 
     (basilys_get_mapobjects((basilysmapobjects_ptr_t)(/*_.MAPCST__V16*/ curfptr[15]), (basilysobject_ptr_t)(/*_.SCONST__V13*/ curfptr[12])));;
    /*_.MCST__V17*/ curfptr[16] = /*_.MAPOBJECT_GET__V18*/ curfptr[17];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:181:/ clear");
    /*clear*/ /*_.MAPOBJECT_GET__V18*/ curfptr[17] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:181:/ cond.else");
  
   /*_.MCST__V17*/ curfptr[16] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:182:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 5, "PCTN_BINDLIST");/*_.BINDLIST__V19*/ curfptr[17] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:183:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 2, "PCTN_ENV");/*_.ENV__V20*/ curfptr[19] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:184:/ msend");
  /*msend*/{
   union basilysparam_un argtab[1];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:184:/ ojbmsend.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V20*/ curfptr[19];
   /*_.RECTYP__V21*/ curfptr[20] = basilysgc_send((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!GET_CTYPE*/ curfrout->tabval[3])), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:186:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:186:/ block");
   /*block*/{
    /*_#IS_A__L5*/ curfnum[4] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.RECTYP__V21*/ curfptr[20]), (basilys_ptr_t)((/*!CLASS_CTYPE*/ curfrout->tabval[4])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:186:/ cond");
    /*cond*/ if (/*_#IS_A__L5*/ curfnum[4]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:186:/ cond.then");
    /*_.IFELSE___V23*/ curfptr[22] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:186:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:186:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check rectyp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(186)?(186):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V23*/ curfptr[22] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V22*/ curfptr[21] = /*_.IFELSE___V23*/ curfptr[22];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:186:/ clear");
    /*clear*/ /*_#IS_A__L5*/ curfnum[4] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:186:/ clear");
    /*clear*/ /*_.IFELSE___V23*/ curfptr[22] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:186:/ cppif.else");
  /*_.IFCPP___V22*/ curfptr[21] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_#__L6*/ curfnum[4] = 
   ((/*_.RECTYP__V21*/ curfptr[20]) != (/*_.CTYP__V4*/ curfptr[3]));;
  BASILYS_LOCATION("warmelt-normatch.bysl:187:/ cond");
  /*cond*/ if (/*_#__L6*/ curfnum[4]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:187:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:187:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:189:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECTYP__V21*/ curfptr[20]), 1, "NAMED_NAME");/*_.NAMED_NAME__V25*/ curfptr[24] = slot; };
    ;
    
    basilys_error_str((basilys_ptr_t)(/*_.SLOC__V14*/ curfptr[13]), ( "invalid ctype in constant pattern - expecting"), (basilys_ptr_t)(/*_.NAMED_NAME__V25*/ curfptr[24]));
    BASILYS_LOCATION("warmelt-normatch.bysl:191:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3]), 1, "NAMED_NAME");/*_.NAMED_NAME__V26*/ curfptr[25] = slot; };
    ;
    
    basilys_warning_str(0, (basilys_ptr_t)(/*_.SLOC__V14*/ curfptr[13]), ( "got ctype"), (basilys_ptr_t)(/*_.NAMED_NAME__V26*/ curfptr[25]));
    BASILYS_LOCATION("warmelt-normatch.bysl:187:/ block");
    /*block*/{}
    ;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:187:/ clear");
    /*clear*/ /*_.NAMED_NAME__V25*/ curfptr[24] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:187:/ clear");
    /*clear*/ /*_.NAMED_NAME__V26*/ curfptr[25] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:187:/ cond.else");
  
   /*_.IFELSE___V24*/ curfptr[22] = NULL;;
   }
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:193:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:193:/ block");
   /*block*/{
    /*_#IS_LIST__L7*/ curfnum[6] = 
     (basilys_magic_discr((basilys_ptr_t)(/*_.BINDLIST__V19*/ curfptr[17])) == OBMAG_LIST);;
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ cond");
    /*cond*/ if (/*_#IS_LIST__L7*/ curfnum[6]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ cond.then");
    /*_.IFELSE___V28*/ curfptr[25] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:193:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:193:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check bindlist"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(193)?(193):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V28*/ curfptr[25] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V27*/ curfptr[24] = /*_.IFELSE___V28*/ curfptr[25];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ clear");
    /*clear*/ /*_#IS_LIST__L7*/ curfnum[6] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ clear");
    /*clear*/ /*_.IFELSE___V28*/ curfptr[25] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:193:/ cppif.else");
  /*_.IFCPP___V27*/ curfptr[24] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_#IS_OBJECT__L8*/ curfnum[6] = 
    (basilys_magic_discr((basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12])) == OBMAG_OBJECT);;
  /*_#NOT__L9*/ curfnum[8] = 
   (!(/*_#IS_OBJECT__L8*/ curfnum[6]));;
  BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cond");
  /*cond*/ if (/*_#NOT__L9*/ curfnum[8]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:195:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:196:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:196:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:196:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:196:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatconst nonobj literal const";
       BASILYS_LOCATION("warmelt-normatch.bysl:196:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L10*/ curfnum[9];
       BASILYS_LOCATION("warmelt-normatch.bysl:196:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:196:/ apply.arg");
       argtab[3].bp_long = 196;
       /*_.DEBUG_MSG_FUN__V31*/ curfptr[30] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V30*/ curfptr[29] = /*_.DEBUG_MSG_FUN__V31*/ curfptr[30];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V31*/ curfptr[30] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:196:/ cppif.else");
    /*_.IFCPP___V30*/ curfptr[29] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:197:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:197:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:195:/ block");
    /*block*/{
     /*_.PROGN___V33*/ curfptr[32] = /*_.RETURN___V32*/ curfptr[30];;}
    ;
    /*_.IFELSE___V29*/ curfptr[25] = /*_.PROGN___V33*/ curfptr[32];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:195:/ clear");
    /*clear*/ /*_.IFCPP___V30*/ curfptr[29] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:195:/ clear");
    /*clear*/ /*_.RETURN___V32*/ curfptr[30] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:195:/ clear");
    /*clear*/ /*_.PROGN___V33*/ curfptr[32] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cond.else");
  
   BASILYS_LOCATION("warmelt-normatch.bysl:195:/ block");
   /*block*/{
    /*_#NULL__L11*/ curfnum[9] = 
     ((/*_.MCST__V17*/ curfptr[16]) == NULL);;
    BASILYS_LOCATION("warmelt-normatch.bysl:199:/ cond");
    /*cond*/ if (/*_#NULL__L11*/ curfnum[9]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:199:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:199:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:200:/ block");
      /*block*/{
       BASILYS_LOCATION("warmelt-normatch.bysl:200:/ multimsend");
       /*multimsend*/{
        union basilysparam_un argtab[3];
        union basilysparam_un restab[1];
        memset(&argtab, 0, sizeof(argtab));
        memset(&restab, 0, sizeof(restab));
        BASILYS_LOCATION("warmelt-normatch.bysl:200:/ multimsend.arg");
        argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V20*/ curfptr[19];BASILYS_LOCATION("warmelt-normatch.bysl:200:/ multimsend.arg");
        argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V15*/ curfptr[14];BASILYS_LOCATION("warmelt-normatch.bysl:200:/ multimsend.arg");
        argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V14*/ curfptr[13];
        BASILYS_LOCATION("warmelt-normatch.bysl:200:/ multimsend.xres");
        restab[0].bp_aptr = (basilys_ptr_t*) &/*_.NBINDCONST__V37*/ curfptr[36];BASILYS_LOCATION("warmelt-normatch.bysl:200:/ multimsend.send");
        /*_.NCONST__V36*/ curfptr[32] =  basilysgc_send ((basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12]), ((basilys_ptr_t)((/*!NORMAL_EXP*/ curfrout->tabval[5]))), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
        }
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:200:/ block");
       /*block*/{
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:203:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:203:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L12*/ curfnum[11] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:203:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:203:/ apply.arg");
           argtab[0].bp_cstring =  "scanpat_srcpatconst nconst";
           BASILYS_LOCATION("warmelt-normatch.bysl:203:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L12*/ curfnum[11];
           BASILYS_LOCATION("warmelt-normatch.bysl:203:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:203:/ apply.arg");
           argtab[3].bp_long = 203;
           /*_.DEBUG_MSG_FUN__V39*/ curfptr[38] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.NCONST__V36*/ curfptr[32]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V38*/ curfptr[37] = /*_.DEBUG_MSG_FUN__V39*/ curfptr[38];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:203:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L12*/ curfnum[11] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:203:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V39*/ curfptr[38] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:203:/ cppif.else");
        /*_.IFCPP___V38*/ curfptr[37] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        
         basilysgc_put_mapobjects( (basilysmapobjects_ptr_t) (/*_.MAPCST__V16*/ curfptr[15]), (basilysobject_ptr_t) (/*_.SCONST__V13*/ curfptr[12]), (basilys_ptr_t)(/*_.NCONST__V36*/ curfptr[32]));
        BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cond");
        /*cond*/ if (/*_.NBINDCONST__V37*/ curfptr[36]) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cond.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:205:/ block");
         /*block*/{
          BASILYS_LOCATION("warmelt-normatch.bysl:205:/ apply");
          /*apply*/{
           union basilysparam_un argtab[1];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:205:/ apply.arg");
           argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NBINDCONST__V37*/ curfptr[36];
           /*_.LIST_APPEND2LIST__V41*/ curfptr[40] =  basilys_apply ((basilysclosure_ptr_t)((/*!LIST_APPEND2LIST*/ curfrout->tabval[6])), (basilys_ptr_t)(/*_.BINDLIST__V19*/ curfptr[17]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IF___V40*/ curfptr[38] = /*_.LIST_APPEND2LIST__V41*/ curfptr[40];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:205:/ clear");
          /*clear*/ /*_.LIST_APPEND2LIST__V41*/ curfptr[40] = 0 ;}
         ;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cond.else");
        
         /*_.IF___V40*/ curfptr[38] = NULL;;
         }
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:206:/ block");
        /*block*/{
         /*_.RETVAL___V1*/ curfptr[0] = NULL;;
         BASILYS_LOCATION("warmelt-normatch.bysl:206:/ finalreturn");
         ;
         /*finalret*/ goto labend_rout ;}
        ;
        /*_.MULTI___V35*/ curfptr[30] = /*_.RETURN___V42*/ curfptr[40];;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:200:/ clear");
        /*clear*/ /*_.IFCPP___V38*/ curfptr[37] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:200:/ clear");
        /*clear*/ /*_.IF___V40*/ curfptr[38] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:200:/ clear");
        /*clear*/ /*_.RETURN___V42*/ curfptr[40] = 0 ;}
       ;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:200:/ clear");
       /*clear*/ /*_.NBINDCONST__V37*/ curfptr[36] = 0 ;}
      ;
      /*_.IFELSE___V34*/ curfptr[29] = /*_.MULTI___V35*/ curfptr[30];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:199:/ clear");
      /*clear*/ /*_.MULTI___V35*/ curfptr[30] = 0 ;}
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:199:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:199:/ block");
     /*block*/{
      
      #if ENABLE_CHECKING
       BASILYS_LOCATION("warmelt-normatch.bysl:209:/ cppif.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:209:/ block");
       /*block*/{
        /*_#THE_CALLCOUNT__L13*/ curfnum[11] = 
         callcount;;
        BASILYS_LOCATION("warmelt-normatch.bysl:209:/ apply");
        /*apply*/{
         union basilysparam_un argtab[4];
         memset(&argtab, 0, sizeof(argtab));
         BASILYS_LOCATION("warmelt-normatch.bysl:209:/ apply.arg");
         argtab[0].bp_cstring =  "scanpat_srcpatconst got mcst";
         BASILYS_LOCATION("warmelt-normatch.bysl:209:/ apply.arg");
         argtab[1].bp_long = /*_#THE_CALLCOUNT__L13*/ curfnum[11];
         BASILYS_LOCATION("warmelt-normatch.bysl:209:/ apply.arg");
         argtab[2].bp_cstring =  "warmelt-normatch.bysl";
         BASILYS_LOCATION("warmelt-normatch.bysl:209:/ apply.arg");
         argtab[3].bp_long = 209;
         /*_.DEBUG_MSG_FUN__V44*/ curfptr[38] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.MCST__V17*/ curfptr[16]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
         }
        ;
        /*_.IFCPP___V43*/ curfptr[37] = /*_.DEBUG_MSG_FUN__V44*/ curfptr[38];;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:209:/ clear");
        /*clear*/ /*_#THE_CALLCOUNT__L13*/ curfnum[11] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:209:/ clear");
        /*clear*/ /*_.DEBUG_MSG_FUN__V44*/ curfptr[38] = 0 ;}
       
       #else /*ENABLE_CHECKING*/
       BASILYS_LOCATION("warmelt-normatch.bysl:209:/ cppif.else");
      /*_.IFCPP___V43*/ curfptr[37] = (/*nil*/NULL);
       #endif /*ENABLE_CHECKING*/
       ;
      BASILYS_LOCATION("warmelt-normatch.bysl:210:/ block");
      /*block*/{
       /*_.RETVAL___V1*/ curfptr[0] = NULL;;
       BASILYS_LOCATION("warmelt-normatch.bysl:210:/ finalreturn");
       ;
       /*finalret*/ goto labend_rout ;}
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:208:/ block");
      /*block*/{
       /*_.PROGN___V46*/ curfptr[32] = /*_.RETURN___V45*/ curfptr[40];;}
      ;
      /*_.IFELSE___V34*/ curfptr[29] = /*_.PROGN___V46*/ curfptr[32];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:199:/ clear");
      /*clear*/ /*_.IFCPP___V43*/ curfptr[37] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:199:/ clear");
      /*clear*/ /*_.RETURN___V45*/ curfptr[40] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:199:/ clear");
      /*clear*/ /*_.PROGN___V46*/ curfptr[32] = 0 ;}
     ;
     }
    ;
    /*_.IFELSE___V29*/ curfptr[25] = /*_.IFELSE___V34*/ curfptr[29];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:195:/ clear");
    /*clear*/ /*_#NULL__L11*/ curfnum[9] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:195:/ clear");
    /*clear*/ /*_.IFELSE___V34*/ curfptr[29] = 0 ;}
   ;
   }
  ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFELSE___V29*/ curfptr[25];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.SCONST__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.SLOC__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.NCX__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.MAPCST__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_#IS_OBJECT__L4*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.MCST__V17*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.BINDLIST__V19*/ curfptr[17] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.ENV__V20*/ curfptr[19] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.RECTYP__V21*/ curfptr[20] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.IFCPP___V22*/ curfptr[21] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_#__L6*/ curfnum[4] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.IFELSE___V24*/ curfptr[22] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.IFCPP___V27*/ curfptr[24] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_#IS_OBJECT__L8*/ curfnum[6] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_#NOT__L9*/ curfnum[8] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
  /*clear*/ /*_.IFELSE___V29*/ curfptr[25] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:173:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATCONST", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_8_SCANPAT_SRCPATCONST*/





static basilys_ptr_t
rout_9_SCANPAT_SRCPATOBJECT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_9_SCANPAT_SRCPATOBJECT_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 39
  void* varptr[39];
#define CURFRAM_NBVARNUM 11
  long varnum[11];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<39; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 39;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATOBJECT", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:218:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:219:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:219:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:219:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:219:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatobject recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:219:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:219:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:219:/ apply.arg");
    argtab[3].bp_long = 219;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:219:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:219:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:219:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:220:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:220:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:220:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:220:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:220:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:220:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(220)?(220):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:220:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:220:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:220:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:221:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:221:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_OBJECT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:221:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:221:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:221:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:221:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(221)?(221):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:221:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:221:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:221:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:222:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:222:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:223:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SPAT_CLASS");/*_.SCLA__V14*/ curfptr[13] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:224:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 2, "SPAT_FIELDS");/*_.SFIELDS__V15*/ curfptr[14] = slot; };
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:226:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:226:/ block");
   /*block*/{
    /*_#IS_A__L4*/ curfnum[0] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.SCLA__V14*/ curfptr[13]), (basilys_ptr_t)((/*!CLASS_CLASS*/ curfrout->tabval[3])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:226:/ cond");
    /*cond*/ if (/*_#IS_A__L4*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:226:/ cond.then");
    /*_.IFELSE___V17*/ curfptr[16] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:226:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:226:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check scla"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(226)?(226):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V16*/ curfptr[15] = /*_.IFELSE___V17*/ curfptr[16];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:226:/ clear");
    /*clear*/ /*_#IS_A__L4*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:226:/ clear");
    /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:226:/ cppif.else");
  /*_.IFCPP___V16*/ curfptr[15] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:227:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:227:/ block");
   /*block*/{
    /*_#IS_MULTIPLE__L5*/ curfnum[0] = 
     (basilys_magic_discr((basilys_ptr_t)(/*_.SFIELDS__V15*/ curfptr[14])) == OBMAG_MULTIPLE);;
    BASILYS_LOCATION("warmelt-normatch.bysl:227:/ cond");
    /*cond*/ if (/*_#IS_MULTIPLE__L5*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:227:/ cond.then");
    /*_.IFELSE___V19*/ curfptr[18] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:227:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:227:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check sfields"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(227)?(227):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V18*/ curfptr[16] = /*_.IFELSE___V19*/ curfptr[18];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:227:/ clear");
    /*clear*/ /*_#IS_MULTIPLE__L5*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:227:/ clear");
    /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:227:/ cppif.else");
  /*_.IFCPP___V18*/ curfptr[16] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*citerblock FOREACH_IN_MULTIPLE*/ {
  long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SFIELDS__V15*/ curfptr[14]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.FLDPA__V20*/ curfptr[18] = basilys_multiple_nth((basilys_ptr_t)(/*_.SFIELDS__V15*/ curfptr[14]), cit1__EACHTUP_ix);
/*_#IX__L6*/ curfnum[0] = cit1__EACHTUP_ix;

  
   
   #if ENABLE_CHECKING
    BASILYS_LOCATION("warmelt-normatch.bysl:231:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:231:/ block");
    /*block*/{
     /*_#IS_A__L7*/ curfnum[6] = 
       basilys_is_instance_of((basilys_ptr_t)(/*_.FLDPA__V20*/ curfptr[18]), (basilys_ptr_t)((/*!CLASS_SRCFIELDPATTERN*/ curfrout->tabval[4])));;
     BASILYS_LOCATION("warmelt-normatch.bysl:231:/ cond");
     /*cond*/ if (/*_#IS_A__L7*/ curfnum[6]) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:231:/ cond.then");
     /*_.IFELSE___V22*/ curfptr[21] = (/*nil*/NULL);;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:231:/ cond.else");
     
      BASILYS_LOCATION("warmelt-normatch.bysl:231:/ block");
      /*block*/{
       /*block*/{
        
        basilys_assert_failed(( "check fldp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(231)?(231):__LINE__, __FUNCTION__);
        /*clear*/ /*_.IFELSE___V22*/ curfptr[21] = 0 ;}
       ;
       /*epilog*/}
      ;
      }
     ;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
     /*clear*/ /*_#IS_A__L7*/ curfnum[6] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
     /*clear*/ /*_.IFELSE___V22*/ curfptr[21] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:231:/ cppif.else");
   (/*nil*/NULL)
    #endif /*ENABLE_CHECKING*/
    ;
   BASILYS_LOCATION("warmelt-normatch.bysl:232:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ block");
    /*block*/{
     BASILYS_LOCATION("warmelt-normatch.bysl:232:/ cond");
     /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.FLDPA__V20*/ curfptr[18]), (basilys_ptr_t)((/*!CLASS_SRCFIELDPATTERN*/ curfrout->tabval[4])))) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:232:/ cond.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:232:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLDPA__V20*/ curfptr[18]), 1, "SPAF_FIELD");/*_.F__V25*/ curfptr[24] = slot; };
      ;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:232:/ cond.else");
     
      /*_.F__V25*/ curfptr[24] = NULL;;
      }
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:233:/ block");
      /*block*/{
       /*_#IS_A__L8*/ curfnum[6] = 
         basilys_is_instance_of((basilys_ptr_t)(/*_.F__V25*/ curfptr[24]), (basilys_ptr_t)((/*!CLASS_FIELD*/ curfrout->tabval[5])));;
       BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cond");
       /*cond*/ if (/*_#IS_A__L8*/ curfnum[6]) /*then*/ {
       BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cond.then");
       /*_.IFELSE___V27*/ curfptr[26] = (/*nil*/NULL);;
       } else {BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cond.else");
       
        BASILYS_LOCATION("warmelt-normatch.bysl:233:/ block");
        /*block*/{
         /*block*/{
          
          basilys_assert_failed(( "check fld"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(233)?(233):__LINE__, __FUNCTION__);
          /*clear*/ /*_.IFELSE___V27*/ curfptr[26] = 0 ;}
         ;
         /*epilog*/}
        ;
        }
       ;
       /*_.IFCPP___V26*/ curfptr[25] = /*_.IFELSE___V27*/ curfptr[26];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:233:/ clear");
       /*clear*/ /*_#IS_A__L8*/ curfnum[6] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:233:/ clear");
       /*clear*/ /*_.IFELSE___V27*/ curfptr[26] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cppif.else");
     /*_.IFCPP___V26*/ curfptr[25] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     /*_.LET___V24*/ curfptr[23] = /*_.F__V25*/ curfptr[24];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
     /*clear*/ /*_.F__V25*/ curfptr[24] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
     /*clear*/ /*_.IFCPP___V26*/ curfptr[25] = 0 ;}
    ;
    /*_.FLD__V28*/ curfptr[26] = /*_.LET___V24*/ curfptr[23];;
    BASILYS_LOCATION("warmelt-normatch.bysl:235:/ cond");
    /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.FLD__V28*/ curfptr[26]), (basilys_ptr_t)((/*!CLASS_FIELD*/ curfrout->tabval[5])))) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:235:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:235:/ getslot");
     { basilys_ptr_t slot=0;
      basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLD__V28*/ curfptr[26]), 2, "FLD_OWNCLASS");/*_.FLCLA__V29*/ curfptr[24] = slot; };
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:235:/ cond.else");
    
     /*_.FLCLA__V29*/ curfptr[24] = NULL;;
     }
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:236:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLDPA__V20*/ curfptr[18]), 2, "SPAF_PATTERN");/*_.FPAT__V30*/ curfptr[25] = slot; };
    ;
    /*_#SUBCLASS_OR_EQ__L9*/ curfnum[6] = 
     ((/*_.SCLA__V14*/ curfptr[13] == /*_.FLCLA__V29*/ curfptr[24]) || basilys_is_subclass_of((basilysobject_ptr_t)(/*_.SCLA__V14*/ curfptr[13]), (basilysobject_ptr_t)(/*_.FLCLA__V29*/ curfptr[24])));;
    /*_#NOT__L10*/ curfnum[9] = 
     (!(/*_#SUBCLASS_OR_EQ__L9*/ curfnum[6]));;
    BASILYS_LOCATION("warmelt-normatch.bysl:238:/ cond");
    /*cond*/ if (/*_#NOT__L10*/ curfnum[9]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:238:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:238:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:240:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLD__V28*/ curfptr[26]), 1, "NAMED_NAME");/*_.NAMED_NAME__V32*/ curfptr[31] = slot; };
      ;
      
      basilys_error_str((basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "bad field in object pattern"), (basilys_ptr_t)(/*_.NAMED_NAME__V32*/ curfptr[31]));
      BASILYS_LOCATION("warmelt-normatch.bysl:241:/ cond");
      /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.SCLA__V14*/ curfptr[13]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[6])))) /*then*/ {
      BASILYS_LOCATION("warmelt-normatch.bysl:241:/ cond.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:241:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SCLA__V14*/ curfptr[13]), 1, "NAMED_NAME");/*_.NAMED_NAME__V33*/ curfptr[32] = slot; };
       ;
      } else {BASILYS_LOCATION("warmelt-normatch.bysl:241:/ cond.else");
      
       /*_.NAMED_NAME__V33*/ curfptr[32] = NULL;;
       }
      ;
      
      basilys_inform_str((basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "class in pattern is"), (basilys_ptr_t)(/*_.NAMED_NAME__V33*/ curfptr[32]));
      BASILYS_LOCATION("warmelt-normatch.bysl:242:/ cond");
      /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.FLCLA__V29*/ curfptr[24]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[6])))) /*then*/ {
      BASILYS_LOCATION("warmelt-normatch.bysl:242:/ cond.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:242:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLCLA__V29*/ curfptr[24]), 1, "NAMED_NAME");/*_.NAMED_NAME__V34*/ curfptr[33] = slot; };
       ;
      } else {BASILYS_LOCATION("warmelt-normatch.bysl:242:/ cond.else");
      
       /*_.NAMED_NAME__V34*/ curfptr[33] = NULL;;
       }
      ;
      
      basilys_inform_str((basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "class of field is"), (basilys_ptr_t)(/*_.NAMED_NAME__V34*/ curfptr[33]));
      BASILYS_LOCATION("warmelt-normatch.bysl:243:/ block");
      /*block*/{
       /*_.RETVAL___V1*/ curfptr[0] = NULL;;
       BASILYS_LOCATION("warmelt-normatch.bysl:243:/ finalreturn");
       ;
       /*finalret*/ goto labend_rout ;}
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ block");
      /*block*/{
       /*_.PROGN___V36*/ curfptr[35] = /*_.RETURN___V35*/ curfptr[34];;}
      ;
      /*_.IFELSE___V31*/ curfptr[30] = /*_.PROGN___V36*/ curfptr[35];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ clear");
      /*clear*/ /*_.NAMED_NAME__V32*/ curfptr[31] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ clear");
      /*clear*/ /*_.NAMED_NAME__V33*/ curfptr[32] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ clear");
      /*clear*/ /*_.NAMED_NAME__V34*/ curfptr[33] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ clear");
      /*clear*/ /*_.RETURN___V35*/ curfptr[34] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ clear");
      /*clear*/ /*_.PROGN___V36*/ curfptr[35] = 0 ;}
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:238:/ cond.else");
    
     /*_.IFELSE___V31*/ curfptr[30] = NULL;;
     }
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:244:/ msend");
    /*msend*/{
     union basilysparam_un argtab[3];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:244:/ ojbmsend.arg");
     argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
     BASILYS_LOCATION("warmelt-normatch.bysl:244:/ ojbmsend.arg");
     argtab[1].bp_aptr = (basilys_ptr_t*) &(/*!CTYPE_VALUE*/ curfrout->tabval[8]);
     BASILYS_LOCATION("warmelt-normatch.bysl:244:/ ojbmsend.arg");
     argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
     /*_.SCAN_PATTERN__V37*/ curfptr[31] = basilysgc_send((basilys_ptr_t)(/*_.FPAT__V30*/ curfptr[25]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[7])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_.LET___V24*/ curfptr[23] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_.FLD__V28*/ curfptr[26] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_.FLCLA__V29*/ curfptr[24] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_.FPAT__V30*/ curfptr[25] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_#SUBCLASS_OR_EQ__L9*/ curfnum[6] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_#NOT__L10*/ curfnum[9] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_.IFELSE___V31*/ curfptr[30] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
    /*clear*/ /*_.SCAN_PATTERN__V37*/ curfptr[31] = 0 ;}
   ;
  } /* end cit1__EACHTUP*/
  
   /*citerepilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ clear");
   /*clear*/ /*_.FLDPA__V20*/ curfptr[18] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ clear");
   /*clear*/ /*_#IX__L6*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ clear");
   /*clear*/ /*_.IFCPP___V21*/ curfptr[20] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ clear");
   /*clear*/ /*_.LET___V23*/ curfptr[21] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:247:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:247:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L11*/ curfnum[6] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:247:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:247:/ apply.arg");
     argtab[0].bp_cstring =  "scanpat_srcpatobject end recv";
     BASILYS_LOCATION("warmelt-normatch.bysl:247:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L11*/ curfnum[6];
     BASILYS_LOCATION("warmelt-normatch.bysl:247:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:247:/ apply.arg");
     argtab[3].bp_long = 247;
     /*_.DEBUG_MSG_FUN__V39*/ curfptr[33] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V38*/ curfptr[32] = /*_.DEBUG_MSG_FUN__V39*/ curfptr[33];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:247:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L11*/ curfnum[6] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:247:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V39*/ curfptr[33] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:247:/ cppif.else");
  /*_.IFCPP___V38*/ curfptr[32] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFCPP___V38*/ curfptr[32];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:222:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:222:/ clear");
  /*clear*/ /*_.SCLA__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:222:/ clear");
  /*clear*/ /*_.SFIELDS__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:222:/ clear");
  /*clear*/ /*_.IFCPP___V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:222:/ clear");
  /*clear*/ /*_.IFCPP___V18*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:222:/ clear");
  /*clear*/ /*_.IFCPP___V38*/ curfptr[32] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:218:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATOBJECT", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_9_SCANPAT_SRCPATOBJECT*/





static basilys_ptr_t
rout_10_SCANPAT_SRCPATCOMPOSITE(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_10_SCANPAT_SRCPATCOMPOSITE_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 58
  void* varptr[58];
#define CURFRAM_NBVARNUM 14
  long varnum[14];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<58; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 58;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATCOMPOSITE", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:256:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:257:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:257:/ block");
  /*block*/{
   /*_#IS_A__L1*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[0])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:257:/ cond");
   /*cond*/ if (/*_#IS_A__L1*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:257:/ cond.then");
   /*_.IFELSE___V7*/ curfptr[6] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:257:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:257:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(257)?(257):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.IFELSE___V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:257:/ clear");
   /*clear*/ /*_#IS_A__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:257:/ clear");
   /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:257:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:258:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:258:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_COMPOSITE*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:258:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:258:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:258:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:258:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(258)?(258):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:258:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:258:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:258:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:259:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:259:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L3*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:259:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:259:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatcomposite recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:259:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L3*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:259:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:259:/ apply.arg");
    argtab[3].bp_long = 259;
    /*_.DEBUG_MSG_FUN__V11*/ curfptr[10] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.DEBUG_MSG_FUN__V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:259:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:259:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:259:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:260:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:261:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:261:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 3, "SPAC_OPERATOR");/*_.SOP__V15*/ curfptr[14] = slot; };
   ;
   
   #if ENABLE_CHECKING
    BASILYS_LOCATION("warmelt-normatch.bysl:262:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:262:/ block");
    /*block*/{
     /*_#IS_A__L4*/ curfnum[0] = 
       basilys_is_instance_of((basilys_ptr_t)(/*_.SOP__V15*/ curfptr[14]), (basilys_ptr_t)((/*!CLASS_ANY_MATCHER*/ curfrout->tabval[3])));;
     BASILYS_LOCATION("warmelt-normatch.bysl:262:/ cond");
     /*cond*/ if (/*_#IS_A__L4*/ curfnum[0]) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:262:/ cond.then");
     /*_.IFELSE___V17*/ curfptr[16] = (/*nil*/NULL);;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:262:/ cond.else");
     
      BASILYS_LOCATION("warmelt-normatch.bysl:262:/ block");
      /*block*/{
       /*block*/{
        
        basilys_assert_failed(( "check soper"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(262)?(262):__LINE__, __FUNCTION__);
        /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
       ;
       /*epilog*/}
      ;
      }
     ;
     /*_.IFCPP___V16*/ curfptr[15] = /*_.IFELSE___V17*/ curfptr[16];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:262:/ clear");
     /*clear*/ /*_#IS_A__L4*/ curfnum[0] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:262:/ clear");
     /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:262:/ cppif.else");
   /*_.IFCPP___V16*/ curfptr[15] = (/*nil*/NULL);
    #endif /*ENABLE_CHECKING*/
    ;
   /*_.LET___V14*/ curfptr[13] = /*_.SOP__V15*/ curfptr[14];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:261:/ clear");
   /*clear*/ /*_.SOP__V15*/ curfptr[14] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:261:/ clear");
   /*clear*/ /*_.IFCPP___V16*/ curfptr[15] = 0 ;}
  ;
  /*_.SOPER__V18*/ curfptr[16] = /*_.LET___V14*/ curfptr[13];;
  BASILYS_LOCATION("warmelt-normatch.bysl:264:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 4, "SPAC_INARGS");/*_.SINS__V19*/ curfptr[14] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:265:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 5, "SPAC_OUTARGS");/*_.SOUTS__V20*/ curfptr[15] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:266:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:266:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 3, "AMATCH_MATCHBIND");/*_.MB__V22*/ curfptr[21] = slot; };
   ;
   
   #if ENABLE_CHECKING
    BASILYS_LOCATION("warmelt-normatch.bysl:267:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:267:/ block");
    /*block*/{
     /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 
      callcount;;
     BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply");
     /*apply*/{
      union basilysparam_un argtab[4];
      memset(&argtab, 0, sizeof(argtab));
      BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
      argtab[0].bp_cstring =  "scanpat_srcpatcomposite matmb";
      BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
      argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[0];
      BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
      argtab[2].bp_cstring =  "warmelt-normatch.bysl";
      BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
      argtab[3].bp_long = 267;
      /*_.DEBUG_MSG_FUN__V24*/ curfptr[23] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.MB__V22*/ curfptr[21]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
      }
     ;
     /*_.IFCPP___V23*/ curfptr[22] = /*_.DEBUG_MSG_FUN__V24*/ curfptr[23];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:267:/ clear");
     /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:267:/ clear");
     /*clear*/ /*_.DEBUG_MSG_FUN__V24*/ curfptr[23] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:267:/ cppif.else");
   /*_.IFCPP___V23*/ curfptr[22] = (/*nil*/NULL);
    #endif /*ENABLE_CHECKING*/
    ;
   
   #if ENABLE_CHECKING
    BASILYS_LOCATION("warmelt-normatch.bysl:268:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:268:/ block");
    /*block*/{
     /*_#IS_A__L6*/ curfnum[0] = 
       basilys_is_instance_of((basilys_ptr_t)(/*_.MB__V22*/ curfptr[21]), (basilys_ptr_t)((/*!CLASS_FORMAL_BINDING*/ curfrout->tabval[4])));;
     BASILYS_LOCATION("warmelt-normatch.bysl:268:/ cond");
     /*cond*/ if (/*_#IS_A__L6*/ curfnum[0]) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:268:/ cond.then");
     /*_.IFELSE___V26*/ curfptr[25] = (/*nil*/NULL);;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:268:/ cond.else");
     
      BASILYS_LOCATION("warmelt-normatch.bysl:268:/ block");
      /*block*/{
       /*block*/{
        
        basilys_assert_failed(( "check matmb"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(268)?(268):__LINE__, __FUNCTION__);
        /*clear*/ /*_.IFELSE___V26*/ curfptr[25] = 0 ;}
       ;
       /*epilog*/}
      ;
      }
     ;
     /*_.IFCPP___V25*/ curfptr[23] = /*_.IFELSE___V26*/ curfptr[25];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:268:/ clear");
     /*clear*/ /*_#IS_A__L6*/ curfnum[0] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:268:/ clear");
     /*clear*/ /*_.IFELSE___V26*/ curfptr[25] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:268:/ cppif.else");
   /*_.IFCPP___V25*/ curfptr[23] = (/*nil*/NULL);
    #endif /*ENABLE_CHECKING*/
    ;
   /*_.LET___V21*/ curfptr[20] = /*_.MB__V22*/ curfptr[21];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:266:/ clear");
   /*clear*/ /*_.MB__V22*/ curfptr[21] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:266:/ clear");
   /*clear*/ /*_.IFCPP___V23*/ curfptr[22] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:266:/ clear");
   /*clear*/ /*_.IFCPP___V25*/ curfptr[23] = 0 ;}
  ;
  /*_.MATMB__V27*/ curfptr[25] = /*_.LET___V21*/ curfptr[20];;
  BASILYS_LOCATION("warmelt-normatch.bysl:270:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 2, "AMATCH_IN");/*_.MATIN__V28*/ curfptr[21] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:271:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 4, "AMATCH_OUT");/*_.MATOUT__V29*/ curfptr[22] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:272:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 1, "NAMED_NAME");/*_.OPNAM__V30*/ curfptr[23] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:273:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MATMB__V27*/ curfptr[25]), 1, "FBIND_TYPE");/*_.MATCTYP__V31*/ curfptr[30] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:274:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 2, "PCTN_ENV");/*_.ENV__V32*/ curfptr[31] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:275:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 0, "PCTN_NORMCTXT");/*_.NCX__V33*/ curfptr[32] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:276:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 5, "PCTN_BINDLIST");/*_.BINDLIST__V34*/ curfptr[33] = slot; };
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:278:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:278:/ block");
   /*block*/{
    /*_#IS_A__L7*/ curfnum[0] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3]), (basilys_ptr_t)((/*!CLASS_CTYPE*/ curfrout->tabval[5])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:278:/ cond");
    /*cond*/ if (/*_#IS_A__L7*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:278:/ cond.then");
    /*_.IFELSE___V36*/ curfptr[35] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:278:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:278:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check ctyp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(278)?(278):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V36*/ curfptr[35] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V35*/ curfptr[34] = /*_.IFELSE___V36*/ curfptr[35];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:278:/ clear");
    /*clear*/ /*_#IS_A__L7*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:278:/ clear");
    /*clear*/ /*_.IFELSE___V36*/ curfptr[35] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:278:/ cppif.else");
  /*_.IFCPP___V35*/ curfptr[34] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:279:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:279:/ block");
   /*block*/{
    /*_#IS_A__L8*/ curfnum[0] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.MATCTYP__V31*/ curfptr[30]), (basilys_ptr_t)((/*!CLASS_CTYPE*/ curfrout->tabval[5])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:279:/ cond");
    /*cond*/ if (/*_#IS_A__L8*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:279:/ cond.then");
    /*_.IFELSE___V38*/ curfptr[37] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:279:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:279:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check matctyp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(279)?(279):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V38*/ curfptr[37] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V37*/ curfptr[35] = /*_.IFELSE___V38*/ curfptr[37];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:279:/ clear");
    /*clear*/ /*_#IS_A__L8*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:279:/ clear");
    /*clear*/ /*_.IFELSE___V38*/ curfptr[37] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:279:/ cppif.else");
  /*_.IFCPP___V37*/ curfptr[35] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_#__L9*/ curfnum[0] = 
   ((/*_.CTYP__V4*/ curfptr[3]) != (/*_.MATCTYP__V31*/ curfptr[30]));;
  BASILYS_LOCATION("warmelt-normatch.bysl:280:/ cond");
  /*cond*/ if (/*_#__L9*/ curfnum[0]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:280:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:280:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:282:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MATCTYP__V31*/ curfptr[30]), 1, "NAMED_NAME");/*_.NAMED_NAME__V40*/ curfptr[39] = slot; };
    ;
    
    basilys_error_str((basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "invalid ctype in composite partner - expecting"), (basilys_ptr_t)(/*_.NAMED_NAME__V40*/ curfptr[39]));
    BASILYS_LOCATION("warmelt-normatch.bysl:284:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3]), 1, "NAMED_NAME");/*_.NAMED_NAME__V41*/ curfptr[40] = slot; };
    ;
    
    basilys_warning_str(0, (basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "got ctype"), (basilys_ptr_t)(/*_.NAMED_NAME__V41*/ curfptr[40]));
    BASILYS_LOCATION("warmelt-normatch.bysl:285:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:285:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:280:/ block");
    /*block*/{
     /*_.PROGN___V43*/ curfptr[42] = /*_.RETURN___V42*/ curfptr[41];;}
    ;
    /*_.IFELSE___V39*/ curfptr[37] = /*_.PROGN___V43*/ curfptr[42];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:280:/ clear");
    /*clear*/ /*_.NAMED_NAME__V40*/ curfptr[39] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:280:/ clear");
    /*clear*/ /*_.NAMED_NAME__V41*/ curfptr[40] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:280:/ clear");
    /*clear*/ /*_.RETURN___V42*/ curfptr[41] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:280:/ clear");
    /*clear*/ /*_.PROGN___V43*/ curfptr[42] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:280:/ cond.else");
  
   /*_.IFELSE___V39*/ curfptr[37] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:286:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:286:/ multiapply");
   /*multiapply 4args, 1x.res*/ {
    union basilysparam_un argtab[3];
    
    union basilysparam_un restab[1];
    memset(&restab, 0, sizeof(restab));
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:286:/ multiapply.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V32*/ curfptr[31];BASILYS_LOCATION("warmelt-normatch.bysl:286:/ multiapply.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V33*/ curfptr[32];BASILYS_LOCATION("warmelt-normatch.bysl:286:/ multiapply.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
    BASILYS_LOCATION("warmelt-normatch.bysl:286:/ multiapply.xres");
    restab[0].bp_aptr = (basilys_ptr_t*) &/*_.BINDINS__V46*/ curfptr[41];BASILYS_LOCATION("warmelt-normatch.bysl:286:/ multiapply.appl");
    /*_.NINS__V45*/ curfptr[40] =  basilys_apply ((basilysclosure_ptr_t)((/*!NORMALIZE_TUPLE*/ curfrout->tabval[6])), (basilys_ptr_t)(/*_.SINS__V19*/ curfptr[14]), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
    }
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:286:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:289:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:289:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:289:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:289:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatcomposite nins";
       BASILYS_LOCATION("warmelt-normatch.bysl:289:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L10*/ curfnum[9];
       BASILYS_LOCATION("warmelt-normatch.bysl:289:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:289:/ apply.arg");
       argtab[3].bp_long = 289;
       /*_.DEBUG_MSG_FUN__V48*/ curfptr[47] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.NINS__V45*/ curfptr[40]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V47*/ curfptr[42] = /*_.DEBUG_MSG_FUN__V48*/ curfptr[47];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:289:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:289:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V48*/ curfptr[47] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:289:/ cppif.else");
    /*_.IFCPP___V47*/ curfptr[42] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:290:/ cond");
    /*cond*/ if (/*_.BINDINS__V46*/ curfptr[41]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:290:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:290:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:290:/ apply");
      /*apply*/{
       union basilysparam_un argtab[1];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:290:/ apply.arg");
       argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.BINDINS__V46*/ curfptr[41];
       /*_.LIST_APPEND2LIST__V50*/ curfptr[49] =  basilys_apply ((basilysclosure_ptr_t)((/*!LIST_APPEND2LIST*/ curfrout->tabval[7])), (basilys_ptr_t)(/*_.BINDLIST__V34*/ curfptr[33]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IF___V49*/ curfptr[47] = /*_.LIST_APPEND2LIST__V50*/ curfptr[49];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:290:/ clear");
      /*clear*/ /*_.LIST_APPEND2LIST__V50*/ curfptr[49] = 0 ;}
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:290:/ cond.else");
    
     /*_.IF___V49*/ curfptr[47] = NULL;;
     }
    ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:291:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:291:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L11*/ curfnum[9] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:291:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:291:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatcomposite souts";
       BASILYS_LOCATION("warmelt-normatch.bysl:291:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L11*/ curfnum[9];
       BASILYS_LOCATION("warmelt-normatch.bysl:291:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:291:/ apply.arg");
       argtab[3].bp_long = 291;
       /*_.DEBUG_MSG_FUN__V52*/ curfptr[51] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.SOUTS__V20*/ curfptr[15]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V51*/ curfptr[49] = /*_.DEBUG_MSG_FUN__V52*/ curfptr[51];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:291:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L11*/ curfnum[9] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:291:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V52*/ curfptr[51] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:291:/ cppif.else");
    /*_.IFCPP___V51*/ curfptr[49] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    /*citerblock FOREACH_IN_MULTIPLE*/ {
    long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SOUTS__V20*/ curfptr[15]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.POUT__V53*/ curfptr[51] = basilys_multiple_nth((basilys_ptr_t)(/*_.SOUTS__V20*/ curfptr[15]), cit1__EACHTUP_ix);
/*_#IX__L12*/ curfnum[9] = cit1__EACHTUP_ix;

    
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:295:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:295:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L13*/ curfnum[12] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:295:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:295:/ apply.arg");
        argtab[0].bp_cstring =  "scanpat_srcpatcomposite pout";
        BASILYS_LOCATION("warmelt-normatch.bysl:295:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L13*/ curfnum[12];
        BASILYS_LOCATION("warmelt-normatch.bysl:295:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:295:/ apply.arg");
        argtab[3].bp_long = 295;
        /*_.DEBUG_MSG_FUN__V55*/ curfptr[54] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.POUT__V53*/ curfptr[51]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:295:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L13*/ curfnum[12] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:295:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V55*/ curfptr[54] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:295:/ cppif.else");
     (/*nil*/NULL)
      #endif /*ENABLE_CHECKING*/
      ;
     BASILYS_LOCATION("warmelt-normatch.bysl:296:/ msend");
     /*msend*/{
      union basilysparam_un argtab[3];
      memset(&argtab, 0, sizeof(argtab));
      BASILYS_LOCATION("warmelt-normatch.bysl:296:/ ojbmsend.arg");
      argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
      BASILYS_LOCATION("warmelt-normatch.bysl:296:/ ojbmsend.arg");
      argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V4*/ curfptr[3];
      BASILYS_LOCATION("warmelt-normatch.bysl:296:/ ojbmsend.arg");
      argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
      basilysgc_send((basilys_ptr_t)(/*_.POUT__V53*/ curfptr[51]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[8])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
      }
     ;
    } /* end cit1__EACHTUP*/
    
     /*citerepilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
     /*clear*/ /*_.POUT__V53*/ curfptr[51] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
     /*clear*/ /*_#IX__L12*/ curfnum[9] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
     /*clear*/ /*_.IFCPP___V54*/ curfptr[53] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
     /*clear*/ /*_.SCAN_PATTERN__V56*/ curfptr[54] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
    ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:298:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:298:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L14*/ curfnum[12] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:298:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:298:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatcomposite end recv";
       BASILYS_LOCATION("warmelt-normatch.bysl:298:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L14*/ curfnum[12];
       BASILYS_LOCATION("warmelt-normatch.bysl:298:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:298:/ apply.arg");
       argtab[3].bp_long = 298;
       /*_.DEBUG_MSG_FUN__V58*/ curfptr[57] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V57*/ curfptr[56] = /*_.DEBUG_MSG_FUN__V58*/ curfptr[57];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:298:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L14*/ curfnum[12] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:298:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V58*/ curfptr[57] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:298:/ cppif.else");
    /*_.IFCPP___V57*/ curfptr[56] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    /*_.MULTI___V44*/ curfptr[39] = /*_.IFCPP___V57*/ curfptr[56];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:286:/ clear");
    /*clear*/ /*_.IFCPP___V47*/ curfptr[42] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:286:/ clear");
    /*clear*/ /*_.IF___V49*/ curfptr[47] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:286:/ clear");
    /*clear*/ /*_.IFCPP___V51*/ curfptr[49] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:286:/ clear");
    /*clear*/ /*_.IFCPP___V57*/ curfptr[56] = 0 ;}
   ;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:286:/ clear");
   /*clear*/ /*_.BINDINS__V46*/ curfptr[41] = 0 ;}
  ;
  /*_.LET___V12*/ curfptr[10] = /*_.MULTI___V44*/ curfptr[39];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.LET___V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.SOPER__V18*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.SINS__V19*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.SOUTS__V20*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.LET___V21*/ curfptr[20] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.MATMB__V27*/ curfptr[25] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.MATIN__V28*/ curfptr[21] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.MATOUT__V29*/ curfptr[22] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.OPNAM__V30*/ curfptr[23] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.MATCTYP__V31*/ curfptr[30] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.ENV__V32*/ curfptr[31] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.NCX__V33*/ curfptr[32] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.BINDLIST__V34*/ curfptr[33] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.IFCPP___V35*/ curfptr[34] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.IFCPP___V37*/ curfptr[35] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_#__L9*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.IFELSE___V39*/ curfptr[37] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
  /*clear*/ /*_.MULTI___V44*/ curfptr[39] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:256:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATCOMPOSITE", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_10_SCANPAT_SRCPATCOMPOSITE*/





static basilys_ptr_t
rout_11_NORMPAT_ANYRECV(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_11_NORMPAT_ANYRECV_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 17
  void* varptr[17];
#define CURFRAM_NBVARNUM 2
  long varnum[2];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<17; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 17;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("NORMPAT_ANYRECV", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.NMATCH__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.NMATCH__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.SPATLIST__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.TESTPTLIST__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4])) != NULL);
 
 
 /*getarg#4*/
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ getarg");
 if (xargdescr_[3] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V6*/ curfptr[5] = (xargtab_[3].bp_aptr) ? (*(xargtab_[3].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:366:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:367:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:367:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:367:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:367:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_anyrecv recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:367:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:367:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:367:/ apply.arg");
    argtab[3].bp_long = 367;
    /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V7*/ curfptr[6] = /*_.DEBUG_MSG_FUN__V8*/ curfptr[7];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:367:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:367:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:367:/ cppif.else");
 /*_.IFCPP___V7*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:368:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:368:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:368:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:368:/ cond.then");
   /*_.IFELSE___V10*/ curfptr[9] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:368:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:368:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(368)?(368):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V10*/ curfptr[9] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V9*/ curfptr[7] = /*_.IFELSE___V10*/ curfptr[9];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:368:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:368:/ clear");
   /*clear*/ /*_.IFELSE___V10*/ curfptr[9] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:368:/ cppif.else");
 /*_.IFCPP___V9*/ curfptr[7] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:369:/ block");
 /*block*/{
  /*_.MYCLASS__V12*/ curfptr[11] = 
   (basilys_discr((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1])));;
  BASILYS_LOCATION("warmelt-normatch.bysl:370:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MYCLASS__V12*/ curfptr[11]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[2])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:370:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:370:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MYCLASS__V12*/ curfptr[11]), 1, "NAMED_NAME");/*_.MYCLASSNAME__V13*/ curfptr[12] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:370:/ cond.else");
  
   /*_.MYCLASSNAME__V13*/ curfptr[12] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:371:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:371:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:371:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), 1, "PCTN_SRC");/*_.PCTN_SRC__V14*/ curfptr[13] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:371:/ cond.else");
  
   /*_.PCTN_SRC__V14*/ curfptr[13] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:371:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCTN_SRC__V14*/ curfptr[13]), (basilys_ptr_t)((/*!CLASS_SRC*/ curfrout->tabval[3])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:371:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:371:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCTN_SRC__V14*/ curfptr[13]), 0, "SRC_LOC");/*_.PSLOC__V15*/ curfptr[14] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:371:/ cond.else");
  
   /*_.PSLOC__V15*/ curfptr[14] = NULL;;
   }
  ;
  
  basilys_error_str((basilys_ptr_t)(/*_.PSLOC__V15*/ curfptr[14]), ( "unimplemented normal_pattern for any "), (basilys_ptr_t)(/*_.MYCLASSNAME__V13*/ curfptr[12]));
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:375:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:375:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:375:/ cond");
    /*cond*/ if ((/*nil*/NULL)) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:375:/ cond.then");
    /*_.IFELSE___V17*/ curfptr[16] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:375:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:375:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "catchall normal_pattern unimplemented"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(375)?(375):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V16*/ curfptr[15] = /*_.IFELSE___V17*/ curfptr[16];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:375:/ clear");
    /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:375:/ cppif.else");
  /*_.IFCPP___V16*/ curfptr[15] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V11*/ curfptr[9] = /*_.IFCPP___V16*/ curfptr[15];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:369:/ clear");
  /*clear*/ /*_.MYCLASS__V12*/ curfptr[11] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:369:/ clear");
  /*clear*/ /*_.MYCLASSNAME__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:369:/ clear");
  /*clear*/ /*_.PCTN_SRC__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:369:/ clear");
  /*clear*/ /*_.PSLOC__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:369:/ clear");
  /*clear*/ /*_.IFCPP___V16*/ curfptr[15] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V11*/ curfptr[9];;
  BASILYS_LOCATION("warmelt-normatch.bysl:366:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ clear");
 /*clear*/ /*_.IFCPP___V7*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ clear");
 /*clear*/ /*_.IFCPP___V9*/ curfptr[7] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ clear");
 /*clear*/ /*_.LET___V11*/ curfptr[9] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("NORMPAT_ANYRECV", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_11_NORMPAT_ANYRECV*/





static basilys_ptr_t
rout_12_NORMPAT_ANYPAT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_12_NORMPAT_ANYPAT_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 19
  void* varptr[19];
#define CURFRAM_NBVARNUM 3
  long varnum[3];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<19; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 19;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("NORMPAT_ANYPAT", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.NMATCH__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.NMATCH__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.SPATLIST__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.TESTPTLIST__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4])) != NULL);
 
 
 /*getarg#4*/
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ getarg");
 if (xargdescr_[3] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V6*/ curfptr[5] = (xargtab_[3].bp_aptr) ? (*(xargtab_[3].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:382:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:383:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:383:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:383:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:383:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_anyrecv recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:383:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:383:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:383:/ apply.arg");
    argtab[3].bp_long = 383;
    /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V7*/ curfptr[6] = /*_.DEBUG_MSG_FUN__V8*/ curfptr[7];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:383:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:383:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:383:/ cppif.else");
 /*_.IFCPP___V7*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:384:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:384:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:384:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:384:/ cond.then");
   /*_.IFELSE___V10*/ curfptr[9] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:384:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:384:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(384)?(384):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V10*/ curfptr[9] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V9*/ curfptr[7] = /*_.IFELSE___V10*/ curfptr[9];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:384:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:384:/ clear");
   /*clear*/ /*_.IFELSE___V10*/ curfptr[9] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:384:/ cppif.else");
 /*_.IFCPP___V9*/ curfptr[7] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:385:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:385:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_ANY*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:385:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:385:/ cond.then");
   /*_.IFELSE___V12*/ curfptr[11] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:385:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:385:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(385)?(385):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V12*/ curfptr[11] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V11*/ curfptr[9] = /*_.IFELSE___V12*/ curfptr[11];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:385:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:385:/ clear");
   /*clear*/ /*_.IFELSE___V12*/ curfptr[11] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:385:/ cppif.else");
 /*_.IFCPP___V11*/ curfptr[9] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:386:/ block");
 /*block*/{
  /*_.MYCLASS__V14*/ curfptr[13] = 
   (basilys_discr((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1])));;
  BASILYS_LOCATION("warmelt-normatch.bysl:387:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MYCLASS__V14*/ curfptr[13]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[3])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:387:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:387:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MYCLASS__V14*/ curfptr[13]), 1, "NAMED_NAME");/*_.MYCLASSNAME__V15*/ curfptr[14] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:387:/ cond.else");
  
   /*_.MYCLASSNAME__V15*/ curfptr[14] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:388:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:388:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:388:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), 1, "PCTN_SRC");/*_.PCTN_SRC__V16*/ curfptr[15] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:388:/ cond.else");
  
   /*_.PCTN_SRC__V16*/ curfptr[15] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:388:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCTN_SRC__V16*/ curfptr[15]), (basilys_ptr_t)((/*!CLASS_SRC*/ curfrout->tabval[4])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:388:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:388:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCTN_SRC__V16*/ curfptr[15]), 0, "SRC_LOC");/*_.PSLOC__V17*/ curfptr[16] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:388:/ cond.else");
  
   /*_.PSLOC__V17*/ curfptr[16] = NULL;;
   }
  ;
  
  basilys_error_str((basilys_ptr_t)(/*_.PSLOC__V17*/ curfptr[16]), ( "unimplemented NORMAL_PATTERN for pattern of "), (basilys_ptr_t)(/*_.MYCLASSNAME__V15*/ curfptr[14]));
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:392:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:392:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:392:/ cond");
    /*cond*/ if ((/*nil*/NULL)) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:392:/ cond.then");
    /*_.IFELSE___V19*/ curfptr[18] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:392:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:392:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "catchall normal_pattern unimplemented on pattern"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(392)?(392):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V18*/ curfptr[17] = /*_.IFELSE___V19*/ curfptr[18];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:392:/ clear");
    /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:392:/ cppif.else");
  /*_.IFCPP___V18*/ curfptr[17] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V13*/ curfptr[11] = /*_.IFCPP___V18*/ curfptr[17];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:386:/ clear");
  /*clear*/ /*_.MYCLASS__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:386:/ clear");
  /*clear*/ /*_.MYCLASSNAME__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:386:/ clear");
  /*clear*/ /*_.PCTN_SRC__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:386:/ clear");
  /*clear*/ /*_.PSLOC__V17*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:386:/ clear");
  /*clear*/ /*_.IFCPP___V18*/ curfptr[17] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V13*/ curfptr[11];;
  BASILYS_LOCATION("warmelt-normatch.bysl:382:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ clear");
 /*clear*/ /*_.IFCPP___V7*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ clear");
 /*clear*/ /*_.IFCPP___V9*/ curfptr[7] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ clear");
 /*clear*/ /*_.IFCPP___V11*/ curfptr[9] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ clear");
 /*clear*/ /*_.LET___V13*/ curfptr[11] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("NORMPAT_ANYPAT", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_12_NORMPAT_ANYPAT*/





static basilys_ptr_t
rout_13_NORMPAT_INSTANCEPAT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_13_NORMPAT_INSTANCEPAT_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 19
  void* varptr[19];
#define CURFRAM_NBVARNUM 4
  long varnum[4];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<19; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 19;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("NORMPAT_INSTANCEPAT", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.NMATCH__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.NMATCH__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.SPATLIST__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.TESTPTLIST__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4])) != NULL);
 
 
 /*getarg#4*/
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ getarg");
 if (xargdescr_[3] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V6*/ curfptr[5] = (xargtab_[3].bp_aptr) ? (*(xargtab_[3].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:400:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:401:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:401:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:401:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:401:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_instancepat recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:401:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:401:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:401:/ apply.arg");
    argtab[3].bp_long = 401;
    /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V7*/ curfptr[6] = /*_.DEBUG_MSG_FUN__V8*/ curfptr[7];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:401:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:401:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:401:/ cppif.else");
 /*_.IFCPP___V7*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:402:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:402:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L2*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:402:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:402:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_instancepat nmatch";
    BASILYS_LOCATION("warmelt-normatch.bysl:402:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L2*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:402:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:402:/ apply.arg");
    argtab[3].bp_long = 402;
    /*_.DEBUG_MSG_FUN__V10*/ curfptr[9] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.NMATCH__V3*/ curfptr[2]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V9*/ curfptr[7] = /*_.DEBUG_MSG_FUN__V10*/ curfptr[9];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:402:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:402:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V10*/ curfptr[9] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:402:/ cppif.else");
 /*_.IFCPP___V9*/ curfptr[7] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:403:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:403:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:403:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:403:/ cond.then");
   /*_.IFELSE___V12*/ curfptr[11] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:403:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:403:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(403)?(403):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V12*/ curfptr[11] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V11*/ curfptr[9] = /*_.IFELSE___V12*/ curfptr[11];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:403:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:403:/ clear");
   /*clear*/ /*_.IFELSE___V12*/ curfptr[11] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:403:/ cppif.else");
 /*_.IFCPP___V11*/ curfptr[9] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:404:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:404:/ block");
  /*block*/{
   /*_#IS_A__L4*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_INSTANCE*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:404:/ cond");
   /*cond*/ if (/*_#IS_A__L4*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:404:/ cond.then");
   /*_.IFELSE___V14*/ curfptr[13] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:404:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:404:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(404)?(404):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V14*/ curfptr[13] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V13*/ curfptr[11] = /*_.IFELSE___V14*/ curfptr[13];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:404:/ clear");
   /*clear*/ /*_#IS_A__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:404:/ clear");
   /*clear*/ /*_.IFELSE___V14*/ curfptr[13] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:404:/ cppif.else");
 /*_.IFCPP___V13*/ curfptr[11] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:405:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), 1, "PCTN_SRC");/*_.PCTN_SRC__V16*/ curfptr[15] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:405:/ cond.else");
  
   /*_.PCTN_SRC__V16*/ curfptr[15] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCTN_SRC__V16*/ curfptr[15]), (basilys_ptr_t)((/*!CLASS_SRC*/ curfrout->tabval[3])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCTN_SRC__V16*/ curfptr[15]), 0, "SRC_LOC");/*_.PSLOC__V17*/ curfptr[16] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:405:/ cond.else");
  
   /*_.PSLOC__V17*/ curfptr[16] = NULL;;
   }
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:407:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:407:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:407:/ cond");
    /*cond*/ if ((/*nil*/NULL)) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:407:/ cond.then");
    /*_.IFELSE___V19*/ curfptr[18] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:407:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:407:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "unimplemented normpat_instancepat"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(407)?(407):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V18*/ curfptr[17] = /*_.IFELSE___V19*/ curfptr[18];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:407:/ clear");
    /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:407:/ cppif.else");
  /*_.IFCPP___V18*/ curfptr[17] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V15*/ curfptr[13] = /*_.IFCPP___V18*/ curfptr[17];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ clear");
  /*clear*/ /*_.PCTN_SRC__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ clear");
  /*clear*/ /*_.PSLOC__V17*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:405:/ clear");
  /*clear*/ /*_.IFCPP___V18*/ curfptr[17] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V15*/ curfptr[13];;
  BASILYS_LOCATION("warmelt-normatch.bysl:400:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ clear");
 /*clear*/ /*_.IFCPP___V7*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ clear");
 /*clear*/ /*_.IFCPP___V9*/ curfptr[7] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ clear");
 /*clear*/ /*_.IFCPP___V11*/ curfptr[9] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ clear");
 /*clear*/ /*_.IFCPP___V13*/ curfptr[11] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ clear");
 /*clear*/ /*_.LET___V15*/ curfptr[13] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("NORMPAT_INSTANCEPAT", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_13_NORMPAT_INSTANCEPAT*/





static basilys_ptr_t
rout_14_NORMPAT_ANYMATCHPAT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_14_NORMPAT_ANYMATCHPAT_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 118
  void* varptr[118];
#define CURFRAM_NBVARNUM 40
  long varnum[40];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<118; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 118;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("NORMPAT_ANYMATCHPAT", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.NMATCH__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.NMATCH__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.SPATLIST__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.TESTPTLIST__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4])) != NULL);
 
 
 /*getarg#4*/
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ getarg");
 if (xargdescr_[3] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V6*/ curfptr[5] = (xargtab_[3].bp_aptr) ? (*(xargtab_[3].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:415:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:416:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:416:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:416:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:416:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_anymatchpat recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:416:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:416:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:416:/ apply.arg");
    argtab[3].bp_long = 416;
    /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V7*/ curfptr[6] = /*_.DEBUG_MSG_FUN__V8*/ curfptr[7];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:416:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:416:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V8*/ curfptr[7] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:416:/ cppif.else");
 /*_.IFCPP___V7*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:417:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:417:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L2*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:417:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:417:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_anymatchpat nmatch";
    BASILYS_LOCATION("warmelt-normatch.bysl:417:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L2*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:417:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:417:/ apply.arg");
    argtab[3].bp_long = 417;
    /*_.DEBUG_MSG_FUN__V10*/ curfptr[9] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.NMATCH__V3*/ curfptr[2]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V9*/ curfptr[7] = /*_.DEBUG_MSG_FUN__V10*/ curfptr[9];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:417:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:417:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V10*/ curfptr[9] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:417:/ cppif.else");
 /*_.IFCPP___V9*/ curfptr[7] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:418:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:418:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L3*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:418:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:418:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_anymatchpat spatlist";
    BASILYS_LOCATION("warmelt-normatch.bysl:418:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L3*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:418:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:418:/ apply.arg");
    argtab[3].bp_long = 418;
    /*_.DEBUG_MSG_FUN__V12*/ curfptr[11] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V11*/ curfptr[9] = /*_.DEBUG_MSG_FUN__V12*/ curfptr[11];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:418:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:418:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V12*/ curfptr[11] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:418:/ cppif.else");
 /*_.IFCPP___V11*/ curfptr[9] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:419:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:419:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:419:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:419:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_anymatchpat testptlist";
    BASILYS_LOCATION("warmelt-normatch.bysl:419:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L4*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:419:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:419:/ apply.arg");
    argtab[3].bp_long = 419;
    /*_.DEBUG_MSG_FUN__V14*/ curfptr[13] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V13*/ curfptr[11] = /*_.DEBUG_MSG_FUN__V14*/ curfptr[13];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:419:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:419:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V14*/ curfptr[13] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:419:/ cppif.else");
 /*_.IFCPP___V13*/ curfptr[11] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:420:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:420:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:420:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:420:/ apply.arg");
    argtab[0].bp_cstring =  "normpat_anymatchpat pcn";
    BASILYS_LOCATION("warmelt-normatch.bysl:420:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:420:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:420:/ apply.arg");
    argtab[3].bp_long = 420;
    /*_.DEBUG_MSG_FUN__V16*/ curfptr[15] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V15*/ curfptr[13] = /*_.DEBUG_MSG_FUN__V16*/ curfptr[15];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:420:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:420:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V16*/ curfptr[15] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:420:/ cppif.else");
 /*_.IFCPP___V15*/ curfptr[13] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:421:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:421:/ block");
  /*block*/{
   /*_#IS_A__L6*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:421:/ cond");
   /*cond*/ if (/*_#IS_A__L6*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:421:/ cond.then");
   /*_.IFELSE___V18*/ curfptr[17] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:421:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:421:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(421)?(421):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V18*/ curfptr[17] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V17*/ curfptr[15] = /*_.IFELSE___V18*/ curfptr[17];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:421:/ clear");
   /*clear*/ /*_#IS_A__L6*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:421:/ clear");
   /*clear*/ /*_.IFELSE___V18*/ curfptr[17] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:421:/ cppif.else");
 /*_.IFCPP___V17*/ curfptr[15] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:422:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:422:/ block");
  /*block*/{
   /*_#IS_A__L7*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_ANYMATCH*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:422:/ cond");
   /*cond*/ if (/*_#IS_A__L7*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:422:/ cond.then");
   /*_.IFELSE___V20*/ curfptr[19] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:422:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:422:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(422)?(422):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V20*/ curfptr[19] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V19*/ curfptr[17] = /*_.IFELSE___V20*/ curfptr[19];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:422:/ clear");
   /*clear*/ /*_#IS_A__L7*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:422:/ clear");
   /*clear*/ /*_.IFELSE___V20*/ curfptr[19] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:422:/ cppif.else");
 /*_.IFCPP___V19*/ curfptr[17] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:423:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:424:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRC*/ curfrout->tabval[3])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:424:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:424:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V22*/ curfptr[21] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:424:/ cond.else");
  
   /*_.SLOC__V22*/ curfptr[21] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:425:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_COMPOSITE*/ curfrout->tabval[4])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:425:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:425:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 3, "SPAC_OPERATOR");/*_.MAT__V23*/ curfptr[22] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:425:/ cond.else");
  
   /*_.MAT__V23*/ curfptr[22] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:426:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_COMPOSITE*/ curfrout->tabval[4])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:426:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:426:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 4, "SPAC_INARGS");/*_.INARGS__V24*/ curfptr[23] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:426:/ cond.else");
  
   /*_.INARGS__V24*/ curfptr[23] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:427:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_COMPOSITE*/ curfrout->tabval[4])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:427:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:427:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 5, "SPAC_OUTARGS");/*_.OUTARGS__V25*/ curfptr[24] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:427:/ cond.else");
  
   /*_.OUTARGS__V25*/ curfptr[24] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:428:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), 2, "PCTN_ENV");/*_.ENV__V26*/ curfptr[25] = slot; };
  ;
  /*_.OLDENV__V27*/ curfptr[26] = /*_.ENV__V26*/ curfptr[25];;
  BASILYS_LOCATION("warmelt-normatch.bysl:430:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), 0, "PCTN_NORMCTXT");/*_.NCX__V28*/ curfptr[27] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:431:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), 5, "PCTN_BINDLIST");/*_.BINDLIST__V29*/ curfptr[28] = slot; };
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:433:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:433:/ block");
   /*block*/{
    /*_#IS_A__L8*/ curfnum[0] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (basilys_ptr_t)((/*!CLASS_ANY_MATCHER*/ curfrout->tabval[5])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:433:/ cond");
    /*cond*/ if (/*_#IS_A__L8*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:433:/ cond.then");
    /*_.IFELSE___V31*/ curfptr[30] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:433:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:433:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check mat"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(433)?(433):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V31*/ curfptr[30] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V30*/ curfptr[29] = /*_.IFELSE___V31*/ curfptr[30];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:433:/ clear");
    /*clear*/ /*_#IS_A__L8*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:433:/ clear");
    /*clear*/ /*_.IFELSE___V31*/ curfptr[30] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:433:/ cppif.else");
  /*_.IFCPP___V30*/ curfptr[29] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:434:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:434:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L9*/ curfnum[0] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:434:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:434:/ apply.arg");
     argtab[0].bp_cstring =  "normpat_anymatchpat inargs";
     BASILYS_LOCATION("warmelt-normatch.bysl:434:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L9*/ curfnum[0];
     BASILYS_LOCATION("warmelt-normatch.bysl:434:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:434:/ apply.arg");
     argtab[3].bp_long = 434;
     /*_.DEBUG_MSG_FUN__V33*/ curfptr[32] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.INARGS__V24*/ curfptr[23]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V32*/ curfptr[30] = /*_.DEBUG_MSG_FUN__V33*/ curfptr[32];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:434:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L9*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:434:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V33*/ curfptr[32] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:434:/ cppif.else");
  /*_.IFCPP___V32*/ curfptr[30] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:435:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:435:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L10*/ curfnum[0] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:435:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:435:/ apply.arg");
     argtab[0].bp_cstring =  "normpat_anymatchpat mat";
     BASILYS_LOCATION("warmelt-normatch.bysl:435:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L10*/ curfnum[0];
     BASILYS_LOCATION("warmelt-normatch.bysl:435:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:435:/ apply.arg");
     argtab[3].bp_long = 435;
     /*_.DEBUG_MSG_FUN__V35*/ curfptr[34] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V34*/ curfptr[32] = /*_.DEBUG_MSG_FUN__V35*/ curfptr[34];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:435:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L10*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:435:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V35*/ curfptr[34] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:435:/ cppif.else");
  /*_.IFCPP___V34*/ curfptr[32] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  BASILYS_LOCATION("warmelt-normatch.bysl:436:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:436:/ multiapply");
   /*multiapply 4args, 1x.res*/ {
    union basilysparam_un argtab[3];
    
    union basilysparam_un restab[1];
    memset(&restab, 0, sizeof(restab));
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:436:/ multiapply.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V26*/ curfptr[25];BASILYS_LOCATION("warmelt-normatch.bysl:436:/ multiapply.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V28*/ curfptr[27];BASILYS_LOCATION("warmelt-normatch.bysl:436:/ multiapply.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V22*/ curfptr[21];
    BASILYS_LOCATION("warmelt-normatch.bysl:436:/ multiapply.xres");
    restab[0].bp_aptr = (basilys_ptr_t*) &/*_.BINDINS__V38*/ curfptr[37];BASILYS_LOCATION("warmelt-normatch.bysl:436:/ multiapply.appl");
    /*_.NINS__V37*/ curfptr[36] =  basilys_apply ((basilysclosure_ptr_t)((/*!NORMALIZE_TUPLE*/ curfrout->tabval[6])), (basilys_ptr_t)(/*_.INARGS__V24*/ curfptr[23]), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
    }
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:436:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:439:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:439:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L11*/ curfnum[0] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:439:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:439:/ apply.arg");
       argtab[0].bp_cstring =  "normpat_anymatchpat nins";
       BASILYS_LOCATION("warmelt-normatch.bysl:439:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L11*/ curfnum[0];
       BASILYS_LOCATION("warmelt-normatch.bysl:439:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:439:/ apply.arg");
       argtab[3].bp_long = 439;
       /*_.DEBUG_MSG_FUN__V40*/ curfptr[39] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.NINS__V37*/ curfptr[36]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V39*/ curfptr[38] = /*_.DEBUG_MSG_FUN__V40*/ curfptr[39];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:439:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L11*/ curfnum[0] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:439:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V40*/ curfptr[39] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:439:/ cppif.else");
    /*_.IFCPP___V39*/ curfptr[38] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:440:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:440:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L12*/ curfnum[0] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:440:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:440:/ apply.arg");
       argtab[0].bp_cstring =  "normpat_anymatchpat bindins";
       BASILYS_LOCATION("warmelt-normatch.bysl:440:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L12*/ curfnum[0];
       BASILYS_LOCATION("warmelt-normatch.bysl:440:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:440:/ apply.arg");
       argtab[3].bp_long = 440;
       /*_.DEBUG_MSG_FUN__V42*/ curfptr[41] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.BINDINS__V38*/ curfptr[37]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V41*/ curfptr[39] = /*_.DEBUG_MSG_FUN__V42*/ curfptr[41];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:440:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L12*/ curfnum[0] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:440:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V42*/ curfptr[41] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:440:/ cppif.else");
    /*_.IFCPP___V41*/ curfptr[39] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:441:/ cond");
    /*cond*/ if (/*_.BINDINS__V38*/ curfptr[37]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:441:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:441:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:441:/ apply");
      /*apply*/{
       union basilysparam_un argtab[1];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:441:/ apply.arg");
       argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.BINDINS__V38*/ curfptr[37];
       /*_.LIST_APPEND2LIST__V44*/ curfptr[43] =  basilys_apply ((basilysclosure_ptr_t)((/*!LIST_APPEND2LIST*/ curfrout->tabval[7])), (basilys_ptr_t)(/*_.BINDLIST__V29*/ curfptr[28]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IF___V43*/ curfptr[41] = /*_.LIST_APPEND2LIST__V44*/ curfptr[43];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:441:/ clear");
      /*clear*/ /*_.LIST_APPEND2LIST__V44*/ curfptr[43] = 0 ;}
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:441:/ cond.else");
    
     /*_.IF___V43*/ curfptr[41] = NULL;;
     }
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:442:/ block");
    /*block*/{
     BASILYS_LOCATION("warmelt-normatch.bysl:443:/ cond");
     /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (basilys_ptr_t)((/*!CLASS_ANY_MATCHER*/ curfrout->tabval[5])))) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:443:/ cond.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:443:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), 2, "AMATCH_IN");/*_.INMATCHS__V46*/ curfptr[45] = slot; };
      ;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:443:/ cond.else");
     
      /*_.INMATCHS__V46*/ curfptr[45] = NULL;;
      }
     ;
     BASILYS_LOCATION("warmelt-normatch.bysl:444:/ cond");
     /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (basilys_ptr_t)((/*!CLASS_ANY_MATCHER*/ curfrout->tabval[5])))) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:444:/ cond.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:444:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), 4, "AMATCH_OUT");/*_.OUTMATCHS__V47*/ curfptr[46] = slot; };
      ;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:444:/ cond.else");
     
      /*_.OUTMATCHS__V47*/ curfptr[46] = NULL;;
      }
     ;
     BASILYS_LOCATION("warmelt-normatch.bysl:445:/ cond");
     /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (basilys_ptr_t)((/*!CLASS_ANY_MATCHER*/ curfrout->tabval[5])))) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:445:/ cond.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:445:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), 3, "AMATCH_MATCHBIND");/*_.MATBIND__V48*/ curfptr[47] = slot; };
      ;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:445:/ cond.else");
     
      /*_.MATBIND__V48*/ curfptr[47] = NULL;;
      }
     ;
     BASILYS_LOCATION("warmelt-normatch.bysl:448:/ cond");
     /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MATBIND__V48*/ curfptr[47]), (basilys_ptr_t)((/*!CLASS_FORMAL_BINDING*/ curfrout->tabval[9])))) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:448:/ cond.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:448:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MATBIND__V48*/ curfptr[47]), 1, "FBIND_TYPE");/*_.FBIND_TYPE__V49*/ curfptr[48] = slot; };
      ;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:448:/ cond.else");
     
      /*_.FBIND_TYPE__V49*/ curfptr[48] = NULL;;
      }
     ;
     BASILYS_LOCATION("warmelt-normatch.bysl:446:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:446:/ rawallocobj");
      /*rawallocobj*/ { basilys_ptr_t newobj = 0;
       basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_NREP_TESTP_MATCHER*/ curfrout->tabval[8])), (7), "CLASS_NREP_TESTP_MATCHER");/*_.INST__V51*/ curfptr[50] =
       newobj; };
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:446:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (0), (/*_.SLOC__V22*/ curfptr[21]), "NREP_LOC");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:446:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (1), (/*_.FBIND_TYPE__V49*/ curfptr[48]), "NEXPR_CTYP");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:446:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (4), (/*_.NMATCH__V3*/ curfptr[2]), "NTSP_TEST");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:446:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (3>=0 && 3< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (3), ((/*nil*/NULL)), "NTSP_SUCCP");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:446:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (5), (/*_.MAT__V23*/ curfptr[22]), "NTSM_MATCHER");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:446:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (6), (/*_.BINDINS__V38*/ curfptr[37]), "NTSM_BINDS");
      ;
      /*_.CURTESTP__V50*/ curfptr[49] = /*_.INST__V51*/ curfptr[50];;}
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:456:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:456:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L13*/ curfnum[0] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:456:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:456:/ apply.arg");
        argtab[0].bp_cstring =  "normpat_anymatchpat inargs";
        BASILYS_LOCATION("warmelt-normatch.bysl:456:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L13*/ curfnum[0];
        BASILYS_LOCATION("warmelt-normatch.bysl:456:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:456:/ apply.arg");
        argtab[3].bp_long = 456;
        /*_.DEBUG_MSG_FUN__V53*/ curfptr[52] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.INARGS__V24*/ curfptr[23]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V52*/ curfptr[51] = /*_.DEBUG_MSG_FUN__V53*/ curfptr[52];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:456:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L13*/ curfnum[0] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:456:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V53*/ curfptr[52] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:456:/ cppif.else");
     /*_.IFCPP___V52*/ curfptr[51] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:457:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:457:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L14*/ curfnum[0] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:457:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:457:/ apply.arg");
        argtab[0].bp_cstring =  "normpat_anymatchpat inmatchs";
        BASILYS_LOCATION("warmelt-normatch.bysl:457:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L14*/ curfnum[0];
        BASILYS_LOCATION("warmelt-normatch.bysl:457:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:457:/ apply.arg");
        argtab[3].bp_long = 457;
        /*_.DEBUG_MSG_FUN__V55*/ curfptr[54] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.INMATCHS__V46*/ curfptr[45]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V54*/ curfptr[52] = /*_.DEBUG_MSG_FUN__V55*/ curfptr[54];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:457:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L14*/ curfnum[0] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:457:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V55*/ curfptr[54] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:457:/ cppif.else");
     /*_.IFCPP___V54*/ curfptr[52] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     BASILYS_LOCATION("warmelt-normatch.bysl:458:/ block");
     /*block*/{
      /*_#NBINARGS__L15*/ curfnum[0] = 
       (basilys_multiple_length((basilys_ptr_t)(/*_.INARGS__V24*/ curfptr[23])));;
      /*_#MULTIPLE_LENGTH__L16*/ curfnum[15] = 
       (basilys_multiple_length((basilys_ptr_t)(/*_.INMATCHS__V46*/ curfptr[45])));;
      /*_#I__L17*/ curfnum[16] = 
       ((/*_#NBINARGS__L15*/ curfnum[0]) != (/*_#MULTIPLE_LENGTH__L16*/ curfnum[15]));;
      BASILYS_LOCATION("warmelt-normatch.bysl:459:/ cond");
      /*cond*/ if (/*_#I__L17*/ curfnum[16]) /*then*/ {
      BASILYS_LOCATION("warmelt-normatch.bysl:459:/ cond.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:459:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:463:/ cond");
        /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[10])))) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:463:/ cond.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:463:/ getslot");
         { basilys_ptr_t slot=0;
          basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), 1, "NAMED_NAME");/*_.NAMED_NAME__V56*/ curfptr[54] = slot; };
         ;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:463:/ cond.else");
        
         /*_.NAMED_NAME__V56*/ curfptr[54] = NULL;;
         }
        ;
        
        basilys_error_str((basilys_ptr_t)(/*_.SLOC__V22*/ curfptr[21]), ( "formal and actual number of input matching arguments differ for anymatch"), (basilys_ptr_t)(/*_.NAMED_NAME__V56*/ curfptr[54]));
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:459:/ clear");
        /*clear*/ /*_.NAMED_NAME__V56*/ curfptr[54] = 0 ;}
       ;
      } /*noelse*/
      ;
      /*_#I__L18*/ curfnum[17] = 
       ((/*_#NBINARGS__L15*/ curfnum[0]) > (0));;
      BASILYS_LOCATION("warmelt-normatch.bysl:464:/ cond");
      /*cond*/ if (/*_#I__L18*/ curfnum[17]) /*then*/ {
      BASILYS_LOCATION("warmelt-normatch.bysl:464:/ cond.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:464:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:465:/ block");
        /*block*/{
         BASILYS_LOCATION("warmelt-normatch.bysl:465:/ apply");
         /*apply*/{
          /*_.NEWENV__V57*/ curfptr[54] =  basilys_apply ((basilysclosure_ptr_t)((/*!FRESH_ENV*/ curfrout->tabval[11])), (basilys_ptr_t)(/*_.ENV__V26*/ curfptr[25]), (""), (union basilysparam_un*)0, "", (union basilysparam_un*)0);
          }
         ;
         /*citerblock FOREACH_IN_MULTIPLE*/ {
         long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.NINS__V37*/ curfptr[36]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.NCURIN__V58*/ curfptr[57] = basilys_multiple_nth((basilys_ptr_t)(/*_.NINS__V37*/ curfptr[36]), cit1__EACHTUP_ix);
/*_#IX__L19*/ curfnum[18] = cit1__EACHTUP_ix;

         
          
          #if ENABLE_CHECKING
           BASILYS_LOCATION("warmelt-normatch.bysl:469:/ cppif.then");
          BASILYS_LOCATION("warmelt-normatch.bysl:469:/ block");
           /*block*/{
            /*_#THE_CALLCOUNT__L20*/ curfnum[19] = 
             callcount;;
            BASILYS_LOCATION("warmelt-normatch.bysl:469:/ apply");
            /*apply*/{
             union basilysparam_un argtab[4];
             memset(&argtab, 0, sizeof(argtab));
             BASILYS_LOCATION("warmelt-normatch.bysl:469:/ apply.arg");
             argtab[0].bp_cstring =  "normpat_anymatchpat ncurin";
             BASILYS_LOCATION("warmelt-normatch.bysl:469:/ apply.arg");
             argtab[1].bp_long = /*_#THE_CALLCOUNT__L20*/ curfnum[19];
             BASILYS_LOCATION("warmelt-normatch.bysl:469:/ apply.arg");
             argtab[2].bp_cstring =  "warmelt-normatch.bysl";
             BASILYS_LOCATION("warmelt-normatch.bysl:469:/ apply.arg");
             argtab[3].bp_long = 469;
             /*_.DEBUG_MSG_FUN__V60*/ curfptr[59] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.NCURIN__V58*/ curfptr[57]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
             }
            ;
            /*epilog*/
            BASILYS_LOCATION("warmelt-normatch.bysl:469:/ clear");
            /*clear*/ /*_#THE_CALLCOUNT__L20*/ curfnum[19] = 0 ;
            BASILYS_LOCATION("warmelt-normatch.bysl:469:/ clear");
            /*clear*/ /*_.DEBUG_MSG_FUN__V60*/ curfptr[59] = 0 ;}
           
           #else /*ENABLE_CHECKING*/
           BASILYS_LOCATION("warmelt-normatch.bysl:469:/ cppif.else");
          (/*nil*/NULL)
           #endif /*ENABLE_CHECKING*/
           ;
          BASILYS_LOCATION("warmelt-normatch.bysl:470:/ block");
          /*block*/{
           /*_.CURMATCH__V62*/ curfptr[61] = 
            (basilys_multiple_nth((basilys_ptr_t)(/*_.INMATCHS__V46*/ curfptr[45]), (/*_#IX__L19*/ curfnum[18])));;
           BASILYS_LOCATION("warmelt-normatch.bysl:471:/ msend");
           /*msend*/{
            union basilysparam_un argtab[1];
            memset(&argtab, 0, sizeof(argtab));
            BASILYS_LOCATION("warmelt-normatch.bysl:471:/ ojbmsend.arg");
            argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V26*/ curfptr[25];
            /*_.CURCTYPE__V63*/ curfptr[62] = basilysgc_send((basilys_ptr_t)(/*_.NCURIN__V58*/ curfptr[57]), (basilys_ptr_t)((/*!GET_CTYPE*/ curfrout->tabval[12])), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
            }
           ;
           BASILYS_LOCATION("warmelt-normatch.bysl:472:/ cond");
           /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), (basilys_ptr_t)((/*!CLASS_FORMAL_BINDING*/ curfrout->tabval[9])))) /*then*/ {
           BASILYS_LOCATION("warmelt-normatch.bysl:472:/ cond.then");
           BASILYS_LOCATION("warmelt-normatch.bysl:472:/ getslot");
            { basilys_ptr_t slot=0;
             basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), 1, "FBIND_TYPE");/*_.MATCHTYPE__V64*/ curfptr[63] = slot; };
            ;
           } else {BASILYS_LOCATION("warmelt-normatch.bysl:472:/ cond.else");
           
            /*_.MATCHTYPE__V64*/ curfptr[63] = NULL;;
            }
           ;
           
           #if ENABLE_CHECKING
            BASILYS_LOCATION("warmelt-normatch.bysl:474:/ cppif.then");
           BASILYS_LOCATION("warmelt-normatch.bysl:474:/ block");
            /*block*/{
             /*_#THE_CALLCOUNT__L21*/ curfnum[19] = 
              callcount;;
             BASILYS_LOCATION("warmelt-normatch.bysl:474:/ apply");
             /*apply*/{
              union basilysparam_un argtab[4];
              memset(&argtab, 0, sizeof(argtab));
              BASILYS_LOCATION("warmelt-normatch.bysl:474:/ apply.arg");
              argtab[0].bp_cstring =  "normpat_anymatchpat curmatch";
              BASILYS_LOCATION("warmelt-normatch.bysl:474:/ apply.arg");
              argtab[1].bp_long = /*_#THE_CALLCOUNT__L21*/ curfnum[19];
              BASILYS_LOCATION("warmelt-normatch.bysl:474:/ apply.arg");
              argtab[2].bp_cstring =  "warmelt-normatch.bysl";
              BASILYS_LOCATION("warmelt-normatch.bysl:474:/ apply.arg");
              argtab[3].bp_long = 474;
              /*_.DEBUG_MSG_FUN__V66*/ curfptr[65] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
              }
             ;
             /*_.IFCPP___V65*/ curfptr[64] = /*_.DEBUG_MSG_FUN__V66*/ curfptr[65];;
             /*epilog*/
             BASILYS_LOCATION("warmelt-normatch.bysl:474:/ clear");
             /*clear*/ /*_#THE_CALLCOUNT__L21*/ curfnum[19] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:474:/ clear");
             /*clear*/ /*_.DEBUG_MSG_FUN__V66*/ curfptr[65] = 0 ;}
            
            #else /*ENABLE_CHECKING*/
            BASILYS_LOCATION("warmelt-normatch.bysl:474:/ cppif.else");
           /*_.IFCPP___V65*/ curfptr[64] = (/*nil*/NULL);
            #endif /*ENABLE_CHECKING*/
            ;
           
           #if ENABLE_CHECKING
            BASILYS_LOCATION("warmelt-normatch.bysl:475:/ cppif.then");
           BASILYS_LOCATION("warmelt-normatch.bysl:475:/ block");
            /*block*/{
             /*_#IS_A__L22*/ curfnum[19] = 
               basilys_is_instance_of((basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), (basilys_ptr_t)((/*!CLASS_FORMAL_BINDING*/ curfrout->tabval[9])));;
             BASILYS_LOCATION("warmelt-normatch.bysl:475:/ cond");
             /*cond*/ if (/*_#IS_A__L22*/ curfnum[19]) /*then*/ {
             BASILYS_LOCATION("warmelt-normatch.bysl:475:/ cond.then");
             /*_.IFELSE___V68*/ curfptr[67] = (/*nil*/NULL);;
             } else {BASILYS_LOCATION("warmelt-normatch.bysl:475:/ cond.else");
             
              BASILYS_LOCATION("warmelt-normatch.bysl:475:/ block");
              /*block*/{
               /*block*/{
                
                basilys_assert_failed(( "check curmatch"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(475)?(475):__LINE__, __FUNCTION__);
                /*clear*/ /*_.IFELSE___V68*/ curfptr[67] = 0 ;}
               ;
               /*epilog*/}
              ;
              }
             ;
             /*_.IFCPP___V67*/ curfptr[65] = /*_.IFELSE___V68*/ curfptr[67];;
             /*epilog*/
             BASILYS_LOCATION("warmelt-normatch.bysl:475:/ clear");
             /*clear*/ /*_#IS_A__L22*/ curfnum[19] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:475:/ clear");
             /*clear*/ /*_.IFELSE___V68*/ curfptr[67] = 0 ;}
            
            #else /*ENABLE_CHECKING*/
            BASILYS_LOCATION("warmelt-normatch.bysl:475:/ cppif.else");
           /*_.IFCPP___V67*/ curfptr[65] = (/*nil*/NULL);
            #endif /*ENABLE_CHECKING*/
            ;
           
           #if ENABLE_CHECKING
            BASILYS_LOCATION("warmelt-normatch.bysl:477:/ cppif.then");
           BASILYS_LOCATION("warmelt-normatch.bysl:477:/ block");
            /*block*/{
             /*_#THE_CALLCOUNT__L23*/ curfnum[19] = 
              callcount;;
             BASILYS_LOCATION("warmelt-normatch.bysl:477:/ apply");
             /*apply*/{
              union basilysparam_un argtab[4];
              memset(&argtab, 0, sizeof(argtab));
              BASILYS_LOCATION("warmelt-normatch.bysl:477:/ apply.arg");
              argtab[0].bp_cstring =  "normpat_anymatchpat curctype";
              BASILYS_LOCATION("warmelt-normatch.bysl:477:/ apply.arg");
              argtab[1].bp_long = /*_#THE_CALLCOUNT__L23*/ curfnum[19];
              BASILYS_LOCATION("warmelt-normatch.bysl:477:/ apply.arg");
              argtab[2].bp_cstring =  "warmelt-normatch.bysl";
              BASILYS_LOCATION("warmelt-normatch.bysl:477:/ apply.arg");
              argtab[3].bp_long = 477;
              /*_.DEBUG_MSG_FUN__V70*/ curfptr[69] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.CURCTYPE__V63*/ curfptr[62]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
              }
             ;
             /*_.IFCPP___V69*/ curfptr[67] = /*_.DEBUG_MSG_FUN__V70*/ curfptr[69];;
             /*epilog*/
             BASILYS_LOCATION("warmelt-normatch.bysl:477:/ clear");
             /*clear*/ /*_#THE_CALLCOUNT__L23*/ curfnum[19] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:477:/ clear");
             /*clear*/ /*_.DEBUG_MSG_FUN__V70*/ curfptr[69] = 0 ;}
            
            #else /*ENABLE_CHECKING*/
            BASILYS_LOCATION("warmelt-normatch.bysl:477:/ cppif.else");
           /*_.IFCPP___V69*/ curfptr[67] = (/*nil*/NULL);
            #endif /*ENABLE_CHECKING*/
            ;
           
           #if ENABLE_CHECKING
            BASILYS_LOCATION("warmelt-normatch.bysl:478:/ cppif.then");
           BASILYS_LOCATION("warmelt-normatch.bysl:478:/ block");
            /*block*/{
             /*_#THE_CALLCOUNT__L24*/ curfnum[19] = 
              callcount;;
             BASILYS_LOCATION("warmelt-normatch.bysl:478:/ apply");
             /*apply*/{
              union basilysparam_un argtab[4];
              memset(&argtab, 0, sizeof(argtab));
              BASILYS_LOCATION("warmelt-normatch.bysl:478:/ apply.arg");
              argtab[0].bp_cstring =  "normpat_anymatchpat matchtype";
              BASILYS_LOCATION("warmelt-normatch.bysl:478:/ apply.arg");
              argtab[1].bp_long = /*_#THE_CALLCOUNT__L24*/ curfnum[19];
              BASILYS_LOCATION("warmelt-normatch.bysl:478:/ apply.arg");
              argtab[2].bp_cstring =  "warmelt-normatch.bysl";
              BASILYS_LOCATION("warmelt-normatch.bysl:478:/ apply.arg");
              argtab[3].bp_long = 478;
              /*_.DEBUG_MSG_FUN__V72*/ curfptr[71] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.MATCHTYPE__V64*/ curfptr[63]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
              }
             ;
             /*_.IFCPP___V71*/ curfptr[69] = /*_.DEBUG_MSG_FUN__V72*/ curfptr[71];;
             /*epilog*/
             BASILYS_LOCATION("warmelt-normatch.bysl:478:/ clear");
             /*clear*/ /*_#THE_CALLCOUNT__L24*/ curfnum[19] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:478:/ clear");
             /*clear*/ /*_.DEBUG_MSG_FUN__V72*/ curfptr[71] = 0 ;}
            
            #else /*ENABLE_CHECKING*/
            BASILYS_LOCATION("warmelt-normatch.bysl:478:/ cppif.else");
           /*_.IFCPP___V71*/ curfptr[69] = (/*nil*/NULL);
            #endif /*ENABLE_CHECKING*/
            ;
           /*_#__L25*/ curfnum[19] = 
            ((/*_.CURCTYPE__V63*/ curfptr[62]) != (/*_.MATCHTYPE__V64*/ curfptr[63]));;
           BASILYS_LOCATION("warmelt-normatch.bysl:479:/ cond");
           /*cond*/ if (/*_#__L25*/ curfnum[19]) /*then*/ {
           BASILYS_LOCATION("warmelt-normatch.bysl:479:/ cond.then");
           BASILYS_LOCATION("warmelt-normatch.bysl:479:/ block");
            /*block*/{
             BASILYS_LOCATION("warmelt-normatch.bysl:483:/ cond");
             /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[10])))) /*then*/ {
             BASILYS_LOCATION("warmelt-normatch.bysl:483:/ cond.then");
             BASILYS_LOCATION("warmelt-normatch.bysl:483:/ getslot");
              { basilys_ptr_t slot=0;
               basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), 1, "NAMED_NAME");/*_.NAMED_NAME__V74*/ curfptr[73] = slot; };
              ;
             } else {BASILYS_LOCATION("warmelt-normatch.bysl:483:/ cond.else");
             
              /*_.NAMED_NAME__V74*/ curfptr[73] = NULL;;
              }
             ;
             
             basilys_error_str((basilys_ptr_t)(/*_.SLOC__V22*/ curfptr[21]), ( "type mismatch for match argument in matcher"), (basilys_ptr_t)(/*_.NAMED_NAME__V74*/ curfptr[73]));
             BASILYS_LOCATION("warmelt-normatch.bysl:486:/ cond");
             /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), (basilys_ptr_t)((/*!CLASS_ANY_BINDING*/ curfrout->tabval[13])))) /*then*/ {
             BASILYS_LOCATION("warmelt-normatch.bysl:486:/ cond.then");
             BASILYS_LOCATION("warmelt-normatch.bysl:486:/ getslot");
              { basilys_ptr_t slot=0;
               basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), 0, "BINDER");/*_.BINDER__V75*/ curfptr[74] = slot; };
              ;
             } else {BASILYS_LOCATION("warmelt-normatch.bysl:486:/ cond.else");
             
              /*_.BINDER__V75*/ curfptr[74] = NULL;;
              }
             ;
             BASILYS_LOCATION("warmelt-normatch.bysl:486:/ cond");
             /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.BINDER__V75*/ curfptr[74]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[10])))) /*then*/ {
             BASILYS_LOCATION("warmelt-normatch.bysl:486:/ cond.then");
             BASILYS_LOCATION("warmelt-normatch.bysl:486:/ getslot");
              { basilys_ptr_t slot=0;
               basilys_object_get_field(slot,(basilys_ptr_t)(/*_.BINDER__V75*/ curfptr[74]), 1, "NAMED_NAME");/*_.NAMED_NAME__V76*/ curfptr[75] = slot; };
              ;
             } else {BASILYS_LOCATION("warmelt-normatch.bysl:486:/ cond.else");
             
              /*_.NAMED_NAME__V76*/ curfptr[75] = NULL;;
              }
             ;
             
             basilys_inform_str((basilys_ptr_t)(/*_.SLOC__V22*/ curfptr[21]), ( "mismatched formal name in matcher is"), (basilys_ptr_t)(/*_.NAMED_NAME__V76*/ curfptr[75]));
             BASILYS_LOCATION("warmelt-normatch.bysl:489:/ cond");
             /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.CURCTYPE__V63*/ curfptr[62]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[10])))) /*then*/ {
             BASILYS_LOCATION("warmelt-normatch.bysl:489:/ cond.then");
             BASILYS_LOCATION("warmelt-normatch.bysl:489:/ getslot");
              { basilys_ptr_t slot=0;
               basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURCTYPE__V63*/ curfptr[62]), 1, "NAMED_NAME");/*_.NAMED_NAME__V77*/ curfptr[76] = slot; };
              ;
             } else {BASILYS_LOCATION("warmelt-normatch.bysl:489:/ cond.else");
             
              /*_.NAMED_NAME__V77*/ curfptr[76] = NULL;;
              }
             ;
             
             basilys_inform_str((basilys_ptr_t)(/*_.SLOC__V22*/ curfptr[21]), ( "mismatched input type is"), (basilys_ptr_t)(/*_.NAMED_NAME__V77*/ curfptr[76]));
             BASILYS_LOCATION("warmelt-normatch.bysl:492:/ cond");
             /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MATCHTYPE__V64*/ curfptr[63]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[10])))) /*then*/ {
             BASILYS_LOCATION("warmelt-normatch.bysl:492:/ cond.then");
             BASILYS_LOCATION("warmelt-normatch.bysl:492:/ getslot");
              { basilys_ptr_t slot=0;
               basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MATCHTYPE__V64*/ curfptr[63]), 1, "NAMED_NAME");/*_.NAMED_NAME__V78*/ curfptr[77] = slot; };
              ;
             } else {BASILYS_LOCATION("warmelt-normatch.bysl:492:/ cond.else");
             
              /*_.NAMED_NAME__V78*/ curfptr[77] = NULL;;
              }
             ;
             
             basilys_inform_str((basilys_ptr_t)(/*_.SLOC__V22*/ curfptr[21]), ( "expected match type type is"), (basilys_ptr_t)(/*_.NAMED_NAME__V78*/ curfptr[77]));
             BASILYS_LOCATION("warmelt-normatch.bysl:480:/ block");
             /*block*/{}
             ;
             /*epilog*/
             BASILYS_LOCATION("warmelt-normatch.bysl:479:/ clear");
             /*clear*/ /*_.NAMED_NAME__V74*/ curfptr[73] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:479:/ clear");
             /*clear*/ /*_.BINDER__V75*/ curfptr[74] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:479:/ clear");
             /*clear*/ /*_.NAMED_NAME__V76*/ curfptr[75] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:479:/ clear");
             /*clear*/ /*_.NAMED_NAME__V77*/ curfptr[76] = 0 ;
             BASILYS_LOCATION("warmelt-normatch.bysl:479:/ clear");
             /*clear*/ /*_.NAMED_NAME__V78*/ curfptr[77] = 0 ;}
            ;
           } else {BASILYS_LOCATION("warmelt-normatch.bysl:479:/ cond.else");
           
            BASILYS_LOCATION("warmelt-normatch.bysl:479:/ block");
            /*block*/{
             BASILYS_LOCATION("warmelt-normatch.bysl:494:/ block");
             /*block*/{
              BASILYS_LOCATION("warmelt-normatch.bysl:496:/ cond");
              /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), (basilys_ptr_t)((/*!CLASS_ANY_BINDING*/ curfrout->tabval[13])))) /*then*/ {
              BASILYS_LOCATION("warmelt-normatch.bysl:496:/ cond.then");
              BASILYS_LOCATION("warmelt-normatch.bysl:496:/ getslot");
               { basilys_ptr_t slot=0;
                basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURMATCH__V62*/ curfptr[61]), 0, "BINDER");/*_.BINDER__V80*/ curfptr[74] = slot; };
               ;
              } else {BASILYS_LOCATION("warmelt-normatch.bysl:496:/ cond.else");
              
               /*_.BINDER__V80*/ curfptr[74] = NULL;;
               }
              ;
              BASILYS_LOCATION("warmelt-normatch.bysl:494:/ block");
              /*block*/{
               BASILYS_LOCATION("warmelt-normatch.bysl:494:/ rawallocobj");
               /*rawallocobj*/ { basilys_ptr_t newobj = 0;
                basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_NORMLET_BINDING*/ curfrout->tabval[14])), (4), "CLASS_NORMLET_BINDING");/*_.INST__V82*/ curfptr[76] =
                newobj; };
               ;
               BASILYS_LOCATION("warmelt-normatch.bysl:494:/ putslot");
               /*putslot*/
               basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76])) == OBMAG_OBJECT);
               basilys_assertmsg("putslot checkoff", (3>=0 && 3< basilys_object_length((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76]))));basilys_putfield_object((/*_.INST__V82*/ curfptr[76]), (3), (/*_.SLOC__V22*/ curfptr[21]), "LETBIND_LOC");
               ;
               BASILYS_LOCATION("warmelt-normatch.bysl:494:/ putslot");
               /*putslot*/
               basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76])) == OBMAG_OBJECT);
               basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76]))));basilys_putfield_object((/*_.INST__V82*/ curfptr[76]), (0), (/*_.BINDER__V80*/ curfptr[74]), "BINDER");
               ;
               BASILYS_LOCATION("warmelt-normatch.bysl:494:/ putslot");
               /*putslot*/
               basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76])) == OBMAG_OBJECT);
               basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76]))));basilys_putfield_object((/*_.INST__V82*/ curfptr[76]), (1), (/*_.CURCTYPE__V63*/ curfptr[62]), "LETBIND_TYPE");
               ;
               BASILYS_LOCATION("warmelt-normatch.bysl:494:/ putslot");
               /*putslot*/
               basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76])) == OBMAG_OBJECT);
               basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.INST__V82*/ curfptr[76]))));basilys_putfield_object((/*_.INST__V82*/ curfptr[76]), (2), (/*_.NCURIN__V58*/ curfptr[57]), "LETBIND_EXPR");
               ;
               /*_.NEWB__V81*/ curfptr[75] = /*_.INST__V82*/ curfptr[76];;}
              ;
              
              #if ENABLE_CHECKING
               BASILYS_LOCATION("warmelt-normatch.bysl:501:/ cppif.then");
              BASILYS_LOCATION("warmelt-normatch.bysl:501:/ block");
               /*block*/{
                /*_#THE_CALLCOUNT__L26*/ curfnum[25] = 
                 callcount;;
                BASILYS_LOCATION("warmelt-normatch.bysl:501:/ apply");
                /*apply*/{
                 union basilysparam_un argtab[4];
                 memset(&argtab, 0, sizeof(argtab));
                 BASILYS_LOCATION("warmelt-normatch.bysl:501:/ apply.arg");
                 argtab[0].bp_cstring =  "normpat_anymatchpat newb";
                 BASILYS_LOCATION("warmelt-normatch.bysl:501:/ apply.arg");
                 argtab[1].bp_long = /*_#THE_CALLCOUNT__L26*/ curfnum[25];
                 BASILYS_LOCATION("warmelt-normatch.bysl:501:/ apply.arg");
                 argtab[2].bp_cstring =  "warmelt-normatch.bysl";
                 BASILYS_LOCATION("warmelt-normatch.bysl:501:/ apply.arg");
                 argtab[3].bp_long = 501;
                 /*_.DEBUG_MSG_FUN__V84*/ curfptr[83] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.NEWB__V81*/ curfptr[75]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
                 }
                ;
                /*_.IFCPP___V83*/ curfptr[77] = /*_.DEBUG_MSG_FUN__V84*/ curfptr[83];;
                /*epilog*/
                BASILYS_LOCATION("warmelt-normatch.bysl:501:/ clear");
                /*clear*/ /*_#THE_CALLCOUNT__L26*/ curfnum[25] = 0 ;
                BASILYS_LOCATION("warmelt-normatch.bysl:501:/ clear");
                /*clear*/ /*_.DEBUG_MSG_FUN__V84*/ curfptr[83] = 0 ;}
               
               #else /*ENABLE_CHECKING*/
               BASILYS_LOCATION("warmelt-normatch.bysl:501:/ cppif.else");
              /*_.IFCPP___V83*/ curfptr[77] = (/*nil*/NULL);
               #endif /*ENABLE_CHECKING*/
               ;
              BASILYS_LOCATION("warmelt-normatch.bysl:502:/ apply");
              /*apply*/{
               union basilysparam_un argtab[1];
               memset(&argtab, 0, sizeof(argtab));
               BASILYS_LOCATION("warmelt-normatch.bysl:502:/ apply.arg");
               argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NEWB__V81*/ curfptr[75];
               /*_.PUT_ENV__V85*/ curfptr[83] =  basilys_apply ((basilysclosure_ptr_t)((/*!PUT_ENV*/ curfrout->tabval[15])), (basilys_ptr_t)(/*_.NEWENV__V57*/ curfptr[54]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
               }
              ;
              /*_.LET___V79*/ curfptr[73] = /*_.PUT_ENV__V85*/ curfptr[83];;
              /*epilog*/
              BASILYS_LOCATION("warmelt-normatch.bysl:494:/ clear");
              /*clear*/ /*_.BINDER__V80*/ curfptr[74] = 0 ;
              BASILYS_LOCATION("warmelt-normatch.bysl:494:/ clear");
              /*clear*/ /*_.NEWB__V81*/ curfptr[75] = 0 ;
              BASILYS_LOCATION("warmelt-normatch.bysl:494:/ clear");
              /*clear*/ /*_.IFCPP___V83*/ curfptr[77] = 0 ;
              BASILYS_LOCATION("warmelt-normatch.bysl:494:/ clear");
              /*clear*/ /*_.PUT_ENV__V85*/ curfptr[83] = 0 ;}
             ;
             /*_.IFELSE___V73*/ curfptr[71] = /*_.LET___V79*/ curfptr[73];;
             /*epilog*/
             BASILYS_LOCATION("warmelt-normatch.bysl:479:/ clear");
             /*clear*/ /*_.LET___V79*/ curfptr[73] = 0 ;}
            ;
            }
           ;
           /*epilog*/
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.CURMATCH__V62*/ curfptr[61] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.CURCTYPE__V63*/ curfptr[62] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.MATCHTYPE__V64*/ curfptr[63] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.IFCPP___V65*/ curfptr[64] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.IFCPP___V67*/ curfptr[65] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.IFCPP___V69*/ curfptr[67] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.IFCPP___V71*/ curfptr[69] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_#__L25*/ curfnum[19] = 0 ;
           BASILYS_LOCATION("warmelt-normatch.bysl:470:/ clear");
           /*clear*/ /*_.IFELSE___V73*/ curfptr[71] = 0 ;}
          ;
         } /* end cit1__EACHTUP*/
         
          /*citerepilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:466:/ clear");
          /*clear*/ /*_.NCURIN__V58*/ curfptr[57] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:466:/ clear");
          /*clear*/ /*_#IX__L19*/ curfnum[18] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:466:/ clear");
          /*clear*/ /*_.IFCPP___V59*/ curfptr[58] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:466:/ clear");
          /*clear*/ /*_.LET___V61*/ curfptr[59] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:504:/ compute");
         /*_.ENV__V26*/ curfptr[25] = /*_.SETQ___V86*/ curfptr[74] = /*_.NEWENV__V57*/ curfptr[54];;
         BASILYS_LOCATION("warmelt-normatch.bysl:505:/ cond");
         /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])))) /*then*/ {
         BASILYS_LOCATION("warmelt-normatch.bysl:505:/ cond.then");
         BASILYS_LOCATION("warmelt-normatch.bysl:505:/ block");
          /*block*/{
           BASILYS_LOCATION("warmelt-normatch.bysl:505:/ putslot");
           /*putslot*/
           basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5])) == OBMAG_OBJECT);
           basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]))));basilys_putfield_object((/*_.PCN__V6*/ curfptr[5]), (2), (/*_.NEWENV__V57*/ curfptr[54]), "PCTN_ENV");
           ;
           BASILYS_LOCATION("warmelt-normatch.bysl:505:/ touch");
           basilysgc_touch(/*_.PCN__V6*/ curfptr[5]);
           ;
           /*epilog*/}
          ;
         } /*noelse*/
         ;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:465:/ clear");
         /*clear*/ /*_.NEWENV__V57*/ curfptr[54] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:465:/ clear");
         /*clear*/ /*_.SETQ___V86*/ curfptr[74] = 0 ;}
        ;
        /*epilog*/}
       ;
      } /*noelse*/
      ;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:458:/ clear");
      /*clear*/ /*_#NBINARGS__L15*/ curfnum[0] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:458:/ clear");
      /*clear*/ /*_#MULTIPLE_LENGTH__L16*/ curfnum[15] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:458:/ clear");
      /*clear*/ /*_#I__L17*/ curfnum[16] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:458:/ clear");
      /*clear*/ /*_#I__L18*/ curfnum[17] = 0 ;}
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:508:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:508:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L27*/ curfnum[25] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:508:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:508:/ apply.arg");
        argtab[0].bp_cstring =  "normpat_anymatchpat outargs";
        BASILYS_LOCATION("warmelt-normatch.bysl:508:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L27*/ curfnum[25];
        BASILYS_LOCATION("warmelt-normatch.bysl:508:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:508:/ apply.arg");
        argtab[3].bp_long = 508;
        /*_.DEBUG_MSG_FUN__V88*/ curfptr[77] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.OUTARGS__V25*/ curfptr[24]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V87*/ curfptr[75] = /*_.DEBUG_MSG_FUN__V88*/ curfptr[77];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:508:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L27*/ curfnum[25] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:508:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V88*/ curfptr[77] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:508:/ cppif.else");
     /*_.IFCPP___V87*/ curfptr[75] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:509:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:509:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L28*/ curfnum[19] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:509:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:509:/ apply.arg");
        argtab[0].bp_cstring =  "normpat_anymatchpat outmatchs";
        BASILYS_LOCATION("warmelt-normatch.bysl:509:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L28*/ curfnum[19];
        BASILYS_LOCATION("warmelt-normatch.bysl:509:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:509:/ apply.arg");
        argtab[3].bp_long = 509;
        /*_.DEBUG_MSG_FUN__V90*/ curfptr[73] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.OUTMATCHS__V47*/ curfptr[46]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V89*/ curfptr[83] = /*_.DEBUG_MSG_FUN__V90*/ curfptr[73];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:509:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L28*/ curfnum[19] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:509:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V90*/ curfptr[73] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:509:/ cppif.else");
     /*_.IFCPP___V89*/ curfptr[83] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     BASILYS_LOCATION("warmelt-normatch.bysl:510:/ block");
     /*block*/{
      /*_#NBOUTARGS__L29*/ curfnum[0] = 
       (basilys_multiple_length((basilys_ptr_t)(/*_.OUTARGS__V25*/ curfptr[24])));;
      /*_#MULTIPLE_LENGTH__L30*/ curfnum[15] = 
       (basilys_multiple_length((basilys_ptr_t)(/*_.OUTMATCHS__V47*/ curfptr[46])));;
      /*_#I__L31*/ curfnum[16] = 
       ((/*_#NBOUTARGS__L29*/ curfnum[0]) != (/*_#MULTIPLE_LENGTH__L30*/ curfnum[15]));;
      BASILYS_LOCATION("warmelt-normatch.bysl:511:/ cond");
      /*cond*/ if (/*_#I__L31*/ curfnum[16]) /*then*/ {
      BASILYS_LOCATION("warmelt-normatch.bysl:511:/ cond.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:511:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:515:/ cond");
        /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[10])))) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:515:/ cond.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:515:/ getslot");
         { basilys_ptr_t slot=0;
          basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MAT__V23*/ curfptr[22]), 1, "NAMED_NAME");/*_.NAMED_NAME__V92*/ curfptr[62] = slot; };
         ;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:515:/ cond.else");
        
         /*_.NAMED_NAME__V92*/ curfptr[62] = NULL;;
         }
        ;
        
        basilys_error_str((basilys_ptr_t)(/*_.SLOC__V22*/ curfptr[21]), ( "formal and actual number of output matched arguments differ for anymatch"), (basilys_ptr_t)(/*_.NAMED_NAME__V92*/ curfptr[62]));
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:511:/ clear");
        /*clear*/ /*_.NAMED_NAME__V92*/ curfptr[62] = 0 ;}
       ;
      } /*noelse*/
      ;
      
      basilysgc_prepend_list((basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4]), (basilys_ptr_t)(/*_.CURTESTP__V50*/ curfptr[49]));
      
      #if ENABLE_CHECKING
       BASILYS_LOCATION("warmelt-normatch.bysl:518:/ cppif.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:518:/ block");
       /*block*/{
        /*_#THE_CALLCOUNT__L32*/ curfnum[17] = 
         callcount;;
        BASILYS_LOCATION("warmelt-normatch.bysl:518:/ apply");
        /*apply*/{
         union basilysparam_un argtab[4];
         memset(&argtab, 0, sizeof(argtab));
         BASILYS_LOCATION("warmelt-normatch.bysl:518:/ apply.arg");
         argtab[0].bp_cstring =  "normpat_anymatchpat updated testptlist";
         BASILYS_LOCATION("warmelt-normatch.bysl:518:/ apply.arg");
         argtab[1].bp_long = /*_#THE_CALLCOUNT__L32*/ curfnum[17];
         BASILYS_LOCATION("warmelt-normatch.bysl:518:/ apply.arg");
         argtab[2].bp_cstring =  "warmelt-normatch.bysl";
         BASILYS_LOCATION("warmelt-normatch.bysl:518:/ apply.arg");
         argtab[3].bp_long = 518;
         /*_.DEBUG_MSG_FUN__V94*/ curfptr[64] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
         }
        ;
        /*_.IFCPP___V93*/ curfptr[63] = /*_.DEBUG_MSG_FUN__V94*/ curfptr[64];;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:518:/ clear");
        /*clear*/ /*_#THE_CALLCOUNT__L32*/ curfnum[17] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:518:/ clear");
        /*clear*/ /*_.DEBUG_MSG_FUN__V94*/ curfptr[64] = 0 ;}
       
       #else /*ENABLE_CHECKING*/
       BASILYS_LOCATION("warmelt-normatch.bysl:518:/ cppif.else");
      /*_.IFCPP___V93*/ curfptr[63] = (/*nil*/NULL);
       #endif /*ENABLE_CHECKING*/
       ;
      
      basilysgc_prepend_list((basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3]), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]));
      
      #if ENABLE_CHECKING
       BASILYS_LOCATION("warmelt-normatch.bysl:521:/ cppif.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:521:/ block");
       /*block*/{
        /*_#THE_CALLCOUNT__L33*/ curfnum[25] = 
         callcount;;
        BASILYS_LOCATION("warmelt-normatch.bysl:521:/ apply");
        /*apply*/{
         union basilysparam_un argtab[4];
         memset(&argtab, 0, sizeof(argtab));
         BASILYS_LOCATION("warmelt-normatch.bysl:521:/ apply.arg");
         argtab[0].bp_cstring =  "normpat_anymatchpat updated spatlist";
         BASILYS_LOCATION("warmelt-normatch.bysl:521:/ apply.arg");
         argtab[1].bp_long = /*_#THE_CALLCOUNT__L33*/ curfnum[25];
         BASILYS_LOCATION("warmelt-normatch.bysl:521:/ apply.arg");
         argtab[2].bp_cstring =  "warmelt-normatch.bysl";
         BASILYS_LOCATION("warmelt-normatch.bysl:521:/ apply.arg");
         argtab[3].bp_long = 521;
         /*_.DEBUG_MSG_FUN__V96*/ curfptr[67] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
         }
        ;
        /*_.IFCPP___V95*/ curfptr[65] = /*_.DEBUG_MSG_FUN__V96*/ curfptr[67];;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:521:/ clear");
        /*clear*/ /*_#THE_CALLCOUNT__L33*/ curfnum[25] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:521:/ clear");
        /*clear*/ /*_.DEBUG_MSG_FUN__V96*/ curfptr[67] = 0 ;}
       
       #else /*ENABLE_CHECKING*/
       BASILYS_LOCATION("warmelt-normatch.bysl:521:/ cppif.else");
      /*_.IFCPP___V95*/ curfptr[65] = (/*nil*/NULL);
       #endif /*ENABLE_CHECKING*/
       ;
      /*citerblock FOREACH_IN_MULTIPLE*/ {
      long cit2__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.OUTMATCHS__V47*/ curfptr[46]);
int cit2__EACHTUP_ix = 0;
for (cit2__EACHTUP_ix = 0; cit2__EACHTUP_ix < cit2__EACHTUP_ln; cit2__EACHTUP_ix ++) {
/*_.CURMATCH__V97*/ curfptr[69] = basilys_multiple_nth((basilys_ptr_t)(/*_.OUTMATCHS__V47*/ curfptr[46]), cit2__EACHTUP_ix);
/*_#IX__L34*/ curfnum[19] = cit2__EACHTUP_ix;

      
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:526:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:526:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L35*/ curfnum[17] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:526:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:526:/ apply.arg");
          argtab[0].bp_cstring =  "normpat_anymatchpat curmatch";
          BASILYS_LOCATION("warmelt-normatch.bysl:526:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L35*/ curfnum[17];
          BASILYS_LOCATION("warmelt-normatch.bysl:526:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:526:/ apply.arg");
          argtab[3].bp_long = 526;
          /*_.DEBUG_MSG_FUN__V99*/ curfptr[54] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:526:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L35*/ curfnum[17] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:526:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V99*/ curfptr[54] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:526:/ cppif.else");
       (/*nil*/NULL)
        #endif /*ENABLE_CHECKING*/
        ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:527:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:527:/ block");
        /*block*/{
         /*_#IS_A__L36*/ curfnum[25] = 
           basilys_is_instance_of((basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), (basilys_ptr_t)((/*!CLASS_FORMAL_BINDING*/ curfrout->tabval[9])));;
         BASILYS_LOCATION("warmelt-normatch.bysl:527:/ cond");
         /*cond*/ if (/*_#IS_A__L36*/ curfnum[25]) /*then*/ {
         BASILYS_LOCATION("warmelt-normatch.bysl:527:/ cond.then");
         /*_.IFELSE___V101*/ curfptr[77] = (/*nil*/NULL);;
         } else {BASILYS_LOCATION("warmelt-normatch.bysl:527:/ cond.else");
         
          BASILYS_LOCATION("warmelt-normatch.bysl:527:/ block");
          /*block*/{
           /*block*/{
            
            basilys_assert_failed(( "check curmatch"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(527)?(527):__LINE__, __FUNCTION__);
            /*clear*/ /*_.IFELSE___V101*/ curfptr[77] = 0 ;}
           ;
           /*epilog*/}
          ;
          }
         ;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:527:/ clear");
         /*clear*/ /*_#IS_A__L36*/ curfnum[25] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:527:/ clear");
         /*clear*/ /*_.IFELSE___V101*/ curfptr[77] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:527:/ cppif.else");
       (/*nil*/NULL)
        #endif /*ENABLE_CHECKING*/
        ;
       BASILYS_LOCATION("warmelt-normatch.bysl:528:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ cond");
        /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), (basilys_ptr_t)((/*!CLASS_ANY_BINDING*/ curfrout->tabval[13])))) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ cond.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ getslot");
         { basilys_ptr_t slot=0;
          basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), 0, "BINDER");/*_.BINDER__V103*/ curfptr[62] = slot; };
         ;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:528:/ cond.else");
        
         /*_.BINDER__V103*/ curfptr[62] = NULL;;
         }
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ apply");
        /*apply*/{
         /*_.CSYM__V104*/ curfptr[64] =  basilys_apply ((basilysclosure_ptr_t)((/*!CLONE_SYMBOL*/ curfrout->tabval[16])), (basilys_ptr_t)(/*_.BINDER__V103*/ curfptr[62]), (""), (union basilysparam_un*)0, "", (union basilysparam_un*)0);
         }
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:529:/ cond");
        /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), (basilys_ptr_t)((/*!CLASS_FORMAL_BINDING*/ curfrout->tabval[9])))) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:529:/ cond.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:529:/ getslot");
         { basilys_ptr_t slot=0;
          basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), 1, "FBIND_TYPE");/*_.CURTYP__V105*/ curfptr[67] = slot; };
         ;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:529:/ cond.else");
        
         /*_.CURTYP__V105*/ curfptr[67] = NULL;;
         }
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:530:/ block");
        /*block*/{
         BASILYS_LOCATION("warmelt-normatch.bysl:530:/ rawallocobj");
         /*rawallocobj*/ { basilys_ptr_t newobj = 0;
          basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_NORMLET_BINDING*/ curfrout->tabval[14])), (4), "CLASS_NORMLET_BINDING");/*_.INST__V107*/ curfptr[77] =
          newobj; };
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:530:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (3>=0 && 3< basilys_object_length((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77]))));basilys_putfield_object((/*_.INST__V107*/ curfptr[77]), (3), (/*_.SLOC__V22*/ curfptr[21]), "LETBIND_LOC");
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:530:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77]))));basilys_putfield_object((/*_.INST__V107*/ curfptr[77]), (0), (/*_.CSYM__V104*/ curfptr[64]), "BINDER");
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:530:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77]))));basilys_putfield_object((/*_.INST__V107*/ curfptr[77]), (1), (/*_.CURTYP__V105*/ curfptr[67]), "LETBIND_TYPE");
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:530:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.INST__V107*/ curfptr[77]))));basilys_putfield_object((/*_.INST__V107*/ curfptr[77]), (2), ((/*nil*/NULL)), "LETBIND_EXPR");
         ;
         /*_.CBIND__V106*/ curfptr[54] = /*_.INST__V107*/ curfptr[77];;}
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:536:/ block");
        /*block*/{
         BASILYS_LOCATION("warmelt-normatch.bysl:536:/ rawallocobj");
         /*rawallocobj*/ { basilys_ptr_t newobj = 0;
          basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_NREP_LOCSYMOCC*/ curfrout->tabval[17])), (4), "CLASS_NREP_LOCSYMOCC");/*_.INST__V109*/ curfptr[108] =
          newobj; };
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:536:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108]))));basilys_putfield_object((/*_.INST__V109*/ curfptr[108]), (0), (/*_.SLOC__V22*/ curfptr[21]), "NREP_LOC");
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:536:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108]))));basilys_putfield_object((/*_.INST__V109*/ curfptr[108]), (2), (/*_.CURTYP__V105*/ curfptr[67]), "NOCC_CTYP");
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:536:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108]))));basilys_putfield_object((/*_.INST__V109*/ curfptr[108]), (1), (/*_.CSYM__V104*/ curfptr[64]), "NOCC_SYMB");
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:536:/ putslot");
         /*putslot*/
         basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108])) == OBMAG_OBJECT);
         basilys_assertmsg("putslot checkoff", (3>=0 && 3< basilys_object_length((basilys_ptr_t)(/*_.INST__V109*/ curfptr[108]))));basilys_putfield_object((/*_.INST__V109*/ curfptr[108]), (3), (/*_.CBIND__V106*/ curfptr[54]), "NOCC_BIND");
         ;
         /*_.CLOCC__V108*/ curfptr[107] = /*_.INST__V109*/ curfptr[108];;}
        ;
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:542:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:542:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L37*/ curfnum[17] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:542:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:542:/ apply.arg");
           argtab[0].bp_cstring =  "normpat_anymatchpat clocc before normal_pattern";
           BASILYS_LOCATION("warmelt-normatch.bysl:542:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L37*/ curfnum[17];
           BASILYS_LOCATION("warmelt-normatch.bysl:542:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:542:/ apply.arg");
           argtab[3].bp_long = 542;
           /*_.DEBUG_MSG_FUN__V111*/ curfptr[110] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.CLOCC__V108*/ curfptr[107]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V110*/ curfptr[109] = /*_.DEBUG_MSG_FUN__V111*/ curfptr[110];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:542:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L37*/ curfnum[17] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:542:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V111*/ curfptr[110] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:542:/ cppif.else");
        /*_.IFCPP___V110*/ curfptr[109] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        BASILYS_LOCATION("warmelt-normatch.bysl:543:/ msend");
        /*msend*/{
         union basilysparam_un argtab[4];
         memset(&argtab, 0, sizeof(argtab));
         BASILYS_LOCATION("warmelt-normatch.bysl:543:/ ojbmsend.arg");
         argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.CLOCC__V108*/ curfptr[107];
         BASILYS_LOCATION("warmelt-normatch.bysl:543:/ ojbmsend.arg");
         argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.SPATLIST__V4*/ curfptr[3];
         BASILYS_LOCATION("warmelt-normatch.bysl:543:/ ojbmsend.arg");
         argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.TESTPTLIST__V5*/ curfptr[4];
         BASILYS_LOCATION("warmelt-normatch.bysl:543:/ ojbmsend.arg");
         argtab[3].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V6*/ curfptr[5];
         /*_.NORMAL_PATTERN__V112*/ curfptr[110] = basilysgc_send((basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), (basilys_ptr_t)((/*!NORMAL_PATTERN*/ curfrout->tabval[18])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
         }
        ;
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:544:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:544:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L38*/ curfnum[25] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:544:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:544:/ apply.arg");
           argtab[0].bp_cstring =  "normpat_anymatchpat curmatch after normal_pattern";
           BASILYS_LOCATION("warmelt-normatch.bysl:544:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L38*/ curfnum[25];
           BASILYS_LOCATION("warmelt-normatch.bysl:544:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:544:/ apply.arg");
           argtab[3].bp_long = 544;
           /*_.DEBUG_MSG_FUN__V114*/ curfptr[113] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.CURMATCH__V97*/ curfptr[69]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V113*/ curfptr[112] = /*_.DEBUG_MSG_FUN__V114*/ curfptr[113];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:544:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L38*/ curfnum[25] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:544:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V114*/ curfptr[113] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:544:/ cppif.else");
        /*_.IFCPP___V113*/ curfptr[112] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.BINDER__V103*/ curfptr[62] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.CSYM__V104*/ curfptr[64] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.CURTYP__V105*/ curfptr[67] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.CBIND__V106*/ curfptr[54] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.CLOCC__V108*/ curfptr[107] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.IFCPP___V110*/ curfptr[109] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.NORMAL_PATTERN__V112*/ curfptr[110] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:528:/ clear");
        /*clear*/ /*_.IFCPP___V113*/ curfptr[112] = 0 ;}
       ;
      } /* end cit2__EACHTUP*/
      
       /*citerepilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:523:/ clear");
       /*clear*/ /*_.CURMATCH__V97*/ curfptr[69] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:523:/ clear");
       /*clear*/ /*_#IX__L34*/ curfnum[19] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:523:/ clear");
       /*clear*/ /*_.IFCPP___V98*/ curfptr[71] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:523:/ clear");
       /*clear*/ /*_.IFCPP___V100*/ curfptr[74] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:523:/ clear");
       /*clear*/ /*_.LET___V102*/ curfptr[73] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
      ;
      /*_.LIST_POPFIRST__V115*/ curfptr[113] = 
       (basilysgc_popfirst_list((basilys_ptr_t)(/*_.SPATLIST__V4*/ curfptr[3])));;
      /*_.LIST_POPFIRST__V116*/ curfptr[62] = 
       (basilysgc_popfirst_list((basilys_ptr_t)(/*_.TESTPTLIST__V5*/ curfptr[4])));;
      /*_.LET___V91*/ curfptr[61] = /*_.LIST_POPFIRST__V116*/ curfptr[62];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:510:/ clear");
      /*clear*/ /*_#NBOUTARGS__L29*/ curfnum[0] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:510:/ clear");
      /*clear*/ /*_#MULTIPLE_LENGTH__L30*/ curfnum[15] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:510:/ clear");
      /*clear*/ /*_#I__L31*/ curfnum[16] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:510:/ clear");
      /*clear*/ /*_.IFCPP___V93*/ curfptr[63] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:510:/ clear");
      /*clear*/ /*_.IFCPP___V95*/ curfptr[65] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:510:/ clear");
      /*clear*/ /*_.LIST_POPFIRST__V115*/ curfptr[113] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:510:/ clear");
      /*clear*/ /*_.LIST_POPFIRST__V116*/ curfptr[62] = 0 ;}
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:553:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:553:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L39*/ curfnum[17] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:553:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:553:/ apply.arg");
        argtab[0].bp_cstring =  "normpat_anymatchpat curtestp";
        BASILYS_LOCATION("warmelt-normatch.bysl:553:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L39*/ curfnum[17];
        BASILYS_LOCATION("warmelt-normatch.bysl:553:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:553:/ apply.arg");
        argtab[3].bp_long = 553;
        /*_.DEBUG_MSG_FUN__V118*/ curfptr[67] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.CURTESTP__V50*/ curfptr[49]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V117*/ curfptr[64] = /*_.DEBUG_MSG_FUN__V118*/ curfptr[67];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:553:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L39*/ curfnum[17] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:553:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V118*/ curfptr[67] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:553:/ cppif.else");
     /*_.IFCPP___V117*/ curfptr[64] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     /*_.LET___V45*/ curfptr[43] = /*_.IFCPP___V117*/ curfptr[64];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.INMATCHS__V46*/ curfptr[45] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.OUTMATCHS__V47*/ curfptr[46] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.MATBIND__V48*/ curfptr[47] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.FBIND_TYPE__V49*/ curfptr[48] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.CURTESTP__V50*/ curfptr[49] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.IFCPP___V52*/ curfptr[51] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.IFCPP___V54*/ curfptr[52] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.IFCPP___V87*/ curfptr[75] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.IFCPP___V89*/ curfptr[83] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.LET___V91*/ curfptr[61] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:442:/ clear");
     /*clear*/ /*_.IFCPP___V117*/ curfptr[64] = 0 ;}
    ;
    /*_#__L40*/ curfnum[25] = 
     ((/*_.ENV__V26*/ curfptr[25]) != (/*_.OLDENV__V27*/ curfptr[26]));;
    BASILYS_LOCATION("warmelt-normatch.bysl:555:/ cond");
    /*cond*/ if (/*_#__L40*/ curfnum[25]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:555:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:555:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:556:/ cond");
      /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])))) /*then*/ {
      BASILYS_LOCATION("warmelt-normatch.bysl:556:/ cond.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:556:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:556:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.PCN__V6*/ curfptr[5]))));basilys_putfield_object((/*_.PCN__V6*/ curfptr[5]), (2), (/*_.OLDENV__V27*/ curfptr[26]), "PCTN_ENV");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:556:/ touch");
        basilysgc_touch(/*_.PCN__V6*/ curfptr[5]);
        ;
        /*epilog*/}
       ;
      } /*noelse*/
      ;
      /*epilog*/}
     ;
    } /*noelse*/
    ;
    /*_.MULTI___V36*/ curfptr[34] = (/*nil*/NULL);;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:436:/ clear");
    /*clear*/ /*_.IFCPP___V39*/ curfptr[38] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:436:/ clear");
    /*clear*/ /*_.IFCPP___V41*/ curfptr[39] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:436:/ clear");
    /*clear*/ /*_.IF___V43*/ curfptr[41] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:436:/ clear");
    /*clear*/ /*_.LET___V45*/ curfptr[43] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:436:/ clear");
    /*clear*/ /*_#__L40*/ curfnum[25] = 0 ;}
   ;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:436:/ clear");
   /*clear*/ /*_.BINDINS__V38*/ curfptr[37] = 0 ;}
  ;
  /*_.LET___V21*/ curfptr[19] = /*_.MULTI___V36*/ curfptr[34];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.SLOC__V22*/ curfptr[21] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.MAT__V23*/ curfptr[22] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.INARGS__V24*/ curfptr[23] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.OUTARGS__V25*/ curfptr[24] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.ENV__V26*/ curfptr[25] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.OLDENV__V27*/ curfptr[26] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.NCX__V28*/ curfptr[27] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.BINDLIST__V29*/ curfptr[28] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.IFCPP___V30*/ curfptr[29] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.IFCPP___V32*/ curfptr[30] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.IFCPP___V34*/ curfptr[32] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:423:/ clear");
  /*clear*/ /*_.MULTI___V36*/ curfptr[34] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V21*/ curfptr[19];;
  BASILYS_LOCATION("warmelt-normatch.bysl:415:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.IFCPP___V7*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.IFCPP___V9*/ curfptr[7] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.IFCPP___V11*/ curfptr[9] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.IFCPP___V13*/ curfptr[11] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.IFCPP___V15*/ curfptr[13] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.IFCPP___V17*/ curfptr[15] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.IFCPP___V19*/ curfptr[17] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ clear");
 /*clear*/ /*_.LET___V21*/ curfptr[19] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("NORMPAT_ANYMATCHPAT", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_14_NORMPAT_ANYMATCHPAT*/





static basilys_ptr_t
rout_15_NORMEXP_MATCH(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_15_NORMEXP_MATCH_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 86
  void* varptr[86];
#define CURFRAM_NBVARNUM 25
  long varnum[25];
/*others*/
  long _spare_; }
    *framptr_=0,    curfram__;
  if (BASILYS_UNLIKELY(xargdescr_ == BASILYSPAR_MARKGGC)) { /*mark for ggc*/
   int ix=0;
   framptr_ = (void*)firstargp_;
   gt_ggc_mx_basilys_un (framptr_->clos);
   for(ix=0; ix<86; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   return NULL;
  }/*end markggc*/;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 86;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("NORMEXP_MATCH", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.ENV__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.ENV__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.NCX__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.NCX__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:566:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:567:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:567:/ block");
  /*block*/{
   /*_#IS_A__L1*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRC_MATCH*/ curfrout->tabval[0])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:567:/ cond");
   /*cond*/ if (/*_#IS_A__L1*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:567:/ cond.then");
   /*_.IFELSE___V7*/ curfptr[6] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:567:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:567:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check match recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(567)?(567):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.IFELSE___V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:567:/ clear");
   /*clear*/ /*_#IS_A__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:567:/ clear");
   /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:567:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:568:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:568:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.ENV__V3*/ curfptr[2]), (basilys_ptr_t)((/*!CLASS_ENVIRONMENT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:568:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:568:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:568:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:568:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check env"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(568)?(568):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:568:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:568:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:568:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:569:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:569:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.NCX__V4*/ curfptr[3]), (basilys_ptr_t)((/*!CLASS_NORMCONTEXT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:569:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:569:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:569:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:569:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check nctxt"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(569)?(569):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:569:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:569:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:569:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:570:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:570:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:570:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:570:/ apply.arg");
    argtab[0].bp_cstring =  "normexp_match recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:570:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L4*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:570:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:570:/ apply.arg");
    argtab[3].bp_long = 570;
    /*_.DEBUG_MSG_FUN__V13*/ curfptr[12] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V12*/ curfptr[10] = /*_.DEBUG_MSG_FUN__V13*/ curfptr[12];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:570:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:570:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V13*/ curfptr[12] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:570:/ cppif.else");
 /*_.IFCPP___V12*/ curfptr[10] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:571:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V15*/ curfptr[14] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:572:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SMAT_MATCHEDX");/*_.SMATSX__V16*/ curfptr[15] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:573:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 2, "SMAT_CASES");/*_.SCASES__V17*/ curfptr[16] = slot; };
  ;
  /*_#NBCASES__L5*/ curfnum[0] = 
   (basilys_multiple_length((basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16])));;
  /*_.TUPVARMAP__V18*/ curfptr[17] = 
   (basilysgc_new_multiple((basilysobject_ptr_t)((/*!DISCR_MULTIPLE*/ curfrout->tabval[4])), (/*_#NBCASES__L5*/ curfnum[0])));;
  /*_.TUPCSTMAP__V19*/ curfptr[18] = 
   (basilysgc_new_multiple((basilysobject_ptr_t)((/*!DISCR_MULTIPLE*/ curfrout->tabval[4])), (/*_#NBCASES__L5*/ curfnum[0])));;
  /*_.TUPBINDLIST__V20*/ curfptr[19] = 
   (basilysgc_new_multiple((basilysobject_ptr_t)((/*!DISCR_MULTIPLE*/ curfrout->tabval[4])), (/*_#NBCASES__L5*/ curfnum[0])));;
  /*_.SPATLIST__V21*/ curfptr[20] = 
   (basilysgc_new_list((basilysobject_ptr_t)((/*!DISCR_LIST*/ curfrout->tabval[5]))));;
  /*_.TESTPTLIST__V22*/ curfptr[21] = 
   (basilysgc_new_list((basilysobject_ptr_t)((/*!DISCR_LIST*/ curfrout->tabval[5]))));;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:581:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:581:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L6*/ curfnum[5] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:581:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:581:/ apply.arg");
     argtab[0].bp_cstring =  "normexp_match smatsx";
     BASILYS_LOCATION("warmelt-normatch.bysl:581:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L6*/ curfnum[5];
     BASILYS_LOCATION("warmelt-normatch.bysl:581:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:581:/ apply.arg");
     argtab[3].bp_long = 581;
     /*_.DEBUG_MSG_FUN__V24*/ curfptr[23] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.SMATSX__V16*/ curfptr[15]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V23*/ curfptr[22] = /*_.DEBUG_MSG_FUN__V24*/ curfptr[23];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:581:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L6*/ curfnum[5] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:581:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V24*/ curfptr[23] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:581:/ cppif.else");
  /*_.IFCPP___V23*/ curfptr[22] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  BASILYS_LOCATION("warmelt-normatch.bysl:582:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:582:/ multimsend");
   /*multimsend*/{
    union basilysparam_un argtab[3];
    union basilysparam_un restab[1];
    memset(&argtab, 0, sizeof(argtab));
    memset(&restab, 0, sizeof(restab));
    BASILYS_LOCATION("warmelt-normatch.bysl:582:/ multimsend.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V3*/ curfptr[2];BASILYS_LOCATION("warmelt-normatch.bysl:582:/ multimsend.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V4*/ curfptr[3];BASILYS_LOCATION("warmelt-normatch.bysl:582:/ multimsend.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V15*/ curfptr[14];
    BASILYS_LOCATION("warmelt-normatch.bysl:582:/ multimsend.xres");
    restab[0].bp_aptr = (basilys_ptr_t*) &/*_.NBINDMATX__V27*/ curfptr[26];BASILYS_LOCATION("warmelt-normatch.bysl:582:/ multimsend.send");
    /*_.NMATX__V26*/ curfptr[25] =  basilysgc_send ((basilys_ptr_t)(/*_.SMATSX__V16*/ curfptr[15]), ((basilys_ptr_t)((/*!NORMAL_EXP*/ curfrout->tabval[6]))), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
    }
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:582:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:585:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:585:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L7*/ curfnum[5] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:585:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:585:/ apply.arg");
       argtab[0].bp_cstring =  "normexp_match nmatx";
       BASILYS_LOCATION("warmelt-normatch.bysl:585:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L7*/ curfnum[5];
       BASILYS_LOCATION("warmelt-normatch.bysl:585:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:585:/ apply.arg");
       argtab[3].bp_long = 585;
       /*_.DEBUG_MSG_FUN__V29*/ curfptr[28] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.NMATX__V26*/ curfptr[25]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V28*/ curfptr[27] = /*_.DEBUG_MSG_FUN__V29*/ curfptr[28];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:585:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L7*/ curfnum[5] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:585:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V29*/ curfptr[28] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:585:/ cppif.else");
    /*_.IFCPP___V28*/ curfptr[27] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:586:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:586:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L8*/ curfnum[5] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:586:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:586:/ apply.arg");
       argtab[0].bp_cstring =  "normexp_match scases";
       BASILYS_LOCATION("warmelt-normatch.bysl:586:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L8*/ curfnum[5];
       BASILYS_LOCATION("warmelt-normatch.bysl:586:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:586:/ apply.arg");
       argtab[3].bp_long = 586;
       /*_.DEBUG_MSG_FUN__V31*/ curfptr[30] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V30*/ curfptr[28] = /*_.DEBUG_MSG_FUN__V31*/ curfptr[30];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:586:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L8*/ curfnum[5] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:586:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V31*/ curfptr[30] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:586:/ cppif.else");
    /*_.IFCPP___V30*/ curfptr[28] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:587:/ block");
    /*block*/{
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ msend");
     /*msend*/{
      union basilysparam_un argtab[1];
      memset(&argtab, 0, sizeof(argtab));
      BASILYS_LOCATION("warmelt-normatch.bysl:587:/ ojbmsend.arg");
      argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V3*/ curfptr[2];
      /*_.CTYP__V33*/ curfptr[32] = basilysgc_send((basilys_ptr_t)(/*_.NMATX__V26*/ curfptr[25]), (basilys_ptr_t)((/*!GET_CTYPE*/ curfrout->tabval[7])), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
      }
     ;
     BASILYS_LOCATION("warmelt-normatch.bysl:588:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:588:/ rawallocobj");
      /*rawallocobj*/ { basilys_ptr_t newobj = 0;
       basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_NREP_TESTPOINT*/ curfrout->tabval[8])), (5), "CLASS_NREP_TESTPOINT");/*_.INST__V35*/ curfptr[34] =
       newobj; };
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:588:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34]))));basilys_putfield_object((/*_.INST__V35*/ curfptr[34]), (0), (/*_.SLOC__V15*/ curfptr[14]), "NREP_LOC");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:588:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34]))));basilys_putfield_object((/*_.INST__V35*/ curfptr[34]), (1), (/*_.CTYP__V33*/ curfptr[32]), "NEXPR_CTYP");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:588:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34]))));basilys_putfield_object((/*_.INST__V35*/ curfptr[34]), (4), (/*_.NMATX__V26*/ curfptr[25]), "NTSP_TEST");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:588:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34]))));basilys_putfield_object((/*_.INST__V35*/ curfptr[34]), (2), ((/*nil*/NULL)), "NTSP_FAILP");
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:588:/ putslot");
      /*putslot*/
      basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34])) == OBMAG_OBJECT);
      basilys_assertmsg("putslot checkoff", (3>=0 && 3< basilys_object_length((basilys_ptr_t)(/*_.INST__V35*/ curfptr[34]))));basilys_putfield_object((/*_.INST__V35*/ curfptr[34]), (3), ((/*nil*/NULL)), "NTSP_SUCCP");
      ;
      /*_.ROOTPOINT__V34*/ curfptr[33] = /*_.INST__V35*/ curfptr[34];;}
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:595:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:595:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L9*/ curfnum[5] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:595:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:595:/ apply.arg");
        argtab[0].bp_cstring =  "normexp_match ctyp";
        BASILYS_LOCATION("warmelt-normatch.bysl:595:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L9*/ curfnum[5];
        BASILYS_LOCATION("warmelt-normatch.bysl:595:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:595:/ apply.arg");
        argtab[3].bp_long = 595;
        /*_.DEBUG_MSG_FUN__V37*/ curfptr[36] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.CTYP__V33*/ curfptr[32]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V36*/ curfptr[35] = /*_.DEBUG_MSG_FUN__V37*/ curfptr[36];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:595:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L9*/ curfnum[5] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:595:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V37*/ curfptr[36] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:595:/ cppif.else");
     /*_.IFCPP___V36*/ curfptr[35] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     /*citerblock FOREACH_IN_MULTIPLE*/ {
     long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SCASES__V17*/ curfptr[16]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.CURCAS__V38*/ curfptr[36] = basilys_multiple_nth((basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16]), cit1__EACHTUP_ix);
/*_#IX__L10*/ curfnum[5] = cit1__EACHTUP_ix;

     
      
      #if ENABLE_CHECKING
       BASILYS_LOCATION("warmelt-normatch.bysl:599:/ cppif.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:599:/ block");
       /*block*/{
        /*_#THE_CALLCOUNT__L11*/ curfnum[10] = 
         callcount;;
        BASILYS_LOCATION("warmelt-normatch.bysl:599:/ apply");
        /*apply*/{
         union basilysparam_un argtab[4];
         memset(&argtab, 0, sizeof(argtab));
         BASILYS_LOCATION("warmelt-normatch.bysl:599:/ apply.arg");
         argtab[0].bp_cstring =  "normexp_match curcas";
         BASILYS_LOCATION("warmelt-normatch.bysl:599:/ apply.arg");
         argtab[1].bp_long = /*_#THE_CALLCOUNT__L11*/ curfnum[10];
         BASILYS_LOCATION("warmelt-normatch.bysl:599:/ apply.arg");
         argtab[2].bp_cstring =  "warmelt-normatch.bysl";
         BASILYS_LOCATION("warmelt-normatch.bysl:599:/ apply.arg");
         argtab[3].bp_long = 599;
         /*_.DEBUG_MSG_FUN__V40*/ curfptr[39] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.CURCAS__V38*/ curfptr[36]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
         }
        ;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:599:/ clear");
        /*clear*/ /*_#THE_CALLCOUNT__L11*/ curfnum[10] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:599:/ clear");
        /*clear*/ /*_.DEBUG_MSG_FUN__V40*/ curfptr[39] = 0 ;}
       
       #else /*ENABLE_CHECKING*/
       BASILYS_LOCATION("warmelt-normatch.bysl:599:/ cppif.else");
      (/*nil*/NULL)
       #endif /*ENABLE_CHECKING*/
       ;
      
      #if ENABLE_CHECKING
       BASILYS_LOCATION("warmelt-normatch.bysl:600:/ cppif.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:600:/ block");
       /*block*/{
        /*_#IS_A__L12*/ curfnum[10] = 
          basilys_is_instance_of((basilys_ptr_t)(/*_.CURCAS__V38*/ curfptr[36]), (basilys_ptr_t)((/*!CLASS_SRC_CASEMATCH*/ curfrout->tabval[9])));;
        BASILYS_LOCATION("warmelt-normatch.bysl:600:/ cond");
        /*cond*/ if (/*_#IS_A__L12*/ curfnum[10]) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:600:/ cond.then");
        /*_.IFELSE___V42*/ curfptr[41] = (/*nil*/NULL);;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:600:/ cond.else");
        
         BASILYS_LOCATION("warmelt-normatch.bysl:600:/ block");
         /*block*/{
          /*block*/{
           
           basilys_assert_failed(( "check curcas"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(600)?(600):__LINE__, __FUNCTION__);
           /*clear*/ /*_.IFELSE___V42*/ curfptr[41] = 0 ;}
          ;
          /*epilog*/}
         ;
         }
        ;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:600:/ clear");
        /*clear*/ /*_#IS_A__L12*/ curfnum[10] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:600:/ clear");
        /*clear*/ /*_.IFELSE___V42*/ curfptr[41] = 0 ;}
       
       #else /*ENABLE_CHECKING*/
       BASILYS_LOCATION("warmelt-normatch.bysl:600:/ cppif.else");
      (/*nil*/NULL)
       #endif /*ENABLE_CHECKING*/
       ;
      BASILYS_LOCATION("warmelt-normatch.bysl:601:/ block");
      /*block*/{
       BASILYS_LOCATION("warmelt-normatch.bysl:602:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURCAS__V38*/ curfptr[36]), 0, "SRC_LOC");/*_.CURLOC__V44*/ curfptr[43] = slot; };
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:603:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURCAS__V38*/ curfptr[36]), 1, "SCAM_PATT");/*_.CURPAT__V45*/ curfptr[44] = slot; };
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:604:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURCAS__V38*/ curfptr[36]), 2, "SCAM_BODY");/*_.CURBODY__V46*/ curfptr[45] = slot; };
       ;
       /*_.MAPVAR__V47*/ curfptr[46] = 
         (basilysgc_new_mapobjects( (basilysobject_ptr_t) ((/*!DISCR_MAPOBJECTS*/ curfrout->tabval[10])), (13)));;
       /*_.MAPCST__V48*/ curfptr[47] = 
         (basilysgc_new_mapobjects( (basilysobject_ptr_t) ((/*!DISCR_MAPOBJECTS*/ curfrout->tabval[10])), (11)));;
       /*_.BINDLIST__V49*/ curfptr[48] = 
        (basilysgc_new_list((basilysobject_ptr_t)((/*!DISCR_LIST*/ curfrout->tabval[5]))));;
       BASILYS_LOCATION("warmelt-normatch.bysl:608:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:608:/ rawallocobj");
        /*rawallocobj*/ { basilys_ptr_t newobj = 0;
         basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[11])), (6), "CLASS_PATTERNCONTEXT");/*_.INST__V51*/ curfptr[50] =
         newobj; };
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:608:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (0), (/*_.NCX__V4*/ curfptr[3]), "PCTN_NORMCTXT");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:608:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (1), (/*_.RECV__V2*/ curfptr[1]), "PCTN_SRC");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:608:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (2), (/*_.ENV__V3*/ curfptr[2]), "PCTN_ENV");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:608:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (3>=0 && 3< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (3), (/*_.MAPVAR__V47*/ curfptr[46]), "PCTN_MAPATVAR");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:608:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (4), (/*_.MAPCST__V48*/ curfptr[47]), "PCTN_MAPATCST");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:608:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.INST__V51*/ curfptr[50]))));basilys_putfield_object((/*_.INST__V51*/ curfptr[50]), (5), (/*_.BINDLIST__V49*/ curfptr[48]), "PCTN_BINDLIST");
        ;
        /*_.PCN__V50*/ curfptr[49] = /*_.INST__V51*/ curfptr[50];;}
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:617:/ msend");
       /*msend*/{
        union basilysparam_un argtab[3];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:617:/ ojbmsend.arg");
        argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.CURLOC__V44*/ curfptr[43];
        BASILYS_LOCATION("warmelt-normatch.bysl:617:/ ojbmsend.arg");
        argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V33*/ curfptr[32];
        BASILYS_LOCATION("warmelt-normatch.bysl:617:/ ojbmsend.arg");
        argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V50*/ curfptr[49];
        /*_.SCAN_PATTERN__V52*/ curfptr[51] = basilysgc_send((basilys_ptr_t)(/*_.CURPAT__V45*/ curfptr[44]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[12])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:618:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:618:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L13*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:618:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:618:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match mapvar";
          BASILYS_LOCATION("warmelt-normatch.bysl:618:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L13*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:618:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:618:/ apply.arg");
          argtab[3].bp_long = 618;
          /*_.DEBUG_MSG_FUN__V54*/ curfptr[53] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.MAPVAR__V47*/ curfptr[46]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V53*/ curfptr[52] = /*_.DEBUG_MSG_FUN__V54*/ curfptr[53];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:618:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L13*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:618:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V54*/ curfptr[53] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:618:/ cppif.else");
       /*_.IFCPP___V53*/ curfptr[52] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:619:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:619:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L14*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:619:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:619:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match mapcst";
          BASILYS_LOCATION("warmelt-normatch.bysl:619:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L14*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:619:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:619:/ apply.arg");
          argtab[3].bp_long = 619;
          /*_.DEBUG_MSG_FUN__V56*/ curfptr[55] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.MAPCST__V48*/ curfptr[47]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V55*/ curfptr[53] = /*_.DEBUG_MSG_FUN__V56*/ curfptr[55];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:619:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L14*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:619:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V56*/ curfptr[55] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:619:/ cppif.else");
       /*_.IFCPP___V55*/ curfptr[53] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:620:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:620:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L15*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:620:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:620:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match bindlist";
          BASILYS_LOCATION("warmelt-normatch.bysl:620:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L15*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:620:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:620:/ apply.arg");
          argtab[3].bp_long = 620;
          /*_.DEBUG_MSG_FUN__V58*/ curfptr[57] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.BINDLIST__V49*/ curfptr[48]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V57*/ curfptr[55] = /*_.DEBUG_MSG_FUN__V58*/ curfptr[57];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:620:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L15*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:620:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V58*/ curfptr[57] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:620:/ cppif.else");
       /*_.IFCPP___V57*/ curfptr[55] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       
        basilysgc_multiple_put_nth((basilys_ptr_t)(/*_.TUPVARMAP__V18*/ curfptr[17]), (/*_#IX__L10*/ curfnum[5]), (basilys_ptr_t)(/*_.MAPVAR__V47*/ curfptr[46]));
       
        basilysgc_multiple_put_nth((basilys_ptr_t)(/*_.TUPCSTMAP__V19*/ curfptr[18]), (/*_#IX__L10*/ curfnum[5]), (basilys_ptr_t)(/*_.MAPCST__V48*/ curfptr[47]));
       
        basilysgc_multiple_put_nth((basilys_ptr_t)(/*_.TUPBINDLIST__V20*/ curfptr[19]), (/*_#IX__L10*/ curfnum[5]), (basilys_ptr_t)(/*_.BINDLIST__V49*/ curfptr[48]));
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:624:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:624:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L16*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:624:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:624:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match curpat before normal_pattern";
          BASILYS_LOCATION("warmelt-normatch.bysl:624:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L16*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:624:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:624:/ apply.arg");
          argtab[3].bp_long = 624;
          /*_.DEBUG_MSG_FUN__V60*/ curfptr[59] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.CURPAT__V45*/ curfptr[44]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V59*/ curfptr[57] = /*_.DEBUG_MSG_FUN__V60*/ curfptr[59];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:624:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L16*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:624:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V60*/ curfptr[59] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:624:/ cppif.else");
       /*_.IFCPP___V59*/ curfptr[57] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       BASILYS_LOCATION("warmelt-normatch.bysl:625:/ msend");
       /*msend*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:625:/ ojbmsend.arg");
        argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NMATX__V26*/ curfptr[25];
        BASILYS_LOCATION("warmelt-normatch.bysl:625:/ ojbmsend.arg");
        argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.SPATLIST__V21*/ curfptr[20];
        BASILYS_LOCATION("warmelt-normatch.bysl:625:/ ojbmsend.arg");
        argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.TESTPTLIST__V22*/ curfptr[21];
        BASILYS_LOCATION("warmelt-normatch.bysl:625:/ ojbmsend.arg");
        argtab[3].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V50*/ curfptr[49];
        /*_.NORMAL_PATTERN__V61*/ curfptr[59] = basilysgc_send((basilys_ptr_t)(/*_.CURPAT__V45*/ curfptr[44]), (basilys_ptr_t)((/*!NORMAL_PATTERN*/ curfrout->tabval[13])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:626:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ cond");
        /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V50*/ curfptr[49]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[11])))) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ cond.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ getslot");
         { basilys_ptr_t slot=0;
          basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V50*/ curfptr[49]), 2, "PCTN_ENV");/*_.NEWENV__V63*/ curfptr[62] = slot; };
         ;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:626:/ cond.else");
        
         /*_.NEWENV__V63*/ curfptr[62] = NULL;;
         }
        ;
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:627:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:627:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L17*/ curfnum[10] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:627:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:627:/ apply.arg");
           argtab[0].bp_cstring =  "normexp_match newenv";
           BASILYS_LOCATION("warmelt-normatch.bysl:627:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L17*/ curfnum[10];
           BASILYS_LOCATION("warmelt-normatch.bysl:627:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:627:/ apply.arg");
           argtab[3].bp_long = 627;
           /*_.DEBUG_MSG_FUN__V65*/ curfptr[64] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.NEWENV__V63*/ curfptr[62]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V64*/ curfptr[63] = /*_.DEBUG_MSG_FUN__V65*/ curfptr[64];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:627:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L17*/ curfnum[10] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:627:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V65*/ curfptr[64] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:627:/ cppif.else");
        /*_.IFCPP___V64*/ curfptr[63] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:628:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:628:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L18*/ curfnum[10] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:628:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:628:/ apply.arg");
           argtab[0].bp_cstring =  "normexp_match curcas after normal_pattern";
           BASILYS_LOCATION("warmelt-normatch.bysl:628:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L18*/ curfnum[10];
           BASILYS_LOCATION("warmelt-normatch.bysl:628:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:628:/ apply.arg");
           argtab[3].bp_long = 628;
           /*_.DEBUG_MSG_FUN__V67*/ curfptr[66] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.CURCAS__V38*/ curfptr[36]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V66*/ curfptr[64] = /*_.DEBUG_MSG_FUN__V67*/ curfptr[66];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:628:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L18*/ curfnum[10] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:628:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V67*/ curfptr[66] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:628:/ cppif.else");
        /*_.IFCPP___V66*/ curfptr[64] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:629:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:629:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L19*/ curfnum[10] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:629:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:629:/ apply.arg");
           argtab[0].bp_cstring =  "normexp_match spatlist after normal_pattern";
           BASILYS_LOCATION("warmelt-normatch.bysl:629:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L19*/ curfnum[10];
           BASILYS_LOCATION("warmelt-normatch.bysl:629:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:629:/ apply.arg");
           argtab[3].bp_long = 629;
           /*_.DEBUG_MSG_FUN__V69*/ curfptr[68] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.SPATLIST__V21*/ curfptr[20]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V68*/ curfptr[66] = /*_.DEBUG_MSG_FUN__V69*/ curfptr[68];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:629:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L19*/ curfnum[10] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:629:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V69*/ curfptr[68] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:629:/ cppif.else");
        /*_.IFCPP___V68*/ curfptr[66] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:630:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:630:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L20*/ curfnum[10] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:630:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:630:/ apply.arg");
           argtab[0].bp_cstring =  "normexp_match testptlist after normal_pattern";
           BASILYS_LOCATION("warmelt-normatch.bysl:630:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L20*/ curfnum[10];
           BASILYS_LOCATION("warmelt-normatch.bysl:630:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:630:/ apply.arg");
           argtab[3].bp_long = 630;
           /*_.DEBUG_MSG_FUN__V71*/ curfptr[70] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.TESTPTLIST__V22*/ curfptr[21]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V70*/ curfptr[68] = /*_.DEBUG_MSG_FUN__V71*/ curfptr[70];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:630:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L20*/ curfnum[10] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:630:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V71*/ curfptr[70] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:630:/ cppif.else");
        /*_.IFCPP___V70*/ curfptr[68] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        BASILYS_LOCATION("warmelt-normatch.bysl:631:/ block");
        /*block*/{
         BASILYS_LOCATION("warmelt-normatch.bysl:631:/ multiapply");
         /*multiapply 4args, 1x.res*/ {
          union basilysparam_un argtab[3];
          
          union basilysparam_un restab[1];
          memset(&restab, 0, sizeof(restab));
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:631:/ multiapply.arg");
          argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NEWENV__V63*/ curfptr[62];BASILYS_LOCATION("warmelt-normatch.bysl:631:/ multiapply.arg");
          argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V4*/ curfptr[3];BASILYS_LOCATION("warmelt-normatch.bysl:631:/ multiapply.arg");
          argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V15*/ curfptr[14];
          BASILYS_LOCATION("warmelt-normatch.bysl:631:/ multiapply.xres");
          restab[0].bp_aptr = (basilys_ptr_t*) &/*_.BINDSBODY__V74*/ curfptr[73];BASILYS_LOCATION("warmelt-normatch.bysl:631:/ multiapply.appl");
          /*_.NBODY__V73*/ curfptr[72] =  basilys_apply ((basilysclosure_ptr_t)((/*!NORMALIZE_TUPLE*/ curfrout->tabval[14])), (basilys_ptr_t)(/*_.CURBODY__V46*/ curfptr[45]), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
          }
         ;
         BASILYS_LOCATION("warmelt-normatch.bysl:631:/ block");
         /*block*/{
          
          #if ENABLE_CHECKING
           BASILYS_LOCATION("warmelt-normatch.bysl:634:/ cppif.then");
          BASILYS_LOCATION("warmelt-normatch.bysl:634:/ block");
           /*block*/{
            /*_#THE_CALLCOUNT__L21*/ curfnum[10] = 
             callcount;;
            BASILYS_LOCATION("warmelt-normatch.bysl:634:/ apply");
            /*apply*/{
             union basilysparam_un argtab[4];
             memset(&argtab, 0, sizeof(argtab));
             BASILYS_LOCATION("warmelt-normatch.bysl:634:/ apply.arg");
             argtab[0].bp_cstring =  "normexp_match nbody";
             BASILYS_LOCATION("warmelt-normatch.bysl:634:/ apply.arg");
             argtab[1].bp_long = /*_#THE_CALLCOUNT__L21*/ curfnum[10];
             BASILYS_LOCATION("warmelt-normatch.bysl:634:/ apply.arg");
             argtab[2].bp_cstring =  "warmelt-normatch.bysl";
             BASILYS_LOCATION("warmelt-normatch.bysl:634:/ apply.arg");
             argtab[3].bp_long = 634;
             /*_.DEBUG_MSG_FUN__V76*/ curfptr[75] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.NBODY__V73*/ curfptr[72]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
             }
            ;
            /*_.IFCPP___V75*/ curfptr[74] = /*_.DEBUG_MSG_FUN__V76*/ curfptr[75];;
            /*epilog*/
            BASILYS_LOCATION("warmelt-normatch.bysl:634:/ clear");
            /*clear*/ /*_#THE_CALLCOUNT__L21*/ curfnum[10] = 0 ;
            BASILYS_LOCATION("warmelt-normatch.bysl:634:/ clear");
            /*clear*/ /*_.DEBUG_MSG_FUN__V76*/ curfptr[75] = 0 ;}
           
           #else /*ENABLE_CHECKING*/
           BASILYS_LOCATION("warmelt-normatch.bysl:634:/ cppif.else");
          /*_.IFCPP___V75*/ curfptr[74] = (/*nil*/NULL);
           #endif /*ENABLE_CHECKING*/
           ;
          
          #if ENABLE_CHECKING
           BASILYS_LOCATION("warmelt-normatch.bysl:635:/ cppif.then");
          BASILYS_LOCATION("warmelt-normatch.bysl:635:/ block");
           /*block*/{
            /*_#THE_CALLCOUNT__L22*/ curfnum[10] = 
             callcount;;
            BASILYS_LOCATION("warmelt-normatch.bysl:635:/ apply");
            /*apply*/{
             union basilysparam_un argtab[4];
             memset(&argtab, 0, sizeof(argtab));
             BASILYS_LOCATION("warmelt-normatch.bysl:635:/ apply.arg");
             argtab[0].bp_cstring =  "normexp_match bindsbody";
             BASILYS_LOCATION("warmelt-normatch.bysl:635:/ apply.arg");
             argtab[1].bp_long = /*_#THE_CALLCOUNT__L22*/ curfnum[10];
             BASILYS_LOCATION("warmelt-normatch.bysl:635:/ apply.arg");
             argtab[2].bp_cstring =  "warmelt-normatch.bysl";
             BASILYS_LOCATION("warmelt-normatch.bysl:635:/ apply.arg");
             argtab[3].bp_long = 635;
             /*_.DEBUG_MSG_FUN__V78*/ curfptr[77] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.BINDSBODY__V74*/ curfptr[73]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
             }
            ;
            /*_.IFCPP___V77*/ curfptr[75] = /*_.DEBUG_MSG_FUN__V78*/ curfptr[77];;
            /*epilog*/
            BASILYS_LOCATION("warmelt-normatch.bysl:635:/ clear");
            /*clear*/ /*_#THE_CALLCOUNT__L22*/ curfnum[10] = 0 ;
            BASILYS_LOCATION("warmelt-normatch.bysl:635:/ clear");
            /*clear*/ /*_.DEBUG_MSG_FUN__V78*/ curfptr[77] = 0 ;}
           
           #else /*ENABLE_CHECKING*/
           BASILYS_LOCATION("warmelt-normatch.bysl:635:/ cppif.else");
          /*_.IFCPP___V77*/ curfptr[75] = (/*nil*/NULL);
           #endif /*ENABLE_CHECKING*/
           ;
          /*_.MULTI___V72*/ curfptr[70] = (/*nil*/NULL);;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:631:/ clear");
          /*clear*/ /*_.IFCPP___V75*/ curfptr[74] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:631:/ clear");
          /*clear*/ /*_.IFCPP___V77*/ curfptr[75] = 0 ;}
         ;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:631:/ clear");
         /*clear*/ /*_.BINDSBODY__V74*/ curfptr[73] = 0 ;}
        ;
        /*_.LET___V62*/ curfptr[61] = /*_.MULTI___V72*/ curfptr[70];;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ clear");
        /*clear*/ /*_.NEWENV__V63*/ curfptr[62] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ clear");
        /*clear*/ /*_.IFCPP___V64*/ curfptr[63] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ clear");
        /*clear*/ /*_.IFCPP___V66*/ curfptr[64] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ clear");
        /*clear*/ /*_.IFCPP___V68*/ curfptr[66] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ clear");
        /*clear*/ /*_.IFCPP___V70*/ curfptr[68] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:626:/ clear");
        /*clear*/ /*_.MULTI___V72*/ curfptr[70] = 0 ;}
       ;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.CURLOC__V44*/ curfptr[43] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.CURPAT__V45*/ curfptr[44] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.CURBODY__V46*/ curfptr[45] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.MAPVAR__V47*/ curfptr[46] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.MAPCST__V48*/ curfptr[47] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.BINDLIST__V49*/ curfptr[48] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.PCN__V50*/ curfptr[49] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.SCAN_PATTERN__V52*/ curfptr[51] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.IFCPP___V53*/ curfptr[52] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.IFCPP___V55*/ curfptr[53] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.IFCPP___V57*/ curfptr[55] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.IFCPP___V59*/ curfptr[57] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.NORMAL_PATTERN__V61*/ curfptr[59] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:601:/ clear");
       /*clear*/ /*_.LET___V62*/ curfptr[61] = 0 ;}
      ;
     } /* end cit1__EACHTUP*/
     
      /*citerepilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:596:/ clear");
      /*clear*/ /*_.CURCAS__V38*/ curfptr[36] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:596:/ clear");
      /*clear*/ /*_#IX__L10*/ curfnum[5] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:596:/ clear");
      /*clear*/ /*_.IFCPP___V39*/ curfptr[38] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:596:/ clear");
      /*clear*/ /*_.IFCPP___V41*/ curfptr[39] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:596:/ clear");
      /*clear*/ /*_.LET___V43*/ curfptr[41] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:641:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:641:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L23*/ curfnum[10] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:641:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:641:/ apply.arg");
        argtab[0].bp_cstring =  "normexp_match tupvarmap";
        BASILYS_LOCATION("warmelt-normatch.bysl:641:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L23*/ curfnum[10];
        BASILYS_LOCATION("warmelt-normatch.bysl:641:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:641:/ apply.arg");
        argtab[3].bp_long = 641;
        /*_.DEBUG_MSG_FUN__V80*/ curfptr[74] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.TUPVARMAP__V18*/ curfptr[17]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V79*/ curfptr[77] = /*_.DEBUG_MSG_FUN__V80*/ curfptr[74];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:641:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L23*/ curfnum[10] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:641:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V80*/ curfptr[74] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:641:/ cppif.else");
     /*_.IFCPP___V79*/ curfptr[77] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:642:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:642:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L24*/ curfnum[10] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:642:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:642:/ apply.arg");
        argtab[0].bp_cstring =  "normexp_match tupcstmap";
        BASILYS_LOCATION("warmelt-normatch.bysl:642:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L24*/ curfnum[10];
        BASILYS_LOCATION("warmelt-normatch.bysl:642:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:642:/ apply.arg");
        argtab[3].bp_long = 642;
        /*_.DEBUG_MSG_FUN__V82*/ curfptr[72] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.TUPCSTMAP__V19*/ curfptr[18]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V81*/ curfptr[75] = /*_.DEBUG_MSG_FUN__V82*/ curfptr[72];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:642:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L24*/ curfnum[10] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:642:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V82*/ curfptr[72] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:642:/ cppif.else");
     /*_.IFCPP___V81*/ curfptr[75] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:643:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:643:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L25*/ curfnum[10] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:643:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:643:/ apply.arg");
        argtab[0].bp_cstring =  "normexp_match tupbindlist";
        BASILYS_LOCATION("warmelt-normatch.bysl:643:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L25*/ curfnum[10];
        BASILYS_LOCATION("warmelt-normatch.bysl:643:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:643:/ apply.arg");
        argtab[3].bp_long = 643;
        /*_.DEBUG_MSG_FUN__V84*/ curfptr[62] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.TUPBINDLIST__V20*/ curfptr[19]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V83*/ curfptr[73] = /*_.DEBUG_MSG_FUN__V84*/ curfptr[62];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:643:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L25*/ curfnum[10] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:643:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V84*/ curfptr[62] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:643:/ cppif.else");
     /*_.IFCPP___V83*/ curfptr[73] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:644:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:644:/ block");
      /*block*/{
       BASILYS_LOCATION("warmelt-normatch.bysl:644:/ cond");
       /*cond*/ if ((/*nil*/NULL)) /*then*/ {
       BASILYS_LOCATION("warmelt-normatch.bysl:644:/ cond.then");
       /*_.IFELSE___V86*/ curfptr[64] = (/*nil*/NULL);;
       } else {BASILYS_LOCATION("warmelt-normatch.bysl:644:/ cond.else");
       
        BASILYS_LOCATION("warmelt-normatch.bysl:644:/ block");
        /*block*/{
         /*block*/{
          
          basilys_assert_failed(( "unimplemented yet normexp_match"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(644)?(644):__LINE__, __FUNCTION__);
          /*clear*/ /*_.IFELSE___V86*/ curfptr[64] = 0 ;}
         ;
         /*epilog*/}
        ;
        }
       ;
       /*_.IFCPP___V85*/ curfptr[63] = /*_.IFELSE___V86*/ curfptr[64];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:644:/ clear");
       /*clear*/ /*_.IFELSE___V86*/ curfptr[64] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:644:/ cppif.else");
     /*_.IFCPP___V85*/ curfptr[63] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     /*_.LET___V32*/ curfptr[30] = (/*nil*/NULL);;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ clear");
     /*clear*/ /*_.CTYP__V33*/ curfptr[32] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ clear");
     /*clear*/ /*_.ROOTPOINT__V34*/ curfptr[33] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ clear");
     /*clear*/ /*_.IFCPP___V36*/ curfptr[35] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ clear");
     /*clear*/ /*_.IFCPP___V79*/ curfptr[77] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ clear");
     /*clear*/ /*_.IFCPP___V81*/ curfptr[75] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ clear");
     /*clear*/ /*_.IFCPP___V83*/ curfptr[73] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:587:/ clear");
     /*clear*/ /*_.IFCPP___V85*/ curfptr[63] = 0 ;}
    ;
    /*_.MULTI___V25*/ curfptr[23] = /*_.LET___V32*/ curfptr[30];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:582:/ clear");
    /*clear*/ /*_.IFCPP___V28*/ curfptr[27] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:582:/ clear");
    /*clear*/ /*_.IFCPP___V30*/ curfptr[28] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:582:/ clear");
    /*clear*/ /*_.LET___V32*/ curfptr[30] = 0 ;}
   ;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:582:/ clear");
   /*clear*/ /*_.NBINDMATX__V27*/ curfptr[26] = 0 ;}
  ;
  /*_.LET___V14*/ curfptr[12] = /*_.MULTI___V25*/ curfptr[23];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.SLOC__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.SMATSX__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.SCASES__V17*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_#NBCASES__L5*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.TUPVARMAP__V18*/ curfptr[17] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.TUPCSTMAP__V19*/ curfptr[18] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.TUPBINDLIST__V20*/ curfptr[19] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.SPATLIST__V21*/ curfptr[20] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.TESTPTLIST__V22*/ curfptr[21] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.IFCPP___V23*/ curfptr[22] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:571:/ clear");
  /*clear*/ /*_.MULTI___V25*/ curfptr[23] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V14*/ curfptr[12];;
  BASILYS_LOCATION("warmelt-normatch.bysl:566:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ clear");
 /*clear*/ /*_.IFCPP___V12*/ curfptr[10] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ clear");
 /*clear*/ /*_.LET___V14*/ curfptr[12] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("NORMEXP_MATCH", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_15_NORMEXP_MATCH*/



typedef 
 struct frame_start_module_basilys_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *noinitialclos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 299
  void* varptr[299];
/*no varnum*/
#define CURFRAM_NBVARNUM /*none*/0
/*others*/
  long _spare_; }
 initial_frame_st;


static void initialize_module_cdata(initial_frame_st *iniframp__, char predefinited[])
{
#define curfram__  (*iniframp__)
 
 struct cdata_st {
  struct BASILYS_OBJECT_STRUCT(3) dsym_1__CLASS_PATTERNCONTEXT;
  struct BASILYS_STRING_STRUCT(20) dstr_2__CLASS_PATTERNCON;
  struct BASILYS_OBJECT_STRUCT(3) dsym_3__CLASS_ROOT;
  struct BASILYS_STRING_STRUCT(10) dstr_4__CLASS_ROOT;
  struct BASILYS_OBJECT_STRUCT(9) dobj_5__CLASS_PATTERNCONTEXT;
  struct BASILYS_STRING_STRUCT(20) dstr_6__CLASS_PATTERNCON;
  struct BASILYS_MULTIPLE_STRUCT(1) dtup_7__CLASS_PATTERNCONTEXT;
  struct BASILYS_MULTIPLE_STRUCT(6) dtup_8__CLASS_PATTERNCONTEXT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_9__PCTN_NORMCTXT;
  struct BASILYS_STRING_STRUCT(13) dstr_10__PCTN_NORMCTXT;
  struct BASILYS_OBJECT_STRUCT(4) dobj_11__PCTN_NORMCTXT;
  struct BASILYS_STRING_STRUCT(13) dstr_12__PCTN_NORMCTXT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_13__PCTN_SRC;
  struct BASILYS_STRING_STRUCT(8) dstr_14__PCTN_SRC;
  struct BASILYS_OBJECT_STRUCT(4) dobj_15__PCTN_SRC;
  struct BASILYS_STRING_STRUCT(8) dstr_16__PCTN_SRC;
  struct BASILYS_OBJECT_STRUCT(3) dsym_17__PCTN_ENV;
  struct BASILYS_STRING_STRUCT(8) dstr_18__PCTN_ENV;
  struct BASILYS_OBJECT_STRUCT(4) dobj_19__PCTN_ENV;
  struct BASILYS_STRING_STRUCT(8) dstr_20__PCTN_ENV;
  struct BASILYS_OBJECT_STRUCT(3) dsym_21__PCTN_MAPATVAR;
  struct BASILYS_STRING_STRUCT(13) dstr_22__PCTN_MAPATVAR;
  struct BASILYS_OBJECT_STRUCT(4) dobj_23__PCTN_MAPATVAR;
  struct BASILYS_STRING_STRUCT(13) dstr_24__PCTN_MAPATVAR;
  struct BASILYS_OBJECT_STRUCT(3) dsym_25__PCTN_MAPATCST;
  struct BASILYS_STRING_STRUCT(13) dstr_26__PCTN_MAPATCST;
  struct BASILYS_OBJECT_STRUCT(4) dobj_27__PCTN_MAPATCST;
  struct BASILYS_STRING_STRUCT(13) dstr_28__PCTN_MAPATCST;
  struct BASILYS_OBJECT_STRUCT(3) dsym_29__PCTN_BINDLIST;
  struct BASILYS_STRING_STRUCT(13) dstr_30__PCTN_BINDLIST;
  struct BASILYS_OBJECT_STRUCT(4) dobj_31__PCTN_BINDLIST;
  struct BASILYS_STRING_STRUCT(13) dstr_32__PCTN_BINDLIST;
  struct BASILYS_OBJECT_STRUCT(3) dsym_33__SCAN_PATTERN;
  struct BASILYS_STRING_STRUCT(12) dstr_34__SCAN_PATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_35__CLASS_SELECTOR;
  struct BASILYS_STRING_STRUCT(14) dstr_36__CLASS_SELECTOR;
  struct BASILYS_OBJECT_STRUCT(4) dobj_37__SCAN_PATTERN;
  struct BASILYS_STRING_STRUCT(12) dstr_38__SCAN_PATTERN;
  struct BASILYS_ROUTINE_STRUCT(2) drout_39__SCANPAT_NIL;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_40__SCANPAT_NIL;
  struct BASILYS_OBJECT_STRUCT(3) dsym_41__DEBUG_MSG_FUN;
  struct BASILYS_STRING_STRUCT(13) dstr_42__DEBUG_MSG_FUN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_43__INSTALL_METHOD;
  struct BASILYS_STRING_STRUCT(14) dstr_44__INSTALL_METHOD;
  struct BASILYS_OBJECT_STRUCT(3) dsym_45__DISCR_NULLRECV;
  struct BASILYS_STRING_STRUCT(14) dstr_46__DISCR_NULLRECV;
  struct BASILYS_ROUTINE_STRUCT(3) drout_47__SCANPAT_ANYRECV;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_48__SCANPAT_ANYRECV;
  struct BASILYS_OBJECT_STRUCT(3) dsym_49__CLASS_NAMED;
  struct BASILYS_STRING_STRUCT(11) dstr_50__CLASS_NAMED;
  struct BASILYS_OBJECT_STRUCT(3) dsym_51__DISCR_ANYRECV;
  struct BASILYS_STRING_STRUCT(13) dstr_52__DISCR_ANYRECV;
  struct BASILYS_ROUTINE_STRUCT(4) drout_53__SCANPAT_ANYPATTERN;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_54__SCANPAT_ANYPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_55__CLASS_SRCPATTERN_ANY;
  struct BASILYS_STRING_STRUCT(20) dstr_56__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(4) drout_57__SCANPAT_SRCPATOR;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_58__SCANPAT_SRCPATOR;
  struct BASILYS_OBJECT_STRUCT(3) dsym_59__CLASS_SRCPATTERN_OR;
  struct BASILYS_STRING_STRUCT(19) dstr_60__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(4) drout_61__SCANPAT_SRCPATAND;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_62__SCANPAT_SRCPATAND;
  struct BASILYS_OBJECT_STRUCT(3) dsym_63__CLASS_SRCPATTERN_AND;
  struct BASILYS_STRING_STRUCT(20) dstr_64__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(3) drout_65__SCANPAT_SRCPATVAR;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_66__SCANPAT_SRCPATVAR;
  struct BASILYS_OBJECT_STRUCT(3) dsym_67__CLASS_SRCPATTERN_VARIABLE;
  struct BASILYS_STRING_STRUCT(25) dstr_68__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(3) drout_69__SCANPAT_SRCPATJOKER;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_70__SCANPAT_SRCPATJOKER;
  struct BASILYS_OBJECT_STRUCT(3) dsym_71__CLASS_SRCPATTERN_JOKERVAR;
  struct BASILYS_STRING_STRUCT(25) dstr_72__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(7) drout_73__SCANPAT_SRCPATCONST;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_74__SCANPAT_SRCPATCONST;
  struct BASILYS_OBJECT_STRUCT(3) dsym_75__CLASS_SRCPATTERN_CONSTANT;
  struct BASILYS_STRING_STRUCT(25) dstr_76__CLASS_SRCPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_77__GET_CTYPE;
  struct BASILYS_STRING_STRUCT(9) dstr_78__GET_CTYPE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_79__CLASS_CTYPE;
  struct BASILYS_STRING_STRUCT(11) dstr_80__CLASS_CTYPE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_81__NORMAL_EXP;
  struct BASILYS_STRING_STRUCT(10) dstr_82__NORMAL_EXP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_83__LIST_APPEND2LIST;
  struct BASILYS_STRING_STRUCT(16) dstr_84__LIST_APPEND2LIST;
  struct BASILYS_ROUTINE_STRUCT(9) drout_85__SCANPAT_SRCPATOBJECT;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_86__SCANPAT_SRCPATOBJECT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_87__CLASS_SRCPATTERN_OBJECT;
  struct BASILYS_STRING_STRUCT(23) dstr_88__CLASS_SRCPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_89__CLASS_CLASS;
  struct BASILYS_STRING_STRUCT(11) dstr_90__CLASS_CLASS;
  struct BASILYS_OBJECT_STRUCT(3) dsym_91__CLASS_SRCFIELDPATTERN;
  struct BASILYS_STRING_STRUCT(21) dstr_92__CLASS_SRCFIELDPA;
  struct BASILYS_OBJECT_STRUCT(3) dsym_93__CLASS_FIELD;
  struct BASILYS_STRING_STRUCT(11) dstr_94__CLASS_FIELD;
  struct BASILYS_OBJECT_STRUCT(3) dsym_95__CTYPE_VALUE;
  struct BASILYS_STRING_STRUCT(11) dstr_96__CTYPE_VALUE;
  struct BASILYS_ROUTINE_STRUCT(9) drout_97__SCANPAT_SRCPATCOMPOSITE;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_98__SCANPAT_SRCPATCOMPOSITE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_99__CLASS_SRCPATTERN_COMPOSITE;
  struct BASILYS_STRING_STRUCT(26) dstr_100__CLASS_SRCPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_101__CLASS_ANY_MATCHER;
  struct BASILYS_STRING_STRUCT(17) dstr_102__CLASS_ANY_MATCHE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_103__CLASS_FORMAL_BINDING;
  struct BASILYS_STRING_STRUCT(20) dstr_104__CLASS_FORMAL_BIN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_105__NORMALIZE_TUPLE;
  struct BASILYS_STRING_STRUCT(15) dstr_106__NORMALIZE_TUPLE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_107__CLASS_NREP_BACKPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_108__CLASS_NREP_BACKP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_109__CLASS_NREP_TYPEXPR;
  struct BASILYS_STRING_STRUCT(18) dstr_110__CLASS_NREP_TYPEX;
  struct BASILYS_OBJECT_STRUCT(9) dobj_111__CLASS_NREP_BACKPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_112__CLASS_NREP_BACKP;
  struct BASILYS_MULTIPLE_STRUCT(4) dtup_113__CLASS_NREP_BACKPOINT;
  struct BASILYS_MULTIPLE_STRUCT(4) dtup_114__CLASS_NREP_BACKPOINT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_115__NBKP_DO;
  struct BASILYS_STRING_STRUCT(7) dstr_116__NBKP_DO;
  struct BASILYS_OBJECT_STRUCT(4) dobj_117__NBKP_DO;
  struct BASILYS_STRING_STRUCT(7) dstr_118__NBKP_DO;
  struct BASILYS_OBJECT_STRUCT(3) dsym_119__NBKP_FAIL;
  struct BASILYS_STRING_STRUCT(9) dstr_120__NBKP_FAIL;
  struct BASILYS_OBJECT_STRUCT(4) dobj_121__NBKP_FAIL;
  struct BASILYS_STRING_STRUCT(9) dstr_122__NBKP_FAIL;
  struct BASILYS_OBJECT_STRUCT(3) dsym_123__CLASS_NREP_TESTPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_124__CLASS_NREP_TESTP;
  struct BASILYS_OBJECT_STRUCT(9) dobj_125__CLASS_NREP_TESTPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_126__CLASS_NREP_TESTP;
  struct BASILYS_MULTIPLE_STRUCT(4) dtup_127__CLASS_NREP_TESTPOINT;
  struct BASILYS_MULTIPLE_STRUCT(5) dtup_128__CLASS_NREP_TESTPOINT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_129__NTSP_FAILP;
  struct BASILYS_STRING_STRUCT(10) dstr_130__NTSP_FAILP;
  struct BASILYS_OBJECT_STRUCT(4) dobj_131__NTSP_FAILP;
  struct BASILYS_STRING_STRUCT(10) dstr_132__NTSP_FAILP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_133__NTSP_SUCCP;
  struct BASILYS_STRING_STRUCT(10) dstr_134__NTSP_SUCCP;
  struct BASILYS_OBJECT_STRUCT(4) dobj_135__NTSP_SUCCP;
  struct BASILYS_STRING_STRUCT(10) dstr_136__NTSP_SUCCP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_137__NTSP_TEST;
  struct BASILYS_STRING_STRUCT(9) dstr_138__NTSP_TEST;
  struct BASILYS_OBJECT_STRUCT(4) dobj_139__NTSP_TEST;
  struct BASILYS_STRING_STRUCT(9) dstr_140__NTSP_TEST;
  struct BASILYS_OBJECT_STRUCT(3) dsym_141__CLASS_NREP_TESTP_MATCHER;
  struct BASILYS_STRING_STRUCT(24) dstr_142__CLASS_NREP_TESTP;
  struct BASILYS_OBJECT_STRUCT(9) dobj_143__CLASS_NREP_TESTP_MATCHER;
  struct BASILYS_STRING_STRUCT(24) dstr_144__CLASS_NREP_TESTP;
  struct BASILYS_MULTIPLE_STRUCT(5) dtup_145__CLASS_NREP_TESTP_MATCHER;
  struct BASILYS_MULTIPLE_STRUCT(7) dtup_146__CLASS_NREP_TESTP_MATCHER;
  struct BASILYS_OBJECT_STRUCT(3) dsym_147__NTSM_MATCHER;
  struct BASILYS_STRING_STRUCT(12) dstr_148__NTSM_MATCHER;
  struct BASILYS_OBJECT_STRUCT(4) dobj_149__NTSM_MATCHER;
  struct BASILYS_STRING_STRUCT(12) dstr_150__NTSM_MATCHER;
  struct BASILYS_OBJECT_STRUCT(3) dsym_151__NTSM_BINDS;
  struct BASILYS_STRING_STRUCT(10) dstr_152__NTSM_BINDS;
  struct BASILYS_OBJECT_STRUCT(4) dobj_153__NTSM_BINDS;
  struct BASILYS_STRING_STRUCT(10) dstr_154__NTSM_BINDS;
  struct BASILYS_OBJECT_STRUCT(3) dsym_155__CLASS_NREP_TESTP_ISA;
  struct BASILYS_STRING_STRUCT(20) dstr_156__CLASS_NREP_TESTP;
  struct BASILYS_OBJECT_STRUCT(9) dobj_157__CLASS_NREP_TESTP_ISA;
  struct BASILYS_STRING_STRUCT(20) dstr_158__CLASS_NREP_TESTP;
  struct BASILYS_MULTIPLE_STRUCT(5) dtup_159__CLASS_NREP_TESTP_ISA;
  struct BASILYS_MULTIPLE_STRUCT(6) dtup_160__CLASS_NREP_TESTP_ISA;
  struct BASILYS_OBJECT_STRUCT(3) dsym_161__NTSI_CLASS;
  struct BASILYS_STRING_STRUCT(10) dstr_162__NTSI_CLASS;
  struct BASILYS_OBJECT_STRUCT(4) dobj_163__NTSI_CLASS;
  struct BASILYS_STRING_STRUCT(10) dstr_164__NTSI_CLASS;
  struct BASILYS_OBJECT_STRUCT(3) dsym_165__CLASS_NREP_TESTP_DISCRIM;
  struct BASILYS_STRING_STRUCT(24) dstr_166__CLASS_NREP_TESTP;
  struct BASILYS_OBJECT_STRUCT(9) dobj_167__CLASS_NREP_TESTP_DISCRIM;
  struct BASILYS_STRING_STRUCT(24) dstr_168__CLASS_NREP_TESTP;
  struct BASILYS_MULTIPLE_STRUCT(6) dtup_169__CLASS_NREP_TESTP_DISCRIM;
  struct BASILYS_MULTIPLE_STRUCT(6) dtup_170__CLASS_NREP_TESTP_DISCRIM;
  struct BASILYS_OBJECT_STRUCT(3) dsym_171__CLASS_NREP_TESTP_COND;
  struct BASILYS_STRING_STRUCT(21) dstr_172__CLASS_NREP_TESTP;
  struct BASILYS_OBJECT_STRUCT(9) dobj_173__CLASS_NREP_TESTP_COND;
  struct BASILYS_STRING_STRUCT(21) dstr_174__CLASS_NREP_TESTP;
  struct BASILYS_MULTIPLE_STRUCT(5) dtup_175__CLASS_NREP_TESTP_COND;
  struct BASILYS_MULTIPLE_STRUCT(6) dtup_176__CLASS_NREP_TESTP_COND;
  struct BASILYS_OBJECT_STRUCT(3) dsym_177__NTSC_COND;
  struct BASILYS_STRING_STRUCT(9) dstr_178__NTSC_COND;
  struct BASILYS_OBJECT_STRUCT(4) dobj_179__NTSC_COND;
  struct BASILYS_STRING_STRUCT(9) dstr_180__NTSC_COND;
  struct BASILYS_OBJECT_STRUCT(3) dsym_181__NORMAL_PATTERN;
  struct BASILYS_STRING_STRUCT(14) dstr_182__NORMAL_PATTERN;
  struct BASILYS_OBJECT_STRUCT(4) dobj_183__NORMAL_PATTERN;
  struct BASILYS_STRING_STRUCT(14) dstr_184__NORMAL_PATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_185__ADD_WHEN_PATTERN;
  struct BASILYS_STRING_STRUCT(16) dstr_186__ADD_WHEN_PATTERN;
  struct BASILYS_OBJECT_STRUCT(4) dobj_187__ADD_WHEN_PATTERN;
  struct BASILYS_STRING_STRUCT(16) dstr_188__ADD_WHEN_PATTERN;
  struct BASILYS_ROUTINE_STRUCT(4) drout_189__NORMPAT_ANYRECV;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_190__NORMPAT_ANYRECV;
  struct BASILYS_OBJECT_STRUCT(3) dsym_191__CLASS_SRC;
  struct BASILYS_STRING_STRUCT(9) dstr_192__CLASS_SRC;
  struct BASILYS_ROUTINE_STRUCT(5) drout_193__NORMPAT_ANYPAT;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_194__NORMPAT_ANYPAT;
  struct BASILYS_ROUTINE_STRUCT(4) drout_195__NORMPAT_INSTANCEPAT;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_196__NORMPAT_INSTANCEPAT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_197__CLASS_SRCPATTERN_INSTANCE;
  struct BASILYS_STRING_STRUCT(25) dstr_198__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(19) drout_199__NORMPAT_ANYMATCHPAT;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_200__NORMPAT_ANYMATCHPAT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_201__CLASS_SRCPATTERN_ANYMATCH;
  struct BASILYS_STRING_STRUCT(25) dstr_202__CLASS_SRCPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_203__FRESH_ENV;
  struct BASILYS_STRING_STRUCT(9) dstr_204__FRESH_ENV;
  struct BASILYS_OBJECT_STRUCT(3) dsym_205__CLASS_ANY_BINDING;
  struct BASILYS_STRING_STRUCT(17) dstr_206__CLASS_ANY_BINDIN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_207__CLASS_NORMLET_BINDING;
  struct BASILYS_STRING_STRUCT(21) dstr_208__CLASS_NORMLET_BI;
  struct BASILYS_OBJECT_STRUCT(3) dsym_209__PUT_ENV;
  struct BASILYS_STRING_STRUCT(7) dstr_210__PUT_ENV;
  struct BASILYS_OBJECT_STRUCT(3) dsym_211__CLONE_SYMBOL;
  struct BASILYS_STRING_STRUCT(12) dstr_212__CLONE_SYMBOL;
  struct BASILYS_OBJECT_STRUCT(3) dsym_213__CLASS_NREP_LOCSYMOCC;
  struct BASILYS_STRING_STRUCT(20) dstr_214__CLASS_NREP_LOCSY;
  struct BASILYS_ROUTINE_STRUCT(15) drout_215__NORMEXP_MATCH;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_216__NORMEXP_MATCH;
  struct BASILYS_OBJECT_STRUCT(3) dsym_217__CLASS_SRC_MATCH;
  struct BASILYS_STRING_STRUCT(15) dstr_218__CLASS_SRC_MATCH;
  struct BASILYS_OBJECT_STRUCT(3) dsym_219__CLASS_ENVIRONMENT;
  struct BASILYS_STRING_STRUCT(17) dstr_220__CLASS_ENVIRONMEN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_221__CLASS_NORMCONTEXT;
  struct BASILYS_STRING_STRUCT(17) dstr_222__CLASS_NORMCONTEX;
  struct BASILYS_OBJECT_STRUCT(3) dsym_223__DISCR_MULTIPLE;
  struct BASILYS_STRING_STRUCT(14) dstr_224__DISCR_MULTIPLE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_225__DISCR_LIST;
  struct BASILYS_STRING_STRUCT(10) dstr_226__DISCR_LIST;
  struct BASILYS_OBJECT_STRUCT(3) dsym_227__CLASS_SRC_CASEMATCH;
  struct BASILYS_STRING_STRUCT(19) dstr_228__CLASS_SRC_CASEMA;
  struct BASILYS_OBJECT_STRUCT(3) dsym_229__DISCR_MAPOBJECTS;
  struct BASILYS_STRING_STRUCT(16) dstr_230__DISCR_MAPOBJECTS;
 long spare_;
} *cdat = NULL;
 cdat = (struct cdata_st*) basilysgc_allocate(sizeof(*cdat),0);
  basilys_prohibit_garbcoll = TRUE;
 /*initial routine predef*/
 /*initial routine fill*/
 
 /*iniobj dsym_1__CLASS_PATTERNCONTEXT*/
 if (!/*_.VALDATA___V50*/ curfptr[49]) /*_.VALDATA___V50*/ curfptr[49] = (void*)&cdat->dsym_1__CLASS_PATTERNCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dsym_1__CLASS_PATTERNCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_hash = 411470679;
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_1__CLASS_PATTERNCONTEXT));
 
 /*inistring dstr_2__CLASS_PATTERNCON*/
 /*_.VALSTR___V51*/ curfptr[50] = (void*)&cdat->dstr_2__CLASS_PATTERNCON;
  cdat->dstr_2__CLASS_PATTERNCON.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_2__CLASS_PATTERNCON.val, "CLASS_PATTERNCONTEXT");
 
 /*iniobj dsym_3__CLASS_ROOT*/
 if (!/*_.VALDATA___V52*/ curfptr[51]) /*_.VALDATA___V52*/ curfptr[51] = (void*)&cdat->dsym_3__CLASS_ROOT;
  basilys_assertmsg("iniobj checkdiscr dsym_3__CLASS_ROOT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_3__CLASS_ROOT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_3__CLASS_ROOT.obj_hash = 60568825;
  cdat->dsym_3__CLASS_ROOT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_3__CLASS_ROOT));
 
 /*inistring dstr_4__CLASS_ROOT*/
 /*_.VALSTR___V53*/ curfptr[52] = (void*)&cdat->dstr_4__CLASS_ROOT;
  cdat->dstr_4__CLASS_ROOT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_4__CLASS_ROOT.val, "CLASS_ROOT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_5__CLASS_PATTERNCONTEXT*/
 if (!/*_.VALDATA___V54*/ curfptr[53]) /*_.VALDATA___V54*/ curfptr[53] = (void*)&cdat->dobj_5__CLASS_PATTERNCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dobj_5__CLASS_PATTERNCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_num = OBMAG_OBJECT;
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_hash = 93108985;
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_5__CLASS_PATTERNCONTEXT));
 
 /*inistring dstr_6__CLASS_PATTERNCON*/
 /*_.VALSTR___V55*/ curfptr[54] = (void*)&cdat->dstr_6__CLASS_PATTERNCON;
  cdat->dstr_6__CLASS_PATTERNCON.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_6__CLASS_PATTERNCON.val, "CLASS_PATTERNCONTEXT");
 
 /*inimult dtup_7__CLASS_PATTERNCONTEXT*/
 /*_.VALTUP___V56*/ curfptr[55] = (void*)&cdat->dtup_7__CLASS_PATTERNCONTEXT;
  cdat->dtup_7__CLASS_PATTERNCONTEXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_7__CLASS_PATTERNCONTEXT.nbval = 1;
 /*inimult dtup_8__CLASS_PATTERNCONTEXT*/
 /*_.VALTUP___V69*/ curfptr[68] = (void*)&cdat->dtup_8__CLASS_PATTERNCONTEXT;
  cdat->dtup_8__CLASS_PATTERNCONTEXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_8__CLASS_PATTERNCONTEXT.nbval = 6;
 /*iniobj dsym_9__PCTN_NORMCTXT*/
 if (!/*_.VALDATA___V70*/ curfptr[69]) /*_.VALDATA___V70*/ curfptr[69] = (void*)&cdat->dsym_9__PCTN_NORMCTXT;
  basilys_assertmsg("iniobj checkdiscr dsym_9__PCTN_NORMCTXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_9__PCTN_NORMCTXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_9__PCTN_NORMCTXT.obj_hash = 955957534;
  cdat->dsym_9__PCTN_NORMCTXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_9__PCTN_NORMCTXT));
 
 /*inistring dstr_10__PCTN_NORMCTXT*/
 /*_.VALSTR___V71*/ curfptr[70] = (void*)&cdat->dstr_10__PCTN_NORMCTXT;
  cdat->dstr_10__PCTN_NORMCTXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_10__PCTN_NORMCTXT.val, "PCTN_NORMCTXT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_11__PCTN_NORMCTXT*/
 if (!/*_.VALDATA___V57*/ curfptr[56]) /*_.VALDATA___V57*/ curfptr[56] = (void*)&cdat->dobj_11__PCTN_NORMCTXT;
  basilys_assertmsg("iniobj checkdiscr dobj_11__PCTN_NORMCTXT", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_11__PCTN_NORMCTXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_11__PCTN_NORMCTXT.obj_num = 0;
  cdat->dobj_11__PCTN_NORMCTXT.obj_hash = 134524560;
  cdat->dobj_11__PCTN_NORMCTXT.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_11__PCTN_NORMCTXT));
 
 /*inistring dstr_12__PCTN_NORMCTXT*/
 /*_.VALSTR___V58*/ curfptr[57] = (void*)&cdat->dstr_12__PCTN_NORMCTXT;
  cdat->dstr_12__PCTN_NORMCTXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_12__PCTN_NORMCTXT.val, "PCTN_NORMCTXT");
 
 /*iniobj dsym_13__PCTN_SRC*/
 if (!/*_.VALDATA___V72*/ curfptr[71]) /*_.VALDATA___V72*/ curfptr[71] = (void*)&cdat->dsym_13__PCTN_SRC;
  basilys_assertmsg("iniobj checkdiscr dsym_13__PCTN_SRC", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_13__PCTN_SRC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_13__PCTN_SRC.obj_hash = 902509187;
  cdat->dsym_13__PCTN_SRC.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_13__PCTN_SRC));
 
 /*inistring dstr_14__PCTN_SRC*/
 /*_.VALSTR___V73*/ curfptr[72] = (void*)&cdat->dstr_14__PCTN_SRC;
  cdat->dstr_14__PCTN_SRC.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_14__PCTN_SRC.val, "PCTN_SRC");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_15__PCTN_SRC*/
 if (!/*_.VALDATA___V59*/ curfptr[58]) /*_.VALDATA___V59*/ curfptr[58] = (void*)&cdat->dobj_15__PCTN_SRC;
  basilys_assertmsg("iniobj checkdiscr dobj_15__PCTN_SRC", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_15__PCTN_SRC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_15__PCTN_SRC.obj_num = 1;
  cdat->dobj_15__PCTN_SRC.obj_hash = 448669600;
  cdat->dobj_15__PCTN_SRC.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_15__PCTN_SRC));
 
 /*inistring dstr_16__PCTN_SRC*/
 /*_.VALSTR___V60*/ curfptr[59] = (void*)&cdat->dstr_16__PCTN_SRC;
  cdat->dstr_16__PCTN_SRC.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_16__PCTN_SRC.val, "PCTN_SRC");
 
 /*iniobj dsym_17__PCTN_ENV*/
 if (!/*_.VALDATA___V74*/ curfptr[73]) /*_.VALDATA___V74*/ curfptr[73] = (void*)&cdat->dsym_17__PCTN_ENV;
  basilys_assertmsg("iniobj checkdiscr dsym_17__PCTN_ENV", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_17__PCTN_ENV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_17__PCTN_ENV.obj_hash = 327113752;
  cdat->dsym_17__PCTN_ENV.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_17__PCTN_ENV));
 
 /*inistring dstr_18__PCTN_ENV*/
 /*_.VALSTR___V75*/ curfptr[74] = (void*)&cdat->dstr_18__PCTN_ENV;
  cdat->dstr_18__PCTN_ENV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_18__PCTN_ENV.val, "PCTN_ENV");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_19__PCTN_ENV*/
 if (!/*_.VALDATA___V61*/ curfptr[60]) /*_.VALDATA___V61*/ curfptr[60] = (void*)&cdat->dobj_19__PCTN_ENV;
  basilys_assertmsg("iniobj checkdiscr dobj_19__PCTN_ENV", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_19__PCTN_ENV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_19__PCTN_ENV.obj_num = 2;
  cdat->dobj_19__PCTN_ENV.obj_hash = 928690259;
  cdat->dobj_19__PCTN_ENV.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_19__PCTN_ENV));
 
 /*inistring dstr_20__PCTN_ENV*/
 /*_.VALSTR___V62*/ curfptr[61] = (void*)&cdat->dstr_20__PCTN_ENV;
  cdat->dstr_20__PCTN_ENV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_20__PCTN_ENV.val, "PCTN_ENV");
 
 /*iniobj dsym_21__PCTN_MAPATVAR*/
 if (!/*_.VALDATA___V76*/ curfptr[75]) /*_.VALDATA___V76*/ curfptr[75] = (void*)&cdat->dsym_21__PCTN_MAPATVAR;
  basilys_assertmsg("iniobj checkdiscr dsym_21__PCTN_MAPATVAR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_21__PCTN_MAPATVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_21__PCTN_MAPATVAR.obj_hash = 620129723;
  cdat->dsym_21__PCTN_MAPATVAR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_21__PCTN_MAPATVAR));
 
 /*inistring dstr_22__PCTN_MAPATVAR*/
 /*_.VALSTR___V77*/ curfptr[76] = (void*)&cdat->dstr_22__PCTN_MAPATVAR;
  cdat->dstr_22__PCTN_MAPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_22__PCTN_MAPATVAR.val, "PCTN_MAPATVAR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_23__PCTN_MAPATVAR*/
 if (!/*_.VALDATA___V63*/ curfptr[62]) /*_.VALDATA___V63*/ curfptr[62] = (void*)&cdat->dobj_23__PCTN_MAPATVAR;
  basilys_assertmsg("iniobj checkdiscr dobj_23__PCTN_MAPATVAR", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_23__PCTN_MAPATVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_23__PCTN_MAPATVAR.obj_num = 3;
  cdat->dobj_23__PCTN_MAPATVAR.obj_hash = 425412870;
  cdat->dobj_23__PCTN_MAPATVAR.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_23__PCTN_MAPATVAR));
 
 /*inistring dstr_24__PCTN_MAPATVAR*/
 /*_.VALSTR___V64*/ curfptr[63] = (void*)&cdat->dstr_24__PCTN_MAPATVAR;
  cdat->dstr_24__PCTN_MAPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_24__PCTN_MAPATVAR.val, "PCTN_MAPATVAR");
 
 /*iniobj dsym_25__PCTN_MAPATCST*/
 if (!/*_.VALDATA___V78*/ curfptr[77]) /*_.VALDATA___V78*/ curfptr[77] = (void*)&cdat->dsym_25__PCTN_MAPATCST;
  basilys_assertmsg("iniobj checkdiscr dsym_25__PCTN_MAPATCST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_25__PCTN_MAPATCST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_25__PCTN_MAPATCST.obj_hash = 626578931;
  cdat->dsym_25__PCTN_MAPATCST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_25__PCTN_MAPATCST));
 
 /*inistring dstr_26__PCTN_MAPATCST*/
 /*_.VALSTR___V79*/ curfptr[78] = (void*)&cdat->dstr_26__PCTN_MAPATCST;
  cdat->dstr_26__PCTN_MAPATCST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_26__PCTN_MAPATCST.val, "PCTN_MAPATCST");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_27__PCTN_MAPATCST*/
 if (!/*_.VALDATA___V65*/ curfptr[64]) /*_.VALDATA___V65*/ curfptr[64] = (void*)&cdat->dobj_27__PCTN_MAPATCST;
  basilys_assertmsg("iniobj checkdiscr dobj_27__PCTN_MAPATCST", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_27__PCTN_MAPATCST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_27__PCTN_MAPATCST.obj_num = 4;
  cdat->dobj_27__PCTN_MAPATCST.obj_hash = 355022041;
  cdat->dobj_27__PCTN_MAPATCST.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_27__PCTN_MAPATCST));
 
 /*inistring dstr_28__PCTN_MAPATCST*/
 /*_.VALSTR___V66*/ curfptr[65] = (void*)&cdat->dstr_28__PCTN_MAPATCST;
  cdat->dstr_28__PCTN_MAPATCST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_28__PCTN_MAPATCST.val, "PCTN_MAPATCST");
 
 /*iniobj dsym_29__PCTN_BINDLIST*/
 if (!/*_.VALDATA___V80*/ curfptr[79]) /*_.VALDATA___V80*/ curfptr[79] = (void*)&cdat->dsym_29__PCTN_BINDLIST;
  basilys_assertmsg("iniobj checkdiscr dsym_29__PCTN_BINDLIST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_29__PCTN_BINDLIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_29__PCTN_BINDLIST.obj_hash = 47093239;
  cdat->dsym_29__PCTN_BINDLIST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_29__PCTN_BINDLIST));
 
 /*inistring dstr_30__PCTN_BINDLIST*/
 /*_.VALSTR___V81*/ curfptr[80] = (void*)&cdat->dstr_30__PCTN_BINDLIST;
  cdat->dstr_30__PCTN_BINDLIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_30__PCTN_BINDLIST.val, "PCTN_BINDLIST");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_31__PCTN_BINDLIST*/
 if (!/*_.VALDATA___V67*/ curfptr[66]) /*_.VALDATA___V67*/ curfptr[66] = (void*)&cdat->dobj_31__PCTN_BINDLIST;
  basilys_assertmsg("iniobj checkdiscr dobj_31__PCTN_BINDLIST", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_31__PCTN_BINDLIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_31__PCTN_BINDLIST.obj_num = 5;
  cdat->dobj_31__PCTN_BINDLIST.obj_hash = 676163371;
  cdat->dobj_31__PCTN_BINDLIST.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_31__PCTN_BINDLIST));
 
 /*inistring dstr_32__PCTN_BINDLIST*/
 /*_.VALSTR___V68*/ curfptr[67] = (void*)&cdat->dstr_32__PCTN_BINDLIST;
  cdat->dstr_32__PCTN_BINDLIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_32__PCTN_BINDLIST.val, "PCTN_BINDLIST");
 
 /*iniobj dsym_33__SCAN_PATTERN*/
 if (!/*_.VALDATA___V82*/ curfptr[81]) /*_.VALDATA___V82*/ curfptr[81] = (void*)&cdat->dsym_33__SCAN_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dsym_33__SCAN_PATTERN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_33__SCAN_PATTERN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_33__SCAN_PATTERN.obj_hash = 97353985;
  cdat->dsym_33__SCAN_PATTERN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_33__SCAN_PATTERN));
 
 /*inistring dstr_34__SCAN_PATTERN*/
 /*_.VALSTR___V83*/ curfptr[82] = (void*)&cdat->dstr_34__SCAN_PATTERN;
  cdat->dstr_34__SCAN_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_34__SCAN_PATTERN.val, "SCAN_PATTERN");
 
 /*iniobj dsym_35__CLASS_SELECTOR*/
 if (!/*_.VALDATA___V84*/ curfptr[83]) /*_.VALDATA___V84*/ curfptr[83] = (void*)&cdat->dsym_35__CLASS_SELECTOR;
  basilys_assertmsg("iniobj checkdiscr dsym_35__CLASS_SELECTOR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_35__CLASS_SELECTOR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_35__CLASS_SELECTOR.obj_hash = 100646971;
  cdat->dsym_35__CLASS_SELECTOR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_35__CLASS_SELECTOR));
 
 /*inistring dstr_36__CLASS_SELECTOR*/
 /*_.VALSTR___V85*/ curfptr[84] = (void*)&cdat->dstr_36__CLASS_SELECTOR;
  cdat->dstr_36__CLASS_SELECTOR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_36__CLASS_SELECTOR.val, "CLASS_SELECTOR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:52:/ iniobj");
 /*iniobj dobj_37__SCAN_PATTERN*/
 if (!/*_.VALDATA___V86*/ curfptr[85]) /*_.VALDATA___V86*/ curfptr[85] = (void*)&cdat->dobj_37__SCAN_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dobj_37__SCAN_PATTERN", NULL != (void*)/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_37__SCAN_PATTERN.obj_class = (basilysobject_ptr_t)(/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_37__SCAN_PATTERN.obj_hash = 1000518790;
  cdat->dobj_37__SCAN_PATTERN.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_37__SCAN_PATTERN));
 
 /*inistring dstr_38__SCAN_PATTERN*/
 /*_.VALSTR___V87*/ curfptr[86] = (void*)&cdat->dstr_38__SCAN_PATTERN;
  cdat->dstr_38__SCAN_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_38__SCAN_PATTERN.val, "SCAN_PATTERN");
 
 /*inirout drout_39__SCANPAT_NIL*/
 /*_.VALROUT___V88*/ curfptr[87] = (void*)&cdat->drout_39__SCANPAT_NIL;
  cdat->drout_39__SCANPAT_NIL.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_39__SCANPAT_NIL.routdescr, "SCANPAT_NIL @warmelt-normatch.bysl:59",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_39__SCANPAT_NIL.nbval = 2;
 *(basilysroutfun_t **) (cdat->drout_39__SCANPAT_NIL.routaddr) = rout_1_SCANPAT_NIL;
 
 /*iniclos dclo_40__SCANPAT_NIL*/
 /*_.VALCLO___V89*/ curfptr[88] = (void*)&cdat->dclo_40__SCANPAT_NIL;
  cdat->dclo_40__SCANPAT_NIL.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_40__SCANPAT_NIL.nbval = 0;
 
 /*iniobj dsym_41__DEBUG_MSG_FUN*/
 if (!/*_.VALDATA___V90*/ curfptr[89]) /*_.VALDATA___V90*/ curfptr[89] = (void*)&cdat->dsym_41__DEBUG_MSG_FUN;
  basilys_assertmsg("iniobj checkdiscr dsym_41__DEBUG_MSG_FUN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_41__DEBUG_MSG_FUN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_41__DEBUG_MSG_FUN.obj_hash = 938829072;
  cdat->dsym_41__DEBUG_MSG_FUN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_41__DEBUG_MSG_FUN));
 
 /*inistring dstr_42__DEBUG_MSG_FUN*/
 /*_.VALSTR___V91*/ curfptr[90] = (void*)&cdat->dstr_42__DEBUG_MSG_FUN;
  cdat->dstr_42__DEBUG_MSG_FUN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_42__DEBUG_MSG_FUN.val, "DEBUG_MSG_FUN");
 
 /*iniobj dsym_43__INSTALL_METHOD*/
 if (!/*_.VALDATA___V92*/ curfptr[91]) /*_.VALDATA___V92*/ curfptr[91] = (void*)&cdat->dsym_43__INSTALL_METHOD;
  basilys_assertmsg("iniobj checkdiscr dsym_43__INSTALL_METHOD", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_43__INSTALL_METHOD.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_43__INSTALL_METHOD.obj_hash = 680699224;
  cdat->dsym_43__INSTALL_METHOD.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_43__INSTALL_METHOD));
 
 /*inistring dstr_44__INSTALL_METHOD*/
 /*_.VALSTR___V93*/ curfptr[92] = (void*)&cdat->dstr_44__INSTALL_METHOD;
  cdat->dstr_44__INSTALL_METHOD.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_44__INSTALL_METHOD.val, "INSTALL_METHOD");
 
 /*iniobj dsym_45__DISCR_NULLRECV*/
 if (!/*_.VALDATA___V94*/ curfptr[93]) /*_.VALDATA___V94*/ curfptr[93] = (void*)&cdat->dsym_45__DISCR_NULLRECV;
  basilys_assertmsg("iniobj checkdiscr dsym_45__DISCR_NULLRECV", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_45__DISCR_NULLRECV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_45__DISCR_NULLRECV.obj_hash = 39603699;
  cdat->dsym_45__DISCR_NULLRECV.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_45__DISCR_NULLRECV));
 
 /*inistring dstr_46__DISCR_NULLRECV*/
 /*_.VALSTR___V95*/ curfptr[94] = (void*)&cdat->dstr_46__DISCR_NULLRECV;
  cdat->dstr_46__DISCR_NULLRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_46__DISCR_NULLRECV.val, "DISCR_NULLRECV");
 
 /*inirout drout_47__SCANPAT_ANYRECV*/
 /*_.VALROUT___V96*/ curfptr[95] = (void*)&cdat->drout_47__SCANPAT_ANYRECV;
  cdat->drout_47__SCANPAT_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_47__SCANPAT_ANYRECV.routdescr, "SCANPAT_ANYRECV @warmelt-normatch.bysl:69",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_47__SCANPAT_ANYRECV.nbval = 3;
 *(basilysroutfun_t **) (cdat->drout_47__SCANPAT_ANYRECV.routaddr) = rout_2_SCANPAT_ANYRECV;
 
 /*iniclos dclo_48__SCANPAT_ANYRECV*/
 /*_.VALCLO___V97*/ curfptr[96] = (void*)&cdat->dclo_48__SCANPAT_ANYRECV;
  cdat->dclo_48__SCANPAT_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_48__SCANPAT_ANYRECV.nbval = 0;
 
 /*iniobj dsym_49__CLASS_NAMED*/
 if (!/*_.VALDATA___V98*/ curfptr[97]) /*_.VALDATA___V98*/ curfptr[97] = (void*)&cdat->dsym_49__CLASS_NAMED;
  basilys_assertmsg("iniobj checkdiscr dsym_49__CLASS_NAMED", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_49__CLASS_NAMED.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_49__CLASS_NAMED.obj_hash = 60407004;
  cdat->dsym_49__CLASS_NAMED.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_49__CLASS_NAMED));
 
 /*inistring dstr_50__CLASS_NAMED*/
 /*_.VALSTR___V99*/ curfptr[98] = (void*)&cdat->dstr_50__CLASS_NAMED;
  cdat->dstr_50__CLASS_NAMED.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_50__CLASS_NAMED.val, "CLASS_NAMED");
 
 /*iniobj dsym_51__DISCR_ANYRECV*/
 if (!/*_.VALDATA___V100*/ curfptr[99]) /*_.VALDATA___V100*/ curfptr[99] = (void*)&cdat->dsym_51__DISCR_ANYRECV;
  basilys_assertmsg("iniobj checkdiscr dsym_51__DISCR_ANYRECV", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_51__DISCR_ANYRECV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_51__DISCR_ANYRECV.obj_hash = 88765237;
  cdat->dsym_51__DISCR_ANYRECV.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_51__DISCR_ANYRECV));
 
 /*inistring dstr_52__DISCR_ANYRECV*/
 /*_.VALSTR___V101*/ curfptr[100] = (void*)&cdat->dstr_52__DISCR_ANYRECV;
  cdat->dstr_52__DISCR_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_52__DISCR_ANYRECV.val, "DISCR_ANYRECV");
 
 /*inirout drout_53__SCANPAT_ANYPATTERN*/
 /*_.VALROUT___V102*/ curfptr[101] = (void*)&cdat->drout_53__SCANPAT_ANYPATTERN;
  cdat->drout_53__SCANPAT_ANYPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_53__SCANPAT_ANYPATTERN.routdescr, "SCANPAT_ANYPATTERN @warmelt-normatch.bysl:83",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_53__SCANPAT_ANYPATTERN.nbval = 4;
 *(basilysroutfun_t **) (cdat->drout_53__SCANPAT_ANYPATTERN.routaddr) = rout_3_SCANPAT_ANYPATTERN;
 
 /*iniclos dclo_54__SCANPAT_ANYPATTERN*/
 /*_.VALCLO___V103*/ curfptr[102] = (void*)&cdat->dclo_54__SCANPAT_ANYPATTERN;
  cdat->dclo_54__SCANPAT_ANYPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_54__SCANPAT_ANYPATTERN.nbval = 0;
 
 /*iniobj dsym_55__CLASS_SRCPATTERN_ANY*/
 if (!/*_.VALDATA___V104*/ curfptr[103]) /*_.VALDATA___V104*/ curfptr[103] = (void*)&cdat->dsym_55__CLASS_SRCPATTERN_ANY;
  basilys_assertmsg("iniobj checkdiscr dsym_55__CLASS_SRCPATTERN_ANY", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_55__CLASS_SRCPATTERN_ANY.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_55__CLASS_SRCPATTERN_ANY.obj_hash = 855771772;
  cdat->dsym_55__CLASS_SRCPATTERN_ANY.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_55__CLASS_SRCPATTERN_ANY));
 
 /*inistring dstr_56__CLASS_SRCPATTERN*/
 /*_.VALSTR___V105*/ curfptr[104] = (void*)&cdat->dstr_56__CLASS_SRCPATTERN;
  cdat->dstr_56__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_56__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_ANY");
 
 /*inirout drout_57__SCANPAT_SRCPATOR*/
 /*_.VALROUT___V106*/ curfptr[105] = (void*)&cdat->drout_57__SCANPAT_SRCPATOR;
  cdat->drout_57__SCANPAT_SRCPATOR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_57__SCANPAT_SRCPATOR.routdescr, "SCANPAT_SRCPATOR @warmelt-normatch.bysl:98",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_57__SCANPAT_SRCPATOR.nbval = 4;
 *(basilysroutfun_t **) (cdat->drout_57__SCANPAT_SRCPATOR.routaddr) = rout_4_SCANPAT_SRCPATOR;
 
 /*iniclos dclo_58__SCANPAT_SRCPATOR*/
 /*_.VALCLO___V107*/ curfptr[106] = (void*)&cdat->dclo_58__SCANPAT_SRCPATOR;
  cdat->dclo_58__SCANPAT_SRCPATOR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_58__SCANPAT_SRCPATOR.nbval = 0;
 
 /*iniobj dsym_59__CLASS_SRCPATTERN_OR*/
 if (!/*_.VALDATA___V108*/ curfptr[107]) /*_.VALDATA___V108*/ curfptr[107] = (void*)&cdat->dsym_59__CLASS_SRCPATTERN_OR;
  basilys_assertmsg("iniobj checkdiscr dsym_59__CLASS_SRCPATTERN_OR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_59__CLASS_SRCPATTERN_OR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_59__CLASS_SRCPATTERN_OR.obj_hash = 920875001;
  cdat->dsym_59__CLASS_SRCPATTERN_OR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_59__CLASS_SRCPATTERN_OR));
 
 /*inistring dstr_60__CLASS_SRCPATTERN*/
 /*_.VALSTR___V109*/ curfptr[108] = (void*)&cdat->dstr_60__CLASS_SRCPATTERN;
  cdat->dstr_60__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_60__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_OR");
 
 /*inirout drout_61__SCANPAT_SRCPATAND*/
 /*_.VALROUT___V110*/ curfptr[109] = (void*)&cdat->drout_61__SCANPAT_SRCPATAND;
  cdat->drout_61__SCANPAT_SRCPATAND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_61__SCANPAT_SRCPATAND.routdescr, "SCANPAT_SRCPATAND @warmelt-normatch.bysl:117",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_61__SCANPAT_SRCPATAND.nbval = 4;
 *(basilysroutfun_t **) (cdat->drout_61__SCANPAT_SRCPATAND.routaddr) = rout_5_SCANPAT_SRCPATAND;
 
 /*iniclos dclo_62__SCANPAT_SRCPATAND*/
 /*_.VALCLO___V111*/ curfptr[110] = (void*)&cdat->dclo_62__SCANPAT_SRCPATAND;
  cdat->dclo_62__SCANPAT_SRCPATAND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_62__SCANPAT_SRCPATAND.nbval = 0;
 
 /*iniobj dsym_63__CLASS_SRCPATTERN_AND*/
 if (!/*_.VALDATA___V112*/ curfptr[111]) /*_.VALDATA___V112*/ curfptr[111] = (void*)&cdat->dsym_63__CLASS_SRCPATTERN_AND;
  basilys_assertmsg("iniobj checkdiscr dsym_63__CLASS_SRCPATTERN_AND", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_63__CLASS_SRCPATTERN_AND.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_63__CLASS_SRCPATTERN_AND.obj_hash = 181310597;
  cdat->dsym_63__CLASS_SRCPATTERN_AND.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_63__CLASS_SRCPATTERN_AND));
 
 /*inistring dstr_64__CLASS_SRCPATTERN*/
 /*_.VALSTR___V113*/ curfptr[112] = (void*)&cdat->dstr_64__CLASS_SRCPATTERN;
  cdat->dstr_64__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_64__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_AND");
 
 /*inirout drout_65__SCANPAT_SRCPATVAR*/
 /*_.VALROUT___V114*/ curfptr[113] = (void*)&cdat->drout_65__SCANPAT_SRCPATVAR;
  cdat->drout_65__SCANPAT_SRCPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_65__SCANPAT_SRCPATVAR.routdescr, "SCANPAT_SRCPATVAR @warmelt-normatch.bysl:136",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_65__SCANPAT_SRCPATVAR.nbval = 3;
 *(basilysroutfun_t **) (cdat->drout_65__SCANPAT_SRCPATVAR.routaddr) = rout_6_SCANPAT_SRCPATVAR;
 
 /*iniclos dclo_66__SCANPAT_SRCPATVAR*/
 /*_.VALCLO___V115*/ curfptr[114] = (void*)&cdat->dclo_66__SCANPAT_SRCPATVAR;
  cdat->dclo_66__SCANPAT_SRCPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_66__SCANPAT_SRCPATVAR.nbval = 0;
 
 /*iniobj dsym_67__CLASS_SRCPATTERN_VARIABLE*/
 if (!/*_.VALDATA___V116*/ curfptr[115]) /*_.VALDATA___V116*/ curfptr[115] = (void*)&cdat->dsym_67__CLASS_SRCPATTERN_VARIABLE;
  basilys_assertmsg("iniobj checkdiscr dsym_67__CLASS_SRCPATTERN_VARIABLE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_67__CLASS_SRCPATTERN_VARIABLE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_67__CLASS_SRCPATTERN_VARIABLE.obj_hash = 280604106;
  cdat->dsym_67__CLASS_SRCPATTERN_VARIABLE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_67__CLASS_SRCPATTERN_VARIABLE));
 
 /*inistring dstr_68__CLASS_SRCPATTERN*/
 /*_.VALSTR___V117*/ curfptr[116] = (void*)&cdat->dstr_68__CLASS_SRCPATTERN;
  cdat->dstr_68__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_68__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_VARIABLE");
 
 /*inirout drout_69__SCANPAT_SRCPATJOKER*/
 /*_.VALROUT___V118*/ curfptr[117] = (void*)&cdat->drout_69__SCANPAT_SRCPATJOKER;
  cdat->drout_69__SCANPAT_SRCPATJOKER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_69__SCANPAT_SRCPATJOKER.routdescr, "SCANPAT_SRCPATJOKER @warmelt-normatch.bysl:160",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_69__SCANPAT_SRCPATJOKER.nbval = 3;
 *(basilysroutfun_t **) (cdat->drout_69__SCANPAT_SRCPATJOKER.routaddr) = rout_7_SCANPAT_SRCPATJOKER;
 
 /*iniclos dclo_70__SCANPAT_SRCPATJOKER*/
 /*_.VALCLO___V119*/ curfptr[118] = (void*)&cdat->dclo_70__SCANPAT_SRCPATJOKER;
  cdat->dclo_70__SCANPAT_SRCPATJOKER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_70__SCANPAT_SRCPATJOKER.nbval = 0;
 
 /*iniobj dsym_71__CLASS_SRCPATTERN_JOKERVAR*/
 if (!/*_.VALDATA___V120*/ curfptr[119]) /*_.VALDATA___V120*/ curfptr[119] = (void*)&cdat->dsym_71__CLASS_SRCPATTERN_JOKERVAR;
  basilys_assertmsg("iniobj checkdiscr dsym_71__CLASS_SRCPATTERN_JOKERVAR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_71__CLASS_SRCPATTERN_JOKERVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_71__CLASS_SRCPATTERN_JOKERVAR.obj_hash = 233170336;
  cdat->dsym_71__CLASS_SRCPATTERN_JOKERVAR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_71__CLASS_SRCPATTERN_JOKERVAR));
 
 /*inistring dstr_72__CLASS_SRCPATTERN*/
 /*_.VALSTR___V121*/ curfptr[120] = (void*)&cdat->dstr_72__CLASS_SRCPATTERN;
  cdat->dstr_72__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_72__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_JOKERVAR");
 
 /*inirout drout_73__SCANPAT_SRCPATCONST*/
 /*_.VALROUT___V122*/ curfptr[121] = (void*)&cdat->drout_73__SCANPAT_SRCPATCONST;
  cdat->drout_73__SCANPAT_SRCPATCONST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_73__SCANPAT_SRCPATCONST.routdescr, "SCANPAT_SRCPATCONST @warmelt-normatch.bysl:173",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_73__SCANPAT_SRCPATCONST.nbval = 7;
 *(basilysroutfun_t **) (cdat->drout_73__SCANPAT_SRCPATCONST.routaddr) = rout_8_SCANPAT_SRCPATCONST;
 
 /*iniclos dclo_74__SCANPAT_SRCPATCONST*/
 /*_.VALCLO___V123*/ curfptr[122] = (void*)&cdat->dclo_74__SCANPAT_SRCPATCONST;
  cdat->dclo_74__SCANPAT_SRCPATCONST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_74__SCANPAT_SRCPATCONST.nbval = 0;
 
 /*iniobj dsym_75__CLASS_SRCPATTERN_CONSTANT*/
 if (!/*_.VALDATA___V124*/ curfptr[123]) /*_.VALDATA___V124*/ curfptr[123] = (void*)&cdat->dsym_75__CLASS_SRCPATTERN_CONSTANT;
  basilys_assertmsg("iniobj checkdiscr dsym_75__CLASS_SRCPATTERN_CONSTANT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_75__CLASS_SRCPATTERN_CONSTANT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_75__CLASS_SRCPATTERN_CONSTANT.obj_hash = 762423769;
  cdat->dsym_75__CLASS_SRCPATTERN_CONSTANT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_75__CLASS_SRCPATTERN_CONSTANT));
 
 /*inistring dstr_76__CLASS_SRCPATTERN*/
 /*_.VALSTR___V125*/ curfptr[124] = (void*)&cdat->dstr_76__CLASS_SRCPATTERN;
  cdat->dstr_76__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_76__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_CONSTANT");
 
 /*iniobj dsym_77__GET_CTYPE*/
 if (!/*_.VALDATA___V126*/ curfptr[125]) /*_.VALDATA___V126*/ curfptr[125] = (void*)&cdat->dsym_77__GET_CTYPE;
  basilys_assertmsg("iniobj checkdiscr dsym_77__GET_CTYPE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_77__GET_CTYPE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_77__GET_CTYPE.obj_hash = 747649571;
  cdat->dsym_77__GET_CTYPE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_77__GET_CTYPE));
 
 /*inistring dstr_78__GET_CTYPE*/
 /*_.VALSTR___V127*/ curfptr[126] = (void*)&cdat->dstr_78__GET_CTYPE;
  cdat->dstr_78__GET_CTYPE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_78__GET_CTYPE.val, "GET_CTYPE");
 
 /*iniobj dsym_79__CLASS_CTYPE*/
 if (!/*_.VALDATA___V128*/ curfptr[127]) /*_.VALDATA___V128*/ curfptr[127] = (void*)&cdat->dsym_79__CLASS_CTYPE;
  basilys_assertmsg("iniobj checkdiscr dsym_79__CLASS_CTYPE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_79__CLASS_CTYPE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_79__CLASS_CTYPE.obj_hash = 78610803;
  cdat->dsym_79__CLASS_CTYPE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_79__CLASS_CTYPE));
 
 /*inistring dstr_80__CLASS_CTYPE*/
 /*_.VALSTR___V129*/ curfptr[128] = (void*)&cdat->dstr_80__CLASS_CTYPE;
  cdat->dstr_80__CLASS_CTYPE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_80__CLASS_CTYPE.val, "CLASS_CTYPE");
 
 /*iniobj dsym_81__NORMAL_EXP*/
 if (!/*_.VALDATA___V130*/ curfptr[129]) /*_.VALDATA___V130*/ curfptr[129] = (void*)&cdat->dsym_81__NORMAL_EXP;
  basilys_assertmsg("iniobj checkdiscr dsym_81__NORMAL_EXP", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_81__NORMAL_EXP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_81__NORMAL_EXP.obj_hash = 420282450;
  cdat->dsym_81__NORMAL_EXP.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_81__NORMAL_EXP));
 
 /*inistring dstr_82__NORMAL_EXP*/
 /*_.VALSTR___V131*/ curfptr[130] = (void*)&cdat->dstr_82__NORMAL_EXP;
  cdat->dstr_82__NORMAL_EXP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_82__NORMAL_EXP.val, "NORMAL_EXP");
 
 /*iniobj dsym_83__LIST_APPEND2LIST*/
 if (!/*_.VALDATA___V132*/ curfptr[131]) /*_.VALDATA___V132*/ curfptr[131] = (void*)&cdat->dsym_83__LIST_APPEND2LIST;
  basilys_assertmsg("iniobj checkdiscr dsym_83__LIST_APPEND2LIST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_83__LIST_APPEND2LIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_83__LIST_APPEND2LIST.obj_hash = 744841630;
  cdat->dsym_83__LIST_APPEND2LIST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_83__LIST_APPEND2LIST));
 
 /*inistring dstr_84__LIST_APPEND2LIST*/
 /*_.VALSTR___V133*/ curfptr[132] = (void*)&cdat->dstr_84__LIST_APPEND2LIST;
  cdat->dstr_84__LIST_APPEND2LIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_84__LIST_APPEND2LIST.val, "LIST_APPEND2LIST");
 
 /*inirout drout_85__SCANPAT_SRCPATOBJECT*/
 /*_.VALROUT___V134*/ curfptr[133] = (void*)&cdat->drout_85__SCANPAT_SRCPATOBJECT;
  cdat->drout_85__SCANPAT_SRCPATOBJECT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_85__SCANPAT_SRCPATOBJECT.routdescr, "SCANPAT_SRCPATOBJECT @warmelt-normatch.bysl:218",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_85__SCANPAT_SRCPATOBJECT.nbval = 9;
 *(basilysroutfun_t **) (cdat->drout_85__SCANPAT_SRCPATOBJECT.routaddr) = rout_9_SCANPAT_SRCPATOBJECT;
 
 /*iniclos dclo_86__SCANPAT_SRCPATOBJECT*/
 /*_.VALCLO___V135*/ curfptr[134] = (void*)&cdat->dclo_86__SCANPAT_SRCPATOBJECT;
  cdat->dclo_86__SCANPAT_SRCPATOBJECT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_86__SCANPAT_SRCPATOBJECT.nbval = 0;
 
 /*iniobj dsym_87__CLASS_SRCPATTERN_OBJECT*/
 if (!/*_.VALDATA___V136*/ curfptr[135]) /*_.VALDATA___V136*/ curfptr[135] = (void*)&cdat->dsym_87__CLASS_SRCPATTERN_OBJECT;
  basilys_assertmsg("iniobj checkdiscr dsym_87__CLASS_SRCPATTERN_OBJECT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_87__CLASS_SRCPATTERN_OBJECT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_87__CLASS_SRCPATTERN_OBJECT.obj_hash = 666808646;
  cdat->dsym_87__CLASS_SRCPATTERN_OBJECT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_87__CLASS_SRCPATTERN_OBJECT));
 
 /*inistring dstr_88__CLASS_SRCPATTERN*/
 /*_.VALSTR___V137*/ curfptr[136] = (void*)&cdat->dstr_88__CLASS_SRCPATTERN;
  cdat->dstr_88__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_88__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_OBJECT");
 
 /*iniobj dsym_89__CLASS_CLASS*/
 if (!/*_.VALDATA___V138*/ curfptr[137]) /*_.VALDATA___V138*/ curfptr[137] = (void*)&cdat->dsym_89__CLASS_CLASS;
  basilys_assertmsg("iniobj checkdiscr dsym_89__CLASS_CLASS", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_89__CLASS_CLASS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_89__CLASS_CLASS.obj_hash = 60141292;
  cdat->dsym_89__CLASS_CLASS.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_89__CLASS_CLASS));
 
 /*inistring dstr_90__CLASS_CLASS*/
 /*_.VALSTR___V139*/ curfptr[138] = (void*)&cdat->dstr_90__CLASS_CLASS;
  cdat->dstr_90__CLASS_CLASS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_90__CLASS_CLASS.val, "CLASS_CLASS");
 
 /*iniobj dsym_91__CLASS_SRCFIELDPATTERN*/
 if (!/*_.VALDATA___V140*/ curfptr[139]) /*_.VALDATA___V140*/ curfptr[139] = (void*)&cdat->dsym_91__CLASS_SRCFIELDPATTERN;
  basilys_assertmsg("iniobj checkdiscr dsym_91__CLASS_SRCFIELDPATTERN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_91__CLASS_SRCFIELDPATTERN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_91__CLASS_SRCFIELDPATTERN.obj_hash = 780247789;
  cdat->dsym_91__CLASS_SRCFIELDPATTERN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_91__CLASS_SRCFIELDPATTERN));
 
 /*inistring dstr_92__CLASS_SRCFIELDPA*/
 /*_.VALSTR___V141*/ curfptr[140] = (void*)&cdat->dstr_92__CLASS_SRCFIELDPA;
  cdat->dstr_92__CLASS_SRCFIELDPA.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_92__CLASS_SRCFIELDPA.val, "CLASS_SRCFIELDPATTERN");
 
 /*iniobj dsym_93__CLASS_FIELD*/
 if (!/*_.VALDATA___V142*/ curfptr[141]) /*_.VALDATA___V142*/ curfptr[141] = (void*)&cdat->dsym_93__CLASS_FIELD;
  basilys_assertmsg("iniobj checkdiscr dsym_93__CLASS_FIELD", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_93__CLASS_FIELD.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_93__CLASS_FIELD.obj_hash = 523073;
  cdat->dsym_93__CLASS_FIELD.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_93__CLASS_FIELD));
 
 /*inistring dstr_94__CLASS_FIELD*/
 /*_.VALSTR___V143*/ curfptr[142] = (void*)&cdat->dstr_94__CLASS_FIELD;
  cdat->dstr_94__CLASS_FIELD.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_94__CLASS_FIELD.val, "CLASS_FIELD");
 
 /*iniobj dsym_95__CTYPE_VALUE*/
 if (!/*_.VALDATA___V144*/ curfptr[143]) /*_.VALDATA___V144*/ curfptr[143] = (void*)&cdat->dsym_95__CTYPE_VALUE;
  basilys_assertmsg("iniobj checkdiscr dsym_95__CTYPE_VALUE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_95__CTYPE_VALUE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_95__CTYPE_VALUE.obj_hash = 11062864;
  cdat->dsym_95__CTYPE_VALUE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_95__CTYPE_VALUE));
 
 /*inistring dstr_96__CTYPE_VALUE*/
 /*_.VALSTR___V145*/ curfptr[144] = (void*)&cdat->dstr_96__CTYPE_VALUE;
  cdat->dstr_96__CTYPE_VALUE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_96__CTYPE_VALUE.val, "CTYPE_VALUE");
 
 /*inirout drout_97__SCANPAT_SRCPATCOMPOSITE*/
 /*_.VALROUT___V146*/ curfptr[145] = (void*)&cdat->drout_97__SCANPAT_SRCPATCOMPOSITE;
  cdat->drout_97__SCANPAT_SRCPATCOMPOSITE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_97__SCANPAT_SRCPATCOMPOSITE.routdescr, "SCANPAT_SRCPATCOMPOSITE @warmelt-normatch.bysl:256",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_97__SCANPAT_SRCPATCOMPOSITE.nbval = 9;
 *(basilysroutfun_t **) (cdat->drout_97__SCANPAT_SRCPATCOMPOSITE.routaddr) = rout_10_SCANPAT_SRCPATCOMPOSITE;
 
 /*iniclos dclo_98__SCANPAT_SRCPATCOMPOSITE*/
 /*_.VALCLO___V147*/ curfptr[146] = (void*)&cdat->dclo_98__SCANPAT_SRCPATCOMPOSITE;
  cdat->dclo_98__SCANPAT_SRCPATCOMPOSITE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_98__SCANPAT_SRCPATCOMPOSITE.nbval = 0;
 
 /*iniobj dsym_99__CLASS_SRCPATTERN_COMPOSITE*/
 if (!/*_.VALDATA___V148*/ curfptr[147]) /*_.VALDATA___V148*/ curfptr[147] = (void*)&cdat->dsym_99__CLASS_SRCPATTERN_COMPOSITE;
  basilys_assertmsg("iniobj checkdiscr dsym_99__CLASS_SRCPATTERN_COMPOSITE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_99__CLASS_SRCPATTERN_COMPOSITE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_99__CLASS_SRCPATTERN_COMPOSITE.obj_hash = 228072300;
  cdat->dsym_99__CLASS_SRCPATTERN_COMPOSITE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_99__CLASS_SRCPATTERN_COMPOSITE));
 
 /*inistring dstr_100__CLASS_SRCPATTERN*/
 /*_.VALSTR___V149*/ curfptr[148] = (void*)&cdat->dstr_100__CLASS_SRCPATTERN;
  cdat->dstr_100__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_100__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_COMPOSITE");
 
 /*iniobj dsym_101__CLASS_ANY_MATCHER*/
 if (!/*_.VALDATA___V150*/ curfptr[149]) /*_.VALDATA___V150*/ curfptr[149] = (void*)&cdat->dsym_101__CLASS_ANY_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dsym_101__CLASS_ANY_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_101__CLASS_ANY_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_101__CLASS_ANY_MATCHER.obj_hash = 781265508;
  cdat->dsym_101__CLASS_ANY_MATCHER.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_101__CLASS_ANY_MATCHER));
 
 /*inistring dstr_102__CLASS_ANY_MATCHE*/
 /*_.VALSTR___V151*/ curfptr[150] = (void*)&cdat->dstr_102__CLASS_ANY_MATCHE;
  cdat->dstr_102__CLASS_ANY_MATCHE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_102__CLASS_ANY_MATCHE.val, "CLASS_ANY_MATCHER");
 
 /*iniobj dsym_103__CLASS_FORMAL_BINDING*/
 if (!/*_.VALDATA___V152*/ curfptr[151]) /*_.VALDATA___V152*/ curfptr[151] = (void*)&cdat->dsym_103__CLASS_FORMAL_BINDING;
  basilys_assertmsg("iniobj checkdiscr dsym_103__CLASS_FORMAL_BINDING", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_103__CLASS_FORMAL_BINDING.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_103__CLASS_FORMAL_BINDING.obj_hash = 3733780;
  cdat->dsym_103__CLASS_FORMAL_BINDING.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_103__CLASS_FORMAL_BINDING));
 
 /*inistring dstr_104__CLASS_FORMAL_BIN*/
 /*_.VALSTR___V153*/ curfptr[152] = (void*)&cdat->dstr_104__CLASS_FORMAL_BIN;
  cdat->dstr_104__CLASS_FORMAL_BIN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_104__CLASS_FORMAL_BIN.val, "CLASS_FORMAL_BINDING");
 
 /*iniobj dsym_105__NORMALIZE_TUPLE*/
 if (!/*_.VALDATA___V154*/ curfptr[153]) /*_.VALDATA___V154*/ curfptr[153] = (void*)&cdat->dsym_105__NORMALIZE_TUPLE;
  basilys_assertmsg("iniobj checkdiscr dsym_105__NORMALIZE_TUPLE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_105__NORMALIZE_TUPLE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_105__NORMALIZE_TUPLE.obj_hash = 805824045;
  cdat->dsym_105__NORMALIZE_TUPLE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_105__NORMALIZE_TUPLE));
 
 /*inistring dstr_106__NORMALIZE_TUPLE*/
 /*_.VALSTR___V155*/ curfptr[154] = (void*)&cdat->dstr_106__NORMALIZE_TUPLE;
  cdat->dstr_106__NORMALIZE_TUPLE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_106__NORMALIZE_TUPLE.val, "NORMALIZE_TUPLE");
 
 /*iniobj dsym_107__CLASS_NREP_BACKPOINT*/
 if (!/*_.VALDATA___V156*/ curfptr[155]) /*_.VALDATA___V156*/ curfptr[155] = (void*)&cdat->dsym_107__CLASS_NREP_BACKPOINT;
  basilys_assertmsg("iniobj checkdiscr dsym_107__CLASS_NREP_BACKPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_107__CLASS_NREP_BACKPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_107__CLASS_NREP_BACKPOINT.obj_hash = 287839181;
  cdat->dsym_107__CLASS_NREP_BACKPOINT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_107__CLASS_NREP_BACKPOINT));
 
 /*inistring dstr_108__CLASS_NREP_BACKP*/
 /*_.VALSTR___V157*/ curfptr[156] = (void*)&cdat->dstr_108__CLASS_NREP_BACKP;
  cdat->dstr_108__CLASS_NREP_BACKP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_108__CLASS_NREP_BACKP.val, "CLASS_NREP_BACKPOINT");
 
 /*iniobj dsym_109__CLASS_NREP_TYPEXPR*/
 if (!/*_.VALDATA___V158*/ curfptr[157]) /*_.VALDATA___V158*/ curfptr[157] = (void*)&cdat->dsym_109__CLASS_NREP_TYPEXPR;
  basilys_assertmsg("iniobj checkdiscr dsym_109__CLASS_NREP_TYPEXPR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_109__CLASS_NREP_TYPEXPR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_109__CLASS_NREP_TYPEXPR.obj_hash = 874739025;
  cdat->dsym_109__CLASS_NREP_TYPEXPR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_109__CLASS_NREP_TYPEXPR));
 
 /*inistring dstr_110__CLASS_NREP_TYPEX*/
 /*_.VALSTR___V159*/ curfptr[158] = (void*)&cdat->dstr_110__CLASS_NREP_TYPEX;
  cdat->dstr_110__CLASS_NREP_TYPEX.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_110__CLASS_NREP_TYPEX.val, "CLASS_NREP_TYPEXPR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ iniobj");
 /*iniobj dobj_111__CLASS_NREP_BACKPOINT*/
 if (!/*_.VALDATA___V160*/ curfptr[159]) /*_.VALDATA___V160*/ curfptr[159] = (void*)&cdat->dobj_111__CLASS_NREP_BACKPOINT;
  basilys_assertmsg("iniobj checkdiscr dobj_111__CLASS_NREP_BACKPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_111__CLASS_NREP_BACKPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_111__CLASS_NREP_BACKPOINT.obj_num = OBMAG_OBJECT;
  cdat->dobj_111__CLASS_NREP_BACKPOINT.obj_hash = 71903082;
  cdat->dobj_111__CLASS_NREP_BACKPOINT.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_111__CLASS_NREP_BACKPOINT));
 
 /*inistring dstr_112__CLASS_NREP_BACKP*/
 /*_.VALSTR___V161*/ curfptr[160] = (void*)&cdat->dstr_112__CLASS_NREP_BACKP;
  cdat->dstr_112__CLASS_NREP_BACKP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_112__CLASS_NREP_BACKP.val, "CLASS_NREP_BACKPOINT");
 
 /*inimult dtup_113__CLASS_NREP_BACKPOINT*/
 /*_.VALTUP___V162*/ curfptr[161] = (void*)&cdat->dtup_113__CLASS_NREP_BACKPOINT;
  cdat->dtup_113__CLASS_NREP_BACKPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_113__CLASS_NREP_BACKPOINT.nbval = 4;
 /*inimult dtup_114__CLASS_NREP_BACKPOINT*/
 /*_.VALTUP___V167*/ curfptr[166] = (void*)&cdat->dtup_114__CLASS_NREP_BACKPOINT;
  cdat->dtup_114__CLASS_NREP_BACKPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_114__CLASS_NREP_BACKPOINT.nbval = 4;
 /*iniobj dsym_115__NBKP_DO*/
 if (!/*_.VALDATA___V168*/ curfptr[167]) /*_.VALDATA___V168*/ curfptr[167] = (void*)&cdat->dsym_115__NBKP_DO;
  basilys_assertmsg("iniobj checkdiscr dsym_115__NBKP_DO", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_115__NBKP_DO.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_115__NBKP_DO.obj_hash = 949502636;
  cdat->dsym_115__NBKP_DO.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_115__NBKP_DO));
 
 /*inistring dstr_116__NBKP_DO*/
 /*_.VALSTR___V169*/ curfptr[168] = (void*)&cdat->dstr_116__NBKP_DO;
  cdat->dstr_116__NBKP_DO.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_116__NBKP_DO.val, "NBKP_DO");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ iniobj");
 /*iniobj dobj_117__NBKP_DO*/
 if (!/*_.VALDATA___V163*/ curfptr[162]) /*_.VALDATA___V163*/ curfptr[162] = (void*)&cdat->dobj_117__NBKP_DO;
  basilys_assertmsg("iniobj checkdiscr dobj_117__NBKP_DO", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_117__NBKP_DO.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_117__NBKP_DO.obj_num = 2;
  cdat->dobj_117__NBKP_DO.obj_hash = 1051152197;
  cdat->dobj_117__NBKP_DO.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_117__NBKP_DO));
 
 /*inistring dstr_118__NBKP_DO*/
 /*_.VALSTR___V164*/ curfptr[163] = (void*)&cdat->dstr_118__NBKP_DO;
  cdat->dstr_118__NBKP_DO.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_118__NBKP_DO.val, "NBKP_DO");
 
 /*iniobj dsym_119__NBKP_FAIL*/
 if (!/*_.VALDATA___V170*/ curfptr[169]) /*_.VALDATA___V170*/ curfptr[169] = (void*)&cdat->dsym_119__NBKP_FAIL;
  basilys_assertmsg("iniobj checkdiscr dsym_119__NBKP_FAIL", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_119__NBKP_FAIL.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_119__NBKP_FAIL.obj_hash = 481812719;
  cdat->dsym_119__NBKP_FAIL.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_119__NBKP_FAIL));
 
 /*inistring dstr_120__NBKP_FAIL*/
 /*_.VALSTR___V171*/ curfptr[170] = (void*)&cdat->dstr_120__NBKP_FAIL;
  cdat->dstr_120__NBKP_FAIL.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_120__NBKP_FAIL.val, "NBKP_FAIL");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ iniobj");
 /*iniobj dobj_121__NBKP_FAIL*/
 if (!/*_.VALDATA___V165*/ curfptr[164]) /*_.VALDATA___V165*/ curfptr[164] = (void*)&cdat->dobj_121__NBKP_FAIL;
  basilys_assertmsg("iniobj checkdiscr dobj_121__NBKP_FAIL", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_121__NBKP_FAIL.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_121__NBKP_FAIL.obj_num = 3;
  cdat->dobj_121__NBKP_FAIL.obj_hash = 909779112;
  cdat->dobj_121__NBKP_FAIL.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_121__NBKP_FAIL));
 
 /*inistring dstr_122__NBKP_FAIL*/
 /*_.VALSTR___V166*/ curfptr[165] = (void*)&cdat->dstr_122__NBKP_FAIL;
  cdat->dstr_122__NBKP_FAIL.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_122__NBKP_FAIL.val, "NBKP_FAIL");
 
 /*iniobj dsym_123__CLASS_NREP_TESTPOINT*/
 if (!/*_.VALDATA___V172*/ curfptr[171]) /*_.VALDATA___V172*/ curfptr[171] = (void*)&cdat->dsym_123__CLASS_NREP_TESTPOINT;
  basilys_assertmsg("iniobj checkdiscr dsym_123__CLASS_NREP_TESTPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_123__CLASS_NREP_TESTPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_123__CLASS_NREP_TESTPOINT.obj_hash = 207132463;
  cdat->dsym_123__CLASS_NREP_TESTPOINT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_123__CLASS_NREP_TESTPOINT));
 
 /*inistring dstr_124__CLASS_NREP_TESTP*/
 /*_.VALSTR___V173*/ curfptr[172] = (void*)&cdat->dstr_124__CLASS_NREP_TESTP;
  cdat->dstr_124__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_124__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTPOINT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ iniobj");
 /*iniobj dobj_125__CLASS_NREP_TESTPOINT*/
 if (!/*_.VALDATA___V174*/ curfptr[173]) /*_.VALDATA___V174*/ curfptr[173] = (void*)&cdat->dobj_125__CLASS_NREP_TESTPOINT;
  basilys_assertmsg("iniobj checkdiscr dobj_125__CLASS_NREP_TESTPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_125__CLASS_NREP_TESTPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_125__CLASS_NREP_TESTPOINT.obj_num = OBMAG_OBJECT;
  cdat->dobj_125__CLASS_NREP_TESTPOINT.obj_hash = 1009617525;
  cdat->dobj_125__CLASS_NREP_TESTPOINT.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_125__CLASS_NREP_TESTPOINT));
 
 /*inistring dstr_126__CLASS_NREP_TESTP*/
 /*_.VALSTR___V175*/ curfptr[174] = (void*)&cdat->dstr_126__CLASS_NREP_TESTP;
  cdat->dstr_126__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_126__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTPOINT");
 
 /*inimult dtup_127__CLASS_NREP_TESTPOINT*/
 /*_.VALTUP___V176*/ curfptr[175] = (void*)&cdat->dtup_127__CLASS_NREP_TESTPOINT;
  cdat->dtup_127__CLASS_NREP_TESTPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_127__CLASS_NREP_TESTPOINT.nbval = 4;
 /*inimult dtup_128__CLASS_NREP_TESTPOINT*/
 /*_.VALTUP___V183*/ curfptr[182] = (void*)&cdat->dtup_128__CLASS_NREP_TESTPOINT;
  cdat->dtup_128__CLASS_NREP_TESTPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_128__CLASS_NREP_TESTPOINT.nbval = 5;
 /*iniobj dsym_129__NTSP_FAILP*/
 if (!/*_.VALDATA___V184*/ curfptr[183]) /*_.VALDATA___V184*/ curfptr[183] = (void*)&cdat->dsym_129__NTSP_FAILP;
  basilys_assertmsg("iniobj checkdiscr dsym_129__NTSP_FAILP", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_129__NTSP_FAILP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_129__NTSP_FAILP.obj_hash = 857283480;
  cdat->dsym_129__NTSP_FAILP.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_129__NTSP_FAILP));
 
 /*inistring dstr_130__NTSP_FAILP*/
 /*_.VALSTR___V185*/ curfptr[184] = (void*)&cdat->dstr_130__NTSP_FAILP;
  cdat->dstr_130__NTSP_FAILP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_130__NTSP_FAILP.val, "NTSP_FAILP");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ iniobj");
 /*iniobj dobj_131__NTSP_FAILP*/
 if (!/*_.VALDATA___V177*/ curfptr[176]) /*_.VALDATA___V177*/ curfptr[176] = (void*)&cdat->dobj_131__NTSP_FAILP;
  basilys_assertmsg("iniobj checkdiscr dobj_131__NTSP_FAILP", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_131__NTSP_FAILP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_131__NTSP_FAILP.obj_num = 2;
  cdat->dobj_131__NTSP_FAILP.obj_hash = 327367966;
  cdat->dobj_131__NTSP_FAILP.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_131__NTSP_FAILP));
 
 /*inistring dstr_132__NTSP_FAILP*/
 /*_.VALSTR___V178*/ curfptr[177] = (void*)&cdat->dstr_132__NTSP_FAILP;
  cdat->dstr_132__NTSP_FAILP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_132__NTSP_FAILP.val, "NTSP_FAILP");
 
 /*iniobj dsym_133__NTSP_SUCCP*/
 if (!/*_.VALDATA___V186*/ curfptr[185]) /*_.VALDATA___V186*/ curfptr[185] = (void*)&cdat->dsym_133__NTSP_SUCCP;
  basilys_assertmsg("iniobj checkdiscr dsym_133__NTSP_SUCCP", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_133__NTSP_SUCCP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_133__NTSP_SUCCP.obj_hash = 106029957;
  cdat->dsym_133__NTSP_SUCCP.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_133__NTSP_SUCCP));
 
 /*inistring dstr_134__NTSP_SUCCP*/
 /*_.VALSTR___V187*/ curfptr[186] = (void*)&cdat->dstr_134__NTSP_SUCCP;
  cdat->dstr_134__NTSP_SUCCP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_134__NTSP_SUCCP.val, "NTSP_SUCCP");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ iniobj");
 /*iniobj dobj_135__NTSP_SUCCP*/
 if (!/*_.VALDATA___V179*/ curfptr[178]) /*_.VALDATA___V179*/ curfptr[178] = (void*)&cdat->dobj_135__NTSP_SUCCP;
  basilys_assertmsg("iniobj checkdiscr dobj_135__NTSP_SUCCP", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_135__NTSP_SUCCP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_135__NTSP_SUCCP.obj_num = 3;
  cdat->dobj_135__NTSP_SUCCP.obj_hash = 333804812;
  cdat->dobj_135__NTSP_SUCCP.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_135__NTSP_SUCCP));
 
 /*inistring dstr_136__NTSP_SUCCP*/
 /*_.VALSTR___V180*/ curfptr[179] = (void*)&cdat->dstr_136__NTSP_SUCCP;
  cdat->dstr_136__NTSP_SUCCP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_136__NTSP_SUCCP.val, "NTSP_SUCCP");
 
 /*iniobj dsym_137__NTSP_TEST*/
 if (!/*_.VALDATA___V188*/ curfptr[187]) /*_.VALDATA___V188*/ curfptr[187] = (void*)&cdat->dsym_137__NTSP_TEST;
  basilys_assertmsg("iniobj checkdiscr dsym_137__NTSP_TEST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_137__NTSP_TEST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_137__NTSP_TEST.obj_hash = 728021283;
  cdat->dsym_137__NTSP_TEST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_137__NTSP_TEST));
 
 /*inistring dstr_138__NTSP_TEST*/
 /*_.VALSTR___V189*/ curfptr[188] = (void*)&cdat->dstr_138__NTSP_TEST;
  cdat->dstr_138__NTSP_TEST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_138__NTSP_TEST.val, "NTSP_TEST");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ iniobj");
 /*iniobj dobj_139__NTSP_TEST*/
 if (!/*_.VALDATA___V181*/ curfptr[180]) /*_.VALDATA___V181*/ curfptr[180] = (void*)&cdat->dobj_139__NTSP_TEST;
  basilys_assertmsg("iniobj checkdiscr dobj_139__NTSP_TEST", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_139__NTSP_TEST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_139__NTSP_TEST.obj_num = 4;
  cdat->dobj_139__NTSP_TEST.obj_hash = 997525726;
  cdat->dobj_139__NTSP_TEST.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_139__NTSP_TEST));
 
 /*inistring dstr_140__NTSP_TEST*/
 /*_.VALSTR___V182*/ curfptr[181] = (void*)&cdat->dstr_140__NTSP_TEST;
  cdat->dstr_140__NTSP_TEST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_140__NTSP_TEST.val, "NTSP_TEST");
 
 /*iniobj dsym_141__CLASS_NREP_TESTP_MATCHER*/
 if (!/*_.VALDATA___V190*/ curfptr[189]) /*_.VALDATA___V190*/ curfptr[189] = (void*)&cdat->dsym_141__CLASS_NREP_TESTP_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dsym_141__CLASS_NREP_TESTP_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_141__CLASS_NREP_TESTP_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_141__CLASS_NREP_TESTP_MATCHER.obj_hash = 195718540;
  cdat->dsym_141__CLASS_NREP_TESTP_MATCHER.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_141__CLASS_NREP_TESTP_MATCHER));
 
 /*inistring dstr_142__CLASS_NREP_TESTP*/
 /*_.VALSTR___V191*/ curfptr[190] = (void*)&cdat->dstr_142__CLASS_NREP_TESTP;
  cdat->dstr_142__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_142__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_MATCHER");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ iniobj");
 /*iniobj dobj_143__CLASS_NREP_TESTP_MATCHER*/
 if (!/*_.VALDATA___V192*/ curfptr[191]) /*_.VALDATA___V192*/ curfptr[191] = (void*)&cdat->dobj_143__CLASS_NREP_TESTP_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dobj_143__CLASS_NREP_TESTP_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_143__CLASS_NREP_TESTP_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_143__CLASS_NREP_TESTP_MATCHER.obj_num = OBMAG_OBJECT;
  cdat->dobj_143__CLASS_NREP_TESTP_MATCHER.obj_hash = 336327927;
  cdat->dobj_143__CLASS_NREP_TESTP_MATCHER.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_143__CLASS_NREP_TESTP_MATCHER));
 
 /*inistring dstr_144__CLASS_NREP_TESTP*/
 /*_.VALSTR___V193*/ curfptr[192] = (void*)&cdat->dstr_144__CLASS_NREP_TESTP;
  cdat->dstr_144__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_144__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_MATCHER");
 
 /*inimult dtup_145__CLASS_NREP_TESTP_MATCHER*/
 /*_.VALTUP___V194*/ curfptr[193] = (void*)&cdat->dtup_145__CLASS_NREP_TESTP_MATCHER;
  cdat->dtup_145__CLASS_NREP_TESTP_MATCHER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_145__CLASS_NREP_TESTP_MATCHER.nbval = 5;
 /*inimult dtup_146__CLASS_NREP_TESTP_MATCHER*/
 /*_.VALTUP___V199*/ curfptr[198] = (void*)&cdat->dtup_146__CLASS_NREP_TESTP_MATCHER;
  cdat->dtup_146__CLASS_NREP_TESTP_MATCHER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_146__CLASS_NREP_TESTP_MATCHER.nbval = 7;
 /*iniobj dsym_147__NTSM_MATCHER*/
 if (!/*_.VALDATA___V200*/ curfptr[199]) /*_.VALDATA___V200*/ curfptr[199] = (void*)&cdat->dsym_147__NTSM_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dsym_147__NTSM_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_147__NTSM_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_147__NTSM_MATCHER.obj_hash = 800150315;
  cdat->dsym_147__NTSM_MATCHER.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_147__NTSM_MATCHER));
 
 /*inistring dstr_148__NTSM_MATCHER*/
 /*_.VALSTR___V201*/ curfptr[200] = (void*)&cdat->dstr_148__NTSM_MATCHER;
  cdat->dstr_148__NTSM_MATCHER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_148__NTSM_MATCHER.val, "NTSM_MATCHER");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ iniobj");
 /*iniobj dobj_149__NTSM_MATCHER*/
 if (!/*_.VALDATA___V195*/ curfptr[194]) /*_.VALDATA___V195*/ curfptr[194] = (void*)&cdat->dobj_149__NTSM_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dobj_149__NTSM_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_149__NTSM_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_149__NTSM_MATCHER.obj_num = 5;
  cdat->dobj_149__NTSM_MATCHER.obj_hash = 56644888;
  cdat->dobj_149__NTSM_MATCHER.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_149__NTSM_MATCHER));
 
 /*inistring dstr_150__NTSM_MATCHER*/
 /*_.VALSTR___V196*/ curfptr[195] = (void*)&cdat->dstr_150__NTSM_MATCHER;
  cdat->dstr_150__NTSM_MATCHER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_150__NTSM_MATCHER.val, "NTSM_MATCHER");
 
 /*iniobj dsym_151__NTSM_BINDS*/
 if (!/*_.VALDATA___V202*/ curfptr[201]) /*_.VALDATA___V202*/ curfptr[201] = (void*)&cdat->dsym_151__NTSM_BINDS;
  basilys_assertmsg("iniobj checkdiscr dsym_151__NTSM_BINDS", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_151__NTSM_BINDS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_151__NTSM_BINDS.obj_hash = 864286297;
  cdat->dsym_151__NTSM_BINDS.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_151__NTSM_BINDS));
 
 /*inistring dstr_152__NTSM_BINDS*/
 /*_.VALSTR___V203*/ curfptr[202] = (void*)&cdat->dstr_152__NTSM_BINDS;
  cdat->dstr_152__NTSM_BINDS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_152__NTSM_BINDS.val, "NTSM_BINDS");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ iniobj");
 /*iniobj dobj_153__NTSM_BINDS*/
 if (!/*_.VALDATA___V197*/ curfptr[196]) /*_.VALDATA___V197*/ curfptr[196] = (void*)&cdat->dobj_153__NTSM_BINDS;
  basilys_assertmsg("iniobj checkdiscr dobj_153__NTSM_BINDS", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_153__NTSM_BINDS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_153__NTSM_BINDS.obj_num = 6;
  cdat->dobj_153__NTSM_BINDS.obj_hash = 486602634;
  cdat->dobj_153__NTSM_BINDS.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_153__NTSM_BINDS));
 
 /*inistring dstr_154__NTSM_BINDS*/
 /*_.VALSTR___V198*/ curfptr[197] = (void*)&cdat->dstr_154__NTSM_BINDS;
  cdat->dstr_154__NTSM_BINDS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_154__NTSM_BINDS.val, "NTSM_BINDS");
 
 /*iniobj dsym_155__CLASS_NREP_TESTP_ISA*/
 if (!/*_.VALDATA___V204*/ curfptr[203]) /*_.VALDATA___V204*/ curfptr[203] = (void*)&cdat->dsym_155__CLASS_NREP_TESTP_ISA;
  basilys_assertmsg("iniobj checkdiscr dsym_155__CLASS_NREP_TESTP_ISA", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_155__CLASS_NREP_TESTP_ISA.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_155__CLASS_NREP_TESTP_ISA.obj_hash = 681264655;
  cdat->dsym_155__CLASS_NREP_TESTP_ISA.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_155__CLASS_NREP_TESTP_ISA));
 
 /*inistring dstr_156__CLASS_NREP_TESTP*/
 /*_.VALSTR___V205*/ curfptr[204] = (void*)&cdat->dstr_156__CLASS_NREP_TESTP;
  cdat->dstr_156__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_156__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_ISA");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ iniobj");
 /*iniobj dobj_157__CLASS_NREP_TESTP_ISA*/
 if (!/*_.VALDATA___V206*/ curfptr[205]) /*_.VALDATA___V206*/ curfptr[205] = (void*)&cdat->dobj_157__CLASS_NREP_TESTP_ISA;
  basilys_assertmsg("iniobj checkdiscr dobj_157__CLASS_NREP_TESTP_ISA", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_157__CLASS_NREP_TESTP_ISA.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_157__CLASS_NREP_TESTP_ISA.obj_num = OBMAG_OBJECT;
  cdat->dobj_157__CLASS_NREP_TESTP_ISA.obj_hash = 666316343;
  cdat->dobj_157__CLASS_NREP_TESTP_ISA.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_157__CLASS_NREP_TESTP_ISA));
 
 /*inistring dstr_158__CLASS_NREP_TESTP*/
 /*_.VALSTR___V207*/ curfptr[206] = (void*)&cdat->dstr_158__CLASS_NREP_TESTP;
  cdat->dstr_158__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_158__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_ISA");
 
 /*inimult dtup_159__CLASS_NREP_TESTP_ISA*/
 /*_.VALTUP___V208*/ curfptr[207] = (void*)&cdat->dtup_159__CLASS_NREP_TESTP_ISA;
  cdat->dtup_159__CLASS_NREP_TESTP_ISA.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_159__CLASS_NREP_TESTP_ISA.nbval = 5;
 /*inimult dtup_160__CLASS_NREP_TESTP_ISA*/
 /*_.VALTUP___V211*/ curfptr[210] = (void*)&cdat->dtup_160__CLASS_NREP_TESTP_ISA;
  cdat->dtup_160__CLASS_NREP_TESTP_ISA.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_160__CLASS_NREP_TESTP_ISA.nbval = 6;
 /*iniobj dsym_161__NTSI_CLASS*/
 if (!/*_.VALDATA___V212*/ curfptr[211]) /*_.VALDATA___V212*/ curfptr[211] = (void*)&cdat->dsym_161__NTSI_CLASS;
  basilys_assertmsg("iniobj checkdiscr dsym_161__NTSI_CLASS", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_161__NTSI_CLASS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_161__NTSI_CLASS.obj_hash = 232718874;
  cdat->dsym_161__NTSI_CLASS.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_161__NTSI_CLASS));
 
 /*inistring dstr_162__NTSI_CLASS*/
 /*_.VALSTR___V213*/ curfptr[212] = (void*)&cdat->dstr_162__NTSI_CLASS;
  cdat->dstr_162__NTSI_CLASS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_162__NTSI_CLASS.val, "NTSI_CLASS");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ iniobj");
 /*iniobj dobj_163__NTSI_CLASS*/
 if (!/*_.VALDATA___V209*/ curfptr[208]) /*_.VALDATA___V209*/ curfptr[208] = (void*)&cdat->dobj_163__NTSI_CLASS;
  basilys_assertmsg("iniobj checkdiscr dobj_163__NTSI_CLASS", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_163__NTSI_CLASS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_163__NTSI_CLASS.obj_num = 5;
  cdat->dobj_163__NTSI_CLASS.obj_hash = 82717200;
  cdat->dobj_163__NTSI_CLASS.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_163__NTSI_CLASS));
 
 /*inistring dstr_164__NTSI_CLASS*/
 /*_.VALSTR___V210*/ curfptr[209] = (void*)&cdat->dstr_164__NTSI_CLASS;
  cdat->dstr_164__NTSI_CLASS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_164__NTSI_CLASS.val, "NTSI_CLASS");
 
 /*iniobj dsym_165__CLASS_NREP_TESTP_DISCRIM*/
 if (!/*_.VALDATA___V214*/ curfptr[213]) /*_.VALDATA___V214*/ curfptr[213] = (void*)&cdat->dsym_165__CLASS_NREP_TESTP_DISCRIM;
  basilys_assertmsg("iniobj checkdiscr dsym_165__CLASS_NREP_TESTP_DISCRIM", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_165__CLASS_NREP_TESTP_DISCRIM.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_165__CLASS_NREP_TESTP_DISCRIM.obj_hash = 896425522;
  cdat->dsym_165__CLASS_NREP_TESTP_DISCRIM.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_165__CLASS_NREP_TESTP_DISCRIM));
 
 /*inistring dstr_166__CLASS_NREP_TESTP*/
 /*_.VALSTR___V215*/ curfptr[214] = (void*)&cdat->dstr_166__CLASS_NREP_TESTP;
  cdat->dstr_166__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_166__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_DISCRIM");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:332:/ iniobj");
 /*iniobj dobj_167__CLASS_NREP_TESTP_DISCRIM*/
 if (!/*_.VALDATA___V216*/ curfptr[215]) /*_.VALDATA___V216*/ curfptr[215] = (void*)&cdat->dobj_167__CLASS_NREP_TESTP_DISCRIM;
  basilys_assertmsg("iniobj checkdiscr dobj_167__CLASS_NREP_TESTP_DISCRIM", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_167__CLASS_NREP_TESTP_DISCRIM.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_167__CLASS_NREP_TESTP_DISCRIM.obj_num = OBMAG_OBJECT;
  cdat->dobj_167__CLASS_NREP_TESTP_DISCRIM.obj_hash = 218219321;
  cdat->dobj_167__CLASS_NREP_TESTP_DISCRIM.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_167__CLASS_NREP_TESTP_DISCRIM));
 
 /*inistring dstr_168__CLASS_NREP_TESTP*/
 /*_.VALSTR___V217*/ curfptr[216] = (void*)&cdat->dstr_168__CLASS_NREP_TESTP;
  cdat->dstr_168__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_168__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_DISCRIM");
 
 /*inimult dtup_169__CLASS_NREP_TESTP_DISCRIM*/
 /*_.VALTUP___V218*/ curfptr[217] = (void*)&cdat->dtup_169__CLASS_NREP_TESTP_DISCRIM;
  cdat->dtup_169__CLASS_NREP_TESTP_DISCRIM.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_169__CLASS_NREP_TESTP_DISCRIM.nbval = 6;
 /*inimult dtup_170__CLASS_NREP_TESTP_DISCRIM*/
 /*_.VALTUP___V219*/ curfptr[218] = (void*)&cdat->dtup_170__CLASS_NREP_TESTP_DISCRIM;
  cdat->dtup_170__CLASS_NREP_TESTP_DISCRIM.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_170__CLASS_NREP_TESTP_DISCRIM.nbval = 6;
 /*iniobj dsym_171__CLASS_NREP_TESTP_COND*/
 if (!/*_.VALDATA___V220*/ curfptr[219]) /*_.VALDATA___V220*/ curfptr[219] = (void*)&cdat->dsym_171__CLASS_NREP_TESTP_COND;
  basilys_assertmsg("iniobj checkdiscr dsym_171__CLASS_NREP_TESTP_COND", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_171__CLASS_NREP_TESTP_COND.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_171__CLASS_NREP_TESTP_COND.obj_hash = 205121291;
  cdat->dsym_171__CLASS_NREP_TESTP_COND.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_171__CLASS_NREP_TESTP_COND));
 
 /*inistring dstr_172__CLASS_NREP_TESTP*/
 /*_.VALSTR___V221*/ curfptr[220] = (void*)&cdat->dstr_172__CLASS_NREP_TESTP;
  cdat->dstr_172__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_172__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_COND");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ iniobj");
 /*iniobj dobj_173__CLASS_NREP_TESTP_COND*/
 if (!/*_.VALDATA___V222*/ curfptr[221]) /*_.VALDATA___V222*/ curfptr[221] = (void*)&cdat->dobj_173__CLASS_NREP_TESTP_COND;
  basilys_assertmsg("iniobj checkdiscr dobj_173__CLASS_NREP_TESTP_COND", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_173__CLASS_NREP_TESTP_COND.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_173__CLASS_NREP_TESTP_COND.obj_num = OBMAG_OBJECT;
  cdat->dobj_173__CLASS_NREP_TESTP_COND.obj_hash = 432599916;
  cdat->dobj_173__CLASS_NREP_TESTP_COND.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_173__CLASS_NREP_TESTP_COND));
 
 /*inistring dstr_174__CLASS_NREP_TESTP*/
 /*_.VALSTR___V223*/ curfptr[222] = (void*)&cdat->dstr_174__CLASS_NREP_TESTP;
  cdat->dstr_174__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_174__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTP_COND");
 
 /*inimult dtup_175__CLASS_NREP_TESTP_COND*/
 /*_.VALTUP___V224*/ curfptr[223] = (void*)&cdat->dtup_175__CLASS_NREP_TESTP_COND;
  cdat->dtup_175__CLASS_NREP_TESTP_COND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_175__CLASS_NREP_TESTP_COND.nbval = 5;
 /*inimult dtup_176__CLASS_NREP_TESTP_COND*/
 /*_.VALTUP___V227*/ curfptr[226] = (void*)&cdat->dtup_176__CLASS_NREP_TESTP_COND;
  cdat->dtup_176__CLASS_NREP_TESTP_COND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_176__CLASS_NREP_TESTP_COND.nbval = 6;
 /*iniobj dsym_177__NTSC_COND*/
 if (!/*_.VALDATA___V228*/ curfptr[227]) /*_.VALDATA___V228*/ curfptr[227] = (void*)&cdat->dsym_177__NTSC_COND;
  basilys_assertmsg("iniobj checkdiscr dsym_177__NTSC_COND", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_177__NTSC_COND.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_177__NTSC_COND.obj_hash = 1059758839;
  cdat->dsym_177__NTSC_COND.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_177__NTSC_COND));
 
 /*inistring dstr_178__NTSC_COND*/
 /*_.VALSTR___V229*/ curfptr[228] = (void*)&cdat->dstr_178__NTSC_COND;
  cdat->dstr_178__NTSC_COND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_178__NTSC_COND.val, "NTSC_COND");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ iniobj");
 /*iniobj dobj_179__NTSC_COND*/
 if (!/*_.VALDATA___V225*/ curfptr[224]) /*_.VALDATA___V225*/ curfptr[224] = (void*)&cdat->dobj_179__NTSC_COND;
  basilys_assertmsg("iniobj checkdiscr dobj_179__NTSC_COND", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_179__NTSC_COND.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_179__NTSC_COND.obj_num = 5;
  cdat->dobj_179__NTSC_COND.obj_hash = 1019122328;
  cdat->dobj_179__NTSC_COND.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_179__NTSC_COND));
 
 /*inistring dstr_180__NTSC_COND*/
 /*_.VALSTR___V226*/ curfptr[225] = (void*)&cdat->dstr_180__NTSC_COND;
  cdat->dstr_180__NTSC_COND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_180__NTSC_COND.val, "NTSC_COND");
 
 /*iniobj dsym_181__NORMAL_PATTERN*/
 if (!/*_.VALDATA___V230*/ curfptr[229]) /*_.VALDATA___V230*/ curfptr[229] = (void*)&cdat->dsym_181__NORMAL_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dsym_181__NORMAL_PATTERN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_181__NORMAL_PATTERN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_181__NORMAL_PATTERN.obj_hash = 859540740;
  cdat->dsym_181__NORMAL_PATTERN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_181__NORMAL_PATTERN));
 
 /*inistring dstr_182__NORMAL_PATTERN*/
 /*_.VALSTR___V231*/ curfptr[230] = (void*)&cdat->dstr_182__NORMAL_PATTERN;
  cdat->dstr_182__NORMAL_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_182__NORMAL_PATTERN.val, "NORMAL_PATTERN");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:353:/ iniobj");
 /*iniobj dobj_183__NORMAL_PATTERN*/
 if (!/*_.VALDATA___V232*/ curfptr[231]) /*_.VALDATA___V232*/ curfptr[231] = (void*)&cdat->dobj_183__NORMAL_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dobj_183__NORMAL_PATTERN", NULL != (void*)/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_183__NORMAL_PATTERN.obj_class = (basilysobject_ptr_t)(/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_183__NORMAL_PATTERN.obj_hash = 541426307;
  cdat->dobj_183__NORMAL_PATTERN.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_183__NORMAL_PATTERN));
 
 /*inistring dstr_184__NORMAL_PATTERN*/
 /*_.VALSTR___V233*/ curfptr[232] = (void*)&cdat->dstr_184__NORMAL_PATTERN;
  cdat->dstr_184__NORMAL_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_184__NORMAL_PATTERN.val, "NORMAL_PATTERN");
 
 /*iniobj dsym_185__ADD_WHEN_PATTERN*/
 if (!/*_.VALDATA___V234*/ curfptr[233]) /*_.VALDATA___V234*/ curfptr[233] = (void*)&cdat->dsym_185__ADD_WHEN_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dsym_185__ADD_WHEN_PATTERN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_185__ADD_WHEN_PATTERN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_185__ADD_WHEN_PATTERN.obj_hash = 612584448;
  cdat->dsym_185__ADD_WHEN_PATTERN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_185__ADD_WHEN_PATTERN));
 
 /*inistring dstr_186__ADD_WHEN_PATTERN*/
 /*_.VALSTR___V235*/ curfptr[234] = (void*)&cdat->dstr_186__ADD_WHEN_PATTERN;
  cdat->dstr_186__ADD_WHEN_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_186__ADD_WHEN_PATTERN.val, "ADD_WHEN_PATTERN");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:358:/ iniobj");
 /*iniobj dobj_187__ADD_WHEN_PATTERN*/
 if (!/*_.VALDATA___V236*/ curfptr[235]) /*_.VALDATA___V236*/ curfptr[235] = (void*)&cdat->dobj_187__ADD_WHEN_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dobj_187__ADD_WHEN_PATTERN", NULL != (void*)/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_187__ADD_WHEN_PATTERN.obj_class = (basilysobject_ptr_t)(/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_187__ADD_WHEN_PATTERN.obj_hash = 513760105;
  cdat->dobj_187__ADD_WHEN_PATTERN.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_187__ADD_WHEN_PATTERN));
 
 /*inistring dstr_188__ADD_WHEN_PATTERN*/
 /*_.VALSTR___V237*/ curfptr[236] = (void*)&cdat->dstr_188__ADD_WHEN_PATTERN;
  cdat->dstr_188__ADD_WHEN_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_188__ADD_WHEN_PATTERN.val, "ADD_WHEN_PATTERN");
 
 /*inirout drout_189__NORMPAT_ANYRECV*/
 /*_.VALROUT___V238*/ curfptr[237] = (void*)&cdat->drout_189__NORMPAT_ANYRECV;
  cdat->drout_189__NORMPAT_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_189__NORMPAT_ANYRECV.routdescr, "NORMPAT_ANYRECV @warmelt-normatch.bysl:366",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_189__NORMPAT_ANYRECV.nbval = 4;
 *(basilysroutfun_t **) (cdat->drout_189__NORMPAT_ANYRECV.routaddr) = rout_11_NORMPAT_ANYRECV;
 
 /*iniclos dclo_190__NORMPAT_ANYRECV*/
 /*_.VALCLO___V239*/ curfptr[238] = (void*)&cdat->dclo_190__NORMPAT_ANYRECV;
  cdat->dclo_190__NORMPAT_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_190__NORMPAT_ANYRECV.nbval = 0;
 
 /*iniobj dsym_191__CLASS_SRC*/
 if (!/*_.VALDATA___V240*/ curfptr[239]) /*_.VALDATA___V240*/ curfptr[239] = (void*)&cdat->dsym_191__CLASS_SRC;
  basilys_assertmsg("iniobj checkdiscr dsym_191__CLASS_SRC", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_191__CLASS_SRC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_191__CLASS_SRC.obj_hash = 631779221;
  cdat->dsym_191__CLASS_SRC.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_191__CLASS_SRC));
 
 /*inistring dstr_192__CLASS_SRC*/
 /*_.VALSTR___V241*/ curfptr[240] = (void*)&cdat->dstr_192__CLASS_SRC;
  cdat->dstr_192__CLASS_SRC.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_192__CLASS_SRC.val, "CLASS_SRC");
 
 /*inirout drout_193__NORMPAT_ANYPAT*/
 /*_.VALROUT___V242*/ curfptr[241] = (void*)&cdat->drout_193__NORMPAT_ANYPAT;
  cdat->drout_193__NORMPAT_ANYPAT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_193__NORMPAT_ANYPAT.routdescr, "NORMPAT_ANYPAT @warmelt-normatch.bysl:382",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_193__NORMPAT_ANYPAT.nbval = 5;
 *(basilysroutfun_t **) (cdat->drout_193__NORMPAT_ANYPAT.routaddr) = rout_12_NORMPAT_ANYPAT;
 
 /*iniclos dclo_194__NORMPAT_ANYPAT*/
 /*_.VALCLO___V243*/ curfptr[242] = (void*)&cdat->dclo_194__NORMPAT_ANYPAT;
  cdat->dclo_194__NORMPAT_ANYPAT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_194__NORMPAT_ANYPAT.nbval = 0;
 
 /*inirout drout_195__NORMPAT_INSTANCEPAT*/
 /*_.VALROUT___V244*/ curfptr[243] = (void*)&cdat->drout_195__NORMPAT_INSTANCEPAT;
  cdat->drout_195__NORMPAT_INSTANCEPAT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_195__NORMPAT_INSTANCEPAT.routdescr, "NORMPAT_INSTANCEPAT @warmelt-normatch.bysl:400",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_195__NORMPAT_INSTANCEPAT.nbval = 4;
 *(basilysroutfun_t **) (cdat->drout_195__NORMPAT_INSTANCEPAT.routaddr) = rout_13_NORMPAT_INSTANCEPAT;
 
 /*iniclos dclo_196__NORMPAT_INSTANCEPAT*/
 /*_.VALCLO___V245*/ curfptr[244] = (void*)&cdat->dclo_196__NORMPAT_INSTANCEPAT;
  cdat->dclo_196__NORMPAT_INSTANCEPAT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_196__NORMPAT_INSTANCEPAT.nbval = 0;
 
 /*iniobj dsym_197__CLASS_SRCPATTERN_INSTANCE*/
 if (!/*_.VALDATA___V246*/ curfptr[245]) /*_.VALDATA___V246*/ curfptr[245] = (void*)&cdat->dsym_197__CLASS_SRCPATTERN_INSTANCE;
  basilys_assertmsg("iniobj checkdiscr dsym_197__CLASS_SRCPATTERN_INSTANCE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_197__CLASS_SRCPATTERN_INSTANCE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_197__CLASS_SRCPATTERN_INSTANCE.obj_hash = 375415023;
  cdat->dsym_197__CLASS_SRCPATTERN_INSTANCE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_197__CLASS_SRCPATTERN_INSTANCE));
 
 /*inistring dstr_198__CLASS_SRCPATTERN*/
 /*_.VALSTR___V247*/ curfptr[246] = (void*)&cdat->dstr_198__CLASS_SRCPATTERN;
  cdat->dstr_198__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_198__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_INSTANCE");
 
 /*inirout drout_199__NORMPAT_ANYMATCHPAT*/
 /*_.VALROUT___V248*/ curfptr[247] = (void*)&cdat->drout_199__NORMPAT_ANYMATCHPAT;
  cdat->drout_199__NORMPAT_ANYMATCHPAT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_199__NORMPAT_ANYMATCHPAT.routdescr, "NORMPAT_ANYMATCHPAT @warmelt-normatch.bysl:415",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_199__NORMPAT_ANYMATCHPAT.nbval = 19;
 *(basilysroutfun_t **) (cdat->drout_199__NORMPAT_ANYMATCHPAT.routaddr) = rout_14_NORMPAT_ANYMATCHPAT;
 
 /*iniclos dclo_200__NORMPAT_ANYMATCHPAT*/
 /*_.VALCLO___V249*/ curfptr[248] = (void*)&cdat->dclo_200__NORMPAT_ANYMATCHPAT;
  cdat->dclo_200__NORMPAT_ANYMATCHPAT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_200__NORMPAT_ANYMATCHPAT.nbval = 0;
 
 /*iniobj dsym_201__CLASS_SRCPATTERN_ANYMATCH*/
 if (!/*_.VALDATA___V250*/ curfptr[249]) /*_.VALDATA___V250*/ curfptr[249] = (void*)&cdat->dsym_201__CLASS_SRCPATTERN_ANYMATCH;
  basilys_assertmsg("iniobj checkdiscr dsym_201__CLASS_SRCPATTERN_ANYMATCH", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_201__CLASS_SRCPATTERN_ANYMATCH.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_201__CLASS_SRCPATTERN_ANYMATCH.obj_hash = 211006227;
  cdat->dsym_201__CLASS_SRCPATTERN_ANYMATCH.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_201__CLASS_SRCPATTERN_ANYMATCH));
 
 /*inistring dstr_202__CLASS_SRCPATTERN*/
 /*_.VALSTR___V251*/ curfptr[250] = (void*)&cdat->dstr_202__CLASS_SRCPATTERN;
  cdat->dstr_202__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_202__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_ANYMATCH");
 
 /*iniobj dsym_203__FRESH_ENV*/
 if (!/*_.VALDATA___V252*/ curfptr[251]) /*_.VALDATA___V252*/ curfptr[251] = (void*)&cdat->dsym_203__FRESH_ENV;
  basilys_assertmsg("iniobj checkdiscr dsym_203__FRESH_ENV", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_203__FRESH_ENV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_203__FRESH_ENV.obj_hash = 1002363940;
  cdat->dsym_203__FRESH_ENV.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_203__FRESH_ENV));
 
 /*inistring dstr_204__FRESH_ENV*/
 /*_.VALSTR___V253*/ curfptr[252] = (void*)&cdat->dstr_204__FRESH_ENV;
  cdat->dstr_204__FRESH_ENV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_204__FRESH_ENV.val, "FRESH_ENV");
 
 /*iniobj dsym_205__CLASS_ANY_BINDING*/
 if (!/*_.VALDATA___V254*/ curfptr[253]) /*_.VALDATA___V254*/ curfptr[253] = (void*)&cdat->dsym_205__CLASS_ANY_BINDING;
  basilys_assertmsg("iniobj checkdiscr dsym_205__CLASS_ANY_BINDING", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_205__CLASS_ANY_BINDING.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_205__CLASS_ANY_BINDING.obj_hash = 935770;
  cdat->dsym_205__CLASS_ANY_BINDING.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_205__CLASS_ANY_BINDING));
 
 /*inistring dstr_206__CLASS_ANY_BINDIN*/
 /*_.VALSTR___V255*/ curfptr[254] = (void*)&cdat->dstr_206__CLASS_ANY_BINDIN;
  cdat->dstr_206__CLASS_ANY_BINDIN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_206__CLASS_ANY_BINDIN.val, "CLASS_ANY_BINDING");
 
 /*iniobj dsym_207__CLASS_NORMLET_BINDING*/
 if (!/*_.VALDATA___V256*/ curfptr[255]) /*_.VALDATA___V256*/ curfptr[255] = (void*)&cdat->dsym_207__CLASS_NORMLET_BINDING;
  basilys_assertmsg("iniobj checkdiscr dsym_207__CLASS_NORMLET_BINDING", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_207__CLASS_NORMLET_BINDING.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_207__CLASS_NORMLET_BINDING.obj_hash = 17546439;
  cdat->dsym_207__CLASS_NORMLET_BINDING.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_207__CLASS_NORMLET_BINDING));
 
 /*inistring dstr_208__CLASS_NORMLET_BI*/
 /*_.VALSTR___V257*/ curfptr[256] = (void*)&cdat->dstr_208__CLASS_NORMLET_BI;
  cdat->dstr_208__CLASS_NORMLET_BI.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_208__CLASS_NORMLET_BI.val, "CLASS_NORMLET_BINDING");
 
 /*iniobj dsym_209__PUT_ENV*/
 if (!/*_.VALDATA___V258*/ curfptr[257]) /*_.VALDATA___V258*/ curfptr[257] = (void*)&cdat->dsym_209__PUT_ENV;
  basilys_assertmsg("iniobj checkdiscr dsym_209__PUT_ENV", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_209__PUT_ENV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_209__PUT_ENV.obj_hash = 440202203;
  cdat->dsym_209__PUT_ENV.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_209__PUT_ENV));
 
 /*inistring dstr_210__PUT_ENV*/
 /*_.VALSTR___V259*/ curfptr[258] = (void*)&cdat->dstr_210__PUT_ENV;
  cdat->dstr_210__PUT_ENV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_210__PUT_ENV.val, "PUT_ENV");
 
 /*iniobj dsym_211__CLONE_SYMBOL*/
 if (!/*_.VALDATA___V260*/ curfptr[259]) /*_.VALDATA___V260*/ curfptr[259] = (void*)&cdat->dsym_211__CLONE_SYMBOL;
  basilys_assertmsg("iniobj checkdiscr dsym_211__CLONE_SYMBOL", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_211__CLONE_SYMBOL.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_211__CLONE_SYMBOL.obj_hash = 213258240;
  cdat->dsym_211__CLONE_SYMBOL.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_211__CLONE_SYMBOL));
 
 /*inistring dstr_212__CLONE_SYMBOL*/
 /*_.VALSTR___V261*/ curfptr[260] = (void*)&cdat->dstr_212__CLONE_SYMBOL;
  cdat->dstr_212__CLONE_SYMBOL.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_212__CLONE_SYMBOL.val, "CLONE_SYMBOL");
 
 /*iniobj dsym_213__CLASS_NREP_LOCSYMOCC*/
 if (!/*_.VALDATA___V262*/ curfptr[261]) /*_.VALDATA___V262*/ curfptr[261] = (void*)&cdat->dsym_213__CLASS_NREP_LOCSYMOCC;
  basilys_assertmsg("iniobj checkdiscr dsym_213__CLASS_NREP_LOCSYMOCC", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_213__CLASS_NREP_LOCSYMOCC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_213__CLASS_NREP_LOCSYMOCC.obj_hash = 614887922;
  cdat->dsym_213__CLASS_NREP_LOCSYMOCC.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_213__CLASS_NREP_LOCSYMOCC));
 
 /*inistring dstr_214__CLASS_NREP_LOCSY*/
 /*_.VALSTR___V263*/ curfptr[262] = (void*)&cdat->dstr_214__CLASS_NREP_LOCSY;
  cdat->dstr_214__CLASS_NREP_LOCSY.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_214__CLASS_NREP_LOCSY.val, "CLASS_NREP_LOCSYMOCC");
 
 /*inirout drout_215__NORMEXP_MATCH*/
 /*_.VALROUT___V264*/ curfptr[263] = (void*)&cdat->drout_215__NORMEXP_MATCH;
  cdat->drout_215__NORMEXP_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_215__NORMEXP_MATCH.routdescr, "NORMEXP_MATCH @warmelt-normatch.bysl:566",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_215__NORMEXP_MATCH.nbval = 15;
 *(basilysroutfun_t **) (cdat->drout_215__NORMEXP_MATCH.routaddr) = rout_15_NORMEXP_MATCH;
 
 /*iniclos dclo_216__NORMEXP_MATCH*/
 /*_.VALCLO___V265*/ curfptr[264] = (void*)&cdat->dclo_216__NORMEXP_MATCH;
  cdat->dclo_216__NORMEXP_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_216__NORMEXP_MATCH.nbval = 0;
 
 /*iniobj dsym_217__CLASS_SRC_MATCH*/
 if (!/*_.VALDATA___V266*/ curfptr[265]) /*_.VALDATA___V266*/ curfptr[265] = (void*)&cdat->dsym_217__CLASS_SRC_MATCH;
  basilys_assertmsg("iniobj checkdiscr dsym_217__CLASS_SRC_MATCH", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_217__CLASS_SRC_MATCH.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_217__CLASS_SRC_MATCH.obj_hash = 898626999;
  cdat->dsym_217__CLASS_SRC_MATCH.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_217__CLASS_SRC_MATCH));
 
 /*inistring dstr_218__CLASS_SRC_MATCH*/
 /*_.VALSTR___V267*/ curfptr[266] = (void*)&cdat->dstr_218__CLASS_SRC_MATCH;
  cdat->dstr_218__CLASS_SRC_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_218__CLASS_SRC_MATCH.val, "CLASS_SRC_MATCH");
 
 /*iniobj dsym_219__CLASS_ENVIRONMENT*/
 if (!/*_.VALDATA___V268*/ curfptr[267]) /*_.VALDATA___V268*/ curfptr[267] = (void*)&cdat->dsym_219__CLASS_ENVIRONMENT;
  basilys_assertmsg("iniobj checkdiscr dsym_219__CLASS_ENVIRONMENT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_219__CLASS_ENVIRONMENT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_219__CLASS_ENVIRONMENT.obj_hash = 59212821;
  cdat->dsym_219__CLASS_ENVIRONMENT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_219__CLASS_ENVIRONMENT));
 
 /*inistring dstr_220__CLASS_ENVIRONMEN*/
 /*_.VALSTR___V269*/ curfptr[268] = (void*)&cdat->dstr_220__CLASS_ENVIRONMEN;
  cdat->dstr_220__CLASS_ENVIRONMEN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_220__CLASS_ENVIRONMEN.val, "CLASS_ENVIRONMENT");
 
 /*iniobj dsym_221__CLASS_NORMCONTEXT*/
 if (!/*_.VALDATA___V270*/ curfptr[269]) /*_.VALDATA___V270*/ curfptr[269] = (void*)&cdat->dsym_221__CLASS_NORMCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dsym_221__CLASS_NORMCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_221__CLASS_NORMCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_221__CLASS_NORMCONTEXT.obj_hash = 317209306;
  cdat->dsym_221__CLASS_NORMCONTEXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_221__CLASS_NORMCONTEXT));
 
 /*inistring dstr_222__CLASS_NORMCONTEX*/
 /*_.VALSTR___V271*/ curfptr[270] = (void*)&cdat->dstr_222__CLASS_NORMCONTEX;
  cdat->dstr_222__CLASS_NORMCONTEX.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_222__CLASS_NORMCONTEX.val, "CLASS_NORMCONTEXT");
 
 /*iniobj dsym_223__DISCR_MULTIPLE*/
 if (!/*_.VALDATA___V272*/ curfptr[271]) /*_.VALDATA___V272*/ curfptr[271] = (void*)&cdat->dsym_223__DISCR_MULTIPLE;
  basilys_assertmsg("iniobj checkdiscr dsym_223__DISCR_MULTIPLE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_223__DISCR_MULTIPLE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_223__DISCR_MULTIPLE.obj_hash = 91864583;
  cdat->dsym_223__DISCR_MULTIPLE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_223__DISCR_MULTIPLE));
 
 /*inistring dstr_224__DISCR_MULTIPLE*/
 /*_.VALSTR___V273*/ curfptr[272] = (void*)&cdat->dstr_224__DISCR_MULTIPLE;
  cdat->dstr_224__DISCR_MULTIPLE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_224__DISCR_MULTIPLE.val, "DISCR_MULTIPLE");
 
 /*iniobj dsym_225__DISCR_LIST*/
 if (!/*_.VALDATA___V274*/ curfptr[273]) /*_.VALDATA___V274*/ curfptr[273] = (void*)&cdat->dsym_225__DISCR_LIST;
  basilys_assertmsg("iniobj checkdiscr dsym_225__DISCR_LIST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_225__DISCR_LIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_225__DISCR_LIST.obj_hash = 98407634;
  cdat->dsym_225__DISCR_LIST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_225__DISCR_LIST));
 
 /*inistring dstr_226__DISCR_LIST*/
 /*_.VALSTR___V275*/ curfptr[274] = (void*)&cdat->dstr_226__DISCR_LIST;
  cdat->dstr_226__DISCR_LIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_226__DISCR_LIST.val, "DISCR_LIST");
 
 /*iniobj dsym_227__CLASS_SRC_CASEMATCH*/
 if (!/*_.VALDATA___V276*/ curfptr[275]) /*_.VALDATA___V276*/ curfptr[275] = (void*)&cdat->dsym_227__CLASS_SRC_CASEMATCH;
  basilys_assertmsg("iniobj checkdiscr dsym_227__CLASS_SRC_CASEMATCH", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_227__CLASS_SRC_CASEMATCH.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_227__CLASS_SRC_CASEMATCH.obj_hash = 695254918;
  cdat->dsym_227__CLASS_SRC_CASEMATCH.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_227__CLASS_SRC_CASEMATCH));
 
 /*inistring dstr_228__CLASS_SRC_CASEMA*/
 /*_.VALSTR___V277*/ curfptr[276] = (void*)&cdat->dstr_228__CLASS_SRC_CASEMA;
  cdat->dstr_228__CLASS_SRC_CASEMA.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_228__CLASS_SRC_CASEMA.val, "CLASS_SRC_CASEMATCH");
 
 /*iniobj dsym_229__DISCR_MAPOBJECTS*/
 if (!/*_.VALDATA___V278*/ curfptr[277]) /*_.VALDATA___V278*/ curfptr[277] = (void*)&cdat->dsym_229__DISCR_MAPOBJECTS;
  basilys_assertmsg("iniobj checkdiscr dsym_229__DISCR_MAPOBJECTS", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_229__DISCR_MAPOBJECTS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_229__DISCR_MAPOBJECTS.obj_hash = 95669218;
  cdat->dsym_229__DISCR_MAPOBJECTS.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_229__DISCR_MAPOBJECTS));
 
 /*inistring dstr_230__DISCR_MAPOBJECTS*/
 /*_.VALSTR___V279*/ curfptr[278] = (void*)&cdat->dstr_230__DISCR_MAPOBJECTS;
  cdat->dstr_230__DISCR_MAPOBJECTS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_230__DISCR_MAPOBJECTS.val, "DISCR_MAPOBJECTS");
  cdat = NULL;
 basilys_prohibit_garbcoll = FALSE;

#undef curfram__
} /*end initialize_module_cdata*/


void* start_module_basilys(void* modargp_) {

 char predefinited[BGLOB__LASTGLOB+8];
 
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

initial_frame_st     curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 299;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
/**initial routine prologue**/
 initial_frame_basilys = (void*) &curfram__;
 /**COMMENT: get previous environment **/;
 
 /*_.PREVENV___V4*/ curfptr[3] = modargp_;
 /**COMMENT: compute boxloc **/;
 
 /*cond*/ if (/*_.CONTENV___V2*/ curfptr[1] || basilys_object_length((basilys_ptr_t)BASILYSG(INITIAL_SYSTEM_DATA))<FSYSDAT_BOX_FRESH_ENV) /*then*/ {
 } else {
  /*com.block:compute fresh module environment*/{
   /**COMMENT: start computing boxloc **/;
   ;
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.FRESHENV___V3*/ curfptr[2] = slot; };
   ;
   
    if ((/*_.PREVENV___V4*/ curfptr[3]) && basilys_magic_discr((basilys_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2])) != OBMAG_CLOSURE) warning(0, "bad BASILYS fresh_env @%p in system data <%s:%d>", /*_.FRESHENV___V3*/ curfptr[2], __FILE__, __LINE__);;;
   /*apply*/{
    /*_.CONTENV___V2*/ curfptr[1] =  basilys_apply ((basilysclosure_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2]), (basilys_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (""), (union basilysparam_un*)0, "", (union basilysparam_un*)0);
    }
   ;}/*com.end block:compute fresh module environment*/
  ;
  }
 
 /**COMMENT: get symbols & keywords **/;
 
 /*getnamedsym:CLASS_PATTERNCONTEXT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_PATTERNCONTEXT = basilysgc_named_symbol("CLASS_PATTERNCONTEXT", BASILYS_GET);
  if (sy_CLASS_PATTERNCONTEXT && NULL == /*_.VALDATA___V50*/ curfptr[49])
  /*_.VALDATA___V50*/ curfptr[49] = (void*) sy_CLASS_PATTERNCONTEXT; }
 
 /*getnamedsym:CLASS_ROOT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ROOT = basilysgc_named_symbol("CLASS_ROOT", BASILYS_GET);
  if (sy_CLASS_ROOT && NULL == /*_.VALDATA___V52*/ curfptr[51])
  /*_.VALDATA___V52*/ curfptr[51] = (void*) sy_CLASS_ROOT; }
 
 /*getnamedsym:PCTN_NORMCTXT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_NORMCTXT = basilysgc_named_symbol("PCTN_NORMCTXT", BASILYS_GET);
  if (sy_PCTN_NORMCTXT && NULL == /*_.VALDATA___V70*/ curfptr[69])
  /*_.VALDATA___V70*/ curfptr[69] = (void*) sy_PCTN_NORMCTXT; }
 
 /*getnamedsym:PCTN_SRC*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_SRC = basilysgc_named_symbol("PCTN_SRC", BASILYS_GET);
  if (sy_PCTN_SRC && NULL == /*_.VALDATA___V72*/ curfptr[71])
  /*_.VALDATA___V72*/ curfptr[71] = (void*) sy_PCTN_SRC; }
 
 /*getnamedsym:PCTN_ENV*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_ENV = basilysgc_named_symbol("PCTN_ENV", BASILYS_GET);
  if (sy_PCTN_ENV && NULL == /*_.VALDATA___V74*/ curfptr[73])
  /*_.VALDATA___V74*/ curfptr[73] = (void*) sy_PCTN_ENV; }
 
 /*getnamedsym:PCTN_MAPATVAR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_MAPATVAR = basilysgc_named_symbol("PCTN_MAPATVAR", BASILYS_GET);
  if (sy_PCTN_MAPATVAR && NULL == /*_.VALDATA___V76*/ curfptr[75])
  /*_.VALDATA___V76*/ curfptr[75] = (void*) sy_PCTN_MAPATVAR; }
 
 /*getnamedsym:PCTN_MAPATCST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_MAPATCST = basilysgc_named_symbol("PCTN_MAPATCST", BASILYS_GET);
  if (sy_PCTN_MAPATCST && NULL == /*_.VALDATA___V78*/ curfptr[77])
  /*_.VALDATA___V78*/ curfptr[77] = (void*) sy_PCTN_MAPATCST; }
 
 /*getnamedsym:PCTN_BINDLIST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_BINDLIST = basilysgc_named_symbol("PCTN_BINDLIST", BASILYS_GET);
  if (sy_PCTN_BINDLIST && NULL == /*_.VALDATA___V80*/ curfptr[79])
  /*_.VALDATA___V80*/ curfptr[79] = (void*) sy_PCTN_BINDLIST; }
 
 /*getnamedsym:SCAN_PATTERN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_SCAN_PATTERN = basilysgc_named_symbol("SCAN_PATTERN", BASILYS_GET);
  if (sy_SCAN_PATTERN && NULL == /*_.VALDATA___V82*/ curfptr[81])
  /*_.VALDATA___V82*/ curfptr[81] = (void*) sy_SCAN_PATTERN; }
 
 /*getnamedsym:CLASS_SELECTOR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SELECTOR = basilysgc_named_symbol("CLASS_SELECTOR", BASILYS_GET);
  if (sy_CLASS_SELECTOR && NULL == /*_.VALDATA___V84*/ curfptr[83])
  /*_.VALDATA___V84*/ curfptr[83] = (void*) sy_CLASS_SELECTOR; }
 
 /*getnamedsym:DEBUG_MSG_FUN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DEBUG_MSG_FUN = basilysgc_named_symbol("DEBUG_MSG_FUN", BASILYS_GET);
  if (sy_DEBUG_MSG_FUN && NULL == /*_.VALDATA___V90*/ curfptr[89])
  /*_.VALDATA___V90*/ curfptr[89] = (void*) sy_DEBUG_MSG_FUN; }
 
 /*getnamedsym:INSTALL_METHOD*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_INSTALL_METHOD = basilysgc_named_symbol("INSTALL_METHOD", BASILYS_GET);
  if (sy_INSTALL_METHOD && NULL == /*_.VALDATA___V92*/ curfptr[91])
  /*_.VALDATA___V92*/ curfptr[91] = (void*) sy_INSTALL_METHOD; }
 
 /*getnamedsym:DISCR_NULLRECV*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_NULLRECV = basilysgc_named_symbol("DISCR_NULLRECV", BASILYS_GET);
  if (sy_DISCR_NULLRECV && NULL == /*_.VALDATA___V94*/ curfptr[93])
  /*_.VALDATA___V94*/ curfptr[93] = (void*) sy_DISCR_NULLRECV; }
 
 /*getnamedsym:CLASS_NAMED*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NAMED = basilysgc_named_symbol("CLASS_NAMED", BASILYS_GET);
  if (sy_CLASS_NAMED && NULL == /*_.VALDATA___V98*/ curfptr[97])
  /*_.VALDATA___V98*/ curfptr[97] = (void*) sy_CLASS_NAMED; }
 
 /*getnamedsym:DISCR_ANYRECV*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_ANYRECV = basilysgc_named_symbol("DISCR_ANYRECV", BASILYS_GET);
  if (sy_DISCR_ANYRECV && NULL == /*_.VALDATA___V100*/ curfptr[99])
  /*_.VALDATA___V100*/ curfptr[99] = (void*) sy_DISCR_ANYRECV; }
 
 /*getnamedsym:CLASS_SRCPATTERN_ANY*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_ANY = basilysgc_named_symbol("CLASS_SRCPATTERN_ANY", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_ANY && NULL == /*_.VALDATA___V104*/ curfptr[103])
  /*_.VALDATA___V104*/ curfptr[103] = (void*) sy_CLASS_SRCPATTERN_ANY; }
 
 /*getnamedsym:CLASS_SRCPATTERN_OR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_OR = basilysgc_named_symbol("CLASS_SRCPATTERN_OR", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_OR && NULL == /*_.VALDATA___V108*/ curfptr[107])
  /*_.VALDATA___V108*/ curfptr[107] = (void*) sy_CLASS_SRCPATTERN_OR; }
 
 /*getnamedsym:CLASS_SRCPATTERN_AND*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_AND = basilysgc_named_symbol("CLASS_SRCPATTERN_AND", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_AND && NULL == /*_.VALDATA___V112*/ curfptr[111])
  /*_.VALDATA___V112*/ curfptr[111] = (void*) sy_CLASS_SRCPATTERN_AND; }
 
 /*getnamedsym:CLASS_SRCPATTERN_VARIABLE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_VARIABLE = basilysgc_named_symbol("CLASS_SRCPATTERN_VARIABLE", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_VARIABLE && NULL == /*_.VALDATA___V116*/ curfptr[115])
  /*_.VALDATA___V116*/ curfptr[115] = (void*) sy_CLASS_SRCPATTERN_VARIABLE; }
 
 /*getnamedsym:CLASS_SRCPATTERN_JOKERVAR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_JOKERVAR = basilysgc_named_symbol("CLASS_SRCPATTERN_JOKERVAR", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_JOKERVAR && NULL == /*_.VALDATA___V120*/ curfptr[119])
  /*_.VALDATA___V120*/ curfptr[119] = (void*) sy_CLASS_SRCPATTERN_JOKERVAR; }
 
 /*getnamedsym:CLASS_SRCPATTERN_CONSTANT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_CONSTANT = basilysgc_named_symbol("CLASS_SRCPATTERN_CONSTANT", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_CONSTANT && NULL == /*_.VALDATA___V124*/ curfptr[123])
  /*_.VALDATA___V124*/ curfptr[123] = (void*) sy_CLASS_SRCPATTERN_CONSTANT; }
 
 /*getnamedsym:GET_CTYPE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_GET_CTYPE = basilysgc_named_symbol("GET_CTYPE", BASILYS_GET);
  if (sy_GET_CTYPE && NULL == /*_.VALDATA___V126*/ curfptr[125])
  /*_.VALDATA___V126*/ curfptr[125] = (void*) sy_GET_CTYPE; }
 
 /*getnamedsym:CLASS_CTYPE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_CTYPE = basilysgc_named_symbol("CLASS_CTYPE", BASILYS_GET);
  if (sy_CLASS_CTYPE && NULL == /*_.VALDATA___V128*/ curfptr[127])
  /*_.VALDATA___V128*/ curfptr[127] = (void*) sy_CLASS_CTYPE; }
 
 /*getnamedsym:NORMAL_EXP*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NORMAL_EXP = basilysgc_named_symbol("NORMAL_EXP", BASILYS_GET);
  if (sy_NORMAL_EXP && NULL == /*_.VALDATA___V130*/ curfptr[129])
  /*_.VALDATA___V130*/ curfptr[129] = (void*) sy_NORMAL_EXP; }
 
 /*getnamedsym:LIST_APPEND2LIST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_LIST_APPEND2LIST = basilysgc_named_symbol("LIST_APPEND2LIST", BASILYS_GET);
  if (sy_LIST_APPEND2LIST && NULL == /*_.VALDATA___V132*/ curfptr[131])
  /*_.VALDATA___V132*/ curfptr[131] = (void*) sy_LIST_APPEND2LIST; }
 
 /*getnamedsym:CLASS_SRCPATTERN_OBJECT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_OBJECT = basilysgc_named_symbol("CLASS_SRCPATTERN_OBJECT", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_OBJECT && NULL == /*_.VALDATA___V136*/ curfptr[135])
  /*_.VALDATA___V136*/ curfptr[135] = (void*) sy_CLASS_SRCPATTERN_OBJECT; }
 
 /*getnamedsym:CLASS_CLASS*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_CLASS = basilysgc_named_symbol("CLASS_CLASS", BASILYS_GET);
  if (sy_CLASS_CLASS && NULL == /*_.VALDATA___V138*/ curfptr[137])
  /*_.VALDATA___V138*/ curfptr[137] = (void*) sy_CLASS_CLASS; }
 
 /*getnamedsym:CLASS_SRCFIELDPATTERN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCFIELDPATTERN = basilysgc_named_symbol("CLASS_SRCFIELDPATTERN", BASILYS_GET);
  if (sy_CLASS_SRCFIELDPATTERN && NULL == /*_.VALDATA___V140*/ curfptr[139])
  /*_.VALDATA___V140*/ curfptr[139] = (void*) sy_CLASS_SRCFIELDPATTERN; }
 
 /*getnamedsym:CLASS_FIELD*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_FIELD = basilysgc_named_symbol("CLASS_FIELD", BASILYS_GET);
  if (sy_CLASS_FIELD && NULL == /*_.VALDATA___V142*/ curfptr[141])
  /*_.VALDATA___V142*/ curfptr[141] = (void*) sy_CLASS_FIELD; }
 
 /*getnamedsym:CTYPE_VALUE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CTYPE_VALUE = basilysgc_named_symbol("CTYPE_VALUE", BASILYS_GET);
  if (sy_CTYPE_VALUE && NULL == /*_.VALDATA___V144*/ curfptr[143])
  /*_.VALDATA___V144*/ curfptr[143] = (void*) sy_CTYPE_VALUE; }
 
 /*getnamedsym:CLASS_SRCPATTERN_COMPOSITE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_COMPOSITE = basilysgc_named_symbol("CLASS_SRCPATTERN_COMPOSITE", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_COMPOSITE && NULL == /*_.VALDATA___V148*/ curfptr[147])
  /*_.VALDATA___V148*/ curfptr[147] = (void*) sy_CLASS_SRCPATTERN_COMPOSITE; }
 
 /*getnamedsym:CLASS_ANY_MATCHER*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ANY_MATCHER = basilysgc_named_symbol("CLASS_ANY_MATCHER", BASILYS_GET);
  if (sy_CLASS_ANY_MATCHER && NULL == /*_.VALDATA___V150*/ curfptr[149])
  /*_.VALDATA___V150*/ curfptr[149] = (void*) sy_CLASS_ANY_MATCHER; }
 
 /*getnamedsym:CLASS_FORMAL_BINDING*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_FORMAL_BINDING = basilysgc_named_symbol("CLASS_FORMAL_BINDING", BASILYS_GET);
  if (sy_CLASS_FORMAL_BINDING && NULL == /*_.VALDATA___V152*/ curfptr[151])
  /*_.VALDATA___V152*/ curfptr[151] = (void*) sy_CLASS_FORMAL_BINDING; }
 
 /*getnamedsym:NORMALIZE_TUPLE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NORMALIZE_TUPLE = basilysgc_named_symbol("NORMALIZE_TUPLE", BASILYS_GET);
  if (sy_NORMALIZE_TUPLE && NULL == /*_.VALDATA___V154*/ curfptr[153])
  /*_.VALDATA___V154*/ curfptr[153] = (void*) sy_NORMALIZE_TUPLE; }
 
 /*getnamedsym:CLASS_NREP_BACKPOINT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_BACKPOINT = basilysgc_named_symbol("CLASS_NREP_BACKPOINT", BASILYS_GET);
  if (sy_CLASS_NREP_BACKPOINT && NULL == /*_.VALDATA___V156*/ curfptr[155])
  /*_.VALDATA___V156*/ curfptr[155] = (void*) sy_CLASS_NREP_BACKPOINT; }
 
 /*getnamedsym:CLASS_NREP_TYPEXPR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TYPEXPR = basilysgc_named_symbol("CLASS_NREP_TYPEXPR", BASILYS_GET);
  if (sy_CLASS_NREP_TYPEXPR && NULL == /*_.VALDATA___V158*/ curfptr[157])
  /*_.VALDATA___V158*/ curfptr[157] = (void*) sy_CLASS_NREP_TYPEXPR; }
 
 /*getnamedsym:NBKP_DO*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NBKP_DO = basilysgc_named_symbol("NBKP_DO", BASILYS_GET);
  if (sy_NBKP_DO && NULL == /*_.VALDATA___V168*/ curfptr[167])
  /*_.VALDATA___V168*/ curfptr[167] = (void*) sy_NBKP_DO; }
 
 /*getnamedsym:NBKP_FAIL*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NBKP_FAIL = basilysgc_named_symbol("NBKP_FAIL", BASILYS_GET);
  if (sy_NBKP_FAIL && NULL == /*_.VALDATA___V170*/ curfptr[169])
  /*_.VALDATA___V170*/ curfptr[169] = (void*) sy_NBKP_FAIL; }
 
 /*getnamedsym:CLASS_NREP_TESTPOINT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TESTPOINT = basilysgc_named_symbol("CLASS_NREP_TESTPOINT", BASILYS_GET);
  if (sy_CLASS_NREP_TESTPOINT && NULL == /*_.VALDATA___V172*/ curfptr[171])
  /*_.VALDATA___V172*/ curfptr[171] = (void*) sy_CLASS_NREP_TESTPOINT; }
 
 /*getnamedsym:NTSP_FAILP*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSP_FAILP = basilysgc_named_symbol("NTSP_FAILP", BASILYS_GET);
  if (sy_NTSP_FAILP && NULL == /*_.VALDATA___V184*/ curfptr[183])
  /*_.VALDATA___V184*/ curfptr[183] = (void*) sy_NTSP_FAILP; }
 
 /*getnamedsym:NTSP_SUCCP*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSP_SUCCP = basilysgc_named_symbol("NTSP_SUCCP", BASILYS_GET);
  if (sy_NTSP_SUCCP && NULL == /*_.VALDATA___V186*/ curfptr[185])
  /*_.VALDATA___V186*/ curfptr[185] = (void*) sy_NTSP_SUCCP; }
 
 /*getnamedsym:NTSP_TEST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSP_TEST = basilysgc_named_symbol("NTSP_TEST", BASILYS_GET);
  if (sy_NTSP_TEST && NULL == /*_.VALDATA___V188*/ curfptr[187])
  /*_.VALDATA___V188*/ curfptr[187] = (void*) sy_NTSP_TEST; }
 
 /*getnamedsym:CLASS_NREP_TESTP_MATCHER*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TESTP_MATCHER = basilysgc_named_symbol("CLASS_NREP_TESTP_MATCHER", BASILYS_GET);
  if (sy_CLASS_NREP_TESTP_MATCHER && NULL == /*_.VALDATA___V190*/ curfptr[189])
  /*_.VALDATA___V190*/ curfptr[189] = (void*) sy_CLASS_NREP_TESTP_MATCHER; }
 
 /*getnamedsym:NTSM_MATCHER*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSM_MATCHER = basilysgc_named_symbol("NTSM_MATCHER", BASILYS_GET);
  if (sy_NTSM_MATCHER && NULL == /*_.VALDATA___V200*/ curfptr[199])
  /*_.VALDATA___V200*/ curfptr[199] = (void*) sy_NTSM_MATCHER; }
 
 /*getnamedsym:NTSM_BINDS*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSM_BINDS = basilysgc_named_symbol("NTSM_BINDS", BASILYS_GET);
  if (sy_NTSM_BINDS && NULL == /*_.VALDATA___V202*/ curfptr[201])
  /*_.VALDATA___V202*/ curfptr[201] = (void*) sy_NTSM_BINDS; }
 
 /*getnamedsym:CLASS_NREP_TESTP_ISA*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TESTP_ISA = basilysgc_named_symbol("CLASS_NREP_TESTP_ISA", BASILYS_GET);
  if (sy_CLASS_NREP_TESTP_ISA && NULL == /*_.VALDATA___V204*/ curfptr[203])
  /*_.VALDATA___V204*/ curfptr[203] = (void*) sy_CLASS_NREP_TESTP_ISA; }
 
 /*getnamedsym:NTSI_CLASS*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSI_CLASS = basilysgc_named_symbol("NTSI_CLASS", BASILYS_GET);
  if (sy_NTSI_CLASS && NULL == /*_.VALDATA___V212*/ curfptr[211])
  /*_.VALDATA___V212*/ curfptr[211] = (void*) sy_NTSI_CLASS; }
 
 /*getnamedsym:CLASS_NREP_TESTP_DISCRIM*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TESTP_DISCRIM = basilysgc_named_symbol("CLASS_NREP_TESTP_DISCRIM", BASILYS_GET);
  if (sy_CLASS_NREP_TESTP_DISCRIM && NULL == /*_.VALDATA___V214*/ curfptr[213])
  /*_.VALDATA___V214*/ curfptr[213] = (void*) sy_CLASS_NREP_TESTP_DISCRIM; }
 
 /*getnamedsym:CLASS_NREP_TESTP_COND*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TESTP_COND = basilysgc_named_symbol("CLASS_NREP_TESTP_COND", BASILYS_GET);
  if (sy_CLASS_NREP_TESTP_COND && NULL == /*_.VALDATA___V220*/ curfptr[219])
  /*_.VALDATA___V220*/ curfptr[219] = (void*) sy_CLASS_NREP_TESTP_COND; }
 
 /*getnamedsym:NTSC_COND*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSC_COND = basilysgc_named_symbol("NTSC_COND", BASILYS_GET);
  if (sy_NTSC_COND && NULL == /*_.VALDATA___V228*/ curfptr[227])
  /*_.VALDATA___V228*/ curfptr[227] = (void*) sy_NTSC_COND; }
 
 /*getnamedsym:NORMAL_PATTERN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NORMAL_PATTERN = basilysgc_named_symbol("NORMAL_PATTERN", BASILYS_GET);
  if (sy_NORMAL_PATTERN && NULL == /*_.VALDATA___V230*/ curfptr[229])
  /*_.VALDATA___V230*/ curfptr[229] = (void*) sy_NORMAL_PATTERN; }
 
 /*getnamedsym:ADD_WHEN_PATTERN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_ADD_WHEN_PATTERN = basilysgc_named_symbol("ADD_WHEN_PATTERN", BASILYS_GET);
  if (sy_ADD_WHEN_PATTERN && NULL == /*_.VALDATA___V234*/ curfptr[233])
  /*_.VALDATA___V234*/ curfptr[233] = (void*) sy_ADD_WHEN_PATTERN; }
 
 /*getnamedsym:CLASS_SRC*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRC = basilysgc_named_symbol("CLASS_SRC", BASILYS_GET);
  if (sy_CLASS_SRC && NULL == /*_.VALDATA___V240*/ curfptr[239])
  /*_.VALDATA___V240*/ curfptr[239] = (void*) sy_CLASS_SRC; }
 
 /*getnamedsym:CLASS_SRCPATTERN_INSTANCE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_INSTANCE = basilysgc_named_symbol("CLASS_SRCPATTERN_INSTANCE", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_INSTANCE && NULL == /*_.VALDATA___V246*/ curfptr[245])
  /*_.VALDATA___V246*/ curfptr[245] = (void*) sy_CLASS_SRCPATTERN_INSTANCE; }
 
 /*getnamedsym:CLASS_SRCPATTERN_ANYMATCH*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_ANYMATCH = basilysgc_named_symbol("CLASS_SRCPATTERN_ANYMATCH", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_ANYMATCH && NULL == /*_.VALDATA___V250*/ curfptr[249])
  /*_.VALDATA___V250*/ curfptr[249] = (void*) sy_CLASS_SRCPATTERN_ANYMATCH; }
 
 /*getnamedsym:FRESH_ENV*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_FRESH_ENV = basilysgc_named_symbol("FRESH_ENV", BASILYS_GET);
  if (sy_FRESH_ENV && NULL == /*_.VALDATA___V252*/ curfptr[251])
  /*_.VALDATA___V252*/ curfptr[251] = (void*) sy_FRESH_ENV; }
 
 /*getnamedsym:CLASS_ANY_BINDING*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ANY_BINDING = basilysgc_named_symbol("CLASS_ANY_BINDING", BASILYS_GET);
  if (sy_CLASS_ANY_BINDING && NULL == /*_.VALDATA___V254*/ curfptr[253])
  /*_.VALDATA___V254*/ curfptr[253] = (void*) sy_CLASS_ANY_BINDING; }
 
 /*getnamedsym:CLASS_NORMLET_BINDING*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NORMLET_BINDING = basilysgc_named_symbol("CLASS_NORMLET_BINDING", BASILYS_GET);
  if (sy_CLASS_NORMLET_BINDING && NULL == /*_.VALDATA___V256*/ curfptr[255])
  /*_.VALDATA___V256*/ curfptr[255] = (void*) sy_CLASS_NORMLET_BINDING; }
 
 /*getnamedsym:PUT_ENV*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PUT_ENV = basilysgc_named_symbol("PUT_ENV", BASILYS_GET);
  if (sy_PUT_ENV && NULL == /*_.VALDATA___V258*/ curfptr[257])
  /*_.VALDATA___V258*/ curfptr[257] = (void*) sy_PUT_ENV; }
 
 /*getnamedsym:CLONE_SYMBOL*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLONE_SYMBOL = basilysgc_named_symbol("CLONE_SYMBOL", BASILYS_GET);
  if (sy_CLONE_SYMBOL && NULL == /*_.VALDATA___V260*/ curfptr[259])
  /*_.VALDATA___V260*/ curfptr[259] = (void*) sy_CLONE_SYMBOL; }
 
 /*getnamedsym:CLASS_NREP_LOCSYMOCC*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_LOCSYMOCC = basilysgc_named_symbol("CLASS_NREP_LOCSYMOCC", BASILYS_GET);
  if (sy_CLASS_NREP_LOCSYMOCC && NULL == /*_.VALDATA___V262*/ curfptr[261])
  /*_.VALDATA___V262*/ curfptr[261] = (void*) sy_CLASS_NREP_LOCSYMOCC; }
 
 /*getnamedsym:CLASS_SRC_MATCH*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRC_MATCH = basilysgc_named_symbol("CLASS_SRC_MATCH", BASILYS_GET);
  if (sy_CLASS_SRC_MATCH && NULL == /*_.VALDATA___V266*/ curfptr[265])
  /*_.VALDATA___V266*/ curfptr[265] = (void*) sy_CLASS_SRC_MATCH; }
 
 /*getnamedsym:CLASS_ENVIRONMENT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ENVIRONMENT = basilysgc_named_symbol("CLASS_ENVIRONMENT", BASILYS_GET);
  if (sy_CLASS_ENVIRONMENT && NULL == /*_.VALDATA___V268*/ curfptr[267])
  /*_.VALDATA___V268*/ curfptr[267] = (void*) sy_CLASS_ENVIRONMENT; }
 
 /*getnamedsym:CLASS_NORMCONTEXT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NORMCONTEXT = basilysgc_named_symbol("CLASS_NORMCONTEXT", BASILYS_GET);
  if (sy_CLASS_NORMCONTEXT && NULL == /*_.VALDATA___V270*/ curfptr[269])
  /*_.VALDATA___V270*/ curfptr[269] = (void*) sy_CLASS_NORMCONTEXT; }
 
 /*getnamedsym:DISCR_MULTIPLE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_MULTIPLE = basilysgc_named_symbol("DISCR_MULTIPLE", BASILYS_GET);
  if (sy_DISCR_MULTIPLE && NULL == /*_.VALDATA___V272*/ curfptr[271])
  /*_.VALDATA___V272*/ curfptr[271] = (void*) sy_DISCR_MULTIPLE; }
 
 /*getnamedsym:DISCR_LIST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_LIST = basilysgc_named_symbol("DISCR_LIST", BASILYS_GET);
  if (sy_DISCR_LIST && NULL == /*_.VALDATA___V274*/ curfptr[273])
  /*_.VALDATA___V274*/ curfptr[273] = (void*) sy_DISCR_LIST; }
 
 /*getnamedsym:CLASS_SRC_CASEMATCH*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRC_CASEMATCH = basilysgc_named_symbol("CLASS_SRC_CASEMATCH", BASILYS_GET);
  if (sy_CLASS_SRC_CASEMATCH && NULL == /*_.VALDATA___V276*/ curfptr[275])
  /*_.VALDATA___V276*/ curfptr[275] = (void*) sy_CLASS_SRC_CASEMATCH; }
 
 /*getnamedsym:DISCR_MAPOBJECTS*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_MAPOBJECTS = basilysgc_named_symbol("DISCR_MAPOBJECTS", BASILYS_GET);
  if (sy_DISCR_MAPOBJECTS && NULL == /*_.VALDATA___V278*/ curfptr[277])
  /*_.VALDATA___V278*/ curfptr[277] = (void*) sy_DISCR_MAPOBJECTS; }
 
 /**COMMENT: get the value importer **/;
 
 { basilys_ptr_t slot=0;
  basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 12, "SYSDATA_VALUE_IMPORTER");/*_.VALIMPORT___V6*/ curfptr[5] = slot; };
 
 /**COMMENT: before getting imported values **/;
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ROOT__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SELECTOR__V8*/ curfptr[7] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V84*/ curfptr[83]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DEBUG_MSG_FUN__V9*/ curfptr[8] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V90*/ curfptr[89]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.INSTALL_METHOD__V10*/ curfptr[9] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V92*/ curfptr[91]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_NULLRECV__V11*/ curfptr[10] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V94*/ curfptr[93]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NAMED__V12*/ curfptr[11] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V98*/ curfptr[97]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_ANYRECV__V13*/ curfptr[12] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V100*/ curfptr[99]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_ANY__V14*/ curfptr[13] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V104*/ curfptr[103]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_OR__V15*/ curfptr[14] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V108*/ curfptr[107]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_AND__V16*/ curfptr[15] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V112*/ curfptr[111]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_VARIABLE__V17*/ curfptr[16] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V116*/ curfptr[115]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_JOKERVAR__V18*/ curfptr[17] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V120*/ curfptr[119]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_CONSTANT__V19*/ curfptr[18] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V124*/ curfptr[123]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.GET_CTYPE__V20*/ curfptr[19] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V126*/ curfptr[125]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_CTYPE__V21*/ curfptr[20] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V128*/ curfptr[127]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.NORMAL_EXP__V22*/ curfptr[21] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V130*/ curfptr[129]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.LIST_APPEND2LIST__V23*/ curfptr[22] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V132*/ curfptr[131]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_OBJECT__V24*/ curfptr[23] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V136*/ curfptr[135]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_CLASS__V25*/ curfptr[24] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V138*/ curfptr[137]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCFIELDPATTERN__V26*/ curfptr[25] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V140*/ curfptr[139]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_FIELD__V27*/ curfptr[26] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V142*/ curfptr[141]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CTYPE_VALUE__V28*/ curfptr[27] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_COMPOSITE__V29*/ curfptr[28] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V148*/ curfptr[147]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ANY_MATCHER__V30*/ curfptr[29] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V150*/ curfptr[149]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_FORMAL_BINDING__V31*/ curfptr[30] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V152*/ curfptr[151]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.NORMALIZE_TUPLE__V32*/ curfptr[31] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V154*/ curfptr[153]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRC__V34*/ curfptr[33] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V240*/ curfptr[239]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_INSTANCE__V35*/ curfptr[34] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V246*/ curfptr[245]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_ANYMATCH__V36*/ curfptr[35] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V250*/ curfptr[249]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.FRESH_ENV__V37*/ curfptr[36] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V252*/ curfptr[251]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ANY_BINDING__V38*/ curfptr[37] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V254*/ curfptr[253]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NORMLET_BINDING__V39*/ curfptr[38] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V256*/ curfptr[255]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.PUT_ENV__V40*/ curfptr[39] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V258*/ curfptr[257]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLONE_SYMBOL__V41*/ curfptr[40] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V260*/ curfptr[259]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NREP_LOCSYMOCC__V42*/ curfptr[41] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V262*/ curfptr[261]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRC_MATCH__V43*/ curfptr[42] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V266*/ curfptr[265]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ENVIRONMENT__V44*/ curfptr[43] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V268*/ curfptr[267]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NORMCONTEXT__V45*/ curfptr[44] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V270*/ curfptr[269]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_MULTIPLE__V46*/ curfptr[45] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V272*/ curfptr[271]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_LIST__V47*/ curfptr[46] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V274*/ curfptr[273]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRC_CASEMATCH__V48*/ curfptr[47] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V276*/ curfptr[275]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_MAPOBJECTS__V49*/ curfptr[48] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V278*/ curfptr[277]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /**COMMENT: after getting imported values **/;
 
 /**initial routine cdata initializer**/

 memset(predefinited, 0, sizeof(predefinited));
 initialize_module_cdata(&curfram__, predefinited);
 /**initial routine body**/
/**COMMENT: start of oinibody **/;
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V51*/ curfptr[50]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V50*/ curfptr[49]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V53*/ curfptr[52]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V52*/ curfptr[51]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V55*/ curfptr[54]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.CLASS_ROOT__V7*/ curfptr[6]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V56*/ curfptr[55]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V56*/ curfptr[55]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V56*/ curfptr[55]))->tabval[0] = (basilys_ptr_t)(/*_.CLASS_ROOT__V7*/ curfptr[6]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V56*/ curfptr[55]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V56*/ curfptr[55]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V57*/ curfptr[56])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V57*/ curfptr[56]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V57*/ curfptr[56]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V58*/ curfptr[57]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V57*/ curfptr[56])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V57*/ curfptr[56]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V57*/ curfptr[56]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V57*/ curfptr[56]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V60*/ curfptr[59]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V59*/ curfptr[58]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V62*/ curfptr[61]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V61*/ curfptr[60]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V64*/ curfptr[63]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V63*/ curfptr[62]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V66*/ curfptr[65]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V65*/ curfptr[64]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V68*/ curfptr[67]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V67*/ curfptr[66]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))->tabval[0] = (basilys_ptr_t)(/*_.VALDATA___V57*/ curfptr[56]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (5>=0 && 5< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V69*/ curfptr[68]))->tabval[5] = (basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V69*/ curfptr[68]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V69*/ curfptr[68]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V54*/ curfptr[53]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V70*/ curfptr[69])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V70*/ curfptr[69]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V70*/ curfptr[69]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V71*/ curfptr[70]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V70*/ curfptr[69]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V72*/ curfptr[71])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V72*/ curfptr[71]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V72*/ curfptr[71]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V73*/ curfptr[72]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V72*/ curfptr[71]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V74*/ curfptr[73])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V74*/ curfptr[73]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V74*/ curfptr[73]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V75*/ curfptr[74]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V74*/ curfptr[73]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V76*/ curfptr[75])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V76*/ curfptr[75]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V76*/ curfptr[75]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V77*/ curfptr[76]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V76*/ curfptr[75]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V78*/ curfptr[77])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V78*/ curfptr[77]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V78*/ curfptr[77]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V79*/ curfptr[78]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V78*/ curfptr[77]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V80*/ curfptr[79])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V80*/ curfptr[79]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V80*/ curfptr[79]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V81*/ curfptr[80]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V80*/ curfptr[79]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V82*/ curfptr[81])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V82*/ curfptr[81]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V82*/ curfptr[81]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V83*/ curfptr[82]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V82*/ curfptr[81]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V84*/ curfptr[83])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V84*/ curfptr[83]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V84*/ curfptr[83]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V85*/ curfptr[84]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V84*/ curfptr[83]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:52:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V86*/ curfptr[85])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V86*/ curfptr[85]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V86*/ curfptr[85]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V87*/ curfptr[86]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:52:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V86*/ curfptr[85]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V88*/ curfptr[87])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_39__SCANPAT_NIL#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V88*/ curfptr[87])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V88*/ curfptr[87])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V88*/ curfptr[87])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V88*/ curfptr[87]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V89*/ curfptr[88])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V88*/ curfptr[87])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V89*/ curfptr[88])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V88*/ curfptr[87]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V89*/ curfptr[88]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V90*/ curfptr[89])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V90*/ curfptr[89]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V90*/ curfptr[89]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V91*/ curfptr[90]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V90*/ curfptr[89]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V92*/ curfptr[91])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V92*/ curfptr[91]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V92*/ curfptr[91]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V93*/ curfptr[92]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V92*/ curfptr[91]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V94*/ curfptr[93])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V94*/ curfptr[93]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V94*/ curfptr[93]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V95*/ curfptr[94]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V94*/ curfptr[93]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V96*/ curfptr[95])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_47__SCANPAT_ANYRECV#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V96*/ curfptr[95])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V96*/ curfptr[95])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V96*/ curfptr[95])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V96*/ curfptr[95])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_47__SCANPAT_ANYRECV#2", NULL != (/*_.CLASS_NAMED__V12*/ curfptr[11]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V96*/ curfptr[95])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_NAMED__V12*/ curfptr[11]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:69:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V96*/ curfptr[95]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V97*/ curfptr[96])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V96*/ curfptr[95])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V97*/ curfptr[96])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V96*/ curfptr[95]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V97*/ curfptr[96]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V98*/ curfptr[97])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V98*/ curfptr[97]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V98*/ curfptr[97]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V99*/ curfptr[98]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V98*/ curfptr[97]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V100*/ curfptr[99])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V100*/ curfptr[99]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V100*/ curfptr[99]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V101*/ curfptr[100]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V100*/ curfptr[99]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V102*/ curfptr[101])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_53__SCANPAT_ANYPATTERN#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V102*/ curfptr[101])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V102*/ curfptr[101])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_53__SCANPAT_ANYPATTERN#1", NULL != (/*_.CLASS_SRCPATTERN_ANY__V14*/ curfptr[13]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V102*/ curfptr[101])->tabval[1] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_ANY__V14*/ curfptr[13]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V102*/ curfptr[101])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V102*/ curfptr[101])->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V102*/ curfptr[101])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_53__SCANPAT_ANYPATTERN#3", NULL != (/*_.CLASS_NAMED__V12*/ curfptr[11]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V102*/ curfptr[101])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NAMED__V12*/ curfptr[11]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:83:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V102*/ curfptr[101]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V103*/ curfptr[102])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V102*/ curfptr[101])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V103*/ curfptr[102])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V102*/ curfptr[101]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V103*/ curfptr[102]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V104*/ curfptr[103])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V104*/ curfptr[103]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V104*/ curfptr[103]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V105*/ curfptr[104]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V104*/ curfptr[103]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V106*/ curfptr[105])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_57__SCANPAT_SRCPATOR#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V106*/ curfptr[105])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V106*/ curfptr[105])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V106*/ curfptr[105])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V106*/ curfptr[105])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_57__SCANPAT_SRCPATOR#2", NULL != (/*_.CLASS_SRCPATTERN_OR__V15*/ curfptr[14]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V106*/ curfptr[105])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OR__V15*/ curfptr[14]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V106*/ curfptr[105])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V86*/ curfptr[85]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V106*/ curfptr[105])->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V86*/ curfptr[85]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:98:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V106*/ curfptr[105]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V107*/ curfptr[106])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V106*/ curfptr[105])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V107*/ curfptr[106])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V106*/ curfptr[105]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V107*/ curfptr[106]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V108*/ curfptr[107])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V108*/ curfptr[107]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V108*/ curfptr[107]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V109*/ curfptr[108]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V108*/ curfptr[107]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V110*/ curfptr[109])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_61__SCANPAT_SRCPATAND#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V110*/ curfptr[109])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V110*/ curfptr[109])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V110*/ curfptr[109])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V110*/ curfptr[109])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_61__SCANPAT_SRCPATAND#2", NULL != (/*_.CLASS_SRCPATTERN_AND__V16*/ curfptr[15]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V110*/ curfptr[109])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_AND__V16*/ curfptr[15]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V110*/ curfptr[109])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V86*/ curfptr[85]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V110*/ curfptr[109])->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V86*/ curfptr[85]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:117:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V110*/ curfptr[109]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V111*/ curfptr[110])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V110*/ curfptr[109])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V111*/ curfptr[110])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V110*/ curfptr[109]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V111*/ curfptr[110]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V112*/ curfptr[111])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V112*/ curfptr[111]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V112*/ curfptr[111]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V113*/ curfptr[112]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V112*/ curfptr[111]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V114*/ curfptr[113])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATVAR#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V114*/ curfptr[113])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V114*/ curfptr[113])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V114*/ curfptr[113])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V114*/ curfptr[113])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATVAR#2", NULL != (/*_.CLASS_SRCPATTERN_VARIABLE__V17*/ curfptr[16]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V114*/ curfptr[113])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_VARIABLE__V17*/ curfptr[16]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V114*/ curfptr[113]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V115*/ curfptr[114])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V114*/ curfptr[113])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V115*/ curfptr[114])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V114*/ curfptr[113]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V115*/ curfptr[114]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V116*/ curfptr[115])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V116*/ curfptr[115]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V116*/ curfptr[115]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V117*/ curfptr[116]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V116*/ curfptr[115]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V118*/ curfptr[117])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_69__SCANPAT_SRCPATJOKER#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V118*/ curfptr[117])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V118*/ curfptr[117])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V118*/ curfptr[117])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V118*/ curfptr[117])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_69__SCANPAT_SRCPATJOKER#2", NULL != (/*_.CLASS_SRCPATTERN_JOKERVAR__V18*/ curfptr[17]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V118*/ curfptr[117])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_JOKERVAR__V18*/ curfptr[17]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:160:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V118*/ curfptr[117]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V119*/ curfptr[118])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V118*/ curfptr[117])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V119*/ curfptr[118])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V118*/ curfptr[117]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V119*/ curfptr[118]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V120*/ curfptr[119])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V120*/ curfptr[119]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V120*/ curfptr[119]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V121*/ curfptr[120]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V120*/ curfptr[119]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_73__SCANPAT_SRCPATCONST#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V122*/ curfptr[121])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V122*/ curfptr[121])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_73__SCANPAT_SRCPATCONST#2", NULL != (/*_.CLASS_SRCPATTERN_CONSTANT__V19*/ curfptr[18]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V122*/ curfptr[121])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_CONSTANT__V19*/ curfptr[18]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_73__SCANPAT_SRCPATCONST#3", NULL != (/*_.GET_CTYPE__V20*/ curfptr[19]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V122*/ curfptr[121])->tabval[3] = (basilys_ptr_t)(/*_.GET_CTYPE__V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_73__SCANPAT_SRCPATCONST#4", NULL != (/*_.CLASS_CTYPE__V21*/ curfptr[20]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V122*/ curfptr[121])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_CTYPE__V21*/ curfptr[20]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_73__SCANPAT_SRCPATCONST#5", NULL != (/*_.NORMAL_EXP__V22*/ curfptr[21]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V122*/ curfptr[121])->tabval[5] = (basilys_ptr_t)(/*_.NORMAL_EXP__V22*/ curfptr[21]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_73__SCANPAT_SRCPATCONST#6", NULL != (/*_.LIST_APPEND2LIST__V23*/ curfptr[22]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V122*/ curfptr[121])->tabval[6] = (basilys_ptr_t)(/*_.LIST_APPEND2LIST__V23*/ curfptr[22]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:173:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V122*/ curfptr[121]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V123*/ curfptr[122])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V122*/ curfptr[121])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V123*/ curfptr[122])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V122*/ curfptr[121]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V123*/ curfptr[122]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V124*/ curfptr[123])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V124*/ curfptr[123]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V124*/ curfptr[123]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V125*/ curfptr[124]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V124*/ curfptr[123]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V126*/ curfptr[125])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V126*/ curfptr[125]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V126*/ curfptr[125]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V127*/ curfptr[126]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V126*/ curfptr[125]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V128*/ curfptr[127])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V128*/ curfptr[127]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V128*/ curfptr[127]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V129*/ curfptr[128]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V128*/ curfptr[127]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V130*/ curfptr[129])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V130*/ curfptr[129]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V130*/ curfptr[129]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V131*/ curfptr[130]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V130*/ curfptr[129]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V132*/ curfptr[131])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V132*/ curfptr[131]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V132*/ curfptr[131]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V133*/ curfptr[132]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V132*/ curfptr[131]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_85__SCANPAT_SRCPATOBJECT#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_85__SCANPAT_SRCPATOBJECT#2", NULL != (/*_.CLASS_SRCPATTERN_OBJECT__V24*/ curfptr[23]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OBJECT__V24*/ curfptr[23]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_85__SCANPAT_SRCPATOBJECT#3", NULL != (/*_.CLASS_CLASS__V25*/ curfptr[24]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_CLASS__V25*/ curfptr[24]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_85__SCANPAT_SRCPATOBJECT#4", NULL != (/*_.CLASS_SRCFIELDPATTERN__V26*/ curfptr[25]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_SRCFIELDPATTERN__V26*/ curfptr[25]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_85__SCANPAT_SRCPATOBJECT#5", NULL != (/*_.CLASS_FIELD__V27*/ curfptr[26]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[5] = (basilys_ptr_t)(/*_.CLASS_FIELD__V27*/ curfptr[26]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_85__SCANPAT_SRCPATOBJECT#6", NULL != (/*_.CLASS_NAMED__V12*/ curfptr[11]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[6] = (basilys_ptr_t)(/*_.CLASS_NAMED__V12*/ curfptr[11]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V86*/ curfptr[85]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[7] = (basilys_ptr_t)(/*_.VALDATA___V86*/ curfptr[85]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_85__SCANPAT_SRCPATOBJECT#8", NULL != (/*_.CTYPE_VALUE__V28*/ curfptr[27]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V134*/ curfptr[133])->tabval[8] = (basilys_ptr_t)(/*_.CTYPE_VALUE__V28*/ curfptr[27]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:218:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V134*/ curfptr[133]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V135*/ curfptr[134])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V134*/ curfptr[133])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V135*/ curfptr[134])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V134*/ curfptr[133]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V135*/ curfptr[134]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V136*/ curfptr[135])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V136*/ curfptr[135]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V136*/ curfptr[135]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V137*/ curfptr[136]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V136*/ curfptr[135]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V138*/ curfptr[137])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V138*/ curfptr[137]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V138*/ curfptr[137]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V139*/ curfptr[138]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V138*/ curfptr[137]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V140*/ curfptr[139])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V140*/ curfptr[139]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V140*/ curfptr[139]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V141*/ curfptr[140]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V140*/ curfptr[139]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V142*/ curfptr[141])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V142*/ curfptr[141]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V142*/ curfptr[141]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V143*/ curfptr[142]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V142*/ curfptr[141]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V145*/ curfptr[144]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V144*/ curfptr[143]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[0] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_97__SCANPAT_SRCPATCOMPOSITE#1", NULL != (/*_.CLASS_SRCPATTERN_COMPOSITE__V29*/ curfptr[28]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[1] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_COMPOSITE__V29*/ curfptr[28]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_97__SCANPAT_SRCPATCOMPOSITE#2", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[2] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_97__SCANPAT_SRCPATCOMPOSITE#3", NULL != (/*_.CLASS_ANY_MATCHER__V30*/ curfptr[29]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_ANY_MATCHER__V30*/ curfptr[29]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_97__SCANPAT_SRCPATCOMPOSITE#4", NULL != (/*_.CLASS_FORMAL_BINDING__V31*/ curfptr[30]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_FORMAL_BINDING__V31*/ curfptr[30]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_97__SCANPAT_SRCPATCOMPOSITE#5", NULL != (/*_.CLASS_CTYPE__V21*/ curfptr[20]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[5] = (basilys_ptr_t)(/*_.CLASS_CTYPE__V21*/ curfptr[20]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_97__SCANPAT_SRCPATCOMPOSITE#6", NULL != (/*_.NORMALIZE_TUPLE__V32*/ curfptr[31]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[6] = (basilys_ptr_t)(/*_.NORMALIZE_TUPLE__V32*/ curfptr[31]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_97__SCANPAT_SRCPATCOMPOSITE#7", NULL != (/*_.LIST_APPEND2LIST__V23*/ curfptr[22]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[7] = (basilys_ptr_t)(/*_.LIST_APPEND2LIST__V23*/ curfptr[22]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V86*/ curfptr[85]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V146*/ curfptr[145])->tabval[8] = (basilys_ptr_t)(/*_.VALDATA___V86*/ curfptr[85]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:256:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V146*/ curfptr[145]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V147*/ curfptr[146])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V146*/ curfptr[145])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V147*/ curfptr[146])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V146*/ curfptr[145]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V147*/ curfptr[146]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V148*/ curfptr[147])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V148*/ curfptr[147]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V148*/ curfptr[147]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V149*/ curfptr[148]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V148*/ curfptr[147]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V150*/ curfptr[149])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V150*/ curfptr[149]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V150*/ curfptr[149]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V151*/ curfptr[150]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V150*/ curfptr[149]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V152*/ curfptr[151])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V152*/ curfptr[151]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V152*/ curfptr[151]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V153*/ curfptr[152]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V152*/ curfptr[151]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V154*/ curfptr[153])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V154*/ curfptr[153]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V154*/ curfptr[153]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V155*/ curfptr[154]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V154*/ curfptr[153]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V156*/ curfptr[155])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V156*/ curfptr[155]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V156*/ curfptr[155]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V157*/ curfptr[156]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V156*/ curfptr[155]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V159*/ curfptr[158]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V158*/ curfptr[157]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V161*/ curfptr[160]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V162*/ curfptr[161]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V162*/ curfptr[161]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V164*/ curfptr[163]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V160*/ curfptr[159]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V163*/ curfptr[162]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V166*/ curfptr[165]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V160*/ curfptr[159]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V165*/ curfptr[164]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V167*/ curfptr[166]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V167*/ curfptr[166]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V167*/ curfptr[166]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:304:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V160*/ curfptr[159]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V168*/ curfptr[167])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V168*/ curfptr[167]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V168*/ curfptr[167]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V169*/ curfptr[168]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V168*/ curfptr[167]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V170*/ curfptr[169])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V170*/ curfptr[169]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V170*/ curfptr[169]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V171*/ curfptr[170]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V170*/ curfptr[169]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V172*/ curfptr[171])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V172*/ curfptr[171]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V172*/ curfptr[171]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V173*/ curfptr[172]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V172*/ curfptr[171]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V175*/ curfptr[174]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V176*/ curfptr[175]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V176*/ curfptr[175]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V176*/ curfptr[175]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V178*/ curfptr[177]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V174*/ curfptr[173]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V177*/ curfptr[176]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V180*/ curfptr[179]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V174*/ curfptr[173]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V179*/ curfptr[178]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V182*/ curfptr[181]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V174*/ curfptr[173]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V181*/ curfptr[180]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V183*/ curfptr[182]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V183*/ curfptr[182]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V183*/ curfptr[182]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:311:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V174*/ curfptr[173]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V184*/ curfptr[183])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V184*/ curfptr[183]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V184*/ curfptr[183]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V185*/ curfptr[184]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V184*/ curfptr[183]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V186*/ curfptr[185])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V186*/ curfptr[185]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V186*/ curfptr[185]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V187*/ curfptr[186]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V186*/ curfptr[185]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V188*/ curfptr[187])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V188*/ curfptr[187]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V188*/ curfptr[187]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V189*/ curfptr[188]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V188*/ curfptr[187]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V190*/ curfptr[189])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V190*/ curfptr[189]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V190*/ curfptr[189]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V191*/ curfptr[190]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V190*/ curfptr[189]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V193*/ curfptr[192]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.VALDATA___V174*/ curfptr[173]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V194*/ curfptr[193]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V194*/ curfptr[193]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V194*/ curfptr[193]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V195*/ curfptr[194])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V195*/ curfptr[194]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V195*/ curfptr[194]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V196*/ curfptr[195]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V195*/ curfptr[194])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V195*/ curfptr[194]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V195*/ curfptr[194]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V192*/ curfptr[191]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V195*/ curfptr[194]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V197*/ curfptr[196])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V197*/ curfptr[196]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V197*/ curfptr[196]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V198*/ curfptr[197]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V197*/ curfptr[196])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V197*/ curfptr[196]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V197*/ curfptr[196]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V192*/ curfptr[191]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V197*/ curfptr[196]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (5>=0 && 5< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))->tabval[5] = (basilys_ptr_t)(/*_.VALDATA___V195*/ curfptr[194]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (6>=0 && 6< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V199*/ curfptr[198]))->tabval[6] = (basilys_ptr_t)(/*_.VALDATA___V197*/ curfptr[196]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V199*/ curfptr[198]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V199*/ curfptr[198]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:319:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V192*/ curfptr[191]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V200*/ curfptr[199])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V200*/ curfptr[199]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V200*/ curfptr[199]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V201*/ curfptr[200]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V200*/ curfptr[199]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V202*/ curfptr[201])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V202*/ curfptr[201]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V202*/ curfptr[201]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V203*/ curfptr[202]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V202*/ curfptr[201]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V204*/ curfptr[203])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V204*/ curfptr[203]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V204*/ curfptr[203]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V205*/ curfptr[204]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V204*/ curfptr[203]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V207*/ curfptr[206]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.VALDATA___V174*/ curfptr[173]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V208*/ curfptr[207]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V208*/ curfptr[207]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V208*/ curfptr[207]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V209*/ curfptr[208])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V209*/ curfptr[208]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V209*/ curfptr[208]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V210*/ curfptr[209]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V209*/ curfptr[208])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V209*/ curfptr[208]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V209*/ curfptr[208]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V206*/ curfptr[205]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V209*/ curfptr[208]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (5>=0 && 5< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V211*/ curfptr[210]))->tabval[5] = (basilys_ptr_t)(/*_.VALDATA___V209*/ curfptr[208]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V211*/ curfptr[210]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V211*/ curfptr[210]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:326:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V206*/ curfptr[205]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V212*/ curfptr[211])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V212*/ curfptr[211]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V212*/ curfptr[211]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V213*/ curfptr[212]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V212*/ curfptr[211]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V214*/ curfptr[213])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V214*/ curfptr[213]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V214*/ curfptr[213]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V215*/ curfptr[214]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V214*/ curfptr[213]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:332:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V217*/ curfptr[216]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:332:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.VALDATA___V206*/ curfptr[205]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (5>=0 && 5< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V218*/ curfptr[217]))->tabval[5] = (basilys_ptr_t)(/*_.VALDATA___V206*/ curfptr[205]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V218*/ curfptr[217]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:332:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V218*/ curfptr[217]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (5>=0 && 5< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V219*/ curfptr[218]))->tabval[5] = (basilys_ptr_t)(/*_.VALDATA___V209*/ curfptr[208]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V219*/ curfptr[218]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:332:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V216*/ curfptr[215]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V219*/ curfptr[218]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:332:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V216*/ curfptr[215]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V220*/ curfptr[219])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V220*/ curfptr[219]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V220*/ curfptr[219]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V221*/ curfptr[220]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V220*/ curfptr[219]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V223*/ curfptr[222]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.VALDATA___V174*/ curfptr[173]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V224*/ curfptr[223]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V224*/ curfptr[223]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V224*/ curfptr[223]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V225*/ curfptr[224])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V225*/ curfptr[224]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V225*/ curfptr[224]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V226*/ curfptr[225]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V225*/ curfptr[224])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V225*/ curfptr[224]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V225*/ curfptr[224]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V222*/ curfptr[221]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V225*/ curfptr[224]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V33*/ curfptr[32]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (5>=0 && 5< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V227*/ curfptr[226]))->tabval[5] = (basilys_ptr_t)(/*_.VALDATA___V225*/ curfptr[224]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V227*/ curfptr[226]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V222*/ curfptr[221]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V227*/ curfptr[226]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:338:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V222*/ curfptr[221]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V228*/ curfptr[227])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V228*/ curfptr[227]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V228*/ curfptr[227]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V229*/ curfptr[228]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V228*/ curfptr[227]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V230*/ curfptr[229])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V230*/ curfptr[229]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V230*/ curfptr[229]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V231*/ curfptr[230]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V230*/ curfptr[229]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:353:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V232*/ curfptr[231])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V232*/ curfptr[231]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V232*/ curfptr[231]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V233*/ curfptr[232]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:353:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V232*/ curfptr[231]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V234*/ curfptr[233])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V234*/ curfptr[233]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V234*/ curfptr[233]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V235*/ curfptr[234]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V234*/ curfptr[233]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:358:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V236*/ curfptr[235])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V236*/ curfptr[235]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V236*/ curfptr[235]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V237*/ curfptr[236]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:358:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V236*/ curfptr[235]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V238*/ curfptr[237])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_189__NORMPAT_ANYRECV#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V238*/ curfptr[237])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V238*/ curfptr[237])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V238*/ curfptr[237])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V238*/ curfptr[237])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_189__NORMPAT_ANYRECV#2", NULL != (/*_.CLASS_NAMED__V12*/ curfptr[11]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V238*/ curfptr[237])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_NAMED__V12*/ curfptr[11]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V238*/ curfptr[237])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_189__NORMPAT_ANYRECV#3", NULL != (/*_.CLASS_SRC__V34*/ curfptr[33]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V238*/ curfptr[237])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_SRC__V34*/ curfptr[33]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:366:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V238*/ curfptr[237]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V239*/ curfptr[238])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V238*/ curfptr[237])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V239*/ curfptr[238])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V238*/ curfptr[237]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V239*/ curfptr[238]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V240*/ curfptr[239])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V240*/ curfptr[239]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V240*/ curfptr[239]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V241*/ curfptr[240]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V240*/ curfptr[239]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V242*/ curfptr[241])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_193__NORMPAT_ANYPAT#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V242*/ curfptr[241])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V242*/ curfptr[241])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V242*/ curfptr[241])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V242*/ curfptr[241])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_193__NORMPAT_ANYPAT#2", NULL != (/*_.CLASS_SRCPATTERN_ANY__V14*/ curfptr[13]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V242*/ curfptr[241])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_ANY__V14*/ curfptr[13]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V242*/ curfptr[241])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_193__NORMPAT_ANYPAT#3", NULL != (/*_.CLASS_NAMED__V12*/ curfptr[11]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V242*/ curfptr[241])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NAMED__V12*/ curfptr[11]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V242*/ curfptr[241])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_193__NORMPAT_ANYPAT#4", NULL != (/*_.CLASS_SRC__V34*/ curfptr[33]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V242*/ curfptr[241])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_SRC__V34*/ curfptr[33]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:382:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V242*/ curfptr[241]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V243*/ curfptr[242])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V242*/ curfptr[241])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V243*/ curfptr[242])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V242*/ curfptr[241]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V243*/ curfptr[242]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V244*/ curfptr[243])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_195__NORMPAT_INSTANCEPAT#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V244*/ curfptr[243])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V244*/ curfptr[243])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V244*/ curfptr[243])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V244*/ curfptr[243])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_195__NORMPAT_INSTANCEPAT#2", NULL != (/*_.CLASS_SRCPATTERN_INSTANCE__V35*/ curfptr[34]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V244*/ curfptr[243])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_INSTANCE__V35*/ curfptr[34]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V244*/ curfptr[243])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_195__NORMPAT_INSTANCEPAT#3", NULL != (/*_.CLASS_SRC__V34*/ curfptr[33]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V244*/ curfptr[243])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_SRC__V34*/ curfptr[33]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:400:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V244*/ curfptr[243]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V245*/ curfptr[244])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V244*/ curfptr[243])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V245*/ curfptr[244])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V244*/ curfptr[243]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V245*/ curfptr[244]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V246*/ curfptr[245])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V246*/ curfptr[245]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V246*/ curfptr[245]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V247*/ curfptr[246]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V246*/ curfptr[245]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#2", NULL != (/*_.CLASS_SRCPATTERN_ANYMATCH__V36*/ curfptr[35]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_ANYMATCH__V36*/ curfptr[35]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#3", NULL != (/*_.CLASS_SRC__V34*/ curfptr[33]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_SRC__V34*/ curfptr[33]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#4", NULL != (/*_.CLASS_SRCPATTERN_COMPOSITE__V29*/ curfptr[28]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_COMPOSITE__V29*/ curfptr[28]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#5", NULL != (/*_.CLASS_ANY_MATCHER__V30*/ curfptr[29]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[5] = (basilys_ptr_t)(/*_.CLASS_ANY_MATCHER__V30*/ curfptr[29]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#6", NULL != (/*_.NORMALIZE_TUPLE__V32*/ curfptr[31]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[6] = (basilys_ptr_t)(/*_.NORMALIZE_TUPLE__V32*/ curfptr[31]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#7", NULL != (/*_.LIST_APPEND2LIST__V23*/ curfptr[22]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[7] = (basilys_ptr_t)(/*_.LIST_APPEND2LIST__V23*/ curfptr[22]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V192*/ curfptr[191]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[8] = (basilys_ptr_t)(/*_.VALDATA___V192*/ curfptr[191]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#9", NULL != (/*_.CLASS_FORMAL_BINDING__V31*/ curfptr[30]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[9] = (basilys_ptr_t)(/*_.CLASS_FORMAL_BINDING__V31*/ curfptr[30]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#10", NULL != (/*_.CLASS_NAMED__V12*/ curfptr[11]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[10] = (basilys_ptr_t)(/*_.CLASS_NAMED__V12*/ curfptr[11]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#11", NULL != (/*_.FRESH_ENV__V37*/ curfptr[36]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[11] = (basilys_ptr_t)(/*_.FRESH_ENV__V37*/ curfptr[36]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#12", NULL != (/*_.GET_CTYPE__V20*/ curfptr[19]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[12] = (basilys_ptr_t)(/*_.GET_CTYPE__V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#13", NULL != (/*_.CLASS_ANY_BINDING__V38*/ curfptr[37]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[13] = (basilys_ptr_t)(/*_.CLASS_ANY_BINDING__V38*/ curfptr[37]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#14", NULL != (/*_.CLASS_NORMLET_BINDING__V39*/ curfptr[38]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[14] = (basilys_ptr_t)(/*_.CLASS_NORMLET_BINDING__V39*/ curfptr[38]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#15", NULL != (/*_.PUT_ENV__V40*/ curfptr[39]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[15] = (basilys_ptr_t)(/*_.PUT_ENV__V40*/ curfptr[39]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#16", NULL != (/*_.CLONE_SYMBOL__V41*/ curfptr[40]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[16] = (basilys_ptr_t)(/*_.CLONE_SYMBOL__V41*/ curfptr[40]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_199__NORMPAT_ANYMATCHPAT#17", NULL != (/*_.CLASS_NREP_LOCSYMOCC__V42*/ curfptr[41]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[17] = (basilys_ptr_t)(/*_.CLASS_NREP_LOCSYMOCC__V42*/ curfptr[41]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V232*/ curfptr[231]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V248*/ curfptr[247])->tabval[18] = (basilys_ptr_t)(/*_.VALDATA___V232*/ curfptr[231]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:415:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V248*/ curfptr[247]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V249*/ curfptr[248])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V248*/ curfptr[247])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V249*/ curfptr[248])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V248*/ curfptr[247]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V249*/ curfptr[248]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V250*/ curfptr[249])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V250*/ curfptr[249]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V250*/ curfptr[249]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V251*/ curfptr[250]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V250*/ curfptr[249]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V252*/ curfptr[251])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V252*/ curfptr[251]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V252*/ curfptr[251]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V253*/ curfptr[252]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V252*/ curfptr[251]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V254*/ curfptr[253])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V254*/ curfptr[253]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V254*/ curfptr[253]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V255*/ curfptr[254]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V254*/ curfptr[253]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V256*/ curfptr[255])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V256*/ curfptr[255]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V256*/ curfptr[255]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V257*/ curfptr[256]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V256*/ curfptr[255]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V258*/ curfptr[257])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V258*/ curfptr[257]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V258*/ curfptr[257]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V259*/ curfptr[258]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V258*/ curfptr[257]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V260*/ curfptr[259])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V260*/ curfptr[259]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V260*/ curfptr[259]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V261*/ curfptr[260]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V260*/ curfptr[259]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V262*/ curfptr[261])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V262*/ curfptr[261]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V262*/ curfptr[261]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V263*/ curfptr[262]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V262*/ curfptr[261]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#0", NULL != (/*_.CLASS_SRC_MATCH__V43*/ curfptr[42]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[0] = (basilys_ptr_t)(/*_.CLASS_SRC_MATCH__V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#1", NULL != (/*_.CLASS_ENVIRONMENT__V44*/ curfptr[43]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[1] = (basilys_ptr_t)(/*_.CLASS_ENVIRONMENT__V44*/ curfptr[43]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#2", NULL != (/*_.CLASS_NORMCONTEXT__V45*/ curfptr[44]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_NORMCONTEXT__V45*/ curfptr[44]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#3", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[3] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#4", NULL != (/*_.DISCR_MULTIPLE__V46*/ curfptr[45]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[4] = (basilys_ptr_t)(/*_.DISCR_MULTIPLE__V46*/ curfptr[45]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#5", NULL != (/*_.DISCR_LIST__V47*/ curfptr[46]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[5] = (basilys_ptr_t)(/*_.DISCR_LIST__V47*/ curfptr[46]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#6", NULL != (/*_.NORMAL_EXP__V22*/ curfptr[21]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[6] = (basilys_ptr_t)(/*_.NORMAL_EXP__V22*/ curfptr[21]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#7", NULL != (/*_.GET_CTYPE__V20*/ curfptr[19]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[7] = (basilys_ptr_t)(/*_.GET_CTYPE__V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V174*/ curfptr[173]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[8] = (basilys_ptr_t)(/*_.VALDATA___V174*/ curfptr[173]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#9", NULL != (/*_.CLASS_SRC_CASEMATCH__V48*/ curfptr[47]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[9] = (basilys_ptr_t)(/*_.CLASS_SRC_CASEMATCH__V48*/ curfptr[47]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#10", NULL != (/*_.DISCR_MAPOBJECTS__V49*/ curfptr[48]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[10] = (basilys_ptr_t)(/*_.DISCR_MAPOBJECTS__V49*/ curfptr[48]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V54*/ curfptr[53]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[11] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V86*/ curfptr[85]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[12] = (basilys_ptr_t)(/*_.VALDATA___V86*/ curfptr[85]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V232*/ curfptr[231]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[13] = (basilys_ptr_t)(/*_.VALDATA___V232*/ curfptr[231]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_215__NORMEXP_MATCH#14", NULL != (/*_.NORMALIZE_TUPLE__V32*/ curfptr[31]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V264*/ curfptr[263])->tabval[14] = (basilys_ptr_t)(/*_.NORMALIZE_TUPLE__V32*/ curfptr[31]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:566:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V264*/ curfptr[263]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V265*/ curfptr[264])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V264*/ curfptr[263])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V265*/ curfptr[264])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V264*/ curfptr[263]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V265*/ curfptr[264]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V266*/ curfptr[265])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V266*/ curfptr[265]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V266*/ curfptr[265]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V267*/ curfptr[266]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V266*/ curfptr[265]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V268*/ curfptr[267])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V268*/ curfptr[267]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V268*/ curfptr[267]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V269*/ curfptr[268]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V268*/ curfptr[267]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V270*/ curfptr[269])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V270*/ curfptr[269]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V270*/ curfptr[269]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V271*/ curfptr[270]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V270*/ curfptr[269]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V272*/ curfptr[271])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V272*/ curfptr[271]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V272*/ curfptr[271]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V273*/ curfptr[272]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V272*/ curfptr[271]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V274*/ curfptr[273])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V274*/ curfptr[273]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V274*/ curfptr[273]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V275*/ curfptr[274]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V274*/ curfptr[273]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V276*/ curfptr[275])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V276*/ curfptr[275]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V276*/ curfptr[275]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V277*/ curfptr[276]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V276*/ curfptr[275]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V278*/ curfptr[277])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V278*/ curfptr[277]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V278*/ curfptr[277]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V279*/ curfptr[278]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V278*/ curfptr[277]);
 
 /**COMMENT: before toplevel body **/;
 
 /*block*/{
  /*cond*/ if (
   /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1]) /*then*/ {
  /*_.OR___V280*/ curfptr[279] = 
    /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1];;
  } else {
   /*block*/{
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.SYSDATA_CONT_FRESH_ENV__V281*/ curfptr[280] = slot; };
    ;
    /*apply*/{
     union basilysparam_un argtab[1];
     memset(&argtab, 0, sizeof(argtab));
     argtab[0].bp_cstring =  "warmelt-normatch-3.c";
     /*_.FUN___V282*/ curfptr[281] =  basilys_apply ((basilysclosure_ptr_t)(/*_.SYSDATA_CONT_FRESH_ENV__V281*/ curfptr[280]), (basilys_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (BPARSTR_CSTRING ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.OR___V280*/ curfptr[279] = /*_.FUN___V282*/ curfptr[281];;
    /*epilog*/
    /*clear*/ /*_.SYSDATA_CONT_FRESH_ENV__V281*/ curfptr[280] = 0 ;
    /*clear*/ /*_.FUN___V282*/ curfptr[281] = 0 ;}
   ;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:3:/ block");
  /*com.block:upd.cur.mod.env.cont : at very start*/{}/*com.end block:upd.cur.mod.env.cont : at very start*/
  ;
  /*epilog*/
  /*clear*/ /*_.OR___V280*/ curfptr[279] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:64:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:64:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:64:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V89*/ curfptr[88];
   /*_.INSTALL_METHOD__V283*/ curfptr[280] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.DISCR_NULLRECV__V11*/ curfptr[10]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V283*/ curfptr[280] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:78:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:78:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:78:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V97*/ curfptr[96];
   /*_.INSTALL_METHOD__V284*/ curfptr[281] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.DISCR_ANYRECV__V13*/ curfptr[12]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V284*/ curfptr[281] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:93:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:93:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:93:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V103*/ curfptr[102];
   /*_.INSTALL_METHOD__V285*/ curfptr[279] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_ANY__V14*/ curfptr[13]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V285*/ curfptr[279] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:112:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:112:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:112:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V107*/ curfptr[106];
   /*_.INSTALL_METHOD__V286*/ curfptr[280] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OR__V15*/ curfptr[14]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V286*/ curfptr[280] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:131:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:131:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:131:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V111*/ curfptr[110];
   /*_.INSTALL_METHOD__V287*/ curfptr[281] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_AND__V16*/ curfptr[15]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V287*/ curfptr[281] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:155:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:155:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:155:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V115*/ curfptr[114];
   /*_.INSTALL_METHOD__V288*/ curfptr[279] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_VARIABLE__V17*/ curfptr[16]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V288*/ curfptr[279] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:167:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:167:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:167:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V119*/ curfptr[118];
   /*_.INSTALL_METHOD__V289*/ curfptr[280] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_JOKERVAR__V18*/ curfptr[17]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V289*/ curfptr[280] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:213:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:213:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:213:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V123*/ curfptr[122];
   /*_.INSTALL_METHOD__V290*/ curfptr[281] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_CONSTANT__V19*/ curfptr[18]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V290*/ curfptr[281] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:250:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:250:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:250:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V135*/ curfptr[134];
   /*_.INSTALL_METHOD__V291*/ curfptr[279] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OBJECT__V24*/ curfptr[23]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V291*/ curfptr[279] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:300:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:300:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:300:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V147*/ curfptr[146];
   /*_.INSTALL_METHOD__V292*/ curfptr[280] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_COMPOSITE__V29*/ curfptr[28]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V292*/ curfptr[280] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:377:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:377:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V232*/ curfptr[231];
   BASILYS_LOCATION("warmelt-normatch.bysl:377:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V239*/ curfptr[238];
   /*_.INSTALL_METHOD__V293*/ curfptr[281] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.DISCR_ANYRECV__V13*/ curfptr[12]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V293*/ curfptr[281] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:394:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:394:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V232*/ curfptr[231];
   BASILYS_LOCATION("warmelt-normatch.bysl:394:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V243*/ curfptr[242];
   /*_.INSTALL_METHOD__V294*/ curfptr[279] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_ANY__V14*/ curfptr[13]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V294*/ curfptr[279] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:410:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:410:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V232*/ curfptr[231];
   BASILYS_LOCATION("warmelt-normatch.bysl:410:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V245*/ curfptr[244];
   /*_.INSTALL_METHOD__V295*/ curfptr[280] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_INSTANCE__V35*/ curfptr[34]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V295*/ curfptr[280] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:561:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:561:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V232*/ curfptr[231];
   BASILYS_LOCATION("warmelt-normatch.bysl:561:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V249*/ curfptr[248];
   /*_.INSTALL_METHOD__V296*/ curfptr[281] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_ANYMATCH__V36*/ curfptr[35]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V296*/ curfptr[281] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:649:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:649:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NORMAL_EXP__V22*/ curfptr[21];
   BASILYS_LOCATION("warmelt-normatch.bysl:649:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V265*/ curfptr[264];
   /*_.INSTALL_METHOD__V297*/ curfptr[279] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V10*/ curfptr[9]), (basilys_ptr_t)(/*_.CLASS_SRC_MATCH__V43*/ curfptr[42]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V297*/ curfptr[279] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 4, "SYSDATA_VALUE_EXPORTER");/*_.VALUEXPORTER___V298*/ curfptr[280] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V54*/ curfptr[53];
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : CLASS_PATTERNCONTEXT*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V298*/ curfptr[280]), (basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V57*/ curfptr[56];
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_NORMCTXT*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V298*/ curfptr[280]), (basilys_ptr_t)(/*_.VALDATA___V70*/ curfptr[69]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V59*/ curfptr[58];
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_SRC*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V298*/ curfptr[280]), (basilys_ptr_t)(/*_.VALDATA___V72*/ curfptr[71]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V61*/ curfptr[60];
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_ENV*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V298*/ curfptr[280]), (basilys_ptr_t)(/*_.VALDATA___V74*/ curfptr[73]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V63*/ curfptr[62];
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_MAPATVAR*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V298*/ curfptr[280]), (basilys_ptr_t)(/*_.VALDATA___V76*/ curfptr[75]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V65*/ curfptr[64];
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_MAPATCST*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V298*/ curfptr[280]), (basilys_ptr_t)(/*_.VALDATA___V78*/ curfptr[77]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V67*/ curfptr[66];
   BASILYS_LOCATION("warmelt-normatch.bysl:654:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_BINDLIST*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V298*/ curfptr[280]), (basilys_ptr_t)(/*_.VALDATA___V80*/ curfptr[79]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.VALUEXPORTER___V298*/ curfptr[280] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:659:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 4, "SYSDATA_VALUE_EXPORTER");/*_.VALUEXPORTER___V299*/ curfptr[281] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:659:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:659:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V86*/ curfptr[85];
   BASILYS_LOCATION("warmelt-normatch.bysl:659:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : SCAN_PATTERN*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V299*/ curfptr[281]), (basilys_ptr_t)(/*_.VALDATA___V82*/ curfptr[81]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:659:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:659:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V232*/ curfptr[231];
   BASILYS_LOCATION("warmelt-normatch.bysl:659:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : NORMAL_PATTERN*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V299*/ curfptr[281]), (basilys_ptr_t)(/*_.VALDATA___V230*/ curfptr[229]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.VALUEXPORTER___V299*/ curfptr[281] = 0 ;}
 
 /**COMMENT: after toplevel body **/;
 
 /**COMMENT: compute boxloc again **/;
 
 /*cond*/ if (/*_.CONTENV___V2*/ curfptr[1] || basilys_object_length((basilys_ptr_t)BASILYSG(INITIAL_SYSTEM_DATA))<FSYSDAT_BOX_FRESH_ENV) /*then*/ {
 } else {
  /*com.block:compute fresh module environment*/{
   /**COMMENT: start computing boxloc **/;
   ;
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.FRESHENV___V3*/ curfptr[2] = slot; };
   ;
   
    if ((/*_.PREVENV___V4*/ curfptr[3]) && basilys_magic_discr((basilys_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2])) != OBMAG_CLOSURE) warning(0, "bad BASILYS fresh_env @%p in system data <%s:%d>", /*_.FRESHENV___V3*/ curfptr[2], __FILE__, __LINE__);;;
   /*apply*/{
    /*_.CONTENV___V2*/ curfptr[1] =  basilys_apply ((basilysclosure_ptr_t)(/*_.FRESHENV___V3*/ curfptr[2]), (basilys_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (""), (union basilysparam_un*)0, "", (union basilysparam_un*)0);
    }
   ;}/*com.end block:compute fresh module environment*/
  ;
  }
 
 /**COMMENT: intern symbols **/;
 
 /*internsym:CLASS_PATTERNCONTEXT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]));
 
 /*internsym:CLASS_ROOT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]));
 
 /*internsym:PCTN_NORMCTXT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V70*/ curfptr[69]));
 
 /*internsym:PCTN_SRC*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V72*/ curfptr[71]));
 
 /*internsym:PCTN_ENV*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V74*/ curfptr[73]));
 
 /*internsym:PCTN_MAPATVAR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V76*/ curfptr[75]));
 
 /*internsym:PCTN_MAPATCST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V78*/ curfptr[77]));
 
 /*internsym:PCTN_BINDLIST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V80*/ curfptr[79]));
 
 /*internsym:SCAN_PATTERN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V82*/ curfptr[81]));
 
 /*internsym:CLASS_SELECTOR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V84*/ curfptr[83]));
 
 /*internsym:DEBUG_MSG_FUN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V90*/ curfptr[89]));
 
 /*internsym:INSTALL_METHOD*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V92*/ curfptr[91]));
 
 /*internsym:DISCR_NULLRECV*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V94*/ curfptr[93]));
 
 /*internsym:CLASS_NAMED*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V98*/ curfptr[97]));
 
 /*internsym:DISCR_ANYRECV*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V100*/ curfptr[99]));
 
 /*internsym:CLASS_SRCPATTERN_ANY*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V104*/ curfptr[103]));
 
 /*internsym:CLASS_SRCPATTERN_OR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V108*/ curfptr[107]));
 
 /*internsym:CLASS_SRCPATTERN_AND*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V112*/ curfptr[111]));
 
 /*internsym:CLASS_SRCPATTERN_VARIABLE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V116*/ curfptr[115]));
 
 /*internsym:CLASS_SRCPATTERN_JOKERVAR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V120*/ curfptr[119]));
 
 /*internsym:CLASS_SRCPATTERN_CONSTANT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V124*/ curfptr[123]));
 
 /*internsym:GET_CTYPE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V126*/ curfptr[125]));
 
 /*internsym:CLASS_CTYPE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V128*/ curfptr[127]));
 
 /*internsym:NORMAL_EXP*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V130*/ curfptr[129]));
 
 /*internsym:LIST_APPEND2LIST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V132*/ curfptr[131]));
 
 /*internsym:CLASS_SRCPATTERN_OBJECT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V136*/ curfptr[135]));
 
 /*internsym:CLASS_CLASS*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V138*/ curfptr[137]));
 
 /*internsym:CLASS_SRCFIELDPATTERN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V140*/ curfptr[139]));
 
 /*internsym:CLASS_FIELD*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V142*/ curfptr[141]));
 
 /*internsym:CTYPE_VALUE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]));
 
 /*internsym:CLASS_SRCPATTERN_COMPOSITE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V148*/ curfptr[147]));
 
 /*internsym:CLASS_ANY_MATCHER*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V150*/ curfptr[149]));
 
 /*internsym:CLASS_FORMAL_BINDING*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V152*/ curfptr[151]));
 
 /*internsym:NORMALIZE_TUPLE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V154*/ curfptr[153]));
 
 /*internsym:CLASS_NREP_BACKPOINT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V156*/ curfptr[155]));
 
 /*internsym:CLASS_NREP_TYPEXPR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]));
 
 /*internsym:NBKP_DO*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V168*/ curfptr[167]));
 
 /*internsym:NBKP_FAIL*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V170*/ curfptr[169]));
 
 /*internsym:CLASS_NREP_TESTPOINT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V172*/ curfptr[171]));
 
 /*internsym:NTSP_FAILP*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V184*/ curfptr[183]));
 
 /*internsym:NTSP_SUCCP*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V186*/ curfptr[185]));
 
 /*internsym:NTSP_TEST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V188*/ curfptr[187]));
 
 /*internsym:CLASS_NREP_TESTP_MATCHER*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V190*/ curfptr[189]));
 
 /*internsym:NTSM_MATCHER*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V200*/ curfptr[199]));
 
 /*internsym:NTSM_BINDS*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V202*/ curfptr[201]));
 
 /*internsym:CLASS_NREP_TESTP_ISA*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V204*/ curfptr[203]));
 
 /*internsym:NTSI_CLASS*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V212*/ curfptr[211]));
 
 /*internsym:CLASS_NREP_TESTP_DISCRIM*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V214*/ curfptr[213]));
 
 /*internsym:CLASS_NREP_TESTP_COND*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V220*/ curfptr[219]));
 
 /*internsym:NTSC_COND*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V228*/ curfptr[227]));
 
 /*internsym:NORMAL_PATTERN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V230*/ curfptr[229]));
 
 /*internsym:ADD_WHEN_PATTERN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V234*/ curfptr[233]));
 
 /*internsym:CLASS_SRC*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V240*/ curfptr[239]));
 
 /*internsym:CLASS_SRCPATTERN_INSTANCE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V246*/ curfptr[245]));
 
 /*internsym:CLASS_SRCPATTERN_ANYMATCH*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V250*/ curfptr[249]));
 
 /*internsym:FRESH_ENV*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V252*/ curfptr[251]));
 
 /*internsym:CLASS_ANY_BINDING*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V254*/ curfptr[253]));
 
 /*internsym:CLASS_NORMLET_BINDING*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V256*/ curfptr[255]));
 
 /*internsym:PUT_ENV*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V258*/ curfptr[257]));
 
 /*internsym:CLONE_SYMBOL*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V260*/ curfptr[259]));
 
 /*internsym:CLASS_NREP_LOCSYMOCC*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V262*/ curfptr[261]));
 
 /*internsym:CLASS_SRC_MATCH*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V266*/ curfptr[265]));
 
 /*internsym:CLASS_ENVIRONMENT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V268*/ curfptr[267]));
 
 /*internsym:CLASS_NORMCONTEXT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V270*/ curfptr[269]));
 
 /*internsym:DISCR_MULTIPLE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V272*/ curfptr[271]));
 
 /*internsym:DISCR_LIST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V274*/ curfptr[273]));
 
 /*internsym:CLASS_SRC_CASEMATCH*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V276*/ curfptr[275]));
 
 /*internsym:DISCR_MAPOBJECTS*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V278*/ curfptr[277]));
 
 /**COMMENT: set retinit from boxloc **/;
 
 /*_.RETINIT___V1*/ curfptr[0] = 
  /*final freshenv get*/ basilys_container_value((basilys_ptr_t)(/*_.CONTENV___V2*/ curfptr[1]));
 /**COMMENT: end the initproc **/;
 
 ;
 goto labend_rout;
labend_rout:  basilys_topframe = (struct callframe_basilys_st *) curfram__.prev;
initial_frame_basilys = (void*)0;
 return /*_.RETINIT___V1*/ curfptr[0];
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR

} /* end start_module_basilys */

void mark_module_basilys(void*fp)
{
 int ix=0;
 initial_frame_st* framptr_= (initial_frame_st*)fp;
 for(ix=0; ix<299; ix++)
    if (framptr_->varptr[ix])
     gt_ggc_mx_basilys_un (framptr_->varptr[ix]);
   
} /* end mark_module_basilys */


/* exported field offsets */
const int fieldoff__PCTN_BINDLIST = 5;
const int fieldoff__PCTN_ENV = 2;
const int fieldoff__PCTN_MAPATCST = 4;
const int fieldoff__PCTN_MAPATVAR = 3;
const int fieldoff__PCTN_NORMCTXT = 0;
const int fieldoff__PCTN_SRC = 1;

/* exported class lengths */
const int classlen__CLASS_PATTERNCONTEXT = 6;


/**** end of warmelt-normatch-3.c ****/
