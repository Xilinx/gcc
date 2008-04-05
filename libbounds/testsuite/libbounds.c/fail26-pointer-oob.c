int main()
{
	int a[10];
	int b[10];

    int *p = a;
	int *q = b;

	p += 10;  /* p is oob and also EQUAL to q */

	*q = 0;   /* valid */
	*p = 0;   /* not valid */

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
