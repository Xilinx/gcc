int main()
{
	int a[10];
	int b[10];
	int* p;

	p = b;
	*p = 0;

	p += 9;
	*p = 0;

	p = a;
	*p = 0;

	p += 9;
	*p = 0;

	return 0;
}
