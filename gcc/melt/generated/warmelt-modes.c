/* GCC MELT GENERATED FILE warmelt-modes.c - DO NOT EDIT */

/***+ warmelt-modes without options +***/
#include "melt-run.h"


/* used hash from melt-run.h when compiling this file: */
const char used_meltrun_md5_melt[] = MELT_RUN_HASHMD5 /* from melt-run.h */ ;


/**** warmelt-modes declarations ****/
#define MELT_HAS_INITIAL_ENVIRONMENT 1

struct melt_callframe_st;	/*defined in melt-runtime.h */


/**!!** ***
    Copyright (C) 2011 Free Software Foundation, Inc.
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




melt_ptr_t MELT_MODULE_VISIBILITY
meltrout_1_warmelt_modes_SHOWVAR_DOCMD (meltclosure_ptr_t meltclosp_,
					melt_ptr_t meltfirstargp_,
					const melt_argdescr_cell_t
					meltxargdescr_[],
					union meltparam_un *meltxargtab_,
					const melt_argdescr_cell_t
					meltxresdescr_[],
					union meltparam_un *meltxrestab_);


void *start_module_melt (void *);

/* define different names when debugging or not */
#if MELT_HAVE_DEBUG
extern const char meltmodule_warmelt_modes__melt_have_debug_enabled[];
#define melt_have_debug_string meltmodule_warmelt_modes__melt_have_debug_enabled
#else /*!MELT_HAVE_DEBUG */
extern const char meltmodule_warmelt_modes__melt_have_debug_disabled[];
#define melt_have_debug_string meltmodule_warmelt_modes__melt_have_debug_disabled
#endif /*!MELT_HAVE_DEBUG */


struct frame_start_module_melt_st;
void MELT_MODULE_VISIBILITY warmelt_modes_initialmeltchunk_0 (struct
							      frame_start_module_melt_st
							      *, char *);
static void forward_or_mark_module_start_frame_warmelt_modes (struct
							      melt_callframe_st
							      *fp,
							      int marking);


/**** warmelt-modes implementations ****/




melt_ptr_t MELT_MODULE_VISIBILITY
meltrout_1_warmelt_modes_SHOWVAR_DOCMD (meltclosure_ptr_t meltclosp_,
					melt_ptr_t meltfirstargp_,
					const melt_argdescr_cell_t
					meltxargdescr_[],
					union meltparam_un *meltxargtab_,
					const melt_argdescr_cell_t
					meltxresdescr_[],
					union meltparam_un *meltxrestab_)
{
#if MELT_HAVE_DEBUG
  static long call_counter__;
  long thiscallcounter__ ATTRIBUTE_UNUSED = ++call_counter__;
#undef meltcallcount
#define meltcallcount thiscallcounter__
#else
#undef meltcallcount
#define meltcallcount 0L
#endif

  struct frame_meltrout_1_warmelt_modes_SHOWVAR_DOCMD_st
  {
    int mcfr_nbvar;
#if MELT_HAVE_DEBUG
    const char *mcfr_flocs;
#else				/*!MELT_HAVE_DEBUG */
    const char *mcfr_unusedflocs;
#endif				/*MELT_HAVE_DEBUG */
    struct meltclosure_st *mcfr_clos;
    struct excepth_melt_st *mcfr_exh;
    struct melt_callframe_st *mcfr_prev;
#define MELTFRAM_NBVARPTR 20
    void *mcfr_varptr[20];
#define MELTFRAM_NBVARNUM 5
    long mcfr_varnum[5];
/*others*/
    const char *loc_CSTRING__o0;
    long _spare_;
  }
   *meltframptr_ = 0, meltfram__;
#define meltframe meltfram__
  if (MELT_UNLIKELY (meltxargdescr_ == MELTPAR_MARKGGC))
    {				/*mark for ggc */
      int ix = 0;
      meltframptr_ = (void *) meltfirstargp_;
      gt_ggc_mx_melt_un (meltframptr_->mcfr_clos);
      for (ix = 0; ix < 20; ix++)
	if (meltframptr_->mcfr_varptr[ix])
	  gt_ggc_mx_melt_un (meltframptr_->mcfr_varptr[ix]);
      return NULL;
    } /*end markggc */ ;
  memset (&meltfram__, 0, sizeof (meltfram__));
  meltfram__.mcfr_nbvar = 20;
  meltfram__.mcfr_clos = meltclosp_;
  meltfram__.mcfr_prev = (struct melt_callframe_st *) melt_topframe;
  melt_topframe = (struct melt_callframe_st *) &meltfram__;
  melt_trace_start ("SHOWVAR_DOCMD", meltcallcount);
/*getargs*/

  /*getarg#0 */
  MELT_LOCATION ("warmelt-modes.melt:33:/ getarg");
 /*_.CMD__V2*/ meltfptr[1] = (melt_ptr_t) meltfirstargp_;

  /*getarg#1 */
  /*^getarg */
  if (meltxargdescr_[0] != MELTBPAR_PTR)
    goto lab_endgetargs;
 /*_.MODULDATA__V3*/ meltfptr[2] =
    (meltxargtab_[0].meltbp_aptr) ? (*(meltxargtab_[0].meltbp_aptr)) : NULL;
  gcc_assert (melt_discr ((melt_ptr_t) ( /*_.MODULDATA__V3*/ meltfptr[2])) !=
	      NULL);

