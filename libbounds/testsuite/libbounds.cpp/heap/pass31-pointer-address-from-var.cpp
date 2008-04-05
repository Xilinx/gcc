int main()
{
	int* p;
	int* a = new int[10];
	int i = 9;

	p = &a[i];

	*p = 0;

	delete a;

	return 0;
}
