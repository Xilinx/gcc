int main ()
{
	int *a; int *b;
	int *p;

	a = new int[10];
	b = new int[10];

	p = b;
	*p = 0;

	p += 9;
	*p = 0;

	p = a;
	*p = 0;

	p += 9;
	*p = 0;

	delete b;
	delete a;

	return 0;
}
