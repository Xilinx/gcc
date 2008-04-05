int main()
{
    int *p = new int[10];
	int *q = new int[10];

	p += 10;  /* p is oob and also EQUAL to q */

	*q = 0;   /* valid */
	*p = 0;   /* not valid */

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
