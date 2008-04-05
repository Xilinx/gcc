int main()
{
	int* p;
	int* a = (int *) malloc(10*sizeof(int));

	p = &a[9];

	*p = 0;

	free( a );

	return 0;
}
