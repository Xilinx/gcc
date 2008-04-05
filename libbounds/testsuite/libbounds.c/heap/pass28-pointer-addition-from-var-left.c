int main()
{
	int* p;
	int i = 9;

	/* a[9] = 0; */
	p = (int *) malloc(10*sizeof(int));
	p = i + p;
	*p = 0;

	return 0;
}
