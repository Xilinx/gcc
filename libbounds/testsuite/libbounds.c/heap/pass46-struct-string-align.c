/* The struct has space remaining at the end of a word (alignment) */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int i;
  short a[3];
  char b[8];
} sTest;

int main()
{
    sTest *p = (sTest *) malloc( sizeof(sTest) );

	strcpy(p->b,"->test\n");
    printf(p->b);

	free(p);

    return 0;
}
/* { dg-output "\\-\\>test\n" } */
