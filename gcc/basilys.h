/* Basile's static analysis (should have a better name) header basilys.h
   Copyright (C)  2008 Free Software Foundation, Inc.
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
along with GCC; see the file COPYING3.   If not see
<http://www.gnu.org/licenses/>.    */

#ifndef BASILYS_INCLUDED_
#define BASILYS_INCLUDED_

#if !ENABLE_BASILYSMELT
#error basilys.h should enly be used when basilemelt is enabled at configuretion time
#endif

#ifndef MELTGCC_DYNAMIC_OBJSTRUCT
#define MELTGCC_DYNAMIC_OBJSTRUCT 0
#endif

/***** TODO: 

       if GGC-collected data, e.g. tree-s, edge-s, ... is computed by
       basilys/MELT routines and is referenced only by the
       basilys/MELT call frames, it is lost on full basilys garbage
       collections, because the GGC collector is invoked (on full
       basilys GC) without being aware of such data.

       For basilys code which only inspects but does not create or
       modify such data this won't happen.

       A possible solution might be to generate code which copy such
       GGC data outside (e.g. into some specific GGC vector) on full
       garbage collections. This code might either be another routine
       pointer in our basilysroutine_st structure, or simply be
       invoked by calling the closure routine with a magic incantation,
       i.e. with the xargdescr_ set to (char*)-1

 *****/

/* declared in toplev.h which we want to avoid #include-ing */
extern void fatal_error (const char *, ...);

/* use -fdump-ipa-basilys */

#define dbgprintf_raw(Fmt,...) do{if (dump_file) \
      {fprintf(dump_file, Fmt, ##__VA_ARGS__); fflush(dump_file);}}while(0)
#define dbgprintf(Fmt,...) dbgprintf_raw("@%s:%d: " Fmt "\n", \
 basename(__FILE__), __LINE__, ##__VA_ARGS__)

extern long basilys_dbgcounter;
extern long basilys_debugskipcount;

#define debugeprintf_raw(Fmt,...) do{if (flag_basilys_debug) \
      {fprintf(stderr, Fmt, ##__VA_ARGS__); fflush(stderr);}}while(0)
#define debugeprintf(Fmt,...) debugeprintf_raw("!@%s:%d:\n@! " Fmt "\n", \
      basename(__FILE__), __LINE__, ##__VA_ARGS__)
#define debugeprintvalue(Msg,Val) do{if (flag_basilys_debug){	\
      void* __val = (Val);					\
      fprintf(stderr,"!@%s:%d:\n@! %s @%p= ",			\
              basename(__FILE__), __LINE__, (Msg), __val);	\
      basilys_dbgeprint(__val); }} while(0)
#define debugebacktrace(Msg,Depth)  do{if (flag_basilys_debug){	\
      void* __val = (Val);					\
      fprintf(stderr,"!@%s:%d: %s **backtrace** ",		\
              basename(__FILE__), __LINE__, (Msg));	\
      basilys_dbgbacktrace((Depth)); }} while(0)
/* the maximal debug depth - should be a parameter */
#define BASILYSDBG_MAXDEPTH 7

/* unspecified flexible dimension in structure */
#if defined(__STDC__) &&  __STDC__VERSION >= 199901L
#define FLEXIBLE_DIM		/*flexible */
#define HAVE_FLEXIBLE_DIM 1
#elsif __GNUC__>=4
#define FLEXIBLE_DIM /*gcc flexible*/
#define HAVE_FLEXIBLE_DIM 1
#else
#define  FLEXIBLE_DIM /*flexibly*/1
#define HAVE_FLEXIBLE_DIM 0
#endif

/* array of (at least 100, increasing order but non consecutive)
   primes, zero terminated. Each prime is at least 1/8-th bigger than
   previous */
extern const long basilys_primtab[256];


/* naming convention: all struct basilys*_st are inside the
   basilys_un */

typedef union basilys_un *basilys_ptr_t;
typedef struct basilysobject_st *basilysobject_ptr_t;
typedef struct basilysmapobjects_st *basilysmapobjects_ptr_t;
typedef struct basilysclosure_st *basilysclosure_ptr_t;
typedef struct basilysroutine_st *basilysroutine_ptr_t;
typedef struct basilysmultiple_st *basilysmultiple_ptr_t;
typedef struct basilysbox_st *basilysbox_ptr_t;
typedef struct basilyspair_st *basilyspair_ptr_t;

struct debugprint_basilys_st
{
  FILE *dfil;
  int dmaxdepth;
  int dcount;
};

void basilys_debug_out (struct debugprint_basilys_st *dp, basilys_ptr_t ptr,
			int depth);
void basilys_dbgeprint (void *p);
void basilys_dbgbacktrace (int depth);

/******************* closures, routines ************************/

union basilysparam_un
{
  /* for basilys value pointers, we pass the address of a local, to be
     compatible with our copying garbage collector */
  basilys_ptr_t *bp_aptr;	/* letter P */
#define BPAR_PTR          'P'
#define BPARSTR_PTR       "P"

  /* for passing a pair-list of rest arguments, we pass likewise the
     address of a local */
#define BPAR_RESTPTR      'R'
#define BPARSTR_RESTPTR   "R"
  basilys_ptr_t *bp_rptr;	/* letter R */


  tree bp_tree;			/* letter t */
  tree *bp_treeptr;		/* for extra results */
#define BPAR_TREE         't'
#define BPARSTR_TREE      "t"

  gimple bp_gimple;			/* letter g */
  gimple *bp_gimpleptr;		/* for extra results */
#define BPAR_GIMPLE       'g'
#define BPARSTR_GIMPLE    "g"

  gimple_seq bp_gimpleseq;			/* letter g */
  gimple_seq *bp_gimpleseqptr;		/* for extra results */
#define BPAR_GIMPLESEQ       'G'
#define BPARSTR_GIMPLESEQ    "G"

  long bp_long;			/* letter l */
  long *bp_longptr;		/* for results */
#define BPAR_LONG         'l'
#define BPARSTR_LONG      "l"

  edge bp_edge;			/* letter e */
  edge *bp_edgeptr;		/* for results */
#define BPAR_EDGE         'e'
#define BPARSTR_EDGE      "e"

  basic_block bp_bb;		/* letter b */
  basic_block *bp_bbptr;	/* for results */
#define BPAR_BB           'b'
#define BPARSTR_BB        "b"

  /* readonly constant strings - not in GP nor in heap */
  const char *bp_cstring;			/* letter S */
  const char **bp_cstringptr;		/* for results */
#define BPAR_CSTRING         'S'
#define BPARSTR_CSTRING      "S"
};

/*** the closures contain routines which are called by applying
     closures; each routine is called with:

     + the called closure
     + this first pointer argument
     + a (non null, can be empty) constant string describing the extra arguments
       (eg "ppt" for two value pointers and one tree)
     + the array of union basilysparam_un for extra arguments
     + a (non null, can be empty) constant string describing the extra results
     + the array of union basilysparam_un for extra results

     and the result of the call is a pointer (the main result)

BTW, on AMD64 or x86_64 processors [a very common host at time of
writing], http://www.x86-64.org/documentation/abi.pdf the first six
arguments are passed thru registers; on POWERPC eight arguments are
passed thru registers
*/


typedef basilys_ptr_t basilysroutfun_t (basilysclosure_ptr_t closp_,
					basilys_ptr_t firstargp_,
					const char xargdescr_[],
					union basilysparam_un *xargtab_,
					const char xresdescr_[],
					union basilysparam_un *xrestab_);

/* the application routine does not call the GC; of course, the
   applied closure can call the GC! */
basilys_ptr_t basilys_apply (basilysclosure_ptr_t clos_p,
			       basilys_ptr_t firstarg,
			       const char xargdescr_[],
			       union basilysparam_un *xargtab_,
			       const char xresdescr_[],
			       union basilysparam_un *xrestab_);

/* gnu indent has some trouble with GTY hence */
/* *INDENT-OFF* */

DEF_VEC_P (basilys_ptr_t);
DEF_VEC_ALLOC_P (basilys_ptr_t, gc);



DEF_VEC_P (basilysobject_ptr_t);
DEF_VEC_ALLOC_P (basilysobject_ptr_t, gc);


/* sadly we cannot use these types in GTY-ed structure because
   gengtype don't follow typedefs but these typedef-s are still
   useful */

typedef
VEC (basilysobject_ptr_t, gc)
  basilys_objectvec_t;
     
typedef VEC (basilys_ptr_t, gc) basilys_valvec_t;

enum obmag_en    {
  OBMAG__NONE = 0,
  OBMAG_OBJECT = 30000,
  OBMAG_BOX,
  OBMAG_MULTIPLE,
  OBMAG_CLOSURE,
  OBMAG_ROUTINE,
  OBMAG_LIST,
  OBMAG_PAIR,
  OBMAG_TRIPLE,
  OBMAG_INT,
  OBMAG_MIXINT,
  OBMAG_MIXLOC,
  OBMAG_REAL,
  OBMAG_STRING,
  OBMAG_STRBUF,
  OBMAG_TREE,
  OBMAG_GIMPLE,
  OBMAG_GIMPLESEQ,
  OBMAG_BASICBLOCK,
  OBMAG_EDGE,
  OBMAG_MAPOBJECTS,
  OBMAG_MAPSTRINGS,
  OBMAG_MAPTREES,
  OBMAG_MAPGIMPLES,
  OBMAG_MAPGIMPLESEQS,
  OBMAG_MAPBASICBLOCKS,
  OBMAG_MAPEDGES,
  OBMAG_DECAY,
  OBMAG__SPARE1,
  OBMAG__SPARE2,
  OBMAG__SPARE3,
  OBMAG__SPARE4,
  OBMAG__SPARE5,
  OBMAG__SPARE6,
  OBMAG__SPARE7,
  OBMAG__SPARE8,
  OBMAG__SPARE9,
  OBMAG__SPARE10,
  OBMAG__SPARE11,
  OBMAG__SPARE12,
  OBMAG__SPARE13,
  OBMAG__SPARE14,
  OBMAG__SPARE15,
  OBMAG__SPARE16,
  OBMAG__SPARE17,
  OBMAG__SPARE18,
  OBMAG__SPARE19,
  OBMAG__SPARE20,
  OBMAG__SPARE21,
  OBMAG__SPARE22,
  OBMAG__SPARE23,
  OBMAG__SPARE24,
  OBMAG__SPARE25,
  OBMAG__SPARE26,
  OBMAG__SPARE27,
  OBMAG__SPARE28,
  OBMAG__SPARE29,
  OBMAG__SPARE30,
  OBMAG__SPARE31,
  OBMAG__SPARE32,
  OBMAG__SPARE33,
  OBMAG__SPARE34,
  OBMAG__SPARE35,
  OBMAG__SPARE36,
  OBMAG__SPARE37,
  OBMAG__SPARE38,
  OBMAG_SPEC_FILE,
  OBMAG_SPEC_MPFR,
  OBMAG_SPECPPL_COEFFICIENT,
  OBMAG_SPECPPL_LINEAR_EXPRESSION,
  OBMAG_SPECPPL_CONSTRAINT,
  OBMAG_SPECPPL_CONSTRAINT_SYSTEM,
  OBMAG_SPECPPL_GENERATOR,
  OBMAG_SPECPPL_GENERATOR_SYSTEM,
  OBMAG__LAST
};


/* maxhash can also be used as a bit mask */
#define BASILYS_MAXHASH 0x3fffffff

/* maxlen can also be used as a bit mask */
#define BASILYS_MAXLEN 0x1fffffff


/***
    objects are a la ObjVlisp, single-inheritance with a root class, 
    the discr of an object is its class
    each object has its hashcode, its magic (used to discriminate non-object values),
    its number of slots or instance variables object_arity, and an array of slots called vartab[] 

    objects should be allocated in young region, hence discr should be
    forwarded in the garbage collector



*/

/* *INDENT-ON* */

/* when OBMAG_OBJECT -- */

/* we now have fixed length objects - so the number of variables cannot change */
#define BASILYS_HAS_OBJ_TAB_FIELDS 0

struct basilysobject_st
GTY (())
{
  /* for objects, the discriminant is their class */
  basilysobject_ptr_t obj_class;
  unsigned obj_hash;		/* hash code of the object */
  unsigned short obj_num;
/* discriminate the basilys_un containing it as discr */
#define object_magic obj_num
  unsigned short obj_len;
#if ENABLE_CHECKING
  unsigned long obj_serial;
#endif
  basilys_ptr_t GTY ((length ("%h.obj_len"))) obj_vartab[FLEXIBLE_DIM];
};

#if ENABLE_CHECKING

#if BASILYS_HAS_OBJ_TAB_FIELDS
#error BASILYS_HAS_OBJ_TAB_FIELDS is no longer supported, because gengtype dislike CPP conditionals.
/* 
#define BASILYS_OBJECT_STRUCT(N) {		\
  basilysobject_ptr_t obj_class;		\
  unsigned obj_hash;				\
  unsigned short obj_num;			\
  unsigned short obj_len;			\
  unsigned long obj_serial;                     \
  basilys_ptr_t* obj_vartab;			\
  basilys_ptr_t obj__tabfields[N];		\
  long _gap; }
*/
#else /*!BASILYS_HAS_OBJ_TAB_FIELDS*/
#define BASILYS_OBJECT_STRUCT(N) {		\
  basilysobject_ptr_t obj_class;		\
  unsigned obj_hash;				\
  unsigned short obj_num;			\
  unsigned short obj_len;			\
  unsigned long obj_serial;                     \
  basilys_ptr_t* obj_vartab[N];			\
  long _gap; }
#endif /*BASILYS_HAS_OBJ_TAB_FIELDS*/

void basilys_object_set_serial(basilysobject_ptr_t ob);

#else /*!ENABLE_CHECKING*/

