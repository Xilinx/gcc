/* <feature>
     Unsigned wraparound is not allowed
   </feature>
*/

int a[50000];

int main(void)
{
    unsigned char i;
    _Cilk_for(i = 2; i != 1; i++) /* { dg-error "end-condition value is greater than starting" } */
    {
	a[i] = i;
    }
    return 0;
}
