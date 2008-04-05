#include <string.h>

typedef struct {
  char b[7];
} sTest;

int main()
{
    sTest s;

	strcpy(s.b,". test\n");

    return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
