int main()
{
	int foo[2];
	foo[2] = 0;
	return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
