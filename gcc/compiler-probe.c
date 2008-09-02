/* Compiler probe
   Copyright (C) 2007,2008 Free Software Foundation, Inc.
   Contributed by Basile Starynkevitch <basile@starynkevitch.net>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "regs.h"
#include "timevar.h"
#include "intl.h"
#include "diagnostic.h"
#include "hashtab.h"
#include "tree.h"
#include "tree-pass.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "tree-inline.h"
#include "tree-iterator.h"
#include "diagnostic.h"
#include "toplev.h"
#include "ggc.h"
#include "vec.h"
#include "cgraph.h"
#include "flags.h"
#include "options.h"
#include "safe-ctype.h"
#include "basic-block.h"
#include "rtl.h"
#include "version.h"

#include "compiler-probe.h"

#if !defined(ENABLE_COMPILER_PROBE) || ENABLE_COMPILER_PROBE==0
/* this file is linked in only if the compiler probe is enabled at
   configure time */
#error messy configuration: compiler-probe.c compiled but not enabled
#endif

const char *tree_code_names[] = {
#define  DEFTREECODE(SYM, STRING, TYPE, NARGS) STRING,
#include "tree.def"
#undef DEFTREECODE
  (char *) 0
};


#define debugeprintf_raw(Fmt,...) do{if (flag_compiler_probe_debug) \
      {fprintf(stderr, Fmt, ##__VA_ARGS__); fflush(stderr);}}while(0)
#define debugeprintf(Fmt,...) debugeprintf_raw("@!%s:%d: " Fmt "\n", \
       basename(__FILE__), __LINE__, ##__VA_ARGS__)
/* some debugeprintf give warnings so I disable them */
#define nodebugeprintf(Fmt,...) do {}while(0)

/* #define nodebugprintf debugprintf */

#if ! defined( SIGCHLD ) && defined( SIGCLD )
#  define SIGCHLD SIGCLD
#endif

/* set this flag on SIGIO */
volatile sig_atomic_t comprobe_interrupted;

int comprobe_bb_ok_rtl;		/* declared in basic-block.h */

/* the probe command process */
pid_t comprobe_pid;
/* the command stream piped  into the compiler probe process */
FILE *comprobe_replf;
/* the file descriptor for requests from the probe process; it is never 0 */
int comprobe_reqfd;



/* grace delay in milliseconds for the compiler probe */
#define PROBE_GRACE_DELAY_MS   250


struct proberequest_buffer_st
{
  unsigned len;
  unsigned used;
  char str[1];			/* actual size is len, zero-terminated */
};
/* the buffer above is allocate in multiple of (should be a power of 2) : */
#define PROBUF_GRAN 0x1000
static struct proberequest_buffer_st *proberequest_buf;


/* hash table for requests (from probe to compiler) handlers contain entries like */
struct proberequesthentry_st
{
  const char *verb;		/* strdup-ed */
  void *data;			/* explicitly allocated & freed by caller */
  comprobe_requestfun_t *rout;
};

static htab_t proberequest_htable;


/* filename are e.g. unix paths */
typedef char *filename_t;
/* hash table for filenames contain entries like */
struct filenamehentry_st
{
  filename_t file;		/* strdup-ed filename */
  int rank;			/* strictly positive rank in filename vector */
};

static htab_t filename_htable;

/* *INDENT-OFF* */
static GTY(()) VEC(tree,gc) *unique_tree_vector;
static GTY(()) VEC(gimple,gc) *unique_gimple_vector;
static GTY(()) VEC(basic_block,gc) *unique_bb_vector;
/* each tree of unique_tree_vector or basic_block of unique_bb_vector is
   unique; we manage an hash table of indexes (>2) there. the index 0
   is HTAB_EMPTY_ENTRY, the index 1 is HTAB_DELETED_ENTRY, the index 2
   is for the seeked entry */
static GTY (()) tree unique_seeked_tree;
static GTY (()) basic_block unique_seeked_bb;
static GTY (()) gimple unique_seeked_gimple;
/* *INDENT-ON* */
/* hashtables of integer indexes into the unique_tree_vector & unique_bb_vector */
static htab_t unique_tree_htable;
static htab_t unique_bb_htable;
static htab_t unique_gimple_htable;
#define HTAB_SEEKED_ENTRY ((PTR) 2)

/***
we would like to use vectors of filenames, but I cannot make them work
   with GTY:
       // see thread http://gcc.gnu.org/ml/gcc/2007-01/msg00172.html
       DEF_VEC_P (filename_t); 
       DEF_VEC_ALLOC_P (filename_t,heap); // don't work
       VEC(filename_t,heap) *file_vector;
 the gengtype parser choke on this with an error: unidentified type
`filename_t'
****/

/* variable array of filenames */
static struct
{
  int size;			/* allocated size */
  int last;			/* last used index */
  char **tab;
} files_varr;

struct displaychoice_st
{
  long di_magic;
#define DI_MAGIC 2491573L
  HOST_WIDE_INT di_data;
  comprobe_infodisplay_fun_t *di_fun;
  char di_msg[1];		/* duplicated display message
				   - longer than 1 */
};
typedef struct displaychoice_st *displaychoice_ptr_t;
/* *INDENT-OFF* */
DEF_VEC_P (displaychoice_ptr_t);
DEF_VEC_ALLOC_P (displaychoice_ptr_t, heap);

struct comprobe_infodisplay_st {
  int idis_num;			/* unique infodisplay number */
  int idis_choice;			/* current display choice*/
  struct infopointhentry_st* idis_infp;	/* the information point */
  VEC(displaychoice_ptr_t,heap) *idis_navig; /* the navigation vector */
  void*idis_data;		/* client data */
  void (*idis_destroy)(struct comprobe_infodisplay_st*idi); /* destructor */
};

typedef struct comprobe_infodisplay_st* infodisplay_ptr_t;
DEF_VEC_P(infodisplay_ptr_t);
DEF_VEC_ALLOC_P(infodisplay_ptr_t,heap);
static VEC(infodisplay_ptr_t,heap) *infodisplay_vector;

/* the info point hashtable contains entries like : */
struct infopointhentry_st
{
  int infp_filerank;	/* file rank */
  int infp_lineno;		/* line number */
  int infp_num;		/* unique infopoint number */
  VEC(displaychoice_ptr_t,heap) *infp_dischvec;
};
typedef struct infopointhentry_st *infopoint_ptr_t;
static htab_t infopoint_htable;
DEF_VEC_P (infopoint_ptr_t);
DEF_VEC_ALLOC_P (infopoint_ptr_t, heap);
VEC (infopoint_ptr_t, heap) * infopoint_vector;
/* *INDENT-ON* */

/* random data for multiline replies */
static struct drand48_data randata;


/****
 * supporting functions for probe reply hashtable 
 *****/
static hashval_t
hash_proberequest (const void *d)
{
  const struct proberequesthentry_st *p = (const struct proberequesthentry_st *) d;
  return htab_hash_string (p->verb);
}

static int
eq_proberequest (const void *dx, const void *dy)
{
  const struct proberequesthentry_st *px = (const struct proberequesthentry_st *) dx;
  const struct proberequesthentry_st *py = (const struct proberequesthentry_st *) dy;
  return !strcmp (px->verb, py->verb);
}

static void
del_proberequest (void *d)
{
  struct proberequesthentry_st *p = (const struct proberequesthentry_st *) d;
  gcc_assert (p && p->verb);
  free ((void *) p->verb);
  p->verb = NULL;
}



/****
 * supporting functions for filename hashtable 
 *****/
static hashval_t
hash_filename (const void *d)
{
  const struct filenamehentry_st *p = (struct filenamehentry_st *) d;
  return htab_hash_string (p->file);
}

static int
eq_filename (const void *dx, const void *dy)
{
  const struct filenamehentry_st *px = (struct filenamehentry_st *) dx;
  const struct filenamehentry_st *py = (struct filenamehentry_st *) dy;
  return !strcmp (px->file, py->file);
}

static void
del_filename (void *d)
{
  struct filenamehentry_st *p = (struct filenamehentry_st *) d;
  gcc_assert (p && p->file && p->rank > 0);
  free ((void *) p->file);
  p->file = NULL;
}

/****
 * supporting functions for infopoint hashtable 
 *****/
static hashval_t
hash_infopoint (const void *d)
{
  const struct infopointhentry_st *ifp = (const struct infopointhentry_st *) d;
  return (hashval_t)
    (((long) (ifp->infp_filerank << 12)) ^ ((long) ifp->infp_lineno));
}

static int
eq_infopoint (const void *dx, const void *dy)
{
  const struct infopointhentry_st *ifx = (const struct infopointhentry_st *) dx;
  const struct infopointhentry_st *ify = (const struct infopointhentry_st *) dy;
  return ifx->infp_lineno == ify->infp_lineno
    && ifx->infp_filerank == ify->infp_filerank;
}

/***
 * supporting functions for unique_tree_htable
 ***/
static hashval_t
hash_info_tree (const void *d)
{
  comprobe_ix_t lg = (comprobe_ix_t) d;
  switch (lg)
    {
    case (comprobe_ix_t) HTAB_EMPTY_ENTRY:
    case (comprobe_ix_t) HTAB_DELETED_ENTRY:
      return (hashval_t) 0;
    case (comprobe_ix_t) HTAB_SEEKED_ENTRY:
      lg = (comprobe_ix_t) unique_seeked_tree;
      return (hashval_t) (lg ^ (lg >> 10));
    default:
      if (lg > 2 && unique_tree_vector
	  && lg < VEC_length (tree, unique_tree_vector))
	{
	  lg = (comprobe_ix_t) VEC_index (tree, unique_tree_vector, lg);
	  return (hashval_t) (lg ^ (lg >> 10));
	};
      return 0;
    }
}


static int
eq_info_tree (const void *dx, const void *dy)
{
  comprobe_ix_t lx = (comprobe_ix_t) dx;
  comprobe_ix_t ly = (comprobe_ix_t) dy;
  comprobe_ix_t nbtree = 0;
  tree tx = NULL_TREE, ty = NULL_TREE;
  if (lx == ly)
    return 1;
  if (unique_tree_vector)
    nbtree = VEC_length (tree, unique_tree_vector);
  else
    return 0;
  if (lx == (comprobe_ix_t) HTAB_SEEKED_ENTRY)
    tx = unique_seeked_tree;
  else if (lx > 2 && lx < nbtree)
    tx = VEC_index (tree, unique_tree_vector, lx);
  if (ly == (comprobe_ix_t) HTAB_SEEKED_ENTRY)
    ty = unique_seeked_tree;
  else if (ly > 2 && ly < nbtree)
    ty = VEC_index (tree, unique_tree_vector, ly);
  return tx == ty && tx != NULL_TREE;
}


comprobe_ix_t
comprobe_unique_index_of_tree (tree tr)
{
  comprobe_ix_t trix = 0;
  comprobe_ix_t l = 0, nbtree = 0;
  void **sp = NULL;
  if (tr == NULL_TREE)
    return 0;
  gcc_assert (unique_tree_vector
	      && VEC_length (tree, unique_tree_vector) > 2);
  l = (comprobe_ix_t) HTAB_SEEKED_ENTRY;
  nbtree = VEC_length (tree, unique_tree_vector);
  unique_seeked_tree = tr;
  sp = htab_find_slot (unique_tree_htable, &l, INSERT);
  if (sp)
    {
      if (*sp != HTAB_EMPTY_ENTRY && *sp != HTAB_DELETED_ENTRY
	  && *sp != HTAB_SEEKED_ENTRY)
	l = *(comprobe_ix_t *) (*sp);
      else
	l = 0;
      if (l > 2)
	{
	  gcc_assert (l < nbtree
		      && VEC_index (tree, unique_tree_vector, l) == tr);
	  trix = l;
	}
      else
	{
	  VEC_safe_push (tree, gc, unique_tree_vector, tr);
	  trix = nbtree;
	  *(comprobe_ix_t *) (sp) = trix;
	}
    }
  else				/* failed to insert into unique_tree_htable */
    gcc_unreachable ();
  return trix;
}

tree
comprobe_tree_of_unique_index (comprobe_ix_t ix)
{
  unsigned nbtree = 0;
  if (ix < 2 || !unique_tree_vector)
    return 0;
  nbtree = VEC_length (tree, unique_tree_vector);
  if ((int) ix < (int) nbtree)
    return VEC_index (tree, unique_tree_vector, ix);
  return 0;
}



/***
 * supporting functions for unique_bb_htable
 ***/
static hashval_t
hash_info_bb (const void *d)
{
  comprobe_ix_t lg = (comprobe_ix_t) d;
  switch (lg)
    {
    case (comprobe_ix_t) HTAB_EMPTY_ENTRY:
    case (comprobe_ix_t) HTAB_DELETED_ENTRY:
      return (hashval_t) 0;
    case (comprobe_ix_t) HTAB_SEEKED_ENTRY:
      lg = (comprobe_ix_t) unique_seeked_bb;
      return (hashval_t) (lg ^ (lg >> 10));
    default:
      if (lg > 2 && unique_bb_vector
	  && lg < VEC_length (basic_block, unique_bb_vector))
	{
	  lg = (comprobe_ix_t) VEC_index (basic_block, unique_bb_vector, lg);
	  return (hashval_t) (lg ^ (lg >> 10));
	};
      return 0;
    }
}


static int
eq_info_bb (const void *dx, const void *dy)
{
  comprobe_ix_t lx = (comprobe_ix_t) dx;
  comprobe_ix_t ly = (comprobe_ix_t) dy;
  long nbbb = 0;
  basic_block bx = NULL, by = NULL;
  if (lx == ly)
    return 1;
  if (unique_bb_vector)
    nbbb = VEC_length (basic_block, unique_bb_vector);
  else
    return 0;
  if (lx == (comprobe_ix_t) HTAB_SEEKED_ENTRY)
    bx = unique_seeked_bb;
  else if (lx > 2 && lx < nbbb)
    bx = VEC_index (basic_block, unique_bb_vector, lx);
  if (ly == (comprobe_ix_t) HTAB_SEEKED_ENTRY)
    by = unique_seeked_bb;
  else if (ly > 2 && ly < nbbb)
    by = VEC_index (basic_block, unique_bb_vector, ly);
  return bx == by && bx != NULL;
}

/****@@@ ADDING****/

/***
 * supporting functions for unique_gimple_htable
 ***/
static hashval_t
hash_info_gimple (const void *d)
{
  comprobe_ix_t lg = (comprobe_ix_t) d;
  switch (lg)
    {
    case (comprobe_ix_t) HTAB_EMPTY_ENTRY:
    case (comprobe_ix_t) HTAB_DELETED_ENTRY:
      return (hashval_t) 0;
    case (comprobe_ix_t) HTAB_SEEKED_ENTRY:
      lg = (comprobe_ix_t) unique_seeked_gimple;
      return (hashval_t) (lg ^ (lg >> 10));
    default:
      if (lg > 2 && unique_gimple_vector
	  && lg < VEC_length (gimple, unique_gimple_vector))
	{
	  lg = (comprobe_ix_t) VEC_index (gimple, unique_gimple_vector, lg);
	  return (hashval_t) (lg ^ (lg >> 10));
	};
      return 0;
    }
}


static int
eq_info_gimple (const void *dx, const void *dy)
{
  comprobe_ix_t lx = (comprobe_ix_t) dx;
  comprobe_ix_t ly = (comprobe_ix_t) dy;
  comprobe_ix_t nbgimple = 0;
  gimple tx = NULL, ty = NULL;
  if (lx == ly)
    return 1;
  if (unique_gimple_vector)
    nbgimple = VEC_length (gimple, unique_gimple_vector);
  else
    return 0;
  if (lx == (comprobe_ix_t) HTAB_SEEKED_ENTRY)
    tx = unique_seeked_gimple;
  else if (lx > 2 && lx < nbgimple)
    tx = VEC_index (gimple, unique_gimple_vector, lx);
  if (ly == (comprobe_ix_t) HTAB_SEEKED_ENTRY)
    ty = unique_seeked_gimple;
  else if (ly > 2 && ly < nbgimple)
    ty = VEC_index (gimple, unique_gimple_vector, ly);
  return tx == ty && tx != NULL;
}


comprobe_ix_t
comprobe_unique_index_of_gimple (gimple tr)
{
  comprobe_ix_t trix = 0;
  comprobe_ix_t l = 0, nbgimple = 0;
  void **sp = NULL;
  if (tr == NULL)
    return 0;
  gcc_assert (unique_gimple_vector
	      && VEC_length (gimple, unique_gimple_vector) > 2);
  l = (comprobe_ix_t) HTAB_SEEKED_ENTRY;
  nbgimple = VEC_length (gimple, unique_gimple_vector);
  unique_seeked_gimple = tr;
  sp = htab_find_slot (unique_gimple_htable, &l, INSERT);
  if (sp)
    {
      if (*sp != HTAB_EMPTY_ENTRY && *sp != HTAB_DELETED_ENTRY
	  && *sp != HTAB_SEEKED_ENTRY)
	l = *(comprobe_ix_t *) (*sp);
      else
	l = 0;
      if (l > 2)
	{
	  gcc_assert (l < nbgimple
		      && VEC_index (gimple, unique_gimple_vector, l) == tr);
	  trix = l;
	}
      else
	{
	  VEC_safe_push (gimple, gc, unique_gimple_vector, tr);
	  trix = nbgimple;
	  *(comprobe_ix_t *) (sp) = trix;
	}
    }
  else				/* failed to insert into unique_gimple_htable */
    gcc_unreachable ();
  return trix;
}

gimple
comprobe_gimple_of_unique_index (comprobe_ix_t ix)
{
  unsigned nbgimple = 0;
  if (ix < 2 || !unique_gimple_vector)
    return 0;
  nbgimple = VEC_length (gimple, unique_gimple_vector);
  if ((int) ix < (int) nbgimple)
    return VEC_index (gimple, unique_gimple_vector, ix);
  return 0;
}


/****@@@ ADDED****/
/****
 * register a reply verb
 ****/
static void
comprobe_register_unchecked (const char *verb,
			     comprobe_requestfun_t * handler, void *data)
{
  struct proberequesthentry_st slot;
  struct proberequesthentry_st **slotptr;
  hashval_t h;
  memset (&slot, 0, sizeof (slot));
  slot.verb = verb;
  h = hash_proberequest (&slot);
  slotptr = (struct proberequesthentry_st **)
    htab_find_slot_with_hash (proberequest_htable, &slot, h, INSERT);
  if (!slotptr)
    fatal_error
      ("compiler probe failed to register request verb %s (memory full): %m",
       verb);
  if (*slotptr == HTAB_EMPTY_ENTRY || (*slotptr) == HTAB_DELETED_ENTRY)
    {
      struct proberequesthentry_st *newslot;
      newslot = (struct proberequesthentry_st *) xcalloc (sizeof (struct proberequesthentry_st), 1);
      newslot->verb = xstrdup (verb);
      newslot->rout = handler;
      newslot->data = data;
      *slotptr = newslot;
    }
  else
    {
      struct proberequesthentry_st *oldslot = *slotptr;
      gcc_assert (!strcmp (oldslot->verb, verb));
      oldslot->rout = handler;
      oldslot->data = data;
    }
}


/****
 * register a verb (and check that it is alphanumerical) 
 ****/
void
comprobe_register (const char *verb, comprobe_requestfun_t * handler,
		   void *data)
{
  const char *pc;
  if (!verb)
    return;
  if (!ISALPHA (verb[0]))
    fatal_error ("compiler probe: invalid verb %s to register", verb);
  for (pc = verb + 1; *pc; pc++)
    if (!ISALNUM (*pc) && *pc != '_')
      fatal_error ("compiler probe: invalid verb %s to register", verb);
  comprobe_register_unchecked (verb, handler, data);
}


/****
 * remove a registered verb  @@@@@@ HAS TO BE CODED
 ****/
void
comprobe_unregister (const char *verb)
{
  gcc_assert (verb && *verb);
  /* #warning to be written comprobe_unregister */
  gcc_unreachable ();
}


void
comprobe_infopoint_add_display (int infoptrank,
				comprobe_infodisplay_fun_t * dispfun,
				const char *msg, HOST_WIDE_INT data)
{
  infopoint_ptr_t infp = NULL;
  displaychoice_ptr_t dch = NULL;
  size_t msgl = 0;
  if (!comprobe_replf)
    return;
  debugeprintf ("infopoint_add_display  infoptrank %d msg %s", infoptrank,
		msg);
  if (infoptrank < 0 || !infopoint_vector
      || infoptrank >= (int) VEC_length (infopoint_ptr_t, infopoint_vector))
    return;
  infp = VEC_index (infopoint_ptr_t, infopoint_vector, infoptrank);
  nodebugeprintf ("infopoint_add_display infp %p msg %s dispfun %p",
		infp, msg, (void *) dispfun);
  if (!infp || !msg || !dispfun)
    return;
  msgl = strlen (msg);
  gcc_assert (infp->infp_num == infoptrank);
  dch = (displaychoice_ptr_t) xcalloc (sizeof (*dch) + msgl, 1);
  dch->di_data = data;
  dch->di_magic = DI_MAGIC;
  dch->di_fun = dispfun;
  memcpy (dch->di_msg, msg, msgl);
  debugeprintf ("infopoint_add_display dch %p", (void*)dch);
  VEC_safe_push (displaychoice_ptr_t, heap, infp->infp_dischvec, dch);
}

void
comprobe_display_add_navigator (struct comprobe_infodisplay_st *idi,
				comprobe_infodisplay_fun_t * navfun,
				const char *msg, HOST_WIDE_INT data)
{
  displaychoice_ptr_t dch = NULL;
  size_t msgl = 0;
  gcc_assert (idi);
  gcc_assert (msg);
  if (!comprobe_replf)
    return;
  msgl = strlen (msg);
  if (!idi->idis_navig)
    idi->idis_navig = VEC_alloc (displaychoice_ptr_t, heap, 3);
  dch = (displaychoice_ptr_t) xcalloc (sizeof (*dch) + msgl, 1);
  dch->di_data = data;
  dch->di_fun = navfun;
  dch->di_magic = DI_MAGIC;
  memcpy (dch->di_msg, msg, msgl);
  nodebugeprintf ("add_navigator display %d navfun %p msg '%s'",
		idi->idis_num, (void *) navfun, msg);
  VEC_safe_push (displaychoice_ptr_t, heap, idi->idis_navig, dch);
}



/****
 * the SIGIO and SIGCHLD signal handler just sets a flag
 ****/
static void
sig_interrupted (int sig)
{
  gcc_assert (sig != 0);
  comprobe_interrupted = 1;
}


/****
 * Create the probing process and set up the pipes to it. 
 * Do not exchange anything yet 
 ****/
static void
create_probe_process (void)
{
  /* the probe process */
  pid_t probpid;
  /* the pipes from probe to gcc, and from gcc to probe */
  int pip2gcc[2], pip2probe[2];
  const char *progarg[5];
  if (comprobe_pid > 0 && comprobe_replf && comprobe_reqfd >= 0)
    return;
  /* we do not use the pex_* routines from liberty.h because the
     compiler probe is quite specific to Linux and similar Unix
     operating systems with SIGIO, select, .... (probably Solaris,
     and recent *BSD should be ok) */
  pip2gcc[0] = pip2gcc[1] = -1;
  pip2probe[0] = pip2probe[1] = -1;
  if (pipe (pip2gcc) || pipe (pip2probe))
    fatal_error ("failed to create pipes for compiler probe: %m");
  probpid = fork ();
  if (probpid == (pid_t) 0)
    {
      /* child process */
      int ifd;
      signal (SIGIO, SIG_DFL);
#ifdef SIGSEGV
      signal (SIGSEGV, SIG_DFL);
#endif
#ifdef SIGINT
      signal (SIGINT, SIG_DFL);
#endif
#ifdef SIGTERM
      signal (SIGTERM, SIG_DFL);
#endif
#ifdef SIGQUIT
      signal (SIGQUIT, SIG_DFL);
#endif
#define perror_exit(Msg) do{perror(Msg); exit(1);}while(0)
      /* write-pipe from probe to gcc is our stdout */
      if (pip2gcc[1] != STDOUT_FILENO)
	{
	  if (dup2 (pip2gcc[1], STDOUT_FILENO) < 0)
	    perror_exit
	      ("comprobe child process failed to dup2 pipe to stdout to gcc");
	  (void) close (pip2gcc[1]);
	}
      /* read-pipe from gcc to probe is our stdin */
      if (pip2probe[0] != STDIN_FILENO)
	{
	  if (dup2 (pip2probe[0], STDIN_FILENO) < 0)
	    perror_exit
	      ("comprobe child process failed to dup2 pipe to stdin from gcc");
	  (void) close (pip2probe[0]);
	}
      /* close useless fds in the child before exec */
      for (ifd = STDERR_FILENO + 1; ifd < 64; ifd++)
	(void) close (ifd);
      /* use sh -c for the compiler probe command */
      memset (progarg, 0, sizeof (progarg));
      progarg[0] = "sh";
      progarg[1] = "-c";
      progarg[2] = compiler_probe_string;
      progarg[3] = (char *) 0;
      execv ("/bin/sh", (char *const *) progarg);
      perror_exit ("comprobe child process failed to exec /bin/sh");
    }
#undef perror_exit
  else if (probpid < (pid_t) 0)
    {
      /* error fork failed */
      fatal_error ("failed to fork for compiler probe: %m");
    }
  else
    {				/* parent */
      (void) close (pip2gcc[1]);
      (void) close (pip2probe[0]);
      comprobe_pid = probpid;
      comprobe_replf = fdopen (pip2probe[1], "a");
      comprobe_reqfd = pip2gcc[0];
      if (!comprobe_replf)
	fatal_error ("failed to open pipe stream to compiler probe: %m");
      if (comprobe_reqfd <= 0)
	{
	  /* this should almost never happen; I could think it might
	     happen if gcc is run without any open file descriptor - not
	     even stdin; but I expect the request file descriptor to be
	     positive */
	  fatal_error ("failed to get pipe fd %d from compiler probe: %m",
		       comprobe_reqfd);
	};
      if (fcntl (comprobe_reqfd, F_SETFL, (long) O_NONBLOCK) < 0)
	fatal_error
	  ("failed to make non-blocking the pipe fd %d from compiler probe: %m",
	   comprobe_reqfd);
      if (fcntl (comprobe_reqfd, F_SETOWN, (long) getpid ()) < 0)
	fatal_error ("failed to SETOWN pipe fd %d from compiler probe: %m",
		     comprobe_reqfd);
      setlinebuf (comprobe_replf);
      fprintf (stderr, "created compiler probe '%s' process %ld\n",
	       compiler_probe_string, (long) comprobe_pid);
    }
}



/****
 * wait for the compiler probe process, returns 0 when waited successfully;
 * if BLOCKING is set, wait indefinitely, otherwise just test without
 * blocking
 ****/
static int
wait_for_probe (int blocking, int *pstatus)
{
  int probstatus = 0;
  gcc_assert (comprobe_pid > 0);
#if HAVE_WAITPID || HAVE_SYS_WAIT_H
  if (waitpid (comprobe_pid, &probstatus, blocking ? WNOHANG : 0) ==
      comprobe_pid)
    {
      if (pstatus)
	*pstatus = probstatus;
      return 0;
    }
#elif HAVE_WAIT4
  if (wait4
      (comprobe_pid, &probstatus, blocking ? WNOHANG : 0,
       (struct rusage *) 0) == comprobepid)
    {
      if (pstatus)
	*pstatus = probstatus;
      return 0;
    };
#else
#error should have waitpid or wait4
#endif
  return 1;
}

/****
 * output a string URL encoded wtih STR89' prefix for a string of 89
 * chars and a ' suffix
 ****/
void
comprobe_outenc_string (const char *s)
{
  int c, l;
  if (!comprobe_replf || !s)
    return;
  l = strlen (s);
  comprobe_printf ("STR%d'", l);
  for (; (c = (*s)) != 0; s++)
    {
      if ((c >= 'A' && c <= 'Z')
	  || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
	putc (c, comprobe_replf);
      else
	switch (c)
	  {
	  case ' ':
	    putc ('+', comprobe_replf);
	    break;
	  case '*':
	  case '/':
	  case '_':
	  case '@':
	  case '-':
	  case '(':
	  case ')':
	  case '!':
	  case '[':
	  case ']':
	  case '.':
	  case ',':
	  case ';':
	    putc (c, comprobe_replf);
	    break;
	  default:
	    fprintf (comprobe_replf, "%%%02x", c & 0xff);
	    break;
	  }
    }
  putc ('\'', comprobe_replf);
}

/****
 * process a single line of request (from probe to compiler) 
 ****/
static void
process_request_line (struct comprobe_whatpos_st *wp, char *line)
{
  struct proberequesthentry_st *ptrslot;
  struct proberequesthentry_st slot;
  char *pc;
  char *argreq;
  if (line[0] == '#' || line[0] == 0)
    return;
  debugeprintf ("req.lin: %s", line);
  memset (&slot, 0, sizeof (slot));
  for (pc = line; ISALNUM (*pc) || *pc == '_'; pc++);
  argreq = 0;
  if (*pc)
    argreq = pc + 1;
  *pc = 0;
  slot.verb = line;
  ptrslot =
    (struct proberequesthentry_st *) htab_find (proberequest_htable, &slot);
  if (ptrslot && ptrslot->rout)
    (*ptrslot->rout) (wp, argreq, ptrslot->data);
  else
    fprintf (stderr, "gcc compiler-probe don't understand request: %s\n",
	     line);
  debugeprintf ("req.lin done: %s", line);
}

/****
 * stop the compiler probe 
 ****/
void
comprobe_stop (void)
{
  int status = 0;
  if (!compiler_probe_string || !compiler_probe_string[0] || !comprobe_pid)
    return;
  /* kill the probing process and wait for it nicely, first by closing the pipe to it */
  fclose (comprobe_replf);
  close (comprobe_reqfd);
  /* give a little time to the probe */
  usleep ((unsigned long) PROBE_GRACE_DELAY_MS * 1000);
  if (wait_for_probe (0, &status))
    {
      (void) kill (comprobe_pid, SIGTERM);
      while (!wait_for_probe (1, &status))
	{
	  usleep ((unsigned long) PROBE_GRACE_DELAY_MS * 1000);
	  (void) kill (comprobe_pid, SIGQUIT);
	};
    };
  fprintf (stderr, "ended compiler probe process %ld [status %d]:",
	   (long) comprobe_pid, status);
  if (WIFEXITED (status))
    fprintf (stderr, "probe exited %d\n", WEXITSTATUS (status));
  else if (WIFSIGNALED (status))
    psignal (WTERMSIG (status),
	     WCOREDUMP (status)
	     ? "probe coredumped on signal" : "probe terminated with signal");
  putc ('\n', stderr);
  fflush (stderr);
  comprobe_reqfd = -1;
  comprobe_replf = (FILE *) 0;
  comprobe_pid = 0;
  compiler_probe_string = 0;
  signal (SIGIO, SIG_DFL);
  signal (SIGCHLD, SIG_DFL);
}

/* forced kill of probe - called only on unrecoverable errors */
void
comprobe_forced_kill (void)
{
  if (!compiler_probe_string || !compiler_probe_string[0]
      || !comprobe_pid || !comprobe_replf)
    return;
  fflush (comprobe_replf);
  (void) kill (comprobe_pid, SIGTERM);
  comprobe_stop ();
}

/****
 * read all probe requests, waiting for them for a delay in milliseconds 
 * and process every newline terminated reply line
 ****/
static void
read_probe_requests (struct comprobe_whatpos_st *wp, unsigned millisec)
{
  fd_set rdset;
  struct timeval tv;
  int maxfd = 0;
  int selnb = 0;
  if (comprobe_reqfd <= 0)
    return;
  /* flush the reply stream to send any pending stuff */
  if (comprobe_replf)
    fflush (comprobe_replf);
  FD_ZERO (&rdset);
  if (comprobe_reqfd >= 0)
    FD_SET (comprobe_reqfd, &rdset);
  maxfd = MAX (maxfd, comprobe_reqfd);
  if (millisec >= 1000)
    millisec = 999;
  tv.tv_sec = 0;
  tv.tv_usec = millisec * 1000;
  selnb = select (maxfd + 1, &rdset, (fd_set *) 0, (fd_set *) 0, &tv);
  if (selnb > 0 && FD_ISSET (comprobe_reqfd, &rdset))
    {
      int again = 0;
      do
	{
	  int newsiz;
	  int readcnt, readlen;
	  if (comprobe_reqfd < 0)
	    break;
	  newsiz = 0;
	  if (!proberequest_buf)
	    newsiz = 2 * PIPE_BUF;
	  else if (proberequest_buf->used + PIPE_BUF <= proberequest_buf->len)
	    newsiz =
	      (((5 * proberequest_buf->len / 4 + 2 * PIPE_BUF))
	       | (PROBUF_GRAN - 1)) + 1;
	  if (newsiz > 0)
	    {
	      struct proberequest_buffer_st *newbuf;
	      newbuf = (struct proberequest_buffer_st *)
		xcalloc (1, sizeof (struct proberequest_buffer_st) + newsiz - 1);
	      newbuf->len = newsiz;
	      if (proberequest_buf)
		{
		  gcc_assert (proberequest_buf->used <=
			      proberequest_buf->len);
		  memcpy (newbuf->str, proberequest_buf->str,
			  proberequest_buf->used);
		  newbuf->used = proberequest_buf->used;
		  free (proberequest_buf);
		};
	      proberequest_buf = newbuf;
	    };
	  gcc_assert (proberequest_buf->used + PIPE_BUF <
		      proberequest_buf->len);
	  readlen = proberequest_buf->len - proberequest_buf->used - 1;
	  readcnt = read (comprobe_reqfd,
			  proberequest_buf->str + proberequest_buf->used,
			  readlen);
	  again = 0;
	  if (readcnt == 0)
	    {			/* got end of file on the probe reply pipe */
	      again = 0;
	      comprobe_stop ();
	    }
	  else if (readcnt < 0)
	    {
	      if (errno == EINTR)
		again = 1;
	      else if (errno == EAGAIN)
		again = 0;
	      else
		fatal_error ("unexpected read error from compiler probe: %m");
	    }
	  else			/*readcnt>0 */
	    {
	      char *eol = 0;
	      char *pc = 0;
	      off_t off, rlen;
	      proberequest_buf->used += readcnt;
	      proberequest_buf->str[proberequest_buf->used] = 0;
	      for (pc = proberequest_buf->str; (eol = strchr (pc, '\n')) != 0;
		   pc = eol + 1)
		{
		  *eol = 0;
		  process_request_line (wp, pc);
		  if (comprobe_replf)
		    fflush (comprobe_replf);
		};
	      off = pc - proberequest_buf->str;
	      if (off > 0)
		{
		  rlen = proberequest_buf->str + proberequest_buf->used - pc;
		  memmove (proberequest_buf->str, pc, rlen);
		  proberequest_buf->str[rlen] = 0;
		  proberequest_buf->used = rlen;
		  if (proberequest_buf->len - rlen > 3 * PIPE_BUF)
		    {
		      unsigned newsiz =
			((rlen + 2 * PIPE_BUF) | (PROBUF_GRAN - 1)) + 1;
		      if (newsiz < proberequest_buf->len && newsiz > rlen)
			{
			  proberequest_buf = (struct proberequest_buffer_st *)
			    xrealloc (proberequest_buf, newsiz);
			  proberequest_buf->len = newsiz;
			}
		    }
		};
	      again = 1;
	    }
	}
      while (again);
    }
}

/****
 * handle a request (called by comprobe_check macro) 
 ****/
void
comprobe_handle_probe (const char *what, const char *file, int lineno)
{
  struct comprobe_whatpos_st wp;
  if (!compiler_probe_string || !compiler_probe_string[0]
      || comprobe_pid <= 0)
    return;
  memset (&wp, 0, sizeof (wp));
  wp.wp_what = what;
  wp.wp_file = file;
  wp.wp_line = lineno;
  read_probe_requests (&wp, 0);
}

/****
 * handle all requests until a given variable is cleared, or the probe ended 
 ****/
void
comprobe_while_probe (const char *what, const char *file, int lineno,
		      int *pvar)
{
  struct comprobe_whatpos_st wp;
  if (comprobe_pid <= 0 || !pvar)
    return;
  memset (&wp, 0, sizeof (wp));
  wp.wp_what = what;
  wp.wp_file = file;
  wp.wp_line = lineno;
  while (comprobe_pid > 0 && *pvar)
    {
      read_probe_requests (&wp, PROBE_GRACE_DELAY_MS);
    };
}

/***
 * big commands are uniquely bracketed 
 ***/
static long leftcode, rightcode;
void
comprobe_begin_big (void)
{
  gcc_assert (leftcode == 0 && rightcode == 0);
  if (!comprobe_replf)
    return;
  do
    {
      lrand48_r (&randata, &leftcode);
    }
  while (leftcode == 0);
  do
    {
      lrand48_r (&randata, &rightcode);
    }
  while (rightcode == 0);
  fprintf (comprobe_replf, "\n!#%x/%X[\n",
	   (int) (leftcode & 0xfffffff), (int) (rightcode & 0xfffffff));
}

void
comprobe_end_big (void)
{
  gcc_assert (leftcode != 0 && rightcode != 0);
  if (comprobe_replf)
    {
      fprintf (comprobe_replf, "\n!#%x/%X]\n",
	       (int) (leftcode & 0xfffffff), (int) (rightcode & 0xfffffff));
      fflush (comprobe_replf);
    }
  leftcode = rightcode = 0;
}


static int nonstopped;

/* the STOP request from probe stops properly the probe */
static void
stop_reqfun (struct comprobe_whatpos_st *wp ATTRIBUTE_UNUSED,
	     char *reqlin ATTRIBUTE_UNUSED, void *data ATTRIBUTE_UNUSED)
{
  if (comprobe_replf)
    fflush (comprobe_replf);
  nonstopped = 0;
  comprobe_stop ();
}

static int
cmp_displaychoice_ptr (const void *x, const void *y)
{
  displaychoice_ptr_t dx = *(displaychoice_ptr_t *) x;
  displaychoice_ptr_t dy = *(displaychoice_ptr_t *) y;
  return strcmp (dx->di_msg, dy->di_msg);
}

/* the NEWINFODIALOG request from probe make a new dialog for an infopoint */
static void
newinfodialog_reqfun (struct comprobe_whatpos_st *wp ATTRIBUTE_UNUSED,
		      char *reqlin, void *data ATTRIBUTE_UNUSED)
{
  int infork = -1, dialrk = -1, nbdisp = 0, chix = 0;
  infopoint_ptr_t ip = NULL;
  infodisplay_ptr_t idisp = NULL;
  if (!comprobe_replf)
    return;
  debugeprintf ("newinfodialog_reqfun reqlin: %s", reqlin);
  if (sscanf (reqlin, " pt: %d dia: %d", &infork, &dialrk) < 2
      || infork < 0 || dialrk < 0)
    return;
  debugeprintf ("newinfodialog_reqfun infork=%d dialrk=%d", infork, dialrk);
  if (infork >= (int) VEC_length (infopoint_ptr_t, infopoint_vector))
    return;
  ip = VEC_index (infopoint_ptr_t, infopoint_vector, infork);
  if (!ip)
    return;
  if ((int) VEC_length (infodisplay_ptr_t, infodisplay_vector) <= dialrk)
    VEC_safe_grow_cleared (infodisplay_ptr_t, heap, infodisplay_vector,
			   5 * dialrk / 4 + 16);
  idisp = (infodisplay_ptr_t) xcalloc (sizeof (*idisp), 1);
  idisp->idis_num = dialrk;
  idisp->idis_infp = ip;
  VEC_replace (infodisplay_ptr_t, infodisplay_vector, dialrk, idisp);
  /* sort the display choices in alphanumerical order */
  if (ip->infp_dischvec)
    nbdisp = VEC_length (displaychoice_ptr_t, ip->infp_dischvec);
  debugeprintf ("newinfodialog_reqfun nbdisp=%d", nbdisp);
  if (nbdisp > 0)
    qsort ((VEC_address (displaychoice_ptr_t, ip->infp_dischvec)),
	   (size_t) nbdisp, sizeof (displaychoice_ptr_t),
	   cmp_displaychoice_ptr);
  for (chix = 0; chix < nbdisp; chix++)
    {
      displaychoice_ptr_t dch =
	VEC_index (displaychoice_ptr_t, ip->infp_dischvec, chix);
      gcc_assert (dch && dch->di_fun);
      gcc_assert (dch->di_magic == DI_MAGIC);
      comprobe_printf ("PROB_dialogchoice dia:%d msg:", dialrk);
      comprobe_outenc_string (dch->di_msg);
      comprobe_printf (" ch:%d\n", chix);
    };
  comprobe_printf ("PROB_showdialog dia:%d\n", dialrk);
  comprobe_flush ();
}

static void bb_starting_displayer (struct comprobe_whatpos_st *wp,
				   struct comprobe_infodisplay_st *di,
				   HOST_WIDE_INT data, HOST_WIDE_INT navig);

static void tree_starting_displayer (struct comprobe_whatpos_st *wp,
				     struct comprobe_infodisplay_st *di,
				     HOST_WIDE_INT data, HOST_WIDE_INT navig);

static void gimple_starting_displayer (struct comprobe_whatpos_st *wp,
				   struct comprobe_infodisplay_st *di,
				   HOST_WIDE_INT data, HOST_WIDE_INT navig);

static void tree_ending_displayer (struct comprobe_whatpos_st *wp,
				   struct comprobe_infodisplay_st *di,
				   HOST_WIDE_INT data, HOST_WIDE_INT navig);

static void
display_tree (tree tr, struct comprobe_infodisplay_st *di)
{
  gcc_assert (di != 0);
  if (!tr)
    comprobe_printf ("*** NULL TREE %p ***\n", (void*)tr);
  else if (EXPR_P (tr))
    {
      comprobe_printf ("*** EXPR %p ***\n", (void*)tr);
      print_generic_expr (comprobe_replf, tr,
			  TDF_LINENO | TDF_VOPS | TDF_MEMSYMS);
    }
  else if (TREE_CODE (tr) == STATEMENT_LIST)
    {
      tree_stmt_iterator tsi;
      int rk = 0;
      comprobe_printf ("*** STATEMENT LIST %p ***\n", (void*)tr);
      print_generic_expr (comprobe_replf, tr,
			  TDF_LINENO | TDF_VOPS | TDF_MEMSYMS);
      for (tsi = tsi_start (tr); !tsi_end_p (tsi); tsi_next (&tsi))
	{
	  tree stmt = tsi_stmt (tsi);
	  rk++;
	  if (stmt)
	    {
	      static char titbuf[64];
	      memset (titbuf, 0, sizeof (titbuf));
	      snprintf (titbuf, sizeof (titbuf) - 1, "%d-th tree substmt", rk);
	      comprobe_display_add_navigator (di, tree_starting_displayer,
					      titbuf,
					      comprobe_unique_index_of_tree
					      (stmt));
	    }

	}
    }
  else
    comprobe_printf ("*** tree of code %d <%s>***\n",
		     TREE_CODE (tr), tree_code_names[TREE_CODE (tr)]);
}

static void
display_gimple_seq (gimple_seq sq, struct comprobe_infodisplay_st *di)
{
  gimple_stmt_iterator gsi;
  int rk = 0;
  for (gsi = gsi_start(sq); !gsi_end_p(gsi); gsi_next(&gsi)) 
    {
      gimple s = gsi_stmt(gsi);
      static char titbuf[64];
      memset (titbuf, 0, sizeof (titbuf));
      rk++;
      snprintf (titbuf, sizeof (titbuf) - 1, "%d-th gimple substmt", rk);
      comprobe_display_add_navigator (di, gimple_starting_displayer,
				      titbuf,
				      comprobe_unique_index_of_gimple(s));
    }
}

static void
display_gimple (gimple g, struct comprobe_infodisplay_st *di)
{
  gcc_assert (di != 0);
  if (!g) 
    {
      comprobe_printf ("*** NULL GIMPLE %p ***\n", (void*)g);
      return;
    }
  print_gimple_stmt (comprobe_replf, g, 1,
		     TDF_LINENO | TDF_VOPS | TDF_MEMSYMS);
  if (gimple_has_substatements(g)) 
    {
      switch (gimple_code (g))
	{
	case GIMPLE_BIND:
	  display_gimple_seq(gimple_bind_body(g), di);
	  break;
	case GIMPLE_CATCH:
	  display_gimple_seq(gimple_catch_handler(g), di);
	  break;
	case GIMPLE_EH_FILTER:
	    display_gimple_seq(gimple_eh_filter_failure(g), di);
	  break;
	case GIMPLE_TRY:
	  display_gimple_seq(gimple_try_eval(g), di);
	  display_gimple_seq(gimple_try_cleanup(g), di);
	  break;
	case GIMPLE_WITH_CLEANUP_EXPR:
	  display_gimple_seq(gimple_wce_cleanup(g), di);
	  break;
	case GIMPLE_OMP_FOR:
	case GIMPLE_OMP_MASTER:
	case GIMPLE_OMP_ORDERED:
	case GIMPLE_OMP_SECTION:
	case GIMPLE_OMP_PARALLEL:
	case GIMPLE_OMP_TASK:
	case GIMPLE_OMP_SECTIONS:
	case GIMPLE_OMP_SINGLE:
	  {
	    static bool warned;
	    if (!warned) {
	      warning(0, "compiler probe not implemented for OpenMP stuff");
	      warned = true;
	    }
	    return;
	  }
	  break;
	default:
	  gcc_unreachable();
	}
    }
}


static void
gimple_starting_displayer (struct comprobe_whatpos_st *wp,
			 struct comprobe_infodisplay_st *di,
			 HOST_WIDE_INT data,
			 HOST_WIDE_INT navig ATTRIBUTE_UNUSED)
{
  gimple g = 0;
  comprobe_ix_t ix = (comprobe_ix_t) data;
  unsigned nbgimple = VEC_length (gimple, unique_gimple_vector);
  if (ix > 0 && ix < (long) nbgimple)
    {
      g = VEC_index (gimple, unique_gimple_vector, ix);
      comprobe_printf
	("// starting gimple_%ld #%d shown when '%s' \n// from gcc file %s line %d\n",
	 ix, di->idis_infp->infp_num, wp->wp_what, basename (wp->wp_file),
	 wp->wp_line);
      display_gimple (g, di);
    }
  else
    comprobe_printf
      (" ?? invalid starting gimple index %ld nbgimple %d info #%d??", (long) ix,
       (int) nbgimple, di->idis_infp->infp_num);
}

static void
tree_starting_displayer (struct comprobe_whatpos_st *wp,
			 struct comprobe_infodisplay_st *di,
			 HOST_WIDE_INT data,
			 HOST_WIDE_INT navig ATTRIBUTE_UNUSED)
{
  tree tr = NULL_TREE;
  comprobe_ix_t ix = (comprobe_ix_t) data;
  unsigned nbtree = VEC_length (tree, unique_tree_vector);
  debugeprintf ("tree_starting_displayer ix %d nbtree %d", (int) ix,
		(int) nbtree);
  if (ix > 0 && ix < (long) nbtree)
    {
      tr = VEC_index (tree, unique_tree_vector, ix);
      comprobe_printf
	("// starting tree_%ld #%d shown when '%s' \n// from gcc file %s line %d\n",
	 ix, di->idis_infp->infp_num, wp->wp_what, basename (wp->wp_file),
	 wp->wp_line);
      display_tree (tr, di);
    }
  else
    comprobe_printf
      (" ?? invalid starting tree index %ld nbtree %d info #%d??", (long) ix,
       (int) nbtree, di->idis_infp->infp_num);
}

static void
tree_ending_displayer (struct comprobe_whatpos_st *wp,
		       struct comprobe_infodisplay_st *di,
		       HOST_WIDE_INT data,
		       HOST_WIDE_INT navig ATTRIBUTE_UNUSED)
{
  tree tr = NULL_TREE;
  comprobe_ix_t ix = (comprobe_ix_t) data;
  unsigned nbtree = VEC_length (tree, unique_tree_vector);
  debugeprintf ("tree_ending_displayer ix %d nbtree %d", (int) ix,
		(int) nbtree);
  if (ix > 0 && ix < (long) nbtree)
    {
      tr = VEC_index (tree, unique_tree_vector, ix);
      comprobe_printf
	("// ending tree_%ld #%d shown when '%s'\n// from gcc file %s line %d\n",
	 (long) ix, di->idis_infp->infp_num, wp->wp_what,
	 basename (wp->wp_file), wp->wp_line);
      display_tree (tr, di);
    }
  else
    comprobe_printf (" ?? invalid ending tree index %ld nbtree %d info #%d??",
		     (long) ix, (int) nbtree, di->idis_infp->infp_num);
}


static void
bb_starting_displayer (struct comprobe_whatpos_st *wp,
		       struct comprobe_infodisplay_st *di,
		       HOST_WIDE_INT data,
		       HOST_WIDE_INT navig ATTRIBUTE_UNUSED)
{
  comprobe_ix_t ix = (comprobe_ix_t) data;
  char pfx[24];
  basic_block bb = NULL;
  gimple_seq phis = NULL;
  debugeprintf ("bb_starting_displayer ix %d", (int) ix);
  comprobe_bb_ok_rtl = 1;
  if (ix > 0 && ix < VEC_length (basic_block, unique_bb_vector))
    {
      bb = VEC_index (basic_block, unique_bb_vector, ix);
      debugeprintf ("bb_starting_displayer bb %p", (void*) bb);
      if (flag_compiler_probe_debug) 
	gimple_debug_bb(bb);
      comprobe_printf
	("// starting basic block _%ld #%d shown when '%s' \n// from gcc file %s line %d\n",
	 (long) ix, di->idis_infp->infp_num, wp->wp_what,
	 basename (wp->wp_file), wp->wp_line);
      snprintf (pfx, sizeof (pfx), "[+bb#%d] ", di->idis_infp->infp_num);
      comprobe_printf ("\n// basic block info _%ld #%d is\n",
		       (long) ix, di->idis_infp->infp_num);
      dump_bb_info (bb, true, true,
		    TDF_DETAILS | TDF_LINENO | TDF_VOPS | TDF_MEMSYMS,
		    pfx, comprobe_replf);
      phis = phi_nodes(bb);
      debugeprintf ("bb_starting_displayer phis %p", (void*) phis);
      if (phis)
	{
	  gimple_stmt_iterator gsi = {0};
	  comprobe_printf ("\n// basic block phi_nodes _%ld #%d is\n",
			   ix, di->idis_infp->infp_num);
	  for (gsi = gsi_start (phis); !gsi_end_p (gsi); gsi_next (&gsi)) {
	    gimple g = gsi_stmt(gsi);
	    debugeprintf ("bb_starting_displayer gimple g %p", (void*) g);
	    display_gimple(g, di);
	    comprobe_display_add_navigator
	      (di, tree_starting_displayer,
	       "phi nodes", comprobe_unique_index_of_gimple(g));
	  }
	};
      debugeprintf ("bb_starting_displayer again bb %p", (void*) bb);
      /* basic blocks [almost?] always have statements */
      {
	gimple_stmt_iterator gsi;
	comprobe_printf ("\n// basic block statements _%ld #%d is\n", ix,
			 di->idis_infp->infp_num);
	for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) 
	  {
	    gimple g = gsi_stmt(gsi);
	    display_gimple(g, di);
	    comprobe_display_add_navigator
	      (di, tree_starting_displayer,
	       "stmt", comprobe_unique_index_of_gimple(g));
	  }
      }
    }
  else
    comprobe_printf ("?? invalid starting basic block index %ld info #%d??",
		     ix, di->idis_infp->infp_num);
  comprobe_bb_ok_rtl = 0;
}

static void
bb_ending_displayer (struct comprobe_whatpos_st *wp,
		     struct comprobe_infodisplay_st *di,
		     HOST_WIDE_INT data, HOST_WIDE_INT navig ATTRIBUTE_UNUSED)
{
  comprobe_ix_t ix = (comprobe_ix_t) data;
  char pfx[24];
  basic_block bb = NULL;
  int infoptrk = di->idis_infp->infp_num;
  debugeprintf ("bb_ending_displayer ix %d", (int) ix);  
  comprobe_bb_ok_rtl = 1;
  if (ix > 0 && ix < VEC_length (basic_block, unique_bb_vector))
    {
      bb = VEC_index (basic_block, unique_bb_vector, ix);
      debugeprintf ("bb_ending_displayer bb %p", (void*) bb);  
      comprobe_printf
	("// ending basic block _%ld #%d shown when '%s'\n// from gcc file %s line %d\n",
	 ix, infoptrk, wp->wp_what, basename (wp->wp_file), wp->wp_line);
      snprintf (pfx, sizeof (pfx), "[-bb#%d] ", infoptrk);
      dump_bb_info (bb, true, true,
		    TDF_DETAILS | TDF_LINENO | TDF_VOPS | TDF_MEMSYMS,
		    pfx, comprobe_replf);
    }
  else
    comprobe_printf ("?? invalid ending basic block index %ld info #%d??", ix,
		     infoptrk);  
  comprobe_bb_ok_rtl = 0;
}

/* clear the navigation vector inside a display */
static void
infodialog_clear_navig (infodisplay_ptr_t disp)
{
  int navix = 0;
  displaychoice_ptr_t navch = NULL;
  debugeprintf ("infodialog_clear_navig disp %p num %d", (void*)disp,
		disp->idis_num);
  if (disp->idis_navig)
    {
      for (navix = 0;
	   VEC_iterate (displaychoice_ptr_t, disp->idis_navig, navix,
			navch); navix++)
	{
	  if (!navch)
	    continue;
	  gcc_assert (navch->di_magic == DI_MAGIC);
	  VEC_replace (displaychoice_ptr_t, disp->idis_navig, navix, NULL);
	  memset (navch, 0, sizeof (navch));
	  free (navch);
	}
      VEC_free (displaychoice_ptr_t, heap, disp->idis_navig);
      disp->idis_navig = NULL;
    }
}


static void
fill_infodialog (struct comprobe_whatpos_st *wp, infodisplay_ptr_t disp,
		 displaychoice_ptr_t ch, comprobe_ix_t chix)
{
  debugeprintf("fill_infodialog ch %p chix %d", (void*)ch, (int)chix);
  gcc_assert (ch && ch->di_magic == DI_MAGIC);
  infodialog_clear_navig (disp);
  if (ch->di_fun)
    {
      comprobe_begin_big_printf ("PROB_dialogcontent dia:%d\n",
				 disp->idis_num);
      (*ch->di_fun) (wp, disp, ch->di_data, chix);
      comprobe_end_big ();
      if (disp->idis_navig
	  && VEC_length (displaychoice_ptr_t, disp->idis_navig) > 0)
	{
	  int navix = 0;
	  displaychoice_ptr_t navch = NULL;
	  for (navix = 0;
	       VEC_iterate (displaychoice_ptr_t, disp->idis_navig, navix,
			    navch); navix++)
	    {
	      gcc_assert (navch->di_magic == DI_MAGIC);
	      debugeprintf ("fill_infodialog %d navix %d navch msg '%s'",
			    disp->idis_num, navix, navch->di_msg);
	      comprobe_printf ("PROB_dialognavig dia:%d msg:",
			       disp->idis_num);
	      comprobe_outenc_string (navch->di_msg);
	      comprobe_printf (" nav:%d\n", navix);
	    }
	}
      comprobe_printf ("PROB_showdialog dia:%d\n", disp->idis_num);
      comprobe_flush ();
    }
  else debugeprintf("fill_infodialog no function in ch %p", (void*)ch);
}


/* the SHOWINFODIALOG request from probe ask for the dialog to be shown */
static void
showinfodialog_reqfun (struct comprobe_whatpos_st *wp,
		       char *reqlin, void *data ATTRIBUTE_UNUSED)
{
  int dialrk = -1, chrk = -1;
  infodisplay_ptr_t disp = NULL;
  infopoint_ptr_t ip = NULL;
  displaychoice_ptr_t ch = NULL;
  if (!comprobe_replf)
    return;
  debugeprintf ("showinfodialog_reqfun reqlin: %s", reqlin);
  if (sscanf (reqlin, " dia: %d ch: %d", &dialrk, &chrk) < 2
      || chrk < 0 || dialrk < 0)
    return;
  if (!infodisplay_vector
      || dialrk >= (int) VEC_length (infodisplay_ptr_t, infodisplay_vector))
    return;
  disp = VEC_index (infodisplay_ptr_t, infodisplay_vector, dialrk);
  if (!disp)
    return;
  gcc_assert (disp->idis_num == dialrk);
  ip = disp->idis_infp;
  gcc_assert (ip);
  if (chrk >= (int) VEC_length (displaychoice_ptr_t, ip->infp_dischvec))
    return;
  ch = VEC_index (displaychoice_ptr_t, ip->infp_dischvec, chrk);
  if (!ch)
    return;
  gcc_assert (ch->di_magic == DI_MAGIC);
  disp->idis_choice = chrk;
  debugeprintf ("showinfodialog_reqfun ch %p str= %s", (void*)ch, ch->di_msg);
  fill_infodialog (wp, disp, ch, -1);
  debugeprintf ("showinfodialog_reqfun end reqlin: %s", reqlin);
}


static void
updateinfodialog_reqfun (struct comprobe_whatpos_st *wp,
			 char *reqlin, void *data ATTRIBUTE_UNUSED)
{
  int dialrk = -1;
  infodisplay_ptr_t disp = NULL;
  infopoint_ptr_t ip = NULL;
  displaychoice_ptr_t ch = NULL;
  if (!comprobe_replf)
    return;
  debugeprintf ("updateinfodialog_reqfun reqlin: %s", reqlin);
  if (sscanf (reqlin, " dia: %d", &dialrk) <= 0 || dialrk < 0)
    return;
  if (!infodisplay_vector
      || dialrk >= (int) VEC_length (infodisplay_ptr_t, infodisplay_vector))
    return;
  disp = VEC_index (infodisplay_ptr_t, infodisplay_vector, dialrk);
  if (!disp)
    return;
  gcc_assert (disp->idis_num == dialrk);
  ip = disp->idis_infp;
  if (disp->idis_choice >= 0
      && disp->idis_choice <
      (int) VEC_length (displaychoice_ptr_t, ip->infp_dischvec))
    ch =
      VEC_index (displaychoice_ptr_t, ip->infp_dischvec, disp->idis_choice);
  if (!ch) {
    debugeprintf("updateinfodialog_reqfun no ch %p", (void*)ch);
    return;
  };
  gcc_assert (ch->di_magic == DI_MAGIC);
  debugeprintf ("updateinfodialog_reqfun ch %p str= %s", (void*)ch, ch->di_msg);
  fill_infodialog (wp, disp, ch, -1);
  debugeprintf ("updateinfodialog_reqfun end reqlin: %s", reqlin);
}

static void
naviginfodialog_reqfun (struct comprobe_whatpos_st *wp,
			char *reqlin, void *data ATTRIBUTE_UNUSED)
{
  int dialrk = -1, navrk = -1;
  infodisplay_ptr_t disp = NULL;
  infopoint_ptr_t ip = NULL;
  displaychoice_ptr_t ch = NULL;
  if (!comprobe_replf)
    return;
  debugeprintf ("naviginfodialog_reqfun reqlin: %s", reqlin);
  if (sscanf (reqlin, " dia: %d nav: %d", &dialrk, &navrk) <= 0
      || dialrk < 0 || navrk < 0)
    return;
  if (!infodisplay_vector
      || dialrk >= (int) VEC_length (infodisplay_ptr_t, infodisplay_vector))
    return;
  disp = VEC_index (infodisplay_ptr_t, infodisplay_vector, dialrk);
  debugeprintf ("naviginfodialog_reqfun disp %p", (void*)disp);
  if (!disp)
    return;
  gcc_assert (disp->idis_num == dialrk);
  ip = disp->idis_infp;
  if (navrk < (int) VEC_length (displaychoice_ptr_t, disp->idis_navig))
    ch = VEC_index (displaychoice_ptr_t, disp->idis_navig, navrk);
  if (!ch)
    return;
  debugeprintf ("naviginfodialog_reqfun ch %p navrk %d", (void*)ch, navrk);
  gcc_assert (ch->di_magic == DI_MAGIC);
  debugeprintf ("naviginfodialog_reqfun ch %p str= %s", (void*)ch, ch->di_msg);
  fill_infodialog (wp, disp, ch, navrk);
  debugeprintf ("naviginfodialog_reqfun end reqlin: %s\n", reqlin);
}

static void
removeinfodialog_reqfun (struct comprobe_whatpos_st *wp ATTRIBUTE_UNUSED,
			 char *reqlin, void *data ATTRIBUTE_UNUSED)
{
  int dialrk = -1;
  infodisplay_ptr_t disp = NULL;
  if (!comprobe_replf)
    return;
  debugeprintf ("removeinfodialog_reqfun reqlin: %s", reqlin);
  if (sscanf (reqlin, " dia: %d", &dialrk) <= 0 || dialrk < 0)
    return;
  if (!infodisplay_vector
      || dialrk >= (int) VEC_length (infodisplay_ptr_t, infodisplay_vector))
    return;
  disp = VEC_index (infodisplay_ptr_t, infodisplay_vector, dialrk);
  if (!disp)
    return;
  gcc_assert (disp->idis_num == dialrk);
  infodialog_clear_navig (disp);
  free (disp);
  VEC_replace (infodisplay_ptr_t, infodisplay_vector, dialrk, 0);
  comprobe_printf ("PROB_destroydialog dia:%d\n", dialrk);
  comprobe_flush ();
  debugeprintf ("removeinfodialog_reqfun end reqlin: %s", reqlin);
}

/****
 * send a message to be shown
 ****/
void
comprobe_show_message (const char *msg)
{
  if (!msg || !comprobe_replf)
    return;
  comprobe_puts ("PROB_message msg:");
  comprobe_outenc_string (msg);
  comprobe_puts ("\n");
  comprobe_flush ();
}


/****
 * Initialize the probe. Called from toplev.c
 ****/
void
comprobe_initialize (void)
{
  static int inited;
  long seed = 0;
  const char *pc;
  const char* randomseed = get_random_seed(false);
  gcc_assert (!inited);
  gcc_assert (randomseed != (char *) 0);
  inited = 1;
  if (!compiler_probe_string || !compiler_probe_string[0])
    return;
  for (pc = randomseed; *pc; pc++)
    seed ^= (seed << 5) + (*pc);
  srand48_r ((long) seed, &randata);
  signal (SIGIO, sig_interrupted);
  signal (SIGCHLD, sig_interrupted);
  proberequest_htable =
    htab_create (113, hash_proberequest, eq_proberequest, del_proberequest);
  filename_htable =
    htab_create (229, hash_filename, eq_filename, del_filename);
  infopoint_htable = htab_create (2081, hash_infopoint, eq_infopoint, NULL);
  infopoint_vector = VEC_alloc (infopoint_ptr_t, heap, 2048);
  unique_tree_vector = VEC_alloc (tree, gc, 1024);
  unique_bb_vector = VEC_alloc (basic_block, gc, 512);
  /* reserve but don't use the first 4 entries hence HTAB_SEEKED_ENTRY unused */
  VEC_safe_push (tree, gc, unique_tree_vector, (tree) 0);
  VEC_safe_push (tree, gc, unique_tree_vector, (tree) 0);
  VEC_safe_push (tree, gc, unique_tree_vector, (tree) 0);
  VEC_safe_push (tree, gc, unique_tree_vector, (tree) 0);
  VEC_safe_push (basic_block, gc, unique_bb_vector, (basic_block) 0);
  VEC_safe_push (basic_block, gc, unique_bb_vector, (basic_block) 0);
  VEC_safe_push (basic_block, gc, unique_bb_vector, (basic_block) 0);
  VEC_safe_push (basic_block, gc, unique_bb_vector, (basic_block) 0);
  VEC_safe_push (gimple, gc, unique_gimple_vector, (gimple) 0);
  VEC_safe_push (gimple, gc, unique_gimple_vector, (gimple) 0);
  VEC_safe_push (gimple, gc, unique_gimple_vector, (gimple) 0);
  VEC_safe_push (gimple, gc, unique_gimple_vector, (gimple) 0);
  unique_tree_htable = htab_create (4007, hash_info_tree, eq_info_tree, NULL);
  unique_bb_htable = htab_create (3001, hash_info_bb, eq_info_bb, NULL);
  unique_gimple_htable = htab_create (6173, hash_info_gimple, eq_info_gimple, NULL);
  files_varr.tab = XNEWVEC (char *, 100);
  files_varr.size = 100;
  files_varr.last = 0;
  memset (files_varr.tab, 0, sizeof (char **) * files_varr.size);
  comprobe_register ("prob_NAVIGINFODIALOG", naviginfodialog_reqfun, (void *) 0);
  comprobe_register ("prob_NEWINFODIALOG", newinfodialog_reqfun, (void *) 0);
  comprobe_register ("prob_REMOVEINFODIALOG", removeinfodialog_reqfun, (void *) 0);
  comprobe_register ("prob_SHOWINFODIALOG", showinfodialog_reqfun, (void *) 0);
  comprobe_register ("prob_STOP", stop_reqfun, (void *) 0);
  comprobe_register ("prob_UPDATEINFODIALOG", updateinfodialog_reqfun, (void *) 0);
  create_probe_process ();
  comprobe_printf ("PROB_version proto:%d msg:", COMPROBE_PROTOCOL_NUMBER);
  comprobe_outenc_string (version_string);
  comprobe_puts ("\n");
  comprobe_flush ();
  atexit (comprobe_forced_kill);
}

/****
 * Finish the probe. Called from toplev.c
 ****/
void
comprobe_finish (void)
{
  nonstopped = 1;
  comprobe_puts ("PROB_message msg:");
  comprobe_outenc_string ("probe finished");
  comprobe_puts ("\n");
  comprobe_flush ();
  comprobe_while ("finishing", &nonstopped);
  comprobe_stop ();
  if (proberequest_htable)
    {
      htab_empty (proberequest_htable);
      htab_delete (proberequest_htable);
      proberequest_htable = 0;
    };
  if (filename_htable)
    {
      htab_empty (filename_htable);
      htab_delete (filename_htable);
      filename_htable = 0;
    };
}



int
comprobe_file_rank (const char *filename)
{
  struct filenamehentry_st slot;
  struct filenamehentry_st **slotptr = 0;
  int filerank = 0;

  if (!filename || !comprobe_replf)
    return 0;
  memset (&slot, 0, sizeof (slot));
  slot.file = (char *) filename;
  slotptr = (struct filenamehentry_st **)
    htab_find_slot (filename_htable, &slot, INSERT);
  if (!slotptr)
    fatal_error
      ("compiler probe failed to register file %s (memory full): %m",
       filename);
  if (*slotptr == HTAB_EMPTY_ENTRY || (*slotptr) == HTAB_DELETED_ENTRY)
    {
      struct filenamehentry_st *newslot = 0;
      const char *dupfilename = xstrdup (filename);
      newslot = (struct filenamehentry_st *) xcalloc (sizeof (*newslot), 1);
      if (files_varr.last + 1 >= files_varr.size)
	{
	  int newsiz = ((5 * files_varr.last) / 4 + 50) | 0x1f;
	  int ix;
	  char **newarr = XNEWVEC (char *, newsiz);
	  newarr[0] = 0;
	  for (ix = files_varr.last; ix > 0; ix--)
	    newarr[ix] = files_varr.tab[ix];
	  for (ix = files_varr.last + 1; ix < newsiz; ix++)
	    newarr[ix] = (char *) 0;
	  memset (files_varr.tab, 0, sizeof (char *) * files_varr.size);
	  XDELETEVEC (files_varr.tab);
	  files_varr.tab = newarr;
	}
      /* dont use index 0 */
      filerank = ++files_varr.last;
      files_varr.tab[filerank] = (char *) dupfilename;
      gcc_assert (filerank > 0);
      newslot->file = (char *) dupfilename;
      newslot->rank = filerank;
      *slotptr = newslot;
      debugeprintf ("new file rank filerank%d file %s newslot %p", filerank,
		    dupfilename, (void*)newslot);
      comprobe_printf ("PROB_file rank:%d fpath:", filerank);
      comprobe_outenc_string(dupfilename);
      comprobe_puts("\n");
      comprobe_flush();
    }
  else
    {
      struct filenamehentry_st *oldslot = *slotptr;
      filerank = oldslot->rank;
      gcc_assert (!strcmp (files_varr.tab[filerank], filename));
      debugeprintf ("old file rank filerank%d file %s oldslot %p", filerank,
		    filename, (void*)oldslot);
    }
  return filerank;
}


int
comprobe_infopoint_rank (int filerank, int lineno)
{
  int inforank = 0;
  struct infopointhentry_st slot;
  struct infopointhentry_st **slotptr = 0;
  gcc_assert (filerank > 0 && filerank <= files_varr.last);
  gcc_assert (lineno > 0);
  gcc_assert (infopoint_vector != NULL);
  memset (&slot, 0, sizeof (slot));
  slot.infp_filerank = filerank;
  slot.infp_lineno = lineno;
  slotptr = (struct infopointhentry_st **)
    htab_find_slot (infopoint_htable, &slot, INSERT);
  if (!slotptr)
    fatal_error
      ("compiler probe failed to register infopoint filerank=%d,lineno=%d (memory full): %m",
       filerank, lineno);
  if (*slotptr == HTAB_EMPTY_ENTRY || (*slotptr) == HTAB_DELETED_ENTRY)
    {
      struct infopointhentry_st *newslot = 0;
      newslot = ( struct infopointhentry_st *) xcalloc (sizeof (*newslot), 1);
      /* dont use index 0 */
      if (VEC_length (infopoint_ptr_t, infopoint_vector) == 0)
	VEC_safe_push (infopoint_ptr_t, heap, infopoint_vector,
		       (struct infopointhentry_st *) 0);
      VEC_safe_push (infopoint_ptr_t, heap, infopoint_vector, newslot);
      inforank = VEC_length (infopoint_ptr_t, infopoint_vector) - 1;
      comprobe_printf ("PROB_infopoint fil:%d lin:%d rk:%d\n", filerank,
		       lineno, inforank);
      comprobe_flush ();
      newslot->infp_filerank = filerank;
      newslot->infp_lineno = lineno;
      newslot->infp_num = inforank;
      newslot->infp_dischvec = VEC_alloc (displaychoice_ptr_t, heap, 3);
      debugeprintf
	("new infopoint slot filerank%d lineno%d inforank%d slot%p", filerank,
	 lineno, inforank, (void*) newslot);
      *slotptr = newslot;
    }
  else
    {
      struct infopointhentry_st *oldslot = (*slotptr);
      gcc_assert (oldslot->infp_filerank == filerank
		  && oldslot->infp_lineno == lineno);
      inforank = oldslot->infp_num;
      gcc_assert (inforank >= 0
		  && inforank < (int) VEC_length (infopoint_ptr_t,
						  infopoint_vector));
      gcc_assert (VEC_index (infopoint_ptr_t, infopoint_vector, inforank)
		  == oldslot);
      debugeprintf
	("old infopoint slot filerank%d lineno%d inforank%d oldslot%p",
	 filerank, lineno, inforank, (void*)oldslot);
    }
  return inforank;
}



/** convenience function for iterating **/
static bool 
get_gimple_position_seq(gimple_seq sq, char **pfilename, int *plineno, int end)
{
  gimple_stmt_iterator gsi;
  if (!end) 
    {
      for (gsi = gsi_start(sq); !gsi_end_p(gsi); gsi_next(&gsi)) 
	{
	  gimple s = gsi_stmt(gsi);
	  if (comprobe_get_gimple_position(s, pfilename, plineno, POS_START))
	    return true;
	}
    }
  else 
    {
      for (gsi_last(sq); !gsi_end_p(gsi); gsi_prev(&gsi)) {
	gimple s = gsi_stmt(gsi);
	if (comprobe_get_gimple_position(s, pfilename, plineno, POS_END))
	  return true;
      }
    }
  return false;
}

/***
 * return true if a gimple G has a position 
 * and in that case fill the PFILENAME and PLINENO
 * if the END flag is set, return the last position
 ***/
bool
comprobe_get_gimple_position (gimple g, char **pfilename, int *plineno, int end)
{
  location_t loc = 0;
  if (!g) 
    return false;
  loc = gimple_location(g);
  if (loc != UNKNOWN_LOCATION && loc != BUILTINS_LOCATION) 
    {
      if (pfilename) 
	*pfilename = LOCATION_FILE(loc);
      if (plineno)
	*plineno = LOCATION_LINE(loc);
      return true;
    }
  else if (gimple_has_substatements(g)) 
    {
      switch (gimple_code (g))
	{
	case GIMPLE_BIND:
	  {
	    gimple_seq sq = gimple_bind_body(g);
	    return get_gimple_position_seq(sq, pfilename, plineno, end);
	  }
	case GIMPLE_CATCH:
	  {
	    gimple_seq sq = gimple_catch_handler(g);
	    return get_gimple_position_seq(sq, pfilename, plineno, end);
	  }
	case GIMPLE_EH_FILTER:
	  {
	    gimple_seq sq = gimple_eh_filter_failure(g);
	    return get_gimple_position_seq(sq, pfilename, plineno, end);
	  }
	case GIMPLE_TRY:
	  {
	    gimple_seq sq;
	    sq = gimple_try_eval(g);
	    if (get_gimple_position_seq(sq, pfilename, plineno, end))
	      return true;
	    sq = gimple_try_cleanup(g);
	    return get_gimple_position_seq(sq, pfilename, plineno, end);
	  }
	case GIMPLE_WITH_CLEANUP_EXPR:
	  {
	    gimple_seq sq = gimple_wce_cleanup(g);
	    return get_gimple_position_seq(sq, pfilename, plineno, end);
	  }
	case GIMPLE_OMP_FOR:
	case GIMPLE_OMP_MASTER:
	case GIMPLE_OMP_ORDERED:
	case GIMPLE_OMP_SECTION:
	case GIMPLE_OMP_PARALLEL:
	case GIMPLE_OMP_TASK:
	case GIMPLE_OMP_SECTIONS:
	case GIMPLE_OMP_SINGLE:
	  {
	    static bool warned;
	    if (!warned) {
	      warning(0, "compiler probe not implemented for OpenMP stuff");
	      warned = true;
	    }
	    return false;
	  }
	  break;
	default:
	  gcc_unreachable();
	}
    }
    return false;
}

bool
comprobe_get_tree_position (tree t, char **pfilename, int *plineno, int end)
{
  location_t loc = 0;
  if (!t) 
    return false;
  loc = EXPR_LOCATION(t);
  if (loc != UNKNOWN_LOCATION && loc != BUILTINS_LOCATION) 
    {
      if (pfilename) 
	*pfilename = LOCATION_FILE(loc);
      if (plineno)
	*plineno = LOCATION_LINE(loc);
      return true;
    }
  return false;
}

int
comprobe_file_rank_of_gimple (gimple g, int *plineno)
{
  char *filename = 0;
  int lineno = 0, filerank = 0;
  if (!g)
    return 0;
  if (!comprobe_get_gimple_position (g, &filename, &lineno, POS_START))
    return 0;
  filerank = comprobe_file_rank (filename);
  if (filerank > 0 && plineno)
    *plineno = lineno;
  return filerank;
}

int
comprobe_file_rank_of_tree (tree tr, int *plineno)
{
  char *filename = 0;
  int lineno = 0, filerank = 0;
  if (!tr)
    return 0;
  if (!comprobe_get_tree_position (tr, &filename, &lineno, POS_START))
    return 0;
  filerank = comprobe_file_rank (filename);
  if (filerank > 0 && plineno)
    *plineno = lineno;
  return filerank;
}


/* run the compiler probe pass only if we have a real probe */
static bool
gate_comprobe (void)
{
  return comprobe_replf != (FILE *) 0 && comprobe_pid != (pid_t) 0;
}


/* add information point and display start of a given fimple G with
   string DMESG - return the infopoint rank  */
static int
added_infopoint_display_tree (gimple g, const char *dmesg)
{
  int frk = 0, lin = 0, infrk = 0;
  comprobe_ix_t trix = 0;
  if (!g)
    return 0;
  frk = comprobe_file_rank_of_gimple (g, &lin);
  if (frk > 0 && lin > 0)
    {
      trix = comprobe_unique_index_of_gimple (g);
      gcc_assert (trix > 2);
      infrk = comprobe_infopoint_rank (frk, lin);
      comprobe_infopoint_add_display (infrk, tree_starting_displayer, dmesg,
				      (HOST_WIDE_INT) trix);
      return infrk;
    };
  return 0;
}


#if 0 && old
/* add information point for a given function body */
static void
add_infopoint_funbody (tree tr_body)
{
  int frk_body = 0, lin_body = 0;
  debugeprintf ("add_infopoint_funbody tr_body %p start", (void*)tr_body);
  frk_body = comprobe_file_rank_of_tree (tr_body, &lin_body);
  if (frk_body >= 0 && lin_body > 0)
    {
      int esnumins = 0;
      char *endfile = 0;
      /* @@@ should probably dynamically allocate the message buffer */
      static char msgbuf[200];
      int endline = -1, endfrk = -1, infendnum = -1;
      comprobe_ix_t trix = 0;
      int infstartnum = -1;
      infstartnum = comprobe_infopoint_rank (frk_body, lin_body);
      if (comprobe_get_position (tr_body, &endfile, &endline, POS_END)
	  && (endfrk = comprobe_file_rank (endfile)) >= 0)
	infendnum = comprobe_infopoint_rank (endfrk, endline);
      trix = comprobe_unique_index_of_tree (tr_body);
      esnumins = estimate_num_insns (tr_body, &eni_inlining_weights);
      debugeprintf
	("add_infopoint_funbody tr%p infstartnum%d lin_body%d infendnum%d endline%d",
	 (void*)tr_body, infstartnum, lin_body, infendnum, endline);
      if (infstartnum > 0 && lin_body > 0)
	{
	  memset (msgbuf, 0, sizeof (msgbuf));
	  if (frk_body == endfrk && endline > lin_body)
	    snprintf (msgbuf, sizeof (msgbuf) - 1,
		      _("starting body of %d lines & %d instrs"),
		      endline - lin_body, esnumins);
	  else
	    snprintf (msgbuf, sizeof (msgbuf) - 1,
		      _("starting body of %d instrs"), esnumins);
	  if (added_infopoint_display_tree (tr_body, msgbuf) != infstartnum)
	    gcc_unreachable ();
	}
      if (infendnum > 0 && endline > 0)
	{
	  memset (msgbuf, 0, sizeof (msgbuf));
	  if (frk_body == endfrk && endline > lin_body && lin_body > 0)
	    snprintf (msgbuf,
		      sizeof (msgbuf) - 1,
		      _("ending body of %d lines & %d instrs"),
		      endline - lin_body, esnumins);
	  else
	    snprintf (msgbuf,
		      sizeof (msgbuf) - 1,
		      _("ending body of %d instrs"), esnumins);
	  comprobe_infopoint_add_display
	    (infendnum, tree_ending_displayer, msgbuf, (HOST_WIDE_INT) trix);
	}
    }
  debugeprintf ("add_infopoint_funbody tr_body %p end", (void*)tr_body);
}
#endif /*old code*/


comprobe_ix_t
comprobe_unique_index_of_basic_block (basic_block bb)
{
  comprobe_ix_t bbix = 0;
  comprobe_ix_t l = 0, nbbb = 0;
  void **sp = 0;
  if (bb == NULL || !unique_bb_vector)
    return 0;
  gcc_assert (unique_bb_vector
	      && VEC_length (basic_block, unique_bb_vector) > 2);
  l = (comprobe_ix_t) HTAB_SEEKED_ENTRY;
  nbbb = VEC_length (basic_block, unique_bb_vector);
  unique_seeked_bb = bb;
  sp = htab_find_slot (unique_bb_htable, &l, INSERT);
  if (sp)
    {
      if (*sp != HTAB_EMPTY_ENTRY && *sp != HTAB_DELETED_ENTRY
	  && *sp != HTAB_SEEKED_ENTRY)
	l = *(comprobe_ix_t *) (*sp);
      else
	l = 0;
      if (l > 2)
	{
	  gcc_assert (l < nbbb
		      && VEC_index (basic_block, unique_bb_vector, l) == bb);
	  bbix = l;
	}
      else
	{
	  VEC_safe_push (basic_block, gc, unique_bb_vector, bb);
	  bbix = nbbb;
	  *(comprobe_ix_t *) (sp) = bbix;
	}
    }
  else				/* failed to insert into unique_bb_htable */
    gcc_unreachable ();
  return bbix;
}


basic_block
comprobe_basic_block_of_unique_index (comprobe_ix_t ix)
{
  int nbbb = 0;
  if (ix < 2 || !unique_bb_vector)
    return NULL;
  nbbb = VEC_length (basic_block, unique_bb_vector);
  if ((long) ix < (long) nbbb)
    return VEC_index (basic_block, unique_bb_vector, ix);
  return NULL;
}

/* add information point for a given basic block */
static void
add_infopoint_basic_block (basic_block bb)
{
  gimple_stmt_iterator gsi;
  int stmtcnt = 0;
  comprobe_ix_t bbix = 0;
  bool bbgotpos = 0;
  debugeprintf ("add_infopoint_basic_block bb %p #%d start", (void*)bb, bb?bb->index:-1);
  if (bb == NULL)
    return;
  if (flag_compiler_probe_debug) 
    dump_bb(bb, stderr, 0);
  bbix = comprobe_unique_index_of_basic_block (bb);
  gcc_assert (bbix > 2);
  bbgotpos = FALSE;
  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);
      char *filename = 0;
      int lineno = 0;
      int filrk = 0, infrk = 0;
      static char msgbuf[64];
      if (stmt)
	stmtcnt++;
      else
	continue;
      if (comprobe_get_gimple_position (stmt, &filename, &lineno, POS_START))
	{
	  filrk = comprobe_file_rank (filename);
	  infrk = comprobe_infopoint_rank (filrk, lineno);
	  if (!bbgotpos)
	    {
	      bbgotpos = TRUE;
	      memset (msgbuf, 0, sizeof (msgbuf));
	      snprintf (msgbuf,
			sizeof (msgbuf) - 1, "start bb#%d", bb->index);
	      comprobe_infopoint_add_display
		(infrk, bb_starting_displayer, msgbuf, (HOST_WIDE_INT) bbix);
	    };
	  memset (msgbuf, 0, sizeof (msgbuf));
	  snprintf (msgbuf,
		    sizeof (msgbuf) - 1, "stmt#%d bb#%d", stmtcnt, bb->index);
	  (void) added_infopoint_display_tree (stmt, msgbuf);
	}
    }
  for (gsi = gsi_last_bb (bb); !gsi_end_p (gsi); gsi_prev (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);
      char *filename = 0;
      int lineno = 0;
      int filrk = 0, infrk = 0;
      static char msgbuf[64];
      if (comprobe_get_gimple_position (stmt, &filename, &lineno, POS_END))
	{
	  filrk = comprobe_file_rank (filename);
	  infrk = comprobe_infopoint_rank (filrk, lineno);
	  memset (msgbuf, 0, sizeof (msgbuf));
	  snprintf (msgbuf, sizeof (msgbuf) - 1, "end bb#%d", bb->index);
	  comprobe_infopoint_add_display
	    (infrk, bb_ending_displayer, msgbuf, (HOST_WIDE_INT) bbix);
	  break;
	}
    };
  debugeprintf ("add_infopoint_basic_block bb %p end", (void*)bb);
}


