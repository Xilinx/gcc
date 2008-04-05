#include <stdlib.h>

int main ()
{
	int* p;
	int* a;

	a = (int *) malloc( 10 * sizeof(int) );

	*p = 0;

	free( a );

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