#if BASILYS_HAS_OBJ_TAB_FIELDS
#error BASILYS_HAS_OBJ_TAB_FIELDS is no longer supported, because gengtype dislike CPP conditionals.
/*
#define BASILYS_OBJECT_STRUCT(N) {		\
  basilysobject_ptr_t obj_class;		\
  unsigned obj_hash;				\
  unsigned short obj_num;			\
  unsigned short obj_len;			\
  basilys_ptr_t* obj_vartab;			\
  basilys_ptr_t obj__tabfields[N];		\
  long _gap; }
*/
#else /*!BASILYS_HAS_OBJ_TAB_FIELDS*/
#define BASILYS_OBJECT_STRUCT(N) {		\
  basilysobject_ptr_t obj_class;		\
  unsigned obj_hash;				\
  unsigned short obj_num;			\
  unsigned short obj_len;			\
  basilys_ptr_t* obj_vartab[N];			\
  long _gap; }
#endif /*BASILYS_HAS_OBJ_TAB_FIELDS*/

/* set serial is a nop */
static inline void basilys_object_set_serial(basilysobject_ptr_t ob) {}
#endif

/* some types, including objects, strbuf, stringmaps, objectmaps, all
   the other *maps, contain a pointer to a non value; this pointer
   should be carefully updated in the forwarding step (and checked if
   young) */

/* forwarded pointers; nobody see them except the basilys copying
   garbage collector */
struct basilysforward_st
GTY (())
{
  basilysobject_ptr_t discr;	/* actually always (void*)1 for forwarded */
  basilys_ptr_t forward;
};

/* when OBMAG_DECAY  */
struct basilysdecay_st
GTY ((mark_hook ("basilys_mark_decay")))
{
  basilysobject_ptr_t discr;
  basilys_ptr_t val;
  unsigned remain;		/* remaining number of marking */
};


/* when OBMAG_BOX  */
struct basilysbox_st
GTY (())
{
  basilysobject_ptr_t discr;
  basilys_ptr_t val;
};

/* when OBMAG_MULTIPLE  */
struct basilysmultiple_st
GTY (())
{
  basilysobject_ptr_t discr;
  unsigned nbval;
  basilys_ptr_t GTY ((length ("%h.nbval"))) tabval[FLEXIBLE_DIM];
};

#define BASILYS_MULTIPLE_STRUCT(N) {		\
  basilysobject_ptr_t discr;			\
  unsigned nbval;				\
  basilys_ptr_t tabval[N];			\
  long _gap; }

/* when OBMAG_CLOSURE */
struct basilysclosure_st
GTY (())
{
  basilysobject_ptr_t discr;
  basilysroutine_ptr_t rout;
  unsigned nbval;
  basilys_ptr_t GTY ((length ("%h.nbval"))) tabval[FLEXIBLE_DIM];
};

#define BASILYS_CLOSURE_STRUCT(N) {		\
  basilysobject_ptr_t discr;			\
  basilysroutine_ptr_t rout;			\
  unsigned nbval;				\
  basilys_ptr_t tabval[N];			\
  long _gap; }

/* when OBMAG_ROUTINE */
#define BASILYS_ROUTDESCR_LEN 96
#define BASILYS_ROUTADDR_LEN  (1 + (2*sizeof (basilysroutfun_t *))/ sizeof (long))

struct basilysroutine_st
GTY (())
{
  basilysobject_ptr_t discr;
  char routdescr[BASILYS_ROUTDESCR_LEN];
  long GTY ((skip)) routaddr[BASILYS_ROUTADDR_LEN];
  basilys_ptr_t routdata;
  unsigned nbval;
  basilys_ptr_t GTY ((length ("%h.nbval"))) tabval[FLEXIBLE_DIM];
};

#define BASILYS_ROUTINE_STRUCT(N) {				\
  basilysobject_ptr_t discr;					\
  char routdescr[BASILYS_ROUTDESCR_LEN];			\
  long routaddr[1+2*sizeof(basilysroutfun_t *)/sizeof(long)];	\
  basilys_ptr_t routdata;					\
  unsigned nbval;						\
  basilys_ptr_t tabval[N];					\
  long _gap; }


/* when OBMAG_PAIR */
struct basilyspair_st
GTY ((chain_next ("%h.tl")))
{
  basilysobject_ptr_t discr;
  basilys_ptr_t hd;
  struct basilyspair_st *tl;
};

/* when OBMAG_TRIPLE */
struct basilystriple_st
GTY ((chain_next ("%h.tl")))
{
  basilysobject_ptr_t discr;
  basilys_ptr_t hd;
  basilys_ptr_t mi;
  struct basilystriple_st *tl;
};

/* when OBMAG_LIST   */
struct basilyslist_st
GTY (())
{
  basilysobject_ptr_t discr;
  struct basilyspair_st *first;
  struct basilyspair_st *last;
};





/* when OBMAG_INT -  */
struct basilysint_st
GTY (())
{
  basilysobject_ptr_t discr;
  long val;
};

/* when OBMAG_MIXINT -  */
struct basilysmixint_st
GTY (())
{
  basilysobject_ptr_t discr;
  basilys_ptr_t ptrval;
  long intval;
};

/* when OBMAG_MIXLOC -  */
struct basilysmixloc_st
GTY (())
{
  basilysobject_ptr_t discr;
  basilys_ptr_t ptrval;
  long intval;
  location_t locval;
};



/* when OBMAG_REAL   */
struct basilysreal_st
GTY (())
{
  basilysobject_ptr_t discr;
  REAL_VALUE_TYPE val;
};


/* a union of special pointers which have to be explicitly deleted */
union special_basilys_un
{
  /* all the pointers here have to be pointers to struct or to void,
     because the generated gtype-desc.c don't include all the files
     which define mpfr_ptr ppl_Coefficient_t etc... */
  /* generic pointer */ void *sp_pointer;
  /* stdio file */ FILE *sp_file;
  /*mpfr_ptr= */ void *sp_mpfr;
  /* malloced pointer to mpfr_t */
  /*ppl_Coefficient_t= */ struct ppl_Coefficient_tag *sp_coefficient;
  /*ppl_Linear_Expression_t= */ struct ppl_Linear_Expression_tag
   *sp_linear_expression;
  /*ppl_Constraint_t= */ struct ppl_Constraint_tag *sp_constraint;
  /*ppl_Constraint_System_t= */ struct ppl_Constraint_System_tag
   *sp_constraint_system;
  /*ppl_Generator_t= */ struct ppl_Generator_tag *sp_generator;
  /*ppl_Generator_System_t= */ struct ppl_Generator_System_tag
   *sp_generator_system;
};

/* PPL special have to be explicitly deleted; hence we need a hook
   to mark them, an inside mark, and to maintain lists of existing
   such PPL special boxes -which we scan to delete the unmarked
   ones */
/* when OBMAG_SPEC* eg OBMAG_SPEC_MPFR, OBMAG_SPECPPL_COEFFICIENT; etc.
    */
struct basilysspecial_st
GTY ((mark_hook ("basilys_mark_special")))
{
  basilysobject_ptr_t discr;
  int mark;
  struct basilysspecial_st *GTY ((skip)) nextspec;
  union special_basilys_un GTY ((skip)) val;
};

static inline void
basilys_mark_special (struct basilysspecial_st *p)
{
  p->mark = 1;
}

static inline void
basilys_mark_decay (struct basilysdecay_st *p)
{
  /* this is tricky since it actually changes the marked data; however,
     changing pointers to NULL is ok! */
  if (p->remain <= 0)
    p->val = NULL;
  else
    p->remain--;
}

/* when OBMAG_STRING -  */
struct basilysstring_st
GTY (())
{
  basilysobject_ptr_t discr;
  char val[FLEXIBLE_DIM];	/* null terminated */
};

#define BASILYS_STRING_STRUCT(N) {		\
  basilysobject_ptr_t discr;			\
  char val[(N)+1];	/* null terminated */	\
  long _gap; }

/* when OBMAG_STRBUF -   */
struct basilysstrbuf_st
GTY (())
{
  basilysobject_ptr_t discr;
  char *GTY ((length ("1+basilys_primtab[%h.buflenix]"))) bufzn;
  unsigned char buflenix;	/* allocated length index of
				   buffer */
  unsigned bufstart;
  unsigned bufend;		/* start & end useful positions */
  /* the following field is usually the value of buf (for
     objects in the young zone), to allocate the object and its fields
     at once; hence its GTY-ed length is zero */
  char GTY ((length ("0"))) buf_space[FLEXIBLE_DIM];
};

/* when OBMAG_TREE  */
struct basilystree_st
GTY (())
{
  basilysobject_ptr_t discr;
  tree val;
};

/* when OBMAG_GIMPLE  */
struct basilysgimple_st
GTY (())
{
  basilysobject_ptr_t discr;
  gimple val;
};

/* when OBMAG_GIMPLESEQ  */
struct basilysgimpleseq_st
GTY (())
{
  basilysobject_ptr_t discr;
  gimple_seq val;
};

/* when OBMAG_BASICBLOCK   */
struct basilysbasicblock_st
GTY (())
{
  basilysobject_ptr_t discr;
  basic_block val;
};

/* when OBMAG_EDGE   */
struct basilysedge_st
GTY (())
{
  basilysobject_ptr_t discr;
  edge val;
};




/*** hashed maps of objects to basilys ***/
struct entryobjectsbasilys_st
GTY (())
{
  basilysobject_ptr_t e_at;
  basilys_ptr_t e_va;
};

/* when OBMAG_MAPOBJECTS */
struct basilysmapobjects_st
GTY (())
{
  basilysobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entryobjectsbasilys_st *GTY ((length ("basilys_primtab[%h.lenix]")))
    entab;
  /* the following field is usually the value of entab (for
     objects in the young zone), to allocate the object and its fields
     at once; hence its GTY-ed length is zero */
  struct entryobjectsbasilys_st GTY ((length ("0"))) map_space[FLEXIBLE_DIM];
};

/*** hashed maps of trees to basilys ***/
struct entrytreesbasilys_st
GTY (())
{
  tree e_at;
  basilys_ptr_t e_va;
};

/* when OBMAG_MAPTREES */
struct basilysmaptrees_st
GTY (())
{
  /* change basilysmappointers_st when changing this structure */
  basilysobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entrytreesbasilys_st *GTY ((length ("basilys_primtab[%h.lenix]")))
    entab;
};



/*** hashed maps of gimples to basilys ***/
struct entrygimplesbasilys_st
GTY (())
{
  gimple e_at;
  basilys_ptr_t e_va;
};

/* when OBMAG_MAPGIMPLES */
struct basilysmapgimples_st
GTY (())
{
  /* change basilysmappointers_st when changing this structure */
  basilysobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entrygimplesbasilys_st *GTY ((length ("basilys_primtab[%h.lenix]")))
    entab;
};


/*** hashed maps of gimpleseqs to basilys ***/
struct entrygimpleseqsbasilys_st
GTY (())
{
  gimple_seq e_at;
  basilys_ptr_t e_va;
};

/* when OBMAG_MAPGIMPLESEQS */
struct basilysmapgimpleseqs_st
GTY (())
{
  /* change basilysmappointers_st when changing this structure */
  basilysobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entrygimpleseqsbasilys_st *GTY ((length ("basilys_primtab[%h.lenix]")))
    entab;
};



/*** hashed maps of strings to basilys ***/
struct entrystringsbasilys_st
GTY (())
{
  const char *GTY (()) e_at;
  basilys_ptr_t e_va;
};

/* when OBMAG_MAPSTRINGS */
struct basilysmapstrings_st
GTY (())
{
  basilysobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entrystringsbasilys_st *GTY ((length ("basilys_primtab[%h.lenix]")))
    entab;
};


/*** hashed maps of basicblocks to basilys ***/

struct entrybasicblocksbasilys_st
GTY (())
{
  basic_block e_at;
  basilys_ptr_t e_va;
};

/* when OBMAG_MAPBASICBLOCKS */
struct basilysmapbasicblocks_st
GTY (())
{
  /* change basilysmappointers_st when changing this structure */
  basilysobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entrybasicblocksbasilys_st
    *GTY ((length ("basilys_primtab[%h.lenix]"))) entab;
};

/*** hashed maps of edges to basilys ***/
struct entryedgesbasilys_st
GTY (())
{
  edge e_at;
  basilys_ptr_t e_va;
};

/* when OBMAG_MAPEDGES */
struct basilysmapedges_st
GTY (())
{
  /* change basilysmappointers_st when changing this structure */
  basilysobject_ptr_t discr;
  unsigned count;
  unsigned char lenix;
  struct entryedgesbasilys_st *GTY ((length ("basilys_primtab[%h.lenix]")))
    entab;
};