  ;
  goto lab_endgetargs;
lab_endgetargs:;
/*body*/
/*^block*/
/*anyblock*/
  {


#if MELT_HAVE_DEBUG
    MELT_LOCATION ("warmelt-modes.melt:34:/ cppif.then");
    /*^block */
    /*anyblock */
    {

   /*_#THE_MELTCALLCOUNT__L1*/ meltfnum[0] =
	meltcallcount;;
      MELT_LOCATION ("warmelt-modes.melt:34:/ apply");
      /*apply */
      {
	union meltparam_un argtab[4];
	memset (&argtab, 0, sizeof (argtab));
	/*^apply.arg */
	argtab[0].meltbp_cstring = "start showvar_docmd cmd";
	/*^apply.arg */
	argtab[1].meltbp_long = /*_#THE_MELTCALLCOUNT__L1*/ meltfnum[0];
	/*^apply.arg */
	argtab[2].meltbp_cstring = "warmelt-modes.melt";
	/*^apply.arg */
	argtab[3].meltbp_long = 34;
	/*_.DEBUG_MSG_FUN__V5*/ meltfptr[4] =
	  melt_apply ((meltclosure_ptr_t)
		      (( /*!DEBUG_MSG_FUN */ meltfrout->tabval[0])),
		      (melt_ptr_t) ( /*_.CMD__V2*/ meltfptr[1]),
		      (MELTBPARSTR_CSTRING MELTBPARSTR_LONG
		       MELTBPARSTR_CSTRING MELTBPARSTR_LONG ""), argtab, "",
		      (union meltparam_un *) 0);
      }
      ;
      /*_.IFCPP___V4*/ meltfptr[3] = /*_.DEBUG_MSG_FUN__V5*/ meltfptr[4];;
      /*epilog */

      MELT_LOCATION ("warmelt-modes.melt:34:/ clear");
	     /*clear *//*_#THE_MELTCALLCOUNT__L1*/ meltfnum[0] = 0;
      /*^clear */
	     /*clear *//*_.DEBUG_MSG_FUN__V5*/ meltfptr[4] = 0;
    }

#else /*MELT_HAVE_DEBUG */
    /*^cppif.else */
    /*_.IFCPP___V4*/ meltfptr[3] = ( /*nil */ NULL);
#endif /*MELT_HAVE_DEBUG */
    ;

#if MELT_HAVE_DEBUG
    MELT_LOCATION ("warmelt-modes.melt:35:/ cppif.then");
    /*^block */
    /*anyblock */
    {

   /*_#THE_MELTCALLCOUNT__L2*/ meltfnum[0] =
	meltcallcount;;
      MELT_LOCATION ("warmelt-modes.melt:35:/ apply");
      /*apply */
      {
	union meltparam_un argtab[4];
	memset (&argtab, 0, sizeof (argtab));
	/*^apply.arg */
	argtab[0].meltbp_cstring = "start showvar_docmd moduldata";
	/*^apply.arg */
	argtab[1].meltbp_long = /*_#THE_MELTCALLCOUNT__L2*/ meltfnum[0];
	/*^apply.arg */
	argtab[2].meltbp_cstring = "warmelt-modes.melt";
	/*^apply.arg */
	argtab[3].meltbp_long = 35;
	/*_.DEBUG_MSG_FUN__V7*/ meltfptr[6] =
	  melt_apply ((meltclosure_ptr_t)
		      (( /*!DEBUG_MSG_FUN */ meltfrout->tabval[0])),
		      (melt_ptr_t) ( /*_.MODULDATA__V3*/ meltfptr[2]),
		      (MELTBPARSTR_CSTRING MELTBPARSTR_LONG
		       MELTBPARSTR_CSTRING MELTBPARSTR_LONG ""), argtab, "",
		      (union meltparam_un *) 0);
      }
      ;
      /*_.IFCPP___V6*/ meltfptr[4] = /*_.DEBUG_MSG_FUN__V7*/ meltfptr[6];;
      /*epilog */

      MELT_LOCATION ("warmelt-modes.melt:35:/ clear");
	     /*clear *//*_#THE_MELTCALLCOUNT__L2*/ meltfnum[0] = 0;
      /*^clear */
	     /*clear *//*_.DEBUG_MSG_FUN__V7*/ meltfptr[6] = 0;
    }

#else /*MELT_HAVE_DEBUG */
    /*^cppif.else */
    /*_.IFCPP___V6*/ meltfptr[4] = ( /*nil */ NULL);
#endif /*MELT_HAVE_DEBUG */
    ;
    MELT_LOCATION ("warmelt-modes.melt:36:/ quasiblock");


    /*_.PARMODENV__V8*/ meltfptr[6] = ( /*!konst_1 */ meltfrout->tabval[1]);;
    MELT_LOCATION ("warmelt-modes.melt:38:/ cond");
    /*cond */ if ( /*_.MODULDATA__V3*/ meltfptr[2])	/*then */
      {
	/*^cond.then */
	/*_.ENV__V9*/ meltfptr[8] = /*_.MODULDATA__V3*/ meltfptr[2];;
      }
    else
      {
	MELT_LOCATION ("warmelt-modes.melt:38:/ cond.else");

	/*_.ENV__V9*/ meltfptr[8] = /*_.PARMODENV__V8*/ meltfptr[6];;
      }
    ;
    /*^compute */
 /*_?*/ meltfram__.loc_CSTRING__o0 =
      melt_argument ("arg");;
    /*^compute */
 /*_.INARG__V10*/ meltfptr[9] =
      (meltgc_new_stringdup
       ((meltobject_ptr_t) (( /*!DISCR_STRING */ meltfrout->tabval[2])),
	( /*_?*/ meltfram__.loc_CSTRING__o0)));;
    /*^compute */
 /*_.INSYMB__V11*/ meltfptr[10] =
      meltgc_named_symbol (melt_string_str
			   ((melt_ptr_t) ( /*_.INARG__V10*/ meltfptr[9])),
			   MELT_GET);;

#if MELT_HAVE_DEBUG
    MELT_LOCATION ("warmelt-modes.melt:42:/ cppif.then");
    /*^block */
    /*anyblock */
    {

   /*_#IS_A__L3*/ meltfnum[0] =
	melt_is_instance_of ((melt_ptr_t) ( /*_.MODULDATA__V3*/ meltfptr[2]),
			     (melt_ptr_t) (( /*!CLASS_ENVIRONMENT */
					    meltfrout->tabval[3])));;
      MELT_LOCATION ("warmelt-modes.melt:42:/ cond");
      /*cond */ if ( /*_#IS_A__L3*/ meltfnum[0])	/*then */
	{
	  /*^cond.then */
	  /*_.IFELSE___V13*/ meltfptr[12] = ( /*nil */ NULL);;
	}
      else
	{
	  MELT_LOCATION ("warmelt-modes.melt:42:/ cond.else");

	  /*^block */
	  /*anyblock */
	  {




	    {
	      /*^locexp */
	      melt_assert_failed (("check moduldata"),
				  ("warmelt-modes.melt")
				  ? ("warmelt-modes.melt") : __FILE__,
				  (42) ? (42) : __LINE__, __FUNCTION__);
	      ;
	    }
	    ;
	       /*clear *//*_.IFELSE___V13*/ meltfptr[12] = 0;
	    /*epilog */
	  }
	  ;
	}
      ;
      /*^compute */
      /*_.IFCPP___V12*/ meltfptr[11] = /*_.IFELSE___V13*/ meltfptr[12];;
      /*epilog */

      MELT_LOCATION ("warmelt-modes.melt:42:/ clear");
	     /*clear *//*_#IS_A__L3*/ meltfnum[0] = 0;
      /*^clear */
	     /*clear *//*_.IFELSE___V13*/ meltfptr[12] = 0;
    }

#else /*MELT_HAVE_DEBUG */
    /*^cppif.else */
    /*_.IFCPP___V12*/ meltfptr[11] = ( /*nil */ NULL);
#endif /*MELT_HAVE_DEBUG */
    ;
    /*^compute */
 /*_#IS_A__L4*/ meltfnum[0] =
      melt_is_instance_of ((melt_ptr_t) ( /*_.INSYMB__V11*/ meltfptr[10]),
			   (melt_ptr_t) (( /*!CLASS_SYMBOL */ meltfrout->
					  tabval[4])));;
    MELT_LOCATION ("warmelt-modes.melt:43:/ cond");
    /*cond */ if ( /*_#IS_A__L4*/ meltfnum[0])	/*then */
      {
	/*^cond.then */
	/*^block */
	/*anyblock */
	{

	  MELT_LOCATION ("warmelt-modes.melt:44:/ quasiblock");


	  /*^apply */
	  /*apply */
	  {
	    union meltparam_un argtab[1];
	    memset (&argtab, 0, sizeof (argtab));
	    /*^apply.arg */
	    argtab[0].meltbp_aptr =
	      (melt_ptr_t *) & /*_.INSYMB__V11*/ meltfptr[10];
	    /*_.SYMBIND__V14*/ meltfptr[12] =
	      melt_apply ((meltclosure_ptr_t)
			  (( /*!FIND_ENV */ meltfrout->tabval[5])),
			  (melt_ptr_t) ( /*_.ENV__V9*/ meltfptr[8]),
			  (MELTBPARSTR_PTR ""), argtab, "",
			  (union meltparam_un *) 0);
	  }
	  ;
	  MELT_LOCATION ("warmelt-modes.melt:45:/ getslot");
	  {
	    melt_ptr_t slot = 0;
	    melt_object_get_field (slot, (melt_ptr_t) ( /*_.INSYMB__V11*/ meltfptr[10]), 1, "NAMED_NAME");
											      /*_.SYMNAME__V15*/
	    meltfptr[14] = slot;
	  };
	  ;
	  MELT_LOCATION ("warmelt-modes.melt:47:/ cond");
	  /*cond */ if ( /*_.SYMBIND__V14*/ meltfptr[12])	/*then */
	    {
	      /*^cond.then */
	      /*^block */
	      /*anyblock */
	      {


#if MELT_HAVE_DEBUG
		MELT_LOCATION ("warmelt-modes.melt:49:/ cppif.then");
		/*^block */
		/*anyblock */
		{

       /*_#THE_MELTCALLCOUNT__L5*/ meltfnum[4] =
		    meltcallcount;;
		  MELT_LOCATION ("warmelt-modes.melt:49:/ apply");
		  /*apply */
		  {
		    union meltparam_un argtab[4];
		    memset (&argtab, 0, sizeof (argtab));
		    /*^apply.arg */
		    argtab[0].meltbp_cstring = "showvar_docmd symbind";
		    /*^apply.arg */
		    argtab[1].meltbp_long =
		      /*_#THE_MELTCALLCOUNT__L5*/ meltfnum[4];
		    /*^apply.arg */
		    argtab[2].meltbp_cstring = "warmelt-modes.melt";
		    /*^apply.arg */
		    argtab[3].meltbp_long = 49;
		    /*_.DEBUG_MSG_FUN__V18*/ meltfptr[17] =
		      melt_apply ((meltclosure_ptr_t)
				  (( /*!DEBUG_MSG_FUN */ meltfrout->
				    tabval[0])),
				  (melt_ptr_t) ( /*_.SYMBIND__V14*/
						meltfptr[12]),
				  (MELTBPARSTR_CSTRING MELTBPARSTR_LONG
				   MELTBPARSTR_CSTRING MELTBPARSTR_LONG ""),
				  argtab, "", (union meltparam_un *) 0);
		  }
		  ;
		  /*_.IFCPP___V17*/ meltfptr[16] =
		    /*_.DEBUG_MSG_FUN__V18*/ meltfptr[17];;
		  /*epilog */

		  MELT_LOCATION ("warmelt-modes.melt:49:/ clear");
		 /*clear *//*_#THE_MELTCALLCOUNT__L5*/ meltfnum[4] = 0;
		  /*^clear */
		 /*clear *//*_.DEBUG_MSG_FUN__V18*/ meltfptr[17] = 0;
		}

#else /*MELT_HAVE_DEBUG */
		/*^cppif.else */
		/*_.IFCPP___V17*/ meltfptr[16] = ( /*nil */ NULL);
#endif /*MELT_HAVE_DEBUG */
		;

		{
		  MELT_LOCATION ("warmelt-modes.melt:50:/ locexp");
		  inform (UNKNOWN_LOCATION, "MELT INFORM [#%ld]: %s - %s",
			  melt_dbgcounter, ("found binding for "),
			  melt_string_str ( /*_.SYMNAME__V15*/ meltfptr[14]));
		}
		;
		MELT_LOCATION ("warmelt-modes.melt:51:/ quasiblock");


     /*_.RETVAL___V1*/ meltfptr[0] = NULL;;
		MELT_LOCATION ("warmelt-modes.melt:51:/ finalreturn");
		;
		/*finalret */ goto labend_rout;
		MELT_LOCATION ("warmelt-modes.melt:48:/ quasiblock");


		/*_.PROGN___V20*/ meltfptr[19] =
		  /*_.RETURN___V19*/ meltfptr[17];;
		/*^compute */
		/*_.IFELSE___V16*/ meltfptr[15] =
		  /*_.PROGN___V20*/ meltfptr[19];;
		/*epilog */

		MELT_LOCATION ("warmelt-modes.melt:47:/ clear");
	       /*clear *//*_.IFCPP___V17*/ meltfptr[16] = 0;
		/*^clear */
	       /*clear *//*_.RETURN___V19*/ meltfptr[17] = 0;
		/*^clear */
	       /*clear *//*_.PROGN___V20*/ meltfptr[19] = 0;
	      }
	      ;
	    }
	  else
	    {			/*^cond.else */

	      /*^block */
	      /*anyblock */
	      {




		{
		  MELT_LOCATION ("warmelt-modes.melt:52:/ locexp");
		  error ("MELT ERROR MSG [#%ld]::: %s - %s", melt_dbgcounter,
			 ("no binding found for "),
			 melt_string_str ((melt_ptr_t)
					  ( /*_.SYMNAME__V15*/
					   meltfptr[14])));
		}
		;
	       /*clear *//*_.IFELSE___V16*/ meltfptr[15] = 0;
		/*epilog */
	      }
	      ;
	    }
	  ;

	  {
	    MELT_LOCATION ("warmelt-modes.melt:54:/ locexp");
	    error ("MELT ERROR MSG [#%ld]::: %s - %s", melt_dbgcounter,
		   ("unknown symbol to show"),
		   melt_string_str ((melt_ptr_t)
				    ( /*_.INARG__V10*/ meltfptr[9])));
	  }
	  ;

	  MELT_LOCATION ("warmelt-modes.melt:44:/ clear");
	     /*clear *//*_.SYMBIND__V14*/ meltfptr[12] = 0;
	  /*^clear */
	     /*clear *//*_.SYMNAME__V15*/ meltfptr[14] = 0;
	  /*^clear */
	     /*clear *//*_.IFELSE___V16*/ meltfptr[15] = 0;
	  /*epilog */
	}
	;
      }				/*noelse */
    ;

    MELT_LOCATION ("warmelt-modes.melt:36:/ clear");
	   /*clear *//*_.PARMODENV__V8*/ meltfptr[6] = 0;
    /*^clear */
	   /*clear *//*_.ENV__V9*/ meltfptr[8] = 0;
    /*^clear */
	   /*clear *//*_?*/ meltfram__.loc_CSTRING__o0 = 0;
    /*^clear */
	   /*clear *//*_.INARG__V10*/ meltfptr[9] = 0;
    /*^clear */
	   /*clear *//*_.INSYMB__V11*/ meltfptr[10] = 0;
    /*^clear */
	   /*clear *//*_.IFCPP___V12*/ meltfptr[11] = 0;
    /*^clear */
	   /*clear *//*_#IS_A__L4*/ meltfnum[0] = 0;
    /*epilog */

    MELT_LOCATION ("warmelt-modes.melt:33:/ clear");
	   /*clear *//*_.IFCPP___V4*/ meltfptr[3] = 0;
    /*^clear */
	   /*clear *//*_.IFCPP___V6*/ meltfptr[4] = 0;
  }

