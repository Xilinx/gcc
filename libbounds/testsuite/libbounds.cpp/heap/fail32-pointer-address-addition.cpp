int main()
{
	int* p;
	int* a = new int[10];

	p = &a[6] + 4;

	*p = 0;

	delete a;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
