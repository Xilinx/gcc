/* The struct has space remaining at the end of a word (alignment) */

#include <stdlib.h>
#include <string.h>

typedef struct {
  int i;
  short a[3];
  char b[7];
} sTest;

int main()
{
    sTest *p = (sTest *) malloc( sizeof(sTest) );

	strcpy(p->b,"->test\n");

    return 0;
}

/* This testcase is supposed to fail, but it is not, because the
   checker is not strong enough.  */

/* { dg-output "bounds violation.*" { xfail *-*-* } } */
/* { dg-do run } */
