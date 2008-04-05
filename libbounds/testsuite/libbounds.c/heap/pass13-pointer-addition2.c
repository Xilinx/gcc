#include <stdlib.h>

int main ()
{
	int *p;
	p = (int *) malloc( 10 * sizeof(int) );

	/* p[3] = 0; */
	p += 3;
	*p = 0;

	return 0;
}
