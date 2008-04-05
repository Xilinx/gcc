int main()
{
	int* p;
	int a[10];

	/* a[3] = 0; */
	p = a + 3;
	*p = 0;

	return 0;
}