/**** our union for everything ***/
/* never use an array of basilys_un, only array of pointers basilys_ptr_t */
typedef union basilys_un
GTY ((desc ("%0.u_discr->object_magic")))
{
  basilysobject_ptr_t GTY ((skip)) u_discr;
  struct basilysforward_st GTY ((skip)) u_forward;
  struct basilysobject_st GTY ((tag ("OBMAG_OBJECT"))) u_object;
  struct basilysbox_st GTY ((tag ("OBMAG_BOX"))) u_box;
  struct basilysdecay_st GTY ((tag ("OBMAG_DECAY"))) u_decay;
  struct basilysmultiple_st GTY ((tag ("OBMAG_MULTIPLE"))) u_multiple;
  struct basilysclosure_st GTY ((tag ("OBMAG_CLOSURE"))) u_closure;
  struct basilysroutine_st GTY ((tag ("OBMAG_ROUTINE"))) u_routine;
  struct basilyslist_st GTY ((tag ("OBMAG_LIST"))) u_list;
  struct basilysint_st GTY ((tag ("OBMAG_INT"))) u_int;
  struct basilysmixint_st GTY ((tag ("OBMAG_MIXINT"))) u_mixint;
  struct basilysmixloc_st GTY ((tag ("OBMAG_MIXLOC"))) u_mixloc;
  struct basilysreal_st GTY ((tag ("OBMAG_REAL"))) u_real;
  struct basilyspair_st GTY ((tag ("OBMAG_PAIR"))) u_pair;
  struct basilystriple_st GTY ((tag ("OBMAG_TRIPLE"))) u_triple;
  struct basilysspecial_st
    GTY ((tag ("OBMAG_SPEC_FILE"),
	  tag ("OBMAG_SPEC_MPFR"),
	  tag ("OBMAG_SPECPPL_COEFFICIENT"),
	  tag ("OBMAG_SPECPPL_LINEAR_EXPRESSION"),
	  tag ("OBMAG_SPECPPL_CONSTRAINT"),
	  tag ("OBMAG_SPECPPL_CONSTRAINT_SYSTEM"),
	  tag ("OBMAG_SPECPPL_GENERATOR"),
	  tag ("OBMAG_SPECPPL_GENERATOR_SYSTEM"))) u_special;
  struct basilysstring_st GTY ((tag ("OBMAG_STRING"))) u_string;
  struct basilysstrbuf_st GTY ((tag ("OBMAG_STRBUF"))) u_strbuf;
  struct basilystree_st GTY ((tag ("OBMAG_TREE"))) u_tree;
  struct basilysgimple_st GTY ((tag ("OBMAG_GIMPLE"))) u_gimple;
  struct basilysgimpleseq_st GTY ((tag ("OBMAG_GIMPLESEQ"))) u_gimpleseq;
  struct basilysbasicblock_st GTY ((tag ("OBMAG_BASICBLOCK"))) u_basicblock;
  struct basilysedge_st GTY ((tag ("OBMAG_EDGE"))) u_edge;
  struct basilysmapobjects_st GTY ((tag ("OBMAG_MAPOBJECTS"))) u_mapobjects;
  struct basilysmapstrings_st GTY ((tag ("OBMAG_MAPSTRINGS"))) u_mapstrings;
  struct basilysmaptrees_st GTY ((tag ("OBMAG_MAPTREES"))) u_maptrees;
  struct basilysmapgimples_st GTY ((tag ("OBMAG_MAPGIMPLES"))) u_mapgimples;
  struct basilysmapgimpleseqs_st GTY ((tag ("OBMAG_MAPGIMPLESEQS"))) u_mapgimpleseqs;
  struct basilysmapbasicblocks_st GTY ((tag ("OBMAG_MAPBASICBLOCKS")))
    u_mapbasicblocks;
  struct basilysmapedges_st GTY ((tag ("OBMAG_MAPEDGES"))) u_mapedges;
} basilys_un_t;

/* return the magic of the discriminant or 0 */
static inline int
basilys_magic_discr (basilys_ptr_t p)
{
  if (!p ||  !p->u_discr)
    return 0;
  return p->u_discr->object_magic;
}


/* return the nth of a multiple (starting from 0) */
static inline basilys_ptr_t
basilys_multiple_nth (basilys_ptr_t mul, int n)
{
  if (!mul || ((basilysmultiple_ptr_t)mul)->discr->object_magic != OBMAG_MULTIPLE)
    return NULL;
  if (n >= 0 && n < (int) ((basilysmultiple_ptr_t)mul)->nbval)
    return ((basilysmultiple_ptr_t)mul)->tabval[n];
  else if (n < 0 && n + (int) ((basilysmultiple_ptr_t)mul)->nbval >= 0)
    return ((basilysmultiple_ptr_t)mul)->tabval[n + ((basilysmultiple_ptr_t)mul)->nbval];
  return NULL;
}

/* set the nth of a multiple (but beware of circularities!) */
void basilysgc_multiple_put_nth (basilys_ptr_t mul, int n,
				 basilys_ptr_t val);

/* return the length of a multiple */
static inline int
basilys_multiple_length (basilys_ptr_t mul)
{
  if (!mul || ((basilysmultiple_ptr_t)mul)->discr->object_magic != OBMAG_MULTIPLE)
    return 0;
  return ((basilysmultiple_ptr_t)mul)->nbval;
}

/* sort a multiple MUL using as compare function the closure CMPCLO
   which should return a boxed integer (0 for equality, <0 for less
   than, >0 for greater than), when applied to two values to
   compare. If the closure does not return an integer the whole sort
   returns null; otherwise it returns a new multiple value of
   discriminant DISCRM */
basilys_ptr_t
basilysgc_sort_multiple(basilys_ptr_t mult_p, basilys_ptr_t clo_p, basilys_ptr_t discrm_p); 


/* allocate a new box of given DISCR & content VAL */
basilys_ptr_t basilysgc_new_box (basilysobject_ptr_t discr_p,
				 basilys_ptr_t val_p);

/* return the content of a box */
static inline basilys_ptr_t
basilys_box_content (basilysbox_ptr_t box)
{
  if (!box || box->discr->object_magic != OBMAG_BOX)
    return NULL;
  return box->val;
}

void basilysgc_box_put (basilys_ptr_t box, basilys_ptr_t val);

/* safely return the calue inside a container - instance of CLASS_CONTAINER */
basilys_ptr_t
basilys_container_value  (basilys_ptr_t cont);


void *basilysgc_raw_new_mappointers (basilysobject_ptr_t discr_p,
				     unsigned len);

void
basilysgc_raw_put_mappointers (void *mappointer_p,
			       const void *attr, basilys_ptr_t valu_p);

basilys_ptr_t
basilys_raw_get_mappointers (void *mappointer_p, const void *attr);

basilys_ptr_t
basilysgc_raw_remove_mappointers (void *mappointer_p, const void *attr);


/* big macro to implement a mapFOOs */
#define BASILYS_DEFINE_MAPTR(Obmag,Ptyp,Mapstruct,Newf,Getf,Putf,Removef,Countf,Sizef,Nthattrf,Nthvalf)	\
									\
static inline basilys_ptr_t						\
Newf (basilysobject_ptr_t discr, unsigned len)				\
{									\
  if (basilys_magic_discr ((basilys_ptr_t) discr) != OBMAG_OBJECT)	\
    return NULL;							\
  if (discr->object_magic != Obmag)					\
    return NULL;							\
  return (basilys_ptr_t) basilysgc_raw_new_mappointers (discr, len);	\
}									\
									\
static inline basilys_ptr_t						\
Getf (basilys_ptr_t map_p, Ptyp attr)					\
{									\
  if (basilys_magic_discr ((basilys_ptr_t) map_p) != Obmag || !attr)	\
    return NULL;							\
  return basilys_raw_get_mappointers (map_p, attr);			\
}									\
									\
static inline void							\
Putf (struct Mapstruct *map_p,						\
	Ptyp attr, basilys_ptr_t valu_p)				\
{									\
  if (basilys_magic_discr ((basilys_ptr_t) map_p) != Obmag		\
      || !attr || !valu_p)						\
    return;								\
  basilysgc_raw_put_mappointers (map_p, attr, valu_p);			\
}									\
									\
static inline basilys_ptr_t						\
Removef (struct Mapstruct *map, Ptyp attr)				\
{									\
  if (basilys_magic_discr ((basilys_ptr_t) map) != Obmag || !attr)	\
    return NULL;							\
  return basilysgc_raw_remove_mappointers (map, attr);			\
}									\
									\
static inline unsigned							\
Countf (struct Mapstruct* map_p)					\
{									\
  if (!map_p || map_p->discr->obj_num != Obmag)				\
    return 0;								\
  return map_p->count;							\
}									\
									\
static inline int							\
Sizef (struct Mapstruct* map_p)						\
{									\
  if (!map_p || map_p->discr->obj_num != Obmag)				\
    return 0;								\
  return basilys_primtab[map_p->lenix];					\
}									\
									\
static inline Ptyp							\
Nthattrf(struct Mapstruct* map_p, int ix)				\
{									\
  Ptyp at = 0;								\
  if (!map_p || map_p->discr->obj_num != Obmag)				\
    return 0;								\
  if (ix < 0 || ix >= basilys_primtab[map_p->lenix])			\
    return 0;								\
  at = map_p->entab[ix].e_at;						\
  if ((void *) at == (void *) HTAB_DELETED_ENTRY)			\
    return 0;								\
  return at;								\
}									\
									\
static inline basilys_ptr_t						\
Nthvalf(struct Mapstruct* map_p, int ix)				\
{									\
  Ptyp at = 0;								\
  if (!map_p || map_p->discr->obj_num != Obmag)				\
    return 0;								\
  if (ix < 0 || ix >= basilys_primtab[map_p->lenix])			\
    return 0;								\
  at = map_p->entab[ix].e_at;						\
  if (!at || (void *) at == (void *) HTAB_DELETED_ENTRY)		\
    return 0;								\
  return map_p->entab[ix].e_va;					        \
}

/* end of BASILYS_DEFINE_MAPTR macro */

BASILYS_DEFINE_MAPTR(OBMAG_MAPTREES, tree, basilysmaptrees_st,
		      basilysgc_new_maptrees,
		      basilys_get_maptrees,
		      basilys_put_maptrees,
		      basilys_remove_maptrees,
		      basilys_count_maptrees,
		      basilys_size_maptrees,
		      basilys_nthattr_maptrees,
		      basilys_nthval_maptrees)

BASILYS_DEFINE_MAPTR(OBMAG_MAPGIMPLES, gimple, basilysmapgimples_st,
		      basilysgc_new_mapgimples,
		      basilys_get_mapgimples,
		      basilys_put_mapgimples,
		      basilys_remove_mapgimples,
		      basilys_count_mapgimples,
		      basilys_size_mapgimples,
		      basilys_nthattr_mapgimples,
		      basilys_nthval_mapgimples)

BASILYS_DEFINE_MAPTR(OBMAG_MAPGIMPLESEQS, gimple_seq, basilysmapgimpleseqs_st,
		      basilysgc_new_mapgimpleseqs,
		      basilys_get_mapgimpleseqs,
		      basilys_put_mapgimpleseqs,
		      basilys_remove_mapgimpleseqs,
		      basilys_count_mapgimpleseqs,
		      basilys_size_mapgimpleseqs,
		      basilys_nthattr_mapgimpleseqs,
		      basilys_nthval_mapgimpleseqs)


BASILYS_DEFINE_MAPTR(OBMAG_MAPEDGES, edge, basilysmapedges_st,
		      basilysgc_new_mapedges,
		      basilys_get_mapedges,
		      basilys_put_mapedges,
		      basilys_remove_mapedges,
		      basilys_count_mapedges,
		      basilys_size_mapedges,
		      basilys_nthattr_mapedges,
		      basilys_nthval_mapedges)

BASILYS_DEFINE_MAPTR(OBMAG_MAPBASICBLOCKS, basic_block, basilysmapbasicblocks_st,
		      basilysgc_new_mapbasicblocks,
		      basilys_get_mapbasicblocks,
		      basilys_put_mapbasicblocks,
		      basilys_remove_mapbasicblocks,
		      basilys_count_mapbasicblocks,
		      basilys_size_mapbasicblocks,
		      basilys_nthattr_mapbasicblocks,
		      basilys_nthval_mapbasicblocks)


/* do not use BASILYS_DEFINE_MAPTR elsewhere */
#undef BASILYS_DEFINE_MAPTR

/* allocate a new boxed tree of given DISCR [DISCR_TREE if null] &
   content VAL */
basilys_ptr_t basilysgc_new_tree (basilysobject_ptr_t discr_p,
				 tree val);

/* return the content of a boxed tree */
static inline tree
basilys_tree_content (basilys_ptr_t box)
{
  struct basilystree_st* tr = (struct basilystree_st*)box;
  if (!tr || tr->discr->object_magic != OBMAG_TREE)
    return NULL;
  return tr->val;
}

/* allocate a new boxed gimple of given DISCR [DISCR_GIMPLE if null] &
   content VAL */
basilys_ptr_t basilysgc_new_gimple (basilysobject_ptr_t discr_p,
				    gimple val);

/* return the content of a boxed gimple */
static inline gimple
basilys_gimple_content (basilys_ptr_t box)
{
  struct basilysgimple_st* g = (struct basilysgimple_st*)box;
  if (!g || g->discr->object_magic != OBMAG_GIMPLE)
    return NULL;
  return g->val;
}

/* allocate a new boxed gimpleseq of given DISCR [DISCR_GIMPLESEQ if null] &
   content VAL */
basilys_ptr_t basilysgc_new_gimpleseq (basilysobject_ptr_t discr_p,
				    gimple_seq val);

/* return the content of a boxed gimple */
static inline gimple_seq
basilys_gimpleseq_content (basilys_ptr_t box)
{
  struct basilysgimpleseq_st* g = (struct basilysgimpleseq_st*)box;
  if (!g || g->discr->object_magic != OBMAG_GIMPLESEQ)
    return NULL;
  return g->val;
}

/* allocate a new boxed basicblock of given DISCR [DISCR_BASICBLOCK if null] &
   content VAL */
basilys_ptr_t basilysgc_new_basicblock (basilysobject_ptr_t discr_p,
					basic_block val);

/* return the content of a boxed gimple */
static inline basic_block
basilys_basicblock_content (basilys_ptr_t box)
{
  struct basilysbasicblock_st* b = (struct basilysbasicblock_st*)box;
  if (!b || b->discr->object_magic != OBMAG_BASICBLOCK)
    return NULL;
  return b->val;
}

/* return the seq of a boxed basicblock */
static inline gimple_seq
basilys_basicblock_gimpleseq(basilys_ptr_t box)
{
  struct basilysbasicblock_st* b = (struct basilysbasicblock_st*)box;
  if (!b || b->discr->object_magic != OBMAG_BASICBLOCK || !b->val)
    return NULL;
  return bb_seq(b->val);
}

/* return the phinodes of a boxed basicblock */
static inline gimple_seq
basilys_basicblock_phinodes(basilys_ptr_t box)
{
  struct basilysbasicblock_st* b = (struct basilysbasicblock_st*)box;
  if (!b || b->discr->object_magic != OBMAG_BASICBLOCK || !b->val)
    return NULL;
  return phi_nodes(b->val);
}

