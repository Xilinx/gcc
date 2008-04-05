int main()
{
	int* p;
	int a[10];
	int i = 2;

	/* a[7] = 0; */
	p = &a[9];
	p = p - i;
	*p = 0;

	return 0;
}
