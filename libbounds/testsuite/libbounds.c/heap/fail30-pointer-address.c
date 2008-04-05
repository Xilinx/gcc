int main()
{
	int* p;
	int* a = (int *) malloc( 10*sizeof(int) );

	p = &a[10];

	*p = 0;

	free( a );

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
