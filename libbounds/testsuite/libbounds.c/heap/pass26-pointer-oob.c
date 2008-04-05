int main()
{
    int *p = (int *) malloc(10*sizeof(int));
	int *q = (int *) malloc(10*sizeof(int));

	p += 9;  /* p is within bounds and one less than q */

	*q = 0;   /* valid */
	*p = 0;   /* valid */

	return 0;
}
