
/* run in build dir to check the meltframe mode
 ./cc1 -O -fmelt-module-path=melt-modules:. \
    -fmelt-source-path=melt-sources:. \
    -fmelt-tempdir=/tmp \
    -fmelt-mode=meltframe \
    -fmelt-debug $GCCMELT_SOURCE/gcc/testsuite/melt/t-meltgcnewint.c \
    -o /dev/null
*/

struct meltclosure_st;
struct meltobject_st;
struct excepth_melt_st;
typedef unsigned long size_t;
typedef union melt_un *melt_ptr_t;

typedef struct meltobject_st *meltobject_ptr_t;
void *meltgc_allocate (size_t basesz, size_t gap);
int melt_magic_discr (melt_ptr_t p);

const char *basename (const char *path);

extern int snprintf (char *__restrict __s, size_t __maxlen,
		     __const char *__restrict __format, ...)
  __attribute__ ((__format__ (__printf__, 3, 4)));
extern void *memset (void *__s, int __c, size_t __n);


struct meltobject_st /* when MELTOBMAG_OBJECT */
{
  meltobject_ptr_t discr;
  /* from VALDESC_OBJECT in warmelt-base.melt */
  unsigned obj_hash;		/* hash code of the object */
  unsigned short obj_num;
  unsigned short obj_len;
  melt_ptr_t obj_vartab[0];
};				/* end meltobject_st */


struct meltint_st /* when MELTOBMAG_INT */
{
  meltobject_ptr_t discr;
  long val;
};

union melt_un
{
  meltobject_ptr_t u_discr;
  struct meltint_st u_int;
  struct meltobject_st u_object;
};

struct callframe_melt_st
{
  const char *mcfr_flocs;
  union
  {
    struct meltclosure_st *mcfr_closp_;	/* when mcfr_nbvar >= 0 */
    void (*mcfr_forwmarkrout_) (struct callframe_melt_st *, int);	/* when mcfr_nbvar < 0 */
  } mcfr_un_;
  /* Interface: void mcfr_forwmarkrout (void* frame, int marking) */
  struct excepth_melt_st *mcfr_exh;	/* for our exceptions - not implemented yet */
  struct callframe_melt_st *mcfr_prev;
  melt_ptr_t mcfr_varptr[0];
};


/* the topmost call frame */
extern struct callframe_melt_st *melt_topframe;

enum meltobmag_en /*generated */
{
  MELTOBMAG__NONE = 0,
  MELTOBMAG__FIRST /* first valid object magic */  = 20000,
  MELTOBMAG_INT /*valmagic */  = 20003,
  MELTOBMAG_OBJECT /*valmagic */  = 20011,
};


melt_ptr_t
meltgc_new_int (meltobject_ptr_t discr_p, long num)
{
  struct
  {
    int mcfr_nbvar;
    const char *mcfr_flocs;
    struct meltclosure_st *mcfr_clos;
    struct excepth_melt_st *mcfr_exh;
    struct callframe_melt_st *mcfr_prev;
    void *mcfr_varptr[2];
  } meltfram__;
  do
    {
      static char locbuf_1591[84];
      if (!locbuf_1591[0])
	snprintf (locbuf_1591, sizeof (locbuf_1591) - 1, "%s:%d",
		  basename ("gcc/melt-runtime.c"), (int) 1591);
      memset (&meltfram__, 0, sizeof (meltfram__));
      meltfram__.mcfr_nbvar = (2);
      meltfram__.mcfr_flocs = locbuf_1591;
      meltfram__.mcfr_prev = (struct callframe_melt_st *) melt_topframe;
      meltfram__.mcfr_clos = (((void *) 0));
      melt_topframe = ((struct callframe_melt_st *) &meltfram__);
    }
  while (0);
  meltfram__.mcfr_varptr[1] = (void *) discr_p;
  if (melt_magic_discr ((melt_ptr_t) (meltfram__.mcfr_varptr[1])) !=
      MELTOBMAG_OBJECT)
    goto end;
  if (((meltobject_ptr_t) (meltfram__.mcfr_varptr[1]))->obj_num !=
      MELTOBMAG_INT)
    goto end;
  meltfram__.mcfr_varptr[0] = meltgc_allocate (sizeof (struct meltint_st), 0);
  ((struct meltint_st *) (meltfram__.mcfr_varptr[0]))->discr =
    ((meltobject_ptr_t) (meltfram__.mcfr_varptr[1]));
  ((struct meltint_st *) (meltfram__.mcfr_varptr[0]))->val = num;
end:
  do
    {
      melt_topframe = (struct callframe_melt_st *) (meltfram__.mcfr_prev);
    }
  while (0);
  return (melt_ptr_t) meltfram__.mcfr_varptr[0];
}
