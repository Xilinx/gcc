int main()
{
	int* p;
	int a[10];

	/* a[9] = 0; */
	p = &a[8];
	++ p;
	*p = 0;

	return 0;
}
