int main ()
{
	int *p;
	p = new int[10];

	/* p[0] = 0; */
	p = &p[1];
	-- p;
	*p = 0;

	return 0;
}
