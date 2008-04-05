int main()
{
	int* p;
	int* a = new int[10];
	int i = 2;

	/* a[7] = 0; */
	p = &a[9];
	p = p - i;
	*p = 0;

	delete a;

	return 0;
}