/*************************************************************
 * young generation copying garbage collector 
 *
 * the young generation is managed specifically by an additional
 * copying garbage collector, which copies basilys_ptr_t data into the
 * GGC heap from a young region.  This requires that every local
 * variable is known to our copying basilys GC. For that purpose,
 * locals are copied (or used) inside a chain of callframe_basilys_st
 * structures. Since our copying GC change pointers, every allocation
 * or call may change all the frames. Also stores inside data should
 * be explicitly managed in a store list
 *
 * the young allocation zone is typically of a few megabytes when it
 * is full, a minor garbage collection occur (and possibly a full GGC
 * collection afterwards) which changes all the locals
 *************************************************************/

/* start and end of young allocation zone */
extern void *basilys_startalz;
extern void *basilys_endalz;
/* current allocation pointer aligned */
extern char *basilys_curalz;
/* the store vector grows downward */
extern void **basilys_storalz;
/* list of specials in the allocation zone */
extern struct basilysspecial_st *basilys_newspeclist;
/* list of specials in the heap */
extern struct basilysspecial_st *basilys_oldspeclist;
/* kilowords allocated since last full collection */
extern unsigned long basilys_kilowords_sincefull;
/* number of full & any basilys garbage collections */
extern unsigned long basilys_nb_full_garbcoll;
extern unsigned long basilys_nb_garbcoll;

extern bool basilys_prohibit_garbcoll;

/* extra scanning routine pointer - only for GC experts, and mostly
   useful to possibly scan bison or yacc semantic value stack */
extern void (*basilys_extra_scanrout_p) (void);

/* the basilys copying garbage collector routine - moves all locals on the stack! */
enum
{ BASILYS_MINOR_OR_FULL = 0, BASILYS_NEED_FULL = 1 };
void basilys_garbcoll (size_t wanted, bool needfull);

/* the alignment */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define BASILYS_ALIGN (__alignof__(union basilys_un))
#define BASILYS_LIKELY(P) __builtin_expect((P),1)
#define BASILYS_UNLIKELY(P) __builtin_expect((P),0)
#else
#define BASILYS_ALIGN (2*sizeof(void*))
#define BASILYS_LIKELY(P) (P)
#define BASILYS_UNLIKELY(P) (P)
#endif

static inline bool
basilys_is_young (const void *const p)
{
  return (const char * const) p >= (const char * const) basilys_startalz
    && (const char * const) p < (const char * const) basilys_endalz;
}


#if ENABLE_CHECKING
/* to ease debugging we sometimes want to know when some pointer is
   allocated: set these variables in the debugger */
static void* tracedptr1;
static void* tracedptr2;
#endif

/* the allocator routine allocates a zone of BASESZ with extra GAP */
static inline void *
basilysgc_allocate (size_t basesz, size_t gap)
{
  size_t wanted;
  void *ptr;
  if (basesz < sizeof (struct basilysforward_st))
    basesz = sizeof (struct basilysforward_st);
  if ((basesz % BASILYS_ALIGN) != 0)
    basesz += (BASILYS_ALIGN - (basesz % BASILYS_ALIGN));
  if ((gap % BASILYS_ALIGN) != 0)
    gap += (BASILYS_ALIGN - (gap % BASILYS_ALIGN));
  wanted = basesz + gap;
  gcc_assert (wanted >= sizeof (struct basilysforward_st));
  if (BASILYS_UNLIKELY (basilys_curalz + wanted + 2 * BASILYS_ALIGN
			>= (char *) basilys_storalz))
    basilys_garbcoll (wanted, BASILYS_MINOR_OR_FULL);
  ptr = basilys_curalz;
#if ENABLE_CHECKING
  if (ptr == tracedptr1)
    debugeprintf("allocated tracedptr1 %p", ptr);
  else if (ptr == tracedptr2)
    debugeprintf("allocated tracedptr2 %p", ptr);
#endif
  basilys_curalz += wanted;
  return ptr;
}

/* we need sometimes to reserve some wanted size in the allocation
   zone without actaully using it now; this is needed for the few
   basilys data structures, e.g. basilysstrbuf_st, which have some
   content (e.g. the buffer zone itself bufzn) which should be kept
   young if the datastructure is young, and should become old (ie. GGC
   allocated) when it becomes old */
static inline void
basilysgc_reserve(size_t wanted) 
{
  if (wanted < 100*sizeof(void*) + sizeof(struct basilysforward_st))
    wanted = 100*sizeof(void*) + sizeof(struct basilysforward_st);
  if ((wanted  % BASILYS_ALIGN) != 0)
    wanted += (BASILYS_ALIGN - (wanted % BASILYS_ALIGN));
  if (BASILYS_UNLIKELY (basilys_curalz + wanted + 2 * BASILYS_ALIGN
			>= (char *) basilys_storalz))
    basilys_garbcoll (wanted, BASILYS_MINOR_OR_FULL);
}

/* we need a function to detect failure in reserved allocation; this
   basilys_reserved_allocation_failure function should never be
   called; we do not want to use fatal_error which requires toplev.h
   inclusion; never call this function outside of
   basilys_allocatereserved */
void basilys_reserved_allocation_failure(long siz);

/*  allocates a previously reserved zone of BASESZ with extra GAP;
    this should never trigger the GC, because space was reserved
    earlier */
static inline void *
basilys_allocatereserved (size_t basesz, size_t gap)
{
  size_t wanted;
  void *ptr;
  if (basesz < sizeof (struct basilysforward_st))
    basesz = sizeof (struct basilysforward_st);
  if ((basesz % BASILYS_ALIGN) != 0)
    basesz += (BASILYS_ALIGN - (basesz % BASILYS_ALIGN));
  if ((gap % BASILYS_ALIGN) != 0)
    gap += (BASILYS_ALIGN - (gap % BASILYS_ALIGN));
  wanted = basesz + gap;
  gcc_assert (wanted >= sizeof (struct basilysforward_st));
  if (BASILYS_UNLIKELY (basilys_curalz + wanted + 2 * BASILYS_ALIGN
			>= (char *) basilys_storalz))
    /* this should never happen */
    basilys_reserved_allocation_failure((long) wanted);
  ptr = basilys_curalz;
#if ENABLE_CHECKING
  if (ptr == tracedptr1)
    debugeprintf("allocated tracedptr1 %p", ptr);
  else if (ptr == tracedptr2)
    debugeprintf("allocated tracedptr2 %p", ptr);
#endif
  basilys_curalz += wanted;
  return ptr;
}

/* we maintain a small cache hasharray of touched values - the touched
   cache size should be a small prime */
#define BASILYS_TOUCHED_CACHE_SIZE 17
extern void *basilys_touched_cache[BASILYS_TOUCHED_CACHE_SIZE];
/* the touching routine should be called on every basilys value which
   has been touched (by mutating one of its internal pointers) - it
   may add the touched value to the store "array" and may trigger our
   basilys copying garbage collector */
static inline void
basilysgc_touch (void *touchedptr)
{
  /* we know that this may loose -eg on some 64bits hosts- some
     highend bits of the pointer but we don't care, since the 32
     lowest bits are enough (as hash); we need a double cast to avoid
     a warning */
  unsigned pad = (unsigned) (HOST_WIDE_INT) touchedptr;
  if ((char *) touchedptr >= (char *) basilys_startalz
      && (char *) touchedptr <= (char *) basilys_endalz)
    return;
  pad = pad % (unsigned) BASILYS_TOUCHED_CACHE_SIZE;
  if (basilys_touched_cache[pad] == touchedptr)
    return;
  *basilys_storalz = touchedptr;
  basilys_storalz--;
  basilys_touched_cache[pad] = touchedptr;
  if (BASILYS_UNLIKELY
      ((char *) ((void **) basilys_storalz - 3) <= (char *) basilys_curalz))
    basilys_garbcoll (1024 * sizeof (void *) +
		      ((char *) basilys_endalz - (char *) basilys_storalz),
		      BASILYS_MINOR_OR_FULL);
}

/* we can avoid the hassle of adding a touched pointer to the store
   list if we know that the newly added pointer inside does not point
   into the new allocation zone; TOUCHEDPTR is the mutated value and
   DSTPTR is the newly added pointer insided */
static inline void
basilysgc_touch_dest (void *touchedptr, void *destptr)
{
  /* if we add an old pointer we don't care */
  if (!basilys_is_young(destptr))
    return;
  basilysgc_touch (touchedptr);
}






/* low level map routines */


/***
 * allocation routines that may trigger a garbage collection 
 * (their name starts with basilysgc)
 ***/

/* allocate a boxed long integer (or null if bad DISCR) fillen with NUM */
basilys_ptr_t basilysgc_new_int (basilysobject_ptr_t discr, long num);

static inline long
basilys_get_int (basilys_ptr_t v)
{
  switch (basilys_magic_discr (v))
    {
    case OBMAG_INT:
      return ((struct basilysint_st *) (v))->val;
    case OBMAG_MIXINT:
      return ((struct basilysmixint_st *) (v))->intval;
    case OBMAG_MIXLOC:
      return ((struct basilysmixloc_st *) (v))->intval;
    case OBMAG_OBJECT:
      return ((basilysobject_ptr_t) (v))->obj_num;
    default:
      return 0;
    }
}

static inline bool
basilys_put_int (basilys_ptr_t v, long x)
{
  switch (basilys_magic_discr (v))
    {
    case OBMAG_INT:
      ((struct basilysint_st *) (v))->val = x;
      return TRUE;
    case OBMAG_MIXINT:
      ((struct basilysmixint_st *) (v))->intval = x;
      return TRUE;
    case OBMAG_MIXLOC:
      ((struct basilysmixloc_st *) (v))->intval = x;
      return TRUE;
    case OBMAG_OBJECT:
      if (((basilysobject_ptr_t) (v))->obj_num != 0)
	return FALSE;
      ((basilysobject_ptr_t) (v))->obj_num = (unsigned short) x;
      return TRUE;
    default:
      return FALSE;
    }
}


static inline long
basilys_obj_hash (basilys_ptr_t v)
{
  if (basilys_magic_discr (v) == OBMAG_OBJECT)
    return ((basilysobject_ptr_t) (v))->obj_hash;
  return 0;
}

static inline unsigned long
basilys_obj_serial (basilys_ptr_t v)
{
#if ENABLE_CHECKING
  if (basilys_magic_discr (v) == OBMAG_OBJECT)
    return ((basilysobject_ptr_t) (v))->obj_serial;
#endif
  return 0;
}


static inline long
basilys_obj_len (basilys_ptr_t v)
{
  if (basilys_magic_discr (v) == OBMAG_OBJECT)
    return ((basilysobject_ptr_t) (v))->obj_len;
  return 0;
}

static inline long
basilys_obj_num (basilys_ptr_t v)
{
  if (basilys_magic_discr (v) == OBMAG_OBJECT)
    return ((basilysobject_ptr_t) (v))->obj_num;
  return 0;
}

/* safe integer div & mod */
static inline long
basilys_idiv (long i, long j)
{
  return (j != 0) ? (i / j) : 0;
}
static inline long
basilys_imod (long i, long j)
{
  return (j != 0) ? (i % j) : 0;
}



/* allocate a boxed mixed integer & value) */
basilys_ptr_t
basilysgc_new_mixint (basilysobject_ptr_t discr_p, basilys_ptr_t val_p,
		      long num);
basilys_ptr_t
basilysgc_new_mixloc (basilysobject_ptr_t discr_p, basilys_ptr_t val_p,
		      long num, location_t loc);

static inline basilys_ptr_t
basilys_val_mixint (basilys_ptr_t mix)
{
  struct basilysmixint_st *smix = (struct basilysmixint_st *) mix;
  if (basilys_magic_discr (mix) == OBMAG_MIXINT)
    return smix->ptrval;
  return NULL;
}

static inline long
basilys_num_mixint (basilys_ptr_t mix)
{
  struct basilysmixint_st *smix = (struct basilysmixint_st *) mix;
  if (basilys_magic_discr (mix) == OBMAG_MIXINT)
    return smix->intval;
  return 0;
}

static inline long
basilys_num_mixloc (basilys_ptr_t mix)
{
  struct basilysmixloc_st *smix = (struct basilysmixloc_st *) mix;
  if (basilys_magic_discr (mix) == OBMAG_MIXLOC)
    return smix->intval;
  return 0;
}

static inline basilys_ptr_t
basilys_val_mixloc (basilys_ptr_t mix)
{
  struct basilysmixloc_st *smix = (struct basilysmixloc_st *) mix;
  if (basilys_magic_discr (mix) == OBMAG_MIXLOC)
    return smix->ptrval;
  return NULL;
}


static inline location_t
basilys_location_mixloc (basilys_ptr_t mix)
{
  struct basilysmixloc_st *smix = (struct basilysmixloc_st *) mix;
  if (basilys_magic_discr (mix) == OBMAG_MIXLOC)
    return smix->locval;
  return (location_t)0;
}


/* get (safely) the nth (counting from 0) field of an object */
static inline basilys_ptr_t
basilys_field_object (basilys_ptr_t ob, unsigned off)
{
  if (basilys_magic_discr (ob) == OBMAG_OBJECT)
    {
      basilysobject_ptr_t pob = (basilysobject_ptr_t) ob;
      if (off < pob->obj_len)
	return pob->obj_vartab[off];
    };
  return NULL;
}

/* allocate a new raw object of given KLASS (unchecked) with LEN slots */
basilysobject_ptr_t basilysgc_new_raw_object (basilysobject_ptr_t klass_p,
					      unsigned len);


/* basilys diagnostic routine */
void basilys_error_str(basilys_ptr_t mixloc_p, const char* msg, basilys_ptr_t str_p);
void basilys_warning_str(int opt, basilys_ptr_t mixloc_p, const char* msg, basilys_ptr_t str_p);
void basilys_inform_str(basilys_ptr_t mixloc_p, const char* msg, basilys_ptr_t str_p);

