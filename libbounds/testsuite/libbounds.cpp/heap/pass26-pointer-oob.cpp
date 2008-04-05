int main()
{
    int *p = new int[10];
	int *q = new int[10];

	p += 9;  /* p is within bounds and one less than q */

	*q = 0;   /* valid */
	*p = 0;   /* valid */

	return 0;
}
