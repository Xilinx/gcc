int main()
{
	int* p;
	int i = 9;

	/* a[9] = 0; */
	p = new int[10];
	p = p + i;
	*p = 0;

	return 0;
}
