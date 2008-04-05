int main()
{
	int a[100];
	int b[100];

    for (int i = 0; i < 101; i++)
		b[i] = 0;

    /* TODO: This should work also for 
       
        return b[10];

	and it is not yet, because MEM expressions are not handled.
    */


	return b[10];
}
/* { dg-output "bounds violation.*" { xfail *-*-* } } */
/* { dg-do run { xfail *-*-* } } */
