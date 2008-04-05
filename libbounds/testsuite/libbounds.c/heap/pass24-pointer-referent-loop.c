#include <stdlib.h>

#define SIZE 100

int main ()
{
	int *a; int *b;
	int *p;

	a = (int *) malloc( SIZE * sizeof(int) );
	b = (int *) malloc( SIZE * sizeof(int) );

    for (p = &a[0]; p < &a[SIZE]; p++)
		*p = 0;
    for (p = &b[0]; p < &b[SIZE]; p++)
		*p = 0;

	free( a );
	free( b );

	return 0;
}
