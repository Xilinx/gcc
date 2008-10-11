/* GCC BASILYS GENERATED FILE warmelt-normatch-3.c - DO NOT EDIT */
/* generated on Sat Oct 11 16:49:46 2008 */

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




static basilys_ptr_t rout_1_NORMEXP_MATCH(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_, const char xresdescr_[], union basilysparam_un *xrestab_);


void* start_module_basilys(void*);


/**** warmelt-normatch-3.c implementations ****/




static basilys_ptr_t
rout_1_NORMEXP_MATCH(basilysclosure_ptr_t closp_, basilys_ptr_t firstargp_, const char xargdescr_[], union basilysparam_un *xargtab_,
      const char xresdescr_[], union basilysparam_un *xrestab_)
{
#if ENABLE_CHECKING
 static long call_counter__;
 long thiscallcounter__ ATTRIBUTE_UNUSED = ++ call_counter__;
#define callcount thiscallcounter__
#else
#define callcount 0L
#endif

 struct frame_rout_1_NORMEXP_MATCH_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 32
  void* varptr[32];
#define CURFRAM_NBVARNUM 7
  long varnum[7];
/*others*/
  long _spare_; }
  curfram__;
  memset(&curfram__, 0, sizeof(curfram__));
 curfram__.nbvar = 32;
  curfram__.clos = closp_;
 curfram__.prev = (struct callframe_basilys_st *) basilys_topframe;
 basilys_topframe = (struct callframe_basilys_st *) &curfram__;
basilys_trace_start("NORMEXP_MATCH", callcount);
/*getargs*/

 /*getarg#0*/
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ getarg");
 /*_.RECV__V2*/ curfptr[1] = (basilys_ptr_t) firstargp_;
 
 /*getarg#1*/
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ getarg");
 if (xargdescr_[0] != BPAR_PTR) goto lab_endgetargs;
 /*_.ENV__V3*/ curfptr[2] = (xargtab_[0].bp_aptr) ? (*(xargtab_[0].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.ENV__V3*/ curfptr[2])) != NULL);
 
 
 /*getarg#2*/
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ getarg");
 if (xargdescr_[1] != BPAR_PTR) goto lab_endgetargs;
 /*_.NCX__V4*/ curfptr[3] = (xargtab_[1].bp_aptr) ? (*(xargtab_[1].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.NCX__V4*/ curfptr[3])) != NULL);
 
 
 /*getarg#3*/
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ getarg");
 if (xargdescr_[2] != BPAR_PTR) goto lab_endgetargs;
 /*_.PSLOC__V5*/ curfptr[4] = (xargtab_[2].bp_aptr) ? (*(xargtab_[2].bp_aptr)) : NULL;
 gcc_assert(basilys_discr((basilys_ptr_t)(/*_.PSLOC__V5*/ curfptr[4])) != NULL);
 
 ;
 goto lab_endgetargs;