/* this function does the bulk of the work of the pass and returns
   additional TODOs to the pass machinery */
static unsigned int
execute_comprobe (void)
{
  struct cgraph_node *cgr_fun = 0;
  basic_block bb = 0;
  for (cgr_fun = cgraph_nodes; cgr_fun; cgr_fun = cgr_fun->next)
    {
      tree tr_decl = 0;
      int frk_decl = 0;
      int lin_decl = 0;
      gimple_seq sq = 0;
      if (!comprobe_replf)
	break;
      debugeprintf ("execute_comprobe cgr_fun=%p", (void*)cgr_fun);
      if (flag_compiler_probe_debug)
	dump_cgraph_node (stderr, cgr_fun);
      tr_decl = cgr_fun->decl;
      if (TREE_CODE(tr_decl) != FUNCTION_DECL) 
	continue;
      frk_decl = comprobe_file_rank_of_tree (tr_decl, &lin_decl);
      sq = gimple_body(tr_decl);
      if (!sq) 
	continue;
      sq = gimple_body(tr_decl);
#warning should do something with the body...
#if 0 && oldcode
      tr_body = DECL_SAVED_TREE (tr_decl);
      if (!tr_body)
	continue;
      comprobe_check ("comprobe cgraph loop");
      add_infopoint_funbody (tr_body);
#endif
      comprobe_flush ();
    }
  FOR_EACH_BB (bb)
  {
    if (!comprobe_replf)
      break;
    debugeprintf ("execute_comprobe bb %p", (void*)bb);
    comprobe_check ("comprobe bb loop");
    add_infopoint_basic_block (bb);
    comprobe_flush ();
  }
  comprobe_flush ();
  return 0;			/* no additional todos */
}

struct gimple_opt_pass pass_compiler_probe = 
{
  {
    GIMPLE_PASS,			/* type */
    "comprobe",			/* name */
    gate_comprobe,		/* gate */
    execute_comprobe,		/* execute */
    NULL,				/* sub */
    NULL,				/* next */
    0,				/* static_pass_number */
    0,				/* tv_id */
    PROP_cfg,			/* properties_required */
    0,				/* properties_provided */
    0,				/* properties_destroyed */
    0,				/* todo_flags_start */
    0,				/* todo_flags_finish */
  }
};

#include "gt-compiler-probe.h"
/* eof compiler-probe.c */