int* basilys_dynobjstruct_fieldoffset_at(const char*fldnam, const char*fil, int lin);
int* basilys_dynobjstruct_classlength_at(const char*clanam, const char* fil, int lin);

#if MELTGCC_DYNAMIC_OBJSTRUCT

static inline basilys_ptr_t
basilys_dynobjstruct_getfield_object_at (basilys_ptr_t ob, unsigned off, const char*fldnam, const char*fil, int lin, int**poff)
{
  if (poff && !*poff) 
    *poff = basilys_dynobjstruct_fieldoffset_at(fldnam, fil, lin);
  if (basilys_magic_discr (ob) == OBMAG_OBJECT)
    {
      basilysobject_ptr_t pob = (basilysobject_ptr_t) ob;
      if (poff && *poff) off = **poff;
      if (off < pob->obj_len)
	return pob->obj_vartab[off];
      fatal_error("checked dynamic field access failed (bad offset %d/%d [%s:%d]) - %s", (int)off, (int)pob->obj_len, fil, lin, fldnam?fldnam:"...");
      return NULL;
    }
  fatal_error("checked dynamic field access failed (not object [%s:%d]) - %s", fil, lin, fldnam?fldnam:"...");
  return NULL;
}

#define basilys_object_get_field_at(Slot,Obj,Off,Fldnam,Fil,Lin) do {	\
  static int *offptr_##Lin;						\
  Slot =								\
    basilys_dynobjstruct_getfield_object_at((basilys_ptr_t)(Obj),	\
					    (Off),Fldnam,Fil,Lin,	\
					    &offptr_##Lin);		\
} while(0)

#define basilys_object_get_field(Slot,Obj,Off,Fldnam) \
  basilys_object_get_field_at(Slot,Obj,Off,Fldnam,__FILE__,__LINE__)

#define basilys_getfield_object(Obj,Off,Fldnam)				\
    basilys_dynobjstruct_getfield_object_at((basilys_ptr_t)(Obj),	\
					    (Off),Fldnam,__FILE__,	\
                                            __LINE__,			\
					    (int**)0)

static inline void 
basilys_dynobjstruct_putfield_object_at(basilys_ptr_t ob, unsigned off, basilys_ptr_t val, const char*fldnam, const char*fil, int lin, int**poff)
{
  if (poff && !*poff) 
    *poff = basilys_dynobjstruct_fieldoffset_at(fldnam, fil, lin);
  if (basilys_magic_discr (ob) == OBMAG_OBJECT)
    {
      basilysobject_ptr_t pob = (basilysobject_ptr_t) ob;
      if (poff && *poff) off = **poff;
      if (off < pob->obj_len) {
	pob->obj_vartab[off] = val;
	return;
      }
      fatal_error("checked dynamic field put failed (bad offset %d/%d [%s:%d]) - %s", (int)off, (int)pob->obj_len, fil, lin, fldnam?fldnam:"...");
    }
  fatal_error("checked dynamic field put failed (not object [%s:%d]) - %s", fil, lin, fldnam?fldnam:"...");
}

#define basilys_putfield_object_at(Obj,Off,Val,Fldnam,Fil,Lin) do {	\
  static int* ptroff_##Lin;						\
  basilys_dynobjstruct_putfield_object_at((basilys_ptr_t)(Obj),		\
					  (Off),			\
					  (basilys_ptr_t)(Val),Fldnam,  \
					  Fil,Lin,			\
					  &ptroff_##Lin); } while(0)
#define basilys_putfield_object(Obj,Off,Val,Fldnam) \
  basilys_putfield_object_at(Obj,Off,Val,Fldnam,__FILE__,__LINE__)


static inline basilys_ptr_t
basilys_dynobjstruct_make_raw_object(basilys_ptr_t klas, int len, 
				     const char*clanam, const char*fil, int lin, int**pptr) {
  if (pptr && !*pptr) 
    *pptr = basilys_dynobjstruct_classlength_at(clanam,fil,lin);
  if (pptr && *pptr) 
    len = **pptr;
  return (basilys_ptr_t)basilysgc_new_raw_object((basilysobject_ptr_t)klas,len);
}

#define basilys_raw_object_create_at(Newobj,Klas,Len,Clanam,Fil,Lin) do { \
  static int* ptrlen_##Lin;						\
  Newobj =								\
    basilys_dynobjstruct_make_raw_object((Klas),(Len),			\
					 Clanam,Fil,Lin,		\
					 &ptrlen_##Lin); } while(0)

#define basilys_raw_object_create(Newobj,Klas,Len,Clanam) \
  basilys_raw_object_create_at(Newobj,Klas,Len,Clanam,__FILE__,__LINE__)

#define basilys_make_raw_object(Klas,Len,Clanam)			\
    basilys_dynobjstruct_make_raw_object((Klas),(Len),			\
					 Clanam, __FILE__, __LINE__,	\
					 (int**)0)

#elif ENABLE_CHECKING
static inline basilys_ptr_t
basilys_getfield_object_at (basilys_ptr_t ob, unsigned off, const char*msg, const char*fil, int lin)
{
  if (basilys_magic_discr (ob) == OBMAG_OBJECT)
    {
      basilysobject_ptr_t pob = (basilysobject_ptr_t) ob;
      if (off < pob->obj_len)
	return pob->obj_vartab[off];
      fatal_error("checked field access failed (bad offset %d/%d [%s:%d]) - %s", (int)off, (int)pob->obj_len, fil, lin, msg?msg:"...");
      return NULL;
    }
  fatal_error("checked field access failed (not object [%s:%d]) - %s", fil, lin, msg?msg:"...");
  return NULL;
}

static inline void
basilys_putfield_object_at(basilys_ptr_t ob, unsigned off, basilys_ptr_t val, const char*msg, const char*fil, int lin)
{
  if (basilys_magic_discr (ob) == OBMAG_OBJECT)
    {
      basilysobject_ptr_t pob = (basilysobject_ptr_t) ob;
      if (off < pob->obj_len) {
	pob->obj_vartab[off] = val;
	return;
      }
      fatal_error("checked field put failed (bad offset %d/%d [%s:%d]) - %s", (int)off, (int)pob->obj_len, fil, lin, msg?msg:"...");
    }
  fatal_error("checked field put failed (not object [%s:%d]) - %s", fil, lin, msg?msg:"...");
}

static inline basilys_ptr_t
basilys_make_raw_object(basilys_ptr_t klas, int len, const char*clanam) {
  gcc_assert(clanam != NULL);
  return (basilys_ptr_t)basilysgc_new_raw_object((basilysobject_ptr_t)klas,len);
}

#define basilys_raw_object_create(Newobj,Klas,Len,Clanam) do {	\
  Newobj = basilys_make_raw_object(Klas,Len,Clanam); } while(0)
#define basilys_getfield_object(Obj,Off,Fldnam) basilys_getfield_object_at((basilys_ptr_t)(Obj),(Off),(Fldnam),__FILE__,__LINE__)
#define basilys_object_get_field(Slot,Obj,Off,Fldnam) do {	\
  Slot = basilys_getfield_object(Obj,Off,Fldnam);} while(0)
#define basilys_putfield_object(Obj,Off,Val,Fldnam) basilys_putfield_object_at((basilys_ptr_t)(Obj),(Off),(basilys_ptr_t)(Val),(Fldnam),__FILE__,__LINE__)
#else
#define basilys_getfield_object(Obj,Off,Fldnam) (((basilysobject_ptr_t)(Obj))->obj_vartab[Off])
#define basilys_object_get_field(Slot,Obj,Off,Fldnam) do {	\
  Slot = basilys_getfield_object(Obj,Off,Fldnam);} while(0)
#define basilys_putfield_object(Obj,Off,Val,Fldnam) do {		\
((basilysobject_ptr_t)(Obj))->obj_vartab[Off] = (basilys_ptr_t)(Val);	\
}while(0)
#define basilys_make_raw_object(Klas,Len) ((basilys_ptr_t)basilysgc_new_raw_object(Klas,Len))
#define basilys_raw_object_create(Newobj,Klas,Len,Clanam) do { 
  Newobj = basilys_make_raw_object(Klas,Len,Clanam); } while(0)
#endif

/* get (safely) the length of an object */
static inline int
basilys_object_length (basilys_ptr_t ob)
{
  if (basilys_magic_discr (ob) == OBMAG_OBJECT)
    {
      basilysobject_ptr_t pob = (basilysobject_ptr_t) ob;
      return pob->obj_len;
    }
  return 0;
}

/* get safely the nth field of an object or NULL */
static inline basilys_ptr_t
basilys_object_nth_field(basilys_ptr_t ob, int rk) 
{
  if (basilys_magic_discr (ob) == OBMAG_OBJECT)
    {
      basilysobject_ptr_t pob = (basilysobject_ptr_t) ob;
      if (rk<0) 
	rk += pob->obj_len;
      if (rk>=0 && rk<pob->obj_len) 
	return (basilys_ptr_t)(pob->obj_vartab[rk]);
    }
  return NULL;
}

/* allocate a new string (or null if bad DISCR or null STR) initialized from
   _static_ (non gc-ed) memory STR  */
basilys_ptr_t basilysgc_new_string (basilysobject_ptr_t discr,
				    const char *str);

/* allocate a new string (or null if bad DISCR or null STR) initialized from
   a memory STR which is temporarily duplicated (so can be in gc-ed) */
basilys_ptr_t basilysgc_new_stringdup (basilysobject_ptr_t discr,
				       const char *str);

/* get the naked nasename of a path, ie from "/foo/bar.gyz" return "bar"; argument is duplicated */
basilys_ptr_t basilysgc_new_string_nakedbasename (basilysobject_ptr_t discr,
						  const char *str);

static inline const char *
basilys_string_str (basilys_ptr_t v)
{
  if (basilys_magic_discr (v) == OBMAG_STRING)
    return ((struct basilysstring_st *) v)->val;
  return 0;
}

static inline int
basilys_string_length (basilys_ptr_t v)
{
  if (basilys_magic_discr (v) == OBMAG_STRING)
    return strlen(((struct basilysstring_st *) v)->val);
  return 0;
}

static inline bool
basilys_string_same (basilys_ptr_t v1, basilys_ptr_t v2)
{
  if (basilys_magic_discr (v1) == OBMAG_STRING
      && basilys_magic_discr (v2) == OBMAG_STRING)
    {
      return 0 == strcmp (((struct basilysstring_st *) v1)->val,
			  ((struct basilysstring_st *) v2)->val);
    }
  return 0;
}

static inline bool
basilys_string_less (basilys_ptr_t v1, basilys_ptr_t v2)
{
  if (basilys_magic_discr (v1) == OBMAG_STRING
      && basilys_magic_discr (v2) == OBMAG_STRING)
    {
      return strcmp (((struct basilysstring_st *) v1)->val,
		     ((struct basilysstring_st *) v2)->val) < 0;
    }
  return 0;
}

static inline bool
basilys_is_string_const (basilys_ptr_t v, const char *s) {
  if (s && basilys_magic_discr (v) == OBMAG_STRING)
    return 0 == strcmp (((struct basilysstring_st *) v)->val, s);
  return 0;
}

static inline const char *
basilys_strbuf_str (basilys_ptr_t v)
{
  if (basilys_magic_discr (v) == OBMAG_STRBUF)
    {
      struct basilysstrbuf_st *sb = (struct basilysstrbuf_st*) v;
      if (sb->bufend >= sb->bufstart)
	return sb->bufzn + sb->bufstart;
    }
  return 0;
}

static inline int
basilys_strbuf_usedlength(basilys_ptr_t v)
{
  if (basilys_magic_discr (v) == OBMAG_STRBUF)
    {
      struct basilysstrbuf_st *sb = (struct basilysstrbuf_st *) v;
      if (sb->bufend >= sb->bufstart)
	return sb->bufend - sb->bufstart;
    }
  return 0;
}


/* allocate a pair of given head and tail */
basilys_ptr_t basilysgc_new_pair (basilysobject_ptr_t discr,
				  void *head, void *tail);

/* change the head of a pair */
void basilysgc_pair_set_head(basilys_ptr_t pair, void* head);

/* allocate a new multiple of given DISCR & length LEN */
basilys_ptr_t basilysgc_new_multiple (basilysobject_ptr_t discr_p,
				      unsigned len);

/* allocate a multiple of arity 1 */
basilys_ptr_t basilysgc_new_mult1 (basilysobject_ptr_t discr_p,
				   basilys_ptr_t v0_p);
/* allocate a multiple of arity 2 */
basilys_ptr_t basilysgc_new_mult2 (basilysobject_ptr_t discr_p,
				   basilys_ptr_t v0_p, basilys_ptr_t v1_p);
/* allocate a multiple of arity 3 */
basilys_ptr_t basilysgc_new_mult3 (basilysobject_ptr_t discr_p,
				   basilys_ptr_t v0_p, basilys_ptr_t v1_p,
				   basilys_ptr_t v2_p);
/* allocate a multiple of arity 4 */
basilys_ptr_t basilysgc_new_mult4 (basilysobject_ptr_t discr_p,
				   basilys_ptr_t v0_p, basilys_ptr_t v1_p,
				   basilys_ptr_t v2_p, basilys_ptr_t v3_p);
/* allocate a multiple of arity 5 */
basilys_ptr_t basilysgc_new_mult5 (basilysobject_ptr_t discr_p,
				   basilys_ptr_t v0_p, basilys_ptr_t v1_p,
				   basilys_ptr_t v2_p, basilys_ptr_t v3_p,
				   basilys_ptr_t v4_p);
/* allocate a multiple of arity 6 */
basilys_ptr_t basilysgc_new_mult6 (basilysobject_ptr_t discr_p,
				   basilys_ptr_t v0_p, basilys_ptr_t v1_p,
				   basilys_ptr_t v2_p, basilys_ptr_t v3_p,
				   basilys_ptr_t v4_p, basilys_ptr_t v5_p);
