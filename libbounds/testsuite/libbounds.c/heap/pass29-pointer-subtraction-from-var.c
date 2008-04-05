int main()
{
	int* p;
	int* a = (int *) malloc(10*sizeof(int));
	int i = 2;

	/* a[7] = 0; */
	p = &a[9];
	p = p - i;
	*p = 0;

	free( a );

	return 0;
}
