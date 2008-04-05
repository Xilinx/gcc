int main()
{
	int* p;
	int* a = (int *) malloc( 10 * sizeof(int) );

	/* a[0] = 0; */
	p = a;
	*p = 0;

	free( a );

	return 0;
}
