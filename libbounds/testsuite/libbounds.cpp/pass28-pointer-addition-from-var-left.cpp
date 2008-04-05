int main()
{
	int* p;
	int a[10];
	int i = 9;

	/* a[9] = 0; */
	p = a;
	p = i + p;
	*p = 0;

	return 0;
}
