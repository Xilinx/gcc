#include <stdlib.h>

int main ()
{
	int *p;
	p = (int *) malloc( 10 * sizeof(int) );

	/* p[10] = 0; */
	p = &p[9];
	++ p;
	*p = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
