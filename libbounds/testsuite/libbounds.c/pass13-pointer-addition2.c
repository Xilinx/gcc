int main()
{
	int* p;
	int a[10];

	/* a[3] = 0; */
	p = a;
	p += 3;
	*p = 0;

	return 0;
}
