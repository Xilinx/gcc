#include <stdio.h>
#include <string.h>

typedef struct {
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
