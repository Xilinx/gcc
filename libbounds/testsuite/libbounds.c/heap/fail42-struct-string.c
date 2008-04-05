#include <stdlib.h>
#include <string.h>

typedef struct {
  char b[7];
} sTest;

int main()
{
    sTest *p = (sTest *) malloc( sizeof(sTest) );

	strcpy(p->b,"->test\n");

    return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */

