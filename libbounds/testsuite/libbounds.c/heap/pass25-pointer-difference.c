int main()
{
	int* p; int* q;
	int size;

	p = (int *) malloc(10*sizeof(int));
	q = &p[9];
	size = q - p;

	free( p );

	return size - 9;
}
