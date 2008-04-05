int main()
{
	int* p;
	int i = 9;

	/* a[9] = 0; */
	p = new int[10];
	p = i + p;
	*p = 0;

	return 0;
}
