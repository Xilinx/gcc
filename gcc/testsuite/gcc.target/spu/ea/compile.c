/* Valid __ea declarations.  */
/* { dg-do compile } */
/* { dg-options "-std=gnu99 -pedantic-errors" } */

/* Externs.  */

__ea extern int i1;
extern __ea int i2;
extern int __ea i3;
extern int __ea *ppu;

/* Pointers.  */
__ea int *i4p;

/* Typedefs.  */
typedef __ea int ea_int_t;
typedef __ea int *ea_int_star_t;

void
f1 ()
{
  int *spu;
  ppu = (ea_int_t *) spu;
  ppu = (ea_int_star_t) spu;
}

void
f2 ()
{
  int *spu;
  spu = (int *) ppu;
  ppu = (__ea int *) spu;
}

void
f3 ()
{
  int i = sizeof (__ea int);
}

__ea int *f4 (void)
{
  return 0;
}

void f5 (__ea int *parm)
{
  ;
}

static inline __ea void *f6 (__ea void *start)
{
  return 0;
}

void f7 (void)
{
  __ea void *s1;
  auto __ea void *s2;
}
