int main()
{
	int* p;
	int a[10];

	p = &a[6] + 3;

	*p = 0;

	return 0;
}
