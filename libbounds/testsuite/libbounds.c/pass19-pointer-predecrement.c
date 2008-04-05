int main()
{
	int* p;
	int a[10];

	/* a[0] = 0; */
	p = &a[1];
	-- p;
	*p = 0;

	return 0;
}
