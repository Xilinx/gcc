int main()
{
	int* p;
	int a[10];

	/* a[9] = 0; */
	p = a;
	p = p + 9;
	*p = 0;

	return 0;
}