/* allocate a multiple of arity 7 */
basilys_ptr_t basilysgc_new_mult7 (basilysobject_ptr_t discr_p,
				   basilys_ptr_t v0_p, basilys_ptr_t v1_p,
				   basilys_ptr_t v2_p, basilys_ptr_t v3_p,
				   basilys_ptr_t v4_p, basilys_ptr_t v5_p,
				   basilys_ptr_t v6_p);


/* allocate a new (empty) list */
basilys_ptr_t basilysgc_new_list (basilysobject_ptr_t discr_p);
/* append to the tail of a list */
void basilysgc_append_list (basilys_ptr_t list_p, basilys_ptr_t val_p);
/* prepend to the head of a list */
void basilysgc_prepend_list (basilys_ptr_t list_p, basilys_ptr_t val_p);
/* pop from head of list (and remove) */
basilys_ptr_t basilysgc_popfirst_list (basilys_ptr_t list_p);

/* return the length of a list, 0 for nil, or -1 iff non list */
int basilys_list_length (basilys_ptr_t list_p);

/* allocate e new empty mapobjects */
basilys_ptr_t basilysgc_new_mapobjects (basilysobject_ptr_t discr_p,
					unsigned len);

/* put into a mapobjects */
void basilysgc_put_mapobjects (basilysmapobjects_ptr_t mapobject_p,
			       basilysobject_ptr_t attrobject_p,
			       basilys_ptr_t valu_p);

/* get from a mapobject */
basilys_ptr_t basilys_get_mapobjects (basilysmapobjects_ptr_t mapobject_p,
				      basilysobject_ptr_t attrobject_p);

/* remove from a mapobject (return the removed value) */
basilys_ptr_t basilysgc_remove_mapobjects (basilysmapobjects_ptr_t
					   mapobject_p,
					   basilysobject_ptr_t attrobject_p);

static inline int
basilys_size_mapobjects (basilysmapobjects_ptr_t mapobject_p)
{
  if (!mapobject_p || mapobject_p->discr->obj_num != OBMAG_MAPOBJECTS)
    return 0;
  return basilys_primtab[mapobject_p->lenix];
}

static inline unsigned
basilys_count_mapobjects (basilysmapobjects_ptr_t mapobject_p)
{
  if (!mapobject_p || mapobject_p->discr->obj_num != OBMAG_MAPOBJECTS)
    return 0;
  return mapobject_p->count;
}

static inline basilysobject_ptr_t
basilys_nthattr_mapobjects (basilysmapobjects_ptr_t mapobject_p, int ix)
{
  basilysobject_ptr_t at = 0;
  if (!mapobject_p || mapobject_p->discr->obj_num != OBMAG_MAPOBJECTS)
    return 0;
  if (ix < 0 || ix >= basilys_primtab[mapobject_p->lenix])
    return 0;
  at = mapobject_p->entab[ix].e_at;
  if ((void *) at == (void *) HTAB_DELETED_ENTRY)
    return 0;
  return at;
}

static inline basilys_ptr_t
basilys_nthval_mapobjects (basilysmapobjects_ptr_t mapobject_p, int ix)
{
  basilysobject_ptr_t at = 0;
  if (!mapobject_p || mapobject_p->discr->obj_num != OBMAG_MAPOBJECTS)
    return 0;
  if (ix < 0 || ix >= basilys_primtab[mapobject_p->lenix])
    return 0;
  at = mapobject_p->entab[ix].e_at;
  if ((void *) at == (void *) HTAB_DELETED_ENTRY)
    return 0;
  return mapobject_p->entab[ix].e_va;
}

/* allocate a new empty mapstrings */
basilys_ptr_t basilysgc_new_mapstrings (basilysobject_ptr_t discr_p,
					unsigned len);

/* put into a mapstrings, the string is copied so can be in the gc-ed heap */
void basilysgc_put_mapstrings (struct basilysmapstrings_st *mapstring_p,
			       const char *str, basilys_ptr_t valu_p);

/* get from a mapstring */
basilys_ptr_t basilys_get_mapstrings (struct basilysmapstrings_st
				      *mapstring_p, const char *attr);


/* remove from a mapstring (return the removed value) */
basilys_ptr_t basilysgc_remove_mapstrings (struct basilysmapstrings_st
					   *mapstring_p, const char *str);

static inline int
basilys_size_mapstrings (struct basilysmapstrings_st *mapstring_p)
{
  if (!mapstring_p || mapstring_p->discr->obj_num != OBMAG_MAPSTRINGS)
    return 0;
  return basilys_primtab[mapstring_p->lenix];
}

static inline unsigned
basilys_count_mapstrings (struct basilysmapstrings_st *mapstring_p)
{
  if (!mapstring_p || mapstring_p->discr->obj_num != OBMAG_MAPSTRINGS)
    return 0;
  return mapstring_p->count;
}

static inline const char *
basilys_nthattrraw_mapstrings (struct basilysmapstrings_st *mapstring_p,
			       int ix)
{
  const char *at = 0;
  if (!mapstring_p || mapstring_p->discr->obj_num != OBMAG_MAPSTRINGS)
    return 0;
  if (ix < 0 || ix >= basilys_primtab[mapstring_p->lenix])
    return 0;
  at = mapstring_p->entab[ix].e_at;
  if ((const void *) at == (const void *) HTAB_DELETED_ENTRY)
    return 0;
  return at;
}

static inline basilys_ptr_t
basilys_nthval_mapstrings (struct basilysmapstrings_st *mapstring_p, int ix)
{
  const char *at = 0;
  if (!mapstring_p || mapstring_p->discr->obj_num != OBMAG_MAPSTRINGS)
    return 0;
  if (ix < 0 || ix >= basilys_primtab[mapstring_p->lenix])
    return 0;
  at = mapstring_p->entab[ix].e_at;
  if ((const void *) at == (const void *) HTAB_DELETED_ENTRY)
    return 0;
  return mapstring_p->entab[ix].e_va;
}

/* allocate a new routine object of given DISCR and of length LEN,
   with a DESCR-iptive string a a PROC-edure */
basilysroutine_ptr_t basilysgc_new_routine (basilysobject_ptr_t discr_p,
					    unsigned len, const char *descr,
					    basilysroutfun_t * proc);


void basilysgc_set_routine_data(basilys_ptr_t rout_p, basilys_ptr_t data_p);

static inline basilys_ptr_t 
basilys_routine_data(basilys_ptr_t rout)
{
  if (rout && ((basilysroutine_ptr_t) rout)->discr->obj_num == OBMAG_ROUTINE)
    return ((basilysroutine_ptr_t) rout)->routdata;
  return NULL;
}

static inline char *
basilys_routine_descrstr (basilys_ptr_t rout)
{
  if (rout && ((basilysroutine_ptr_t) rout)->discr->obj_num == OBMAG_ROUTINE)
    return ((basilysroutine_ptr_t) rout)->routdescr;
  return (char *) 0;
}

static inline int
basilys_routine_size (basilys_ptr_t rout)
{
  if (rout && ((basilysroutine_ptr_t) rout)->discr->obj_num == OBMAG_ROUTINE)
    return ((basilysroutine_ptr_t) rout)->nbval;
  return 0;
}

static inline basilys_ptr_t
basilys_routine_nth (basilys_ptr_t rout, int ix)
{
  if (rout && ((basilysroutine_ptr_t) rout)->discr->obj_num == OBMAG_ROUTINE)
    if (ix >= 0 && ix < (int) ((basilysroutine_ptr_t) rout)->nbval)
      return ((basilysroutine_ptr_t) rout)->tabval[ix];
  return 0;
}

/*********/
/* allocate a new closure of given DISCR with a given ROUT,  and of length LEN
 */
basilysclosure_ptr_t basilysgc_new_closure (basilysobject_ptr_t discr_p,
					    basilysroutine_ptr_t rout_p,
					    unsigned len);

static inline int
basilys_closure_size (basilys_ptr_t clo)
{
  if (clo && ((basilysclosure_ptr_t) clo)->discr->obj_num == OBMAG_CLOSURE)
    return ((basilysclosure_ptr_t) clo)->nbval;
  return 0;
}

static inline basilys_ptr_t
basilys_closure_routine (basilys_ptr_t clo)
{
  if (clo && ((basilysclosure_ptr_t) clo)->discr->obj_num == OBMAG_CLOSURE)
    return (basilys_ptr_t) (((basilysclosure_ptr_t) clo)->rout);
  return 0;
}

static inline basilys_ptr_t
basilys_closure_nth (basilys_ptr_t clo, int ix)
{
  if (clo && ((basilysclosure_ptr_t) clo)->discr->obj_num == OBMAG_CLOSURE
      && ix >= 0 && ix < (int) (((basilysclosure_ptr_t) clo)->nbval))
    return (basilys_ptr_t) (((basilysclosure_ptr_t) clo)->tabval[ix]);
  return 0;
}


/***** list and pairs accessors ****/
/* safe pair head & tail accessors */
static inline basilys_ptr_t
basilys_pair_head (basilys_ptr_t pair)
{
  if (pair && ((struct basilyspair_st *) pair)->discr->obj_num == OBMAG_PAIR)
    return ((struct basilyspair_st *) pair)->hd;
  return 0;
}

static inline basilys_ptr_t
basilys_pair_tail (basilys_ptr_t pair)
{
  if (pair && ((struct basilyspair_st *) pair)->discr->obj_num == OBMAG_PAIR)
    return (basilys_ptr_t) (((struct basilyspair_st *) pair)->tl);
  return 0;
}

/* compute the length of a pairlist */
static inline long
basilys_pair_listlength (basilys_ptr_t pair)
{
  long l = 0;
  while (pair
	 && ((struct basilyspair_st *) pair)->discr->obj_num == OBMAG_PAIR)
    {
      l++;
      pair = (basilys_ptr_t) (((struct basilyspair_st *) pair)->tl);
    };
  return l;
}

static inline basilys_ptr_t
basilys_list_first (basilys_ptr_t lis)
{
  if (lis && ((struct basilyslist_st *) lis)->discr->obj_num == OBMAG_LIST)
    return (basilys_ptr_t) (((struct basilyslist_st *) lis)->first);
  return NULL;
}

static inline basilys_ptr_t
basilys_list_last (basilys_ptr_t lis)
{
  if (lis && ((struct basilyslist_st *) lis)->discr->obj_num == OBMAG_LIST)
    return (basilys_ptr_t) (((struct basilyslist_st *) lis)->last);
  return NULL;
}



/***** STRBUF ie string buffers *****/

/* allocate a new strbuf of given DISCR with initial content STR */
struct basilysstrbuf_st *basilysgc_new_strbuf (basilysobject_ptr_t discr_p,
					       const char *str);

/* add into STRBUF the static string STR (which is not in the basilys heap) */
void basilysgc_add_strbuf_raw (struct basilysstrbuf_st *strbuf_p,
			       const char *str);

/* add safely into STRBUF the string STR (which is first copied, so
   can be in the basilys heap) */
void basilysgc_add_strbuf (struct basilysstrbuf_st *strbuf_p,
			   const char *str);
/* add safely into STRBUF the string STR encoded as a C string with
   backslash escapes */
void basilysgc_add_strbuf_cstr (struct basilysstrbuf_st *strbuf_p,
				const char *str);
/* add safely into STRBUF the string STR encoded as the interior of a
   C comment with slash star and star slash replaced by slash plus and
   plus slash */
void basilysgc_add_strbuf_ccomment (struct basilysstrbuf_st *strbuf_p,
				    const char *str);

/* add safely into STRBUF the string STR (which is copied at first)
   encoded as a C identifier; ie non-alphanum begine encoded as an
   underscore */
void basilysgc_add_strbuf_cident (struct basilysstrbuf_st *strbuf_p,
				  const char *str);

/* add safely into STRBUF the initial prefix of string STR (which is
   copied at first), with a length of at most PREFLEN encoded as a C
   identifier; ie non-alphanum begine encoded as an underscore */
void
basilysgc_add_strbuf_cidentprefix (struct basilysstrbuf_st
				   *strbuf_p, const char *str, int preflen);

/* add safely into STRBUF the hex encoded number L */
void basilysgc_add_strbuf_hex (struct basilysstrbuf_st *strbuf_p,
			       unsigned long l);
/* add safely into STRBUF the decimal encoded number L */
void basilysgc_add_strbuf_dec (struct basilysstrbuf_st *strbuf_p, long l);

/* add safely into STRBUF a printf like stuff with FMT */
void
basilysgc_strbuf_printf (struct basilysstrbuf_st *strbuf_p, const char *fmt,
			 ...) ATTRIBUTE_PRINTF (2, 3);

/* add safely into STRBUF either a space or an indented newline if the current line is bigger than the threshold */
void basilysgc_strbuf_add_indent (struct basilysstrbuf_st *strbuf_p,
				  int indeptn, int linethresh);

/* pretty print into an sbuf a gimple */
void basilysgc_ppstrbuf_gimple(basilys_ptr_t sbuf_p, int indentsp, gimple gstmt);

/* pretty print into an sbuf a gimple seq */
void basilysgc_ppstrbuf_gimple_seq(basilys_ptr_t sbuf_p, int indentsp, gimple_seq gseq);

/* pretty print into an sbuf a tree */
void basilysgc_ppstrbuf_tree(basilys_ptr_t sbuf_p, int indentsp, tree tr);

/* pretty print into an sbuf a basic_block */
void basilysgc_ppstrbuf_basicblock(basilys_ptr_t sbuf_p, int indentsp, basic_block bb);


/**************************** misc *****************************/
/* a random generator */
long basilys_lrand (void);

static inline unsigned basilys_nonzerohash (void)
{
  unsigned h;
  do
    {
      h = basilys_lrand () & BASILYS_MAXHASH;
    }
  while (h == 0);
  return h;
}

/* initialize all - don't do anything when called more than once */
void basilys_initialize (void);
/* finalize all */
void basilys_finalize (void);

/* find a symbol in all the loaded modules */
void* basilys_dlsym_all(const char*nam);

/* returns malloc-ed path inside a temporary directory, with a given basename  */
char* basilys_tempdir_path(const char* basnam);

