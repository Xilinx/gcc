int main()
{
	int a[10];
	int b[10];

    int *p = a;
	int *q = b;

	p += 9;  /* p is within bounds and one less than q */

	*q = 0;   /* valid */
	*p = 0;   /* valid */

	return 0;
}
