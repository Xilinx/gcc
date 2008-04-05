int main ()
{
	unsigned int b[] = { 0, 5, 10 };
	unsigned int index_index = 2;

	int *p;
	p = new int[10];

	/* p[10] = 0; */
	p = p + b[index_index];
	*p = 0;

	delete p;

	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
