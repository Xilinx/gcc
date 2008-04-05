#include <stdlib.h>

int main ()
{
	int *p;
	p = (int *) malloc( 10 * sizeof(int) );

	/* p[7] = 0; */
	p = &p[9];
	p -= 2;
	*p = 0;

	return 0;
}
