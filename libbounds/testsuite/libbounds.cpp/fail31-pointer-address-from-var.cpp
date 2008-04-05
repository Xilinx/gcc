int main()
{
	int* p;
	int a[10];
	int i = 10;

	p = &a[i];

	*p = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
