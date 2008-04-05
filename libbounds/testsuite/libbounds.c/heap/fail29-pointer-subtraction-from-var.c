int main()
{
	int* p;
	int* a = (int *) malloc( 10*sizeof(int) );
	int i = 10;

	/* a[-1] = 0; */
	p = &a[9];
	p = p - i;
	*p = 0;

	free( a );

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
