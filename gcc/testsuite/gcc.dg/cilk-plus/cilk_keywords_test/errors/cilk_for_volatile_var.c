/* <feature>
    The variable may not be const or volatile
   </feature>
*/
#define SMALL_INT_ARRAY_SIZE 10000
int a[SMALL_INT_ARRAY_SIZE];

int main (void)
{
    volatile int ii;
    const int jj;

    
    _Cilk_for(ii = 0; ii < SMALL_INT_ARRAY_SIZE; ii++) /* { dg-error "_Cilk_for induction variable cannot be volatile." } */
    {
	a[ii] = ii;
    }
}
