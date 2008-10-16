/* GCC BASILYS GENERATED FILE warmelt-normatch-3.c - DO NOT EDIT */
/* generated on Thu Oct 16 12:03:31 2008 */

#include "run-basilys.h"

/**** warmelt-normatch-3.c declarations ****/

/**!!** 
Copyright 2008 Free Software Foundation, Inc.
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




static basilys_ptr_t rout_1_SCANPAT_ANYRECV(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_2_SCANPAT_SRCPATOR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_3_SCANPAT_SRCPATAND(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_4_SCANPAT_SRCPATVAR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_5_SCANPAT_SRCPATJOKER(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_6_SCANPAT_SRCPATCONST(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_7_SCANPAT_SRCPATOBJECT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_8_SCANPAT_SRCPATCOMPOSITE(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);




static basilys_ptr_t rout_9_NORMEXP_MATCH(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);


void* start_module_basilys(void*);


/**** warmelt-normatch-3.c implementations ****/




static basilys_ptr_t
rout_1_SCANPAT_ANYRECV(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_1_SCANPAT_ANYRECV_st { unsigned nbvar;
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
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 14;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_ANYRECV", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:58:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:59:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:59:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:59:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:59:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_anyrecv recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:59:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:59:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:59:/ apply.arg");
    argtab[3].bp_long = 59;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:59:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:59:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:59:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:60:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:60:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:60:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:60:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:60:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:60:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(60)?(60):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:60:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:60:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:60:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:61:/ block");
 /*block*/{
  /*_.MYCLASS__V11*/ curfptr[10] = 
   (basilys_discr((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1])));;
  BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cond");
  /*cond*/ if (/*ifisa*/ basilys_is_instance_of((basilys_ptr_t)(/*_.MYCLASS__V11*/ curfptr[10]), (basilys_ptr_t)((/*!CLASS_NAMED*/ curfrout->tabval[2])))) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:62:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MYCLASS__V11*/ curfptr[10]), 1, "NAMED_NAME");/*_.MYCLASSNAME__V12*/ curfptr[11] = slot; };
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:62:/ cond.else");
  
   /*_.MYCLASSNAME__V12*/ curfptr[11] = NULL;;
   }
  ;
  
  basilys_error_str((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2]), ( "unimplemented scan_pattern for "), (basilys_ptr_t)(/*_.MYCLASSNAME__V12*/ curfptr[11]));
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:65:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:65:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:65:/ cond");
    /*cond*/ if ((/*nil*/NULL)) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:65:/ cond.then");
    /*_.IFELSE___V14*/ curfptr[13] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:65:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:65:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "catchall scan_pattern unimplemented"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(65)?(65):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V14*/ curfptr[13] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V13*/ curfptr[12] = /*_.IFELSE___V14*/ curfptr[13];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:65:/ clear");
    /*clear*/ /*_.IFELSE___V14*/ curfptr[13] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:65:/ cppif.else");
  /*_.IFCPP___V13*/ curfptr[12] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V10*/ curfptr[8] = /*_.IFCPP___V13*/ curfptr[12];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:61:/ clear");
  /*clear*/ /*_.MYCLASS__V11*/ curfptr[10] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:61:/ clear");
  /*clear*/ /*_.MYCLASSNAME__V12*/ curfptr[11] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:61:/ clear");
  /*clear*/ /*_.IFCPP___V13*/ curfptr[12] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V10*/ curfptr[8];;
  BASILYS_LOCATION("warmelt-normatch.bysl:58:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ clear");
 /*clear*/ /*_.LET___V10*/ curfptr[8] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_ANYRECV", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_1_SCANPAT_ANYRECV*/





