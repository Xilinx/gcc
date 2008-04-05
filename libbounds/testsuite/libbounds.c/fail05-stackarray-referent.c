int main()
{
	int a[10];
	int b[10];

	b[10] = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
