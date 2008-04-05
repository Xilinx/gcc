int main()
{
	int* p;
	int* a = (int *) malloc(10*sizeof(int));
	int i = 9;

	p = &a[i];

	*p = 0;

	free( a );

	return 0;
}
