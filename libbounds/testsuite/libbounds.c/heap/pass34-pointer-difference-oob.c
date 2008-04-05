int main()
{
	int* p; int* q;
	int size;

	p = (int *) malloc(10*sizeof(int));
	q = &p[10];
	size = q - p;

	free( p );

	return size - 10;
}
