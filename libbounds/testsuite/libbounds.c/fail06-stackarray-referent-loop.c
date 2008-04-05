int main()
{
	int a[100];
	int b[100];

    int i;
    for (i = 0; i < 101; i++)
		b[i] = a[i] * 3;

    /* TODO: This should work also for 
       
        return b[10];

	and it is not yet, because MEM expressions are not handled.
    */
    
    return b[i];
}

/* { dg-output "bounds violation.*"  } */
/* { dg-do run { xfail *-*-* } } */