static basilys_ptr_t
rout_2_SCANPAT_SRCPATOR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_2_SCANPAT_SRCPATOR_st { unsigned nbvar;
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
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 18;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATOR", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:72:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:73:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:73:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:73:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:73:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpator recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:73:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:73:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:73:/ apply.arg");
    argtab[3].bp_long = 73;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:73:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:73:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:73:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:74:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:74:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:74:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:74:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:74:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:74:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(74)?(74):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:74:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:74:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:74:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:75:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:75:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_OR*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:75:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:75:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:75:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:75:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(75)?(75):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:75:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:75:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:75:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:76:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:76:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:77:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "ORPAT_DISJ");/*_.SDISJ__V14*/ curfptr[13] = slot; };
  ;
  /*citerblock FOREACH_IN_MULTIPLE*/ {
  long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SDISJ__V14*/ curfptr[13]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.DIS__V15*/ curfptr[14] = basilys_multiple_nth((basilys_ptr_t)(/*_.SDISJ__V14*/ curfptr[13]), cit1__EACHTUP_ix);
/*_#IX__L4*/ curfnum[0] = cit1__EACHTUP_ix;

  
   BASILYS_LOCATION("warmelt-normatch.bysl:82:/ msend");
   /*msend*/{
    union basilysparam_un argtab[3];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:82:/ ojbmsend.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
    BASILYS_LOCATION("warmelt-normatch.bysl:82:/ ojbmsend.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V4*/ curfptr[3];
    BASILYS_LOCATION("warmelt-normatch.bysl:82:/ ojbmsend.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
    basilysgc_send((basilys_ptr_t)(/*_.DIS__V15*/ curfptr[14]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[3])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
  } /* end cit1__EACHTUP*/
  
   /*citerepilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:79:/ clear");
   /*clear*/ /*_.DIS__V15*/ curfptr[14] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:79:/ clear");
   /*clear*/ /*_#IX__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:79:/ clear");
   /*clear*/ /*_.SCAN_PATTERN__V16*/ curfptr[15] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:84:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:84:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
     argtab[0].bp_cstring =  "scanpat_srcpator end recv";
     BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[4];
     BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:84:/ apply.arg");
     argtab[3].bp_long = 84;
     /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V17*/ curfptr[16] = /*_.DEBUG_MSG_FUN__V18*/ curfptr[17];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:84:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:84:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:84:/ cppif.else");
  /*_.IFCPP___V17*/ curfptr[16] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFCPP___V17*/ curfptr[16];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:76:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:76:/ clear");
  /*clear*/ /*_.SDISJ__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:76:/ clear");
  /*clear*/ /*_.IFCPP___V17*/ curfptr[16] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:72:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATOR", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_2_SCANPAT_SRCPATOR*/





static basilys_ptr_t
rout_3_SCANPAT_SRCPATAND(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_3_SCANPAT_SRCPATAND_st { unsigned nbvar;
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
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 18;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATAND", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:91:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:92:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:92:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:92:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:92:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatand recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:92:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:92:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:92:/ apply.arg");
    argtab[3].bp_long = 92;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:92:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:92:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:92:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:93:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:93:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:93:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:93:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:93:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:93:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(93)?(93):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:93:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:93:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:93:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:94:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:94:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_AND*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:94:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:94:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:94:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:94:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(94)?(94):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:94:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:94:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:94:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:95:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:95:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:96:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "ANDPAT_CONJ");/*_.SCONJ__V14*/ curfptr[13] = slot; };
  ;
  /*citerblock FOREACH_IN_MULTIPLE*/ {
  long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SCONJ__V14*/ curfptr[13]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.DIS__V15*/ curfptr[14] = basilys_multiple_nth((basilys_ptr_t)(/*_.SCONJ__V14*/ curfptr[13]), cit1__EACHTUP_ix);
/*_#IX__L4*/ curfnum[0] = cit1__EACHTUP_ix;

  
   BASILYS_LOCATION("warmelt-normatch.bysl:101:/ msend");
   /*msend*/{
    union basilysparam_un argtab[3];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:101:/ ojbmsend.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
    BASILYS_LOCATION("warmelt-normatch.bysl:101:/ ojbmsend.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V4*/ curfptr[3];
    BASILYS_LOCATION("warmelt-normatch.bysl:101:/ ojbmsend.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
    basilysgc_send((basilys_ptr_t)(/*_.DIS__V15*/ curfptr[14]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[3])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
  } /* end cit1__EACHTUP*/
  
   /*citerepilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:98:/ clear");
   /*clear*/ /*_.DIS__V15*/ curfptr[14] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:98:/ clear");
   /*clear*/ /*_#IX__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:98:/ clear");
   /*clear*/ /*_.SCAN_PATTERN__V16*/ curfptr[15] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:103:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:103:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:103:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:103:/ apply.arg");
     argtab[0].bp_cstring =  "scanpat_srcpatand end recv";
     BASILYS_LOCATION("warmelt-normatch.bysl:103:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[4];
     BASILYS_LOCATION("warmelt-normatch.bysl:103:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:103:/ apply.arg");
     argtab[3].bp_long = 103;
     /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V17*/ curfptr[16] = /*_.DEBUG_MSG_FUN__V18*/ curfptr[17];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:103:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:103:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V18*/ curfptr[17] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:103:/ cppif.else");
  /*_.IFCPP___V17*/ curfptr[16] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFCPP___V17*/ curfptr[16];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:95:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:95:/ clear");
  /*clear*/ /*_.SCONJ__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:95:/ clear");
  /*clear*/ /*_.IFCPP___V17*/ curfptr[16] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:91:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATAND", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_3_SCANPAT_SRCPATAND*/





static basilys_ptr_t
rout_4_SCANPAT_SRCPATVAR(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_4_SCANPAT_SRCPATVAR_st { unsigned nbvar;
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
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 25;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATVAR", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:110:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:111:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:111:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:111:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:111:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatvar recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:111:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:111:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:111:/ apply.arg");
    argtab[3].bp_long = 111;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:111:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:111:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:111:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:112:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:112:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:112:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:112:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:112:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:112:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(112)?(112):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:112:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:112:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:112:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:113:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:113:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_VARIABLE*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:113:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:113:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:113:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:113:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(113)?(113):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:113:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:113:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:113:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:114:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:114:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:115:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SPAT_VAR");/*_.SVAR__V14*/ curfptr[13] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:116:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 3, "PCTN_MAPATVAR");/*_.MAPVAR__V15*/ curfptr[14] = slot; };
  ;
  /*_.MVAR__V16*/ curfptr[15] = 
   (basilys_get_mapobjects((basilysmapobjects_ptr_t)(/*_.MAPVAR__V15*/ curfptr[14]), (basilysobject_ptr_t)(/*_.SVAR__V14*/ curfptr[13])));;
  /*_#NULL__L4*/ curfnum[0] = 
   ((/*_.MVAR__V16*/ curfptr[15]) == NULL);;
  BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cond");
  /*cond*/ if (/*_#NULL__L4*/ curfnum[0]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:119:/ block");
   /*block*/{
    
     basilysgc_put_mapobjects( (basilysmapobjects_ptr_t) (/*_.MAPVAR__V15*/ curfptr[14]), (basilysobject_ptr_t) (/*_.SVAR__V14*/ curfptr[13]), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]));
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:121:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:121:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:121:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:121:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatvar return new svar";
       BASILYS_LOCATION("warmelt-normatch.bysl:121:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[4];
       BASILYS_LOCATION("warmelt-normatch.bysl:121:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:121:/ apply.arg");
       argtab[3].bp_long = 121;
       /*_.DEBUG_MSG_FUN__V19*/ curfptr[18] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.SVAR__V14*/ curfptr[13]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V18*/ curfptr[17] = /*_.DEBUG_MSG_FUN__V19*/ curfptr[18];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:121:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[4] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:121:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V19*/ curfptr[18] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:121:/ cppif.else");
    /*_.IFCPP___V18*/ curfptr[17] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:122:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:122:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ block");
    /*block*/{
     /*_.PROGN___V21*/ curfptr[20] = /*_.RETURN___V20*/ curfptr[18];;}
    ;
    /*_.IFELSE___V17*/ curfptr[16] = /*_.PROGN___V21*/ curfptr[20];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
    /*clear*/ /*_.IFCPP___V18*/ curfptr[17] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
    /*clear*/ /*_.RETURN___V20*/ curfptr[18] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
    /*clear*/ /*_.PROGN___V21*/ curfptr[20] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:119:/ cond.else");
  
   BASILYS_LOCATION("warmelt-normatch.bysl:119:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:125:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:125:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L6*/ curfnum[4] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:125:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:125:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatvar return found mvar";
       BASILYS_LOCATION("warmelt-normatch.bysl:125:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L6*/ curfnum[4];
       BASILYS_LOCATION("warmelt-normatch.bysl:125:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:125:/ apply.arg");
       argtab[3].bp_long = 125;
       /*_.DEBUG_MSG_FUN__V23*/ curfptr[18] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.MVAR__V16*/ curfptr[15]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V22*/ curfptr[17] = /*_.DEBUG_MSG_FUN__V23*/ curfptr[18];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:125:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L6*/ curfnum[4] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:125:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V23*/ curfptr[18] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:125:/ cppif.else");
    /*_.IFCPP___V22*/ curfptr[17] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:126:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:126:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:124:/ block");
    /*block*/{
     /*_.PROGN___V25*/ curfptr[18] = /*_.RETURN___V24*/ curfptr[20];;}
    ;
    /*_.IFELSE___V17*/ curfptr[16] = /*_.PROGN___V25*/ curfptr[18];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
    /*clear*/ /*_.IFCPP___V22*/ curfptr[17] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
    /*clear*/ /*_.RETURN___V24*/ curfptr[20] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:119:/ clear");
    /*clear*/ /*_.PROGN___V25*/ curfptr[18] = 0 ;}
   ;
   }
  ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFELSE___V17*/ curfptr[16];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:114:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:114:/ clear");
  /*clear*/ /*_.SVAR__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:114:/ clear");
  /*clear*/ /*_.MAPVAR__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:114:/ clear");
  /*clear*/ /*_.MVAR__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:114:/ clear");
  /*clear*/ /*_#NULL__L4*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:114:/ clear");
  /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:110:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATVAR", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_4_SCANPAT_SRCPATVAR*/





static basilys_ptr_t
rout_5_SCANPAT_SRCPATJOKER(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_5_SCANPAT_SRCPATJOKER_st { unsigned nbvar;
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
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 12;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATJOKER", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:134:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:135:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:135:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:135:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:135:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatjoker recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:135:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:135:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:135:/ apply.arg");
    argtab[3].bp_long = 135;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:135:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:135:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:135:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:136:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:136:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:136:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:136:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:136:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:136:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(136)?(136):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:136:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:136:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:136:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:137:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:137:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_JOKERVAR*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:137:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:137:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:137:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:137:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(137)?(137):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:137:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:137:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:137:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:139:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = NULL;;
  BASILYS_LOCATION("warmelt-normatch.bysl:139:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.RETURN___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:134:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ clear");
 /*clear*/ /*_.RETURN___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATJOKER", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_5_SCANPAT_SRCPATJOKER*/





static basilys_ptr_t
rout_6_SCANPAT_SRCPATCONST(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_6_SCANPAT_SRCPATCONST_st { unsigned nbvar;
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
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 46;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATCONST", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:147:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:148:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:148:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:148:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:148:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatconst recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:148:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:148:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:148:/ apply.arg");
    argtab[3].bp_long = 148;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:148:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:148:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:148:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:149:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:149:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:149:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:149:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:149:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:149:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(149)?(149):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:149:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:149:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:149:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:150:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:150:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_CONSTANT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:150:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:150:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:150:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:150:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(150)?(150):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:150:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:150:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:150:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:151:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SPAT_CONSTX");/*_.SCONST__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:152:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V14*/ curfptr[13] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:153:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 0, "PCTN_NORMCTXT");/*_.NCX__V15*/ curfptr[14] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:154:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 4, "PCTN_MAPATCST");/*_.MAPCST__V16*/ curfptr[15] = slot; };
  ;
  /*_#IS_OBJECT__L4*/ curfnum[0] = 
    (basilys_magic_discr((basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12])) == OBMAG_OBJECT);;
  BASILYS_LOCATION("warmelt-normatch.bysl:155:/ cond");
  /*cond*/ if (/*_#IS_OBJECT__L4*/ curfnum[0]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:155:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:155:/ block");
   /*block*/{
    /*_.MAPOBJECT_GET__V18*/ curfptr[17] = 
     (basilys_get_mapobjects((basilysmapobjects_ptr_t)(/*_.MAPCST__V16*/ curfptr[15]), (basilysobject_ptr_t)(/*_.SCONST__V13*/ curfptr[12])));;
    /*_.MCST__V17*/ curfptr[16] = /*_.MAPOBJECT_GET__V18*/ curfptr[17];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:155:/ clear");
    /*clear*/ /*_.MAPOBJECT_GET__V18*/ curfptr[17] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:155:/ cond.else");
  
   /*_.MCST__V17*/ curfptr[16] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:156:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 5, "PCTN_BINDLIST");/*_.BINDLIST__V19*/ curfptr[17] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:157:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 2, "PCTN_ENV");/*_.ENV__V20*/ curfptr[19] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:158:/ msend");
  /*msend*/{
   union basilysparam_un argtab[1];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:158:/ ojbmsend.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V20*/ curfptr[19];
   /*_.RECTYP__V21*/ curfptr[20] = basilysgc_send((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!GET_CTYPE*/ curfrout->tabval[3])), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:160:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:160:/ block");
   /*block*/{
    /*_#IS_A__L5*/ curfnum[4] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.RECTYP__V21*/ curfptr[20]), (basilys_ptr_t)((/*!CLASS_CTYPE*/ curfrout->tabval[4])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:160:/ cond");
    /*cond*/ if (/*_#IS_A__L5*/ curfnum[4]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:160:/ cond.then");
    /*_.IFELSE___V23*/ curfptr[22] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:160:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:160:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check rectyp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(160)?(160):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V23*/ curfptr[22] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V22*/ curfptr[21] = /*_.IFELSE___V23*/ curfptr[22];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:160:/ clear");
    /*clear*/ /*_#IS_A__L5*/ curfnum[4] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:160:/ clear");
    /*clear*/ /*_.IFELSE___V23*/ curfptr[22] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:160:/ cppif.else");
  /*_.IFCPP___V22*/ curfptr[21] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_#__L6*/ curfnum[4] = 
   ((/*_.RECTYP__V21*/ curfptr[20]) != (/*_.CTYP__V4*/ curfptr[3]));;
  BASILYS_LOCATION("warmelt-normatch.bysl:161:/ cond");
  /*cond*/ if (/*_#__L6*/ curfnum[4]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:161:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:161:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:163:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECTYP__V21*/ curfptr[20]), 1, "NAMED_NAME");/*_.NAMED_NAME__V25*/ curfptr[24] = slot; };
    ;
    
    basilys_error_str((basilys_ptr_t)(/*_.SLOC__V14*/ curfptr[13]), ( "invalid ctype in constant pattern - expecting"), (basilys_ptr_t)(/*_.NAMED_NAME__V25*/ curfptr[24]));
    BASILYS_LOCATION("warmelt-normatch.bysl:165:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3]), 1, "NAMED_NAME");/*_.NAMED_NAME__V26*/ curfptr[25] = slot; };
    ;
    
    basilys_warning_str(0, (basilys_ptr_t)(/*_.SLOC__V14*/ curfptr[13]), ( "got ctype"), (basilys_ptr_t)(/*_.NAMED_NAME__V26*/ curfptr[25]));
    BASILYS_LOCATION("warmelt-normatch.bysl:161:/ block");
    /*block*/{}
    ;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:161:/ clear");
    /*clear*/ /*_.NAMED_NAME__V25*/ curfptr[24] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:161:/ clear");
    /*clear*/ /*_.NAMED_NAME__V26*/ curfptr[25] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:161:/ cond.else");
  
   /*_.IFELSE___V24*/ curfptr[22] = NULL;;
   }
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:167:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:167:/ block");
   /*block*/{
    /*_#IS_LIST__L7*/ curfnum[6] = 
     (basilys_magic_discr((basilys_ptr_t)(/*_.BINDLIST__V19*/ curfptr[17])) == OBMAG_LIST);;
    BASILYS_LOCATION("warmelt-normatch.bysl:167:/ cond");
    /*cond*/ if (/*_#IS_LIST__L7*/ curfnum[6]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:167:/ cond.then");
    /*_.IFELSE___V28*/ curfptr[25] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:167:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:167:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check bindlist"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(167)?(167):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V28*/ curfptr[25] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V27*/ curfptr[24] = /*_.IFELSE___V28*/ curfptr[25];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:167:/ clear");
    /*clear*/ /*_#IS_LIST__L7*/ curfnum[6] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:167:/ clear");
    /*clear*/ /*_.IFELSE___V28*/ curfptr[25] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:167:/ cppif.else");
  /*_.IFCPP___V27*/ curfptr[24] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_#IS_OBJECT__L8*/ curfnum[6] = 
    (basilys_magic_discr((basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12])) == OBMAG_OBJECT);;
  /*_#NOT__L9*/ curfnum[8] = 
   (!(/*_#IS_OBJECT__L8*/ curfnum[6]));;
  BASILYS_LOCATION("warmelt-normatch.bysl:169:/ cond");
  /*cond*/ if (/*_#NOT__L9*/ curfnum[8]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:169:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:169:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:170:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:170:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:170:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:170:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatconst nonobj literal const";
       BASILYS_LOCATION("warmelt-normatch.bysl:170:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L10*/ curfnum[9];
       BASILYS_LOCATION("warmelt-normatch.bysl:170:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:170:/ apply.arg");
       argtab[3].bp_long = 170;
       /*_.DEBUG_MSG_FUN__V31*/ curfptr[30] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V30*/ curfptr[29] = /*_.DEBUG_MSG_FUN__V31*/ curfptr[30];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:170:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:170:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V31*/ curfptr[30] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:170:/ cppif.else");
    /*_.IFCPP___V30*/ curfptr[29] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:171:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:171:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:169:/ block");
    /*block*/{
     /*_.PROGN___V33*/ curfptr[32] = /*_.RETURN___V32*/ curfptr[30];;}
    ;
    /*_.IFELSE___V29*/ curfptr[25] = /*_.PROGN___V33*/ curfptr[32];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:169:/ clear");
    /*clear*/ /*_.IFCPP___V30*/ curfptr[29] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:169:/ clear");
    /*clear*/ /*_.RETURN___V32*/ curfptr[30] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:169:/ clear");
    /*clear*/ /*_.PROGN___V33*/ curfptr[32] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:169:/ cond.else");
  
   BASILYS_LOCATION("warmelt-normatch.bysl:169:/ block");
   /*block*/{
    /*_#NULL__L11*/ curfnum[9] = 
     ((/*_.MCST__V17*/ curfptr[16]) == NULL);;
    BASILYS_LOCATION("warmelt-normatch.bysl:173:/ cond");
    /*cond*/ if (/*_#NULL__L11*/ curfnum[9]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:173:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:173:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:174:/ block");
      /*block*/{
       BASILYS_LOCATION("warmelt-normatch.bysl:174:/ multimsend");
       /*multimsend*/{
        union basilysparam_un argtab[3];
        union basilysparam_un restab[1];
        memset(&argtab, 0, sizeof(argtab));
        memset(&restab, 0, sizeof(restab));
        BASILYS_LOCATION("warmelt-normatch.bysl:174:/ multimsend.arg");
        argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V20*/ curfptr[19];BASILYS_LOCATION("warmelt-normatch.bysl:174:/ multimsend.arg");
        argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V15*/ curfptr[14];BASILYS_LOCATION("warmelt-normatch.bysl:174:/ multimsend.arg");
        argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V14*/ curfptr[13];
        BASILYS_LOCATION("warmelt-normatch.bysl:174:/ multimsend.xres");
        restab[0].bp_aptr = (basilys_ptr_t*) &/*_.NBINDCONST__V37*/ curfptr[36];BASILYS_LOCATION("warmelt-normatch.bysl:174:/ multimsend.send");
        /*_.NCONST__V36*/ curfptr[32] =  basilysgc_send ((basilys_ptr_t)(/*_.SCONST__V13*/ curfptr[12]), ((basilys_ptr_t)((/*!NORMAL_EXP*/ curfrout->tabval[5]))), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
        }
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:174:/ block");
       /*block*/{
        
        #if ENABLE_CHECKING
         BASILYS_LOCATION("warmelt-normatch.bysl:177:/ cppif.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:177:/ block");
         /*block*/{
          /*_#THE_CALLCOUNT__L12*/ curfnum[11] = 
           callcount;;
          BASILYS_LOCATION("warmelt-normatch.bysl:177:/ apply");
          /*apply*/{
           union basilysparam_un argtab[4];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:177:/ apply.arg");
           argtab[0].bp_cstring =  "scanpat_srcpatconst nconst";
           BASILYS_LOCATION("warmelt-normatch.bysl:177:/ apply.arg");
           argtab[1].bp_long = /*_#THE_CALLCOUNT__L12*/ curfnum[11];
           BASILYS_LOCATION("warmelt-normatch.bysl:177:/ apply.arg");
           argtab[2].bp_cstring =  "warmelt-normatch.bysl";
           BASILYS_LOCATION("warmelt-normatch.bysl:177:/ apply.arg");
           argtab[3].bp_long = 177;
           /*_.DEBUG_MSG_FUN__V39*/ curfptr[38] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.NCONST__V36*/ curfptr[32]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IFCPP___V38*/ curfptr[37] = /*_.DEBUG_MSG_FUN__V39*/ curfptr[38];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
          /*clear*/ /*_#THE_CALLCOUNT__L12*/ curfnum[11] = 0 ;
          BASILYS_LOCATION("warmelt-normatch.bysl:177:/ clear");
          /*clear*/ /*_.DEBUG_MSG_FUN__V39*/ curfptr[38] = 0 ;}
         
         #else /*ENABLE_CHECKING*/
         BASILYS_LOCATION("warmelt-normatch.bysl:177:/ cppif.else");
        /*_.IFCPP___V38*/ curfptr[37] = (/*nil*/NULL);
         #endif /*ENABLE_CHECKING*/
         ;
        
         basilysgc_put_mapobjects( (basilysmapobjects_ptr_t) (/*_.MAPCST__V16*/ curfptr[15]), (basilysobject_ptr_t) (/*_.SCONST__V13*/ curfptr[12]), (basilys_ptr_t)(/*_.NCONST__V36*/ curfptr[32]));
        BASILYS_LOCATION("warmelt-normatch.bysl:179:/ cond");
        /*cond*/ if (/*_.NBINDCONST__V37*/ curfptr[36]) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:179:/ cond.then");
        BASILYS_LOCATION("warmelt-normatch.bysl:179:/ block");
         /*block*/{
          BASILYS_LOCATION("warmelt-normatch.bysl:179:/ apply");
          /*apply*/{
           union basilysparam_un argtab[1];
           memset(&argtab, 0, sizeof(argtab));
           BASILYS_LOCATION("warmelt-normatch.bysl:179:/ apply.arg");
           argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NBINDCONST__V37*/ curfptr[36];
           /*_.LIST_APPEND2LIST__V41*/ curfptr[40] =  basilys_apply ((basilysclosure_ptr_t)((/*!LIST_APPEND2LIST*/ curfrout->tabval[6])), (basilys_ptr_t)(/*_.BINDLIST__V19*/ curfptr[17]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
           }
          ;
          /*_.IF___V40*/ curfptr[38] = /*_.LIST_APPEND2LIST__V41*/ curfptr[40];;
          /*epilog*/
          BASILYS_LOCATION("warmelt-normatch.bysl:179:/ clear");
          /*clear*/ /*_.LIST_APPEND2LIST__V41*/ curfptr[40] = 0 ;}
         ;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:179:/ cond.else");
        
         /*_.IF___V40*/ curfptr[38] = NULL;;
         }
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:180:/ block");
        /*block*/{
         /*_.RETVAL___V1*/ curfptr[0] = NULL;;
         BASILYS_LOCATION("warmelt-normatch.bysl:180:/ finalreturn");
         ;
         /*finalret*/ goto labend_rout ;}
        ;
        /*_.MULTI___V35*/ curfptr[30] = /*_.RETURN___V42*/ curfptr[40];;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:174:/ clear");
        /*clear*/ /*_.IFCPP___V38*/ curfptr[37] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:174:/ clear");
        /*clear*/ /*_.IF___V40*/ curfptr[38] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:174:/ clear");
        /*clear*/ /*_.RETURN___V42*/ curfptr[40] = 0 ;}
       ;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:174:/ clear");
       /*clear*/ /*_.NBINDCONST__V37*/ curfptr[36] = 0 ;}
      ;
      /*_.IFELSE___V34*/ curfptr[29] = /*_.MULTI___V35*/ curfptr[30];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
      /*clear*/ /*_.MULTI___V35*/ curfptr[30] = 0 ;}
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:173:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:173:/ block");
     /*block*/{
      
      #if ENABLE_CHECKING
       BASILYS_LOCATION("warmelt-normatch.bysl:183:/ cppif.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:183:/ block");
       /*block*/{
        /*_#THE_CALLCOUNT__L13*/ curfnum[11] = 
         callcount;;
        BASILYS_LOCATION("warmelt-normatch.bysl:183:/ apply");
        /*apply*/{
         union basilysparam_un argtab[4];
         memset(&argtab, 0, sizeof(argtab));
         BASILYS_LOCATION("warmelt-normatch.bysl:183:/ apply.arg");
         argtab[0].bp_cstring =  "scanpat_srcpatconst got mcst";
         BASILYS_LOCATION("warmelt-normatch.bysl:183:/ apply.arg");
         argtab[1].bp_long = /*_#THE_CALLCOUNT__L13*/ curfnum[11];
         BASILYS_LOCATION("warmelt-normatch.bysl:183:/ apply.arg");
         argtab[2].bp_cstring =  "warmelt-normatch.bysl";
         BASILYS_LOCATION("warmelt-normatch.bysl:183:/ apply.arg");
         argtab[3].bp_long = 183;
         /*_.DEBUG_MSG_FUN__V44*/ curfptr[38] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.MCST__V17*/ curfptr[16]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
         }
        ;
        /*_.IFCPP___V43*/ curfptr[37] = /*_.DEBUG_MSG_FUN__V44*/ curfptr[38];;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:183:/ clear");
        /*clear*/ /*_#THE_CALLCOUNT__L13*/ curfnum[11] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:183:/ clear");
        /*clear*/ /*_.DEBUG_MSG_FUN__V44*/ curfptr[38] = 0 ;}
       
       #else /*ENABLE_CHECKING*/
       BASILYS_LOCATION("warmelt-normatch.bysl:183:/ cppif.else");
      /*_.IFCPP___V43*/ curfptr[37] = (/*nil*/NULL);
       #endif /*ENABLE_CHECKING*/
       ;
      BASILYS_LOCATION("warmelt-normatch.bysl:184:/ block");
      /*block*/{
       /*_.RETVAL___V1*/ curfptr[0] = NULL;;
       BASILYS_LOCATION("warmelt-normatch.bysl:184:/ finalreturn");
       ;
       /*finalret*/ goto labend_rout ;}
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:182:/ block");
      /*block*/{
       /*_.PROGN___V46*/ curfptr[32] = /*_.RETURN___V45*/ curfptr[40];;}
      ;
      /*_.IFELSE___V34*/ curfptr[29] = /*_.PROGN___V46*/ curfptr[32];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
      /*clear*/ /*_.IFCPP___V43*/ curfptr[37] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
      /*clear*/ /*_.RETURN___V45*/ curfptr[40] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:173:/ clear");
      /*clear*/ /*_.PROGN___V46*/ curfptr[32] = 0 ;}
     ;
     }
    ;
    /*_.IFELSE___V29*/ curfptr[25] = /*_.IFELSE___V34*/ curfptr[29];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:169:/ clear");
    /*clear*/ /*_#NULL__L11*/ curfnum[9] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:169:/ clear");
    /*clear*/ /*_.IFELSE___V34*/ curfptr[29] = 0 ;}
   ;
   }
  ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFELSE___V29*/ curfptr[25];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.SCONST__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.SLOC__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.NCX__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.MAPCST__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_#IS_OBJECT__L4*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.MCST__V17*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.BINDLIST__V19*/ curfptr[17] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.ENV__V20*/ curfptr[19] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.RECTYP__V21*/ curfptr[20] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.IFCPP___V22*/ curfptr[21] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_#__L6*/ curfnum[4] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.IFELSE___V24*/ curfptr[22] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.IFCPP___V27*/ curfptr[24] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_#IS_OBJECT__L8*/ curfnum[6] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_#NOT__L9*/ curfnum[8] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:151:/ clear");
  /*clear*/ /*_.IFELSE___V29*/ curfptr[25] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:147:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATCONST", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_6_SCANPAT_SRCPATCONST*/





static basilys_ptr_t
rout_7_SCANPAT_SRCPATOBJECT(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_7_SCANPAT_SRCPATOBJECT_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 36
  void* varptr[36];
#define CURFRAM_NBVARNUM 11
  long varnum[11];
/*others*/
  long _spare_; }
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 36;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATOBJECT", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:192:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:193:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:193:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:193:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatobject recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L1*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:193:/ apply.arg");
    argtab[3].bp_long = 193;
    /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.DEBUG_MSG_FUN__V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:193:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:193:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:193:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:194:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:194:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:194:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:194:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:194:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:194:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(194)?(194):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:194:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:194:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:194:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:195:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_OBJECT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:195:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(195)?(195):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:195:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:195:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:195:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:196:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:196:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:197:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SPAT_CLASS");/*_.SCLA__V14*/ curfptr[13] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:198:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 2, "SPAT_FIELDS");/*_.SFIELDS__V15*/ curfptr[14] = slot; };
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:200:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:200:/ block");
   /*block*/{
    /*_#IS_A__L4*/ curfnum[0] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.SCLA__V14*/ curfptr[13]), (basilys_ptr_t)((/*!CLASS_CLASS*/ curfrout->tabval[3])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:200:/ cond");
    /*cond*/ if (/*_#IS_A__L4*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:200:/ cond.then");
    /*_.IFELSE___V17*/ curfptr[16] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:200:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:200:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check scla"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(200)?(200):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V16*/ curfptr[15] = /*_.IFELSE___V17*/ curfptr[16];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:200:/ clear");
    /*clear*/ /*_#IS_A__L4*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:200:/ clear");
    /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:200:/ cppif.else");
  /*_.IFCPP___V16*/ curfptr[15] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:201:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:201:/ block");
   /*block*/{
    /*_#IS_MULTIPLE__L5*/ curfnum[0] = 
     (basilys_magic_discr((basilys_ptr_t)(/*_.SFIELDS__V15*/ curfptr[14])) == OBMAG_MULTIPLE);;
    BASILYS_LOCATION("warmelt-normatch.bysl:201:/ cond");
    /*cond*/ if (/*_#IS_MULTIPLE__L5*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:201:/ cond.then");
    /*_.IFELSE___V19*/ curfptr[18] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:201:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:201:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check sfields"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(201)?(201):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V18*/ curfptr[16] = /*_.IFELSE___V19*/ curfptr[18];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:201:/ clear");
    /*clear*/ /*_#IS_MULTIPLE__L5*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:201:/ clear");
    /*clear*/ /*_.IFELSE___V19*/ curfptr[18] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:201:/ cppif.else");
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
    BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:205:/ block");
    /*block*/{
     /*_#IS_A__L7*/ curfnum[6] = 
       basilys_is_instance_of((basilys_ptr_t)(/*_.FLDPA__V20*/ curfptr[18]), (basilys_ptr_t)((/*!CLASS_SRCFIELDPATTERN*/ curfrout->tabval[4])));;
     BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cond");
     /*cond*/ if (/*_#IS_A__L7*/ curfnum[6]) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cond.then");
     /*_.IFELSE___V22*/ curfptr[21] = (/*nil*/NULL);;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cond.else");
     
      BASILYS_LOCATION("warmelt-normatch.bysl:205:/ block");
      /*block*/{
       /*block*/{
        
        basilys_assert_failed(( "check fldp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(205)?(205):__LINE__, __FUNCTION__);
        /*clear*/ /*_.IFELSE___V22*/ curfptr[21] = 0 ;}
       ;
       /*epilog*/}
      ;
      }
     ;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:205:/ clear");
     /*clear*/ /*_#IS_A__L7*/ curfnum[6] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:205:/ clear");
     /*clear*/ /*_.IFELSE___V22*/ curfptr[21] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:205:/ cppif.else");
   (/*nil*/NULL)
    #endif /*ENABLE_CHECKING*/
    ;
   BASILYS_LOCATION("warmelt-normatch.bysl:206:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLDPA__V20*/ curfptr[18]), 1, "SPAF_FIELD");/*_.FLD__V24*/ curfptr[23] = slot; };
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:207:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLDPA__V20*/ curfptr[18]), 2, "SPAF_PATTERN");/*_.FPAT__V25*/ curfptr[24] = slot; };
    ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:209:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:209:/ block");
     /*block*/{
      /*_#IS_A__L8*/ curfnum[6] = 
        basilys_is_instance_of((basilys_ptr_t)(/*_.FLD__V24*/ curfptr[23]), (basilys_ptr_t)((/*!CLASS_FIELD*/ curfrout->tabval[5])));;
      BASILYS_LOCATION("warmelt-normatch.bysl:209:/ cond");
      /*cond*/ if (/*_#IS_A__L8*/ curfnum[6]) /*then*/ {
      BASILYS_LOCATION("warmelt-normatch.bysl:209:/ cond.then");
      /*_.IFELSE___V27*/ curfptr[26] = (/*nil*/NULL);;
      } else {BASILYS_LOCATION("warmelt-normatch.bysl:209:/ cond.else");
      
       BASILYS_LOCATION("warmelt-normatch.bysl:209:/ block");
       /*block*/{
        /*block*/{
         
         basilys_assert_failed(( "check fld"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(209)?(209):__LINE__, __FUNCTION__);
         /*clear*/ /*_.IFELSE___V27*/ curfptr[26] = 0 ;}
        ;
        /*epilog*/}
       ;
       }
      ;
      /*_.IFCPP___V26*/ curfptr[25] = /*_.IFELSE___V27*/ curfptr[26];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:209:/ clear");
      /*clear*/ /*_#IS_A__L8*/ curfnum[6] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:209:/ clear");
      /*clear*/ /*_.IFELSE___V27*/ curfptr[26] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:209:/ cppif.else");
    /*_.IFCPP___V26*/ curfptr[25] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:210:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLD__V24*/ curfptr[23]), 2, "FLD_OWNCLASS");/*_.FLD_OWNCLASS__V28*/ curfptr[26] = slot; };
    ;
    /*_#SUBCLASS_OF__L9*/ curfnum[6] = 
     basilys_is_subclass_of((basilysobject_ptr_t)(/*_.SCLA__V14*/ curfptr[13]), (basilysobject_ptr_t)(/*_.FLD_OWNCLASS__V28*/ curfptr[26]));;
    /*_#NOT__L10*/ curfnum[9] = 
     (!(/*_#SUBCLASS_OF__L9*/ curfnum[6]));;
    BASILYS_LOCATION("warmelt-normatch.bysl:210:/ cond");
    /*cond*/ if (/*_#NOT__L10*/ curfnum[9]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:210:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:210:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:212:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.FLD__V24*/ curfptr[23]), 1, "NAMED_NAME");/*_.NAMED_NAME__V30*/ curfptr[29] = slot; };
      ;
      
      basilys_error_str((basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "bad field in object pattern"), (basilys_ptr_t)(/*_.NAMED_NAME__V30*/ curfptr[29]));
      BASILYS_LOCATION("warmelt-normatch.bysl:213:/ getslot");
      { basilys_ptr_t slot=0;
       basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SCLA__V14*/ curfptr[13]), 1, "NAMED_NAME");/*_.NAMED_NAME__V31*/ curfptr[30] = slot; };
      ;
      
      basilys_inform_str((basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "class of pattern is"), (basilys_ptr_t)(/*_.NAMED_NAME__V31*/ curfptr[30]));
      BASILYS_LOCATION("warmelt-normatch.bysl:214:/ block");
      /*block*/{
       /*_.RETVAL___V1*/ curfptr[0] = NULL;;
       BASILYS_LOCATION("warmelt-normatch.bysl:214:/ finalreturn");
       ;
       /*finalret*/ goto labend_rout ;}
      ;
      BASILYS_LOCATION("warmelt-normatch.bysl:210:/ block");
      /*block*/{
       /*_.PROGN___V33*/ curfptr[32] = /*_.RETURN___V32*/ curfptr[31];;}
      ;
      /*_.IFELSE___V29*/ curfptr[28] = /*_.PROGN___V33*/ curfptr[32];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:210:/ clear");
      /*clear*/ /*_.NAMED_NAME__V30*/ curfptr[29] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:210:/ clear");
      /*clear*/ /*_.NAMED_NAME__V31*/ curfptr[30] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:210:/ clear");
      /*clear*/ /*_.RETURN___V32*/ curfptr[31] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:210:/ clear");
      /*clear*/ /*_.PROGN___V33*/ curfptr[32] = 0 ;}
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:210:/ cond.else");
    
     /*_.IFELSE___V29*/ curfptr[28] = NULL;;
     }
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:215:/ msend");
    /*msend*/{
     union basilysparam_un argtab[3];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:215:/ ojbmsend.arg");
     argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
     BASILYS_LOCATION("warmelt-normatch.bysl:215:/ ojbmsend.arg");
     argtab[1].bp_aptr = (basilys_ptr_t*) &(/*!CTYPE_VALUE*/ curfrout->tabval[7]);
     BASILYS_LOCATION("warmelt-normatch.bysl:215:/ ojbmsend.arg");
     argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
     /*_.SCAN_PATTERN__V34*/ curfptr[29] = basilysgc_send((basilys_ptr_t)(/*_.FPAT__V25*/ curfptr[24]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[6])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_.FLD__V24*/ curfptr[23] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_.FPAT__V25*/ curfptr[24] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_.IFCPP___V26*/ curfptr[25] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_.FLD_OWNCLASS__V28*/ curfptr[26] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_#SUBCLASS_OF__L9*/ curfnum[6] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_#NOT__L10*/ curfnum[9] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_.IFELSE___V29*/ curfptr[28] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:206:/ clear");
    /*clear*/ /*_.SCAN_PATTERN__V34*/ curfptr[29] = 0 ;}
   ;
  } /* end cit1__EACHTUP*/
  
   /*citerepilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:202:/ clear");
   /*clear*/ /*_.FLDPA__V20*/ curfptr[18] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:202:/ clear");
   /*clear*/ /*_#IX__L6*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:202:/ clear");
   /*clear*/ /*_.IFCPP___V21*/ curfptr[20] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:202:/ clear");
   /*clear*/ /*_.LET___V23*/ curfptr[21] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:218:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:218:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L11*/ curfnum[6] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:218:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:218:/ apply.arg");
     argtab[0].bp_cstring =  "scanpat_srcpatobject end recv";
     BASILYS_LOCATION("warmelt-normatch.bysl:218:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L11*/ curfnum[6];
     BASILYS_LOCATION("warmelt-normatch.bysl:218:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:218:/ apply.arg");
     argtab[3].bp_long = 218;
     /*_.DEBUG_MSG_FUN__V36*/ curfptr[31] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[0])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V35*/ curfptr[30] = /*_.DEBUG_MSG_FUN__V36*/ curfptr[31];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:218:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L11*/ curfnum[6] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:218:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V36*/ curfptr[31] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:218:/ cppif.else");
  /*_.IFCPP___V35*/ curfptr[30] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V12*/ curfptr[10] = /*_.IFCPP___V35*/ curfptr[30];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
  /*clear*/ /*_.SCLA__V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
  /*clear*/ /*_.SFIELDS__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
  /*clear*/ /*_.IFCPP___V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
  /*clear*/ /*_.IFCPP___V18*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:196:/ clear");
  /*clear*/ /*_.IFCPP___V35*/ curfptr[30] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:192:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATOBJECT", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_7_SCANPAT_SRCPATOBJECT*/





static basilys_ptr_t
rout_8_SCANPAT_SRCPATCOMPOSITE(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_8_SCANPAT_SRCPATCOMPOSITE_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 54
  void* varptr[54];
#define CURFRAM_NBVARNUM 12
  long varnum[12];
/*others*/
  long _spare_; }
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 54;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("SCANPAT_SRCPATCOMPOSITE", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.CTYP__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PCN__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:227:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:228:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:228:/ block");
  /*block*/{
   /*_#IS_A__L1*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), (basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[0])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ cond");
   /*cond*/ if (/*_#IS_A__L1*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ cond.then");
   /*_.IFELSE___V7*/ curfptr[6] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:228:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:228:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check pcn"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(228)?(228):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.IFELSE___V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ clear");
   /*clear*/ /*_#IS_A__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:228:/ clear");
   /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:228:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:229:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:229:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRCPATTERN_COMPOSITE*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:229:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:229:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:229:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:229:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(229)?(229):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:229:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:229:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:229:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:230:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:230:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L3*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:230:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:230:/ apply.arg");
    argtab[0].bp_cstring =  "scanpat_srcpatcomposite recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:230:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L3*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:230:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:230:/ apply.arg");
    argtab[3].bp_long = 230;
    /*_.DEBUG_MSG_FUN__V11*/ curfptr[10] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.DEBUG_MSG_FUN__V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:230:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:230:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:230:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:231:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V13*/ curfptr[12] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:232:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:232:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 3, "SPAC_OPERATOR");/*_.SOP__V15*/ curfptr[14] = slot; };
   ;
   
   #if ENABLE_CHECKING
    BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:233:/ block");
    /*block*/{
     /*_#IS_A__L4*/ curfnum[0] = 
       basilys_is_instance_of((basilys_ptr_t)(/*_.SOP__V15*/ curfptr[14]), (basilys_ptr_t)((/*!CLASS_ANY_MATCHER*/ curfrout->tabval[3])));;
     BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cond");
     /*cond*/ if (/*_#IS_A__L4*/ curfnum[0]) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cond.then");
     /*_.IFELSE___V17*/ curfptr[16] = (/*nil*/NULL);;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cond.else");
     
      BASILYS_LOCATION("warmelt-normatch.bysl:233:/ block");
      /*block*/{
       /*block*/{
        
        basilys_assert_failed(( "check soper"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(233)?(233):__LINE__, __FUNCTION__);
        /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
       ;
       /*epilog*/}
      ;
      }
     ;
     /*_.IFCPP___V16*/ curfptr[15] = /*_.IFELSE___V17*/ curfptr[16];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:233:/ clear");
     /*clear*/ /*_#IS_A__L4*/ curfnum[0] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:233:/ clear");
     /*clear*/ /*_.IFELSE___V17*/ curfptr[16] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:233:/ cppif.else");
   /*_.IFCPP___V16*/ curfptr[15] = (/*nil*/NULL);
    #endif /*ENABLE_CHECKING*/
    ;
   /*_.LET___V14*/ curfptr[13] = /*_.SOP__V15*/ curfptr[14];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
   /*clear*/ /*_.SOP__V15*/ curfptr[14] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:232:/ clear");
   /*clear*/ /*_.IFCPP___V16*/ curfptr[15] = 0 ;}
  ;
  /*_.SOPER__V18*/ curfptr[16] = /*_.LET___V14*/ curfptr[13];;
  BASILYS_LOCATION("warmelt-normatch.bysl:235:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 4, "SPAC_INARGS");/*_.SINS__V19*/ curfptr[14] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:236:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 5, "SPAC_OUTARGS");/*_.SOUTS__V20*/ curfptr[15] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:237:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:237:/ getslot");
   { basilys_ptr_t slot=0;
    basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 3, "AMATCH_MATCHBIND");/*_.MB__V22*/ curfptr[21] = slot; };
   ;
   
   #if ENABLE_CHECKING
    BASILYS_LOCATION("warmelt-normatch.bysl:238:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:238:/ block");
    /*block*/{
     /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 
      callcount;;
     BASILYS_LOCATION("warmelt-normatch.bysl:238:/ apply");
     /*apply*/{
      union basilysparam_un argtab[4];
      memset(&argtab, 0, sizeof(argtab));
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ apply.arg");
      argtab[0].bp_cstring =  "scanpat_srcpatcomposite matmb";
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ apply.arg");
      argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[0];
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ apply.arg");
      argtab[2].bp_cstring =  "warmelt-normatch.bysl";
      BASILYS_LOCATION("warmelt-normatch.bysl:238:/ apply.arg");
      argtab[3].bp_long = 238;
      /*_.DEBUG_MSG_FUN__V24*/ curfptr[23] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.MB__V22*/ curfptr[21]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
      }
     ;
     /*_.IFCPP___V23*/ curfptr[22] = /*_.DEBUG_MSG_FUN__V24*/ curfptr[23];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:238:/ clear");
     /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:238:/ clear");
     /*clear*/ /*_.DEBUG_MSG_FUN__V24*/ curfptr[23] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:238:/ cppif.else");
   /*_.IFCPP___V23*/ curfptr[22] = (/*nil*/NULL);
    #endif /*ENABLE_CHECKING*/
    ;
   
   #if ENABLE_CHECKING
    BASILYS_LOCATION("warmelt-normatch.bysl:239:/ cppif.then");
   BASILYS_LOCATION("warmelt-normatch.bysl:239:/ block");
    /*block*/{
     /*_#IS_A__L6*/ curfnum[0] = 
       basilys_is_instance_of((basilys_ptr_t)(/*_.MB__V22*/ curfptr[21]), (basilys_ptr_t)((/*!CLASS_FORMAL_BINDING*/ curfrout->tabval[4])));;
     BASILYS_LOCATION("warmelt-normatch.bysl:239:/ cond");
     /*cond*/ if (/*_#IS_A__L6*/ curfnum[0]) /*then*/ {
     BASILYS_LOCATION("warmelt-normatch.bysl:239:/ cond.then");
     /*_.IFELSE___V26*/ curfptr[25] = (/*nil*/NULL);;
     } else {BASILYS_LOCATION("warmelt-normatch.bysl:239:/ cond.else");
     
      BASILYS_LOCATION("warmelt-normatch.bysl:239:/ block");
      /*block*/{
       /*block*/{
        
        basilys_assert_failed(( "check matmb"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(239)?(239):__LINE__, __FUNCTION__);
        /*clear*/ /*_.IFELSE___V26*/ curfptr[25] = 0 ;}
       ;
       /*epilog*/}
      ;
      }
     ;
     /*_.IFCPP___V25*/ curfptr[23] = /*_.IFELSE___V26*/ curfptr[25];;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:239:/ clear");
     /*clear*/ /*_#IS_A__L6*/ curfnum[0] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:239:/ clear");
     /*clear*/ /*_.IFELSE___V26*/ curfptr[25] = 0 ;}
    
    #else /*ENABLE_CHECKING*/
    BASILYS_LOCATION("warmelt-normatch.bysl:239:/ cppif.else");
   /*_.IFCPP___V25*/ curfptr[23] = (/*nil*/NULL);
    #endif /*ENABLE_CHECKING*/
    ;
   /*_.LET___V21*/ curfptr[20] = /*_.MB__V22*/ curfptr[21];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:237:/ clear");
   /*clear*/ /*_.MB__V22*/ curfptr[21] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:237:/ clear");
   /*clear*/ /*_.IFCPP___V23*/ curfptr[22] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:237:/ clear");
   /*clear*/ /*_.IFCPP___V25*/ curfptr[23] = 0 ;}
  ;
  /*_.MATMB__V27*/ curfptr[25] = /*_.LET___V21*/ curfptr[20];;
  BASILYS_LOCATION("warmelt-normatch.bysl:241:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 2, "AMATCH_IN");/*_.MATIN__V28*/ curfptr[21] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:242:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 4, "AMATCH_OUT");/*_.MATOUT__V29*/ curfptr[22] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:243:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.SOPER__V18*/ curfptr[16]), 1, "NAMED_NAME");/*_.OPNAM__V30*/ curfptr[23] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:244:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MATMB__V27*/ curfptr[25]), 1, "FBIND_TYPE");/*_.MATCTYP__V31*/ curfptr[30] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:245:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 2, "PCTN_ENV");/*_.ENV__V32*/ curfptr[31] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:246:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 0, "PCTN_NORMCTXT");/*_.NCX__V33*/ curfptr[32] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:247:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.PCN__V5*/ curfptr[4]), 5, "PCTN_BINDLIST");/*_.BINDLIST__V34*/ curfptr[33] = slot; };
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:249:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:249:/ block");
   /*block*/{
    /*_#IS_A__L7*/ curfnum[0] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3]), (basilys_ptr_t)((/*!CLASS_CTYPE*/ curfrout->tabval[5])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:249:/ cond");
    /*cond*/ if (/*_#IS_A__L7*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:249:/ cond.then");
    /*_.IFELSE___V36*/ curfptr[35] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:249:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:249:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check ctyp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(249)?(249):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V36*/ curfptr[35] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V35*/ curfptr[34] = /*_.IFELSE___V36*/ curfptr[35];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:249:/ clear");
    /*clear*/ /*_#IS_A__L7*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:249:/ clear");
    /*clear*/ /*_.IFELSE___V36*/ curfptr[35] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:249:/ cppif.else");
  /*_.IFCPP___V35*/ curfptr[34] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:250:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:250:/ block");
   /*block*/{
    /*_#IS_A__L8*/ curfnum[0] = 
      basilys_is_instance_of((basilys_ptr_t)(/*_.MATCTYP__V31*/ curfptr[30]), (basilys_ptr_t)((/*!CLASS_CTYPE*/ curfrout->tabval[5])));;
    BASILYS_LOCATION("warmelt-normatch.bysl:250:/ cond");
    /*cond*/ if (/*_#IS_A__L8*/ curfnum[0]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:250:/ cond.then");
    /*_.IFELSE___V38*/ curfptr[37] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:250:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:250:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "check matctyp"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(250)?(250):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V38*/ curfptr[37] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V37*/ curfptr[35] = /*_.IFELSE___V38*/ curfptr[37];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:250:/ clear");
    /*clear*/ /*_#IS_A__L8*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:250:/ clear");
    /*clear*/ /*_.IFELSE___V38*/ curfptr[37] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:250:/ cppif.else");
  /*_.IFCPP___V37*/ curfptr[35] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_#__L9*/ curfnum[0] = 
   ((/*_.CTYP__V4*/ curfptr[3]) != (/*_.MATCTYP__V31*/ curfptr[30]));;
  BASILYS_LOCATION("warmelt-normatch.bysl:251:/ cond");
  /*cond*/ if (/*_#__L9*/ curfnum[0]) /*then*/ {
  BASILYS_LOCATION("warmelt-normatch.bysl:251:/ cond.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:251:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:253:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.MATCTYP__V31*/ curfptr[30]), 1, "NAMED_NAME");/*_.NAMED_NAME__V40*/ curfptr[39] = slot; };
    ;
    
    basilys_error_str((basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "invalid ctype in composite partner - expecting"), (basilys_ptr_t)(/*_.NAMED_NAME__V40*/ curfptr[39]));
    BASILYS_LOCATION("warmelt-normatch.bysl:255:/ getslot");
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CTYP__V4*/ curfptr[3]), 1, "NAMED_NAME");/*_.NAMED_NAME__V41*/ curfptr[40] = slot; };
    ;
    
    basilys_warning_str(0, (basilys_ptr_t)(/*_.SLOC__V13*/ curfptr[12]), ( "got ctype"), (basilys_ptr_t)(/*_.NAMED_NAME__V41*/ curfptr[40]));
    BASILYS_LOCATION("warmelt-normatch.bysl:256:/ block");
    /*block*/{
     /*_.RETVAL___V1*/ curfptr[0] = NULL;;
     BASILYS_LOCATION("warmelt-normatch.bysl:256:/ finalreturn");
     ;
     /*finalret*/ goto labend_rout ;}
    ;
    BASILYS_LOCATION("warmelt-normatch.bysl:251:/ block");
    /*block*/{
     /*_.PROGN___V43*/ curfptr[42] = /*_.RETURN___V42*/ curfptr[41];;}
    ;
    /*_.IFELSE___V39*/ curfptr[37] = /*_.PROGN___V43*/ curfptr[42];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:251:/ clear");
    /*clear*/ /*_.NAMED_NAME__V40*/ curfptr[39] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:251:/ clear");
    /*clear*/ /*_.NAMED_NAME__V41*/ curfptr[40] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:251:/ clear");
    /*clear*/ /*_.RETURN___V42*/ curfptr[41] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:251:/ clear");
    /*clear*/ /*_.PROGN___V43*/ curfptr[42] = 0 ;}
   ;
  } else {BASILYS_LOCATION("warmelt-normatch.bysl:251:/ cond.else");
  
   /*_.IFELSE___V39*/ curfptr[37] = NULL;;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:257:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:257:/ multiapply");
   /*multiapply 4args, 1x.res*/ {
    union basilysparam_un argtab[3];
    
    union basilysparam_un restab[1];
    memset(&restab, 0, sizeof(restab));
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:257:/ multiapply.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V32*/ curfptr[31];BASILYS_LOCATION("warmelt-normatch.bysl:257:/ multiapply.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V33*/ curfptr[32];BASILYS_LOCATION("warmelt-normatch.bysl:257:/ multiapply.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
    BASILYS_LOCATION("warmelt-normatch.bysl:257:/ multiapply.xres");
    restab[0].bp_aptr = (basilys_ptr_t*) &/*_.BINDINS__V46*/ curfptr[41];BASILYS_LOCATION("warmelt-normatch.bysl:257:/ multiapply.appl");
    /*_.NINS__V45*/ curfptr[40] =  basilys_apply ((basilysclosure_ptr_t)((/*!NORMALIZE_TUPLE*/ curfrout->tabval[6])), (basilys_ptr_t)(/*_.SINS__V19*/ curfptr[14]), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
    }
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:257:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:260:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:260:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:260:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:260:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatcomposite nins";
       BASILYS_LOCATION("warmelt-normatch.bysl:260:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L10*/ curfnum[9];
       BASILYS_LOCATION("warmelt-normatch.bysl:260:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:260:/ apply.arg");
       argtab[3].bp_long = 260;
       /*_.DEBUG_MSG_FUN__V48*/ curfptr[47] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.NINS__V45*/ curfptr[40]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V47*/ curfptr[42] = /*_.DEBUG_MSG_FUN__V48*/ curfptr[47];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L10*/ curfnum[9] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:260:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V48*/ curfptr[47] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:260:/ cppif.else");
    /*_.IFCPP___V47*/ curfptr[42] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:261:/ cond");
    /*cond*/ if (/*_.BINDINS__V46*/ curfptr[41]) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:261:/ cond.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:261:/ block");
     /*block*/{
      BASILYS_LOCATION("warmelt-normatch.bysl:261:/ apply");
      /*apply*/{
       union basilysparam_un argtab[1];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:261:/ apply.arg");
       argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.BINDINS__V46*/ curfptr[41];
       /*_.LIST_APPEND2LIST__V50*/ curfptr[49] =  basilys_apply ((basilysclosure_ptr_t)((/*!LIST_APPEND2LIST*/ curfrout->tabval[7])), (basilys_ptr_t)(/*_.BINDLIST__V34*/ curfptr[33]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IF___V49*/ curfptr[47] = /*_.LIST_APPEND2LIST__V50*/ curfptr[49];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:261:/ clear");
      /*clear*/ /*_.LIST_APPEND2LIST__V50*/ curfptr[49] = 0 ;}
     ;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:261:/ cond.else");
    
     /*_.IF___V49*/ curfptr[47] = NULL;;
     }
    ;
    /*citerblock FOREACH_IN_MULTIPLE*/ {
    long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SOUTS__V20*/ curfptr[15]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.POUT__V51*/ curfptr[49] = basilys_multiple_nth((basilys_ptr_t)(/*_.SOUTS__V20*/ curfptr[15]), cit1__EACHTUP_ix);
/*_#IX__L11*/ curfnum[9] = cit1__EACHTUP_ix;

    
     BASILYS_LOCATION("warmelt-normatch.bysl:265:/ msend");
     /*msend*/{
      union basilysparam_un argtab[3];
      memset(&argtab, 0, sizeof(argtab));
      BASILYS_LOCATION("warmelt-normatch.bysl:265:/ ojbmsend.arg");
      argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V13*/ curfptr[12];
      BASILYS_LOCATION("warmelt-normatch.bysl:265:/ ojbmsend.arg");
      argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V4*/ curfptr[3];
      BASILYS_LOCATION("warmelt-normatch.bysl:265:/ ojbmsend.arg");
      argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V5*/ curfptr[4];
      basilysgc_send((basilys_ptr_t)(/*_.POUT__V51*/ curfptr[49]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[8])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
      }
     ;
    } /* end cit1__EACHTUP*/
    
     /*citerepilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:262:/ clear");
     /*clear*/ /*_.POUT__V51*/ curfptr[49] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:262:/ clear");
     /*clear*/ /*_#IX__L11*/ curfnum[9] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:262:/ clear");
     /*clear*/ /*_.SCAN_PATTERN__V52*/ curfptr[51] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
    ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:267:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:267:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L12*/ curfnum[11] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
       argtab[0].bp_cstring =  "scanpat_srcpatcomposite end recv";
       BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L12*/ curfnum[11];
       BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:267:/ apply.arg");
       argtab[3].bp_long = 267;
       /*_.DEBUG_MSG_FUN__V54*/ curfptr[53] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[2])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V53*/ curfptr[52] = /*_.DEBUG_MSG_FUN__V54*/ curfptr[53];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:267:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L12*/ curfnum[11] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:267:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V54*/ curfptr[53] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:267:/ cppif.else");
    /*_.IFCPP___V53*/ curfptr[52] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    /*_.MULTI___V44*/ curfptr[39] = /*_.IFCPP___V53*/ curfptr[52];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:257:/ clear");
    /*clear*/ /*_.IFCPP___V47*/ curfptr[42] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:257:/ clear");
    /*clear*/ /*_.IF___V49*/ curfptr[47] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:257:/ clear");
    /*clear*/ /*_.IFCPP___V53*/ curfptr[52] = 0 ;}
   ;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:257:/ clear");
   /*clear*/ /*_.BINDINS__V46*/ curfptr[41] = 0 ;}
  ;
  /*_.LET___V12*/ curfptr[10] = /*_.MULTI___V44*/ curfptr[39];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.SLOC__V13*/ curfptr[12] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.LET___V14*/ curfptr[13] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.SOPER__V18*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.SINS__V19*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.SOUTS__V20*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.LET___V21*/ curfptr[20] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.MATMB__V27*/ curfptr[25] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.MATIN__V28*/ curfptr[21] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.MATOUT__V29*/ curfptr[22] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.OPNAM__V30*/ curfptr[23] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.MATCTYP__V31*/ curfptr[30] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.ENV__V32*/ curfptr[31] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.NCX__V33*/ curfptr[32] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.BINDLIST__V34*/ curfptr[33] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.IFCPP___V35*/ curfptr[34] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.IFCPP___V37*/ curfptr[35] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_#__L9*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.IFELSE___V39*/ curfptr[37] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:231:/ clear");
  /*clear*/ /*_.MULTI___V44*/ curfptr[39] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V12*/ curfptr[10];;
  BASILYS_LOCATION("warmelt-normatch.bysl:227:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ clear");
 /*clear*/ /*_.LET___V12*/ curfptr[10] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("SCANPAT_SRCPATCOMPOSITE", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_8_SCANPAT_SRCPATCOMPOSITE*/





static basilys_ptr_t
rout_9_NORMEXP_MATCH(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_9_NORMEXP_MATCH_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 55
  void* varptr[55];
#define CURFRAM_NBVARNUM 15
  long varnum[15];
/*others*/
  long _spare_; }
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 55;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("NORMEXP_MATCH", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.ENV__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.ENV__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.NCX__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.NCX__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:292:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:293:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:293:/ block");
  /*block*/{
   /*_#IS_A__L1*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRC_MATCH*/ curfrout->tabval[0])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:293:/ cond");
   /*cond*/ if (/*_#IS_A__L1*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:293:/ cond.then");
   /*_.IFELSE___V7*/ curfptr[6] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:293:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:293:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check match recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(293)?(293):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.IFELSE___V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:293:/ clear");
   /*clear*/ /*_#IS_A__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:293:/ clear");
   /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:293:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:294:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:294:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.ENV__V3*/ curfptr[2]), (basilys_ptr_t)((/*!CLASS_ENVIRONMENT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:294:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:294:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:294:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:294:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check env"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(294)?(294):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:294:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:294:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:294:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:295:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:295:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.NCX__V4*/ curfptr[3]), (basilys_ptr_t)((/*!CLASS_NORMCONTEXT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:295:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:295:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:295:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:295:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check nctxt"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(295)?(295):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:295:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:295:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:295:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:296:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:296:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:296:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:296:/ apply.arg");
    argtab[0].bp_cstring =  "normexp_match recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:296:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L4*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:296:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:296:/ apply.arg");
    argtab[3].bp_long = 296;
    /*_.DEBUG_MSG_FUN__V13*/ curfptr[12] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V12*/ curfptr[10] = /*_.DEBUG_MSG_FUN__V13*/ curfptr[12];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:296:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:296:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V13*/ curfptr[12] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:296:/ cppif.else");
 /*_.IFCPP___V12*/ curfptr[10] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:297:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V15*/ curfptr[14] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:298:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SMAT_MATCHEDX");/*_.SMATSX__V16*/ curfptr[15] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:299:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 2, "SMAT_CASES");/*_.SCASES__V17*/ curfptr[16] = slot; };
  ;
  /*_#NBCASES__L5*/ curfnum[0] = 
   (basilys_multiple_length((basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16])));;
  /*_.TUPVAR__V18*/ curfptr[17] = 
   (basilysgc_new_multiple((basilysobject_ptr_t)((/*!DISCR_MULTIPLE*/ curfrout->tabval[4])), (/*_#NBCASES__L5*/ curfnum[0])));;
  /*_.TUPCST__V19*/ curfptr[18] = 
   (basilysgc_new_multiple((basilysobject_ptr_t)((/*!DISCR_MULTIPLE*/ curfrout->tabval[4])), (/*_#NBCASES__L5*/ curfnum[0])));;
  /*_.TUPBINDLIST__V20*/ curfptr[19] = 
   (basilysgc_new_multiple((basilysobject_ptr_t)((/*!DISCR_MULTIPLE*/ curfrout->tabval[4])), (/*_#NBCASES__L5*/ curfnum[0])));;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:305:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:305:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L6*/ curfnum[5] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:305:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:305:/ apply.arg");
     argtab[0].bp_cstring =  "normexp_match smatsx";
     BASILYS_LOCATION("warmelt-normatch.bysl:305:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L6*/ curfnum[5];
     BASILYS_LOCATION("warmelt-normatch.bysl:305:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:305:/ apply.arg");
     argtab[3].bp_long = 305;
     /*_.DEBUG_MSG_FUN__V22*/ curfptr[21] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.SMATSX__V16*/ curfptr[15]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V21*/ curfptr[20] = /*_.DEBUG_MSG_FUN__V22*/ curfptr[21];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:305:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L6*/ curfnum[5] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:305:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V22*/ curfptr[21] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:305:/ cppif.else");
  /*_.IFCPP___V21*/ curfptr[20] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  BASILYS_LOCATION("warmelt-normatch.bysl:306:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:306:/ multimsend");
   /*multimsend*/{
    union basilysparam_un argtab[3];
    union basilysparam_un restab[1];
    memset(&argtab, 0, sizeof(argtab));
    memset(&restab, 0, sizeof(restab));
    BASILYS_LOCATION("warmelt-normatch.bysl:306:/ multimsend.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V3*/ curfptr[2];BASILYS_LOCATION("warmelt-normatch.bysl:306:/ multimsend.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V4*/ curfptr[3];BASILYS_LOCATION("warmelt-normatch.bysl:306:/ multimsend.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V15*/ curfptr[14];
    BASILYS_LOCATION("warmelt-normatch.bysl:306:/ multimsend.xres");
    restab[0].bp_aptr = (basilys_ptr_t*) &/*_.NBINDMATX__V25*/ curfptr[24];BASILYS_LOCATION("warmelt-normatch.bysl:306:/ multimsend.send");
    /*_.NMATX__V24*/ curfptr[23] =  basilysgc_send ((basilys_ptr_t)(/*_.SMATSX__V16*/ curfptr[15]), ((basilys_ptr_t)((/*!NORMAL_EXP*/ curfrout->tabval[5]))), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
    }
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:306:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:309:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:309:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L7*/ curfnum[5] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:309:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:309:/ apply.arg");
       argtab[0].bp_cstring =  "normexp_match nmatx";
       BASILYS_LOCATION("warmelt-normatch.bysl:309:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L7*/ curfnum[5];
       BASILYS_LOCATION("warmelt-normatch.bysl:309:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:309:/ apply.arg");
       argtab[3].bp_long = 309;
       /*_.DEBUG_MSG_FUN__V27*/ curfptr[26] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.NMATX__V24*/ curfptr[23]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V26*/ curfptr[25] = /*_.DEBUG_MSG_FUN__V27*/ curfptr[26];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:309:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L7*/ curfnum[5] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:309:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V27*/ curfptr[26] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:309:/ cppif.else");
    /*_.IFCPP___V26*/ curfptr[25] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:310:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:310:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L8*/ curfnum[5] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:310:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:310:/ apply.arg");
       argtab[0].bp_cstring =  "normexp_match scases";
       BASILYS_LOCATION("warmelt-normatch.bysl:310:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L8*/ curfnum[5];
       BASILYS_LOCATION("warmelt-normatch.bysl:310:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:310:/ apply.arg");
       argtab[3].bp_long = 310;
       /*_.DEBUG_MSG_FUN__V29*/ curfptr[28] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V28*/ curfptr[26] = /*_.DEBUG_MSG_FUN__V29*/ curfptr[28];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:310:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L8*/ curfnum[5] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:310:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V29*/ curfptr[28] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:310:/ cppif.else");
    /*_.IFCPP___V28*/ curfptr[26] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    BASILYS_LOCATION("warmelt-normatch.bysl:311:/ block");
    /*block*/{
     BASILYS_LOCATION("warmelt-normatch.bysl:311:/ msend");
     /*msend*/{
      union basilysparam_un argtab[1];
      memset(&argtab, 0, sizeof(argtab));
      BASILYS_LOCATION("warmelt-normatch.bysl:311:/ ojbmsend.arg");
      argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V3*/ curfptr[2];
      /*_.CTYP__V31*/ curfptr[30] = basilysgc_send((basilys_ptr_t)(/*_.NMATX__V24*/ curfptr[23]), (basilys_ptr_t)((/*!GET_CTYPE*/ curfrout->tabval[6])), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
      }
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:312:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:312:/ block");
      /*block*/{
       /*_#THE_CALLCOUNT__L9*/ curfnum[5] = 
        callcount;;
       BASILYS_LOCATION("warmelt-normatch.bysl:312:/ apply");
       /*apply*/{
        union basilysparam_un argtab[4];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:312:/ apply.arg");
        argtab[0].bp_cstring =  "normexp_match ctyp";
        BASILYS_LOCATION("warmelt-normatch.bysl:312:/ apply.arg");
        argtab[1].bp_long = /*_#THE_CALLCOUNT__L9*/ curfnum[5];
        BASILYS_LOCATION("warmelt-normatch.bysl:312:/ apply.arg");
        argtab[2].bp_cstring =  "warmelt-normatch.bysl";
        BASILYS_LOCATION("warmelt-normatch.bysl:312:/ apply.arg");
        argtab[3].bp_long = 312;
        /*_.DEBUG_MSG_FUN__V33*/ curfptr[32] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.CTYP__V31*/ curfptr[30]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       /*_.IFCPP___V32*/ curfptr[31] = /*_.DEBUG_MSG_FUN__V33*/ curfptr[32];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:312:/ clear");
       /*clear*/ /*_#THE_CALLCOUNT__L9*/ curfnum[5] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:312:/ clear");
       /*clear*/ /*_.DEBUG_MSG_FUN__V33*/ curfptr[32] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:312:/ cppif.else");
     /*_.IFCPP___V32*/ curfptr[31] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     /*citerblock FOREACH_IN_MULTIPLE*/ {
     long cit1__EACHTUP_ln = basilys_multiple_length((basilys_ptr_t)/*_.SCASES__V17*/ curfptr[16]);
int cit1__EACHTUP_ix = 0;
for (cit1__EACHTUP_ix = 0; cit1__EACHTUP_ix < cit1__EACHTUP_ln; cit1__EACHTUP_ix ++) {
/*_.CURCAS__V34*/ curfptr[32] = basilys_multiple_nth((basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16]), cit1__EACHTUP_ix);
/*_#IX__L10*/ curfnum[5] = cit1__EACHTUP_ix;

     
      
      #if ENABLE_CHECKING
       BASILYS_LOCATION("warmelt-normatch.bysl:316:/ cppif.then");
      BASILYS_LOCATION("warmelt-normatch.bysl:316:/ block");
       /*block*/{
        /*_#IS_A__L11*/ curfnum[10] = 
          basilys_is_instance_of((basilys_ptr_t)(/*_.CURCAS__V34*/ curfptr[32]), (basilys_ptr_t)((/*!CLASS_SRC_CASEMATCH*/ curfrout->tabval[7])));;
        BASILYS_LOCATION("warmelt-normatch.bysl:316:/ cond");
        /*cond*/ if (/*_#IS_A__L11*/ curfnum[10]) /*then*/ {
        BASILYS_LOCATION("warmelt-normatch.bysl:316:/ cond.then");
        /*_.IFELSE___V36*/ curfptr[35] = (/*nil*/NULL);;
        } else {BASILYS_LOCATION("warmelt-normatch.bysl:316:/ cond.else");
        
         BASILYS_LOCATION("warmelt-normatch.bysl:316:/ block");
         /*block*/{
          /*block*/{
           
           basilys_assert_failed(( "check curcas"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(316)?(316):__LINE__, __FUNCTION__);
           /*clear*/ /*_.IFELSE___V36*/ curfptr[35] = 0 ;}
          ;
          /*epilog*/}
         ;
         }
        ;
        /*epilog*/
        BASILYS_LOCATION("warmelt-normatch.bysl:316:/ clear");
        /*clear*/ /*_#IS_A__L11*/ curfnum[10] = 0 ;
        BASILYS_LOCATION("warmelt-normatch.bysl:316:/ clear");
        /*clear*/ /*_.IFELSE___V36*/ curfptr[35] = 0 ;}
       
       #else /*ENABLE_CHECKING*/
       BASILYS_LOCATION("warmelt-normatch.bysl:316:/ cppif.else");
      (/*nil*/NULL)
       #endif /*ENABLE_CHECKING*/
       ;
      BASILYS_LOCATION("warmelt-normatch.bysl:317:/ block");
      /*block*/{
       BASILYS_LOCATION("warmelt-normatch.bysl:318:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURCAS__V34*/ curfptr[32]), 0, "SRC_LOC");/*_.CURLOC__V37*/ curfptr[35] = slot; };
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:319:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURCAS__V34*/ curfptr[32]), 1, "SCAM_PATT");/*_.CURPAT__V38*/ curfptr[37] = slot; };
       ;
       BASILYS_LOCATION("warmelt-normatch.bysl:320:/ getslot");
       { basilys_ptr_t slot=0;
        basilys_object_get_field(slot,(basilys_ptr_t)(/*_.CURCAS__V34*/ curfptr[32]), 2, "SCAM_BODY");/*_.CURBODY__V39*/ curfptr[38] = slot; };
       ;
       /*_.MAPVAR__V40*/ curfptr[39] = 
         (basilysgc_new_mapobjects( (basilysobject_ptr_t) ((/*!DISCR_MAPOBJECTS*/ curfrout->tabval[8])), (13)));;
       /*_.MAPCST__V41*/ curfptr[40] = 
         (basilysgc_new_mapobjects( (basilysobject_ptr_t) ((/*!DISCR_MAPOBJECTS*/ curfrout->tabval[8])), (11)));;
       /*_.BINDLIST__V42*/ curfptr[41] = 
        (basilysgc_new_list((basilysobject_ptr_t)((/*!DISCR_LIST*/ curfrout->tabval[9]))));;
       BASILYS_LOCATION("warmelt-normatch.bysl:324:/ block");
       /*block*/{
        BASILYS_LOCATION("warmelt-normatch.bysl:324:/ rawallocobj");
        /*rawallocobj*/ { basilys_ptr_t newobj = 0;
         basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[10])), (6), "CLASS_PATTERNCONTEXT");/*_.INST__V44*/ curfptr[43] =
         newobj; };
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:324:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43]))));basilys_putfield_object((/*_.INST__V44*/ curfptr[43]), (0), (/*_.NCX__V4*/ curfptr[3]), "PCTN_NORMCTXT");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:324:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43]))));basilys_putfield_object((/*_.INST__V44*/ curfptr[43]), (1), (/*_.RECV__V2*/ curfptr[1]), "PCTN_SRC");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:324:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43]))));basilys_putfield_object((/*_.INST__V44*/ curfptr[43]), (2), (/*_.ENV__V3*/ curfptr[2]), "PCTN_ENV");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:324:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (3>=0 && 3< basilys_object_length((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43]))));basilys_putfield_object((/*_.INST__V44*/ curfptr[43]), (3), (/*_.MAPVAR__V40*/ curfptr[39]), "PCTN_MAPATVAR");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:324:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43]))));basilys_putfield_object((/*_.INST__V44*/ curfptr[43]), (4), (/*_.MAPCST__V41*/ curfptr[40]), "PCTN_MAPATCST");
        ;
        BASILYS_LOCATION("warmelt-normatch.bysl:324:/ putslot");
        /*putslot*/
        basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43])) == OBMAG_OBJECT);
        basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.INST__V44*/ curfptr[43]))));basilys_putfield_object((/*_.INST__V44*/ curfptr[43]), (5), (/*_.BINDLIST__V42*/ curfptr[41]), "PCTN_BINDLIST");
        ;
        /*_.PCN__V43*/ curfptr[42] = /*_.INST__V44*/ curfptr[43];;}
       ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:333:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:333:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L12*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:333:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:333:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match curcas";
          BASILYS_LOCATION("warmelt-normatch.bysl:333:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L12*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:333:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:333:/ apply.arg");
          argtab[3].bp_long = 333;
          /*_.DEBUG_MSG_FUN__V46*/ curfptr[45] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.CURCAS__V34*/ curfptr[32]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V45*/ curfptr[44] = /*_.DEBUG_MSG_FUN__V46*/ curfptr[45];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:333:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L12*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:333:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V46*/ curfptr[45] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:333:/ cppif.else");
       /*_.IFCPP___V45*/ curfptr[44] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       BASILYS_LOCATION("warmelt-normatch.bysl:334:/ msend");
       /*msend*/{
        union basilysparam_un argtab[3];
        memset(&argtab, 0, sizeof(argtab));
        BASILYS_LOCATION("warmelt-normatch.bysl:334:/ ojbmsend.arg");
        argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.CURLOC__V37*/ curfptr[35];
        BASILYS_LOCATION("warmelt-normatch.bysl:334:/ ojbmsend.arg");
        argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.CTYP__V31*/ curfptr[30];
        BASILYS_LOCATION("warmelt-normatch.bysl:334:/ ojbmsend.arg");
        argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.PCN__V43*/ curfptr[42];
        /*_.SCAN_PATTERN__V47*/ curfptr[45] = basilysgc_send((basilys_ptr_t)(/*_.CURPAT__V38*/ curfptr[37]), (basilys_ptr_t)((/*!SCAN_PATTERN*/ curfrout->tabval[11])), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
        }
       ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:335:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:335:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L13*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:335:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:335:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match mapvar";
          BASILYS_LOCATION("warmelt-normatch.bysl:335:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L13*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:335:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:335:/ apply.arg");
          argtab[3].bp_long = 335;
          /*_.DEBUG_MSG_FUN__V49*/ curfptr[48] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.MAPVAR__V40*/ curfptr[39]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V48*/ curfptr[47] = /*_.DEBUG_MSG_FUN__V49*/ curfptr[48];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:335:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L13*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:335:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V49*/ curfptr[48] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:335:/ cppif.else");
       /*_.IFCPP___V48*/ curfptr[47] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:336:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:336:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L14*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:336:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:336:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match mapcst";
          BASILYS_LOCATION("warmelt-normatch.bysl:336:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L14*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:336:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:336:/ apply.arg");
          argtab[3].bp_long = 336;
          /*_.DEBUG_MSG_FUN__V51*/ curfptr[50] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.MAPCST__V41*/ curfptr[40]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V50*/ curfptr[48] = /*_.DEBUG_MSG_FUN__V51*/ curfptr[50];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:336:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L14*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:336:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V51*/ curfptr[50] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:336:/ cppif.else");
       /*_.IFCPP___V50*/ curfptr[48] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       
       #if ENABLE_CHECKING
        BASILYS_LOCATION("warmelt-normatch.bysl:337:/ cppif.then");
       BASILYS_LOCATION("warmelt-normatch.bysl:337:/ block");
        /*block*/{
         /*_#THE_CALLCOUNT__L15*/ curfnum[10] = 
          callcount;;
         BASILYS_LOCATION("warmelt-normatch.bysl:337:/ apply");
         /*apply*/{
          union basilysparam_un argtab[4];
          memset(&argtab, 0, sizeof(argtab));
          BASILYS_LOCATION("warmelt-normatch.bysl:337:/ apply.arg");
          argtab[0].bp_cstring =  "normexp_match bindlist";
          BASILYS_LOCATION("warmelt-normatch.bysl:337:/ apply.arg");
          argtab[1].bp_long = /*_#THE_CALLCOUNT__L15*/ curfnum[10];
          BASILYS_LOCATION("warmelt-normatch.bysl:337:/ apply.arg");
          argtab[2].bp_cstring =  "warmelt-normatch.bysl";
          BASILYS_LOCATION("warmelt-normatch.bysl:337:/ apply.arg");
          argtab[3].bp_long = 337;
          /*_.DEBUG_MSG_FUN__V53*/ curfptr[52] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.BINDLIST__V42*/ curfptr[41]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
          }
         ;
         /*_.IFCPP___V52*/ curfptr[50] = /*_.DEBUG_MSG_FUN__V53*/ curfptr[52];;
         /*epilog*/
         BASILYS_LOCATION("warmelt-normatch.bysl:337:/ clear");
         /*clear*/ /*_#THE_CALLCOUNT__L15*/ curfnum[10] = 0 ;
         BASILYS_LOCATION("warmelt-normatch.bysl:337:/ clear");
         /*clear*/ /*_.DEBUG_MSG_FUN__V53*/ curfptr[52] = 0 ;}
        
        #else /*ENABLE_CHECKING*/
        BASILYS_LOCATION("warmelt-normatch.bysl:337:/ cppif.else");
       /*_.IFCPP___V52*/ curfptr[50] = (/*nil*/NULL);
        #endif /*ENABLE_CHECKING*/
        ;
       
        basilysgc_multiple_put_nth((basilys_ptr_t)(/*_.TUPVAR__V18*/ curfptr[17]), (/*_#IX__L10*/ curfnum[5]), (basilys_ptr_t)(/*_.MAPVAR__V40*/ curfptr[39]));
       
        basilysgc_multiple_put_nth((basilys_ptr_t)(/*_.TUPCST__V19*/ curfptr[18]), (/*_#IX__L10*/ curfnum[5]), (basilys_ptr_t)(/*_.MAPCST__V41*/ curfptr[40]));
       
        basilysgc_multiple_put_nth((basilys_ptr_t)(/*_.TUPBINDLIST__V20*/ curfptr[19]), (/*_#IX__L10*/ curfnum[5]), (basilys_ptr_t)(/*_.BINDLIST__V42*/ curfptr[41]));
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.CURLOC__V37*/ curfptr[35] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.CURPAT__V38*/ curfptr[37] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.CURBODY__V39*/ curfptr[38] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.MAPVAR__V40*/ curfptr[39] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.MAPCST__V41*/ curfptr[40] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.BINDLIST__V42*/ curfptr[41] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.PCN__V43*/ curfptr[42] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.IFCPP___V45*/ curfptr[44] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.SCAN_PATTERN__V47*/ curfptr[45] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.IFCPP___V48*/ curfptr[47] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.IFCPP___V50*/ curfptr[48] = 0 ;
       BASILYS_LOCATION("warmelt-normatch.bysl:317:/ clear");
       /*clear*/ /*_.IFCPP___V52*/ curfptr[50] = 0 ;}
      ;
     } /* end cit1__EACHTUP*/
     
      /*citerepilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:313:/ clear");
      /*clear*/ /*_.CURCAS__V34*/ curfptr[32] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:313:/ clear");
      /*clear*/ /*_#IX__L10*/ curfnum[5] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:313:/ clear");
      /*clear*/ /*_.IFCPP___V35*/ curfptr[34] = 0 ;} /*endciterblock FOREACH_IN_MULTIPLE*/
     ;
     
     #if ENABLE_CHECKING
      BASILYS_LOCATION("warmelt-normatch.bysl:343:/ cppif.then");
     BASILYS_LOCATION("warmelt-normatch.bysl:343:/ block");
      /*block*/{
       BASILYS_LOCATION("warmelt-normatch.bysl:343:/ cond");
       /*cond*/ if ((/*nil*/NULL)) /*then*/ {
       BASILYS_LOCATION("warmelt-normatch.bysl:343:/ cond.then");
       /*_.IFELSE___V55*/ curfptr[35] = (/*nil*/NULL);;
       } else {BASILYS_LOCATION("warmelt-normatch.bysl:343:/ cond.else");
       
        BASILYS_LOCATION("warmelt-normatch.bysl:343:/ block");
        /*block*/{
         /*block*/{
          
          basilys_assert_failed(( "unimplemented normexp_match"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(343)?(343):__LINE__, __FUNCTION__);
          /*clear*/ /*_.IFELSE___V55*/ curfptr[35] = 0 ;}
         ;
         /*epilog*/}
        ;
        }
       ;
       /*_.IFCPP___V54*/ curfptr[52] = /*_.IFELSE___V55*/ curfptr[35];;
       /*epilog*/
       BASILYS_LOCATION("warmelt-normatch.bysl:343:/ clear");
       /*clear*/ /*_.IFELSE___V55*/ curfptr[35] = 0 ;}
      
      #else /*ENABLE_CHECKING*/
      BASILYS_LOCATION("warmelt-normatch.bysl:343:/ cppif.else");
     /*_.IFCPP___V54*/ curfptr[52] = (/*nil*/NULL);
      #endif /*ENABLE_CHECKING*/
      ;
     /*_.LET___V30*/ curfptr[28] = (/*nil*/NULL);;
     /*epilog*/
     BASILYS_LOCATION("warmelt-normatch.bysl:311:/ clear");
     /*clear*/ /*_.CTYP__V31*/ curfptr[30] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:311:/ clear");
     /*clear*/ /*_.IFCPP___V32*/ curfptr[31] = 0 ;
     BASILYS_LOCATION("warmelt-normatch.bysl:311:/ clear");
     /*clear*/ /*_.IFCPP___V54*/ curfptr[52] = 0 ;}
    ;
    /*_.MULTI___V23*/ curfptr[21] = /*_.LET___V30*/ curfptr[28];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:306:/ clear");
    /*clear*/ /*_.IFCPP___V26*/ curfptr[25] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:306:/ clear");
    /*clear*/ /*_.IFCPP___V28*/ curfptr[26] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:306:/ clear");
    /*clear*/ /*_.LET___V30*/ curfptr[28] = 0 ;}
   ;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:306:/ clear");
   /*clear*/ /*_.NBINDMATX__V25*/ curfptr[24] = 0 ;}
  ;
  /*_.LET___V14*/ curfptr[12] = /*_.MULTI___V23*/ curfptr[21];;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.SLOC__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.SMATSX__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.SCASES__V17*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_#NBCASES__L5*/ curfnum[0] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.TUPVAR__V18*/ curfptr[17] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.TUPCST__V19*/ curfptr[18] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.TUPBINDLIST__V20*/ curfptr[19] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.IFCPP___V21*/ curfptr[20] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:297:/ clear");
  /*clear*/ /*_.MULTI___V23*/ curfptr[21] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V14*/ curfptr[12];;
  BASILYS_LOCATION("warmelt-normatch.bysl:292:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
 /*clear*/ /*_.IFCPP___V12*/ curfptr[10] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ clear");
 /*clear*/ /*_.LET___V14*/ curfptr[12] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("NORMEXP_MATCH", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_9_NORMEXP_MATCH*/



typedef 
 struct frame_start_module_basilys_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *noinitialclos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 196
  void* varptr[196];
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
  struct BASILYS_ROUTINE_STRUCT(3) drout_39__SCANPAT_ANYRECV;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_40__SCANPAT_ANYRECV;
  struct BASILYS_OBJECT_STRUCT(3) dsym_41__DEBUG_MSG_FUN;
  struct BASILYS_STRING_STRUCT(13) dstr_42__DEBUG_MSG_FUN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_43__CLASS_NAMED;
  struct BASILYS_STRING_STRUCT(11) dstr_44__CLASS_NAMED;
  struct BASILYS_OBJECT_STRUCT(3) dsym_45__INSTALL_METHOD;
  struct BASILYS_STRING_STRUCT(14) dstr_46__INSTALL_METHOD;
  struct BASILYS_OBJECT_STRUCT(3) dsym_47__DISCR_ANYRECV;
  struct BASILYS_STRING_STRUCT(13) dstr_48__DISCR_ANYRECV;
  struct BASILYS_ROUTINE_STRUCT(4) drout_49__SCANPAT_SRCPATOR;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_50__SCANPAT_SRCPATOR;
  struct BASILYS_OBJECT_STRUCT(3) dsym_51__CLASS_SRCPATTERN_OR;
  struct BASILYS_STRING_STRUCT(19) dstr_52__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(4) drout_53__SCANPAT_SRCPATAND;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_54__SCANPAT_SRCPATAND;
  struct BASILYS_OBJECT_STRUCT(3) dsym_55__CLASS_SRCPATTERN_AND;
  struct BASILYS_STRING_STRUCT(20) dstr_56__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(3) drout_57__SCANPAT_SRCPATVAR;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_58__SCANPAT_SRCPATVAR;
  struct BASILYS_OBJECT_STRUCT(3) dsym_59__CLASS_SRCPATTERN_VARIABLE;
  struct BASILYS_STRING_STRUCT(25) dstr_60__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(3) drout_61__SCANPAT_SRCPATJOKER;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_62__SCANPAT_SRCPATJOKER;
  struct BASILYS_OBJECT_STRUCT(3) dsym_63__CLASS_SRCPATTERN_JOKERVAR;
  struct BASILYS_STRING_STRUCT(25) dstr_64__CLASS_SRCPATTERN;
  struct BASILYS_ROUTINE_STRUCT(7) drout_65__SCANPAT_SRCPATCONST;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_66__SCANPAT_SRCPATCONST;
  struct BASILYS_OBJECT_STRUCT(3) dsym_67__CLASS_SRCPATTERN_CONSTANT;
  struct BASILYS_STRING_STRUCT(25) dstr_68__CLASS_SRCPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_69__GET_CTYPE;
  struct BASILYS_STRING_STRUCT(9) dstr_70__GET_CTYPE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_71__CLASS_CTYPE;
  struct BASILYS_STRING_STRUCT(11) dstr_72__CLASS_CTYPE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_73__NORMAL_EXP;
  struct BASILYS_STRING_STRUCT(10) dstr_74__NORMAL_EXP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_75__LIST_APPEND2LIST;
  struct BASILYS_STRING_STRUCT(16) dstr_76__LIST_APPEND2LIST;
  struct BASILYS_ROUTINE_STRUCT(8) drout_77__SCANPAT_SRCPATOBJECT;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_78__SCANPAT_SRCPATOBJECT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_79__CLASS_SRCPATTERN_OBJECT;
  struct BASILYS_STRING_STRUCT(23) dstr_80__CLASS_SRCPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_81__CLASS_CLASS;
  struct BASILYS_STRING_STRUCT(11) dstr_82__CLASS_CLASS;
  struct BASILYS_OBJECT_STRUCT(3) dsym_83__CLASS_SRCFIELDPATTERN;
  struct BASILYS_STRING_STRUCT(21) dstr_84__CLASS_SRCFIELDPA;
  struct BASILYS_OBJECT_STRUCT(3) dsym_85__CLASS_FIELD;
  struct BASILYS_STRING_STRUCT(11) dstr_86__CLASS_FIELD;
  struct BASILYS_OBJECT_STRUCT(3) dsym_87__CTYPE_VALUE;
  struct BASILYS_STRING_STRUCT(11) dstr_88__CTYPE_VALUE;
  struct BASILYS_ROUTINE_STRUCT(9) drout_89__SCANPAT_SRCPATCOMPOSITE;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_90__SCANPAT_SRCPATCOMPOSITE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_91__CLASS_SRCPATTERN_COMPOSITE;
  struct BASILYS_STRING_STRUCT(26) dstr_92__CLASS_SRCPATTERN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_93__CLASS_ANY_MATCHER;
  struct BASILYS_STRING_STRUCT(17) dstr_94__CLASS_ANY_MATCHE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_95__CLASS_FORMAL_BINDING;
  struct BASILYS_STRING_STRUCT(20) dstr_96__CLASS_FORMAL_BIN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_97__NORMALIZE_TUPLE;
  struct BASILYS_STRING_STRUCT(15) dstr_98__NORMALIZE_TUPLE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_99__CLASS_NREP_BACKPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_100__CLASS_NREP_BACKP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_101__CLASS_NREP_TYPEXPR;
  struct BASILYS_STRING_STRUCT(18) dstr_102__CLASS_NREP_TYPEX;
  struct BASILYS_OBJECT_STRUCT(9) dobj_103__CLASS_NREP_BACKPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_104__CLASS_NREP_BACKP;
  struct BASILYS_MULTIPLE_STRUCT(4) dtup_105__CLASS_NREP_BACKPOINT;
  struct BASILYS_MULTIPLE_STRUCT(4) dtup_106__CLASS_NREP_BACKPOINT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_107__NBKP_FIRST;
  struct BASILYS_STRING_STRUCT(10) dstr_108__NBKP_FIRST;
  struct BASILYS_OBJECT_STRUCT(4) dobj_109__NBKP_FIRST;
  struct BASILYS_STRING_STRUCT(10) dstr_110__NBKP_FIRST;
  struct BASILYS_OBJECT_STRUCT(3) dsym_111__NBKP_ELSE;
  struct BASILYS_STRING_STRUCT(9) dstr_112__NBKP_ELSE;
  struct BASILYS_OBJECT_STRUCT(4) dobj_113__NBKP_ELSE;
  struct BASILYS_STRING_STRUCT(9) dstr_114__NBKP_ELSE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_115__CLASS_NREP_TESTPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_116__CLASS_NREP_TESTP;
  struct BASILYS_OBJECT_STRUCT(9) dobj_117__CLASS_NREP_TESTPOINT;
  struct BASILYS_STRING_STRUCT(20) dstr_118__CLASS_NREP_TESTP;
  struct BASILYS_MULTIPLE_STRUCT(4) dtup_119__CLASS_NREP_TESTPOINT;
  struct BASILYS_MULTIPLE_STRUCT(4) dtup_120__CLASS_NREP_TESTPOINT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_121__NTSP_FAILBP;
  struct BASILYS_STRING_STRUCT(11) dstr_122__NTSP_FAILBP;
  struct BASILYS_OBJECT_STRUCT(4) dobj_123__NTSP_FAILBP;
  struct BASILYS_STRING_STRUCT(11) dstr_124__NTSP_FAILBP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_125__NTSP_MATCHER;
  struct BASILYS_STRING_STRUCT(12) dstr_126__NTSP_MATCHER;
  struct BASILYS_OBJECT_STRUCT(4) dobj_127__NTSP_MATCHER;
  struct BASILYS_STRING_STRUCT(12) dstr_128__NTSP_MATCHER;
  struct BASILYS_ROUTINE_STRUCT(12) drout_129__NORMEXP_MATCH;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_130__NORMEXP_MATCH;
  struct BASILYS_OBJECT_STRUCT(3) dsym_131__CLASS_SRC_MATCH;
  struct BASILYS_STRING_STRUCT(15) dstr_132__CLASS_SRC_MATCH;
  struct BASILYS_OBJECT_STRUCT(3) dsym_133__CLASS_ENVIRONMENT;
  struct BASILYS_STRING_STRUCT(17) dstr_134__CLASS_ENVIRONMEN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_135__CLASS_NORMCONTEXT;
  struct BASILYS_STRING_STRUCT(17) dstr_136__CLASS_NORMCONTEX;
  struct BASILYS_OBJECT_STRUCT(3) dsym_137__DISCR_MULTIPLE;
  struct BASILYS_STRING_STRUCT(14) dstr_138__DISCR_MULTIPLE;
  struct BASILYS_OBJECT_STRUCT(3) dsym_139__CLASS_SRC_CASEMATCH;
  struct BASILYS_STRING_STRUCT(19) dstr_140__CLASS_SRC_CASEMA;
  struct BASILYS_OBJECT_STRUCT(3) dsym_141__DISCR_MAPOBJECTS;
  struct BASILYS_STRING_STRUCT(16) dstr_142__DISCR_MAPOBJECTS;
  struct BASILYS_OBJECT_STRUCT(3) dsym_143__DISCR_LIST;
  struct BASILYS_STRING_STRUCT(10) dstr_144__DISCR_LIST;
 long spare_;
} *cdat = NULL;
 cdat = (struct cdata_st*) basilysgc_allocate(sizeof(*cdat),0);
  basilys_prohibit_garbcoll = TRUE;
 /*initial routine predef*/
 /*initial routine fill*/
 
 /*iniobj dsym_1__CLASS_PATTERNCONTEXT*/
 if (!/*_.VALDATA___V39*/ curfptr[38]) /*_.VALDATA___V39*/ curfptr[38] = (void*)&cdat->dsym_1__CLASS_PATTERNCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dsym_1__CLASS_PATTERNCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_hash = 411470679;
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_1__CLASS_PATTERNCONTEXT));
 
 /*inistring dstr_2__CLASS_PATTERNCON*/
 /*_.VALSTR___V40*/ curfptr[39] = (void*)&cdat->dstr_2__CLASS_PATTERNCON;
  cdat->dstr_2__CLASS_PATTERNCON.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_2__CLASS_PATTERNCON.val, "CLASS_PATTERNCONTEXT");
 
 /*iniobj dsym_3__CLASS_ROOT*/
 if (!/*_.VALDATA___V41*/ curfptr[40]) /*_.VALDATA___V41*/ curfptr[40] = (void*)&cdat->dsym_3__CLASS_ROOT;
  basilys_assertmsg("iniobj checkdiscr dsym_3__CLASS_ROOT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_3__CLASS_ROOT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_3__CLASS_ROOT.obj_hash = 60568825;
  cdat->dsym_3__CLASS_ROOT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_3__CLASS_ROOT));
 
 /*inistring dstr_4__CLASS_ROOT*/
 /*_.VALSTR___V42*/ curfptr[41] = (void*)&cdat->dstr_4__CLASS_ROOT;
  cdat->dstr_4__CLASS_ROOT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_4__CLASS_ROOT.val, "CLASS_ROOT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_5__CLASS_PATTERNCONTEXT*/
 if (!/*_.VALDATA___V43*/ curfptr[42]) /*_.VALDATA___V43*/ curfptr[42] = (void*)&cdat->dobj_5__CLASS_PATTERNCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dobj_5__CLASS_PATTERNCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_num = OBMAG_OBJECT;
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_hash = 546037020;
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_5__CLASS_PATTERNCONTEXT));
 
 /*inistring dstr_6__CLASS_PATTERNCON*/
 /*_.VALSTR___V44*/ curfptr[43] = (void*)&cdat->dstr_6__CLASS_PATTERNCON;
  cdat->dstr_6__CLASS_PATTERNCON.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_6__CLASS_PATTERNCON.val, "CLASS_PATTERNCONTEXT");
 
 /*inimult dtup_7__CLASS_PATTERNCONTEXT*/
 /*_.VALTUP___V45*/ curfptr[44] = (void*)&cdat->dtup_7__CLASS_PATTERNCONTEXT;
  cdat->dtup_7__CLASS_PATTERNCONTEXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_7__CLASS_PATTERNCONTEXT.nbval = 1;
 /*inimult dtup_8__CLASS_PATTERNCONTEXT*/
 /*_.VALTUP___V58*/ curfptr[57] = (void*)&cdat->dtup_8__CLASS_PATTERNCONTEXT;
  cdat->dtup_8__CLASS_PATTERNCONTEXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_8__CLASS_PATTERNCONTEXT.nbval = 6;
 /*iniobj dsym_9__PCTN_NORMCTXT*/
 if (!/*_.VALDATA___V59*/ curfptr[58]) /*_.VALDATA___V59*/ curfptr[58] = (void*)&cdat->dsym_9__PCTN_NORMCTXT;
  basilys_assertmsg("iniobj checkdiscr dsym_9__PCTN_NORMCTXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_9__PCTN_NORMCTXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_9__PCTN_NORMCTXT.obj_hash = 955957534;
  cdat->dsym_9__PCTN_NORMCTXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_9__PCTN_NORMCTXT));
 
 /*inistring dstr_10__PCTN_NORMCTXT*/
 /*_.VALSTR___V60*/ curfptr[59] = (void*)&cdat->dstr_10__PCTN_NORMCTXT;
  cdat->dstr_10__PCTN_NORMCTXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_10__PCTN_NORMCTXT.val, "PCTN_NORMCTXT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_11__PCTN_NORMCTXT*/
 if (!/*_.VALDATA___V46*/ curfptr[45]) /*_.VALDATA___V46*/ curfptr[45] = (void*)&cdat->dobj_11__PCTN_NORMCTXT;
  basilys_assertmsg("iniobj checkdiscr dobj_11__PCTN_NORMCTXT", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_11__PCTN_NORMCTXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_11__PCTN_NORMCTXT.obj_num = 0;
  cdat->dobj_11__PCTN_NORMCTXT.obj_hash = 103577599;
  cdat->dobj_11__PCTN_NORMCTXT.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_11__PCTN_NORMCTXT));
 
 /*inistring dstr_12__PCTN_NORMCTXT*/
 /*_.VALSTR___V47*/ curfptr[46] = (void*)&cdat->dstr_12__PCTN_NORMCTXT;
  cdat->dstr_12__PCTN_NORMCTXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_12__PCTN_NORMCTXT.val, "PCTN_NORMCTXT");
 
 /*iniobj dsym_13__PCTN_SRC*/
 if (!/*_.VALDATA___V61*/ curfptr[60]) /*_.VALDATA___V61*/ curfptr[60] = (void*)&cdat->dsym_13__PCTN_SRC;
  basilys_assertmsg("iniobj checkdiscr dsym_13__PCTN_SRC", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_13__PCTN_SRC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_13__PCTN_SRC.obj_hash = 902509187;
  cdat->dsym_13__PCTN_SRC.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_13__PCTN_SRC));
 
 /*inistring dstr_14__PCTN_SRC*/
 /*_.VALSTR___V62*/ curfptr[61] = (void*)&cdat->dstr_14__PCTN_SRC;
  cdat->dstr_14__PCTN_SRC.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_14__PCTN_SRC.val, "PCTN_SRC");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_15__PCTN_SRC*/
 if (!/*_.VALDATA___V48*/ curfptr[47]) /*_.VALDATA___V48*/ curfptr[47] = (void*)&cdat->dobj_15__PCTN_SRC;
  basilys_assertmsg("iniobj checkdiscr dobj_15__PCTN_SRC", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_15__PCTN_SRC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_15__PCTN_SRC.obj_num = 1;
  cdat->dobj_15__PCTN_SRC.obj_hash = 31081828;
  cdat->dobj_15__PCTN_SRC.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_15__PCTN_SRC));
 
 /*inistring dstr_16__PCTN_SRC*/
 /*_.VALSTR___V49*/ curfptr[48] = (void*)&cdat->dstr_16__PCTN_SRC;
  cdat->dstr_16__PCTN_SRC.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_16__PCTN_SRC.val, "PCTN_SRC");
 
 /*iniobj dsym_17__PCTN_ENV*/
 if (!/*_.VALDATA___V63*/ curfptr[62]) /*_.VALDATA___V63*/ curfptr[62] = (void*)&cdat->dsym_17__PCTN_ENV;
  basilys_assertmsg("iniobj checkdiscr dsym_17__PCTN_ENV", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_17__PCTN_ENV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_17__PCTN_ENV.obj_hash = 327113752;
  cdat->dsym_17__PCTN_ENV.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_17__PCTN_ENV));
 
 /*inistring dstr_18__PCTN_ENV*/
 /*_.VALSTR___V64*/ curfptr[63] = (void*)&cdat->dstr_18__PCTN_ENV;
  cdat->dstr_18__PCTN_ENV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_18__PCTN_ENV.val, "PCTN_ENV");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_19__PCTN_ENV*/
 if (!/*_.VALDATA___V50*/ curfptr[49]) /*_.VALDATA___V50*/ curfptr[49] = (void*)&cdat->dobj_19__PCTN_ENV;
  basilys_assertmsg("iniobj checkdiscr dobj_19__PCTN_ENV", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_19__PCTN_ENV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_19__PCTN_ENV.obj_num = 2;
  cdat->dobj_19__PCTN_ENV.obj_hash = 1032914009;
  cdat->dobj_19__PCTN_ENV.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_19__PCTN_ENV));
 
 /*inistring dstr_20__PCTN_ENV*/
 /*_.VALSTR___V51*/ curfptr[50] = (void*)&cdat->dstr_20__PCTN_ENV;
  cdat->dstr_20__PCTN_ENV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_20__PCTN_ENV.val, "PCTN_ENV");
 
 /*iniobj dsym_21__PCTN_MAPATVAR*/
 if (!/*_.VALDATA___V65*/ curfptr[64]) /*_.VALDATA___V65*/ curfptr[64] = (void*)&cdat->dsym_21__PCTN_MAPATVAR;
  basilys_assertmsg("iniobj checkdiscr dsym_21__PCTN_MAPATVAR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_21__PCTN_MAPATVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_21__PCTN_MAPATVAR.obj_hash = 620129723;
  cdat->dsym_21__PCTN_MAPATVAR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_21__PCTN_MAPATVAR));
 
 /*inistring dstr_22__PCTN_MAPATVAR*/
 /*_.VALSTR___V66*/ curfptr[65] = (void*)&cdat->dstr_22__PCTN_MAPATVAR;
  cdat->dstr_22__PCTN_MAPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_22__PCTN_MAPATVAR.val, "PCTN_MAPATVAR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_23__PCTN_MAPATVAR*/
 if (!/*_.VALDATA___V52*/ curfptr[51]) /*_.VALDATA___V52*/ curfptr[51] = (void*)&cdat->dobj_23__PCTN_MAPATVAR;
  basilys_assertmsg("iniobj checkdiscr dobj_23__PCTN_MAPATVAR", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_23__PCTN_MAPATVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_23__PCTN_MAPATVAR.obj_num = 3;
  cdat->dobj_23__PCTN_MAPATVAR.obj_hash = 672037609;
  cdat->dobj_23__PCTN_MAPATVAR.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_23__PCTN_MAPATVAR));
 
 /*inistring dstr_24__PCTN_MAPATVAR*/
 /*_.VALSTR___V53*/ curfptr[52] = (void*)&cdat->dstr_24__PCTN_MAPATVAR;
  cdat->dstr_24__PCTN_MAPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_24__PCTN_MAPATVAR.val, "PCTN_MAPATVAR");
 
 /*iniobj dsym_25__PCTN_MAPATCST*/
 if (!/*_.VALDATA___V67*/ curfptr[66]) /*_.VALDATA___V67*/ curfptr[66] = (void*)&cdat->dsym_25__PCTN_MAPATCST;
  basilys_assertmsg("iniobj checkdiscr dsym_25__PCTN_MAPATCST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_25__PCTN_MAPATCST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_25__PCTN_MAPATCST.obj_hash = 626578931;
  cdat->dsym_25__PCTN_MAPATCST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_25__PCTN_MAPATCST));
 
 /*inistring dstr_26__PCTN_MAPATCST*/
 /*_.VALSTR___V68*/ curfptr[67] = (void*)&cdat->dstr_26__PCTN_MAPATCST;
  cdat->dstr_26__PCTN_MAPATCST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_26__PCTN_MAPATCST.val, "PCTN_MAPATCST");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_27__PCTN_MAPATCST*/
 if (!/*_.VALDATA___V54*/ curfptr[53]) /*_.VALDATA___V54*/ curfptr[53] = (void*)&cdat->dobj_27__PCTN_MAPATCST;
  basilys_assertmsg("iniobj checkdiscr dobj_27__PCTN_MAPATCST", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_27__PCTN_MAPATCST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_27__PCTN_MAPATCST.obj_num = 4;
  cdat->dobj_27__PCTN_MAPATCST.obj_hash = 482008671;
  cdat->dobj_27__PCTN_MAPATCST.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_27__PCTN_MAPATCST));
 
 /*inistring dstr_28__PCTN_MAPATCST*/
 /*_.VALSTR___V55*/ curfptr[54] = (void*)&cdat->dstr_28__PCTN_MAPATCST;
  cdat->dstr_28__PCTN_MAPATCST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_28__PCTN_MAPATCST.val, "PCTN_MAPATCST");
 
 /*iniobj dsym_29__PCTN_BINDLIST*/
 if (!/*_.VALDATA___V69*/ curfptr[68]) /*_.VALDATA___V69*/ curfptr[68] = (void*)&cdat->dsym_29__PCTN_BINDLIST;
  basilys_assertmsg("iniobj checkdiscr dsym_29__PCTN_BINDLIST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_29__PCTN_BINDLIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_29__PCTN_BINDLIST.obj_hash = 47093239;
  cdat->dsym_29__PCTN_BINDLIST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_29__PCTN_BINDLIST));
 
 /*inistring dstr_30__PCTN_BINDLIST*/
 /*_.VALSTR___V70*/ curfptr[69] = (void*)&cdat->dstr_30__PCTN_BINDLIST;
  cdat->dstr_30__PCTN_BINDLIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_30__PCTN_BINDLIST.val, "PCTN_BINDLIST");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_31__PCTN_BINDLIST*/
 if (!/*_.VALDATA___V56*/ curfptr[55]) /*_.VALDATA___V56*/ curfptr[55] = (void*)&cdat->dobj_31__PCTN_BINDLIST;
  basilys_assertmsg("iniobj checkdiscr dobj_31__PCTN_BINDLIST", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_31__PCTN_BINDLIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_31__PCTN_BINDLIST.obj_num = 5;
  cdat->dobj_31__PCTN_BINDLIST.obj_hash = 670326545;
  cdat->dobj_31__PCTN_BINDLIST.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_31__PCTN_BINDLIST));
 
 /*inistring dstr_32__PCTN_BINDLIST*/
 /*_.VALSTR___V57*/ curfptr[56] = (void*)&cdat->dstr_32__PCTN_BINDLIST;
  cdat->dstr_32__PCTN_BINDLIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_32__PCTN_BINDLIST.val, "PCTN_BINDLIST");
 
 /*iniobj dsym_33__SCAN_PATTERN*/
 if (!/*_.VALDATA___V71*/ curfptr[70]) /*_.VALDATA___V71*/ curfptr[70] = (void*)&cdat->dsym_33__SCAN_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dsym_33__SCAN_PATTERN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_33__SCAN_PATTERN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_33__SCAN_PATTERN.obj_hash = 97353985;
  cdat->dsym_33__SCAN_PATTERN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_33__SCAN_PATTERN));
 
 /*inistring dstr_34__SCAN_PATTERN*/
 /*_.VALSTR___V72*/ curfptr[71] = (void*)&cdat->dstr_34__SCAN_PATTERN;
  cdat->dstr_34__SCAN_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_34__SCAN_PATTERN.val, "SCAN_PATTERN");
 
 /*iniobj dsym_35__CLASS_SELECTOR*/
 if (!/*_.VALDATA___V73*/ curfptr[72]) /*_.VALDATA___V73*/ curfptr[72] = (void*)&cdat->dsym_35__CLASS_SELECTOR;
  basilys_assertmsg("iniobj checkdiscr dsym_35__CLASS_SELECTOR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_35__CLASS_SELECTOR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_35__CLASS_SELECTOR.obj_hash = 100646971;
  cdat->dsym_35__CLASS_SELECTOR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_35__CLASS_SELECTOR));
 
 /*inistring dstr_36__CLASS_SELECTOR*/
 /*_.VALSTR___V74*/ curfptr[73] = (void*)&cdat->dstr_36__CLASS_SELECTOR;
  cdat->dstr_36__CLASS_SELECTOR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_36__CLASS_SELECTOR.val, "CLASS_SELECTOR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:52:/ iniobj");
 /*iniobj dobj_37__SCAN_PATTERN*/
 if (!/*_.VALDATA___V75*/ curfptr[74]) /*_.VALDATA___V75*/ curfptr[74] = (void*)&cdat->dobj_37__SCAN_PATTERN;
  basilys_assertmsg("iniobj checkdiscr dobj_37__SCAN_PATTERN", NULL != (void*)/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_37__SCAN_PATTERN.obj_class = (basilysobject_ptr_t)(/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_37__SCAN_PATTERN.obj_hash = 6449164;
  cdat->dobj_37__SCAN_PATTERN.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_37__SCAN_PATTERN));
 
 /*inistring dstr_38__SCAN_PATTERN*/
 /*_.VALSTR___V76*/ curfptr[75] = (void*)&cdat->dstr_38__SCAN_PATTERN;
  cdat->dstr_38__SCAN_PATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_38__SCAN_PATTERN.val, "SCAN_PATTERN");
 
 /*inirout drout_39__SCANPAT_ANYRECV*/
 /*_.VALROUT___V77*/ curfptr[76] = (void*)&cdat->drout_39__SCANPAT_ANYRECV;
  cdat->drout_39__SCANPAT_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_39__SCANPAT_ANYRECV.routdescr, "SCANPAT_ANYRECV @warmelt-normatch.bysl:58",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_39__SCANPAT_ANYRECV.nbval = 3;
 *(basilysroutfun_t **) (cdat->drout_39__SCANPAT_ANYRECV.routaddr) = rout_1_SCANPAT_ANYRECV;
 
 /*iniclos dclo_40__SCANPAT_ANYRECV*/
 /*_.VALCLO___V78*/ curfptr[77] = (void*)&cdat->dclo_40__SCANPAT_ANYRECV;
  cdat->dclo_40__SCANPAT_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_40__SCANPAT_ANYRECV.nbval = 0;
 
 /*iniobj dsym_41__DEBUG_MSG_FUN*/
 if (!/*_.VALDATA___V79*/ curfptr[78]) /*_.VALDATA___V79*/ curfptr[78] = (void*)&cdat->dsym_41__DEBUG_MSG_FUN;
  basilys_assertmsg("iniobj checkdiscr dsym_41__DEBUG_MSG_FUN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_41__DEBUG_MSG_FUN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_41__DEBUG_MSG_FUN.obj_hash = 938829072;
  cdat->dsym_41__DEBUG_MSG_FUN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_41__DEBUG_MSG_FUN));
 
 /*inistring dstr_42__DEBUG_MSG_FUN*/
 /*_.VALSTR___V80*/ curfptr[79] = (void*)&cdat->dstr_42__DEBUG_MSG_FUN;
  cdat->dstr_42__DEBUG_MSG_FUN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_42__DEBUG_MSG_FUN.val, "DEBUG_MSG_FUN");
 
 /*iniobj dsym_43__CLASS_NAMED*/
 if (!/*_.VALDATA___V81*/ curfptr[80]) /*_.VALDATA___V81*/ curfptr[80] = (void*)&cdat->dsym_43__CLASS_NAMED;
  basilys_assertmsg("iniobj checkdiscr dsym_43__CLASS_NAMED", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_43__CLASS_NAMED.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_43__CLASS_NAMED.obj_hash = 60407004;
  cdat->dsym_43__CLASS_NAMED.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_43__CLASS_NAMED));
 
 /*inistring dstr_44__CLASS_NAMED*/
 /*_.VALSTR___V82*/ curfptr[81] = (void*)&cdat->dstr_44__CLASS_NAMED;
  cdat->dstr_44__CLASS_NAMED.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_44__CLASS_NAMED.val, "CLASS_NAMED");
 
 /*iniobj dsym_45__INSTALL_METHOD*/
 if (!/*_.VALDATA___V83*/ curfptr[82]) /*_.VALDATA___V83*/ curfptr[82] = (void*)&cdat->dsym_45__INSTALL_METHOD;
  basilys_assertmsg("iniobj checkdiscr dsym_45__INSTALL_METHOD", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_45__INSTALL_METHOD.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_45__INSTALL_METHOD.obj_hash = 680699224;
  cdat->dsym_45__INSTALL_METHOD.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_45__INSTALL_METHOD));
 
 /*inistring dstr_46__INSTALL_METHOD*/
 /*_.VALSTR___V84*/ curfptr[83] = (void*)&cdat->dstr_46__INSTALL_METHOD;
  cdat->dstr_46__INSTALL_METHOD.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_46__INSTALL_METHOD.val, "INSTALL_METHOD");
 
 /*iniobj dsym_47__DISCR_ANYRECV*/
 if (!/*_.VALDATA___V85*/ curfptr[84]) /*_.VALDATA___V85*/ curfptr[84] = (void*)&cdat->dsym_47__DISCR_ANYRECV;
  basilys_assertmsg("iniobj checkdiscr dsym_47__DISCR_ANYRECV", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_47__DISCR_ANYRECV.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_47__DISCR_ANYRECV.obj_hash = 88765237;
  cdat->dsym_47__DISCR_ANYRECV.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_47__DISCR_ANYRECV));
 
 /*inistring dstr_48__DISCR_ANYRECV*/
 /*_.VALSTR___V86*/ curfptr[85] = (void*)&cdat->dstr_48__DISCR_ANYRECV;
  cdat->dstr_48__DISCR_ANYRECV.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_48__DISCR_ANYRECV.val, "DISCR_ANYRECV");
 
 /*inirout drout_49__SCANPAT_SRCPATOR*/
 /*_.VALROUT___V87*/ curfptr[86] = (void*)&cdat->drout_49__SCANPAT_SRCPATOR;
  cdat->drout_49__SCANPAT_SRCPATOR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_49__SCANPAT_SRCPATOR.routdescr, "SCANPAT_SRCPATOR @warmelt-normatch.bysl:72",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_49__SCANPAT_SRCPATOR.nbval = 4;
 *(basilysroutfun_t **) (cdat->drout_49__SCANPAT_SRCPATOR.routaddr) = rout_2_SCANPAT_SRCPATOR;
 
 /*iniclos dclo_50__SCANPAT_SRCPATOR*/
 /*_.VALCLO___V88*/ curfptr[87] = (void*)&cdat->dclo_50__SCANPAT_SRCPATOR;
  cdat->dclo_50__SCANPAT_SRCPATOR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_50__SCANPAT_SRCPATOR.nbval = 0;
 
 /*iniobj dsym_51__CLASS_SRCPATTERN_OR*/
 if (!/*_.VALDATA___V89*/ curfptr[88]) /*_.VALDATA___V89*/ curfptr[88] = (void*)&cdat->dsym_51__CLASS_SRCPATTERN_OR;
  basilys_assertmsg("iniobj checkdiscr dsym_51__CLASS_SRCPATTERN_OR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_51__CLASS_SRCPATTERN_OR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_51__CLASS_SRCPATTERN_OR.obj_hash = 920875001;
  cdat->dsym_51__CLASS_SRCPATTERN_OR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_51__CLASS_SRCPATTERN_OR));
 
 /*inistring dstr_52__CLASS_SRCPATTERN*/
 /*_.VALSTR___V90*/ curfptr[89] = (void*)&cdat->dstr_52__CLASS_SRCPATTERN;
  cdat->dstr_52__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_52__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_OR");
 
 /*inirout drout_53__SCANPAT_SRCPATAND*/
 /*_.VALROUT___V91*/ curfptr[90] = (void*)&cdat->drout_53__SCANPAT_SRCPATAND;
  cdat->drout_53__SCANPAT_SRCPATAND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_53__SCANPAT_SRCPATAND.routdescr, "SCANPAT_SRCPATAND @warmelt-normatch.bysl:91",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_53__SCANPAT_SRCPATAND.nbval = 4;
 *(basilysroutfun_t **) (cdat->drout_53__SCANPAT_SRCPATAND.routaddr) = rout_3_SCANPAT_SRCPATAND;
 
 /*iniclos dclo_54__SCANPAT_SRCPATAND*/
 /*_.VALCLO___V92*/ curfptr[91] = (void*)&cdat->dclo_54__SCANPAT_SRCPATAND;
  cdat->dclo_54__SCANPAT_SRCPATAND.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_54__SCANPAT_SRCPATAND.nbval = 0;
 
 /*iniobj dsym_55__CLASS_SRCPATTERN_AND*/
 if (!/*_.VALDATA___V93*/ curfptr[92]) /*_.VALDATA___V93*/ curfptr[92] = (void*)&cdat->dsym_55__CLASS_SRCPATTERN_AND;
  basilys_assertmsg("iniobj checkdiscr dsym_55__CLASS_SRCPATTERN_AND", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_55__CLASS_SRCPATTERN_AND.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_55__CLASS_SRCPATTERN_AND.obj_hash = 181310597;
  cdat->dsym_55__CLASS_SRCPATTERN_AND.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_55__CLASS_SRCPATTERN_AND));
 
 /*inistring dstr_56__CLASS_SRCPATTERN*/
 /*_.VALSTR___V94*/ curfptr[93] = (void*)&cdat->dstr_56__CLASS_SRCPATTERN;
  cdat->dstr_56__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_56__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_AND");
 
 /*inirout drout_57__SCANPAT_SRCPATVAR*/
 /*_.VALROUT___V95*/ curfptr[94] = (void*)&cdat->drout_57__SCANPAT_SRCPATVAR;
  cdat->drout_57__SCANPAT_SRCPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_57__SCANPAT_SRCPATVAR.routdescr, "SCANPAT_SRCPATVAR @warmelt-normatch.bysl:110",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_57__SCANPAT_SRCPATVAR.nbval = 3;
 *(basilysroutfun_t **) (cdat->drout_57__SCANPAT_SRCPATVAR.routaddr) = rout_4_SCANPAT_SRCPATVAR;
 
 /*iniclos dclo_58__SCANPAT_SRCPATVAR*/
 /*_.VALCLO___V96*/ curfptr[95] = (void*)&cdat->dclo_58__SCANPAT_SRCPATVAR;
  cdat->dclo_58__SCANPAT_SRCPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_58__SCANPAT_SRCPATVAR.nbval = 0;
 
 /*iniobj dsym_59__CLASS_SRCPATTERN_VARIABLE*/
 if (!/*_.VALDATA___V97*/ curfptr[96]) /*_.VALDATA___V97*/ curfptr[96] = (void*)&cdat->dsym_59__CLASS_SRCPATTERN_VARIABLE;
  basilys_assertmsg("iniobj checkdiscr dsym_59__CLASS_SRCPATTERN_VARIABLE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_59__CLASS_SRCPATTERN_VARIABLE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_59__CLASS_SRCPATTERN_VARIABLE.obj_hash = 280604106;
  cdat->dsym_59__CLASS_SRCPATTERN_VARIABLE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_59__CLASS_SRCPATTERN_VARIABLE));
 
 /*inistring dstr_60__CLASS_SRCPATTERN*/
 /*_.VALSTR___V98*/ curfptr[97] = (void*)&cdat->dstr_60__CLASS_SRCPATTERN;
  cdat->dstr_60__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_60__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_VARIABLE");
 
 /*inirout drout_61__SCANPAT_SRCPATJOKER*/
 /*_.VALROUT___V99*/ curfptr[98] = (void*)&cdat->drout_61__SCANPAT_SRCPATJOKER;
  cdat->drout_61__SCANPAT_SRCPATJOKER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_61__SCANPAT_SRCPATJOKER.routdescr, "SCANPAT_SRCPATJOKER @warmelt-normatch.bysl:134",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_61__SCANPAT_SRCPATJOKER.nbval = 3;
 *(basilysroutfun_t **) (cdat->drout_61__SCANPAT_SRCPATJOKER.routaddr) = rout_5_SCANPAT_SRCPATJOKER;
 
 /*iniclos dclo_62__SCANPAT_SRCPATJOKER*/
 /*_.VALCLO___V100*/ curfptr[99] = (void*)&cdat->dclo_62__SCANPAT_SRCPATJOKER;
  cdat->dclo_62__SCANPAT_SRCPATJOKER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_62__SCANPAT_SRCPATJOKER.nbval = 0;
 
 /*iniobj dsym_63__CLASS_SRCPATTERN_JOKERVAR*/
 if (!/*_.VALDATA___V101*/ curfptr[100]) /*_.VALDATA___V101*/ curfptr[100] = (void*)&cdat->dsym_63__CLASS_SRCPATTERN_JOKERVAR;
  basilys_assertmsg("iniobj checkdiscr dsym_63__CLASS_SRCPATTERN_JOKERVAR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_63__CLASS_SRCPATTERN_JOKERVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_63__CLASS_SRCPATTERN_JOKERVAR.obj_hash = 233170336;
  cdat->dsym_63__CLASS_SRCPATTERN_JOKERVAR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_63__CLASS_SRCPATTERN_JOKERVAR));
 
 /*inistring dstr_64__CLASS_SRCPATTERN*/
 /*_.VALSTR___V102*/ curfptr[101] = (void*)&cdat->dstr_64__CLASS_SRCPATTERN;
  cdat->dstr_64__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_64__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_JOKERVAR");
 
 /*inirout drout_65__SCANPAT_SRCPATCONST*/
 /*_.VALROUT___V103*/ curfptr[102] = (void*)&cdat->drout_65__SCANPAT_SRCPATCONST;
  cdat->drout_65__SCANPAT_SRCPATCONST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_65__SCANPAT_SRCPATCONST.routdescr, "SCANPAT_SRCPATCONST @warmelt-normatch.bysl:147",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_65__SCANPAT_SRCPATCONST.nbval = 7;
 *(basilysroutfun_t **) (cdat->drout_65__SCANPAT_SRCPATCONST.routaddr) = rout_6_SCANPAT_SRCPATCONST;
 
 /*iniclos dclo_66__SCANPAT_SRCPATCONST*/
 /*_.VALCLO___V104*/ curfptr[103] = (void*)&cdat->dclo_66__SCANPAT_SRCPATCONST;
  cdat->dclo_66__SCANPAT_SRCPATCONST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_66__SCANPAT_SRCPATCONST.nbval = 0;
 
 /*iniobj dsym_67__CLASS_SRCPATTERN_CONSTANT*/
 if (!/*_.VALDATA___V105*/ curfptr[104]) /*_.VALDATA___V105*/ curfptr[104] = (void*)&cdat->dsym_67__CLASS_SRCPATTERN_CONSTANT;
  basilys_assertmsg("iniobj checkdiscr dsym_67__CLASS_SRCPATTERN_CONSTANT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_67__CLASS_SRCPATTERN_CONSTANT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_67__CLASS_SRCPATTERN_CONSTANT.obj_hash = 762423769;
  cdat->dsym_67__CLASS_SRCPATTERN_CONSTANT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_67__CLASS_SRCPATTERN_CONSTANT));
 
 /*inistring dstr_68__CLASS_SRCPATTERN*/
 /*_.VALSTR___V106*/ curfptr[105] = (void*)&cdat->dstr_68__CLASS_SRCPATTERN;
  cdat->dstr_68__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_68__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_CONSTANT");
 
 /*iniobj dsym_69__GET_CTYPE*/
 if (!/*_.VALDATA___V107*/ curfptr[106]) /*_.VALDATA___V107*/ curfptr[106] = (void*)&cdat->dsym_69__GET_CTYPE;
  basilys_assertmsg("iniobj checkdiscr dsym_69__GET_CTYPE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_69__GET_CTYPE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_69__GET_CTYPE.obj_hash = 747649571;
  cdat->dsym_69__GET_CTYPE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_69__GET_CTYPE));
 
 /*inistring dstr_70__GET_CTYPE*/
 /*_.VALSTR___V108*/ curfptr[107] = (void*)&cdat->dstr_70__GET_CTYPE;
  cdat->dstr_70__GET_CTYPE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_70__GET_CTYPE.val, "GET_CTYPE");
 
 /*iniobj dsym_71__CLASS_CTYPE*/
 if (!/*_.VALDATA___V109*/ curfptr[108]) /*_.VALDATA___V109*/ curfptr[108] = (void*)&cdat->dsym_71__CLASS_CTYPE;
  basilys_assertmsg("iniobj checkdiscr dsym_71__CLASS_CTYPE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_71__CLASS_CTYPE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_71__CLASS_CTYPE.obj_hash = 78610803;
  cdat->dsym_71__CLASS_CTYPE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_71__CLASS_CTYPE));
 
 /*inistring dstr_72__CLASS_CTYPE*/
 /*_.VALSTR___V110*/ curfptr[109] = (void*)&cdat->dstr_72__CLASS_CTYPE;
  cdat->dstr_72__CLASS_CTYPE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_72__CLASS_CTYPE.val, "CLASS_CTYPE");
 
 /*iniobj dsym_73__NORMAL_EXP*/
 if (!/*_.VALDATA___V111*/ curfptr[110]) /*_.VALDATA___V111*/ curfptr[110] = (void*)&cdat->dsym_73__NORMAL_EXP;
  basilys_assertmsg("iniobj checkdiscr dsym_73__NORMAL_EXP", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_73__NORMAL_EXP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_73__NORMAL_EXP.obj_hash = 420282450;
  cdat->dsym_73__NORMAL_EXP.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_73__NORMAL_EXP));
 
 /*inistring dstr_74__NORMAL_EXP*/
 /*_.VALSTR___V112*/ curfptr[111] = (void*)&cdat->dstr_74__NORMAL_EXP;
  cdat->dstr_74__NORMAL_EXP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_74__NORMAL_EXP.val, "NORMAL_EXP");
 
 /*iniobj dsym_75__LIST_APPEND2LIST*/
 if (!/*_.VALDATA___V113*/ curfptr[112]) /*_.VALDATA___V113*/ curfptr[112] = (void*)&cdat->dsym_75__LIST_APPEND2LIST;
  basilys_assertmsg("iniobj checkdiscr dsym_75__LIST_APPEND2LIST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_75__LIST_APPEND2LIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_75__LIST_APPEND2LIST.obj_hash = 744841630;
  cdat->dsym_75__LIST_APPEND2LIST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_75__LIST_APPEND2LIST));
 
 /*inistring dstr_76__LIST_APPEND2LIST*/
 /*_.VALSTR___V114*/ curfptr[113] = (void*)&cdat->dstr_76__LIST_APPEND2LIST;
  cdat->dstr_76__LIST_APPEND2LIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_76__LIST_APPEND2LIST.val, "LIST_APPEND2LIST");
 
 /*inirout drout_77__SCANPAT_SRCPATOBJECT*/
 /*_.VALROUT___V115*/ curfptr[114] = (void*)&cdat->drout_77__SCANPAT_SRCPATOBJECT;
  cdat->drout_77__SCANPAT_SRCPATOBJECT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_77__SCANPAT_SRCPATOBJECT.routdescr, "SCANPAT_SRCPATOBJECT @warmelt-normatch.bysl:192",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_77__SCANPAT_SRCPATOBJECT.nbval = 8;
 *(basilysroutfun_t **) (cdat->drout_77__SCANPAT_SRCPATOBJECT.routaddr) = rout_7_SCANPAT_SRCPATOBJECT;
 
 /*iniclos dclo_78__SCANPAT_SRCPATOBJECT*/
 /*_.VALCLO___V116*/ curfptr[115] = (void*)&cdat->dclo_78__SCANPAT_SRCPATOBJECT;
  cdat->dclo_78__SCANPAT_SRCPATOBJECT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_78__SCANPAT_SRCPATOBJECT.nbval = 0;
 
 /*iniobj dsym_79__CLASS_SRCPATTERN_OBJECT*/
 if (!/*_.VALDATA___V117*/ curfptr[116]) /*_.VALDATA___V117*/ curfptr[116] = (void*)&cdat->dsym_79__CLASS_SRCPATTERN_OBJECT;
  basilys_assertmsg("iniobj checkdiscr dsym_79__CLASS_SRCPATTERN_OBJECT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_79__CLASS_SRCPATTERN_OBJECT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_79__CLASS_SRCPATTERN_OBJECT.obj_hash = 666808646;
  cdat->dsym_79__CLASS_SRCPATTERN_OBJECT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_79__CLASS_SRCPATTERN_OBJECT));
 
 /*inistring dstr_80__CLASS_SRCPATTERN*/
 /*_.VALSTR___V118*/ curfptr[117] = (void*)&cdat->dstr_80__CLASS_SRCPATTERN;
  cdat->dstr_80__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_80__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_OBJECT");
 
 /*iniobj dsym_81__CLASS_CLASS*/
 if (!/*_.VALDATA___V119*/ curfptr[118]) /*_.VALDATA___V119*/ curfptr[118] = (void*)&cdat->dsym_81__CLASS_CLASS;
  basilys_assertmsg("iniobj checkdiscr dsym_81__CLASS_CLASS", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_81__CLASS_CLASS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_81__CLASS_CLASS.obj_hash = 60141292;
  cdat->dsym_81__CLASS_CLASS.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_81__CLASS_CLASS));
 
 /*inistring dstr_82__CLASS_CLASS*/
 /*_.VALSTR___V120*/ curfptr[119] = (void*)&cdat->dstr_82__CLASS_CLASS;
  cdat->dstr_82__CLASS_CLASS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_82__CLASS_CLASS.val, "CLASS_CLASS");
 
 /*iniobj dsym_83__CLASS_SRCFIELDPATTERN*/
 if (!/*_.VALDATA___V121*/ curfptr[120]) /*_.VALDATA___V121*/ curfptr[120] = (void*)&cdat->dsym_83__CLASS_SRCFIELDPATTERN;
  basilys_assertmsg("iniobj checkdiscr dsym_83__CLASS_SRCFIELDPATTERN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_83__CLASS_SRCFIELDPATTERN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_83__CLASS_SRCFIELDPATTERN.obj_hash = 780247789;
  cdat->dsym_83__CLASS_SRCFIELDPATTERN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_83__CLASS_SRCFIELDPATTERN));
 
 /*inistring dstr_84__CLASS_SRCFIELDPA*/
 /*_.VALSTR___V122*/ curfptr[121] = (void*)&cdat->dstr_84__CLASS_SRCFIELDPA;
  cdat->dstr_84__CLASS_SRCFIELDPA.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_84__CLASS_SRCFIELDPA.val, "CLASS_SRCFIELDPATTERN");
 
 /*iniobj dsym_85__CLASS_FIELD*/
 if (!/*_.VALDATA___V123*/ curfptr[122]) /*_.VALDATA___V123*/ curfptr[122] = (void*)&cdat->dsym_85__CLASS_FIELD;
  basilys_assertmsg("iniobj checkdiscr dsym_85__CLASS_FIELD", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_85__CLASS_FIELD.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_85__CLASS_FIELD.obj_hash = 523073;
  cdat->dsym_85__CLASS_FIELD.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_85__CLASS_FIELD));
 
 /*inistring dstr_86__CLASS_FIELD*/
 /*_.VALSTR___V124*/ curfptr[123] = (void*)&cdat->dstr_86__CLASS_FIELD;
  cdat->dstr_86__CLASS_FIELD.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_86__CLASS_FIELD.val, "CLASS_FIELD");
 
 /*iniobj dsym_87__CTYPE_VALUE*/
 if (!/*_.VALDATA___V125*/ curfptr[124]) /*_.VALDATA___V125*/ curfptr[124] = (void*)&cdat->dsym_87__CTYPE_VALUE;
  basilys_assertmsg("iniobj checkdiscr dsym_87__CTYPE_VALUE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_87__CTYPE_VALUE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_87__CTYPE_VALUE.obj_hash = 11062864;
  cdat->dsym_87__CTYPE_VALUE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_87__CTYPE_VALUE));
 
 /*inistring dstr_88__CTYPE_VALUE*/
 /*_.VALSTR___V126*/ curfptr[125] = (void*)&cdat->dstr_88__CTYPE_VALUE;
  cdat->dstr_88__CTYPE_VALUE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_88__CTYPE_VALUE.val, "CTYPE_VALUE");
 
 /*inirout drout_89__SCANPAT_SRCPATCOMPOSITE*/
 /*_.VALROUT___V127*/ curfptr[126] = (void*)&cdat->drout_89__SCANPAT_SRCPATCOMPOSITE;
  cdat->drout_89__SCANPAT_SRCPATCOMPOSITE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_89__SCANPAT_SRCPATCOMPOSITE.routdescr, "SCANPAT_SRCPATCOMPOSITE @warmelt-normatch.bysl:227",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_89__SCANPAT_SRCPATCOMPOSITE.nbval = 9;
 *(basilysroutfun_t **) (cdat->drout_89__SCANPAT_SRCPATCOMPOSITE.routaddr) = rout_8_SCANPAT_SRCPATCOMPOSITE;
 
 /*iniclos dclo_90__SCANPAT_SRCPATCOMPOSITE*/
 /*_.VALCLO___V128*/ curfptr[127] = (void*)&cdat->dclo_90__SCANPAT_SRCPATCOMPOSITE;
  cdat->dclo_90__SCANPAT_SRCPATCOMPOSITE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_90__SCANPAT_SRCPATCOMPOSITE.nbval = 0;
 
 /*iniobj dsym_91__CLASS_SRCPATTERN_COMPOSITE*/
 if (!/*_.VALDATA___V129*/ curfptr[128]) /*_.VALDATA___V129*/ curfptr[128] = (void*)&cdat->dsym_91__CLASS_SRCPATTERN_COMPOSITE;
  basilys_assertmsg("iniobj checkdiscr dsym_91__CLASS_SRCPATTERN_COMPOSITE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_91__CLASS_SRCPATTERN_COMPOSITE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_91__CLASS_SRCPATTERN_COMPOSITE.obj_hash = 228072300;
  cdat->dsym_91__CLASS_SRCPATTERN_COMPOSITE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_91__CLASS_SRCPATTERN_COMPOSITE));
 
 /*inistring dstr_92__CLASS_SRCPATTERN*/
 /*_.VALSTR___V130*/ curfptr[129] = (void*)&cdat->dstr_92__CLASS_SRCPATTERN;
  cdat->dstr_92__CLASS_SRCPATTERN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_92__CLASS_SRCPATTERN.val, "CLASS_SRCPATTERN_COMPOSITE");
 
 /*iniobj dsym_93__CLASS_ANY_MATCHER*/
 if (!/*_.VALDATA___V131*/ curfptr[130]) /*_.VALDATA___V131*/ curfptr[130] = (void*)&cdat->dsym_93__CLASS_ANY_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dsym_93__CLASS_ANY_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_93__CLASS_ANY_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_93__CLASS_ANY_MATCHER.obj_hash = 781265508;
  cdat->dsym_93__CLASS_ANY_MATCHER.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_93__CLASS_ANY_MATCHER));
 
 /*inistring dstr_94__CLASS_ANY_MATCHE*/
 /*_.VALSTR___V132*/ curfptr[131] = (void*)&cdat->dstr_94__CLASS_ANY_MATCHE;
  cdat->dstr_94__CLASS_ANY_MATCHE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_94__CLASS_ANY_MATCHE.val, "CLASS_ANY_MATCHER");
 
 /*iniobj dsym_95__CLASS_FORMAL_BINDING*/
 if (!/*_.VALDATA___V133*/ curfptr[132]) /*_.VALDATA___V133*/ curfptr[132] = (void*)&cdat->dsym_95__CLASS_FORMAL_BINDING;
  basilys_assertmsg("iniobj checkdiscr dsym_95__CLASS_FORMAL_BINDING", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_95__CLASS_FORMAL_BINDING.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_95__CLASS_FORMAL_BINDING.obj_hash = 3733780;
  cdat->dsym_95__CLASS_FORMAL_BINDING.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_95__CLASS_FORMAL_BINDING));
 
 /*inistring dstr_96__CLASS_FORMAL_BIN*/
 /*_.VALSTR___V134*/ curfptr[133] = (void*)&cdat->dstr_96__CLASS_FORMAL_BIN;
  cdat->dstr_96__CLASS_FORMAL_BIN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_96__CLASS_FORMAL_BIN.val, "CLASS_FORMAL_BINDING");
 
 /*iniobj dsym_97__NORMALIZE_TUPLE*/
 if (!/*_.VALDATA___V135*/ curfptr[134]) /*_.VALDATA___V135*/ curfptr[134] = (void*)&cdat->dsym_97__NORMALIZE_TUPLE;
  basilys_assertmsg("iniobj checkdiscr dsym_97__NORMALIZE_TUPLE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_97__NORMALIZE_TUPLE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_97__NORMALIZE_TUPLE.obj_hash = 805824045;
  cdat->dsym_97__NORMALIZE_TUPLE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_97__NORMALIZE_TUPLE));
 
 /*inistring dstr_98__NORMALIZE_TUPLE*/
 /*_.VALSTR___V136*/ curfptr[135] = (void*)&cdat->dstr_98__NORMALIZE_TUPLE;
  cdat->dstr_98__NORMALIZE_TUPLE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_98__NORMALIZE_TUPLE.val, "NORMALIZE_TUPLE");
 
 /*iniobj dsym_99__CLASS_NREP_BACKPOINT*/
 if (!/*_.VALDATA___V137*/ curfptr[136]) /*_.VALDATA___V137*/ curfptr[136] = (void*)&cdat->dsym_99__CLASS_NREP_BACKPOINT;
  basilys_assertmsg("iniobj checkdiscr dsym_99__CLASS_NREP_BACKPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_99__CLASS_NREP_BACKPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_99__CLASS_NREP_BACKPOINT.obj_hash = 287839181;
  cdat->dsym_99__CLASS_NREP_BACKPOINT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_99__CLASS_NREP_BACKPOINT));
 
 /*inistring dstr_100__CLASS_NREP_BACKP*/
 /*_.VALSTR___V138*/ curfptr[137] = (void*)&cdat->dstr_100__CLASS_NREP_BACKP;
  cdat->dstr_100__CLASS_NREP_BACKP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_100__CLASS_NREP_BACKP.val, "CLASS_NREP_BACKPOINT");
 
 /*iniobj dsym_101__CLASS_NREP_TYPEXPR*/
 if (!/*_.VALDATA___V139*/ curfptr[138]) /*_.VALDATA___V139*/ curfptr[138] = (void*)&cdat->dsym_101__CLASS_NREP_TYPEXPR;
  basilys_assertmsg("iniobj checkdiscr dsym_101__CLASS_NREP_TYPEXPR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_101__CLASS_NREP_TYPEXPR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_101__CLASS_NREP_TYPEXPR.obj_hash = 874739025;
  cdat->dsym_101__CLASS_NREP_TYPEXPR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_101__CLASS_NREP_TYPEXPR));
 
 /*inistring dstr_102__CLASS_NREP_TYPEX*/
 /*_.VALSTR___V140*/ curfptr[139] = (void*)&cdat->dstr_102__CLASS_NREP_TYPEX;
  cdat->dstr_102__CLASS_NREP_TYPEX.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_102__CLASS_NREP_TYPEX.val, "CLASS_NREP_TYPEXPR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ iniobj");
 /*iniobj dobj_103__CLASS_NREP_BACKPOINT*/
 if (!/*_.VALDATA___V141*/ curfptr[140]) /*_.VALDATA___V141*/ curfptr[140] = (void*)&cdat->dobj_103__CLASS_NREP_BACKPOINT;
  basilys_assertmsg("iniobj checkdiscr dobj_103__CLASS_NREP_BACKPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_103__CLASS_NREP_BACKPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_103__CLASS_NREP_BACKPOINT.obj_num = OBMAG_OBJECT;
  cdat->dobj_103__CLASS_NREP_BACKPOINT.obj_hash = 179190460;
  cdat->dobj_103__CLASS_NREP_BACKPOINT.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_103__CLASS_NREP_BACKPOINT));
 
 /*inistring dstr_104__CLASS_NREP_BACKP*/
 /*_.VALSTR___V142*/ curfptr[141] = (void*)&cdat->dstr_104__CLASS_NREP_BACKP;
  cdat->dstr_104__CLASS_NREP_BACKP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_104__CLASS_NREP_BACKP.val, "CLASS_NREP_BACKPOINT");
 
 /*inimult dtup_105__CLASS_NREP_BACKPOINT*/
 /*_.VALTUP___V143*/ curfptr[142] = (void*)&cdat->dtup_105__CLASS_NREP_BACKPOINT;
  cdat->dtup_105__CLASS_NREP_BACKPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_105__CLASS_NREP_BACKPOINT.nbval = 4;
 /*inimult dtup_106__CLASS_NREP_BACKPOINT*/
 /*_.VALTUP___V148*/ curfptr[147] = (void*)&cdat->dtup_106__CLASS_NREP_BACKPOINT;
  cdat->dtup_106__CLASS_NREP_BACKPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_106__CLASS_NREP_BACKPOINT.nbval = 4;
 /*iniobj dsym_107__NBKP_FIRST*/
 if (!/*_.VALDATA___V149*/ curfptr[148]) /*_.VALDATA___V149*/ curfptr[148] = (void*)&cdat->dsym_107__NBKP_FIRST;
  basilys_assertmsg("iniobj checkdiscr dsym_107__NBKP_FIRST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_107__NBKP_FIRST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_107__NBKP_FIRST.obj_hash = 190350631;
  cdat->dsym_107__NBKP_FIRST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_107__NBKP_FIRST));
 
 /*inistring dstr_108__NBKP_FIRST*/
 /*_.VALSTR___V150*/ curfptr[149] = (void*)&cdat->dstr_108__NBKP_FIRST;
  cdat->dstr_108__NBKP_FIRST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_108__NBKP_FIRST.val, "NBKP_FIRST");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ iniobj");
 /*iniobj dobj_109__NBKP_FIRST*/
 if (!/*_.VALDATA___V144*/ curfptr[143]) /*_.VALDATA___V144*/ curfptr[143] = (void*)&cdat->dobj_109__NBKP_FIRST;
  basilys_assertmsg("iniobj checkdiscr dobj_109__NBKP_FIRST", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_109__NBKP_FIRST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_109__NBKP_FIRST.obj_num = 2;
  cdat->dobj_109__NBKP_FIRST.obj_hash = 163070548;
  cdat->dobj_109__NBKP_FIRST.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_109__NBKP_FIRST));
 
 /*inistring dstr_110__NBKP_FIRST*/
 /*_.VALSTR___V145*/ curfptr[144] = (void*)&cdat->dstr_110__NBKP_FIRST;
  cdat->dstr_110__NBKP_FIRST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_110__NBKP_FIRST.val, "NBKP_FIRST");
 
 /*iniobj dsym_111__NBKP_ELSE*/
 if (!/*_.VALDATA___V151*/ curfptr[150]) /*_.VALDATA___V151*/ curfptr[150] = (void*)&cdat->dsym_111__NBKP_ELSE;
  basilys_assertmsg("iniobj checkdiscr dsym_111__NBKP_ELSE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_111__NBKP_ELSE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_111__NBKP_ELSE.obj_hash = 990781148;
  cdat->dsym_111__NBKP_ELSE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_111__NBKP_ELSE));
 
 /*inistring dstr_112__NBKP_ELSE*/
 /*_.VALSTR___V152*/ curfptr[151] = (void*)&cdat->dstr_112__NBKP_ELSE;
  cdat->dstr_112__NBKP_ELSE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_112__NBKP_ELSE.val, "NBKP_ELSE");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ iniobj");
 /*iniobj dobj_113__NBKP_ELSE*/
 if (!/*_.VALDATA___V146*/ curfptr[145]) /*_.VALDATA___V146*/ curfptr[145] = (void*)&cdat->dobj_113__NBKP_ELSE;
  basilys_assertmsg("iniobj checkdiscr dobj_113__NBKP_ELSE", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_113__NBKP_ELSE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_113__NBKP_ELSE.obj_num = 3;
  cdat->dobj_113__NBKP_ELSE.obj_hash = 970363355;
  cdat->dobj_113__NBKP_ELSE.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_113__NBKP_ELSE));
 
 /*inistring dstr_114__NBKP_ELSE*/
 /*_.VALSTR___V147*/ curfptr[146] = (void*)&cdat->dstr_114__NBKP_ELSE;
  cdat->dstr_114__NBKP_ELSE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_114__NBKP_ELSE.val, "NBKP_ELSE");
 
 /*iniobj dsym_115__CLASS_NREP_TESTPOINT*/
 if (!/*_.VALDATA___V153*/ curfptr[152]) /*_.VALDATA___V153*/ curfptr[152] = (void*)&cdat->dsym_115__CLASS_NREP_TESTPOINT;
  basilys_assertmsg("iniobj checkdiscr dsym_115__CLASS_NREP_TESTPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_115__CLASS_NREP_TESTPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_115__CLASS_NREP_TESTPOINT.obj_hash = 207132463;
  cdat->dsym_115__CLASS_NREP_TESTPOINT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_115__CLASS_NREP_TESTPOINT));
 
 /*inistring dstr_116__CLASS_NREP_TESTP*/
 /*_.VALSTR___V154*/ curfptr[153] = (void*)&cdat->dstr_116__CLASS_NREP_TESTP;
  cdat->dstr_116__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_116__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTPOINT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ iniobj");
 /*iniobj dobj_117__CLASS_NREP_TESTPOINT*/
 if (!/*_.VALDATA___V155*/ curfptr[154]) /*_.VALDATA___V155*/ curfptr[154] = (void*)&cdat->dobj_117__CLASS_NREP_TESTPOINT;
  basilys_assertmsg("iniobj checkdiscr dobj_117__CLASS_NREP_TESTPOINT", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_117__CLASS_NREP_TESTPOINT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_117__CLASS_NREP_TESTPOINT.obj_num = OBMAG_OBJECT;
  cdat->dobj_117__CLASS_NREP_TESTPOINT.obj_hash = 350422253;
  cdat->dobj_117__CLASS_NREP_TESTPOINT.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_117__CLASS_NREP_TESTPOINT));
 
 /*inistring dstr_118__CLASS_NREP_TESTP*/
 /*_.VALSTR___V156*/ curfptr[155] = (void*)&cdat->dstr_118__CLASS_NREP_TESTP;
  cdat->dstr_118__CLASS_NREP_TESTP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_118__CLASS_NREP_TESTP.val, "CLASS_NREP_TESTPOINT");
 
 /*inimult dtup_119__CLASS_NREP_TESTPOINT*/
 /*_.VALTUP___V157*/ curfptr[156] = (void*)&cdat->dtup_119__CLASS_NREP_TESTPOINT;
  cdat->dtup_119__CLASS_NREP_TESTPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_119__CLASS_NREP_TESTPOINT.nbval = 4;
 /*inimult dtup_120__CLASS_NREP_TESTPOINT*/
 /*_.VALTUP___V162*/ curfptr[161] = (void*)&cdat->dtup_120__CLASS_NREP_TESTPOINT;
  cdat->dtup_120__CLASS_NREP_TESTPOINT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_120__CLASS_NREP_TESTPOINT.nbval = 4;
 /*iniobj dsym_121__NTSP_FAILBP*/
 if (!/*_.VALDATA___V163*/ curfptr[162]) /*_.VALDATA___V163*/ curfptr[162] = (void*)&cdat->dsym_121__NTSP_FAILBP;
  basilys_assertmsg("iniobj checkdiscr dsym_121__NTSP_FAILBP", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_121__NTSP_FAILBP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_121__NTSP_FAILBP.obj_hash = 695204297;
  cdat->dsym_121__NTSP_FAILBP.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_121__NTSP_FAILBP));
 
 /*inistring dstr_122__NTSP_FAILBP*/
 /*_.VALSTR___V164*/ curfptr[163] = (void*)&cdat->dstr_122__NTSP_FAILBP;
  cdat->dstr_122__NTSP_FAILBP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_122__NTSP_FAILBP.val, "NTSP_FAILBP");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ iniobj");
 /*iniobj dobj_123__NTSP_FAILBP*/
 if (!/*_.VALDATA___V158*/ curfptr[157]) /*_.VALDATA___V158*/ curfptr[157] = (void*)&cdat->dobj_123__NTSP_FAILBP;
  basilys_assertmsg("iniobj checkdiscr dobj_123__NTSP_FAILBP", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_123__NTSP_FAILBP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_123__NTSP_FAILBP.obj_num = 2;
  cdat->dobj_123__NTSP_FAILBP.obj_hash = 1039756320;
  cdat->dobj_123__NTSP_FAILBP.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_123__NTSP_FAILBP));
 
 /*inistring dstr_124__NTSP_FAILBP*/
 /*_.VALSTR___V159*/ curfptr[158] = (void*)&cdat->dstr_124__NTSP_FAILBP;
  cdat->dstr_124__NTSP_FAILBP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_124__NTSP_FAILBP.val, "NTSP_FAILBP");
 
 /*iniobj dsym_125__NTSP_MATCHER*/
 if (!/*_.VALDATA___V165*/ curfptr[164]) /*_.VALDATA___V165*/ curfptr[164] = (void*)&cdat->dsym_125__NTSP_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dsym_125__NTSP_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_125__NTSP_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_125__NTSP_MATCHER.obj_hash = 468726161;
  cdat->dsym_125__NTSP_MATCHER.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_125__NTSP_MATCHER));
 
 /*inistring dstr_126__NTSP_MATCHER*/
 /*_.VALSTR___V166*/ curfptr[165] = (void*)&cdat->dstr_126__NTSP_MATCHER;
  cdat->dstr_126__NTSP_MATCHER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_126__NTSP_MATCHER.val, "NTSP_MATCHER");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ iniobj");
 /*iniobj dobj_127__NTSP_MATCHER*/
 if (!/*_.VALDATA___V160*/ curfptr[159]) /*_.VALDATA___V160*/ curfptr[159] = (void*)&cdat->dobj_127__NTSP_MATCHER;
  basilys_assertmsg("iniobj checkdiscr dobj_127__NTSP_MATCHER", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_127__NTSP_MATCHER.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_127__NTSP_MATCHER.obj_num = 3;
  cdat->dobj_127__NTSP_MATCHER.obj_hash = 929435319;
  cdat->dobj_127__NTSP_MATCHER.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_127__NTSP_MATCHER));
 
 /*inistring dstr_128__NTSP_MATCHER*/
 /*_.VALSTR___V161*/ curfptr[160] = (void*)&cdat->dstr_128__NTSP_MATCHER;
  cdat->dstr_128__NTSP_MATCHER.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_128__NTSP_MATCHER.val, "NTSP_MATCHER");
 
 /*inirout drout_129__NORMEXP_MATCH*/
 /*_.VALROUT___V167*/ curfptr[166] = (void*)&cdat->drout_129__NORMEXP_MATCH;
  cdat->drout_129__NORMEXP_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_129__NORMEXP_MATCH.routdescr, "NORMEXP_MATCH @warmelt-normatch.bysl:292",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_129__NORMEXP_MATCH.nbval = 12;
 *(basilysroutfun_t **) (cdat->drout_129__NORMEXP_MATCH.routaddr) = rout_9_NORMEXP_MATCH;
 
 /*iniclos dclo_130__NORMEXP_MATCH*/
 /*_.VALCLO___V168*/ curfptr[167] = (void*)&cdat->dclo_130__NORMEXP_MATCH;
  cdat->dclo_130__NORMEXP_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_130__NORMEXP_MATCH.nbval = 0;
 
 /*iniobj dsym_131__CLASS_SRC_MATCH*/
 if (!/*_.VALDATA___V169*/ curfptr[168]) /*_.VALDATA___V169*/ curfptr[168] = (void*)&cdat->dsym_131__CLASS_SRC_MATCH;
  basilys_assertmsg("iniobj checkdiscr dsym_131__CLASS_SRC_MATCH", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_131__CLASS_SRC_MATCH.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_131__CLASS_SRC_MATCH.obj_hash = 898626999;
  cdat->dsym_131__CLASS_SRC_MATCH.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_131__CLASS_SRC_MATCH));
 
 /*inistring dstr_132__CLASS_SRC_MATCH*/
 /*_.VALSTR___V170*/ curfptr[169] = (void*)&cdat->dstr_132__CLASS_SRC_MATCH;
  cdat->dstr_132__CLASS_SRC_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_132__CLASS_SRC_MATCH.val, "CLASS_SRC_MATCH");
 
 /*iniobj dsym_133__CLASS_ENVIRONMENT*/
 if (!/*_.VALDATA___V171*/ curfptr[170]) /*_.VALDATA___V171*/ curfptr[170] = (void*)&cdat->dsym_133__CLASS_ENVIRONMENT;
  basilys_assertmsg("iniobj checkdiscr dsym_133__CLASS_ENVIRONMENT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_133__CLASS_ENVIRONMENT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_133__CLASS_ENVIRONMENT.obj_hash = 59212821;
  cdat->dsym_133__CLASS_ENVIRONMENT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_133__CLASS_ENVIRONMENT));
 
 /*inistring dstr_134__CLASS_ENVIRONMEN*/
 /*_.VALSTR___V172*/ curfptr[171] = (void*)&cdat->dstr_134__CLASS_ENVIRONMEN;
  cdat->dstr_134__CLASS_ENVIRONMEN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_134__CLASS_ENVIRONMEN.val, "CLASS_ENVIRONMENT");
 
 /*iniobj dsym_135__CLASS_NORMCONTEXT*/
 if (!/*_.VALDATA___V173*/ curfptr[172]) /*_.VALDATA___V173*/ curfptr[172] = (void*)&cdat->dsym_135__CLASS_NORMCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dsym_135__CLASS_NORMCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_135__CLASS_NORMCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_135__CLASS_NORMCONTEXT.obj_hash = 317209306;
  cdat->dsym_135__CLASS_NORMCONTEXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_135__CLASS_NORMCONTEXT));
 
 /*inistring dstr_136__CLASS_NORMCONTEX*/
 /*_.VALSTR___V174*/ curfptr[173] = (void*)&cdat->dstr_136__CLASS_NORMCONTEX;
  cdat->dstr_136__CLASS_NORMCONTEX.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_136__CLASS_NORMCONTEX.val, "CLASS_NORMCONTEXT");
 
 /*iniobj dsym_137__DISCR_MULTIPLE*/
 if (!/*_.VALDATA___V175*/ curfptr[174]) /*_.VALDATA___V175*/ curfptr[174] = (void*)&cdat->dsym_137__DISCR_MULTIPLE;
  basilys_assertmsg("iniobj checkdiscr dsym_137__DISCR_MULTIPLE", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_137__DISCR_MULTIPLE.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_137__DISCR_MULTIPLE.obj_hash = 91864583;
  cdat->dsym_137__DISCR_MULTIPLE.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_137__DISCR_MULTIPLE));
 
 /*inistring dstr_138__DISCR_MULTIPLE*/
 /*_.VALSTR___V176*/ curfptr[175] = (void*)&cdat->dstr_138__DISCR_MULTIPLE;
  cdat->dstr_138__DISCR_MULTIPLE.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_138__DISCR_MULTIPLE.val, "DISCR_MULTIPLE");
 
 /*iniobj dsym_139__CLASS_SRC_CASEMATCH*/
 if (!/*_.VALDATA___V177*/ curfptr[176]) /*_.VALDATA___V177*/ curfptr[176] = (void*)&cdat->dsym_139__CLASS_SRC_CASEMATCH;
  basilys_assertmsg("iniobj checkdiscr dsym_139__CLASS_SRC_CASEMATCH", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_139__CLASS_SRC_CASEMATCH.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_139__CLASS_SRC_CASEMATCH.obj_hash = 695254918;
  cdat->dsym_139__CLASS_SRC_CASEMATCH.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_139__CLASS_SRC_CASEMATCH));
 
 /*inistring dstr_140__CLASS_SRC_CASEMA*/
 /*_.VALSTR___V178*/ curfptr[177] = (void*)&cdat->dstr_140__CLASS_SRC_CASEMA;
  cdat->dstr_140__CLASS_SRC_CASEMA.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_140__CLASS_SRC_CASEMA.val, "CLASS_SRC_CASEMATCH");
 
 /*iniobj dsym_141__DISCR_MAPOBJECTS*/
 if (!/*_.VALDATA___V179*/ curfptr[178]) /*_.VALDATA___V179*/ curfptr[178] = (void*)&cdat->dsym_141__DISCR_MAPOBJECTS;
  basilys_assertmsg("iniobj checkdiscr dsym_141__DISCR_MAPOBJECTS", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_141__DISCR_MAPOBJECTS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_141__DISCR_MAPOBJECTS.obj_hash = 95669218;
  cdat->dsym_141__DISCR_MAPOBJECTS.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_141__DISCR_MAPOBJECTS));
 
 /*inistring dstr_142__DISCR_MAPOBJECTS*/
 /*_.VALSTR___V180*/ curfptr[179] = (void*)&cdat->dstr_142__DISCR_MAPOBJECTS;
  cdat->dstr_142__DISCR_MAPOBJECTS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_142__DISCR_MAPOBJECTS.val, "DISCR_MAPOBJECTS");
 
 /*iniobj dsym_143__DISCR_LIST*/
 if (!/*_.VALDATA___V181*/ curfptr[180]) /*_.VALDATA___V181*/ curfptr[180] = (void*)&cdat->dsym_143__DISCR_LIST;
  basilys_assertmsg("iniobj checkdiscr dsym_143__DISCR_LIST", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_143__DISCR_LIST.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_143__DISCR_LIST.obj_hash = 98407634;
  cdat->dsym_143__DISCR_LIST.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_143__DISCR_LIST));
 
 /*inistring dstr_144__DISCR_LIST*/
 /*_.VALSTR___V182*/ curfptr[181] = (void*)&cdat->dstr_144__DISCR_LIST;
  cdat->dstr_144__DISCR_LIST.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_144__DISCR_LIST.val, "DISCR_LIST");
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

