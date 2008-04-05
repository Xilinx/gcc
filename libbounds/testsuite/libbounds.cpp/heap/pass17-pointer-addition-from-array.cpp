int main ()
{
	unsigned int b[] = { 0, 5, 10 };
	unsigned int index_index = 1;

	int *p;
	p = new int[10];

	/* p[5] = 0; */
	p = p + b[index_index];
	*p = 0;

	return 0;
}