/* compile (as a dynamically loadable module) some (usually generated)
   C code and dynamically load it; the C code should contain a
   function named start_module_basilys; that function is called with
   the given modata and returns the module */
basilys_ptr_t
basilysgc_compile_dyn (basilys_ptr_t modata_p, const char *srcfile);

/* load a list of modules from a file whose basename MODLISTBASE is
   given without its suffix '.modlis' */
basilys_ptr_t
basilysgc_load_modulelist(basilys_ptr_t modata_p, const char *modlistbase);

/* first_module_basilys is the function start_module_basilys in first-basilys.c */
basilys_ptr_t first_module_basilys (basilys_ptr_t);


/* get (or create) the symbol of a given name, using the INITIAL_SYSTEM_DATA
   global; the NAM string can be in the GC-allocated heap since it is
   copied */
enum
{				/* a meningful enum for the create flag below */
  BASILYS_GET = 0,
  BASILYS_CREATE
};

basilys_ptr_t basilysgc_named_symbol (const char *nam, int create);

/* get (or create) the keyword of a given name (without the colon),
   using the INITIAL_SYSTEM_DATA global; the NAM string can be in the
   GC-allocated heap since it is copied */
basilys_ptr_t basilysgc_named_keyword (const char *nam, int create);

/* intern a symbol, ie add it into the global name map; if the symbol
   is new, return it otherwise return the older homonymous symnol */
basilys_ptr_t basilysgc_intern_symbol (basilys_ptr_t symb);

/* intern a keyword, ie add it into the global name map; if the symbol
   is new, return it otherwise return the older homonymous symnol */
basilys_ptr_t basilysgc_intern_keyword (basilys_ptr_t symb);

/* read a list of sexpressions from a file; if the second argument is non-empty and non-null, it is used for locations; otherwise the basename of the filnam is used */
basilys_ptr_t basilysgc_read_file (const char *filnam, const char* locnam);

/* read a list of sexpressions from a raw string [which should not be
   in the basilys heap] using a raw location name and a location in source */
basilys_ptr_t basilysgc_read_from_rawstring(const char* rawstr, const char* rawlocnam, location_t loch);

/* read a list of sexpressions from a string or strbuf value or named
   object; if the second argument is non-empty and non-null, it is
   used for locations */
basilys_ptr_t basilysgc_read_from_val(basilys_ptr_t strv_p, basilys_ptr_t locnam_p);

/* called from c-common.c in handle_melt_attribute */
void basilys_handle_melt_attribute(tree decl, tree name, const char* attrstr, location_t loch);

/* Use basilys_assert(MESSAGE,EXPR) to test invariants.  The MESSAGE
   should be a constant string displayed when asserted EXPR is
   false */
#if ENABLE_ASSERT_CHECKING
void
basilys_assert_failed (const char *msg, const char *filnam, int lineno,
		       const char *fun)
  ATTRIBUTE_NORETURN;
void
basilys_check_failed (const char *msg, const char *filnam, int lineno,
		      const char *fun);
enum { BASILYS_ANYWHERE=0, BASILYS_NOYOUNG };
void  basilys_check_call_frames_at(int youngflag, const char*msg, const char*filenam, int lineno);
#define basilys_assertmsg(MSG,EXPR)   \
  (!(EXPR)?(basilys_assert_failed((MSG),__FILE__,__LINE__,__FUNCTION__), 0):0)
#define basilys_checkmsg(MSG,EXPR)   \
  (!(EXPR)?(basilys_check_failed((MSG),__FILE__,__LINE__,__FUNCTION__), 0):0)
#define basilys_check_call_frames(YNG,MSG)				\
  ((void)(basilys_check_call_frames_at((YNG),(MSG),__FILE__,__LINE__)))
#else
/* Include EXPR, so that unused variable warnings do not occur.  */
#define basilys_assertmsg(MSG,EXPR) ((void)(0 && (MSG) && (EXPR)))
#define basilys_assert_failed(MSG,FIL,LIN,FUN) ((void)(0 && (MSG)))
#define basilys_checkmsg(MSG,EXPR) ((void)(0 && (MSG) && (EXPR)))
#define basilys_check_failed(MSG,FIL,LIN,FUN) ((void)(0 && (MSG)))
#define basilys_check_call_frames(YNG,MSG) (void)(0)
#endif


/******************* method sending ************************/
basilys_ptr_t basilysgc_send (basilys_ptr_t recv,
			      basilys_ptr_t sel,
			      const char *xargdescr_,
			      union basilysparam_un *xargtab_,
			      const char *xresdescr_,
			      union basilysparam_un *xrestab_);

/**************************** globals **************************/

/* enumeration of predefined global object indexes inside
   basilys_globvec; the firsts are wired predefined, in the sense that
   they are automagically allocated and partly filled before loading
   the basilys file. The others are named, and are expected to be
   created by loading the basilysfile  */
enum basilys_globalix_en
{
  BGLOB__NONE,
  /************************* wired predefined */
  /****@@@ MAYBE SHOULD GO IN A SEPARATE FILE??? ***/
  /***
   * don't forget to fill these objects in warmelt*.bysl file and to
   * define the appropriate global_* primitives near
   * create_normcontext function in warmelt-normal.bysl
   * e.g. (defprimitive global_DISCR_NULLRECV ....)
   ***/
  /* the pseudo discriminant using for sending to null pointers */
  BGLOB_DISCR_NULLRECV,
  /* the initial discriminant of string */
  BGLOB_DISCR_STRING,
  /* the initial discriminant of integer */
  BGLOB_DISCR_INTEGER,
  /* the initial discriminant of lists */
  BGLOB_DISCR_LIST,
  /* the initial discriminant of multiples */
  BGLOB_DISCR_MULTIPLE,
  /* the initial discriminant of boxes */
  BGLOB_DISCR_BOX,
  /* the initial discriminant of mapobjects */
  BGLOB_DISCR_MAPOBJECTS,
  /* the initial discriminant of mapstrings */
  BGLOB_DISCR_MAPSTRINGS,
  /* the initial discriminant of charcode integer */
  BGLOB_DISCR_CHARINTEGER,
  /* the initial discriminant of mixedint */
  BGLOB_DISCR_MIXEDINT,
  /* the discriminant of ancestors or other class sequences */
  BGLOB_DISCR_SEQCLASS,
  /* the discriminant of closures */
  BGLOB_DISCR_CLOSURE,
  /* the discriminant of routine */
  BGLOB_DISCR_ROUTINE,
  /* the discriminant of pairs */
  BGLOB_DISCR_PAIR,
  /* the discriminant of short name strings */
  BGLOB_DISCR_NAMESTRING,
  /* the discriminant of verbatim strings (in primitive expansions) */
  BGLOB_DISCR_VERBATIMSTRING,
  /* discriminant for sequence of fields */
  BGLOB_DISCR_SEQFIELD,
  /* discriminant for dictionnaries (mapobjects) of methods */
  BGLOB_DISCR_METHODMAP,
  /* the class of classes */
  BGLOB_CLASS_CLASS,
  /* the root class */
  BGLOB_CLASS_ROOT,
  /* the proped class */
  BGLOB_CLASS_PROPED,
  /* the named class */
  BGLOB_CLASS_NAMED,
  /* the located class */
  BGLOB_CLASS_LOCATED,
  /* the symbol class */
  BGLOB_CLASS_SYMBOL,
  /* the keyword class */
  BGLOB_CLASS_KEYWORD,
  /* the sexpr class */
  BGLOB_CLASS_SEXPR,
  /* class of fields */
  BGLOB_CLASS_FIELD,
  /* class of message selectors */
  BGLOB_CLASS_SELECTOR,
  /* class of primitives */
  BGLOB_CLASS_PRIMITIVE,
  /* class of formal bindings */
  BGLOB_CLASS_FORMAL_BINDING,
  /* the discr class */
  BGLOB_CLASS_DISCR,
  /* the class of system data */
  BGLOB_CLASS_SYSTEM_DATA,
  /* the class of ctypes */
  BGLOB_CLASS_CTYPE,
  /* the class of environments */
  BGLOB_CLASS_ENVIRONMENT,
  /**** every ctype should be predefined ****/
  /* ctype of longs */
  BGLOB_CTYPE_LONG,
  /* ctype of values */
  BGLOB_CTYPE_VALUE,
  /* ctype of trees */
  BGLOB_CTYPE_TREE,
  /* ctype of gimples */
  BGLOB_CTYPE_GIMPLE,
  /* ctype of gimpleseqs */
  BGLOB_CTYPE_GIMPLESEQ,
  /* ctype of basicblocks */
  BGLOB_CTYPE_BASICBLOCK,
  /* ctype of edges */
  BGLOB_CTYPE_EDGE,
  /* ctype for void */
  BGLOB_CTYPE_VOID,
  /* ctype of constant cstrings */
  BGLOB_CTYPE_CSTRING,
  /* the initial system data */
  BGLOB_INITIAL_SYSTEM_DATA,
  /* the atom for true */
  BGLOB_ATOM_TRUE,
  /* the class of containers */
  BGLOB_CLASS_CONTAINER,
  /* the class of basilys GCC compiler passes */
  BGLOB_CLASS_GCC_PASS,
  /* the class of C iterators */
  BGLOB_CLASS_CITERATOR,
  /* the class of C matchers [in patterns] */
  BGLOB_CLASS_CMATCHER,
  /* the initial discriminant of mixedloc */
  BGLOB_DISCR_MIXEDLOC,
  /* the initial discriminant of tree-s */
  BGLOB_DISCR_TREE,
  /* the initial discriminant of gimple-s */
  BGLOB_DISCR_GIMPLE,
  /* the initial discriminant of gimpleseq-s */
  BGLOB_DISCR_GIMPLESEQ,
  /* the initial discriminant of edge-s */
  BGLOB_DISCR_EDGE,
  /* the initial discriminant of basicblock-s */
  BGLOB_DISCR_BASICBLOCK,
  /* the initial discriminant of tree map-s */
  BGLOB_DISCR_MAPTREES,
  /* the initial discriminant of gimple map-s */
  BGLOB_DISCR_MAPGIMPLES,
  /* the initial discriminant of gimpleseq map-s */
  BGLOB_DISCR_MAPGIMPLESEQS,
  /* the initial discriminant of edge map-s */
  BGLOB_DISCR_MAPEDGES,
  /* the initial discriminant of basicblock map-s */
  BGLOB_DISCR_MAPBASICBLOCKS,
  /**************************** placeholder for last wired */
  BGLOB__LASTWIRED,
  BGLOB___SPARE1,
  BGLOB___SPARE2,
  BGLOB___SPARE3,
  BGLOB___SPARE4,
  /*****/
  BGLOB__LASTGLOB
};


/* *INDENT-OFF* */

/* the array of global values */
extern GTY (()) basilys_ptr_t basilys_globarr[BGLOB__LASTGLOB];

/* *INDENT-ON* */

/* fields inside container */
enum
{
  FCONTAINER_VALUE = 0,
  FCONTAINER__LAST
};
    
/* fields inside every proped object */
enum
{
  FPROPED_PROP = 0,
  FPROPED__LAST
};
/* fields inside every named object */
enum
{
  FNAMED_NAME = FPROPED__LAST,
  FNAMED__LAST
};

/* fields inside every discriminant */
enum
{
  FDISCR_METHODICT = FNAMED__LAST,	/* a mapobjects for method mapping
					   selectors to closures */
  FDISCR_SENDER,		/* the closure doing the send if a
				   selector is not in the method
				   dict */
  FDISCR_SUPER,			/* the "superclass" or "parent discrim" */
  FDISCR__LAST
};

/* fields inside every class */
enum
{
  FCLASS_ANCESTORS = FDISCR__LAST,	/* a multiple for the class ancestors
					   (first being the CLASS:ROOT last
					   being the immediate superclass) */
  FCLASS_FIELDS,		/* a multiple for the class fields */
  FCLASS_OBJNUMDESCR,		/* a description of the objnum */
  FCLASS_DATA,			/* class variables */
  FCLASS__LAST
};


/* fields inside each symbol */
enum
{
  FSYMB_DATA = FNAMED__LAST,
  FSYMB__LAST
};

/* fields inside a source expression (sexpr) */
enum
{
  FSEXPR_LOCATION = FPROPED__LAST,
  FSEXPR_CONTENTS,		/* the contents of the sexpression (as a list) */
  FSEXPR__LAST
};

/* fields inside the system data - keep in sync with the
   class_system_data definition in MELT file warmelt-first.bysl */
enum
{
  FSYSDAT_CMD_FUNDICT = FNAMED__LAST,	/* the stringdict of commands */
  FSYSDAT_BOX_FRESH_ENV,			/* closure to make a fresh environment box */
  FSYSDAT_VALUE_EXPORTER,	       /* closure to export a value */
  FSYSDAT_MACRO_EXPORTER,	       /* closure to export a macro */
  FSYSDAT_SYMBOLDICT, 	       /* stringmap for symbols */
  FSYSDAT_KEYWDICT, 		       /* stringmap for keywords */
  FSYSDAT_ADDSYMBOL, 	       /* closure to add a symbol of given name */
  FSYSDAT_ADDKEYW,		/* closure to add a keyword of given name */
  FSYSDAT_INTERNSYMBOL,	       /* closure to intern a symbol */
  FSYSDAT_INTERNKEYW,		/* closure to intern a keyword */
  FSYSDAT_VALUE_IMPORTER,	/* closure to import a value */
  FSYSDAT_PASS_DICT,		/* dictionnary of passes */
  FSYSDAT_EXIT_FINALIZER,	/* closure to call at exit */
  FSYSDAT_MELTATTR_DEFINER,	/* closure for melt attributes */
  FSYSDAT_PATMACRO_EXPORTER,    /* closure to export patmacro */
  FSYSDAT__LAST
};


