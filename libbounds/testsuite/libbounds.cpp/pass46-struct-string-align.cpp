/* The struct has space remaining at the end of a word (alignment) */

#include <string.h>
#include <stdio.h>

typedef struct {
  int i;
  short a[3];
  char b[8];
} sTest;

int main()
{
    sTest s;

	strcpy(s.b,". test\n");
    printf(s.b);

    return 0;
}
/* { dg-output "\.\ test\n" } */
