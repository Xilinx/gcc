int main()
{
	int a[10];
	int b[10];
	int* p;

	p = &b[9];
	p++;
	*p = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
