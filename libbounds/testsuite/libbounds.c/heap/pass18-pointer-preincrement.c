#include <stdlib.h>

int main ()
{
	int *p;
	p = (int *) malloc( 10 * sizeof(int) );

	/* p[9] = 0; */
	p = &p[8];
	++ p;
	*p = 0;

	return 0;
}
