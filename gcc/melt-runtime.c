/* Basile's static analysis (should have a better name) melt-runtime.c
   Middle End Lisp Translator = MELT
   Copyright (C) 2008, 2009 Free Software Foundation, Inc.
   Contributed by Basile Starynkevitch <basile@starynkevitch.net>
   Indented with GNU indent 

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
along with GCC; see the file COPYING3.   If not see
<http://www.gnu.org/licenses/>.   
  */

/* for debugging -fmelt-debug is useful */

/* to compile as a plugin, try compiling with -DMELT_IS_PLUGIN */

#ifdef MELT_IS_PLUGIN
#include "gcc-plugin.h"
#endif


/* the files marked notpluginexported are not exported by gcc-trunk in
   PLUGIN_HEADERS */
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "obstack.h"
#include "tm.h"
#include "tree.h"
#include "gimple.h"
#include "intl.h"
#include "filenames.h"
#include "tree-pass.h"
#include "tree-dump.h"
#include "tree-flow.h" /*notpluginexported*/
#include "tree-iterator.h" /*notpluginexported*/
#include "tree-inline.h" /*notpluginexported*/
#include "basic-block.h"
#include "timevar.h"


#include "ggc.h"
#include "cgraph.h" /*notpluginexported*/
#include "diagnostic.h" /*notpluginexported*/
#include "flags.h" /*notpluginexported*/
#include "toplev.h"
#include "options.h"
#include "params.h" /*notpluginexported*/
#include "real.h" /*notpluginexported*/
#include "prefix.h" /*notpluginexported*/
#include "md5.h" /*notpluginexported*/
#include "plugin.h" /*notpluginexported*/
#include "cppdefault.h" /*notpluginexported*/

/* executable_checksum is declared in c-common.h which we can't
   include here.. */
extern const unsigned char executable_checksum[16];

/* the generating GGC marking routine */
extern void gt_ggc_mx_melt_un (void *);




#include <dirent.h>

#include <dlfcn.h>


#include <ppl_c.h>


/* meltgc_sort_multiple needs setjmp */
#include <setjmp.h>

/* we need GDBM here */
#include <gdbm.h>

#include "melt-runtime.h"


#ifdef MELT_IS_PLUGIN
int flag_melt_debug;
/**
   NOTE:  july 2009
  
   This code does not yet compile in plugin mode, unless the gengtype
   is suitably patched.

   in addition, libiberty is not fully available from a plugin. So we
   need to reproduce here some functions provided in libiberty.h
**/
char *
xstrndup (const char *s, size_t n)
{
  char *result;
  size_t len = strlen (s);
  if (n < len)
    len = n;
  result = XNEWVEC (char, len + 1);
  result[len] = '\0';
  return (char *) memcpy (result, s, len);
}
#endif




#ifndef MELT_PRIVATE_INCLUDE_DIR
#error MELT_PRIVATE_INCLUDE_DIR is not defined thru compile flags
#endif

#ifndef MELT_SOURCE_DIR
#error MELT_SOURCE_DIR is not defined thru compile flags
#endif

#ifndef MELT_MODULE_DIR
#error MELT_MODULE_DIR is not defined thru compile flags
#endif

#ifndef MELT_COMPILE_SCRIPT
#error MELT_COMPILE_SCRIPT  is not defined thru compile flags
#endif

#ifndef MELT_DEFAULT_MODLIS
#error MELT_DEFAULT_MODLIS is not defined thru compile flags
#endif

/* we use the plugin registration facilities, so this is the plugin
   name in use */
static const char* melt_plugin_name;




/* *INDENT-OFF* */
static const char melt_private_include_dir[] = MELT_PRIVATE_INCLUDE_DIR;
static const char melt_source_dir[] = MELT_SOURCE_DIR;
static const char melt_module_dir[] = MELT_MODULE_DIR;
static const char melt_compile_script[] = MELT_COMPILE_SCRIPT;

melt_ptr_t melt_globarr[MELTGLOB__LASTGLOB]={0};
void* melt_startalz=NULL;
void* melt_endalz=NULL;
char* melt_curalz=NULL;
void** melt_storalz=NULL;

static long melt_minorsizekilow = 0;
static long melt_fullthresholdkilow = 0;

typedef struct melt_module_info_st
{
  void *dlh;			/* dlopen handle */
  void **iniframp;		/* initial frame pointer adress */
  void (*marker_rout) (void *);	/* marking routine of initial frame */
  melt_ptr_t (*start_rout) (melt_ptr_t);	/* start routine */
} melt_module_info_t;

DEF_VEC_O (melt_module_info_t);
DEF_VEC_ALLOC_O (melt_module_info_t, heap);

static VEC (melt_module_info_t, heap) *modinfvec = 0;

struct callframe_melt_st* melt_topframe; 
struct meltlocalsptr_st* melt_localtab;


/** special values are linked in a list to permit their explicit deletion */
struct meltspecial_st* melt_newspeclist;
struct meltspecial_st* melt_oldspeclist;
					 
unsigned long melt_kilowords_sincefull;
/* number of full & any melt garbage collections */
unsigned long melt_nb_full_garbcoll;
unsigned long melt_nb_garbcoll;
void* melt_touched_cache[MELT_TOUCHED_CACHE_SIZE];
bool melt_prohibit_garbcoll;

long melt_dbgcounter;
long melt_debugskipcount;


int melt_last_global_ix = MELTGLOB__LASTGLOB;

/* our copying garbage collector needs a vector of melt_ptr_t to
   scan, a la Cheney.  */
static GTY(()) VEC(melt_ptr_t,gc) *bscanvec;

				  

static void* proghandle;


/* *INDENT-ON* */

/* to code case ALL_OBMAG_SPECIAL_CASES: */
#define ALL_OBMAG_SPECIAL_CASES			\
         OBMAG_SPEC_FILE:			\
    case OBMAG_SPEC_RAWFILE:			\
    case OBMAG_SPECPPL_COEFFICIENT:   		\
    case OBMAG_SPECPPL_LINEAR_EXPRESSION:	\
    case OBMAG_SPECPPL_CONSTRAINT:		\
    case OBMAG_SPECPPL_CONSTRAINT_SYSTEM:	\
    case OBMAG_SPECPPL_GENERATOR:		\
    case OBMAG_SPECPPL_GENERATOR_SYSTEM:	\
    case OBMAG_SPECPPL_POLYHEDRON:		\
    case OBMAG_SPEC_MPFR

/* Obstack used for reading names */
static struct obstack bname_obstack;


/* retrieve a MELT related program or plugin argument */
#ifdef MELT_IS_PLUGIN
static int melt_plugin_argc;
static struct plugin_argument* melt_plugin_argv;

const char*
melt_argument(const char* argname)
{
  int argix=0;
  if (!argname || !argname[0])
    return NULL;
  for (argix = 0; argix < melt_plugin_argc; argix ++) 
    {
      if (!strcmp(argname, melt_plugin_argv[argix].key)) 
	{
	  char* val = melt_plugin_argv[argix].value;
	  /* never return NULL if the argument is found; return an
	     empty string if no value given */
	  if (!val)
	    return "";
	  return val;
	}
    }
  return NULL;
}

#else
/* builtin MELT, retrieve the MELT relevant program argument */
const char*
melt_argument (const char* argname)
{
  if (!argname || !argname[0]) 
    return NULL;
  else if (!strcmp (argname, "mode")) 
    return melt_mode_string;
  else if (!strcmp (argname, "arg"))
    return melt_argument_string;
  else if (!strcmp (argname, "arglist"))
    return melt_arglist_string;
  else if (!strcmp (argname, "compile-script"))
    return melt_compile_script_string;
  else if (!strcmp (argname, "debug"))
    return flag_melt_debug?"yes":NULL;
  else if (!strcmp (argname, "debugskip"))
    return count_melt_debugskip_string;
  else if (!strcmp (argname, "module-path"))
    return melt_dynmodpath_string;
  else if (!strcmp (argname, "gdbmstate"))
    return melt_gdbmstate_string;
  else if (!strcmp (argname, "source-path"))
    return melt_srcpath_string;
  else if (!strcmp (argname, "init"))
    return melt_init_string;
  else if (!strcmp (argname, "secondarg"))
    return melt_secondargument_string;
  else if (!strcmp (argname, "tempdir"))
    return melt_tempdir_string;
  /* currently, minor-zone & full-threshold are parameters, so we make
     a string out of them */
  else if (!strcmp (argname, "minor-zone"))
    {
      static char minzonstr[40];
      if (!minzonstr[0])
	snprintf(minzonstr, sizeof (minzonstr) - 1, "%d", 
		 PARAM_VALUE(PARAM_MELT_MINOR_ZONE));
      return minzonstr;
    }
  else if (!strcmp (argname, "full-threshold"))
    {
      static char fullthrstr[40];
      if (!fullthrstr[0])
	snprintf(fullthrstr, sizeof (fullthrstr) - 1, "%d",
		 PARAM_VALUE(PARAM_MELT_FULL_THRESHOLD));
      return fullthrstr;
    }
  return NULL;
}
#endif /*MELT_IS_PLUGIN*/
#if defined(__GNUC__) && __GNUC__>3
#pragma GCC poison melt_mode_string melt_argument_string melt_arglist_string
/* don't poison flag_melt_debug */
#pragma GCC poison melt_compile_script_string count_melt_debugskip_string
#pragma GCC poison melt_dynmodpath_string melt_gdbmstate_string melt_srcpath_string
#pragma GCC poison melt_init_string melt_secondargument_string melt_tempdir_string
#endif



static inline void
delete_special (struct meltspecial_st *sp)
{
  switch (sp->discr->object_magic)
    {
    case OBMAG_SPEC_FILE:
      if (sp->val.sp_file)
	{
	  fclose (sp->val.sp_file);
	  sp->val.sp_file = NULL;
	};
      break;
    case OBMAG_SPEC_RAWFILE:
      if (sp->val.sp_file)
	{
	  fflush (sp->val.sp_file);
	  sp->val.sp_file = NULL;
	};
      break;
    case OBMAG_SPEC_MPFR:
      if (sp->val.sp_mpfr)
	{
	  mpfr_clear ((mpfr_ptr) (sp->val.sp_mpfr));
	  free (sp->val.sp_mpfr);
	  sp->val.sp_mpfr = NULL;
	};
      break;
    case OBMAG_SPECPPL_COEFFICIENT:
      if (sp->val.sp_coefficient)
	ppl_delete_Coefficient (sp->val.sp_coefficient);
      sp->val.sp_coefficient = NULL;
      break;
    case OBMAG_SPECPPL_LINEAR_EXPRESSION:
      if (sp->val.sp_linear_expression)
	ppl_delete_Linear_Expression (sp->val.sp_linear_expression);
      sp->val.sp_linear_expression = NULL;
      break;
    case OBMAG_SPECPPL_CONSTRAINT:
      if (sp->val.sp_constraint)
	ppl_delete_Constraint (sp->val.sp_constraint);
      sp->val.sp_constraint = NULL;
      break;
    case OBMAG_SPECPPL_CONSTRAINT_SYSTEM:
      if (sp->val.sp_constraint_system)
	ppl_delete_Constraint_System (sp->val.sp_constraint_system);
      sp->val.sp_constraint_system = NULL;
      break;
    case OBMAG_SPECPPL_GENERATOR:
      if (sp->val.sp_generator)
	ppl_delete_Generator (sp->val.sp_generator);
      sp->val.sp_generator = NULL;
      break;
    case OBMAG_SPECPPL_GENERATOR_SYSTEM:
      if (sp->val.sp_generator_system)
	ppl_delete_Generator_System (sp->val.sp_generator_system);
      sp->val.sp_generator_system = NULL;
      break;
    case OBMAG_SPECPPL_POLYHEDRON:
      if (sp->val.sp_polyhedron)
	ppl_delete_Polyhedron (sp->val.sp_polyhedron);
      sp->val.sp_polyhedron = NULL;
      break;
    default:
      break;
    }
}

#define FORWARDED_DISCR (meltobject_ptr_t)1
static melt_ptr_t forwarded_copy (melt_ptr_t);

#ifdef ENABLE_CHECKING
/* only for debugging, to be set from the debugger */


static FILE *debughack_file;
FILE *melt_dbgtracefile;
void *melt_checkedp_ptr1;
void *melt_checkedp_ptr2;
#endif /*ENABLE_CHECKING */

/*** GDBM state ****/
static GDBM_FILE gdbm_melt;


static inline void *
forwarded (void *ptr)
{
  melt_ptr_t p = (melt_ptr_t) ptr;
  if (p && melt_is_young (p))
    {
      if (p->u_discr == FORWARDED_DISCR)
	p = ((struct meltforward_st *) p)->forward;
      else
	p = forwarded_copy (p);
    }
  return p;
}

#if GCC_VERSION > 4000
#define FORWARDED(P) do {if (P) { \
  (P) = (__typeof__(P))forwarded((void*)(P));} } while(0)
#else
#define FORWARDED(P) do {if (P) { 		       		\
       (P) = (melt_ptr_t)forwarded((melt_ptr_t)(P));} }  while(0)
#endif
static void scanning (melt_ptr_t);



#if ENABLE_CHECKING
/***
 * check our call frames
 ***/
static inline void
check_pointer_at (const char msg[], long count, melt_ptr_t * pptr,
		  const char *filenam, int lineno)
{
  melt_ptr_t ptr = *pptr;
  if (!ptr)
    return;
  if (!ptr->u_discr)
    fatal_error
      ("<%s#%ld> corrupted pointer %p (at %p) without discr at %s:%d", msg,
       count, (void *) ptr, (void *) pptr, basename (filenam), lineno);
  switch (ptr->u_discr->object_magic)
    {
    case OBMAG_OBJECT:
    case OBMAG_DECAY:
    case OBMAG_BOX:
    case OBMAG_MULTIPLE:
    case OBMAG_CLOSURE:
    case OBMAG_ROUTINE:
    case OBMAG_LIST:
    case OBMAG_PAIR:
    case OBMAG_TRIPLE:
    case OBMAG_INT:
    case OBMAG_MIXINT:
    case OBMAG_MIXLOC:
    case OBMAG_MIXBIGINT:
    case OBMAG_REAL:
    case OBMAG_STRING:
    case OBMAG_STRBUF:
    case OBMAG_TREE:
    case OBMAG_GIMPLE:
    case OBMAG_GIMPLESEQ:
    case OBMAG_BASICBLOCK:
    case OBMAG_EDGE:
    case OBMAG_MAPOBJECTS:
    case OBMAG_MAPTREES:
    case OBMAG_MAPGIMPLES:
    case OBMAG_MAPGIMPLESEQS:
    case OBMAG_MAPSTRINGS:
    case OBMAG_MAPBASICBLOCKS:
    case OBMAG_MAPEDGES:
    case ALL_OBMAG_SPECIAL_CASES:
      break;
    default:
      fatal_error ("<%s#%ld> bad pointer %p (at %p) bad magic %d at %s:%d",
		   msg, count, (void *) ptr, (void *) pptr,
		   (int) ptr->u_discr->object_magic, basename (filenam),
		   lineno);
    }
}

static long nbcheckcallframes;
static long thresholdcheckcallframes;


/* make a special value; return NULL if the discriminant is not special */
struct meltspecial_st*
meltgc_make_special(melt_ptr_t discr_p)
{
  MELT_ENTERFRAME (2, NULL);
#define discrv       curfram__.varptr[0]
#define specv      curfram__.varptr[1]
#define sp_specv ((struct meltspecial_st*)(specv))
  discrv = discr_p;
  if (!discrv || melt_magic_discr((melt_ptr_t)discrv) != OBMAG_OBJECT)
    goto end;
  switch (((meltobject_ptr_t)discrv)->object_magic) 
    {
    case ALL_OBMAG_SPECIAL_CASES:
      specv = meltgc_allocate (sizeof(struct meltspecial_st),0);
      sp_specv->discr = (meltobject_ptr_t) discrv;
      sp_specv->mark = 0;
      sp_specv->nextspec = melt_newspeclist;
      melt_newspeclist = sp_specv;
      break;
    default: goto end;
    }
 end:
  MELT_EXITFRAME();
  return sp_specv;
#undef discrv
#undef specv
#undef sp_specv
}

void
melt_check_call_frames_at (int noyoungflag, const char *msg,
			      const char *filenam, int lineno)
{
  struct callframe_melt_st *cfram = NULL;
  int nbfram = 0, nbvar = 0;
  nbcheckcallframes++;
  if (!msg)
    msg = "/";
  if (thresholdcheckcallframes > 0
      && nbcheckcallframes > thresholdcheckcallframes)
    {
      debugeprintf
	("start check_call_frames#%ld {%s} from %s:%d",
	 nbcheckcallframes, msg, basename (filenam), lineno);
    }
  for (cfram = melt_topframe; cfram != NULL; cfram = cfram->prev)
    {
      int varix = 0;
      nbfram++;
      if (cfram->clos)
	{
	  if (noyoungflag && melt_is_young (cfram->clos))
	    fatal_error
	      ("bad frame <%s#%ld> unexpected young closure %p in frame %p at %s:%d",
	       msg, nbcheckcallframes,
	       (void *) cfram->clos, (void *) cfram, basename (filenam),
	       lineno);

	  check_pointer_at (msg, nbcheckcallframes,
			    (melt_ptr_t *) (void *) &cfram->clos, filenam,
			    lineno);
	  if (cfram->clos->discr->object_magic != OBMAG_CLOSURE)
	    fatal_error
	      ("bad frame <%s#%ld> invalid closure %p in frame %p at %s:%d",
	       msg, nbcheckcallframes,
	       (void *) cfram->clos, (void *) cfram, basename (filenam),
	       lineno);
	}
      for (varix = ((int) cfram->nbvar) - 1; varix >= 0; varix--)
	{
	  nbvar++;
	  if (noyoungflag && cfram->varptr[varix] != NULL
	      && melt_is_young (cfram->varptr[varix]))
	    fatal_error
	      ("bad frame <%s#%ld> unexpected young pointer %p in frame %p at %s:%d",
	       msg, nbcheckcallframes, (void *) cfram->varptr[varix],
	       (void *) cfram, basename (filenam), lineno);

	  check_pointer_at (msg, nbcheckcallframes, &cfram->varptr[varix],
			    filenam, lineno);
	}
    }
  if (thresholdcheckcallframes > 0
      && nbcheckcallframes > thresholdcheckcallframes)
    debugeprintf ("end check_call_frames#%ld {%s} %d frames/%d vars %s:%d",
		  nbcheckcallframes, msg, nbfram, nbvar, basename (filenam),
		  lineno);
  if (debughack_file)
    {
      fprintf (debughack_file,
	       "check_call_frames#%ld {%s} %d frames/%d vars %s:%d\n",
	       nbcheckcallframes, msg, nbfram, nbvar, basename (filenam),
	       lineno);
      fflush (debughack_file);
    }
}

void
melt_caught_assign_at (void *ptr, const char *fil, int lin,
			  const char *msg)
{
  if (debughack_file)
    {
      fprintf (debughack_file, "caught assign %p at %s:%d /// %s\n", ptr,
	       basename (fil), lin, msg);
      fflush (debughack_file);
    }
  debugeprintf ("caught assign %p at %s:%d /// %s", ptr, basename (fil), lin,
		msg);
}

static long nbcbreak;

void
melt_cbreak_at (const char *msg, const char *fil, int lin)
{
  nbcbreak++;
  if (debughack_file)
    {
      fprintf (debughack_file, "CBREAK#%ld AT %s:%d - %s\n", nbcbreak,
	       basename (fil), lin, msg);
      fflush (debughack_file);
    };
  debugeprintf_raw ("%s:%d: CBREAK#%ld %s\n", basename (fil), lin, nbcbreak,
		    msg);
}

#endif


/***
 * the marking routine is registered thru PLUGIN_GGC_MARKING
 * it makes GGC play nice with MELT.
 **/
static long meltmarkingcount;

static void
melt_marking_callback (void *gcc_data ATTRIBUTE_UNUSED,
			  void* user_data ATTRIBUTE_UNUSED)
{
  int ix = 0;
  melt_module_info_t *mi = 0;
  struct callframe_melt_st *cf = 0;
  meltmarkingcount++;
  dbgprintf("start of melt_marking_callback %ld", meltmarkingcount);
  /* first, scan all the modules and mark their frame if it is non null */
  if (modinfvec) 
    for (ix = 0; VEC_iterate (melt_module_info_t, modinfvec, ix, mi); ix++)
      {
        if ( !mi->marker_rout || !mi->iniframp || !*mi->iniframp) 
	  continue;
        (mi->marker_rout) (*mi->iniframp);
      };
  /* then scan all the MELT call frames */
  for (cf = (struct callframe_melt_st*) melt_topframe; cf != NULL;
       cf = cf->prev) {
    if (cf->clos) {
      meltroutfun_t*funp = 0;
      gcc_assert(cf->clos->rout);
      funp = cf->clos->rout->routfunad;
      gcc_assert(funp);
      gt_ggc_mx_melt_un ((melt_ptr_t)(cf->clos));
      /* call the function specially with the MARKGCC special
	 parameter descriptor */
      funp(cf->clos, (melt_ptr_t)cf, MELTPAR_MARKGGC, 
	   (union meltparam_un*)0, (char*)0, (union meltparam_un*)0);
      continue;
    }
    else
      {
	extern void gt_ggc_mx_melt_un (void *);
	/* if no closure, mark the local pointers */
	for (ix= 0; ix<(int) cf->nbvar; ix++) 
	  if (cf->varptr[ix]) 
	    gt_ggc_mx_melt_un ((melt_ptr_t)(cf->varptr[ix]));
      } 
  }
  dbgprintf("end of melt_marking_callback %ld", meltmarkingcount);
}

/***
 * our copying garbage collector 
 ***/
void
melt_garbcoll (size_t wanted, enum melt_gckind_en gckd)
{

  int ix = 0;
  struct callframe_melt_st *cfram = NULL;
  melt_ptr_t *storp = NULL;
  bool needfull = FALSE;
  struct meltspecial_st *specp = NULL;
  struct meltspecial_st **prevspecptr = NULL;
  struct meltspecial_st *nextspecp = NULL;
  if (melt_prohibit_garbcoll)
    fatal_error ("melt garbage collection prohibited");
  melt_nb_garbcoll++;
  if (gckd == MELT_NEED_FULL)
    needfull = true;
  if (melt_minorsizekilow == 0)
    {
      const char* minzstr = melt_argument ("minor-zone");
      melt_minorsizekilow = minzstr? (atol (minzstr)):0;
      if (melt_minorsizekilow<256) melt_minorsizekilow=256;
      else if (melt_minorsizekilow>16384) melt_minorsizekilow=16384;
    }
  if (melt_fullthresholdkilow == 0)
    {
      const char* fullthstr = melt_argument ("full-threshold");
      melt_fullthresholdkilow = fullthstr ? (atol (fullthstr)) : 0;
      if (melt_fullthresholdkilow<512) melt_fullthresholdkilow=512;
      if (melt_fullthresholdkilow<2*melt_minorsizekilow)
	melt_fullthresholdkilow = 2*melt_minorsizekilow;
      else if (melt_fullthresholdkilow>65536) melt_fullthresholdkilow=65536;
    }
  melt_check_call_frames (MELT_ANYWHERE, "before garbage collection");
  gcc_assert ((char *) melt_startalz < (char *) melt_endalz);
  gcc_assert ((char *) melt_curalz >= (char *) melt_startalz
	      && (char *) melt_curalz < (char *) melt_storalz);
  gcc_assert ((char *) melt_storalz < (char *) melt_endalz);
  bscanvec = VEC_alloc (melt_ptr_t, gc, 1024 + 32 * melt_minorsizekilow);
  wanted += wanted / 4 + melt_minorsizekilow * 1000;
  wanted |= 0x3fff;
  wanted++;
  if (wanted < melt_minorsizekilow * sizeof (void *) * 1024)
    wanted = melt_minorsizekilow * sizeof (void *) * 1024;
  /* don't need anymore to allocate a local table, because of
     melt_marking_callback */
  for (ix = 0; ix < MELTGLOB__LASTGLOB; ix++)
    FORWARDED (melt_globarr[ix]);
  for (cfram = melt_topframe; cfram != NULL; cfram = cfram->prev)
    {
      int varix;
      if (cfram->clos)
	{
	  FORWARDED (cfram->clos);
	}
      for (varix = ((int) cfram->nbvar) - 1; varix >= 0; varix--)
	{
	  if (!cfram->varptr[varix])
	    continue;
	  FORWARDED (cfram->varptr[varix]);
	}
    };
  /* scan the store list */
  for (storp = (melt_ptr_t *) melt_storalz;
       (char *) storp < (char *) melt_endalz; storp++)
    {
      if (*storp)
	scanning (*storp);
    }
  memset (melt_touched_cache, 0, sizeof (melt_touched_cache));
  /* sort of Cheney loop; http://en.wikipedia.org/wiki/Cheney%27s_algorithm */
  while (!VEC_empty (melt_ptr_t, bscanvec))
    {
      melt_ptr_t p = VEC_pop (melt_ptr_t, bscanvec);
      if (!p)
	continue;
#if ENABLE_CHECKING
      if (debughack_file)
	fprintf (debughack_file, "cheney scan %p\n", (void *) p);
#endif
      scanning (p);
    }
  /* delete every unmarked special on the new list and clear it */
  for (specp = melt_newspeclist; specp; specp = specp->nextspec)
    {
      gcc_assert (melt_is_young (specp));
      if (specp->mark)
	continue;
      delete_special (specp);
    }
  melt_newspeclist = NULL;
  VEC_free (melt_ptr_t, gc, bscanvec);
  bscanvec = NULL;
  /* free the previous young zone and allocate a new one */
#if ENABLE_CHECKING
  if (debughack_file)
    {
      fprintf (debughack_file,
	       "%s:%d free previous young %p - %p GC#%ld\n",
	       basename (__FILE__), __LINE__, melt_startalz,
	       melt_endalz, melt_nb_garbcoll);
      fflush (debughack_file);
    }
  memset (melt_startalz, 0,
	  (char *) melt_endalz - (char *) melt_startalz);
#endif
  free (melt_startalz);
  melt_startalz = melt_endalz = melt_curalz = NULL;
  melt_storalz = NULL;
  melt_kilowords_sincefull += wanted / (1024 * sizeof (void *));
  if (gckd > MELT_ONLY_MINOR && melt_kilowords_sincefull >
      (unsigned long) melt_fullthresholdkilow)
    needfull = TRUE;
  melt_startalz = melt_curalz =
    (char *) xcalloc (sizeof (void *), wanted / sizeof (void *));
  melt_endalz = (char *) melt_curalz + wanted;
  melt_storalz = ((void **) melt_endalz) - 2;
  if (needfull)
    {
      bool wasforced = ggc_force_collect;
      melt_nb_full_garbcoll++;
      debugeprintf ("melt_garbcoll #%ld fullgarbcoll #%ld",
		    melt_nb_garbcoll, melt_nb_full_garbcoll);
      /* clear marks on the old spec list */
      for (specp = melt_oldspeclist; specp; specp = specp->nextspec)
	specp->mark = 0;
      /* force major collection, with our callback */
      ggc_force_collect = true;
      ggc_collect ();
      ggc_force_collect = wasforced;
      /* delete the unmarked spec */
      prevspecptr = &melt_oldspeclist;
      for (specp = melt_oldspeclist; specp; specp = nextspecp)
	{
	  nextspecp = specp->nextspec;
	  if (specp->mark)
	    {
	      prevspecptr = &specp->nextspec;
	      continue;
	    }
	  delete_special (specp);
	  memset (specp, 0, sizeof (*specp));
	  ggc_free (specp);
	  *prevspecptr = nextspecp;
	}
      melt_kilowords_sincefull = 0;
    }
  melt_check_call_frames (MELT_NOYOUNG, "after garbage collection");
}


/* the inline function melt_allocatereserved is the only one
   calling this melt_reserved_allocation_failure function, which
   should never be called. If it is indeed called, you've been bitten
   by a severe bug. In principle melt_allocatereserved should have
   been called with a suitable previous call to meltgc_reserve such
   that all the reserved allocations fits into the reserved size */
void
melt_reserved_allocation_failure (long siz)
{
  /* this function should never really be called */
  fatal_error ("memory corruption in melt reserved allocation: "
	       "requiring %ld bytes but only %ld available in young zone",
	       siz,
	       (long) ((char *) melt_storalz - (char *) melt_curalz));
}

/* cheney like forwarding */
static melt_ptr_t
forwarded_copy (melt_ptr_t p)
{
  melt_ptr_t n = 0;
  int mag = 0;
  gcc_assert (melt_is_young (p));
  gcc_assert (p->u_discr && p->u_discr != FORWARDED_DISCR);
  if (p->u_discr->obj_class == FORWARDED_DISCR)
    mag =
      ((meltobject_ptr_t)
       (((struct meltforward_st *) p->u_discr)->forward))->object_magic;
  else
    mag = p->u_discr->object_magic;
  /***
   * we can copy *dst = *src only for structures which do not use
   * FLEXIBLE_DIM; for those that do and which are "empty" this is not
   * possible, since when FLEXIBLE_DIM is 1 it would overwrite
   * something else. 
   *
   * I really hate the C dialect which long time ago
   * prohibited zero-length arrays.
   ***/
  switch (mag)
    {
    case OBMAG_OBJECT:
      {
	struct meltobject_st *src = (struct meltobject_st *) p;
	unsigned oblen = src->obj_len;
	struct meltobject_st *dst = (struct meltobject_st *)
	  ggc_alloc_cleared (offsetof (struct meltobject_st,
				       obj_vartab) +
			     oblen * sizeof (src->obj_vartab[0]));
	int ix;
	/* we cannot copy the whole src, because FLEXIBLE_DIM might be 1 */
	dst->obj_class = src->obj_class;
	dst->obj_hash = src->obj_hash;
	dst->obj_num = src->obj_num;
	dst->obj_len = src->obj_len;
#if ENABLE_CHECKING
	dst->obj_serial = src->obj_serial;
#endif
	if (oblen > 0)
	  for (ix = (int) oblen - 1; ix >= 0; ix--)
	    dst->obj_vartab[ix] = src->obj_vartab[ix];
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_DECAY:
      {
	struct meltdecay_st *src = (struct meltdecay_st *) p;
	struct meltdecay_st *dst = (struct meltdecay_st *)
	  ggc_alloc_cleared (sizeof (struct meltdecay_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_BOX:
      {
	struct meltbox_st *src = (struct meltbox_st *) p;
	struct meltbox_st *dst = (struct meltbox_st *)
	  ggc_alloc_cleared (sizeof (struct meltbox_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MULTIPLE:
      {
	struct meltmultiple_st *src = (struct meltmultiple_st *) p;
	unsigned nbv = src->nbval;
	int ix;
	struct meltmultiple_st *dst = (struct meltmultiple_st *)
	  ggc_alloc_cleared (sizeof (struct meltmultiple_st) +
			     nbv * sizeof (void *));
	/* we cannot copy the whole src, because FLEXIBLE_DIM might be
	   1 and nbval could be 0 */
	dst->discr = src->discr;
	dst->nbval = src->nbval;
	for (ix = (int) nbv; ix >= 0; ix--)
	  dst->tabval[ix] = src->tabval[ix];
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_CLOSURE:
      {
	struct meltclosure_st *src = (struct meltclosure_st *) p;
	unsigned nbv = src->nbval;
	int ix;
	struct meltclosure_st *dst = (struct meltclosure_st *)
	  ggc_alloc_cleared (sizeof (struct meltclosure_st) +
			     nbv * sizeof (void *));
	dst->discr = src->discr;
	dst->rout = src->rout;
	dst->nbval = src->nbval;
	for (ix = (int) nbv; ix >= 0; ix--)
	  dst->tabval[ix] = src->tabval[ix];
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_ROUTINE:
      {
	struct meltroutine_st *src = (struct meltroutine_st *) p;
	unsigned nbv = src->nbval;
	int ix;
	struct meltroutine_st *dst = (struct meltroutine_st *)
	  ggc_alloc_cleared (sizeof (struct meltroutine_st) +
			     nbv * sizeof (void *));
	dst->discr = src->discr;
	strncpy (dst->routdescr, src->routdescr, MELT_ROUTDESCR_LEN);
	dst->routdescr[MELT_ROUTDESCR_LEN - 1] = 0;
	dst->nbval = src->nbval;
	dst->routfunad = src->routfunad;
	for (ix = (int) nbv; ix >= 0; ix--)
	  dst->tabval[ix] = src->tabval[ix];
	dst->routdata = src->routdata;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_LIST:
      {
	struct meltlist_st *src = (struct meltlist_st *) p;
	struct meltlist_st *dst = (struct meltlist_st *)
	  ggc_alloc_cleared (sizeof (struct meltlist_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_PAIR:
      {
	struct meltpair_st *src = (struct meltpair_st *) p;
	struct meltpair_st *dst = (struct meltpair_st *)
	  ggc_alloc_cleared (sizeof (struct meltpair_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_TRIPLE:
      {
	struct melttriple_st *src = (struct melttriple_st *) p;
	struct melttriple_st *dst = (struct melttriple_st *)
	  ggc_alloc_cleared (sizeof (struct melttriple_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_INT:
      {
	struct meltint_st *src = (struct meltint_st *) p;
	struct meltint_st *dst = (struct meltint_st *)
	  ggc_alloc_cleared (sizeof (struct meltint_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MIXINT:
      {
	struct meltmixint_st *src = (struct meltmixint_st *) p;
	struct meltmixint_st *dst = (struct meltmixint_st *)
	  ggc_alloc_cleared (sizeof (struct meltmixint_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MIXLOC:
      {
	struct meltmixloc_st *src = (struct meltmixloc_st *) p;
	struct meltmixloc_st *dst = (struct meltmixloc_st *)
	  ggc_alloc_cleared (sizeof (struct meltmixloc_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MIXBIGINT:
      {
	struct meltmixbigint_st *src = (struct meltmixbigint_st *) p;
	unsigned blen = src->biglen;
	struct meltmixbigint_st *dst = (struct meltmixbigint_st *)
	  ggc_alloc_cleared (sizeof (struct meltmixbigint_st)
			     + blen * sizeof(src->tabig[0]));
	dst->discr = src->discr;
	dst->ptrval = src->ptrval;
	dst->negative = src->negative;
	dst->biglen = blen;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_REAL:
      {
	struct meltreal_st *src = (struct meltreal_st *) p;
	struct meltreal_st *dst = (struct meltreal_st *)
	  ggc_alloc_cleared (sizeof (struct meltreal_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case ALL_OBMAG_SPECIAL_CASES:
      {
	struct meltspecial_st *src = (struct meltspecial_st *) p;
	struct meltspecial_st *dst = (struct meltspecial_st *)
	  ggc_alloc_cleared (sizeof (struct meltspecial_st));
	*dst = *src;
	/* add the new copy to the old (major) special list */
	dst->nextspec = melt_oldspeclist;
	melt_oldspeclist = dst;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_STRING:
      {
	struct meltstring_st *src = (struct meltstring_st *) p;
	int srclen = strlen (src->val);
	struct meltstring_st *dst = (struct meltstring_st *)
	  ggc_alloc_cleared (sizeof (struct meltstring_st) + srclen + 1);
	dst->discr = src->discr;
	memcpy (dst->val, src->val, srclen);
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_STRBUF:
      {
	struct meltstrbuf_st *src = (struct meltstrbuf_st *) p;
	unsigned blen = melt_primtab[src->buflenix];
	struct meltstrbuf_st *dst = (struct meltstrbuf_st *)
	  ggc_alloc_cleared (sizeof (struct meltstrbuf_st));
	dst->discr = src->discr;
	dst->bufstart = src->bufstart;
	dst->bufend = src->bufend;
	dst->buflenix = src->buflenix;
	if (blen > 0)
	  {
	    dst->bufzn = (char *) ggc_alloc_cleared (1 + blen);
	    memcpy (dst->bufzn, src->bufzn, blen);
	  }
	else
	  dst->bufzn = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_TREE:
      {
	struct melttree_st *src = (struct melttree_st *) p;
	struct melttree_st *dst = (struct melttree_st *)
	  ggc_alloc_cleared (sizeof (struct melttree_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_GIMPLE:
      {
	struct meltgimple_st *src = (struct meltgimple_st *) p;
	struct meltgimple_st *dst = (struct meltgimple_st *)
	  ggc_alloc_cleared (sizeof (struct meltgimple_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_GIMPLESEQ:
      {
	struct meltgimpleseq_st *src = (struct meltgimpleseq_st *) p;
	struct meltgimpleseq_st *dst = (struct meltgimpleseq_st *)
	  ggc_alloc_cleared (sizeof (struct meltgimpleseq_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_BASICBLOCK:
      {
	struct meltbasicblock_st *src = (struct meltbasicblock_st *) p;
	struct meltbasicblock_st *dst = (struct meltbasicblock_st *)
	  ggc_alloc_cleared (sizeof (struct meltbasicblock_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_EDGE:
      {
	struct meltedge_st *src = (struct meltedge_st *) p;
	struct meltedge_st *dst = (struct meltedge_st *)
	  ggc_alloc_cleared (sizeof (struct meltedge_st));
	*dst = *src;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MAPOBJECTS:
      {
	struct meltmapobjects_st *src = (struct meltmapobjects_st *) p;
	int siz = melt_primtab[src->lenix];
	struct meltmapobjects_st *dst = (struct meltmapobjects_st *)
	  ggc_alloc_cleared (sizeof (struct meltmapobjects_st));
	dst->discr = src->discr;
	dst->count = src->count;
	dst->lenix = src->lenix;
	if (siz > 0 && src->entab)
	  {
	    dst->entab =
	      (struct entryobjectsmelt_st *) ggc_alloc_cleared (siz *
								   sizeof
								   (dst->entab
								    [0]));
	    memcpy (dst->entab, src->entab, siz * sizeof (dst->entab[0]));
	  }
	else
	  dst->entab = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MAPTREES:
      {
	struct meltmaptrees_st *src = (struct meltmaptrees_st *) p;
	int siz = melt_primtab[src->lenix];
	struct meltmaptrees_st *dst = (struct meltmaptrees_st *)
	  ggc_alloc_cleared (sizeof (struct meltmaptrees_st));
	dst->discr = src->discr;
	dst->count = src->count;
	dst->lenix = src->lenix;
	if (siz > 0 && src->entab)
	  {
	    dst->entab =
	      (struct entrytreesmelt_st *) ggc_alloc_cleared (siz *
								 sizeof
								 (dst->entab
								  [0]));
	    memcpy (dst->entab, src->entab, siz * sizeof (dst->entab[0]));
	  }
	else
	  dst->entab = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MAPGIMPLES:
      {
	struct meltmapgimples_st *src = (struct meltmapgimples_st *) p;
	int siz = melt_primtab[src->lenix];
	struct meltmapgimples_st *dst = (struct meltmapgimples_st *)
	  ggc_alloc_cleared (sizeof (struct meltmapgimples_st));
	dst->discr = src->discr;
	dst->count = src->count;
	dst->lenix = src->lenix;
	if (siz > 0 && src->entab)
	  {
	    dst->entab =
	      (struct entrygimplesmelt_st *)
	      ggc_alloc_cleared (siz * sizeof (dst->entab[0]));
	    memcpy (dst->entab, src->entab, siz * sizeof (dst->entab[0]));
	  }
	else
	  dst->entab = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MAPGIMPLESEQS:
      {
	struct meltmapgimpleseqs_st *src =
	  (struct meltmapgimpleseqs_st *) p;
	int siz = melt_primtab[src->lenix];
	struct meltmapgimpleseqs_st *dst =
	  (struct meltmapgimpleseqs_st *)
	  ggc_alloc_cleared (sizeof (struct meltmapgimpleseqs_st));
	dst->discr = src->discr;
	dst->count = src->count;
	dst->lenix = src->lenix;
	if (siz > 0 && src->entab)
	  {
	    dst->entab =
	      (struct entrygimpleseqsmelt_st *)
	      ggc_alloc_cleared (siz * sizeof (dst->entab[0]));
	    memcpy (dst->entab, src->entab, siz * sizeof (dst->entab[0]));
	  }
	else
	  dst->entab = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MAPSTRINGS:
      {
	struct meltmapstrings_st *src = (struct meltmapstrings_st *) p;
	int siz = melt_primtab[src->lenix];
	struct meltmapstrings_st *dst = (struct meltmapstrings_st *)
	  ggc_alloc_cleared (sizeof (struct meltmapstrings_st));
	dst->discr = src->discr;
	dst->count = src->count;
	dst->lenix = src->lenix;
	if (siz > 0 && src->entab)
	  {
	    dst->entab =
	      (struct entrystringsmelt_st *) ggc_alloc_cleared (siz *
								   sizeof
								   (dst->entab
								    [0]));
	    memcpy (dst->entab, src->entab, siz * sizeof (dst->entab[0]));
	  }
	else
	  dst->entab = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MAPBASICBLOCKS:
      {
	struct meltmapbasicblocks_st *src =
	  (struct meltmapbasicblocks_st *) p;
	int siz = melt_primtab[src->lenix];
	struct meltmapbasicblocks_st *dst =
	  (struct meltmapbasicblocks_st *)
	  ggc_alloc_cleared (sizeof (struct meltmapbasicblocks_st));
	dst->discr = src->discr;
	dst->count = src->count;
	dst->lenix = src->lenix;
	if (siz > 0 && src->entab)
	  {
	    dst->entab =
	      (struct entrybasicblocksmelt_st *) ggc_alloc_cleared (siz *
								       sizeof
								       (dst->
									entab
									[0]));
	    memcpy (dst->entab, src->entab, siz * sizeof (dst->entab[0]));
	  }
	else
	  dst->entab = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    case OBMAG_MAPEDGES:
      {
	struct meltmapedges_st *src = (struct meltmapedges_st *) p;
	int siz = melt_primtab[src->lenix];
	struct meltmapedges_st *dst = (struct meltmapedges_st *)
	  ggc_alloc_cleared (sizeof (struct meltmapedges_st));
	dst->discr = src->discr;
	dst->count = src->count;
	dst->lenix = src->lenix;
	if (siz > 0 && src->entab)
	  {
	    dst->entab =
	      (struct entryedgesmelt_st *) ggc_alloc_cleared (siz *
								 sizeof
								 (dst->entab
								  [0]));
	    memcpy (dst->entab, src->entab, siz * sizeof (dst->entab[0]));
	  }
	else
	  dst->entab = NULL;
	n = (melt_ptr_t) dst;
	break;
      }
    default:
      fatal_error ("corruption: forward invalid p=%p discr=%p magic=%d",
		   (void *) p, (void *) p->u_discr, mag);
    }
  if (n)
    {
      p->u_forward.discr = FORWARDED_DISCR;
      p->u_forward.forward = n;
#ifdef ENABLE_CHECKING
      if (debughack_file)
	{
	  fprintf (debughack_file, "forwarded pushing %p to scan\n",
		   (void *) n);
	}
#endif
      VEC_safe_push (melt_ptr_t, gc, bscanvec, n);
    }
  return n;
  /* end of forwarded_copy */
}



/* the scanning routine is mostly chesney like; however some types,
   including objects, strbuf, stringmaps, objectmaps, all the other
   *maps, contain a pointer to a non value; this pointer should be
   carefully updated if it was young */
static void
scanning (melt_ptr_t p)
{
  unsigned omagic = 0;
  if (!p)
    return;
  gcc_assert (p != (void *) 1);
#if ENABLE_CHECKING
  if (debughack_file)
    {
      fprintf (debughack_file, "scanning %p\n", (void *) p);
    }
#endif
  gcc_assert (p->u_discr && p->u_discr != (meltobject_ptr_t) 1);
  FORWARDED (p->u_discr);
  gcc_assert (!melt_is_young (p));
  omagic = p->u_discr->object_magic;
  switch (omagic)
    {
    case OBMAG_OBJECT:
      {
	int ix;
	struct meltobject_st *src = (meltobject_ptr_t) p;
	for (ix = (int) (src->obj_len) - 1; ix >= 0; ix--)
	  FORWARDED (src->obj_vartab[ix]);
	break;
      }
    case OBMAG_DECAY:
      {
	struct meltdecay_st *src = (struct meltdecay_st *) p;
	FORWARDED (src->val);
	break;
      }
    case OBMAG_BOX:
      {
	struct meltbox_st *src = (struct meltbox_st *) p;
	FORWARDED (src->val);
	break;
      }
    case OBMAG_MULTIPLE:
      {
	struct meltmultiple_st *src = (struct meltmultiple_st *) p;
	unsigned nbval = src->nbval;
	int ix;
	for (ix = (int) nbval - 1; ix >= 0; ix--)
	  FORWARDED (src->tabval[ix]);
	break;
      }
    case OBMAG_CLOSURE:
      {
	struct meltclosure_st *src = (struct meltclosure_st *) p;
	unsigned nbval = src->nbval;
	int ix;
	FORWARDED (src->rout);
	for (ix = (int) nbval - 1; ix >= 0; ix--)
	  FORWARDED (src->tabval[ix]);
	break;
      }
    case OBMAG_ROUTINE:
      {
	struct meltroutine_st *src = (struct meltroutine_st *) p;
	unsigned nbval = src->nbval;
	int ix;
	for (ix = (int) nbval - 1; ix >= 0; ix--)
	  FORWARDED (src->tabval[ix]);
	break;
      }
    case OBMAG_LIST:
      {
	struct meltlist_st *src = (struct meltlist_st *) p;
	FORWARDED (src->first);
	FORWARDED (src->last);
	break;
      }
    case OBMAG_PAIR:
      {
	struct meltpair_st *src = (struct meltpair_st *) p;
	FORWARDED (src->hd);
	FORWARDED (src->tl);
	break;
      }
    case OBMAG_TRIPLE:
      {
	struct melttriple_st *src = (struct melttriple_st *) p;
	FORWARDED (src->hd);
	FORWARDED (src->mi);
	FORWARDED (src->tl);
	break;
      }
    case ALL_OBMAG_SPECIAL_CASES:
      {
	struct meltspecial_st *src = (struct meltspecial_st *) p;
	src->mark = 1;
	break;
      }
    case OBMAG_MAPOBJECTS:
      {
	struct meltmapobjects_st *src = (struct meltmapobjects_st *) p;
	int siz, ix;
	if (!src->entab)
	  break;
	siz = melt_primtab[src->lenix];
	gcc_assert (siz > 0);
	if (melt_is_young (src->entab))
	  {
	    struct entryobjectsmelt_st *newtab =
	      (struct entryobjectsmelt_st *) ggc_alloc_cleared (siz *
								   sizeof
								   (struct
								    entryobjectsmelt_st));
	    memcpy (newtab, src->entab,
		    siz * sizeof (struct entryobjectsmelt_st));
	    src->entab = newtab;
	  }
	for (ix = 0; ix < siz; ix++)
	  {
	    meltobject_ptr_t at = src->entab[ix].e_at;
	    if (!at || at == (void *) 1)
	      {
		src->entab[ix].e_va = NULL;
		continue;
	      }
	    FORWARDED (at);
	    src->entab[ix].e_at = at;
	    FORWARDED (src->entab[ix].e_va);
	  }
	break;
      }
    case OBMAG_MAPTREES:
      {
	struct meltmaptrees_st *src = (struct meltmaptrees_st *) p;
	int ix, siz;
	if (!src->entab)
	  break;
	siz = melt_primtab[src->lenix];
	gcc_assert (siz > 0);
	if (melt_is_young (src->entab))
	  {
	    struct entrytreesmelt_st *newtab =
	      (struct entrytreesmelt_st *) ggc_alloc_cleared (siz *
								 sizeof
								 (struct
								  entrytreesmelt_st));
	    memcpy (newtab, src->entab,
		    siz * sizeof (struct entrytreesmelt_st));
	    src->entab = newtab;
	  }
	for (ix = 0; ix < siz; ix++)
	  {
	    tree at = src->entab[ix].e_at;
	    if (!at || at == (void *) 1)
	      {
		src->entab[ix].e_va = NULL;
		continue;
	      }
	    FORWARDED (src->entab[ix].e_va);
	  }
	break;
      }
    case OBMAG_MAPGIMPLES:
      {
	struct meltmapgimples_st *src = (struct meltmapgimples_st *) p;
	int ix, siz;
	if (!src->entab)
	  break;
	siz = melt_primtab[src->lenix];
	gcc_assert (siz > 0);
	if (melt_is_young (src->entab))
	  {
	    struct entrygimplesmelt_st *newtab =
	      (struct entrygimplesmelt_st *) ggc_alloc_cleared (siz *
								   sizeof
								   (struct
								    entrygimplesmelt_st));
	    memcpy (newtab, src->entab,
		    siz * sizeof (struct entrygimplesmelt_st));
	    src->entab = newtab;
	  }
	for (ix = 0; ix < siz; ix++)
	  {
	    gimple at = src->entab[ix].e_at;
	    if (!at || at == (void *) 1)
	      {
		src->entab[ix].e_va = NULL;
		continue;
	      }
	    FORWARDED (src->entab[ix].e_va);
	  }
	break;
      }
    case OBMAG_MAPGIMPLESEQS:
      {
	struct meltmapgimpleseqs_st *src =
	  (struct meltmapgimpleseqs_st *) p;
	int ix, siz;
	if (!src->entab)
	  break;
	siz = melt_primtab[src->lenix];
	gcc_assert (siz > 0);
	if (melt_is_young (src->entab))
	  {
	    struct entrygimpleseqsmelt_st *newtab =
	      (struct entrygimpleseqsmelt_st *)
	      ggc_alloc_cleared (siz *
				 sizeof (struct entrygimpleseqsmelt_st));
	    memcpy (newtab, src->entab,
		    siz * sizeof (struct entrygimpleseqsmelt_st));
	    src->entab = newtab;
	  }
	for (ix = 0; ix < siz; ix++)
	  {
	    gimple_seq at = src->entab[ix].e_at;
	    if (!at || at == (void *) 1)
	      {
		src->entab[ix].e_va = NULL;
		continue;
	      }
	    FORWARDED (src->entab[ix].e_va);
	  }
	break;
      }
    case OBMAG_MAPSTRINGS:
      {
	struct meltmapstrings_st *src = (struct meltmapstrings_st *) p;
	int ix, siz;
	if (!src->entab)
	  break;
	siz = melt_primtab[src->lenix];
	gcc_assert (siz > 0);
	if (melt_is_young (src->entab))
	  {
	    struct entrystringsmelt_st *newtab
	      = (struct entrystringsmelt_st *)
	      ggc_alloc_cleared (siz *
				 sizeof (struct entrystringsmelt_st));
	    memcpy (newtab, src->entab,
		    siz * sizeof (struct entrystringsmelt_st));
	    src->entab = newtab;
	  }
	for (ix = 0; ix < siz; ix++)
	  {
	    const char *at = src->entab[ix].e_at;
	    if (!at || at == (void *) 1)
	      {
		src->entab[ix].e_va = NULL;
		continue;
	      }
	    if (melt_is_young ((const void *) at))
	      src->entab[ix].e_at = (const char *) ggc_strdup (at);
	    FORWARDED (src->entab[ix].e_va);
	  }
	break;
      }
    case OBMAG_MAPBASICBLOCKS:
      {
	struct meltmapbasicblocks_st *src =
	  (struct meltmapbasicblocks_st *) p;
	int ix, siz;
	if (!src->entab)
	  break;
	siz = melt_primtab[src->lenix];
	gcc_assert (siz > 0);
	if (melt_is_young (src->entab))
	  {
	    struct entrybasicblocksmelt_st *newtab
	      = (struct entrybasicblocksmelt_st *)
	      ggc_alloc_cleared (siz *
				 sizeof (struct entrybasicblocksmelt_st));
	    memcpy (newtab, src->entab,
		    siz * sizeof (struct entrybasicblocksmelt_st));
	    src->entab = newtab;
	  }
	for (ix = 0; ix < siz; ix++)
	  {
	    basic_block at = src->entab[ix].e_at;
	    if (!at || at == (void *) 1)
	      {
		src->entab[ix].e_va = NULL;
		continue;
	      }
	    FORWARDED (src->entab[ix].e_va);
	  }
	break;
      }
    case OBMAG_MAPEDGES:
      {
	struct meltmapedges_st *src = (struct meltmapedges_st *) p;
	int siz, ix;
	if (!src->entab)
	  break;
	siz = melt_primtab[src->lenix];
	gcc_assert (siz > 0);
	if (melt_is_young (src->entab))
	  {
	    struct entryedgesmelt_st *newtab
	      = (struct entryedgesmelt_st *)
	      ggc_alloc_cleared (siz * sizeof (struct entryedgesmelt_st));
	    memcpy (newtab, src->entab,
		    siz * sizeof (struct entryedgesmelt_st));
	    src->entab = newtab;
	  }
	for (ix = 0; ix < siz; ix++)
	  {
	    edge at = src->entab[ix].e_at;
	    if (!at || at == (void *) 1)
	      {
		src->entab[ix].e_va = NULL;
		continue;
	      }
	    FORWARDED (src->entab[ix].e_va);
	  }
	break;
      }
    case OBMAG_MIXINT:
      {
	struct meltmixint_st *src = (struct meltmixint_st *) p;
	FORWARDED (src->ptrval);
	break;
      }
    case OBMAG_MIXLOC:
      {
	struct meltmixloc_st *src = (struct meltmixloc_st *) p;
	FORWARDED (src->ptrval);
	break;
      }
    case OBMAG_MIXBIGINT:
      {
	struct meltmixbigint_st *src = (struct meltmixbigint_st *) p;
	FORWARDED (src->ptrval);
	break;
      }
    case OBMAG_STRBUF:
      {
	struct meltstrbuf_st *src = (struct meltstrbuf_st *) p;
	char *oldbufzn = src->bufzn;
	if (melt_is_young (oldbufzn))
	  {
	    int bsiz = melt_primtab[src->buflenix];
	    if (bsiz > 0)
	      {
		char *newbufzn = (char *) ggc_alloc_cleared (bsiz + 1);
		memcpy (newbufzn, oldbufzn, bsiz);
		src->bufzn = newbufzn;
		memset (oldbufzn, 0, bsiz);
	      }
	    else
	      src->bufzn = NULL;
	  }
	break;
      }
    case OBMAG_INT:
    case OBMAG_REAL:
    case OBMAG_STRING:
    case OBMAG_TREE:
    case OBMAG_GIMPLE:
    case OBMAG_GIMPLESEQ:
    case OBMAG_BASICBLOCK:
    case OBMAG_EDGE:
      break;
    default:
      /* gcc_unreachable (); */
      fatal_error ("melt scanning GC: corrupted heap, p=%p omagic=%d\n",
		   (void *) p, (int) omagic);
    }
}






/** array of about 190 primes gotten by shell command 
    primes 3 2000000000 | awk '($1>p+p/8){print $1, ","; p=$1}'  **/
const long melt_primtab[256] = {
  0,				/* the first entry indexed #0 is 0 to never be used */
  3, 5, 7, 11, 13, 17, 23, 29, 37, 43, 53, 61, 71, 83, 97, 113,
  131, 149, 173, 197, 223, 251, 283, 331, 373, 421, 479, 541,
  613, 691, 787, 887, 1009, 1151, 1297, 1471, 1657, 1867, 2111,
  2377, 2677, 3019, 3407, 3833, 4327, 4871, 5483, 6173, 6947,
  7817, 8803, 9907, 11149, 12547, 14143, 15913, 17903, 20143,
  22669, 25523, 28723, 32321, 36373, 40927, 46049, 51817,
  58309, 65599, 73819, 83047, 93463, 105167, 118343, 133153,
  149803, 168533, 189613, 213319, 239999, 270001, 303767,
  341743, 384469, 432539, 486617, 547453, 615887, 692893,
  779507, 876947, 986567, 1109891, 1248631, 1404721, 1580339,
  1777891, 2000143, 2250163, 2531443, 2847893, 3203909,
  3604417, 4054987, 4561877, 5132117, 5773679, 6495389,
  7307323, 8220743, 9248339, 10404403, 11704963, 13168091,
  14814103, 16665881, 18749123, 21092779, 23729411, 26695609,
  30032573, 33786659, 38010019, 42761287, 48106453, 54119761,
  60884741, 68495347, 77057297, 86689469, 97525661, 109716379,
  123430961, 138859837, 156217333, 175744531, 197712607,
  222426683, 250230023, 281508827, 316697431, 356284619,
  400820209, 450922753, 507288107, 570699121, 642036517,
  722291083, 812577517, 914149741,
#if HOST_BITS_PER_LONG >= 64
  1028418463, 1156970821, 1301592203,
  1464291239, 1647327679, 1853243677, 2084899139, 2345511541,
  2638700497, 2968538081, 3339605383, 3757056091, 4226688133,
  4755024167, 5349402193, 6018077509, 6770337239, 7616629399,
  8568708139, 9639796667, 10844771263, 12200367671,
  13725413633, 15441090347, 17371226651, 19542629983,
  21985458749, 24733641113, 27825346259, 31303514549,
  35216453869, 39618510629, 44570824481, 50142177559,
#endif
  0, 0
};

/* index of entry to get or add an attribute in an mapobject (or -1 on error) */
static inline int
unsafe_index_mapobject (struct entryobjectsmelt_st *tab,
			meltobject_ptr_t attr, int siz)
{
  int da = 0, ix = 0, frix = -1;
  unsigned h = 0;
#if 0 && ENABLE_CHECKING
  static long samehashcnt;
#endif
  if (!tab)
    return -1;
  da = attr->obj_class->object_magic;
  if (da == OBMAG_OBJECT)
    h = ((struct meltobject_st *) attr)->obj_hash;
  else
    return -1;
  h = h % siz;
  for (ix = h; ix < siz; ix++)
    {
      meltobject_ptr_t curat = tab[ix].e_at;
      if (curat == attr)
	return ix;
      else if (curat == (void *) HTAB_DELETED_ENTRY)
	{
	  if (frix < 0)
	    frix = ix;
	}
      else if (!curat)
	{
	  if (frix < 0)
	    frix = ix;
	  return frix;
	}
    }
  for (ix = 0; ix < (int) h; ix++)
    {
      meltobject_ptr_t curat = tab[ix].e_at;
      if (curat == attr)
	return ix;
      else if (curat == (void *) HTAB_DELETED_ENTRY)
	{
	  if (frix < 0)
	    frix = ix;
	}
      else if (!curat)
	{
	  if (frix < 0)
	    frix = ix;
	  return frix;
	}
    }
  if (frix >= 0)
    return frix;		/* found some place in a table with
				   deleted entries but no empty
				   entries */
  return -1;			/* entirely full, should not happen */
}


melt_ptr_t
meltgc_new_int (meltobject_ptr_t discr_p, long num)
{
  MELT_ENTERFRAME (2, NULL);
#define newintv curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define int_newintv ((struct meltint_st*)(newintv))
  discrv = (void *) discr_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_INT)
    goto end;
  newintv = meltgc_allocate (sizeof (struct meltint_st), 0);
  int_newintv->discr = object_discrv;
  int_newintv->val = num;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newintv;
#undef newintv
#undef discrv
#undef int_newintv
#undef object_discrv
}


melt_ptr_t
meltgc_new_mixint (meltobject_ptr_t discr_p,
		      melt_ptr_t val_p, long num)
{
  MELT_ENTERFRAME (3, NULL);
#define newmix  curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define valv    curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mix_newmix ((struct meltmixint_st*)(newmix))
  newmix = NULL;
  discrv = (void *) discr_p;
  valv = val_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MIXINT)
    goto end;
  newmix = meltgc_allocate (sizeof (struct meltmixint_st), 0);
  mix_newmix->discr = object_discrv;
  mix_newmix->intval = num;
  mix_newmix->ptrval = (melt_ptr_t) valv;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmix;
#undef newmix
#undef valv
#undef discrv
#undef mix_newmix
#undef object_discrv
}


melt_ptr_t
meltgc_new_mixloc (meltobject_ptr_t discr_p,
		      melt_ptr_t val_p, long num, location_t loc)
{
  MELT_ENTERFRAME (3, NULL);
#define newmix  curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define valv    curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mix_newmix ((struct meltmixloc_st*)(newmix))
  newmix = NULL;
  discrv = (void *) discr_p;
  valv = val_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MIXLOC)
    goto end;
  newmix = meltgc_allocate (sizeof (struct meltmixloc_st), 0);
  mix_newmix->discr = object_discrv;
  mix_newmix->intval = num;
  mix_newmix->ptrval = (melt_ptr_t) valv;
  mix_newmix->locval = loc;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmix;
#undef newmix
#undef valv
#undef discrv
#undef mix_newmix
#undef object_discrv
}


melt_ptr_t
meltgc_new_mixbigint_mpz (meltobject_ptr_t discr_p,
			     melt_ptr_t val_p, mpz_t mp)
{
  unsigned numb = 0, blen = 0;
  size_t wcnt = 0;
  MELT_ENTERFRAME (3, NULL);
#define newbig  curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define valv    curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mix_newbig ((struct meltmixbigint_st*)(newbig))
  newbig = NULL;
  discrv = (void *) discr_p;
  if (!discrv)
    discrv = (meltobject_ptr_t) MELT_PREDEF (DISCR_MIXBIGINT);
  valv = val_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MIXBIGINT)
    goto end;
  if (!mp) 
    goto end; 
  numb = 8*sizeof(mix_newbig->tabig[0]);
  blen = (mpz_sizeinbase (mp, 2) + numb-1) / numb;
  newbig = meltgc_allocate (sizeof (struct meltmixbigint_st),
			       blen*sizeof(mix_newbig->tabig[0]));
  mix_newbig->discr = object_discrv;
  mix_newbig->ptrval = (melt_ptr_t) valv;
  mix_newbig->negative = (mpz_sgn (mp)<0);
  mix_newbig->biglen = blen;
  mpz_export (mix_newbig->tabig, &wcnt, 
	      /*most significant word first */ 1, 
	      sizeof(mix_newbig->tabig[0]), 
	      /*native endian*/ 0, 
	      /* no nails bits */ 0, 
	      mp);
  gcc_assert(wcnt <= blen);
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newbig;
#undef newbig
#undef valv
#undef discrv
#undef mix_newbig
#undef object_discrv
}


/* allocate a new routine object of given DISCR and of length LEN,
   with a DESCR-iptive string a a PROC-edure */
meltroutine_ptr_t
meltgc_new_routine (meltobject_ptr_t discr_p,
		       unsigned len, const char *descr,
		       meltroutfun_t * proc)
{
  MELT_ENTERFRAME (2, NULL);
#define newroutv curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define obj_discrv ((meltobject_ptr_t)(discrv))
#define rou_newroutv ((meltroutine_ptr_t)(newroutv))
  newroutv = NULL;
  discrv = discr_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT
      || obj_discrv->object_magic != OBMAG_ROUTINE || !descr || !descr[0]
      || !proc || len > MELT_MAXLEN)
    goto end;
  newroutv =
    meltgc_allocate (sizeof (struct meltroutine_st),
			len * sizeof (void *));
  rou_newroutv->discr = (meltobject_ptr_t) discrv;
  rou_newroutv->nbval = len;
  rou_newroutv->routfunad = proc;
  strncpy (rou_newroutv->routdescr, descr, MELT_ROUTDESCR_LEN - 1);
  rou_newroutv->routdescr[MELT_ROUTDESCR_LEN - 1] = (char) 0;
end:
  MELT_EXITFRAME ();
  return (meltroutine_ptr_t) newroutv;
#undef newroutv
#undef discrv
#undef obj_discrv
#undef rou_newroutv
}

void
meltgc_set_routine_data (melt_ptr_t rout_p, melt_ptr_t data_p)
{
  MELT_ENTERFRAME (2, NULL);
#define routv curfram__.varptr[0]
#define datav  curfram__.varptr[1]
  routv = rout_p;
  datav = data_p;
  if (melt_magic_discr ((melt_ptr_t) routv) == OBMAG_ROUTINE)
    {
      ((meltroutine_ptr_t) routv)->routdata = (melt_ptr_t) datav;
      meltgc_touch_dest (routv, datav);
    }
  MELT_EXITFRAME ();
#undef routv
#undef datav
}

meltclosure_ptr_t
meltgc_new_closure (meltobject_ptr_t discr_p,
		       meltroutine_ptr_t rout_p, unsigned len)
{
  MELT_ENTERFRAME (3, NULL);
#define newclosv  curfram__.varptr[0]
#define discrv    curfram__.varptr[1]
#define routv     curfram__.varptr[2]
#define clo_newclosv ((meltclosure_ptr_t)(newclosv))
#define obj_discrv   ((meltobject_ptr_t)(discrv))
#define rou_routv    ((meltroutine_ptr_t)(routv))
  discrv = discr_p;
  routv = rout_p;
  newclosv = NULL;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT
      || obj_discrv->object_magic != OBMAG_CLOSURE
      || melt_magic_discr ((melt_ptr_t) (routv)) != OBMAG_ROUTINE
      || len > MELT_MAXLEN)
    goto end;
  newclosv =
    meltgc_allocate (sizeof (struct meltclosure_st),
			sizeof (void *) * len);
  clo_newclosv->discr = (meltobject_ptr_t) discrv;
  clo_newclosv->rout = (meltroutine_ptr_t) routv;
  clo_newclosv->nbval = len;
end:
  MELT_EXITFRAME ();
  return (meltclosure_ptr_t) newclosv;
#undef newclosv
#undef discrv
#undef routv
#undef clo_newclosv
#undef obj_discrv
#undef rou_routv
}



struct meltstrbuf_st *
meltgc_new_strbuf (meltobject_ptr_t discr_p, const char *str)
{
  int slen = 0, blen = 0, ix = 0;
  MELT_ENTERFRAME (2, NULL);
#define newbufv  curfram__.varptr[0]
#define discrv   curfram__.varptr[1]
#define buf_newbufv ((struct meltstrbuf_st*)(newbufv))
  discrv = discr_p;
  newbufv = NULL;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (((meltobject_ptr_t) (discrv))->object_magic != OBMAG_STRBUF)
    goto end;
  if (str)
    slen = strlen (str);
  gcc_assert (slen < MELT_MAXLEN);
  slen += slen / 5 + 40;
  for (ix = 2; (blen = melt_primtab[ix]) != 0 && blen < slen; ix++);
  gcc_assert (blen != 0);
  newbufv =
    meltgc_allocate (offsetof
			(struct meltstrbuf_st, buf_space), blen + 1);
  buf_newbufv->discr = (meltobject_ptr_t) discrv;
  buf_newbufv->bufzn = buf_newbufv->buf_space;
  buf_newbufv->buflenix = ix;
  buf_newbufv->bufstart = 0;
  if (str)
    {
      strcpy (buf_newbufv->bufzn, str);
      buf_newbufv->bufend = strlen (str);
    }
  else
    buf_newbufv->bufend = 0;
end:
  MELT_EXITFRAME ();
  return (struct meltstrbuf_st *) newbufv;
#undef newbufv
#undef discrv
#undef buf_newbufv
}

/* we need to be able to compute the length of the last line of a
   FILE* filled by MELT output primitives; very often this FILE* will
   be stdout or stderr; and we don't care that much if the computed
   length of the last [i.e. current] line is wrong. So we keep an
   array of positions in FILE*, indexed by their fileno, which we
   suppose is small */
#define MELTMAXFILE 512
static long lasteol[MELTMAXFILE];

void
meltgc_add_out_raw_len (melt_ptr_t outbuf_p, const char *str, int slen)
{
#ifdef ENABLE_CHECKING
  static long addcount;
#endif
  int blen = 0;
  MELT_ENTERFRAME (2, NULL);
#define outbufv  curfram__.varptr[0]
#define buf_outbufv  ((struct meltstrbuf_st*)(outbufv))
#define spec_outbufv  ((struct meltspecial_st*)(outbufv))
  outbufv = outbuf_p;
  if (!str)
    goto end;
  if (slen<0)
    slen = strlen (str);
  if (slen<=0) 
    goto end;
  switch (melt_magic_discr ((melt_ptr_t) (outbufv))) {
  case OBMAG_SPEC_FILE:
  case OBMAG_SPEC_RAWFILE:
    {
      FILE* f = spec_outbufv->val.sp_file;
      if (f) {
	int fno = fileno(f);
	char* eol = 0;
	long fp = ftell(f);
	(void) fwrite(str, (size_t)slen, (size_t)1, f);
	if (fno < MELTMAXFILE && fno >= 0 && (eol = strchr(str, '\n')) && eol-str < slen)
	  lasteol[fno] = fp + (eol-str);
      }
    }
    break;
  case OBMAG_STRBUF:
    gcc_assert (!melt_is_young (str));
    blen = melt_primtab[buf_outbufv->buflenix];
    gcc_assert (blen > 0);
#ifdef ENABLE_CHECKING
    addcount++;
#endif
    gcc_assert (buf_outbufv->bufstart <= buf_outbufv->bufend
		&& buf_outbufv->bufend < (unsigned) blen);
    if ((int) buf_outbufv->bufend + slen + 2 < blen)
      {				/* simple case, just copy at end */
	strcpy (buf_outbufv->bufzn + buf_outbufv->bufend, str);
	buf_outbufv->bufend += slen;
	buf_outbufv->bufzn[buf_outbufv->bufend] = 0;
      }
    else
      if ((int) buf_outbufv->bufstart > (int) 0
	  && (int) buf_outbufv->bufend -
	  (int) buf_outbufv->bufstart + (int) slen + 2 < (int) blen)
	{				/* should move the buffer to fit */
	  int siz = buf_outbufv->bufend - buf_outbufv->bufstart;
	  gcc_assert (siz > 0);
	  memmove (buf_outbufv->bufzn,
		   buf_outbufv->bufzn + buf_outbufv->bufstart, siz);
	  buf_outbufv->bufstart = 0;
	  strcpy (buf_outbufv->bufzn + siz, str);
	  buf_outbufv->bufend = siz + slen;
	  buf_outbufv->bufzn[buf_outbufv->bufend] = 0;
	}
      else
	{				/* should grow the buffer to fit */
	  int siz = buf_outbufv->bufend - buf_outbufv->bufstart;
	  int newsiz = (siz + slen + 50 + siz / 8) | 0x1f;
	  int newix = 0, newblen = 0;
	  char *newb = NULL;
	  int oldblen = melt_primtab[buf_outbufv->buflenix];
	  for (newix = buf_outbufv->buflenix + 1;
	       (newblen = melt_primtab[newix]) != 0
		 && newblen < newsiz; newix++);
	  gcc_assert (newblen >= newsiz);
	  gcc_assert (siz >= 0);
	  if (newblen > MELT_MAXLEN)
	    fatal_error ("strbuf overflow to %d bytes", newblen);
	  /* the newly grown buffer is allocated in young memory if the
	     previous was young, or in old memory if it was already old;
	     but we have to deal with the rare case when the allocation
	     triggers a GC which migrate the strbuf from young to old */
	  if (melt_is_young (buf_outbufv->bufzn))
	    {
	      /* bug to avoid: the strbuf was young, the allocation of
		 newb triggers a GC, and then the strbuf becomes old. we
		 cannot put newb inside it (this violate the GC invariant
		 of no unfollowed -on store list- old to young
		 pointers). So we reserve the required length to make sure
		 that the following newb allocation does not trigger a
		 GC */
	      meltgc_reserve (newblen + 10 * sizeof (void *));
	      /* does the above reservation triggered a GC which moved buf_outbufv to old? */
	      if (!melt_is_young (buf_outbufv->bufzn))
		goto strbuf_in_old_memory;
	      gcc_assert (melt_is_young (buf_outbufv));
	      newb = (char *) melt_allocatereserved (newblen + 1, 0);
	      gcc_assert (melt_is_young (buf_outbufv));
	      memcpy (newb, buf_outbufv->bufzn + buf_outbufv->bufstart, siz);
	      strcpy (newb + siz, str);
	      memset (buf_outbufv->bufzn, 0, oldblen);
	      buf_outbufv->bufzn = newb;
	    }
	  else
	    {
	      /* we may come here if the strbuf was young but became old
		 by the meltgc_reserve call above */
	    strbuf_in_old_memory:
	      gcc_assert (!melt_is_young (buf_outbufv));
	      newb = (char *) ggc_alloc_cleared (newblen + 1);
	      memcpy (newb, buf_outbufv->bufzn + buf_outbufv->bufstart, siz);
	      strcpy (newb + siz, str);
	      memset (buf_outbufv->bufzn, 0, oldblen);
	      ggc_free (buf_outbufv->bufzn);
	      buf_outbufv->bufzn = newb;
	    }
	  buf_outbufv->buflenix = newix;
	  buf_outbufv->bufstart = 0;
	  buf_outbufv->bufend = siz + slen;
	  buf_outbufv->bufzn[buf_outbufv->bufend] = 0;
	  /* touch the buffer so that it will be scanned if not young */
	  meltgc_touch (outbufv);
	}
    break;
  default: 
    goto end;
  }
 end:
  MELT_EXITFRAME ();
#undef outbufv
#undef buf_outbufv
#undef fil_outbufv
}

void
meltgc_add_out_raw (melt_ptr_t out_p, const char *str)
{
  meltgc_add_out_raw_len(out_p, str, -1);
}

void
meltgc_add_out (melt_ptr_t out_p, const char *str)
{
  char sbuf[80];
  char *cstr = NULL;
  int slen = 0;
  if (str)
    slen = strlen (str);
  if (slen <= 0)
    return;
  if (slen < (int) sizeof (sbuf) - 1)
    {
      memset (sbuf, 0, sizeof (sbuf));
      strcpy (sbuf, str);
      meltgc_add_out_raw (out_p, sbuf);
    }
  else
    {
      cstr = xstrdup (str);
      meltgc_add_out_raw (out_p, cstr);
      free (cstr);
    }
}

void
meltgc_add_out_cstr (melt_ptr_t outbuf_p, const char *str)
{
  int slen = str ? strlen (str) : 0;
  const char *ps = NULL;
  char *pd = NULL;
  char *cstr = NULL;
  if (!str || !str[0])
    return;
  cstr = (char *) xcalloc (slen + 5, 4);
  pd = cstr;
  for (ps = str; *ps; ps++)
    {
      switch (*ps)
	{
#define ADDS(S) strcpy(pd, S); pd+=sizeof(S)-1; break
	case '\n':
	  ADDS ("\\n");
	case '\r':
	  ADDS ("\\r");
	case '\t':
	  ADDS ("\\t");
	case '\v':
	  ADDS ("\\v");
	case '\f':
	  ADDS ("\\f");
	case '\'':
	  ADDS ("\\\'");
	case '\"':
	  ADDS ("\\\"");
	case '\\':
	  ADDS ("\\\\");
#undef ADDS
	default:
	  if (ISPRINT (*ps))
	    *(pd++) = *ps;
	  else
	    {
	      sprintf (pd, "\\%03o", (*ps) & 0xff);
	      pd += 4;
	    }
	}
    };
  meltgc_add_out_raw (outbuf_p, cstr);
  free (cstr);
}


void
meltgc_add_out_ccomment (melt_ptr_t outbuf_p, const char *str)
{
  int slen = str ? strlen (str) : 0;
  const char *ps = NULL;
  char *pd = NULL;
  char *cstr = NULL;
  if (!str || !str[0])
    return;
  cstr = (char *) xcalloc (slen + 4, 4);
  pd = cstr;
  for (ps = str; *ps; ps++)
    {
      if (ps[0] == '/' && ps[1] == '*')
	{
	  pd[0] = '/';
	  pd[1] = '+';
	  pd += 2;
	  ps++;
	}
      else if (ps[0] == '*' && ps[1] == '/')
	{
	  pd[0] = '+';
	  pd[1] = '/';
	  pd += 2;
	  ps++;
	}
      else
	*(pd++) = *ps;
    };
  meltgc_add_out_raw (outbuf_p, cstr);
  free (cstr);
}

void
meltgc_add_out_cident (melt_ptr_t outbuf_p, const char *str)
{
  int slen = str ? strlen (str) : 0;
  char *dupstr = 0;
  const char *ps = 0;
  char *pd = 0;
  char tinybuf[80];
  if (!str || !str[0])
    return;
  if (slen < (int) sizeof (tinybuf) - 2)
    {
      memset (tinybuf, 0, sizeof (tinybuf));
      dupstr = tinybuf;
    }
  else
    dupstr = (char *) xcalloc (slen + 2, 1);
  if (str)
    for (ps = (const char *) str, pd = dupstr; *ps; ps++)
      {
	if (ISALNUM (*ps))
	  *(pd++) = *ps;
	else if (pd > dupstr && pd[-1] != '_')
	  *(pd++) = '_';
	else
	  *pd = (char) 0;
	pd[1] = (char) 0;
      }
  meltgc_add_out_raw (outbuf_p, dupstr);
  if (dupstr && dupstr != tinybuf)
    free (dupstr);
}

void
meltgc_add_out_cidentprefix (melt_ptr_t outbuf_p, const char *str, int preflen)
{
  const char *ps = 0;
  char *pd = 0;
  char tinybuf[80];
  if (str)
    {
      int lenst = strlen (str);
      if (lenst < preflen)
	preflen = lenst;
    }
  else
    return;
  /* we don't care to trim the C identifier in generated stuff */
  if (preflen >= (int) sizeof (tinybuf) - 1)
    preflen = sizeof (tinybuf) - 2;
  if (preflen <= 0)
    return;
  memset (tinybuf, 0, sizeof (tinybuf));
  for (pd = tinybuf, ps = str; ps < str + preflen && *ps; ps++)
    {
      if (ISALNUM (*ps))
	*(pd++) = *ps;
      else if (pd > tinybuf && pd[-1] != '_')
	*(pd++) = '_';
    }
  meltgc_add_out_raw (outbuf_p, tinybuf);
}


void
meltgc_add_out_hex (melt_ptr_t outbuf_p, unsigned long l)
{
  if (l == 0UL)
    meltgc_add_out_raw (outbuf_p, "0");
  else
    {
      int ix = 0, j = 0;
      char revbuf[80], thebuf[80];
      memset (revbuf, 0, sizeof (revbuf));
      memset (thebuf, 0, sizeof (thebuf));
      while (ix < (int) sizeof (revbuf) - 1 && l != 0UL)
	{
	  unsigned h = l & 15;
	  l >>= 4;
	  revbuf[ix++] = "0123456789abcdef"[h];
	}
      ix--;
      for (j = 0; j < (int) sizeof (thebuf) - 1 && ix >= 0; j++, ix--)
	thebuf[j] = revbuf[ix];
      meltgc_add_out_raw (outbuf_p, thebuf);
    }
}


void
meltgc_add_out_dec (melt_ptr_t outbuf_p, long l)
{
  if (l == 0L)
    meltgc_add_out_raw (outbuf_p, "0");
  else
    {
      int ix = 0, j = 0, neg = 0;
      char revbuf[96], thebuf[96];
      memset (revbuf, 0, sizeof (revbuf));
      memset (thebuf, 0, sizeof (thebuf));
      if (l < 0)
	{
	  l = -l;
	  neg = 1;
	};
      while (ix < (int) sizeof (revbuf) - 1 && l != 0UL)
	{
	  unsigned h = l % 10;
	  l = l / 10;
	  revbuf[ix++] = "0123456789"[h];
	}
      ix--;
      if (neg)
	{
	  thebuf[0] = '-';
	  j = 1;
	};
      for (; j < (int) sizeof (thebuf) - 1 && ix >= 0; j++, ix--)
	thebuf[j] = revbuf[ix];
      meltgc_add_out_raw (outbuf_p, thebuf);
    }
}


void
meltgc_out_printf (melt_ptr_t outbuf_p,
			 const char *fmt, ...)
{
  char *cstr = NULL;
  va_list ap;
  int l = 0;
  static char tinybuf[80];
  memset (tinybuf, 0, sizeof (tinybuf));
  va_start (ap, fmt);
  l = vsnprintf (tinybuf, sizeof (tinybuf) - 1, fmt, ap);
  va_end (ap);
  if (l < (int) sizeof (tinybuf) - 3)
    {
      meltgc_add_strbuf_raw (outbuf_p, tinybuf);
      return;
    }
  va_start (ap, fmt);
  vasprintf (&cstr, fmt, ap);
  va_end (ap);
  meltgc_add_out_raw (outbuf_p, cstr);
  free (cstr);
}


/* add safely into OUTBUF either a space or an indented newline if the current line is bigger than the threshold */
void
meltgc_out_add_indent (melt_ptr_t outbuf_p, int depth, int linethresh)
{
  int llln = 0;			/* last line length */
  int outmagic = 0;		/* the magic of outbuf */
  MELT_ENTERFRAME (2, NULL);
  /* we need a frame, because we have more than one call to
     meltgc_add_outbuf_raw */
#define outbv   curfram__.varptr[0]
#define outbufv ((struct meltstrbuf_st*)(outbv))
  outbv = outbuf_p;
  if (!outbv)
    goto end;
  outmagic = melt_magic_discr((melt_ptr_t) outbv);
  if (linethresh > 0 && linethresh < 40)
    linethresh = 40;
  /* compute the last line length llln */
  if (outmagic == OBMAG_STRBUF) 
    {
      char *bs = 0, *be = 0, *nl = 0;
      bs = outbufv->bufzn + outbufv->bufstart;
      be = outbufv->bufzn + outbufv->bufend;
      for (nl = be - 1; nl > bs && *nl && *nl != '\n'; nl--);
      llln = be - nl;
      gcc_assert (llln >= 0);
  }
  else if (outmagic == OBMAG_SPEC_FILE || outmagic == OBMAG_SPEC_RAWFILE) 
    {
      FILE *f = spec_outbufv->val.sp_file;
      int fn = f?fileno(f):-1;
      if (f && fn>=0 && fn<=MELTMAXFILE)
	llln = ftell(f) - lasteol[fn];
    }
  if (linethresh > 0 && llln < linethresh)
    meltgc_add_out_raw ((melt_ptr_t) outbv, " ");
  else
    {
      int nbsp = depth;
      static const char spaces32[] = "                                ";
      meltgc_add_out_raw ((melt_ptr_t) outbv, "\n");
      if (nbsp < 0)
	nbsp = 0;
      if (nbsp > 0 && nbsp % 32 != 0)
	meltgc_add_out_raw ((melt_ptr_t) outbv, spaces32 + (32 - nbsp % 32));
    }
end:
  MELT_EXITFRAME ();
#undef outbufv
#undef outbv
}




/***************/

#if ENABLE_CHECKING

/* just for debugging */
unsigned long melt_serial_1, melt_serial_2, melt_serial_3;
unsigned long melt_countserial;

void
melt_object_set_serial (meltobject_ptr_t ob)
{
  if (ob && ob->obj_serial == 0)
    ob->obj_serial = ++melt_countserial;
  else
    return;
  if (melt_serial_1 > 0 && melt_countserial == melt_serial_1)
    {
      debugeprintf ("set serial_1 ob=%p ##%ld", (void *) ob,
		    melt_countserial);
      gcc_assert (ob);
    }
  else if (melt_serial_2 > 0 && melt_countserial == melt_serial_1)
    {
      debugeprintf ("set serial_2 ob=%p ##%ld", (void *) ob,
		    melt_countserial);
      gcc_assert (ob);
    }
  else if (melt_serial_3 > 0 && melt_countserial == melt_serial_3)
    {
      debugeprintf ("set serial_3 ob=%p ##%ld", (void *) ob,
		    melt_countserial);
      gcc_assert (ob);
    }
}
#endif

meltobject_ptr_t
meltgc_new_raw_object (meltobject_ptr_t klass_p, unsigned len)
{
  unsigned h = 0;
  MELT_ENTERFRAME (2, NULL);
#define newobjv   curfram__.varptr[0]
#define klassv    curfram__.varptr[1]
#define obj_newobjv  ((meltobject_ptr_t)(newobjv))
#define obj_klassv   ((meltobject_ptr_t)(klassv))
  newobjv = NULL;
  klassv = klass_p;
  if (melt_magic_discr ((melt_ptr_t) (klassv)) != OBMAG_OBJECT
      || obj_klassv->object_magic != OBMAG_OBJECT || len >= SHRT_MAX)
    goto end;
  /* the sizeof below could be the offsetof obj__tabfields */
  newobjv =
    meltgc_allocate (sizeof (struct meltobject_st),
			len * sizeof (void *));
  obj_newobjv->obj_class = (meltobject_ptr_t) klassv;
  do
    {
      h = melt_lrand () & MELT_MAXHASH;
    }
  while (h == 0);
  obj_newobjv->obj_hash = h;
  obj_newobjv->obj_len = len;
  melt_object_set_serial (obj_newobjv);
end:
  MELT_EXITFRAME ();
  return (meltobject_ptr_t) newobjv;
#undef newobjv
#undef klassv
#undef obj_newobjv
#undef obj_klassv
}


/* allocate a new multiple of given DISCR & length LEN */
melt_ptr_t
meltgc_new_multiple (meltobject_ptr_t discr_p, unsigned len)
{
  MELT_ENTERFRAME (2, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  newmul = NULL;
  gcc_assert (len < MELT_MAXLEN);
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * len);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = len;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discr
#undef mult_newmul
#undef object_discrv
}

/* make a subsequence of a given multiple OLDMUL_P from STARTIX to
   ENDIX; if either index is negative, take it from last.  return null
   if arguments are incorrect, or a fresh subsequence of same
   discriminant as source otherwise */
melt_ptr_t
meltgc_new_subseq_multiple (melt_ptr_t oldmul_p, int startix, int endix)
{
  int oldlen=0, newlen=0, i=0;
  MELT_ENTERFRAME(3, NULL);
#define oldmulv   curfram__.varptr[0]
#define newmulv   curfram__.varptr[1]
#define mult_oldmulv   ((struct meltmultiple_st*)(oldmulv))
#define mult_newmulv   ((struct meltmultiple_st*)(newmulv))
  oldmulv = oldmul_p;
  newmulv = NULL;
  if (melt_magic_discr ((melt_ptr_t) (oldmulv)) != OBMAG_MULTIPLE)
    goto end;
  oldlen = mult_oldmulv->nbval;
  if (startix < 0)
    startix += oldlen;
  if (endix < 0)
    endix += oldlen;
  if (startix < 0 || startix >= oldlen)
    goto end;
  if (endix < 0 || endix >= oldlen || endix < startix)
    goto end;
  newlen = endix - startix;
  newmulv =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * newlen);
  mult_newmulv->discr = mult_oldmulv->discr;
  mult_newmulv->nbval = newlen;
  for (i=0; i<newlen; i++)
    mult_newmulv->tabval[i] = mult_oldmulv->tabval[startix+i];
 end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmulv;
#undef oldmulv
#undef newmulv
#undef mult_oldmulv
#undef mult_newmulv
}

void
meltgc_multiple_put_nth (melt_ptr_t mul_p, int n, melt_ptr_t val_p)
{
  int ln = 0;
  MELT_ENTERFRAME (3, NULL);
#define mulv    curfram__.varptr[0]
#define mult_mulv ((struct meltmultiple_st*)(mulv))
#define discrv  curfram__.varptr[1]
#define valv    curfram__.varptr[2]
  mulv = mul_p;
  valv = val_p;
  if (melt_magic_discr ((melt_ptr_t) (mulv)) != OBMAG_MULTIPLE)
    goto end;
  ln = mult_mulv->nbval;
  if (n < 0)
    n += ln;
  if (n >= 0 && n < ln)
    {
      mult_mulv->tabval[n] = (melt_ptr_t) valv;
      meltgc_touch_dest (mulv, valv);
    }
end:
  MELT_EXITFRAME ();
#undef mulv
#undef mult_mulv
#undef discrv
#undef valv
}



/*** sort a multiple with a compare closure which should return a
     number; if it does not, the sort return nil, by longjmp-ing out
     of qsort
 ***/
static jmp_buf mulsort_escapjmp;
static melt_ptr_t *mulsort_mult_ad;
static melt_ptr_t *mulsort_clos_ad;
static int
mulsort_cmp (const void *p1, const void *p2)
{
  int ok = 0;
  int cmp = 0;
  int ix1 = -1, ix2 = -1;
  union meltparam_un argtab[2];
  MELT_ENTERFRAME (5, NULL);
#define rescmpv curfram__.varptr[0]
#define val1v curfram__.varptr[1]
#define val2v curfram__.varptr[2]
#define clov  curfram__.varptr[3]
#define mulv  curfram__.varptr[4]
  mulv = *mulsort_mult_ad;
  clov = *mulsort_clos_ad;
  ix1 = *(const int *) p1;
  ix2 = *(const int *) p2;
  val1v = melt_multiple_nth ((melt_ptr_t) mulv, ix1);
  val2v = melt_multiple_nth ((melt_ptr_t) mulv, ix2);
  if (val1v == val2v)
    {
      ok = 1;
      cmp = 0;
      goto end;
    }
  memset (argtab, 0, sizeof (argtab));
  argtab[0].bp_aptr = (melt_ptr_t *) & val2v;
  rescmpv =
    melt_apply ((meltclosure_ptr_t) clov, (melt_ptr_t) val1v,
		   BPARSTR_PTR, argtab, "", NULL);
  if (melt_magic_discr ((melt_ptr_t) rescmpv) == OBMAG_INT)
    {
      ok = 1;
      cmp = melt_get_int ((melt_ptr_t) rescmpv);
    }
end:
  MELT_EXITFRAME ();
#undef rescmpv
#undef val1v
#undef val2v
#undef clov
  if (!ok)
    {
      longjmp (mulsort_escapjmp, 1);
    }
  return cmp;
}

melt_ptr_t
meltgc_sort_multiple (melt_ptr_t mult_p, melt_ptr_t clo_p,
			 melt_ptr_t discrm_p)
{
  int *ixtab = 0;
  int i = 0;
  unsigned mulen = 0;
  MELT_ENTERFRAME (5, NULL);
#define multv      curfram__.varptr[0]
#define clov       curfram__.varptr[1]
#define discrmv    curfram__.varptr[2]
#define resv       curfram__.varptr[3]
  multv = mult_p;
  clov = clo_p;
  discrmv = discrm_p;
  resv = NULL;
  if (melt_magic_discr ((melt_ptr_t) multv) != OBMAG_MULTIPLE)
    goto end;
  if (melt_magic_discr ((melt_ptr_t) clov) != OBMAG_CLOSURE)
    goto end;
  if (!discrmv)
    discrmv = (meltobject_ptr_t) MELT_PREDEF (DISCR_MULTIPLE);
  if (melt_magic_discr ((melt_ptr_t) discrmv) != OBMAG_OBJECT)
    goto end;
  if (((meltobject_ptr_t) discrmv)->obj_num != OBMAG_MULTIPLE)
    goto end;
  mulen = (int) (((meltmultiple_ptr_t) multv)->nbval);
  /* allocate and fill ixtab with indexes */
  ixtab = (int *) xcalloc (mulen + 1, sizeof (ixtab[0]));
  for (i = 0; i < (int) mulen; i++)
    ixtab[i] = i;
  mulsort_mult_ad = (melt_ptr_t *) & multv;
  mulsort_clos_ad = (melt_ptr_t *) & clov;
  if (!setjmp (mulsort_escapjmp))
    {
      qsort (ixtab, (size_t) mulen, sizeof (ixtab[0]), mulsort_cmp);
      resv =
	meltgc_new_multiple ((meltobject_ptr_t) discrmv,
				(unsigned) mulen);
      for (i = 0; i < (int) mulen; i++)
	meltgc_multiple_put_nth ((melt_ptr_t) resv, i,
				    melt_multiple_nth ((melt_ptr_t)
							  multv, ixtab[i]));
    }
  else
    {
      resv = NULL;
    }
end:
  if (ixtab)
    free (ixtab);
  memset (&mulsort_escapjmp, 0, sizeof (mulsort_escapjmp));
  mulsort_mult_ad = 0;
  mulsort_clos_ad = 0;
  MELT_EXITFRAME ();
  return (melt_ptr_t) resv;
#undef multv
#undef clov
#undef discrmv
#undef resv
}



/* allocate a new box of given DISCR & content VAL */
melt_ptr_t
meltgc_new_box (meltobject_ptr_t discr_p, melt_ptr_t val_p)
{
  MELT_ENTERFRAME (3, NULL);
#define boxv curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define valv   curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
  discrv = (void *) discr_p;
  valv = (void *) val_p;
  boxv = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_BOX)
    goto end;
  boxv = meltgc_allocate (sizeof (struct meltbox_st), 0);
  ((struct meltbox_st *) (boxv))->discr = (meltobject_ptr_t) discrv;
  ((struct meltbox_st *) (boxv))->val = (melt_ptr_t) valv;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) boxv;
#undef boxv
#undef discrv
#undef valv
#undef object_discrv
}

/* put inside a box */
void
meltgc_box_put (melt_ptr_t box_p, melt_ptr_t val_p)
{
  MELT_ENTERFRAME (2, NULL);
#define boxv curfram__.varptr[0]
#define valv   curfram__.varptr[1]
  boxv = box_p;
  valv = val_p;
  if (melt_magic_discr ((melt_ptr_t) boxv) != OBMAG_BOX)
    goto end;
  ((meltbox_ptr_t) boxv)->val = (melt_ptr_t) valv;
  meltgc_touch_dest (boxv, valv);
end:
  MELT_EXITFRAME ();
#undef boxv
#undef valv
}

/* safely return the content of a container - instance of CLASS_CONTAINER */
melt_ptr_t
melt_container_value (melt_ptr_t cont)
{
  if (melt_magic_discr (cont) != OBMAG_OBJECT
      || ((meltobject_ptr_t) cont)->obj_len < FCONTAINER__LAST)
    return 0;
  if (!melt_is_instance_of
      ((melt_ptr_t) cont, (melt_ptr_t) MELT_PREDEF (CLASS_CONTAINER)))
    return 0;
  return ((meltobject_ptr_t) cont)->obj_vartab[FCONTAINER_VALUE];
}

/* allocate a new boxed tree of given DISCR [or DISCR_TREE] & content
   VAL */
melt_ptr_t
meltgc_new_tree (meltobject_ptr_t discr_p, tree tr)
{
  MELT_ENTERFRAME (2, NULL);
#define btreev  curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
  discrv = (void *) discr_p;
  if (!discrv)
    discrv = MELT_PREDEF (DISCR_TREE);
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_TREE)
    goto end;
  btreev = meltgc_allocate (sizeof (struct melttree_st), 0);
  ((struct melttree_st *) (btreev))->discr = (meltobject_ptr_t) discrv;
  ((struct melttree_st *) (btreev))->val = tr;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) btreev;
#undef btreev
#undef discrv
#undef object_discrv
}



/* allocate a new boxed gimple of given DISCR [or DISCR_GIMPLE] & content
   VAL */
melt_ptr_t
meltgc_new_gimple (meltobject_ptr_t discr_p, gimple g)
{
  MELT_ENTERFRAME (2, NULL);
#define bgimplev  curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
  discrv = (void *) discr_p;
  if (!discrv)
    discrv = MELT_PREDEF (DISCR_GIMPLE);
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_GIMPLE)
    goto end;
  bgimplev = meltgc_allocate (sizeof (struct meltgimple_st), 0);
  ((struct meltgimple_st *) (bgimplev))->discr =
    (meltobject_ptr_t) discrv;
  ((struct meltgimple_st *) (bgimplev))->val = g;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) bgimplev;
#undef bgimplev
#undef discrv
#undef object_discrv
}


/* allocate a new boxed gimpleseq of given DISCR [or DISCR_GIMPLE] & content
   VAL */
melt_ptr_t
meltgc_new_gimpleseq (meltobject_ptr_t discr_p, gimple_seq g)
{
  MELT_ENTERFRAME (2, NULL);
#define bgimplev  curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
  discrv = (void *) discr_p;
  if (!discrv)
    discrv = MELT_PREDEF (DISCR_GIMPLESEQ);
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_GIMPLESEQ)
    goto end;
  bgimplev = meltgc_allocate (sizeof (struct meltgimpleseq_st), 0);
  ((struct meltgimpleseq_st *) (bgimplev))->discr =
    (meltobject_ptr_t) discrv;
  ((struct meltgimpleseq_st *) (bgimplev))->val = g;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) bgimplev;
#undef bgimplev
#undef discrv
#undef object_discrv
}


/* allocate a new boxed basic_block of given DISCR [or DISCR_BASICBLOCK] & content
   VAL */
melt_ptr_t
meltgc_new_basicblock (meltobject_ptr_t discr_p, basic_block bb)
{
  MELT_ENTERFRAME (2, NULL);
#define bbbv    curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
  discrv = (void *) discr_p;
  if (!discrv)
    discrv = MELT_PREDEF (DISCR_BASICBLOCK);
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_BASICBLOCK)
    goto end;
  bbbv = meltgc_allocate (sizeof (struct meltbasicblock_st), 0);
  ((struct meltbasicblock_st *) (bbbv))->discr =
    (meltobject_ptr_t) discrv;
  ((struct meltbasicblock_st *) (bbbv))->val = bb;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) bbbv;
#undef bbbv
#undef discrv
#undef object_discrv
}




/****** MULTIPLES ******/

/* allocate a multiple of arity 1 */
melt_ptr_t
meltgc_new_mult1 (meltobject_ptr_t discr_p, melt_ptr_t v0_p)
{
  MELT_ENTERFRAME (3, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define v0   curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  v0 = v0_p;
  newmul = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * 1);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = 1;
  mult_newmul->tabval[0] = (melt_ptr_t) v0;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discr
#undef v0
#undef mult_newmul
#undef object_discrv
}

melt_ptr_t
meltgc_new_mult2 (meltobject_ptr_t discr_p,
		     melt_ptr_t v0_p, melt_ptr_t v1_p)
{
  MELT_ENTERFRAME (4, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define v0   curfram__.varptr[2]
#define v1   curfram__.varptr[3]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  v0 = v0_p;
  v1 = v1_p;
  newmul = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * 2);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = 2;
  mult_newmul->tabval[0] = (melt_ptr_t) v0;
  mult_newmul->tabval[1] = (melt_ptr_t) v1;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discr
#undef v0
#undef v1
#undef mult_newmul
#undef object_discrv
}

melt_ptr_t
meltgc_new_mult3 (meltobject_ptr_t discr_p,
		     melt_ptr_t v0_p, melt_ptr_t v1_p,
		     melt_ptr_t v2_p)
{
  MELT_ENTERFRAME (5, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define v0   curfram__.varptr[2]
#define v1   curfram__.varptr[3]
#define v2   curfram__.varptr[4]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  v0 = v0_p;
  v1 = v1_p;
  v2 = v2_p;
  newmul = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * 3);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = 3;
  mult_newmul->tabval[0] = (melt_ptr_t) v0;
  mult_newmul->tabval[1] = (melt_ptr_t) v1;
  mult_newmul->tabval[2] = (melt_ptr_t) v2;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discrv
#undef v0
#undef v1
#undef v2
#undef mult_newmul
#undef object_discrv
}

melt_ptr_t
meltgc_new_mult4 (meltobject_ptr_t discr_p,
		     melt_ptr_t v0_p, melt_ptr_t v1_p,
		     melt_ptr_t v2_p, melt_ptr_t v3_p)
{
  MELT_ENTERFRAME (6, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define v0   curfram__.varptr[2]
#define v1   curfram__.varptr[3]
#define v2   curfram__.varptr[4]
#define v3   curfram__.varptr[5]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  v0 = v0_p;
  v1 = v1_p;
  v2 = v2_p;
  v3 = v3_p;
  newmul = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * 4);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = 4;
  mult_newmul->tabval[0] = (melt_ptr_t) v0;
  mult_newmul->tabval[1] = (melt_ptr_t) v1;
  mult_newmul->tabval[2] = (melt_ptr_t) v2;
  mult_newmul->tabval[3] = (melt_ptr_t) v3;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discrv
#undef v0
#undef v1
#undef v2
#undef v3
#undef mult_newmul
#undef object_discrv
}


melt_ptr_t
meltgc_new_mult5 (meltobject_ptr_t discr_p,
		     melt_ptr_t v0_p, melt_ptr_t v1_p,
		     melt_ptr_t v2_p, melt_ptr_t v3_p,
		     melt_ptr_t v4_p)
{
  MELT_ENTERFRAME (7, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define v0   curfram__.varptr[2]
#define v1   curfram__.varptr[3]
#define v2   curfram__.varptr[4]
#define v3   curfram__.varptr[5]
#define v4   curfram__.varptr[6]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  v0 = v0_p;
  v1 = v1_p;
  v2 = v2_p;
  v3 = v3_p;
  v4 = v4_p;
  newmul = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * 5);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = 5;
  mult_newmul->tabval[0] = (melt_ptr_t) v0;
  mult_newmul->tabval[1] = (melt_ptr_t) v1;
  mult_newmul->tabval[2] = (melt_ptr_t) v2;
  mult_newmul->tabval[3] = (melt_ptr_t) v3;
  mult_newmul->tabval[4] = (melt_ptr_t) v4;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discrv
#undef v0
#undef v1
#undef v2
#undef v3
#undef v4
#undef mult_newmul
#undef object_discrv
}


melt_ptr_t
meltgc_new_mult6 (meltobject_ptr_t discr_p,
		     melt_ptr_t v0_p, melt_ptr_t v1_p,
		     melt_ptr_t v2_p, melt_ptr_t v3_p,
		     melt_ptr_t v4_p, melt_ptr_t v5_p)
{
  MELT_ENTERFRAME (8, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define v0   curfram__.varptr[2]
#define v1   curfram__.varptr[3]
#define v2   curfram__.varptr[4]
#define v3   curfram__.varptr[5]
#define v4   curfram__.varptr[6]
#define v5   curfram__.varptr[7]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  v0 = v0_p;
  v1 = v1_p;
  v2 = v2_p;
  v3 = v3_p;
  v4 = v4_p;
  v5 = v5_p;
  newmul = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * 6);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = 6;
  mult_newmul->tabval[0] = (melt_ptr_t) v0;
  mult_newmul->tabval[1] = (melt_ptr_t) v1;
  mult_newmul->tabval[2] = (melt_ptr_t) v2;
  mult_newmul->tabval[3] = (melt_ptr_t) v3;
  mult_newmul->tabval[4] = (melt_ptr_t) v4;
  mult_newmul->tabval[5] = (melt_ptr_t) v5;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discrv
#undef v0
#undef v1
#undef v2
#undef v3
#undef v4
#undef v5
#undef mult_newmul
#undef object_discrv
}

melt_ptr_t
meltgc_new_mult7 (meltobject_ptr_t discr_p,
		     melt_ptr_t v0_p, melt_ptr_t v1_p,
		     melt_ptr_t v2_p, melt_ptr_t v3_p,
		     melt_ptr_t v4_p, melt_ptr_t v5_p,
		     melt_ptr_t v6_p)
{
  MELT_ENTERFRAME (9, NULL);
#define newmul curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define v0   curfram__.varptr[2]
#define v1   curfram__.varptr[3]
#define v2   curfram__.varptr[4]
#define v3   curfram__.varptr[5]
#define v4   curfram__.varptr[6]
#define v5   curfram__.varptr[7]
#define v6   curfram__.varptr[8]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mult_newmul ((struct meltmultiple_st*)(newmul))
  discrv = (void *) discr_p;
  v0 = v0_p;
  v1 = v1_p;
  v2 = v2_p;
  v3 = v3_p;
  v4 = v4_p;
  v5 = v5_p;
  v6 = v6_p;
  newmul = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MULTIPLE)
    goto end;
  newmul =
    meltgc_allocate (sizeof (struct meltmultiple_st),
			sizeof (void *) * 7);
  mult_newmul->discr = object_discrv;
  mult_newmul->nbval = 7;
  mult_newmul->tabval[0] = (melt_ptr_t) v0;
  mult_newmul->tabval[1] = (melt_ptr_t) v1;
  mult_newmul->tabval[2] = (melt_ptr_t) v2;
  mult_newmul->tabval[3] = (melt_ptr_t) v3;
  mult_newmul->tabval[4] = (melt_ptr_t) v4;
  mult_newmul->tabval[5] = (melt_ptr_t) v5;
  mult_newmul->tabval[6] = (melt_ptr_t) v6;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmul;
#undef newmul
#undef discrv
#undef v0
#undef v1
#undef v2
#undef v3
#undef v4
#undef v5
#undef v6
#undef mult_newmul
#undef object_discrv
}


melt_ptr_t
meltgc_new_list (meltobject_ptr_t discr_p)
{
  MELT_ENTERFRAME (2, NULL);
#define discrv curfram__.varptr[0]
#define newlist curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define list_newlist ((struct meltlist_st*)(newlist))
  discrv = (void *) discr_p;
  newlist = NULL;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_LIST)
    goto end;
  newlist = meltgc_allocate (sizeof (struct meltlist_st), 0);
  list_newlist->discr = object_discrv;
  list_newlist->first = NULL;
  list_newlist->last = NULL;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newlist;
#undef newlist
#undef discrv
#undef list_newlist
#undef object_discrv
}

/* allocate a pair of given head and tail */
melt_ptr_t
meltgc_new_pair (meltobject_ptr_t discr_p, void *head_p, void *tail_p)
{
  MELT_ENTERFRAME (4, NULL);
#define pairv   curfram__.varptr[0]
#define discrv  curfram__.varptr[1]
#define headv   curfram__.varptr[2]
#define tailv   curfram__.varptr[3]
  discrv = discr_p;
  headv = head_p;
  tailv = tail_p;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT
      || ((meltobject_ptr_t) (discrv))->object_magic != OBMAG_PAIR)
    goto end;
  if (melt_magic_discr ((melt_ptr_t) tailv) != OBMAG_PAIR)
    tailv = NULL;
  pairv = meltgc_allocate (sizeof (struct meltpair_st), 0);
  ((struct meltpair_st *) (pairv))->discr = (meltobject_ptr_t) discrv;
  ((struct meltpair_st *) (pairv))->hd = (melt_ptr_t) headv;
  ((struct meltpair_st *) (pairv))->tl = (struct meltpair_st *) tailv;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) pairv;
#undef pairv
#undef headv
#undef tailv
#undef discrv
}

/* change the head of a pair */
void
meltgc_pair_set_head (melt_ptr_t pair_p, void *head_p)
{
  MELT_ENTERFRAME (2, NULL);
#define pairv   curfram__.varptr[0]
#define headv   curfram__.varptr[1]
  pairv = pair_p;
  headv = head_p;
  if (melt_magic_discr ((melt_ptr_t) pairv) != OBMAG_PAIR)
    goto end;
  ((struct meltpair_st *) pairv)->hd = (melt_ptr_t) headv;
  meltgc_touch_dest (pairv, headv);
end:
  MELT_EXITFRAME ();
#undef pairv
#undef headv
}


void
meltgc_append_list (melt_ptr_t list_p, melt_ptr_t valu_p)
{
  MELT_ENTERFRAME (4, NULL);
#define list curfram__.varptr[0]
#define valu curfram__.varptr[1]
#define pairv curfram__.varptr[2]
#define lastv curfram__.varptr[3]
#define pai_pairv ((struct meltpair_st*)(pairv))
#define list_list ((struct meltlist_st*)(list))
  list = list_p;
  valu = valu_p;
  if (melt_magic_discr ((melt_ptr_t) list) != OBMAG_LIST
      || ! MELT_PREDEF (DISCR_PAIR))
    goto end;
  pairv = meltgc_allocate (sizeof (struct meltpair_st), 0);
  pai_pairv->discr = (meltobject_ptr_t) MELT_PREDEF (DISCR_PAIR);
  pai_pairv->hd = (melt_ptr_t) valu;
  pai_pairv->tl = NULL;
  gcc_assert (melt_magic_discr ((melt_ptr_t) pairv) == OBMAG_PAIR);
  lastv = list_list->last;
  if (melt_magic_discr ((melt_ptr_t) lastv) == OBMAG_PAIR)
    {
      gcc_assert (((struct meltpair_st *) lastv)->tl == NULL);
      ((struct meltpair_st *) lastv)->tl = (struct meltpair_st *) pairv;
      meltgc_touch_dest (lastv, pairv);
    }
  else
    list_list->first = (struct meltpair_st *) pairv;
  list_list->last = (struct meltpair_st *) pairv;
  meltgc_touch_dest (list, pairv);
end:
  MELT_EXITFRAME ();
#undef list
#undef valu
#undef list_list
#undef pairv
#undef pai_pairv
#undef lastv
}

void
meltgc_prepend_list (melt_ptr_t list_p, melt_ptr_t valu_p)
{
  MELT_ENTERFRAME (4, NULL);
#define list curfram__.varptr[0]
#define valu curfram__.varptr[1]
#define pairv curfram__.varptr[2]
#define firstv curfram__.varptr[3]
#define pai_pairv ((struct meltpair_st*)(pairv))
#define list_list ((struct meltlist_st*)(list))
  list = list_p;
  valu = valu_p;
  if (melt_magic_discr ((melt_ptr_t) list) != OBMAG_LIST
      || ! MELT_PREDEF (DISCR_PAIR))
    goto end;
  pairv = meltgc_allocate (sizeof (struct meltpair_st), 0);
  pai_pairv->discr = (meltobject_ptr_t) MELT_PREDEF (DISCR_PAIR);
  pai_pairv->hd = (melt_ptr_t) valu;
  pai_pairv->tl = NULL;
  gcc_assert (melt_magic_discr ((melt_ptr_t) pairv) == OBMAG_PAIR);
  firstv = (melt_ptr_t) (list_list->first);
  if (melt_magic_discr ((melt_ptr_t) firstv) == OBMAG_PAIR)
    {
      pai_pairv->tl = (struct meltpair_st *) firstv;
      meltgc_touch_dest (pairv, firstv);
    }
  else
    list_list->last = (struct meltpair_st *) pairv;
  list_list->first = (struct meltpair_st *) pairv;
  meltgc_touch_dest (list, pairv);
end:
  MELT_EXITFRAME ();
#undef list
#undef valu
#undef list_list
#undef pairv
#undef pai_pairv
}


melt_ptr_t
meltgc_popfirst_list (melt_ptr_t list_p)
{
  MELT_ENTERFRAME (3, NULL);
#define list curfram__.varptr[0]
#define valu curfram__.varptr[1]
#define pairv curfram__.varptr[2]
#define pai_pairv ((struct meltpair_st*)(pairv))
#define list_list ((struct meltlist_st*)(list))
  list = list_p;
  if (melt_magic_discr ((melt_ptr_t) list) != OBMAG_LIST)
    goto end;
  pairv = list_list->first;
  if (melt_magic_discr ((melt_ptr_t) pairv) != OBMAG_PAIR)
    goto end;
  if (list_list->last == pairv)
    {
      valu = pai_pairv->hd;
      list_list->first = NULL;
      list_list->last = NULL;
    }
  else
    {
      valu = pai_pairv->hd;
      list_list->first = pai_pairv->tl;
    }
  meltgc_touch (list);
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) valu;
#undef list
#undef value
#undef list_list
#undef pairv
#undef pai_pairv
}				/* enf of popfirst */


/* return the length of a list or -1 iff non list */
int
melt_list_length (melt_ptr_t list_p)
{
  struct meltpair_st *pair = NULL;
  int ln = 0;
  if (!list_p)
    return 0;
  if (melt_magic_discr (list_p) != OBMAG_LIST)
    return -1;
  for (pair = ((struct meltlist_st *) list_p)->first;
       melt_magic_discr ((melt_ptr_t) pair) ==
       OBMAG_PAIR; pair = (struct meltpair_st *) (pair->tl))
    ln++;
  return ln;
}


/* allocate a new empty mapobjects */
melt_ptr_t
meltgc_new_mapobjects (meltobject_ptr_t discr_p, unsigned len)
{
  int maplen = 0;
  int lenix = 0, primlen = 0;
  MELT_ENTERFRAME (2, NULL);
#define discrv curfram__.varptr[0]
#define newmapv curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mapobject_newmapv ((struct meltmapobjects_st*)(newmapv))
  discrv = discr_p;
  if (!discrv || object_discrv->obj_class->object_magic != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MAPOBJECTS)
    goto end;
  if (len > 0)
    {
      gcc_assert (len < (unsigned) MELT_MAXLEN);
      for (lenix = 1;
	   (primlen = (int) melt_primtab[lenix]) != 0
	   && primlen <= (int) len; lenix++);
      maplen = primlen;
    };
  newmapv =
    meltgc_allocate (offsetof
			(struct meltmapobjects_st, map_space),
			maplen * sizeof (struct entryobjectsmelt_st));
  mapobject_newmapv->discr = object_discrv;
  if (len > 0)
    {
      mapobject_newmapv->entab = mapobject_newmapv->map_space;
      mapobject_newmapv->lenix = lenix;
    };
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmapv;
#undef discrv
#undef newmapv
#undef object_discrv
#undef mapobject_newmapv
}

/* get from a mapobject */
melt_ptr_t
melt_get_mapobjects (meltmapobjects_ptr_t mapobject_p,
			meltobject_ptr_t attrobject_p)
{
  long ix, len;
  melt_ptr_t val = NULL;
  if (!mapobject_p || !attrobject_p
      || mapobject_p->discr->object_magic != OBMAG_MAPOBJECTS
      || !mapobject_p->entab
      || attrobject_p->obj_class->object_magic != OBMAG_OBJECT)
    return NULL;
  len = melt_primtab[mapobject_p->lenix];
  if (len <= 0)
    return NULL;
  ix = unsafe_index_mapobject (mapobject_p->entab, attrobject_p, len);
  if (ix < 0)
    return NULL;
  if (mapobject_p->entab[ix].e_at == attrobject_p)
    val = mapobject_p->entab[ix].e_va;
  return val;
}

void
meltgc_put_mapobjects (meltmapobjects_ptr_t
			  mapobject_p,
			  meltobject_ptr_t attrobject_p,
			  melt_ptr_t valu_p)
{
  long ix = 0, len = 0, cnt = 0;
#if ENABLE_CHECKING
  static long callcount;
  long curcount = ++callcount;
#endif
  MELT_ENTERFRAME (4, NULL);
#define discrv curfram__.varptr[0]
#define mapobjectv curfram__.varptr[1]
#define attrobjectv curfram__.varptr[2]
#define valuv curfram__.varptr[3]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define object_attrobjectv ((meltobject_ptr_t)(attrobjectv))
#define map_mapobjectv ((meltmapobjects_ptr_t)(mapobjectv))
  mapobjectv = mapobject_p;
  attrobjectv = attrobject_p;
  valuv = valu_p;
  if (!mapobjectv || !attrobjectv || !valuv)
    goto end;
  discrv = map_mapobjectv->discr;
  if (!discrv || object_discrv->object_magic != OBMAG_MAPOBJECTS)
    goto end;
  discrv = object_attrobjectv->obj_class;
  if (!discrv || object_discrv->object_magic != OBMAG_OBJECT)
    goto end;
  if (!map_mapobjectv->entab)
    {
      /* fresh map without any entab; allocate it minimally */
      size_t lensiz = 0;
      len = melt_primtab[1];	/* i.e. 3 */
      lensiz = len * sizeof (struct entryobjectsmelt_st);
      if (melt_is_young (mapobjectv))
	{
	  meltgc_reserve (lensiz + 20);
	  if (!melt_is_young (mapobjectv))
	    goto alloc_old_smallmapobj;
	  map_mapobjectv->entab =
	    (struct entryobjectsmelt_st *)
	    melt_allocatereserved (lensiz, 0);
	}
      else
	{
	alloc_old_smallmapobj:
	  map_mapobjectv->entab =
	    (struct entryobjectsmelt_st *) ggc_alloc_cleared (lensiz);
	}
      map_mapobjectv->lenix = 1;
      meltgc_touch (map_mapobjectv);
    }
  else
    if ((len = melt_primtab[map_mapobjectv->lenix]) <=
	(5 * (cnt = map_mapobjectv->count)) / 4
	|| (len <= 5 && cnt + 1 >= len))
    {
      /* entab is nearly full so need to be resized */
      int ix, newcnt = 0;
      int newlen = melt_primtab[map_mapobjectv->lenix + 1];
      size_t newlensiz = 0;
      struct entryobjectsmelt_st *newtab = NULL;
      struct entryobjectsmelt_st *oldtab = NULL;
      newlensiz = newlen * sizeof (struct entryobjectsmelt_st);
      if (melt_is_young (map_mapobjectv->entab))
	{
	  meltgc_reserve (newlensiz + 100);
	  if (!melt_is_young (map_mapobjectv))
	    goto alloc_old_mapobj;
	  newtab =
	    (struct entryobjectsmelt_st *)
	    melt_allocatereserved (newlensiz, 0);
	}
      else
	{
	alloc_old_mapobj:
	  newtab =
	    (struct entryobjectsmelt_st *) ggc_alloc_cleared (newlensiz);
	};
      oldtab = map_mapobjectv->entab;
      for (ix = 0; ix < len; ix++)
	{
	  meltobject_ptr_t curat = oldtab[ix].e_at;
	  int newix;
	  if (!curat || curat == (void *) HTAB_DELETED_ENTRY)
	    continue;
	  newix = unsafe_index_mapobject (newtab, curat, newlen);
	  gcc_assert (newix >= 0);
	  newtab[newix] = oldtab[ix];
	  newcnt++;
	}
      if (!melt_is_young (oldtab))
	/* free oldtab since it is in old ggc space */
	ggc_free (oldtab);
      map_mapobjectv->entab = newtab;
      map_mapobjectv->count = newcnt;
      map_mapobjectv->lenix++;
      meltgc_touch (map_mapobjectv);
      len = newlen;
    }
  ix =
    unsafe_index_mapobject (map_mapobjectv->entab, object_attrobjectv, len);
#if ENABLE_CHECKING
  if (ix < 0)
    debugeprintf
      ("put_mapobjects failed curcount %ld len %ld map %p count %d lenix %d",
       curcount, len, mapobjectv, map_mapobjectv->count,
       map_mapobjectv->lenix);
#endif
  gcc_assert (ix >= 0);
  if (map_mapobjectv->entab[ix].e_at != attrobjectv)
    {
      map_mapobjectv->entab[ix].e_at = (meltobject_ptr_t) attrobjectv;
      map_mapobjectv->count++;
    }
  map_mapobjectv->entab[ix].e_va = (melt_ptr_t) valuv;
  meltgc_touch_dest (map_mapobjectv, attrobjectv);
  meltgc_touch_dest (map_mapobjectv, valuv);
end:
  MELT_EXITFRAME ();
#undef discrv
#undef mapobjectv
#undef attrobjectv
#undef valuv
#undef object_discrv
#undef object_attrobjectv
#undef map_mapobjectv
}


melt_ptr_t
meltgc_remove_mapobjects (meltmapobjects_ptr_t
			     mapobject_p, meltobject_ptr_t attrobject_p)
{
  long ix = 0, len = 0, cnt = 0;
  MELT_ENTERFRAME (4, NULL);
#define discrv curfram__.varptr[0]
#define mapobjectv curfram__.varptr[1]
#define attrobjectv curfram__.varptr[2]
#define valuv curfram__.varptr[3]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define object_attrobjectv ((meltobject_ptr_t)(attrobjectv))
#define map_mapobjectv ((meltmapobjects_ptr_t)(mapobjectv))
  mapobjectv = mapobject_p;
  attrobjectv = attrobject_p;
  valuv = NULL;
  if (!mapobjectv || !attrobjectv)
    goto end;
  discrv = map_mapobjectv->discr;
  if (!discrv || object_discrv->object_magic != OBMAG_MAPOBJECTS)
    goto end;
  discrv = object_attrobjectv->obj_class;
  if (!discrv || object_discrv->object_magic != OBMAG_OBJECT)
    goto end;
  if (!map_mapobjectv->entab)
    goto end;
  len = melt_primtab[map_mapobjectv->lenix];
  if (len <= 0)
    goto end;
  ix = unsafe_index_mapobject (map_mapobjectv->entab, attrobject_p, len);
  if (ix < 0 || map_mapobjectv->entab[ix].e_at != attrobjectv)
    goto end;
  map_mapobjectv->entab[ix].e_at = (meltobject_ptr_t) HTAB_DELETED_ENTRY;
  valuv = map_mapobjectv->entab[ix].e_va;
  map_mapobjectv->entab[ix].e_va = NULL;
  map_mapobjectv->count--;
  cnt = map_mapobjectv->count;
  if (len >= 7 && cnt < len / 2 - 2)
    {
      int newcnt = 0, newlen = 0, newlenix = 0;
      size_t newlensiz = 0;
      struct entryobjectsmelt_st *oldtab = NULL, *newtab = NULL;
      for (newlenix = map_mapobjectv->lenix;
	   (newlen = melt_primtab[newlenix]) > 2 * cnt + 3; newlenix--);
      if (newlen >= len)
	goto end;
      newlensiz = newlen * sizeof (struct entryobjectsmelt_st);
      if (melt_is_young (map_mapobjectv->entab))
	{
	  /* reserve a zone; if a GC occurred, the mapobject & entab
	     could become old */
	  meltgc_reserve (newlensiz + 10 * sizeof (void *));
	  if (!melt_is_young (map_mapobjectv))
	    goto alloc_old_entries;
	  newtab =
	    (struct entryobjectsmelt_st *)
	    melt_allocatereserved (newlensiz, 0);
	}
      else
	{
	alloc_old_entries:
	  newtab =
	    (struct entryobjectsmelt_st *) ggc_alloc_cleared (newlensiz);
	}
      oldtab = map_mapobjectv->entab;
      for (ix = 0; ix < len; ix++)
	{
	  meltobject_ptr_t curat = oldtab[ix].e_at;
	  int newix;
	  if (!curat || curat == (void *) HTAB_DELETED_ENTRY)
	    continue;
	  newix = unsafe_index_mapobject (newtab, curat, newlen);
	  gcc_assert (newix >= 0);
	  newtab[newix] = oldtab[ix];
	  newcnt++;
	}
      if (!melt_is_young (oldtab))
	/* free oldtab since it is in old ggc space */
	ggc_free (oldtab);
      map_mapobjectv->entab = newtab;
      map_mapobjectv->count = newcnt;
      map_mapobjectv->lenix = newlenix;
    }
  meltgc_touch (map_mapobjectv);
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) valuv;
#undef discrv
#undef mapobjectv
#undef attrobjectv
#undef valuv
#undef object_discrv
#undef object_attrobjectv
#undef map_mapobjectv
}



/* index of entry to get or add an attribute in an mapstring (or -1 on error) */
static inline int
unsafe_index_mapstring (struct entrystringsmelt_st *tab,
			const char *attr, int siz)
{
  int ix = 0, frix = -1;
  unsigned h = 0;
  if (!tab || !attr || siz <= 0)
    return -1;
  h = (unsigned) htab_hash_string (attr) & MELT_MAXHASH;
  h = h % siz;
  for (ix = h; ix < siz; ix++)
    {
      const char *curat = tab[ix].e_at;
      if (curat == (void *) HTAB_DELETED_ENTRY)
	{
	  if (frix < 0)
	    frix = ix;
	}
      else if (!curat)
	{
	  if (frix < 0)
	    frix = ix;
	  return frix;
	}
      else if (!strcmp (curat, attr))
	return ix;
    }
  for (ix = 0; ix < (int) h; ix++)
    {
      const char *curat = tab[ix].e_at;
      if (curat == (void *) HTAB_DELETED_ENTRY)
	{
	  if (frix < 0)
	    frix = ix;
	}
      else if (!curat)
	{
	  if (frix < 0)
	    frix = ix;
	  return frix;
	}
      else if (!strcmp (curat, attr))
	return ix;
    }
  if (frix >= 0)		/* found a place in a table with deleted entries */
    return frix;
  return -1;			/* entirely full, should not happen */
}

/* allocate a new empty mapstrings */
melt_ptr_t
meltgc_new_mapstrings (meltobject_ptr_t discr_p, unsigned len)
{
  MELT_ENTERFRAME (2, NULL);
#define discrv curfram__.varptr[0]
#define newmapv curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define mapstring_newmapv ((struct meltmapstrings_st*)(newmapv))
  discrv = discr_p;
  if (!discrv || object_discrv->obj_class->object_magic != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_MAPSTRINGS)
    goto end;
  newmapv = meltgc_allocate (sizeof (struct meltmapstrings_st), 0);
  mapstring_newmapv->discr = object_discrv;
  if (len > 0)
    {
      int lenix, primlen;
      gcc_assert (len < (unsigned) MELT_MAXLEN);
      for (lenix = 1;
	   (primlen = (int) melt_primtab[lenix]) != 0
	   && primlen <= (int) len; lenix++);
      /* the newmapv is always young */
      mapstring_newmapv->entab = (struct entrystringsmelt_st *)
	meltgc_allocate (primlen *
			    sizeof (struct entrystringsmelt_st), 0);
      mapstring_newmapv->lenix = lenix;
      meltgc_touch_dest (newmapv, mapstring_newmapv->entab);
    }
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) newmapv;
#undef discrv
#undef newmapv
#undef object_discrv
#undef mapstring_newmapv
}


void
meltgc_put_mapstrings (struct meltmapstrings_st
			  *mapstring_p, const char *attr,
			  melt_ptr_t valu_p)
{
  long ix = 0, len = 0, cnt = 0, atlen = 0;
  char *attrdup = 0;
  char tinybuf[130];
  MELT_ENTERFRAME (3, NULL);
#define discrv curfram__.varptr[0]
#define mapstringv curfram__.varptr[1]
#define valuv curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define map_mapstringv ((struct meltmapstrings_st*)(mapstringv))
  mapstringv = mapstring_p;
  valuv = valu_p;
  if (!mapstringv || !attr || !attr[0] || !valuv)
    goto end;
  discrv = map_mapstringv->discr;
  if (!discrv || object_discrv->object_magic != OBMAG_MAPSTRINGS)
    goto end;
  atlen = strlen (attr);
  if (atlen < (int) sizeof (tinybuf) - 1)
    {
      memset (tinybuf, 0, sizeof (tinybuf));
      attrdup = strcpy (tinybuf, attr);
    }
  else
    attrdup = strcpy ((char *) xcalloc (atlen + 1, 1), attr);
  if (!map_mapstringv->entab)
    {
      size_t lensiz = 0;
      len = melt_primtab[1];	/* i.e. 3 */
      lensiz = len * sizeof (struct entrystringsmelt_st);
      if (melt_is_young (mapstringv))
	{
	  meltgc_reserve (lensiz + 16 * sizeof (void *));
	  if (!melt_is_young (mapstringv))
	    goto alloc_old_small_mapstring;
	  map_mapstringv->entab =
	    (struct entrystringsmelt_st *)
	    melt_allocatereserved (lensiz, 0);
	}
      else
	{
	alloc_old_small_mapstring:
	  map_mapstringv->entab =
	    (struct entrystringsmelt_st *) ggc_alloc_cleared (lensiz);
	}
      map_mapstringv->lenix = 1;
      meltgc_touch (map_mapstringv);
    }
  else
    if ((len = melt_primtab[map_mapstringv->lenix]) <=
	(5 * (cnt = map_mapstringv->count)) / 4
	|| (len <= 5 && cnt + 1 >= len))
    {
      int ix, newcnt = 0;
      int newlen = melt_primtab[map_mapstringv->lenix + 1];
      struct entrystringsmelt_st *oldtab = NULL;
      struct entrystringsmelt_st *newtab = NULL;
      size_t newlensiz = newlen * sizeof (struct entrystringsmelt_st);
      if (melt_is_young (mapstringv))
	{
	  meltgc_reserve (newlensiz + 10 * sizeof (void *));
	  if (!melt_is_young (mapstringv))
	    goto alloc_old_mapstring;
	  newtab =
	    (struct entrystringsmelt_st *)
	    melt_allocatereserved (newlensiz, 0);
	}
      else
	{
	alloc_old_mapstring:
	  newtab =
	    (struct entrystringsmelt_st *) ggc_alloc_cleared (newlensiz);
	};
      oldtab = map_mapstringv->entab;
      for (ix = 0; ix < len; ix++)
	{
	  const char *curat = oldtab[ix].e_at;
	  int newix;
	  if (!curat || curat == (void *) HTAB_DELETED_ENTRY)
	    continue;
	  newix = unsafe_index_mapstring (newtab, curat, newlen);
	  gcc_assert (newix >= 0);
	  newtab[newix] = oldtab[ix];
	  newcnt++;
	}
      if (!melt_is_young (oldtab))
	/* free oldtab since it is in old ggc space */
	ggc_free (oldtab);
      map_mapstringv->entab = newtab;
      map_mapstringv->count = newcnt;
      map_mapstringv->lenix++;
      meltgc_touch (map_mapstringv);
      len = newlen;
    }
  ix = unsafe_index_mapstring (map_mapstringv->entab, attrdup, len);
  gcc_assert (ix >= 0);
  if (!map_mapstringv->entab[ix].e_at
      || map_mapstringv->entab[ix].e_at == HTAB_DELETED_ENTRY)
    {
      char *newat = (char *) meltgc_allocate (atlen + 1, 0);
      strcpy (newat, attrdup);
      map_mapstringv->entab[ix].e_at = newat;
      map_mapstringv->count++;
    }
  map_mapstringv->entab[ix].e_va = (melt_ptr_t) valuv;
  meltgc_touch_dest (map_mapstringv, valuv);
end:
  if (attrdup && attrdup != tinybuf)
    free (attrdup);
  MELT_EXITFRAME ();
#undef discrv
#undef mapstringv
#undef attrobjectv
#undef valuv
#undef object_discrv
#undef object_attrobjectv
#undef map_mapstringv
}

melt_ptr_t
melt_get_mapstrings (struct meltmapstrings_st
			*mapstring_p, const char *attr)
{
  long ix = 0, len = 0;
  const char *oldat = NULL;
  if (!mapstring_p || !attr)
    return NULL;
  if (mapstring_p->discr->object_magic != OBMAG_MAPSTRINGS)
    return NULL;
  if (!mapstring_p->entab)
    return NULL;
  len = melt_primtab[mapstring_p->lenix];
  if (len <= 0)
    return NULL;
  ix = unsafe_index_mapstring (mapstring_p->entab, attr, len);
  if (ix < 0 || !(oldat = mapstring_p->entab[ix].e_at)
      || oldat == HTAB_DELETED_ENTRY)
    return NULL;
  return mapstring_p->entab[ix].e_va;
}

melt_ptr_t
meltgc_remove_mapstrings (struct meltmapstrings_st *
			     mapstring_p, const char *attr)
{
  long ix = 0, len = 0, cnt = 0, atlen = 0;
  const char *oldat = NULL;
  char *attrdup = 0;
  char tinybuf[130];
  MELT_ENTERFRAME (3, NULL);
#define discrv curfram__.varptr[0]
#define mapstringv curfram__.varptr[1]
#define valuv curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define map_mapstringv ((struct meltmapstrings_st*)(mapstringv))
  mapstringv = mapstring_p;
  valuv = NULL;
  if (!mapstringv || !attr || !valuv || !attr[0])
    goto end;
  atlen = strlen (attr);
  discrv = map_mapstringv->discr;
  if (!discrv || object_discrv->object_magic != OBMAG_MAPSTRINGS)
    goto end;
  if (!map_mapstringv->entab)
    goto end;
  len = melt_primtab[map_mapstringv->lenix];
  if (len <= 0)
    goto end;
  if (atlen < (int) sizeof (tinybuf) - 1)
    {
      memset (tinybuf, 0, sizeof (tinybuf));
      attrdup = strcpy (tinybuf, attr);
    }
  else
    attrdup = strcpy ((char *) xcalloc (atlen + 1, 1), attr);
  ix = unsafe_index_mapstring (map_mapstringv->entab, attrdup, len);
  if (ix < 0 || !(oldat = map_mapstringv->entab[ix].e_at)
      || oldat == HTAB_DELETED_ENTRY)
    goto end;
  if (!melt_is_young (oldat))
    ggc_free (CONST_CAST(char *, oldat));
  map_mapstringv->entab[ix].e_at = (char *) HTAB_DELETED_ENTRY;
  valuv = map_mapstringv->entab[ix].e_va;
  map_mapstringv->entab[ix].e_va = NULL;
  map_mapstringv->count--;
  cnt = map_mapstringv->count;
  if (len > 7 && 2 * cnt + 2 < len)
    {
      int newcnt = 0, newlen = 0, newlenix = 0;
      size_t newlensiz = 0;
      struct entrystringsmelt_st *oldtab = NULL, *newtab = NULL;
      for (newlenix = map_mapstringv->lenix;
	   (newlen = melt_primtab[newlenix]) > 2 * cnt + 3; newlenix--);
      if (newlen >= len)
	goto end;
      newlensiz = newlen * sizeof (struct entrystringsmelt_st);
      if (melt_is_young (mapstringv))
	{
	  meltgc_reserve (newlensiz + 10 * sizeof (void *));
	  if (!melt_is_young (mapstringv))
	    goto alloc_old_mapstring_newtab;
	  newtab =
	    (struct entrystringsmelt_st *)
	    melt_allocatereserved (newlensiz, 0);
	}
      else
	{
	alloc_old_mapstring_newtab:
	  newtab =
	    (struct entrystringsmelt_st *) ggc_alloc_cleared (newlensiz);
	}
      oldtab = map_mapstringv->entab;
      for (ix = 0; ix < len; ix++)
	{
	  const char *curat = oldtab[ix].e_at;
	  int newix;
	  if (!curat || curat == (void *) HTAB_DELETED_ENTRY)
	    continue;
	  newix = unsafe_index_mapstring (newtab, curat, newlen);
	  gcc_assert (newix >= 0);
	  newtab[newix] = oldtab[ix];
	  newcnt++;
	}
      if (!melt_is_young (oldtab))
	/* free oldtab since it is in ol<d ggc space */
	ggc_free (oldtab);
      map_mapstringv->entab = newtab;
      map_mapstringv->count = newcnt;
    }
  meltgc_touch (map_mapstringv);
end:
  if (attrdup && attrdup != tinybuf)
    free (attrdup);
  MELT_EXITFRAME ();
  return (melt_ptr_t) valuv;
#undef discrv
#undef mapstringv
#undef valuv
#undef object_discrv
#undef map_mapstringv
}







/* index of entry to get or add an attribute in an mappointer (or -1 on error) */
struct entrypointermelt_st
{
  const void *e_at;
  melt_ptr_t e_va;
};
static inline int
unsafe_index_mappointer (struct entrypointermelt_st *tab,
			 const void *attr, int siz)
{
  int ix = 0, frix = -1;
  unsigned h = 0;
  if (!tab || !attr || siz <= 0)
    return -1;
  h = ((unsigned) (((long) (attr)) >> 3)) & MELT_MAXHASH;
  h = h % siz;
  for (ix = h; ix < siz; ix++)
    {
      const void *curat = tab[ix].e_at;
      if (curat == (void *) HTAB_DELETED_ENTRY)
	{
	  if (frix < 0)
	    frix = ix;
	}
      else if (!curat)
	{
	  if (frix < 0)
	    frix = ix;
	  return frix;
	}
      else if (curat == attr)
	return ix;
    }
  for (ix = 0; ix < (int) h; ix++)
    {
      const void *curat = tab[ix].e_at;
      if (curat == (void *) HTAB_DELETED_ENTRY)
	{
	  if (frix < 0)
	    frix = ix;
	}
      else if (!curat)
	{
	  if (frix < 0)
	    frix = ix;
	  return frix;
	}
      else if (curat == attr)
	return ix;
    }
  if (frix >= 0)		/* found some place in a table with deleted entries */
    return frix;
  return -1;			/* entirely full, should not happen */
}


/* this should be the same as meltmaptrees_st, meltmapedges_st,
   meltmapbasicblocks_st, .... */
struct meltmappointers_st
{
  meltobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entrypointermelt_st *entab;
  /* the following field is usually the value of entab (for
     objects in the young zone), to allocate the object and its fields
     at once */
  struct entrypointermelt_st map_space[FLEXIBLE_DIM];
};
/* allocate a new empty mappointers without checks */
void *
meltgc_raw_new_mappointers (meltobject_ptr_t discr_p, unsigned len)
{
  int lenix = 0, primlen = 0;
  MELT_ENTERFRAME (2, NULL);
#define discrv curfram__.varptr[0]
#define newmapv curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define map_newmapv ((struct meltmappointers_st*)(newmapv))
  discrv = discr_p;
  if (len > 0)
    {
      gcc_assert (len < (unsigned) MELT_MAXLEN);
      for (lenix = 1;
	   (primlen = (int) melt_primtab[lenix]) != 0
	   && primlen <= (int) len; lenix++);
    };
  gcc_assert (sizeof (struct entrypointermelt_st) ==
	      sizeof (struct entrytreesmelt_st));
  gcc_assert (sizeof (struct entrypointermelt_st) ==
	      sizeof (struct entrygimplesmelt_st));
  gcc_assert (sizeof (struct entrypointermelt_st) ==
	      sizeof (struct entryedgesmelt_st));
  gcc_assert (sizeof (struct entrypointermelt_st) ==
	      sizeof (struct entrybasicblocksmelt_st));
  newmapv =
    meltgc_allocate (offsetof
			(struct meltmappointers_st,
			 map_space),
			primlen * sizeof (struct entrypointermelt_st));
  map_newmapv->discr = object_discrv;
  map_newmapv->count = 0;
  map_newmapv->lenix = lenix;
  if (len > 0)
    map_newmapv->entab = map_newmapv->map_space;
  else
    map_newmapv->entab = NULL;
  MELT_EXITFRAME ();
  return newmapv;
#undef discrv
#undef newmapv
#undef object_discrv
#undef map_newmapv
}


void
meltgc_raw_put_mappointers (void *mappointer_p,
			       const void *attr, melt_ptr_t valu_p)
{
  long ix = 0, len = 0, cnt = 0;
  size_t lensiz = 0;
  MELT_ENTERFRAME (2, NULL);
#define mappointerv curfram__.varptr[0]
#define valuv curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define map_mappointerv ((struct meltmappointers_st*)(mappointerv))
  mappointerv = mappointer_p;
  valuv = valu_p;
  if (!map_mappointerv->entab)
    {
      len = melt_primtab[1];	/* i.e. 3 */
      lensiz = len * sizeof (struct entrypointermelt_st);
      if (melt_is_young (mappointerv))
	{
	  meltgc_reserve (lensiz + 10 * sizeof (void *));
	  if (!melt_is_young (mappointerv))
	    goto alloc_old_mappointer_small_entab;
	  map_mappointerv->entab =
	    (struct entrypointermelt_st *)
	    melt_allocatereserved (lensiz, 0);
	}
      else
	{
	alloc_old_mappointer_small_entab:
	  map_mappointerv->entab = (struct entrypointermelt_st *)
	    ggc_alloc_cleared (len * sizeof (struct entrypointermelt_st));
	}
      map_mappointerv->lenix = 1;
      meltgc_touch (map_mappointerv);
    }
  else
    if ((len = melt_primtab[map_mappointerv->lenix]) <=
	(5 * (cnt = map_mappointerv->count)) / 4
	|| (len <= 5 && cnt + 1 >= len))
    {
      int ix, newcnt = 0;
      int newlen = melt_primtab[map_mappointerv->lenix + 1];
      struct entrypointermelt_st *oldtab = NULL;
      struct entrypointermelt_st *newtab = NULL;
      size_t newlensiz = newlen * sizeof (struct entrypointermelt_st);
      if (melt_is_young (mappointerv))
	{
	  meltgc_reserve (newlensiz + 10 * sizeof (void *));
	  if (!melt_is_young (mappointerv))
	    goto alloc_old_mappointer_entab;
	  newtab =
	    (struct entrypointermelt_st *)
	    melt_allocatereserved (newlensiz, 0);
	}
      else
	{
	alloc_old_mappointer_entab:
	  newtab = (struct entrypointermelt_st *)
	    ggc_alloc_cleared (newlen *
			       sizeof (struct entrypointermelt_st));
	}
      oldtab = map_mappointerv->entab;
      for (ix = 0; ix < len; ix++)
	{
	  const void *curat = oldtab[ix].e_at;
	  int newix;
	  if (!curat || curat == (void *) HTAB_DELETED_ENTRY)
	    continue;
	  newix = unsafe_index_mappointer (newtab, curat, newlen);
	  gcc_assert (newix >= 0);
	  newtab[newix] = oldtab[ix];
	  newcnt++;
	}
      if (!melt_is_young (oldtab))
	/* free oldtab since it is in old ggc space */
	ggc_free (oldtab);
      map_mappointerv->entab = newtab;
      map_mappointerv->count = newcnt;
      map_mappointerv->lenix++;
      meltgc_touch (map_mappointerv);
      len = newlen;
    }
  ix = unsafe_index_mappointer (map_mappointerv->entab, attr, len);
  gcc_assert (ix >= 0);
  if (!map_mappointerv->entab[ix].e_at
      || map_mappointerv->entab[ix].e_at == HTAB_DELETED_ENTRY)
    {
      map_mappointerv->entab[ix].e_at = attr;
      map_mappointerv->count++;
    }
  map_mappointerv->entab[ix].e_va = (melt_ptr_t) valuv;
  meltgc_touch_dest (map_mappointerv, valuv);
  MELT_EXITFRAME ();
#undef discrv
#undef mappointerv
#undef valuv
#undef object_discrv
#undef map_mappointerv
}

melt_ptr_t
melt_raw_get_mappointers (void *map, const void *attr)
{
  long ix = 0, len = 0;
  const void *oldat = NULL;
  struct meltmappointers_st *mappointer_p =
    (struct meltmappointers_st *) map;
  if (!mappointer_p->entab)
    return NULL;
  len = melt_primtab[mappointer_p->lenix];
  if (len <= 0)
    return NULL;
  ix = unsafe_index_mappointer (mappointer_p->entab, attr, len);
  if (ix < 0 || !(oldat = mappointer_p->entab[ix].e_at)
      || oldat == HTAB_DELETED_ENTRY)
    return NULL;
  return mappointer_p->entab[ix].e_va;
}

melt_ptr_t
meltgc_raw_remove_mappointers (void *mappointer_p, const void *attr)
{
  long ix = 0, len = 0, cnt = 0;
  const char *oldat = NULL;
  MELT_ENTERFRAME (3, NULL);
#define discrv curfram__.varptr[0]
#define mappointerv curfram__.varptr[1]
#define valuv curfram__.varptr[2]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define map_mappointerv ((struct meltmappointers_st*)(mappointerv))
  mappointerv = mappointer_p;
  valuv = NULL;
  if (!map_mappointerv->entab)
    goto end;
  len = melt_primtab[map_mappointerv->lenix];
  if (len <= 0)
    goto end;
  ix = unsafe_index_mappointer (map_mappointerv->entab, attr, len);
  if (ix < 0 || !(oldat = (const char *) map_mappointerv->entab[ix].e_at)
      || oldat == HTAB_DELETED_ENTRY)
    goto end;
  map_mappointerv->entab[ix].e_at = (void *) HTAB_DELETED_ENTRY;
  valuv = map_mappointerv->entab[ix].e_va;
  map_mappointerv->entab[ix].e_va = NULL;
  map_mappointerv->count--;
  cnt = map_mappointerv->count;
  if (len > 7 && 2 * cnt + 2 < len)
    {
      int newcnt = 0, newlen = 0, newlenix = 0;
      struct entrypointermelt_st *oldtab = NULL, *newtab = NULL;
      size_t newlensiz = 0;
      for (newlenix = map_mappointerv->lenix;
	   (newlen = melt_primtab[newlenix]) > 2 * cnt + 3; newlenix--);
      if (newlen >= len)
	goto end;
      newlensiz = newlen * sizeof (struct entrypointermelt_st);
      if (melt_is_young (mappointerv))
	{
	  meltgc_reserve (newlensiz + 10 * sizeof (void *));
	  if (!melt_is_young (mappointerv))
	    goto allocate_old_newtab_mappointer;
	  newtab =
	    (struct entrypointermelt_st *)
	    melt_allocatereserved (newlensiz, 0);
	}
      else
	{
	allocate_old_newtab_mappointer:
	  newtab = (struct entrypointermelt_st *)
	    ggc_alloc_cleared (newlen *
			       sizeof (struct entrypointermelt_st));
	};
      oldtab = map_mappointerv->entab;
      for (ix = 0; ix < len; ix++)
	{
	  const void *curat = oldtab[ix].e_at;
	  int newix;
	  if (!curat || curat == (void *) HTAB_DELETED_ENTRY)
	    continue;
	  newix = unsafe_index_mappointer (newtab, curat, newlen);
	  gcc_assert (newix >= 0);
	  newtab[newix] = oldtab[ix];
	  newcnt++;
	}
      if (!melt_is_young (oldtab))
	/* free oldtab since it is in old ggc space */
	ggc_free (oldtab);
      map_mappointerv->entab = newtab;
      map_mappointerv->count = newcnt;
    }
  meltgc_touch (map_mappointerv);
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) valuv;
#undef discrv
#undef mappointerv
#undef valuv
#undef object_discrv
#undef map_mappointerv
}


/***************** objvlisp test of strict subclassing */
bool
melt_is_subclass_of (meltobject_ptr_t subclass_p,
			meltobject_ptr_t superclass_p)
{
  struct meltmultiple_st *subanc = NULL;
  struct meltmultiple_st *superanc = NULL;
  unsigned subdepth = 0, superdepth = 0;
  if (melt_magic_discr ((melt_ptr_t) subclass_p) !=
      OBMAG_OBJECT || subclass_p->object_magic != OBMAG_OBJECT
      || melt_magic_discr ((melt_ptr_t) superclass_p) !=
      OBMAG_OBJECT || superclass_p->object_magic != OBMAG_OBJECT)
    {
      return FALSE;
    }
  if (subclass_p->obj_len < FCLASS__LAST
      || !subclass_p->obj_vartab
      || superclass_p->obj_len < FCLASS__LAST || !superclass_p->obj_vartab)
    {
      return FALSE;
    }
  if (superclass_p == (meltobject_ptr_t) MELT_PREDEF (CLASS_ROOT))
    return TRUE;
  subanc =
    (struct meltmultiple_st *) subclass_p->obj_vartab[FCLASS_ANCESTORS];
  superanc =
    (struct meltmultiple_st *) superclass_p->obj_vartab[FCLASS_ANCESTORS];
  if (melt_magic_discr ((melt_ptr_t) subanc) != OBMAG_MULTIPLE
      || subanc->discr != (meltobject_ptr_t) MELT_PREDEF (DISCR_SEQCLASS))
    {
      return FALSE;
    }
  if (melt_magic_discr ((melt_ptr_t) superanc) != OBMAG_MULTIPLE
      || superanc->discr != (meltobject_ptr_t) MELT_PREDEF (DISCR_SEQCLASS))
    {
      return FALSE;
    }
  subdepth = subanc->nbval;
  superdepth = superanc->nbval;
  if (subdepth <= superdepth)
    return FALSE;
  if ((melt_ptr_t) subanc->tabval[superdepth] ==
      (melt_ptr_t) superclass_p)
    return TRUE;
  return FALSE;
}


melt_ptr_t
meltgc_new_string_raw_len (meltobject_ptr_t discr_p, const char *str, int slen)
{
  MELT_ENTERFRAME (2, NULL);
#define discrv     curfram__.varptr[0]
#define strv       curfram__.varptr[1]
#define obj_discrv  ((struct meltobject_st*)(discrv))
#define str_strv  ((struct meltstring_st*)(strv))
  strv = 0;
  if (!str)
    goto end;
  if (slen<0)
    slen = strlen (str);
  discrv = discr_p;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (obj_discrv->object_magic != OBMAG_STRING)
    goto end;
  strv = meltgc_allocate (sizeof (struct meltstring_st), slen + 1);
  str_strv->discr = obj_discrv;
  memcpy (str_strv->val, str, slen);
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) strv;
#undef discrv
#undef strv
#undef obj_discrv
#undef str_strv
}

melt_ptr_t
meltgc_new_string (meltobject_ptr_t discr_p, const char *str)
{
  return meltgc_new_string_raw_len(discr_p, str, -1);
}

melt_ptr_t
meltgc_new_stringdup (meltobject_ptr_t discr_p, const char *str)
{
  int slen = 0;
  char tinybuf[80];
  char *strcop = 0;
  MELT_ENTERFRAME (2, NULL);
#define discrv     curfram__.varptr[0]
#define strv       curfram__.varptr[1]
#define obj_discrv  ((struct meltobject_st*)(discrv))
#define str_strv  ((struct meltstring_st*)(strv))
  strv = 0;
  if (!str)
    goto end;
  discrv = discr_p;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (obj_discrv->object_magic != OBMAG_STRING)
    goto end;
  slen = strlen (str);
  if (slen < (int) sizeof (tinybuf) - 1)
    {
      memset (tinybuf, 0, sizeof (tinybuf));
      strcop = strcpy (tinybuf, str);
    }
  else
    strcop = strcpy ((char *) xcalloc (1, slen + 1), str);
  strv = meltgc_allocate (sizeof (struct meltstring_st), slen + 1);
  str_strv->discr = obj_discrv;
  strcpy (str_strv->val, strcop);
end:
  if (strcop && strcop != tinybuf)
    free (strcop);
  memset (tinybuf, 0, sizeof (tinybuf));
  MELT_EXITFRAME ();
  return (melt_ptr_t) strv;
#undef discrv
#undef strv
#undef obj_discrv
#undef str_strv
}

melt_ptr_t
meltgc_new_string_nakedbasename (meltobject_ptr_t
				    discr_p, const char *str)
{
  int slen = 0;
  char tinybuf[120];
  char *strcop = 0;
  const char *basestr = 0;
  char *dot = 0;
  MELT_ENTERFRAME (2, NULL);
#define discrv     curfram__.varptr[0]
#define strv       curfram__.varptr[1]
#define obj_discrv  ((struct meltobject_st*)(discrv))
#define str_strv  ((struct meltstring_st*)(strv))
  strv = 0;
  if (!str)
    goto end;
  discrv = discr_p;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (obj_discrv->object_magic != OBMAG_STRING)
    goto end;
  slen = strlen (str);
  if (slen < (int) sizeof (tinybuf) - 1)
    {
      memset (tinybuf, 0, sizeof (tinybuf));
      strcop = strcpy (tinybuf, str);
    }
  else
    strcop = strcpy ((char *) xcalloc (1, slen + 1), str);
  basestr = (const char *) lbasename (strcop);
  dot = strchr (basestr, '.');
  if (dot)
    *dot = 0;
  strv =
    meltgc_allocate (sizeof (struct meltstring_st),
			strlen (basestr) + 1);
  str_strv->discr = obj_discrv;
  strcpy (str_strv->val, basestr);
end:
  if (strcop && strcop != tinybuf)
    free (strcop);
  memset (tinybuf, 0, sizeof (tinybuf));
  MELT_EXITFRAME ();
  return (melt_ptr_t) strv;
#undef discrv
#undef strv
#undef obj_discrv
#undef str_strv
}


melt_ptr_t
meltgc_new_string_tempname_suffixed (meltobject_ptr_t
					discr_p, const char *namstr, const char *suffstr)
{
  int slen = 0;
  char suffix[16];
  const char *basestr = xstrdup(lbasename(namstr));
  const char* tempnampath = 0;
  char *dot = 0;
  MELT_ENTERFRAME (2, NULL);
#define discrv     curfram__.varptr[0]
#define strv       curfram__.varptr[1]
#define obj_discrv  ((struct meltobject_st*)(discrv))
#define str_strv  ((struct meltstring_st*)(strv))
  memset(suffix, 0, sizeof(suffix));
  if (suffstr) strncpy(suffix, suffstr, sizeof(suffix)-1);
  if (basestr)
    dot = strrchr(basestr, '.');
  if (dot)
    *dot=0;
  tempnampath = melt_tempdir_path (basestr, suffix);
  dbgprintf ("new_string_tempbasename basestr='%s' tempnampath='%s'", basestr, tempnampath);
  free(CONST_CAST(char*,basestr));
  basestr = 0;
  strv = 0;
  if (!tempnampath)
    goto end;
  discrv = discr_p;
  if (melt_magic_discr ((melt_ptr_t) discrv) != OBMAG_OBJECT)
    goto end;
  if (obj_discrv->object_magic != OBMAG_STRING)
    goto end;
  slen = strlen (tempnampath);
  strv =
    meltgc_allocate (sizeof (struct meltstring_st),
			slen + 1);
  str_strv->discr = obj_discrv;
  strcpy (str_strv->val, tempnampath);
end:
  if (tempnampath)
    free (CONST_CAST (char*,tempnampath));
  MELT_EXITFRAME ();
  return (melt_ptr_t) strv;
#undef discrv
#undef strv
#undef obj_discrv
#undef str_strv
}




#if ENABLE_CHECKING
static long applcount_melt;
static int appldepth_melt;
#define MAXDEPTH_APPLY_MELT 2000
#endif
/*************** closure application ********************/
melt_ptr_t
melt_apply (meltclosure_ptr_t clos_p,
	       melt_ptr_t arg1_p,
	       const char *xargdescr_,
	       union meltparam_un *xargtab_,
	       const char *xresdescr_, union meltparam_un *xrestab_)
{
  melt_ptr_t res = NULL;
  meltroutfun_t*routfun = 0;
#if ENABLE_CHECKING
  applcount_melt++;
  appldepth_melt++;
  if (appldepth_melt > MAXDEPTH_APPLY_MELT)
    {
      melt_dbgshortbacktrace ("too deep applications", 200);
      fatal_error ("too deep (%d) melt applications", appldepth_melt);
    }
#endif
  if (melt_magic_discr ((melt_ptr_t) clos_p) != OBMAG_CLOSURE)
    goto end;
  if (melt_magic_discr ((melt_ptr_t) (clos_p->rout)) !=
      OBMAG_ROUTINE || !(routfun = clos_p->rout->routfunad))
    goto end;
  res = (*routfun) (clos_p, arg1_p, xargdescr_, xargtab_, xresdescr_, xrestab_);
 end:
#if ENABLE_CHECKING
  appldepth_melt--;
#endif
  return res;
}



/************** method sending ***************/
melt_ptr_t
meltgc_send (melt_ptr_t recv_p,
		melt_ptr_t sel_p,
		const char *xargdescr_,
		union meltparam_un * xargtab_,
		const char *xresdescr_, union meltparam_un * xrestab_)
{
  /* NAUGHTY TRICK here: message sending is very common, and we want
     to avoid having the current frame (the frame declared by the
     B*ENTERFRAME macro call below) to be active when the application
     for the sending is performed. This should make our call frames'
     linked list shorter. To do so, we put the closure to apply and
     the reciever in the two variables below. Yes this is dirty, but
     it works! 

     We should be very careful when modifying this routine */
  /* never assign to these if a GC could happen */
  meltclosure_ptr_t closure_dirtyptr = NULL;
  melt_ptr_t recv_dirtyptr = NULL;

#ifdef ENABLE_CHECKING
  static long sendcount;
  long sendnum = ++sendcount;
#endif
  MELT_ENTERFRAME (9, NULL);
#define recv    curfram__.varptr[0]
#define selv    curfram__.varptr[1]
#define argv    curfram__.varptr[2]
#define closv   curfram__.varptr[3]
#define discrv  curfram__.varptr[4]
#define mapv    curfram__.varptr[5]
#define superv  curfram__.varptr[6]
#define resv    curfram__.varptr[7]
#define ancv    curfram__.varptr[8]
#define obj_discrv ((meltobject_ptr_t)(discrv))
#define obj_selv ((meltobject_ptr_t)(selv))
#define clo_closv ((meltclosure_ptr_t)(closv))
#define mul_ancv  ((struct meltmultiple_st*)(ancv))
  recv = recv_p;
  selv = sel_p;
  /* the reciever can be null, using DISCR_NULLRECV */
#ifdef ENABLE_CHECKING
  (void) sendnum;		/* to use it */
#endif
  if (melt_magic_discr ((melt_ptr_t) selv) != OBMAG_OBJECT)
    goto end;
  if (!melt_is_instance_of
      ((melt_ptr_t) selv, (melt_ptr_t) MELT_PREDEF (CLASS_SELECTOR)))
    goto end;
#if 0 && ENABLE_CHECKING
  debugeprintf ("send #%ld recv %p", sendnum, (void *) recv);
  debugeprintf ("send #%ld selv %p <%s>", sendnum,
		(void *) obj_selv,
		melt_string_str (obj_selv->obj_vartab[FNAMED_NAME]));
#endif
  if (recv != NULL)
    {
      discrv = ((melt_ptr_t) recv)->u_discr;
      gcc_assert (discrv != NULL);
    }
  else
    {
      discrv = ((meltobject_ptr_t) MELT_PREDEF (DISCR_NULLRECV));
      gcc_assert (discrv != NULL);
    };
  while (discrv)
    {
      gcc_assert (melt_magic_discr ((melt_ptr_t) discrv) ==
		  OBMAG_OBJECT);
      gcc_assert (obj_discrv->obj_len >= FDISCR__LAST);
#if 0 && ENABLE_CHECKING
      debugeprintf ("send #%ld discrv %p <%s>",
		    sendnum, discrv,
		    melt_string_str (obj_discrv->obj_vartab[FNAMED_NAME]));
#endif
      mapv = obj_discrv->obj_vartab[FDISCR_METHODICT];
      if (melt_magic_discr ((melt_ptr_t) mapv) == OBMAG_MAPOBJECTS)
	{
	  closv =
	    (melt_ptr_t) melt_get_mapobjects ((meltmapobjects_ptr_t)
						    mapv,
						    (meltobject_ptr_t)
						    selv);
	}
      else
	{
	  closv = obj_discrv->obj_vartab[FDISCR_SENDER];
	  if (melt_magic_discr ((melt_ptr_t) closv) == OBMAG_CLOSURE)
	    {
	      union meltparam_un pararg[1];
	      pararg[0].bp_aptr = (melt_ptr_t *) & selv;
	      resv =
		melt_apply ((meltclosure_ptr_t) closv,
			       (melt_ptr_t) recv, BPARSTR_PTR, pararg, "",
			       NULL);
	      closv = resv;
	    }
	}
      if (melt_magic_discr ((melt_ptr_t) closv) == OBMAG_CLOSURE)
	{
	  /* NAUGHTY TRICK: assign to dirty (see comments near start of function) */
	  closure_dirtyptr = (meltclosure_ptr_t) closv;
	  recv_dirtyptr = (melt_ptr_t) recv;
	  /*** OLD CODE: 
	  resv =
	    melt_apply (closv, recv, xargdescr_, xargtab_,
			     xresdescr_, xrestab_);
	  ***/
	  goto end;
	}
      discrv = obj_discrv->obj_vartab[FDISCR_SUPER];
    }				/* end while discrv */
  resv = NULL;
end:
#if 0 && ENABLE_CHECKING
  debugeprintf ("endsend #%ld recv %p resv %p selv %p <%s>",
		sendnum, recv, resv, (void *) obj_selv,
		melt_string_str (obj_selv->obj_vartab[FNAMED_NAME]));
#endif
  MELT_EXITFRAME ();
  /* NAUGHTY TRICK  (see comments near start of function) */
  if (closure_dirtyptr)
    return melt_apply (closure_dirtyptr, recv_dirtyptr, xargdescr_,
			  xargtab_, xresdescr_, xrestab_);
  return (melt_ptr_t) resv;
#undef recv
#undef selv
#undef closv
#undef discrv
#undef argv
#undef mapv
#undef superv
#undef resv
#undef ancv
#undef obj_discrv
#undef obj_selv
#undef clo_closv
}


static inline melt_ptr_t
melt_get_inisysdata(int off)
{
  meltobject_ptr_t inisys = (meltobject_ptr_t) MELT_PREDEF(INITIAL_SYSTEM_DATA);
  if (melt_magic_discr ((melt_ptr_t) inisys) == OBMAG_OBJECT) 
    {
      int leninisys = inisys->obj_len;
      gcc_assert(melt_is_instance_of
		 ((melt_ptr_t) inisys,
		  (melt_ptr_t) MELT_PREDEF (CLASS_SYSTEM_DATA)));
      if (off>=0 && off<leninisys)
	return inisys->obj_vartab[off];
    }
  return NULL;  
  
}


/* our temporary directory */
/* maybe it should not be static, or have a bigger length */
static char tempdir_melt[1024];
static bool made_tempdir_melt;
/* returns malloc-ed path inside a temporary directory, with a given basename  */
char *
melt_tempdir_path (const char *srcnam, const char* suffix)
{
  int loopcnt = 0;
  const char *basnam = 0;
  static const char* tmpdirstr = 0;
  extern char *choose_tmpdir (void);	/* from libiberty/choose-temp.c */
  basnam = lbasename (CONST_CAST (char*,srcnam));
  debugeprintf ("melt_tempdir_path srcnam %s basnam %s suffix %s", srcnam, basnam, suffix);
  if (!tmpdirstr)
    tmpdirstr = melt_argument ("tempdir");
  gcc_assert (basnam && (ISALNUM (basnam[0]) || basnam[0] == '_'));
  if (tmpdirstr && tmpdirstr[0])
    {
      if (access (tmpdirstr, F_OK))
	{
	  if (mkdir (tmpdirstr, 0700))
	    fatal_error ("failed to mkdir melt_tempdir %s - %m",
			 tmpdirstr);
	  made_tempdir_melt = true;
	}
      return concat (tmpdirstr, "/", basnam, suffix, NULL);
    }
  if (!tempdir_melt[0])
    {
      /* usually this loop runs only once */
      for (loopcnt = 0; loopcnt < 1000; loopcnt++)
	{
	  int n = melt_lrand () & 0xfffffff;
	  memset(tempdir_melt, 0, sizeof(tempdir_melt));
#ifdef MELT_IS_PLUGIN
	  /* we don't have choose_tmpdir in plugin mode because it is
	     in libiberty */
	  snprintf (tempdir_melt, sizeof(tempdir_melt)-1,
		    "%s-GccMelt%d-%d-d",
		    tmpnam(NULL), (int)getpid (), n);
#else
	  snprintf (tempdir_melt, sizeof(tempdir_melt)-1, 
		     "%s/GccMeltmpd%d-%d", choose_tmpdir (), (int) getpid (),
		    n);
#endif
	  if (!mkdir (tempdir_melt, 0700))
	    {
	      made_tempdir_melt = true;
	      break;
	    }
	  tempdir_melt[0] = '\0';
	}
      if (!tempdir_melt[0])
	fatal_error ("failed to create temporary directory for MELT");
    };
  return concat (tempdir_melt, "/", basnam, suffix, NULL);
}





/* the srcfile is a generated .c file or otherwise a dynamic library,
   the dlfile has no suffix, because the suffix is expected to be
   added by the melt-gcc script */
static void
compile_to_dyl (const char *srcfile, const char *dlfile)
{
  /* possible improvement  : avoid recompiling
     when not necessary; using timestamps a la make is not enough,
     since the C source files are generated.  

     The melt-cc-script takes two arguments: the C source file path to
     compile as a melt plugin, and the naked dynamic library file to be
     generated. Standard path are in Makefile.in $(melt_compile_script)

     The melt-cc-script should be generated in the building process.
     In addition of compiling the C source file, it should put into the
     generated dynamic library the following two constant strings;
     const char melt_compiled_timestamp[];
     const char melt_md5[];

     the melt_compiled_timestamp should contain a human readable
     timestamp the melt_md5 should contain the hexadecimal md5 digest,
     followed by the source file name (i.e. the single line output by the
     command: md5sum $Csourcefile; where $Csourcefile is replaced by the
     source file path)

  */
  const char *ourmeltcompilescript = NULL;
  const char* compscrstr = melt_argument ("compile-script");
  char pwdbuf[500];
  memset(pwdbuf, 0, sizeof(pwdbuf));
  getcwd(pwdbuf, sizeof(pwdbuf)-1);
  ourmeltcompilescript = CONST_CAST (char *, compscrstr);
  if (!ourmeltcompilescript || !ourmeltcompilescript[0])
    ourmeltcompilescript = melt_compile_script;
  debugeprintf ("compile_to_dyl compscrstr %s", compscrstr);
  debugeprintf ("compile_to_dyl melt ourmeltcompilescript=%s pwd %s",
		ourmeltcompilescript, pwdbuf);
  debugeprintf ("compile_to_dyl srcfile %s dlfile %s", srcfile, dlfile);
  fflush (stdout);
  fflush (stderr);
#ifdef MELT_IS_PLUGIN
  {
    int err = 0;
    char * cmd = concat(ourmeltcompilescript, 
#if ENABLE_CHECKING
			flag_melt_debug?" -x ":" ",
#else
			" ",
#endif
			srcfile, " ",
			dlfile, NULL);
    gcc_assert (cmd != NULL);
    debugeprintf("compile_to_dyl cmd: %s", cmd);
    err = system (cmd);
    if (err) 
      fatal_error
	("failed to melt compile to dyl with command: %s : in %s, error code %d", cmd, pwdbuf, err);
    free (cmd);
    return;
  } 
#else
  {
    int argc = 0;
    int err = 0;
    int cstatus = 0;
    const char *errmsg = 0;
    const char *argv[6] = { 0,0,0,0,0,0 };
    struct pex_obj *pex = 0;
    struct pex_time ptime;
    memset (&ptime, 0, sizeof (ptime));
    /* compute the ourmeltcompilscript */
    pex = pex_init (PEX_RECORD_TIMES, ourmeltcompilescript, NULL);
    argv[argc++] = ourmeltcompilescript;
    debugeprintf("compile_to_dyl adding %s", (argv[argc++]="-x"));
    argv[argc++] = srcfile;
    argv[argc++] = dlfile;
    argv[argc] = NULL;
    debugeprintf("compile_to_dyl pex: %s %s %s", ourmeltcompilescript, srcfile, dlfile);
    errmsg =
      pex_run (pex, PEX_LAST | PEX_SEARCH, ourmeltcompilescript, 
	       CONST_CAST (char**, argv),
	       NULL, NULL, &err);
    if (errmsg)
      fatal_error
	("failed to melt compile to dyl: %s %s %s : %s",
	 ourmeltcompilescript, srcfile, dlfile, errmsg);
    if (!pex_get_status (pex, 1, &cstatus))
      fatal_error
	("failed to get status of melt dynamic compilation to dyl:  %s %s %s - %m",
	 ourmeltcompilescript, srcfile, dlfile);
    if (!pex_get_times (pex, 1, &ptime))
      fatal_error
	("failed to get time of melt dynamic compilation to dyl:  %s %s %s - %m",
	 ourmeltcompilescript, srcfile, dlfile);
    pex_free (pex);
  }
#endif
  debugeprintf ("compile_to_dyl done srcfile %s dlfile %s", srcfile, dlfile);
}



/* following code and comment is taken from the gcc/plugin.c file of
   the plugins branch */

/* We need a union to cast dlsym return value to a function pointer
   as ISO C forbids assignment between function pointer and 'void *'.
   Use explicit union instead of __extension__(<union_cast>) for
   portability.  */
#define PTR_UNION_TYPE(TOTYPE) union { void *_q; TOTYPE _nq; }
#define PTR_UNION_AS_VOID_PTR(NAME) (NAME._q)
#define PTR_UNION_AS_CAST_PTR(NAME) (NAME._nq)




/* load a dynamic library using the filepath DYPATH; if MD5SRC is
   given, check that the melt_md5 inside is indeed MD5SRC, fill the
   info of the modulinfo stack and return the positive index,
   otherwise return 0 */
static int
load_checked_dynamic_module_index (const char *dypath, char *md5src)
{
  int ix = 0;
  char *dynmd5 = NULL;
  char *dynchecksum = NULL;
  void *dlh = NULL;
  char *dyncomptimstamp = NULL;
  typedef melt_ptr_t startroutine_t (melt_ptr_t);
  typedef void markroutine_t (void *);
  PTR_UNION_TYPE(startroutine_t*) startrout_uf = {0};
  PTR_UNION_TYPE(markroutine_t*) markrout_uf = {0};
  PTR_UNION_TYPE(void **) iniframe_up = {0};
  int i = 0, c = 0;
  char hbuf[4];
  debugeprintf ("load_check_dynamic_module_index dypath=%s md5src=%s", dypath, md5src);
  dlh = (void *) dlopen (dypath, RTLD_NOW | RTLD_GLOBAL);
  debugeprintf ("load_check_dynamic_module_index dlh=%p dypath=%s", dlh, dypath);
  if (!dlh) 
    {
      debugeprintf("load_check_dynamic_module_index dlerror %s", dlerror());
      return 0;
    };
  /* we always check that a melt_md5 exists within the dynamically
     loaded stuff; otherwise it was not generated from MELT/melt */
  dynmd5 = (char *) dlsym ((void *) dlh, "melt_md5");
  if (!dynmd5)
    dynmd5 = (char *) dlsym ((void *) dlh, "melt_md5");
  debugeprintf ("dynmd5=%s", dynmd5);
  if (!dynmd5) 
    {
      warning (0, "missing md5 signature in MELT module %s", dypath);
      goto bad;
  }
  dyncomptimstamp =
    (char *) dlsym ((void *) dlh, "melt_compiled_timestamp");
  if (!dyncomptimstamp)
    dyncomptimstamp =
      (char *) dlsym ((void *) dlh, "melt_compiled_timestamp");
  debugeprintf ("dyncomptimstamp=%s", dyncomptimstamp);
  if (!dyncomptimstamp) 
    {
      warning (0, "missing timestamp in MELT module %s", dypath);
      goto bad;
    };
  /* check the checksum of the generating compiler with current */
  dynchecksum  =
    (char *) dlsym ((void *) dlh, "genchecksum_melt");
  if (dynchecksum && memcmp(dynchecksum, executable_checksum, 16))
    warning(0, "loaded MELT plugin %s with a checksum mismatch!", dypath);
  PTR_UNION_AS_VOID_PTR(startrout_uf) =
    dlsym ((void *) dlh, "start_module_melt");
  if (!PTR_UNION_AS_VOID_PTR(startrout_uf))
    PTR_UNION_AS_VOID_PTR(startrout_uf) 
      = dlsym ((void *) dlh, "start_module_melt");
  if (!PTR_UNION_AS_VOID_PTR(startrout_uf)) 
    {
      warning (0, "missing start_module_melt routine in MELT module %s", dypath);
      goto bad;
    };
  PTR_UNION_AS_VOID_PTR(markrout_uf) =
    dlsym ((void *) dlh, "mark_module_melt");
  if (!PTR_UNION_AS_VOID_PTR(markrout_uf))
   PTR_UNION_AS_VOID_PTR(markrout_uf) =
     dlsym ((void *) dlh, "mark_module_melt");
  if (!PTR_UNION_AS_VOID_PTR(markrout_uf))
    {
      warning (0, "missing mark_module_melt routine in MELT module %s", dypath);
      goto bad;
    };
  PTR_UNION_AS_VOID_PTR(iniframe_up) 
    = dlsym ((void *) dlh, "initial_frame_melt");
  if (!PTR_UNION_AS_VOID_PTR(iniframe_up) )
    PTR_UNION_AS_VOID_PTR(iniframe_up) =
      dlsym ((void *) dlh, "initial_frame_melt");
  if (!PTR_UNION_AS_VOID_PTR(iniframe_up))
    {
      warning (0, "missing initial_frame_melt routine in MELT module %s", dypath);
      goto bad;
    }
  if (md5src && dynmd5)
    {
      for (i = 0; i < 16; i++)
	{
	  if (ISXDIGIT (dynmd5[2 * i]) && ISXDIGIT (dynmd5[2 * i + 1]))
	    {
	      hbuf[0] = dynmd5[2 * i];
	      hbuf[1] = dynmd5[2 * i + 1];
	      hbuf[2] = (char) 0;
	      c = (int) strtol (hbuf, (char **) 0, 16);
	      if (c != (int) (md5src[i] & 0xff))
		{
		  char hexmd5src[40];
		  int j;
		  memset (hexmd5src, 0, sizeof (hexmd5src));
		  for (j = 0; j < 16; j++) 
		    {
		      hexmd5src[2*j] = "0123456789abcdef" [(md5src[j]>>4)&0xf];
		      hexmd5src[2*j+1] = "0123456789abcdef" [md5src[j]&0xf];
		    }
		  debugeprintf("mismatched md5src=%#x hexmd5src=%s",
			       *(int*)md5src, hexmd5src);
		  warning (0, "md5 source mismatch in MELT module %s", dypath);
		  inform (UNKNOWN_LOCATION, "recomputed md5 of MELT C code is %s", hexmd5src);
		  inform (UNKNOWN_LOCATION, "MELT module contains registered md5sum %s", dynmd5);
		  goto bad;
		}
	    }
	  else
	    {
	      warning (0, "md5 source invalid in MELT module %s", dypath);
	      goto bad;
	    }
	}
    }
  {
    melt_module_info_t minf = { 0, 0, 0, 0 };
    minf.dlh = dlh;
    minf.iniframp = PTR_UNION_AS_CAST_PTR (iniframe_up);
    minf.marker_rout = PTR_UNION_AS_CAST_PTR (markrout_uf);
    minf.start_rout = PTR_UNION_AS_CAST_PTR (startrout_uf);
    ix = VEC_length (melt_module_info_t, modinfvec);
    VEC_safe_push (melt_module_info_t, heap, modinfvec, &minf);
  }
  debugeprintf
    ("load_checked_dynamic_module_index %s dynmd5 %s dyncomptimstamp %s ix %d",
     dypath, dynmd5, dyncomptimstamp, ix);
  return ix;
bad:
  debugeprintf ("load_checked_dynamic_module_index failed dlerror:%s",
		dlerror ());
  dlclose ((void *) dlh);
  return 0;
}

void *
melt_dlsym_all (const char *nam)
{
  int ix = 0;
  melt_module_info_t *mi = 0;
  for (ix = 0; VEC_iterate (melt_module_info_t, modinfvec, ix, mi); ix++)
    {
      void *p = (void *) dlsym ((void *) mi->dlh, nam);
      if (p)
	return p;
    };
  return (void *) dlsym (proghandle, nam);
}



/* lookup inside a colon-separated PATH for a file of given base and
   suffix; return the malloc-ed full file path if found */
static char*
lookup_path(const char*path, const char* base, const char* suffix)
{
  const char* pc = NULL;
  const char* col = NULL;
  char* dir = NULL;
  char* filnam = NULL;
  size_t dirnamlen = 0;  
  debugeprintf ("start lookup_path path=%s base=%s suffix=%s", path, base, suffix);
  if (!path || !base) 
    return NULL;
  pc = path;
  do {
    col = strchr(pc, ':');
    if (!col) 
      col = pc + strlen(pc);
    dirnamlen = col - pc;
    dir = xstrndup (pc, dirnamlen);
    if (dir && *dir)
      {
	filnam = concat(dir, "/", base, suffix, NULL);
	free (dir);
	dir = NULL;
      }
    else 
      filnam = concat("./", base, suffix, NULL);
    if (!access (filnam, R_OK))
      {
	debugeprintf("lookup_path found filnam %s", filnam);
	return filnam;
      }
    free (filnam);
    filnam = NULL;
    if (*col == ':') 
      pc=col+1;
    else
      break;
  } while (pc && *pc);
  debugeprintf("lookup_path not found base %s suffix %s", base, suffix);
  return NULL;
}

/* compile (as a dynamically loadable module) some (usually generated)
   C code (or a dynamically loaded stuff) and dynamically load it; the
   C code should contain a function named start_module_melt; that
   function is called with the given modata and returns the module;
   the modulnam should contain only letter, digits or one of +-_ */
melt_ptr_t
meltgc_load_melt_module (melt_ptr_t modata_p, const char *modulnam)
{
  char *srcpath = NULL;
  char *dynpath = NULL;
  FILE *srcfi = NULL;
  FILE *oldf = NULL;
  int dlix = 0;
  int specialsuffix = 0; /* set for .d or .n suffix */
  int srcpathlen = 0;
  char md5srctab[16];
  char *md5src = NULL;
  char *tmpath = NULL;
  char *dupmodulnam = NULL;
  char *envpath = NULL;
  melt_module_info_t *moduptr = 0;
  melt_ptr_t (*startroutp) (melt_ptr_t);	/* start routine */
  int modulnamlen = 0;
  const char* srcpathstr = melt_argument ("source-path");
  const char* modpathstr = melt_argument ("module-path");
  MELT_ENTERFRAME (4, NULL);
#define modulv curfram__.varptr[0]
#define mdatav curfram__.varptr[1]
#define dumpv  curfram__.varptr[2]
  mdatav = modata_p;
  if (!modulnam || !modulnam[0]) {
    error ("cannot load MELT module, no MELT module name given");
    goto end;
  }
  debugeprintf("meltgc_load_melt_module modulnam %s", modulnam);
  if (!ISALNUM(modulnam[0]) && modulnam[0] != '_')
    error ("bad MELT module name %s to load", modulnam);
  /* always check the module name */
  {
    const char* p = 0;
    for (p=modulnam; *p; p++)
      {
	/* special hack for names like warmelt-first-0.d or
	   warmelt-foo-1.n */
	if (p[0] == '.' && ISALNUM(p[1]) && !p[2])
	  {
	    specialsuffix = 1;
	    break;
	  };
	if (!ISALNUM(*p) && *p != '+' && *p != '-' && *p != '_')
	  {
	    error ("invalid MELT module name %s to load", modulnam);
	    goto end;
	  }
      }
  }
  /* duplicate the module name for safety, i.e. because it was in MELT
     heap or whatever ... */
  dupmodulnam = xstrdup(modulnam);
  if (specialsuffix) {
    modulnamlen = strlen(modulnam);
    gcc_assert (modulnamlen>2);
    gcc_assert (dupmodulnam[modulnamlen-2] == '.');
    dupmodulnam[modulnamlen-2] = '\0';
  }
  /***** first find the source path if possible ******/
  /* look first in the temporary directory */
  tmpath = melt_tempdir_path (dupmodulnam, ".c");
  debugeprintf ("meltgc_load_melt_module trying in tempdir %s", tmpath);
  if (tmpath && !access (tmpath, R_OK))
    {
      debugeprintf ("meltgc_load_melt_module found source in tempdir %s", tmpath);
      srcpath = tmpath;
      goto foundsrcpath;
    }
  free(tmpath);
  tmpath = NULL;
  /* look in the source path if given */
  if (srcpathstr && srcpathstr[0]) {
    debugeprintf("meltgc_load_melt_module trying in MELT srcpath %s", srcpathstr);
    tmpath = lookup_path (srcpathstr, dupmodulnam, ".c");
    debugeprintf("meltgc_load_melt_module got in MELT srcpath %s", tmpath);
    if (tmpath) {
      srcpath = tmpath;
      goto foundsrcpath;
    }
  }
  else 
    envpath = getenv("GCCMELT_SOURCE_PATH");
  /* look into the GCCMELT_SOURCE_PATH environment variable if no
     source path was given */
  if (envpath && *envpath) 
    {
      debugeprintf("meltgc_load_melt_module trying in GCCMELT_SOURCE_PATH %s",
		   envpath);
      tmpath = lookup_path (envpath, dupmodulnam, ".c");
      debugeprintf("meltgc_load_melt_module got in  GCCMELT_SOURCE_PATH %s", tmpath);
      if (tmpath) {
	srcpath = tmpath;
	goto foundsrcpath;
      }
    }
  /* perhaps use make_relative_prefix  for the melt source directory ... */
  /* look into the melt source dir */
  tmpath = concat (melt_source_dir, "/", dupmodulnam, ".c", NULL);
  debugeprintf ("meltgc_load_melt_module trying in meltsrcdir %s", tmpath);
  if (tmpath && !access (tmpath, R_OK))
    {
      debugeprintf ("meltgc_load_melt_module found source in meltsrcdir %s", tmpath);
      srcpath = tmpath;
      goto foundsrcpath;
    }
  free (tmpath);
  tmpath = NULL;
  /* we didn't found the source */
  debugeprintf ("meltgc_load_melt_module cannot find source for mudule %s", dupmodulnam);
  warning (0, "Didn't find MELT module %s 's C source code; perhaps need -fmelt-srcpath=...", dupmodulnam);
  inform (UNKNOWN_LOCATION, "MELT temporary source path tried %s for C source code", 
	  melt_tempdir_path (dupmodulnam, ".c"));
  {
    /* explain only once the path we searched the C source code in */
    static int nbexplain;
    if (nbexplain <= 0) 
      {
	if (srcpathstr && srcpathstr[0])
	  inform (UNKNOWN_LOCATION, "MELT given source path is %s",
		  srcpathstr);
	if (envpath && envpath[0])
	  inform (UNKNOWN_LOCATION, "MELT environment GCCMELT_SOURCE_PATH is %s", 
		  envpath);
	inform (UNKNOWN_LOCATION, "MELT builtin source directory is %s", 
		melt_source_dir);
      };
    nbexplain++;
  }
  if (srcpath) 
    {
    foundsrcpath:  /* we found the source file */
      srcpathlen = strlen (srcpath);
      debugeprintf ("meltgc_load_melt_module found srcpathlen %d srcpath %s", srcpathlen, srcpath);
      /* compute the md5 hash of the source code */
      srcfi = fopen (srcpath, "r");
      if (!srcfi)
	/* this really should not happen, we checked with access before
	   that the source file existed! */
	fatal_error ("cannot open generated source file %s for MELT : %m",
		     srcpath);
      memset (md5srctab, 0, sizeof (md5srctab));
      if (md5_stream (srcfi, &md5srctab))
	fatal_error
	  ("failed to compute md5sum of generated source file %s for MELT",
	   srcpath);
      md5src = md5srctab;
      fclose (srcfi);
      srcfi = NULL;
    }
  /* if it had a special suffix, restore it */
  if (specialsuffix)
    {
      gcc_assert (modulnamlen>2);
      dupmodulnam[modulnamlen-2] = '.';
      debugeprintf ("meltgc_load_melt_module restored dupmodulnam %s", 
		    dupmodulnam);
    }
  /**
     We have to scan several dynlib directories to find the module;
     when we find a module, we dynamically load it to check that it
     has the right md5 sum (i.e. that its melt_md5 is correct); if no
     dynlib is found, we have to compile the generated C source.  We
     should really scan the module path incrementally, i.e. testing
     the foo.so file in every element of the path. We don't do that yet.
  **/
  /* if a dynlib directory is given, check it */
  if (modpathstr && modpathstr[0])
    {
      tmpath = lookup_path (modpathstr, dupmodulnam, ".so");
      MELT_LOCATION_HERE
	("meltgc_load_melt_module before load_checked_dylib pathed");
      dlix = load_checked_dynamic_module_index (tmpath, md5src);
      debugeprintf ("meltgc_load_melt_module dlix=%d dynlib tmpath=%s", dlix,
		    tmpath);
      if (dlix > 0)
	{
	  dynpath = tmpath;
	  goto dylibfound;
	};
      free (tmpath);
      tmpath = NULL;
    }
  /* check in the builtin melt module directory */
  tmpath = concat (melt_module_dir, "/", dupmodulnam, NULL);
  MELT_LOCATION_HERE
    ("meltgc_load_melt_module before load_checked_dylib builtin");
  dlix = load_checked_dynamic_module_index (tmpath, md5src);
  debugeprintf ("meltgc_load_melt_module dlix=%d meltdynlib tmpath=%s", dlix,
		tmpath);
  if (dlix > 0)
    {
      dynpath = tmpath;
      goto dylibfound;
    };
  free (tmpath);
  tmpath = NULL;
  /* check in the temporary directory */
  tmpath = melt_tempdir_path (dupmodulnam, ".so");
  debugeprintf ("meltgc_load_melt_module trying %s", tmpath);
  MELT_LOCATION_HERE
    ("meltgc_load_melt_module before load_checked_dylib tmpath");
  dlix = tmpath ? load_checked_dynamic_module_index (tmpath, md5src) : 0;
  debugeprintf ("meltgc_load_melt_module dlix=%d tempdir tmpath=%s", dlix,
		tmpath);
  if (dlix > 0)
    {
      dynpath = tmpath;
      goto dylibfound;
    };
  free (tmpath);
  tmpath = NULL;
  debugeprintf ("meltgc_load_melt_module md5src %p", (void*)md5src);
  /* if we really have the source, we can afford to check in the current directory */
  if (md5src)
    {
      tmpath = concat ("./", dupmodulnam, ".so", NULL);
      debugeprintf ("meltgc_load_melt_module tmpath %s", tmpath);
      MELT_LOCATION_HERE
	("meltgc_load_melt_module before load_checked_dylib src");
      dlix = load_checked_dynamic_module_index (tmpath, md5src);
      debugeprintf ("meltgc_load_melt_module dlix=%d curdir tmpath=%s", dlix,
		    tmpath);
      if (dlix > 0)
	{
	  dynpath = tmpath;
	  goto dylibfound;
	};
      free (tmpath);
    }
  debugeprintf ("meltgc_load_melt_module srcpath %s dynpath %s", srcpath, dynpath);
  /* if we have the srcpath but did'nt found the stuff, try to compile it using the temporary directory */
  if (srcpath)
    {
      tmpath = melt_tempdir_path (dupmodulnam, ".so");
      debugeprintf ("meltgc_load_melt_module before compiling tmpath %s", tmpath);
      compile_to_dyl (srcpath, tmpath);
      debugeprintf ("meltgc_compile srcpath=%s compiled to tmpath=%s",
		    srcpath, tmpath);
      MELT_LOCATION_HERE
	("meltgc_load_melt_module before load_checked_dylib compiled tmpath");
      dlix = load_checked_dynamic_module_index (tmpath, md5src);
      debugeprintf ("meltgc_load_melt_module dlix=%d tempdirpath tmpath=%s",
		    dlix, tmpath);
      if (dlix > 0)
	{
	  dynpath = tmpath;
	  goto dylibfound;
	}
      else
	error ("compilation & loading of MELT generated file %s to temporary module %s failed",
	       srcpath, tmpath);
    }
  debugeprintf ("failed here dlix=%d", dlix);
  /* catch all situation, failed to find the dynamic stuff */
  /* give info to user */
  error("failed to find dynamic stuff for MELT module %s (%s)",
	dupmodulnam, dlerror ());
  inform (UNKNOWN_LOCATION, 
	  "not found dynamic stuff using tempdir %s", 
	  melt_tempdir_path (dupmodulnam, NULL));
  if (srcpath)
    inform (UNKNOWN_LOCATION, 
	    "not found dynamic stuff using srcpath %s", 
	    srcpath);
  fatal_error ("unable to continue since failed to load MELT module %s", 
	       dupmodulnam);
 dylibfound:
  debugeprintf ("dylibfound dlix=%d", dlix);
  gcc_assert (dlix > 0
	      && dlix < (int) VEC_length (melt_module_info_t, modinfvec));
  moduptr = VEC_index (melt_module_info_t, modinfvec, dlix);
  debugeprintf ("dylibfound moduptr=%p", (void *) moduptr);
  gcc_assert (moduptr != 0);
  startroutp = moduptr->start_rout;
  gcc_assert (moduptr->iniframp != 0 && *moduptr->iniframp == (void *) 0);
  dumpv = melt_get_inisysdata (FSYSDAT_DUMPFILE);
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      oldf = ((struct meltspecial_st*)dumpv)->val.sp_file;
      ((struct meltspecial_st*)dumpv)->val.sp_file = dump_file;
    }
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_load_melt_module before calling module %s",
	      basename (__FILE__), __LINE__, dupmodulnam);
    curfram__.flocs = locbuf;
  }
#endif
  modulv = (*startroutp) ((melt_ptr_t) mdatav);
  gcc_assert (moduptr->iniframp != 0 && *moduptr->iniframp == (void *) 0);
  MELT_LOCATION_HERE ("meltgc_load_melt_module after calling module");
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      FILE *df = melt_get_file ((melt_ptr_t) dumpv);
      if (df)
	fflush (df);
      ((struct meltspecial_st*)dumpv)->val.sp_file = oldf;
    };
 end:
  debugeprintf ("meltgc_load_melt_module returns modulv %p", (void *) modulv);
  /* we never free dynpath -since it is stored in moduptr- and we
     never release the shared library with a dlclose or something! */
  MELT_EXITFRAME ();
  free(dupmodulnam);
  return (melt_ptr_t) modulv;
#undef mdatav
#undef modulv
#undef dumpv
}


/* generate a loadable module from a MELT generated C source file; the
   out is the dynloaded module without any *.so suffix */
void
meltgc_generate_melt_module (melt_ptr_t src_p, melt_ptr_t out_p)
{
  char*srcdup = NULL;
  char* outdup = NULL;
  char* outso = NULL;
  MELT_ENTERFRAME (2, NULL);
#define srcv   curfram__.varptr[0]
#define outv      curfram__.varptr[1]
  srcv = src_p;
  outv = out_p;
  if (melt_magic_discr((melt_ptr_t) srcv) != OBMAG_STRING 
      || melt_magic_discr((melt_ptr_t) outv) != OBMAG_STRING)
    goto end;
  srcdup = xstrdup(melt_string_str((melt_ptr_t) srcv));
  outdup = xstrdup(melt_string_str((melt_ptr_t) outv));
  outso = concat(outdup, ".so", NULL);
  (void) remove (outso);
  debugeprintf("meltgc_generate_melt_module start srcdup %s outdup %s",
	       srcdup, outdup);
  if (access(srcdup, R_OK)) 
    {
      error("no MELT generated source file %s", srcdup);
      goto end;
    }
  compile_to_dyl (srcdup, outdup);
  debugeprintf ("meltgc_generate_module did srcdup %s outdup %s",
	       srcdup, outdup);
  if (!access(outso, R_OK))
    inform (UNKNOWN_LOCATION, "MELT generated module %s", outso);
 end:
  free (srcdup);
  free (outdup);
  free (outso);
  MELT_EXITFRAME ();
#undef srcv
#undef outv
}


#define MODLIS_SUFFIX ".modlis"

melt_ptr_t
meltgc_load_modulelist (melt_ptr_t modata_p, const char *modlistbase)
{
  char *modlistpath = 0;
  char* envpath = 0;
  FILE *filmod = 0;
  const char* srcpathstr = melt_argument ("source-path");
  const char* modpathstr = melt_argument ("module-path");
  /* @@@ ugly, we should have a getline function */
  char linbuf[1024];
  MELT_ENTERFRAME (1, NULL);
  memset (linbuf, 0, sizeof (linbuf));
#define mdatav curfram__.varptr[0]
  mdatav = modata_p;
  debugeprintf ("meltgc_load_modulelist start modlistbase %s", modlistbase);
  /* first check directly for the file */
  modlistpath = concat (modlistbase, MODLIS_SUFFIX, NULL);
  if (IS_ABSOLUTE_PATH (modlistpath) || !access (modlistpath, R_OK))
    goto loadit;
  free (modlistpath);
  modlistpath = 0;
  /* check for module list in given melt source path */
  if (srcpathstr && srcpathstr[0]) 
    {
      modlistpath = lookup_path(srcpathstr, modlistbase, MODLIS_SUFFIX);
      if (modlistpath)
	goto loadit;
    }
  else
    envpath = getenv("GCCMELT_SOURCE_PATH");
  /* check for module list in $GCCMELT_SOURCE_PATH */
  if (envpath && envpath[0]) 
    {
      modlistpath = lookup_path(envpath, modlistbase, MODLIS_SUFFIX);
      if (modlistpath)
	goto loadit;
    }
  envpath = NULL;
  /* check for module list in builtin melt_source_dir */
  modlistpath = concat (melt_source_dir,
			"/", modlistbase, MODLIS_SUFFIX, NULL);
  if (!access (modlistpath, R_OK))
    goto loadit;
  free (modlistpath);
  modlistpath = 0;
  /* check for module list in module path */
  if (modpathstr && modpathstr[0])
    {
      modlistpath = lookup_path (modpathstr, modlistbase, MODLIS_SUFFIX);
      if (modlistpath)
	goto loadit;
    }
  else
    envpath = getenv("GCCMELT_MODULE_PATH");
  /* check for module list in $GCCMELT_MODULE_PATH */
  if (envpath && envpath[0]) 
    {
      modlistpath = lookup_path(envpath, modlistbase, MODLIS_SUFFIX);
      if (modlistpath)
	goto loadit;
    }
  envpath = NULL;
  /* check for module list in melt_module_dir */
  modlistpath = concat (melt_module_dir,
			"/", modlistbase, MODLIS_SUFFIX, NULL);
  if (!access (modlistpath, R_OK))
    goto loadit;
  free (modlistpath);
  modlistpath = 0;
  /* check in the temporary directory */
  modlistpath = melt_tempdir_path (modlistbase, MODLIS_SUFFIX);
  if (!access (modlistpath, R_OK))
    goto loadit;
  free (modlistpath);
  modlistpath = 0;
  fatal_error ("cannot load MELT module list %s - incorrect name?",
	       modlistbase);
  if (!modlistpath)
    goto end;
loadit:
  debugeprintf ("meltgc_load_modulelist loadit modlistpath %s", modlistpath);
  filmod = fopen (modlistpath, "r");
  dbgprintf ("reading module list '%s'", modlistpath);
  if (!filmod)
    fatal_error ("failed to open melt module list file %s - %m",
		 modlistpath);
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_load_modulelist before reading module list : %s",
	      basename (__FILE__), __LINE__, modlistpath);
    curfram__.flocs = locbuf;
  }
#endif
  while (!feof (filmod))
    {
      char *pc = 0;
      memset (linbuf, 0, sizeof (linbuf));
      fgets (linbuf, sizeof (linbuf) - 1, filmod);
      pc = strchr (linbuf, '\n');
      if (pc)
	*pc = (char) 0;
      /* maybe we should not skip spaces */
      for (pc = linbuf; *pc && ISSPACE (*pc); pc++);
      if (*pc == '#' || *pc == (char) 0)
	continue;
      dbgprintf ("in module list %s loading module '%s'", modlistbase, pc);
      mdatav = meltgc_load_melt_module ((melt_ptr_t) mdatav, pc);
    }
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) mdatav;
#undef mdatav
}

/*************** initial load machinery *******************/


struct reading_st
{
  FILE *rfil;
  const char *rpath;
  char *rcurlin;		/* current line mallocated buffer */
  int rlineno;			/* current line number */
  int rcol;			/* current column */
  source_location rsrcloc;	/* current source location */
  melt_ptr_t *rpfilnam;	/* pointer to location of file name string */
  melt_ptr_t *rpgenv;	/* pointer to location of environment */
};
/* Obstack used for reading strings */
static struct obstack bstring_obstack;
#define rdback() (rd->rcol--)
#define rdnext() (rd->rcol++)
#define rdcurc() rd->rcurlin[rd->rcol]
#define rdfollowc(Rk) rd->rcurlin[rd->rcol + (Rk)]
#define rdeof() ((rd->rfil?feof(rd->rfil):1) && rd->rcurlin[rd->rcol]==0)
#define READ_ERROR(Fmt,...)	do {		\
  error_at(rd->rsrcloc, Fmt, ##__VA_ARGS__);	\
  fatal_error("MELT read failure <%s:%d>",	\
	      basename(__FILE__), __LINE__);	\
} while(0)
/* readval returns the read value and sets *PGOT to true if something
   was read */
static melt_ptr_t readval (struct reading_st *rd, bool * pgot);

enum commenthandling_en
{ COMMENT_SKIP = 0, COMMENT_NO };
static int
skipspace_getc (struct reading_st *rd, enum commenthandling_en comh)
{
  int c = 0;
  int incomm = 0;
readagain:
  if (rdeof ())
    return EOF;
  if (!rd->rcurlin)
    goto readline;
  c = rdcurc ();
  if ((c == '\n' && !rdfollowc (1)) || c == 0)
  readline:
    {
      /* we expect most lines to fit into linbuf, so we don't handle
         efficiently long lines */
      static char linbuf[400];
      char *mlin = 0;		/* partial mallocated line buffer when
				   not fitting into linbuf */
      char *eol = 0;
      if (!rd->rfil)		/* reading from a buffer */
	return EOF;
      if (rd->rcurlin)
	free ((void *) rd->rcurlin);
      rd->rcurlin = NULL;
      /* we really want getline here .... */
      for (;;)
	{
	  memset (linbuf, 0, sizeof (linbuf));
	  eol = NULL;
	  if (!fgets (linbuf, sizeof (linbuf) - 2, rd->rfil))
	    {
	      /* reached eof, so either give mlin or duplicate an empty
	         line */
	      if (mlin)
		rd->rcurlin = mlin;
	      else
		rd->rcurlin = xstrdup ("");
	      break;
	    }
	  else
	    eol = strchr (linbuf, '\n');
	  if (eol)
	    {
	      if (rd->rcurlin)
		free ((void *) rd->rcurlin);
	      if (!mlin)
		rd->rcurlin = xstrdup (linbuf);
	      else
		{
		  rd->rcurlin = concat (mlin, linbuf, NULL);
		  free (mlin);
		}
	      break;
	    }
	  else
	    {
	      /* read partly a long line without reaching the end of line */
	      if (mlin)
		{
		  char *newmlin = concat (mlin, linbuf, NULL);
		  free (mlin);
		  mlin = newmlin;
		}
	      else
		mlin = xstrdup (linbuf);
	    }
	};
      rd->rlineno++;
      rd->rsrcloc =
	linemap_line_start (line_table, rd->rlineno, strlen (linbuf) + 1);
      rd->rcol = 0;
      if (comh == COMMENT_NO) 
	return rdcurc();
      goto readagain;
    }
  else if (c == ';' && comh == COMMENT_SKIP)
    goto readline;
  else if (c == '#' && comh == COMMENT_SKIP && rdfollowc (1) == '|')
    {
      incomm = 1;
      rdnext ();
      c = rdcurc ();
      goto readagain;
    }
  else if (incomm && comh == COMMENT_SKIP && c == '|' && rdfollowc (1) == '#')
    {
      incomm = 0;
      rdnext ();
      rdnext ();
      c = rdcurc ();
      goto readagain;
    }
  else if (ISSPACE (c) || incomm)
    {
      rdnext ();
      c = rdcurc ();
      goto readagain;
    }
  else
    return c;
}


#define EXTRANAMECHARS "_+-*/<>=!?:%~&@$"
/* read a simple name on the bname_obstack */
static char *
readsimplename (struct reading_st *rd)
{
  int c = 0;
  while (!rdeof () && (c = rdcurc ()) > 0 &&
	 (ISALNUM (c) || strchr (EXTRANAMECHARS, c) != NULL))
    {
      obstack_1grow (&bname_obstack, (char) c);
      rdnext ();
    }
  obstack_1grow (&bname_obstack, (char) 0);
  return XOBFINISH (&bname_obstack, char *);
}


/* read an integer, like +123, which may also be +%numbername or +|fieldname */
static long
readsimplelong (struct reading_st *rd)
{
  int c = 0;
  long r = 0;
  char *endp = 0;
  char *nam = 0;
  bool neg = FALSE;
  /* we do not need any GC locals ie MELT_ENTERFRAME because no
     garbage collection occurs here */
  c = rdcurc ();
  if (((c == '+' || c == '-') && ISDIGIT (rdfollowc (1))) || ISDIGIT (c))
    {
      /* R5RS and R6RS require decimal notation -since the binary and
         hex numbers are hash-prefixed but for convenience we accept
         them thru strtol */
      r = strtol (&rdcurc (), &endp, 0);
      if (r == 0 && endp <= &rdcurc ())
	READ_ERROR ("MELT: failed to read number %.20s", &rdcurc ());
      rd->rcol += endp - &rdcurc ();
      return r;
    }
  else if ((c == '+' || c == '-') && rdfollowc (1) == '%')
    {
      neg = (c == '-');
      rdnext ();
      rdnext ();
      nam = readsimplename (rd);
      r = -1;
      /* the +%magicname notation is seldom used, we don't care to do
         many needless strcmp-s in that case, to be able to define the
         below simple macro */
      if (!nam)
	READ_ERROR
	  ("MELT: magic number name expected after +%% or -%% for magic %s",
	   nam);
#define NUMNAM(N) else if (!strcmp(nam,#N)) r = (N)
      NUMNAM (OBMAG_OBJECT);
      NUMNAM (OBMAG_MULTIPLE);
      NUMNAM (OBMAG_BOX);
      NUMNAM (OBMAG_CLOSURE);
      NUMNAM (OBMAG_ROUTINE);
      NUMNAM (OBMAG_LIST);
      NUMNAM (OBMAG_PAIR);
      NUMNAM (OBMAG_TRIPLE);
      NUMNAM (OBMAG_INT);
      NUMNAM (OBMAG_MIXINT);
      NUMNAM (OBMAG_MIXLOC);
      NUMNAM (OBMAG_REAL);
      NUMNAM (OBMAG_STRING);
      NUMNAM (OBMAG_STRBUF);
      NUMNAM (OBMAG_TREE);
      NUMNAM (OBMAG_GIMPLE);
      NUMNAM (OBMAG_GIMPLESEQ);
      NUMNAM (OBMAG_BASICBLOCK);
      NUMNAM (OBMAG_EDGE);
      NUMNAM (OBMAG_MAPOBJECTS);
      NUMNAM (OBMAG_MAPSTRINGS);
      NUMNAM (OBMAG_MAPTREES);
      NUMNAM (OBMAG_MAPGIMPLES);
      NUMNAM (OBMAG_MAPGIMPLESEQS);
      NUMNAM (OBMAG_MAPBASICBLOCKS);
      NUMNAM (OBMAG_MAPEDGES);
      NUMNAM (OBMAG_DECAY);
      NUMNAM (OBMAG_SPEC_FILE);
      NUMNAM (OBMAG_SPEC_RAWFILE);
      NUMNAM (OBMAG_SPEC_MPFR);
      NUMNAM (OBMAG_SPECPPL_COEFFICIENT);
      NUMNAM (OBMAG_SPECPPL_LINEAR_EXPRESSION);
      NUMNAM (OBMAG_SPECPPL_CONSTRAINT);
      NUMNAM (OBMAG_SPECPPL_CONSTRAINT_SYSTEM);
      NUMNAM (OBMAG_SPECPPL_GENERATOR);
      NUMNAM (OBMAG_SPECPPL_GENERATOR_SYSTEM);
      NUMNAM (OBMAG_SPECPPL_POLYHEDRON);
      /** the fields' ranks of melt.h have been removed in rev126278 */
#undef NUMNAM
      if (r < 0)
	READ_ERROR ("MELT: bad magic number name %s", nam);
      obstack_free (&bname_obstack, nam);
      return neg ? -r : r;
    }
  else
    READ_ERROR ("MELT: invalid number %.20s", &rdcurc ());
  return 0;
}


static melt_ptr_t
readseqlist (struct reading_st *rd, int endc)
{
  int c = 0;
  int nbcomp = 0;
  int startlin = rd->rlineno;
  bool got = FALSE;
  MELT_ENTERFRAME (2, NULL);
#define seqv curfram__.varptr[0]
#define compv curfram__.varptr[1]
  seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
readagain:
  compv = NULL;
  c = skipspace_getc (rd, COMMENT_SKIP);
  if (c == endc)
    {
      rdnext ();
      goto end;
    }
  got = FALSE;
  compv = readval (rd, &got);
  if (!compv && !got)
    READ_ERROR ("MELT: unexpected stuff in seq %.20s ... started line %d",
		&rdcurc (), startlin);
  meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) compv);
  nbcomp++;
  goto readagain;
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) seqv;
#undef compv
#undef seqv
}




static melt_ptr_t
makesexpr (struct reading_st *rd, int lineno, melt_ptr_t contents_p,
	   location_t loc)
{
  MELT_ENTERFRAME (4, NULL);
#define sexprv  curfram__.varptr[0]
#define contsv   curfram__.varptr[1]
#define locmixv curfram__.varptr[2]
  contsv = contents_p;
  gcc_assert (melt_magic_discr ((melt_ptr_t) contsv) == OBMAG_LIST);
  if (loc == 0)
    locmixv = meltgc_new_mixint ((meltobject_ptr_t) MELT_PREDEF (DISCR_MIXEDINT),
				    *rd->rpfilnam, (long) lineno);
  else
    locmixv = meltgc_new_mixloc ((meltobject_ptr_t) MELT_PREDEF (DISCR_MIXEDLOC),
				    *rd->rpfilnam, (long) lineno, loc);
  sexprv = meltgc_new_raw_object ((meltobject_ptr_t) MELT_PREDEF (CLASS_SEXPR), FSEXPR__LAST);
  ((meltobject_ptr_t) (sexprv))->obj_vartab[FSEXPR_LOCATION] =
    (melt_ptr_t) locmixv;
  ((meltobject_ptr_t) (sexprv))->obj_vartab[FSEXPR_CONTENTS] =
    (melt_ptr_t) contsv;
  meltgc_touch (sexprv);
  MELT_EXITFRAME ();
  return (melt_ptr_t) sexprv;
#undef sexprv
#undef contsv
#undef locmixv
}



melt_ptr_t
meltgc_named_symbol (const char *nam, int create)
{
  int namlen = 0, ix = 0;
  char *namdup = 0;
  char tinybuf[130];
  MELT_ENTERFRAME (4, NULL);
#define symbv    curfram__.varptr[0]
#define dictv    curfram__.varptr[1]
#define closv    curfram__.varptr[2]
#define nstrv    curfram__.varptr[3]
  symbv = NULL;
  dictv = NULL;
  closv = NULL;
  if (!nam)
    goto end;
  namlen = strlen (nam);
  memset (tinybuf, 0, sizeof (tinybuf));
  if (namlen < (int) sizeof (tinybuf) - 2)
    namdup = strcpy (tinybuf, nam);
  else
    namdup = strcpy ((char *) xcalloc (namlen + 1, 1), nam);
  gcc_assert (melt_magic_discr ((melt_ptr_t) MELT_PREDEF (CLASS_SYSTEM_DATA))
	      == OBMAG_OBJECT);
  gcc_assert (melt_magic_discr ((melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA)) ==
	      OBMAG_OBJECT);
  for (ix = 0; ix < namlen; ix++)
    if (ISALPHA (namdup[ix]))
      namdup[ix] = TOUPPER (namdup[ix]);
  if (MELT_PREDEF (INITIAL_SYSTEM_DATA) != 0)
    {
      dictv = melt_get_inisysdata (FSYSDAT_SYMBOLDICT);
      if (melt_magic_discr ((melt_ptr_t) dictv) == OBMAG_MAPSTRINGS)
	symbv =
	  melt_get_mapstrings ((struct meltmapstrings_st *) dictv,
				  namdup);
      if (symbv || !create)
	goto end;
      closv = melt_get_inisysdata (FSYSDAT_ADDSYMBOL);
      if (melt_magic_discr ((melt_ptr_t) closv) == OBMAG_CLOSURE)
	{
	  union meltparam_un pararg[1];
	  memset (&pararg, 0, sizeof (pararg));
	  nstrv = meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), namdup);
	  pararg[0].bp_aptr = (melt_ptr_t *) & nstrv;
	  symbv =
	    melt_apply ((meltclosure_ptr_t) closv,
			   (melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA),
			   BPARSTR_PTR, pararg, "", NULL);
	  goto end;
	}
    }
end:;
  if (namdup && namdup != tinybuf)
    free (namdup);
  MELT_EXITFRAME ();
  return (melt_ptr_t) symbv;
#undef symbv
#undef dictv
#undef closv
#undef nstrv
}

melt_ptr_t
meltgc_intern_symbol (melt_ptr_t symb_p)
{
  MELT_ENTERFRAME (5, NULL);
#define symbv    curfram__.varptr[0]
#define dictv    curfram__.varptr[1]
#define closv    curfram__.varptr[2]
#define nstrv    curfram__.varptr[3]
#define resv     curfram__.varptr[4]
#define obj_symbv    ((meltobject_ptr_t)(symbv))
  symbv = symb_p;
  if (melt_magic_discr ((melt_ptr_t) symbv) != OBMAG_OBJECT
      || obj_symbv->obj_len < FSYMB__LAST
      || !melt_is_instance_of ((melt_ptr_t) symbv,
				  (melt_ptr_t) MELT_PREDEF (CLASS_SYMBOL)))
    goto fail;
  nstrv = obj_symbv->obj_vartab[FNAMED_NAME];
  if (melt_magic_discr ((melt_ptr_t) nstrv) != OBMAG_STRING)
    goto fail;
  closv = melt_get_inisysdata (FSYSDAT_INTERNSYMBOL);
  if (melt_magic_discr ((melt_ptr_t) closv) != OBMAG_CLOSURE)
    goto fail;
  else
    {
      union meltparam_un pararg[1];
      memset (&pararg, 0, sizeof (pararg));
      pararg[0].bp_aptr = (melt_ptr_t *) & symbv;
      MELT_LOCATION_HERE ("intern symbol before apply");
      resv =
	melt_apply ((meltclosure_ptr_t) closv,
		       (melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA),
		       BPARSTR_PTR, pararg, "", NULL);
      goto end;
    }
fail:
  resv = NULL;
end:;
  MELT_EXITFRAME ();
  return (melt_ptr_t) resv;
#undef symbv
#undef dictv
#undef closv
#undef nstrv
#undef resv
#undef obj_symbv
}


melt_ptr_t
meltgc_intern_keyword (melt_ptr_t keyw_p)
{
  MELT_ENTERFRAME (5, NULL);
#define keywv    curfram__.varptr[0]
#define dictv    curfram__.varptr[1]
#define closv    curfram__.varptr[2]
#define nstrv    curfram__.varptr[3]
#define resv     curfram__.varptr[4]
#define obj_keywv    ((meltobject_ptr_t)(keywv))
  keywv = keyw_p;
  if (melt_magic_discr ((melt_ptr_t) keywv) != OBMAG_OBJECT
      || melt_object_length ((melt_ptr_t) obj_keywv) < FSYMB__LAST
      || !melt_is_instance_of ((melt_ptr_t) keywv,
				  (melt_ptr_t) MELT_PREDEF (CLASS_KEYWORD)))
    goto fail;
  nstrv = obj_keywv->obj_vartab[FNAMED_NAME];
  if (melt_magic_discr ((melt_ptr_t) nstrv) != OBMAG_STRING)
    goto fail;
  closv = melt_get_inisysdata (FSYSDAT_INTERNKEYW);
  if (melt_magic_discr ((melt_ptr_t) closv) != OBMAG_CLOSURE)
    goto fail;
  else
    {
      union meltparam_un pararg[1];
      memset (&pararg, 0, sizeof (pararg));
      pararg[0].bp_aptr = (melt_ptr_t *) & keywv;
      MELT_LOCATION_HERE ("intern keyword before apply");
      resv =
	melt_apply ((meltclosure_ptr_t) closv,
		       (melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA),
		       BPARSTR_PTR, pararg, "", NULL);
      goto end;
    }
fail:
  resv = NULL;
end:;
  MELT_EXITFRAME ();
  return (melt_ptr_t) resv;
#undef symbv
#undef dictv
#undef closv
#undef nstrv
#undef resv
#undef obj_symbv
}






melt_ptr_t
meltgc_named_keyword (const char *nam, int create)
{
  int namlen = 0, ix = 0;
  char *namdup = 0;
  char tinybuf[130];
  MELT_ENTERFRAME (4, NULL);
#define keywv    curfram__.varptr[0]
#define dictv    curfram__.varptr[1]
#define closv    curfram__.varptr[2]
#define nstrv    curfram__.varptr[3]
  keywv = NULL;
  dictv = NULL;
  closv = NULL;
  if (!nam)
    goto end;
  if (nam[0] == ':')
    nam++;
  namlen = strlen (nam);
  memset (tinybuf, 0, sizeof (tinybuf));
  if (namlen < (int) sizeof (tinybuf) - 2)
    namdup = strcpy (tinybuf, nam);
  else
    namdup = strcpy ((char *) xcalloc (namlen + 1, 1), nam);
  for (ix = 0; ix < namlen; ix++)
    if (ISALPHA (namdup[ix]))
      namdup[ix] = TOUPPER (namdup[ix]);
  gcc_assert (melt_magic_discr ((melt_ptr_t) MELT_PREDEF (CLASS_SYSTEM_DATA))
	      == OBMAG_OBJECT);
  gcc_assert (melt_magic_discr ((melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA)) ==
	      OBMAG_OBJECT);
  if (MELT_PREDEF (INITIAL_SYSTEM_DATA))
    {
      dictv = melt_get_inisysdata (FSYSDAT_KEYWDICT);
      if (melt_magic_discr ((melt_ptr_t) dictv) == OBMAG_MAPSTRINGS)
	keywv =
	  melt_get_mapstrings ((struct meltmapstrings_st *) dictv,
				  namdup);
      if (keywv || !create)
	goto end;
      closv = melt_get_inisysdata (FSYSDAT_ADDKEYW);
      if (melt_magic_discr ((melt_ptr_t) closv) == OBMAG_CLOSURE)
	{
	  union meltparam_un pararg[1];
	  memset (&pararg, 0, sizeof (pararg));
	  nstrv = meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), namdup);
	  pararg[0].bp_aptr = (melt_ptr_t *) & nstrv;
	  keywv =
	    melt_apply ((meltclosure_ptr_t) closv,
			   (melt_ptr_t) MELT_PREDEF (INITIAL_SYSTEM_DATA),
			   BPARSTR_PTR, pararg, "", NULL);
	  goto end;
	}
    }
end:;
  if (namdup && namdup != tinybuf)
    free (namdup);
  MELT_EXITFRAME ();
  return (melt_ptr_t) keywv;
#undef keywv
#undef dictv
#undef closv
#undef nstrv
}



static melt_ptr_t
readsexpr (struct reading_st *rd, int endc)
{
  int c = 0, lineno = rd->rlineno;
  location_t loc = 0;
  MELT_ENTERFRAME (3, NULL);
#define sexprv  curfram__.varptr[0]
#define contv   curfram__.varptr[1]
#define locmixv curfram__.varptr[2]
  if (!endc || rdeof ())
    READ_ERROR ("MELT: eof in s-expr (lin%d)", lineno);
  c = skipspace_getc (rd, COMMENT_SKIP);
  LINEMAP_POSITION_FOR_COLUMN (loc, line_table, rd->rcol);
  contv = readseqlist (rd, endc);
  sexprv = makesexpr (rd, lineno, (melt_ptr_t) contv, loc);
  MELT_EXITFRAME ();
  return (melt_ptr_t) sexprv;
#undef sexprv
#undef contv
#undef locmixv
}


static melt_ptr_t
readassoc (struct reading_st *rd)
{
  int sz = 0, c = 0, ln = 0, pos = 0;
  MELT_ENTERFRAME (3, NULL);
#define mapv  curfram__.varptr[0]
#define attrv curfram__.varptr[1]
#define valv   curfram__.varptr[2]
  /* maybe read the size */
  if (rdcurc () == '/')
    {
      sscanf (&rdcurc (), "/%d%n", &sz, &pos);
      if (pos > 0)
	rd->rcol += pos;
    };
  if (sz > MELT_MAXLEN)
    sz = MELT_MAXLEN;
  else if (sz < 0)
    sz = 2;
  mapv = meltgc_new_mapobjects ((meltobject_ptr_t) MELT_PREDEF (DISCR_MAPOBJECTS), sz);
  c = skipspace_getc (rd, COMMENT_SKIP);
  while (c != '}' && !rdeof ())
    {
      bool gotat = FALSE, gotva = FALSE;
      ln = rd->rlineno;
      attrv = readval (rd, &gotat);
      if (!gotat || !attrv
	  || melt_magic_discr ((melt_ptr_t) attrv) != OBMAG_OBJECT)
	READ_ERROR ("MELT: bad attribute in mapoobject line %d", ln);
      c = skipspace_getc (rd, COMMENT_SKIP);
      if (c != '=')
	READ_ERROR ("MELT: expected equal = after attribute but got %c",
		    ISPRINT (c) ? c : ' ');
      rdnext ();
      ln = rd->rlineno;
      valv = readval (rd, &gotva);
      if (!valv)
	READ_ERROR ("MELT: null or missing value in mapobject line %d", ln);
      c = skipspace_getc (rd, COMMENT_SKIP);
      if (c == '.')
	c = skipspace_getc (rd, COMMENT_SKIP);
    }
  if (c == '}')
    rdnext ();
  MELT_EXITFRAME ();
  return (melt_ptr_t) mapv;
#undef mapv
#undef attrv
#undef valv
}


/* if the string ends with "_ call gettext on it to have it
   localized/internationlized -i18n- */
static melt_ptr_t
readstring (struct reading_st *rd)
{
  int c = 0;
  int nbesc = 0;
  char *cstr = 0, *endc = 0;
  bool isintl = false;
  MELT_ENTERFRAME (1, NULL);
#define strv   curfram__.varptr[0]
#define str_strv  ((struct meltstring_st*)(strv))
  while ((c = rdcurc ()) != '"' && !rdeof ())
    {
      if (c != '\\')
	{
	  obstack_1grow (&bstring_obstack, (char) c);
	  if (c == '\n')
	    c = skipspace_getc (rd, COMMENT_NO);
	  else
	    rdnext ();
	}
      else
	{
	  rdnext ();
	  c = rdcurc ();
	  nbesc++;
	  switch (c)
	    {
	    case 'a':
	      c = '\a';
	      rdnext ();
	      break;
	    case 'b':
	      c = '\b';
	      rdnext ();
	      break;
	    case 't':
	      c = '\t';
	      rdnext ();
	      break;
	    case 'n':
	      c = '\n';
	      rdnext ();
	      break;
	    case 'v':
	      c = '\v';
	      rdnext ();
	      break;
	    case 'f':
	      c = '\f';
	      rdnext ();
	      break;
	    case 'r':
	      c = '\r';
	      rdnext ();
	      break;
	    case '"':
	      c = '\"';
	      rdnext ();
	      break;
	    case '\\':
	      c = '\\';
	      rdnext ();
	      break;
	    case '\n':
	    case '\r':
	      skipspace_getc (rd, COMMENT_NO);
	      continue;
	    case ' ':
	      c = ' ';
	      rdnext ();
	      break;
	    case 'x':
	      rdnext ();
	      c = (char) strtol (&rdcurc (), &endc, 16);
	      if (c == 0 && endc <= &rdcurc ())
		READ_ERROR ("MELT: illegal hex \\x escape in string %.20s",
			    &rdcurc ());
	      if (*endc == ';')
		endc++;
	      rd->rcol += endc - &rdcurc ();
	      break;
	    case '{':
	      {
		int linbrac = rd->rlineno;
		/* the escaped left brace \{ read verbatim all the string till the right brace } */
		rdnext ();
		while (rdcurc () != '}')
		  {
		    int cc;
		    if (rdeof ())
		      READ_ERROR
			("MELT: reached end of file in braced block string starting line %d",
			 linbrac);
		    cc = rdcurc ();
		    if (cc == '\n')
		      cc = skipspace_getc (rd, COMMENT_NO);
		    else
		      obstack_1grow (&bstring_obstack, (char) cc);
		    rdnext ();
		  };
		rdnext ();
	      }
	      break;
	    default:
	      READ_ERROR
		("MELT: illegal escape sequence %.10s in string -- got \\%c (hex %x)",
		 &rdcurc () - 1, c, c);
	    }
	  obstack_1grow (&bstring_obstack, (char) c);
	}
    }
  if (c == '"')
    rdnext ();
  else
    READ_ERROR ("MELT: unterminated string %.20s", &rdcurc ());
  c = rdcurc ();
  if (c == '_' && !rdeof ())
    {
      isintl = true;
      rdnext ();
    }
  obstack_1grow (&bstring_obstack, (char) 0);
  cstr = XOBFINISH (&bstring_obstack, char *);
  if (isintl)
    cstr = gettext (cstr);
  strv = meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), cstr);
  obstack_free (&bstring_obstack, cstr);
  MELT_EXITFRAME ();
  return (melt_ptr_t) strv;
#undef strv
#undef str_strv
}

/**
   macrostring so #{if ($A>0) printf("%s", $B);}# is parsed as would
   be parsed the s-expr ("if (" A ">0) printf(\"%s\", " B ");")

   read a macrostring sequence starting with #{ and ending with }#
   perhaps spanning several lines in the source no escape characters
   are handled (in particular no backslash escapes) except the dollar
   sign $ and then ending }# 

   A $ followed by alphabetical caracters (or as in C by underscores
   or digits, provided the first is not a digit) is handled as a
   symbol. If it is immediately followed by an hash # the # is
   skipped

**/
static melt_ptr_t
readmacrostringsequence (struct reading_st *rd) 
{
#if ENABLE_CHECKING
  static long _herecallcount;
  long callcount = ++_herecallcount;
#endif
  int lineno = rd->rlineno;
  location_t loc = 0;
  MELT_ENTERFRAME (6, NULL);
#define readv    curfram__.varptr[0]
#define strv     curfram__.varptr[1]
#define symbv    curfram__.varptr[2]
#define seqv     curfram__.varptr[3]
#define sbufv    curfram__.varptr[4]
  LINEMAP_POSITION_FOR_COLUMN (loc, line_table, rd->rcol);
  seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
  sbufv = meltgc_new_strbuf((meltobject_ptr_t) MELT_PREDEF(DISCR_STRBUF), (char*)0);
  for(;;) {
    if (!rdcurc()) {
      /* reached end of line */
      skipspace_getc(rd, COMMENT_NO);
      continue;
    }
    if (rdeof()) 
      READ_ERROR("reached end of file in macrostring sequence started line %d", lineno);
    debugeprintf ("macrostring startloop rdcur:%s", &rdcurc());
    debugeprintf ("macrostring startloop sbuf@%pL%d:%s", sbufv, 
		 melt_strbuf_usedlength((melt_ptr_t)sbufv),
		 melt_strbuf_str((melt_ptr_t) sbufv));
    if (rdcurc()=='}' && rdfollowc(1)=='#') {
      rdnext(); 
      rdnext();
      if (sbufv && melt_strbuf_usedlength((melt_ptr_t)sbufv)>0) {
	strv = meltgc_new_stringdup ((meltobject_ptr_t) MELT_PREDEF(DISCR_STRING),
					melt_strbuf_str((melt_ptr_t) sbufv));
	debugeprintf ("macrostring end appending str:%s", melt_string_str((melt_ptr_t)strv));
	meltgc_append_list((melt_ptr_t) seqv, (melt_ptr_t) strv);
	sbufv = NULL;
	strv = NULL;
      }
      break;
    }
    else if (rdcurc()=='$') {
      /* $ followed by letters or underscore makes a symbol */
      if (ISALPHA(rdfollowc(1)) || rdfollowc(1)=='_') {
	int lnam = 1;
	char tinybuf[64];
	/* if there is any sbuf, make a string of it and add the
	   string into the sequence */
	if (sbufv && melt_strbuf_usedlength((melt_ptr_t)sbufv)>0) {
	  strv = meltgc_new_stringdup((meltobject_ptr_t) MELT_PREDEF(DISCR_STRING),
					 melt_strbuf_str((melt_ptr_t) sbufv));
	  meltgc_append_list((melt_ptr_t) seqv, (melt_ptr_t) strv);
	  debugeprintf ("macrostring var appending str:%s", melt_string_str((melt_ptr_t)strv));
	  sbufv = NULL;
	  strv = NULL;
	}
	while (ISALNUM(rdfollowc(lnam)) || rdfollowc(lnam) == '_') 
	  lnam++;
	if (lnam< (int)sizeof(tinybuf)-2) {
	  memset(tinybuf, 0, sizeof(tinybuf));
	  memcpy(tinybuf, &rdfollowc(1), lnam-1);
	  tinybuf[lnam] = (char)0;
	  debugeprintf ("macrostring tinysymb lnam.%d <%s>", lnam, tinybuf);
	  symbv = meltgc_named_symbol(tinybuf, MELT_CREATE);
	}
	else {
	  char *nambuf = (char*) xcalloc(lnam+2, 1);
	  memcpy(nambuf, &rdfollowc(1), lnam-1);
	  nambuf[lnam] = (char)0;
	  symbv = meltgc_named_symbol(nambuf, MELT_CREATE);
	  debugeprintf ("macrostring bigsymb <%s>", tinybuf);
	  free(nambuf);
	}
	rd->rcol += lnam;
	/* skip the hash # if just after the symbol */
	if (rdcurc() == '#') 
	  rdnext();
	/* append the symbol */
	meltgc_append_list((melt_ptr_t) seqv, (melt_ptr_t) symbv);
	symbv = NULL;
      }
      /* $. is silently skipped */
      else if (rdfollowc(1) == '.') {
	rdnext(); 
	rdnext();
      }
      /* $$ is handled as a single dollar $ */
      else if (rdfollowc(1) == '$') {
	if (!sbufv)
	  sbufv = meltgc_new_strbuf((meltobject_ptr_t) MELT_PREDEF(DISCR_STRBUF), (char*)0);
	meltgc_add_strbuf_raw_len((melt_ptr_t)sbufv, "$", 1);
	rdnext();
	rdnext();
      }
      /* $# is handled as a single hash # */
      else if (rdfollowc(1) == '#') {
	if (!sbufv)
	  sbufv = meltgc_new_strbuf((meltobject_ptr_t) MELT_PREDEF(DISCR_STRBUF), (char*)0);
	meltgc_add_strbuf_raw_len((melt_ptr_t)sbufv, "#", 1);
	rdnext();
	rdnext();
      }
      /* any other dollar something is an error */
      else READ_ERROR("unexpected dollar escape in macrostring %.4s started line %d",
		      &rdcurc(), lineno);
    }
    else if ( ISALNUM(rdcurc()) || ISSPACE(rdcurc()) ) { 
      /* handle efficiently the common case of alphanum and spaces */
      int nbc = 0;
      if (!sbufv)
	sbufv = meltgc_new_strbuf((meltobject_ptr_t) MELT_PREDEF(DISCR_STRBUF), (char*)0);
      while (ISALNUM(rdfollowc(nbc)) || ISSPACE(rdfollowc(nbc))) 
	nbc++;
      meltgc_add_strbuf_raw_len((melt_ptr_t)sbufv, &rdcurc(), nbc);
      rd->rcol += nbc;
    }
    else { /* the current char is not a dollar $ */
      if (!sbufv)
	sbufv = meltgc_new_strbuf((meltobject_ptr_t) MELT_PREDEF(DISCR_STRBUF), (char*)0);
      meltgc_add_strbuf_raw_len((melt_ptr_t)sbufv, &rdcurc(), 1);
      rdnext();
    }
  }
  debugmsgval("readmacrostringsequence seqv", seqv, callcount);
  readv = makesexpr (rd, lineno, (melt_ptr_t) seqv, loc);
  debugmsgval("readmacrostringsequence readv", readv, callcount);
  MELT_EXITFRAME ();
  return (melt_ptr_t) readv;
#undef readv
#undef strv
#undef symbv
#undef seqv
#undef sbufv
}


static melt_ptr_t
readhashescape (struct reading_st *rd)
{
  int c = 0;
  char *nam = NULL;
  MELT_ENTERFRAME (4, NULL);
#define readv  curfram__.varptr[0]
#define compv  curfram__.varptr[1]
#define listv  curfram__.varptr[2]
#define pairv  curfram__.varptr[3]
  readv = NULL;
  c = rdcurc ();
  if (!c || rdeof ())
    READ_ERROR ("MELT: eof in hashescape %.20s", &rdcurc ());
  if (c == '\\')
    {
      rdnext ();
      if (ISALPHA (rdcurc ()) && rdcurc () != 'x' && ISALPHA (rdfollowc (1)))
	{
	  nam = readsimplename (rd);
	  c = 0;
	  if (!strcmp (nam, "nul"))
	    c = 0;
	  else if (!strcmp (nam, "alarm"))
	    c = '\a';
	  else if (!strcmp (nam, "backspace"))
	    c = '\b';
	  else if (!strcmp (nam, "tab"))
	    c = '\t';
	  else if (!strcmp (nam, "linefeed"))
	    c = '\n';
	  else if (!strcmp (nam, "vtab"))
	    c = '\v';
	  else if (!strcmp (nam, "page"))
	    c = '\f';
	  else if (!strcmp (nam, "return"))
	    c = '\r';
	  else if (!strcmp (nam, "space"))
	    c = ' ';
	  /* won't work on non ASCII or ISO or Unicode host, but we don't care */
	  else if (!strcmp (nam, "delete"))
	    c = 0xff;
	  else if (!strcmp (nam, "esc"))
	    c = 0x1b;
	  else
	    READ_ERROR ("MELT: invalid char escape %s", nam);
	  obstack_free (&bname_obstack, nam);
	char_escape:
	  readv = meltgc_new_int ((meltobject_ptr_t) MELT_PREDEF (DISCR_CHARINTEGER), c);
	}
      else if (rdcurc () == 'x' && ISXDIGIT (rdfollowc (1)))
	{
	  char *endc = 0;
	  rdnext ();
	  c = strtol (&rdcurc (), &endc, 16);
	  if (c == 0 && endc <= &rdcurc ())
	    READ_ERROR ("MELT: illigal hex #\\x escape in char %.20s",
			&rdcurc ());
	  rd->rcol += endc - &rdcurc ();
	  goto char_escape;
	}
      else if (ISPRINT (rdcurc ()))
	{
	  c = rdcurc ();
	  rdnext ();
	  goto char_escape;
	}
      else
	READ_ERROR ("MELT: unrecognized char escape #\\%s", &rdcurc ());
    }
  else if (c == '(')
    {
      int ln = 0, ix = 0;
      listv = readseqlist (rd, ')');
      ln = melt_list_length ((melt_ptr_t) listv);
      gcc_assert (ln >= 0);
      readv = meltgc_new_multiple ((meltobject_ptr_t) MELT_PREDEF (DISCR_MULTIPLE), ln);
      for ((ix = 0), (pairv =
		      ((struct meltlist_st *) (listv))->first);
	   ix < ln
	   && melt_magic_discr ((melt_ptr_t) pairv) == OBMAG_PAIR;
	   pairv = ((struct meltpair_st *) (pairv))->tl)
	((struct meltmultiple_st *) (readv))->tabval[ix++] =
	  ((struct meltpair_st *) (pairv))->hd;
      meltgc_touch (readv);
    }
  else if (c == '[')
    {
      /* a melt extension #[ .... ] for lists */
      readv = readseqlist (rd, ']');
    }
  else if ((c == 'b' || c == 'B') && ISDIGIT (rdfollowc (1)))
    {
      /* binary number */
      char *endc = 0;
      long n = 0;
      rdnext ();
      n = strtol (&rdcurc (), &endc, 2);
      if (n == 0 && endc <= &rdcurc ())
	READ_ERROR ("MELT: bad binary number %s", endc);
      readv = meltgc_new_int ((meltobject_ptr_t) MELT_PREDEF (DISCR_INTEGER), n);
    }
  else if ((c == 'o' || c == 'O') && ISDIGIT (rdfollowc (1)))
    {
      /* octal number */
      char *endc = 0;
      long n = 0;
      rdnext ();
      n = strtol (&rdcurc (), &endc, 8);
      if (n == 0 && endc <= &rdcurc ())
	READ_ERROR ("MELT: bad octal number %s", endc);
      readv = meltgc_new_int ((meltobject_ptr_t) MELT_PREDEF (DISCR_INTEGER), n);
    }
  else if ((c == 'd' || c == 'D') && ISDIGIT (rdfollowc (1)))
    {
      /* decimal number */
      char *endc = 0;
      long n = 0;
      rdnext ();
      n = strtol (&rdcurc (), &endc, 10);
      if (n == 0 && endc <= &rdcurc ())
	READ_ERROR ("MELT: bad decimal number %s", endc);
      readv = meltgc_new_int ((meltobject_ptr_t) MELT_PREDEF (DISCR_INTEGER), n);
    }
  else if ((c == 'x' || c == 'x') && ISDIGIT (rdfollowc (1)))
    {
      /* hex number */
      char *endc = 0;
      long n = 0;
      rdnext ();
      n = strtol (&rdcurc (), &endc, 16);
      if (n == 0 && endc <= &rdcurc ())
	READ_ERROR ("MELT: bad octal number %s", endc);
      readv = meltgc_new_int ((meltobject_ptr_t) MELT_PREDEF (DISCR_INTEGER), n);
    }
  else if (c == '+' && ISALPHA (rdfollowc (1)))
    {
      bool gotcomp = FALSE;
      char *nam = 0;
      nam = readsimplename (rd);
      compv = readval (rd, &gotcomp);
      if (!strcmp (nam, "MELT"))
	readv = compv;
      else
	readv = readval (rd, &gotcomp);
    }
  /* #{ is a macrostringsequence; it is terminated by }# and each
      occurrence of $ followed by alphanum char is considered as a
      MELT symbol, the other caracters are considered as string
      chunks; the entire read is a sequence */
  else if (c == '{') {
      rdnext ();
      readv = readmacrostringsequence(rd);
  }
  else
    READ_ERROR ("MELT: invalid escape %.20s", &rdcurc ());
  MELT_EXITFRAME ();
  return (melt_ptr_t) readv;
#undef readv
#undef listv
#undef compv
#undef pairv
}



static melt_ptr_t
readval (struct reading_st *rd, bool * pgot)
{
  int c = 0;
  char *nam = 0;
  MELT_ENTERFRAME (4, NULL);
#define readv   curfram__.varptr[0]
#define compv   curfram__.varptr[1]
#define seqv    curfram__.varptr[2]
#define altv    curfram__.varptr[3]
  readv = NULL;
  c = skipspace_getc (rd, COMMENT_SKIP);
  /*   debugeprintf ("start readval line %d col %d char %c", rd->rlineno, rd->rcol,
     ISPRINT (c) ? c : ' '); */
  if (ISDIGIT (c)
      || ((c == '-' || c == '+')
	  && (ISDIGIT (rdfollowc (1)) || rdfollowc (1) == '%'
	      || rdfollowc (1) == '|')))
    {
      long num = 0;
      num = readsimplelong (rd);
      readv =
	meltgc_new_int ((meltobject_ptr_t) MELT_PREDEF (DISCR_INTEGER),
			   num);
      *pgot = TRUE;
      goto end;
    }				/* end if ISDIGIT or '-' or '+' */
  else if (c == '"')
    {
      rdnext ();
      readv = readstring (rd);
      *pgot = TRUE;
      goto end;
    }				/* end if '"' */
  else if (c == '(')
    {
      rdnext ();
      if (rdcurc () == ')')
	{
	  rdnext ();
	  readv = NULL;
	  *pgot = TRUE;
	  goto end;
	}
      readv = readsexpr (rd, ')');
      *pgot = TRUE;
      goto end;
    }				/* end if '(' */
  else if (c == '[')
    {
      rdnext ();
      readv = readsexpr (rd, ']');
      *pgot = TRUE;
      goto end;
    }				/* end if '[' */
  else if (c == '{')
    {
      rdnext ();
      readv = readassoc (rd);
      *pgot = TRUE;
      goto end;
    }
  else if (c == '#')
    {
      rdnext ();
      c = rdcurc ();
      readv = readhashescape (rd);
      *pgot = TRUE;
      goto end;
    }
  else if (c == '\'')
    {
      int lineno = rd->rlineno;
      bool got = false;
      location_t loc = 0;
      rdnext ();
      compv = readval (rd, &got);
      if (!got)
	READ_ERROR ("MELT: expecting value after quote %.20s", &rdcurc ());
      seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
      altv = meltgc_named_symbol ("quote", MELT_CREATE);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) altv);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) compv);
      LINEMAP_POSITION_FOR_COLUMN (loc, line_table, rd->rcol);
      readv = makesexpr (rd, lineno, (melt_ptr_t) seqv, loc);
      *pgot = TRUE;
      goto end;
    }
  else if (c == '`')
    {
      int lineno = rd->rlineno;
      bool got = false;
      location_t loc = 0;
      rdnext ();
      LINEMAP_POSITION_FOR_COLUMN (loc, line_table, rd->rcol);
      compv = readval (rd, &got);
      if (!got)
	READ_ERROR ("MELT: expecting value after backquote %.20s",
		    &rdcurc ());
      seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
      altv = meltgc_named_symbol ("backquote", MELT_CREATE);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) altv);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) compv);
      readv = makesexpr (rd, lineno, (melt_ptr_t) seqv, loc);
      *pgot = TRUE;
      goto end;
    }
  else if (c == ',')
    {
      int lineno = rd->rlineno;
      bool got = false;
      location_t loc = 0;
      rdnext ();
      LINEMAP_POSITION_FOR_COLUMN (loc, line_table, rd->rcol);
      compv = readval (rd, &got);
      if (!got)
	READ_ERROR ("MELT: expecting value after comma %.20s", &rdcurc ());
      seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
      altv = meltgc_named_symbol ("comma", MELT_CREATE);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) altv);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) compv);
      readv = makesexpr (rd, lineno, (melt_ptr_t) seqv, loc);
      *pgot = TRUE;
      goto end;
    }
  else if (c == '?')
    {
      int lineno = rd->rlineno;
      bool got = false;
      location_t loc = 0;
      rdnext ();
      LINEMAP_POSITION_FOR_COLUMN (loc, line_table, rd->rcol);
      compv = readval (rd, &got);
      if (!got)
	READ_ERROR ("MELT: expecting value after question %.20s", &rdcurc ());
      seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
      altv = meltgc_named_symbol ("question", MELT_CREATE);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) altv);
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) compv);
      readv = makesexpr (rd, lineno, (melt_ptr_t) seqv, loc);
      *pgot = TRUE;
      goto end;
    }
  else if (c == ':')
    {
      nam = readsimplename (rd);
      readv = meltgc_named_keyword (nam, MELT_CREATE);
      if (!readv)
	READ_ERROR ("MELT: unknown named keyword %s", nam);
      *pgot = TRUE;
      goto end;
    }
  else if (ISALPHA (c) || strchr (EXTRANAMECHARS, c) != NULL)
    {
      nam = readsimplename (rd);
      readv = meltgc_named_symbol (nam, MELT_CREATE);
      *pgot = TRUE;
      goto end;
    }
  else
    {
      if (c >= 0)
	rdback ();
      readv = NULL;
    }
end:
  MELT_EXITFRAME ();
  if (nam)
    {
      *nam = 0;
      obstack_free (&bname_obstack, nam);
    };
  return (melt_ptr_t) readv;
#undef readv
#undef compv
#undef seqv
#undef altv
}


void
melt_error_str (melt_ptr_t mixloc_p, const char *msg,
		   melt_ptr_t str_p)
{
  int mixmag = 0;
  int lineno = 0;
  location_t loc = 0;
  MELT_ENTERFRAME (3, NULL);
#define mixlocv    curfram__.varptr[0]
#define strv       curfram__.varptr[1]
#define finamv     curfram__.varptr[2]
  gcc_assert (msg && msg[0]);
  mixlocv = mixloc_p;
  strv = str_p;
  mixmag = melt_magic_discr ((melt_ptr_t) mixlocv);
  if (mixmag == OBMAG_MIXLOC)
    {
      loc = melt_location_mixloc ((melt_ptr_t) mixlocv);
      finamv = melt_val_mixloc ((melt_ptr_t) mixlocv);
      lineno = melt_num_mixloc ((melt_ptr_t) mixlocv);
    }
  else if (mixmag == OBMAG_MIXINT)
    {
      loc = 0;
      finamv = melt_val_mixint ((melt_ptr_t) mixlocv);
      lineno = melt_num_mixint ((melt_ptr_t) mixlocv);
    }
  else
    {
      loc = 0;
      finamv = NULL;
      lineno = 0;
    }
  if (loc)
    {
      const char *cstr = melt_string_str ((melt_ptr_t) strv);
      if (cstr)
	error_at (loc, "Melt Error[#%ld]: %s - %s", melt_dbgcounter,
		  msg, cstr);
      else
	error_at (loc, "Melt Error[#%ld]: %s", melt_dbgcounter, msg);
    }
  else
    {
      const char *cfilnam = melt_string_str ((melt_ptr_t) finamv);
      const char *cstr = melt_string_str ((melt_ptr_t) strv);
      if (cfilnam)
	{
	  if (cstr)
	    error ("Melt Error[#%ld] @ %s:%d: %s - %s", melt_dbgcounter,
		   cfilnam, lineno, msg, cstr);
	  else
	    error ("Melt Error[#%ld] @ %s:%d: %s", melt_dbgcounter,
		   cfilnam, lineno, msg);
	}
      else
	{
	  if (cstr)
	    error ("Melt Error[#%ld]: %s - %s", melt_dbgcounter, msg,
		   cstr);
	  else
	    error ("Melt Error[#%ld]: %s", melt_dbgcounter, msg);
	}
    }
  MELT_EXITFRAME ();
}

#undef mixlocv
#undef strv
#undef finamv


void
melt_warning_str (int opt, melt_ptr_t mixloc_p, const char *msg,
		     melt_ptr_t str_p)
{
  int mixmag = 0;
  int lineno = 0;
  location_t loc = 0;
  MELT_ENTERFRAME (3, NULL);
#define mixlocv    curfram__.varptr[0]
#define strv       curfram__.varptr[1]
#define finamv     curfram__.varptr[2]
  gcc_assert (msg && msg[0]);
  mixlocv = mixloc_p;
  strv = str_p;
  mixmag = melt_magic_discr ((melt_ptr_t) mixlocv);
  if (mixmag == OBMAG_MIXLOC)
    {
      loc = melt_location_mixloc ((melt_ptr_t) mixlocv);
      finamv = melt_val_mixloc ((melt_ptr_t) mixlocv);
      lineno = melt_num_mixloc ((melt_ptr_t) mixlocv);
    }
  else if (mixmag == OBMAG_MIXINT)
    {
      loc = 0;
      finamv = melt_val_mixint ((melt_ptr_t) mixlocv);
      lineno = melt_num_mixint ((melt_ptr_t) mixlocv);
    }
  else
    {
      loc = 0;
      finamv = NULL;
      lineno = 0;
    }
  if (loc)
    {
      const char *cstr = melt_string_str ((melt_ptr_t) strv);
      if (cstr)
	warning_at (loc, opt, "Melt Warning[#%ld]: %s - %s",
		    melt_dbgcounter, msg, cstr);
      else
	warning_at (loc, opt, "Melt Warning[#%ld]: %s", 
		    melt_dbgcounter, msg);
    }
  else
    {
      const char *cfilnam = melt_string_str ((melt_ptr_t) finamv);
      const char *cstr = melt_string_str ((melt_ptr_t) strv);
      if (cfilnam)
	{
	  if (cstr)
	    warning (opt, "Melt Warning[#%ld] @ %s:%d: %s - %s",
		     melt_dbgcounter, cfilnam, lineno, msg, cstr);
	  else
	    warning (opt, "Melt Warning[#%ld] @ %s:%d: %s",
		     melt_dbgcounter, cfilnam, lineno, msg);
	}
      else
	{
	  if (cstr)
	    warning (opt, "Melt Warning[#%ld]: %s - %s",
		     melt_dbgcounter, msg, cstr);
	  else
	    warning (opt, "Melt Warning[#%ld]: %s", melt_dbgcounter,
		     msg);
	}
    }
  MELT_EXITFRAME ();
}

#undef mixlocv
#undef strv
#undef finamv



void
melt_inform_str (melt_ptr_t mixloc_p, const char *msg,
		    melt_ptr_t str_p)
{
  int mixmag = 0;
  int lineno = 0;
  location_t loc = 0;
  MELT_ENTERFRAME (3, NULL);
#define mixlocv    curfram__.varptr[0]
#define strv       curfram__.varptr[1]
#define finamv     curfram__.varptr[2]
  gcc_assert (msg && msg[0]);
  mixlocv = mixloc_p;
  strv = str_p;
  mixmag = melt_magic_discr ((melt_ptr_t) mixlocv);
  if (mixmag == OBMAG_MIXLOC)
    {
      loc = melt_location_mixloc ((melt_ptr_t) mixlocv);
      finamv = melt_val_mixloc ((melt_ptr_t) mixlocv);
      lineno = melt_num_mixloc ((melt_ptr_t) mixlocv);
    }
  else if (mixmag == OBMAG_MIXINT)
    {
      loc = 0;
      finamv = melt_val_mixint ((melt_ptr_t) mixlocv);
      lineno = melt_num_mixint ((melt_ptr_t) mixlocv);
    }
  else
    {
      loc = 0;
      finamv = NULL;
      lineno = 0;
    }
  if (loc)
    {
      const char *cstr = melt_string_str ((melt_ptr_t) strv);
      if (cstr)
	inform (loc, "Melt Inform[#%ld]: %s - %s", melt_dbgcounter,
		msg, cstr);
      else
	inform (loc, "Melt Inform[#%ld]: %s", melt_dbgcounter, msg);
    }
  else
    {
      const char *cfilnam = melt_string_str ((melt_ptr_t) finamv);
      const char *cstr = melt_string_str ((melt_ptr_t) strv);
      if (cfilnam)
	{
	  if (cstr)
	    inform (UNKNOWN_LOCATION, "Melt Inform[#%ld] @ %s:%d: %s - %s",
		    melt_dbgcounter, cfilnam, lineno, msg, cstr);
	  else
	    inform (UNKNOWN_LOCATION, "Melt Inform[#%ld] @ %s:%d: %s",
		    melt_dbgcounter, cfilnam, lineno, msg);
	}
      else
	{
	  if (cstr)
	    inform (UNKNOWN_LOCATION, "Melt Inform[#%ld]: %s - %s",
		    melt_dbgcounter, msg, cstr);
	  else
	    inform (UNKNOWN_LOCATION, "Melt Inform[#%ld]: %s",
		    melt_dbgcounter, msg);
	}
    }
  MELT_EXITFRAME ();
}

#undef mixlocv
#undef strv
#undef finamv




melt_ptr_t
meltgc_read_file (const char *filnam, const char *locnam)
{
  struct reading_st rds;
  FILE *fil = 0;
  struct reading_st *rd = 0;
  char *filnamdup = 0;
  MELT_ENTERFRAME (4, NULL);
#define genv      curfram__.varptr[0]
#define valv      curfram__.varptr[1]
#define locnamv   curfram__.varptr[2]
#define seqv      curfram__.varptr[3]
  memset (&rds, 0, sizeof (rds));
  debugeprintf ("meltgc_read_file filnam %s locnam %s", filnam, locnam);
  if (!filnam || !filnam[0])
    goto end;
  if (!locnam || !locnam[0])
    locnam = basename (filnam);
  filnamdup = xstrdup (filnam);	/* filnamdup is never freed */
  debugeprintf ("meltgc_read_file filnam %s locnam %s", filnam, locnam);
  fil = fopen (filnam, "rt");
  if (!fil)
    fatal_error ("cannot open MELT file %s - %m", filnam);
  /*  debugeprintf ("starting loading file %s", filnamdup); */
  rds.rfil = fil;
  rds.rpath = filnam;
  rds.rlineno = 0;
  linemap_add (line_table, LC_RENAME, false, filnamdup, 0);
  rd = &rds;
  locnamv = meltgc_new_stringdup ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), locnam);
  rds.rpfilnam = (melt_ptr_t *) & locnamv;
  rds.rpgenv = (melt_ptr_t *) & genv;
  seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
  while (!rdeof ())
    {
      bool got = FALSE;
      skipspace_getc (rd, COMMENT_SKIP);
      if (rdeof ())
	break;
      valv = readval (rd, &got);
      if (!got)
	READ_ERROR ("MELT: no value read %.20s", &rdcurc ());
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) valv);
    };
  rd = 0;
end:
  if (!seqv)
    {
      debugeprintf ("meltgc_read_file filnam %s fail & return NULL", filnam);
      warning(0, "MELT file %s read without content, perhaps failed.", filnam);
    }
  else
    debugeprintf ("meltgc_read_file filnam %s return list of %d elem",
		  filnam, melt_list_length ((melt_ptr_t) seqv));
  MELT_EXITFRAME ();
  return (melt_ptr_t) seqv;
#undef vecshv
#undef genv
#undef locnamv
#undef seqv
}


melt_ptr_t
meltgc_read_from_rawstring (const char *rawstr, const char *locnam,
			       location_t loch)
{
  struct reading_st rds;
  char *rbuf = 0;
  struct reading_st *rd = 0;
  MELT_ENTERFRAME (4, NULL);
#define genv      curfram__.varptr[0]
#define valv      curfram__.varptr[1]
#define locnamv   curfram__.varptr[2]
#define seqv      curfram__.varptr[3]
  memset (&rds, 0, sizeof (rds));
  if (!rawstr)
    goto end;
  rbuf = xstrdup (rawstr);
  rds.rfil = 0;
  rds.rpath = 0;
  rds.rlineno = 0;
  rds.rcurlin = rbuf;
  rds.rsrcloc = loch;
  rd = &rds;
  if (locnam)
    locnamv = meltgc_new_stringdup ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), locnam);
  seqv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
  rds.rpfilnam = (melt_ptr_t *) & locnamv;
  rds.rpgenv = (melt_ptr_t *) & genv;
  while (rdcurc ())
    {
      bool got = FALSE;
      skipspace_getc (rd, COMMENT_SKIP);
      if (!rdcurc () || rdeof ())
	break;
      valv = readval (rd, &got);
      if (!got)
	READ_ERROR ("MELT: no value read %.20s", &rdcurc ());
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) valv);
    };
  rd = 0;
  free (rbuf);
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) seqv;
#undef vecshv
#undef genv
#undef locnamv
#undef seqv
}


melt_ptr_t
meltgc_read_from_val (melt_ptr_t strv_p, melt_ptr_t locnam_p)
{
  struct reading_st rds;
  char *rbuf = 0;
  struct reading_st *rd = 0;
  int strmagic = 0;
  MELT_ENTERFRAME (5, NULL);
#define genv      curfram__.varptr[0]
#define valv      curfram__.varptr[1]
#define locnamv   curfram__.varptr[2]
#define seqv      curfram__.varptr[3]
#define strv      curfram__.varptr[4]
  memset (&rds, 0, sizeof (rds));
  strv = strv_p;
  locnamv = locnam_p;
  rbuf = 0;
  strmagic = melt_magic_discr ((melt_ptr_t) strv);
  switch (strmagic)
    {
    case OBMAG_STRING:
      rbuf = (char *) xstrdup (melt_string_str ((melt_ptr_t) strv));
      break;
    case OBMAG_STRBUF:
      rbuf = xstrdup (melt_strbuf_str ((melt_ptr_t) strv));
      break;
    case OBMAG_OBJECT:
      if (melt_is_instance_of
	  ((melt_ptr_t) strv, (melt_ptr_t) MELT_PREDEF (CLASS_NAMED)))
	strv = melt_object_nth_field ((melt_ptr_t) strv, FNAMED_NAME);
      else
	strv = NULL;
      if (melt_string_str ((melt_ptr_t) strv))
	rbuf = xstrdup (melt_string_str ((melt_ptr_t) strv));
      break;
    default:
      break;
    }
  if (!rbuf)
    goto end;
  rds.rfil = 0;
  rds.rpath = 0;
  rds.rlineno = 0;
  rds.rcurlin = rbuf;
  rd = &rds;
  rds.rpfilnam = (melt_ptr_t *) & locnamv;
  rds.rpgenv = (melt_ptr_t *) & genv;
  while (rdcurc ())
    {
      bool got = FALSE;
      skipspace_getc (rd, COMMENT_SKIP);
      if (!rdcurc () || rdeof ())
	break;
      valv = readval (rd, &got);
      if (!got)
	READ_ERROR ("MELT: no value read %.20s", &rdcurc ());
      meltgc_append_list ((melt_ptr_t) seqv, (melt_ptr_t) valv);
    };
  rd = 0;
  free (rbuf);
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) seqv;
#undef vecshv
#undef genv
#undef locnamv
#undef seqv
#undef strv
#undef valv
}

static void
do_initial_command (melt_ptr_t modata_p)
{
  const char* modstr = NULL;
  const char* argstr = NULL;
  const char* argliststr = NULL;
  const char* secondargstr = NULL;
  MELT_ENTERFRAME (8, NULL);
#define dictv     curfram__.varptr[0]
#define closv     curfram__.varptr[1]
#define cstrv     curfram__.varptr[2]
#define arglv     curfram__.varptr[3]
#define csecstrv  curfram__.varptr[4]
#define modatav   curfram__.varptr[5]
#define curargv   curfram__.varptr[6]
#define resv      curfram__.varptr[7]
  modatav = modata_p;
  modstr = melt_argument ("mode");
  argstr = melt_argument ("arg");
  argliststr = melt_argument ("arglist");
  secondargstr = melt_argument ("secondarg");
  debugeprintf ("do_initial_command mode_string %s modatav %p",
		modstr, (void *) modatav);
  debugeprintf ("do_initial_command argstr %s", argstr);
  debugeprintf ("do_initial_command argliststr %s", argliststr);
  debugeprintf ("do_initial_command secondargstr %s", secondargstr);
  if (!MELT_PREDEF (INITIAL_SYSTEM_DATA)) 
    {
      error("MELT cannot execute initial command mode %s without INITIAL_SYSTEM_DATA", modstr);
      goto end;
    }
  dictv = melt_get_inisysdata(FSYSDAT_CMD_FUNDICT);
  debugeprintf ("do_initial_command dictv=%p", dictv);
  debugeprintvalue ("do_initial_command dictv", dictv);
  if (melt_magic_discr ((melt_ptr_t) dictv) != OBMAG_MAPSTRINGS)
    goto end;
  closv =
    melt_get_mapstrings ((struct meltmapstrings_st *) dictv,
			    modstr);
  debugeprintf ("do_initial_command closv=%p", closv);
  if (melt_magic_discr ((melt_ptr_t) closv) != OBMAG_CLOSURE)
    {
      error ("no closure for melt command %s", modstr);
      goto end;
    };
  debugeprintf ("do_initial_command argument_string %s",
		argstr);
  debugeprintf ("do_initial_command arglist_string %s",
		argliststr);
  debugeprintf ("do_initial_command secondargument_string %s",
		secondargstr);
  if (argstr && argstr[0]
      && argliststr && argliststr[0])
    {
      error
	("cannot have both -fmelt-arg=%s & -fmelt-arglist=%s given as program arguments",
	 argstr, argliststr);
      goto end;
    }
  {
    union meltparam_un pararg[3];
    memset (pararg, 0, sizeof (pararg));
    if (argstr && argstr[0])
      {
	cstrv =
	  meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING),
				argstr);
	pararg[0].bp_aptr = (melt_ptr_t *) & cstrv;
      }
    else if (argliststr && argliststr[0])
      {
	char *comma = 0;
	char *pc = 0;
	arglv = meltgc_new_list ((meltobject_ptr_t) MELT_PREDEF (DISCR_LIST));
	for (pc = CONST_CAST(char *, argliststr); pc;
	     pc = comma ? (comma + 1) : 0)
	  {
	    comma = strchr (pc, ',');
	    if (comma)
	      *comma = (char) 0;
	    curargv = meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), pc);
	    if (comma)
	      *comma = ',';
	    meltgc_append_list ((melt_ptr_t) arglv,
				   (melt_ptr_t) curargv);
	  }
	pararg[0].bp_aptr = (melt_ptr_t *) & arglv;
      };
    if (secondargstr && secondargstr[0])
      {
	csecstrv =
	  meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING),
				secondargstr);
	pararg[1].bp_aptr = (melt_ptr_t *) & csecstrv;
      }
    else
      {
	debugeprintf ("do_initial_command no second argument %p",
		      secondargstr);
	csecstrv = NULL;
	pararg[1].bp_aptr = (melt_ptr_t *) 0;
      }
    pararg[2].bp_aptr = (melt_ptr_t *) & modatav;
    debugeprintf ("do_initial_command before apply closv %p", closv);
    MELT_LOCATION_HERE ("do_initial_command before apply");
    resv = melt_apply ((meltclosure_ptr_t) closv,
			  (melt_ptr_t) MELT_PREDEF
			  (INITIAL_SYSTEM_DATA),
			  BPARSTR_PTR BPARSTR_PTR BPARSTR_PTR, pararg, "",
			  NULL);
    debugeprintf ("do_initial_command after apply closv %p resv %p", closv,
		  resv);
    exit_after_options = (resv == NULL);
  }
end:
  debugeprintf ("do_initial_command end %s", argstr);
  MELT_EXITFRAME ();
#undef dictv
#undef closv
#undef cstrv
#undef csecstrv
#undef modatav
#undef arglv
#undef curargv
#undef resv
}


/****
 * load the initial modules do the initial command if needed
 * the initstring is a semi-colon separated list of module names
 * and do the initial command
 ****/

static void
load_melt_modules_and_do_command (void)
{
  char *dupmodpath = 0;
  char *curmod = 0;
  char *nextmod = 0;
  const char *modstr = 0;
  const char *inistr = 0;
  const char* dbgstr = melt_argument("debug");
  MELT_ENTERFRAME (3, NULL);
#define modatv curfram__.varptr[0]
#define dumpv  curfram__.varptr[1]
  modstr = melt_argument ("mode");
  inistr = melt_argument ("init");
  debugeprintf ("load_melt_modules_and_do_command start init=%s command=%s",
		inistr, modstr);
  /* if there is no -fmelt-init use the default list of modules */
  if (!inistr || !inistr[0])
  {
    inistr = "@" MELT_DEFAULT_MODLIS;
    debugeprintf("inistr set to default %s", inistr);
  }
  dupmodpath = xstrdup (inistr);
  if (dbgstr && !dump_file)
    {
      fflush (stderr);
      dump_file = stderr;
      fflush (stderr);
      
    }
#if ENABLE_CHECKING
  if (dbgstr)
    {
      char *tracenam = getenv ("MELTTRACE");
      if (tracenam)
	melt_dbgtracefile = fopen (tracenam, "w");
      if (melt_dbgtracefile)
	{
	  time_t now = 0;
	  time (&now);
	  debugeprintf ("load_melt_modules_and_do_command dbgtracefile %s",
			tracenam);
	  fprintf (melt_dbgtracefile, "**MELT TRACE %s pid %d at %s",
		   tracenam, (int) getpid (), ctime (&now));
	  fflush (melt_dbgtracefile);
	}
    }
#endif
  curmod = dupmodpath;
  modatv = NULL;
  /**
   * first we load all the initial modules 
   **/
  while (curmod && curmod[0])
    {
      /* modules are separated by a semicolon ';' - this should be
         acceptable on Unixes and even Windows */
      nextmod = strchr (curmod, ';');
#if !HAVE_DOS_BASED_FILE_SYSTEM
      /* for convenience, on non DOS based systems like Unix-es and
         Linux, we also accept the colon ':' */
      if (!nextmod)
	nextmod = strchr (curmod, ':');
#endif
      if (nextmod)
	{
	  *nextmod = (char) 0;
	  nextmod++;
	}
      debugeprintf ("load_initial_melt_modules curmod %s before", curmod);
      MELT_LOCATION_HERE
	("load_initial_melt_modules before compile_dyn");
      if (curmod[0] == '@' && curmod[1])
	{
	  /* read the file which contains a list of modules, one per
	     non empty, non comment line */
	  modatv =
	    meltgc_load_modulelist ((melt_ptr_t) modatv, curmod + 1);
	  debugeprintf
	    ("load_initial_melt_modules curmod %s loaded modulist %p",
	     curmod, (void *) modatv);
	}
      else
	{
	  modatv = meltgc_load_melt_module ((melt_ptr_t) modatv, curmod);
	  debugeprintf
	    ("load_initial_melt_modules curmod %s loaded modatv %p",
	     curmod, (void *) modatv);
	}
      curmod = nextmod;
    }
  /**
   * then we do the command if needed 
   **/
  /* the command exit is builtin */
  if (modstr && !strcmp (modstr, "exit"))
    exit_after_options = true;
  /* other commands */
  else if (melt_get_inisysdata (FSYSDAT_CMD_FUNDICT) && modstr
	   && modstr[0])
    {
      debugeprintf
	("load_melt_modules_and_do_command sets exit_after_options for command %s",
	 modstr);
      MELT_LOCATION_HERE
	("load_initial_melt_modules before do_initial_command");
      do_initial_command ((melt_ptr_t) modatv);
      debugeprintf
	("load_melt_modules_and_do_command after do_initial_command  command_string %s",
	 modstr);
      if (dump_file == stderr && dbgstr)
	{
	  debugeprintf
	    ("load_melt_modules_and_do_command dump_file cleared was %p",
	     (void *) dump_file);
	  fflush (dump_file);
	  dump_file = 0;
	}
    }
  else if (modstr)
    fatal_error ("melt with command string %s without command dispatcher",
		 modstr);
  debugeprintf
    ("load_melt_modules_and_do_command ended with %ld GarbColl, %ld fullGc",
     melt_nb_garbcoll, melt_nb_full_garbcoll);
#if ENABLE_CHECKING
  if (melt_dbgtracefile)
    {
      fprintf (melt_dbgtracefile, "\n**END TRACE\n");
      fclose (melt_dbgtracefile);
      melt_dbgtracefile = NULL;
    }
#endif
  free (dupmodpath);
  debugeprintf
    ("load_melt_modules_and_do_command done modules %s command %s",
     inistr, modstr);
  MELT_EXITFRAME ();
#undef modatv
#undef dumpv
}


static void melt_ppl_error_handler(enum ppl_enum_error_code err, const char* descr);



/* handle a "melt" attribute
 */
static tree
handle_melt_attribute(tree *node, tree name,
		      tree args,
		      int flag ATTRIBUTE_UNUSED,
		      bool *no_add_attrs ATTRIBUTE_UNUSED)
{
  tree decl = *node;
  tree id = 0;
  const char* attrstr = 0;
  id = TREE_VALUE (args);
  if (TREE_CODE (id) != STRING_CST)
    {
      error ("melt attribute argument not a string");
      return NULL_TREE;
    }
  attrstr = TREE_STRING_POINTER (id);
  melt_handle_melt_attribute (decl, name, attrstr, input_location);
  return NULL_TREE;
}

static struct attribute_spec 
melt_attr_spec =
  { "melt",                   1, 1, true, false, false,
    handle_melt_attribute };


/* the plugin callback to register melt attributes */
static void 
melt_attribute_callback(void *gcc_data ATTRIBUTE_UNUSED,
			void* user_data ATTRIBUTE_UNUSED) 
{
  register_attribute(&melt_attr_spec);
}


/* the plugin callback when starting a compilation unit */
static void
melt_startunit_callback(void *gcc_data ATTRIBUTE_UNUSED,
			void* user_data ATTRIBUTE_UNUSED) 
{
  MELT_ENTERFRAME (1, NULL);
#define staclosv curfram__.varptr[0]
  staclosv = melt_get_inisysdata (FSYSDAT_UNIT_STARTER);
  if (melt_magic_discr ((melt_ptr_t) staclosv) == OBMAG_CLOSURE)
    {
      MELT_LOCATION_HERE
	("melt_startunit_callback before applying start unit closure");
      (void) melt_apply ((meltclosure_ptr_t) staclosv,
			 (melt_ptr_t) NULL, "", NULL, "", NULL);
      /* force a minor GC to be sure nothing stays in young region */
      melt_garbcoll (0, MELT_ONLY_MINOR);
    }
  MELT_EXITFRAME ();
#undef staclosv
}


/* the plugin callback when finishing a compilation unit */
static void
melt_finishunit_callback(void *gcc_data ATTRIBUTE_UNUSED,
			void* user_data ATTRIBUTE_UNUSED) 
{
  MELT_ENTERFRAME (1, NULL);
#define finclosv curfram__.varptr[0]
  finclosv = melt_get_inisysdata (FSYSDAT_UNIT_FINISHER);
  if (melt_magic_discr ((melt_ptr_t) finclosv) == OBMAG_CLOSURE)
    {
      MELT_LOCATION_HERE
	("melt_finishunit_callback before applying finish unit closure");
      (void) melt_apply ((meltclosure_ptr_t) finclosv,
			 (melt_ptr_t) NULL, "", NULL, "", NULL);
      /* force a minor GC to be sure nothing stays in young region */
      melt_garbcoll (0, MELT_ONLY_MINOR);
    }
  MELT_EXITFRAME ();
#undef finclosv
}



static void do_finalize_melt (void);


/* the plugin callback when finishing all */
static void
melt_finishall_callback(void *gcc_data ATTRIBUTE_UNUSED,
			void* user_data ATTRIBUTE_UNUSED) 
{
  do_finalize_melt ();
}


/****
 * Initialize melt.  Called from toplevel.c before pass management.
 * Should become the MELT plugin initializer.
 ****/
static void
melt_really_initialize (const char* pluginame)
{
  static int inited;
  long seed;
  const char *pc;
  const char *randomseed = 0;
  const char *modstr = 0;
  const char *inistr = 0;
  const char *countdbgstr = 0;
  if (inited)
    return;
  gcc_assert(pluginame && pluginame[0]);
  melt_plugin_name = xstrdup(pluginame);
  modstr = melt_argument ("mode");
  inistr = melt_argument ("init");
  countdbgstr = melt_argument ("debugskip");
#ifdef MELT_IS_PLUGIN
  { 
    const char *dbgstr = melt_argument ("debug");
    /* debug=n or debug=0 is handled as no debug */
    if (dbgstr && (!dbgstr[0] || !strchr("Nn0", dbgstr[0])))
      flag_melt_debug = 1;
  }
#endif
  if (melt_minorsizekilow == 0)
    {
      const char* minzstr = melt_argument ("minor-zone");
      melt_minorsizekilow = minzstr ? (atol (minzstr)) : 0;
      if (melt_minorsizekilow<256) melt_minorsizekilow=256;
      else if (melt_minorsizekilow>16384) melt_minorsizekilow=16384;
    }
  modinfvec = VEC_alloc (melt_module_info_t, heap, 32);
  /* don't use the index 0 so push a null */
  VEC_safe_push (melt_module_info_t, heap, modinfvec,
		 (melt_module_info_t *) 0);
  proghandle = dlopen (NULL, RTLD_NOW | RTLD_GLOBAL);
  if (!proghandle)
    fatal_error ("melt failed to get whole program handle - %s",
		 dlerror ());
  if (countdbgstr != (char *) 0)
    melt_debugskipcount = atol (countdbgstr);
  seed = 0;
  randomseed = get_random_seed (false);
  gcc_assert (randomseed != (char *) 0);
  gcc_assert (MELT_ALIGN == sizeof (void *)
	      || MELT_ALIGN == 2 * sizeof (void *)
	      || MELT_ALIGN == 4 * sizeof (void *));
  inited = 1;
  ggc_collect ();
  obstack_init (&bstring_obstack);
  obstack_init (&bname_obstack);
  for (pc = randomseed; *pc; pc++)
    seed ^= (seed << 6) + (*pc);
  srand48 (seed);
  gcc_assert (!melt_curalz);
  {
    size_t wantedwords = melt_minorsizekilow * 4096;
    if (wantedwords < (1 << 20))
      wantedwords = (1 << 20);
    gcc_assert (melt_startalz == NULL && melt_endalz == NULL);
    gcc_assert (wantedwords * sizeof (void *) >
		300 * MELTGLOB__LASTGLOB * sizeof (struct meltobject_st));
    melt_curalz = (char *) xcalloc (sizeof (void *), wantedwords);
    melt_startalz = melt_curalz;
    melt_endalz = (char *) melt_curalz + wantedwords * sizeof (void *);
    melt_storalz = ((void **) melt_endalz) - 2;
    melt_newspeclist = NULL;
    melt_oldspeclist = NULL;
    debugeprintf ("melt_really_initialize alloczon %p - %p (%ld Kw)",
		  melt_startalz, melt_endalz, (long) wantedwords >> 10);
  }
  /* we are using register_callback here, even if MELT is not compiled as a plugin. */
  register_callback (melt_plugin_name, PLUGIN_GGC_MARKING, 
		     melt_marking_callback,
		     NULL);
  register_callback (melt_plugin_name, PLUGIN_ATTRIBUTES,
		     melt_attribute_callback,
		     NULL);
  register_callback (melt_plugin_name, PLUGIN_START_UNIT,
		     melt_startunit_callback,
		     NULL);
  register_callback (melt_plugin_name, PLUGIN_FINISH_UNIT,
		     melt_finishunit_callback,
		     NULL);
  register_callback (melt_plugin_name, PLUGIN_FINISH,
		     melt_finishall_callback,
		     NULL);
  debugeprintf ("melt_really_initialize cpp_PREFIX=%s", cpp_PREFIX);
  debugeprintf ("melt_really_initialize cpp_EXEC_PREFIX=%s", cpp_EXEC_PREFIX);
  debugeprintf ("melt_really_initialize gcc_exec_prefix=%s", gcc_exec_prefix);
  debugeprintf ("melt_really_initialize melt_private_include_dir=%s",
		melt_private_include_dir);
  debugeprintf ("melt_really_initialize melt_source_dir=%s", melt_source_dir);
  debugeprintf ("melt_really_initialize melt_module_dir=%s", melt_module_dir);
  debugeprintf ("melt_really_initialize inistr=%s", inistr);
  if (ppl_set_error_handler(melt_ppl_error_handler))
    fatal_error ("failed to set PPL handler");
  load_melt_modules_and_do_command ();
  debugeprintf ("melt_really_initialize ended init=%s command=%s",
		inistr, modstr);
}



typedef char *char_p;

DEF_VEC_P (char_p);
DEF_VEC_ALLOC_P (char_p, heap);

static void
do_finalize_melt (void)
{
  static int didfinal;
  MELT_ENTERFRAME (1, NULL);
  if (didfinal++>0)
    goto end;
#define finclosv curfram__.varptr[0]
  finclosv = melt_get_inisysdata (FSYSDAT_EXIT_FINALIZER);
  if (melt_magic_discr ((melt_ptr_t) finclosv) == OBMAG_CLOSURE)
    {
      MELT_LOCATION_HERE
	("do_finalize_melt before applying final closure");
      (void) melt_apply ((meltclosure_ptr_t) finclosv,
			    (melt_ptr_t) NULL, "", NULL, "", NULL);
      /* force a minor GC to be sure nothing stays in young region */
      melt_garbcoll (0, MELT_ONLY_MINOR);
    }
  if (gdbm_melt)
    {
      gdbm_close (gdbm_melt);
      gdbm_melt = NULL;
    }
  if (tempdir_melt[0])
    {
      DIR *tdir = opendir (tempdir_melt);
      VEC (char_p, heap) * dirvec = 0;
      struct dirent *dent = 0;
      if (!tdir)
	fatal_error ("failed to open tempdir %s %m", tempdir_melt);
      dirvec = VEC_alloc (char_p, heap, 30);
      while ((dent = readdir (tdir)) != NULL)
	{
	  if (dent->d_name[0] && dent->d_name[0] != '.')
	    /* this skips  '.' & '..' and we have no  .* file */
	    VEC_safe_push (char_p, heap, dirvec,
			   concat (tempdir_melt, "/", dent->d_name, NULL));
	}
      closedir (tdir);
      while (!VEC_empty (char_p, dirvec))
	{
	  char *tfilnam = VEC_pop (char_p, dirvec);
	  debugeprintf ("melt_finalize remove file %s", tfilnam);
	  remove (tfilnam);
	  free (tfilnam);
	};
      VEC_free (char_p, heap, dirvec);
    }
  if (made_tempdir_melt && tempdir_melt[0])
    {
      errno = 0;
      if (rmdir (tempdir_melt))
	/* @@@ I don't know if it should be a warning or a fatal error -
	   we are finalizing! */
	warning (0, "failed to rmdir melt tempdir %s (%s)",
		 tempdir_melt, strerror (errno));
    }
 end:
  MELT_EXITFRAME ();
#undef finclosv
}

#ifdef MELT_IS_PLUGIN
/* this code is GPLv3 licenced & FSF copyrighted, so of course it is a
   GPL compatible GCC plugin. */
int plugin_is_GPL_compatible = 1;

/* the plugin initialization code has to be exactly plugin_init */
int
plugin_init(struct plugin_name_args* plugin_info,
	    struct plugin_gcc_version* version) 
{
  gcc_assert (plugin_info);
  melt_plugin_argc = plugin_info->argc;
  melt_plugin_argv = plugin_info->argv;
  melt_really_initialize (plugin_info->base_name);
  debugeprintf ("end of melt plugin_init");
  return 0; /* success */
}

#else
void
melt_initialize (void)
{
  melt_really_initialize ("__MELT/buitin");
  debugeprintf ("end of melt_initialize [builtin MELT] meltruntime %s", __DATE__);
}
#endif


int *
melt_dynobjstruct_fieldoffset_at (const char *fldnam, const char *fil,
				     int lin)
{
  char *nam = 0;
  void *ptr = 0;
  nam = concat ("fieldoff__", fldnam, NULL);
  ptr = melt_dlsym_all (nam);
  if (!ptr)
    fatal_error ("melt failed to find field offset %s - %s (%s:%d)", nam,
		 dlerror (), fil, lin);
  free (nam);
  return (int *) ptr;
}


int *
melt_dynobjstruct_classlength_at (const char *clanam, const char *fil,
				     int lin)
{
  char *nam = 0;
  void *ptr = 0;
  nam = concat ("classlen__", clanam, NULL);
  ptr = melt_dlsym_all (nam);
  if (!ptr)
    fatal_error ("melt failed to find class length %s - %s (%s:%d)", nam,
		 dlerror (), fil, lin);
  free (nam);
  return (int *) ptr;
}


/****
 * finalize melt. Called from toplevel.c after all is done
 ****/
void
melt_finalize (void)
{
  do_finalize_melt ();
  debugeprintf ("melt_finalize with %ld GarbColl, %ld fullGc",
		melt_nb_garbcoll, melt_nb_full_garbcoll);
}




static void
discr_out (struct debugprint_melt_st *dp, meltobject_ptr_t odiscr)
{
  int dmag = melt_magic_discr ((melt_ptr_t) odiscr);
  struct meltstring_st *str = NULL;
  if (dmag != OBMAG_OBJECT)
    {
      fprintf (dp->dfil, "?discr@%p?", (void *) odiscr);
      return;
    }
  if (odiscr->obj_len >= FNAMED__LAST && odiscr->obj_vartab)
    {
      str = (struct meltstring_st *) odiscr->obj_vartab[FNAMED_NAME];
      if (melt_magic_discr ((melt_ptr_t) str) != OBMAG_STRING)
	str = NULL;
    }
  if (!str)
    {
      fprintf (dp->dfil, "?odiscr/%d?", odiscr->obj_hash);
      return;
    }
  fprintf (dp->dfil, "#%s", str->val);
}


static void
nl_debug_out (struct debugprint_melt_st *dp, int depth)
{
  int i;
  putc ('\n', dp->dfil);
  for (i = 0; i < depth; i++)
    putc (' ', dp->dfil);
}

static void
skip_debug_out (struct debugprint_melt_st *dp, int depth)
{
  if (dp->dcount % 4 == 0)
    nl_debug_out (dp, depth);
  else
    putc (' ', dp->dfil);
}


static bool
is_named_obj (meltobject_ptr_t ob)
{
  struct meltstring_st *str = 0;
  if (melt_magic_discr ((melt_ptr_t) ob) != OBMAG_OBJECT)
    return FALSE;
  if (ob->obj_len < FNAMED__LAST || !ob->obj_vartab)
    return FALSE;
  str = (struct meltstring_st *) ob->obj_vartab[FNAMED_NAME];
  if (melt_magic_discr ((melt_ptr_t) str) != OBMAG_STRING)
    return FALSE;
  if (melt_is_instance_of ((melt_ptr_t) ob, (melt_ptr_t) MELT_PREDEF (CLASS_NAMED)))
    return TRUE;
  return FALSE;
}

static void
debug_outstr (struct debugprint_melt_st *dp, const char *str)
{
  int nbclin = 0;
  const char *pc;
  for (pc = str; *pc; pc++)
    {
      nbclin++;
      if (nbclin > 60 && strlen (pc) > 5)
	{
	  if (ISSPACE (*pc) || ISPUNCT (*pc) || nbclin > 72)
	    {
	      fputs ("\\\n", dp->dfil);
	      nbclin = 0;
	    }
	}
      switch (*pc)
	{
	case '\n':
	  fputs ("\\n", dp->dfil);
	  break;
	case '\r':
	  fputs ("\\r", dp->dfil);
	  break;
	case '\t':
	  fputs ("\\t", dp->dfil);
	  break;
	case '\v':
	  fputs ("\\v", dp->dfil);
	  break;
	case '\f':
	  fputs ("\\f", dp->dfil);
	  break;
	case '\"':
	  fputs ("\\q", dp->dfil);
	  break;
	case '\'':
	  fputs ("\\a", dp->dfil);
	  break;
	default:
	  if (ISPRINT (*pc))
	    putc (*pc, dp->dfil);
	  else
	    fprintf (dp->dfil, "\\x%02x", (*pc) & 0xff);
	  break;
	}
    }
}


void
melt_debug_out (struct debugprint_melt_st *dp,
		   melt_ptr_t ptr, int depth)
{
  int mag = melt_magic_discr (ptr);
  int ix;
  if (!dp->dfil)
    return;
  dp->dcount++;
  switch (mag)
    {
    case 0:
      {
	if (ptr)
	  fprintf (dp->dfil, "??@%p??", (void *) ptr);
	else
	  fputs ("@@", dp->dfil);
	break;
      }
    case OBMAG_OBJECT:
      {
	struct meltobject_st *p = (struct meltobject_st *) ptr;
	bool named = is_named_obj (p);
	fputs ("%", dp->dfil);
	discr_out (dp, p->obj_class);
	fprintf (dp->dfil, "/L%dH%d", p->obj_len, p->obj_hash);
	if (p->obj_num)
	  fprintf (dp->dfil, "N%d", p->obj_num);
#if ENABLE_CHECKING
	if (p->obj_serial)
	  fprintf (dp->dfil, "S##%ld", p->obj_serial);
#endif
	if (named)
	  fprintf (dp->dfil, "<#%s>",
		   ((struct meltstring_st *) (p->obj_vartab
						 [FNAMED_NAME]))->val);
	if ((!named || depth == 0) && depth < dp->dmaxdepth)
	  {
	    fputs ("[", dp->dfil);
	    if (p->obj_vartab)
	      for (ix = 0; ix < p->obj_len; ix++)
		{
		  if (ix > 0)
		    skip_debug_out (dp, depth);
		  melt_debug_out (dp, p->obj_vartab[ix], depth + 1);
		}
	    fputs ("]", dp->dfil);
	  }
	else if (!named)
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_MULTIPLE:
      {
	struct meltmultiple_st *p = (struct meltmultiple_st *) ptr;
	fputs ("*", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    fputs ("(", dp->dfil);
	    for (ix = 0; ix < (int) p->nbval; ix++)
	      {
		if (ix > 0)
		  skip_debug_out (dp, depth);
		melt_debug_out (dp, p->tabval[ix], depth + 1);
	      }
	    fputs (")", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_STRING:
      {
	struct meltstring_st *p = (struct meltstring_st *) ptr;
	fputs ("!", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    fputs ("\"", dp->dfil);
	    debug_outstr (dp, p->val);
	    fputs ("\"", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_INT:
      {
	struct meltint_st *p = (struct meltint_st *) ptr;
	fputs ("!", dp->dfil);
	discr_out (dp, p->discr);
	fprintf (dp->dfil, "#%ld", p->val);
	break;
      }
    case OBMAG_MIXINT:
      {
	struct meltmixint_st *p = (struct meltmixint_st *) ptr;
	fputs ("!", dp->dfil);
	discr_out (dp, p->discr);
	fprintf (dp->dfil, "[#%ld&", p->intval);
	melt_debug_out (dp, p->ptrval, depth + 1);
	fputs ("]", dp->dfil);
	break;
      }
    case OBMAG_MIXLOC:
      {
	struct meltmixloc_st *p = (struct meltmixloc_st *) ptr;
	fputs ("!", dp->dfil);
	discr_out (dp, p->discr);
	fprintf (dp->dfil, "[#%ld&", p->intval);
	melt_debug_out (dp, p->ptrval, depth + 1);
	fputs ("]", dp->dfil);
	break;
      }
    case OBMAG_LIST:
      {
	struct meltlist_st *p = (struct meltlist_st *) ptr;
	fputs ("!", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    int ln = melt_list_length ((melt_ptr_t) p);
	    struct meltpair_st *pr = 0;
	    if (ln > 2)
	      fprintf (dp->dfil, "[/%d ", ln);
	    else
	      fputs ("[", dp->dfil);
	    for (pr = p->first;
		 pr && melt_magic_discr ((melt_ptr_t) pr) == OBMAG_PAIR;
		 pr = pr->tl)
	      {
		melt_debug_out (dp, pr->hd, depth + 1);
		if (pr->tl)
		  skip_debug_out (dp, depth);
	      }
	    fputs ("]", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_MAPSTRINGS:
      {
	struct meltmapstrings_st *p = (struct meltmapstrings_st *) ptr;
	fputs ("|", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    int ln = melt_primtab[p->lenix];
	    fprintf (dp->dfil, "{~%d/", p->count);
	    if (p->entab)
	      for (ix = 0; ix < ln; ix++)
		{
		  const char *ats = p->entab[ix].e_at;
		  if (!ats || ats == HTAB_DELETED_ENTRY)
		    continue;
		  nl_debug_out (dp, depth);
		  fputs ("'", dp->dfil);
		  debug_outstr (dp, ats);
		  fputs ("' = ", dp->dfil);
		  melt_debug_out (dp, p->entab[ix].e_va, depth + 1);
		  fputs (";", dp->dfil);
		}
	    fputs (" ~}", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_MAPOBJECTS:
      {
	struct meltmapobjects_st *p = (struct meltmapobjects_st *) ptr;
	fputs ("|", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    int ln = melt_primtab[p->lenix];
	    fprintf (dp->dfil, "{%d/", p->count);
	    if (p->entab)
	      for (ix = 0; ix < ln; ix++)
		{
		  meltobject_ptr_t atp = p->entab[ix].e_at;
		  if (!atp || atp == HTAB_DELETED_ENTRY)
		    continue;
		  nl_debug_out (dp, depth);
		  melt_debug_out (dp, (melt_ptr_t) atp, dp->dmaxdepth);
		  fputs ("' = ", dp->dfil);
		  melt_debug_out (dp, p->entab[ix].e_va, depth + 1);
		  fputs (";", dp->dfil);
		}
	    fputs (" }", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_CLOSURE:
      {
	struct meltclosure_st *p = (struct meltclosure_st *) ptr;
	fputs ("!.", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    fprintf (dp->dfil, "[. rout=");
	    melt_debug_out (dp, (melt_ptr_t) p->rout, depth + 1);
	    skip_debug_out (dp, depth);
	    fprintf (dp->dfil, " /%d: ", p->nbval);
	    for (ix = 0; ix < (int) p->nbval; ix++)
	      {
		if (ix > 0)
		  skip_debug_out (dp, depth);
		melt_debug_out (dp, p->tabval[ix], depth + 1);
	      }
	    fputs (".]", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_ROUTINE:
      {
	struct meltroutine_st *p = (struct meltroutine_st *) ptr;
	fputs ("!:", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    fprintf (dp->dfil, ".%s[:/%d ", p->routdescr, p->nbval);
	    for (ix = 0; ix < (int) p->nbval; ix++)
	      {
		if (ix > 0)
		  skip_debug_out (dp, depth);
		melt_debug_out (dp, p->tabval[ix], depth + 1);
	      }
	    fputs (":]", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_STRBUF:
      {
	struct meltstrbuf_st *p = (struct meltstrbuf_st *) ptr;
	fputs ("!`", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    fprintf (dp->dfil, "[`buflen=%ld ", melt_primtab[p->buflenix]);
	    gcc_assert (p->bufstart <= p->bufend
			&& p->bufend < melt_primtab[p->buflenix]);
	    fprintf (dp->dfil, "bufstart=%u bufend=%u buf='",
		     p->bufstart, p->bufend);
	    if (p->bufzn)
	      debug_outstr (dp, p->bufzn + p->bufstart);
	    fputs ("' `]", dp->dfil);
	  }
	else
	  fputs ("..", dp->dfil);
	break;
      }
    case OBMAG_PAIR:
      {
	struct meltpair_st *p = (struct meltpair_st *) ptr;
	fputs ("[pair:", dp->dfil);
	discr_out (dp, p->discr);
	if (depth < dp->dmaxdepth)
	  {
	    fputs ("hd:", dp->dfil);
	    melt_debug_out (dp, p->hd, depth + 1);
	    fputs ("; ti:", dp->dfil);
	    melt_debug_out (dp, (melt_ptr_t) p->tl, depth + 1);
	  }
	else
	  fputs ("..", dp->dfil);
	fputs ("]", dp->dfil);
	break;
      }
    case OBMAG_TRIPLE:
    case OBMAG_TREE:
    case OBMAG_GIMPLE:
    case OBMAG_GIMPLESEQ:
    case OBMAG_BASICBLOCK:
    case OBMAG_EDGE:
    case OBMAG_MAPTREES:
    case OBMAG_MAPGIMPLES:
    case OBMAG_MAPGIMPLESEQS:
    case OBMAG_MAPBASICBLOCKS:
    case OBMAG_MAPEDGES:
    case OBMAG_DECAY:
      fatal_error ("debug_out unimplemented magic %d", mag);
    default:
      fatal_error ("debug_out invalid magic %d", mag);
    }
}



void
melt_dbgeprint (void *p)
{
  struct debugprint_melt_st dps = {
    0, 4, 0
  };
  dps.dfil = stderr;
  melt_debug_out (&dps, (melt_ptr_t) p, 0);
  putc ('\n', stderr);
  fflush (stderr);
}


void meltgc_debugmsgval(void* val_p, const char*msg, long count)
{ 
  MELT_ENTERFRAME(2,NULL);
#define valv   curfram__.varptr[0]
#define dbgfv  curfram__.varptr[1]
  valv = val_p;
  dbgfv = melt_get_inisysdata (FSYSDAT_DEBUGMSG);
  {
    union meltparam_un argtab[2];
    memset(argtab, 0, sizeof(argtab));
    argtab[0].bp_cstring = msg;
    argtab[1].bp_long = count;
    (void) melt_apply ((meltclosure_ptr_t) dbgfv, (melt_ptr_t)valv, 
			  BPARSTR_CSTRING BPARSTR_LONG, argtab, "", NULL);
  }
  MELT_EXITFRAME();
#undef valv
#undef dbgfv
}

void
melt_dbgbacktrace (int depth)
{
  int curdepth = 1, totdepth = 0;
  struct callframe_melt_st *fr = 0;
  fprintf (stderr, "    <{\n");
  for (fr = melt_topframe; fr != NULL && curdepth < depth;
       (fr = fr->prev), (curdepth++))
    {
      fprintf (stderr, "frame#%d closure: ", curdepth);
#if ENABLE_CHECKING
      if (fr->flocs)
	fprintf (stderr, "{%s} ", fr->flocs);
      else
	fputs (" ", stderr);
#endif
      melt_dbgeprint (fr->clos);
    }
  for (totdepth = curdepth; fr != NULL; fr = fr->prev);
  fprintf (stderr, "}> backtraced %d frames of %d\n", curdepth, totdepth);
  fflush (stderr);
}


void
melt_dbgshortbacktrace (const char *msg, int maxdepth)
{
  int curdepth = 1;
  struct callframe_melt_st *fr = 0;
  if (maxdepth < 2)
    maxdepth = 2;
  fprintf (stderr, "\nSHORT BACKTRACE[#%ld] %s;", melt_dbgcounter,
	   msg ? msg : "/");
  for (fr = melt_topframe; fr != NULL && curdepth < maxdepth;
       (fr = fr->prev), (curdepth++))
    {
      fputs ("\n", stderr);
      fprintf (stderr, "#%d:", curdepth);
      if (melt_magic_discr ((melt_ptr_t) fr->clos) == OBMAG_CLOSURE)
	{
	  meltroutine_ptr_t curout = fr->clos->rout;
	  if (melt_magic_discr ((melt_ptr_t) curout) == OBMAG_ROUTINE)
	    fprintf (stderr, "<%s> ", curout->routdescr);
	  else
	    fputs ("?norout?", stderr);
	}
      else
	fprintf (stderr, "_ ");
#if ENABLE_CHECKING
      if (fr->flocs)
	fprintf (stderr, "{%s} ", fr->flocs);
      else
	fputs (" ", stderr);
#endif
    };
  if (fr)
    fprintf (stderr, "...&%d", maxdepth - curdepth);
  else
    fputs (".", stderr);
  putc ('\n', stderr);
  putc ('\n', stderr);
  fflush (stderr);
}


/*****************
 * state GDBM access
 ****************/

static const char*meltgdbmstate;

static void
fatal_gdbm (char *msg)
{
  if (!meltgdbmstate)
    meltgdbmstate = melt_argument ("gdbmstate");
  fatal_error ("fatal melt GDBM (%s) error : %s", meltgdbmstate,
	       msg);
}

static inline GDBM_FILE
get_melt_gdbm (void)
{
  if (!meltgdbmstate)
    meltgdbmstate = melt_argument ("gdbmstate");
  if (gdbm_melt)
    return gdbm_melt;
  if (!meltgdbmstate || !meltgdbmstate[0])
    return NULL;
  gdbm_melt =
    gdbm_open (CONST_CAST(char*, meltgdbmstate), 8192, GDBM_WRCREAT, 0600,
	       fatal_gdbm);
  if (!gdbm_melt)
    fatal_error ("failed to lazily open melt GDBM (%s) - %m",
		 meltgdbmstate);
  return gdbm_melt;
}

bool
melt_has_gdbmstate (void)
{
  return get_melt_gdbm () != NULL;
}

melt_ptr_t
meltgc_fetch_gdbmstate_constr (const char *key)
{
  datum keydata = { 0, 0 };
  datum valdata = { 0, 0 };
  GDBM_FILE dbf = get_melt_gdbm ();
  MELT_ENTERFRAME (1, NULL);
#define restrv curfram__.varptr[0]
  if (!meltgdbmstate)
    meltgdbmstate = melt_argument ("gdbmstate");
  if (!dbf || !key || !key[0])
    goto end;
  keydata.dptr = CONST_CAST (char *, key);
  keydata.dsize = strlen (key);
  valdata = gdbm_fetch (dbf, keydata);
  if (valdata.dptr != NULL && valdata.dsize >= 0)
    {
      gcc_assert ((int) valdata.dsize == (int) strlen (valdata.dptr));
      restrv = meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), valdata.dptr);
      free (valdata.dptr);
      valdata.dptr = 0;
    }
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) restrv;
#undef restrv
}

melt_ptr_t
meltgc_fetch_gdbmstate (melt_ptr_t key_p)
{
  datum keydata = { 0, 0 };
  datum valdata = { 0, 0 };
  int keymagic = 0;
  GDBM_FILE dbf = get_melt_gdbm ();
  MELT_ENTERFRAME (3, NULL);
#define restrv curfram__.varptr[0]
#define keyv   curfram__.varptr[1]
#define kstrv  curfram__.varptr[2]
  keyv = key_p;
  if (!meltgdbmstate)
    meltgdbmstate = melt_argument ("gdbmstate");
  if (!dbf || !keyv)
    goto end;
  keymagic = melt_magic_discr ((melt_ptr_t) keyv);
  if (keymagic == OBMAG_STRING)
    {
      keydata.dptr = CONST_CAST (char *, melt_string_str ((melt_ptr_t) keyv));
      keydata.dsize = strlen (keydata.dptr);
      valdata = gdbm_fetch (dbf, keydata);
    }
  else if (keymagic == OBMAG_STRBUF)
    {
      keydata.dptr = CONST_CAST(char*, melt_strbuf_str ((melt_ptr_t) keyv));
      keydata.dsize = strlen (keydata.dptr);
      valdata = gdbm_fetch (dbf, keydata);
    }
  else if (keymagic == OBMAG_OBJECT
	   && melt_is_instance_of ((melt_ptr_t) keyv,
				      (melt_ptr_t) MELT_PREDEF (CLASS_NAMED)))
    {
      kstrv = melt_field_object ((melt_ptr_t) keyv, FNAMED_NAME);
      if (melt_magic_discr ((melt_ptr_t) kstrv) == OBMAG_STRING)
	{
	  keydata.dptr = CONST_CAST(char*, melt_string_str ((melt_ptr_t) kstrv));
	  keydata.dsize = strlen (keydata.dptr);
	  valdata = gdbm_fetch (dbf, keydata);
	}
      else
	goto end;
    }
  else
    goto end;
  if (valdata.dptr != NULL && valdata.dsize >= 0)
    {
      gcc_assert ((int) valdata.dsize == (int) strlen (valdata.dptr));
      restrv = meltgc_new_string ((meltobject_ptr_t) MELT_PREDEF (DISCR_STRING), valdata.dptr);
      free (valdata.dptr);
      valdata.dptr = 0;
    }
end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) restrv;
#undef keyv
#undef restrv
#undef kstrv
}



void
meltgc_put_gdbmstate_constr (const char *key, melt_ptr_t data_p)
{
  datum keydata = { 0, 0 };
  datum valdata = { 0, 0 };
  int datamagic = 0;
  GDBM_FILE dbf = get_melt_gdbm ();
  MELT_ENTERFRAME (2, NULL);
#define datav  curfram__.varptr[0]
#define dstrv  curfram__.varptr[1]
  if (!meltgdbmstate)
    meltgdbmstate = melt_argument ("gdbmstate");
  datav = data_p;
  if (!dbf || !key || !key[0])
    goto end;
  keydata.dptr = CONST_CAST(char *, key);
  keydata.dsize = strlen (key);
  if (!datav)
    {
      gdbm_delete (dbf, keydata);
      goto end;
    }
  datamagic = melt_magic_discr ((melt_ptr_t) datav);
  if (datamagic == OBMAG_STRING)
    {
      valdata.dptr = CONST_CAST(char *, melt_string_str ((melt_ptr_t) datav));
      valdata.dsize = strlen (keydata.dptr);
      gdbm_store (dbf, keydata, valdata, GDBM_REPLACE);
    }
  else if (datamagic == OBMAG_STRBUF)
    {
      valdata.dptr = CONST_CAST(char*, melt_strbuf_str ((melt_ptr_t) datav));
      valdata.dsize = strlen (keydata.dptr);
      gdbm_store (dbf, keydata, valdata, GDBM_REPLACE);
    }
  else if (datamagic == OBMAG_OBJECT
	   && melt_is_instance_of ((melt_ptr_t) datav,
				      (melt_ptr_t) MELT_PREDEF (CLASS_NAMED)))
    {
      dstrv = melt_field_object ((melt_ptr_t) datav, FNAMED_NAME);
      if (melt_magic_discr ((melt_ptr_t) dstrv) == OBMAG_STRING)
	{
	  valdata.dptr = CONST_CAST (char *, melt_string_str ((melt_ptr_t) dstrv));
	  valdata.dsize = strlen (keydata.dptr);
	  gdbm_store (dbf, keydata, valdata, GDBM_REPLACE);
	}
      else
	goto end;
    }
  else
    goto end;
end:
  MELT_EXITFRAME ();
#undef datav
#undef dstrv
}


void
meltgc_put_gdbmstate (melt_ptr_t key_p, melt_ptr_t data_p)
{
  datum keydata = { 0, 0 };
  datum valdata = { 0, 0 };
  int keymagic = 0;
  int datamagic = 0;
  GDBM_FILE dbf = get_melt_gdbm ();
  MELT_ENTERFRAME (4, NULL);
#define keyv   curfram__.varptr[0]
#define kstrv  curfram__.varptr[1]
#define datav  curfram__.varptr[2]
#define dstrv  curfram__.varptr[3]
  keyv = key_p;
  datav = data_p;
  if (!meltgdbmstate)
    meltgdbmstate = melt_argument ("gdbmstate");
  if (!dbf || !keyv)
    goto end;
  keymagic = melt_magic_discr ((melt_ptr_t) keyv);
  if (keymagic == OBMAG_STRING)
    {
      keydata.dptr = CONST_CAST(char*, melt_string_str ((melt_ptr_t) keyv));
      keydata.dsize = strlen (keydata.dptr);
    }
  else if (keymagic == OBMAG_STRBUF)
    {
      keydata.dptr = CONST_CAST(char*, melt_strbuf_str ((melt_ptr_t) keyv));
      keydata.dsize = strlen (keydata.dptr);
    }
  else if (keymagic == OBMAG_OBJECT
	   && melt_is_instance_of ((melt_ptr_t) keyv,
				      (melt_ptr_t) MELT_PREDEF (CLASS_NAMED)))
    {
      kstrv = melt_field_object ((melt_ptr_t) keyv, FNAMED_NAME);
      if (melt_magic_discr ((melt_ptr_t) kstrv) == OBMAG_STRING)
	{
	  keydata.dptr = CONST_CAST(char*, melt_string_str ((melt_ptr_t) kstrv));
	  keydata.dsize = strlen (keydata.dptr);
	}
      else
	goto end;
    }
  else
    goto end;
  if (!datav)
    {
      gdbm_delete (dbf, keydata);
      goto end;
    }
  datamagic = melt_magic_discr ((melt_ptr_t) datav);
  if (datamagic == OBMAG_STRING)
    {
      valdata.dptr = CONST_CAST(char*, melt_string_str ((melt_ptr_t) datav));
      valdata.dsize = strlen (keydata.dptr);
      gdbm_store (dbf, keydata, valdata, GDBM_REPLACE);
    }
  else if (datamagic == OBMAG_STRBUF)
    {
      valdata.dptr = CONST_CAST(char*, melt_strbuf_str ((melt_ptr_t) datav));
      valdata.dsize = strlen (keydata.dptr);
      gdbm_store (dbf, keydata, valdata, GDBM_REPLACE);
    }
  else if (datamagic == OBMAG_OBJECT
	   && melt_is_instance_of ((melt_ptr_t) datav,
				      (melt_ptr_t) MELT_PREDEF (CLASS_NAMED)))
    {
      dstrv = melt_field_object ((melt_ptr_t) datav, FNAMED_NAME);
      if (melt_magic_discr ((melt_ptr_t) dstrv) == OBMAG_STRING)
	{
	  valdata.dptr = CONST_CAST(char*, melt_string_str ((melt_ptr_t) dstrv));
	  valdata.dsize = strlen (keydata.dptr);
	  gdbm_store (dbf, keydata, valdata, GDBM_REPLACE);
	}
      else
	goto end;
    }
  else
    goto end;
end:
  MELT_EXITFRAME ();
#undef keyv
#undef kstrv
#undef datav
#undef dstrv
}

/* wrapping gimple & tree prettyprinting for MELT debug */


/* we really need in memory FILE* output; GNU libc -ie Linux- provides
   open_memstream for that; on other systems we use a temporary file,
   which would be very slow if it happens to not be cached in
   memory */

static char* meltppbuffer;
static size_t meltppbufsiz;
static FILE* meltppfile;

#if !HAVE_OPEN_MEMSTREAM
static char* meltppfilename;
#endif

/* open the melttppfile for pretty printing */
static void 
open_meltpp_file(void)
{
#if HAVE_OPEN_MEMSTREAM
  meltppbufsiz = 1024;
  meltppbuffer = xcalloc (1, meltppbufsiz);
  meltppfile = open_memstream (&meltppbuffer, &meltppsiz);
  if (!meltppfile)
    fatal_error ("failed to open meltpp file in memory");
#else
  if (!meltppfilename) 
    {
#ifdef MELT_IS_PLUGIN
      /* in plugin mode, make_temp_file is not available from cc1,
	 because make_temp_file is defined in libiberty.a and cc1 does
	 not use make_temp_file so do not load the make_temp_file.o
	 member of the static library libiberty!
	 See also http://gcc.gnu.org/ml/gcc/2009-07/msg00157.html
      */
      static char ourtempnamebuf[L_tmpnam+1];
      int tfd = -1;
      strcpy (ourtempnamebuf, "/tmp/meltemp_XXXXXX");
      tfd = mkstemp (ourtempnamebuf);
      if (tfd>=0)
	meltppfilename = ourtempnamebuf;
      else
	fatal_error ("melt temporary file: mkstemp %s failed", ourtempnamebuf);
#else
      meltppfilename = make_temp_file (".meltmem");
      if (!meltppfilename)
	fatal_error ("failed to get melt memory temporary file");
#endif
    }
  meltppfile = fopen (meltppfilename, "w+");
#endif
}

/* close the meltppfile for pretty printing; after than, the
   meltppbuffer & meltppbufsize contains the FILE* content */
static void
close_meltpp_file(void)
{
  gcc_assert (meltppfile != (FILE*)0);
#if HAVE_OPEN_MEMSTREAM
  /* the fclose automagically updates meltppbuffer & meltppbufsiz */
  fclose (meltppfile);
#else
  /* we don't have an in-memory FILE*; so we read the file; you'll
     better have it in a fast file system, like a memory one. */
  meltppbufsiz = (size_t) ftell (meltppfile);
  rewind (meltppfile);
  meltppbuffer = (char*) xcalloc(1, meltppbufsiz);
  if (fread (meltppbuffer, meltppbufsiz, 1, meltppfile) <= 0)
    fatal_error ("failed to re-read melt buffer temporary file");
  fclose (meltppfile);
#endif
  meltppfile = NULL;
}



/* pretty print into an outbuf a gimple */
void
meltgc_ppout_gimple (melt_ptr_t out_p, int indentsp, gimple gstmt)
{
  int outmagic = 0;
#define outv curfram__.varptr[0]
  MELT_ENTERFRAME (2, NULL);
  outv = out_p;
  if (!outv) 
    goto end;
  outmagic = melt_magic_discr ((melt_ptr_t) outv);
  if (!gstmt)
    {
      meltgc_add_out ((melt_ptr_t) outv,
			    "%nullgimple%");
      goto end;
    }
  switch (outmagic) 
    {
    case OBMAG_STRBUF:
      {
	open_meltpp_file ();
	print_gimple_stmt (meltppfile, gstmt, indentsp,
			   TDF_LINENO | TDF_SLIM | TDF_VOPS);
	close_meltpp_file ();
	meltgc_add_out_raw_len ((melt_ptr_t) outv, meltppbuffer, (int) meltppbufsiz);
	free(meltppbuffer);
	meltppbuffer = 0;
	meltppbufsiz = 0;
      }
      break;
    case OBMAG_SPEC_FILE:
    case OBMAG_SPEC_RAWFILE:
      {
	FILE* f = ((struct meltspecial_st*)outv)->val.sp_file;
	if (!f) 
	  goto end;
	print_gimple_stmt (f, gstmt, indentsp,
			   TDF_LINENO | TDF_SLIM | TDF_VOPS);
	fflush (f);
      }
      break;
    default:
      goto end;
    }
end:
  MELT_EXITFRAME ();
#undef outv
}

/* pretty print into an outbuf a gimple seq */
void
meltgc_ppout_gimple_seq (melt_ptr_t out_p, int indentsp,
			       gimple_seq gseq)
{
  int outmagic = 0;
#define outv curfram__.varptr[0]
  MELT_ENTERFRAME (2, NULL);
  outv = out_p;
  if (!outv)
    goto end;
  if (!gseq)
    {
      meltgc_add_out ((melt_ptr_t) outv,
			    "%nullgimpleseq%");
      goto end;
    }
  outmagic = melt_magic_discr ((melt_ptr_t) outv);
  switch (outmagic) 
    {
    case OBMAG_STRBUF:
      {
	open_meltpp_file ();
	print_gimple_seq (meltppfile, gseq, indentsp,
			   TDF_LINENO | TDF_SLIM | TDF_VOPS);
	close_meltpp_file ();
	meltgc_add_out_raw_len ((melt_ptr_t) outv, meltppbuffer, (int) meltppbufsiz);
	free(meltppbuffer);
	meltppbuffer = 0;
	meltppbufsiz = 0;
      }
      break;
    case OBMAG_SPEC_FILE:
    case OBMAG_SPEC_RAWFILE:
      {
	FILE* f = ((struct meltspecial_st*)outv)->val.sp_file;
	if (!f) 
	  goto end;
	print_gimple_seq (f, gseq, indentsp,
			   TDF_LINENO | TDF_SLIM | TDF_VOPS);
	fflush (f);
      }
      break;
    default:
      goto end;
    }
end:
  MELT_EXITFRAME ();
#undef endv
}

/* pretty print a tree */
void
meltgc_ppout_tree (melt_ptr_t out_p, int indentsp, tree tr)
{
  int outmagic = 0;
#define outv curfram__.varptr[0]
  MELT_ENTERFRAME (2, NULL);
  outv = out_p;
  if (!outv)
    goto end;
  if (!tr) 
    {
      meltgc_add_out_raw ((melt_ptr_t) outv, "%nulltree%");
      goto end;
    }
  outmagic = melt_magic_discr ((melt_ptr_t) outv);
  switch (outmagic) 
    {
    case OBMAG_STRBUF:
      {
	open_meltpp_file ();
	print_node_brief (meltppfile, "", tr, indentsp);
	close_meltpp_file ();
	meltgc_add_out_raw_len ((melt_ptr_t) outv, meltppbuffer, (int) meltppbufsiz);
	free(meltppbuffer);
	meltppbuffer = 0;
	meltppbufsiz = 0;
      }
      break;
    case OBMAG_SPEC_FILE:
    case OBMAG_SPEC_RAWFILE:
      {
	FILE* f = ((struct meltspecial_st*)outv)->val.sp_file;
	if (!f) 
	  goto end;
	print_node_brief (f, "", tr, indentsp);
	fflush (f);
      }
      break;
    default:
      goto end;
    }
end:
  MELT_EXITFRAME ();
#undef outv
}


/* pretty print into an outbuf a basicblock */
void
meltgc_ppout_basicblock (melt_ptr_t out_p, int indentsp,
			       basic_block bb)
{
  gimple_seq gsq = 0;
#define outv curfram__.varptr[0]
  MELT_ENTERFRAME (2, NULL);
  outv = out_p;
  if (!outv)
    goto end;
  if (!bb)
    {
      meltgc_add_out_raw ((melt_ptr_t) outv,
			    "%nullbasicblock%");
      goto end;
    }
  meltgc_out_printf ((melt_ptr_t) outv,
			"basicblock ix%d", bb->index);
  gsq = bb_seq (bb);
  if (gsq)
    {
      meltgc_add_out_raw ((melt_ptr_t) outv, "{.");
      meltgc_ppout_gimple_seq ((melt_ptr_t) outv,
				     indentsp + 1, gsq);
      meltgc_add_out_raw ((melt_ptr_t) outv, ".}");
    }
  else
    meltgc_add_out_raw ((melt_ptr_t) outv, ";");
end:
  MELT_EXITFRAME ();
#undef sbufv
}


/* pretty print into an sbuf a mpz_t GMP multiprecision integer */
void
meltgc_ppout_mpz (melt_ptr_t out_p, int indentsp, mpz_t mp)
{
  int len = 0;
  char* cbuf = 0;
  char tinybuf [64];
#define outv curfram__.varptr[0]
  MELT_ENTERFRAME (2, NULL);
  outv = out_p;
  memset(tinybuf, 0, sizeof (tinybuf));
  if (!outv || indentsp<0)
    goto end;
  if (!mp)
    {
      meltgc_add_out_raw ((melt_ptr_t) outv, "%nullmp%");
      goto end;
    }
  len = mpz_sizeinbase(mp, 10) + 2;
  if (len < (int)sizeof(tinybuf)-2) 
    {
      mpz_get_str (tinybuf, 10, mp);
      meltgc_add_out_raw ((melt_ptr_t) outv, tinybuf);
    }
  else
    {
      cbuf = (char*) xcalloc(len+2, 1);
      mpz_get_str(cbuf, 10, mp);
      meltgc_add_out_raw ((melt_ptr_t) outv, cbuf);
      free(cbuf);
    }
 end:
  MELT_EXITFRAME ();
#undef sbufv
}


/* pretty print into an out the GMP multiprecision integer of a mixbigint */
void
meltgc_ppout_mixbigint (melt_ptr_t out_p, int indentsp,
			      melt_ptr_t big_p)
{
#define outv curfram__.varptr[0]
#define bigv  curfram__.varptr[1]
  MELT_ENTERFRAME (3, NULL);
  outv = out_p;
  bigv = big_p;
  if (!outv)
    goto end;
  if (!bigv || melt_magic_discr ((melt_ptr_t) bigv) != OBMAG_MIXBIGINT)
    goto end;
  {
    mpz_t mp;
    mpz_init (mp);
    if (melt_fill_mpz_from_mixbigint((melt_ptr_t) bigv, mp)) 
      meltgc_ppout_mpz ((melt_ptr_t) outv, indentsp, mp);
    mpz_clear (mp);
  }
 end:
  MELT_EXITFRAME ();
#undef sbufv
#undef bigv
}

/* make a new boxed file */
melt_ptr_t
meltgc_new_file(melt_ptr_t discr_p, FILE* fil)
{
  MELT_ENTERFRAME(2, NULL);
#define discrv curfram__.varptr[0]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define resv   curfram__.varptr[1]
#define spec_resv ((struct meltspecial_st*)(resv))
  discrv = (void *) discr_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_SPEC_FILE
      && object_discrv->object_magic != OBMAG_SPEC_RAWFILE)
    goto end;
  resv = meltgc_make_special ((melt_ptr_t) discrv);
  spec_resv->val.sp_file = fil;
 end:
  MELT_EXITFRAME ();
  return (melt_ptr_t) resv;
}

/***********************************************************************
 *   P A R M A     P O L Y H E D R A     L I B R A R Y     S T U F F   *
 ***********************************************************************/

/* utility to make a ppl_Coefficient_t out of a constant tree */
ppl_Coefficient_t
melt_make_ppl_coefficient_from_tree(tree tr)
{
  HOST_WIDE_INT lo=0, hi=0;
  ppl_Coefficient_t coef=NULL;
  mpz_t mp;
  if (!tr) return NULL;
  switch (TREE_CODE(tr)) {
  case INTEGER_CST:
    mpz_init(mp);
    lo = TREE_INT_CST_LOW(tr);
    hi = TREE_INT_CST_HIGH(tr);
    if (hi==0 && lo>=0) 
      mpz_set_ui(mp, lo);
    else if (hi== -1 && lo<0)
      mpz_set_si(mp, lo);
    else {
      mpz_t mp2;
      mpz_init_set_ui (mp2, lo);
      mpz_set_si(mp, hi);
      mpz_mul_2exp(mp, mp, HOST_BITS_PER_WIDE_INT);
      mpz_add(mp, mp, mp2);
      mpz_clear(mp2);
    };
    if (ppl_new_Coefficient_from_mpz_t (&coef, mp))
      fatal_error("ppl_new_Coefficient_from_mpz_t failed");
    mpz_clear(mp);
    return coef;
  default:
    break;
  }
  return NULL;
}

/* utility to make a ppl_Coefficient_t from a long number */
ppl_Coefficient_t
melt_make_ppl_coefficient_from_long(long l)
{
  ppl_Coefficient_t coef=NULL;
  mpz_t mp;
  mpz_init_set_si (mp, l);
  if (ppl_new_Coefficient_from_mpz_t (&coef, mp))
    fatal_error("ppl_new_Coefficient_from_mpz_t failed");
  mpz_clear(mp);
  return coef;
}

/* make a new boxed PPL empty or unsatisfiable constraint system */
melt_ptr_t
meltgc_new_ppl_constraint_system(melt_ptr_t discr_p, bool unsatisfiable)
{
  int err = 0;
  MELT_ENTERFRAME(2, NULL);
#define discrv curfram__.varptr[0]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define resv   curfram__.varptr[1]
#define spec_resv ((struct meltspecial_st*)(resv))
  discrv = (void *) discr_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_SPECPPL_CONSTRAINT_SYSTEM)
    goto end;
  resv = meltgc_make_special ((melt_ptr_t) discrv);
  spec_resv->val.sp_pointer = NULL;
  if (!unsatisfiable)
    err = ppl_new_Constraint_System(&spec_resv->val.sp_constraint_system);
  else
    err = ppl_new_Constraint_System_zero_dim_empty(&spec_resv->val.sp_constraint_system);
  if (err) 
    fatal_error("PPL new Constraint System failed in Melt"); 
 end:
  MELT_EXITFRAME();
  return (melt_ptr_t)resv;
#undef discrv
#undef object_discrv
#undef resv
#undef spec_resv
}

/* box clone a PPL constraint system */
melt_ptr_t
meltgc_clone_ppl_constraint_system (melt_ptr_t ppl_p)
{
  int err = 0;
  ppl_Constraint_System_t oldconsys = NULL, newconsys = NULL;
  MELT_ENTERFRAME(3, NULL);
#define pplv   curfram__.varptr[0]
#define spec_pplv ((struct meltspecial_st*)(pplv))
#define discrv curfram__.varptr[1]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define resv   curfram__.varptr[2]
#define spec_resv ((struct meltspecial_st*)(resv))
  pplv = ppl_p;
  resv = NULL;
  if (melt_magic_discr ((melt_ptr_t) (pplv)) != OBMAG_SPECPPL_CONSTRAINT_SYSTEM)
    goto end;
  oldconsys =  spec_pplv->val.sp_constraint_system;
  resv = meltgc_make_special ((melt_ptr_t) discrv);
  if (oldconsys)
    err = ppl_new_Constraint_System_from_Constraint_System(&newconsys, oldconsys);
  if (err) 
    fatal_error("PPL clone Constraint System failed in Melt");
  spec_resv->val.sp_constraint_system = newconsys;
 end:
  MELT_EXITFRAME();
  return (melt_ptr_t)resv;
#undef discrv
#undef object_discrv
#undef resv
#undef spec_resv
#undef pplv
#undef spec_pplv
}

/* insert a raw PPL constraint into a boxed constraint system */
void
melt_insert_ppl_constraint_in_boxed_system(ppl_Constraint_t cons, melt_ptr_t ppl_p) 
{
  MELT_ENTERFRAME(3, NULL);
#define pplv   curfram__.varptr[0]
#define spec_pplv ((struct meltspecial_st*)(pplv))
  pplv = ppl_p;
  if (!pplv || !cons 
      || melt_magic_discr((melt_ptr_t)pplv) != OBMAG_SPECPPL_CONSTRAINT_SYSTEM)
    goto end;
  if (spec_pplv->val.sp_constraint_system
      && ppl_Constraint_System_insert_Constraint (spec_pplv->val.sp_constraint_system,
						  cons))
    fatal_error("failed to ppl_Constraint_System_insert_Constraint");
 end:
  MELT_EXITFRAME();
#undef pplv
#undef spec_pplv
}

/* utility to make a NNC [=not necessarily closed] ppl_Polyhedron_t
   out of a constraint system */
ppl_Polyhedron_t 
melt_make_ppl_NNC_Polyhedron_from_Constraint_System(ppl_Constraint_System_t consys)
{
  ppl_Polyhedron_t poly = NULL;
  if (ppl_new_NNC_Polyhedron_from_Constraint_System(&poly, consys))
    fatal_error("melt_make_ppl_NNC_Polyhedron_from_Constraint_System failed");
  return poly;
}

/* make a new boxed PPL polyhedron; if cloned is true, the poly is
   copied otherwise taken as is */
melt_ptr_t
meltgc_new_ppl_polyhedron(melt_ptr_t discr_p, ppl_Polyhedron_t poly, bool cloned)
{
  MELT_ENTERFRAME(2, NULL);
#define discrv curfram__.varptr[0]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define resv   curfram__.varptr[1]
#define spec_resv ((struct meltspecial_st*)(resv))
  discrv = (void *) discr_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_SPECPPL_POLYHEDRON)
    goto end;
  resv = meltgc_make_special ((melt_ptr_t) discrv);
  spec_resv->val.sp_pointer = NULL;
  if (cloned && poly)
    {
      if (ppl_new_NNC_Polyhedron_from_NNC_Polyhedron(&spec_resv->val.sp_polyhedron, poly))
	fatal_error("failed to ppl_new_NNC_Polyhedron_from_NNC_Polyhedron");
    }
  else
    spec_resv->val.sp_polyhedron = poly;
 end:
  MELT_EXITFRAME();
  return (melt_ptr_t)resv;
#undef discrv
#undef object_discrv
#undef resv
#undef spec_resv
}

/* utility to make a ppl_Linear_Expression_t */
ppl_Linear_Expression_t 
melt_make_ppl_linear_expression(void)
{
  ppl_Linear_Expression_t liex = NULL;
  if (ppl_new_Linear_Expression(&liex))
    fatal_error("melt_make_ppl_linear_expression failed");
  return liex;
}

/* utility to make a ppl_Constraint ; the constraint type is a string
   "==" or "!=" ">" "<" ">=" "<=" because we don't want enums in
   MELT... */
ppl_Constraint_t 
melt_make_ppl_constraint_cstrtype(ppl_Linear_Expression_t liex, const char*constyp) {
  ppl_Constraint_t cons = NULL;
  if (!liex || !constyp) return NULL;
  if (!strcmp(constyp, "==")
      && !ppl_new_Constraint(&cons, liex,
			     PPL_CONSTRAINT_TYPE_EQUAL)) 
    return cons;
  else if (!strcmp(constyp, ">")
	   && !ppl_new_Constraint(&cons, liex, 
				  PPL_CONSTRAINT_TYPE_GREATER_THAN))
    return cons;
  else if (!strcmp(constyp, "<")
	   && !ppl_new_Constraint(&cons, liex, 
				  PPL_CONSTRAINT_TYPE_LESS_THAN))
    return cons;
  else if (!strcmp(constyp, ">=")
	   && !ppl_new_Constraint(&cons, liex, 
				  PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL))
    return cons;
  else if (!strcmp(constyp, "<=")
	   && !ppl_new_Constraint(&cons, liex, 
				  PPL_CONSTRAINT_TYPE_LESS_OR_EQUAL))
    return cons;
  return NULL;
}

/* make a new boxed PPL linear expression  */
melt_ptr_t
meltgc_new_ppl_linear_expression(melt_ptr_t discr_p)
{
  int err = 0;
  MELT_ENTERFRAME(2, NULL);
#define discrv curfram__.varptr[0]
#define object_discrv ((meltobject_ptr_t)(discrv))
#define resv   curfram__.varptr[1]
#define spec_resv ((struct meltspecial_st*)(resv))
  discrv = (void *) discr_p;
  if (melt_magic_discr ((melt_ptr_t) (discrv)) != OBMAG_OBJECT)
    goto end;
  if (object_discrv->object_magic != OBMAG_SPECPPL_LINEAR_EXPRESSION)
    goto end;
  resv = meltgc_make_special ((melt_ptr_t) discrv);
  spec_resv->val.sp_pointer = NULL;
  err = ppl_new_Linear_Expression(&spec_resv->val.sp_linear_expression);
  if (err) 
    fatal_error("PPL new Linear Expression failed in Melt"); 
 end:
  MELT_EXITFRAME();
  return (melt_ptr_t)resv;
#undef discrv
#undef object_discrv
#undef resv
#undef spec_resv
}


void melt_clear_special(melt_ptr_t val_p)
{
  MELT_ENTERFRAME(1, NULL);
#define valv curfram__.varptr[0]
#define spec_valv ((struct meltspecial_st*)valv)
  valv = val_p;
  if (!valv) goto end;
  switch(melt_magic_discr((melt_ptr_t) valv)) {
  case ALL_OBMAG_SPECIAL_CASES:
      delete_special(spec_valv);
      break;
  default:
    break;
  }
 end:
  MELT_EXITFRAME();
#undef valv
#undef spec_valv
}


/***
  pretty print into an sbuf a PPL related value; 

recent PPL (ie 0.10.1) has a ppl_io_asprint_##Type (char** strp,
  ppl_const_##Type##_t x); which mallocs a string buffer, print x
  inside it, and return it in *STRP but this is supposed to
  change. Seee
  http://www.cs.unipr.it/pipermail/ppl-devel/2009-March/014162.html
***/

static melt_ptr_t* melt_pplcoefvectp;

static const char* 
ppl_melt_variable_output_function(ppl_dimension_type var)
{
  static char buf[80];
  const char *s = 0;
  MELT_ENTERFRAME(2, NULL);
#define vectv  curfram__.varptr[0]
#define namv   curfram__.varptr[1]
  if (melt_pplcoefvectp)
    vectv =  *melt_pplcoefvectp;
  memset(buf, 0, sizeof(buf));
  if (vectv)
    namv = melt_multiple_nth((melt_ptr_t) vectv, (int)var);
  if (melt_is_instance_of((melt_ptr_t) namv,
			     (melt_ptr_t) MELT_PREDEF (CLASS_NAMED))) 
    namv = melt_object_nth_field((melt_ptr_t) namv, FNAMED_NAME);
  if (namv)
    s = melt_string_str((melt_ptr_t) namv);
  if (!s && melt_magic_discr((melt_ptr_t) namv) == OBMAG_TREE) {
    tree trnam = melt_tree_content((melt_ptr_t) namv);
    if (trnam) {
      switch (TREE_CODE(trnam)) {
      case IDENTIFIER_NODE:
	s = IDENTIFIER_POINTER(trnam);
	break;
      case VAR_DECL:
      case PARM_DECL:
      case TYPE_DECL:
      case FIELD_DECL:
      case LABEL_DECL:
      case CONST_DECL:
      case RESULT_DECL:
	if (DECL_NAME(trnam))
	  s = IDENTIFIER_POINTER(DECL_NAME(trnam));
	break;
      case SSA_NAME:
	snprintf (buf, sizeof(buf)-1, "%s.%d", 
		 get_name(trnam), SSA_NAME_VERSION(trnam));
	goto end;
      default:
	snprintf (buf, sizeof(buf)-1, "@%p!%s", 
		 (void*)trnam, tree_code_name[TREE_CODE(trnam)]);
	goto end;
      }
    }
  }
  if (s) 
    strncpy(buf, s, sizeof(buf)-1);
  else if (!buf[0])
    snprintf (buf, sizeof(buf)-1, "_$_%d", (int)var);
 end:
  MELT_EXITFRAME();
  return buf;
}


/* call the ppl_io_asprint_##Type (char** strp, ppl_const_##Type##_t
   x); these functions are now stable in PPL */
void
meltgc_ppstrbuf_ppl_varnamvect (melt_ptr_t sbuf_p, int indentsp, melt_ptr_t ppl_p, melt_ptr_t varnamvect_p)
{
  int mag = 0;
  char *ppstr = NULL;
  MELT_ENTERFRAME(4, NULL);
#define sbufv    curfram__.varptr[0]
#define pplv     curfram__.varptr[1]
#define varvectv curfram__.varptr[2]
#define spec_pplv ((struct meltspecial_st*)(pplv))
  sbufv = sbuf_p;
  pplv = ppl_p;
  varvectv = varnamvect_p;
  if (!pplv) 
    goto end;
  ppl_io_set_variable_output_function (ppl_melt_variable_output_function);
  mag = melt_magic_discr((melt_ptr_t) pplv);
  if (varvectv)
    melt_pplcoefvectp = (melt_ptr_t*)&varvectv;
  else
    melt_pplcoefvectp = NULL;
  switch (mag) {
  case OBMAG_SPECPPL_COEFFICIENT:
    if (ppl_io_asprint_Coefficient(&ppstr, 
				   spec_pplv->val.sp_coefficient))
      fatal_error("failed to ppl_io_asprint_Coefficient");
    break;
  case OBMAG_SPECPPL_LINEAR_EXPRESSION:
    if (ppl_io_asprint_Linear_Expression(&ppstr,
					 spec_pplv->val.sp_linear_expression))
      fatal_error("failed to ppl_io_asprint_Linear_Expression");
    break;
  case OBMAG_SPECPPL_CONSTRAINT:
    if (ppl_io_asprint_Constraint(&ppstr, 
				  spec_pplv->val.sp_constraint))
      fatal_error("failed to ppl_io_asprint_Constraint");
    break;
  case OBMAG_SPECPPL_CONSTRAINT_SYSTEM:
    if (ppl_io_asprint_Constraint_System(&ppstr, 
					 spec_pplv->val.sp_constraint_system))
      fatal_error("failed to ppl_io_asprint_Constraint_System");
    break;
  case OBMAG_SPECPPL_GENERATOR:
    if (ppl_io_asprint_Generator(&ppstr, spec_pplv->val.sp_generator))
      fatal_error("failed to ppl_io_asprint_Generator");
    break;
  case OBMAG_SPECPPL_GENERATOR_SYSTEM:
    if (ppl_io_asprint_Generator_System(&ppstr, 
					spec_pplv->val.sp_generator_system))
      fatal_error("failed to ppl_io_asprint_Generator_System");
    break;
  case OBMAG_SPECPPL_POLYHEDRON:
    if (ppl_io_asprint_Polyhedron(&ppstr, 
					spec_pplv->val.sp_polyhedron))
      fatal_error("failed to ppl_io_asprint_Polyhedron");
    break;
  default:
    {
      char errmsg[64];
      memset(errmsg, 0, sizeof(errmsg));
      snprintf (errmsg, sizeof(errmsg)-1, "{{unknown PPL magic %d}}", mag); 
      ppstr = xstrdup(errmsg);
    }
    break;
  }
  if (!ppstr) 
    fatal_error("ppl_io_asprint_* gives a null string pointer");
  /* in the resulting ppstr, replace each newline with appropriate
     indentation */
  {
    char*bl = NULL;		/* current begin of line */
    char*nl = NULL;		/* current newline = end of line */
    for (bl = ppstr; (nl = bl?strchr(bl, '\n'):NULL), bl; bl = nl?(nl+1):NULL) {
      if (nl) 
	*nl = (char)0;
      meltgc_add_strbuf_raw((melt_ptr_t) sbufv, bl);
      if (nl) 
	meltgc_strbuf_add_indent((melt_ptr_t) sbufv, indentsp, 0);
    }
  }
  free(ppstr);
 end:
  melt_pplcoefvectp = (melt_ptr_t*)0;
  MELT_EXITFRAME();
#undef sbufv
#undef pplv
#undef varvectv
#undef spec_pplv
}



static void melt_ppl_error_handler(enum ppl_enum_error_code err, const char* descr)
{
  switch(err) {
  case PPL_ERROR_OUT_OF_MEMORY: 
    error("Melt PPL out of memory: %s", descr);
    return;
  case PPL_ERROR_INVALID_ARGUMENT:
    error("Melt PPL invalid argument: %s", descr);
    return;
  case PPL_ERROR_DOMAIN_ERROR:
    error("Melt PPL domain error: %s", descr);
    return;
  case PPL_ERROR_LENGTH_ERROR:
    error("Melt PPL length error: %s", descr);
    return;
  case PPL_ARITHMETIC_OVERFLOW:
    error("Melt PPL arithmetic overflow: %s", descr);
    return;
  case PPL_STDIO_ERROR:
    error("Melt PPL stdio error: %s", descr);
    return;
  case PPL_ERROR_INTERNAL_ERROR:
    error("Melt PPL internal error: %s", descr);
    return;
  case PPL_ERROR_UNKNOWN_STANDARD_EXCEPTION:
    error("Melt PPL unknown exception: %s", descr);
    return;
  case PPL_ERROR_UNEXPECTED_ERROR:
    error("Melt PPL unexpected error: %s", descr);
    return;
  default:
    fatal_error("Melt unexpected PPL error #%d - %s", err, descr);
  }
}


/***********************************************************
 * generate C code for a melt unit name 
 ***********************************************************/
void
melt_output_cfile_decl_impl (melt_ptr_t unitnam,
				melt_ptr_t declbuf, melt_ptr_t implbuf)
{
  int unamlen = 0;
  char *dotcnam = NULL;
  char *dotcdotnam = NULL;
  char *dotcpercentnam = NULL;
  FILE *cfil = NULL;
  gcc_assert (melt_magic_discr (unitnam) == OBMAG_STRING);
  gcc_assert (melt_magic_discr (declbuf) == OBMAG_STRBUF);
  gcc_assert (melt_magic_discr (implbuf) == OBMAG_STRBUF);
  /** FIXME : should implement some policy about the location of the
      generated C file; currently using the pwd */
  unamlen = strlen (melt_string_str (unitnam));
  dotcnam = (char *) xcalloc (unamlen + 3, 1);
  dotcpercentnam = (char *) xcalloc (unamlen + 4, 1);
  dotcdotnam = (char *) xcalloc (unamlen + 5, 1);
  strcpy (dotcnam, melt_string_str (unitnam));
  if (unamlen > 4
      && (dotcnam[unamlen - 2] != '.' || dotcnam[unamlen - 1] != 'c'))
    strcat (dotcnam, ".c");
  strcpy (dotcpercentnam, dotcnam);
  strcat (dotcpercentnam, "%");
  strcpy (dotcdotnam, dotcnam);
  strcat (dotcdotnam, ".");
  cfil = fopen (dotcdotnam, "w");
  if (!cfil)
    fatal_error ("failed to open melt generated file %s - %m", dotcnam);
  fprintf (cfil,
	   "/* GCC MELT GENERATED FILE %s - DO NOT EDIT */\n", dotcnam);
  {
    time_t now = 0;
    char nowtimstr[64];
    time (&now);
    memset (nowtimstr, 0, sizeof (nowtimstr));
    /* we only write the date (not the hour); the comment is for
       humans only.  This might make ccache happier, when the same
       file is generated several times in the same day */
    strftime(nowtimstr, sizeof(nowtimstr)-1, "%Y %b %d", gmtime(&now));
    if (strlen (nowtimstr) > 2)
      fprintf (cfil, "/* generated on %s */\n\n", nowtimstr);
  }
  /* we protect genchecksum_melt with MELTGCC_DYNAMIC_OBJSTRUCT since
     for sure when compiling the warmelt*0.c it would mismatch, and we
     want to avoid a useless warning */
  fprintf (cfil, "\n#ifndef MELTGCC_DYNAMIC_OBJSTRUCT\n"
	   "/* checksum of the gcc executable generating this file: */\n"
	   "const unsigned char genchecksum_melt[16]=\n {");
  {
    int i;
    for (i=0; i<16; i++) {
      if (i>0) 
	fputc(',', cfil);
      if (i==8) 
	fputs("\n   ", cfil);
      fprintf (cfil, " %#x", executable_checksum[i]);
    }
  };
  fprintf (cfil, "};\n" "#endif\n" "\n");
  fprintf (cfil, "#include \"run-melt.h\"\n");
  fprintf (cfil, "\n/**** %s declarations ****/\n",
	   melt_string_str (unitnam));
  melt_putstrbuf (cfil, declbuf);
  putc ('\n', cfil);
  fflush (cfil);
  fprintf (cfil, "\n/**** %s implementations ****/\n",
	   melt_string_str (unitnam));
  melt_putstrbuf (cfil, implbuf);
  putc ('\n', cfil);
  fflush (cfil);
  fprintf (cfil, "\n/**** end of %s ****/\n", melt_string_str (unitnam));
  fclose (cfil);
  debugeprintf ("output_cfile done dotcnam %s", dotcnam);
  /* if possible, rename the previous 'foo.c' file to 'foo.c%' as a backup */
  (void) rename (dotcnam, dotcpercentnam);
  /* always rename the just generated 'foo.c.' file to 'foo.c' to make
     the generation more atomic */
  if (rename (dotcdotnam, dotcnam))
    fatal_error ("failed to rename melt generated file %s to %s - %m",
		 dotcdotnam, dotcnam);
  {
    static char pwdbuf[500];
    memset(pwdbuf, 0, sizeof(pwdbuf));
    getcwd(pwdbuf, sizeof(pwdbuf)-1);
    inform (UNKNOWN_LOCATION, "MELT generated file %s in %s",
	    dotcnam, pwdbuf);
  }
  free (dotcnam);
  free (dotcdotnam);
  free (dotcpercentnam);
}

/* Added */
#undef melt_assert_failed
#undef melt_check_failed

void
melt_assert_failed (const char *msg, const char *filnam,
		       int lineno, const char *fun)
{
  static char msgbuf[500];
  if (!msg)
    msg = "??no-msg??";
  if (!filnam)
    filnam = "??no-filnam??";
  if (!fun)
    fun = "??no-func??";
  if (melt_dbgcounter > 0)
    snprintf (msgbuf, sizeof (msgbuf) - 1,
	      "%s:%d: MELT ASSERT #!%ld: %s {%s}", basename (filnam),
	      lineno, melt_dbgcounter, fun, msg);
  else
    snprintf (msgbuf, sizeof (msgbuf) - 1, "%s:%d: MELT ASSERT: %s {%s}",
	      basename (filnam), lineno, fun, msg);
  melt_dbgshortbacktrace (msgbuf, 100);
  fatal_error ("%s:%d: MELT ASSERT FAILED <%s> : %s\n",
	       basename (filnam), lineno, fun, msg);
}

void
melt_check_failed (const char *msg, const char *filnam,
		      int lineno, const char *fun)
{
  static char msgbuf[500];
  if (!msg)
    msg = "??no-msg??";
  if (!filnam)
    filnam = "??no-filnam??";
  if (!fun)
    fun = "??no-func??";
  if (melt_dbgcounter > 0)
    snprintf (msgbuf, sizeof (msgbuf) - 1,
	      "%s:%d: MELT CHECK #!%ld: %s {%s}", basename (filnam),
	      lineno, melt_dbgcounter, fun, msg);
  else
    snprintf (msgbuf, sizeof (msgbuf) - 1, "%s:%d: MELT CHECK: %s {%s}",
	      basename (filnam), lineno, fun, msg);
  melt_dbgshortbacktrace (msgbuf, 100);
  warning (0, "%s:%d: MELT CHECK FAILED <%s> : %s\n",
	   basename (filnam), lineno, fun, msg);
}


/* convert a MELT value to a plugin flag or option */
static unsigned long 
melt_val2passflag(melt_ptr_t val_p)
{
  unsigned long res = 0;
  int valmag = 0;
  MELT_ENTERFRAME (3, NULL);
#define valv    curfram__.varptr[0]
#define compv   curfram__.varptr[1]
#define pairv   curfram__.varptr[2]
  valv = val_p;
  if (!valv) goto end;
  valmag = melt_magic_discr((melt_ptr_t) valv);
  if (valmag == OBMAG_INT || valmag == OBMAG_MIXINT)
    { 
      res = melt_get_int((melt_ptr_t) valv);
      goto end;
    }
  else if (valmag == OBMAG_OBJECT 
	   && melt_is_instance_of((melt_ptr_t) valv, 
				  (melt_ptr_t) MELT_PREDEF(CLASS_NAMED)))
    {
      compv = ((meltobject_ptr_t)valv)->obj_vartab[FNAMED_NAME];
      res = melt_val2passflag((melt_ptr_t) compv);
      goto end;
    }
  else if (valmag == OBMAG_STRING) {
    const char *valstr = melt_string_str((melt_ptr_t) valv);
    /* should be kept in sync with the defines in tree-pass.h */
#define WHENFLAG(F) if (!strcasecmp(valstr, #F)) { res = F; goto end; } 
    WHENFLAG(PROP_gimple_any);
    WHENFLAG(PROP_gimple_lcf);		
    WHENFLAG(PROP_gimple_leh);		
    WHENFLAG(PROP_cfg);	
    WHENFLAG(PROP_referenced_vars);
    WHENFLAG(PROP_ssa);
    WHENFLAG(PROP_no_crit_edges);
    WHENFLAG(PROP_rtl);
    WHENFLAG(PROP_gimple_lomp);
    WHENFLAG(PROP_cfglayout);
    WHENFLAG(PROP_trees);
    /* likewise for TODO flags */
    WHENFLAG(TODO_dump_func);
    WHENFLAG(TODO_ggc_collect);
    WHENFLAG(TODO_verify_ssa);
    WHENFLAG(TODO_verify_flow);
    WHENFLAG(TODO_verify_stmts);
    WHENFLAG(TODO_cleanup_cfg);
    WHENFLAG(TODO_verify_loops);
    WHENFLAG(TODO_dump_cgraph);
    WHENFLAG(TODO_remove_functions);
    WHENFLAG(TODO_rebuild_frequencies);
    WHENFLAG(TODO_verify_rtl_sharing);
    WHENFLAG(TODO_update_ssa);
    WHENFLAG(TODO_update_ssa_no_phi);
    WHENFLAG(TODO_update_ssa_full_phi);
    WHENFLAG(TODO_update_ssa_only_virtuals);
    WHENFLAG(TODO_remove_unused_locals);
    WHENFLAG(TODO_df_finish);
    WHENFLAG(TODO_df_verify);
    WHENFLAG(TODO_mark_first_instance);
    WHENFLAG(TODO_rebuild_alias);
    WHENFLAG(TODO_update_address_taken);
    WHENFLAG(TODO_update_ssa_any);
    WHENFLAG(TODO_verify_all);
#undef WHENFLAG
    goto end;
  }
  else if (valmag == OBMAG_LIST) {
    for (pairv = ((struct meltlist_st *) valv)->first;
	 melt_magic_discr ((melt_ptr_t) pairv) ==
	   OBMAG_PAIR; 
	 pairv = ((struct meltpair_st *)pairv)->tl) {
      compv = ((struct meltpair_st *)pairv)->hd;
      res |= melt_val2passflag((melt_ptr_t) compv);
    }
  }
  else if (valmag == OBMAG_MULTIPLE) {
    int i=0, l=0;
    l = melt_multiple_length((melt_ptr_t)valv);
    for (i=0; i<l; i++) {
      compv = melt_multiple_nth((melt_ptr_t) valv, i);
      res |= melt_val2passflag((melt_ptr_t) compv);
    }
  }
 end:
  MELT_EXITFRAME();
  return res;
#undef valv    
#undef compv   
#undef pairv   
}




/* the gate function of MELT gimple passes */
static bool 
meltgc_gimple_gate(void)
{
  int ok = 0;
  static const char* modstr;
  FILE *oldf = NULL;
  MELT_ENTERFRAME(6, NULL);
#define passv        curfram__.varptr[0]
#define passdictv    curfram__.varptr[1]
#define closv        curfram__.varptr[2]
#define resv         curfram__.varptr[3]
#define dumpv        curfram__.varptr[4]
  if (!modstr)
    modstr = melt_argument ("mode");
  if (!modstr || !modstr) 
    goto end;
  gcc_assert(current_pass != NULL);
  gcc_assert(current_pass->name != NULL);
  gcc_assert(current_pass->type == GIMPLE_PASS);
  debugeprintf ("meltgc_gimple_gate pass %s", current_pass->name);
  passdictv = melt_get_inisysdata (FSYSDAT_PASS_DICT);
  if (melt_magic_discr((melt_ptr_t) passdictv) != OBMAG_MAPSTRINGS) 
    goto end;
  passv = melt_get_mapstrings((struct meltmapstrings_st*) passdictv, current_pass->name);
  if (!passv 
      || !melt_is_instance_of((melt_ptr_t) passv, (melt_ptr_t)  MELT_PREDEF(CLASS_GCC_GIMPLE_PASS)))
    goto end;
  closv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_GATE);
  if (melt_magic_discr((melt_ptr_t) closv) != OBMAG_CLOSURE) 
    goto end;
  dumpv = melt_get_inisysdata (FSYSDAT_DUMPFILE);
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      oldf = ((struct meltspecial_st*)dumpv)->val.sp_file;
      ((struct meltspecial_st*)dumpv)->val.sp_file = dump_file;
    }
  debugeprintf ("meltgc_gimple_gate pass %s before apply", current_pass->name);
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_gimple_gate pass %s before apply",
	      basename (__FILE__), __LINE__, current_pass->name);
    curfram__.flocs = locbuf;
  }
#endif
  resv = 
    melt_apply ((struct meltclosure_st *) closv,
		(melt_ptr_t) passv, "",
		(union meltparam_un *) 0, "",
		(union meltparam_un *) 0);
  ok = (resv != NULL);
  debugeprintf ("meltgc_gimple_gate pass %s after apply ok=%d",
		current_pass->name, ok);
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      FILE *df = melt_get_file ((melt_ptr_t) dumpv);
      if (df)
	fflush (df);
      ((struct meltspecial_st*)dumpv)->val.sp_file = oldf;
    };
  /* force a minor GC to be sure that nothing is in the young region */
  melt_garbcoll (0, MELT_ONLY_MINOR);
 end:
  debugeprintf ("meltgc_gimple_gate pass %s ended ok=%d", current_pass->name, ok);
  MELT_EXITFRAME();
  return ok;
#undef passv        
#undef passdictv    
#undef closv        
#undef resv         
#undef dumpv        
}


/* the execute function of MELT gimple passes */
static unsigned int
meltgc_gimple_execute(void)
{
  unsigned int res = 0;
  static const char* modstr;
  MELT_ENTERFRAME(6, NULL);
#define passv        curfram__.varptr[0]
#define passdictv    curfram__.varptr[1]
#define closv        curfram__.varptr[2]
#define resvalv      curfram__.varptr[3]
#define dumpv        curfram__.varptr[4]
  if (!modstr)
    modstr = melt_argument ("mode");
  if (!modstr || !modstr[0])
    goto end;
  gcc_assert (current_pass != NULL);
  gcc_assert (current_pass->name != NULL);
  gcc_assert (current_pass->type == GIMPLE_PASS);
  debugeprintf ("meltgc_gimple_execute pass %s starting", current_pass->name);
  passdictv = melt_get_inisysdata (FSYSDAT_PASS_DICT);
  if (melt_magic_discr((melt_ptr_t) passdictv) != OBMAG_MAPSTRINGS) 
    goto end;
  passv = melt_get_mapstrings((struct meltmapstrings_st *)passdictv, current_pass->name);
  if (!passv 
      || !melt_is_instance_of((melt_ptr_t) passv,
			      (melt_ptr_t) MELT_PREDEF(CLASS_GCC_GIMPLE_PASS)))
    goto end;
  closv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_EXEC);
  if (melt_magic_discr((melt_ptr_t) closv) != OBMAG_CLOSURE) 
    goto end;
  {
    long passdbgcounter = melt_dbgcounter;
    long todol = 0;
    FILE *oldf = NULL;
    union meltparam_un restab[1];
    memset (&restab, 0, sizeof (restab));
    debugeprintf
      ("gimple_execute passname %s dbgcounter %ld cfun %p ",
       current_pass->name, melt_dbgcounter, (void *) cfun);
    if (cfun && flag_melt_debug)
      debug_tree (cfun->decl);
    debugeprintf ("gimple_execute passname %s before apply",
		  current_pass->name);
    if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
      {
	oldf = ((struct meltspecial_st*)dumpv)->val.sp_file;
	((struct meltspecial_st*)dumpv)->val.sp_file = dump_file;
      };
    debugeprintf ("gimple_execute passname %s before apply dbgcounter %ld",
		  current_pass->name, passdbgcounter);
    /* apply with one extra long result */
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_gimple_execute pass %s before apply",
	      basename (__FILE__), __LINE__, current_pass->name);
    curfram__.flocs = locbuf;
  }
#endif
    restab[0].bp_longptr = &todol;
    resvalv =
      melt_apply ((struct meltclosure_st *) closv,
		  (melt_ptr_t) passv, "",
		  (union meltparam_un *) 0, BPARSTR_LONG "",
		  restab);
    debugeprintf ("gimple_execute passname %s after apply dbgcounter %ld",
		  current_pass->name, passdbgcounter);
    if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
      {
	FILE *df = melt_get_file ((melt_ptr_t) dumpv);
	if (df)
	  fflush(df);
	((struct meltspecial_st*)dumpv)->val.sp_file = oldf;
      };
    if (resvalv)
      res = (unsigned int) todol;
    /* force a minor GC to be sure that nothing is in the young region */
    melt_garbcoll (0, MELT_ONLY_MINOR);
  }
 end:
  debugeprintf ("meltgc_gimple_execute pass %s ended res=%ud", current_pass->name, res);
  MELT_EXITFRAME();
  return res;
#undef passv        
#undef passdictv    
#undef closv        
#undef resvalv      
#undef dumpv        
}



/* the gate function of MELT rtl passes */
static bool 
meltgc_rtl_gate(void)
{
  int ok = 0;
  FILE* oldf = NULL;
  static const char* modstr;
  MELT_ENTERFRAME(6, NULL);
#define passv        curfram__.varptr[0]
#define passdictv    curfram__.varptr[1]
#define closv        curfram__.varptr[2]
#define resv         curfram__.varptr[3]
#define dumpv        curfram__.varptr[4]
  if (!modstr)
    modstr = melt_argument ("mode");
  if (!modstr || !modstr[0])
    goto end;
  gcc_assert(current_pass != NULL);
  gcc_assert(current_pass->name != NULL);
  gcc_assert(current_pass->type == RTL_PASS);
  debugeprintf ("meltgc_rtl_gate pass %s start", current_pass->name);
  passdictv =  melt_get_inisysdata (FSYSDAT_PASS_DICT);
  if (melt_magic_discr((melt_ptr_t) passdictv) != OBMAG_MAPSTRINGS) 
    goto end;
  passv = melt_get_mapstrings((struct meltmapstrings_st*) passdictv, 
			      current_pass->name);
  if (!passv 
      || !melt_is_instance_of((melt_ptr_t) passv, 
			      (melt_ptr_t) MELT_PREDEF(CLASS_GCC_RTL_PASS)))
    goto end;
  closv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_GATE);
  if (melt_magic_discr((melt_ptr_t) closv) != OBMAG_CLOSURE) 
    goto end;
  dumpv = melt_get_inisysdata (FSYSDAT_DUMPFILE);
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      oldf = ((struct meltspecial_st*)dumpv)->val.sp_file;
      ((struct meltspecial_st*)dumpv)->val.sp_file = dump_file;
    }
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_rtl_gate pass %s before apply",
	      basename (__FILE__), __LINE__, current_pass->name);
    curfram__.flocs = locbuf;
  }
#endif
  resv = 
    melt_apply ((struct meltclosure_st *) closv,
		(melt_ptr_t) passv, "",
		(union meltparam_un *) 0, "",
		(union meltparam_un *) 0);
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      FILE *df = melt_get_file ((melt_ptr_t) dumpv);
      if (df)
	fflush (df);
      ((struct meltspecial_st*)dumpv)->val.sp_file = oldf;
    };
  ok = (resv != NULL);
  /* force a minor GC to be sure that nothing is in the young region */
  melt_garbcoll (0, MELT_ONLY_MINOR);
 end:
  debugeprintf ("meltgc_rtl_gate pass %s end ok=%d", current_pass->name, ok);
  MELT_EXITFRAME();
  return ok;
}


/* the execute function of MELT rtl passes */
static unsigned int
meltgc_rtl_execute(void)
{
  unsigned int res = 0;
  FILE* oldf = NULL;
  static const char*modstr;
  MELT_ENTERFRAME(6, NULL);
#define passv        curfram__.varptr[0]
#define passdictv    curfram__.varptr[1]
#define closv        curfram__.varptr[2]
#define resvalv      curfram__.varptr[3]
#define dumpv        curfram__.varptr[4]
  if (!modstr)
    modstr = melt_argument ("mode");
  if (!modstr || !modstr[0])
    goto end;
  gcc_assert (current_pass != NULL);
  gcc_assert (current_pass->name != NULL);
  gcc_assert (current_pass->type == RTL_PASS);
  debugeprintf ("meltgc_rtl_execute pass %s start", current_pass->name);
  passdictv = melt_get_inisysdata (FSYSDAT_PASS_DICT);
  if (melt_magic_discr((melt_ptr_t) passdictv) != OBMAG_MAPSTRINGS) 
    goto end;
  passv = melt_get_mapstrings((struct meltmapstrings_st*) passdictv, 
			      current_pass->name);
  if (!passv 
      || !melt_is_instance_of((melt_ptr_t) passv,
			      (melt_ptr_t) MELT_PREDEF(CLASS_GCC_RTL_PASS)))
    goto end;
  closv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_EXEC);
  if (melt_magic_discr((melt_ptr_t) closv) != OBMAG_CLOSURE) 
    goto end;
  {
    long passdbgcounter = melt_dbgcounter;
    long todol = 0;
    union meltparam_un restab[1];
    dumpv = melt_get_inisysdata (FSYSDAT_DUMPFILE);
    if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
      {
	oldf = ((struct meltspecial_st*)dumpv)->val.sp_file;
	((struct meltspecial_st*)dumpv)->val.sp_file = dump_file;
      }
    memset (&restab, 0, sizeof (restab));
    restab[0].bp_longptr = &todol;
    debugeprintf
      ("rtl_execute passname %s dbgcounter %ld",
       current_pass->name, melt_dbgcounter);
    debugeprintf ("rtl_execute passname %s before apply",
		  current_pass->name);
    /* apply with one extra long result */
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_rtl_execute pass %s before apply",
	      basename (__FILE__), __LINE__, current_pass->name);
    curfram__.flocs = locbuf;
  }
#endif
    resvalv =
      melt_apply ((struct meltclosure_st *) closv,
		  (melt_ptr_t) passv, "",
		  (union meltparam_un *) 0, BPARSTR_LONG "",
		  restab);
    debugeprintf ("rtl_execute passname %s after apply dbgcounter %ld",
		  current_pass->name, passdbgcounter);
    if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
      {
	FILE *df = melt_get_file ((melt_ptr_t) dumpv);
	if (df)
	  fflush (df);
	((struct meltspecial_st*)dumpv)->val.sp_file = oldf;
      };
    if (resvalv)
      res = (unsigned int) todol;
    /* force a minor GC to be sure that nothing is in the young region */
    melt_garbcoll (0, MELT_ONLY_MINOR);
  }
 end:
  debugeprintf ("meltgc_rtl_execute pass %s end res=%ud", current_pass->name, res);
  MELT_EXITFRAME();
  return res;
#undef passv        
#undef passdictv    
#undef closv        
#undef resvalv      
#undef dumpv        
}



/* the gate function of MELT simple_ipa passes */
static bool 
meltgc_simple_ipa_gate(void)
{
  int ok = 0;
  FILE* oldf = NULL;
  static const char*modstr;
  MELT_ENTERFRAME(6, NULL);
#define passv        curfram__.varptr[0]
#define passdictv    curfram__.varptr[1]
#define closv        curfram__.varptr[2]
#define resv         curfram__.varptr[3]
#define dumpv        curfram__.varptr[4]
  if (!modstr)
    modstr = melt_argument ("mode");
  if (!modstr || !modstr[0])
    goto end;
  gcc_assert(current_pass != NULL);
  gcc_assert(current_pass->name != NULL);
  gcc_assert(current_pass->type == SIMPLE_IPA_PASS);
  debugeprintf ("meltgc_simple_ipa_gate pass %s start", current_pass->name);
  passdictv = melt_get_inisysdata (FSYSDAT_PASS_DICT);
  if (melt_magic_discr((melt_ptr_t) passdictv) != OBMAG_MAPSTRINGS) 
    goto end;
  passv = melt_get_mapstrings((struct meltmapstrings_st*) passdictv, 
			      current_pass->name);
  if (!passv 
      || !melt_is_instance_of((melt_ptr_t) passv, 
			      (melt_ptr_t) MELT_PREDEF(CLASS_GCC_SIMPLE_IPA_PASS)))
    goto end;
  closv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_GATE);
  if (melt_magic_discr((melt_ptr_t) closv) != OBMAG_CLOSURE) 
    goto end;
  dumpv = melt_get_inisysdata (FSYSDAT_DUMPFILE);
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      oldf = ((struct meltspecial_st*)dumpv)->val.sp_file;
      ((struct meltspecial_st*)dumpv)->val.sp_file = dump_file;
    }
  debugeprintf ("meltgc_simple_ipa_gate pass %s before apply", current_pass->name);
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_simple_ipa_gate pass %s before apply",
	      basename (__FILE__), __LINE__, current_pass->name);
    curfram__.flocs = locbuf;
  }
#endif
  resv = 
    melt_apply ((struct meltclosure_st *) closv,
		(melt_ptr_t) passv, "",
		(union meltparam_un *) 0, "",
		(union meltparam_un *) 0);
  debugeprintf ("meltgc_simple_ipa_gate pass %s after apply", current_pass->name);
  ok = (resv != NULL);
  if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
    {
      FILE *df = melt_get_file ((melt_ptr_t) dumpv);
      if (df)
	fflush (df);
      ((struct meltspecial_st*)dumpv)->val.sp_file = oldf;
    };
  /* force a minor GC to be sure that nothing is in the young region */
  melt_garbcoll (0, MELT_ONLY_MINOR);
 end:
  debugeprintf ("meltgc_simple_ipa_gate pass %s end ok=%d", current_pass->name, ok);
  MELT_EXITFRAME();
  return ok;
#undef passv        
#undef passdictv    
#undef closv        
#undef resv         
#undef dumpv
}



/* the execute function of MELT simple_ipa passes */
static unsigned int
meltgc_simple_ipa_execute(void)
{
  static const char*modstr;
  FILE* oldf = NULL;
  unsigned int res = 0;
  MELT_ENTERFRAME(6, NULL);
#define passv        curfram__.varptr[0]
#define passdictv    curfram__.varptr[1]
#define closv        curfram__.varptr[2]
#define resvalv      curfram__.varptr[3]
#define dumpv        curfram__.varptr[4]
  if (!modstr)
    modstr = melt_argument ("mode");
  if (!modstr || !modstr[0])
    goto end;
  gcc_assert (current_pass != NULL);
  gcc_assert (current_pass->name != NULL);
  gcc_assert (current_pass->type == SIMPLE_IPA_PASS);
  debugeprintf ("meltgc_simple_ipa_execute pass %s start", current_pass->name);
  passdictv = melt_get_inisysdata (FSYSDAT_PASS_DICT);
  if (melt_magic_discr((melt_ptr_t) passdictv) != OBMAG_MAPSTRINGS) 
    goto end;
  passv = melt_get_mapstrings((struct meltmapstrings_st*)passdictv, 
			      current_pass->name);
  if (!passv 
      || !melt_is_instance_of((melt_ptr_t) passv, 
			      (melt_ptr_t) MELT_PREDEF(CLASS_GCC_SIMPLE_IPA_PASS)))
    goto end;
  closv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_EXEC);
  if (melt_magic_discr((melt_ptr_t) closv) != OBMAG_CLOSURE) 
    goto end;
  {
    long passdbgcounter = melt_dbgcounter;
    long todol = 0;
    union meltparam_un restab[1];
    memset (&restab, 0, sizeof (restab));
    restab[0].bp_longptr = &todol;
    debugeprintf
      ("simple_ipa_execute passname %s dbgcounter %ld",
       current_pass->name, melt_dbgcounter);
    debugeprintf ("simple_ipa_execute passname %s before apply",
		  current_pass->name);
    dumpv = melt_get_inisysdata (FSYSDAT_DUMPFILE);
    if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
      {
	oldf = ((struct meltspecial_st*)dumpv)->val.sp_file;
	((struct meltspecial_st*)dumpv)->val.sp_file = dump_file;
      }
    debugeprintf ("meltgc_simple_ipa_execute pass %s before apply", current_pass->name);
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:meltgc_simple_ipa_execute pass %s before apply",
	      basename (__FILE__), __LINE__, current_pass->name);
    curfram__.flocs = locbuf;
  }
#endif
    /* apply with one extra long result */
    resvalv =
      melt_apply ((struct meltclosure_st *) closv,
		  (melt_ptr_t) passv, "",
		  (union meltparam_un *) 0, BPARSTR_LONG "",
		  restab);
    if (melt_magic_discr ((melt_ptr_t) dumpv) == OBMAG_SPEC_RAWFILE) 
      {
	FILE *df = melt_get_file ((melt_ptr_t) dumpv);
	if (df)
	  fflush (df);
	((struct meltspecial_st*)dumpv)->val.sp_file = oldf;
      };
    debugeprintf ("simple_ipa_execute passname %s after apply dbgcounter %ld",
		  current_pass->name, passdbgcounter);
    if (resvalv)
      res = (unsigned int) todol;
    /* force a minor GC to be sure that nothing is in the young region */
    melt_garbcoll (0, MELT_ONLY_MINOR);
  }
 end:
  MELT_EXITFRAME();
  return res;
#undef passv       
#undef passdictv   
#undef closv       
#undef resvalv     
#undef dumpv       
}





/* register a MELT pass; there is no way to unregister it, and the
   opt_pass and plugin_pass used internally are never deallocated.
   Non-simple IPA passes are not yet implemented! */
void
meltgc_register_pass (melt_ptr_t pass_p, 
			 const char* positioning, 
			 const char*refpassname,
			 int refpassnum)
{
  static const char*modstr;
  /* the plugin_pass can be local, since it is only locally used in
     plugin.c */
  struct plugin_pass plugpass = { NULL, NULL, 0, PASS_POS_INSERT_AFTER };
  enum pass_positioning_ops posop = PASS_POS_INSERT_AFTER;
  unsigned long propreq=0, propprov=0, propdest=0, todostart=0, todofinish=0;
  MELT_ENTERFRAME (7, NULL);
#define passv        curfram__.varptr[0]
#define passdictv    curfram__.varptr[1]
#define compv        curfram__.varptr[2]
#define namev        curfram__.varptr[3]
  passv = pass_p;
  debugeprintf ("meltgc_register_pass start passv %p refpassname %s positioning %s",
		(void*)passv, refpassname, positioning);
  if (!modstr)
    modstr = melt_argument("mode");
  if (!modstr || !modstr[0])
    goto end;
  if (!refpassname || !refpassname[0]) 
    goto end;
  if (!positioning || !positioning[0])
    goto end;
  if (!strcasecmp(positioning,"after"))
    posop = PASS_POS_INSERT_AFTER;
  else if (!strcasecmp(positioning,"before"))
    posop = PASS_POS_INSERT_BEFORE;
  else if (!strcasecmp(positioning,"replace"))
    posop = PASS_POS_REPLACE;
  else fatal_error("invalid positioning string %s in MELT pass", positioning);
  if (!passv || melt_object_length((melt_ptr_t) passv) < FGCCPASS__LAST
      || !melt_is_instance_of((melt_ptr_t) passv, 
			      (melt_ptr_t) MELT_PREDEF(CLASS_GCC_PASS)))
    goto end;
  namev = melt_object_nth_field((melt_ptr_t) passv, FNAMED_NAME);
  if (melt_magic_discr((melt_ptr_t) namev) != OBMAG_STRING)
    {
      warning (0, "registering a MELT pass without any name!");
      goto end;
    };
  debugeprintf ("meltgc_register_pass name %s refpassname %s positioning %s posop %d",
		melt_string_str ((melt_ptr_t) namev), refpassname, positioning, (int)posop);
  
  passdictv = melt_get_inisysdata (FSYSDAT_PASS_DICT);
  if (melt_magic_discr((melt_ptr_t)passdictv) != OBMAG_MAPSTRINGS) 
    goto end;
  if (melt_get_mapstrings((struct meltmapstrings_st*)passdictv, 
			  melt_string_str((melt_ptr_t) namev)))
    goto end;
  compv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_PROPERTIES_REQUIRED);
  propreq = melt_val2passflag((melt_ptr_t) compv);
  compv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_PROPERTIES_PROVIDED);
  propprov = melt_val2passflag((melt_ptr_t) compv);
  compv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_TODO_FLAGS_START);
  todostart = melt_val2passflag((melt_ptr_t) compv);
  compv = melt_object_nth_field((melt_ptr_t) passv, FGCCPASS_TODO_FLAGS_FINISH);
  todofinish = melt_val2passflag((melt_ptr_t) compv);
  /* allocate the opt pass and fill it; it is never deallocated (ie it
     is never free-d)! */
  if (melt_is_instance_of((melt_ptr_t) passv,
			  (melt_ptr_t) MELT_PREDEF(CLASS_GCC_GIMPLE_PASS))) {
    struct gimple_opt_pass* gimpass = NULL;     
    gimpass = XNEW(struct gimple_opt_pass);
    memset(gimpass, 0, sizeof(struct gimple_opt_pass));
    gimpass->pass.type = GIMPLE_PASS;
    /* the name of the pass is also strduped and is never deallocated
       (so it it never free-d! */
    gimpass->pass.name = xstrdup(melt_string_str((melt_ptr_t) namev));
    gimpass->pass.gate = meltgc_gimple_gate;
    gimpass->pass.execute = meltgc_gimple_execute;
    gimpass->pass.tv_id = TV_PLUGIN_RUN;
    gimpass->pass.properties_required = propreq;
    gimpass->pass.properties_provided = propprov;
    gimpass->pass.properties_destroyed = propdest;
    gimpass->pass.todo_flags_start = todostart;
    gimpass->pass.todo_flags_finish = todofinish;
    plugpass.pass = (struct opt_pass*) gimpass;
    plugpass.reference_pass_name = refpassname;
    plugpass.ref_pass_instance_number = refpassnum;
    plugpass.pos_op = posop;
    debugeprintf ("meltgc_register_pass name %s GIMPLE_PASS %p refpassname %s",
		  melt_string_str ((melt_ptr_t) namev), (void*)gimpass, refpassname);
    register_callback(melt_plugin_name, PLUGIN_PASS_MANAGER_SETUP, 
		      NULL, &plugpass);
    /* add the pass into the pass dict */
    meltgc_put_mapstrings((struct meltmapstrings_st*) passdictv,
			  gimpass->pass.name, (melt_ptr_t) passv);
  }
  else if (melt_is_instance_of((melt_ptr_t) passv, 
			       (melt_ptr_t) MELT_PREDEF(CLASS_GCC_RTL_PASS))) {
    struct rtl_opt_pass* rtlpass = NULL;     
    rtlpass = XNEW(struct rtl_opt_pass);
    memset(rtlpass, 0, sizeof(struct rtl_opt_pass));
    rtlpass->pass.type = RTL_PASS;
    /* the name of the pass is also strduped and is never deallocated
       (so it it never free-d! */
    rtlpass->pass.name = xstrdup(melt_string_str((melt_ptr_t) namev));
    rtlpass->pass.gate = meltgc_rtl_gate;
    rtlpass->pass.execute = meltgc_rtl_execute;
    rtlpass->pass.tv_id = TV_PLUGIN_RUN;
    rtlpass->pass.properties_required = propreq;
    rtlpass->pass.properties_provided = propprov;
    rtlpass->pass.properties_destroyed = propdest;
    rtlpass->pass.todo_flags_start = todostart;
    rtlpass->pass.todo_flags_finish = todofinish;
    plugpass.pass = (struct opt_pass*)rtlpass;
    plugpass.reference_pass_name = refpassname;
    plugpass.ref_pass_instance_number = refpassnum;
    plugpass.pos_op = posop;
    debugeprintf ("meltgc_register_pass name %s RTL_PASS %p refpassname %s",
		  melt_string_str ((melt_ptr_t) namev), (void*)rtlpass, refpassname);
    register_callback(melt_plugin_name, PLUGIN_PASS_MANAGER_SETUP, 
		      NULL, &plugpass);
    /* add the pass into the pass dict */
    meltgc_put_mapstrings((struct meltmapstrings_st*) passdictv,
			  rtlpass->pass.name, (melt_ptr_t) passv);
  }
  else if (melt_is_instance_of((melt_ptr_t) passv,
			       (melt_ptr_t) MELT_PREDEF(CLASS_GCC_SIMPLE_IPA_PASS))) {
    struct simple_ipa_opt_pass* sipapass = NULL;     
    sipapass = XNEW(struct simple_ipa_opt_pass);
    memset(sipapass, 0, sizeof(struct simple_ipa_opt_pass));
    sipapass->pass.type = SIMPLE_IPA_PASS;
    /* the name of the pass is also strduped and is never deallocated
       (so it it never free-d! */
    sipapass->pass.name = xstrdup(melt_string_str((melt_ptr_t) namev));
    sipapass->pass.gate = meltgc_simple_ipa_gate;
    sipapass->pass.execute = meltgc_simple_ipa_execute;
    sipapass->pass.tv_id = TV_PLUGIN_RUN;
    sipapass->pass.properties_required = propreq;
    sipapass->pass.properties_provided = propprov;
    sipapass->pass.properties_destroyed = propdest;
    sipapass->pass.todo_flags_start = todostart;
    sipapass->pass.todo_flags_finish = todofinish;
    plugpass.pass = (struct opt_pass*) sipapass;
    plugpass.reference_pass_name = refpassname;
    plugpass.ref_pass_instance_number = refpassnum;
    plugpass.pos_op = posop;
    debugeprintf ("meltgc_register_pass name %s SIMPLE_IPA_PASS %p refpassname %s",
		  melt_string_str ((melt_ptr_t) namev), (void*)sipapass, refpassname);
    register_callback(melt_plugin_name, PLUGIN_PASS_MANAGER_SETUP, 
		      NULL, &plugpass);
    /* add the pass into the pass dict */
    meltgc_put_mapstrings((struct meltmapstrings_st*) passdictv,
			  sipapass->pass.name, (melt_ptr_t) passv);
  }
  /* non simple ipa passes are a different story - TODO! */
  else 
    fatal_error ("MELT cannot register pass %s of unexpected class %s",
		 melt_string_str ((melt_ptr_t) namev), 
		 melt_string_str (melt_object_nth_field 
				  ((melt_ptr_t) melt_discr((melt_ptr_t) passv), 
				   FNAMED_NAME)));
 end:
  debugeprintf ("meltgc_register_pass name %s refpassname %s end",
		melt_string_str ((melt_ptr_t) namev), refpassname);
  
  MELT_EXITFRAME();
#undef passv
#undef passdictv
#undef namev
}








/*****
 * called from handle_melt_attribute
 *****/

void
melt_handle_melt_attribute (tree decl, tree name, const char *attrstr,
			       location_t loch)
{
  MELT_ENTERFRAME (4, NULL);
#define seqv       curfram__.varptr[0]
#define declv      curfram__.varptr[1]
#define namev      curfram__.varptr[2]
#define atclov	   curfram__.varptr[3]
  if (!attrstr || !attrstr[0])
    goto end;
  seqv = meltgc_read_from_rawstring (attrstr, "*melt-attr*", loch);
  atclov = melt_get_inisysdata (FSYSDAT_MELTATTR_DEFINER);
  if (melt_magic_discr ((melt_ptr_t) atclov) == OBMAG_CLOSURE)
    {
      union meltparam_un argtab[2];
      MELT_LOCATION_HERE ("melt attribute definer");
      declv =
	meltgc_new_tree ((meltobject_ptr_t) MELT_PREDEF (DISCR_TREE),
			    decl);
      namev =
	meltgc_new_tree ((meltobject_ptr_t) MELT_PREDEF (DISCR_TREE),
			    name);
      memset (argtab, 0, sizeof (argtab));
      argtab[0].bp_aptr = (melt_ptr_t *) & namev;
      argtab[1].bp_aptr = (melt_ptr_t *) & seqv;
#if ENABLE_CHECKING
  {
    static char locbuf[80];
    memset (locbuf, 0, sizeof (locbuf));
    snprintf (locbuf, sizeof (locbuf) - 1,
	      "%s:%d:melt_handle_melt_attribute %s before apply",
	      basename (__FILE__), __LINE__, attrstr);
    curfram__.flocs = locbuf;
  }
#endif
      (void) melt_apply ((meltclosure_ptr_t) atclov,
			    (melt_ptr_t) declv,
			    BPARSTR_PTR BPARSTR_PTR, argtab, "", NULL);
    }
end:
  MELT_EXITFRAME ();
#undef seqv
#undef declv
#undef namev
#undef atclov
}



#include "gt-melt-runtime.h"
/* eof melt-runtime.c */
