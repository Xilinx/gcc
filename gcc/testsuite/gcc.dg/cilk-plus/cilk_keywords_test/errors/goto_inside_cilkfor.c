/* Checks goto inside cilk_for and outside of cilk_for.  */

#define ARRAY_SIZE 10000
int a[ARRAY_SIZE], b[ARRAY_SIZE][ARRAY_SIZE];
int main (int argc, char **argv)
{
    int i = 0, q = 0, j = 0;
    _Cilk_for (i = 0; i < ARRAY_SIZE; i++)
    {
l1:
	a[i] = i;
l2:
	if (i%2)
	  q+=2;
    }
    goto l1; /* { dg-error "Goto label is inside a _Cilk_for while the goto itself is outside." }   */
    if(a[(ARRAY_SIZE / 2)] == (ARRAY_SIZE / 2))
        goto l2; /* { dg-error "Goto label is inside a _Cilk_for while the goto itself is outside." } */

    _Cilk_for (i = 0; i < ARRAY_SIZE; i++)
    {
      /* Goto jumps into another Cilk_for.  */
        goto l2; /* { dg-error "goto destination is outside the _Cilk_for scope." }  */
	a[i] = i;
l3:
	goto l3; /* This is OK!  */
    }
    if(a[(ARRAY_SIZE / 2)] == (ARRAY_SIZE / 2))
      goto l4; /* This is OK!  */
    else
        goto l3; /* { dg-error "Goto label is inside a _Cilk_for while the goto itself is outside." } */

    _Cilk_for (i = 0; i < ARRAY_SIZE; i++) {
l5:
	if (i % 5) {
	    _Cilk_for (j = 0; j < ARRAY_SIZE; j++) {
		goto l5; /* { dg-error "goto destination is outside the _Cilk_for scope." } */
		b[i][j] = (i+j);
l6:
	      q++;
	      goto l6; /* This is OK!  */
	    }
	    goto l6; /* { dg-error "goto destination is outside the _Cilk_for scope." } */
	}
    }

l4:
    return 0;
}
