int main()
{
	int* p;
	int a[10];
	unsigned int b[] = { 0, 5, 10 };
	unsigned int index_index = 1;

	/* a[5] = 0; */
	p = a + b[index_index];
	*p = 0;

	return 0;
}
