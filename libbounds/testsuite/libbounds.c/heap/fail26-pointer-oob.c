int main()
{
    int *p = (int *) malloc(10*sizeof(int));
	int *q = (int *) malloc(10*sizeof(int));

	p += 10;  /* p is oob and also EQUAL to q */

	*q = 0;   /* valid */
	*p = 0;   /* not valid */

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
