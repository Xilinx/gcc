#include <stdlib.h>

int main ()
{
	int *p;
	p = (int *) malloc( 10 * sizeof(int) );

	/* p[0] = 0; */
	p = &p[1];
	-- p;
	*p = 0;

	return 0;
}
