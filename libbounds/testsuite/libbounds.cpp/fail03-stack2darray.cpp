int main ()
{
	int foo [10][10];
	foo[9][10] = 0;
	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