  ;
  goto labend_rout;
labend_rout:
  melt_trace_end ("SHOWVAR_DOCMD", meltcallcount);
  melt_topframe = (struct melt_callframe_st *) meltfram__.mcfr_prev;
  return (melt_ptr_t) ( /*_.RETVAL___V1*/ meltfptr[0]);
#undef meltcallcount
#undef meltfram__
#undef MELTFRAM_NBVARNUM
#undef MELTFRAM_NBVARPTR
}				/*end meltrout_1_warmelt_modes_SHOWVAR_DOCMD */



typedef struct frame_start_module_melt_st
{
  int mcfr_nbvar;
#if MELT_HAVE_DEBUG
  const char *mcfr_flocs;
#else				/*!MELT_HAVE_DEBUG */
  const char *mcfr_unusedflocs;
#endif				/*MELT_HAVE_DEBUG */
  void (*mcfr_initforwmarkrout) (struct melt_callframe_st *, int);
  struct excepth_melt_st *mcfr_exh;
  struct melt_callframe_st *mcfr_prev;
#define MELTFRAM_NBVARPTR 26
  void *mcfr_varptr[26];
/*no varnum*/
#define MELTFRAM_NBVARNUM /*none*/0
/*others*/
  long _spare_;
}
initial_frame_st;