initial_frame_st   curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 196;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
/**initial routine prologue**/
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
  if (sy_CLASS_PATTERNCONTEXT && NULL == /*_.VALDATA___V39*/ curfptr[38])
  /*_.VALDATA___V39*/ curfptr[38] = (void*) sy_CLASS_PATTERNCONTEXT; }
 
 /*getnamedsym:CLASS_ROOT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ROOT = basilysgc_named_symbol("CLASS_ROOT", BASILYS_GET);
  if (sy_CLASS_ROOT && NULL == /*_.VALDATA___V41*/ curfptr[40])
  /*_.VALDATA___V41*/ curfptr[40] = (void*) sy_CLASS_ROOT; }
 
 /*getnamedsym:PCTN_NORMCTXT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_NORMCTXT = basilysgc_named_symbol("PCTN_NORMCTXT", BASILYS_GET);
  if (sy_PCTN_NORMCTXT && NULL == /*_.VALDATA___V59*/ curfptr[58])
  /*_.VALDATA___V59*/ curfptr[58] = (void*) sy_PCTN_NORMCTXT; }
 
 /*getnamedsym:PCTN_SRC*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_SRC = basilysgc_named_symbol("PCTN_SRC", BASILYS_GET);
  if (sy_PCTN_SRC && NULL == /*_.VALDATA___V61*/ curfptr[60])
  /*_.VALDATA___V61*/ curfptr[60] = (void*) sy_PCTN_SRC; }
 
 /*getnamedsym:PCTN_ENV*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_ENV = basilysgc_named_symbol("PCTN_ENV", BASILYS_GET);
  if (sy_PCTN_ENV && NULL == /*_.VALDATA___V63*/ curfptr[62])
  /*_.VALDATA___V63*/ curfptr[62] = (void*) sy_PCTN_ENV; }
 
 /*getnamedsym:PCTN_MAPATVAR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_MAPATVAR = basilysgc_named_symbol("PCTN_MAPATVAR", BASILYS_GET);
  if (sy_PCTN_MAPATVAR && NULL == /*_.VALDATA___V65*/ curfptr[64])
  /*_.VALDATA___V65*/ curfptr[64] = (void*) sy_PCTN_MAPATVAR; }
 
 /*getnamedsym:PCTN_MAPATCST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_MAPATCST = basilysgc_named_symbol("PCTN_MAPATCST", BASILYS_GET);
  if (sy_PCTN_MAPATCST && NULL == /*_.VALDATA___V67*/ curfptr[66])
  /*_.VALDATA___V67*/ curfptr[66] = (void*) sy_PCTN_MAPATCST; }
 
 /*getnamedsym:PCTN_BINDLIST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_BINDLIST = basilysgc_named_symbol("PCTN_BINDLIST", BASILYS_GET);
  if (sy_PCTN_BINDLIST && NULL == /*_.VALDATA___V69*/ curfptr[68])
  /*_.VALDATA___V69*/ curfptr[68] = (void*) sy_PCTN_BINDLIST; }
 
 /*getnamedsym:SCAN_PATTERN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_SCAN_PATTERN = basilysgc_named_symbol("SCAN_PATTERN", BASILYS_GET);
  if (sy_SCAN_PATTERN && NULL == /*_.VALDATA___V71*/ curfptr[70])
  /*_.VALDATA___V71*/ curfptr[70] = (void*) sy_SCAN_PATTERN; }
 
 /*getnamedsym:CLASS_SELECTOR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SELECTOR = basilysgc_named_symbol("CLASS_SELECTOR", BASILYS_GET);
  if (sy_CLASS_SELECTOR && NULL == /*_.VALDATA___V73*/ curfptr[72])
  /*_.VALDATA___V73*/ curfptr[72] = (void*) sy_CLASS_SELECTOR; }
 
 /*getnamedsym:DEBUG_MSG_FUN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DEBUG_MSG_FUN = basilysgc_named_symbol("DEBUG_MSG_FUN", BASILYS_GET);
  if (sy_DEBUG_MSG_FUN && NULL == /*_.VALDATA___V79*/ curfptr[78])
  /*_.VALDATA___V79*/ curfptr[78] = (void*) sy_DEBUG_MSG_FUN; }
 
 /*getnamedsym:CLASS_NAMED*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NAMED = basilysgc_named_symbol("CLASS_NAMED", BASILYS_GET);
  if (sy_CLASS_NAMED && NULL == /*_.VALDATA___V81*/ curfptr[80])
  /*_.VALDATA___V81*/ curfptr[80] = (void*) sy_CLASS_NAMED; }
 
 /*getnamedsym:INSTALL_METHOD*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_INSTALL_METHOD = basilysgc_named_symbol("INSTALL_METHOD", BASILYS_GET);
  if (sy_INSTALL_METHOD && NULL == /*_.VALDATA___V83*/ curfptr[82])
  /*_.VALDATA___V83*/ curfptr[82] = (void*) sy_INSTALL_METHOD; }
 
 /*getnamedsym:DISCR_ANYRECV*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_ANYRECV = basilysgc_named_symbol("DISCR_ANYRECV", BASILYS_GET);
  if (sy_DISCR_ANYRECV && NULL == /*_.VALDATA___V85*/ curfptr[84])
  /*_.VALDATA___V85*/ curfptr[84] = (void*) sy_DISCR_ANYRECV; }
 
 /*getnamedsym:CLASS_SRCPATTERN_OR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_OR = basilysgc_named_symbol("CLASS_SRCPATTERN_OR", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_OR && NULL == /*_.VALDATA___V89*/ curfptr[88])
  /*_.VALDATA___V89*/ curfptr[88] = (void*) sy_CLASS_SRCPATTERN_OR; }
 
 /*getnamedsym:CLASS_SRCPATTERN_AND*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_AND = basilysgc_named_symbol("CLASS_SRCPATTERN_AND", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_AND && NULL == /*_.VALDATA___V93*/ curfptr[92])
  /*_.VALDATA___V93*/ curfptr[92] = (void*) sy_CLASS_SRCPATTERN_AND; }
 
 /*getnamedsym:CLASS_SRCPATTERN_VARIABLE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_VARIABLE = basilysgc_named_symbol("CLASS_SRCPATTERN_VARIABLE", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_VARIABLE && NULL == /*_.VALDATA___V97*/ curfptr[96])
  /*_.VALDATA___V97*/ curfptr[96] = (void*) sy_CLASS_SRCPATTERN_VARIABLE; }
 
 /*getnamedsym:CLASS_SRCPATTERN_JOKERVAR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_JOKERVAR = basilysgc_named_symbol("CLASS_SRCPATTERN_JOKERVAR", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_JOKERVAR && NULL == /*_.VALDATA___V101*/ curfptr[100])
  /*_.VALDATA___V101*/ curfptr[100] = (void*) sy_CLASS_SRCPATTERN_JOKERVAR; }
 
 /*getnamedsym:CLASS_SRCPATTERN_CONSTANT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_CONSTANT = basilysgc_named_symbol("CLASS_SRCPATTERN_CONSTANT", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_CONSTANT && NULL == /*_.VALDATA___V105*/ curfptr[104])
  /*_.VALDATA___V105*/ curfptr[104] = (void*) sy_CLASS_SRCPATTERN_CONSTANT; }
 
 /*getnamedsym:GET_CTYPE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_GET_CTYPE = basilysgc_named_symbol("GET_CTYPE", BASILYS_GET);
  if (sy_GET_CTYPE && NULL == /*_.VALDATA___V107*/ curfptr[106])
  /*_.VALDATA___V107*/ curfptr[106] = (void*) sy_GET_CTYPE; }
 
 /*getnamedsym:CLASS_CTYPE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_CTYPE = basilysgc_named_symbol("CLASS_CTYPE", BASILYS_GET);
  if (sy_CLASS_CTYPE && NULL == /*_.VALDATA___V109*/ curfptr[108])
  /*_.VALDATA___V109*/ curfptr[108] = (void*) sy_CLASS_CTYPE; }
 
 /*getnamedsym:NORMAL_EXP*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NORMAL_EXP = basilysgc_named_symbol("NORMAL_EXP", BASILYS_GET);
  if (sy_NORMAL_EXP && NULL == /*_.VALDATA___V111*/ curfptr[110])
  /*_.VALDATA___V111*/ curfptr[110] = (void*) sy_NORMAL_EXP; }
 
 /*getnamedsym:LIST_APPEND2LIST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_LIST_APPEND2LIST = basilysgc_named_symbol("LIST_APPEND2LIST", BASILYS_GET);
  if (sy_LIST_APPEND2LIST && NULL == /*_.VALDATA___V113*/ curfptr[112])
  /*_.VALDATA___V113*/ curfptr[112] = (void*) sy_LIST_APPEND2LIST; }
 
 /*getnamedsym:CLASS_SRCPATTERN_OBJECT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_OBJECT = basilysgc_named_symbol("CLASS_SRCPATTERN_OBJECT", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_OBJECT && NULL == /*_.VALDATA___V117*/ curfptr[116])
  /*_.VALDATA___V117*/ curfptr[116] = (void*) sy_CLASS_SRCPATTERN_OBJECT; }
 
 /*getnamedsym:CLASS_CLASS*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_CLASS = basilysgc_named_symbol("CLASS_CLASS", BASILYS_GET);
  if (sy_CLASS_CLASS && NULL == /*_.VALDATA___V119*/ curfptr[118])
  /*_.VALDATA___V119*/ curfptr[118] = (void*) sy_CLASS_CLASS; }
 
 /*getnamedsym:CLASS_SRCFIELDPATTERN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCFIELDPATTERN = basilysgc_named_symbol("CLASS_SRCFIELDPATTERN", BASILYS_GET);
  if (sy_CLASS_SRCFIELDPATTERN && NULL == /*_.VALDATA___V121*/ curfptr[120])
  /*_.VALDATA___V121*/ curfptr[120] = (void*) sy_CLASS_SRCFIELDPATTERN; }
 
 /*getnamedsym:CLASS_FIELD*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_FIELD = basilysgc_named_symbol("CLASS_FIELD", BASILYS_GET);
  if (sy_CLASS_FIELD && NULL == /*_.VALDATA___V123*/ curfptr[122])
  /*_.VALDATA___V123*/ curfptr[122] = (void*) sy_CLASS_FIELD; }
 
 /*getnamedsym:CTYPE_VALUE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CTYPE_VALUE = basilysgc_named_symbol("CTYPE_VALUE", BASILYS_GET);
  if (sy_CTYPE_VALUE && NULL == /*_.VALDATA___V125*/ curfptr[124])
  /*_.VALDATA___V125*/ curfptr[124] = (void*) sy_CTYPE_VALUE; }
 
 /*getnamedsym:CLASS_SRCPATTERN_COMPOSITE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRCPATTERN_COMPOSITE = basilysgc_named_symbol("CLASS_SRCPATTERN_COMPOSITE", BASILYS_GET);
  if (sy_CLASS_SRCPATTERN_COMPOSITE && NULL == /*_.VALDATA___V129*/ curfptr[128])
  /*_.VALDATA___V129*/ curfptr[128] = (void*) sy_CLASS_SRCPATTERN_COMPOSITE; }
 
 /*getnamedsym:CLASS_ANY_MATCHER*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ANY_MATCHER = basilysgc_named_symbol("CLASS_ANY_MATCHER", BASILYS_GET);
  if (sy_CLASS_ANY_MATCHER && NULL == /*_.VALDATA___V131*/ curfptr[130])
  /*_.VALDATA___V131*/ curfptr[130] = (void*) sy_CLASS_ANY_MATCHER; }
 
 /*getnamedsym:CLASS_FORMAL_BINDING*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_FORMAL_BINDING = basilysgc_named_symbol("CLASS_FORMAL_BINDING", BASILYS_GET);
  if (sy_CLASS_FORMAL_BINDING && NULL == /*_.VALDATA___V133*/ curfptr[132])
  /*_.VALDATA___V133*/ curfptr[132] = (void*) sy_CLASS_FORMAL_BINDING; }
 
 /*getnamedsym:NORMALIZE_TUPLE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NORMALIZE_TUPLE = basilysgc_named_symbol("NORMALIZE_TUPLE", BASILYS_GET);
  if (sy_NORMALIZE_TUPLE && NULL == /*_.VALDATA___V135*/ curfptr[134])
  /*_.VALDATA___V135*/ curfptr[134] = (void*) sy_NORMALIZE_TUPLE; }
 
 /*getnamedsym:CLASS_NREP_BACKPOINT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_BACKPOINT = basilysgc_named_symbol("CLASS_NREP_BACKPOINT", BASILYS_GET);
  if (sy_CLASS_NREP_BACKPOINT && NULL == /*_.VALDATA___V137*/ curfptr[136])
  /*_.VALDATA___V137*/ curfptr[136] = (void*) sy_CLASS_NREP_BACKPOINT; }
 
 /*getnamedsym:CLASS_NREP_TYPEXPR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TYPEXPR = basilysgc_named_symbol("CLASS_NREP_TYPEXPR", BASILYS_GET);
  if (sy_CLASS_NREP_TYPEXPR && NULL == /*_.VALDATA___V139*/ curfptr[138])
  /*_.VALDATA___V139*/ curfptr[138] = (void*) sy_CLASS_NREP_TYPEXPR; }
 
 /*getnamedsym:NBKP_FIRST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NBKP_FIRST = basilysgc_named_symbol("NBKP_FIRST", BASILYS_GET);
  if (sy_NBKP_FIRST && NULL == /*_.VALDATA___V149*/ curfptr[148])
  /*_.VALDATA___V149*/ curfptr[148] = (void*) sy_NBKP_FIRST; }
 
 /*getnamedsym:NBKP_ELSE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NBKP_ELSE = basilysgc_named_symbol("NBKP_ELSE", BASILYS_GET);
  if (sy_NBKP_ELSE && NULL == /*_.VALDATA___V151*/ curfptr[150])
  /*_.VALDATA___V151*/ curfptr[150] = (void*) sy_NBKP_ELSE; }
 
 /*getnamedsym:CLASS_NREP_TESTPOINT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NREP_TESTPOINT = basilysgc_named_symbol("CLASS_NREP_TESTPOINT", BASILYS_GET);
  if (sy_CLASS_NREP_TESTPOINT && NULL == /*_.VALDATA___V153*/ curfptr[152])
  /*_.VALDATA___V153*/ curfptr[152] = (void*) sy_CLASS_NREP_TESTPOINT; }
 
 /*getnamedsym:NTSP_FAILBP*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSP_FAILBP = basilysgc_named_symbol("NTSP_FAILBP", BASILYS_GET);
  if (sy_NTSP_FAILBP && NULL == /*_.VALDATA___V163*/ curfptr[162])
  /*_.VALDATA___V163*/ curfptr[162] = (void*) sy_NTSP_FAILBP; }
 
 /*getnamedsym:NTSP_MATCHER*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NTSP_MATCHER = basilysgc_named_symbol("NTSP_MATCHER", BASILYS_GET);
  if (sy_NTSP_MATCHER && NULL == /*_.VALDATA___V165*/ curfptr[164])
  /*_.VALDATA___V165*/ curfptr[164] = (void*) sy_NTSP_MATCHER; }
 
 /*getnamedsym:CLASS_SRC_MATCH*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRC_MATCH = basilysgc_named_symbol("CLASS_SRC_MATCH", BASILYS_GET);
  if (sy_CLASS_SRC_MATCH && NULL == /*_.VALDATA___V169*/ curfptr[168])
  /*_.VALDATA___V169*/ curfptr[168] = (void*) sy_CLASS_SRC_MATCH; }
 
 /*getnamedsym:CLASS_ENVIRONMENT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ENVIRONMENT = basilysgc_named_symbol("CLASS_ENVIRONMENT", BASILYS_GET);
  if (sy_CLASS_ENVIRONMENT && NULL == /*_.VALDATA___V171*/ curfptr[170])
  /*_.VALDATA___V171*/ curfptr[170] = (void*) sy_CLASS_ENVIRONMENT; }
 
 /*getnamedsym:CLASS_NORMCONTEXT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NORMCONTEXT = basilysgc_named_symbol("CLASS_NORMCONTEXT", BASILYS_GET);
  if (sy_CLASS_NORMCONTEXT && NULL == /*_.VALDATA___V173*/ curfptr[172])
  /*_.VALDATA___V173*/ curfptr[172] = (void*) sy_CLASS_NORMCONTEXT; }
 
 /*getnamedsym:DISCR_MULTIPLE*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_MULTIPLE = basilysgc_named_symbol("DISCR_MULTIPLE", BASILYS_GET);
  if (sy_DISCR_MULTIPLE && NULL == /*_.VALDATA___V175*/ curfptr[174])
  /*_.VALDATA___V175*/ curfptr[174] = (void*) sy_DISCR_MULTIPLE; }
 
 /*getnamedsym:CLASS_SRC_CASEMATCH*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRC_CASEMATCH = basilysgc_named_symbol("CLASS_SRC_CASEMATCH", BASILYS_GET);
  if (sy_CLASS_SRC_CASEMATCH && NULL == /*_.VALDATA___V177*/ curfptr[176])
  /*_.VALDATA___V177*/ curfptr[176] = (void*) sy_CLASS_SRC_CASEMATCH; }
 
 /*getnamedsym:DISCR_MAPOBJECTS*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_MAPOBJECTS = basilysgc_named_symbol("DISCR_MAPOBJECTS", BASILYS_GET);
  if (sy_DISCR_MAPOBJECTS && NULL == /*_.VALDATA___V179*/ curfptr[178])
  /*_.VALDATA___V179*/ curfptr[178] = (void*) sy_DISCR_MAPOBJECTS; }
 
 /*getnamedsym:DISCR_LIST*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_LIST = basilysgc_named_symbol("DISCR_LIST", BASILYS_GET);
  if (sy_DISCR_LIST && NULL == /*_.VALDATA___V181*/ curfptr[180])
  /*_.VALDATA___V181*/ curfptr[180] = (void*) sy_DISCR_LIST; }
 
 /**COMMENT: get the value importer **/;
 
 { basilys_ptr_t slot=0;
  basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 12, "SYSDATA_VALUE_IMPORTER");/*_.VALIMPORT___V6*/ curfptr[5] = slot; };
 
 /**COMMENT: before getting imported values **/;
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ROOT__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V41*/ curfptr[40]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SELECTOR__V8*/ curfptr[7] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V73*/ curfptr[72]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DEBUG_MSG_FUN__V9*/ curfptr[8] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V79*/ curfptr[78]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NAMED__V10*/ curfptr[9] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V81*/ curfptr[80]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.INSTALL_METHOD__V11*/ curfptr[10] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V83*/ curfptr[82]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_ANYRECV__V12*/ curfptr[11] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V85*/ curfptr[84]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_OR__V13*/ curfptr[12] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V89*/ curfptr[88]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_AND__V14*/ curfptr[13] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V93*/ curfptr[92]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_VARIABLE__V15*/ curfptr[14] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V97*/ curfptr[96]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_JOKERVAR__V16*/ curfptr[15] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V101*/ curfptr[100]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_CONSTANT__V17*/ curfptr[16] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V105*/ curfptr[104]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.GET_CTYPE__V18*/ curfptr[17] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V107*/ curfptr[106]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_CTYPE__V19*/ curfptr[18] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V109*/ curfptr[108]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.NORMAL_EXP__V20*/ curfptr[19] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V111*/ curfptr[110]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.LIST_APPEND2LIST__V21*/ curfptr[20] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V113*/ curfptr[112]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_OBJECT__V22*/ curfptr[21] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V117*/ curfptr[116]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_CLASS__V23*/ curfptr[22] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V119*/ curfptr[118]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCFIELDPATTERN__V24*/ curfptr[23] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V121*/ curfptr[120]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_FIELD__V25*/ curfptr[24] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V123*/ curfptr[122]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CTYPE_VALUE__V26*/ curfptr[25] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V125*/ curfptr[124]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRCPATTERN_COMPOSITE__V27*/ curfptr[26] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V129*/ curfptr[128]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ANY_MATCHER__V28*/ curfptr[27] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V131*/ curfptr[130]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_FORMAL_BINDING__V29*/ curfptr[28] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V133*/ curfptr[132]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.NORMALIZE_TUPLE__V30*/ curfptr[29] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V135*/ curfptr[134]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V139*/ curfptr[138]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRC_MATCH__V32*/ curfptr[31] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V169*/ curfptr[168]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ENVIRONMENT__V33*/ curfptr[32] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V171*/ curfptr[170]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NORMCONTEXT__V34*/ curfptr[33] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V173*/ curfptr[172]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_MULTIPLE__V35*/ curfptr[34] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V175*/ curfptr[174]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRC_CASEMATCH__V36*/ curfptr[35] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_MAPOBJECTS__V37*/ curfptr[36] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_LIST__V38*/ curfptr[37] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /**COMMENT: after getting imported values **/;
 
 /**initial routine cdata initializer**/

 memset(predefinited, 0, sizeof(predefinited));
 initialize_module_cdata(&curfram__, predefinited);
 /**initial routine body**/