/* fields inside GCC passes */
enum {
  FGCCPASS_GATE = FNAMED__LAST,	/* the fate closure */
  FGCCPASS_EXEC,		/* the execute closure */
  FGCCPASS_DATA,		/* extra data */
  FGCCPASS__LAST
};

/* BASILYSG(Foo) is the global of index BGLOB_Foo */
#define BASILYSG(Glob) basilys_globarr[BGLOB_##Glob]
#define BASILYSGOB(Glob) ((basilysobject_ptr_t)(BASILYSG(Glob)))

#define BASILYSGIX(Tab,Glob) Tab[BGLOB_##Glob]

/* return the discriminant or class itself */
static inline basilysobject_ptr_t
basilys_discr (basilys_ptr_t p)
{
  if (!p)
    return BASILYSGOB(DISCR_NULLRECV);
  return p->u_discr;
}

bool basilys_is_subclass_of (basilysobject_ptr_t subclass_p,
			     basilysobject_ptr_t superclass_p);

static inline bool
basilys_is_instance_of (basilys_ptr_t inst_p, basilys_ptr_t class_p)
{
  unsigned mag_class = 0;
  unsigned mag_inst = 0;
  if (!inst_p)
    return FALSE;
  if (!class_p)
    return FALSE;
  gcc_assert(class_p->u_discr != NULL);
  gcc_assert(inst_p->u_discr != NULL);
  mag_class = class_p->u_discr->obj_num;
  mag_inst = inst_p->u_discr->obj_num;
  if (mag_class != OBMAG_OBJECT || !mag_inst)
    return FALSE;
  if (((basilysobject_ptr_t) inst_p)->obj_class ==
      (basilysobject_ptr_t) class_p)
    return TRUE;
  if (mag_inst != ((basilysobject_ptr_t) class_p)->object_magic)
    return FALSE;
  if (mag_inst == OBMAG_OBJECT)
    return basilys_is_subclass_of (((basilysobject_ptr_t) inst_p)->obj_class,
				   ((basilysobject_ptr_t) class_p));
  /* the instance is not an object but something else and it has the
     good magic */
  return TRUE;
}

/***
 * CALL FRAMES 
 ***/


/* call frames for our copying garbage collector cannot be GTY-ed
   because they are inside the C call stack */
struct callframe_basilys_st
{
  unsigned nbvar;
#if ENABLE_CHECKING
  const char* flocs;
#endif
  struct basilysclosure_st *clos;
  struct excepth_basilys_st *exh;	/* for our exceptions - not implemented yet */
  struct callframe_basilys_st *prev;
  basilys_ptr_t varptr[FLEXIBLE_DIM];
};

/* maximal number of local variables per frame */
#define BASILYS_MAXNBLOCALVAR 16384

/* the topmost call frame */
extern struct callframe_basilys_st *basilys_topframe;

static inline int basilys_curframdepth(void) {
  int cnt = 0;
  struct callframe_basilys_st* fr = basilys_topframe;
  for (;fr;fr=fr->prev) cnt++;
  return cnt;
}

#if 0
/* the jmpbuf for our catch & throw */
extern jmp_buf *basilys_jmpbuf;
extern basilys_ptr_t basilys_jmpval;
#endif

/* declare the current callframe */
#if ENABLE_CHECKING
#define BASILYS_DECLFRAME(NBVAR) struct {	\
  unsigned nbvar;				\
  const char* flocs;                            \
  struct basilysclosure_st* clos;		\
  struct excepth_basilys_st* exh;               \
  struct callframe_basilys_st* prev;		\
  void*  /* a basilys_ptr_t */ varptr[NBVAR];	\
} curfram__
/* initialize the current callframe and link it at top */
#define BASILYS_INITFRAME_AT(NBVAR,CLOS,FIL,LIN) do {			\
  static char locbuf_##LIN[64];						\
  if (!locbuf_##LIN[0])							\
    snprintf(locbuf_##LIN, sizeof(locbuf_##LIN)-1, "%s:%d",		\
	     basename(FIL), (int)LIN);					\
  memset(&curfram__, 0, sizeof(curfram__));				\
  curfram__.nbvar = (NBVAR);						\
  curfram__.flocs = locbuf_##LIN;					\
  curfram__.prev = (struct callframe_basilys_st*) basilys_topframe;	\
  curfram__.clos = (CLOS);						\
  basilys_topframe = ((struct callframe_basilys_st*)&curfram__);	\
} while(0)
#define BASILYS_INITFRAME(NBVAR,CLOS) BASILYS_INITFRAME_AT(NBVAR,CLOS,__FILE__,__LINE__)
#define BASILYS_LOCATION(LOCS) do{curfram__.flocs= LOCS;}while(0)

#define BASILYS_LOCATION_HERE_AT(FIL,LIN,MSG) do {			\
  static char locbuf_##LIN[72];						\
  if (!locbuf_##LIN[0])							\
    snprintf(locbuf_##LIN, sizeof(locbuf_##LIN)-1, "%s:%d <%s>",	\
	     basename(FIL), (int)LIN, MSG);				\
  curfram__.flocs =  locbuf_##LIN;					\
} while(0)
#define BASILYS_LOCATION_HERE(MSG)  BASILYS_LOCATION_HERE_AT(__FILE__,__LINE__,MSG)
#else
#define BASILYS_DECLFRAME(NBVAR) struct {	\
  unsigned nbvar;				\
  struct basilysclosure_st* clos;		\
  struct excepth_basilys_st* exh;               \
  struct callframe_basilys_st* prev;		\
  void*  /* a basilys_ptr_t */ varptr[NBVAR];	\
} curfram__
#define BASILYS_LOCATION(LOCS) do{}while(0)
#define BASILYS_LOCATION_HERE(MSG) do{}while(0)
/* initialize the current callframe and link it at top */
#define BASILYS_INITFRAME(NBVAR,CLOS) do {				\
  memset(&curfram__, 0, sizeof(curfram__));				\
  curfram__.nbvar = (NBVAR);						\
  curfram__.prev = (struct callframe_basilys_st*)basilys_topframe;	\
  curfram__.clos = (CLOS);						\
  basilys_topframe = ((void*)&curfram__);				\
} while(0)
#endif


/* declare and initialize the current callframe */
#define BASILYS_ENTERFRAME(NBVAR,CLOS) \
  BASILYS_DECLFRAME(NBVAR); BASILYS_INITFRAME(NBVAR,CLOS)

/* exit the current frame and return */
#define BASILYS_EXITFRAME() do {		\
    basilys_topframe = (struct callframe_basilys_st*)(curfram__.prev);	\
} while(0)

/****
#define BASILYS_CATCH(Vcod,Vptr) do {
  jmp_buf __jbuf;
  int __jcod;
  jmp_buf* __prevj = basilys_jmpbuf;
  memset(&__jbuf, 0, sizeof(jmp_buf));
  basilys_jmpbuf = &__jbuf;
  __jcod = setjmp(&__jbuf);
  Vcod = __jcod;
  if (__jcod) {
    basilys_topframe = ((void*)&curfram__);
    Vptr = basilys_jmpval;
  };
} while(0)

#define BASILYS_THROW(Cod,Ptr) do {
} while(0)
***/


/* ====== safer output routines ===== */

/* output a string */
static inline void
basilys_puts (FILE * f, const char *str)
{
  if (f && str)
    fputs (str, f);
}

/* output a number with a prefix & suffix message */
static inline void
basilys_putnum(FILE* f, const char*pref, long l, const char*suff) {
  if (f) 
    fprintf(f, "%s%ld%s", pref?pref:"", l, suff?suff:"");
}

/* safe flush */
static inline void
basilys_flush (FILE * f)
{
  if (f)
    fflush (f);
}

/* safe newline and flush */
static inline void
basilys_newlineflush (FILE * f)
{
  if (f)
    {
      putc ('\n', f);
      fflush (f);
    }
}

/* output a string value */
static inline void
basilys_putstr (FILE * f, basilys_ptr_t sv)
{
  if (f && sv && basilys_magic_discr (sv) == OBMAG_STRING)
    fputs (((struct basilysstring_st *) sv)->val, f);
}

/* output a string buffer */
static inline void
basilys_putstrbuf (FILE * f, basilys_ptr_t sb)
{
  struct basilysstrbuf_st *sbuf = (struct basilysstrbuf_st *) sb;
  if (f && sbuf && basilys_magic_discr ((basilys_ptr_t) sbuf) == OBMAG_STRBUF)
    {
      gcc_assert (sbuf->bufzn);
      if (!sbuf->bufzn || sbuf->bufend <= sbuf->bufstart)
	return;
      fwrite (sbuf->bufzn + sbuf->bufstart, sbuf->bufend - sbuf->bufstart, 1,
	      f);
    }
}


/* output the declaration and implementation buffers of a generated file */
void 
basilys_output_cfile_decl_impl(basilys_ptr_t cfilnam, basilys_ptr_t declbuf, basilys_ptr_t implbuf);

static inline void
debugeputs_at (const char *fil, int lin, const char *msg)
{
  debugeprintf_raw ("!@%s:%d:\n@! %s\n", basename (fil), lin, msg);
}

#define debugeputs(Msg) debugeputs_at(__FILE__,__LINE__,(Msg))

static inline void
debugvalue_at (const char *fil, int lin, const char *msg, void *val)
{
  if (flag_basilys_debug)
    {
      fprintf (stderr, "!@%s:%d:\n@! %s @%p/%d= ",
	       basename (fil), lin, (msg), val, basilys_magic_discr ((basilys_ptr_t)val));
      basilys_dbgeprint (val);
      fflush (stderr);
    }
}

#define debugvalue(Msg,Val) debugvalue_at(__FILE__, __LINE__, (Msg), (Val))

static inline void
debugbacktrace_at (const char *fil, int lin, const char *msg, int depth)
{
  if (flag_basilys_debug)
    {
      fprintf (stderr, "\n!@%s:%d: %s ** BACKTRACE** ",
	       basename (fil), lin, msg);
      basilys_dbgbacktrace (depth);
      fflush (stderr);
    }
}

#define debugbacktrace(Msg,Depth) debugbacktrace_at(__FILE__, __LINE__, (Msg), (Depth))

static inline void
debugnum_at (const char *fil, int lin, const char *msg, long val)
{
  debugeprintf_raw ("!@%s:%d: %s =#= %ld\n", basename (fil), lin, msg, val);
}

#define debugnum(Msg,Val) debugnum_at(__FILE__, __LINE__, (Msg), (Val))

static inline void *
basilys_globpredef (int rank)
{
  if (rank > 0 && rank < BGLOB__LASTGLOB)
    return basilys_globarr[rank];
  return NULL;
}

void basilys_dbgshortbacktrace(const char* msg, int maxdepth);

#if ENABLE_CHECKING
extern void* basilys_checkedp_ptr1;
extern void* basilys_checkedp_ptr2;
extern FILE* basilys_dbgtracefile;
void basilys_caught_assign_at(void*ptr, const char*fil, int lin, const char*msg);
#define basilys_checked_assignmsg_at(Assign,Fil,Lin,Msg) ({		\
      void* p_##Lin = (Assign);						\
      if (p_##Lin && !basilys_discr(p_##Lin))				\
	basilys_assert_failed("bad checked assign (in runtime)",Fil,Lin,__FUNCTION__); \
      if ( (p_##Lin == basilys_checkedp_ptr1 && p_##Lin)		\
	   ||  (p_##Lin == basilys_checkedp_ptr2 && p_##Lin))		\
	basilys_caught_assign_at(p_##Lin,Fil,Lin,Msg); p_##Lin; })
#define basilys_checked_assign(Assign) basilys_checked_assignmsg_at((Assign),__FILE__,__LINE__,__FUNCTION__)
#define basilys_checked_assignmsg(Assign,Msg) basilys_checked_assignmsg_at((Assign),__FILE__,__LINE__,Msg)
void basilys_cbreak_at(const char*msg, const char*fil, int lin);
#define basilys_cbreak(Msg) basilys_cbreak_at((Msg),__FILE__,__LINE__)
#define basilys_trace_start(Msg,Cnt) do {if (basilys_dbgtracefile) \
   fprintf(basilys_dbgtracefile, "+%s %ld\n", Msg, (long)(Cnt));} while(0)
#define basilys_trace_end(Msg,Cnt) do {if (basilys_dbgtracefile) \
   fprintf(basilys_dbgtracefile, "-%s %ld\n", Msg, (long)(Cnt));} while(0)
#else
#define basilys_checked_assign(Assign) Assign
#define basilys_checked_assignmsg(Assign,Msg) Assign
#define basilys_cbreak(Msg) ((void)(Msg))
#define basilys_trace_start(Msg,Cnt) do{}while(0)
#define basilys_trace_end(Msg,Cnt) do{}while(0)
#endif /*ENABLE_CHECKING*/



/** handling GDBM state store; if there is none, these routines do
    nothing (and return NULL if needed); the GDBM database is lazily
    opened at first needed call. pointer key & data can be string,
    strbuf, or named instances (in which case the string name is
    used) **/

/* true if there is a GDBM state; side effect, open it */
bool basilys_has_gdbmstate(void);

/* return a string value associated to a constant key, or null if none */
basilys_ptr_t basilysgc_fetch_gdbmstate_constr(const char*key);

/* return a string value associated to a pointer key [string, strbuf, named] or null if none */
basilys_ptr_t basilysgc_fetch_gdbmstate (basilys_ptr_t key_p);

/* store or remove a constant key; if the value pointer is nil, remove
   it; if it s a string, strbuf, named, put it, eventually replacing
   the previous one; otherwise do nothing */
void basilysgc_put_gdbmstate_constr (const char *key, basilys_ptr_t data_p);

/* store or remove a pointer key (string, strbuf, or named value); if
   the value pointer is nil, remove it; if it s a string, strbuf,
   named, put it, eventually replacing the previous one; otherwise do
   nothing */
void basilysgc_put_gdbmstate (basilys_ptr_t key_p, basilys_ptr_t data_p);

#endif /*BASILYS_INCLUDED_ */
/* eof basilys.h */
