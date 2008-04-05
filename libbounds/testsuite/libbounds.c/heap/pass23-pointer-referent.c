#include <stdlib.h>

int main ()
{
	int *a; int *b;
	int *p;

	a = (int *) malloc( 10 * sizeof(int) );
	b = (int *) malloc( 10 * sizeof(int) );

	p = b;
	*p = 0;

	p += 9;
	*p = 0;

	p = a;
	*p = 0;

	p += 9;
	*p = 0;

	free( b );
	free( a );

	return 0;
}
