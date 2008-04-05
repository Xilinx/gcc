int main()
{
	int* p;
	int a[10];

	/* a[10] = 0; */
	p = a + 10;
	*p = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