static void
initialize_module_meltdata_warmelt_modes (initial_frame_st * iniframp__,
					  char predefinited[])
{
#define meltfram__  (*iniframp__)
  dbgprintf ("start initialize_module_meltdata_warmelt_modes iniframp__=%p",
	     (void *) iniframp__);
  melt_assertmsg ("check module initial frame",
		  iniframp__->mcfr_nbvar == /*minihash */ -2859);

  struct cdata_st
  {
    struct MELT_ROUTINE_STRUCT (6) drout_1__SHOWVAR_DOCMD;
    struct MELT_CLOSURE_STRUCT (0) dclo_2__SHOWVAR_DOCMD;
    struct MELT_OBJECT_STRUCT (3) dsym_3__DEBUG_MSG_FUN;
    struct MELT_STRING_STRUCT (13) dstr_4__DEBUG_MSG_FUN;
    struct MELT_OBJECT_STRUCT (3) dsym_5__DISCR_STRING;
    struct MELT_STRING_STRUCT (12) dstr_6__DISCR_STRING;
    struct MELT_OBJECT_STRUCT (3) dsym_7__CLASS_ENVIRONMENT;
    struct MELT_STRING_STRUCT (17) dstr_8__CLASS_ENVIRONMEN;
    struct MELT_OBJECT_STRUCT (3) dsym_9__CLASS_SYMBOL;
    struct MELT_STRING_STRUCT (12) dstr_10__CLASS_SYMBOL;
    struct MELT_OBJECT_STRUCT (3) dsym_11__FIND_ENV;
    struct MELT_STRING_STRUCT (8) dstr_12__FIND_ENV;
    long spare_;
  } *cdat = NULL;
  cdat = (struct cdata_st *) meltgc_allocate (sizeof (*cdat), 0);
  melt_prohibit_garbcoll = TRUE;
  /*initial routine predef */
  /*initial routine fill */

  /*inirout drout_1__SHOWVAR_DOCMD */
 /*_.VALROUT___V12*/ meltfptr[11] = (void *) &cdat->drout_1__SHOWVAR_DOCMD;
  cdat->drout_1__SHOWVAR_DOCMD.discr =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (DISCR_ROUTINE))));
  strncpy (cdat->drout_1__SHOWVAR_DOCMD.routdescr,
	   "SHOWVAR_DOCMD @warmelt-modes.melt:33", MELT_ROUTDESCR_LEN - 1);
  cdat->drout_1__SHOWVAR_DOCMD.nbval = 6;
  MELT_ROUTINE_SET_ROUTCODE (&cdat->drout_1__SHOWVAR_DOCMD,
			     meltrout_1_warmelt_modes_SHOWVAR_DOCMD);

  /*iniclos dclo_2__SHOWVAR_DOCMD */
 /*_.VALCLO___V13*/ meltfptr[12] = (void *) &cdat->dclo_2__SHOWVAR_DOCMD;
  cdat->dclo_2__SHOWVAR_DOCMD.discr =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (DISCR_CLOSURE))));
  cdat->dclo_2__SHOWVAR_DOCMD.nbval = 0;


/*iniobj dsym_3__DEBUG_MSG_FUN*/
  /*uniqueobj*/ if (! /*_.VALDATA___V14*/ meltfptr[13])
						     /*_.VALDATA___V14*/
    meltfptr[13] = (void *) &cdat->dsym_3__DEBUG_MSG_FUN;
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr isobj dsym_3__DEBUG_MSG_FUN",
		    melt_magic_discr (((void *) (MELT_PREDEF (CLASS_SYMBOL))))
		    == MELTOBMAG_OBJECT);
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr objmagic dsym_3__DEBUG_MSG_FUN",
		    ((meltobject_ptr_t)
		     (((void *) (MELT_PREDEF (CLASS_SYMBOL)))))->
		    meltobj_magic == MELTOBMAG_OBJECT);
  cdat->dsym_3__DEBUG_MSG_FUN.meltobj_class =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (CLASS_SYMBOL))));
  cdat->dsym_3__DEBUG_MSG_FUN.obj_hash = 938829072;
  cdat->dsym_3__DEBUG_MSG_FUN.obj_len = 3;


/*inistring dstr_4__DEBUG_MSG_FUN*/
 /*_.VALSTR___V15*/ meltfptr[14] = (void *) &cdat->dstr_4__DEBUG_MSG_FUN;
  cdat->dstr_4__DEBUG_MSG_FUN.discr =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (DISCR_STRING))));
  /*small inistring */ strncpy (cdat->dstr_4__DEBUG_MSG_FUN.val,
				"DEBUG_MSG_FUN",
				sizeof (cdat->dstr_4__DEBUG_MSG_FUN.val) - 1);

/*iniobj dsym_5__DISCR_STRING*/
  /*uniqueobj*/ if (! /*_.VALDATA___V16*/ meltfptr[15])
						     /*_.VALDATA___V16*/
    meltfptr[15] = (void *) &cdat->dsym_5__DISCR_STRING;
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr isobj dsym_5__DISCR_STRING",
		    melt_magic_discr (((void *) (MELT_PREDEF (CLASS_SYMBOL))))
		    == MELTOBMAG_OBJECT);
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr objmagic dsym_5__DISCR_STRING",
		    ((meltobject_ptr_t)
		     (((void *) (MELT_PREDEF (CLASS_SYMBOL)))))->
		    meltobj_magic == MELTOBMAG_OBJECT);
  cdat->dsym_5__DISCR_STRING.meltobj_class =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (CLASS_SYMBOL))));
  cdat->dsym_5__DISCR_STRING.obj_hash = 100436219;
  cdat->dsym_5__DISCR_STRING.obj_len = 3;


/*inistring dstr_6__DISCR_STRING*/
 /*_.VALSTR___V17*/ meltfptr[16] = (void *) &cdat->dstr_6__DISCR_STRING;
  cdat->dstr_6__DISCR_STRING.discr =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (DISCR_STRING))));
  /*small inistring */ strncpy (cdat->dstr_6__DISCR_STRING.val,
				"DISCR_STRING",
				sizeof (cdat->dstr_6__DISCR_STRING.val) - 1);

/*iniobj dsym_7__CLASS_ENVIRONMENT*/
  /*uniqueobj*/ if (! /*_.VALDATA___V18*/ meltfptr[17])
						     /*_.VALDATA___V18*/
    meltfptr[17] = (void *) &cdat->dsym_7__CLASS_ENVIRONMENT;
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr isobj dsym_7__CLASS_ENVIRONMENT",
		    melt_magic_discr (((void *) (MELT_PREDEF (CLASS_SYMBOL))))
		    == MELTOBMAG_OBJECT);
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr objmagic dsym_7__CLASS_ENVIRONMENT",
		    ((meltobject_ptr_t)
		     (((void *) (MELT_PREDEF (CLASS_SYMBOL)))))->
		    meltobj_magic == MELTOBMAG_OBJECT);
  cdat->dsym_7__CLASS_ENVIRONMENT.meltobj_class =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (CLASS_SYMBOL))));
  cdat->dsym_7__CLASS_ENVIRONMENT.obj_hash = 59212821;
  cdat->dsym_7__CLASS_ENVIRONMENT.obj_len = 3;


/*inistring dstr_8__CLASS_ENVIRONMEN*/
 /*_.VALSTR___V19*/ meltfptr[18] = (void *) &cdat->dstr_8__CLASS_ENVIRONMEN;
  cdat->dstr_8__CLASS_ENVIRONMEN.discr =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (DISCR_STRING))));
  /*small inistring */ strncpy (cdat->dstr_8__CLASS_ENVIRONMEN.val,
				"CLASS_ENVIRONMENT",
				sizeof (cdat->dstr_8__CLASS_ENVIRONMEN.val) -
				1);

/*iniobj dsym_9__CLASS_SYMBOL*/
  /*uniqueobj*/ if (! /*_.VALDATA___V20*/ meltfptr[19])
						     /*_.VALDATA___V20*/
    meltfptr[19] = (void *) &cdat->dsym_9__CLASS_SYMBOL;
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr isobj dsym_9__CLASS_SYMBOL",
		    melt_magic_discr (((void *) (MELT_PREDEF (CLASS_SYMBOL))))
		    == MELTOBMAG_OBJECT);
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr objmagic dsym_9__CLASS_SYMBOL",
		    ((meltobject_ptr_t)
		     (((void *) (MELT_PREDEF (CLASS_SYMBOL)))))->
		    meltobj_magic == MELTOBMAG_OBJECT);
  cdat->dsym_9__CLASS_SYMBOL.meltobj_class =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (CLASS_SYMBOL))));
  cdat->dsym_9__CLASS_SYMBOL.obj_hash = 53524182;
  cdat->dsym_9__CLASS_SYMBOL.obj_len = 3;


