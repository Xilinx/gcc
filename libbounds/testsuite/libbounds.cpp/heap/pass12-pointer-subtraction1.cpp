int main ()
{
	int *p;
	p = new int[10];

	/* p[7] = 0; */
	p = &p[9];
	p = p - 2;
	*p = 0;

	return 0;
}
