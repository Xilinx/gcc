int main ()
{
	int *p;
	p = new int[10];

	/* p[3] = 0; */
	p += 3;
	*p = 0;

	return 0;
}