/*inistring dstr_10__CLASS_SYMBOL*/
 /*_.VALSTR___V21*/ meltfptr[20] = (void *) &cdat->dstr_10__CLASS_SYMBOL;
  cdat->dstr_10__CLASS_SYMBOL.discr =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (DISCR_STRING))));
  /*small inistring */ strncpy (cdat->dstr_10__CLASS_SYMBOL.val,
				"CLASS_SYMBOL",
				sizeof (cdat->dstr_10__CLASS_SYMBOL.val) - 1);

/*iniobj dsym_11__FIND_ENV*/
  /*uniqueobj*/ if (! /*_.VALDATA___V22*/ meltfptr[21])
						     /*_.VALDATA___V22*/
    meltfptr[21] = (void *) &cdat->dsym_11__FIND_ENV;
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr isobj dsym_11__FIND_ENV",
		    melt_magic_discr (((void *) (MELT_PREDEF (CLASS_SYMBOL))))
		    == MELTOBMAG_OBJECT);
  if (MELT_LIKELY (!melt_prohibit_garbcoll))
    melt_assertmsg ("iniobj check.discr objmagic dsym_11__FIND_ENV",
		    ((meltobject_ptr_t)
		     (((void *) (MELT_PREDEF (CLASS_SYMBOL)))))->
		    meltobj_magic == MELTOBMAG_OBJECT);
  cdat->dsym_11__FIND_ENV.meltobj_class =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (CLASS_SYMBOL))));
  cdat->dsym_11__FIND_ENV.obj_hash = 233459738;
  cdat->dsym_11__FIND_ENV.obj_len = 3;


/*inistring dstr_12__FIND_ENV*/
 /*_.VALSTR___V23*/ meltfptr[22] = (void *) &cdat->dstr_12__FIND_ENV;
  cdat->dstr_12__FIND_ENV.discr =
    (meltobject_ptr_t) (((void *) (MELT_PREDEF (DISCR_STRING))));
  /*small inistring */ strncpy (cdat->dstr_12__FIND_ENV.val, "FIND_ENV",
				sizeof (cdat->dstr_12__FIND_ENV.val) - 1);
  cdat = NULL;
  melt_prohibit_garbcoll = FALSE;

#undef meltfram__
}				/*end initialize_module_meltdata_warmelt_modes */


#if MELT_HAVE_DEBUG
const char meltmodule_warmelt_modes__melt_have_debug_enabled[] =
  "MELT module warmelt-modes have debug enabled";
#else /*!MELT_HAVE_DEBUG */
const char meltmodule_warmelt_modes__melt_have_debug_disabled[] =
  "MELT module warmelt-modes have debug disabled";
#endif /*MELT_HAVE_DEBUG */


