/* { dg-do compile } */
/* { dg-options "-O2 -std=gnu99 -pedantic-errors -mea64" } */
/* Make sure __ea structure references work.  */

typedef unsigned long int uintptr_t;

struct tostruct
{
  uintptr_t selfpc;
  long count;
  unsigned short link;
};

/* froms are indexing tos */
static __ea unsigned short *froms;
static __ea struct tostruct *tos = 0;

void
foo (uintptr_t frompc, uintptr_t selfpc)
{
  __ea unsigned short *frompcindex;

  frompcindex = &froms[(frompc) / (4 * sizeof (*froms))];
  *frompcindex = tos[0].link;

  return;
}