lab_endgetargs:;
/*body*/
BASILYS_LOCATION("warmelt-normatch.bysl:50:/ block");
/*block*/{
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:51:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:51:/ block");
  /*block*/{
   /*_#IS_A__L1*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (basilys_ptr_t)((/*!CLASS_SRC_MATCH*/ curfrout->tabval[0])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:51:/ cond");
   /*cond*/ if (/*_#IS_A__L1*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:51:/ cond.then");
   /*_.IFELSE___V7*/ curfptr[6] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:51:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:51:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check match recv"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(51)?(51):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V6*/ curfptr[5] = /*_.IFELSE___V7*/ curfptr[6];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:51:/ clear");
   /*clear*/ /*_#IS_A__L1*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:51:/ clear");
   /*clear*/ /*_.IFELSE___V7*/ curfptr[6] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:51:/ cppif.else");
 /*_.IFCPP___V6*/ curfptr[5] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:52:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:52:/ block");
  /*block*/{
   /*_#IS_A__L2*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.ENV__V3*/ curfptr[2]), (basilys_ptr_t)((/*!CLASS_ENVIRONMENT*/ curfrout->tabval[1])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:52:/ cond");
   /*cond*/ if (/*_#IS_A__L2*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:52:/ cond.then");
   /*_.IFELSE___V9*/ curfptr[8] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:52:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:52:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check env"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(52)?(52):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V8*/ curfptr[6] = /*_.IFELSE___V9*/ curfptr[8];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:52:/ clear");
   /*clear*/ /*_#IS_A__L2*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:52:/ clear");
   /*clear*/ /*_.IFELSE___V9*/ curfptr[8] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:52:/ cppif.else");
 /*_.IFCPP___V8*/ curfptr[6] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:53:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:53:/ block");
  /*block*/{
   /*_#IS_A__L3*/ curfnum[0] = 
     basilys_is_instance_of((basilys_ptr_t)(/*_.NCX__V4*/ curfptr[3]), (basilys_ptr_t)((/*!CLASS_NORMCONTEXT*/ curfrout->tabval[2])));;
   BASILYS_LOCATION("warmelt-normatch.bysl:53:/ cond");
   /*cond*/ if (/*_#IS_A__L3*/ curfnum[0]) /*then*/ {
   BASILYS_LOCATION("warmelt-normatch.bysl:53:/ cond.then");
   /*_.IFELSE___V11*/ curfptr[10] = (/*nil*/NULL);;
   } else {BASILYS_LOCATION("warmelt-normatch.bysl:53:/ cond.else");
   
    BASILYS_LOCATION("warmelt-normatch.bysl:53:/ block");
    /*block*/{
     /*block*/{
      
      basilys_assert_failed(( "check nctxt"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(53)?(53):__LINE__, __FUNCTION__);
      /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
     ;
     /*epilog*/}
    ;
    }
   ;
   /*_.IFCPP___V10*/ curfptr[8] = /*_.IFELSE___V11*/ curfptr[10];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:53:/ clear");
   /*clear*/ /*_#IS_A__L3*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:53:/ clear");
   /*clear*/ /*_.IFELSE___V11*/ curfptr[10] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:53:/ cppif.else");
 /*_.IFCPP___V10*/ curfptr[8] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 
 #if ENABLE_CHECKING
  BASILYS_LOCATION("warmelt-normatch.bysl:54:/ cppif.then");
 BASILYS_LOCATION("warmelt-normatch.bysl:54:/ block");
  /*block*/{
   /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 
    callcount;;
   BASILYS_LOCATION("warmelt-normatch.bysl:54:/ apply");
   /*apply*/{
    union basilysparam_un argtab[4];
    memset(&argtab, 0, sizeof(argtab));
    BASILYS_LOCATION("warmelt-normatch.bysl:54:/ apply.arg");
    argtab[0].bp_cstring =  "normexp_match recv";
    BASILYS_LOCATION("warmelt-normatch.bysl:54:/ apply.arg");
    argtab[1].bp_long = /*_#THE_CALLCOUNT__L4*/ curfnum[0];
    BASILYS_LOCATION("warmelt-normatch.bysl:54:/ apply.arg");
    argtab[2].bp_cstring =  "warmelt-normatch.bysl";
    BASILYS_LOCATION("warmelt-normatch.bysl:54:/ apply.arg");
    argtab[3].bp_long = 54;
    /*_.DEBUG_MSG_FUN__V13*/ curfptr[12] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
    }
   ;
   /*_.IFCPP___V12*/ curfptr[10] = /*_.DEBUG_MSG_FUN__V13*/ curfptr[12];;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:54:/ clear");
   /*clear*/ /*_#THE_CALLCOUNT__L4*/ curfnum[0] = 0 ;
   BASILYS_LOCATION("warmelt-normatch.bysl:54:/ clear");
   /*clear*/ /*_.DEBUG_MSG_FUN__V13*/ curfptr[12] = 0 ;}
  
  #else /*ENABLE_CHECKING*/
  BASILYS_LOCATION("warmelt-normatch.bysl:54:/ cppif.else");
 /*_.IFCPP___V12*/ curfptr[10] = (/*nil*/NULL);
  #endif /*ENABLE_CHECKING*/
  ;
 BASILYS_LOCATION("warmelt-normatch.bysl:55:/ block");
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 0, "SRC_LOC");/*_.SLOC__V15*/ curfptr[14] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:56:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 1, "SMAT_MATCHEDX");/*_.SMATSX__V16*/ curfptr[15] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:57:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 2, "SMAT_CASES");/*_.SCASES__V17*/ curfptr[16] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:58:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(/*_.RECV__V2*/ curfptr[1]), 3, "SMAT_ELSE");/*_.SELSE__V18*/ curfptr[17] = slot; };
  ;
  /*_.MAKE_MAPOBJECT__V19*/ curfptr[18] = 
    (basilysgc_new_mapobjects( (basilysobject_ptr_t) ((/*!DISCR_MAPOBJECTS*/ curfrout->tabval[5])), (20)));;
  BASILYS_LOCATION("warmelt-normatch.bysl:59:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:59:/ rawallocobj");
   /*rawallocobj*/ { basilys_ptr_t newobj = 0;
    basilys_raw_object_create(newobj,(basilys_ptr_t)((/*!CLASS_PATTERNCONTEXT*/ curfrout->tabval[4])), (3), "CLASS_PATTERNCONTEXT");/*_.INST__V21*/ curfptr[20] =
    newobj; };
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:59:/ putslot");
   /*putslot*/
   basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V21*/ curfptr[20])) == OBMAG_OBJECT);
   basilys_assertmsg("putslot checkoff", (0>=0 && 0< basilys_object_length((basilys_ptr_t)(/*_.INST__V21*/ curfptr[20]))));basilys_putfield_object((/*_.INST__V21*/ curfptr[20]), (0), (/*_.NCX__V4*/ curfptr[3]), "PCTN_NORMCTXT");
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:59:/ putslot");
   /*putslot*/
   basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V21*/ curfptr[20])) == OBMAG_OBJECT);
   basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.INST__V21*/ curfptr[20]))));basilys_putfield_object((/*_.INST__V21*/ curfptr[20]), (1), (/*_.RECV__V2*/ curfptr[1]), "PCTN_SRC");
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:59:/ putslot");
   /*putslot*/
   basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.INST__V21*/ curfptr[20])) == OBMAG_OBJECT);
   basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.INST__V21*/ curfptr[20]))));basilys_putfield_object((/*_.INST__V21*/ curfptr[20]), (2), (/*_.MAKE_MAPOBJECT__V19*/ curfptr[18]), "PCTN_MAPATVAR");
   ;
   /*_.PCTX__V20*/ curfptr[19] = /*_.INST__V21*/ curfptr[20];;}
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:65:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:65:/ block");
   /*block*/{
    /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 
     callcount;;
    BASILYS_LOCATION("warmelt-normatch.bysl:65:/ apply");
    /*apply*/{
     union basilysparam_un argtab[4];
     memset(&argtab, 0, sizeof(argtab));
     BASILYS_LOCATION("warmelt-normatch.bysl:65:/ apply.arg");
     argtab[0].bp_cstring =  "normexp_match scases";
     BASILYS_LOCATION("warmelt-normatch.bysl:65:/ apply.arg");
     argtab[1].bp_long = /*_#THE_CALLCOUNT__L5*/ curfnum[0];
     BASILYS_LOCATION("warmelt-normatch.bysl:65:/ apply.arg");
     argtab[2].bp_cstring =  "warmelt-normatch.bysl";
     BASILYS_LOCATION("warmelt-normatch.bysl:65:/ apply.arg");
     argtab[3].bp_long = 65;
     /*_.DEBUG_MSG_FUN__V23*/ curfptr[22] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.IFCPP___V22*/ curfptr[21] = /*_.DEBUG_MSG_FUN__V23*/ curfptr[22];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:65:/ clear");
    /*clear*/ /*_#THE_CALLCOUNT__L5*/ curfnum[0] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:65:/ clear");
    /*clear*/ /*_.DEBUG_MSG_FUN__V23*/ curfptr[22] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:65:/ cppif.else");
  /*_.IFCPP___V22*/ curfptr[21] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  BASILYS_LOCATION("warmelt-normatch.bysl:66:/ block");
  /*block*/{
   BASILYS_LOCATION("warmelt-normatch.bysl:66:/ multimsend");
   /*multimsend*/{
    union basilysparam_un argtab[3];
    union basilysparam_un restab[1];
    memset(&argtab, 0, sizeof(argtab));
    memset(&restab, 0, sizeof(restab));
    BASILYS_LOCATION("warmelt-normatch.bysl:66:/ multimsend.arg");
    argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.ENV__V3*/ curfptr[2];BASILYS_LOCATION("warmelt-normatch.bysl:66:/ multimsend.arg");
    argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.NCX__V4*/ curfptr[3];BASILYS_LOCATION("warmelt-normatch.bysl:66:/ multimsend.arg");
    argtab[2].bp_aptr = (basilys_ptr_t*) &/*_.SLOC__V15*/ curfptr[14];
    BASILYS_LOCATION("warmelt-normatch.bysl:66:/ multimsend.xres");
    restab[0].bp_aptr = (basilys_ptr_t*) &/*_.NBINDMATX__V26*/ curfptr[25];BASILYS_LOCATION("warmelt-normatch.bysl:66:/ multimsend.send");
    /*_.NMATX__V25*/ curfptr[24] =  basilysgc_send ((basilys_ptr_t)(/*_.SMATSX__V16*/ curfptr[15]), ((basilys_ptr_t)((/*!NORMAL_EXP*/ curfrout->tabval[6]))), (BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR ""), argtab, (BPARSTR_PTR ""), restab);
    }
   ;
   BASILYS_LOCATION("warmelt-normatch.bysl:66:/ block");
   /*block*/{
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:69:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:69:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L6*/ curfnum[0] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:69:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:69:/ apply.arg");
       argtab[0].bp_cstring =  "normexp_match nmatx";
       BASILYS_LOCATION("warmelt-normatch.bysl:69:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L6*/ curfnum[0];
       BASILYS_LOCATION("warmelt-normatch.bysl:69:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:69:/ apply.arg");
       argtab[3].bp_long = 69;
       /*_.DEBUG_MSG_FUN__V28*/ curfptr[27] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.NMATX__V25*/ curfptr[24]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V27*/ curfptr[26] = /*_.DEBUG_MSG_FUN__V28*/ curfptr[27];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:69:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L6*/ curfnum[0] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:69:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V28*/ curfptr[27] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:69:/ cppif.else");
    /*_.IFCPP___V27*/ curfptr[26] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    
    #if ENABLE_CHECKING
     BASILYS_LOCATION("warmelt-normatch.bysl:70:/ cppif.then");
    BASILYS_LOCATION("warmelt-normatch.bysl:70:/ block");
     /*block*/{
      /*_#THE_CALLCOUNT__L7*/ curfnum[0] = 
       callcount;;
      BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply");
      /*apply*/{
       union basilysparam_un argtab[4];
       memset(&argtab, 0, sizeof(argtab));
       BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
       argtab[0].bp_cstring =  "normexp_match scases";
       BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
       argtab[1].bp_long = /*_#THE_CALLCOUNT__L7*/ curfnum[0];
       BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
       argtab[2].bp_cstring =  "warmelt-normatch.bysl";
       BASILYS_LOCATION("warmelt-normatch.bysl:70:/ apply.arg");
       argtab[3].bp_long = 70;
       /*_.DEBUG_MSG_FUN__V30*/ curfptr[29] =  basilys_apply ((basilysclosure_ptr_t)((/*!DEBUG_MSG_FUN*/ curfrout->tabval[3])), (basilys_ptr_t)(/*_.SCASES__V17*/ curfptr[16]), (BPARSTR_CSTRING BPARSTR_LONG BPARSTR_CSTRING BPARSTR_LONG ""), argtab, "", (union basilysparam_un*)0);
       }
      ;
      /*_.IFCPP___V29*/ curfptr[27] = /*_.DEBUG_MSG_FUN__V30*/ curfptr[29];;
      /*epilog*/
      BASILYS_LOCATION("warmelt-normatch.bysl:70:/ clear");
      /*clear*/ /*_#THE_CALLCOUNT__L7*/ curfnum[0] = 0 ;
      BASILYS_LOCATION("warmelt-normatch.bysl:70:/ clear");
      /*clear*/ /*_.DEBUG_MSG_FUN__V30*/ curfptr[29] = 0 ;}
     
     #else /*ENABLE_CHECKING*/
     BASILYS_LOCATION("warmelt-normatch.bysl:70:/ cppif.else");
    /*_.IFCPP___V29*/ curfptr[27] = (/*nil*/NULL);
     #endif /*ENABLE_CHECKING*/
     ;
    /*_.MULTI___V24*/ curfptr[22] = /*_.IFCPP___V29*/ curfptr[27];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:66:/ clear");
    /*clear*/ /*_.IFCPP___V27*/ curfptr[26] = 0 ;
    BASILYS_LOCATION("warmelt-normatch.bysl:66:/ clear");
    /*clear*/ /*_.IFCPP___V29*/ curfptr[27] = 0 ;}
   ;
   /*epilog*/
   BASILYS_LOCATION("warmelt-normatch.bysl:66:/ clear");
   /*clear*/ /*_.NBINDMATX__V26*/ curfptr[25] = 0 ;}
  ;
  
  #if ENABLE_CHECKING
   BASILYS_LOCATION("warmelt-normatch.bysl:72:/ cppif.then");
  BASILYS_LOCATION("warmelt-normatch.bysl:72:/ block");
   /*block*/{
    BASILYS_LOCATION("warmelt-normatch.bysl:72:/ cond");
    /*cond*/ if ((/*nil*/NULL)) /*then*/ {
    BASILYS_LOCATION("warmelt-normatch.bysl:72:/ cond.then");
    /*_.IFELSE___V32*/ curfptr[26] = (/*nil*/NULL);;
    } else {BASILYS_LOCATION("warmelt-normatch.bysl:72:/ cond.else");
    
     BASILYS_LOCATION("warmelt-normatch.bysl:72:/ block");
     /*block*/{
      /*block*/{
       
       basilys_assert_failed(( "unimplemented normexp_match"),( "warmelt-normatch.bysl")?( "warmelt-normatch.bysl"):__FILE__,(72)?(72):__LINE__, __FUNCTION__);
       /*clear*/ /*_.IFELSE___V32*/ curfptr[26] = 0 ;}
      ;
      /*epilog*/}
     ;
     }
    ;
    /*_.IFCPP___V31*/ curfptr[29] = /*_.IFELSE___V32*/ curfptr[26];;
    /*epilog*/
    BASILYS_LOCATION("warmelt-normatch.bysl:72:/ clear");
    /*clear*/ /*_.IFELSE___V32*/ curfptr[26] = 0 ;}
   
   #else /*ENABLE_CHECKING*/
   BASILYS_LOCATION("warmelt-normatch.bysl:72:/ cppif.else");
  /*_.IFCPP___V31*/ curfptr[29] = (/*nil*/NULL);
   #endif /*ENABLE_CHECKING*/
   ;
  /*_.LET___V14*/ curfptr[12] = (/*nil*/NULL);;
  /*epilog*/
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.SLOC__V15*/ curfptr[14] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.SMATSX__V16*/ curfptr[15] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.SCASES__V17*/ curfptr[16] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.SELSE__V18*/ curfptr[17] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.MAKE_MAPOBJECT__V19*/ curfptr[18] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.PCTX__V20*/ curfptr[19] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.IFCPP___V22*/ curfptr[21] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.MULTI___V24*/ curfptr[22] = 0 ;
  BASILYS_LOCATION("warmelt-normatch.bysl:55:/ clear");
  /*clear*/ /*_.IFCPP___V31*/ curfptr[29] = 0 ;}
 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ block");
 /*block*/{
  /*_.RETVAL___V1*/ curfptr[0] = /*_.LET___V14*/ curfptr[12];;
  BASILYS_LOCATION("warmelt-normatch.bysl:50:/ finalreturn");
  ;
  /*finalret*/ goto labend_rout ;}
 ;
 /*epilog*/
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ clear");
 /*clear*/ /*_.IFCPP___V6*/ curfptr[5] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ clear");
 /*clear*/ /*_.IFCPP___V8*/ curfptr[6] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ clear");
 /*clear*/ /*_.IFCPP___V10*/ curfptr[8] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ clear");
 /*clear*/ /*_.IFCPP___V12*/ curfptr[10] = 0 ;
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ clear");
 /*clear*/ /*_.LET___V14*/ curfptr[12] = 0 ;}

;
 goto labend_rout;
labend_rout:
basilys_trace_end("NORMEXP_MATCH", callcount);  basilys_topframe = (struct callframe_basilys_st*) curfram__.prev;
 return (basilys_ptr_t)(/*_.RETVAL___V1*/ curfptr[0]);
#undef callcount
#undef CURFRAM_NBVARNUM
#undef CURFRAM_NBVARPTR
} /*end rout_1_NORMEXP_MATCH*/



