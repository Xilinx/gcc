int main()
{
	int* p;
	int a[10];
	int i = 4;

	p = &a[6] + i;

	*p = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
