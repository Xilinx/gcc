/* <feature> No storage class may be specified for the variable within the initialization clause
   </feature>
*/

#define ARRAY_SIZE 10000

int a[ARRAY_SIZE];

void func()
{
    auto int l;
    extern int j;
    static int k;
    register int m;
    _Cilk_for(j = 0; j < ARRAY_SIZE; j += 1) /* { dg-error " _Cilk_for induction variable cannot be declared extern" } */
    {
	a[(int)j] = (int)j;
    }
    _Cilk_for(k = 0; k < ARRAY_SIZE; k += 1) /* { dg-error " _Cilk_for induction variable cannot be declared static" } */
    {
	a[(int)k] += (int)k;
    }
    _Cilk_for(l = 0; l < ARRAY_SIZE; l += 1) /* { dg-error " _Cilk_for induction variable cannot be declared auto" } */
    {
	a[(int)l] += (int)l;
    }
    _Cilk_for(m = 0; m < ARRAY_SIZE; m += 1) /* { dg-error " _Cilk_for induction variable cannot be declared register" } */
    {
	a[(int)m] += (int)m;
    }
}