/**COMMENT: start of oinibody **/;
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V39*/ curfptr[38])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V39*/ curfptr[38]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V39*/ curfptr[38]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V40*/ curfptr[39]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V39*/ curfptr[38]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V41*/ curfptr[40])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V41*/ curfptr[40]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V41*/ curfptr[40]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V42*/ curfptr[41]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V41*/ curfptr[40]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V44*/ curfptr[43]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.CLASS_ROOT__V7*/ curfptr[6]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V45*/ curfptr[44]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V45*/ curfptr[44]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V45*/ curfptr[44]))->tabval[0] = (basilys_ptr_t)(/*_.CLASS_ROOT__V7*/ curfptr[6]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V45*/ curfptr[44]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V45*/ curfptr[44]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V47*/ curfptr[46]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V46*/ curfptr[45]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V49*/ curfptr[48]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V48*/ curfptr[47]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V51*/ curfptr[50]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V50*/ curfptr[49]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V53*/ curfptr[52]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
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
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V54*/ curfptr[53]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V57*/ curfptr[56]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V56*/ curfptr[55]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))->tabval[0] = (basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (4>=0 && 4< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (5>=0 && 5< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V58*/ curfptr[57]))->tabval[5] = (basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V58*/ curfptr[57]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V58*/ curfptr[57]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V43*/ curfptr[42]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V60*/ curfptr[59]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V59*/ curfptr[58]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V62*/ curfptr[61]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V61*/ curfptr[60]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V64*/ curfptr[63]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V63*/ curfptr[62]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V66*/ curfptr[65]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V65*/ curfptr[64]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V68*/ curfptr[67]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V67*/ curfptr[66]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V69*/ curfptr[68])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V69*/ curfptr[68]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V69*/ curfptr[68]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V70*/ curfptr[69]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V69*/ curfptr[68]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V71*/ curfptr[70])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V71*/ curfptr[70]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V71*/ curfptr[70]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V72*/ curfptr[71]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V71*/ curfptr[70]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V73*/ curfptr[72])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V73*/ curfptr[72]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V73*/ curfptr[72]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V74*/ curfptr[73]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V73*/ curfptr[72]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:52:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V75*/ curfptr[74])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V75*/ curfptr[74]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V75*/ curfptr[74]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V76*/ curfptr[75]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:52:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V75*/ curfptr[74]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V77*/ curfptr[76])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_39__SCANPAT_ANYRECV#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V77*/ curfptr[76])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V77*/ curfptr[76])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V77*/ curfptr[76])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V77*/ curfptr[76])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_39__SCANPAT_ANYRECV#2", NULL != (/*_.CLASS_NAMED__V10*/ curfptr[9]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V77*/ curfptr[76])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_NAMED__V10*/ curfptr[9]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:58:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V77*/ curfptr[76]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V78*/ curfptr[77])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V77*/ curfptr[76])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V78*/ curfptr[77])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V77*/ curfptr[76]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V78*/ curfptr[77]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V79*/ curfptr[78])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V79*/ curfptr[78]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V79*/ curfptr[78]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V80*/ curfptr[79]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V79*/ curfptr[78]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V81*/ curfptr[80])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V81*/ curfptr[80]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V81*/ curfptr[80]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V82*/ curfptr[81]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V81*/ curfptr[80]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V83*/ curfptr[82])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V83*/ curfptr[82]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V83*/ curfptr[82]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V84*/ curfptr[83]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V83*/ curfptr[82]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V85*/ curfptr[84])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V85*/ curfptr[84]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V85*/ curfptr[84]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V86*/ curfptr[85]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V85*/ curfptr[84]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V87*/ curfptr[86])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_49__SCANPAT_SRCPATOR#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V87*/ curfptr[86])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V87*/ curfptr[86])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V87*/ curfptr[86])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V87*/ curfptr[86])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_49__SCANPAT_SRCPATOR#2", NULL != (/*_.CLASS_SRCPATTERN_OR__V13*/ curfptr[12]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V87*/ curfptr[86])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OR__V13*/ curfptr[12]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V87*/ curfptr[86])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V75*/ curfptr[74]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V87*/ curfptr[86])->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V75*/ curfptr[74]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:72:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V87*/ curfptr[86]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V88*/ curfptr[87])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V87*/ curfptr[86])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V88*/ curfptr[87])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V87*/ curfptr[86]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V88*/ curfptr[87]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V89*/ curfptr[88])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V89*/ curfptr[88]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V89*/ curfptr[88]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V90*/ curfptr[89]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V89*/ curfptr[88]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V91*/ curfptr[90])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_53__SCANPAT_SRCPATAND#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V91*/ curfptr[90])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V91*/ curfptr[90])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V91*/ curfptr[90])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V91*/ curfptr[90])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_53__SCANPAT_SRCPATAND#2", NULL != (/*_.CLASS_SRCPATTERN_AND__V14*/ curfptr[13]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V91*/ curfptr[90])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_AND__V14*/ curfptr[13]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V91*/ curfptr[90])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V75*/ curfptr[74]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V91*/ curfptr[90])->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V75*/ curfptr[74]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:91:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V91*/ curfptr[90]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V92*/ curfptr[91])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V91*/ curfptr[90])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V92*/ curfptr[91])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V91*/ curfptr[90]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V92*/ curfptr[91]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V93*/ curfptr[92])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V93*/ curfptr[92]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V93*/ curfptr[92]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V94*/ curfptr[93]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V93*/ curfptr[92]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V95*/ curfptr[94])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_57__SCANPAT_SRCPATVAR#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V95*/ curfptr[94])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V95*/ curfptr[94])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V95*/ curfptr[94])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V95*/ curfptr[94])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_57__SCANPAT_SRCPATVAR#2", NULL != (/*_.CLASS_SRCPATTERN_VARIABLE__V15*/ curfptr[14]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V95*/ curfptr[94])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_VARIABLE__V15*/ curfptr[14]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:110:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V95*/ curfptr[94]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V96*/ curfptr[95])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V95*/ curfptr[94])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V96*/ curfptr[95])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V95*/ curfptr[94]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V96*/ curfptr[95]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V97*/ curfptr[96])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V97*/ curfptr[96]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V97*/ curfptr[96]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V98*/ curfptr[97]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V97*/ curfptr[96]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V99*/ curfptr[98])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_61__SCANPAT_SRCPATJOKER#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V99*/ curfptr[98])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V99*/ curfptr[98])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V99*/ curfptr[98])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V99*/ curfptr[98])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_61__SCANPAT_SRCPATJOKER#2", NULL != (/*_.CLASS_SRCPATTERN_JOKERVAR__V16*/ curfptr[15]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V99*/ curfptr[98])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_JOKERVAR__V16*/ curfptr[15]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:134:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V99*/ curfptr[98]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V100*/ curfptr[99])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V99*/ curfptr[98])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V100*/ curfptr[99])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V99*/ curfptr[98]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V100*/ curfptr[99]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V101*/ curfptr[100])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V101*/ curfptr[100]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V101*/ curfptr[100]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V102*/ curfptr[101]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V101*/ curfptr[100]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATCONST#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V103*/ curfptr[102])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V103*/ curfptr[102])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATCONST#2", NULL != (/*_.CLASS_SRCPATTERN_CONSTANT__V17*/ curfptr[16]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V103*/ curfptr[102])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_CONSTANT__V17*/ curfptr[16]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATCONST#3", NULL != (/*_.GET_CTYPE__V18*/ curfptr[17]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V103*/ curfptr[102])->tabval[3] = (basilys_ptr_t)(/*_.GET_CTYPE__V18*/ curfptr[17]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATCONST#4", NULL != (/*_.CLASS_CTYPE__V19*/ curfptr[18]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V103*/ curfptr[102])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_CTYPE__V19*/ curfptr[18]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATCONST#5", NULL != (/*_.NORMAL_EXP__V20*/ curfptr[19]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V103*/ curfptr[102])->tabval[5] = (basilys_ptr_t)(/*_.NORMAL_EXP__V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_65__SCANPAT_SRCPATCONST#6", NULL != (/*_.LIST_APPEND2LIST__V21*/ curfptr[20]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V103*/ curfptr[102])->tabval[6] = (basilys_ptr_t)(/*_.LIST_APPEND2LIST__V21*/ curfptr[20]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:147:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V103*/ curfptr[102]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V104*/ curfptr[103])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V103*/ curfptr[102])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V104*/ curfptr[103])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V103*/ curfptr[102]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V104*/ curfptr[103]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V105*/ curfptr[104])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V105*/ curfptr[104]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V105*/ curfptr[104]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V106*/ curfptr[105]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V105*/ curfptr[104]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V107*/ curfptr[106])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V107*/ curfptr[106]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V107*/ curfptr[106]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V108*/ curfptr[107]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V107*/ curfptr[106]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V109*/ curfptr[108])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V109*/ curfptr[108]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V109*/ curfptr[108]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V110*/ curfptr[109]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V109*/ curfptr[108]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V111*/ curfptr[110])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V111*/ curfptr[110]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V111*/ curfptr[110]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V112*/ curfptr[111]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V111*/ curfptr[110]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V113*/ curfptr[112])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V113*/ curfptr[112]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V113*/ curfptr[112]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V114*/ curfptr[113]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V113*/ curfptr[112]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_77__SCANPAT_SRCPATOBJECT#0", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[0] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_77__SCANPAT_SRCPATOBJECT#2", NULL != (/*_.CLASS_SRCPATTERN_OBJECT__V22*/ curfptr[21]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OBJECT__V22*/ curfptr[21]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_77__SCANPAT_SRCPATOBJECT#3", NULL != (/*_.CLASS_CLASS__V23*/ curfptr[22]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_CLASS__V23*/ curfptr[22]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_77__SCANPAT_SRCPATOBJECT#4", NULL != (/*_.CLASS_SRCFIELDPATTERN__V24*/ curfptr[23]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_SRCFIELDPATTERN__V24*/ curfptr[23]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_77__SCANPAT_SRCPATOBJECT#5", NULL != (/*_.CLASS_FIELD__V25*/ curfptr[24]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[5] = (basilys_ptr_t)(/*_.CLASS_FIELD__V25*/ curfptr[24]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V75*/ curfptr[74]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[6] = (basilys_ptr_t)(/*_.VALDATA___V75*/ curfptr[74]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_77__SCANPAT_SRCPATOBJECT#7", NULL != (/*_.CTYPE_VALUE__V26*/ curfptr[25]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V115*/ curfptr[114])->tabval[7] = (basilys_ptr_t)(/*_.CTYPE_VALUE__V26*/ curfptr[25]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:192:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V115*/ curfptr[114]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V116*/ curfptr[115])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V115*/ curfptr[114])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V116*/ curfptr[115])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V115*/ curfptr[114]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V116*/ curfptr[115]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V117*/ curfptr[116])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V117*/ curfptr[116]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V117*/ curfptr[116]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V118*/ curfptr[117]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V117*/ curfptr[116]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V119*/ curfptr[118])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V119*/ curfptr[118]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V119*/ curfptr[118]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V120*/ curfptr[119]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V119*/ curfptr[118]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V121*/ curfptr[120])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V121*/ curfptr[120]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V121*/ curfptr[120]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V122*/ curfptr[121]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V121*/ curfptr[120]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V123*/ curfptr[122])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V123*/ curfptr[122]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V123*/ curfptr[122]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V124*/ curfptr[123]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V123*/ curfptr[122]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V125*/ curfptr[124])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V125*/ curfptr[124]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V125*/ curfptr[124]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V126*/ curfptr[125]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V125*/ curfptr[124]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[0] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_89__SCANPAT_SRCPATCOMPOSITE#1", NULL != (/*_.CLASS_SRCPATTERN_COMPOSITE__V27*/ curfptr[26]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[1] = (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_COMPOSITE__V27*/ curfptr[26]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_89__SCANPAT_SRCPATCOMPOSITE#2", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[2] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_89__SCANPAT_SRCPATCOMPOSITE#3", NULL != (/*_.CLASS_ANY_MATCHER__V28*/ curfptr[27]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[3] = (basilys_ptr_t)(/*_.CLASS_ANY_MATCHER__V28*/ curfptr[27]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_89__SCANPAT_SRCPATCOMPOSITE#4", NULL != (/*_.CLASS_FORMAL_BINDING__V29*/ curfptr[28]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[4] = (basilys_ptr_t)(/*_.CLASS_FORMAL_BINDING__V29*/ curfptr[28]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_89__SCANPAT_SRCPATCOMPOSITE#5", NULL != (/*_.CLASS_CTYPE__V19*/ curfptr[18]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[5] = (basilys_ptr_t)(/*_.CLASS_CTYPE__V19*/ curfptr[18]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_89__SCANPAT_SRCPATCOMPOSITE#6", NULL != (/*_.NORMALIZE_TUPLE__V30*/ curfptr[29]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[6] = (basilys_ptr_t)(/*_.NORMALIZE_TUPLE__V30*/ curfptr[29]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_89__SCANPAT_SRCPATCOMPOSITE#7", NULL != (/*_.LIST_APPEND2LIST__V21*/ curfptr[20]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[7] = (basilys_ptr_t)(/*_.LIST_APPEND2LIST__V21*/ curfptr[20]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V75*/ curfptr[74]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V127*/ curfptr[126])->tabval[8] = (basilys_ptr_t)(/*_.VALDATA___V75*/ curfptr[74]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:227:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V127*/ curfptr[126]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V128*/ curfptr[127])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V127*/ curfptr[126])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V128*/ curfptr[127])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V127*/ curfptr[126]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V128*/ curfptr[127]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V129*/ curfptr[128])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V129*/ curfptr[128]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V129*/ curfptr[128]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V130*/ curfptr[129]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V129*/ curfptr[128]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V131*/ curfptr[130])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V131*/ curfptr[130]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V131*/ curfptr[130]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V132*/ curfptr[131]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V131*/ curfptr[130]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V133*/ curfptr[132])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V133*/ curfptr[132]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V133*/ curfptr[132]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V134*/ curfptr[133]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V133*/ curfptr[132]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V135*/ curfptr[134])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V135*/ curfptr[134]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V135*/ curfptr[134]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V136*/ curfptr[135]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V135*/ curfptr[134]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V137*/ curfptr[136])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V137*/ curfptr[136]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V137*/ curfptr[136]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V138*/ curfptr[137]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V137*/ curfptr[136]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V139*/ curfptr[138])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V139*/ curfptr[138]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V139*/ curfptr[138]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V140*/ curfptr[139]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V139*/ curfptr[138]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V142*/ curfptr[141]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V143*/ curfptr[142]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V143*/ curfptr[142]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V143*/ curfptr[142]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V145*/ curfptr[144]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V141*/ curfptr[140]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V144*/ curfptr[143]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V146*/ curfptr[145])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V146*/ curfptr[145]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V146*/ curfptr[145]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V147*/ curfptr[146]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V146*/ curfptr[145])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V146*/ curfptr[145]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V146*/ curfptr[145]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V141*/ curfptr[140]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V146*/ curfptr[145]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V144*/ curfptr[143]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V148*/ curfptr[147]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V146*/ curfptr[145]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V148*/ curfptr[147]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V141*/ curfptr[140]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V148*/ curfptr[147]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:273:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V141*/ curfptr[140]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V149*/ curfptr[148])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V149*/ curfptr[148]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V149*/ curfptr[148]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V150*/ curfptr[149]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V149*/ curfptr[148]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V151*/ curfptr[150])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V151*/ curfptr[150]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V151*/ curfptr[150]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V152*/ curfptr[151]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V151*/ curfptr[150]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V153*/ curfptr[152])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V153*/ curfptr[152]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V153*/ curfptr[152]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V154*/ curfptr[153]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V153*/ curfptr[152]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V156*/ curfptr[155]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),5))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),5))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))->tabval[2] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),5))), (2))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V157*/ curfptr[156]))->tabval[3] = (basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V157*/ curfptr[156]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V157*/ curfptr[156]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V159*/ curfptr[158]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V155*/ curfptr[154]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V158*/ curfptr[157]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V161*/ curfptr[160]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V155*/ curfptr[154]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V160*/ curfptr[159]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[0] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),6))), (0))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[1] = (basilys_ptr_t)(/*multacc*/(basilys_multiple_nth((/*fieldacc*/(basilys_field_object((basilys_ptr_t)(/*_.CLASS_NREP_TYPEXPR__V31*/ curfptr[30]),6))), (1))));
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V158*/ curfptr[157]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (3>=0 && 3< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V162*/ curfptr[161]))->tabval[3] = (basilys_ptr_t)(/*_.VALDATA___V160*/ curfptr[159]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V162*/ curfptr[161]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V155*/ curfptr[154]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V162*/ curfptr[161]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:280:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V155*/ curfptr[154]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V164*/ curfptr[163]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V163*/ curfptr[162]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V166*/ curfptr[165]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V165*/ curfptr[164]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#0", NULL != (/*_.CLASS_SRC_MATCH__V32*/ curfptr[31]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[0] = (basilys_ptr_t)(/*_.CLASS_SRC_MATCH__V32*/ curfptr[31]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#1", NULL != (/*_.CLASS_ENVIRONMENT__V33*/ curfptr[32]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[1] = (basilys_ptr_t)(/*_.CLASS_ENVIRONMENT__V33*/ curfptr[32]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#2", NULL != (/*_.CLASS_NORMCONTEXT__V34*/ curfptr[33]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_NORMCONTEXT__V34*/ curfptr[33]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#3", NULL != (/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[3] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#4", NULL != (/*_.DISCR_MULTIPLE__V35*/ curfptr[34]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[4] = (basilys_ptr_t)(/*_.DISCR_MULTIPLE__V35*/ curfptr[34]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#5", NULL != (/*_.NORMAL_EXP__V20*/ curfptr[19]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[5] = (basilys_ptr_t)(/*_.NORMAL_EXP__V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#6", NULL != (/*_.GET_CTYPE__V18*/ curfptr[17]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[6] = (basilys_ptr_t)(/*_.GET_CTYPE__V18*/ curfptr[17]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#7", NULL != (/*_.CLASS_SRC_CASEMATCH__V36*/ curfptr[35]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[7] = (basilys_ptr_t)(/*_.CLASS_SRC_CASEMATCH__V36*/ curfptr[35]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#8", NULL != (/*_.DISCR_MAPOBJECTS__V37*/ curfptr[36]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[8] = (basilys_ptr_t)(/*_.DISCR_MAPOBJECTS__V37*/ curfptr[36]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_129__NORMEXP_MATCH#9", NULL != (/*_.DISCR_LIST__V38*/ curfptr[37]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[9] = (basilys_ptr_t)(/*_.DISCR_LIST__V38*/ curfptr[37]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V43*/ curfptr[42]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[10] = (basilys_ptr_t)(/*_.VALDATA___V43*/ curfptr[42]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V75*/ curfptr[74]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V167*/ curfptr[166])->tabval[11] = (basilys_ptr_t)(/*_.VALDATA___V75*/ curfptr[74]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:292:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V167*/ curfptr[166]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V168*/ curfptr[167])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V167*/ curfptr[166])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V168*/ curfptr[167])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V167*/ curfptr[166]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V168*/ curfptr[167]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V169*/ curfptr[168])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V169*/ curfptr[168]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V169*/ curfptr[168]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V170*/ curfptr[169]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V169*/ curfptr[168]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V171*/ curfptr[170])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V171*/ curfptr[170]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V171*/ curfptr[170]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V172*/ curfptr[171]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V171*/ curfptr[170]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V173*/ curfptr[172])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V173*/ curfptr[172]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V173*/ curfptr[172]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V174*/ curfptr[173]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V173*/ curfptr[172]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V175*/ curfptr[174])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V175*/ curfptr[174]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V175*/ curfptr[174]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V176*/ curfptr[175]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V175*/ curfptr[174]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V178*/ curfptr[177]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V177*/ curfptr[176]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V180*/ curfptr[179]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V179*/ curfptr[178]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V182*/ curfptr[181]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V181*/ curfptr[180]);
 
 /**COMMENT: before toplevel body **/;
 
 /*block*/{
  /*cond*/ if (
   /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1]) /*then*/ {
  /*_.OR___V183*/ curfptr[182] = 
    /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1];;
  } else {
   /*block*/{
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.SYSDATA_CONT_FRESH_ENV__V184*/ curfptr[183] = slot; };
    ;
    /*apply*/{
     union basilysparam_un argtab[1];
     memset(&argtab, 0, sizeof(argtab));
     argtab[0].bp_cstring =  "warmelt-normatch-3.c";
     /*_.FUN___V185*/ curfptr[184] =  basilys_apply ((basilysclosure_ptr_t)(/*_.SYSDATA_CONT_FRESH_ENV__V184*/ curfptr[183]), (basilys_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (BPARSTR_CSTRING ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.OR___V183*/ curfptr[182] = /*_.FUN___V185*/ curfptr[184];;
    /*epilog*/
    /*clear*/ /*_.SYSDATA_CONT_FRESH_ENV__V184*/ curfptr[183] = 0 ;
    /*clear*/ /*_.FUN___V185*/ curfptr[184] = 0 ;}
   ;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:3:/ block");
  /*com.block:upd.cur.mod.env.cont : at very start*/{}/*com.end block:upd.cur.mod.env.cont : at very start*/
  ;
  /*epilog*/
  /*clear*/ /*_.OR___V183*/ curfptr[182] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:67:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:67:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:67:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V78*/ curfptr[77];
   /*_.INSTALL_METHOD__V186*/ curfptr[183] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.DISCR_ANYRECV__V12*/ curfptr[11]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V186*/ curfptr[183] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:86:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V88*/ curfptr[87];
   /*_.INSTALL_METHOD__V187*/ curfptr[184] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OR__V13*/ curfptr[12]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V187*/ curfptr[184] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:105:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:105:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:105:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V92*/ curfptr[91];
   /*_.INSTALL_METHOD__V188*/ curfptr[182] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_AND__V14*/ curfptr[13]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V188*/ curfptr[182] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:129:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V96*/ curfptr[95];
   /*_.INSTALL_METHOD__V189*/ curfptr[183] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_VARIABLE__V15*/ curfptr[14]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V189*/ curfptr[183] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:141:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:141:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:141:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V100*/ curfptr[99];
   /*_.INSTALL_METHOD__V190*/ curfptr[184] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_JOKERVAR__V16*/ curfptr[15]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V190*/ curfptr[184] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:187:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:187:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:187:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V104*/ curfptr[103];
   /*_.INSTALL_METHOD__V191*/ curfptr[182] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_CONSTANT__V17*/ curfptr[16]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V191*/ curfptr[182] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:221:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:221:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:221:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V116*/ curfptr[115];
   /*_.INSTALL_METHOD__V192*/ curfptr[183] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_OBJECT__V22*/ curfptr[21]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V192*/ curfptr[183] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:269:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:269:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:269:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V128*/ curfptr[127];
   /*_.INSTALL_METHOD__V193*/ curfptr[184] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRCPATTERN_COMPOSITE__V27*/ curfptr[26]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V193*/ curfptr[184] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:348:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:348:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NORMAL_EXP__V20*/ curfptr[19];
   BASILYS_LOCATION("warmelt-normatch.bysl:348:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V168*/ curfptr[167];
   /*_.INSTALL_METHOD__V194*/ curfptr[182] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V11*/ curfptr[10]), (basilys_ptr_t)(/*_.CLASS_SRC_MATCH__V32*/ curfptr[31]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V194*/ curfptr[182] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 4, "SYSDATA_VALUE_EXPORTER");/*_.VALUEXPORTER___V195*/ curfptr[183] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V43*/ curfptr[42];
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : CLASS_PATTERNCONTEXT*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V195*/ curfptr[183]), (basilys_ptr_t)(/*_.VALDATA___V39*/ curfptr[38]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V46*/ curfptr[45];
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_NORMCTXT*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V195*/ curfptr[183]), (basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V48*/ curfptr[47];
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_SRC*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V195*/ curfptr[183]), (basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V50*/ curfptr[49];
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_ENV*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V195*/ curfptr[183]), (basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V52*/ curfptr[51];
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_MAPATVAR*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V195*/ curfptr[183]), (basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V54*/ curfptr[53];
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_MAPATCST*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V195*/ curfptr[183]), (basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V56*/ curfptr[55];
   BASILYS_LOCATION("warmelt-normatch.bysl:353:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_BINDLIST*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V195*/ curfptr[183]), (basilys_ptr_t)(/*_.VALDATA___V69*/ curfptr[68]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.VALUEXPORTER___V195*/ curfptr[183] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:358:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 4, "SYSDATA_VALUE_EXPORTER");/*_.VALUEXPORTER___V196*/ curfptr[184] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:358:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:358:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V75*/ curfptr[74];
   BASILYS_LOCATION("warmelt-normatch.bysl:358:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : SCAN_PATTERN*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V196*/ curfptr[184]), (basilys_ptr_t)(/*_.VALDATA___V71*/ curfptr[70]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.VALUEXPORTER___V196*/ curfptr[184] = 0 ;}
 
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
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V39*/ curfptr[38]));
 
 /*internsym:CLASS_ROOT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V41*/ curfptr[40]));
 
 /*internsym:PCTN_NORMCTXT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V59*/ curfptr[58]));
 
 /*internsym:PCTN_SRC*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V61*/ curfptr[60]));
 
 /*internsym:PCTN_ENV*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V63*/ curfptr[62]));
 
 /*internsym:PCTN_MAPATVAR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V65*/ curfptr[64]));
 
 /*internsym:PCTN_MAPATCST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V67*/ curfptr[66]));
 
 /*internsym:PCTN_BINDLIST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V69*/ curfptr[68]));
 
 /*internsym:SCAN_PATTERN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V71*/ curfptr[70]));
 
 /*internsym:CLASS_SELECTOR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V73*/ curfptr[72]));
 
 /*internsym:DEBUG_MSG_FUN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V79*/ curfptr[78]));
 
 /*internsym:CLASS_NAMED*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V81*/ curfptr[80]));
 
 /*internsym:INSTALL_METHOD*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V83*/ curfptr[82]));
 
 /*internsym:DISCR_ANYRECV*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V85*/ curfptr[84]));
 
 /*internsym:CLASS_SRCPATTERN_OR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V89*/ curfptr[88]));
 
 /*internsym:CLASS_SRCPATTERN_AND*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V93*/ curfptr[92]));
 
 /*internsym:CLASS_SRCPATTERN_VARIABLE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V97*/ curfptr[96]));
 
 /*internsym:CLASS_SRCPATTERN_JOKERVAR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V101*/ curfptr[100]));
 
 /*internsym:CLASS_SRCPATTERN_CONSTANT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V105*/ curfptr[104]));
 
 /*internsym:GET_CTYPE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V107*/ curfptr[106]));
 
 /*internsym:CLASS_CTYPE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V109*/ curfptr[108]));
 
 /*internsym:NORMAL_EXP*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V111*/ curfptr[110]));
 
 /*internsym:LIST_APPEND2LIST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V113*/ curfptr[112]));
 
 /*internsym:CLASS_SRCPATTERN_OBJECT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V117*/ curfptr[116]));
 
 /*internsym:CLASS_CLASS*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V119*/ curfptr[118]));
 
 /*internsym:CLASS_SRCFIELDPATTERN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V121*/ curfptr[120]));
 
 /*internsym:CLASS_FIELD*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V123*/ curfptr[122]));
 
 /*internsym:CTYPE_VALUE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V125*/ curfptr[124]));
 
 /*internsym:CLASS_SRCPATTERN_COMPOSITE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V129*/ curfptr[128]));
 
 /*internsym:CLASS_ANY_MATCHER*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V131*/ curfptr[130]));
 
 /*internsym:CLASS_FORMAL_BINDING*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V133*/ curfptr[132]));
 
 /*internsym:NORMALIZE_TUPLE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V135*/ curfptr[134]));
 
 /*internsym:CLASS_NREP_BACKPOINT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V137*/ curfptr[136]));
 
 /*internsym:CLASS_NREP_TYPEXPR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V139*/ curfptr[138]));
 
 /*internsym:NBKP_FIRST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V149*/ curfptr[148]));
 
 /*internsym:NBKP_ELSE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V151*/ curfptr[150]));
 
 /*internsym:CLASS_NREP_TESTPOINT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V153*/ curfptr[152]));
 
 /*internsym:NTSP_FAILBP*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V163*/ curfptr[162]));
 
 /*internsym:NTSP_MATCHER*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V165*/ curfptr[164]));
 
 /*internsym:CLASS_SRC_MATCH*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V169*/ curfptr[168]));
 
 /*internsym:CLASS_ENVIRONMENT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V171*/ curfptr[170]));
 
 /*internsym:CLASS_NORMCONTEXT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V173*/ curfptr[172]));
 
 /*internsym:DISCR_MULTIPLE*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V175*/ curfptr[174]));
 
 /*internsym:CLASS_SRC_CASEMATCH*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V177*/ curfptr[176]));
 
 /*internsym:DISCR_MAPOBJECTS*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V179*/ curfptr[178]));
 
 /*internsym:DISCR_LIST*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V181*/ curfptr[180]));
 
 /**COMMENT: set retinit from boxloc **/;
 
 /*_.RETINIT___V1*/ curfptr[0] = 
  /*final freshenv get*/ basilys_container_value((basilys_ptr_t)(/*_.CONTENV___V2*/ curfptr[1]));
 /**COMMENT: end the initproc **/;
 
 ;
 goto labend_rout;
labend_rout:  basilys_topframe = (struct callframe_basilys_st *) curfram__.prev;
 return /*_.RETINIT___V1*/ curfptr[0];
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR

} /* end start_module_basilys */


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
