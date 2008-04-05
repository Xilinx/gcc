#include <stdlib.h>

int main ()
{
	int *a; int *b;
	int *p;

	a = (int *) malloc( 10 * sizeof(int) );
	b = (int *) malloc( 10 * sizeof(int) );

	p = &b[9];
	p++;
	*p = 0;

	free( a );
	free( b );

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