typedef 
 struct frame_start_module_basilys_st { unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *noinitialclos;
  struct excepth_basilys_st *exh;
  struct callframe_basilys_st *prev;
#define CURFRAM_NBVARPTR 63
  void* varptr[63];
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
  struct BASILYS_MULTIPLE_STRUCT(3) dtup_8__CLASS_PATTERNCONTEXT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_9__PCTN_NORMCTXT;
  struct BASILYS_STRING_STRUCT(13) dstr_10__PCTN_NORMCTXT;
  struct BASILYS_OBJECT_STRUCT(4) dobj_11__PCTN_NORMCTXT;
  struct BASILYS_STRING_STRUCT(13) dstr_12__PCTN_NORMCTXT;
  struct BASILYS_OBJECT_STRUCT(3) dsym_13__PCTN_SRC;
  struct BASILYS_STRING_STRUCT(8) dstr_14__PCTN_SRC;
  struct BASILYS_OBJECT_STRUCT(4) dobj_15__PCTN_SRC;
  struct BASILYS_STRING_STRUCT(8) dstr_16__PCTN_SRC;
  struct BASILYS_OBJECT_STRUCT(3) dsym_17__PCTN_MAPATVAR;
  struct BASILYS_STRING_STRUCT(13) dstr_18__PCTN_MAPATVAR;
  struct BASILYS_OBJECT_STRUCT(4) dobj_19__PCTN_MAPATVAR;
  struct BASILYS_STRING_STRUCT(13) dstr_20__PCTN_MAPATVAR;
  struct BASILYS_OBJECT_STRUCT(3) dsym_21__FIND_PATVAR;
  struct BASILYS_STRING_STRUCT(11) dstr_22__FIND_PATVAR;
  struct BASILYS_OBJECT_STRUCT(3) dsym_23__CLASS_SELECTOR;
  struct BASILYS_STRING_STRUCT(14) dstr_24__CLASS_SELECTOR;
  struct BASILYS_OBJECT_STRUCT(4) dobj_25__FIND_PATVAR;
  struct BASILYS_STRING_STRUCT(11) dstr_26__FIND_PATVAR;
  struct BASILYS_ROUTINE_STRUCT(7) drout_27__NORMEXP_MATCH;
  struct BASILYS_CLOSURE_STRUCT(0) dclo_28__NORMEXP_MATCH;
  struct BASILYS_OBJECT_STRUCT(3) dsym_29__CLASS_SRC_MATCH;
  struct BASILYS_STRING_STRUCT(15) dstr_30__CLASS_SRC_MATCH;
  struct BASILYS_OBJECT_STRUCT(3) dsym_31__CLASS_ENVIRONMENT;
  struct BASILYS_STRING_STRUCT(17) dstr_32__CLASS_ENVIRONMEN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_33__CLASS_NORMCONTEXT;
  struct BASILYS_STRING_STRUCT(17) dstr_34__CLASS_NORMCONTEX;
  struct BASILYS_OBJECT_STRUCT(3) dsym_35__DEBUG_MSG_FUN;
  struct BASILYS_STRING_STRUCT(13) dstr_36__DEBUG_MSG_FUN;
  struct BASILYS_OBJECT_STRUCT(3) dsym_37__DISCR_MAPOBJECTS;
  struct BASILYS_STRING_STRUCT(16) dstr_38__DISCR_MAPOBJECTS;
  struct BASILYS_OBJECT_STRUCT(3) dsym_39__NORMAL_EXP;
  struct BASILYS_STRING_STRUCT(10) dstr_40__NORMAL_EXP;
  struct BASILYS_OBJECT_STRUCT(3) dsym_41__INSTALL_METHOD;
  struct BASILYS_STRING_STRUCT(14) dstr_42__INSTALL_METHOD;
 long spare_;
} *cdat = NULL;
 cdat = (struct cdata_st*) basilysgc_allocate(sizeof(*cdat),0);
  basilys_prohibit_garbcoll = TRUE;
 /*initial routine predef*/
 /*initial routine fill*/
 
 /*iniobj dsym_1__CLASS_PATTERNCONTEXT*/
 if (!/*_.VALDATA___V16*/ curfptr[15]) /*_.VALDATA___V16*/ curfptr[15] = (void*)&cdat->dsym_1__CLASS_PATTERNCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dsym_1__CLASS_PATTERNCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_hash = 411470679;
  cdat->dsym_1__CLASS_PATTERNCONTEXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_1__CLASS_PATTERNCONTEXT));
 
 /*inistring dstr_2__CLASS_PATTERNCON*/
 /*_.VALSTR___V17*/ curfptr[16] = (void*)&cdat->dstr_2__CLASS_PATTERNCON;
  cdat->dstr_2__CLASS_PATTERNCON.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_2__CLASS_PATTERNCON.val, "CLASS_PATTERNCONTEXT");
 
 /*iniobj dsym_3__CLASS_ROOT*/
 if (!/*_.VALDATA___V18*/ curfptr[17]) /*_.VALDATA___V18*/ curfptr[17] = (void*)&cdat->dsym_3__CLASS_ROOT;
  basilys_assertmsg("iniobj checkdiscr dsym_3__CLASS_ROOT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_3__CLASS_ROOT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_3__CLASS_ROOT.obj_hash = 60568825;
  cdat->dsym_3__CLASS_ROOT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_3__CLASS_ROOT));
 
 /*inistring dstr_4__CLASS_ROOT*/
 /*_.VALSTR___V19*/ curfptr[18] = (void*)&cdat->dstr_4__CLASS_ROOT;
  cdat->dstr_4__CLASS_ROOT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_4__CLASS_ROOT.val, "CLASS_ROOT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_5__CLASS_PATTERNCONTEXT*/
 if (!/*_.VALDATA___V20*/ curfptr[19]) /*_.VALDATA___V20*/ curfptr[19] = (void*)&cdat->dobj_5__CLASS_PATTERNCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dobj_5__CLASS_PATTERNCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_CLASS))));
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_num = OBMAG_OBJECT;
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_hash = 925345237;
  cdat->dobj_5__CLASS_PATTERNCONTEXT.obj_len = 9;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_5__CLASS_PATTERNCONTEXT));
 
 /*inistring dstr_6__CLASS_PATTERNCON*/
 /*_.VALSTR___V21*/ curfptr[20] = (void*)&cdat->dstr_6__CLASS_PATTERNCON;
  cdat->dstr_6__CLASS_PATTERNCON.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_6__CLASS_PATTERNCON.val, "CLASS_PATTERNCONTEXT");
 
 /*inimult dtup_7__CLASS_PATTERNCONTEXT*/
 /*_.VALTUP___V22*/ curfptr[21] = (void*)&cdat->dtup_7__CLASS_PATTERNCONTEXT;
  cdat->dtup_7__CLASS_PATTERNCONTEXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQCLASS))));
  cdat->dtup_7__CLASS_PATTERNCONTEXT.nbval = 1;
 /*inimult dtup_8__CLASS_PATTERNCONTEXT*/
 /*_.VALTUP___V29*/ curfptr[28] = (void*)&cdat->dtup_8__CLASS_PATTERNCONTEXT;
  cdat->dtup_8__CLASS_PATTERNCONTEXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_SEQFIELD))));
  cdat->dtup_8__CLASS_PATTERNCONTEXT.nbval = 3;
 /*iniobj dsym_9__PCTN_NORMCTXT*/
 if (!/*_.VALDATA___V30*/ curfptr[29]) /*_.VALDATA___V30*/ curfptr[29] = (void*)&cdat->dsym_9__PCTN_NORMCTXT;
  basilys_assertmsg("iniobj checkdiscr dsym_9__PCTN_NORMCTXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_9__PCTN_NORMCTXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_9__PCTN_NORMCTXT.obj_hash = 955957534;
  cdat->dsym_9__PCTN_NORMCTXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_9__PCTN_NORMCTXT));
 
 /*inistring dstr_10__PCTN_NORMCTXT*/
 /*_.VALSTR___V31*/ curfptr[30] = (void*)&cdat->dstr_10__PCTN_NORMCTXT;
  cdat->dstr_10__PCTN_NORMCTXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_10__PCTN_NORMCTXT.val, "PCTN_NORMCTXT");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_11__PCTN_NORMCTXT*/
 if (!/*_.VALDATA___V23*/ curfptr[22]) /*_.VALDATA___V23*/ curfptr[22] = (void*)&cdat->dobj_11__PCTN_NORMCTXT;
  basilys_assertmsg("iniobj checkdiscr dobj_11__PCTN_NORMCTXT", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_11__PCTN_NORMCTXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_11__PCTN_NORMCTXT.obj_num = 0;
  cdat->dobj_11__PCTN_NORMCTXT.obj_hash = 221813469;
  cdat->dobj_11__PCTN_NORMCTXT.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_11__PCTN_NORMCTXT));
 
 /*inistring dstr_12__PCTN_NORMCTXT*/
 /*_.VALSTR___V24*/ curfptr[23] = (void*)&cdat->dstr_12__PCTN_NORMCTXT;
  cdat->dstr_12__PCTN_NORMCTXT.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_12__PCTN_NORMCTXT.val, "PCTN_NORMCTXT");
 
 /*iniobj dsym_13__PCTN_SRC*/
 if (!/*_.VALDATA___V32*/ curfptr[31]) /*_.VALDATA___V32*/ curfptr[31] = (void*)&cdat->dsym_13__PCTN_SRC;
  basilys_assertmsg("iniobj checkdiscr dsym_13__PCTN_SRC", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_13__PCTN_SRC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_13__PCTN_SRC.obj_hash = 902509187;
  cdat->dsym_13__PCTN_SRC.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_13__PCTN_SRC));
 
 /*inistring dstr_14__PCTN_SRC*/
 /*_.VALSTR___V33*/ curfptr[32] = (void*)&cdat->dstr_14__PCTN_SRC;
  cdat->dstr_14__PCTN_SRC.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_14__PCTN_SRC.val, "PCTN_SRC");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_15__PCTN_SRC*/
 if (!/*_.VALDATA___V25*/ curfptr[24]) /*_.VALDATA___V25*/ curfptr[24] = (void*)&cdat->dobj_15__PCTN_SRC;
  basilys_assertmsg("iniobj checkdiscr dobj_15__PCTN_SRC", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_15__PCTN_SRC.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_15__PCTN_SRC.obj_num = 1;
  cdat->dobj_15__PCTN_SRC.obj_hash = 544215519;
  cdat->dobj_15__PCTN_SRC.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_15__PCTN_SRC));
 
 /*inistring dstr_16__PCTN_SRC*/
 /*_.VALSTR___V26*/ curfptr[25] = (void*)&cdat->dstr_16__PCTN_SRC;
  cdat->dstr_16__PCTN_SRC.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_16__PCTN_SRC.val, "PCTN_SRC");
 
 /*iniobj dsym_17__PCTN_MAPATVAR*/
 if (!/*_.VALDATA___V34*/ curfptr[33]) /*_.VALDATA___V34*/ curfptr[33] = (void*)&cdat->dsym_17__PCTN_MAPATVAR;
  basilys_assertmsg("iniobj checkdiscr dsym_17__PCTN_MAPATVAR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_17__PCTN_MAPATVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_17__PCTN_MAPATVAR.obj_hash = 620129723;
  cdat->dsym_17__PCTN_MAPATVAR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_17__PCTN_MAPATVAR));
 
 /*inistring dstr_18__PCTN_MAPATVAR*/
 /*_.VALSTR___V35*/ curfptr[34] = (void*)&cdat->dstr_18__PCTN_MAPATVAR;
  cdat->dstr_18__PCTN_MAPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_18__PCTN_MAPATVAR.val, "PCTN_MAPATVAR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ iniobj");
 /*iniobj dobj_19__PCTN_MAPATVAR*/
 if (!/*_.VALDATA___V27*/ curfptr[26]) /*_.VALDATA___V27*/ curfptr[26] = (void*)&cdat->dobj_19__PCTN_MAPATVAR;
  basilys_assertmsg("iniobj checkdiscr dobj_19__PCTN_MAPATVAR", NULL != (void*)((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_19__PCTN_MAPATVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_FIELD))));
  cdat->dobj_19__PCTN_MAPATVAR.obj_num = 2;
  cdat->dobj_19__PCTN_MAPATVAR.obj_hash = 198806273;
  cdat->dobj_19__PCTN_MAPATVAR.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_19__PCTN_MAPATVAR));
 
 /*inistring dstr_20__PCTN_MAPATVAR*/
 /*_.VALSTR___V28*/ curfptr[27] = (void*)&cdat->dstr_20__PCTN_MAPATVAR;
  cdat->dstr_20__PCTN_MAPATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_20__PCTN_MAPATVAR.val, "PCTN_MAPATVAR");
 
 /*iniobj dsym_21__FIND_PATVAR*/
 if (!/*_.VALDATA___V36*/ curfptr[35]) /*_.VALDATA___V36*/ curfptr[35] = (void*)&cdat->dsym_21__FIND_PATVAR;
  basilys_assertmsg("iniobj checkdiscr dsym_21__FIND_PATVAR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_21__FIND_PATVAR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_21__FIND_PATVAR.obj_hash = 817644981;
  cdat->dsym_21__FIND_PATVAR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_21__FIND_PATVAR));
 
 /*inistring dstr_22__FIND_PATVAR*/
 /*_.VALSTR___V37*/ curfptr[36] = (void*)&cdat->dstr_22__FIND_PATVAR;
  cdat->dstr_22__FIND_PATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_22__FIND_PATVAR.val, "FIND_PATVAR");
 
 /*iniobj dsym_23__CLASS_SELECTOR*/
 if (!/*_.VALDATA___V38*/ curfptr[37]) /*_.VALDATA___V38*/ curfptr[37] = (void*)&cdat->dsym_23__CLASS_SELECTOR;
  basilys_assertmsg("iniobj checkdiscr dsym_23__CLASS_SELECTOR", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_23__CLASS_SELECTOR.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_23__CLASS_SELECTOR.obj_hash = 100646971;
  cdat->dsym_23__CLASS_SELECTOR.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_23__CLASS_SELECTOR));
 
 /*inistring dstr_24__CLASS_SELECTOR*/
 /*_.VALSTR___V39*/ curfptr[38] = (void*)&cdat->dstr_24__CLASS_SELECTOR;
  cdat->dstr_24__CLASS_SELECTOR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_24__CLASS_SELECTOR.val, "CLASS_SELECTOR");
 
 BASILYS_LOCATION("warmelt-normatch.bysl:45:/ iniobj");
 /*iniobj dobj_25__FIND_PATVAR*/
 if (!/*_.VALDATA___V40*/ curfptr[39]) /*_.VALDATA___V40*/ curfptr[39] = (void*)&cdat->dobj_25__FIND_PATVAR;
  basilys_assertmsg("iniobj checkdiscr dobj_25__FIND_PATVAR", NULL != (void*)/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_25__FIND_PATVAR.obj_class = (basilysobject_ptr_t)(/*_.CLASS_SELECTOR__V8*/ curfptr[7]);
  cdat->dobj_25__FIND_PATVAR.obj_hash = 8403451;
  cdat->dobj_25__FIND_PATVAR.obj_len = 4;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dobj_25__FIND_PATVAR));
 
 /*inistring dstr_26__FIND_PATVAR*/
 /*_.VALSTR___V41*/ curfptr[40] = (void*)&cdat->dstr_26__FIND_PATVAR;
  cdat->dstr_26__FIND_PATVAR.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_26__FIND_PATVAR.val, "FIND_PATVAR");
 
 /*inirout drout_27__NORMEXP_MATCH*/
 /*_.VALROUT___V42*/ curfptr[41] = (void*)&cdat->drout_27__NORMEXP_MATCH;
  cdat->drout_27__NORMEXP_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_ROUTINE))));
  strncpy(cdat->drout_27__NORMEXP_MATCH.routdescr, "NORMEXP_MATCH @warmelt-normatch.bysl:50",  BASILYS_ROUTDESCR_LEN - 1);
  cdat->drout_27__NORMEXP_MATCH.nbval = 7;
 *(basilysroutfun_t **) (cdat->drout_27__NORMEXP_MATCH.routaddr) = rout_1_NORMEXP_MATCH;
 
 /*iniclos dclo_28__NORMEXP_MATCH*/
 /*_.VALCLO___V43*/ curfptr[42] = (void*)&cdat->dclo_28__NORMEXP_MATCH;
  cdat->dclo_28__NORMEXP_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_CLOSURE))));
  cdat->dclo_28__NORMEXP_MATCH.nbval = 0;
 
 /*iniobj dsym_29__CLASS_SRC_MATCH*/
 if (!/*_.VALDATA___V44*/ curfptr[43]) /*_.VALDATA___V44*/ curfptr[43] = (void*)&cdat->dsym_29__CLASS_SRC_MATCH;
  basilys_assertmsg("iniobj checkdiscr dsym_29__CLASS_SRC_MATCH", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_29__CLASS_SRC_MATCH.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_29__CLASS_SRC_MATCH.obj_hash = 898626999;
  cdat->dsym_29__CLASS_SRC_MATCH.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_29__CLASS_SRC_MATCH));
 
 /*inistring dstr_30__CLASS_SRC_MATCH*/
 /*_.VALSTR___V45*/ curfptr[44] = (void*)&cdat->dstr_30__CLASS_SRC_MATCH;
  cdat->dstr_30__CLASS_SRC_MATCH.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_30__CLASS_SRC_MATCH.val, "CLASS_SRC_MATCH");
 
 /*iniobj dsym_31__CLASS_ENVIRONMENT*/
 if (!/*_.VALDATA___V46*/ curfptr[45]) /*_.VALDATA___V46*/ curfptr[45] = (void*)&cdat->dsym_31__CLASS_ENVIRONMENT;
  basilys_assertmsg("iniobj checkdiscr dsym_31__CLASS_ENVIRONMENT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_31__CLASS_ENVIRONMENT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_31__CLASS_ENVIRONMENT.obj_hash = 59212821;
  cdat->dsym_31__CLASS_ENVIRONMENT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_31__CLASS_ENVIRONMENT));
 
 /*inistring dstr_32__CLASS_ENVIRONMEN*/
 /*_.VALSTR___V47*/ curfptr[46] = (void*)&cdat->dstr_32__CLASS_ENVIRONMEN;
  cdat->dstr_32__CLASS_ENVIRONMEN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_32__CLASS_ENVIRONMEN.val, "CLASS_ENVIRONMENT");
 
 /*iniobj dsym_33__CLASS_NORMCONTEXT*/
 if (!/*_.VALDATA___V48*/ curfptr[47]) /*_.VALDATA___V48*/ curfptr[47] = (void*)&cdat->dsym_33__CLASS_NORMCONTEXT;
  basilys_assertmsg("iniobj checkdiscr dsym_33__CLASS_NORMCONTEXT", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_33__CLASS_NORMCONTEXT.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_33__CLASS_NORMCONTEXT.obj_hash = 317209306;
  cdat->dsym_33__CLASS_NORMCONTEXT.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_33__CLASS_NORMCONTEXT));
 
 /*inistring dstr_34__CLASS_NORMCONTEX*/
 /*_.VALSTR___V49*/ curfptr[48] = (void*)&cdat->dstr_34__CLASS_NORMCONTEX;
  cdat->dstr_34__CLASS_NORMCONTEX.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_34__CLASS_NORMCONTEX.val, "CLASS_NORMCONTEXT");
 
 /*iniobj dsym_35__DEBUG_MSG_FUN*/
 if (!/*_.VALDATA___V50*/ curfptr[49]) /*_.VALDATA___V50*/ curfptr[49] = (void*)&cdat->dsym_35__DEBUG_MSG_FUN;
  basilys_assertmsg("iniobj checkdiscr dsym_35__DEBUG_MSG_FUN", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_35__DEBUG_MSG_FUN.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_35__DEBUG_MSG_FUN.obj_hash = 938829072;
  cdat->dsym_35__DEBUG_MSG_FUN.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_35__DEBUG_MSG_FUN));
 
 /*inistring dstr_36__DEBUG_MSG_FUN*/
 /*_.VALSTR___V51*/ curfptr[50] = (void*)&cdat->dstr_36__DEBUG_MSG_FUN;
  cdat->dstr_36__DEBUG_MSG_FUN.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_36__DEBUG_MSG_FUN.val, "DEBUG_MSG_FUN");
 
 /*iniobj dsym_37__DISCR_MAPOBJECTS*/
 if (!/*_.VALDATA___V52*/ curfptr[51]) /*_.VALDATA___V52*/ curfptr[51] = (void*)&cdat->dsym_37__DISCR_MAPOBJECTS;
  basilys_assertmsg("iniobj checkdiscr dsym_37__DISCR_MAPOBJECTS", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_37__DISCR_MAPOBJECTS.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_37__DISCR_MAPOBJECTS.obj_hash = 95669218;
  cdat->dsym_37__DISCR_MAPOBJECTS.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_37__DISCR_MAPOBJECTS));
 
 /*inistring dstr_38__DISCR_MAPOBJECTS*/
 /*_.VALSTR___V53*/ curfptr[52] = (void*)&cdat->dstr_38__DISCR_MAPOBJECTS;
  cdat->dstr_38__DISCR_MAPOBJECTS.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_38__DISCR_MAPOBJECTS.val, "DISCR_MAPOBJECTS");
 
 /*iniobj dsym_39__NORMAL_EXP*/
 if (!/*_.VALDATA___V54*/ curfptr[53]) /*_.VALDATA___V54*/ curfptr[53] = (void*)&cdat->dsym_39__NORMAL_EXP;
  basilys_assertmsg("iniobj checkdiscr dsym_39__NORMAL_EXP", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_39__NORMAL_EXP.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_39__NORMAL_EXP.obj_hash = 420282450;
  cdat->dsym_39__NORMAL_EXP.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_39__NORMAL_EXP));
 
 /*inistring dstr_40__NORMAL_EXP*/
 /*_.VALSTR___V55*/ curfptr[54] = (void*)&cdat->dstr_40__NORMAL_EXP;
  cdat->dstr_40__NORMAL_EXP.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_40__NORMAL_EXP.val, "NORMAL_EXP");
 
 /*iniobj dsym_41__INSTALL_METHOD*/
 if (!/*_.VALDATA___V56*/ curfptr[55]) /*_.VALDATA___V56*/ curfptr[55] = (void*)&cdat->dsym_41__INSTALL_METHOD;
  basilys_assertmsg("iniobj checkdiscr dsym_41__INSTALL_METHOD", NULL != (void*)((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_41__INSTALL_METHOD.obj_class = (basilysobject_ptr_t)(((void*)(BASILYSG(CLASS_SYMBOL))));
  cdat->dsym_41__INSTALL_METHOD.obj_hash = 680699224;
  cdat->dsym_41__INSTALL_METHOD.obj_len = 3;
basilys_object_set_serial((basilysobject_ptr_t) (&cdat->dsym_41__INSTALL_METHOD));
 
 /*inistring dstr_42__INSTALL_METHOD*/
 /*_.VALSTR___V57*/ curfptr[56] = (void*)&cdat->dstr_42__INSTALL_METHOD;
  cdat->dstr_42__INSTALL_METHOD.discr = (basilysobject_ptr_t)(((void*)(BASILYSG(DISCR_STRING))));
  strcpy(cdat->dstr_42__INSTALL_METHOD.val, "INSTALL_METHOD");
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
 curfram__.nbvar = 63;
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
  if (sy_CLASS_PATTERNCONTEXT && NULL == /*_.VALDATA___V16*/ curfptr[15])
  /*_.VALDATA___V16*/ curfptr[15] = (void*) sy_CLASS_PATTERNCONTEXT; }
 
 /*getnamedsym:CLASS_ROOT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ROOT = basilysgc_named_symbol("CLASS_ROOT", BASILYS_GET);
  if (sy_CLASS_ROOT && NULL == /*_.VALDATA___V18*/ curfptr[17])
  /*_.VALDATA___V18*/ curfptr[17] = (void*) sy_CLASS_ROOT; }
 
 /*getnamedsym:PCTN_NORMCTXT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_NORMCTXT = basilysgc_named_symbol("PCTN_NORMCTXT", BASILYS_GET);
  if (sy_PCTN_NORMCTXT && NULL == /*_.VALDATA___V30*/ curfptr[29])
  /*_.VALDATA___V30*/ curfptr[29] = (void*) sy_PCTN_NORMCTXT; }
 
 /*getnamedsym:PCTN_SRC*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_SRC = basilysgc_named_symbol("PCTN_SRC", BASILYS_GET);
  if (sy_PCTN_SRC && NULL == /*_.VALDATA___V32*/ curfptr[31])
  /*_.VALDATA___V32*/ curfptr[31] = (void*) sy_PCTN_SRC; }
 
 /*getnamedsym:PCTN_MAPATVAR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_PCTN_MAPATVAR = basilysgc_named_symbol("PCTN_MAPATVAR", BASILYS_GET);
  if (sy_PCTN_MAPATVAR && NULL == /*_.VALDATA___V34*/ curfptr[33])
  /*_.VALDATA___V34*/ curfptr[33] = (void*) sy_PCTN_MAPATVAR; }
 
 /*getnamedsym:FIND_PATVAR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_FIND_PATVAR = basilysgc_named_symbol("FIND_PATVAR", BASILYS_GET);
  if (sy_FIND_PATVAR && NULL == /*_.VALDATA___V36*/ curfptr[35])
  /*_.VALDATA___V36*/ curfptr[35] = (void*) sy_FIND_PATVAR; }
 
 /*getnamedsym:CLASS_SELECTOR*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SELECTOR = basilysgc_named_symbol("CLASS_SELECTOR", BASILYS_GET);
  if (sy_CLASS_SELECTOR && NULL == /*_.VALDATA___V38*/ curfptr[37])
  /*_.VALDATA___V38*/ curfptr[37] = (void*) sy_CLASS_SELECTOR; }
 
 /*getnamedsym:CLASS_SRC_MATCH*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_SRC_MATCH = basilysgc_named_symbol("CLASS_SRC_MATCH", BASILYS_GET);
  if (sy_CLASS_SRC_MATCH && NULL == /*_.VALDATA___V44*/ curfptr[43])
  /*_.VALDATA___V44*/ curfptr[43] = (void*) sy_CLASS_SRC_MATCH; }
 
 /*getnamedsym:CLASS_ENVIRONMENT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_ENVIRONMENT = basilysgc_named_symbol("CLASS_ENVIRONMENT", BASILYS_GET);
  if (sy_CLASS_ENVIRONMENT && NULL == /*_.VALDATA___V46*/ curfptr[45])
  /*_.VALDATA___V46*/ curfptr[45] = (void*) sy_CLASS_ENVIRONMENT; }
 
 /*getnamedsym:CLASS_NORMCONTEXT*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_CLASS_NORMCONTEXT = basilysgc_named_symbol("CLASS_NORMCONTEXT", BASILYS_GET);
  if (sy_CLASS_NORMCONTEXT && NULL == /*_.VALDATA___V48*/ curfptr[47])
  /*_.VALDATA___V48*/ curfptr[47] = (void*) sy_CLASS_NORMCONTEXT; }
 
 /*getnamedsym:DEBUG_MSG_FUN*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DEBUG_MSG_FUN = basilysgc_named_symbol("DEBUG_MSG_FUN", BASILYS_GET);
  if (sy_DEBUG_MSG_FUN && NULL == /*_.VALDATA___V50*/ curfptr[49])
  /*_.VALDATA___V50*/ curfptr[49] = (void*) sy_DEBUG_MSG_FUN; }
 
 /*getnamedsym:DISCR_MAPOBJECTS*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_DISCR_MAPOBJECTS = basilysgc_named_symbol("DISCR_MAPOBJECTS", BASILYS_GET);
  if (sy_DISCR_MAPOBJECTS && NULL == /*_.VALDATA___V52*/ curfptr[51])
  /*_.VALDATA___V52*/ curfptr[51] = (void*) sy_DISCR_MAPOBJECTS; }
 
 /*getnamedsym:NORMAL_EXP*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_NORMAL_EXP = basilysgc_named_symbol("NORMAL_EXP", BASILYS_GET);
  if (sy_NORMAL_EXP && NULL == /*_.VALDATA___V54*/ curfptr[53])
  /*_.VALDATA___V54*/ curfptr[53] = (void*) sy_NORMAL_EXP; }
 
 /*getnamedsym:INSTALL_METHOD*/
 if (BASILYSG(INITIAL_SYSTEM_DATA)) { basilys_ptr_t sy_INSTALL_METHOD = basilysgc_named_symbol("INSTALL_METHOD", BASILYS_GET);
  if (sy_INSTALL_METHOD && NULL == /*_.VALDATA___V56*/ curfptr[55])
  /*_.VALDATA___V56*/ curfptr[55] = (void*) sy_INSTALL_METHOD; }
 
 /**COMMENT: get the value importer **/;
 
 { basilys_ptr_t slot=0;
  basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 12, "SYSDATA_VALUE_IMPORTER");/*_.VALIMPORT___V6*/ curfptr[5] = slot; };
 
 /**COMMENT: before getting imported values **/;
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ROOT__V7*/ curfptr[6] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V18*/ curfptr[17]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SELECTOR__V8*/ curfptr[7] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V38*/ curfptr[37]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_SRC_MATCH__V9*/ curfptr[8] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V44*/ curfptr[43]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_ENVIRONMENT__V10*/ curfptr[9] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.CLASS_NORMCONTEXT__V11*/ curfptr[10] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DEBUG_MSG_FUN__V12*/ curfptr[11] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.DISCR_MAPOBJECTS__V13*/ curfptr[12] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.NORMAL_EXP__V14*/ curfptr[13] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /*apply*/{
  union basilysparam_un argtab[1];
  memset(&argtab, 0, sizeof(argtab));
  argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.PREVENV___V4*/ curfptr[3];
  /*_.INSTALL_METHOD__V15*/ curfptr[14] =  basilys_apply ((basilysclosure_ptr_t)(/*_.VALIMPORT___V6*/ curfptr[5]), (basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]), (BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
  }
 
 /**COMMENT: after getting imported values **/;
 
 /**initial routine cdata initializer**/

 memset(predefinited, 0, sizeof(predefinited));
 initialize_module_cdata(&curfram__, predefinited);
 /**initial routine body**/
/**COMMENT: start of oinibody **/;
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V16*/ curfptr[15])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V16*/ curfptr[15]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V16*/ curfptr[15]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V17*/ curfptr[16]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V16*/ curfptr[15]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V18*/ curfptr[17])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V18*/ curfptr[17]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V18*/ curfptr[17]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V19*/ curfptr[18]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V18*/ curfptr[17]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V21*/ curfptr[20]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (4>=0 && 4< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))->obj_vartab[4] = (basilys_ptr_t)(
  /*_.CLASS_ROOT__V7*/ curfptr[6]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V22*/ curfptr[21]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V22*/ curfptr[21]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V22*/ curfptr[21]))->tabval[0] = (basilys_ptr_t)(/*_.CLASS_ROOT__V7*/ curfptr[6]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V22*/ curfptr[21]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (5>=0 && 5< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))->obj_vartab[5] = (basilys_ptr_t)(
  /*_.VALTUP___V22*/ curfptr[21]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V23*/ curfptr[22])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V23*/ curfptr[22]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V23*/ curfptr[22]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V24*/ curfptr[23]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V23*/ curfptr[22])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V23*/ curfptr[22]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V23*/ curfptr[22]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V23*/ curfptr[22]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V25*/ curfptr[24])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V25*/ curfptr[24]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V25*/ curfptr[24]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V26*/ curfptr[25]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V25*/ curfptr[24])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V25*/ curfptr[24]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V25*/ curfptr[24]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V25*/ curfptr[24]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V27*/ curfptr[26])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V27*/ curfptr[26]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V27*/ curfptr[26]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V28*/ curfptr[27]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V27*/ curfptr[26])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (2>=0 && 2< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V27*/ curfptr[26]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V27*/ curfptr[26]))->obj_vartab[2] = (basilys_ptr_t)(
  /*_.VALDATA___V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V27*/ curfptr[26]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (0>=0 && 0< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))->tabval[0] = (basilys_ptr_t)(/*_.VALDATA___V23*/ curfptr[22]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (1>=0 && 1< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))->tabval[1] = (basilys_ptr_t)(/*_.VALDATA___V25*/ curfptr[24]);
 
 /*putupl*/
 basilys_assertmsg("putupl checktup", basilys_magic_discr((basilys_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))== OBMAG_MULTIPLE);
 basilys_assertmsg("putupl checkoff", (2>=0 && 2< basilys_multiple_length((basilys_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))));
 ((basilysmultiple_ptr_t)(/*_.VALTUP___V29*/ curfptr[28]))->tabval[2] = (basilys_ptr_t)(/*_.VALDATA___V27*/ curfptr[26]);
 
 /*touch:INITUP_*/
 basilysgc_touch(/*_.VALTUP___V29*/ curfptr[28]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (6>=0 && 6< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]))->obj_vartab[6] = (basilys_ptr_t)(
  /*_.VALTUP___V29*/ curfptr[28]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:33:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V20*/ curfptr[19]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V30*/ curfptr[29])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V30*/ curfptr[29]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V30*/ curfptr[29]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V31*/ curfptr[30]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V30*/ curfptr[29]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V32*/ curfptr[31])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V32*/ curfptr[31]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V32*/ curfptr[31]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V33*/ curfptr[32]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V32*/ curfptr[31]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V34*/ curfptr[33])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V34*/ curfptr[33]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V34*/ curfptr[33]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V35*/ curfptr[34]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V34*/ curfptr[33]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V36*/ curfptr[35])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V36*/ curfptr[35]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V36*/ curfptr[35]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V37*/ curfptr[36]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V36*/ curfptr[35]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V38*/ curfptr[37])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V38*/ curfptr[37]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V38*/ curfptr[37]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V39*/ curfptr[38]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V38*/ curfptr[37]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:45:/ putslot");
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V40*/ curfptr[39])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V40*/ curfptr[39]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V40*/ curfptr[39]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V41*/ curfptr[40]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:45:/ touch");
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V40*/ curfptr[39]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_27__NORMEXP_MATCH#0", NULL != (/*_.CLASS_SRC_MATCH__V9*/ curfptr[8]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V42*/ curfptr[41])->tabval[0] = (basilys_ptr_t)(/*_.CLASS_SRC_MATCH__V9*/ curfptr[8]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_27__NORMEXP_MATCH#1", NULL != (/*_.CLASS_ENVIRONMENT__V10*/ curfptr[9]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V42*/ curfptr[41])->tabval[1] = (basilys_ptr_t)(/*_.CLASS_ENVIRONMENT__V10*/ curfptr[9]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_27__NORMEXP_MATCH#2", NULL != (/*_.CLASS_NORMCONTEXT__V11*/ curfptr[10]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V42*/ curfptr[41])->tabval[2] = (basilys_ptr_t)(/*_.CLASS_NORMCONTEXT__V11*/ curfptr[10]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_27__NORMEXP_MATCH#3", NULL != (/*_.DEBUG_MSG_FUN__V12*/ curfptr[11]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V42*/ curfptr[41])->tabval[3] = (basilys_ptr_t)(/*_.DEBUG_MSG_FUN__V12*/ curfptr[11]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ putroutconstnotnull");
 /*putroutconstnotnull*/
 basilys_assertmsg("putroutconstnotnull checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 basilys_assertmsg("putroutconstnotnull notnullconst", NULL != /*_.VALDATA___V20*/ curfptr[19]);
 ((basilysroutine_ptr_t)/*_.VALROUT___V42*/ curfptr[41])->tabval[4] = (basilys_ptr_t)(/*_.VALDATA___V20*/ curfptr[19]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_27__NORMEXP_MATCH#5", NULL != (/*_.DISCR_MAPOBJECTS__V13*/ curfptr[12]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V42*/ curfptr[41])->tabval[5] = (basilys_ptr_t)(/*_.DISCR_MAPOBJECTS__V13*/ curfptr[12]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ putroutconst");
 /*putroutconst*/
 basilys_assertmsg("putroutconst checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 basilys_checkmsg("putroutconst constnull.drout_27__NORMEXP_MATCH#6", NULL != (/*_.NORMAL_EXP__V14*/ curfptr[13]));
 ((basilysroutine_ptr_t)/*_.VALROUT___V42*/ curfptr[41])->tabval[6] = (basilys_ptr_t)(/*_.NORMAL_EXP__V14*/ curfptr[13]);
 
 BASILYS_LOCATION("warmelt-normatch.bysl:50:/ touch");
 /*touch:IROUTVAL_*/
 basilysgc_touch(/*_.VALROUT___V42*/ curfptr[41]);
 
 /*putclosurout*/
 basilys_assertmsg("putclosrout checkclo", basilys_magic_discr((basilys_ptr_t)(/*_.VALCLO___V43*/ curfptr[42])) == OBMAG_CLOSURE);
 basilys_assertmsg("putclosrout checkrout", basilys_magic_discr((basilys_ptr_t)(/*_.VALROUT___V42*/ curfptr[41])) == OBMAG_ROUTINE);
 ((basilysclosure_ptr_t)/*_.VALCLO___V43*/ curfptr[42])->rout = (basilysroutine_ptr_t) (/*_.VALROUT___V42*/ curfptr[41]);
 
 /*touch:DATACLOSURE_*/
 basilysgc_touch(/*_.VALCLO___V43*/ curfptr[42]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V44*/ curfptr[43])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V44*/ curfptr[43]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V44*/ curfptr[43]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V45*/ curfptr[44]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V44*/ curfptr[43]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V47*/ curfptr[46]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V46*/ curfptr[45]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V49*/ curfptr[48]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V48*/ curfptr[47]);
 
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
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V55*/ curfptr[54]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V54*/ curfptr[53]);
 
 /*putslot*/
 basilys_assertmsg("putslot checkobj", basilys_magic_discr((basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55])) == OBMAG_OBJECT);
 basilys_assertmsg("putslot checkoff", (1>=0 && 1< basilys_object_length((basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]))));
 ((basilysobject_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]))->obj_vartab[1] = (basilys_ptr_t)(
  /*_.VALSTR___V57*/ curfptr[56]);
 
 /*touch:VALUEDATA_*/
 basilysgc_touch(/*_.VALDATA___V56*/ curfptr[55]);
 
 /**COMMENT: before toplevel body **/;
 
 /*block*/{
  /*cond*/ if (
   /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1]) /*then*/ {
  /*_.OR___V58*/ curfptr[57] = 
    /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start*/
/*_.CONTENV___V2*/ curfptr[1];;
  } else {
   /*block*/{
    { basilys_ptr_t slot=0;
     basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");/*_.SYSDATA_CONT_FRESH_ENV__V59*/ curfptr[58] = slot; };
    ;
    /*apply*/{
     union basilysparam_un argtab[1];
     memset(&argtab, 0, sizeof(argtab));
     argtab[0].bp_cstring =  "warmelt-normatch-3.c";
     /*_.FUN___V60*/ curfptr[59] =  basilys_apply ((basilysclosure_ptr_t)(/*_.SYSDATA_CONT_FRESH_ENV__V59*/ curfptr[58]), (basilys_ptr_t)(/*_.PREVENV___V4*/ curfptr[3]), (BPARSTR_CSTRING ""), argtab, "", (union basilysparam_un*)0);
     }
    ;
    /*_.OR___V58*/ curfptr[57] = /*_.FUN___V60*/ curfptr[59];;
    /*epilog*/
    /*clear*/ /*_.SYSDATA_CONT_FRESH_ENV__V59*/ curfptr[58] = 0 ;
    /*clear*/ /*_.FUN___V60*/ curfptr[59] = 0 ;}
   ;
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:3:/ block");
  /*com.block:upd.cur.mod.env.cont : at very start*/{}/*com.end block:upd.cur.mod.env.cont : at very start*/
  ;
  /*epilog*/
  /*clear*/ /*_.OR___V58*/ curfptr[57] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:76:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:76:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.NORMAL_EXP__V14*/ curfptr[13];
   BASILYS_LOCATION("warmelt-normatch.bysl:76:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &/*_.VALCLO___V43*/ curfptr[42];
   /*_.INSTALL_METHOD__V61*/ curfptr[58] =  basilys_apply ((basilysclosure_ptr_t)(/*_.INSTALL_METHOD__V15*/ curfptr[14]), (basilys_ptr_t)(/*_.CLASS_SRC_MATCH__V9*/ curfptr[8]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.INSTALL_METHOD__V61*/ curfptr[58] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:81:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 4, "SYSDATA_VALUE_EXPORTER");/*_.VALUEXPORTER___V62*/ curfptr[59] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V20*/ curfptr[19];
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : CLASS_PATTERNCONTEXT*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V62*/ curfptr[59]), (basilys_ptr_t)(/*_.VALDATA___V16*/ curfptr[15]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V23*/ curfptr[22];
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_NORMCTXT*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V62*/ curfptr[59]), (basilys_ptr_t)(/*_.VALDATA___V30*/ curfptr[29]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V25*/ curfptr[24];
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_SRC*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V62*/ curfptr[59]), (basilys_ptr_t)(/*_.VALDATA___V32*/ curfptr[31]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V27*/ curfptr[26];
   BASILYS_LOCATION("warmelt-normatch.bysl:81:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : PCTN_MAPATVAR*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V62*/ curfptr[59]), (basilys_ptr_t)(/*_.VALDATA___V34*/ curfptr[33]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.VALUEXPORTER___V62*/ curfptr[59] = 0 ;}
 
 /*block*/{
  BASILYS_LOCATION("warmelt-normatch.bysl:86:/ getslot");
  { basilys_ptr_t slot=0;
   basilys_object_get_field(slot,(basilys_ptr_t)(((void*)(BASILYSG(INITIAL_SYSTEM_DATA)))), 4, "SYSDATA_VALUE_EXPORTER");/*_.VALUEXPORTER___V63*/ curfptr[57] = slot; };
  ;
  BASILYS_LOCATION("warmelt-normatch.bysl:86:/ apply");
  /*apply*/{
   union basilysparam_un argtab[2];
   memset(&argtab, 0, sizeof(argtab));
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ apply.arg");
   argtab[0].bp_aptr = (basilys_ptr_t*) &/*_.VALDATA___V40*/ curfptr[39];
   BASILYS_LOCATION("warmelt-normatch.bysl:86:/ apply.arg");
   argtab[1].bp_aptr = (basilys_ptr_t*) &
   /*quasi.cur.mod.env.cont norm.exp.val : FIND_PATVAR*/
/*_.CONTENV___V2*/ curfptr[1];
    basilys_apply ((basilysclosure_ptr_t)(/*_.VALUEXPORTER___V63*/ curfptr[57]), (basilys_ptr_t)(/*_.VALDATA___V36*/ curfptr[35]), (BPARSTR_PTR BPARSTR_PTR ""), argtab, "", (union basilysparam_un*)0);
   }
  ;
  /*epilog*/
  /*clear*/ /*_.VALUEXPORTER___V63*/ curfptr[57] = 0 ;}
 
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
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V16*/ curfptr[15]));
 
 /*internsym:CLASS_ROOT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V18*/ curfptr[17]));
 
 /*internsym:PCTN_NORMCTXT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V30*/ curfptr[29]));
 
 /*internsym:PCTN_SRC*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V32*/ curfptr[31]));
 
 /*internsym:PCTN_MAPATVAR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V34*/ curfptr[33]));
 
 /*internsym:FIND_PATVAR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V36*/ curfptr[35]));
 
 /*internsym:CLASS_SELECTOR*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V38*/ curfptr[37]));
 
 /*internsym:CLASS_SRC_MATCH*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V44*/ curfptr[43]));
 
 /*internsym:CLASS_ENVIRONMENT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V46*/ curfptr[45]));
 
 /*internsym:CLASS_NORMCONTEXT*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V48*/ curfptr[47]));
 
 /*internsym:DEBUG_MSG_FUN*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V50*/ curfptr[49]));
 
 /*internsym:DISCR_MAPOBJECTS*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V52*/ curfptr[51]));
 
 /*internsym:NORMAL_EXP*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V54*/ curfptr[53]));
 
 /*internsym:INSTALL_METHOD*/
 (void) basilysgc_intern_symbol((basilys_ptr_t)(/*_.VALDATA___V56*/ curfptr[55]));
 
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
const int fieldoff__PCTN_MAPATVAR = 2;
const int fieldoff__PCTN_NORMCTXT = 0;
const int fieldoff__PCTN_SRC = 1;

/* exported class lengths */
const int classlen__CLASS_PATTERNCONTEXT = 3;


/**** end of warmelt-normatch-3.c ****/
