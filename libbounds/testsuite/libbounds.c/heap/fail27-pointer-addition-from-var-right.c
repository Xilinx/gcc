int main()
{
	int* p;
	int i = 10;

	/* a[10] = 0; */
	p = (int *) malloc(10*sizeof(int));
	p = p + i;
	*p = 0;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
