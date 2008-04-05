int main()
{
	int* p;
	int a[10];

	p = &a[6] + 4;

	*p = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
