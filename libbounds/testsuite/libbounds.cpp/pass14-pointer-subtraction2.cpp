int main()
{
	int* p;
	int a[10];

	/* a[7] = 0; */
	p = &a[9];
	p -= 2;
	*p = 0;

	return 0;
}
