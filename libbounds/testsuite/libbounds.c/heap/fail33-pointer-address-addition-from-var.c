int main()
{
	int* p;
	int* a = (int *) malloc(10*sizeof(int));
	int i = 4;

	p = &a[6] + i;

	*p = 0;

	free( a );

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
