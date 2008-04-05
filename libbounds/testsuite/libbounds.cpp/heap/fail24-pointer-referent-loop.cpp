int main ()
{
	int *a; int *b;

	a = new int[10];
	b = new int[10];

    for (int *p = &a[0]; p < &b[100]; p++)
		*p = 0;

	delete a;
	delete b;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
