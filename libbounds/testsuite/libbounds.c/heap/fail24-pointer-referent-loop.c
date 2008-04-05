#include <stdlib.h>

int main ()
{
	int *a; int *b;
	int *p;

	a = (int *) malloc( 100 * sizeof(int) );
	b = (int *) malloc( 100 * sizeof(int) );

    for (p = &a[0]; p < &b[100]; p++)
		*p = 0;

	free( a );
	free( b );

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
