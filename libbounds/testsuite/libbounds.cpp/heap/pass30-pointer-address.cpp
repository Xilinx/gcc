int main()
{
	int* p;
	int* a = new int[10];

	p = &a[9];

	*p = 0;

	delete a;

	return 0;
}