void *
start_module_melt (void *modargp_)
{

  char predefinited[MELTGLOB__LASTGLOB + 8];

#if MELT_HAVE_DEBUG
  static long call_counter__;
  long thiscallcounter__ ATTRIBUTE_UNUSED = ++call_counter__;
#undef meltcallcount
#define meltcallcount thiscallcounter__
#else
#undef meltcallcount
#define meltcallcount 0L
#endif

  initial_frame_st meltfram__;
#define meltframe meltfram__
  memset (&meltfram__, 0, sizeof (meltfram__));
  meltfram__.mcfr_nbvar = 26;
  meltfram__.mcfr_prev = (struct melt_callframe_st *) melt_topframe;
  melt_topframe = (struct melt_callframe_st *) &meltfram__;
/**initial routine prologue**/
  /* set initial frame marking */
  ((struct melt_callframe_st *) &meltfram__)->mcfr_nbvar =
    /*minihash */ -2859;
  ((struct melt_callframe_st *) &meltfram__)->mcfr_forwmarkrout =
    forward_or_mark_module_start_frame_warmelt_modes;
 /**COMMENT: get previous environment **/ ;

  /*^compute */
 /*_.PREVENV___V4*/ meltfptr[3] = modargp_;
  /*^comment */
 /**COMMENT: compute boxloc **/ ;

  /*^cond */
  /*cond */ if ( /*_.CONTENV___V2*/ meltfptr[1] || melt_object_length ((melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA)) < FSYSDAT_BOX_FRESH_ENV)	/*then */
    {
    }
  else
    {				/*^cond.else */

      /*^block */
      /*com.block:compute fresh module environment */
      {

	/*^comment */
   /**COMMENT: start computing boxloc **/ ;
	;
	/*^getslot */
	{
	  melt_ptr_t slot = 0;
	  melt_object_get_field (slot, (melt_ptr_t) (((void *) (MELT_PREDEF (INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");
														       /*_.FRESHENV___V3*/
	  meltfptr[2] = slot;
	};
	;
	/*^compute */

	if (( /*_.PREVENV___V4*/ meltfptr[3])
	    &&
	    melt_magic_discr ((melt_ptr_t) ( /*_.FRESHENV___V3*/ meltfptr[2]))
	    != MELTOBMAG_CLOSURE)
	  warning (0, "bad MELT fresh_env @%p in system data <%s:%d>",
		   /*_.FRESHENV___V3*/ meltfptr[2], __FILE__, __LINE__);;;
	/*^apply */
	/*apply */
	{
	  /*_.CONTENV___V2*/ meltfptr[1] =
	    melt_apply ((meltclosure_ptr_t)
			( /*_.FRESHENV___V3*/ meltfptr[2]),
			(melt_ptr_t) ( /*_.PREVENV___V4*/ meltfptr[3]), (""),
			(union meltparam_un *) 0, "",
			(union meltparam_un *) 0);
	}
	;
      }				/*com.end block:compute fresh module environment */
      ;
    }

  /*^comment */
 /**COMMENT: get symbols & keywords **/ ;

  /*^getnamedsymbol */
  /*getnamedsym:DEBUG_MSG_FUN */
  {
    melt_ptr_t sy_DEBUG_MSG_FUN =
      meltgc_named_symbol ("DEBUG_MSG_FUN", MELT_GET);
    if (sy_DEBUG_MSG_FUN && NULL == /*_.VALDATA___V14*/ meltfptr[13])
  /*_.VALDATA___V14*/ meltfptr[13] = (void *) sy_DEBUG_MSG_FUN;
  }

  /*^getnamedsymbol */
  /*getnamedsym:DISCR_STRING */
  {
    melt_ptr_t sy_DISCR_STRING =
      meltgc_named_symbol ("DISCR_STRING", MELT_GET);
    if (sy_DISCR_STRING && NULL == /*_.VALDATA___V16*/ meltfptr[15])
  /*_.VALDATA___V16*/ meltfptr[15] = (void *) sy_DISCR_STRING;
  }

  /*^getnamedsymbol */
  /*getnamedsym:CLASS_ENVIRONMENT */
  {
    melt_ptr_t sy_CLASS_ENVIRONMENT =
      meltgc_named_symbol ("CLASS_ENVIRONMENT", MELT_GET);
    if (sy_CLASS_ENVIRONMENT && NULL == /*_.VALDATA___V18*/ meltfptr[17])
  /*_.VALDATA___V18*/ meltfptr[17] = (void *) sy_CLASS_ENVIRONMENT;
  }

  /*^getnamedsymbol */
  /*getnamedsym:CLASS_SYMBOL */
  {
    melt_ptr_t sy_CLASS_SYMBOL =
      meltgc_named_symbol ("CLASS_SYMBOL", MELT_GET);
    if (sy_CLASS_SYMBOL && NULL == /*_.VALDATA___V20*/ meltfptr[19])
  /*_.VALDATA___V20*/ meltfptr[19] = (void *) sy_CLASS_SYMBOL;
  }

  /*^getnamedsymbol */
  /*getnamedsym:FIND_ENV */
  {
    melt_ptr_t sy_FIND_ENV = meltgc_named_symbol ("FIND_ENV", MELT_GET);
    if (sy_FIND_ENV && NULL == /*_.VALDATA___V22*/ meltfptr[21])
  /*_.VALDATA___V22*/ meltfptr[21] = (void *) sy_FIND_ENV;
  }

  /*^comment */
 /**COMMENT: get the value importer **/ ;

  /*^getslot */
  {
    melt_ptr_t slot = 0;
    melt_object_get_field (slot, (melt_ptr_t) (((void *) (MELT_PREDEF (INITIAL_SYSTEM_DATA)))), 12, "SYSDATA_VALUE_IMPORTER");
														      /*_.VALIMPORT___V6*/
    meltfptr[5] = slot;
  };

  /*^comment */
 /**COMMENT: before getting imported values **/ ;

  /*^apply */
  /*apply */
  {
    union meltparam_un argtab[3];
    memset (&argtab, 0, sizeof (argtab));
    /*^apply.arg */
    argtab[0].meltbp_aptr = (melt_ptr_t *) & /*_.PREVENV___V4*/ meltfptr[3];
    /*^apply.arg */
    argtab[1].meltbp_cstring = "DEBUG_MSG_FUN";
    /*^apply.arg */
    argtab[2].meltbp_cstring = "warmelt-modes";
    /*_.DEBUG_MSG_FUN__V7*/ meltfptr[6] =
      melt_apply ((meltclosure_ptr_t) ( /*_.VALIMPORT___V6*/ meltfptr[5]),
		  (melt_ptr_t) ( /*_.VALDATA___V14*/ meltfptr[13]),
		  (MELTBPARSTR_PTR MELTBPARSTR_CSTRING MELTBPARSTR_CSTRING
		   ""), argtab, "", (union meltparam_un *) 0);
  }

  /*^apply */
  /*apply */
  {
    union meltparam_un argtab[3];
    memset (&argtab, 0, sizeof (argtab));
    /*^apply.arg */
    argtab[0].meltbp_aptr = (melt_ptr_t *) & /*_.PREVENV___V4*/ meltfptr[3];
    /*^apply.arg */
    argtab[1].meltbp_cstring = "DISCR_STRING";
    /*^apply.arg */
    argtab[2].meltbp_cstring = "warmelt-modes";
    /*_.DISCR_STRING__V8*/ meltfptr[7] =
      melt_apply ((meltclosure_ptr_t) ( /*_.VALIMPORT___V6*/ meltfptr[5]),
		  (melt_ptr_t) ( /*_.VALDATA___V16*/ meltfptr[15]),
		  (MELTBPARSTR_PTR MELTBPARSTR_CSTRING MELTBPARSTR_CSTRING
		   ""), argtab, "", (union meltparam_un *) 0);
  }

  /*^apply */
  /*apply */
  {
    union meltparam_un argtab[3];
    memset (&argtab, 0, sizeof (argtab));
    /*^apply.arg */
    argtab[0].meltbp_aptr = (melt_ptr_t *) & /*_.PREVENV___V4*/ meltfptr[3];
    /*^apply.arg */
    argtab[1].meltbp_cstring = "CLASS_ENVIRONMENT";
    /*^apply.arg */
    argtab[2].meltbp_cstring = "warmelt-modes";
    /*_.CLASS_ENVIRONMENT__V9*/ meltfptr[8] =
      melt_apply ((meltclosure_ptr_t) ( /*_.VALIMPORT___V6*/ meltfptr[5]),
		  (melt_ptr_t) ( /*_.VALDATA___V18*/ meltfptr[17]),
		  (MELTBPARSTR_PTR MELTBPARSTR_CSTRING MELTBPARSTR_CSTRING
		   ""), argtab, "", (union meltparam_un *) 0);
  }

  /*^apply */
  /*apply */
  {
    union meltparam_un argtab[3];
    memset (&argtab, 0, sizeof (argtab));
    /*^apply.arg */
    argtab[0].meltbp_aptr = (melt_ptr_t *) & /*_.PREVENV___V4*/ meltfptr[3];
    /*^apply.arg */
    argtab[1].meltbp_cstring = "CLASS_SYMBOL";
    /*^apply.arg */
    argtab[2].meltbp_cstring = "warmelt-modes";
    /*_.CLASS_SYMBOL__V10*/ meltfptr[9] =
      melt_apply ((meltclosure_ptr_t) ( /*_.VALIMPORT___V6*/ meltfptr[5]),
		  (melt_ptr_t) ( /*_.VALDATA___V20*/ meltfptr[19]),
		  (MELTBPARSTR_PTR MELTBPARSTR_CSTRING MELTBPARSTR_CSTRING
		   ""), argtab, "", (union meltparam_un *) 0);
  }

  /*^apply */
  /*apply */
  {
    union meltparam_un argtab[3];
    memset (&argtab, 0, sizeof (argtab));
    /*^apply.arg */
    argtab[0].meltbp_aptr = (melt_ptr_t *) & /*_.PREVENV___V4*/ meltfptr[3];
    /*^apply.arg */
    argtab[1].meltbp_cstring = "FIND_ENV";
    /*^apply.arg */
    argtab[2].meltbp_cstring = "warmelt-modes";
    /*_.FIND_ENV__V11*/ meltfptr[10] =
      melt_apply ((meltclosure_ptr_t) ( /*_.VALIMPORT___V6*/ meltfptr[5]),
		  (melt_ptr_t) ( /*_.VALDATA___V22*/ meltfptr[21]),
		  (MELTBPARSTR_PTR MELTBPARSTR_CSTRING MELTBPARSTR_CSTRING
		   ""), argtab, "", (union meltparam_un *) 0);
  }

  /*^comment */
 /**COMMENT: after getting imported values **/ ;

 /**initial routine cdata initializer**/

  memset (predefinited, 0, sizeof (predefinited));
  initialize_module_meltdata_warmelt_modes (&meltfram__, predefinited);
 /**initial routine body**/

  warmelt_modes_initialmeltchunk_0 (&meltfram__, predefinited);;
  goto labend_rout;
labend_rout:;
  melt_topframe = (struct melt_callframe_st *) meltfram__.mcfr_prev;
/* popped initial frame */
  {				/* clear initial frame & return */
    void *retval = /*_.RETINIT___V1*/ meltfptr[0];
    memset ((void *) &meltfram__, 0, sizeof (meltfram__));
    return retval;
  }
#undef meltcallcount
#undef meltfram__
#undef MELTFRAM_NBVARNUM
#undef MELTFRAM_NBVARPTR

}				/* end start_module_melt */

void MELT_MODULE_VISIBILITY
warmelt_modes_initialmeltchunk_0 (struct frame_start_module_melt_st
				  *meltmeltframptr__, char predefinited[])
{
#define meltfram__ (*meltmeltframptr__)
#undef meltcallcount
#define meltcallcount 0L
/**COMMENT: start of oinibody **/ ;

  MELT_LOCATION ("warmelt-modes.melt:33:/ initchunk");
  /*^putroutconst */
  /*putroutconst */
  melt_assertmsg ("putroutconst checkrout",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALROUT___V12*/ meltfptr[11])) ==
		  MELTOBMAG_ROUTINE);
  if (MELT_HAS_INITIAL_ENVIRONMENT)
    melt_checkmsg ("putroutconst constnull.drout_1__SHOWVAR_DOCMD#0",
		   NULL != ( /*_.DEBUG_MSG_FUN__V7*/ meltfptr[6]));
  ((meltroutine_ptr_t) /*_.VALROUT___V12*/ meltfptr[11])->tabval[0] =
    (melt_ptr_t) ( /*_.DEBUG_MSG_FUN__V7*/ meltfptr[6]);

  MELT_LOCATION ("warmelt-modes.melt:33:/ initchunk");
  /*^putroutconst */
  /*putroutconst */
  melt_assertmsg ("putroutconst checkrout",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALROUT___V12*/ meltfptr[11])) ==
		  MELTOBMAG_ROUTINE);
  if (MELT_HAS_INITIAL_ENVIRONMENT)
    melt_checkmsg ("putroutconst constnull.drout_1__SHOWVAR_DOCMD#1",
		   NULL != ( /*_.PREVENV___V4*/ meltfptr[3]));
  ((meltroutine_ptr_t) /*_.VALROUT___V12*/ meltfptr[11])->tabval[1] =
    (melt_ptr_t) ( /*_.PREVENV___V4*/ meltfptr[3]);

  MELT_LOCATION ("warmelt-modes.melt:33:/ initchunk");
  /*^putroutconst */
  /*putroutconst */
  melt_assertmsg ("putroutconst checkrout",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALROUT___V12*/ meltfptr[11])) ==
		  MELTOBMAG_ROUTINE);
  if (MELT_HAS_INITIAL_ENVIRONMENT)
    melt_checkmsg ("putroutconst constnull.drout_1__SHOWVAR_DOCMD#2",
		   NULL != ( /*_.DISCR_STRING__V8*/ meltfptr[7]));
  ((meltroutine_ptr_t) /*_.VALROUT___V12*/ meltfptr[11])->tabval[2] =
    (melt_ptr_t) ( /*_.DISCR_STRING__V8*/ meltfptr[7]);

  MELT_LOCATION ("warmelt-modes.melt:33:/ initchunk");
  /*^putroutconst */
  /*putroutconst */
  melt_assertmsg ("putroutconst checkrout",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALROUT___V12*/ meltfptr[11])) ==
		  MELTOBMAG_ROUTINE);
  if (MELT_HAS_INITIAL_ENVIRONMENT)
    melt_checkmsg ("putroutconst constnull.drout_1__SHOWVAR_DOCMD#3",
		   NULL != ( /*_.CLASS_ENVIRONMENT__V9*/ meltfptr[8]));
  ((meltroutine_ptr_t) /*_.VALROUT___V12*/ meltfptr[11])->tabval[3] =
    (melt_ptr_t) ( /*_.CLASS_ENVIRONMENT__V9*/ meltfptr[8]);

  MELT_LOCATION ("warmelt-modes.melt:33:/ initchunk");
  /*^putroutconst */
  /*putroutconst */
  melt_assertmsg ("putroutconst checkrout",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALROUT___V12*/ meltfptr[11])) ==
		  MELTOBMAG_ROUTINE);
  if (MELT_HAS_INITIAL_ENVIRONMENT)
    melt_checkmsg ("putroutconst constnull.drout_1__SHOWVAR_DOCMD#4",
		   NULL != ( /*_.CLASS_SYMBOL__V10*/ meltfptr[9]));
  ((meltroutine_ptr_t) /*_.VALROUT___V12*/ meltfptr[11])->tabval[4] =
    (melt_ptr_t) ( /*_.CLASS_SYMBOL__V10*/ meltfptr[9]);

  MELT_LOCATION ("warmelt-modes.melt:33:/ initchunk");
  /*^putroutconst */
  /*putroutconst */
  melt_assertmsg ("putroutconst checkrout",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALROUT___V12*/ meltfptr[11])) ==
		  MELTOBMAG_ROUTINE);
  if (MELT_HAS_INITIAL_ENVIRONMENT)
    melt_checkmsg ("putroutconst constnull.drout_1__SHOWVAR_DOCMD#5",
		   NULL != ( /*_.FIND_ENV__V11*/ meltfptr[10]));
  ((meltroutine_ptr_t) /*_.VALROUT___V12*/ meltfptr[11])->tabval[5] =
    (melt_ptr_t) ( /*_.FIND_ENV__V11*/ meltfptr[10]);

  MELT_LOCATION ("warmelt-modes.melt:33:/ initchunk");
  /*^touch */
  /*touch:IROUTVAL_ */
  meltgc_touch ( /*_.VALROUT___V12*/ meltfptr[11]);

  /*putclosurout#1 */
  melt_assertmsg ("putclosrout#1 checkclo",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALCLO___V13*/ meltfptr[12])) ==
		  MELTOBMAG_CLOSURE);
  melt_assertmsg ("putclosrout#1 checkrout",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALROUT___V12*/ meltfptr[11])) ==
		  MELTOBMAG_ROUTINE);
  ((meltclosure_ptr_t) /*_.VALCLO___V13*/ meltfptr[12])->rout =
    (meltroutine_ptr_t) ( /*_.VALROUT___V12*/ meltfptr[11]);

  /*touch:DATACLOSURE_ */
  meltgc_touch ( /*_.VALCLO___V13*/ meltfptr[12]);

  /*putslot */
  melt_assertmsg ("putslot checkobj",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALDATA___V14*/ meltfptr[13])) ==
		  MELTOBMAG_OBJECT);

  melt_assertmsg ("putslot checkoff",
		  (1 >= 0
		   && 1 <
		   melt_object_length ((melt_ptr_t)
				       ( /*_.VALDATA___V14*/ meltfptr[13]))));
  ((meltobject_ptr_t) ( /*_.VALDATA___V14*/ meltfptr[13]))->obj_vartab[1] =
    (melt_ptr_t) (
  /*_.VALSTR___V15*/ meltfptr[14]);

  /*touch:VALUEDATA_ */
  meltgc_touch ( /*_.VALDATA___V14*/ meltfptr[13]);

  /*putslot */
  melt_assertmsg ("putslot checkobj",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALDATA___V16*/ meltfptr[15])) ==
		  MELTOBMAG_OBJECT);

  melt_assertmsg ("putslot checkoff",
		  (1 >= 0
		   && 1 <
		   melt_object_length ((melt_ptr_t)
				       ( /*_.VALDATA___V16*/ meltfptr[15]))));
  ((meltobject_ptr_t) ( /*_.VALDATA___V16*/ meltfptr[15]))->obj_vartab[1] =
    (melt_ptr_t) (
  /*_.VALSTR___V17*/ meltfptr[16]);

  /*touch:VALUEDATA_ */
  meltgc_touch ( /*_.VALDATA___V16*/ meltfptr[15]);

  /*putslot */
  melt_assertmsg ("putslot checkobj",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALDATA___V18*/ meltfptr[17])) ==
		  MELTOBMAG_OBJECT);

  melt_assertmsg ("putslot checkoff",
		  (1 >= 0
		   && 1 <
		   melt_object_length ((melt_ptr_t)
				       ( /*_.VALDATA___V18*/ meltfptr[17]))));
  ((meltobject_ptr_t) ( /*_.VALDATA___V18*/ meltfptr[17]))->obj_vartab[1] =
    (melt_ptr_t) (
  /*_.VALSTR___V19*/ meltfptr[18]);

  /*touch:VALUEDATA_ */
  meltgc_touch ( /*_.VALDATA___V18*/ meltfptr[17]);

  /*putslot */
  melt_assertmsg ("putslot checkobj",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALDATA___V20*/ meltfptr[19])) ==
		  MELTOBMAG_OBJECT);

  melt_assertmsg ("putslot checkoff",
		  (1 >= 0
		   && 1 <
		   melt_object_length ((melt_ptr_t)
				       ( /*_.VALDATA___V20*/ meltfptr[19]))));
  ((meltobject_ptr_t) ( /*_.VALDATA___V20*/ meltfptr[19]))->obj_vartab[1] =
    (melt_ptr_t) (
  /*_.VALSTR___V21*/ meltfptr[20]);

  /*touch:VALUEDATA_ */
  meltgc_touch ( /*_.VALDATA___V20*/ meltfptr[19]);

  /*putslot */
  melt_assertmsg ("putslot checkobj",
		  melt_magic_discr ((melt_ptr_t)
				    ( /*_.VALDATA___V22*/ meltfptr[21])) ==
		  MELTOBMAG_OBJECT);

  melt_assertmsg ("putslot checkoff",
		  (1 >= 0
		   && 1 <
		   melt_object_length ((melt_ptr_t)
				       ( /*_.VALDATA___V22*/ meltfptr[21]))));
  ((meltobject_ptr_t) ( /*_.VALDATA___V22*/ meltfptr[21]))->obj_vartab[1] =
    (melt_ptr_t) (
  /*_.VALSTR___V23*/ meltfptr[22]);

  /*touch:VALUEDATA_ */
  meltgc_touch ( /*_.VALDATA___V22*/ meltfptr[21]);

 /**COMMENT: before toplevel body **/ ;

  MELT_LOCATION ("warmelt-modes.melt:3:/ initchunk");
  /*^block */
  /*anyblock */
  {

    /*^cond */
    /*cond */ if (
		   /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start */
							/*_.CONTENV___V2*/ meltfptr[1])
							/*then */
      {
	/*^cond.then */
  /*_.OR___V24*/ meltfptr[23] =
	  /*quasi.cur.mod.env.cont cur.mod.env.cont : at very start */
/*_.CONTENV___V2*/ meltfptr[1];;
      }
    else
      {
	MELT_LOCATION ("warmelt-modes.melt:3:/ cond.else");

	/*^block */
	/*anyblock */
	{

	  /*^getslot */
	  {
	    melt_ptr_t slot = 0;
	    melt_object_get_field (slot, (melt_ptr_t) (((void *) (MELT_PREDEF (INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");
															/*_.SYSDATA_CONT_FRESH_ENV__V25*/
	    meltfptr[24] = slot;
	  };
	  ;
	  /*^apply */
	  /*apply */
	  {
	    union meltparam_un argtab[1];
	    memset (&argtab, 0, sizeof (argtab));
	    /*^apply.arg */
	    argtab[0].meltbp_cstring = "warmelt-modes";
	    /*_.FUN___V26*/ meltfptr[25] =
	      melt_apply ((meltclosure_ptr_t)
			  ( /*_.SYSDATA_CONT_FRESH_ENV__V25*/ meltfptr[24]),
			  (melt_ptr_t) ( /*_.PREVENV___V4*/ meltfptr[3]),
			  (MELTBPARSTR_CSTRING ""), argtab, "",
			  (union meltparam_un *) 0);
	  }
	  ;
	  /*_.OR___V24*/ meltfptr[23] = /*_.FUN___V26*/ meltfptr[25];;
	  /*epilog */

	  MELT_LOCATION ("warmelt-modes.melt:3:/ clear");
	      /*clear *//*_.SYSDATA_CONT_FRESH_ENV__V25*/ meltfptr[24] = 0;
	  /*^clear */
	      /*clear *//*_.FUN___V26*/ meltfptr[25] = 0;
	}
	;
      }
    ;
    /*^block */
    /*com.block:upd.cur.mod.env.cont : at very start */
    {
    }				/*com.end block:upd.cur.mod.env.cont : at very start */
    ;
    /*epilog */

    /*^clear */
	    /*clear *//*_.OR___V24*/ meltfptr[23] = 0;
  }

 /**COMMENT: after toplevel body **/ ;

 /**COMMENT: compute boxloc again **/ ;

  /*cond */ if ( /*_.CONTENV___V2*/ meltfptr[1] || melt_object_length ((melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA)) < FSYSDAT_BOX_FRESH_ENV)	/*then */
    {
    }
  else
    {				/*^cond.else */

      /*^block */
      /*com.block:compute fresh module environment */
      {

	/*^comment */
   /**COMMENT: start computing boxloc **/ ;
	;
	/*^getslot */
	{
	  melt_ptr_t slot = 0;
	  melt_object_get_field (slot, (melt_ptr_t) (((void *) (MELT_PREDEF (INITIAL_SYSTEM_DATA)))), 3, "SYSDATA_CONT_FRESH_ENV");
														       /*_.FRESHENV___V3*/
	  meltfptr[2] = slot;
	};
	;
	/*^compute */

	if (( /*_.PREVENV___V4*/ meltfptr[3])
	    &&
	    melt_magic_discr ((melt_ptr_t) ( /*_.FRESHENV___V3*/ meltfptr[2]))
	    != MELTOBMAG_CLOSURE)
	  warning (0, "bad MELT fresh_env @%p in system data <%s:%d>",
		   /*_.FRESHENV___V3*/ meltfptr[2], __FILE__, __LINE__);;;
	/*^apply */
	/*apply */
	{
	  /*_.CONTENV___V2*/ meltfptr[1] =
	    melt_apply ((meltclosure_ptr_t)
			( /*_.FRESHENV___V3*/ meltfptr[2]),
			(melt_ptr_t) ( /*_.PREVENV___V4*/ meltfptr[3]), (""),
			(union meltparam_un *) 0, "",
			(union meltparam_un *) 0);
	}
	;
      }				/*com.end block:compute fresh module environment */
      ;
    }

 /**COMMENT: intern symbols **/ ;

  /*internsym:DEBUG_MSG_FUN */
  (void)
    meltgc_intern_symbol ((melt_ptr_t) ( /*_.VALDATA___V14*/ meltfptr[13]));

  /*internsym:DISCR_STRING */
  (void)
    meltgc_intern_symbol ((melt_ptr_t) ( /*_.VALDATA___V16*/ meltfptr[15]));

  /*internsym:CLASS_ENVIRONMENT */
  (void)
    meltgc_intern_symbol ((melt_ptr_t) ( /*_.VALDATA___V18*/ meltfptr[17]));

  /*internsym:CLASS_SYMBOL */
  (void)
    meltgc_intern_symbol ((melt_ptr_t) ( /*_.VALDATA___V20*/ meltfptr[19]));

  /*internsym:FIND_ENV */
  (void)
    meltgc_intern_symbol ((melt_ptr_t) ( /*_.VALDATA___V22*/ meltfptr[21]));

 /**COMMENT: set retinit from boxloc **/ ;

 /*_.RETINIT___V1*/ meltfptr[0] =
    /*final freshenv get */
    melt_container_value ((melt_ptr_t) ( /*_.CONTENV___V2*/ meltfptr[1]));
 /**COMMENT: end the initproc **/ ;


#undef meltfram__
}				/*end of warmelt_modes_initialmeltchunk_0 */

static void
forward_or_mark_module_start_frame_warmelt_modes (struct melt_callframe_st
						  *fp, int marking)
{
  int ix = 0;
  initial_frame_st *meltframptr_ = (initial_frame_st *) fp;
  melt_assertmsg ("check module frame",
		  meltframptr_->mcfr_nbvar == /*minihash */ -2859);
  if (!marking && melt_is_forwarding)
    {
      dbgprintf
	("forward_or_mark_module_start_frame_warmelt_modes forwarding %d pointers in frame %p",
	 26, (void *) meltframptr_);
      for (ix = 0; ix < 26; ix++)
	MELT_FORWARDED (meltframptr_->mcfr_varptr[ix]);
      return;
    }				/*end forwarding */
  dbgprintf
    ("forward_or_mark_module_start_frame_warmelt_modes marking in frame %p",
     (void *) meltframptr_);
  for (ix = 0; ix < 26; ix++)
    if (meltframptr_->mcfr_varptr[ix])
      gt_ggc_mx_melt_un (meltframptr_->mcfr_varptr[ix]);

}				/* end forward_or_mark_module_start_frame_warmelt_modes */


/* exported 0 field offsets */

/* exported 0 class lengths */


/**** end of warmelt-modes ****/
