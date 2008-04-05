int main ()
{
	int *a; int *b;
	int *p;

	a = new int[10];
	b = new int[10];

	p = &b[9];
	p++;
	*p = 0;

	delete p;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
