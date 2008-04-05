int main()
{
	int* p;
	int a[10];
	int i = 3;

	p = &a[6] + i;

	*p = 0;

	return 0;
}
