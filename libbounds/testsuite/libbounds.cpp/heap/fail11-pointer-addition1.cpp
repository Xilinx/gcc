int main ()
{
	int *p;
	p = new int[10];

	/* p[9] = 0; */
	p = p + 10;
	*p = 0;

	delete p;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
